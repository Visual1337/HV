
u64 hlatp_pfn = 0; 
alloc_page_desc* pml4_hlatp_buff;

alloc_page_desc* alloc_hlat_dir() 
{
	auto dir_ = alloc_page();
	auto dir__ = (PTE*)dir_->addr;

	for (int i = 0; i < 512; ++i) {
		dir__[i].present = true;
		dir__[i].hlat_reset = true;
	}

	return dir_;
}

void apply_hlat_dir(PTE* dir, u64 next_dir_pfn)
{
	PTE tmp1;
	tmp1.value = 0;
	tmp1.present = true;
	tmp1.write = true;
	tmp1.no_execute = false;
	tmp1.pfn = next_dir_pfn;

	dir->value = tmp1.value;
}

void protect_hlat_gva(
	u64 gva, u64 gpfn, 
	bool write, bool exec)
{
	VIRT_ADDR va1;
	va1.value = gva;

	auto hlat_pml4 = (PTE*)pml4_hlatp_buff->addr;
	auto& hlat_pml4e = hlat_pml4[va1.pml4_index];
	
	PTE* hlat_pdpt = nullptr;
	if (hlat_pml4e.hlat_reset) {
		auto hlat_pdpt_ = alloc_hlat_dir();
		hlat_pdpt = (PTE*)hlat_pdpt_->addr;
		apply_hlat_dir(&hlat_pml4e, hlat_pdpt_->org_pfn);
	}

	else {
		///
	}

	auto hlat_pdpte = hlat_pdpt[va1.pdpt_index];

	PTE* hlat_pd = nullptr;
	if (hlat_pdpte.hlat_reset) {
		auto hlat_pd_ = alloc_hlat_dir();
		hlat_pd = (PTE*)hlat_pd_->addr;
		apply_hlat_dir(&hlat_pdpte, hlat_pd_->org_pfn);
	}
	
	else {
		///
	}

	auto hlat_pde = hlat_pd[va1.pd_index];

	PTE* hlat_pt = nullptr;
	if (hlat_pde.hlat_reset) {
		auto hlat_pt_ = alloc_hlat_dir();
		hlat_pt = (PTE*)hlat_pt_->addr;
		apply_hlat_dir(&hlat_pde, hlat_pt_->org_pfn);
	}

	else {
		///
	}

	hlat_pt->no_execute = !exec;
	hlat_pt->write = write;
	hlat_pt->pfn = gpfn;

	//flush cache!
}

u64 init_hlatp()
{
	if (!hlatp_pfn) {
		pml4_hlatp_buff = alloc_hlat_dir();
		hlatp_pfn = pml4_hlatp_buff->org_pfn;
	}

	/*vmwrite(
		vmcs::control::PRIMARY_PROCBASED_EXEC_CONTROLS,
		Self::adjust_vmx_control(
			VmxControl::ProcessorBased,
			IA32_VMX_PROCBASED_CTLS_ACTIVATE_TERTIARY_CONTROLS_FLAG
			| vmread(vmcs::control::PRIMARY_PROCBASED_EXEC_CONTROLS),
			),
		);
	vmwrite(
		VMCS_CTRL_TERTIARY_PROCESSOR_BASED_VM_EXECUTION_CONTROLS,
		Self::adjust_vmx_control(
			VmxControl::ProcessorBased3,
			IA32_VMX_PROCBASED_CTLS3_ENABLE_HLAT_FLAG
			| IA32_VMX_PROCBASED_CTLS3_EPT_PAGING_WRITE_CONTROL_FLAG
			| IA32_VMX_PROCBASED_CTLS3_GUEST_PAGING_VERIFICATION_FLAG,
			),
		);
	vmwrite(VMCS_CTRL_HLAT_POINTER, self.hlat.as_ref() as* const _ as u64);*/

	return hlatp_pfn;
}