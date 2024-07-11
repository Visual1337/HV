#define GUEST_ES_SELECTOR               0x00000800
#define GUEST_CS_SELECTOR               0x00000802
#define GUEST_SS_SELECTOR               0x00000804
#define GUEST_DS_SELECTOR               0x00000806
#define GUEST_FS_SELECTOR               0x00000808
#define GUEST_GS_SELECTOR               0x0000080a
#define GUEST_LDTR_SELECTOR             0x0000080c
#define GUEST_TR_SELECTOR               0x0000080e
#define HOST_ES_SELECTOR                0x00000c00
#define HOST_CS_SELECTOR                0x00000c02
#define HOST_SS_SELECTOR                0x00000c04
#define HOST_DS_SELECTOR                0x00000c06
#define HOST_FS_SELECTOR                0x00000c08
#define HOST_GS_SELECTOR                0x00000c0a
#define HOST_TR_SELECTOR                0x00000c0c
#define IO_BITMAP_A                     0x00002000
#define IO_BITMAP_A_HIGH                0x00002001
#define IO_BITMAP_B                     0x00002002
#define IO_BITMAP_B_HIGH                0x00002003
#define GUEST_PDPTE0                    0x0000280A
#define GUEST_PDPTE1                    0x0000280C
#define GUEST_PDPTE2                    0x0000280E
#define GUEST_PDPTE3                    0x00002810
#define EPT_POINTER0                     0x0000201a
#define EPT_POINTER_HIGH                0x0000201b
#define MSR_BITMAP                      0x00002004
#define MSR_BITMAP_HIGH                 0x00002005
#define VM_EXIT_MSR_STORE_ADDR          0x00002006
#define VM_EXIT_MSR_STORE_ADDR_HIGH     0x00002007
#define VM_EXIT_MSR_LOAD_ADDR           0x00002008
#define VM_EXIT_MSR_LOAD_ADDR_HIGH      0x00002009
#define VM_ENTRY_MSR_LOAD_ADDR          0x0000200a
#define VM_ENTRY_MSR_LOAD_ADDR_HIGH     0x0000200b
#define TSC_OFFSET                      0x00002010
#define TSC_OFFSET_HIGH                 0x00002011
#define VIRTUAL_APIC_PAGE_ADDR          0x00002012
#define VIRTUAL_APIC_PAGE_ADDR_HIGH     0x00002013
#define VMCS_LINK_POINTER               0x00002800
#define VMCS_LINK_POINTER_HIGH          0x00002801
#define GUEST_IA32_DEBUGCTL             0x00002802
#define GUEST_IA32_DEBUGCTL_HIGH        0x00002803
#define PIN_BASED_VM_EXEC_CONTROL       0x00004000
#define CPU_BASED_VM_EXEC_CONTROL       0x00004002
#define EXCEPTION_BITMAP                0x00004004
#define PAGE_FAULT_ERROR_CODE_MASK      0x00004006
#define PAGE_FAULT_ERROR_CODE_MATCH     0x00004008
#define CR3_TARGET_COUNT                0x0000400a
#define VM_EXIT_CONTROLS                0x0000400c
#define VM_EXIT_MSR_STORE_COUNT         0x0000400e
#define VM_EXIT_MSR_LOAD_COUNT          0x00004010
#define VM_ENTRY_CONTROLS               0x00004012
#define VM_ENTRY_MSR_LOAD_COUNT         0x00004014
#define VM_ENTRY_INTR_INFO_FIELD        0x00004016
#define VM_ENTRY_EXCEPTION_ERROR_CODE   0x00004018
#define VM_ENTRY_INSTRUCTION_LEN        0x0000401a
#define TPR_THRESHOLD                   0x0000401c
#define SECONDARY_VM_EXEC_CONTROL       0x0000401e
#define VM_INSTRUCTION_ERROR            0x00004400
#define VM_EXIT_REASON                  0x00004402
#define VM_EXIT_INTR_INFO               0x00004404
#define VM_EXIT_INTR_ERROR_CODE         0x00004406
#define IDT_VECTORING_INFO_FIELD        0x00004408
#define IDT_VECTORING_ERROR_CODE        0x0000440a
#define VM_EXIT_INSTRUCTION_LEN         0x0000440c
#define VMX_INSTRUCTION_INFO            0x0000440e
#define GUEST_ES_LIMIT                  0x00004800
#define GUEST_CS_LIMIT                  0x00004802
#define GUEST_SS_LIMIT                  0x00004804
#define GUEST_DS_LIMIT                  0x00004806
#define GUEST_FS_LIMIT                  0x00004808
#define GUEST_GS_LIMIT                  0x0000480a
#define GUEST_LDTR_LIMIT                0x0000480c
#define GUEST_TR_LIMIT                  0x0000480e
#define GUEST_GDTR_LIMIT                0x00004810
#define GUEST_IDTR_LIMIT                0x00004812
#define GUEST_ES_AR_BYTES               0x00004814
#define GUEST_CS_AR_BYTES               0x00004816
#define GUEST_SS_AR_BYTES               0x00004818
#define GUEST_DS_AR_BYTES               0x0000481a
#define GUEST_FS_AR_BYTES               0x0000481c
#define GUEST_GS_AR_BYTES               0x0000481e
#define GUEST_LDTR_AR_BYTES             0x00004820
#define GUEST_TR_AR_BYTES               0x00004822
#define GUEST_INTERRUPTIBILITY_INFO     0x00004824
#define GUEST_ACTIVITY_STATE            0x00004826
#define GUEST_SM_BASE                   0x00004828
#define GUEST_SYSENTER_CS               0x0000482A
#define HOST_IA32_SYSENTER_CS           0x00004c00
#define CR0_GUEST_HOST_MASK             0x00006000
#define CR4_GUEST_HOST_MASK             0x00006002
#define CR0_READ_SHADOW                 0x00006004
#define CR4_READ_SHADOW                 0x00006006
#define CR3_TARGET_VALUE0               0x00006008
#define CR3_TARGET_VALUE1               0x0000600a
#define CR3_TARGET_VALUE2               0x0000600c
#define CR3_TARGET_VALUE3               0x0000600e
#define EXIT_QUALIFICATION              0x00006400
#define GUEST_LINEAR_ADDRESS            0x0000640a
#define GUEST_CR0                       0x00006800
#define GUEST_CR3                       0x00006802
#define GUEST_CR4                       0x00006804
#define GUEST_ES_BASE                   0x00006806
#define GUEST_CS_BASE                   0x00006808
#define GUEST_SS_BASE                   0x0000680a
#define GUEST_DS_BASE                   0x0000680c
#define GUEST_FS_BASE                   0x0000680e
#define GUEST_GS_BASE                   0x00006810
#define IA32_KERNEL_GS_BASE             0xC0000102
#define GUEST_LDTR_BASE                 0x00006812
#define GUEST_TR_BASE                   0x00006814
#define GUEST_GDTR_BASE                 0x00006816
#define GUEST_IDTR_BASE                 0x00006818
#define GUEST_DR7                       0x0000681a
#define GUEST_RSP                       0x0000681c
#define GUEST_RIP                       0x0000681e
#define GUEST_RFLAGS                    0x00006820
#define GUEST_PENDING_DBG_EXCEPTIONS    0x00006822
#define GUEST_SYSENTER_ESP              0x00006824
#define GUEST_SYSENTER_EIP              0x00006826
#define HOST_CR0                        0x00006c00
#define HOST_CR3                        0x00006c02
#define HOST_CR4                        0x00006c04
#define HOST_FS_BASE                    0x00006c06
#define HOST_GS_BASE                    0x00006c08
#define HOST_TR_BASE                    0x00006c0a
#define HOST_GDTR_BASE                  0x00006c0c
#define HOST_IDTR_BASE                  0x00006c0e
#define HOST_IA32_SYSENTER_ESP          0x00006c10
#define HOST_IA32_SYSENTER_EIP          0x00006c12
#define HOST_RSP                        0x00006c14
#define HOST_RIP                        0x00006c16

