#include <utils/shared/assembler.hpp>
#include <stl/array.hpp>
#include "mtrr.hpp"

//
namespace ept_mgr
{
	//constexpr u64 max_map_gb = 512;
	//u64 fake_ff_pfn = 0xFFFFFFFFFFFFFFFF;
	fstaticvector<u64, 128> hyperv_ept_dirs;
	//constexpr u64 max_map_gb_addr = max_map_gb * PAGE_1GB;
	
	//base
	noinl bool update_ept_entry(u64 ept_entry_ptr, u64 ept_data, bool update = false, bool global = false)
	{
		fspinlock_auto();

		bool ret = write_pa(ept_entry_ptr, ept_data);

		if (update) {
			//if (global) {
			//	for (auto& i : core_data_arr) {
			//		i.need_flush_ept = true;
			//	}
			//}
			update_ept(global);
		}

		return ret;
	}

	__attribute__((clang_func_no_sse)) std::pair<u64, u64> translate_hyperv(u64 pfn, u64 v4 = 0)
	{
		static u64 mem_regions_list = 0;
		static u64 mem_regions_list1 = 0;

		if (!mem_regions_list) 
		{
			sp("2131 __ 1");

			mem_regions_list = *(u64*)RVA(
				FindPatternConstexprS1(mapper_ctx_hv.hv_base_mod.first, ".text",
					"48 8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 48 8B"), 7);
			
			mem_regions_list1 = *(u32*)(
				FindPatternConstexprS1(mapper_ctx_hv.hv_base_mod.first, ".text",
					"48 8B 05 ? ? ? ? 48 8B 8C C8") + 11);

			hp(_readcr3());
			hp(mem_regions_list);
			hp(mem_regions_list1);


			//
			//inf_loop();

			if (!mem_regions_list || !mem_regions_list1) {
				fast_kill(__LINE__);
			}

		}

		const u64 v1 = *(u64*)(/*0x20 * v4 +*/ mem_regions_list + mem_regions_list1);
		if (!v1) {
			fast_kill(__LINE__);
		}

		constexpr u64 phys_filter_mask = 0xFFFFFFFFFF000;
		const u64 data1 = *(u64*)(v1 + (pfn * 8));
		const u64 ret__ = (data1 & phys_filter_mask) >> PAGE_SHIFT;
		return std::pair(data1, ret__);
	}

	//split
	noinl bool split_2mb_ept(u64 PEPDE, EPT_PDE_2MB EPDE)
	{
		u64 pfn_base1 = (u64)EPDE.pfn << 9;

		auto ept_pt_buff = alloc_page();
		auto ept_pt = (EPT_PTE*)ept_pt_buff->addr;

		for (int i = 0; i < 512; ++i)
		{
			auto& EPTE = ept_pt[i];

			EPTE.value = 0;

			EPTE.read = true;
			EPTE.write = true;
			EPTE.exec = true;
			EPTE.um_exec = true;

			EPTE.mem_cache_type = EPDE.mem_cache_type;
			EPTE.ignore_pat = EPDE.ignore_pat;
			EPTE.suppress_ve = EPDE.suppress_ve;

			EPTE.pfn = pfn_base1 + i;
		}

		EPT_PDE ept_pde;
		ept_pde.value = 0;
		ept_pde.read = true;
		ept_pde.write = true;
		ept_pde.exec = true;
		ept_pde.um_exec = true;
		ept_pde.pfn = ept_pt_buff->org_pfn;

		return update_ept_entry(PEPDE, ept_pde.value, true, true);
	}

	noinl bool split_1gb_ept(u64 PEPDPTE, EPT_PDPTE_1GB EPDPTE)
	{
		auto page1 = alloc_page();
		auto Page = (EPT_PDE_2MB*)page1->addr;
		
		for (int i = 0; i < 512; ++i)
		{
			auto& EPDE = Page[i];
		
			EPDE.value = 0;
		
			EPDE.read = true;
			EPDE.write = true;
			EPDE.exec = true;
			EPDE.um_exec = true;
		
			EPDE.large_page = true;
			EPDE.mem_cache_type = EPDPTE.mem_cache_type;
			EPDE.ignore_pat = EPDPTE.ignore_pat;
			EPDE.suppress_ve = EPDPTE.suppress_ve;
		
			EPDE.pfn = ((u64)EPDPTE.pfn << 9) + i;
		}
		
		EPT_PDPTE ept_pdpte;
		ept_pdpte.value = 0;
		ept_pdpte.read = true;
		ept_pdpte.write = true;
		ept_pdpte.exec = true;
		ept_pdpte.um_exec = true;
		ept_pdpte.pfn = page1->org_pfn;

		return update_ept_entry(PEPDPTE, ept_pdpte.value, true, true);
	}

