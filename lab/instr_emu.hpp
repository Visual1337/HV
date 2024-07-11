finl void mix_rflags(u64& dst, u64& src)
{
	std::bitset<64> dst1 = dst;
	std::bitset<64> src1 = src;

	dst1.set(0, src1.test(0));
	dst1.set(2, src1.test(2));
	dst1.set(4, src1.test(4));
	dst1.set(6, src1.test(6));
	dst1.set(7, src1.test(7));
	dst1.set(10, src1.test(10));
	dst1.set(11, src1.test(11));

	dst = dst1.to_ullong();
}

noinl void emu_add(u64& g_rflags, u64& r9__)
{
	u64 r9_ = r9__;
	u64 rflags1 = _readrflags();
	
	mix_rflags(rflags1, g_rflags);

	__asm {
		mov rax, rflags1
		push rax
		popfq

		mov r9, r9__
		and r9d, 0FFFFFC80h
		mov r9_, r9

		pushfq
		pop rax
		mov rflags1, rax
	}

	mix_rflags(g_rflags, rflags1);

	r9__ = r9_;
}