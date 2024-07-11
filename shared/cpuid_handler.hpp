void alloc_hc_pages();

struct npt_entry_hook
{
	bool used;
	bool active;
	bool have_parent;

	u64 virt_addr;
	u64 guest_phys_addr;
	alloc_page_desc* shadow_page;

	union {
		npt_entry_hook* parent;
		u64 shadow_base;
	};
};

u64 set_npt_hook(
	core_data_t& core_ctx,
	u64 guest_cr3,
	u64 guest_va_page,
	u64 patch_buff,
	npt_entry_hook* parent);

bool handle_hv_init(hv_guest_ctx& guest_context)
{
	if (guest_context.rcx == hv_init_cmd)
	{
		//init mem-mgr
		init_memmgr();

		//init hypercallback (HC)
		//init_hypercallback();

		//alloc shadow ndirbase (amd only)
		//if constexpr (std::is_same_v<T, guest_ctx_amd*>) {
		//	alloc_shadow_npt_template();
		//}

		//init hv base
		fake_page_pfn = alloc_page()->org_pfn;
		allowed_va_range = { guest_context.r8, guest_context.r9 };
		
		//u64 dd = IsAddrValid(mapper_ctx_hv.exit_handler_fn)->pfn;
		//hp(dd);
		//
		//ept_mgr::process_org_mappings();
	
		fmtp("hv init ok! core: %d", get_current_core_id());
		
		//init success!
		hv_init = true;
		return true;
	}

	return false;
}

void handle_hv_test(auto& guest_context)
{
	
}

bool handle_cpuid(core_data_t& core_ctx)
{
	auto& ctx = core_ctx.context;

	//hypercallback
	//if (hv_init)
	//{
	//	//continue hypercallback
	//	handle_hc_return(ctx->r12, core_ctx.vm_state->rip);

	//	//enter hypercallback
	//	if (hc_data.present_rip && 
	//		(core_ctx.vm_state->rip == hc_data.present_rip)) {
	//		run_hc(test_present);
	//	}
	//}

	//check hv key
	if ((u64)ctx->rax != mapper_ctx_hv.exit_handler_key) {
		return false;
	}
	
	//handle hv init
	if (hv_init)
	{
		//check allowed rip
		if (!addr_in_allow_range(core_ctx.vm_state->rip)) {
			return false;
		}

		//call handler
		switch (ctx->rcx)
		{
		//	case hv_hc_init_cmd: {
		//		read_gva_arr(core_ctx.vm_state->cr3, core_ctx.context->rdx, &hc_data, sizeof(hc_data));
		//		hp(hc_data.fast_ret_um);
		//		hp(hc_data.fast_ret_km);
		//		hp(hc_data.present_rip);

		//		sp("hc ok!");
		//		//ctx->rax = 1;
		//	} break;

			case hv_test_cmd: {
				handle_hv_test(ctx);
			} break;

			case hv_install_shadow_hook_cmd: 
			{
				if (mapper_ctx_hv.type == hv_amd_vendor) {
					ctx->rax = set_npt_hook(core_ctx, core_ctx.vm_state->cr3, ctx->rdx, ctx->r8, 0);
				}

				else {
					//ctx->rax = set_ept_hook(core_ctx.vm_state->cr3, ctx->rdx, ctx->r8, 7, ctx->r9);
				}
			} break;

			case hv_delete_all_shadow_hook_cmd: {
				__fastfail(0xe0dead);
				break;
			}

			default: {
				//no handled :((
				fast_kill(0xf1);
				return false;
			}
		}
	}

	//handle hv init
	else if (!handle_hv_init(*core_ctx.context)) {
		return false;
	}

	//handled call
	return true;
}