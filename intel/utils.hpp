finl EPT_POINTER get_ept_ptr() {
	EPT_POINTER EptPointer; 
	EptPointer.value = _vmx_vmread(VMCS_CTRL_EPT_POINTER);
	return EptPointer;
}

finl int current_guest_cpl() {
	vmx_segment_access_rights ss;
	ss.flags = _vmx_vmread(VMCS_GUEST_CS_ACCESS_RIGHTS);
	return ss.descriptor_privilege_level;
}

finl void set_ept_ptr(EPT_POINTER EptPointer) {
	_vmx_vmwrite(VMCS_CTRL_EPT_POINTER, EptPointer.value);
}

inl interruptibility_state_t get_guest_interruptibility_state() {
	interruptibility_state_t interruptibility_state;
	interruptibility_state.flags = _vmx_vmread(VMCS_GUEST_INTERRUPTIBILITY_STATE);
	return interruptibility_state;
}

inl void set_guest_interruptibility_state(interruptibility_state_t interruptibility_state) {
	_vmx_vmwrite(VMCS_GUEST_INTERRUPTIBILITY_STATE, interruptibility_state.flags);
}

inl void set_mtf(bool state) 
{
	if (state) {
		auto int_state = get_guest_interruptibility_state();
		int_state.flags |= GUEST_INTR_STATE_MOV_SS;
		set_guest_interruptibility_state(int_state);
	}

	else {
		auto int_state = get_guest_interruptibility_state();
		int_state.flags &= ~GUEST_INTR_STATE_MOV_SS;
		set_guest_interruptibility_state(int_state);
	}

	VMX_CPU_BASED_CONTROLS CpuCtlRequested;
	CpuCtlRequested.All = _vmx_vmread(CPU_BASED_VM_EXEC_CONTROL);
	CpuCtlRequested.Fields.MonitorTrapFlag = state;
	_vmx_vmwrite(CPU_BASED_VM_EXEC_CONTROL, CpuCtlRequested.All);
}

inl void inject_hw_exception(const uint32_t vector, const uint32_t error)
{
	vmentry_interrupt_information interrupt_info;
	interrupt_info.flags = 0;
	interrupt_info.vector = vector;
	interrupt_info.interruption_type = hardware_exception;
	interrupt_info.deliver_error_code = 1;
	interrupt_info.valid = 1;
	_vmx_vmwrite(VMCS_CTRL_VMENTRY_INTERRUPTION_INFORMATION_FIELD, interrupt_info.flags);
	_vmx_vmwrite(VMCS_CTRL_VMENTRY_EXCEPTION_ERROR_CODE, error);
}

finl void inject_pf(u64 va, page_fault_exception flags) {
	_writecr2(va);
	inject_hw_exception(page_fault, flags.flags);
}