#define APIC_BASE_EXTD (1 << 10)
#define MSR_IA32_APICBASE   0x0000001b
#define MSR_IA32_APICBASE_BSP		(1<<8)
#define MSR_IA32_APICBASE_ENABLE	(1<<11)
#define MSR_IA32_APICBASE_BASE		(0xfffff<<12)

#define VM_EXIT_NMI             0       // Exception or non-maskable interrupt (NMI)
#define VM_EXIT_INT             1       // External interrupt
#define VM_EXIT_TF              2       // Triple fault
#define VM_EXIT_INIT            3       // An INIT signal arrived
#define VM_EXIT_SIPI            4       // Start-up IPI (SIPI)
#define VM_EXIT_SMI             5       // I/O system-management interrupt (SMI)
#define VM_EXIT_OTHER_SMI       6       // Other SMI
#define VM_EXIT_INT_WND         7       // Interrupt window
#define VM_EXIT_NMI_WND         8       // NMI window
#define VM_EXIT_TASK            9       // Guest software attempted a task switch
#define VM_EXIT_CPUID           10      // Guest software attempted to execute CPUID
#define VM_EXIT_GETSEC          11      // Guest software attempted to execute GETSEC
#define VM_EXIT_HLT             12      // Guest software attempted to execute HLT
#define VM_EXIT_INVD            13      // Guest software attempted to execute INVD
#define VM_EXIT_INVLPG          14      // Guest software attempted to execute INVLPG
#define VM_EXIT_RDPMC           15      // Guest software attempted to execute RDPMC
#define VM_EXIT_RDTSC           16      // Guest software attempted to execute RDTSC
#define VM_EXIT_RSM             17      // Guest software attempted to execute RSM in SMM
#define VM_EXIT_VMCALL          18      // VMCALL was executed
#define VM_EXIT_VMCLEAR         19      // Guest software attempted to execute VMCLEAR
#define VM_EXIT_VMLAUNCH        20      // Guest software attempted to execute VMLAUNCH
#define VM_EXIT_VMPTRLD         21      // Guest software attempted to execute VMPTRLD
#define VM_EXIT_VMPTRST         22      // Guest software attempted to execute VMPTRST
#define VM_EXIT_VMREAD          23      // Guest software attempted to execute VMREAD
#define VM_EXIT_VMRESUME        24      // Guest software attempted to execute VMRESUME
#define VM_EXIT_VMWRITE         25      // Guest software attempted to execute VMWRITE
#define VM_EXIT_VMXOFF          26      // Guest software attempted to execute VMXOFF
#define VM_EXIT_VMXON           27      // Guest software attempted to execute VMXON
#define VM_EXIT_CR              28      // Guest software attempted to access CR0, CR3, CR4, or CR8
#define VM_EXIT_DR              29      // Guest software attempted a MOV to or from a debug register
#define VM_EXIT_IO              30      // Guest software attempted to execute an I/O instruction
#define VM_EXIT_RDMSR           31      // Guest software attempted to execute RDMSR
#define VM_EXIT_WRMSR           32      // Guest software attempted to execute WRMSR
#define VM_EXIT_VM_FAIL_STATE   33      // VM-entry failure due to invalid guest state
#define VM_EXIT_VM_FAIL_MSR     34      // VM-entry failure due to MSR loading
#define VM_EXIT_MWAIT           36      // Guest software attempted to execute MWAIT
#define VM_EXIT_TRAP            37      // A VM entry occurred due to the 1-setting of the “monitor trap flag”
#define VM_EXIT_MONITOR         39      // Guest software attempted to execute MONITOR
#define VM_EXIT_PAUSE           40      // Guest software attempted to execute PAUSE
#define VM_EXIT_VM_FAIL_MC      41      // VM-entry failure due to machine-check event
#define VM_EXIT_TPR             43      // TPR below threshold
#define VM_EXIT_APIC            44      // APIC access
#define VM_EXIT_EOI             45      // Virtualized EOI
#define VM_EXIT_GDTR            46      // Guest software attempted to execute LGDT, LIDT, SGDT, or SIDT
#define VM_EXIT_LDTR            47      // Guest software attempted to execute LLDT, LTR, SLDT, or STR
#define VM_EXIT_EPT_VIOLATION   48      // EPT violation
#define VM_EXIT_EPT_MISCONFIG   49      // EPT misconfiguration
#define VM_EXIT_INVEPT          50      // Guest software attempted to execute INVEPT
#define VM_EXIT_RDTSCP          51      // Guest software attempted to execute RDTSCP
#define VM_EXIT_VMX_TIMER       52      // VMX-preemption timer expired
#define VM_EXIT_INVVPID         53      // Guest software attempted to execute INVVPID
#define VM_EXIT_WBINVD          54      // Guest software attempted to execute WBINVD
#define VM_EXIT_XSETBV          55      // Guest software attempted to execute XSETBV
#define VM_EXIT_APIC_WRITE      56      // Guest software completed a write to the virtual-APIC page
#define VM_EXIT_RDRAND          57      // Guest software attempted to execute RDRAND
#define VM_EXIT_INVPCID         58      // Guest software attempted to execute INVPCID
#define VM_EXIT_VMFUNC          59      // Guest software invoked a VM function with the VMFUNC instruction
#define VM_EXIT_RDSEED          61      // Guest software attempted to execute RDSEED
#define VM_EXIT_XSAVES          63      // Guest software attempted to execute XSAVES
#define VM_EXIT_XRSTORS         64      // Guest software attempted to execute XRSTORS

