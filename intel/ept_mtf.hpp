//struct guest_hook_entry_t
//{
//	u64 active;
//	u64 no_delete;
//
//	u64 dir_base;
//	u64 virt_addr;
//	u8  patch_len;
//	u8  patch_bytes[15];
//};

//struct alignas(8) ept_hook_t
//{
//	volatile u64 active;
//
//	u64 va;
//
//	u64 ppte;
//	u64 pte_val;
//
//	//pfn
//	u64 orig_host_pfn;
//	u64 orig_guest_pfn;
//	u64 shadow_host_pfn;
//	u64 visible_host_pfn;
//
//	//patch list
//	guest_hook_entry_t patch_offsets[10];
//
//	//
//	void move_changes_to_shadow_page()
//	{
//		auto tmp_page = alloc_page(false);
//		read_pa_in_page_arr(visible_host_pfn << PAGE_SHIFT, tmp_page.first, PAGE_4KB);
//
//		for (auto& i : patch_offsets)
//		{
//			u16 patch_off = i.virt_addr & 0xFFF;
//			if (!patch_off) {
//				break;
//			}
//
//			__memcpy(tmp_page.first + patch_off, &i.patch_bytes, i.patch_len);
//		}
//
//		write_pa_in_page_arr(shadow_host_pfn << PAGE_SHIFT, tmp_page.first, PAGE_4KB);
//		free_page(tmp_page);
//	}
//
//	//
//	void free_entry()
//	{
//		if (active)
//		{
//
//		}
//	}
//};
//
////ept vars
//constexpr size_t ept_hook_cnt = 800;
//volatile u64 ept_hooks_map[ept_hook_cnt];
//volatile ept_hook_t ept_hooks[ept_hook_cnt];
//
//u64 fake_page_pfn;
//
////alloc free ept entrys
//noinl ept_hook_t* alloc_ept()
//{
//	ept_hook_t* ept_hook_entry{};
//
//	for (int i = 0; i < ept_hook_cnt; ++i)
//	{
//		auto& map_data = ept_hooks_map[i];
//		if (!map_data) {
//			map_data = 1;
//			ept_hook_entry = (ept_hook_t*)&ept_hooks[i];
//			break;
//		}
//	}
//
//	return ept_hook_entry;
//}
//
//noinl void free_ept(ept_hook_t* ept_hook_entry)
//{
//	size_t hook_id = ((u64)ept_hook_entry - (u64)&ept_hooks) / sizeof(ept_hook_t);
//	if (hook_id < ept_hook_cnt) {
//		ept_hooks_map[hook_id] = 0;
//	}
//}
//
////update ept
//noinl bool update_ept_entry(u64 ept_entry, u64 ept_data, bool update = false, bool global = false)
//{
//	fspinlock_auto();
//
//	bool ret = write_pa(ept_entry, ept_data);
//
//	if (update) {
//		//if (global) {
//		//	for (auto& i : core_data_arr) {
//		//		i.need_flush_ept = true;
//		//	}
//		//}
//		update_ept(global);
//	}
//
//	return ret;
//}
//
////rebuld ept
//noinl bool rebuild_ept_table(core_data_t* core_ctx, u64 phys, u64 pfn)
//{
//	QUERY_EPT query_ept{};
//	if (!gpa2hpa(phys, &query_ept)) {
//		return false;
//	}
//
//	auto& ept_table_state = core_ctx->ept_table_state;
//
//	ept_table_state.new_pml4 = alloc_page(false);
//	ept_table_state.new_pml3 = alloc_page(false);
//	ept_table_state.new_pml2 = alloc_page(false);
//	ept_table_state.new_pml1 = alloc_page(false);
//
//	read_pa_arr(query_ept.PEPML4E & 0xFFFFFFFFFFFFF000,	ept_table_state.new_pml4.first, PAGE_4KB);
//	read_pa_arr(query_ept.PEPDPTE & 0xFFFFFFFFFFFFF000,	ept_table_state.new_pml3.first, PAGE_4KB);
//	read_pa_arr(query_ept.PEPDE   & 0xFFFFFFFFFFFFF000,	ept_table_state.new_pml2.first, PAGE_4KB);
//	read_pa_arr(query_ept.PEPTE   & 0xFFFFFFFFFFFFF000,	ept_table_state.new_pml1.first, PAGE_4KB);
//
//	auto pml4 = query_ept.EPML4;
//	auto pml3 = query_ept.EPDPTE;
//	auto pml2 = query_ept.EPDE;
//	auto pml1 = query_ept.EPTE;
//
//	pml4.PageFrameNumber =	 ept_table_state.new_pml3.second->pfn;
//	pml3.s.PageFrameNumber = ept_table_state.new_pml2.second->pfn;
//	pml2.s.PageFrameNumber = ept_table_state.new_pml1.second->pfn;
//	
//	pml1.ReadAccess = true;
//	pml1.WriteAccess = true;
//	pml1.ExecuteAccess = true;
//	pml1.UserModeExecute = true;
//	pml1.PageFrameNumber = pfn;
//
//	__memcpy(ept_table_state.new_pml4.first + (query_ept.PEPML4E & PAGE_4KB_OFF), &pml4.Flags,	 8);
//	__memcpy(ept_table_state.new_pml3.first + (query_ept.PEPDPTE & PAGE_4KB_OFF), &pml3.s.Flags, 8);
//	__memcpy(ept_table_state.new_pml2.first + (query_ept.PEPDE   & PAGE_4KB_OFF), &pml2.s.Flags, 8);
//	__memcpy(ept_table_state.new_pml1.first + (query_ept.PEPTE   & PAGE_4KB_OFF), &pml1.Flags,	 8);
//
//	auto ept_pointer = get_ept_ptr();
//	ept_table_state.ept_old_ptr = ept_pointer;
//	ept_pointer.PageFrameNumber = ept_table_state.new_pml4.second->pfn;
//	
//	set_ept_ptr(ept_pointer);
//	update_ept(false);
//
//	return true;
//}
//
//noinl void restore_ept_table(core_data_t* core_ctx)
//{
//	auto& ept_table_state = core_ctx->ept_table_state;
//
//	set_ept_ptr(ept_table_state.ept_old_ptr);
//
//	free_page(ept_table_state.new_pml4);
//	free_page(ept_table_state.new_pml3);
//	free_page(ept_table_state.new_pml2);
//	free_page(ept_table_state.new_pml1);
//}
//
////split
//noinl bool split_1gb_ept(u64 PPDPTE, EPT_PDPTE_1GB PDPTE)
//{
//	auto page1 = alloc_page();
//	auto Page = (EPDE_2MB*)page1.first;
//
//	for (size_t i = 0; i < 512; ++i)
//	{
//		auto& EPDE = Page[i];
//
//		EPDE.Flags = 0;
//
//		EPDE.ReadAccess = true;
//		EPDE.WriteAccess = true;
//		EPDE.ExecuteAccess = true;
//		EPDE.UserModeExecute = true;
//
//		EPDE.LargePage = true;
//		EPDE.MemoryType = PDPTE.MemoryType;
//		EPDE.IgnorePat = PDPTE.IgnorePat;
//		EPDE.SuppressVe = PDPTE.SuppressVe;
//
//		EPDE.PageFrameNumber = (PDPTE.PageFrameNumber << 9) + i;
//	}
//
//	EPT_PDPTE ept_pdpte;
//	ept_pdpte.Flags = 0;
//	ept_pdpte.ReadAccess = true;
//	ept_pdpte.WriteAccess = true;
//	ept_pdpte.ExecuteAccess = true;
//	ept_pdpte.UserModeExecute = true;
//	ept_pdpte.PageFrameNumber = page1.second->pfn;
//
//	return update_ept_entry(PPDPTE, ept_pdpte.Flags, true, true);
//}
//
//noinl bool split_2mb_ept(u64 PEPDE, EPDE_2MB EPDE)
//{
//	auto page1 = alloc_page();
//
//	auto Page = (EPT_PTE*)page1.first;
//
//	for (size_t i = 0; i < 512; ++i)
//	{
//		auto& EPTE = Page[i];
//
//		EPTE.Flags = 0;
//
//		EPTE.ReadAccess = true;
//		EPTE.WriteAccess = true;
//		EPTE.ExecuteAccess = true;
//		EPTE.UserModeExecute = true;
//
//		EPTE.MemoryType = EPDE.MemoryType;
//		EPTE.IgnorePat = EPDE.IgnorePat;
//		EPTE.SuppressVe = EPDE.SuppressVe;
//
//		EPTE.PageFrameNumber = (EPDE.PageFrameNumber << 9) + i;
//	}
//
//	EPT_PDE ept_pde;
//	ept_pde.Flags = 0;
//	ept_pde.ReadAccess = true;
//	ept_pde.WriteAccess = true;
//	ept_pde.ExecuteAccess = true;
//	ept_pde.UserModeExecute = true;
//	ept_pde.PageFrameNumber = page1.second->pfn;
//
//	return update_ept_entry(PEPDE, ept_pde.Flags, true, true);
//}
//
//noinl bool split_pa(u64 guest_pa)
//{
//	QUERY_EPT ept_info;
//	if (!gpa2hpa(guest_pa, &ept_info)) {
//		return false;
//	}
//
//	if (ept_info.page_size == PAGE_4KB) {
//		return true;
//	}
//
//	//split pages
//	switch (ept_info.page_size)
//	{
//	case PAGE_1GB:
//		fmtp("split 1gb");
//		split_1gb_ept(ept_info.PEPDPTE, ept_info.EPDPTE.l);
//		if (!gpa2hpa(guest_pa, &ept_info)) {
//			fmtp("split 1gb fail!");
//			return false;
//		}
//		fmtp("split 1gb ok!");
//
//	case PAGE_2MB:
//		fmtp("split 2mb");
//		split_2mb_ept(ept_info.PEPDE, ept_info.EPDE.l);
//		if (!gpa2hpa(guest_pa, &ept_info)) {
//			fmtp("split 2mb fail");
//			return false;
//		} fmtp("split 2mb ok!");
//
//	case PAGE_4KB:
//		if (ept_info.page_size != PAGE_4KB) {
//			//split (FULL) fail!
//			break;
//		}
//		
//		fmtp("page 4kb ok!");
//		return true;
//		break;
//	}
//
//	fmtp("split (FULL) fail!");
//	return false;
//}
//
////ept hook
//ept_hook_t* add_ept_hook_internal(u64 va, u64 ppte, u64 pte_val, u64 orgPfn, u64 rw_pfn, u64 exec_pfn)
//{
//	//alloc hook ctx
//	auto ept_hook_entry = alloc_ept();
//
//	if (!ept_hook_entry) {
//		return nullptr;
//	}
//
//	//fill ept hook entry
//	ept_hook_entry->va = va;
//	ept_hook_entry->orig_host_pfn = orgPfn;
//	ept_hook_entry->shadow_host_pfn = exec_pfn;
//	ept_hook_entry->visible_host_pfn = rw_pfn;
//
//	//set pte
//	ept_hook_entry->ppte = ppte;
//	ept_hook_entry->pte_val = pte_val;
//
//	EPT_PTE new_ept_pte;
//	new_ept_pte.Flags = pte_val;
//	new_ept_pte.ReadAccess = false;
//	new_ept_pte.WriteAccess = false;
//	new_ept_pte.ExecuteAccess = true;
//	new_ept_pte.UserModeExecute = true;
//	new_ept_pte.PageFrameNumber = exec_pfn;
//
//	//apply hook
//	ept_hook_entry->active = 1;
//	update_ept_entry(ppte, new_ept_pte.Flags, true, true);
//
//	return ept_hook_entry;
//}
//
//bool install_ept_hook(u64 cr3, u64 va, u64 va_src, bool hide_drv)
//{
//	//get guest pa info
//	u64 guest_pa = gva2gpa(cr3, va);
//	fmtp("install ept va:%p src:%p gPA:%p", va, va_src, guest_pa);
//
//	if (!guest_pa) {
//		fmtp("guest phys zero!");
//		return false;
//	}
//
//	//split pages
//	if (!split_pa(guest_pa)) {
//		return false;
//	}
//
//	//get ept paging info
//	QUERY_EPT ept_info;
//	if (!gpa2hpa(guest_pa, &ept_info)) {
//		return false;
//	}
//
//	//not splitted page!
//	if (ept_info.page_size != PAGE_4KB) {
//		return false;
//	}
//
//	//get host org pfn
//	u64 org_pfn = ept_info.EPTE.PageFrameNumber;
//
//	//alloc exec page
//	u64 visible_pfn{}, shadow_pfn{};
//	auto exec_page = alloc_page();
//
//	if (hide_drv) {
//		shadow_pfn = org_pfn;
//		visible_pfn = fake_page_pfn;
//	}
//
//	else {
//		read_guest_va_arr(cr3, va_src, exec_page.first, PAGE_4KB);
//		shadow_pfn = exec_page.second->pfn;
//		visible_pfn = org_pfn;
//	}
//
//	//set ept hook
//	auto ept_entry = add_ept_hook_internal(
//		va,
//		ept_info.PEPTE, ept_info.EPTE.Flags,
//		org_pfn, visible_pfn, shadow_pfn
//	);
//
//	//apply patches
//	if (!hide_drv && ept_entry) {
//		ept_entry->move_changes_to_shadow_page();
//	}
//
//	//cleanup
//	if (!ept_entry || hide_drv) {
//		free_page(exec_page);
//	}
//
//	fmtp("!!ept_entry = %d", !!ept_entry);
//
//	return !!ept_entry;
//}
//
//void remove_all_ept_hooks()
//{
//}

