fspinlock npt_hooks_lock;
npt_entry_hook npt_hook_list[10];

//alloc hook
npt_entry_hook* alloc_npt_hook(u64 gva, u64 gpa)
{
	npt_hooks_lock.lock();

	const u64 gva_align = gva & 0xFFFFFFFFFFFFF000;
	const u64 gpa_align = gpa & 0xFFFFFFFFFFFFF000;

	for (auto& i : npt_hook_list) {
		if (i.active) {
			if ((gva_align == i.virt_addr) ||
				(gpa_align == i.guest_phys_addr)) {
				npt_hooks_lock.unlock();
				return &i;
			}
		}
	}

	for (auto& i : npt_hook_list) {
		if (!i.used) {
			i.used = true;
			i.virt_addr = gva;
			i.guest_phys_addr = gpa_align;
			npt_hooks_lock.unlock();
			return &i;
		}
	}

	npt_hooks_lock.unlock();
	return nullptr;
}

//splits
bool split_pa_amd_(PTE PageEnt, u64 PageDir)
{
	auto page = alloc_page();
	auto pte1 = (PTE*)page->addr;
	for (int i = 0; i < 512; ++i) {
		pte1[i].value = PageEnt.value;
		pte1[i].large_page = 0;
		pte1[i].pfn += i;
	}

	PageEnt.no_execute = false;
	PageEnt.large_page = false;
	PageEnt.pfn = page->org_pfn;

	write_pa(PageDir, PageEnt.value);
	update_npt();
	return true;

}

bool split_pa_amd(u64 pa, u64 npt_base_pfn = 0)
{
	ns:

	QUERY_NPT test123;
	if (!gpa2hpa_amd(pa, &test123, npt_base_pfn)) {
		return false;
	}

	if (test123.page_size == PAGE_1GB) 
	{
		if (!split_pa_amd_(test123.EPDPTE, test123.PEPDPTE)) {
			return false;
		}
		goto ns;
	}

	if (test123.page_size == PAGE_2MB)
	{
		if (!split_pa_amd_(test123.EPDE, test123.PEPDE)) {
			return false;
		}
		goto ns;
	}

	return test123.page_size == PAGE_4KB;
}

//template
u64 alloc_shadow_npt_template()
{
	constexpr int pd_count = 128;
	std::pair<PTE*, u64> npt_shadow_pds[pd_count];

	//alloc nPDPT
	auto pdpt_page_ = alloc_page();
	auto pdpt_page = (PTE*)pdpt_page_->addr;

	//alloc nPML4
	auto pml4_page_ = alloc_page();
	auto pml4_page = (PTE*)pml4_page_->addr;
	const u64 shadow_npt_pfn = pml4_page_->org_pfn;

	//set once PML4e (512gb first!)
	pml4_page[0].present = 1;
	pml4_page[0].write = 1;
	pml4_page[0].user_supervisor = 0;
	pml4_page[0].no_execute = 0;
	pml4_page[0].pfn = pdpt_page_->org_pfn;

	//alloc pd dir
	for (int i = 0; i < pd_count; ++i) 
	{
		auto pd_page = alloc_page();
		
		//fill pdpt
		pdpt_page[i].present = true;
		pdpt_page[i].write = true;
		pdpt_page[i].no_execute = false;
		pdpt_page[i].user_supervisor = false;
		pdpt_page[i].pfn = pd_page->org_pfn;

		//set pde list
		npt_shadow_pds[i] = {
			(PTE*)pd_page->addr,
			(u64)pd_page->org_pfn
		};
	}

	//fill pd (one entry -> 2mb)
	for (u64 a = 0; a < pd_count; ++a) {
		auto pde_dir = npt_shadow_pds[a].first;
		for (u64 b = 0; b < 512; ++b) {
			pde_dir[b].user_supervisor = false;
			pde_dir[b].present = true;
			pde_dir[b].write = true;
			pde_dir[b].no_execute = true; 
			pde_dir[b].large_page = true;
			pde_dir[b].pfn = (a * PAGE_1GB_PFN) + (b * PAGE_2MB_PFN);
		}
	}

	sp("set_npt_hook: shadow template alloc ok!");
	return shadow_npt_pfn;
}

void update_shadow_template(u64 shadow_npt_pfn, u64 gpa, bool rw, bool x, u64 pfn = 0xFFFFFFFFFFFFFFFF)
{
	split_pa_amd(gpa, shadow_npt_pfn);

	QUERY_NPT npt1;
	gpa2hpa_amd(gpa, &npt1, shadow_npt_pfn);

	npt1.EPTE.present = rw;
	npt1.EPTE.write = rw;
	npt1.EPTE.no_execute = !x;

	if (pfn != 0xFFFFFFFFFFFFFFFF) {
		npt1.EPTE.pfn = pfn;
	}

	write_pa(npt1.PEPTE, npt1.EPTE.value);
	update_npt();
}

