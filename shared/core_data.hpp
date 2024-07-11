#include "guest_ctx.hpp"

naked void vmexit_handler_ret_asm();

//
#define core_data_gs_offset 0x120
#define core_data_vmcb_gs_offset 0x128

#include "../../headers/hv_hotpatch_client.hpp"

//guest ctx
class hv_guest_ctx
{
public:
	//int regs
	arch_reg<0, offsetof(vmcb, rax), hv_hotpatch_type::direct_off, hv_hotpatch_type::gs_off> rax;
	u64 rcx;
	u64 rdx;
	u64 rbx;
	arch_reg<GUEST_RSP, offsetof(vmcb, rsp), hv_hotpatch_type::vmx, hv_hotpatch_type::gs_off> rsp;
	u64 rbp;
	u64 rsi;
	u64 rdi;
	u64 r8;
	u64 r9;
	u64 r10;
	u64 r11;
	u64 r12;
	u64 r13;
	u64 r14;
	u64 r15;

	//xmm regs
	M128A xmm0;
	M128A xmm1;
	M128A xmm2;
	M128A xmm3;
	M128A xmm4;
	M128A xmm5;
	M128A xmm6;
	M128A xmm7;
	M128A xmm8;
	M128A xmm9;
	M128A xmm10;
	M128A xmm11;
	M128A xmm12;
	M128A xmm13;
	M128A xmm14;
	M128A xmm15;
};

struct hv_vm_state_ctx
{
	union {
		arch_reg<
			GUEST_RIP, 
			offsetof(vmcb, rip), 
			hv_hotpatch_type::vmx,
			hv_hotpatch_type::direct_off> rip;

		arch_reg<
			GUEST_CR3, 
			offsetof(vmcb, cr3), 
			hv_hotpatch_type::vmx,
			hv_hotpatch_type::direct_off> cr3;
		
		arch_reg<
			VMCS_CTRL_EPT_POINTER,
			offsetof(vmcb, ncr3),
			hv_hotpatch_type::vmx,
			hv_hotpatch_type::direct_off> slat_dir;

		arch_reg<
			VMCS_EXIT_QUALIFICATION,
			offsetof(vmcb, exitinfo1),
			hv_hotpatch_type::vmx,
			hv_hotpatch_type::direct_off> slat_fail_desc;

		arch_reg<
			VMCS_GUEST_PHYSICAL_ADDRESS,
			offsetof(vmcb, exitinfo2),
			hv_hotpatch_type::vmx,
			hv_hotpatch_type::direct_off> slat_fail_gpa;

		arch_reg<
			VMCS_EXIT_GUEST_LINEAR_ADDRESS,
			0,
			hv_hotpatch_type::vmx,
			hv_hotpatch_type::direct_off> slat_fail_gva;

		arch_reg<
			GUEST_RFLAGS,
			offsetof(vmcb, rflags),
			hv_hotpatch_type::vmx, 
			hv_hotpatch_type::direct_off> rflags;
		
		arch_reg<GUEST_GS_BASE,
			offsetof(vmcb, gsbase), 
			hv_hotpatch_type::vmx,
			hv_hotpatch_type::direct_off> gsbase;
		
		arch_reg<GUEST_CS_SELECTOR,
			offsetof(vmcb, csselector), 
			hv_hotpatch_type::vmx, 
			hv_hotpatch_type::direct_off, u16> csselector;
		
		arch_reg<IA32_KERNEL_GS_BASE,
			offsetof(vmcb, kernelgsbase),
			hv_hotpatch_type::msr,
			hv_hotpatch_type::direct_off> kernelgsbase;
		
		arch_reg<VMCS_EXIT_REASON,
			offsetof(vmcb, exitcode),
			hv_hotpatch_type::vmx,
			hv_hotpatch_type::direct_off, u16> exitcode;
	};
};

//
class core_data_t
{
private:
	u64 npt_org_dir;

public:
	//guest state
	pv arg1, arg2;
	bool adjust_rip;
	int cur_instr_len;
	hv_guest_ctx* context;
	hv_vm_state_ctx* vm_state;
	
	//fastret
	u64 ret_val;
	u64 fast_restore_context;

	//slat
	finl u64 slat_org_dir(bool set = true, bool pfn_only = false) 
	{
		if (!npt_org_dir && set) {
			npt_org_dir = vm_state->slat_dir;
		}

		return pfn_only ? (npt_org_dir >> PAGE_SHIFT) : npt_org_dir;
	}

	//rip control
	finl void advance_rip() {
		if (adjust_rip) {
			u64 rip1 = vm_state->rip;
			rip1 += cur_instr_len;
			vm_state->rip = rip1;
		}
	}
};

//core data storage
constexpr int max_cores_cnt = 32;
core_data_t core_data_arr[max_cores_cnt];

template <bool force_update = false>
finl core_data_t* get_core_data() 
{
	//update vars
	if constexpr (force_update) 
	{
		//get cur core_data ptr
		int core_id = get_current_core_id();
		auto core_ctx = &core_data_arr[core_id];

		//write core ctx ptr to cur gs
		_writegsqword(core_data_gs_offset, (u64)core_ctx);

		//amd vars update
		if (mapper_ctx_hv.type == hv_amd_vendor) {
			const auto vmcb1 = call_ptr<u64>(&mapper_ctx_hv.get_vmcb_shell);
			_writegsqword(core_data_vmcb_gs_offset, vmcb1);
			core_ctx->ret_val = __readgsqword(0);
		}

		//ret updated ptr
		return core_ctx;
	}

	//use cached ptr
	return (core_data_t*)__readgsqword(core_data_gs_offset);
}