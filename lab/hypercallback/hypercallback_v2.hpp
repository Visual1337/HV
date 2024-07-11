#include "save_guest_state.hpp"
#include  "../../windows/win_utils.hpp"

class hc_trapframe
{
public:
	u64 r12;	 //0x00
	u64 r13;	 //0x08
	u64 r14;	 //0x10
	u64 r15;	 //0x18
	u64 rdi;	 //0x20
	u64 rsi;	 //0x28
	u64 rbx;	 //0x30
	u64 rbp;	 //0x38
	u64 rsp;	 //0x40
	u64 rip;	 //0x48

	M128A xmm6;	 //0x50
	M128A xmm7;	 //0x60
	M128A xmm8;	 //0x70
	M128A xmm9;	 //0x80
	M128A xmm10; //0x90
	M128A xmm11; //0xA0
	M128A xmm12; //0xB0
	M128A xmm13; //0xC0
	M128A xmm14; //0xD0
	M128A xmm15; //0xE0
};

class hc_ctx_desc
{
public:
	double_llist link;
	u64 index;
	u64 ret_key;
	u64 ret_rip;
	pb stack_addr;
	hc_trapframe trap;
};

//pages vars
constexpr int hc_ctx_cnt = 12;
#pragma clang section bss=".hc_ctx"
hc_ctx_desc hc_ctx_desc_data[hc_ctx_cnt];
fspinlock_static hc_ctx_free_entry_list_lock;
double_llist_head<hc_ctx_desc> hc_ctx_free_entry_list;
#pragma clang section bss=""

//HC entry mgmnt
noinl void init_hypercallback()
{
	hc_ctx_free_entry_list.init();

	constexpr int stack_pages_per_entry = 2;
	for (int i = 0; i < hc_ctx_cnt; ++i) 
	{
		pb last_alloc_ptr = nullptr;
		for (int a = 0; a < stack_pages_per_entry; ++a) {
			last_alloc_ptr = alloc_page()->addr;
		}

		auto entry = &hc_ctx_desc_data[i];
		entry->index = i;
		entry->stack_addr = last_alloc_ptr + 
			(stack_pages_per_entry * PAGE_4KB) - 0x28;

		hc_ctx_free_entry_list.add_head(entry);
	}
}

noinl hc_ctx_desc* alloc_hc_entry() {
	hc_ctx_free_entry_list_lock.lock();
	auto entry = hc_ctx_free_entry_list.remove_head();
	hc_ctx_free_entry_list_lock.unlock();
	return entry;
}

naked void free_hc_entry(u64 ret_ctx, u64 ret_val, hc_ctx_desc* entry)
{
	__asm{
		try_lock_spinlock:
		mov     eax, 1
		xchg    eax, dword ptr hc_ctx_free_entry_list_lock
		test    eax, eax
		jz link_free_hc_entry_to_list
		pause
		jmp try_lock_spinlock
		
		link_free_hc_entry_to_list:
		lea rax, hc_ctx_free_entry_list
		mov r9, [rax]
		mov [r8], r9
		mov [r8 + 8], rax
		mov [r9 + 8], r8
		mov [rax], r8
		
		//unlock_spinlock:
		mov	dword ptr hc_ctx_free_entry_list_lock, 0

		//leave_from_vm:
		jmp vmexit_handler_ret_asm
	}
}

//HC internal mgmnt
naked void hc_restore_tf(hc_trapframe* tf)
{
	__asm 
	{
		//int regs
		mov r12, [rcx + 0x00]
		mov r13, [rcx + 0x08]
		mov r14, [rcx + 0x10]
		mov r15, [rcx + 0x18]
		mov rdi, [rcx + 0x20]
		mov rsi, [rcx + 0x28]
		mov rbx, [rcx + 0x30]
		mov rbp, [rcx + 0x38]
		mov rsp, [rcx + 0x40]

		//float regs
		movups xmm6,  [rcx + 0x50]
		movups xmm7,  [rcx + 0x60]
		movups xmm8,  [rcx + 0x70]
		movups xmm9,  [rcx + 0x80]
		movups xmm10, [rcx + 0x90]
		movups xmm11, [rcx + 0xA0]
		movups xmm12, [rcx + 0xB0]
		movups xmm13, [rcx + 0xC0]
		movups xmm14, [rcx + 0xD0]
		movups xmm15, [rcx + 0xE0]

		//goto rip
		jmp qword ptr [rcx + 0x48]
	}
}

