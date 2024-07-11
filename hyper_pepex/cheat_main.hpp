#include "offsets.hpp"
#include <crt/math.hpp>

template <typename T = u64, typename A>
inl T read_va1(A VirtAddress) {
	return read_guest_va<T>(get_vmcb()->cr3, VirtAddress);
}

template <typename T, typename A>
inl bool write_va1(A VirtAddress, const T& Data) {
	return write_guest_va<T>(get_vmcb()->cr3, VirtAddress, Data);
}

class Matrix4x4 {
public:
	float m[4][4];
};

bool w2s(const Vector3& world_pos, Vector2& screen_pos)
{
	DWORD64 ViewRender = read_va1(apex::game_base + apex::view_render);
	if (!ViewRender) {
		return false;
	}

	Matrix4x4 m_Matrix = read_va1<Matrix4x4>(ViewRender + apex::view_matrix);
	//if (!m_Matrix) 
		//return false;

	float w = 0.f;
	screen_pos.x =
		(m_Matrix.m[0][0] * world_pos.x) + 
		(m_Matrix.m[0][1] * world_pos.y) + 
		(m_Matrix.m[0][2] * world_pos.z + m_Matrix.m[0][3]);

	screen_pos.y =
		(m_Matrix.m[1][0] * world_pos.x) + 
		(m_Matrix.m[1][1] * world_pos.y) + 
		(m_Matrix.m[1][2] * world_pos.z + m_Matrix.m[1][3]);

	w = (m_Matrix.m[3][0] * world_pos.x) + 
		(m_Matrix.m[3][1] * world_pos.y) + 
		(m_Matrix.m[3][2] * world_pos.z + m_Matrix.m[3][3]);

	if (w < 0.01f) {
		return false;
	}
	
	float invw = 1.f / w;
	screen_pos.x *= invw;
	screen_pos.y *= invw;

	auto scr1 = read_va1<std::array<int, 2>>(apex::game_base + apex::scr_size);
	float scr2[2] = { (float)scr1[0], (float)scr1[1] };

	float x = scr2[0] / 2.f;
	float y = scr2[1] / 2.f;
	x += 0.5f * screen_pos.x * scr2[0] + 0.5f;
	y -= 0.5f * screen_pos.y * scr2[1] + 0.5f;
	
	screen_pos.x = x;
	screen_pos.y = y;
	return true;
}

u64 get_ent_by_id(int id) {
	const auto ent_list = apex::game_base + apex::entity_list;
	const auto ent_ptr_ptr = ent_list + ((u64)id << 5);
	const auto ent_ptr = read_va1(ent_ptr_ptr);
	return ent_ptr;
}

bool is_game_process() 
{
	auto name1 = amd_get_eprocess_name();
	fstring<char, 32> name2(name1.data());

	if ((name2 == "EasyAntiCheat_") ||
		(name2 == "r5apex.exe"))
	{
		return true;
	}

	return false;
}

bool game_init()
{
	//FF 50 40 40 38 35 ? ? ? ? 74
	//game present

	auto game_base = amd_get_process_base();
	
	apex::present_from_rip = game_base + 0x3E3500;
	
	const auto rip1 = apex::present_from_rip;
	const auto rva_off = read_va1<long>(rip1 + 2);
	if (!rva_off) {
		return false;
	}

	const auto rip2 = rip1 + 6 + rva_off;
	const auto dst_rip = read_va1(rip2);
	if (!dst_rip) {
		return false;
	}

	apex::game_base = game_base;
	apex::present_to_rip = dst_rip;

	//set present hook test
	u8 test1[2] = { 0x0F, 0xA2 };
	if (!write_guest_va_arr(get_vmcb()->cr3, apex::present_from_rip, &test1, 2)) {
		sp("fail hook! 1");
	}

	fmtp("pepex base: %p", game_base);
	fmtp("pepex present from rip: %p", apex::present_from_rip);
	fmtp("pepex present to rip: %p", apex::present_to_rip);

	sp("game init ok!");
	return true;
}

bool main_za_loop(core_data_amd_t& core_ctx)
{
	if (!apex::game_base) {
		if (is_game_process()) {
			//sp("game init!");
			game_init();
		}
	}

	else if (core_ctx->rip == apex::present_from_rip)
	{
		sp("game tick!");

		//for (int i = 0; i <= 150; ++i)
		//{
		//	auto ent = get_ent_by_id(i);
		//	if (!ent) {
		//		continue;
		//	}
		//}

		core_ctx->rip = apex::present_from_rip;
		core_ctx->rip_instr_len = 6;
	
		return true;
	}

	return false;
}