//set hook
u64 set_npt_hook(
	core_data_t& core_ctx, 
	u64 guest_cr3,
	u64 guest_va_page,
	u64 patch_buff, 
	npt_entry_hook* parent = nullptr)
{
	auto gpa = gva2gpa2(guest_cr3, guest_va_page);
	if (!gpa) {
		//fmtp("set npt hook: fail -> GPA: %p", gpa);
		return pack_slat_ret_err(0, slat_hook_gva2gpa_err);
	}
	
	auto npt_hook_entry = alloc_npt_hook(guest_va_page, gpa);
	if (!npt_hook_entry) {
		//sp("set npt hook: !npt_hook_entry ->  FAIL!");
		return pack_slat_ret_err(0, slat_hook_alloc_entry_max_err);
	}

	if (!split_pa_amd(gpa, core_ctx.slat_org_dir(false, true))) {
		//sp("set npt hook: fail -> split_pa_amd FALSE");
		return pack_slat_ret_err(0, slat_hook_split_pa_err);
	}

	u64 npt_shadow_base;
	if (!npt_hook_entry->active)
	{
		if (!parent)
		{
			npt_shadow_base = alloc_shadow_npt_template();
			if (!npt_shadow_base) {
				//TODO: free
				sp("set npt hook: alloc_shadow_npt_template -> FALSE!");
				return false;
			}

			npt_hook_entry->shadow_base = npt_shadow_base;
		}
		
		else {
			npt_hook_entry->have_parent = true;
			npt_hook_entry->parent = parent;
			npt_shadow_base = parent->shadow_base;
		}
	}

	u64 gpfn = 0xFFFFFFFFFFFFFFFF;
	if (patch_buff) {
		auto buff = alloc_page();
		gpfn = buff->org_pfn;
		npt_hook_entry->shadow_page = buff;
		read_gva_arr(guest_cr3, guest_va_page, buff->addr, PAGE_4KB);

		//TODO: fix
		read_gva_arr(guest_cr3, patch_buff, buff->addr, PAGE_4KB);
	}

	//make hook active!
	update_shadow_template(npt_shadow_base, gpa, true, true, gpfn);
	
	static int hook_id = 0;
	npt_hook_entry->active = true;

	//
	if (parent) {
		//sp("hk parent ok!");
		return true;
	}

	QUERY_NPT q_npt;
	gpa2hpa_amd(gpa, &q_npt);

	//make hook exec!
	q_npt.EPTE.no_execute = true;
	write_pa(q_npt.PEPTE, q_npt.EPTE.value);

	//:))
	sp("hk ok!");
	return pack_slat_ret_err(hook_id++, slat_hook_ok_err);
}

//handle fault
bool handle_npt_fault(core_data_t& core_ctx)
{
	//fault desc
	NestedPageFaultInfo1 exit_info;
	exit_info.as_uint64 = core_ctx.vm_state->slat_fail_desc;
	
	//fault gpa
	const u64 gpa = core_ctx.vm_state->slat_fail_gpa;
	const u64 gpfn = gpa >> PAGE_SHIFT;

	//npt dir
	const u64 cur_npt_dirbase = core_ctx.vm_state->slat_dir;
	const bool shadow_dir_exit = core_ctx.slat_org_dir() != cur_npt_dirbase;

	//cur rip
	const u64 grip_align = core_ctx.vm_state->rip & 0xFFFFFFFFFFFFF000;

	//exec fail
	if (exit_info.fields.execute)
	{
		for (auto& i : npt_hook_list)
		{
			if (i.used && i.active)
			{
				if (i.guest_phys_addr == gpa)
				{
					//save org npt core dir
					core_ctx.slat_org_dir();

					//left handle
					if (i.virt_addr != grip_align) {
						//add npt hook l
						//sp("add npt hook l");
						set_npt_hook(core_ctx, core_ctx.vm_state->cr3, grip_align, 0, &i);
					}

					//center (this page)
					else { }

					//get shadow base
					u64 shadow_npt_pfn = i.shadow_base;
					if (i.have_parent) {
						shadow_npt_pfn = i.parent->shadow_base;
					}
					
					//switch to shadow base!
					update_npt(shadow_npt_pfn << PAGE_SHIFT);
					return true;
				}

				else if (shadow_dir_exit)
				{
					//right handle
					if (i.virt_addr == grip_align) 
					{
						//add npt hook r
						//sp("add npt hook r");
						set_npt_hook(core_ctx, core_ctx.vm_state->cr3, grip_align + PAGE_4KB, 0, &i);

						//re-try exec (use shadow base)!
						update_npt();
						return true;
					}
				}
			}
		}

		//out handle
		if (shadow_dir_exit) {
			//switch to normal base!
			update_npt(core_ctx.slat_org_dir());
			return true;
		}
	}

	return false;
}