noinl bool handle_hc_return(u64 key, u64 rip)
{
	//check return entry key
	const int entry_index = key % hc_ctx_cnt;
	auto& ret_entry = hc_ctx_desc_data[entry_index];
	if (!ret_entry.ret_key || 
		(ret_entry.ret_key != key) ||
		(ret_entry.ret_rip != rip)) {
		return false;
	}
	
	fmtp("return from hc! %p", &ret_entry);
	
	//reset ret state
	ret_entry.ret_rip = 0;
	ret_entry.ret_key = 0;

	//load hc context
	hc_restore_tf(&ret_entry.trap);
	__builtin_unreachable();
}

naked void hc_ret_to_guest(u64 fastret_ctx, u64 ret_val, hc_trapframe* tf)
{
	__asm
	{
		//save non volatile int
		mov [r8 + 0x00], r12 
		mov [r8 + 0x08], r13 
		mov [r8 + 0x10], r14 
		mov [r8 + 0x18], r15 
		mov [r8 + 0x20], rdi 
		mov [r8 + 0x28], rsi 
		mov [r8 + 0x30], rbx 
		mov [r8 + 0x38], rbp 
		
		//save non volatile xmm
		movups [r8 + 0x50], xmm6
		movups [r8 + 0x60], xmm7
		movups [r8 + 0x70], xmm8
		movups [r8 + 0x80], xmm9
		movups [r8 + 0x90], xmm10
		movups [r8 + 0xA0], xmm11
		movups [r8 + 0xB0], xmm12
		movups [r8 + 0xC0], xmm13
		movups [r8 + 0xD0], xmm14
		movups [r8 + 0xE0], xmm15

		//save rsp
		lea rax, [rsp + 8]
		mov [r8 + 0x40], rax

		//save rip
		mov rax, [rsp]
		mov [r8 + 0x48], rax

		//goto fastret
		jmp vmexit_handler_ret_asm
	}
}

noinl void run_hc(pv func, u64 ctx = 0)
{
	const auto entry = alloc_hc_entry();
	u64 new_rsp = (u64)entry->stack_addr;
	
	__asm {
		mov rax, func
		mov rcx, entry
		mov rdx, ctx
		mov rsp, new_rsp
		jmp rax
	}

	__builtin_unreachable();
}

//switch cpl
noinl void hc_emulate_swapgs(core_data_t& core_ctx)
{
	u64 gs1 = core_ctx.vm_state->gsbase;
	u64 gs2 = core_ctx.vm_state->kernelgsbase;

	core_ctx.vm_state->gsbase = gs2;
	core_ctx.vm_state->kernelgsbase = gs1;
}

bool fk1 = false;

class hc_main
{
public:
	core_data_t* core_data_ptr;

private:
	hc_ctx_desc* desc;
	bool need_swap_gs;
	hc_guest_ctx saved_state;

	finl void update_core_data_ptr() {
		core_data_ptr = get_core_data();
	}
	
	noinl void hc_km_to_um()
	{
		sp("hc_km_to_um: no implement!");
		__fastfail(1);

		//auto vmcb = get_vmcb();

		////set cs
		//vmcb->csselector = 0x33;
		//vmcb->csattrib = 0x2FB;
		//vmcb->cslimit = 0xFFFFFFFF;
		//vmcb->csbase = 0x0;

		////set ss
		//vmcb->ssselector = 0x2B;
		//vmcb->ssattrib = 0xCF3;
		//vmcb->sslimit = 0xFFFFFFFF;
		//vmcb->ssbase = 0x0;

		////set rpl
		//vmcb->cpl = 3;

		////emu swapgs
		//hc_emulate_swapgs();

		////update vmcb cache (SEG)
		//vmcb->vmcbclean.reset(8);

		////FIXME: set MXCSR
		//
		// //FIXME: CALC SHIFT STACK!!!
		//const u64 new_rsp = saved_state.rsp;
		//
		////set gs:[KPRCB -> RspBase]
	 //   uf::write_gva<u64>(vmcb->cr3, vmcb->kernelgsbase + 0x1A8, new_rsp);
		//
		////set gs:[KPCR -> TssBase -> Rsp0]
		//auto tss_base = uf::read_gva<u64>(vmcb->cr3, vmcb->kernelgsbase + 0x8);
		//uf::write_gva<u64>(vmcb->cr3, tss_base + 0x4, new_rsp);

		////mov rsp, ethread->trapframe->rsp //load um rsp
		//const auto curr_ethread = amd_get_current_ethread();
		//const auto th_tf = uf::read_gva<u64>(vmcb->cr3, curr_ethread + 0x90);
		//vmcb->rsp = uf::read_gva<u64>(vmcb->cr3, th_tf + 0x180);
	}