	noinl bool split_ept(u64 gpa, QUERY_EPT* ept_info = nullptr, u64 ept_dir_pfn = 0)
	{
		new_try_:

		QUERY_EPT ept_info_;
		if (!gpa2hpa_intel(gpa, &ept_info_, ept_dir_pfn)) {
			return false;
		}

		if (ept_info_.page_size == PAGE_4KB) {
			*ept_info = ept_info_;
			return true;
		}

		if (ept_info_.page_size == PAGE_1GB) {
			if (split_1gb_ept(ept_info_.pdpte_pa, ept_info_.pdpte_val.page_1gb)) {
				goto new_try_;
			}
		}

		if (ept_info_.page_size == PAGE_2MB) {
			if (split_2mb_ept(ept_info_.pde_pa, ept_info_.pde_val.page_2mb)) {
				goto new_try_;
			}
		}
	
		return false;
	}
	
	noinl bool update_gpa(u64 gpa, u64 gpfn, bool r, bool w, bool x)
	{
		//get gpa desc
		QUERY_EPT ept_info_;
		u64 hpa1 = gpa2hpa_intel(gpa, &ept_info_);
		if (!hpa1) {
			return false;
		}

		u64 hpfn = hpa1 >> PAGE_SHIFT;
		if (gpfn == hpfn) {
			//TODO: implement protection
			return true;
		}

		QUERY_EPT ept_info;
		if (!split_ept(gpa, &ept_info)) {
			return false;
		}

		ept_info.pte_val.read = r;
		ept_info.pte_val.write = w;
		ept_info.pte_val.um_exec =
		ept_info.pte_val.exec = x;
		ept_info.pte_val.pfn = gpfn;

		return update_ept_entry(ept_info.pte_pa, ept_info.pte_val.value, true, true);
	}

	//init stage 1
	finl u64 eptp_fake_pdpte_template(u64 max_1gb_pfn = 512/*GB*/) 
	{
		EPT_PDPTE_1GB __;
		__.value = 0;
		__.read = true;
		__.write = true;
		__.exec = true;
		__.um_exec = true;
		__.large_page = true;
		__.mem_cache_type = 6;
		__.pfn = max_1gb_pfn;
		return __.value;
	}

	__attribute__((clang_func_no_sse)) noinl u64 alloc_ept_dirs()
	{
		static u64 eptp_dir_pfn = 0;
		if (eptp_dir_pfn) {
			return eptp_dir_pfn;
		}

		//alloc fake ept pdpt
		//auto ept_pdpt_fake_buff = alloc_page();
		//const u64 fake_1gb_templ = eptp_fake_pdpte_template();
		//auto ept_pdpt_fake = (EPT_PDPTE_1GB*)ept_pdpt_fake_buff->addr;
		//__memset64(&ept_pdpt_fake[0], fake_1gb_templ, 512);

		//alloc ept pdpt
		auto eptp_pdpt_buff = alloc_page();
		auto eptp_pdpt = (EPT_PDPTE*)eptp_pdpt_buff->addr;
		//__memset64(&eptp_pdpt[max_map_gb], fake_1gb_templ, 512 - max_map_gb);

		//alloc ept pd
		
		//for (int i = 0; i < max_map_gb; ++i) {
		//	eptp_pd_buff[i] = alloc_page();
		//	eptp_pd[i] = (EPT_PDE_2MB*)eptp_pd_buff[i]->addr;
		//}

		//alloc ept pml4e
		auto eptp_pml4_buff = alloc_page();
		auto eptp_pml4 = (EPT_PML4E*)eptp_pml4_buff->addr;
		//for (int i = 1; i < 512; ++i) {
		//	auto& ept_pml4 = eptp_pml4[i];
		//	ept_pml4.read = 
		//	ept_pml4.write = 
		//	ept_pml4.exec = 
		//	ept_pml4.um_exec = true;
		//	ept_pml4.pfn = ept_pdpt_fake_buff->org_pfn;
		//}

		//map first (512gb) ept pdpt
		eptp_pml4[0].read =
		eptp_pml4[0].write =
		eptp_pml4[0].exec =
		eptp_pml4[0].um_exec = true;
		eptp_pml4[0].pfn = eptp_pdpt_buff->org_pfn;

		//TODO: maybe fixed! (postfix)
		//TODO: more accurate map LOW PHYS (static mtrr)
		//const auto mtrrs = read_mtrr_data();

		//process guest phys
		for (u64 a = 0; a < 512; ++a)
		{
			//alloc ept pde (2mb)
			auto eptp_pd_buff = alloc_page();
			auto eptp_pd = (EPT_PDE_2MB*)eptp_pd_buff->addr;

			//link ept pdpte to ept pde
			auto& pdpte = eptp_pdpt[a];
			pdpte.read =
			pdpte.write =
			pdpte.exec =
			pdpte.um_exec = true;
			pdpte.pfn = eptp_pd_buff->org_pfn;

			for (u64 b = 0; b < 512; ++b)
			{
				auto& pde = eptp_pd[b];

				pde.read = true;
				pde.write = true;
				pde.exec = true;
				pde.large_page = true;
				pde.um_exec = true;
				pde.suppress_ve = true;
				pde.ignore_pat = false;

				//map 1 - 1 (GPA <-> HPA)
				pde.pfn = (a << 9) + b;

				//TODO: implement static mtrr, variable mtrr, pat, smrr
				//calc_mtrr_mem_type(mtrrs, (u64)pde.pfn << 21, PAGE_2MB);
				pde.mem_cache_type = 6;
			}
		}

		//initial build ept ok!
		eptp_dir_pfn = eptp_pml4_buff->org_pfn;
		return eptp_dir_pfn;
	}

