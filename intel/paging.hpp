template<typename T>
finl bool check_paging_entry_valid(const T& entry) {
	bool r = entry.read;
	bool x = entry.exec;
	bool w = entry.read && entry.write;
	bool is_valid = r || w || x;
	return is_valid;
}

template<typename T>
finl void copy_ept_prot_desc(const T& entry, QUERY_EPT* ept_info) {
	ept_info->r = entry.read;
	ept_info->w = entry.write;
	ept_info->x = entry.exec;
	ept_info->um_x = entry.um_exec;
}

noinl u64 gpa2hpa_intel(
	u64 gpa,
	QUERY_EPT* ept_desc = nullptr,
	u64 ept_ptr_pfn = 0)
{
	//set gpa desc buffer
	QUERY_EPT gpa_desc_buff;
	QUERY_EPT* gpa_desc = ept_desc ? 
		ept_desc : &gpa_desc_buff;
	gpa_desc->status = 0xFFFFFFFF;
	gpa_desc->cache_type = 0xFFFF;
	gpa_desc->page_size = 0;

	//set gpa for translate
	VIRTUAL_ADDRESS GuestPhysical{ gpa };
	ept_ptr_pfn = ept_ptr_pfn ? ept_ptr_pfn : get_ept_ptr().pfn;

	//read pml4e
	auto PhysPML4 = (ept_ptr_pfn << PAGE_SHIFT) + (u64)(8 * GuestPhysical.IndexPML4);
	auto PML4E = read_pa<EPT_PML4E>(PhysPML4);

	//store pml4e
	gpa_desc->pml4e_pa = PhysPML4;
	gpa_desc->pml4e_val.value = PML4E.value;
	copy_ept_prot_desc(PML4E, gpa_desc);

	//check pml4e
	if (!check_paging_entry_valid(PML4E)) {
		gpa_desc->status = 1;
		return 0;
	}
	
	//read pdpte
	auto PhysPDPTE = ((u64)PML4E.pfn << PAGE_SHIFT) + (u64)(8 * GuestPhysical.IndexPDPT);
	auto PDPTE_1GB = read_pa<EPT_PDPTE_1GB>(PhysPDPTE);
	
	//store pdpte
	gpa_desc->pdpte_pa = PhysPDPTE;
	gpa_desc->pdpte_val.page_1gb = PDPTE_1GB;
	copy_ept_prot_desc(PDPTE_1GB, gpa_desc);

	//check pdpte
	if (!check_paging_entry_valid(PDPTE_1GB)) {
		gpa_desc->status = 2;
		return 0;
	}
	
	//check 1gb page
	if (PDPTE_1GB.large_page) 
	{
		//store page info
		gpa_desc->status = 0;
		gpa_desc->page_size = PAGE_1GB;
		gpa_desc->cache_type = PDPTE_1GB.mem_cache_type;

		//1GB Page
		return ((u64)PDPTE_1GB.pfn * PAGE_1GB) + GuestPhysical.Offset1GB;
	}

	//convert pdpte (1gb page) -> pdpte (link to pd)
	EPT_PDPTE PDPTE;
	PDPTE.value = PDPTE_1GB.value;
	copy_ept_prot_desc(PDPTE, gpa_desc);

	//read pde
	auto PhysPDE = ((u64)PDPTE.pfn << PAGE_SHIFT) + (u64)(8 * GuestPhysical.IndexPD);
	auto PDE_2MB = read_pa<EPT_PDE_2MB>(PhysPDE);
	
	//store pde
	gpa_desc->pde_pa = PhysPDE;
	gpa_desc->pde_val.page_2mb = PDE_2MB;
	copy_ept_prot_desc(PDE_2MB, gpa_desc);

	//check pde
	if (!check_paging_entry_valid(PDE_2MB)) {
		gpa_desc->status = 3;
		return 0;
	}

	//check 2mb page
	if (PDE_2MB.large_page) 
	{
		//store page info
		gpa_desc->status = 0;
		gpa_desc->page_size = PAGE_2MB;
		gpa_desc->cache_type = PDE_2MB.mem_cache_type;
		
		//2MB Page
		return ((u64)PDE_2MB.pfn * PAGE_2MB) + GuestPhysical.Offset2MB;
	}
	
	//convert pde (2mb page) -> pde (link to pt)
	EPT_PDE PDE;
	PDE.value = PDE_2MB.value;
	copy_ept_prot_desc(PDE, gpa_desc);

	//read pte
	auto PhysPTE = ((u64)PDE.pfn << PAGE_SHIFT) + (u64)(8 * GuestPhysical.IndexPT);
	auto PTE = read_pa<EPT_PTE>(PhysPTE);
	
	//store pte
	gpa_desc->pte_pa = PhysPTE;
	gpa_desc->pte_val = PTE;
	copy_ept_prot_desc(PTE, gpa_desc);

	//check pte
	if (!check_paging_entry_valid(PTE)) {
		gpa_desc->status = 4;
		return 0;
	}
	
	//store page info
	gpa_desc->status = 0;
	gpa_desc->page_size = PAGE_4KB;
	gpa_desc->cache_type = PTE.mem_cache_type;

	//4KB Page
	return ((u64)PTE.pfn * PAGE_4KB) + GuestPhysical.Offset4KB;
}

inl void update_ept(bool global = false) {
	const u64 cond = global ? INVEPT_ALL_CONTEXT : INVEPT_SINGLE_CONTEXT;
	const u64 ept_ptr = global ? 0 : _vmx_vmread(VMCS_CTRL_EPT_POINTER);
	_invept(cond, ept_ptr);
}