	noinl void hc_um_to_km()
	{
		sp("hc_um_to_km");
		__fastfail(1);

		//set cs
		//core_data_ptr->vm_state.csselector = 0x10;
		//core_data_ptr->vm_state.csattrib = 0x29B;
		//core_data_ptr->vm_state.cslimit = 0xFFFFFFFF;
		//core_data_ptr->vm_state.csbase = 0x0;
		//
		////set ss
		//core_data_ptr->vm_state.ssselector = 0x18;
		//core_data_ptr->vm_state.ssattrib = 0x493;
		//core_data_ptr->vm_state.sslimit = 0x0;
		//core_data_ptr->vm_state.ssbase = 0x0;
		//
		////set CPL
		//if constexpr (std::is_same_v<T, core_data_amd_t>) {
		//	core_data_ptr->vm_state.cpl = 0;
		//}
		//
		////emu swapgs
		//hc_emulate_swapgs();
		//
		////update vmcb cache (SEG)
		//if constexpr (std::is_same_v<T, core_data_amd_t>) {
		//	core_data_ptr->vm_state.vmcbclean.reset(8);
		//}
		//
		////mov rsp, gs:1A8h //load km rsp
		//core_data_ptr->context->rsp = 
		//	uf::read_gva<u64>(
		//		core_data_ptr->vm_state.cr3,
		//		core_data_ptr->vm_state.gsbase + 0x1A8);
	}

	noinl void call_internal(u64 func, u64* stack_vars, int stack_args_cnt)
	{
		auto gctx = core_data_ptr->context;
		
		//detect cpl
		int need_cpl = (func > 0x7FFFFFFFFFFF) ? 0 : 3;
		int cur_cpl = (core_data_ptr->vm_state->rip > 0x7FFFFFFFFFFF) ? 0 : 3;

		//switch cpl
		bool need_switch_cpl = need_cpl != cur_cpl;
		if (need_switch_cpl)
		{
			//restore org th state
			if (need_cpl == saved_state.cpl) {
				sp("restore org th state");
				//saved_state.restore_seg(core_data_ptr);
				hc_emulate_swapgs(*core_data_ptr);
				//core_data_ptr->context->rsp = saved_state.rsp;
				need_swap_gs = false;
			}

			else 
			{
				need_swap_gs = true;

				//switch um -> km exec
				if ((need_cpl == 0) && (cur_cpl == 3)) {
					sp("um -> km");
					hc_um_to_km();

					u64 gg32132 = core_data_arr->context->rsp;
					hp(gg32132);
				}

				//switch km -> um exec
				else if ((need_cpl == 3) && (cur_cpl == 0)) {
					sp("km -> um");
					hc_km_to_um();
				}
			}
		}

		//alloc & rebase stack
		const u64 old_rsp = core_data_ptr->context->rsp;
		u64 new_rsp = old_rsp - 0x100;
		new_rsp = falign(new_rsp, -16) - 8;
		core_data_ptr->context->rsp = new_rsp;

		//set reg args
		//gctx->rcx = a1;
		//gctx->rdx = a2;
		//gctx->r8 = a3;
		//gctx->r9 = a4;

		//set stack args
		const u64 dirbase1 = core_data_ptr->vm_state->cr3;
		write_gva_arr(dirbase1, new_rsp + 0x28, stack_vars, stack_args_cnt * 8);

		//set ret addr
		const u64 fast_ret_ptr = (need_cpl == 0) ? 
			hc_data.fast_ret_km : hc_data.fast_ret_um;
		write_gva(dirbase1, new_rsp, fast_ret_ptr);

		//set rip
		core_data_ptr->vm_state->rip = func;

		//set rflags
		core_data_ptr->vm_state->rflags = 0x246;

		//gen ret key
		const u64 tstamp = __rdtsc();
		const int tstamp_remain = tstamp % hc_ctx_cnt;
		const int tstamp_shift = desc->index - tstamp_remain;
		const u64 ret_key = tstamp + tstamp_shift;

		//save ret key
		desc->ret_rip = fast_ret_ptr;
		desc->ret_key = ret_key;
		gctx->r12 = ret_key;
		
		//u64 gs11 = core_data_ptr->vm_state.gsbase;
		//u64 gs22 = core_data_ptr->vm_state.kernelgsbase;

		//
		//u64 th1 = amd_get_current_ethread();
		//u64 init_rsp = uf::read_gva<u64>(dirbase1, th1 + 0x28);
		//u64 call_state_rsp = uf::write_gva<u64>(dirbase1, init_rsp + 0x20, 0xFFFFFFFFFFFFFFFF);


		//fast ret to guest
		//auto& guest_irql = *(std::bitset<4>*) & core_data_amd->vm_state.vintr;
		//auto guest_irql1 = guest_irql;
		//guest_irql = DISPATCH_LEVEL;

		//core_data_amd->vm_state.interruptshadow.set(1, 0);
		//core_data_amd->vm_state.vmcbclean.set(3, 0);


		//dp(get_current_core_id());
		
		sp("hc enter!");
		hc_ret_to_guest(
			core_data_ptr->fast_restore_context,
			__readgsqword(0), &desc->trap);
		sp("hc leave!");
		
		//core_data_amd->vm_state.interruptshadow.set(1, 1);
		//core_data_amd->vm_state.vmcbclean.set(3, 0);


		//guest_irql = guest_irql1;
		//dp(get_current_core_id());


		//if (core_data_ptr != get_core_data())
		//{
		//	u64 gs11_ = get_core_data<core_data_amd_t>()->vm_state.gsbase;
		//	u64 gs22_ = get_core_data<core_data_amd_t>()->vm_state.kernelgsbase;
		//	
		//	hp(gs11);
		//	hp(gs22);

		//	sp("**");

		//	hp(uf::read_gva<u64>(dirbase1, gs11 + 0x1a8));
		//	hp(uf::read_gva<u64>(dirbase1, gs22 + 0x1a8));

		//	sp("** 1");

		//	hp(gs11_);
		//	hp(gs22_);

		//	sp("**");

		//	hp(uf::read_gva<u64>(dirbase1, gs11_ + 0x1a8));
		//	hp(uf::read_gva<u64>(dirbase1, gs22_ + 0x1a8));

		//	sp("** 2");

		//	u64 th1 = amd_get_current_ethread();
		//	u64 init_rsp = uf::read_gva<u64>(dirbase1, th1 + 0x28);
		//	u64 call_state_rsp = uf::read_gva<u64>(dirbase1, init_rsp + 0x20);
		//	hp(th1);
		//	hp(init_rsp);
		//	hp(call_state_rsp);

		//	sp("** 3");

		//	fk1 = 1;
		//	//__fastfail(1);
		//}

		//restore rsp
		update_core_data_ptr();
		gctx = core_data_ptr->context;
		//core_data_ptr->context->rsp = old_rsp;
	}