////handle ept violation
//bool handle_ept(core_data_t* core_ctx)
//{
//	sp("handle ept");
//
//	VMX_EXIT_QUALIFICATION_EPT_VIOLATION Qualification;
//	Qualification.Flags = _vmx_vmread(VMCS_EXIT_QUALIFICATION);
//
//	const bool read_failure = (Qualification.ReadAccess) && (!Qualification.EptReadable);
//	const bool write_failure = (Qualification.WriteAccess) && (!Qualification.EptWriteable);
//	const bool exec_failure = (Qualification.ExecuteAccess) && (!Qualification.EptExecutable);
//
//	u64 va = _vmx_vmread(VMCS_EXIT_GUEST_LINEAR_ADDRESS);
//	u64 pa = _vmx_vmread(VMCS_GUEST_PHYSICAL_ADDRESS);
//	u64 page_va_error = va & 0xFFFFFFFFFFFFF000;
//	u64 page_pfn_error = pa >> PAGE_SHIFT;
//	
//	auto rip_va = core_ctx->rip;
//	auto rip_va_align_4k = rip_va & 0xFFFFFFFFFFFFF000;
//
//	for (auto& i : ept_hooks)
//	{
//		if (i.active)
//		{
//			if (page_pfn_error == i.shadow_host_pfn ||
//				page_pfn_error == i.visible_host_pfn)
//			{
//				EPT_PTE ept_pte;
//				ept_pte.Flags = i.pte_val;
//
//				bool self_drv_access = addr_in_allow_range(page_va_error) && addr_in_allow_range(rip_va_align_4k);
//				if (((i.va == rip_va_align_4k) || self_drv_access) && (read_failure || write_failure))
//				{
//					if (self_drv_access)
//					{
//						sp("self access");
//					}
//
//					if (i.va == rip_va_align_4k)
//					{
//						sp("rip self access");
//					}
//
//					u64 peek_pfn = self_drv_access ? i.shadow_host_pfn : i.visible_host_pfn;
//					rebuild_ept_table(core_ctx, pa, peek_pfn);
//
//					core_ctx->ept_ptr = (pv)&i;
//					core_ctx->ept_last_self_memop = true;
//
//					set_mtf(true);
//					return true;
//				}
//
//				if (exec_failure) 
//				{
//					sp("exec access");
//
//					ept_pte.ReadAccess = false;
//					ept_pte.WriteAccess = false;
//					ept_pte.ExecuteAccess = true;
//					ept_pte.UserModeExecute = true;
//					ept_pte.PageFrameNumber = i.shadow_host_pfn;
//				}
//
//				else if (read_failure) 
//				{
//					sp("read access");
//
//					ept_pte.ReadAccess = true;
//					ept_pte.WriteAccess = false;
//					ept_pte.ExecuteAccess = false;
//					ept_pte.UserModeExecute = false;
//					ept_pte.PageFrameNumber = i.visible_host_pfn;
//
//					core_ctx->ept_ptr = (pv)&i;
//				}
//
//				else if (write_failure)
//				{
//					sp("write access");
//
//					ept_pte.ReadAccess = true;
//					ept_pte.WriteAccess = true;
//					ept_pte.ExecuteAccess = false;
//					ept_pte.UserModeExecute = false;
//					ept_pte.PageFrameNumber = i.visible_host_pfn;
//
//					core_ctx->ept_ptr = (pv)&i;
//					core_ctx->ept_last_writen = true;
//
//					set_mtf(true);
//				}
//
//				update_ept_entry(i.ppte, ept_pte.Flags);
//				return true;
//			}
//		}
//	}
//
//	return false;
//}
//
////handle monitor trap flag
//bool handle_mtf(core_data_t* core_ctx)
//{
//	const auto& ept_hook_entry = (ept_hook_t*)core_ctx->ept_ptr;
//
//	EPT_PTE ept_pte;
//	ept_pte.Flags = ept_hook_entry->pte_val;
//
//	bool only_exec = true;
//
//	if (core_ctx->ept_last_self_memop) 
//	{
//		restore_ept_table(core_ctx);
//		core_ctx->ept_last_self_memop = false;
//
//		fmtp("mtf self");
//	}
//
//	else if (core_ctx->ept_last_writen)
//	{
//		ept_hook_entry->move_changes_to_shadow_page();
//
//		core_ctx->ept_last_writen = false;
//		only_exec = false;
//
//		fmtp("mtf write");
//	}
//
//	if (only_exec) {
//		ept_pte.ReadAccess = false;
//		ept_pte.WriteAccess = false;
//		ept_pte.ExecuteAccess = true;
//		ept_pte.UserModeExecute = true;
//		ept_pte.PageFrameNumber = ept_hook_entry->shadow_host_pfn;
//	}
//
//	else {
//		ept_pte.ReadAccess = true;
//		ept_pte.WriteAccess = false;
//		ept_pte.ExecuteAccess = false;
//		ept_pte.UserModeExecute = false;
//		ept_pte.PageFrameNumber = ept_hook_entry->visible_host_pfn;
//	}
//	
//	update_ept_entry(ept_hook_entry->ppte, ept_pte.Flags, true, false);
//	set_mtf(false);
//
//	return true;
//}
//
//struct ept_hook_entry 
//{
//	bool active;
//
//	u64 shadow_host_pfn;
//	u64 visible_host_pfn;
//
//	u64 ept_pte_ptr;
//	EPT_PTE ept_pte_template;
//
//	EPT_PTE set_ept_pte_template(EPT_PTE val) {
//		val.pfn = 0;
//		val.read = false;
//		val.write = false;
//		val.exec = false;
//		val.um_exec = false;
//		ept_pte_template = val;
//		return ept_pte_template;
//	}
//};
//
//ept_hook_entry ept_hooks[1];
//
////update ept
//
////split
//noinl bool split_1gb_ept(u64 PPDPTE, EPT_PDPTE_1GB PDPTE)
//{
//	auto page1 = alloc_page();
//	auto Page = (EPT_PDE_2MB*)page1->addr;
//
//	u64 pfn_base1 = (u64)PDPTE.pfn << 9;
//
//	for (size_t i = 0; i < 512; ++i)
//	{
//		auto& EPDE = Page[i];
//
//		EPDE.value = 0;
//
//		EPDE.read = true;
//		EPDE.write = true;
//		EPDE.exec = true;
//		EPDE.um_exec = true;
//
//		EPDE.large_page = true;
//		EPDE.mem_cache_type = PDPTE.mem_cache_type;
//		EPDE.ignore_pat = PDPTE.ignore_pat;
//		EPDE.suppress_ve = PDPTE.suppress_ve;
//
//		EPDE.pfn = pfn_base1 + i;
//	}
//
//	EPT_PDPTE ept_pdpte;
//	ept_pdpte.value = 0;
//	ept_pdpte.read = true;
//	ept_pdpte.write = true;
//	ept_pdpte.exec = true;
//	ept_pdpte.um_exec = true;
//	ept_pdpte.pfn = page1->org_pfn;
//
//	return update_ept_entry(PPDPTE, ept_pdpte.value, true, true);
//}
//
//
//noinl std::pair<bool, QUERY_EPT> split_pa_ept(u64 guest_pa)
//{
//	QUERY_EPT ept_info;
//	if (!gpa2hpa(guest_pa, &ept_info)) {
//		return std::pair(false, ept_info);
//	}
//
//	if (ept_info.page_size == PAGE_4KB) {
//		return std::pair(true, ept_info);
//	}
//
//	//split pages
//	switch (ept_info.page_size)
//	{
//	case PAGE_1GB:
//		fmtp("split 1gb");
//		split_1gb_ept(ept_info.PEPDPTE, ept_info.EPDPTE.l);
//		if (!gpa2hpa(guest_pa, &ept_info)) {
//			fmtp("split 1gb fail!");
//			return std::pair(false, ept_info);
//		}
//		fmtp("split 1gb ok!");
//
//	case PAGE_2MB:
//		fmtp("split 2mb");
//		ept::msplit_2mb_ept(ept_info.PEPDE, ept_info.EPDE.l);
//		if (!gpa2hpa(guest_pa, &ept_info)) {
//			fmtp("split 2mb fail");
//			return std::pair(false, ept_info);
//		} fmtp("split 2mb ok!");
//
//	case PAGE_4KB:
//		if (ept_info.page_size != PAGE_4KB) {
//			//split (FULL) fail!
//			break;
//		}
//		
//		fmtp("page 4kb ok!");
//		return std::pair(true, ept_info);
//		break;
//	}
//
//	fmtp("split (FULL) fail!");
//	return std::pair(false, ept_info);
//}

