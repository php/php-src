/*
 *    Stack-less Just-In-Time compiler
 *
 *    Copyright 2009-2012 Zoltan Herczeg (hzmester@freemail.hu). All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this list of
 *      conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice, this list
 *      of conditions and the following disclaimer in the documentation and/or other materials
 *      provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDER(S) OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Latest MIPS architecture. */
/* Automatically detect SLJIT_MIPS_32_64 */

SLJIT_API_FUNC_ATTRIBUTE SLJIT_CONST char* sljit_get_platform_name(void)
{
#if (defined SLJIT_MIPS_32_64 && SLJIT_MIPS_32_64)
	return "MIPS V" SLJIT_CPUINFO;
#else
	return "MIPS III" SLJIT_CPUINFO;
#endif
}

/* Length of an instruction word
   Both for mips-32 and mips-64 */
typedef sljit_ui sljit_ins;

#define TMP_REG1	(SLJIT_NO_REGISTERS + 1)
#define TMP_REG2	(SLJIT_NO_REGISTERS + 2)
#define TMP_REG3	(SLJIT_NO_REGISTERS + 3)

/* For position independent code, t9 must contain the function address. */
#define PIC_ADDR_REG	TMP_REG2

/* TMP_EREGs are used mainly for arithmetic operations. */
#define TMP_EREG1	15
#define TMP_EREG2	24
/* Floating point status register. */
#define FCSR_REG	31
/* Return address register. */
#define RETURN_ADDR_REG	31

/* Flags are keept in volatile registers. */
#define EQUAL_FLAG	7
/* And carry flag as well. */
#define ULESS_FLAG	10
#define UGREATER_FLAG	11
#define LESS_FLAG	12
#define GREATER_FLAG	13
#define OVERFLOW_FLAG	14

#define TMP_FREG1	(0)
#define TMP_FREG2	((SLJIT_FLOAT_REG6 + 1) << 1)

static SLJIT_CONST sljit_ub reg_map[SLJIT_NO_REGISTERS + 4] = {
	0, 2, 5, 6, 3, 8, 16, 17, 18, 19, 20, 29, 4, 25, 9
};

/* --------------------------------------------------------------------- */
/*  Instrucion forms                                                     */
/* --------------------------------------------------------------------- */

#define S(s)		(reg_map[s] << 21)
#define T(t)		(reg_map[t] << 16)
#define D(d)		(reg_map[d] << 11)
/* Absolute registers. */
#define SA(s)		((s) << 21)
#define TA(t)		((t) << 16)
#define DA(d)		((d) << 11)
#define FT(t)		((t) << 16)
#define FS(s)		((s) << 11)
#define FD(d)		((d) << 6)
#define IMM(imm)	((imm) & 0xffff)
#define SH_IMM(imm)	((imm) << 6)

#define DR(dr)		(reg_map[dr])
#define HI(opcode)	((opcode) << 26)
#define LO(opcode)	(opcode)
/* S = (16 << 21) D = (17 << 21) */
#define FMT_SD		(16 << 21)

#define ABS_fmt		(HI(17) | FMT_SD | LO(5))
#define ADD_fmt		(HI(17) | FMT_SD | LO(0))
#define ADDIU		(HI(9))
#define ADDU		(HI(0) | LO(33))
#define AND		(HI(0) | LO(36))
#define ANDI		(HI(12))
#define B		(HI(4))
#define BAL		(HI(1) | (17 << 16))
#define BC1F		(HI(17) | (8 << 21))
#define BC1T		(HI(17) | (8 << 21) | (1 << 16))
#define BEQ		(HI(4))
#define BGEZ		(HI(1) | (1 << 16))
#define BGTZ		(HI(7))
#define BLEZ		(HI(6))
#define BLTZ		(HI(1) | (0 << 16))
#define BNE		(HI(5))
#define BREAK		(HI(0) | LO(13))
#define CFC1		(HI(17) | (2 << 21))
#define C_UN_fmt	(HI(17) | FMT_SD | LO(49))
#define C_UEQ_fmt	(HI(17) | FMT_SD | LO(51))
#define C_ULE_fmt	(HI(17) | FMT_SD | LO(55))
#define C_ULT_fmt	(HI(17) | FMT_SD | LO(53))
#define DADDIU		(HI(25))
#define DADDU		(HI(0) | LO(45))
#define DDIV		(HI(0) | LO(30))
#define DDIVU		(HI(0) | LO(31))
#define DIV		(HI(0) | LO(26))
#define DIVU		(HI(0) | LO(27))
#define DIV_fmt		(HI(17) | FMT_SD | LO(3))
#define DMULT		(HI(0) | LO(28))
#define DMULTU		(HI(0) | LO(29))
#define DSLL		(HI(0) | LO(56))
#define DSLL32		(HI(0) | LO(60))
#define DSLLV		(HI(0) | LO(20))
#define DSRA		(HI(0) | LO(59))
#define DSRA32		(HI(0) | LO(63))
#define DSRAV		(HI(0) | LO(23))
#define DSRL		(HI(0) | LO(58))
#define DSRL32		(HI(0) | LO(62))
#define DSRLV		(HI(0) | LO(22))
#define DSUBU		(HI(0) | LO(47))
#define J		(HI(2))
#define JAL		(HI(3))
#define JALR		(HI(0) | LO(9))
#define JR		(HI(0) | LO(8))
#define LD		(HI(55))
#define LUI		(HI(15))
#define LW		(HI(35))
#define MFHI		(HI(0) | LO(16))
#define MFLO		(HI(0) | LO(18))
#define MOV_fmt		(HI(17) | FMT_SD | LO(6))
#define MUL_fmt		(HI(17) | FMT_SD | LO(2))
#define MULT		(HI(0) | LO(24))
#define MULTU		(HI(0) | LO(25))
#define NEG_fmt		(HI(17) | FMT_SD | LO(7))
#define NOP		(HI(0) | LO(0))
#define NOR		(HI(0) | LO(39))
#define OR		(HI(0) | LO(37))
#define ORI		(HI(13))
#define SD		(HI(63))
#define SLT		(HI(0) | LO(42))
#define SLTI		(HI(10))
#define SLTIU		(HI(11))
#define SLTU		(HI(0) | LO(43))
#define SLL		(HI(0) | LO(0))
#define SLLV		(HI(0) | LO(4))
#define SRL		(HI(0) | LO(2))
#define SRLV		(HI(0) | LO(6))
#define SRA		(HI(0) | LO(3))
#define SRAV		(HI(0) | LO(7))
#define SUB_fmt		(HI(17) | FMT_SD | LO(1))
#define SUBU		(HI(0) | LO(35))
#define SW		(HI(43))
#define XOR		(HI(0) | LO(38))
#define XORI		(HI(14))

#if (defined SLJIT_MIPS_32_64 && SLJIT_MIPS_32_64)
#define CLZ		(HI(28) | LO(32))
#define DCLZ		(HI(28) | LO(36))
#define MUL		(HI(28) | LO(2))
#define SEB		(HI(31) | (16 << 6) | LO(32))
#define SEH		(HI(31) | (24 << 6) | LO(32))
#endif

#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
#define ADDU_W		ADDU
#define ADDIU_W		ADDIU
#define SLL_W		SLL
#define SUBU_W		SUBU
#else
#define ADDU_W		DADDU
#define ADDIU_W		DADDIU
#define SLL_W		DSLL
#define SUBU_W		DSUBU
#endif

#define SIMM_MAX	(0x7fff)
#define SIMM_MIN	(-0x8000)
#define UIMM_MAX	(0xffff)

/* dest_reg is the absolute name of the register
   Useful for reordering instructions in the delay slot. */
static sljit_si push_inst(struct sljit_compiler *compiler, sljit_ins ins, sljit_si delay_slot)
{
	SLJIT_ASSERT(delay_slot == MOVABLE_INS || delay_slot >= UNMOVABLE_INS
		|| delay_slot == ((ins >> 11) & 0x1f) || delay_slot == ((ins >> 16) & 0x1f));
	sljit_ins *ptr = (sljit_ins*)ensure_buf(compiler, sizeof(sljit_ins));
	FAIL_IF(!ptr);
	*ptr = ins;
	compiler->size++;
	compiler->delay_slot = delay_slot;
	return SLJIT_SUCCESS;
}

static SLJIT_INLINE sljit_ins invert_branch(sljit_si flags)
{
	return (flags & IS_BIT26_COND) ? (1 << 26) : (1 << 16);
}

static SLJIT_INLINE sljit_ins* detect_jump_type(struct sljit_jump *jump, sljit_ins *code_ptr, sljit_ins *code)
{
	sljit_sw diff;
	sljit_uw target_addr;
	sljit_ins *inst;
	sljit_ins saved_inst;

#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
	if (jump->flags & (SLJIT_REWRITABLE_JUMP | IS_CALL))
		return code_ptr;
#else
	if (jump->flags & SLJIT_REWRITABLE_JUMP)
		return code_ptr;
#endif

	if (jump->flags & JUMP_ADDR)
		target_addr = jump->u.target;
	else {
		SLJIT_ASSERT(jump->flags & JUMP_LABEL);
		target_addr = (sljit_uw)(code + jump->u.label->size);
	}
	inst = (sljit_ins*)jump->addr;
	if (jump->flags & IS_COND)
		inst--;

#if (defined SLJIT_CONFIG_MIPS_64 && SLJIT_CONFIG_MIPS_64)
	if (jump->flags & IS_CALL)
		goto keep_address;
#endif

	/* B instructions. */
	if (jump->flags & IS_MOVABLE) {
		diff = ((sljit_sw)target_addr - (sljit_sw)(inst)) >> 2;
		if (diff <= SIMM_MAX && diff >= SIMM_MIN) {
			jump->flags |= PATCH_B;

			if (!(jump->flags & IS_COND)) {
				inst[0] = inst[-1];
				inst[-1] = (jump->flags & IS_JAL) ? BAL : B;
				jump->addr -= sizeof(sljit_ins);
				return inst;
			}
			saved_inst = inst[0];
			inst[0] = inst[-1];
			inst[-1] = saved_inst ^ invert_branch(jump->flags);
			jump->addr -= 2 * sizeof(sljit_ins);
			return inst;
		}
	}
	else {
		diff = ((sljit_sw)target_addr - (sljit_sw)(inst + 1)) >> 2;
		if (diff <= SIMM_MAX && diff >= SIMM_MIN) {
			jump->flags |= PATCH_B;

			if (!(jump->flags & IS_COND)) {
				inst[0] = (jump->flags & IS_JAL) ? BAL : B;
				inst[1] = NOP;
				return inst + 1;
			}
			inst[0] = inst[0] ^ invert_branch(jump->flags);
			inst[1] = NOP;
			jump->addr -= sizeof(sljit_ins);
			return inst + 1;
		}
	}

	if (jump->flags & IS_COND) {
		if ((jump->flags & IS_MOVABLE) && (target_addr & ~0xfffffff) == ((jump->addr + 2 * sizeof(sljit_ins)) & ~0xfffffff)) {
			jump->flags |= PATCH_J;
			saved_inst = inst[0];
			inst[0] = inst[-1];
			inst[-1] = (saved_inst & 0xffff0000) | 3;
			inst[1] = J;
			inst[2] = NOP;
			return inst + 2;
		}
		else if ((target_addr & ~0xfffffff) == ((jump->addr + 3 * sizeof(sljit_ins)) & ~0xfffffff)) {
			jump->flags |= PATCH_J;
			inst[0] = (inst[0] & 0xffff0000) | 3;
			inst[1] = NOP;
			inst[2] = J;
			inst[3] = NOP;
			jump->addr += sizeof(sljit_ins);
			return inst + 3;
		}
	}
	else {
		/* J instuctions. */
		if ((jump->flags & IS_MOVABLE) && (target_addr & ~0xfffffff) == (jump->addr & ~0xfffffff)) {
			jump->flags |= PATCH_J;
			inst[0] = inst[-1];
			inst[-1] = (jump->flags & IS_JAL) ? JAL : J;
			jump->addr -= sizeof(sljit_ins);
			return inst;
		}

		if ((target_addr & ~0xfffffff) == ((jump->addr + sizeof(sljit_ins)) & ~0xfffffff)) {
			jump->flags |= PATCH_J;
			inst[0] = (jump->flags & IS_JAL) ? JAL : J;
			inst[1] = NOP;
			return inst + 1;
		}
	}

#if (defined SLJIT_CONFIG_MIPS_64 && SLJIT_CONFIG_MIPS_64)
keep_address:
	if (target_addr <= 0x7fffffff) {
		jump->flags |= PATCH_ABS32;
		if (jump->flags & IS_COND) {
			inst[0] -= 4;
			inst++;
		}
		inst[2] = inst[6];
		inst[3] = inst[7];
		return inst + 3;
	}
	if (target_addr <= 0x7fffffffffffl) {
		jump->flags |= PATCH_ABS48;
		if (jump->flags & IS_COND) {
			inst[0] -= 2;
			inst++;
		}
		inst[4] = inst[6];
		inst[5] = inst[7];
		return inst + 5;
	}
#endif

	return code_ptr;
}

