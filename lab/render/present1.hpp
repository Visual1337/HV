#include "dx11_defs.hpp"

#include "byte_render.hpp"

inl u64 GetVFunc(u64 cr3_, u64 Class, int Index)
{
	auto vft = read_gva<u64>(cr3_, Class);
	if (!vft) {
		sp("callback: vft null!");
		return 0;
	}

	auto vfunc = read_gva<u64>(cr3_, vft + (Index * 8));
	if (!vfunc) {
		sp("callback: vfunc null!");
		return 0;
	}

	return vfunc;
}

//u8 test_pix[1024 * 768 * 4];

noinl void present3(int w, int h, int pitch)
{
	//_disable();

	ByteRender br;
	br.alpha_sup = true;
	FColor::SetCChannelIndx(0, 1, 2, 3);

	//u32 prev_csr = _mm_getcsr();
	//_mm_setcsr(0x9f80);

	//br.Setup(&test_pix[0], w, h, pitch);
	//br.Clear();

	hp(1);

	br.FillRectangle({ 10,10 }, { 10,10 }, FColor(255, 0, 0));
	br.FillRectangle({ 20,20 }, { 10,10 }, FColor(0, 255, 0));
	br.FillRectangle({ 30,30 }, { 10,10 }, FColor(0, 0, 255));

	//_mm_setcsr(prev_csr);
	//_enable();
}

