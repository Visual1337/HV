//DBG Help
//#undef DBG1

#ifdef DBG1
	volatile u64 dbg_ccnt = 0;
	#define sp(a)		com_printf("%d STR %d: %s", dbg_ccnt++, __LINE__, (a))
	#define hp(a)		com_printf("%d HEX %d: 0x%p", dbg_ccnt++, __LINE__, (a))
	#define dp(a)		com_printf("%d DEC %d: %d", dbg_ccnt++, __LINE__, (a))
	#define fmtp(...)	com_printf(__VA_ARGS__)

	//#define wsp(a) DbgPrintEx(0, 0, "\nFACE WSTR:%d %ws\n", __LINE__, (a))
	//#define hp(a) DbgPrintEx(0, 0, "\nFACE:%d ULL_HEX: 0x%p\n", __LINE__, (a))
	//#define ulp(a) DbgPrintEx(0, 0, "\nFACE:%d DW_HEX: 0x%X\n", __LINE__, (a))
	//#define sp(a) DbgPrintEx(0, 0, "\nFACE:%d STR: %s\n",  __LINE__, (a))
	//#define dp(a) DbgPrintEx(0, 0, "\nFACE:%d DEC: %d\n",  __LINE__, (a))
	//#define fmtp(...) DbgPrintEx(0, 0,  __VA_ARGS__)

#else
	#define wsp(a)
	#define hp(a)
	#define ulp(a)
	#define sp(a)
	#define dp(a)
	#define fmtp(...)
	#define fp(a)
	#define v2p(a)
	#define v3p(a)
	#define com_init()
	#define com_printf()
#endif



inl int get_current_core_id() {
	u64 ret_val; //TODO: FIX
	return call_ptr<int>(mapper_ctx_hv.getcoreid_fn, 0x90003, &ret_val);
}

noinl void fast_kill(u64 key = 0)
{
	_asm {
		mov rbx, key
		ud2
		mov rax, 0
		mov qword ptr [rax], 1
		int 3
	}
}

bool addr_in_allow_range(u64 ptr, u32 size = 0) 
{
	if ((ptr >= allowed_va_range.first) && 
		((ptr + size) < allowed_va_range.second))
	{
		return true;
	}

	return false;
}