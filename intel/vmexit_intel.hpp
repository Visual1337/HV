#include "ept_mtf.hpp"

noinl bool vmexit_handler_intel(core_data_t& core_ctx)
{
	bool handled = false;
	switch (core_ctx.vm_state->exitcode)
	{
		case VMX_EXIT_REASON_EXECUTE_CPUID: {
			handled = handle_cpuid(core_ctx);
		} break;
	
		case VMX_EXIT_REASON_EPT_VIOLATION: {
			if (hv_init) {
				core_ctx.adjust_rip = false;
				handled = handle_ept(core_ctx);
			} break;
		}
	
		case VM_EXIT_EPT_MISCONFIG: {
			//sp("VM_EXIT_EPT_MISCONFIG"); 
			fast_kill(__LINE__);
			//fast_kill(__LINE__);
			break;
		}
	
		//case VMX_EXIT_REASON_MONITOR_TRAP_FLAG:
		//{
		//	if (hv_init) {
		//		handled = handle_mtf(core_ctx);
		//	} break;
		//}
	}

	return handled;
}