#ifdef __GNUC__
static __attribute__ ((noinline)) void sljit_cache_flush(void* code, void* code_ptr)
{
	SLJIT_CACHE_FLUSH(code, code_ptr);
}
#endif

SLJIT_API_FUNC_ATTRIBUTE void* sljit_generate_code(struct sljit_compiler *compiler)
{
	struct sljit_memory_fragment *buf;
	sljit_ins *code;
	sljit_ins *code_ptr;
	sljit_ins *buf_ptr;
	sljit_ins *buf_end;
	sljit_uw word_count;
	sljit_uw addr;

	struct sljit_label *label;
	struct sljit_jump *jump;
	struct sljit_const *const_;

	CHECK_ERROR_PTR();
	check_sljit_generate_code(compiler);
	reverse_buf(compiler);

	code = (sljit_ins*)SLJIT_MALLOC_EXEC(compiler->size * sizeof(sljit_ins));
	PTR_FAIL_WITH_EXEC_IF(code);
	buf = compiler->buf;

	code_ptr = code;
	word_count = 0;
	label = compiler->labels;
	jump = compiler->jumps;
	const_ = compiler->consts;
	do {
		buf_ptr = (sljit_ins*)buf->memory;
		buf_end = buf_ptr + (buf->used_size >> 2);
		do {
			*code_ptr = *buf_ptr++;
			SLJIT_ASSERT(!label || label->size >= word_count);
			SLJIT_ASSERT(!jump || jump->addr >= word_count);
			SLJIT_ASSERT(!const_ || const_->addr >= word_count);
			/* These structures are ordered by their address. */
			if (label && label->size == word_count) {
				/* Just recording the address. */
				label->addr = (sljit_uw)code_ptr;
				label->size = code_ptr - code;
				label = label->next;
			}
			if (jump && jump->addr == word_count) {
#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
				jump->addr = (sljit_uw)(code_ptr - 3);
#else
				jump->addr = (sljit_uw)(code_ptr - 7);
#endif
				code_ptr = detect_jump_type(jump, code_ptr, code);
				jump = jump->next;
			}
			if (const_ && const_->addr == word_count) {
				/* Just recording the address. */
				const_->addr = (sljit_uw)code_ptr;
				const_ = const_->next;
			}
			code_ptr ++;
			word_count ++;
		} while (buf_ptr < buf_end);

		buf = buf->next;
	} while (buf);

	if (label && label->size == word_count) {
		label->addr = (sljit_uw)code_ptr;
		label->size = code_ptr - code;
		label = label->next;
	}

	SLJIT_ASSERT(!label);
	SLJIT_ASSERT(!jump);
	SLJIT_ASSERT(!const_);
	SLJIT_ASSERT(code_ptr - code <= (sljit_sw)compiler->size);

	jump = compiler->jumps;
	while (jump) {
		do {
			addr = (jump->flags & JUMP_LABEL) ? jump->u.label->addr : jump->u.target;
			buf_ptr = (sljit_ins*)jump->addr;

			if (jump->flags & PATCH_B) {
				addr = (sljit_sw)(addr - (jump->addr + sizeof(sljit_ins))) >> 2;
				SLJIT_ASSERT((sljit_sw)addr <= SIMM_MAX && (sljit_sw)addr >= SIMM_MIN);
				buf_ptr[0] = (buf_ptr[0] & 0xffff0000) | (addr & 0xffff);
				break;
			}
			if (jump->flags & PATCH_J) {
				SLJIT_ASSERT((addr & ~0xfffffff) == ((jump->addr + sizeof(sljit_ins)) & ~0xfffffff));
				buf_ptr[0] |= (addr >> 2) & 0x03ffffff;
				break;
			}

			/* Set the fields of immediate loads. */
#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
			buf_ptr[0] = (buf_ptr[0] & 0xffff0000) | ((addr >> 16) & 0xffff);
			buf_ptr[1] = (buf_ptr[1] & 0xffff0000) | (addr & 0xffff);
#else
			if (jump->flags & PATCH_ABS32) {
				SLJIT_ASSERT(addr <= 0x7fffffff);
				buf_ptr[0] = (buf_ptr[0] & 0xffff0000) | ((addr >> 16) & 0xffff);
				buf_ptr[1] = (buf_ptr[1] & 0xffff0000) | (addr & 0xffff);
			}
			else if (jump->flags & PATCH_ABS48) {
				SLJIT_ASSERT(addr <= 0x7fffffffffffl);
				buf_ptr[0] = (buf_ptr[0] & 0xffff0000) | ((addr >> 32) & 0xffff);
				buf_ptr[1] = (buf_ptr[1] & 0xffff0000) | ((addr >> 16) & 0xffff);
				buf_ptr[3] = (buf_ptr[3] & 0xffff0000) | (addr & 0xffff);
			}
			else {
				buf_ptr[0] = (buf_ptr[0] & 0xffff0000) | ((addr >> 48) & 0xffff);
				buf_ptr[1] = (buf_ptr[1] & 0xffff0000) | ((addr >> 32) & 0xffff);
				buf_ptr[3] = (buf_ptr[3] & 0xffff0000) | ((addr >> 16) & 0xffff);
				buf_ptr[5] = (buf_ptr[5] & 0xffff0000) | (addr & 0xffff);
			}
#endif
		} while (0);
		jump = jump->next;
	}

	compiler->error = SLJIT_ERR_COMPILED;
	compiler->executable_size = (code_ptr - code) * sizeof(sljit_ins);
#ifndef __GNUC__
	SLJIT_CACHE_FLUSH(code, code_ptr);
#else
	/* GCC workaround for invalid code generation with -O2. */
	sljit_cache_flush(code, code_ptr);
#endif
	return code;
}

/* --------------------------------------------------------------------- */
/*  Entry, exit                                                          */
/* --------------------------------------------------------------------- */

/* Creates an index in data_transfer_insts array. */
#define LOAD_DATA	0x01
#define WORD_DATA	0x00
#define BYTE_DATA	0x02
#define HALF_DATA	0x04
#define INT_DATA	0x06
#define SIGNED_DATA	0x08
/* Separates integer and floating point registers */
#define GPR_REG		0x0f
#define DOUBLE_DATA	0x10

#define MEM_MASK	0x1f

#define WRITE_BACK	0x00020
#define ARG_TEST	0x00040
#define ALT_KEEP_CACHE	0x00080
#define CUMULATIVE_OP	0x00100
#define LOGICAL_OP	0x00200
#define IMM_OP		0x00400
#define SRC2_IMM	0x00800

#define UNUSED_DEST	0x01000
#define REG_DEST	0x02000
#define REG1_SOURCE	0x04000
#define REG2_SOURCE	0x08000
#define SLOW_SRC1	0x10000
#define SLOW_SRC2	0x20000
#define SLOW_DEST	0x40000

/* Only these flags are set. UNUSED_DEST is not set when no flags should be set. */
#define CHECK_FLAGS(list) \
	(!(flags & UNUSED_DEST) || (op & GET_FLAGS(~(list))))

#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
#define STACK_STORE	SW
#define STACK_LOAD	LW
#else
#define STACK_STORE	SD
#define STACK_LOAD	LD
#endif

#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
#include "sljitNativeMIPS_32.c"
#else
#include "sljitNativeMIPS_64.c"
#endif

SLJIT_API_FUNC_ATTRIBUTE sljit_si sljit_emit_enter(struct sljit_compiler *compiler, sljit_si args, sljit_si scratches, sljit_si saveds, sljit_si local_size)
{
	sljit_ins base;

	CHECK_ERROR();
	check_sljit_emit_enter(compiler, args, scratches, saveds, local_size);

	compiler->scratches = scratches;
	compiler->saveds = saveds;
#if (defined SLJIT_DEBUG && SLJIT_DEBUG)
	compiler->logical_local_size = local_size;
#endif

	local_size += (saveds + 1 + 4) * sizeof(sljit_sw);
#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
	local_size = (local_size + 15) & ~0xf;
#else
	local_size = (local_size + 31) & ~0x1f;
#endif
	compiler->local_size = local_size;

	if (local_size <= SIMM_MAX) {
		/* Frequent case. */
		FAIL_IF(push_inst(compiler, ADDIU_W | S(SLJIT_LOCALS_REG) | T(SLJIT_LOCALS_REG) | IMM(-local_size), DR(SLJIT_LOCALS_REG)));
		base = S(SLJIT_LOCALS_REG);
	}
	else {
		FAIL_IF(load_immediate(compiler, DR(TMP_REG1), local_size));
		FAIL_IF(push_inst(compiler, ADDU_W | S(SLJIT_LOCALS_REG) | TA(0) | D(TMP_REG2), DR(TMP_REG2)));
		FAIL_IF(push_inst(compiler, SUBU_W | S(SLJIT_LOCALS_REG) | T(TMP_REG1) | D(SLJIT_LOCALS_REG), DR(SLJIT_LOCALS_REG)));
		base = S(TMP_REG2);
		local_size = 0;
	}

	FAIL_IF(push_inst(compiler, STACK_STORE | base | TA(RETURN_ADDR_REG) | IMM(local_size - 1 * (sljit_si)sizeof(sljit_sw)), MOVABLE_INS));
	if (saveds >= 1)
		FAIL_IF(push_inst(compiler, STACK_STORE | base | T(SLJIT_SAVED_REG1) | IMM(local_size - 2 * (sljit_si)sizeof(sljit_sw)), MOVABLE_INS));
	if (saveds >= 2)
		FAIL_IF(push_inst(compiler, STACK_STORE | base | T(SLJIT_SAVED_REG2) | IMM(local_size - 3 * (sljit_si)sizeof(sljit_sw)), MOVABLE_INS));
	if (saveds >= 3)
		FAIL_IF(push_inst(compiler, STACK_STORE | base | T(SLJIT_SAVED_REG3) | IMM(local_size - 4 * (sljit_si)sizeof(sljit_sw)), MOVABLE_INS));
	if (saveds >= 4)
		FAIL_IF(push_inst(compiler, STACK_STORE | base | T(SLJIT_SAVED_EREG1) | IMM(local_size - 5 * (sljit_si)sizeof(sljit_sw)), MOVABLE_INS));
	if (saveds >= 5)
		FAIL_IF(push_inst(compiler, STACK_STORE | base | T(SLJIT_SAVED_EREG2) | IMM(local_size - 6 * (sljit_si)sizeof(sljit_sw)), MOVABLE_INS));

	if (args >= 1)
		FAIL_IF(push_inst(compiler, ADDU_W | SA(4) | TA(0) | D(SLJIT_SAVED_REG1), DR(SLJIT_SAVED_REG1)));
	if (args >= 2)
		FAIL_IF(push_inst(compiler, ADDU_W | SA(5) | TA(0) | D(SLJIT_SAVED_REG2), DR(SLJIT_SAVED_REG2)));
	if (args >= 3)
		FAIL_IF(push_inst(compiler, ADDU_W | SA(6) | TA(0) | D(SLJIT_SAVED_REG3), DR(SLJIT_SAVED_REG3)));

	return SLJIT_SUCCESS;
}

