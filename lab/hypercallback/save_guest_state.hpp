class hc_guest_ctx
{
public:
	//cs segment
	u64 csselector;
	u64 csattrib;
	u64 cslimit;
	u64 csbase;

	//ss segment
	u64 ssselector;
	u64 ssattrib;
	u64 sslimit;
	u64 ssbase;

	//cpl
	int cpl;

	//int
	u64 rax;
	u64 rbx;
	u64 rcx;
	u64 rdx;
	u64 rsi;
	u64 rdi;
	u64 rbp;
	u64 r8;
	u64 r9;
	u64 r10;
	u64 r11;
	u64 r12;
	u64 r13;
	u64 r14;
	u64 r15;

	//xmm
	M128A Xmm0;
	M128A Xmm1;
	M128A Xmm2;
	M128A Xmm3;
	M128A Xmm4;
	M128A Xmm5;
	M128A Xmm6;
	M128A Xmm7;
	M128A Xmm8;
	M128A Xmm9;
	M128A Xmm10;
	M128A Xmm11;
	M128A Xmm12;
	M128A Xmm13;
	M128A Xmm14;
	M128A Xmm15;

	//spec regs
	u64 rsp;
	u64 rip;
	u64 rip_len;
	u64 rflags;

public:
	//seg
	//noinl void save_seg(T* core_data)
	//{
	//	//cs
	//	csselector = core_data->vm_state.csselector;
	//	csattrib = core_data->vm_state.csattrib;
	//	cslimit = core_data->vm_state.cslimit;
	//	csbase = core_data->vm_state.csbase;

	//	//ss
	//	ssselector = core_data->vm_state.ssselector;
	//	ssattrib = core_data->vm_state.ssattrib;
	//	sslimit = core_data->vm_state.sslimit;
	//	ssbase = core_data->vm_state.ssbase;

	//	//cpl
	//	cpl = core_data->vm_state.get_cpl();
	//}

	//noinl void restore_seg(T* core_data)
	//{
	//	//restore cs
	//	core_data->vm_state.csselector = csselector;
	//	core_data->vm_state.csattrib = csattrib;
	//	core_data->vm_state.cslimit = cslimit;
	//	core_data->vm_state.csbase = csbase;

	//	//restore ss
	//	core_data->vm_state.ssselector = ssselector;
	//	core_data->vm_state.ssattrib = ssattrib;
	//	core_data->vm_state.sslimit = sslimit;
	//	core_data->vm_state.ssbase = ssbase;

	//	if constexpr (std::is_same_v<T, core_data_amd_t>)
	//	{
	//		//restore cpl
	//		core_data->vm_state.cpl = cpl;

	//		//update VMCB Clean Field -> SEG -> 8 bit
	//		core_data->vm_state.vmcbclean.reset(8);
	//	}
	//}

	//xmm
	noinl void save_xmm(core_data_t& core_data) {
		__memcpy(&Xmm0, &core_data.context->xmm0, sizeof(M128A) * 16);
	}

	noinl void restore_xmm(core_data_t& core_data) {
		__memcpy(&core_data.context->xmm0, &Xmm0, sizeof(M128A) * 16);
	}

	//int
	noinl void save_int(core_data_t& core_data)
	{
		rax = core_data.context->rax;
		rbx = core_data.context->rbx;
		rcx = core_data.context->rcx;
		rdx = core_data.context->rdx;
		rsi = core_data.context->rsi;
		rdi = core_data.context->rdi;
		rbp = core_data.context->rbp;
		r8 =  core_data.context->r8;
		r9 =  core_data.context->r9;
		r10 = core_data.context->r10;
		r11 = core_data.context->r11;
		r12 = core_data.context->r12;
		r13 = core_data.context->r13;
		r14 = core_data.context->r14;
		r15 = core_data.context->r15;
	}

	noinl void restore_int(core_data_t& core_data)
	{
		core_data.context->rax = rax;
		core_data.context->rbx = rbx;
		core_data.context->rcx = rcx;
		core_data.context->rdx = rdx;
		core_data.context->rsi = rsi;
		core_data.context->rdi = rdi;
		core_data.context->rbp = rbp;
		core_data.context->r8 = r8;
		core_data.context->r9 = r9;
		core_data.context->r10 = r10;
		core_data.context->r11 = r11;
		core_data.context->r12 = r12;
		core_data.context->r13 = r13;
		core_data.context->r14 = r14;
		core_data.context->r15 = r15;
	}

	//svc
	noinl void save_svc(core_data_t& core_data)
	{
		//rip
		rip = core_data.vm_state->rip;
		rip_len = core_data.cur_instr_len;
		
		//rsp
		rsp = core_data.context->rsp;

		//eflags
		rflags = core_data.vm_state->rflags;
	}

	template<bool restore_rip>
	noinl void restore_svc(core_data_t& core_data)
	{
		//rip
		if constexpr (restore_rip) {
			core_data.vm_state->rip = rip;
			core_data.cur_instr_len = rip_len;
		}
		
		//rsp
		core_data.context->rsp = rsp;

		//eflags
		core_data.vm_state->rflags = rflags;
	}

	//
	noinl void save_state()
	{
		auto& core_data = *get_core_data();
		
		//
		//save_seg(core_data);
		save_xmm(core_data);
		save_int(core_data);
		save_svc(core_data);
	}

	noinl void restore_state()
	{
		auto& core_data = *get_core_data();

		//
		//restore_seg(core_data);
		restore_xmm(core_data);
		restore_int(core_data);
		restore_svc<true>(core_data);
	}
};