//install
bool set_ept_hook(u64 g_cr3, u64 hook_va, u64 hook_data, u64 hook_data_sz, bool is_inline_hook)
{
//	fmtp("ept hook! cr3: %p va: %p buff: %p inline: %d",
//		g_cr3,
//		hook_va,
//		hook_data,
//		(u64)is_inline_hook);

//	auto guest_pa = gva2gpa(g_cr3, hook_va);
//	if (!guest_pa) {
//		sp("set_ept_hook: err -> guest_pa == 0!");
//		return false;
//	}

//	auto split_ret = split_pa_ept(guest_pa);
//	if (!split_ret.first) {
//		sp("set_ept_hook: err -> split_pa fail!");
//		return false;
//	}

//	const auto& split_info = split_ret.second;
//	fmtp("set_ept_hook: host pfn -> %p", split_info.EPTE.pfn);
//	
//	//
//	hook_data_sz = 6;
//	auto shadow_page = alloc_page();
//	read_gva_arr(g_cr3, hook_va & 0xFFFFFFFFFFFFF000, shadow_page->addr, 0x1000);
//	read_gva_arr(g_cr3, hook_data, shadow_page->addr + (hook_va & 0xFFF), hook_data_sz);


//	auto& test_hook = ept_hooks[0];
//
//	test_hook.ept_pte_ptr = split_info.PEPTE;
//	auto ept_pte_template = test_hook.set_ept_pte_template(split_info.EPTE);
//
//	test_hook.shadow_host_pfn = shadow_page->org_pfn;
//	test_hook.visible_host_pfn = split_info.EPTE.pfn;
//	test_hook.active = true;
//
//	fmtp("set_ept_hook: gpa shadow -> %p, vis -> %p",
//		shadow_page->org_pfn,
//		split_info.EPTE.pfn);
//
//	//apply hook
//	ept_pte_template.read = false;
//	ept_pte_template.write = false;
//	ept_pte_template.exec = true;
//	ept_pte_template.um_exec = true;
//	ept_pte_template.pfn = test_hook.shadow_host_pfn;
//	update_ept_entry(test_hook.ept_pte_ptr, ept_pte_template.value, true, true);

	//ok!!!
	return true;
}