#define VMCS_EXIT_QUALIFICATION                                      0x00006400
#define VMCS_EXIT_GUEST_LINEAR_ADDRESS                               0x0000640A
#define VMCS_GUEST_PHYSICAL_ADDRESS                                  0x00002400
#define VMCS_CTRL_EPT_POINTER                                        0x0000201A
#define VMCS_GUEST_CR3                                               0x00006802
#define VMCS_GUEST_INTERRUPTIBILITY_STATE                            0x00004824
#define VMCS_EXIT_REASON                                             0x00004402
#define VMCS_VMEXIT_INSTRUCTION_LENGTH                               0x0000440C
#define VMCS_GUEST_RIP                                               0x0000681E
#define VMCS_GUEST_RFLAGS                                            0x00006820

#define VMX_EXIT_REASON_EXECUTE_CPUID                                0x0000000A
#define VMX_EXIT_REASON_EXECUTE_HLT                                  0x0000000C
#define VMX_EXIT_REASON_MONITOR_TRAP_FLAG                            0x00000025
#define VMX_EXIT_REASON_EPT_VIOLATION                                0x00000030


#define GUEST_INTR_STATE_STI          0x00000001
#define GUEST_INTR_STATE_MOV_SS       0x00000002
#define GUEST_INTR_STATE_SMI          0x00000004
#define GUEST_INTR_STATE_NMI          0x00000008
#define GUEST_INTR_STATE_ENCLAVE_INTR 0x00000010

#define IA32_MTRR_CAPABILITIES                                       0x000000FE
#define IA32_MTRR_DEF_TYPE                                           0x000002FF
#define IA32_MTRR_PHYSMASK0                                          0x00000201
#define IA32_MTRR_PHYSBASE0                                          0x00000200

#define MEMORY_TYPE_UNCACHEABLE                                      0x00000000
#define MEMORY_TYPE_INVALID                                          0x000000FF


#define VMCS_CTRL_VMENTRY_INTERRUPTION_INFORMATION_FIELD             0x00004016
#define VMCS_CTRL_VMENTRY_EXCEPTION_ERROR_CODE                       0x00004018

//#define GUEST_INTERRUPTIBILITY_INFO 0x00004824;

typedef union
{
    struct
    {
        UINT32 BasicExitReason : 16;
        UINT32 Always0 : 1;
        UINT32 Reserved1 : 10;
        UINT32 EnclaveMode : 1;
        UINT32 PendingMtfVmExit : 1;
        UINT32 VmExitFromVmxRoot : 1;
        UINT32 Reserved2 : 1;
        UINT32 VmEntryFailure : 1;
    };

    UINT32 Flags;
} VMX_VMEXIT_REASON;

typedef union {
    struct {
        u64 ReadAccess : 1;
        u64 WriteAccess : 1;
        u64 ExecuteAccess : 1;
        u64 EptReadable : 1;
        u64 EptWriteable : 1;
        u64 EptExecutable : 1;
        u64 EptExecutableForUserMode : 1;
        u64 ValidGuestLinearAddress : 1;
        u64 CausedByTranslation : 1;
        u64 UserModeLinearAddress : 1;
        u64 ReadableWritablePage : 1;
        u64 ExecuteDisablePage : 1;
        u64 NmiUnblocking : 1;
        u64 Reserved1 : 51;
    };

    u64 Flags;
} VMX_EXIT_QUALIFICATION_EPT_VIOLATION;