SLJIT_API_FUNC_ATTRIBUTE void sljit_set_context(struct sljit_compiler *compiler, sljit_si args, sljit_si scratches, sljit_si saveds, sljit_si local_size)
{
	CHECK_ERROR_VOID();
	check_sljit_set_context(compiler, args, scratches, saveds, local_size);

	compiler->scratches = scratches;
	compiler->saveds = saveds;
#if (defined SLJIT_DEBUG && SLJIT_DEBUG)
	compiler->logical_local_size = local_size;
#endif

	local_size += (saveds + 1 + 4) * sizeof(sljit_sw);
#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
	compiler->local_size = (local_size + 15) & ~0xf;
#else
	compiler->local_size = (local_size + 31) & ~0x1f;
#endif
}

SLJIT_API_FUNC_ATTRIBUTE sljit_si sljit_emit_return(struct sljit_compiler *compiler, sljit_si op, sljit_si src, sljit_sw srcw)
{
	sljit_si local_size;
	sljit_ins base;

	CHECK_ERROR();
	check_sljit_emit_return(compiler, op, src, srcw);

	FAIL_IF(emit_mov_before_return(compiler, op, src, srcw));

	local_size = compiler->local_size;
	if (local_size <= SIMM_MAX)
		base = S(SLJIT_LOCALS_REG);
	else {
		FAIL_IF(load_immediate(compiler, DR(TMP_REG1), local_size));
		FAIL_IF(push_inst(compiler, ADDU_W | S(SLJIT_LOCALS_REG) | T(TMP_REG1) | D(TMP_REG1), DR(TMP_REG1)));
		base = S(TMP_REG1);
		local_size = 0;
	}

	FAIL_IF(push_inst(compiler, STACK_LOAD | base | TA(RETURN_ADDR_REG) | IMM(local_size - 1 * (sljit_si)sizeof(sljit_sw)), RETURN_ADDR_REG));
	if (compiler->saveds >= 5)
		FAIL_IF(push_inst(compiler, STACK_LOAD | base | T(SLJIT_SAVED_EREG2) | IMM(local_size - 6 * (sljit_si)sizeof(sljit_sw)), DR(SLJIT_SAVED_EREG2)));
	if (compiler->saveds >= 4)
		FAIL_IF(push_inst(compiler, STACK_LOAD | base | T(SLJIT_SAVED_EREG1) | IMM(local_size - 5 * (sljit_si)sizeof(sljit_sw)), DR(SLJIT_SAVED_EREG1)));
	if (compiler->saveds >= 3)
		FAIL_IF(push_inst(compiler, STACK_LOAD | base | T(SLJIT_SAVED_REG3) | IMM(local_size - 4 * (sljit_si)sizeof(sljit_sw)), DR(SLJIT_SAVED_REG3)));
	if (compiler->saveds >= 2)
		FAIL_IF(push_inst(compiler, STACK_LOAD | base | T(SLJIT_SAVED_REG2) | IMM(local_size - 3 * (sljit_si)sizeof(sljit_sw)), DR(SLJIT_SAVED_REG2)));
	if (compiler->saveds >= 1)
		FAIL_IF(push_inst(compiler, STACK_LOAD | base | T(SLJIT_SAVED_REG1) | IMM(local_size - 2 * (sljit_si)sizeof(sljit_sw)), DR(SLJIT_SAVED_REG1)));

	FAIL_IF(push_inst(compiler, JR | SA(RETURN_ADDR_REG), UNMOVABLE_INS));
	if (compiler->local_size <= SIMM_MAX)
		return push_inst(compiler, ADDIU_W | S(SLJIT_LOCALS_REG) | T(SLJIT_LOCALS_REG) | IMM(compiler->local_size), UNMOVABLE_INS);
	else
		return push_inst(compiler, ADDU_W | S(TMP_REG1) | TA(0) | D(SLJIT_LOCALS_REG), UNMOVABLE_INS);
}

#undef STACK_STORE
#undef STACK_LOAD

/* --------------------------------------------------------------------- */
/*  Operators                                                            */
/* --------------------------------------------------------------------- */

#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
#define ARCH_32_64(a, b)	a
#else
#define ARCH_32_64(a, b)	b
#endif

static SLJIT_CONST sljit_ins data_transfer_insts[16 + 4] = {
/* u w s */ ARCH_32_64(HI(43) /* sw */, HI(63) /* sd */),
/* u w l */ ARCH_32_64(HI(35) /* lw */, HI(55) /* ld */),
/* u b s */ HI(40) /* sb */,
/* u b l */ HI(36) /* lbu */,
/* u h s */ HI(41) /* sh */,
/* u h l */ HI(37) /* lhu */,
/* u i s */ HI(43) /* sw */,
/* u i l */ ARCH_32_64(HI(35) /* lw */, HI(39) /* lwu */),

/* s w s */ ARCH_32_64(HI(43) /* sw */, HI(63) /* sd */),
/* s w l */ ARCH_32_64(HI(35) /* lw */, HI(55) /* ld */),
/* s b s */ HI(40) /* sb */,
/* s b l */ HI(32) /* lb */,
/* s h s */ HI(41) /* sh */,
/* s h l */ HI(33) /* lh */,
/* s i s */ HI(43) /* sw */,
/* s i l */ HI(35) /* lw */,

/* d   s */ HI(61) /* sdc1 */,
/* d   l */ HI(53) /* ldc1 */,
/* s   s */ HI(57) /* swc1 */,
/* s   l */ HI(49) /* lwc1 */,
};

#undef ARCH_32_64

/* reg_ar is an absoulute register! */

/* Can perform an operation using at most 1 instruction. */
static sljit_si getput_arg_fast(struct sljit_compiler *compiler, sljit_si flags, sljit_si reg_ar, sljit_si arg, sljit_sw argw)
{
	SLJIT_ASSERT(arg & SLJIT_MEM);

	if ((!(flags & WRITE_BACK) || !(arg & REG_MASK)) && !(arg & OFFS_REG_MASK) && argw <= SIMM_MAX && argw >= SIMM_MIN) {
		/* Works for both absoulte and relative addresses. */
		if (SLJIT_UNLIKELY(flags & ARG_TEST))
			return 1;
		FAIL_IF(push_inst(compiler, data_transfer_insts[flags & MEM_MASK] | S(arg & REG_MASK)
			| TA(reg_ar) | IMM(argw), ((flags & MEM_MASK) <= GPR_REG && (flags & LOAD_DATA)) ? reg_ar : MOVABLE_INS));
		return -1;
	}
	return 0;
}

/* See getput_arg below.
   Note: can_cache is called only for binary operators. Those
   operators always uses word arguments without write back. */
static sljit_si can_cache(sljit_si arg, sljit_sw argw, sljit_si next_arg, sljit_sw next_argw)
{
	SLJIT_ASSERT((arg & SLJIT_MEM) && (next_arg & SLJIT_MEM));

	/* Simple operation except for updates. */
	if (arg & OFFS_REG_MASK) {
		argw &= 0x3;
		next_argw &= 0x3;
		if (argw && argw == next_argw && (arg == next_arg || (arg & OFFS_REG_MASK) == (next_arg & OFFS_REG_MASK)))
			return 1;
		return 0;
	}

	if (arg == next_arg) {
		if (((next_argw - argw) <= SIMM_MAX && (next_argw - argw) >= SIMM_MIN))
			return 1;
		return 0;
	}

	return 0;
}

/* Emit the necessary instructions. See can_cache above. */
static sljit_si getput_arg(struct sljit_compiler *compiler, sljit_si flags, sljit_si reg_ar, sljit_si arg, sljit_sw argw, sljit_si next_arg, sljit_sw next_argw)
{
	sljit_si tmp_ar, base, delay_slot;

	SLJIT_ASSERT(arg & SLJIT_MEM);
	if (!(next_arg & SLJIT_MEM)) {
		next_arg = 0;
		next_argw = 0;
	}

	if ((flags & MEM_MASK) <= GPR_REG && (flags & LOAD_DATA)) {
		tmp_ar = reg_ar;
		delay_slot = reg_ar;
	} else {
		tmp_ar = DR(TMP_REG1);
		delay_slot = MOVABLE_INS;
	}
	base = arg & REG_MASK;

	if (SLJIT_UNLIKELY(arg & OFFS_REG_MASK)) {
		argw &= 0x3;
		if ((flags & WRITE_BACK) && reg_ar == DR(base)) {
			SLJIT_ASSERT(!(flags & LOAD_DATA) && DR(TMP_REG1) != reg_ar);
			FAIL_IF(push_inst(compiler, ADDU_W | SA(reg_ar) | TA(0) | D(TMP_REG1), DR(TMP_REG1)));
			reg_ar = DR(TMP_REG1);
		}

		/* Using the cache. */
		if (argw == compiler->cache_argw) {
			if (!(flags & WRITE_BACK)) {
				if (arg == compiler->cache_arg)
					return push_inst(compiler, data_transfer_insts[flags & MEM_MASK] | S(TMP_REG3) | TA(reg_ar), delay_slot);
				if ((SLJIT_MEM | (arg & OFFS_REG_MASK)) == compiler->cache_arg) {
					if (arg == next_arg && argw == (next_argw & 0x3)) {
						compiler->cache_arg = arg;
						compiler->cache_argw = argw;
						FAIL_IF(push_inst(compiler, ADDU_W | S(base) | T(TMP_REG3) | D(TMP_REG3), DR(TMP_REG3)));
						return push_inst(compiler, data_transfer_insts[flags & MEM_MASK] | S(TMP_REG3) | TA(reg_ar), delay_slot);
					}
					FAIL_IF(push_inst(compiler, ADDU_W | S(base) | T(TMP_REG3) | DA(tmp_ar), tmp_ar));
					return push_inst(compiler, data_transfer_insts[flags & MEM_MASK] | SA(tmp_ar) | TA(reg_ar), delay_slot);
				}
			}
			else {
				if ((SLJIT_MEM | (arg & OFFS_REG_MASK)) == compiler->cache_arg) {
					FAIL_IF(push_inst(compiler, ADDU_W | S(base) | T(TMP_REG3) | D(base), DR(base)));
					return push_inst(compiler, data_transfer_insts[flags & MEM_MASK] | S(base) | TA(reg_ar), delay_slot);
				}
			}
		}

		if (SLJIT_UNLIKELY(argw)) {
			compiler->cache_arg = SLJIT_MEM | (arg & OFFS_REG_MASK);
			compiler->cache_argw = argw;
			FAIL_IF(push_inst(compiler, SLL_W | T(OFFS_REG(arg)) | D(TMP_REG3) | SH_IMM(argw), DR(TMP_REG3)));
		}

		if (!(flags & WRITE_BACK)) {
			if (arg == next_arg && argw == (next_argw & 0x3)) {
				compiler->cache_arg = arg;
				compiler->cache_argw = argw;
				FAIL_IF(push_inst(compiler, ADDU_W | S(base) | T(!argw ? OFFS_REG(arg) : TMP_REG3) | D(TMP_REG3), DR(TMP_REG3)));
				tmp_ar = DR(TMP_REG3);
			}
			else
				FAIL_IF(push_inst(compiler, ADDU_W | S(base) | T(!argw ? OFFS_REG(arg) : TMP_REG3) | DA(tmp_ar), tmp_ar));
			return push_inst(compiler, data_transfer_insts[flags & MEM_MASK] | SA(tmp_ar) | TA(reg_ar), delay_slot);
		}
		FAIL_IF(push_inst(compiler, ADDU_W | S(base) | T(!argw ? OFFS_REG(arg) : TMP_REG3) | D(base), DR(base)));
		return push_inst(compiler, data_transfer_insts[flags & MEM_MASK] | S(base) | TA(reg_ar), delay_slot);
	}

	if (SLJIT_UNLIKELY(flags & WRITE_BACK) && base) {
		/* Update only applies if a base register exists. */
		if (reg_ar == DR(base)) {
			SLJIT_ASSERT(!(flags & LOAD_DATA) && DR(TMP_REG1) != reg_ar);
			if (argw <= SIMM_MAX && argw >= SIMM_MIN) {
				FAIL_IF(push_inst(compiler, data_transfer_insts[flags & MEM_MASK] | S(base) | TA(reg_ar) | IMM(argw), MOVABLE_INS));
				if (argw)
					return push_inst(compiler, ADDIU_W | S(base) | T(base) | IMM(argw), DR(base));
				return SLJIT_SUCCESS;
			}
			FAIL_IF(push_inst(compiler, ADDU_W | SA(reg_ar) | TA(0) | D(TMP_REG1), DR(TMP_REG1)));
			reg_ar = DR(TMP_REG1);
		}

		if (argw <= SIMM_MAX && argw >= SIMM_MIN) {
			if (argw)
				FAIL_IF(push_inst(compiler, ADDIU_W | S(base) | T(base) | IMM(argw), DR(base)));
		}
		else {
			if (compiler->cache_arg == SLJIT_MEM && argw - compiler->cache_argw <= SIMM_MAX && argw - compiler->cache_argw >= SIMM_MIN) {
				if (argw != compiler->cache_argw) {
					FAIL_IF(push_inst(compiler, ADDIU_W | S(TMP_REG3) | T(TMP_REG3) | IMM(argw - compiler->cache_argw), DR(TMP_REG3)));
					compiler->cache_argw = argw;
				}
				FAIL_IF(push_inst(compiler, ADDU_W | S(base) | T(TMP_REG3) | D(base), DR(base)));
			}
			else {
				compiler->cache_arg = SLJIT_MEM;
				compiler->cache_argw = argw;
				FAIL_IF(load_immediate(compiler, DR(TMP_REG3), argw));
				FAIL_IF(push_inst(compiler, ADDU_W | S(base) | T(TMP_REG3) | D(base), DR(base)));
			}
		}
		return push_inst(compiler, data_transfer_insts[flags & MEM_MASK] | S(base) | TA(reg_ar), delay_slot);
	}

	if (compiler->cache_arg == arg && argw - compiler->cache_argw <= SIMM_MAX && argw - compiler->cache_argw >= SIMM_MIN) {
		if (argw != compiler->cache_argw) {
			FAIL_IF(push_inst(compiler, ADDIU_W | S(TMP_REG3) | T(TMP_REG3) | IMM(argw - compiler->cache_argw), DR(TMP_REG3)));
			compiler->cache_argw = argw;
		}
		return push_inst(compiler, data_transfer_insts[flags & MEM_MASK] | S(TMP_REG3) | TA(reg_ar), delay_slot);
	}

	if (compiler->cache_arg == SLJIT_MEM && argw - compiler->cache_argw <= SIMM_MAX && argw - compiler->cache_argw >= SIMM_MIN) {
		if (argw != compiler->cache_argw)
			FAIL_IF(push_inst(compiler, ADDIU_W | S(TMP_REG3) | T(TMP_REG3) | IMM(argw - compiler->cache_argw), DR(TMP_REG3)));
	}
	else {
		compiler->cache_arg = SLJIT_MEM;
		FAIL_IF(load_immediate(compiler, DR(TMP_REG3), argw));
	}
	compiler->cache_argw = argw;

	if (!base)
		return push_inst(compiler, data_transfer_insts[flags & MEM_MASK] | S(TMP_REG3) | TA(reg_ar), delay_slot);

	if (arg == next_arg && next_argw - argw <= SIMM_MAX && next_argw - argw >= SIMM_MIN) {
		compiler->cache_arg = arg;
		FAIL_IF(push_inst(compiler, ADDU_W | S(TMP_REG3) | T(base) | D(TMP_REG3), DR(TMP_REG3)));
		return push_inst(compiler, data_transfer_insts[flags & MEM_MASK] | S(TMP_REG3) | TA(reg_ar), delay_slot);
	}

	FAIL_IF(push_inst(compiler, ADDU_W | S(TMP_REG3) | T(base) | DA(tmp_ar), tmp_ar));
	return push_inst(compiler, data_transfer_insts[flags & MEM_MASK] | SA(tmp_ar) | TA(reg_ar), delay_slot);
}

