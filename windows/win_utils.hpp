namespace win_hv
{
	finl bool exec_um(core_data_t& core_data) {
		std::bitset<2> cpl = (u8)core_data.vm_state->csselector;
		return cpl.to_ulong() == 3;
	}

	finl auto get_kgs_cr3(core_data_t& core_data) {
		const auto gs_base = exec_um(core_data) ?
			(u64)core_data.vm_state->kernelgsbase :
			(u64)core_data.vm_state->gsbase;
		return std::pair((u64)core_data.vm_state->cr3, gs_base);
	}

	finl u64 get_current_ethread(core_data_t& core_data) {
		auto [ cr3, gs_base] = get_kgs_cr3(core_data);
		const auto ethread = read_gva<u64>(cr3, gs_base + 0x188);
		return ethread;
	}

	finl u64 get_current_eprocess(core_data_t& core_data) {
		auto [cr3, gs_base] = get_kgs_cr3(core_data);
		const auto ethread = read_gva<u64>(cr3, gs_base + 0x188);
		const auto eprocess = read_gva<u64>(cr3, ethread + 0xB8);
		return eprocess;
	}

	finl u64 get_process_base(core_data_t& core_data) {
		auto [cr3, gs_base] = get_kgs_cr3(core_data);
		const auto ethread = read_gva<u64>(cr3, gs_base + 0x188);
		const auto eprocess = read_gva<u64>(cr3, ethread + 0xB8);
		const auto process_base = read_gva<u64>(cr3, eprocess + 0x520);
		return process_base;
	}

	finl std::array<char, 32> get_eprocess_name(core_data_t& core_data) {
		auto [cr3, gs_base] = get_kgs_cr3(core_data);
		const auto ethread = read_gva<u64>(cr3, gs_base + 0x188);
		const auto eprocess = read_gva<u64>(cr3, ethread + 0xB8);
		const auto proc_name = read_gva<std::array<char, 32>>(cr3, eprocess + 0x5a8);
		return proc_name;
	}
}