	void run(hc_ctx_desc* hc_desc) {
		need_swap_gs = false;
		desc = hc_desc;
		update_core_data_ptr();
		saved_state.save_state();
	}

	void end() {
		if (need_swap_gs) {
			sp("swapgs");
			hc_emulate_swapgs(*core_data_ptr);
		}
		sp("restore_state");
		saved_state.restore_state();
	}

public:
	hc_main() = delete;

	hc_main(hc_ctx_desc* hc_desc) {
		run(hc_desc);
	}

	~hc_main() {
		end();
	}

	template<typename RetT = u64, typename PtrT, typename... ArgsT>
	finl RetT call_func(PtrT func_addr, ArgsT... args_list)
	{
		constexpr int args_count = sizeof...(ArgsT);
		static_assert(args_count <= 8, "hc: too many args!");
		constexpr int stack_args_count = (args_count > 4) ? (args_count - 4) : 0;
		
		u64 stack_args_pack[stack_args_count];
		auto move_args = [&]<typename VarT>(int index, const VarT& var_data) {
			const u64 cast_var = fbitcast<u64>(var_data);
			switch (index) {
				case 0: core_data_ptr->context->rcx = cast_var; break;
				case 1:	core_data_ptr->context->rdx = cast_var; break;
				case 2:	core_data_ptr->context->r8 = cast_var; break;
				case 3:	core_data_ptr->context->r9 = cast_var; break;
				default: stack_args_pack[(index - 4)] = cast_var; break;
			}
		};

		/*for (size_t i = 0; i < stack_args_count; i++)
		{
			hp(stack_args_pack[i]);
		}*/


		std::apply([&](auto const&... Var) { 
			int i{}; (move_args(i++, Var), ...); 
		}, std::tuple(args_list...));

		call_internal(func_addr, stack_args_pack, stack_args_count);

		return core_data_ptr->context->rax;



		////void
		////if constexpr (std::is_same_v<T, void>) {
		////	return;
		////}
		//
		//float
		//if constexpr (std::is_floating_point_v<T>) {
		//	return *(T*)&gctx->xmm0;
		//}
		//
		////int
		//const u64 rax1 = gctx->rax;
		//return *(T*)&rax1;
	}
};


