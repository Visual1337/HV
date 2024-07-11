#include "npt.hpp"

noinl bool vmexit_amd(core_data_t& core_ctx)
{
	bool handled = false;
	switch (core_ctx.vm_state->exitcode)
	{
		case VMEXIT_CPUID: {
			handled = handle_cpuid(core_ctx);
		} break;

		case VMEXIT_NPF: {
			if (hv_init && handle_npt_fault(core_ctx)) {
				core_ctx.adjust_rip = false;
				handled = true;
			}
		} break;
	
		//case VMEXIT_EXCEPTION_PF:
		//case VMEXIT_EXCEPTION_BP:
		//case VMEXIT_EXCEPTION_DB: {
		//	//if (hv_init) {
		//	fmtp("trap! rip:0x%p", (u64)core_ctx.context->rip);
		//	__fastfail(3);
		//	//}
		//} break;
	}
	
	return handled;
}