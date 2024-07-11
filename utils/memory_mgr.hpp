#include <crt/linked_list.hpp>


//pages vars
constexpr int pages_cnt = 1200;
constexpr int reserv_desc = 0x20;

//page desc struct
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-private-field"
class alloc_page_desc
{
private:
	double_llist link;
	u8 reserved[reserv_desc];

public:
    pb addr;
	PTE* pte;
	u64 org_pfn;
	
	template<typename T>
	finl T* get_reserved() {
		return (T*)&reserved[0];
	}

	void restore_page(bool update_tlb, bool cleanup) 
	{
		//restore pfn
		if ((u64)pte->pfn != org_pfn) {
			pte->pfn = org_pfn;
			update_tlb = true;
		}

		//flush tlb
		if (update_tlb) {
			_invlpg(pte);
			_invlpg(addr);
		}

		//cleanup page
		if (cleanup) {
			__memzero(addr, PAGE_4KB);
			__memzero(&reserved[0], reserv_desc);
		}
	}
};
#pragma clang diagnostic pop

//pages data
#pragma clang section bss=".static_pages"
fspinlock_static pages_spinlock;
alloc_page_desc pages_desc_map[pages_cnt];
alignas(PAGE_4KB) u8 pages_data[pages_cnt * PAGE_4KB];
double_llist_head<alloc_page_desc> free_pages_list;
#pragma clang section bss=""

//init
noinl void init_memmgr()
{
	//once call
	static bool init_memmgr_once = false;
	if (init_memmgr_once) { return; }
	init_memmgr_once = true;

	//init paging pointers
	InitPaging(PML4_HYPERV_SELFMAP_IDX);

	//init dlinked list
	free_pages_list.init();

	//loop pages
	for (int i = 0; i < pages_cnt; ++i)
	{
		//get page info
		auto ptr = &pages_data[i * PAGE_4KB];
		auto ptr_pte = MiGetPteAddress(ptr);

		//set page desc
		pages_desc_map[i].addr = ptr;
		pages_desc_map[i].pte = ptr_pte;
		pages_desc_map[i].org_pfn = (u64)ptr_pte->pfn;
		pages_desc_map[i].restore_page(true, true);

		//add page to free list
		free_pages_list.add_tail(&pages_desc_map[i]);
	}

	sp("init_memmgr end!");
}

//alloc page
noinl alloc_page_desc* alloc_page(bool clean_page = true, bool update_tlb = true)
{
	//lock pages list
	pages_spinlock.lock();
	
	//no pages quota
	if (free_pages_list.is_empty()) {
		pages_spinlock.unlock();
		sp("ALLOC_PAGE: no pages!");
		__fastfail(1);
		return nullptr;
	}

	//pop back free page 
	auto page_entry = free_pages_list.remove_head();

	//unlock pages list
	pages_spinlock.unlock();

	//prep page
	page_entry->restore_page(update_tlb, clean_page);

	//ok :))
	return page_entry;
}

//free page
noinl void free_page(alloc_page_desc* page_desc)
{
	//base cleanup
	page_desc->restore_page(false, false);

	//lock pages list
	pages_spinlock.lock();

	//push back free page 
	free_pages_list.add_head(page_desc);

	//unlock pages list
	pages_spinlock.unlock();
}
