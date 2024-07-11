namespace apex
{
	u64 game_base = 0;
	u64 present_to_rip = 0;
	u64 present_from_rip = 0;

	u64 scr_size;
	u64 view_render = 0x7400bd0;
	u64 view_matrix = 0x11a350;

	u64 name_list = 0xbd27ba0;
	u64 entity_list = 0x1d87e88;
	u64 local_player_handle = 0x1582728;

	u64 health = 0x43c;
	u64 max_health = 0x578;

	u64 shield = 0x170;
	u64 shield_max = 0x174;
	u64 last_visible;
};