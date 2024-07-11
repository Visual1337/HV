//fastphys mapped memory range
constexpr_addr(pb, mapped_host_phys, (mapped_host_phys_pml << PXI_SHIFT));
constexpr_addr(pb, mapped_host_phys_end, mapped_host_phys + (mapped_host_phys_gb * PAGE_1GB))

//rw host fastphys primitives
finl bool read_pa_arr(u64 phys_addr, pv buff, u32 size)
{
	const auto full_phys_addr = mapped_host_phys + phys_addr;
	if ((phys_addr + size) > (u64)mapped_host_phys_end) {
		return false;
	}

	if (size == 8) {
		*(u64*)buff = *(u64*)full_phys_addr;
		return true;
	}
	
	else if (size == 16) {
		*(M128A*)buff = *(M128A*)full_phys_addr;
		return true;
	}

	__memcpy(buff, full_phys_addr, size);
	return true;
}

finl bool write_pa_arr(u64 phys_addr, pv buff, u32 size)
{
	const auto full_phys_addr = mapped_host_phys + phys_addr;
	if ((phys_addr + size) > (u64)mapped_host_phys_end) {
		return false;
	}

	if (size == 8) {
		*(u64*)full_phys_addr = *(u64*)buff;
		return true;
	}

	else if (size == 16) {
		*(M128A*)full_phys_addr = *(M128A*)buff;
		return true;
	}

	__memcpy(full_phys_addr, buff, size);
	return true;
}

template <typename T, typename A>
inl T read_pa(A phys_addr, bool* state = nullptr)
{
	T Data{};
	bool ret = read_pa_arr((u64)phys_addr, &Data, sizeof(T));
	if (state) { *state = ret; }
	return Data;
}

template <typename T, typename A>
inl bool write_pa(A phys_addr, const T& Data) {
	return write_pa_arr((u64)phys_addr, (pv)&Data, sizeof(T));
}