//guest physical
inl u64 gpa2hpa(u64 guest_phys_addr, pv np_info = nullptr)
{
	if (!np_info) {
		return guest_phys_addr;
	}

	if (mapper_ctx_hv.type == hv_amd_vendor) {
		return gpa2hpa_amd(guest_phys_addr, (QUERY_NPT*)np_info);
	}

	return gpa2hpa_intel(guest_phys_addr, (QUERY_EPT*)np_info);
}

noinl bool read_gpa_arr(u64 guest_phys_addr, pv buff, u32 size)
{
	u64 offset = 0;
	while (size)
	{
		struct pagingShared { u64 page_size; u8 pad[0x50]; } page_info;
		u64 host_phys = gpa2hpa(guest_phys_addr + offset, &page_info);
		if (!host_phys) {
			return false;
		}

		u64 page_sz = page_info.page_size; u64 page_sz_off = page_sz - 1;
		u32 block_sz = min(page_sz - (host_phys & page_sz_off), size);
		if (!read_pa_arr(host_phys, (pb)buff + offset, block_sz)) {
			return false;
		}

		size -= block_sz;
		offset += block_sz;
	}

	return true;
}

noinl bool write_gpa_arr(u64 guest_phys_addr, pv buff, u32 size)
{
	u64 offset = 0;
	while (size)
	{
		struct pagingShared{ u64 page_size; u8 pad[0x50]; } page_info;
		u64 host_phys = gpa2hpa(guest_phys_addr + offset, &page_info);
		if (!host_phys) {
			return false;
		}

		u64 page_sz = page_info.page_size; u64 page_sz_off = page_sz - 1;
		u32 block_sz = min(page_sz - (host_phys & page_sz_off), size);
		if (!write_pa_arr(host_phys, (pb)buff + offset, block_sz)) {
			return false;
		}

		size -= block_sz;
		offset += block_sz;
	}

	return true;
}

template <typename T, typename A>
inl T read_gpa(A PhysAddress, bool* state = nullptr)
{
	T Data{};
	bool ret = read_gpa_arr((u64)PhysAddress, &Data, sizeof(T));
	if (state) { *state = ret; }
	if (ret) {
		return Data;
	}
	return T{};
}

template <typename T, typename A>
inl bool write_gpa(T PhysAddress, const A& Data) {
	return write_guest_pa_arr((u64)PhysAddress, (pv)&Data, sizeof(A));
}

//guest virtual
noinl u64 gva2gpa(u64 dirbase, u64 addr, QUERY_PTE* PagingData = nullptr)
{
	PTE CR3{ dirbase & ~0xf };
	VIRTUAL_ADDRESS VirtualAddress{ addr };

	//check bullshit!
	if (!dirbase || !addr) {
		return 0;
	}

	//query pml4e
	auto phys_pml4e = CR3.to_phys() + (u64)(8 * VirtualAddress.IndexPML4);
	auto pml4e = read_gpa<PTE>(phys_pml4e);
	if (!pml4e.present || !pml4e.pfn) {
		return 0;
	}

	//save pml4e
	if (PagingData) {
		PagingData->pPML4E = phys_pml4e;
		PagingData->PML4E.value = pml4e.value;
	}

	//query pdpte
	auto phys_pdpte = pml4e.to_phys() + (u64)(8 * VirtualAddress.IndexPDPT);
	auto pdpte = read_gpa<PTE>(phys_pdpte);
	if (!pdpte.present || !pdpte.pfn) {
		return 0;
	}

	//save pdpte
	if (PagingData) {
		PagingData->pPDPTE = phys_pdpte;
		PagingData->PDPTE.value = pdpte.value;
	}

	//is 1gb page
	if (pdpte.large_page)
	{
		if (PagingData) {
			PagingData->page_size = PAGE_1GB;
		}

		return pdpte.to_phys() + VirtualAddress.Offset1GB;
	}

	//query pde
	auto phys_pde = pdpte.to_phys() + (u64)(8 * VirtualAddress.IndexPD);
	auto pde = read_gpa<PTE>(phys_pde);
	if (!pde.present || !pde.pfn) {
		return 0;
	}

	//save pde
	if (PagingData) {
		PagingData->pPDE = phys_pde;
		PagingData->PDE.value = pde.value;
	}

	//is 2mb page
	if (pde.large_page)
	{
		if (PagingData) {
			PagingData->page_size = PAGE_2MB;
		}

		return pde.to_phys() + VirtualAddress.Offset2MB;
	}

	//query pte
	auto phys_pte = pde.to_phys() + (u64)(8 * VirtualAddress.IndexPT);
	auto pte = read_gpa<PTE>(phys_pte);
	if (!pte.present || !pte.pfn) {
		return 0;
	}

	//save pte
	if (PagingData) {
		PagingData->pPTE = phys_pte;
		PagingData->PTE.value = pte.value;
		PagingData->page_size = PAGE_4KB;
	}

	//is 4kb page
	return pte.to_phys() + VirtualAddress.Offset4KB;
}