static SLJIT_INLINE sljit_si emit_op_mem(struct sljit_compiler *compiler, sljit_si flags, sljit_si reg_ar, sljit_si arg, sljit_sw argw)
{
	if (getput_arg_fast(compiler, flags, reg_ar, arg, argw))
		return compiler->error;
	compiler->cache_arg = 0;
	compiler->cache_argw = 0;
	return getput_arg(compiler, flags, reg_ar, arg, argw, 0, 0);
}

static SLJIT_INLINE sljit_si emit_op_mem2(struct sljit_compiler *compiler, sljit_si flags, sljit_si reg, sljit_si arg1, sljit_sw arg1w, sljit_si arg2, sljit_sw arg2w)
{
	if (getput_arg_fast(compiler, flags, reg, arg1, arg1w))
		return compiler->error;
	return getput_arg(compiler, flags, reg, arg1, arg1w, arg2, arg2w);
}

static sljit_si emit_op(struct sljit_compiler *compiler, sljit_si op, sljit_si flags,
	sljit_si dst, sljit_sw dstw,
	sljit_si src1, sljit_sw src1w,
	sljit_si src2, sljit_sw src2w)
{
	/* arg1 goes to TMP_REG1 or src reg
	   arg2 goes to TMP_REG2, imm or src reg
	   TMP_REG3 can be used for caching
	   result goes to TMP_REG2, so put result can use TMP_REG1 and TMP_REG3. */
	sljit_si dst_r = TMP_REG2;
	sljit_si src1_r;
	sljit_sw src2_r = 0;
	sljit_si sugg_src2_r = TMP_REG2;

	if (!(flags & ALT_KEEP_CACHE)) {
		compiler->cache_arg = 0;
		compiler->cache_argw = 0;
	}

	if (SLJIT_UNLIKELY(dst == SLJIT_UNUSED)) {
		if (op >= SLJIT_MOV && op <= SLJIT_MOVU_SI && !(src2 & SLJIT_MEM))
			return SLJIT_SUCCESS;
		if (GET_FLAGS(op))
			flags |= UNUSED_DEST;
	}
	else if (FAST_IS_REG(dst)) {
		dst_r = dst;
		flags |= REG_DEST;
		if (op >= SLJIT_MOV && op <= SLJIT_MOVU_SI)
			sugg_src2_r = dst_r;
	}
	else if ((dst & SLJIT_MEM) && !getput_arg_fast(compiler, flags | ARG_TEST, DR(TMP_REG1), dst, dstw))
		flags |= SLOW_DEST;

	if (flags & IMM_OP) {
		if ((src2 & SLJIT_IMM) && src2w) {
			if ((!(flags & LOGICAL_OP) && (src2w <= SIMM_MAX && src2w >= SIMM_MIN))
				|| ((flags & LOGICAL_OP) && !(src2w & ~UIMM_MAX))) {
				flags |= SRC2_IMM;
				src2_r = src2w;
			}
		}
		if (!(flags & SRC2_IMM) && (flags & CUMULATIVE_OP) && (src1 & SLJIT_IMM) && src1w) {
			if ((!(flags & LOGICAL_OP) && (src1w <= SIMM_MAX && src1w >= SIMM_MIN))
				|| ((flags & LOGICAL_OP) && !(src1w & ~UIMM_MAX))) {
				flags |= SRC2_IMM;
				src2_r = src1w;

				/* And swap arguments. */
				src1 = src2;
				src1w = src2w;
				src2 = SLJIT_IMM;
				/* src2w = src2_r unneeded. */
			}
		}
	}

	/* Source 1. */
	if (FAST_IS_REG(src1)) {
		src1_r = src1;
		flags |= REG1_SOURCE;
	}
	else if (src1 & SLJIT_IMM) {
		if (src1w) {
			FAIL_IF(load_immediate(compiler, DR(TMP_REG1), src1w));
			src1_r = TMP_REG1;
		}
		else
			src1_r = 0;
	}
	else {
		if (getput_arg_fast(compiler, flags | LOAD_DATA, DR(TMP_REG1), src1, src1w))
			FAIL_IF(compiler->error);
		else
			flags |= SLOW_SRC1;
		src1_r = TMP_REG1;
	}

	/* Source 2. */
	if (FAST_IS_REG(src2)) {
		src2_r = src2;
		flags |= REG2_SOURCE;
		if (!(flags & REG_DEST) && op >= SLJIT_MOV && op <= SLJIT_MOVU_SI)
			dst_r = src2_r;
	}
	else if (src2 & SLJIT_IMM) {
		if (!(flags & SRC2_IMM)) {
			if (src2w) {
				FAIL_IF(load_immediate(compiler, DR(sugg_src2_r), src2w));
				src2_r = sugg_src2_r;
			}
			else {
				src2_r = 0;
				if ((op >= SLJIT_MOV && op <= SLJIT_MOVU_SI) && (dst & SLJIT_MEM))
					dst_r = 0;
			}
		}
	}
	else {
		if (getput_arg_fast(compiler, flags | LOAD_DATA, DR(sugg_src2_r), src2, src2w))
			FAIL_IF(compiler->error);
		else
			flags |= SLOW_SRC2;
		src2_r = sugg_src2_r;
	}

	if ((flags & (SLOW_SRC1 | SLOW_SRC2)) == (SLOW_SRC1 | SLOW_SRC2)) {
		SLJIT_ASSERT(src2_r == TMP_REG2);
		if (!can_cache(src1, src1w, src2, src2w) && can_cache(src1, src1w, dst, dstw)) {
			FAIL_IF(getput_arg(compiler, flags | LOAD_DATA, DR(TMP_REG2), src2, src2w, src1, src1w));
			FAIL_IF(getput_arg(compiler, flags | LOAD_DATA, DR(TMP_REG1), src1, src1w, dst, dstw));
		}
		else {
			FAIL_IF(getput_arg(compiler, flags | LOAD_DATA, DR(TMP_REG1), src1, src1w, src2, src2w));
			FAIL_IF(getput_arg(compiler, flags | LOAD_DATA, DR(TMP_REG2), src2, src2w, dst, dstw));
		}
	}
	else if (flags & SLOW_SRC1)
		FAIL_IF(getput_arg(compiler, flags | LOAD_DATA, DR(TMP_REG1), src1, src1w, dst, dstw));
	else if (flags & SLOW_SRC2)
		FAIL_IF(getput_arg(compiler, flags | LOAD_DATA, DR(sugg_src2_r), src2, src2w, dst, dstw));

	FAIL_IF(emit_single_op(compiler, op, flags, dst_r, src1_r, src2_r));

	if (dst & SLJIT_MEM) {
		if (!(flags & SLOW_DEST)) {
			getput_arg_fast(compiler, flags, DR(dst_r), dst, dstw);
			return compiler->error;
		}
		return getput_arg(compiler, flags, DR(dst_r), dst, dstw, 0, 0);
	}

	return SLJIT_SUCCESS;
}