//handle violation
bool handle_ept(core_data_t& core_ctx)
{
	VMX_EXIT_QUALIFICATION_EPT_VIOLATION exit_info;
	exit_info.Flags = core_ctx.vm_state->slat_fail_desc;
	
	const bool read_fail = exit_info.ReadAccess && !exit_info.EptReadable;
	const bool write_fail = exit_info.WriteAccess && !exit_info.EptWriteable;
	const bool exec_fail = exit_info.ExecuteAccess && !exit_info.EptExecutable;
	
	const u64 gva = exit_info.ValidGuestLinearAddress ? 
		core_ctx.vm_state->slat_fail_gva : 0;
	const u64 gpa = core_ctx.vm_state->slat_fail_gpa;
	const u64 gva_align = gva & 0xFFFFFFFFFFFFF000;
	const u64 gpfn = gpa >> PAGE_SHIFT;

	fmtp("handle ept gva %p gpa %p gpfn %p rip %p",
		gva,
		gpa,
		gpfn,
		(u64)core_ctx.vm_state->rip);

	fmtp("handle ept R:%d W:%d X:%d",
		read_fail,
		write_fail,
		exec_fail);
	
	call_ptr(mapper_ctx_hv.exit_handler_fn, core_ctx.arg1, core_ctx.arg2);
	return true;
}