noinl void test_present2(hc_ctx_desc* entry, core_data_t& core_data)
{
	{
		u64 rip1 = core_data.vm_state->rip;
		rip1 += 6;
		core_data.vm_state->rip = rip1;
	}
	

	//sp("present 1");

	auto ff = win_hv::get_eprocess_name(core_data);
	
	if (!__stricmp(ff, "EasyAntiCheat_") &&
		!__stricmp(ff, "R5Apex.exe") &&
		!__stricmp(ff, "example_win32_"))
	{
		//sp(ff.data());

    	return;
	}

	
	sp("present 2");

	hc_main hc_state(entry);

	u64 user_swapchain1 = hc_state.core_data_ptr->context->rcx;
	u64 user_dirbase22 = core_data.vm_state->cr3;
	u64 user_memset1 = hc_data.um_memset;
	u64 user_malloc1 = hc_data.um_malloc;
	u64 user_free1 = hc_data.um_free;
	u64 user_memcpy1 = hc_data.um_memcpy;

	fmtp("swapchain: %p", user_swapchain1);
	
	auto user_buff1 = hc_state.call_func(user_malloc1, 0x1000);

	//get swapchain surface
	u64 dx_bbuff_surface1 = 0;
	{
		struct MyStruct1 {
			GUID guid;
			pv out_ptr;
		};
		hc_state.call_func(user_memset1, user_buff1, 0, sizeof(MyStruct1), 0);

		MyStruct1 var1;
		var1.guid = GUID{ 0x6f15aaf2, 0xd208, 0x4e89, { 0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c } };
		var1.out_ptr = 0;
		if (!write_gva(user_dirbase22, user_buff1, var1)) {
			//sp("write MyStruct1 fail!");
		}

		auto dx_get_buff = GetVFunc(user_dirbase22, user_swapchain1, 9);
		hc_state.call_func(dx_get_buff, user_swapchain1, 0, user_buff1, user_buff1 + FIELD_OFFSET(MyStruct1, out_ptr));

		auto dx_bbuff_surface = read_gva<u64>(user_dirbase22, user_buff1 + FIELD_OFFSET(MyStruct1, out_ptr));

		if (dx_bbuff_surface) {
			auto dx_release = GetVFunc(user_dirbase22, dx_bbuff_surface, 2);
			hc_state.call_func(dx_release, dx_bbuff_surface, 0, 0, 0);

			dx_bbuff_surface1 = dx_bbuff_surface;
			fmtp("dx surface: %p", dx_bbuff_surface);
		}
	}

	//get dx device
	u64 dx_device1 = 0; {
		struct MyStruct1 {
			GUID guid;
			pv out_ptr;
		};
		hc_state.call_func( user_memset1, user_buff1, 0, sizeof(MyStruct1), 0);
	
		MyStruct1 var1;
		var1.guid = GUID{ 0xdb6f6ddb, 0xac77, 0x4e88, { 0x82, 0x53, 0x81, 0x9d, 0xf9, 0xbb, 0xf1, 0x40 } };
		var1.out_ptr = 0;
		if (!write_gva(user_dirbase22, user_buff1, var1)) {
			//sp("write MyStruct1 fail!");
		}
	
		auto dx_get_buff = GetVFunc(user_dirbase22, user_swapchain1, 7);
		hc_state.call_func( dx_get_buff, user_swapchain1, user_buff1, user_buff1 + FIELD_OFFSET(MyStruct1, out_ptr), 0);
	
		auto dx_bbuff_surface = read_gva<u64>(user_dirbase22, user_buff1 + FIELD_OFFSET(MyStruct1, out_ptr));
	
		if (dx_bbuff_surface) {
			auto dx_release = GetVFunc(user_dirbase22, dx_bbuff_surface, 2);
			hc_state.call_func( dx_release, dx_bbuff_surface, 0, 0, 0);
	
			dx_device1 = dx_bbuff_surface;
			fmtp("dx device: %p", dx_bbuff_surface);
		}
	}
	
	//get dx surface desc
	struct MyStruct11 {
		dx_surface_desc desc;
		pv out_ptr;
	};
	static MyStruct11 surdesc1{};
	{
		hc_state.call_func( user_memset1,  user_buff1, 0, sizeof(MyStruct11), 0);
	
		dx_surface_desc var1{};
		if (!write_gva(user_dirbase22, user_buff1, var1)) {
			//sp("write MyStruct2 fail!");
		}
	
		auto dx_get_desc = GetVFunc(user_dirbase22, dx_bbuff_surface1, 10);
		hc_state.call_func( dx_get_desc, dx_bbuff_surface1, user_buff1, 0, 0);
	
		bool read = 1/*false*/;
		auto dx_bbuff_surface_desc = read_gva<MyStruct11>(user_dirbase22, user_buff1);
	
		if (read) {
			fmtp("dx surface: w:%d h:%d", dx_bbuff_surface_desc.desc.Width, dx_bbuff_surface_desc.desc.Height);
			surdesc1 = dx_bbuff_surface_desc;
		}
	}

	////create new d3d texture
	u64 d3d_tex1 = 0;
	{
		surdesc1.desc.BindFlags = 0;
		surdesc1.desc.Usage = D3D11_USAGE_STAGING;
		surdesc1.desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	
		hc_state.call_func( user_memset1, user_buff1, 0, sizeof(MyStruct11));
	
		if (!write_gva(user_dirbase22, user_buff1, surdesc1)) {
			//sp("write MyStruct3 fail!");
		}
	
		auto dx_create_texture = GetVFunc(user_dirbase22, dx_device1, 5);
		hc_state.call_func( dx_create_texture, dx_device1, user_buff1, 0, user_buff1 + FIELD_OFFSET(MyStruct11, out_ptr));
	
		bool read = 1/*false*/;
		auto dx_new_tex = read_gva<u64>(user_dirbase22, user_buff1 + FIELD_OFFSET(MyStruct11, out_ptr));
	
		if (read) {
			fmtp("dx new texture: %p", dx_new_tex);
			d3d_tex1 = dx_new_tex;
		}
	}

	u64 dx_deviceCtx1 = 0;
	{
		hc_state.call_func( user_memset1, user_buff1, 0, 8, 0);
	
		u64 var1 = 0;
		if (!write_gva(user_dirbase22, user_buff1, var1)) {
			//sp("write MyStruct4 fail!");
		}
	
		auto dx_get_buff = GetVFunc(user_dirbase22, dx_device1, 40);
		hc_state.call_func( dx_get_buff, dx_device1, user_buff1, 0, 0);
	
		auto dx_bbuff_surface = read_gva<u64>(user_dirbase22, user_buff1);
	
		if (dx_bbuff_surface) {
			auto dx_release = GetVFunc(user_dirbase22, dx_bbuff_surface, 2);
			hc_state.call_func( dx_release, dx_bbuff_surface, 0, 0, 0);
	
			dx_deviceCtx1 = dx_bbuff_surface;
			fmtp("dx device context: %p", dx_bbuff_surface);
		}
	}
	
	//copy to tmp surface
	{
		auto dx_get_buff = GetVFunc(user_dirbase22, dx_deviceCtx1, 47);
		hc_state.call_func( dx_get_buff,  dx_deviceCtx1, d3d_tex1, dx_bbuff_surface1, 0);
		fmtp("dx copy to tmp surface");
	}
	
	//write shit
	{
		typedef struct D3D11_MAPPED_SUBRESOURCE
		{
			void* pData;
			UINT RowPitch;
			UINT DepthPitch;
		} 	D3D11_MAPPED_SUBRESOURCE;
	
		hc_state.call_func( user_memset1,  user_buff1, 0, sizeof(D3D11_MAPPED_SUBRESOURCE), 0);
	
		//map
		{
			//fmtp("dx map1");
			auto dx_get_buff = GetVFunc(user_dirbase22, dx_deviceCtx1, 14);
			hc_state.call_func( dx_get_buff, dx_deviceCtx1, d3d_tex1, 0, 3, 0, user_buff1);
			//fmtp("dx map2");
		}
	
		auto dx_tex_buff_ = read_gva<u64>(user_dirbase22, user_buff1);
		fmtp("dx map tex: %p", dx_tex_buff_);
	
		//set flex
		fmtp("dx memset pixels1");
		
		const int pxSz1 = surdesc1.desc.Width * surdesc1.desc.Height * 4;
		hc_state.call_func( user_memcpy1, dx_tex_buff_, dx_tex_buff_, pxSz1, 0);
		
		//read_gva_arr(user_dirbase22, dx_tex_buff_, test_pix, pxSz1);

		present3(surdesc1.desc.Width, surdesc1.desc.Height, 0);

		//write_gva_arr(user_dirbase22, dx_tex_buff_, test_pix, pxSz1);

		fmtp("dx memset pixels2");
	
		//unmap
		{
			//fmtp("dx unmap1");
			auto dx_get_buff = GetVFunc(user_dirbase22, dx_deviceCtx1, 15);
			hc_state.call_func( dx_get_buff, dx_deviceCtx1, d3d_tex1, 0, 0);
			fmtp("dx unmap2");
		}
	}
	
	//copy from tmp surface
	{
		auto dx_get_buff = GetVFunc(user_dirbase22, dx_deviceCtx1, 47);
		hc_state.call_func( dx_get_buff, dx_deviceCtx1, dx_bbuff_surface1, d3d_tex1, 0);
	}
	
	//release bullshit
	{
		auto dx_release = GetVFunc(user_dirbase22, d3d_tex1, 2);
		hc_state.call_func( dx_release, d3d_tex1, 0, 0, 0);
	}

	if (user_buff1) {
		hc_state.call_func(user_free1, user_buff1);
	}

	return;


	////hc_state.call_func(hc_data.um_msgbox, 0, 0, 0, 0);

	//
	//return;

	//hc_state.call_func(hc_data.um_msgbox, 0, 0, 0, 0);

	//hc_state.end();
	//
	//auto rip_cur = (u64)core_data->context->rip;
	//core_data->context->rip = rip_cur + 6;
}

noinl void test_present(hc_ctx_desc* entry)
{
	//sp("test_present enter! 1");

	//intel
	u64 ret_val = 0;
	test_present2(entry, *get_core_data());
	//sp("test_present end! 1");

	/*fast ret exit!*/ {
		const auto core_ctx2 = get_core_data();
		free_hc_entry(core_ctx2->fast_restore_context, ret_val, entry);
		__builtin_unreachable();
	}
	
	//sp("test_present end! 2");
}