SLJIT_API_FUNC_ATTRIBUTE sljit_si sljit_emit_op0(struct sljit_compiler *compiler, sljit_si op)
{
#if (defined SLJIT_CONFIG_MIPS_64 && SLJIT_CONFIG_MIPS_64)
	sljit_si int_op = op & SLJIT_INT_OP;
#endif

	CHECK_ERROR();
	check_sljit_emit_op0(compiler, op);

	op = GET_OPCODE(op);
	switch (op) {
	case SLJIT_BREAKPOINT:
		return push_inst(compiler, BREAK, UNMOVABLE_INS);
	case SLJIT_NOP:
		return push_inst(compiler, NOP, UNMOVABLE_INS);
	case SLJIT_UMUL:
	case SLJIT_SMUL:
#if (defined SLJIT_CONFIG_MIPS_64 && SLJIT_CONFIG_MIPS_64)
		FAIL_IF(push_inst(compiler, (op == SLJIT_UMUL ? DMULTU : DMULT) | S(SLJIT_SCRATCH_REG1) | T(SLJIT_SCRATCH_REG2), MOVABLE_INS));
#else
		FAIL_IF(push_inst(compiler, (op == SLJIT_UMUL ? MULTU : MULT) | S(SLJIT_SCRATCH_REG1) | T(SLJIT_SCRATCH_REG2), MOVABLE_INS));
#endif
		FAIL_IF(push_inst(compiler, MFLO | D(SLJIT_SCRATCH_REG1), DR(SLJIT_SCRATCH_REG1)));
		return push_inst(compiler, MFHI | D(SLJIT_SCRATCH_REG2), DR(SLJIT_SCRATCH_REG2));
	case SLJIT_UDIV:
	case SLJIT_SDIV:
#if !(defined SLJIT_MIPS_32_64 && SLJIT_MIPS_32_64)
		FAIL_IF(push_inst(compiler, NOP, UNMOVABLE_INS));
		FAIL_IF(push_inst(compiler, NOP, UNMOVABLE_INS));
#endif

#if (defined SLJIT_CONFIG_MIPS_64 && SLJIT_CONFIG_MIPS_64)
		if (int_op)
			FAIL_IF(push_inst(compiler, (op == SLJIT_UDIV ? DIVU : DIV) | S(SLJIT_SCRATCH_REG1) | T(SLJIT_SCRATCH_REG2), MOVABLE_INS));
		else
			FAIL_IF(push_inst(compiler, (op == SLJIT_UDIV ? DDIVU : DDIV) | S(SLJIT_SCRATCH_REG1) | T(SLJIT_SCRATCH_REG2), MOVABLE_INS));
#else
		FAIL_IF(push_inst(compiler, (op == SLJIT_UDIV ? DIVU : DIV) | S(SLJIT_SCRATCH_REG1) | T(SLJIT_SCRATCH_REG2), MOVABLE_INS));
#endif

		FAIL_IF(push_inst(compiler, MFLO | D(SLJIT_SCRATCH_REG1), DR(SLJIT_SCRATCH_REG1)));
		return push_inst(compiler, MFHI | D(SLJIT_SCRATCH_REG2), DR(SLJIT_SCRATCH_REG2));
	}

	return SLJIT_SUCCESS;
}

SLJIT_API_FUNC_ATTRIBUTE sljit_si sljit_emit_op1(struct sljit_compiler *compiler, sljit_si op,
	sljit_si dst, sljit_sw dstw,
	sljit_si src, sljit_sw srcw)
{
#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
#	define flags 0
#else
	sljit_si flags = 0;
#endif

	CHECK_ERROR();
	check_sljit_emit_op1(compiler, op, dst, dstw, src, srcw);
	ADJUST_LOCAL_OFFSET(dst, dstw);
	ADJUST_LOCAL_OFFSET(src, srcw);

#if (defined SLJIT_CONFIG_MIPS_64 && SLJIT_CONFIG_MIPS_64)
	if ((op & SLJIT_INT_OP) && GET_OPCODE(op) >= SLJIT_NOT) {
		flags |= INT_DATA | SIGNED_DATA;
		if (src & SLJIT_IMM)
			srcw = (sljit_si)srcw;
	}
#endif

	switch (GET_OPCODE(op)) {
	case SLJIT_MOV:
	case SLJIT_MOV_P:
		return emit_op(compiler, SLJIT_MOV, WORD_DATA, dst, dstw, TMP_REG1, 0, src, srcw);

	case SLJIT_MOV_UI:
#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
		return emit_op(compiler, SLJIT_MOV_UI, INT_DATA, dst, dstw, TMP_REG1, 0, src, srcw);
#else
		return emit_op(compiler, SLJIT_MOV_UI, INT_DATA, dst, dstw, TMP_REG1, 0, src, (src & SLJIT_IMM) ? (sljit_ui)srcw : srcw);
#endif

	case SLJIT_MOV_SI:
#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
		return emit_op(compiler, SLJIT_MOV_SI, INT_DATA | SIGNED_DATA, dst, dstw, TMP_REG1, 0, src, srcw);
#else
		return emit_op(compiler, SLJIT_MOV_SI, INT_DATA | SIGNED_DATA, dst, dstw, TMP_REG1, 0, src, (src & SLJIT_IMM) ? (sljit_si)srcw : srcw);
#endif

	case SLJIT_MOV_UB:
		return emit_op(compiler, SLJIT_MOV_UB, BYTE_DATA, dst, dstw, TMP_REG1, 0, src, (src & SLJIT_IMM) ? (sljit_ub)srcw : srcw);

	case SLJIT_MOV_SB:
		return emit_op(compiler, SLJIT_MOV_SB, BYTE_DATA | SIGNED_DATA, dst, dstw, TMP_REG1, 0, src, (src & SLJIT_IMM) ? (sljit_sb)srcw : srcw);

	case SLJIT_MOV_UH:
		return emit_op(compiler, SLJIT_MOV_UH, HALF_DATA, dst, dstw, TMP_REG1, 0, src, (src & SLJIT_IMM) ? (sljit_uh)srcw : srcw);

	case SLJIT_MOV_SH:
		return emit_op(compiler, SLJIT_MOV_SH, HALF_DATA | SIGNED_DATA, dst, dstw, TMP_REG1, 0, src, (src & SLJIT_IMM) ? (sljit_sh)srcw : srcw);

	case SLJIT_MOVU:
	case SLJIT_MOVU_P:
		return emit_op(compiler, SLJIT_MOV, WORD_DATA | WRITE_BACK, dst, dstw, TMP_REG1, 0, src, srcw);

	case SLJIT_MOVU_UI:
#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
		return emit_op(compiler, SLJIT_MOV_UI, INT_DATA | WRITE_BACK, dst, dstw, TMP_REG1, 0, src, srcw);
#else
		return emit_op(compiler, SLJIT_MOV_UI, INT_DATA | WRITE_BACK, dst, dstw, TMP_REG1, 0, src, (src & SLJIT_IMM) ? (sljit_ui)srcw : srcw);
#endif

	case SLJIT_MOVU_SI:
#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
		return emit_op(compiler, SLJIT_MOV_SI, INT_DATA | SIGNED_DATA | WRITE_BACK, dst, dstw, TMP_REG1, 0, src, srcw);
#else
		return emit_op(compiler, SLJIT_MOV_SI, INT_DATA | SIGNED_DATA | WRITE_BACK, dst, dstw, TMP_REG1, 0, src, (src & SLJIT_IMM) ? (sljit_si)srcw : srcw);
#endif

	case SLJIT_MOVU_UB:
		return emit_op(compiler, SLJIT_MOV_UB, BYTE_DATA | WRITE_BACK, dst, dstw, TMP_REG1, 0, src, (src & SLJIT_IMM) ? (sljit_ub)srcw : srcw);

	case SLJIT_MOVU_SB:
		return emit_op(compiler, SLJIT_MOV_SB, BYTE_DATA | SIGNED_DATA | WRITE_BACK, dst, dstw, TMP_REG1, 0, src, (src & SLJIT_IMM) ? (sljit_sb)srcw : srcw);

	case SLJIT_MOVU_UH:
		return emit_op(compiler, SLJIT_MOV_UH, HALF_DATA | WRITE_BACK, dst, dstw, TMP_REG1, 0, src, (src & SLJIT_IMM) ? (sljit_uh)srcw : srcw);

	case SLJIT_MOVU_SH:
		return emit_op(compiler, SLJIT_MOV_SH, HALF_DATA | SIGNED_DATA | WRITE_BACK, dst, dstw, TMP_REG1, 0, src, (src & SLJIT_IMM) ? (sljit_sh)srcw : srcw);

	case SLJIT_NOT:
		return emit_op(compiler, op, flags, dst, dstw, TMP_REG1, 0, src, srcw);

	case SLJIT_NEG:
		return emit_op(compiler, SLJIT_SUB | GET_ALL_FLAGS(op), flags | IMM_OP, dst, dstw, SLJIT_IMM, 0, src, srcw);

	case SLJIT_CLZ:
		return emit_op(compiler, op, flags, dst, dstw, TMP_REG1, 0, src, srcw);
	}

	return SLJIT_SUCCESS;

#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
#	undef flags
#endif
}

SLJIT_API_FUNC_ATTRIBUTE sljit_si sljit_emit_op2(struct sljit_compiler *compiler, sljit_si op,
	sljit_si dst, sljit_sw dstw,
	sljit_si src1, sljit_sw src1w,
	sljit_si src2, sljit_sw src2w)
{
#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
#	define flags 0
#else
	sljit_si flags = 0;
#endif

	CHECK_ERROR();
	check_sljit_emit_op2(compiler, op, dst, dstw, src1, src1w, src2, src2w);
	ADJUST_LOCAL_OFFSET(dst, dstw);
	ADJUST_LOCAL_OFFSET(src1, src1w);
	ADJUST_LOCAL_OFFSET(src2, src2w);

#if (defined SLJIT_CONFIG_MIPS_64 && SLJIT_CONFIG_MIPS_64)
	if (op & SLJIT_INT_OP) {
		flags |= INT_DATA | SIGNED_DATA;
		if (src1 & SLJIT_IMM)
			src1w = (sljit_si)src1w;
		if (src2 & SLJIT_IMM)
			src2w = (sljit_si)src2w;
	}
#endif

	switch (GET_OPCODE(op)) {
	case SLJIT_ADD:
	case SLJIT_ADDC:
		return emit_op(compiler, op, flags | CUMULATIVE_OP | IMM_OP, dst, dstw, src1, src1w, src2, src2w);

	case SLJIT_SUB:
	case SLJIT_SUBC:
		return emit_op(compiler, op, flags | IMM_OP, dst, dstw, src1, src1w, src2, src2w);

	case SLJIT_MUL:
		return emit_op(compiler, op, flags | CUMULATIVE_OP, dst, dstw, src1, src1w, src2, src2w);

	case SLJIT_AND:
	case SLJIT_OR:
	case SLJIT_XOR:
		return emit_op(compiler, op, flags | CUMULATIVE_OP | LOGICAL_OP | IMM_OP, dst, dstw, src1, src1w, src2, src2w);

	case SLJIT_SHL:
	case SLJIT_LSHR:
	case SLJIT_ASHR:
#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
		if (src2 & SLJIT_IMM)
			src2w &= 0x1f;
#else
		if (src2 & SLJIT_IMM) {
			if (op & SLJIT_INT_OP)
				src2w &= 0x1f;
			else
				src2w &= 0x3f;
		}
#endif
		return emit_op(compiler, op, flags | IMM_OP, dst, dstw, src1, src1w, src2, src2w);
	}

	return SLJIT_SUCCESS;

#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
#	undef flags
#endif
}

SLJIT_API_FUNC_ATTRIBUTE sljit_si sljit_get_register_index(sljit_si reg)
{
	check_sljit_get_register_index(reg);
	return reg_map[reg];
}

SLJIT_API_FUNC_ATTRIBUTE sljit_si sljit_get_float_register_index(sljit_si reg)
{
	check_sljit_get_float_register_index(reg);
	return reg << 1;
}

SLJIT_API_FUNC_ATTRIBUTE sljit_si sljit_emit_op_custom(struct sljit_compiler *compiler,
	void *instruction, sljit_si size)
{
	CHECK_ERROR();
	check_sljit_emit_op_custom(compiler, instruction, size);
	SLJIT_ASSERT(size == 4);

	return push_inst(compiler, *(sljit_ins*)instruction, UNMOVABLE_INS);
}

/* --------------------------------------------------------------------- */
/*  Floating point operators                                             */
/* --------------------------------------------------------------------- */

SLJIT_API_FUNC_ATTRIBUTE sljit_si sljit_is_fpu_available(void)
{
#ifdef SLJIT_IS_FPU_AVAILABLE
	return SLJIT_IS_FPU_AVAILABLE;
#elif defined(__GNUC__)
	sljit_sw fir;
	asm ("cfc1 %0, $0" : "=r"(fir));
	return (fir >> 22) & 0x1;
#else
#error "FIR check is not implemented for this architecture"
#endif
}

#define FLOAT_DATA(op) (DOUBLE_DATA | ((op & SLJIT_SINGLE_OP) >> 7))
#define FMT(op) (((op & SLJIT_SINGLE_OP) ^ SLJIT_SINGLE_OP) << (21 - 8))

