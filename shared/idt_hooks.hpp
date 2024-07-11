#include <utils/shared/unwind.hpp>

bool trace_only_jmps = false;

extern "C"
{
	EXCEPTION_DISPOSITION __cdecl __C_specific_handler(
		struct _EXCEPTION_RECORD* ExceptionRecord,
		void* EstablisherFrame,
		struct _CONTEXT* ContextRecord,
		struct _DISPATCHER_CONTEXT* DispatcherContext) {
		return ExceptionContinueExecution;
	}

	//
	struct interrupt_frame
	{
		u64 rip;
		u64 cs;
		u64 rflags;
		u64 rsp;
		u64 ss;
	};

	struct interrupt_frame_errcode
	{
		u64 error_code;
		u64 rip;
		u64 cs;
		u64 rflags;
		u64 rsp;
		u64 ss;
	};

	//
	struct interrupt_frame_new
	{
		u64 rbp;
		u64 rbx;
		u64 rsi;
		u64 rdi;
		u64 r15;
		u64 r14;
		u64 r13;
		u64 r12;
		u64 r11;
		u64 r10;
		u64 r9;
		u64 r8;
		u64 rdx;
		u64 rcx;
		u64 rax;

		u64 rip;
		u64 cs;
		u64 rflags;
		u64 rsp;
		u64 ss;
	};

	struct interrupt_frame_new_errcode
	{
		u64 rbp;
		u64 rbx;
		u64 rsi;
		u64 rdi;
		u64 r15;
		u64 r14;
		u64 r13;
		u64 r12;
		u64 r11;
		u64 r10;
		u64 r9;
		u64 r8;
		u64 rdx;
		u64 rcx;
		u64 rax;

		u64 error_code;
		u64 rip;
		u64 cs;
		u64 rflags;
		u64 rsp;
		u64 ss;
	};

	//dbg help
	bool disasm_instr22(
		u64 runtime_address1,
		bool only_jmps,
		char* str_buff, int max_str_sz)
	{
		str_buff[0] = 0;

		ZydisDecoder decoder;
		ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);

		ZydisFormatter formatter;
		ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

		ZyanU64 runtime_address = (ZyanU64)runtime_address1;

		const ZyanUSize length = ZYDIS_MAX_INSTRUCTION_LENGTH;
		ZydisDecodedInstruction instruction;
		ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT_VISIBLE];
		if (ZYAN_SUCCESS(ZydisDecoderDecodeFull(&decoder, (pv)runtime_address1, length,
			&instruction, operands, ZYDIS_MAX_OPERAND_COUNT_VISIBLE,
			ZYDIS_DFLAG_VISIBLE_OPERANDS_ONLY))) {
			ZydisFormatterFormatInstruction(&formatter, &instruction, operands,
				instruction.operand_count_visible, str_buff, max_str_sz, runtime_address, ZYAN_NULL);
		
			auto str = (fstring<char>*)str_buff;
			if (only_jmps) 
			{
				if (!str->contain("j") &&
					!str->contain("call") &&
					!str->contain("ret")) {
					str_buff[0] = 0;
					return false;
				}
			}

			return true;
		}

		return false;
	}

	void print_regs(interrupt_frame_new_errcode* ctx)
	{
		fmtp("Rbp -> %p", ctx->rbp);
		fmtp("Rbx -> %p", ctx->rbx);
		fmtp("Rsi -> %p", ctx->rsi);
		fmtp("Rdi -> %p", ctx->rdi);
		fmtp("R15 -> %p", ctx->r15);
		fmtp("R14 -> %p", ctx->r14);
		fmtp("R13 -> %p", ctx->r13);
		fmtp("R12 -> %p", ctx->r12);
		fmtp("R11 -> %p", ctx->r11);
		fmtp("R10 -> %p", ctx->r10);
		fmtp("R9 -> %p", ctx->r9);
		fmtp("R8 -> %p", ctx->r8);
		fmtp("Rdx -> %p", ctx->rdx);
		fmtp("Rcx -> %p", ctx->rcx);
		fmtp("Rax -> %p", ctx->rax);
	}

	void print_rip(interrupt_frame_new_errcode* ctx, u64 rip)
	{
		const auto [hv_base, hv_sz] = mapper_ctx_hv.hv_base_mod;
		const auto [hv_payload_base, hv_payload_sz] = mapper_ctx_hv.hv_payload_mod;

		if ((rip >= hv_base) && (rip < hv_base + hv_sz)) 
		{
			const u64 mod_off = rip - hv_base;

			fstring<char> instr;
			if (disasm_instr22(rip, trace_only_jmps, instr.data(), 64)) 
			{
				fmtp("Cur RIP -> hv.exe + %p, i: %s", mod_off, instr);
				print_regs(ctx);
			}
		}

		else if ((rip >= hv_payload_base) && (rip < hv_payload_base + hv_payload_sz))
		{
			const u64 mod_off = rip - hv_payload_base;
			
			fstring<char> instr;
			if (disasm_instr22(rip, trace_only_jmps, instr.data(), 64))
			{
				fmtp("Cur RIP -> hv_payload.dll + %p, i: %s", mod_off, instr);
				print_regs(ctx);
			}
		}
	}
	
	//handlers
	void handle_bp_internal(interrupt_frame_new_errcode* ctx)
	{
		com_init();

		fmtp("\n\rBT DT BDOS!");

		//Display RIP
		print_rip(ctx, ctx->rip);
		
		inf_loop();
		return;

		

		__fastfail(1);
	}

	void handle_pf_internal(interrupt_frame_new_errcode* ctx)
	{
		com_init();
		fmtp("\n\rPF BDOS!");

		//CR2 -> Linear Fail Address
		fmtp("Fail VA -> %p", _readcr2());

		//Display RIP
		print_rip(ctx, ctx->rip);

		inf_loop();
		return;

		__fastfail(1);
	}

	//wrapper
#ifdef DEBUG
	__declspec(dllexport) naked bool handle_bp()
	{
		__asm {
			sub rsp, 8

			push rax
			push rcx
			push rdx
			push r8
			push r9
			push r10
			push r11
			push r12
			push r13
			push r14
			push r15
			push rdi
			push rsi
			push rbx
			push rbp

			cld
			sub rsp, 0x28
			lea rcx, [rsp + 0x28]
			call handle_bp_internal
			add rsp, 0x28

			pop rbp
			pop rbx
			pop rsi
			pop rdi
			pop r15
			pop r14
			pop r13
			pop r12
			pop r11
			pop r10
			pop r9
			pop r8
			pop rdx
			pop rcx
			pop rax

			//skip err code
			add rsp, 8

			iretq
		}
	}

	__declspec(dllexport) naked bool handle_pf()
	{
		__asm {
			push rax
			push rcx
			push rdx
			push r8
			push r9
			push r10
			push r11
			push r12
			push r13
			push r14
			push r15
			push rdi
			push rsi
			push rbx
			push rbp

			cld
			sub rsp, 0x28
			lea rcx, [rsp + 0x28]
			call handle_pf_internal
			add rsp, 0x28

			pop rbp
			pop rbx
			pop rsi
			pop rdi
			pop r15
			pop r14
			pop r13
			pop r12
			pop r11
			pop r10
			pop r9
			pop r8
			pop rdx
			pop rcx
			pop rax

			//skip err code
			add rsp, 8

			iretq
		}
	}
#endif // DEBUG
}