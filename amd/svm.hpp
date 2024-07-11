#pragma once
#ifndef _AMD_SVM_
#define _AMD_SVM_

#include <bitset>
#include <utils/kernel/paging.hpp>

union InterceptVector2
{
	struct
	{
		u32 intercept_de : 1;
		u32 intercept_db : 1;
		u32 intercept_nmi : 1;
		u32 intercept_bp : 1;
		u32 intercept_of : 1;
		u32 intercept_br : 1;
		u32 intercept_ud : 1;
		u32 intercept_nm : 1;
		u32 intercept_df : 1;
		u32 reserved1 : 1;
		u32 intercept_ts : 1;
		u32 intercept_np : 1;
		u32 intercept_ss : 1;
		u32 intercept_gp : 1;
		u32 intercept_pf : 1;
		u32 pad : 15;
		u32 intercept_db2 : 1;
		u32 pad17 : 1;
	};
	u32 as_int32;
};
static_assert(sizeof(InterceptVector2) == 0x4, "InterceptVector2 Size Mismatch");

typedef struct _vmcb
{
	u16 interceptcrread;             // +0x000
	u16 interceptcrwrite;            // +0x002
	u16 interceptdrread;             // +0x004
	u16 interceptdrwrite;            // +0x006
	InterceptVector2 interceptexception; // +0x008
	u32 interceptmisc1;              // +0x00c
	u32 interceptmisc2;              // +0x010
	u8  reserved1[0x03c - 0x014];    // +0x014
	u16 pausefilterthreshold;        // +0x03c
	u16 pausefiltercount;            // +0x03e
	u64 iopmbasepa;                  // +0x040
	u64 msrpmbasepa;                 // +0x048
	u64 tscoffset;                   // +0x050
	u32 guestasid;                   // +0x058
	u32 tlbcontrol;                  // +0x05c
	u64 vintr;						 // +0x060
	std::bitset<64> interruptshadow; // +0x068
	u64 exitcode;                    // +0x070
	u64 exitinfo1;                   // +0x078
	u64 exitinfo2;                   // +0x080
	u64 exitintinfo;                 // +0x088
	u64 npenable;                    // +0x090
	u64 avicapicbar;                 // +0x098
	u64 guestpaofghcb;               // +0x0a0
	u64 eventinj;                    // +0x0a8
	u64 ncr3;                        // +0x0b0
	u64 lbrvirtualizationenable;     // +0x0b8
	std::bitset<64> vmcbclean;       // +0x0c0
	u64 nrip;                        // +0x0c8
	u8  numofbytesfetched;           // +0x0d0
	u8  guestinstructionbytes[15];   // +0x0d1
	u64 avicapicbackingpagepointer;  // +0x0e0
	u64 reserved2;                   // +0x0e8
	u64 aviclogicaltablepointer;     // +0x0f0
	u64 avicphysicaltablepointer;    // +0x0f8
	u64 reserved3;                   // +0x100
	u64 vmcbsavestatepointer;        // +0x108
	u8  reserved4[0x400 - 0x110];    // +0x110
	u16 esselector;                  // +0x000
	u16 esattrib;                    // +0x002
	u32 eslimit;                     // +0x004
	u64 esbase;                      // +0x008
	u16 csselector;                  // +0x010
	u16 csattrib;                    // +0x012
	u32 cslimit;                     // +0x014
	u64 csbase;                      // +0x018
	u16 ssselector;                  // +0x020
	u16 ssattrib;                    // +0x022
	u32 sslimit;                     // +0x024
	u64 ssbase;                      // +0x028
	u16 dsselector;                  // +0x030
	u16 dsattrib;                    // +0x032
	u32 dslimit;                     // +0x034
	u64 dsbase;                      // +0x038
	u16 fsselector;                  // +0x040
	u16 fsattrib;                    // +0x042
	u32 fslimit;                     // +0x044
	u64 fsbase;                      // +0x048
	u16 gsselector;                  // +0x050
	u16 gsattrib;                    // +0x052
	u32 gslimit;                     // +0x054
	u64 gsbase;                      // +0x058
	u16 gdtrselector;                // +0x060
	u16 gdtrattrib;                  // +0x062
	u32 gdtrlimit;                   // +0x064
	u64 gdtrbase;                    // +0x068
	u16 ldtrselector;                // +0x070
	u16 ldtrattrib;                  // +0x072
	u32 ldtrlimit;                   // +0x074
	u64 ldtrbase;                    // +0x078
	u16 idtrselector;                // +0x080
	u16 idtrattrib;                  // +0x082
	u32 idtrlimit;                   // +0x084
	u64 idtrbase;                    // +0x088
	u16 trselector;                  // +0x090
	u16 trattrib;                    // +0x092
	u32 trlimit;                     // +0x094
	u64 trbase;                      // +0x098
	u8  reserved_1[0x0cb - 0x0a0];   // +0x0a0
	u8  cpl;                         // +0x0cb
	u32 reserved_2;                  // +0x0cc
	u64 efer;                        // +0x0d0
	u8  reserved_3[0x148 - 0x0d8];   // +0x0d8
	u64 cr4;                         // +0x148
	u64 cr3;                         // +0x150
	u64 cr0;                         // +0x158
	u64 dr7;                         // +0x160
	u64 dr6;                         // +0x168
	std::bitset<64> rflags;          // +0x170
	u64 rip;                         // +0x178
	u8  reserved_4[0x1d8 - 0x180];   // +0x180
	u64 rsp;                         // +0x1d8
	u8  reserved5[0x1f8 - 0x1e0];    // +0x1e0
	u64 rax;                         // +0x1f8
	u64 star;                        // +0x200
	u64 lstar;                       // +0x208
	u64 cstar;                       // +0x210
	u64 sfmask;                      // +0x218
	u64 kernelgsbase;                // +0x220
	u64 sysentercs;                  // +0x228
	u64 sysenteresp;                 // +0x230
	u64 sysentereip;                 // +0x238
	u64 cr2;                         // +0x240
	u8  reserved6[0x268 - 0x248];    // +0x248
	u64 gpat;                        // +0x268
	u64 dbgctl;                      // +0x270
	u64 brfrom;                      // +0x278
	u64 brto;                        // +0x280
	u64 lastexcepfrom;               // +0x288
	u64 lastexcepto;                 // +0x290

	int instr_len() {
		return nrip - rip;
	}

	finl int get_cpl() {
		std::bitset<2> cpl_cpl_bits = csselector;
		return cpl_cpl_bits.to_ulong();
	}
} vmcb, *pvmcb;