//mtrr
typedef union
{
    struct
    {
        /**
         * @brief VCNT (variable range registers count) field
         *
         * [Bits 7:0] Indicates the number of variable ranges implemented on the processor.
         */
        uint64_t variable_range_count : 8;
#define IA32_MTRR_CAPABILITIES_VARIABLE_RANGE_COUNT_BIT              0
#define IA32_MTRR_CAPABILITIES_VARIABLE_RANGE_COUNT_FLAG             0xFF
#define IA32_MTRR_CAPABILITIES_VARIABLE_RANGE_COUNT_MASK             0xFF
#define IA32_MTRR_CAPABILITIES_VARIABLE_RANGE_COUNT(_)               (((_) >> 0) & 0xFF)

        /**
         * @brief FIX (fixed range registers supported) flag
         *
         * [Bit 8] Fixed range MTRRs (IA32_MTRR_FIX64K_00000 through IA32_MTRR_FIX4K_0F8000) are supported when set; no fixed range
         * registers are supported when clear.
         */
        uint64_t fixed_range_supported : 1;
#define IA32_MTRR_CAPABILITIES_FIXED_RANGE_SUPPORTED_BIT             8
#define IA32_MTRR_CAPABILITIES_FIXED_RANGE_SUPPORTED_FLAG            0x100
#define IA32_MTRR_CAPABILITIES_FIXED_RANGE_SUPPORTED_MASK            0x01
#define IA32_MTRR_CAPABILITIES_FIXED_RANGE_SUPPORTED(_)              (((_) >> 8) & 0x01)
        uint64_t reserved1 : 1;

        /**
         * @brief WC (write combining) flag
         *
         * [Bit 10] The write-combining (WC) memory type is supported when set; the WC type is not supported when clear.
         */
        uint64_t wc_supported : 1;
#define IA32_MTRR_CAPABILITIES_WC_SUPPORTED_BIT                      10
#define IA32_MTRR_CAPABILITIES_WC_SUPPORTED_FLAG                     0x400
#define IA32_MTRR_CAPABILITIES_WC_SUPPORTED_MASK                     0x01
#define IA32_MTRR_CAPABILITIES_WC_SUPPORTED(_)                       (((_) >> 10) & 0x01)

        /**
         * @brief SMRR (System-Management Range Register) flag
         *
         * [Bit 11] The system-management range register (SMRR) interface is supported when bit 11 is set; the SMRR interface is
         * not supported when clear.
         */
        uint64_t smrr_supported : 1;
#define IA32_MTRR_CAPABILITIES_SMRR_SUPPORTED_BIT                    11
#define IA32_MTRR_CAPABILITIES_SMRR_SUPPORTED_FLAG                   0x800
#define IA32_MTRR_CAPABILITIES_SMRR_SUPPORTED_MASK                   0x01
#define IA32_MTRR_CAPABILITIES_SMRR_SUPPORTED(_)                     (((_) >> 11) & 0x01)
        uint64_t reserved2 : 52;
    };

    uint64_t flags;
} ia32_mtrr_capabilities_register;

typedef union
{
    struct
    {
        /**
         * [Bits 2:0] Default Memory Type.
         */
        uint64_t default_memory_type : 3;
#define IA32_MTRR_DEF_TYPE_DEFAULT_MEMORY_TYPE_BIT                   0
#define IA32_MTRR_DEF_TYPE_DEFAULT_MEMORY_TYPE_FLAG                  0x07
#define IA32_MTRR_DEF_TYPE_DEFAULT_MEMORY_TYPE_MASK                  0x07
#define IA32_MTRR_DEF_TYPE_DEFAULT_MEMORY_TYPE(_)                    (((_) >> 0) & 0x07)
        uint64_t reserved1 : 7;

        /**
         * [Bit 10] Fixed Range MTRR Enable.
         */
        uint64_t fixed_range_mtrr_enable : 1;
#define IA32_MTRR_DEF_TYPE_FIXED_RANGE_MTRR_ENABLE_BIT               10
#define IA32_MTRR_DEF_TYPE_FIXED_RANGE_MTRR_ENABLE_FLAG              0x400
#define IA32_MTRR_DEF_TYPE_FIXED_RANGE_MTRR_ENABLE_MASK              0x01
#define IA32_MTRR_DEF_TYPE_FIXED_RANGE_MTRR_ENABLE(_)                (((_) >> 10) & 0x01)

        /**
         * [Bit 11] MTRR Enable.
         */
        uint64_t mtrr_enable : 1;
#define IA32_MTRR_DEF_TYPE_MTRR_ENABLE_BIT                           11
#define IA32_MTRR_DEF_TYPE_MTRR_ENABLE_FLAG                          0x800
#define IA32_MTRR_DEF_TYPE_MTRR_ENABLE_MASK                          0x01
#define IA32_MTRR_DEF_TYPE_MTRR_ENABLE(_)                            (((_) >> 11) & 0x01)
        uint64_t reserved2 : 52;
    };

    uint64_t flags;
} ia32_mtrr_def_type_register;

