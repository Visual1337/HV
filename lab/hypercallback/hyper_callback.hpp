u64 fastRet1;

//callback vars
struct callback_ret1 {
	u64 r12_key;
	u64 stack_load_ptr;
};
constexpr int callback_cnt = 10;
volatile u64 callback_map[callback_cnt];
callback_ret1 callbacks_list[callback_cnt];

//alloc callback
noinl std::pair<u64*, callback_ret1*> alloc_callback()
{
	//take pages arr lock
	fspinlock_static spinlock;
	spinlock.lock();

	for (int i = 0; i < callback_cnt; ++i)
	{
		if (auto& map_data = callback_map[i]; !map_data)
		{
			const auto page_ptr = &callbacks_list[i];

			map_data = 1;
			spinlock.unlock();

			return std::pair((u64*)&map_data, (callback_ret1*)page_ptr);
		}
	}

	spinlock.unlock();
	return std::pair((u64*)0, (callback_ret1*)0);
}

//prep
naked void prep_callback2(u64 stack_buff, u64 new_rsp, u64 new_rip) {
	__asm {
		mov rsp, rdx
		jmp r8
	}
}

noinl void prep_callback(pv callbacked_routine, u64 fast_ret_ctx) {
	auto test_p1 = alloc_page();
	auto stack_base = test_p1.buffer + 0x1000 - 0x28;
	prep_callback2(test_p1.pte->pfn, (u64)stack_base, (u64)callbacked_routine);
}

//callback return
naked void return_from_callback2(u64 new_rsp, pv used_cal_ret_flag) {
	__asm {
		mov rsp, rcx
		mov qword ptr [rdx], 0

		pop rbp
		pop rbx
		pop rsi
		pop rdi
		pop r15
		pop r14
		pop r13
		pop r12

		ret
	}
}

noinl void return_from_callback(guest_ctx_amd* a3) {
	for (int i = 0; i < callback_cnt; ++i) {
		if (auto& used_ent1 = callback_map[i]) {
			auto& call_ctx = callbacks_list[i];
			if (a3->r12 == call_ctx.r12_key) {
				return_from_callback2(call_ctx.stack_load_ptr, (pv)&used_ent1);
			}
		}
	}
}

//callback main
void hv_callback3() {
	get_core_data<core_data_amd_t>().fast_ret();
}

naked u64 hv_callback2(u64 callback_ret_ctx)
{
	__asm {
		push r12
		push r13
		push r14
		push r15
		push rdi
		push rsi
		push rbx
		push rbp

		//save stack ret ptr
		mov [rcx + 8], rsp

		//fast ret
		sub rsp, 0x28
		call hv_callback3

		ret //IDA FIX
	}
}

class callbackRegs 
{
private:
	u64 rax;
	u64 rcx;
	u64 rdx;
	u64 r8;
	u64 r9;
	u64 r10;
	u64 r11;
	u64 r12;
	u64 r13;
	u64 r14;
	u64 r15;
	u64 rsi;
	u64 rbx;
	u64 rbp;
	u64 rsp;
	u64 rip;
	u64 nrip;


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

public:
	noinl void save() {
		const auto& core_tt1 = get_core_data<core_data_amd_t>();
		rax	 = core_tt1.context->rax;	
		rcx	 = core_tt1.context->rcx;	
		rdx	 = core_tt1.context->rdx;	
		r8	 = core_tt1.context->r8;
		r9	 = core_tt1.context->r9;
		r10	 = core_tt1.context->r10;	
		r11	 = core_tt1.context->r11;	
		r12	 = core_tt1.context->r12;	
		r13	 = core_tt1.context->r13;	
		r14	 = core_tt1.context->r14;	
		r15	 = core_tt1.context->r15;	
		rsi	 = core_tt1.context->rsi;	
		rbx	 = core_tt1.context->rbx;	
		rbp	 = core_tt1.context->rbp;	
		rsp	 = core_tt1.context->rsp;	
		rip	 = core_tt1.context->rip;	
		nrip = core_tt1.context->nrip;

		xmm0  = core_tt1.context->xmm0;
		xmm1  = core_tt1.context->xmm1;
		xmm2  = core_tt1.context->xmm2;
		xmm3  = core_tt1.context->xmm3;
		xmm4  = core_tt1.context->xmm4;
		xmm5  = core_tt1.context->xmm5;
		xmm6  = core_tt1.context->xmm6;
		xmm7  = core_tt1.context->xmm7;
		xmm8  = core_tt1.context->xmm8;
		xmm9  = core_tt1.context->xmm9;
		xmm10 = core_tt1.context->xmm10;
		xmm11 = core_tt1.context->xmm11;
		xmm12 = core_tt1.context->xmm12;
		xmm13 = core_tt1.context->xmm13;
		xmm14 = core_tt1.context->xmm14;
		xmm15 = core_tt1.context->xmm15;
	}

