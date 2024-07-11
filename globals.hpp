//#pragma comment(linker, "/MERGE:.rdata=.data")

#define ZYDIS_DISABLE_ENCODER
#include "zydis/Zydis/Zydis.h"
#pragma comment(lib, "zydis/Zydis/Zycore.lib")
#pragma comment(lib, "zydis/Zydis/Zydis.lib")

//libs
#include <fclang_defs.hpp>
#include <stl/types.hpp>
#include <stl/crypt_str.hpp>
#include <stl/string.hpp>
#include <utils/kernel/kernel.hpp>
#include <utils/shared/utils.hpp>
#include <crt/spinlock.hpp>
#include <utils/kernel/paging.hpp>
#include <utils/kernel/port_debug.hpp>
#include <crt/address.hpp>
#include <utils/kernel/internals.hpp>
#include <stl/meta.hpp>
#include <stl/bitcast.hpp>
#include <stl/align.hpp>


//base defs
#include <stdint.h>
#include <functional>
#include <array>
#include <bitset>
#include <optional>

//globals
volatile u64 hv_init = false;
std::pair<u64, u64> allowed_va_range;

//shared data
#define MAP_CONTEXT_HV_DEF
#include "../headers/shared_data_hv.hpp"

extern "C" {
	__declspec(dllexport) __declspec(allocate(".data")) map_ctx_hv_t mapper_ctx_hv {};
}

//helpers
#include "helpers.hpp"
#include "utils/memory_mgr.hpp"
#include "lab/instr_emu.hpp"

//arch
#include "amd/svm.hpp"
#include "intel/vmx.hpp"
//#include "amd/guest_context.hpp"
//#include "intel/guest_context.hpp"

//dbg
#include "utils/debug.hpp"

//phys mem
#include "utils/fast_phys.hpp"

//zydis
#include "zydis/Zydis/Zydis.h"