SLJIT_API_FUNC_ATTRIBUTE sljit_si sljit_emit_fop1(struct sljit_compiler *compiler, sljit_si op,
	sljit_si dst, sljit_sw dstw,
	sljit_si src, sljit_sw srcw)
{
	sljit_si dst_fr;

	CHECK_ERROR();
	check_sljit_emit_fop1(compiler, op, dst, dstw, src, srcw);
	SLJIT_COMPILE_ASSERT((SLJIT_SINGLE_OP == 0x100) && !(DOUBLE_DATA & 0x2), float_transfer_bit_error);

	compiler->cache_arg = 0;
	compiler->cache_argw = 0;

	if (GET_OPCODE(op) == SLJIT_CMPD) {
		if (dst & SLJIT_MEM) {
			FAIL_IF(emit_op_mem2(compiler, FLOAT_DATA(op) | LOAD_DATA, TMP_FREG1, dst, dstw, src, srcw));
			dst = TMP_FREG1;
		}
		else
			dst <<= 1;

		if (src & SLJIT_MEM) {
			FAIL_IF(emit_op_mem2(compiler, FLOAT_DATA(op) | LOAD_DATA, TMP_FREG2, src, srcw, 0, 0));
			src = TMP_FREG2;
		}
		else
			src <<= 1;

		/* src and dst are swapped. */
		if (op & SLJIT_SET_E) {
			FAIL_IF(push_inst(compiler, C_UEQ_fmt | FMT(op) | FT(src) | FS(dst), UNMOVABLE_INS));
			FAIL_IF(push_inst(compiler, CFC1 | TA(EQUAL_FLAG) | DA(FCSR_REG), EQUAL_FLAG));
			FAIL_IF(push_inst(compiler, SRL | TA(EQUAL_FLAG) | DA(EQUAL_FLAG) | SH_IMM(23), EQUAL_FLAG));
			FAIL_IF(push_inst(compiler, ANDI | SA(EQUAL_FLAG) | TA(EQUAL_FLAG) | IMM(1), EQUAL_FLAG));
		}
		if (op & SLJIT_SET_S) {
			/* Mixing the instructions for the two checks. */
			FAIL_IF(push_inst(compiler, C_ULT_fmt | FMT(op) | FT(src) | FS(dst), UNMOVABLE_INS));
			FAIL_IF(push_inst(compiler, CFC1 | TA(ULESS_FLAG) | DA(FCSR_REG), ULESS_FLAG));
			FAIL_IF(push_inst(compiler, C_ULT_fmt | FMT(op) | FT(dst) | FS(src), UNMOVABLE_INS));
			FAIL_IF(push_inst(compiler, SRL | TA(ULESS_FLAG) | DA(ULESS_FLAG) | SH_IMM(23), ULESS_FLAG));
			FAIL_IF(push_inst(compiler, ANDI | SA(ULESS_FLAG) | TA(ULESS_FLAG) | IMM(1), ULESS_FLAG));
			FAIL_IF(push_inst(compiler, CFC1 | TA(UGREATER_FLAG) | DA(FCSR_REG), UGREATER_FLAG));
			FAIL_IF(push_inst(compiler, SRL | TA(UGREATER_FLAG) | DA(UGREATER_FLAG) | SH_IMM(23), UGREATER_FLAG));
			FAIL_IF(push_inst(compiler, ANDI | SA(UGREATER_FLAG) | TA(UGREATER_FLAG) | IMM(1), UGREATER_FLAG));
		}
		return push_inst(compiler, C_UN_fmt | FMT(op) | FT(src) | FS(dst), FCSR_FCC);
	}

	dst_fr = FAST_IS_REG(dst) ? (dst << 1) : TMP_FREG1;

	if (src & SLJIT_MEM) {
		FAIL_IF(emit_op_mem2(compiler, FLOAT_DATA(op) | LOAD_DATA, dst_fr, src, srcw, dst, dstw));
		src = dst_fr;
	}
	else
		src <<= 1;

	switch (GET_OPCODE(op)) {
		case SLJIT_MOVD:
			if (src != dst_fr && dst_fr != TMP_FREG1)
				FAIL_IF(push_inst(compiler, MOV_fmt | FMT(op) | FS(src) | FD(dst_fr), MOVABLE_INS));
			break;
		case SLJIT_NEGD:
			FAIL_IF(push_inst(compiler, NEG_fmt | FMT(op) | FS(src) | FD(dst_fr), MOVABLE_INS));
			break;
		case SLJIT_ABSD:
			FAIL_IF(push_inst(compiler, ABS_fmt | FMT(op) | FS(src) | FD(dst_fr), MOVABLE_INS));
			break;
	}

	if (dst_fr == TMP_FREG1) {
		if (GET_OPCODE(op) == SLJIT_MOVD)
			dst_fr = src;
		FAIL_IF(emit_op_mem2(compiler, FLOAT_DATA(op), dst_fr, dst, dstw, 0, 0));
	}

	return SLJIT_SUCCESS;
}

SLJIT_API_FUNC_ATTRIBUTE sljit_si sljit_emit_fop2(struct sljit_compiler *compiler, sljit_si op,
	sljit_si dst, sljit_sw dstw,
	sljit_si src1, sljit_sw src1w,
	sljit_si src2, sljit_sw src2w)
{
	sljit_si dst_fr, flags = 0;

	CHECK_ERROR();
	check_sljit_emit_fop2(compiler, op, dst, dstw, src1, src1w, src2, src2w);

	compiler->cache_arg = 0;
	compiler->cache_argw = 0;

	dst_fr = FAST_IS_REG(dst) ? (dst << 1) : TMP_FREG2;

	if (src1 & SLJIT_MEM) {
		if (getput_arg_fast(compiler, FLOAT_DATA(op) | LOAD_DATA, TMP_FREG1, src1, src1w)) {
			FAIL_IF(compiler->error);
			src1 = TMP_FREG1;
		} else
			flags |= SLOW_SRC1;
	}
	else
		src1 <<= 1;

	if (src2 & SLJIT_MEM) {
		if (getput_arg_fast(compiler, FLOAT_DATA(op) | LOAD_DATA, TMP_FREG2, src2, src2w)) {
			FAIL_IF(compiler->error);
			src2 = TMP_FREG2;
		} else
			flags |= SLOW_SRC2;
	}
	else
		src2 <<= 1;

	if ((flags & (SLOW_SRC1 | SLOW_SRC2)) == (SLOW_SRC1 | SLOW_SRC2)) {
		if (!can_cache(src1, src1w, src2, src2w) && can_cache(src1, src1w, dst, dstw)) {
			FAIL_IF(getput_arg(compiler, FLOAT_DATA(op) | LOAD_DATA, TMP_FREG2, src2, src2w, src1, src1w));
			FAIL_IF(getput_arg(compiler, FLOAT_DATA(op) | LOAD_DATA, TMP_FREG1, src1, src1w, dst, dstw));
		}
		else {
			FAIL_IF(getput_arg(compiler, FLOAT_DATA(op) | LOAD_DATA, TMP_FREG1, src1, src1w, src2, src2w));
			FAIL_IF(getput_arg(compiler, FLOAT_DATA(op) | LOAD_DATA, TMP_FREG2, src2, src2w, dst, dstw));
		}
	}
	else if (flags & SLOW_SRC1)
		FAIL_IF(getput_arg(compiler, FLOAT_DATA(op) | LOAD_DATA, TMP_FREG1, src1, src1w, dst, dstw));
	else if (flags & SLOW_SRC2)
		FAIL_IF(getput_arg(compiler, FLOAT_DATA(op) | LOAD_DATA, TMP_FREG2, src2, src2w, dst, dstw));

	if (flags & SLOW_SRC1)
		src1 = TMP_FREG1;
	if (flags & SLOW_SRC2)
		src2 = TMP_FREG2;

	switch (GET_OPCODE(op)) {
	case SLJIT_ADDD:
		FAIL_IF(push_inst(compiler, ADD_fmt | FMT(op) | FT(src2) | FS(src1) | FD(dst_fr), MOVABLE_INS));
		break;

	case SLJIT_SUBD:
		FAIL_IF(push_inst(compiler, SUB_fmt | FMT(op) | FT(src2) | FS(src1) | FD(dst_fr), MOVABLE_INS));
		break;

	case SLJIT_MULD:
		FAIL_IF(push_inst(compiler, MUL_fmt | FMT(op) | FT(src2) | FS(src1) | FD(dst_fr), MOVABLE_INS));
		break;

	case SLJIT_DIVD:
		FAIL_IF(push_inst(compiler, DIV_fmt | FMT(op) | FT(src2) | FS(src1) | FD(dst_fr), MOVABLE_INS));
		break;
	}

	if (dst_fr == TMP_FREG2)
		FAIL_IF(emit_op_mem2(compiler, FLOAT_DATA(op), TMP_FREG2, dst, dstw, 0, 0));

	return SLJIT_SUCCESS;
}

/* --------------------------------------------------------------------- */
/*  Other instructions                                                   */
/* --------------------------------------------------------------------- */

SLJIT_API_FUNC_ATTRIBUTE sljit_si sljit_emit_fast_enter(struct sljit_compiler *compiler, sljit_si dst, sljit_sw dstw)
{
	CHECK_ERROR();
	check_sljit_emit_fast_enter(compiler, dst, dstw);
	ADJUST_LOCAL_OFFSET(dst, dstw);

	/* For UNUSED dst. Uncommon, but possible. */
	if (dst == SLJIT_UNUSED)
		return SLJIT_SUCCESS;

	if (FAST_IS_REG(dst))
		return push_inst(compiler, ADDU_W | SA(RETURN_ADDR_REG) | TA(0) | D(dst), DR(dst));

	/* Memory. */
	return emit_op_mem(compiler, WORD_DATA, RETURN_ADDR_REG, dst, dstw);
}

SLJIT_API_FUNC_ATTRIBUTE sljit_si sljit_emit_fast_return(struct sljit_compiler *compiler, sljit_si src, sljit_sw srcw)
{
	CHECK_ERROR();
	check_sljit_emit_fast_return(compiler, src, srcw);
	ADJUST_LOCAL_OFFSET(src, srcw);

	if (FAST_IS_REG(src))
		FAIL_IF(push_inst(compiler, ADDU_W | S(src) | TA(0) | DA(RETURN_ADDR_REG), RETURN_ADDR_REG));
	else if (src & SLJIT_MEM)
		FAIL_IF(emit_op_mem(compiler, WORD_DATA | LOAD_DATA, RETURN_ADDR_REG, src, srcw));
	else if (src & SLJIT_IMM)
		FAIL_IF(load_immediate(compiler, RETURN_ADDR_REG, srcw));

	FAIL_IF(push_inst(compiler, JR | SA(RETURN_ADDR_REG), UNMOVABLE_INS));
	return push_inst(compiler, NOP, UNMOVABLE_INS);
}

/* --------------------------------------------------------------------- */
/*  Conditional instructions                                             */
/* --------------------------------------------------------------------- */

SLJIT_API_FUNC_ATTRIBUTE struct sljit_label* sljit_emit_label(struct sljit_compiler *compiler)
{
	struct sljit_label *label;

	CHECK_ERROR_PTR();
	check_sljit_emit_label(compiler);

	if (compiler->last_label && compiler->last_label->size == compiler->size)
		return compiler->last_label;

	label = (struct sljit_label*)ensure_abuf(compiler, sizeof(struct sljit_label));
	PTR_FAIL_IF(!label);
	set_label(label, compiler);
	compiler->delay_slot = UNMOVABLE_INS;
	return label;
}

#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
#define JUMP_LENGTH	4
#else
#define JUMP_LENGTH	8
#endif

#define BR_Z(src) \
	inst = BEQ | SA(src) | TA(0) | JUMP_LENGTH; \
	flags = IS_BIT26_COND; \
	delay_check = src;

#define BR_NZ(src) \
	inst = BNE | SA(src) | TA(0) | JUMP_LENGTH; \
	flags = IS_BIT26_COND; \
	delay_check = src;

#define BR_T() \
	inst = BC1T | JUMP_LENGTH; \
	flags = IS_BIT16_COND; \
	delay_check = FCSR_FCC;

#define BR_F() \
	inst = BC1F | JUMP_LENGTH; \
	flags = IS_BIT16_COND; \
	delay_check = FCSR_FCC;

