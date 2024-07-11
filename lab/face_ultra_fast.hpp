namespace uf
{
	#define read_pa_unsafe(read_t, phys_address) *(read_t*)(mapped_host_phys + phys_address)
	
	finl u64 gva2pa(u64 dirbase, u64 addr)
	{
		PTE CR3{ dirbase & ~0xf };
		VIRTUAL_ADDRESS VirtualAddress{ addr };

		//check bullshit!
		if (addr < 0x100000/*1mb*/) {
			return 0;
		}

		//query pml4e
		auto phys_pml4e = CR3.to_phys() + (u64)(8 * VirtualAddress.IndexPML4);
		auto pml4e = read_pa_unsafe(PTE, phys_pml4e);
		if (!pml4e.present || !pml4e.pfn) {
			return 0;
		}

		//query pdpte
		auto phys_pdpte = pml4e.to_phys() + (u64)(8 * VirtualAddress.IndexPDPT);
		auto pdpte = read_pa_unsafe(PTE, phys_pdpte);
		if (!pdpte.present || !pdpte.pfn) {
			return 0;
		}

		//is 1gb page
		//if (pdpte.large_page) {
		//	return pdpte.to_phys() + VirtualAddress.Offset1GB;
		//}

		//query pde
		auto phys_pde = pdpte.to_phys() + (u64)(8 * VirtualAddress.IndexPD);
		auto pde = read_pa_unsafe(PTE, phys_pde);
		if (!pde.present || !pde.pfn) {
			return 0;
		}

		//is 2mb page
		if (pde.large_page) {
			return pde.to_phys() + VirtualAddress.Offset2MB;
		}

		//query pte
		auto phys_pte = pde.to_phys() + (u64)(8 * VirtualAddress.IndexPT);
		auto pte = read_pa_unsafe(PTE, phys_pte);
		if (!pte.present || !pte.pfn) {
			return 0;
		}

		//is 4kb page
		return pte.to_phys() + VirtualAddress.Offset4KB;
	}
	
	#undef read_pa_unsafe

	//
	noinl bool read_gva_arr(u64 dirbase, u64 addr, pv buff, u32 size)
	{
		u64 offset = 0;
		while (size)
		{
			auto host_phys = gva2pa(dirbase, addr + offset);
			if (!host_phys) {
				return false;
			}

			//TODO: FIX SIZE
			u32 block_size = min(PAGE_4KB - (host_phys & 0xFFF), size);
			if (!read_pa_arr(host_phys, (pb)buff + offset, block_size)) {
				return false;
			}

			size -= block_size;
			offset += block_size;
		}

		return true;
	}

	noinl bool write_gva_arr(u64 dirbase, u64 addr, pv buff, u32 size)
	{
		u64 offset = 0;
		while (size)
		{
			auto host_phys = gva2pa(dirbase, addr + offset);
			if (!host_phys) {
				return false;
			}

			//TODO: FIX SIZE
			u32 block_size = min(PAGE_4KB - (host_phys & 0xFFF), size);
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
	finl T read_gva(u64 dirbase, A addr)
	{
		T Data{};
		if (read_gva_arr(dirbase, (u64)addr, &Data, sizeof(T))) {
			return Data;
		}
		return T{};
	}

	template <typename T, typename A>
	finl bool write_gva(u64 dirbase, A addr, const T& Data) {
		return write_gva_arr(dirbase, (u64)addr, (pv)&Data, sizeof(T));
	}
}