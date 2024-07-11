#include "globals.hpp"

u64 hv_pfn1;
u64 fake_page_pfn;

//intel
#include "intel/utils.hpp"
#include "intel/paging.hpp"
#include "intel/hlat.hpp"

//amd
#include "amd/utils.hpp"
#include "amd/paging.hpp"

//shared
#include "shared/paging.hpp"
#include "shared/core_data.hpp"
#include "shared/idt_hooks.hpp"

//TODO:
#include "intel/ept_tables_init.hpp"

//lab
#include "lab/hypercallback/hypercallback_v2.hpp"
#include "lab/render/present1.hpp"

#include "shared/cpuid_handler.hpp"


//amd features
//#include "amd/npt.hpp"

//vmexit
#include "amd/vmexit_amd.hpp"
#include "intel/vmexit_intel.hpp"

naked void vmexit_handler_ret_asm()
{
	__asm
	{
		//get core ctx
		mov rax, gs:[core_data_gs_offset]
		
		//restore stack ptr
		mov rsp, [rax] core_data_t.fast_restore_context
		
		//restore ret val
		mov rax, [rax] core_data_t.ret_val

		//load guest context
		pop rcx

		//load non volatile ctx
		pop r12
		pop r13
		pop r14
		pop r15
		pop rdi
		pop rsi
		pop rbx
		pop rbp

		//load guest xmm6-xmm15
		movaps xmm6,  xmmword ptr [rcx] hv_guest_ctx.xmm6
		movaps xmm7,  xmmword ptr [rcx] hv_guest_ctx.xmm7
		movaps xmm8,  xmmword ptr [rcx] hv_guest_ctx.xmm8
		movaps xmm9,  xmmword ptr [rcx] hv_guest_ctx.xmm9
		movaps xmm10, xmmword ptr [rcx] hv_guest_ctx.xmm10
		movaps xmm11, xmmword ptr [rcx] hv_guest_ctx.xmm11
		movaps xmm12, xmmword ptr [rcx] hv_guest_ctx.xmm12
		movaps xmm13, xmmword ptr [rcx] hv_guest_ctx.xmm13
		movaps xmm14, xmmword ptr [rcx] hv_guest_ctx.xmm14
		movaps xmm15, xmmword ptr [rcx] hv_guest_ctx.xmm15

		//go vmload, vmrun!
		ret
	}
}

noinl u64 vmexit_handler_cpp(pv a1, pv a2, hv_guest_ctx* guest_ctx, u64 fast_ret_ctx)
{
	bool handled = false;

	//base fill core ctx
	auto& core_ctx = *get_core_data<true>();
	core_ctx.arg1 = a1;
	core_ctx.arg2 = a2;
	core_ctx.adjust_rip = true;
	core_ctx.context = guest_ctx;
	core_ctx.fast_restore_context = fast_ret_ctx;
	
	//intel vmexit handler
	if (mapper_ctx_hv.type == hv_intel_vendor) 
	{
		//get exit instr len
		core_ctx.cur_instr_len = _vmx_vmread(VMCS_VMEXIT_INSTRUCTION_LENGTH);

		//call intel handler
		handled = vmexit_handler_intel(core_ctx);
	}
	
	//amd vmexit handler
	else
	{
		//get exit instr len
		const auto vmcb = get_vmcb();
		core_ctx.cur_instr_len = vmcb->nrip - vmcb->rip;
		
		//setup vmcb ctx
		core_ctx.vm_state = (hv_vm_state_ctx*)vmcb;
		
		//call amd vmexit handler
		handled = vmexit_amd(core_ctx);
	}
	
	//check need skip instr
	if (handled) {
		core_ctx.advance_rip();
		vmexit_handler_ret_asm();
		__builtin_unreachable();
	}

	//call org hyperv vmexit handler
	return call_ptr<u64>(mapper_ctx_hv.exit_handler_fn, a1, a2);
}

naked pv vmexit_handler_asm(pv, pv, pv)
{
	__asm
	{
		//get hv context reg
		mov eax, mapper_ctx_hv.type
		test eax, eax
		jz save_xmm

		//get intel ctx ptr
		mov r8, [rcx]

		//save guest xmm6-xmm15
		save_xmm:
		movaps xmmword ptr [r8] hv_guest_ctx.xmm6, xmm6
		movaps xmmword ptr [r8] hv_guest_ctx.xmm7, xmm7
		movaps xmmword ptr [r8] hv_guest_ctx.xmm8, xmm8
		movaps xmmword ptr [r8] hv_guest_ctx.xmm9, xmm9
		movaps xmmword ptr [r8] hv_guest_ctx.xmm10, xmm10
		movaps xmmword ptr [r8] hv_guest_ctx.xmm11, xmm11
		movaps xmmword ptr [r8] hv_guest_ctx.xmm12, xmm12
		movaps xmmword ptr [r8] hv_guest_ctx.xmm13, xmm13
		movaps xmmword ptr [r8] hv_guest_ctx.xmm14, xmm14
		movaps xmmword ptr [r8] hv_guest_ctx.xmm15, xmm15

		//save non volatile hvctx
		push rbp
		push rbx
		push rsi
		push rdi
		push r15
		push r14
		push r13
		push r12

		//save guest context ptr
		push r8

		//save ret ctx
		mov r9, rsp

		//call vmexit handler cpp
		sub rsp, 0x30
		call vmexit_handler_cpp
		add rsp, 0x78
		ret
	}
}