typedef union
{
    struct
    {
        /**
         * [Bits 7:0] Specifies the memory type for the range.
         */
        uint64_t type : 8;
#define IA32_MTRR_PHYSBASE_TYPE_BIT                                  0
#define IA32_MTRR_PHYSBASE_TYPE_FLAG                                 0xFF
#define IA32_MTRR_PHYSBASE_TYPE_MASK                                 0xFF
#define IA32_MTRR_PHYSBASE_TYPE(_)                                   (((_) >> 0) & 0xFF)
        uint64_t reserved1 : 4;

        /**
         * [Bits 47:12] Specifies the base address of the address range. This 24-bit value, in the case where MAXPHYADDR is 36
         * bits, is extended by 12 bits at the low end to form the base address (this automatically aligns the address on a 4-KByte
         * boundary).
         */
        uint64_t page_frame_number : 36;
#define IA32_MTRR_PHYSBASE_PAGE_FRAME_NUMBER_BIT                     12
#define IA32_MTRR_PHYSBASE_PAGE_FRAME_NUMBER_FLAG                    0xFFFFFFFFF000
#define IA32_MTRR_PHYSBASE_PAGE_FRAME_NUMBER_MASK                    0xFFFFFFFFF
#define IA32_MTRR_PHYSBASE_PAGE_FRAME_NUMBER(_)                      (((_) >> 12) & 0xFFFFFFFFF)
        uint64_t reserved2 : 16;
    };

    uint64_t flags;
} ia32_mtrr_physbase_register;

typedef union
{
    struct
    {
        uint64_t reserved1 : 11;

        /**
         * [Bit 11] Enables the register pair when set; disables register pair when clear.
         */
        uint64_t valid : 1;
#define IA32_MTRR_PHYSMASK_VALID_BIT                                 11
#define IA32_MTRR_PHYSMASK_VALID_FLAG                                0x800
#define IA32_MTRR_PHYSMASK_VALID_MASK                                0x01
#define IA32_MTRR_PHYSMASK_VALID(_)                                  (((_) >> 11) & 0x01)

        /**
         * [Bits 47:12] Specifies a mask (24 bits if the maximum physical address size is 36 bits, 28 bits if the maximum physical
         * address size is 40 bits). The mask determines the range of the region being mapped, according to the following
         * relationships:
         * - Address_Within_Range AND PhysMask = PhysBase AND PhysMask
         * - This value is extended by 12 bits at the low end to form the mask value.
         * - The width of the PhysMask field depends on the maximum physical address size supported by the processor.
         * CPUID.80000008H reports the maximum physical address size supported by the processor. If CPUID.80000008H is not
         * available, software may assume that the processor supports a 36-bit physical address size.
         *
         * @see Vol3A[11.11.3(Example Base and Mask Calculations)]
         */
        uint64_t page_frame_number : 36;
#define IA32_MTRR_PHYSMASK_PAGE_FRAME_NUMBER_BIT                     12
#define IA32_MTRR_PHYSMASK_PAGE_FRAME_NUMBER_FLAG                    0xFFFFFFFFF000
#define IA32_MTRR_PHYSMASK_PAGE_FRAME_NUMBER_MASK                    0xFFFFFFFFF
#define IA32_MTRR_PHYSMASK_PAGE_FRAME_NUMBER(_)                      (((_) >> 12) & 0xFFFFFFFFF)
        uint64_t reserved2 : 16;
    };

    uint64_t flags;
} ia32_mtrr_physmask_register;

//EPT POINTER
typedef union {
    u64 value;
    struct {
        u64 mem_cache_type : 3;
        u64 page_walk_len : 3;
        u64 enable_access_dirty : 1;
        u64 reserved1 : 5;
        u64 pfn : 36;
        u64 reserved2 : 16;
    };

} EPT_POINTER;

//EPT PML4
typedef union {
    u64 value;
    struct {
        u64 read : 1;
        u64 write : 1;
        u64 exec : 1;
        u64 reserv0 : 5;
        u64 accessed : 1;
        u64 ignore0 : 1;
        u64 um_exec : 1;
        u64 ignore1 : 1;
        u64 pfn : 40; //PDPT PFN
        u64 ignore2 : 12;
    };
    finl u64 to_phys() {
        return (u64)pfn << PAGE_SHIFT;
    }
} EPT_PML4E;

//EPT PDPT
typedef union
{
    u64 value;
    struct {
        u64 read : 1;
        u64 write : 1;
        u64 exec : 1;
        u64 mem_cache_type : 3;
        u64 ignore_pat : 1;
        u64 large_page : 1;
        u64 accessed : 1;
        u64 dirty : 1;
        u64 um_exec : 1;
        u64 ignored0 : 1;
        u64 reserved0 : 18;
        u64 pfn : 22; //1GB PAGE
        u64 ignored1 : 8;
        u64 SupervisorShadowStackAccess : 1;
        u64 ignored2 : 2;
        u64 suppress_ve : 1;
    }; 
    finl u64 to_phys() {
        return (u64)pfn << PAGE_SHIFT;
    }
} EPT_PDPTE_1GB;

typedef union {
    u64 value;
    struct
    {
        u64 read : 1;
        u64 write : 1;
        u64 exec : 1;
        u64 reserved0 : 5;
        u64 accessed : 1;
        u64 ignored0 : 1;
        u64 um_exec : 1;
        u64 ignored1 : 1;
        u64 pfn : 40; //PD PFN
        u64 ignored2 : 12;
    };
    finl u64 to_phys() {
        return (u64)pfn << PAGE_SHIFT;
    }
} EPT_PDPTE;