	//init stage 2
	noinl void process_org_mappings()
	{
		return;

		//alloc fake ff page
		//const auto fake_ff_page_buff = alloc_page();
		//__memset(fake_ff_page_buff->addr, 0xFF, PAGE_4KB);
		//fake_ff_pfn = fake_ff_page_buff->org_pfn;

		//detect hyperv stub pfn
		const u64 stack_pfn = IsAddrValid(&stack_pfn)->pfn;
		const u64 hyperv_stub_pfn = translate_hyperv(stack_pfn).second;
	
		//process ept orig hyperv mappings
		for (u64 i = 0; i < (PAGE_1GB_PFN * 512); ++i)
		{
			u64 gpa__ = i << PAGE_SHIFT;

			auto ret1 = translate_hyperv(i);
			if (ret1.first)
			{
				//zero pfn
				if (!ret1.second) {
					fmtp("zero pfn (maybe apic): %p", gpa__);
					update_gpa(gpa__, 0, true, true, true);
					continue;
				}

				//need set dummy pfn
				else if (ret1.second == hyperv_stub_pfn)
				{
					bool set_dummy = true;
					for (auto& ept_dir_pfn1 : ept_mgr::hyperv_ept_dirs)
					{
						QUERY_EPT inf1;
						const u64 trans_hpa__ = gpa2hpa_intel(gpa__, &inf1, ept_dir_pfn1);
						const u64 trans_hpfn__ = trans_hpa__ >> PAGE_SHIFT;

						if (trans_hpa__ && (trans_hpfn__ != hyperv_stub_pfn)) {
							set_dummy = false;
							break;
						}
					}

					if (set_dummy) {
						//TODO: 
						fmtp("stub pfn (maybe hv mem): %p", gpa__);
						update_gpa(gpa__, hyperv_stub_pfn, true, false, false);
					}

					else {
						//TODO: 
						fmtp("refix org pfn (maybe miss): %p", gpa__);
						update_gpa(gpa__, gpa__ >> PAGE_SHIFT, true, true, true);
					}
				}

				//need restore org pfn
				else if (ret1.second != gpa__) {
					//TODO: 
					update_gpa(gpa__, gpa__ >> PAGE_SHIFT, true, true, true);
				}
			}

			//TODO: 
			//else if (gpa2hpa_intel(gpa__) != max_map_gb_addr) {
			//	fmtp("set new dummy pfn: %p  %p", gpa__, gpa2hpa_intel(gpa__));
			//	set_ept_entry(gpa__, true, true, true, max_map_gb_addr >> PAGE_SHIFT);
			//}
		}

		sp("process_org_mappings() OK!");
	}
}

noinl void handle_ept_ptr_internal(EPT_POINTER* new_ept_ptr)
{
	com_init();
	init_memmgr();
	
	//
	auto new_ept_val = *new_ept_ptr;
	u64 new_ept_pml4_pfn = ept_mgr::alloc_ept_dirs();
	
	//scan duplicate
	u64 org_ept_pml4_pfn = new_ept_val.pfn;
	for (auto& ept_pml4_entry : ept_mgr::hyperv_ept_dirs) {
		if (ept_pml4_entry == org_ept_pml4_pfn) {
			org_ept_pml4_pfn = 0;
			break;
		}
	}
	
	//push new org ept dir
	if (org_ept_pml4_pfn) {
		ept_mgr::hyperv_ept_dirs.push_back(org_ept_pml4_pfn);
	}

	//apply spoofed new ept dir
	new_ept_val.pfn = new_ept_pml4_pfn;
	set_ept_ptr(new_ept_val);

	//detect hyperv stub pfn
	const u64 stack_pfn = IsAddrValid(&stack_pfn)->pfn;
	const u64 hyperv_stub_pfn = ept_mgr::translate_hyperv(stack_pfn).first;


	//hp(&stack_pfn);
	//hp(hyperv_stub_pfn);

	//fmtp("new ept ptr! %d %p %p %p",
	//	get_current_core_id(), 
	//	(u64)new_ept_val.pfn,
	//	new_ept_pml4_pfn, 
	//	gg.first);
}

extern "C" 
{
	__declspec(dllexport) naked void handle_ept_ptr(EPT_POINTER* ept_ptr_ptr) {
		__asm 
		{
			PUSHA

			sub rsp, 0x28
			call handle_ept_ptr_internal
			add rsp, 0x28

			POPA

			ret
		}
	}
}