typedef struct _QUERY_NPT
{
	u64 page_size;

	ULONG64 PEPML4E;
	ULONG64 PEPDPTE;
	ULONG64 PEPDE;
	ULONG64 PEPTE;

	PTE EPML4E;
	PTE EPDPTE;
	PTE EPDE;
	PTE EPTE;
} QUERY_NPT, * PQUERY_NPT;

struct NestedPageFaultInfo1 
{
	union {
		u64 as_uint64;
		struct {
			u64 valid : 1;                   // [0]
			u64 write : 1;                   // [1]
			u64 user : 1;                    // [2]
			u64 reserved : 1;                // [3]
			u64 execute : 1;                 // [4]
			u64 reserved2 : 27;              // [5:31]
			u64 guestphysicaladdress : 1;    // [32]
			u64 guestpagetables : 1;         // [33]
		} fields;
	};
};

#define NP_VMCLEAN_BIT				4
#define TPR_VMCLEAN_BIT				3
#define INTERCEPTS_VMCLEAN_BIT		0

#define VMEXIT_CR0_READ             0x0000
#define VMEXIT_CR1_READ             0x0001
#define VMEXIT_CR2_READ             0x0002
#define VMEXIT_CR3_READ             0x0003
#define VMEXIT_CR4_READ             0x0004
#define VMEXIT_CR5_READ             0x0005
#define VMEXIT_CR6_READ             0x0006
#define VMEXIT_CR7_READ             0x0007
#define VMEXIT_CR8_READ             0x0008
#define VMEXIT_CR9_READ             0x0009
#define VMEXIT_CR10_READ            0x000a
#define VMEXIT_CR11_READ            0x000b
#define VMEXIT_CR12_READ            0x000c
#define VMEXIT_CR13_READ            0x000d
#define VMEXIT_CR14_READ            0x000e
#define VMEXIT_CR15_READ            0x000f
#define VMEXIT_CR0_WRITE            0x0010
#define VMEXIT_CR1_WRITE            0x0011
#define VMEXIT_CR2_WRITE            0x0012
#define VMEXIT_CR3_WRITE            0x0013
#define VMEXIT_CR4_WRITE            0x0014
#define VMEXIT_CR5_WRITE            0x0015
#define VMEXIT_CR6_WRITE            0x0016
#define VMEXIT_CR7_WRITE            0x0017
#define VMEXIT_CR8_WRITE            0x0018
#define VMEXIT_CR9_WRITE            0x0019
#define VMEXIT_CR10_WRITE           0x001a
#define VMEXIT_CR11_WRITE           0x001b
#define VMEXIT_CR12_WRITE           0x001c
#define VMEXIT_CR13_WRITE           0x001d
#define VMEXIT_CR14_WRITE           0x001e
#define VMEXIT_CR15_WRITE           0x001f
#define VMEXIT_DR0_READ             0x0020
#define VMEXIT_DR1_READ             0x0021
#define VMEXIT_DR2_READ             0x0022
#define VMEXIT_DR3_READ             0x0023
#define VMEXIT_DR4_READ             0x0024
#define VMEXIT_DR5_READ             0x0025
#define VMEXIT_DR6_READ             0x0026
#define VMEXIT_DR7_READ             0x0027
#define VMEXIT_DR8_READ             0x0028
#define VMEXIT_DR9_READ             0x0029
#define VMEXIT_DR10_READ            0x002a
#define VMEXIT_DR11_READ            0x002b
#define VMEXIT_DR12_READ            0x002c
#define VMEXIT_DR13_READ            0x002d
#define VMEXIT_DR14_READ            0x002e
#define VMEXIT_DR15_READ            0x002f
#define VMEXIT_DR0_WRITE            0x0030
#define VMEXIT_DR1_WRITE            0x0031
#define VMEXIT_DR2_WRITE            0x0032
#define VMEXIT_DR3_WRITE            0x0033
#define VMEXIT_DR4_WRITE            0x0034
#define VMEXIT_DR5_WRITE            0x0035
#define VMEXIT_DR6_WRITE            0x0036
#define VMEXIT_DR7_WRITE            0x0037
#define VMEXIT_DR8_WRITE            0x0038
#define VMEXIT_DR9_WRITE            0x0039
#define VMEXIT_DR10_WRITE           0x003a
#define VMEXIT_DR11_WRITE           0x003b
#define VMEXIT_DR12_WRITE           0x003c
#define VMEXIT_DR13_WRITE           0x003d
#define VMEXIT_DR14_WRITE           0x003e
#define VMEXIT_DR15_WRITE           0x003f
#define VMEXIT_EXCEPTION_DE         0x0040
#define VMEXIT_EXCEPTION_DB         0x0041
#define VMEXIT_EXCEPTION_NMI        0x0042
#define VMEXIT_EXCEPTION_BP         0x0043
#define VMEXIT_EXCEPTION_OF         0x0044
#define VMEXIT_EXCEPTION_BR         0x0045
#define VMEXIT_EXCEPTION_UD         0x0046
#define VMEXIT_EXCEPTION_NM         0x0047
#define VMEXIT_EXCEPTION_DF         0x0048
#define VMEXIT_EXCEPTION_09         0x0049
#define VMEXIT_EXCEPTION_TS         0x004a
#define VMEXIT_EXCEPTION_NP         0x004b
#define VMEXIT_EXCEPTION_SS         0x004c
#define VMEXIT_EXCEPTION_GP         0x004d
#define VMEXIT_EXCEPTION_PF         0x004e
#define VMEXIT_EXCEPTION_15         0x004f
#define VMEXIT_EXCEPTION_MF         0x0050
#define VMEXIT_EXCEPTION_AC         0x0051
#define VMEXIT_EXCEPTION_MC         0x0052
#define VMEXIT_EXCEPTION_XF         0x0053
#define VMEXIT_EXCEPTION_20         0x0054
#define VMEXIT_EXCEPTION_21         0x0055
#define VMEXIT_EXCEPTION_22         0x0056
#define VMEXIT_EXCEPTION_23         0x0057
#define VMEXIT_EXCEPTION_24         0x0058
#define VMEXIT_EXCEPTION_25         0x0059
#define VMEXIT_EXCEPTION_26         0x005a
#define VMEXIT_EXCEPTION_27         0x005b
#define VMEXIT_EXCEPTION_28         0x005c
#define VMEXIT_EXCEPTION_VC         0x005d
#define VMEXIT_EXCEPTION_SX         0x005e
#define VMEXIT_EXCEPTION_31         0x005f
#define VMEXIT_INTR                 0x0060
#define VMEXIT_NMI                  0x0061
#define VMEXIT_SMI                  0x0062
#define VMEXIT_INIT                 0x0063
#define VMEXIT_VINTR                0x0064
#define VMEXIT_CR0_SEL_WRITE        0x0065
#define VMEXIT_IDTR_READ            0x0066
#define VMEXIT_GDTR_READ            0x0067
#define VMEXIT_LDTR_READ            0x0068
#define VMEXIT_TR_READ              0x0069
#define VMEXIT_IDTR_WRITE           0x006a
#define VMEXIT_GDTR_WRITE           0x006b
#define VMEXIT_LDTR_WRITE           0x006c
#define VMEXIT_TR_WRITE             0x006d
#define VMEXIT_RDTSC                0x006e
#define VMEXIT_RDPMC                0x006f
#define VMEXIT_PUSHF                0x0070
#define VMEXIT_POPF                 0x0071
#define VMEXIT_CPUID                0x0072
#define VMEXIT_RSM                  0x0073
#define VMEXIT_IRET                 0x0074
#define VMEXIT_SWINT                0x0075
#define VMEXIT_INVD                 0x0076
#define VMEXIT_PAUSE                0x0077
#define VMEXIT_HLT                  0x0078
#define VMEXIT_INVLPG               0x0079
#define VMEXIT_INVLPGA              0x007a
#define VMEXIT_IOIO                 0x007b
#define VMEXIT_MSR                  0x007c
#define VMEXIT_TASK_SWITCH          0x007d
#define VMEXIT_FERR_FREEZE          0x007e
#define VMEXIT_SHUTDOWN             0x007f
#define VMEXIT_VMRUN                0x0080
#define VMEXIT_VMMCALL              0x0081
#define VMEXIT_VMLOAD               0x0082
#define VMEXIT_VMSAVE               0x0083
#define VMEXIT_STGI                 0x0084
#define VMEXIT_CLGI                 0x0085
#define VMEXIT_SKINIT               0x0086
#define VMEXIT_RDTSCP               0x0087
#define VMEXIT_ICEBP                0x0088
#define VMEXIT_WBINVD               0x0089
#define VMEXIT_MONITOR              0x008a
#define VMEXIT_MWAIT                0x008b
#define VMEXIT_MWAIT_CONDITIONAL    0x008c
#define VMEXIT_XSETBV               0x008d
#define VMEXIT_EFER_WRITE_TRAP      0x008f
#define VMEXIT_CR0_WRITE_TRAP       0x0090
#define VMEXIT_CR1_WRITE_TRAP       0x0091
#define VMEXIT_CR2_WRITE_TRAP       0x0092
#define VMEXIT_CR3_WRITE_TRAP       0x0093
#define VMEXIT_CR4_WRITE_TRAP       0x0094
#define VMEXIT_CR5_WRITE_TRAP       0x0095
#define VMEXIT_CR6_WRITE_TRAP       0x0096
#define VMEXIT_CR7_WRITE_TRAP       0x0097
#define VMEXIT_CR8_WRITE_TRAP       0x0098
#define VMEXIT_CR9_WRITE_TRAP       0x0099
#define VMEXIT_CR10_WRITE_TRAP      0x009a
#define VMEXIT_CR11_WRITE_TRAP      0x009b
#define VMEXIT_CR12_WRITE_TRAP      0x009c
#define VMEXIT_CR13_WRITE_TRAP      0x009d
#define VMEXIT_CR14_WRITE_TRAP      0x009e
#define VMEXIT_CR15_WRITE_TRAP      0x009f
#define VMEXIT_NPF                  0x0400
#define AVIC_INCOMPLETE_IPI         0x0401
#define AVIC_NOACCEL                0x0402
#define VMEXIT_VMGEXIT              0x0403
#define VMEXIT_INVALID              -1

#endif // _AMD_SVM_