//EPT PDPT
typedef union {
    u64 value;
    struct {
        u64 read : 1;
        u64 write : 1;
        u64 exec : 1;
        u64 mem_cache_type : 3;
        u64 ignore_pat : 1;
        u64 large_page : 1;
        u64 accessed : 1;
        u64 dirty : 1;
        u64 um_exec : 1;
        u64 ignored0 : 1;
        u64 reserved0 : 9;
        u64 pfn : 31; //2MB PAGE
        u64 ignored1 : 8;
        u64 SupervisorShadowStackAccess : 1;
        u64 ignored2 : 2;
        u64 suppress_ve : 1;
    };
    finl u64 to_phys() {
        return (u64)pfn << PAGE_SHIFT;
    }
} EPT_PDE_2MB;

typedef union {
    u64 value;
    struct {
        u64 read : 1;
        u64 write : 1;
        u64 exec : 1;
        u64 reserved0 : 4;
        u64 large_page : 1;
        u64 accessed : 1;
        u64 ignored0 : 1;
        u64 um_exec : 1;
        u64 ignored1 : 1;
        u64 pfn : 40; //PT PFN
        u64 ignored : 12;
    };
    finl u64 to_phys() {
        return (u64)pfn << PAGE_SHIFT;
    }
} EPT_PDE;

//EPT PT
typedef union {
    u64 value;
    struct
    {
        u64 read : 1;
        u64 write : 1;
        u64 exec : 1;
        u64 mem_cache_type : 3;
        u64 ignore_pat : 1;
        u64 ignored0 : 1;
        u64 accessed : 1;
        u64 dirty : 1;
        u64 um_exec : 1;
        u64 ignored1 : 1;
        u64 pfn : 40; //4KB PAGE
        u64 ignored2 : 8;
        u64 SupervisorShadowStackAccess : 1;
        u64 SubPageWritePermissions : 1;
        u64 ignored3 : 1;
        u64 suppress_ve : 1;
    };
    finl u64 to_phys() {
        return (u64)pfn << PAGE_SHIFT;
    }
} EPT_PTE;

//
typedef struct _QUERY_EPT
{
    u32 status;
    u64 page_size;
    u16 cache_type;
    bool r, w, x, um_x;

    u64 pml4e_pa;
    u64 pdpte_pa;
    u64 pde_pa;
    u64 pte_pa;

    EPT_PML4E pml4e_val;

    union _EPDPTE {
        EPT_PDPTE_1GB page_1gb;
        EPT_PDPTE pd_link;
    } pdpte_val;

    union _EPDE
    {
        EPT_PDE_2MB page_2mb;
        EPT_PDE pt_link;
    } pde_val;

    EPT_PTE pte_val;

} QUERY_EPT, * PQUERY_EPT;

typedef struct _QUERY_PTE
{
    u64 page_size;

    PTE PML4E;
    ULONG64 pPML4E;

    PTE PDPTE;
    ULONG64 pPDPTE;

    PTE PDE;
    ULONG64 pPDE;
    
    PTE PTE;
    ULONG64 pPTE;
} QUERY_PTE, * PQUERY_PTE;

typedef union _VMX_CPU_BASED_CONTROLS
{
    ULONG32 All;
    struct
    {
        ULONG32 Reserved1 : 2;                 // [0-1]
        ULONG32 InterruptWindowExiting : 1;    // [2]
        ULONG32 UseTSCOffseting : 1;           // [3]
        ULONG32 Reserved2 : 3;                 // [4-6]
        ULONG32 HLTExiting : 1;                // [7]
        ULONG32 Reserved3 : 1;                 // [8]
        ULONG32 INVLPGExiting : 1;             // [9]
        ULONG32 MWAITExiting : 1;              // [10]
        ULONG32 RDPMCExiting : 1;              // [11]
        ULONG32 RDTSCExiting : 1;              // [12]
        ULONG32 Reserved4 : 2;                 // [13-14]
        ULONG32 CR3LoadExiting : 1;            // [15]
        ULONG32 CR3StoreExiting : 1;           // [16]
        ULONG32 Reserved5 : 2;                 // [17-18]
        ULONG32 CR8LoadExiting : 1;            // [19]
        ULONG32 CR8StoreExiting : 1;           // [20]
        ULONG32 UseTPRShadowExiting : 1;       // [21]
        ULONG32 NMIWindowExiting : 1;          // [22]
        ULONG32 MovDRExiting : 1;              // [23]
        ULONG32 UnconditionalIOExiting : 1;    // [24]
        ULONG32 UseIOBitmaps : 1;              // [25]
        ULONG32 Reserved6 : 1;                 // [26]
        ULONG32 MonitorTrapFlag : 1;           // [27]
        ULONG32 UseMSRBitmaps : 1;             // [28]
        ULONG32 MONITORExiting : 1;            // [29]
        ULONG32 PAUSEExiting : 1;              // [30]
        ULONG32 ActivateSecondaryControl : 1;
    } Fields;
} VMX_CPU_BASED_CONTROLS, * PVMX_CPU_BASED_CONTROLS;

typedef union _VIRTUAL_ADDRESS
{
    u64 Value;

    struct
    {
        u64 Offset4KB : 12;
        u64 IndexPT : 9;
        u64 IndexPD : 9;
        u64 IndexPDPT : 9;
        u64 IndexPML4 : 9;
        u64 ReservePTE : 16;
    };

    struct
    {
       u64 Offset2MB : 21;
       u64 : 43;
    };

    struct
    {
        u64 Offset1GB : 30;
        u64 : 34;
    };

} VIRTUAL_ADDRESS, * PVIRTUAL_ADDRESS;

