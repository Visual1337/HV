inl pvmcb get_vmcb() {
	return call_ptr<pvmcb>(&mapper_ctx_hv.get_vmcb_shell);
}

inl u64 get_amd_gs_base() {
	return get_vmcb()->gsbase;
}