noinl u64 gva2gpa2(u64 dirbase, u64 addr, QUERY_PTE* PagingData = nullptr)
{
	PTE CR3{ dirbase & ~0xf };
	VIRTUAL_ADDRESS VirtualAddress{ addr };

	//check bullshit!
	if (!dirbase || !addr) {
		return 0;
	}

	//query pml4e
	auto phys_pml4e = CR3.to_phys() + (u64)(8 * VirtualAddress.IndexPML4);
	auto pml4e = read_pa<PTE>(phys_pml4e);
	if (!pml4e.present || !pml4e.pfn) {
		return 0;
	}

	//save pml4e
	if (PagingData) {
		PagingData->pPML4E = phys_pml4e;
		PagingData->PML4E.value = pml4e.value;
	}

	//query pdpte
	auto phys_pdpte = pml4e.to_phys() + (u64)(8 * VirtualAddress.IndexPDPT);
	auto pdpte = read_pa<PTE>(phys_pdpte);
	if (!pdpte.present || !pdpte.pfn) {
		return 0;
	}

	//save pdpte
	if (PagingData) {
		PagingData->pPDPTE = phys_pdpte;
		PagingData->PDPTE.value = pdpte.value;
	}

	//is 1gb page
	if (pdpte.large_page)
	{
		if (PagingData) {
			PagingData->page_size = PAGE_1GB;
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
	if (PagingData) {
		PagingData->pPDE = phys_pde;
		PagingData->PDE.value = pde.value;
	}

	//is 2mb page
	if (pde.large_page)
	{
		if (PagingData) {
			PagingData->page_size = PAGE_2MB;
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
	if (PagingData) {
		PagingData->pPTE = phys_pte;
		PagingData->PTE.value = pte.value;
		PagingData->page_size = PAGE_4KB;
	}

	//is 4kb page
	return pte.to_phys() + VirtualAddress.Offset4KB;
}


inl u64 gva2hpa(u64 cr3, u64 va, bool ultrafast = false, pv np_info = nullptr)
{
	auto guest_physical = gva2gpa(cr3, va);
	if (!guest_physical) {
		return 0;
	}

	if (ultrafast) {
		return guest_physical;
	}

	return gpa2hpa((u64)guest_physical, np_info);
}

//
noinl bool read_gva_arr(u64 cr3, u64 addr, pv buff, u32 size, bool ultrafast = true)
{
	u64 offset = 0;
	while (size)
	{
		auto host_phys = gva2hpa(cr3, addr + offset, ultrafast);
		if (!host_phys) {
			return false;
		}

		//TODO: FIX SIZE
		u32 block_size = min(0x1000 - (host_phys & 0xFFF), size);
		if (!read_pa_arr(host_phys, (pb)buff + offset, block_size)) {
			return false;
		}

		size -= block_size;
		offset += block_size;
	}

	return true;
}

noinl bool write_gva_arr(u64 cr3, u64 addr, pv buff, u32 size, bool ultrafast = true)
{
	u64 offset = 0;
	while (size)
	{
		auto host_phys = gva2hpa(cr3, addr + offset, ultrafast);
		if (!host_phys) {
			return false;
		}

		//TODO: FIX SIZE
		u32 block_size = min(0x1000 - (host_phys & 0xFFF), size);
		if (!write_pa_arr(host_phys, (pb)buff + offset, block_size)) {
			return false;
		}

		size -= block_size;
		offset += block_size;
	}

	return true;
}

//
template <typename T, typename A>
inl T read_gva(u64 DirBase, A VirtAddress, bool ultrafast = true, bool* state = nullptr)
{
	T Data{};
	bool ret = read_gva_arr(DirBase, (u64)VirtAddress, &Data, sizeof(T), ultrafast);
	if (state) { *state = ret; }
	if (ret) {
		return Data;
	}
	return T{};
}

template <typename T, typename A>
inl bool write_gva(u64 DirBase, T VirtAddress, const A& Data, bool ultrafast = true) {
	return write_gva_arr(DirBase, (u64)VirtAddress, (pv)&Data, sizeof(A), ultrafast);
}