typedef enum : u64 {
    INVEPT_SINGLE_CONTEXT = 0x00000001,
    INVEPT_ALL_CONTEXT = 0x00000002,
} INVEPT_TYPE;

typedef enum
{
    /**
     * External interrupt.
     */
    external_interrupt = 0x00000000,

    /**
     * Non-maskable interrupt (NMI).
     */
     non_maskable_interrupt = 0x00000002,

     /**
      * Hardware exception (e.g,. \#PF).
      */
      hardware_exception = 0x00000003,

      /**
       * Software interrupt (INT n).
       */
       software_interrupt = 0x00000004,

       /**
        * Privileged software exception (INT1).
        */
        privileged_software_exception = 0x00000005,

        /**
         * Software exception (INT3 or INTO).
         */
         software_exception = 0x00000006,

         /**
          * Other event. This type is used for injection of events that are not delivered through the IDT.
          */
          other_event = 0x00000007,
} interruption_type;

/**
 * @brief VM entry can be configured to conclude by delivering an event through the IDT (after all guest state and MSRs
 *        have been loaded). This process is called event injection and is controlled by these VM-entry control fields
 *
 * @see Vol3A[24.8.3(VM-Entry Controls for Event Injection)] (reference)
 */
typedef union
{
    struct
    {
        /**
         * @brief Vector of interrupt or exception
         *
         * [Bits 7:0] Determines which entry in the IDT is used or which other event is injected.
         */
        uint32_t vector : 8;
#define VMENTRY_INTERRUPT_INFORMATION_VECTOR_BIT                     0
#define VMENTRY_INTERRUPT_INFORMATION_VECTOR_FLAG                    0xFF
#define VMENTRY_INTERRUPT_INFORMATION_VECTOR_MASK                    0xFF
#define VMENTRY_INTERRUPT_INFORMATION_VECTOR(_)                      (((_) >> 0) & 0xFF)

        /**
         * @brief Interruption type
         *
         * [Bits 10:8] Determines details of how the injection is performed.
         */
        uint32_t interruption_type : 3;
#define VMENTRY_INTERRUPT_INFORMATION_INTERRUPTION_TYPE_BIT          8
#define VMENTRY_INTERRUPT_INFORMATION_INTERRUPTION_TYPE_FLAG         0x700
#define VMENTRY_INTERRUPT_INFORMATION_INTERRUPTION_TYPE_MASK         0x07
#define VMENTRY_INTERRUPT_INFORMATION_INTERRUPTION_TYPE(_)           (((_) >> 8) & 0x07)

        /**
         * @brief Deliver error code (0 = do not deliver; 1 = deliver)
         *
         * [Bit 11] Determines whether delivery pushes an error code on the guest stack.
         */
        uint32_t deliver_error_code : 1;
#define VMENTRY_INTERRUPT_INFORMATION_DELIVER_ERROR_CODE_BIT         11
#define VMENTRY_INTERRUPT_INFORMATION_DELIVER_ERROR_CODE_FLAG        0x800
#define VMENTRY_INTERRUPT_INFORMATION_DELIVER_ERROR_CODE_MASK        0x01
#define VMENTRY_INTERRUPT_INFORMATION_DELIVER_ERROR_CODE(_)          (((_) >> 11) & 0x01)
        uint32_t reserved1 : 19;

        /**
         * @brief Valid
         *
         * [Bit 31] VM entry injects an event if and only if the valid bit is 1. The valid bit in this field is cleared on every VM
         * exit.
         */
        uint32_t valid : 1;
#define VMENTRY_INTERRUPT_INFORMATION_VALID_BIT                      31
#define VMENTRY_INTERRUPT_INFORMATION_VALID_FLAG                     0x80000000
#define VMENTRY_INTERRUPT_INFORMATION_VALID_MASK                     0x01
#define VMENTRY_INTERRUPT_INFORMATION_VALID(_)                       (((_) >> 31) & 0x01)
    };

    uint32_t flags;
} vmentry_interrupt_information;

#define VMCS_GUEST_CS_ACCESS_RIGHTS                                  0x00004816
#define VMCS_GUEST_SS_ACCESS_RIGHTS                                  0x00004818

typedef union {
    struct {
        uint32_t type : 4;
        uint32_t descriptor_type : 1;
        uint32_t descriptor_privilege_level : 2;
        uint32_t present : 1;
        uint32_t reserved_1 : 4;
        uint32_t available_bit : 1;
        uint32_t long_mode : 1;
        uint32_t default_big : 1;
        uint32_t granularity : 1;
        uint32_t unusable : 1;
    };

    uint32_t flags;
} vmx_segment_access_rights;

struct interruptibility_state_t
{
    union
    {
        uint32_t flags;

        struct
        {
            uint32_t blocking_by_sti : 1;
            uint32_t blocking_by_mov_ss : 1;
            uint32_t blocking_by_smi : 1;
            uint32_t blocking_by_nmi : 1;
            uint32_t enclave_interruption : 1;
            uint32_t reserved : 27;
        };
    };
};

/**
 * @brief Exceptions that can occur when the instruction is executed in protected mode.
 *        Each exception is given a mnemonic that consists of a pound sign (\#) followed by two letters and an optional error code
 *        in parentheses. For example, \#GP(0) denotes a general protection exception with an error code of 0
 *
 * @see Vol2A[3.1.1.13(Protected Mode Exceptions Section)] (reference)
 * @see Vol3A[6.3.1(External Interrupts)] (reference)
 */
