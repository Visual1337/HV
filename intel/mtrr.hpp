//mtrr
struct mtrr_data
{
	ia32_mtrr_capabilities_register cap;
	ia32_mtrr_def_type_register def_type;

	//fixed mtrr
	struct {
		//TODO:
	} fixed;

	struct {
		ia32_mtrr_physbase_register base;
		ia32_mtrr_physmask_register mask;
	} variable[64];

	int var_count;
};

mtrr_data read_mtrr_data()
{
	mtrr_data mtrrs;

	mtrrs.cap.flags = __readmsr(IA32_MTRR_CAPABILITIES);
	mtrrs.def_type.flags = __readmsr(IA32_MTRR_DEF_TYPE);
	mtrrs.var_count = 0;

	for (uint32_t i = 0; i < mtrrs.cap.variable_range_count; ++i) 
	{
		ia32_mtrr_physmask_register mask;
		mask.flags = __readmsr(IA32_MTRR_PHYSMASK0 + i * 2);

		if (!mask.valid) {
			continue;
		}
		
		mtrrs.variable[mtrrs.var_count].mask = mask;
		mtrrs.variable[mtrrs.var_count].base.flags =
			__readmsr(IA32_MTRR_PHYSBASE0 + i * 2);

		++mtrrs.var_count;
	}

	return mtrrs;
}

uint8_t calc_mtrr_mem_type(mtrr_data const& mtrrs, u64 pfn) 
{
	if (!mtrrs.def_type.mtrr_enable)
		return MEMORY_TYPE_UNCACHEABLE;

	// fixed range MTRRs
	if (pfn < 0x100 && mtrrs.cap.fixed_range_supported
		&& mtrrs.def_type.fixed_range_mtrr_enable) {
		// TODO: implement this
		return MEMORY_TYPE_UNCACHEABLE;
	}

	uint8_t curr_mem_type = MEMORY_TYPE_INVALID;

	// variable-range MTRRs
	for (int i = 0; i < mtrrs.var_count; ++i) 
	{
		auto const base = mtrrs.variable[i].base.page_frame_number;
		auto const mask = mtrrs.variable[i].mask.page_frame_number;

		// 3.11.11.2.3
		// essentially checking if the top part of the address (as specified
		// by the PHYSMASK) is equal to the top part of the PHYSBASE.
		if ((pfn & mask) == (base & mask))
		{
			auto const type = static_cast<uint8_t>(mtrrs.variable[i].base.type);

			// UC takes precedence over everything
			if (type == MEMORY_TYPE_UNCACHEABLE)
				return MEMORY_TYPE_UNCACHEABLE;

			// this works for WT and WB, which is the only other "defined" overlap scenario
			if (type < curr_mem_type)
				curr_mem_type = type;
		}
	}

	// no MTRR covers the specified address
	if (curr_mem_type == MEMORY_TYPE_INVALID)
		return mtrrs.def_type.default_memory_type;

	return curr_mem_type;
}

uint8_t calc_mtrr_mem_type(mtrr_data const& mtrrs, u64 address, u64 size)
{
	address &= ~0xFFFull;
	size = (size + 0xFFF) & ~0xFFFull;

	u8 curr_mem_type = MEMORY_TYPE_INVALID;
	for (u64 curr = address; curr < address + size; curr += PAGE_4KB)
	{
		auto const type = calc_mtrr_mem_type(mtrrs, curr >> PAGE_SHIFT);
		if (type == MEMORY_TYPE_UNCACHEABLE) {
			return type;
		}

		if (type < curr_mem_type) {
			curr_mem_type = type;
		}
	}

	if (curr_mem_type == MEMORY_TYPE_INVALID) {
		return MEMORY_TYPE_UNCACHEABLE;
	}

	return curr_mem_type;
}