SLJIT_API_FUNC_ATTRIBUTE struct sljit_jump* sljit_emit_jump(struct sljit_compiler *compiler, sljit_si type)
{
	struct sljit_jump *jump;
	sljit_ins inst;
	sljit_si flags = 0;
	sljit_si delay_check = UNMOVABLE_INS;

	CHECK_ERROR_PTR();
	check_sljit_emit_jump(compiler, type);

	jump = (struct sljit_jump*)ensure_abuf(compiler, sizeof(struct sljit_jump));
	PTR_FAIL_IF(!jump);
	set_jump(jump, compiler, type & SLJIT_REWRITABLE_JUMP);
	type &= 0xff;

	switch (type) {
	case SLJIT_C_EQUAL:
	case SLJIT_C_FLOAT_NOT_EQUAL:
		BR_NZ(EQUAL_FLAG);
		break;
	case SLJIT_C_NOT_EQUAL:
	case SLJIT_C_FLOAT_EQUAL:
		BR_Z(EQUAL_FLAG);
		break;
	case SLJIT_C_LESS:
	case SLJIT_C_FLOAT_LESS:
		BR_Z(ULESS_FLAG);
		break;
	case SLJIT_C_GREATER_EQUAL:
	case SLJIT_C_FLOAT_GREATER_EQUAL:
		BR_NZ(ULESS_FLAG);
		break;
	case SLJIT_C_GREATER:
	case SLJIT_C_FLOAT_GREATER:
		BR_Z(UGREATER_FLAG);
		break;
	case SLJIT_C_LESS_EQUAL:
	case SLJIT_C_FLOAT_LESS_EQUAL:
		BR_NZ(UGREATER_FLAG);
		break;
	case SLJIT_C_SIG_LESS:
		BR_Z(LESS_FLAG);
		break;
	case SLJIT_C_SIG_GREATER_EQUAL:
		BR_NZ(LESS_FLAG);
		break;
	case SLJIT_C_SIG_GREATER:
		BR_Z(GREATER_FLAG);
		break;
	case SLJIT_C_SIG_LESS_EQUAL:
		BR_NZ(GREATER_FLAG);
		break;
	case SLJIT_C_OVERFLOW:
	case SLJIT_C_MUL_OVERFLOW:
		BR_Z(OVERFLOW_FLAG);
		break;
	case SLJIT_C_NOT_OVERFLOW:
	case SLJIT_C_MUL_NOT_OVERFLOW:
		BR_NZ(OVERFLOW_FLAG);
		break;
	case SLJIT_C_FLOAT_UNORDERED:
		BR_F();
		break;
	case SLJIT_C_FLOAT_ORDERED:
		BR_T();
		break;
	default:
		/* Not conditional branch. */
		inst = 0;
		break;
	}

	jump->flags |= flags;
	if (compiler->delay_slot == MOVABLE_INS || (compiler->delay_slot != UNMOVABLE_INS && compiler->delay_slot != delay_check))
		jump->flags |= IS_MOVABLE;

	if (inst)
		PTR_FAIL_IF(push_inst(compiler, inst, UNMOVABLE_INS));

	PTR_FAIL_IF(emit_const(compiler, TMP_REG2, 0));
	if (type <= SLJIT_JUMP) {
		PTR_FAIL_IF(push_inst(compiler, JR | S(TMP_REG2), UNMOVABLE_INS));
		jump->addr = compiler->size;
		PTR_FAIL_IF(push_inst(compiler, NOP, UNMOVABLE_INS));
	} else {
		SLJIT_ASSERT(DR(PIC_ADDR_REG) == 25 && PIC_ADDR_REG == TMP_REG2);
		/* Cannot be optimized out if type is >= CALL0. */
		jump->flags |= IS_JAL | (type >= SLJIT_CALL0 ? IS_CALL : 0);
		PTR_FAIL_IF(push_inst(compiler, JALR | S(TMP_REG2) | DA(RETURN_ADDR_REG), UNMOVABLE_INS));
		jump->addr = compiler->size;
		/* A NOP if type < CALL1. */
		PTR_FAIL_IF(push_inst(compiler, ADDU_W | S(SLJIT_SCRATCH_REG1) | TA(0) | DA(4), UNMOVABLE_INS));
	}
	return jump;
}

#define RESOLVE_IMM1() \
	if (src1 & SLJIT_IMM) { \
		if (src1w) { \
			PTR_FAIL_IF(load_immediate(compiler, DR(TMP_REG1), src1w)); \
			src1 = TMP_REG1; \
		} \
		else \
			src1 = 0; \
	}

#define RESOLVE_IMM2() \
	if (src2 & SLJIT_IMM) { \
		if (src2w) { \
			PTR_FAIL_IF(load_immediate(compiler, DR(TMP_REG2), src2w)); \
			src2 = TMP_REG2; \
		} \
		else \
			src2 = 0; \
	}

SLJIT_API_FUNC_ATTRIBUTE struct sljit_jump* sljit_emit_cmp(struct sljit_compiler *compiler, sljit_si type,
	sljit_si src1, sljit_sw src1w,
	sljit_si src2, sljit_sw src2w)
{
	struct sljit_jump *jump;
	sljit_si flags;
	sljit_ins inst;

	CHECK_ERROR_PTR();
	check_sljit_emit_cmp(compiler, type, src1, src1w, src2, src2w);
	ADJUST_LOCAL_OFFSET(src1, src1w);
	ADJUST_LOCAL_OFFSET(src2, src2w);

	compiler->cache_arg = 0;
	compiler->cache_argw = 0;
	flags = ((type & SLJIT_INT_OP) ? INT_DATA : WORD_DATA) | LOAD_DATA;
	if (src1 & SLJIT_MEM) {
		PTR_FAIL_IF(emit_op_mem2(compiler, flags, DR(TMP_REG1), src1, src1w, src2, src2w));
		src1 = TMP_REG1;
	}
	if (src2 & SLJIT_MEM) {
		PTR_FAIL_IF(emit_op_mem2(compiler, flags, DR(TMP_REG2), src2, src2w, 0, 0));
		src2 = TMP_REG2;
	}

	jump = (struct sljit_jump*)ensure_abuf(compiler, sizeof(struct sljit_jump));
	PTR_FAIL_IF(!jump);
	set_jump(jump, compiler, type & SLJIT_REWRITABLE_JUMP);
	type &= 0xff;

	if (type <= SLJIT_C_NOT_EQUAL) {
		RESOLVE_IMM1();
		RESOLVE_IMM2();
		jump->flags |= IS_BIT26_COND;
		if (compiler->delay_slot == MOVABLE_INS || (compiler->delay_slot != UNMOVABLE_INS && compiler->delay_slot != DR(src1) && compiler->delay_slot != DR(src2)))
			jump->flags |= IS_MOVABLE;
		PTR_FAIL_IF(push_inst(compiler, (type == SLJIT_C_EQUAL ? BNE : BEQ) | S(src1) | T(src2) | JUMP_LENGTH, UNMOVABLE_INS));
	}
	else if (type >= SLJIT_C_SIG_LESS && (((src1 & SLJIT_IMM) && (src1w == 0)) || ((src2 & SLJIT_IMM) && (src2w == 0)))) {
		inst = NOP;
		if ((src1 & SLJIT_IMM) && (src1w == 0)) {
			RESOLVE_IMM2();
			switch (type) {
			case SLJIT_C_SIG_LESS:
				inst = BLEZ;
				jump->flags |= IS_BIT26_COND;
				break;
			case SLJIT_C_SIG_GREATER_EQUAL:
				inst = BGTZ;
				jump->flags |= IS_BIT26_COND;
				break;
			case SLJIT_C_SIG_GREATER:
				inst = BGEZ;
				jump->flags |= IS_BIT16_COND;
				break;
			case SLJIT_C_SIG_LESS_EQUAL:
				inst = BLTZ;
				jump->flags |= IS_BIT16_COND;
				break;
			}
			src1 = src2;
		}
		else {
			RESOLVE_IMM1();
			switch (type) {
			case SLJIT_C_SIG_LESS:
				inst = BGEZ;
				jump->flags |= IS_BIT16_COND;
				break;
			case SLJIT_C_SIG_GREATER_EQUAL:
				inst = BLTZ;
				jump->flags |= IS_BIT16_COND;
				break;
			case SLJIT_C_SIG_GREATER:
				inst = BLEZ;
				jump->flags |= IS_BIT26_COND;
				break;
			case SLJIT_C_SIG_LESS_EQUAL:
				inst = BGTZ;
				jump->flags |= IS_BIT26_COND;
				break;
			}
		}
		PTR_FAIL_IF(push_inst(compiler, inst | S(src1) | JUMP_LENGTH, UNMOVABLE_INS));
	}
	else {
		if (type == SLJIT_C_LESS || type == SLJIT_C_GREATER_EQUAL || type == SLJIT_C_SIG_LESS || type == SLJIT_C_SIG_GREATER_EQUAL) {
			RESOLVE_IMM1();
			if ((src2 & SLJIT_IMM) && src2w <= SIMM_MAX && src2w >= SIMM_MIN)
				PTR_FAIL_IF(push_inst(compiler, (type <= SLJIT_C_LESS_EQUAL ? SLTIU : SLTI) | S(src1) | T(TMP_REG1) | IMM(src2w), DR(TMP_REG1)));
			else {
				RESOLVE_IMM2();
				PTR_FAIL_IF(push_inst(compiler, (type <= SLJIT_C_LESS_EQUAL ? SLTU : SLT) | S(src1) | T(src2) | D(TMP_REG1), DR(TMP_REG1)));
			}
			type = (type == SLJIT_C_LESS || type == SLJIT_C_SIG_LESS) ? SLJIT_C_NOT_EQUAL : SLJIT_C_EQUAL;
		}
		else {
			RESOLVE_IMM2();
			if ((src1 & SLJIT_IMM) && src1w <= SIMM_MAX && src1w >= SIMM_MIN)
				PTR_FAIL_IF(push_inst(compiler, (type <= SLJIT_C_LESS_EQUAL ? SLTIU : SLTI) | S(src2) | T(TMP_REG1) | IMM(src1w), DR(TMP_REG1)));
			else {
				RESOLVE_IMM1();
				PTR_FAIL_IF(push_inst(compiler, (type <= SLJIT_C_LESS_EQUAL ? SLTU : SLT) | S(src2) | T(src1) | D(TMP_REG1), DR(TMP_REG1)));
			}
			type = (type == SLJIT_C_GREATER || type == SLJIT_C_SIG_GREATER) ? SLJIT_C_NOT_EQUAL : SLJIT_C_EQUAL;
		}

		jump->flags |= IS_BIT26_COND;
		PTR_FAIL_IF(push_inst(compiler, (type == SLJIT_C_EQUAL ? BNE : BEQ) | S(TMP_REG1) | TA(0) | JUMP_LENGTH, UNMOVABLE_INS));
	}

	PTR_FAIL_IF(emit_const(compiler, TMP_REG2, 0));
	PTR_FAIL_IF(push_inst(compiler, JR | S(TMP_REG2), UNMOVABLE_INS));
	jump->addr = compiler->size;
	PTR_FAIL_IF(push_inst(compiler, NOP, UNMOVABLE_INS));
	return jump;
}

#undef RESOLVE_IMM1
#undef RESOLVE_IMM2

