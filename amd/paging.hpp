noinl u64 gpa2hpa_amd(
	u64 PhyicalAddress, 
	QUERY_NPT* NptInfo = nullptr,
	u64 npt_ptr_pfn = 0)
{
	npt_ptr_pfn = 
		(npt_ptr_pfn) ?
		(npt_ptr_pfn << PAGE_SHIFT) :
		(get_vmcb()->ncr3 & ~0xf);

	PTE CR3{ npt_ptr_pfn };
	VIRTUAL_ADDRESS VirtualAddress{ PhyicalAddress };
	
	//query pml4e
	auto phys_pml4e = CR3.to_phys() + (u64)(8 * VirtualAddress.IndexPML4);
	auto pml4e = read_pa<PTE>(phys_pml4e);
	if (!pml4e.present || !pml4e.pfn) {
		return 0;
	}

	//save pml4e
	if (NptInfo) {
		NptInfo->PEPML4E = phys_pml4e;
		NptInfo->EPML4E.value = pml4e.value;
	}

	//query pdpte
	auto phys_pdpte = pml4e.to_phys() + (u64)(8 * VirtualAddress.IndexPDPT);
	auto pdpte = read_pa<PTE>(phys_pdpte);
	if (!pdpte.present || !pdpte.pfn) {
		return 0;
	}

	//save pdpte
	if (NptInfo) {
		NptInfo->PEPDPTE = phys_pdpte;
		NptInfo->EPDPTE.value = pdpte.value;
	}

	//is 1gb page
	if (pdpte.large_page)
	{
		if (NptInfo) {
			NptInfo->page_size = PAGE_1GB;
		}

		return pdpte.to_phys() + VirtualAddress.Offset1GB;
	}

	//query pde
	auto phys_pde = pdpte.to_phys() + (u64)(8 * VirtualAddress.IndexPD);
	auto pde = read_pa<PTE>(phys_pde);
	if (!pde.present || !pde.pfn) {
		return 0;
	}

	//save pde
	if (NptInfo) {
		NptInfo->PEPDE = phys_pde;
		NptInfo->EPDE.value = pde.value;
	}

	//is 2mb page
	if (pde.large_page)
	{
		if (NptInfo) {
			NptInfo->page_size = PAGE_2MB;
		}

		return pde.to_phys() + VirtualAddress.Offset2MB;
	}

	//query pte
	auto phys_pte = pde.to_phys() + (u64)(8 * VirtualAddress.IndexPT);
	auto pte = read_pa<PTE>(phys_pte);
	if (!pte.present || !pte.pfn) {
		return 0;
	}

	//save pte
	if (NptInfo) {
		NptInfo->PEPTE = phys_pte;
		NptInfo->EPTE.value = pte.value;
		NptInfo->page_size = PAGE_4KB;
	}

	//is 4kb page
	return pte.to_phys() + VirtualAddress.Offset4KB;
}

inl void update_npt(u64 ncr3 = 0) {
	auto vmcb1 = get_vmcb();
	if (ncr3) {
		vmcb1->ncr3 = ncr3;
	}
	vmcb1->tlbcontrol = 1;
	vmcb1->vmcbclean.reset(NP_VMCLEAN_BIT);
}
