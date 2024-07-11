noinl void enable_singlestep()
{
	u32 gg66 = __readeflags();
	std::bitset<32> hjj = gg66;
	hjj.set(8, true);
	__writeeflags(hjj.to_ullong());
}

noinl void disable_singlestep()
{
	u32 gg66 = __readeflags();
	std::bitset<32> hjj = gg66;
	hjj.set(8, false);
	__writeeflags(hjj.to_ullong());
}