SLJIT_API_FUNC_ATTRIBUTE struct sljit_jump* sljit_emit_fcmp(struct sljit_compiler *compiler, sljit_si type,
	sljit_si src1, sljit_sw src1w,
	sljit_si src2, sljit_sw src2w)
{
	struct sljit_jump *jump;
	sljit_ins inst;
	sljit_si if_true;

	CHECK_ERROR_PTR();
	check_sljit_emit_fcmp(compiler, type, src1, src1w, src2, src2w);

	compiler->cache_arg = 0;
	compiler->cache_argw = 0;

	if (src1 & SLJIT_MEM) {
		PTR_FAIL_IF(emit_op_mem2(compiler, FLOAT_DATA(type) | LOAD_DATA, TMP_FREG1, src1, src1w, src2, src2w));
		src1 = TMP_FREG1;
	}
	else
		src1 <<= 1;

	if (src2 & SLJIT_MEM) {
		PTR_FAIL_IF(emit_op_mem2(compiler, FLOAT_DATA(type) | LOAD_DATA, TMP_FREG2, src2, src2w, 0, 0));
		src2 = TMP_FREG2;
	}
	else
		src2 <<= 1;

	jump = (struct sljit_jump*)ensure_abuf(compiler, sizeof(struct sljit_jump));
	PTR_FAIL_IF(!jump);
	set_jump(jump, compiler, type & SLJIT_REWRITABLE_JUMP);
	jump->flags |= IS_BIT16_COND;

	switch (type & 0xff) {
	case SLJIT_C_FLOAT_EQUAL:
		inst = C_UEQ_fmt;
		if_true = 1;
		break;
	case SLJIT_C_FLOAT_NOT_EQUAL:
		inst = C_UEQ_fmt;
		if_true = 0;
		break;
	case SLJIT_C_FLOAT_LESS:
		inst = C_ULT_fmt;
		if_true = 1;
		break;
	case SLJIT_C_FLOAT_GREATER_EQUAL:
		inst = C_ULT_fmt;
		if_true = 0;
		break;
	case SLJIT_C_FLOAT_GREATER:
		inst = C_ULE_fmt;
		if_true = 0;
		break;
	case SLJIT_C_FLOAT_LESS_EQUAL:
		inst = C_ULE_fmt;
		if_true = 1;
		break;
	case SLJIT_C_FLOAT_UNORDERED:
		inst = C_UN_fmt;
		if_true = 1;
		break;
	case SLJIT_C_FLOAT_ORDERED:
	default: /* Make compilers happy. */
		inst = C_UN_fmt;
		if_true = 0;
		break;
	}

	PTR_FAIL_IF(push_inst(compiler, inst | FMT(type) | FT(src2) | FS(src1), UNMOVABLE_INS));
	/* Intentionally the other opcode. */
	PTR_FAIL_IF(push_inst(compiler, (if_true ? BC1F : BC1T) | JUMP_LENGTH, UNMOVABLE_INS));
	PTR_FAIL_IF(emit_const(compiler, TMP_REG2, 0));
	PTR_FAIL_IF(push_inst(compiler, JR | S(TMP_REG2), UNMOVABLE_INS));
	jump->addr = compiler->size;
	PTR_FAIL_IF(push_inst(compiler, NOP, UNMOVABLE_INS));
	return jump;
}

#undef JUMP_LENGTH
#undef BR_Z
#undef BR_NZ
#undef BR_T
#undef BR_F

#undef FLOAT_DATA
#undef FMT

SLJIT_API_FUNC_ATTRIBUTE sljit_si sljit_emit_ijump(struct sljit_compiler *compiler, sljit_si type, sljit_si src, sljit_sw srcw)
{
	sljit_si src_r = TMP_REG2;
	struct sljit_jump *jump = NULL;

	CHECK_ERROR();
	check_sljit_emit_ijump(compiler, type, src, srcw);
	ADJUST_LOCAL_OFFSET(src, srcw);

	if (FAST_IS_REG(src)) {
		if (DR(src) != 4)
			src_r = src;
		else
			FAIL_IF(push_inst(compiler, ADDU_W | S(src) | TA(0) | D(TMP_REG2), DR(TMP_REG2)));
	}

	if (type >= SLJIT_CALL0) {
		SLJIT_ASSERT(DR(PIC_ADDR_REG) == 25 && PIC_ADDR_REG == TMP_REG2);
		if (src & (SLJIT_IMM | SLJIT_MEM)) {
			if (src & SLJIT_IMM)
				FAIL_IF(load_immediate(compiler, DR(PIC_ADDR_REG), srcw));
			else {
				SLJIT_ASSERT(src_r == TMP_REG2 && (src & SLJIT_MEM));
				FAIL_IF(emit_op(compiler, SLJIT_MOV, WORD_DATA, TMP_REG2, 0, TMP_REG1, 0, src, srcw));
			}
			FAIL_IF(push_inst(compiler, JALR | S(PIC_ADDR_REG) | DA(RETURN_ADDR_REG), UNMOVABLE_INS));
			/* We need an extra instruction in any case. */
			return push_inst(compiler, ADDU_W | S(SLJIT_SCRATCH_REG1) | TA(0) | DA(4), UNMOVABLE_INS);
		}

		/* Register input. */
		if (type >= SLJIT_CALL1)
			FAIL_IF(push_inst(compiler, ADDU_W | S(SLJIT_SCRATCH_REG1) | TA(0) | DA(4), 4));
		FAIL_IF(push_inst(compiler, JALR | S(src_r) | DA(RETURN_ADDR_REG), UNMOVABLE_INS));
		return push_inst(compiler, ADDU_W | S(src_r) | TA(0) | D(PIC_ADDR_REG), UNMOVABLE_INS);
	}

	if (src & SLJIT_IMM) {
		jump = (struct sljit_jump*)ensure_abuf(compiler, sizeof(struct sljit_jump));
		FAIL_IF(!jump);
		set_jump(jump, compiler, JUMP_ADDR | ((type >= SLJIT_FAST_CALL) ? IS_JAL : 0));
		jump->u.target = srcw;

		if (compiler->delay_slot != UNMOVABLE_INS)
			jump->flags |= IS_MOVABLE;

		FAIL_IF(emit_const(compiler, TMP_REG2, 0));
	}
	else if (src & SLJIT_MEM)
		FAIL_IF(emit_op(compiler, SLJIT_MOV, WORD_DATA, TMP_REG2, 0, TMP_REG1, 0, src, srcw));

	FAIL_IF(push_inst(compiler, JR | S(src_r), UNMOVABLE_INS));
	if (jump)
		jump->addr = compiler->size;
	FAIL_IF(push_inst(compiler, NOP, UNMOVABLE_INS));
	return SLJIT_SUCCESS;
}

SLJIT_API_FUNC_ATTRIBUTE sljit_si sljit_emit_op_flags(struct sljit_compiler *compiler, sljit_si op,
	sljit_si dst, sljit_sw dstw,
	sljit_si src, sljit_sw srcw,
	sljit_si type)
{
	sljit_si sugg_dst_ar, dst_ar;
	sljit_si flags = GET_ALL_FLAGS(op);
#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
#	define mem_type WORD_DATA
#else
	sljit_si mem_type = (op & SLJIT_INT_OP) ? (INT_DATA | SIGNED_DATA) : WORD_DATA;
#endif

	CHECK_ERROR();
	check_sljit_emit_op_flags(compiler, op, dst, dstw, src, srcw, type);
	ADJUST_LOCAL_OFFSET(dst, dstw);

	if (dst == SLJIT_UNUSED)
		return SLJIT_SUCCESS;

	op = GET_OPCODE(op);
#if (defined SLJIT_CONFIG_MIPS_64 && SLJIT_CONFIG_MIPS_64)
	if (op == SLJIT_MOV_SI || op == SLJIT_MOV_UI)
		mem_type = INT_DATA | SIGNED_DATA;
#endif
	sugg_dst_ar = DR((op < SLJIT_ADD && FAST_IS_REG(dst)) ? dst : TMP_REG2);

	compiler->cache_arg = 0;
	compiler->cache_argw = 0;
	if (op >= SLJIT_ADD && (src & SLJIT_MEM)) {
		ADJUST_LOCAL_OFFSET(src, srcw);
		FAIL_IF(emit_op_mem2(compiler, mem_type | LOAD_DATA, DR(TMP_REG1), src, srcw, dst, dstw));
		src = TMP_REG1;
		srcw = 0;
	}

	switch (type) {
	case SLJIT_C_EQUAL:
	case SLJIT_C_NOT_EQUAL:
		FAIL_IF(push_inst(compiler, SLTIU | SA(EQUAL_FLAG) | TA(sugg_dst_ar) | IMM(1), sugg_dst_ar));
		dst_ar = sugg_dst_ar;
		break;
	case SLJIT_C_LESS:
	case SLJIT_C_GREATER_EQUAL:
	case SLJIT_C_FLOAT_LESS:
	case SLJIT_C_FLOAT_GREATER_EQUAL:
		dst_ar = ULESS_FLAG;
		break;
	case SLJIT_C_GREATER:
	case SLJIT_C_LESS_EQUAL:
	case SLJIT_C_FLOAT_GREATER:
	case SLJIT_C_FLOAT_LESS_EQUAL:
		dst_ar = UGREATER_FLAG;
		break;
	case SLJIT_C_SIG_LESS:
	case SLJIT_C_SIG_GREATER_EQUAL:
		dst_ar = LESS_FLAG;
		break;
	case SLJIT_C_SIG_GREATER:
	case SLJIT_C_SIG_LESS_EQUAL:
		dst_ar = GREATER_FLAG;
		break;
	case SLJIT_C_OVERFLOW:
	case SLJIT_C_NOT_OVERFLOW:
		dst_ar = OVERFLOW_FLAG;
		break;
	case SLJIT_C_MUL_OVERFLOW:
	case SLJIT_C_MUL_NOT_OVERFLOW:
		FAIL_IF(push_inst(compiler, SLTIU | SA(OVERFLOW_FLAG) | TA(sugg_dst_ar) | IMM(1), sugg_dst_ar));
		dst_ar = sugg_dst_ar;
		type ^= 0x1; /* Flip type bit for the XORI below. */
		break;
	case SLJIT_C_FLOAT_EQUAL:
	case SLJIT_C_FLOAT_NOT_EQUAL:
		dst_ar = EQUAL_FLAG;
		break;

	case SLJIT_C_FLOAT_UNORDERED:
	case SLJIT_C_FLOAT_ORDERED:
		FAIL_IF(push_inst(compiler, CFC1 | TA(sugg_dst_ar) | DA(FCSR_REG), sugg_dst_ar));
		FAIL_IF(push_inst(compiler, SRL | TA(sugg_dst_ar) | DA(sugg_dst_ar) | SH_IMM(23), sugg_dst_ar));
		FAIL_IF(push_inst(compiler, ANDI | SA(sugg_dst_ar) | TA(sugg_dst_ar) | IMM(1), sugg_dst_ar));
		dst_ar = sugg_dst_ar;
		break;

	default:
		SLJIT_ASSERT_STOP();
		dst_ar = sugg_dst_ar;
		break;
	}

	if (type & 0x1) {
		FAIL_IF(push_inst(compiler, XORI | SA(dst_ar) | TA(sugg_dst_ar) | IMM(1), sugg_dst_ar));
		dst_ar = sugg_dst_ar;
	}

	if (op >= SLJIT_ADD) {
		if (DR(TMP_REG2) != dst_ar)
			FAIL_IF(push_inst(compiler, ADDU_W | SA(dst_ar) | TA(0) | D(TMP_REG2), DR(TMP_REG2)));
		return emit_op(compiler, op | flags, mem_type | CUMULATIVE_OP | LOGICAL_OP | IMM_OP | ALT_KEEP_CACHE, dst, dstw, src, srcw, TMP_REG2, 0);
	}

	if (dst & SLJIT_MEM)
		return emit_op_mem(compiler, mem_type, dst_ar, dst, dstw);

	if (sugg_dst_ar != dst_ar)
		return push_inst(compiler, ADDU_W | SA(dst_ar) | TA(0) | DA(sugg_dst_ar), sugg_dst_ar);
	return SLJIT_SUCCESS;

#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
#	undef mem_type
#endif
}

SLJIT_API_FUNC_ATTRIBUTE struct sljit_const* sljit_emit_const(struct sljit_compiler *compiler, sljit_si dst, sljit_sw dstw, sljit_sw init_value)
{
	struct sljit_const *const_;
	sljit_si reg;

	CHECK_ERROR_PTR();
	check_sljit_emit_const(compiler, dst, dstw, init_value);
	ADJUST_LOCAL_OFFSET(dst, dstw);

	const_ = (struct sljit_const*)ensure_abuf(compiler, sizeof(struct sljit_const));
	PTR_FAIL_IF(!const_);
	set_const(const_, compiler);

	reg = SLOW_IS_REG(dst) ? dst : TMP_REG2;

	PTR_FAIL_IF(emit_const(compiler, reg, init_value));

	if (dst & SLJIT_MEM)
		PTR_FAIL_IF(emit_op(compiler, SLJIT_MOV, WORD_DATA, dst, dstw, TMP_REG1, 0, TMP_REG2, 0));
	return const_;
}