	noinl void load(/*core_data_t* core_tt2*/)
	{
		auto& core_tt2 = get_core_data<core_data_amd_t>();
		core_tt2.context->rax = rax ;
		core_tt2.context->rcx = rcx ;
		core_tt2.context->rdx = rdx ;
		core_tt2.context->r8 = r8 ;
		core_tt2.context->r9 =  r9 ;
		core_tt2.context->r10 = r10 ;
		core_tt2.context->r11 = r11 ;
		core_tt2.context->r12 = r12 ;
		core_tt2.context->r13 = r13 ;
		core_tt2.context->r14 = r14 ;
		core_tt2.context->r15 = r15 ;
		core_tt2.context->rsi = rsi ;
		core_tt2.context->rbx = rbx ;
		core_tt2.context->rbp = rbp ;
		core_tt2.context->rsp = rsp ;
		core_tt2.context->rip = rip ;
		core_tt2.context->nrip = nrip;

		core_tt2.context->xmm0   = xmm0;
		core_tt2.context->xmm1   = xmm1;
		core_tt2.context->xmm2   = xmm2;
		core_tt2.context->xmm3   = xmm3;
		core_tt2.context->xmm4   = xmm4;
		core_tt2.context->xmm5   = xmm5;
		core_tt2.context->xmm6   = xmm6;
		core_tt2.context->xmm7   = xmm7;
		core_tt2.context->xmm8   = xmm8;
		core_tt2.context->xmm9   = xmm9;
		core_tt2.context->xmm10 = xmm10;
		core_tt2.context->xmm11 = xmm11;
		core_tt2.context->xmm12 = xmm12;
		core_tt2.context->xmm13 = xmm13;
		core_tt2.context->xmm14 = xmm14;
		core_tt2.context->xmm15 = xmm15;
	}
};

callbackRegs old_regs;
noinl u64 hv_callback(u64 func, volatile u64 ret_, u64 a1, u64 a2, u64 a3, u64 a4, u64 a5 = 0, u64 a6 = 0)
{
	old_regs.save();

	u64 call_key = __rdtsc();

	auto callback1 = alloc_callback();
	callback1.second->r12_key = call_key;

	auto& core_tt3 = get_core_data<core_data_amd_t>();
	
	u64 new_rsp = core_tt3.context->rsp;
	new_rsp -= 0x100;
	new_rsp &= 0xFFFFFFFFFFFFFF00;
	new_rsp -= 8;
	core_tt3.context->rsp = new_rsp;
	
/*	auto ret_wrt = */write_guest_va(core_tt3.context->cr3, new_rsp, ret_);
	write_guest_va(core_tt3.context->cr3, new_rsp + 0x28, a5);
	write_guest_va(core_tt3.context->cr3, new_rsp + 0x30, a6);

	//hp(core_tt3->vmcb->rsp);
	//hp((u64)ret_wrt);
	//hp(read_guest_va<u64>(core_tt3->vmcb->cr3, core_tt3->vmcb->rsp));
	//hp((u64)&ret_);

	core_tt3.context->rcx = a1;
	core_tt3.context->rdx = a2;
	core_tt3.context->r8 =  a3;
	core_tt3.context->r9 =  a4;
	core_tt3.context->r12 = call_key;
	core_tt3.context->rip = func;

	/*auto core_data_t1 = (core_data_t*)*/hv_callback2((u64)callback1.second);
	
	//auto core_ret = core_data_t1->vmcb->rax;

	old_regs.load(/*core_data_t1*/);

	return 0;// core_ret;
}

inl u64 GetVFunc(u64 Class, int Index)
{
	auto vft = read_guest_va<u64>(get_vmcb()->cr3, Class);
	if (!vft) {
		sp("callback: vft null!");
		return 0;
	}

	auto vfunc = read_guest_va<u64>(get_vmcb()->cr3, vft + (Index * 8));
	if (!vfunc) {
		sp("callback: vfunc null!");
		return 0;
	}

	return vfunc;
}

naked void last_ret(u64 page_free_pfn, u64 ret_ctx)
{
	__asm 
	{
		sub rsp, 0x28
		mov rsi, rdx
		call free_page_by_pfn
		add rsp, 0x28 // NEVER RET!

		mov rcx, rsi
		mov rdx, gs:[0]
		//jmp vmexit_handler_ret_asm
	}
	
}