typedef enum
{
    /**
     * #DE - Divide Error.
     * Source: DIV and IDIV instructions.
     * Error Code: No.
     */
    divide_error = 0x00000000,

    /**
     * #DB - Debug.
     * Source: Any code or data reference.
     * Error Code: No.
     */
     debug = 0x00000001,

     /**
      * Nonmaskable Interrupt.
      * Source: Generated externally by asserting the processor's NMI pin or
      *         through an NMI request set by the I/O APIC to the local APIC.
      * Error Code: No.
      */
      nmi = 0x00000002,

      /**
       * #BP - Breakpoint.
       * Source: INT3 instruction.
       * Error Code: No.
       */
       breakpoint = 0x00000003,

       /**
        * #OF - Overflow.
        * Source: INTO instruction.
        * Error Code: No.
        */
        overflow = 0x00000004,

        /**
         * #BR - BOUND Range Exceeded.
         * Source: BOUND instruction.
         * Error Code: No.
         */
         bound_range_exceeded = 0x00000005,

         /**
          * #UD - Invalid Opcode (Undefined Opcode).
          * Source: UD instruction or reserved opcode.
          * Error Code: No.
          */
          invalid_opcode = 0x00000006,

          /**
           * #NM - Device Not Available (No Math Coprocessor).
           * Source: Floating-point or WAIT/FWAIT instruction.
           * Error Code: No.
           */
           device_not_available = 0x00000007,

           /**
            * #DF - Double Fault.
            * Source: Any instruction that can generate an exception, an NMI, or an INTR.
            * Error Code: Yes (zero).
            */
            double_fault = 0x00000008,

            /**
             * #\## - Coprocessor Segment Overrun (reserved).
             * Source: Floating-point instruction.
             * Error Code: No.
             *
             * @note Processors after the Intel386 processor do not generate this exception.
             */
             coprocessor_segment_overrun = 0x00000009,

             /**
              * #TS - Invalid TSS.
              * Source: Task switch or TSS access.
              * Error Code: Yes.
              */
              invalid_tss = 0x0000000A,

              /**
               * #NP - Segment Not Present.
               * Source: Loading segment registers or accessing system segments.
               * Error Code: Yes.
               */
               segment_not_present = 0x0000000B,

               /**
                * #SS - Stack Segment Fault.
                * Source: Stack operations and SS register loads.
                * Error Code: Yes.
                */
                stack_segment_fault = 0x0000000C,

                /**
                 * #GP - General Protection.
                 * Source: Any memory reference and other protection checks.
                 * Error Code: Yes.
                 */
                 general_protection = 0x0000000D,

                 /**
                  * #PF - Page Fault.
                  * Source: Any memory reference.
                  * Error Code: Yes.
                  */
                  page_fault = 0x0000000E,

                  /**
                   * #MF - Floating-Point Error (Math Fault).
                   * Source: Floating-point or WAIT/FWAIT instruction.
                   * Error Code: No.
                   */
                   x87_floating_point_error = 0x00000010,

                   /**
                    * #AC - Alignment Check.
                    * Source: Any data reference in memory.
                    * Error Code: Yes.
                    */
                    alignment_check = 0x00000011,

                    /**
                     * #MC - Machine Check.
                     * Source: Model dependent machine check errors.
                     * Error Code: No.
                     */
                     machine_check = 0x00000012,

                     /**
                      * #XM - SIMD Floating-Point Numeric Error.
                      * Source: SSE/SSE2/SSE3 floating-point instructions.
                      * Error Code: No.
                      */
                      simd_floating_point_error = 0x00000013,

                      /**
                       * #VE - Virtualization Exception.
                       * Source: EPT violations.
                       * Error Code: No.
                       */
                       virtualization_exception = 0x00000014,

                       /**
                        * #CP - Control Protection Exception.
                        * Source: Control flow transfer attempt violated the control flow enforcement technology constraints.
                        * Error Code: Yes.
                        */
                        control_protection = 0x00000015,
} exception_vector;

typedef union {
    struct {
        uint32_t present : 1;
        uint32_t write : 1;
        uint32_t user_mode_access : 1;
        uint32_t reserved_bit_violation : 1;
        uint32_t execute : 1;
        uint32_t protection_key_violation : 1;
        uint32_t shadow_stack : 1;
        uint32_t hlat : 1;
        uint32_t reserved_1 : 7;
        uint32_t sgx : 1;
    };

    uint32_t flags;
} page_fault_exception;

typedef struct _VM_CONTEXT
{
    ULONG64 Rax;
    ULONG64 Rcx;
    ULONG64 Rdx;
    ULONG64 Rbx;
    ULONG64 Rsp;
    ULONG64 Rbp;
    ULONG64 Rsi;
    ULONG64 Rdi;
    ULONG64 R8;
    ULONG64 R9;
    ULONG64 R10;
    ULONG64 R11;
    ULONG64 R12;
    ULONG64 R13;
    ULONG64 R14;
    ULONG64 R15;

    M128A XMM0;
    M128A XMM1;
    M128A XMM2;
    M128A XMM3;
    M128A XMM4;
    M128A XMM5;
    M128A XMM6;
    M128A XMM7;
    M128A XMM8;
    M128A XMM9;
    M128A XMM10;
    M128A XMM11;
    M128A XMM12;
    M128A XMM13;
    M128A XMM14;
    M128A XMM15;
} VM_CONTEXT, * PVM_CONTEXT;

struct ept_tables_state {
    alloc_page_desc new_pml4;
    alloc_page_desc new_pml3;
    alloc_page_desc new_pml2;
    alloc_page_desc new_pml1;
    EPT_POINTER ept_old_ptr;
};

