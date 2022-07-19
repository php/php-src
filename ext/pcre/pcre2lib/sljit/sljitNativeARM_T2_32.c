/*
 *    Stack-less Just-In-Time compiler
 *
 *    Copyright Zoltan Herczeg (hzmester@freemail.hu). All rights reserved.
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

SLJIT_API_FUNC_ATTRIBUTE const char* sljit_get_platform_name(void)
{
#ifdef __SOFTFP__
	return "ARM-Thumb2" SLJIT_CPUINFO " ABI:softfp";
#else
	return "ARM-Thumb2" SLJIT_CPUINFO " ABI:hardfp";
#endif
}

/* Length of an instruction word. */
typedef sljit_u32 sljit_ins;

/* Last register + 1. */
#define TMP_REG1	(SLJIT_NUMBER_OF_REGISTERS + 2)
#define TMP_REG2	(SLJIT_NUMBER_OF_REGISTERS + 3)
#define TMP_PC		(SLJIT_NUMBER_OF_REGISTERS + 4)

#define TMP_FREG1	(SLJIT_NUMBER_OF_FLOAT_REGISTERS + 1)
#define TMP_FREG2	(SLJIT_NUMBER_OF_FLOAT_REGISTERS + 2)

/* See sljit_emit_enter and sljit_emit_op0 if you want to change them. */
static const sljit_u8 reg_map[SLJIT_NUMBER_OF_REGISTERS + 5] = {
	0, 0, 1, 2, 3, 11, 10, 9, 8, 7, 6, 5, 4, 13, 12, 14, 15
};

static const sljit_u8 freg_map[SLJIT_NUMBER_OF_FLOAT_REGISTERS + 3] = {
	0, 0, 1, 2, 3, 4, 5, 15, 14, 13, 12, 11, 10, 9, 8, 6, 7
};

#define COPY_BITS(src, from, to, bits) \
	((from >= to ? ((sljit_ins)(src) >> (from - to)) : ((sljit_ins)(src) << (to - from))) & (((1 << bits) - 1) << to))

#define NEGATE(uimm) ((sljit_uw)-(sljit_sw)(uimm))

/* Thumb16 encodings. */
#define RD3(rd) ((sljit_ins)reg_map[rd])
#define RN3(rn) ((sljit_ins)reg_map[rn] << 3)
#define RM3(rm) ((sljit_ins)reg_map[rm] << 6)
#define RDN3(rdn) ((sljit_ins)reg_map[rdn] << 8)
#define IMM3(imm) ((sljit_ins)imm << 6)
#define IMM8(imm) ((sljit_ins)imm)

/* Thumb16 helpers. */
#define SET_REGS44(rd, rn) \
	(((sljit_ins)reg_map[rn] << 3) | ((sljit_ins)reg_map[rd] & 0x7) | (((sljit_ins)reg_map[rd] & 0x8) << 4))
#define IS_2_LO_REGS(reg1, reg2) \
	(reg_map[reg1] <= 7 && reg_map[reg2] <= 7)
#define IS_3_LO_REGS(reg1, reg2, reg3) \
	(reg_map[reg1] <= 7 && reg_map[reg2] <= 7 && reg_map[reg3] <= 7)

/* Thumb32 encodings. */
#define RD4(rd) ((sljit_ins)reg_map[rd] << 8)
#define RN4(rn) ((sljit_ins)reg_map[rn] << 16)
#define RM4(rm) ((sljit_ins)reg_map[rm])
#define RT4(rt) ((sljit_ins)reg_map[rt] << 12)
#define DD4(dd) ((sljit_ins)freg_map[dd] << 12)
#define DN4(dn) ((sljit_ins)freg_map[dn] << 16)
#define DM4(dm) ((sljit_ins)freg_map[dm])
#define IMM5(imm) \
	(COPY_BITS(imm, 2, 12, 3) | (((sljit_ins)imm & 0x3) << 6))
#define IMM12(imm) \
	(COPY_BITS(imm, 11, 26, 1) | COPY_BITS(imm, 8, 12, 3) | ((sljit_ins)imm & 0xff))

/* --------------------------------------------------------------------- */
/*  Instrucion forms                                                     */
/* --------------------------------------------------------------------- */

/* dot '.' changed to _
   I immediate form (possibly followed by number of immediate bits). */
#define ADCI		0xf1400000
#define ADCS		0x4140
#define ADC_W		0xeb400000
#define ADD		0x4400
#define ADDS		0x1800
#define ADDSI3		0x1c00
#define ADDSI8		0x3000
#define ADD_W		0xeb000000
#define ADDWI		0xf2000000
#define ADD_SP		0x4485
#define ADD_SP_I	0xb000
#define ADD_W		0xeb000000
#define ADD_WI		0xf1000000
#define ANDI		0xf0000000
#define ANDS		0x4000
#define AND_W		0xea000000
#define ASRS		0x4100
#define ASRSI		0x1000
#define ASR_W		0xfa40f000
#define ASR_WI		0xea4f0020
#define BCC		0xd000
#define BICI		0xf0200000
#define BKPT		0xbe00
#define BLX		0x4780
#define BX		0x4700
#define CLZ		0xfab0f080
#define CMNI_W		0xf1100f00
#define CMP		0x4280
#define CMPI		0x2800
#define CMPI_W		0xf1b00f00
#define CMP_X		0x4500
#define CMP_W		0xebb00f00
#define EORI		0xf0800000
#define EORS		0x4040
#define EOR_W		0xea800000
#define IT		0xbf00
#define LDR_SP		0x9800
#define LDR		0xf8d00000
#define LDRI		0xf8500800
#define LSLS		0x4080
#define LSLSI		0x0000
#define LSL_W		0xfa00f000
#define LSL_WI		0xea4f0000
#define LSRS		0x40c0
#define LSRSI		0x0800
#define LSR_W		0xfa20f000
#define LSR_WI		0xea4f0010
#define MOV		0x4600
#define MOVS		0x0000
#define MOVSI		0x2000
#define MOVT		0xf2c00000
#define MOVW		0xf2400000
#define MOV_W		0xea4f0000
#define MOV_WI		0xf04f0000
#define MUL		0xfb00f000
#define MVNS		0x43c0
#define MVN_W		0xea6f0000
#define MVN_WI		0xf06f0000
#define NOP		0xbf00
#define ORNI		0xf0600000
#define ORRI		0xf0400000
#define ORRS		0x4300
#define ORR_W		0xea400000
#define POP		0xbc00
#define POP_W		0xe8bd0000
#define PUSH		0xb400
#define PUSH_W		0xe92d0000
#define RSB_WI		0xf1c00000
#define RSBSI		0x4240
#define SBCI		0xf1600000
#define SBCS		0x4180
#define SBC_W		0xeb600000
#define SDIV		0xfb90f0f0
#define SMULL		0xfb800000
#define STR_SP		0x9000
#define SUBS		0x1a00
#define SUBSI3		0x1e00
#define SUBSI8		0x3800
#define SUB_W		0xeba00000
#define SUBWI		0xf2a00000
#define SUB_SP_I	0xb080
#define SUB_WI		0xf1a00000
#define SXTB		0xb240
#define SXTB_W		0xfa4ff080
#define SXTH		0xb200
#define SXTH_W		0xfa0ff080
#define TST		0x4200
#define TSTI		0xf0000f00
#define TST_W		0xea000f00
#define UDIV		0xfbb0f0f0
#define UMULL		0xfba00000
#define UXTB		0xb2c0
#define UXTB_W		0xfa5ff080
#define UXTH		0xb280
#define UXTH_W		0xfa1ff080
#define VABS_F32	0xeeb00ac0
#define VADD_F32	0xee300a00
#define VCMP_F32	0xeeb40a40
#define VCVT_F32_S32	0xeeb80ac0
#define VCVT_F64_F32	0xeeb70ac0
#define VCVT_S32_F32	0xeebd0ac0
#define VDIV_F32	0xee800a00
#define VLDR_F32	0xed100a00
#define VMOV_F32	0xeeb00a40
#define VMOV		0xee000a10
#define VMOV2		0xec400a10
#define VMRS		0xeef1fa10
#define VMUL_F32	0xee200a00
#define VNEG_F32	0xeeb10a40
#define VPOP		0xecbd0b00
#define VPUSH		0xed2d0b00
#define VSTR_F32	0xed000a00
#define VSUB_F32	0xee300a40

static sljit_s32 push_inst16(struct sljit_compiler *compiler, sljit_ins inst)
{
	sljit_u16 *ptr;
	SLJIT_ASSERT(!(inst & 0xffff0000));

	ptr = (sljit_u16*)ensure_buf(compiler, sizeof(sljit_u16));
	FAIL_IF(!ptr);
	*ptr = (sljit_u16)(inst);
	compiler->size++;
	return SLJIT_SUCCESS;
}

static sljit_s32 push_inst32(struct sljit_compiler *compiler, sljit_ins inst)
{
	sljit_u16 *ptr = (sljit_u16*)ensure_buf(compiler, sizeof(sljit_ins));
	FAIL_IF(!ptr);
	*ptr++ = (sljit_u16)(inst >> 16);
	*ptr = (sljit_u16)(inst);
	compiler->size += 2;
	return SLJIT_SUCCESS;
}

static SLJIT_INLINE sljit_s32 emit_imm32_const(struct sljit_compiler *compiler, sljit_s32 dst, sljit_uw imm)
{
	FAIL_IF(push_inst32(compiler, MOVW | RD4(dst)
		| COPY_BITS(imm, 12, 16, 4) | COPY_BITS(imm, 11, 26, 1) | COPY_BITS(imm, 8, 12, 3) | (imm & 0xff)));
	return push_inst32(compiler, MOVT | RD4(dst)
		| COPY_BITS(imm, 12 + 16, 16, 4) | COPY_BITS(imm, 11 + 16, 26, 1) | COPY_BITS(imm, 8 + 16, 12, 3) | ((imm & 0xff0000) >> 16));
}

static SLJIT_INLINE void modify_imm32_const(sljit_u16 *inst, sljit_uw new_imm)
{
	sljit_ins dst = inst[1] & 0x0f00;
	SLJIT_ASSERT(((inst[0] & 0xfbf0) == (MOVW >> 16)) && ((inst[2] & 0xfbf0) == (MOVT >> 16)) && dst == (inst[3] & 0x0f00));
	inst[0] = (sljit_u16)((MOVW >> 16) | COPY_BITS(new_imm, 12, 0, 4) | COPY_BITS(new_imm, 11, 10, 1));
	inst[1] = (sljit_u16)(dst | COPY_BITS(new_imm, 8, 12, 3) | (new_imm & 0xff));
	inst[2] = (sljit_u16)((MOVT >> 16) | COPY_BITS(new_imm, 12 + 16, 0, 4) | COPY_BITS(new_imm, 11 + 16, 10, 1));
	inst[3] = (sljit_u16)(dst | COPY_BITS(new_imm, 8 + 16, 12, 3) | ((new_imm & 0xff0000) >> 16));
}

static SLJIT_INLINE sljit_s32 detect_jump_type(struct sljit_jump *jump, sljit_u16 *code_ptr, sljit_u16 *code, sljit_sw executable_offset)
{
	sljit_sw diff;

	if (jump->flags & SLJIT_REWRITABLE_JUMP)
		return 0;

	if (jump->flags & JUMP_ADDR) {
		/* Branch to ARM code is not optimized yet. */
		if (!(jump->u.target & 0x1))
			return 0;
		diff = ((sljit_sw)jump->u.target - (sljit_sw)(code_ptr + 2) - executable_offset) >> 1;
	}
	else {
		SLJIT_ASSERT(jump->flags & JUMP_LABEL);
		diff = ((sljit_sw)(code + jump->u.label->size) - (sljit_sw)(code_ptr + 2)) >> 1;
	}

	if (jump->flags & IS_COND) {
		SLJIT_ASSERT(!(jump->flags & IS_BL));
		if (diff <= 127 && diff >= -128) {
			jump->flags |= PATCH_TYPE1;
			return 5;
		}
		if (diff <= 524287 && diff >= -524288) {
			jump->flags |= PATCH_TYPE2;
			return 4;
		}
		/* +1 comes from the prefix IT instruction. */
		diff--;
		if (diff <= 8388607 && diff >= -8388608) {
			jump->flags |= PATCH_TYPE3;
			return 3;
		}
	}
	else if (jump->flags & IS_BL) {
		if (diff <= 8388607 && diff >= -8388608) {
			jump->flags |= PATCH_BL;
			return 3;
		}
	}
	else {
		if (diff <= 1023 && diff >= -1024) {
			jump->flags |= PATCH_TYPE4;
			return 4;
		}
		if (diff <= 8388607 && diff >= -8388608) {
			jump->flags |= PATCH_TYPE5;
			return 3;
		}
	}

	return 0;
}

static SLJIT_INLINE void set_jump_instruction(struct sljit_jump *jump, sljit_sw executable_offset)
{
	sljit_s32 type = (jump->flags >> 4) & 0xf;
	sljit_sw diff;
	sljit_u16 *jump_inst;
	sljit_s32 s, j1, j2;

	if (SLJIT_UNLIKELY(type == 0)) {
		modify_imm32_const((sljit_u16*)jump->addr, (jump->flags & JUMP_LABEL) ? jump->u.label->addr : jump->u.target);
		return;
	}

	if (jump->flags & JUMP_ADDR) {
		SLJIT_ASSERT(jump->u.target & 0x1);
		diff = ((sljit_sw)jump->u.target - (sljit_sw)(jump->addr + sizeof(sljit_u32)) - executable_offset) >> 1;
	}
	else {
		SLJIT_ASSERT(jump->u.label->addr & 0x1);
		diff = ((sljit_sw)(jump->u.label->addr) - (sljit_sw)(jump->addr + sizeof(sljit_u32)) - executable_offset) >> 1;
	}
	jump_inst = (sljit_u16*)jump->addr;

	switch (type) {
	case 1:
		/* Encoding T1 of 'B' instruction */
		SLJIT_ASSERT(diff <= 127 && diff >= -128 && (jump->flags & IS_COND));
		jump_inst[0] = (sljit_u16)(0xd000 | (jump->flags & 0xf00) | ((sljit_ins)diff & 0xff));
		return;
	case 2:
		/* Encoding T3 of 'B' instruction */
		SLJIT_ASSERT(diff <= 524287 && diff >= -524288 && (jump->flags & IS_COND));
		jump_inst[0] = (sljit_u16)(0xf000 | COPY_BITS(jump->flags, 8, 6, 4) | COPY_BITS(diff, 11, 0, 6) | COPY_BITS(diff, 19, 10, 1));
		jump_inst[1] = (sljit_u16)(0x8000 | COPY_BITS(diff, 17, 13, 1) | COPY_BITS(diff, 18, 11, 1) | ((sljit_ins)diff & 0x7ff));
		return;
	case 3:
		SLJIT_ASSERT(jump->flags & IS_COND);
		*jump_inst++ = (sljit_u16)(IT | ((jump->flags >> 4) & 0xf0) | 0x8);
		diff--;
		type = 5;
		break;
	case 4:
		/* Encoding T2 of 'B' instruction */
		SLJIT_ASSERT(diff <= 1023 && diff >= -1024 && !(jump->flags & IS_COND));
		jump_inst[0] = (sljit_u16)(0xe000 | (diff & 0x7ff));
		return;
	}

	SLJIT_ASSERT(diff <= 8388607 && diff >= -8388608);

	/* Really complex instruction form for branches. */
	s = (diff >> 23) & 0x1;
	j1 = (~(diff >> 22) ^ s) & 0x1;
	j2 = (~(diff >> 21) ^ s) & 0x1;
	jump_inst[0] = (sljit_u16)(0xf000 | ((sljit_ins)s << 10) | COPY_BITS(diff, 11, 0, 10));
	jump_inst[1] = (sljit_u16)((j1 << 13) | (j2 << 11) | (diff & 0x7ff));

	/* The others have a common form. */
	if (type == 5) /* Encoding T4 of 'B' instruction */
		jump_inst[1] |= 0x9000;
	else if (type == 6) /* Encoding T1 of 'BL' instruction */
		jump_inst[1] |= 0xd000;
	else
		SLJIT_UNREACHABLE();
}

SLJIT_API_FUNC_ATTRIBUTE void* sljit_generate_code(struct sljit_compiler *compiler)
{
	struct sljit_memory_fragment *buf;
	sljit_u16 *code;
	sljit_u16 *code_ptr;
	sljit_u16 *buf_ptr;
	sljit_u16 *buf_end;
	sljit_uw half_count;
	sljit_uw next_addr;
	sljit_sw executable_offset;

	struct sljit_label *label;
	struct sljit_jump *jump;
	struct sljit_const *const_;
	struct sljit_put_label *put_label;

	CHECK_ERROR_PTR();
	CHECK_PTR(check_sljit_generate_code(compiler));
	reverse_buf(compiler);

	code = (sljit_u16*)SLJIT_MALLOC_EXEC(compiler->size * sizeof(sljit_u16), compiler->exec_allocator_data);
	PTR_FAIL_WITH_EXEC_IF(code);
	buf = compiler->buf;

	code_ptr = code;
	half_count = 0;
	next_addr = 0;
	executable_offset = SLJIT_EXEC_OFFSET(code);

	label = compiler->labels;
	jump = compiler->jumps;
	const_ = compiler->consts;
	put_label = compiler->put_labels;

	do {
		buf_ptr = (sljit_u16*)buf->memory;
		buf_end = buf_ptr + (buf->used_size >> 1);
		do {
			*code_ptr = *buf_ptr++;
			if (next_addr == half_count) {
				SLJIT_ASSERT(!label || label->size >= half_count);
				SLJIT_ASSERT(!jump || jump->addr >= half_count);
				SLJIT_ASSERT(!const_ || const_->addr >= half_count);
				SLJIT_ASSERT(!put_label || put_label->addr >= half_count);

				/* These structures are ordered by their address. */
				if (label && label->size == half_count) {
					label->addr = ((sljit_uw)SLJIT_ADD_EXEC_OFFSET(code_ptr, executable_offset)) | 0x1;
					label->size = (sljit_uw)(code_ptr - code);
					label = label->next;
				}
				if (jump && jump->addr == half_count) {
						jump->addr = (sljit_uw)code_ptr - ((jump->flags & IS_COND) ? 10 : 8);
						code_ptr -= detect_jump_type(jump, code_ptr, code, executable_offset);
						jump = jump->next;
				}
				if (const_ && const_->addr == half_count) {
					const_->addr = (sljit_uw)code_ptr;
					const_ = const_->next;
				}
				if (put_label && put_label->addr == half_count) {
					SLJIT_ASSERT(put_label->label);
					put_label->addr = (sljit_uw)code_ptr;
					put_label = put_label->next;
				}
				next_addr = compute_next_addr(label, jump, const_, put_label);
			}
			code_ptr ++;
			half_count ++;
		} while (buf_ptr < buf_end);

		buf = buf->next;
	} while (buf);

	if (label && label->size == half_count) {
		label->addr = ((sljit_uw)SLJIT_ADD_EXEC_OFFSET(code_ptr, executable_offset)) | 0x1;
		label->size = (sljit_uw)(code_ptr - code);
		label = label->next;
	}

	SLJIT_ASSERT(!label);
	SLJIT_ASSERT(!jump);
	SLJIT_ASSERT(!const_);
	SLJIT_ASSERT(!put_label);
	SLJIT_ASSERT(code_ptr - code <= (sljit_sw)compiler->size);

	jump = compiler->jumps;
	while (jump) {
		set_jump_instruction(jump, executable_offset);
		jump = jump->next;
	}

	put_label = compiler->put_labels;
	while (put_label) {
		modify_imm32_const((sljit_u16 *)put_label->addr, put_label->label->addr);
		put_label = put_label->next;
	}

	compiler->error = SLJIT_ERR_COMPILED;
	compiler->executable_offset = executable_offset;
	compiler->executable_size = (sljit_uw)(code_ptr - code) * sizeof(sljit_u16);

	code = (sljit_u16 *)SLJIT_ADD_EXEC_OFFSET(code, executable_offset);
	code_ptr = (sljit_u16 *)SLJIT_ADD_EXEC_OFFSET(code_ptr, executable_offset);

	SLJIT_CACHE_FLUSH(code, code_ptr);
	SLJIT_UPDATE_WX_FLAGS(code, code_ptr, 1);

	/* Set thumb mode flag. */
	return (void*)((sljit_uw)code | 0x1);
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_has_cpu_feature(sljit_s32 feature_type)
{
	switch (feature_type) {
	case SLJIT_HAS_FPU:
#ifdef SLJIT_IS_FPU_AVAILABLE
		return SLJIT_IS_FPU_AVAILABLE;
#else
		/* Available by default. */
		return 1;
#endif

	case SLJIT_HAS_CLZ:
	case SLJIT_HAS_CMOV:
	case SLJIT_HAS_PREFETCH:
		return 1;

	default:
		return 0;
	}
}

/* --------------------------------------------------------------------- */
/*  Core code generator functions.                                       */
/* --------------------------------------------------------------------- */

#define INVALID_IMM	0x80000000
static sljit_uw get_imm(sljit_uw imm)
{
	/* Thumb immediate form. */
	sljit_s32 counter;

	if (imm <= 0xff)
		return imm;

	if ((imm & 0xffff) == (imm >> 16)) {
		/* Some special cases. */
		if (!(imm & 0xff00))
			return (1 << 12) | (imm & 0xff);
		if (!(imm & 0xff))
			return (2 << 12) | ((imm >> 8) & 0xff);
		if ((imm & 0xff00) == ((imm & 0xff) << 8))
			return (3 << 12) | (imm & 0xff);
	}

	/* Assembly optimization: count leading zeroes? */
	counter = 8;
	if (!(imm & 0xffff0000)) {
		counter += 16;
		imm <<= 16;
	}
	if (!(imm & 0xff000000)) {
		counter += 8;
		imm <<= 8;
	}
	if (!(imm & 0xf0000000)) {
		counter += 4;
		imm <<= 4;
	}
	if (!(imm & 0xc0000000)) {
		counter += 2;
		imm <<= 2;
	}
	if (!(imm & 0x80000000)) {
		counter += 1;
		imm <<= 1;
	}
	/* Since imm >= 128, this must be true. */
	SLJIT_ASSERT(counter <= 31);

	if (imm & 0x00ffffff)
		return INVALID_IMM; /* Cannot be encoded. */

	return ((imm >> 24) & 0x7f) | COPY_BITS(counter, 4, 26, 1) | COPY_BITS(counter, 1, 12, 3) | COPY_BITS(counter, 0, 7, 1);
}

static sljit_s32 load_immediate(struct sljit_compiler *compiler, sljit_s32 dst, sljit_uw imm)
{
	sljit_uw tmp;

	/* MOVS cannot be used since it destroy flags. */

	if (imm >= 0x10000) {
		tmp = get_imm(imm);
		if (tmp != INVALID_IMM)
			return push_inst32(compiler, MOV_WI | RD4(dst) | tmp);
		tmp = get_imm(~imm);
		if (tmp != INVALID_IMM)
			return push_inst32(compiler, MVN_WI | RD4(dst) | tmp);
	}

	/* set low 16 bits, set hi 16 bits to 0. */
	FAIL_IF(push_inst32(compiler, MOVW | RD4(dst)
		| COPY_BITS(imm, 12, 16, 4) | COPY_BITS(imm, 11, 26, 1) | COPY_BITS(imm, 8, 12, 3) | (imm & 0xff)));

	/* set hi 16 bit if needed. */
	if (imm >= 0x10000)
		return push_inst32(compiler, MOVT | RD4(dst)
			| COPY_BITS(imm, 12 + 16, 16, 4) | COPY_BITS(imm, 11 + 16, 26, 1) | COPY_BITS(imm, 8 + 16, 12, 3) | ((imm & 0xff0000) >> 16));
	return SLJIT_SUCCESS;
}

#define ARG1_IMM	0x0010000
#define ARG2_IMM	0x0020000
/* SET_FLAGS must be 0x100000 as it is also the value of S bit (can be used for optimization). */
#define SET_FLAGS	0x0100000
#define UNUSED_RETURN	0x0200000

static sljit_s32 emit_op_imm(struct sljit_compiler *compiler, sljit_s32 flags, sljit_s32 dst, sljit_uw arg1, sljit_uw arg2)
{
	/* dst must be register, TMP_REG1
	   arg1 must be register, imm
	   arg2 must be register, imm */
	sljit_s32 reg;
	sljit_uw imm, nimm;

	if (SLJIT_UNLIKELY((flags & (ARG1_IMM | ARG2_IMM)) == (ARG1_IMM | ARG2_IMM))) {
		/* Both are immediates, no temporaries are used. */
		flags &= ~ARG1_IMM;
		FAIL_IF(load_immediate(compiler, TMP_REG1, arg1));
		arg1 = TMP_REG1;
	}

	if (flags & (ARG1_IMM | ARG2_IMM)) {
		reg = (sljit_s32)((flags & ARG2_IMM) ? arg1 : arg2);
		imm = (flags & ARG2_IMM) ? arg2 : arg1;

		switch (flags & 0xffff) {
		case SLJIT_CLZ:
		case SLJIT_MUL:
			/* No form with immediate operand. */
			break;
		case SLJIT_MOV:
			SLJIT_ASSERT(!(flags & SET_FLAGS) && (flags & ARG2_IMM) && arg1 == TMP_REG2);
			return load_immediate(compiler, dst, imm);
		case SLJIT_NOT:
			if (!(flags & SET_FLAGS))
				return load_immediate(compiler, dst, ~imm);
			/* Since the flags should be set, we just fallback to the register mode.
			   Although some clever things could be done here, "NOT IMM" does not worth the efforts. */
			break;
		case SLJIT_ADD:
			compiler->status_flags_state = SLJIT_CURRENT_FLAGS_ADD;
			nimm = NEGATE(imm);
			if (IS_2_LO_REGS(reg, dst)) {
				if (imm <= 0x7)
					return push_inst16(compiler, ADDSI3 | IMM3(imm) | RD3(dst) | RN3(reg));
				if (nimm <= 0x7)
					return push_inst16(compiler, SUBSI3 | IMM3(nimm) | RD3(dst) | RN3(reg));
				if (reg == dst) {
					if (imm <= 0xff)
						return push_inst16(compiler, ADDSI8 | IMM8(imm) | RDN3(dst));
					if (nimm <= 0xff)
						return push_inst16(compiler, SUBSI8 | IMM8(nimm) | RDN3(dst));
				}
			}
			if (!(flags & SET_FLAGS)) {
				if (imm <= 0xfff)
					return push_inst32(compiler, ADDWI | RD4(dst) | RN4(reg) | IMM12(imm));
				if (nimm <= 0xfff)
					return push_inst32(compiler, SUBWI | RD4(dst) | RN4(reg) | IMM12(nimm));
			}
			nimm = get_imm(imm);
			if (nimm != INVALID_IMM)
				return push_inst32(compiler, ADD_WI | (flags & SET_FLAGS) | RD4(dst) | RN4(reg) | nimm);
			nimm = get_imm(NEGATE(imm));
			if (nimm != INVALID_IMM)
				return push_inst32(compiler, SUB_WI | (flags & SET_FLAGS) | RD4(dst) | RN4(reg) | nimm);
			break;
		case SLJIT_ADDC:
			compiler->status_flags_state = SLJIT_CURRENT_FLAGS_ADD;
			imm = get_imm(imm);
			if (imm != INVALID_IMM)
				return push_inst32(compiler, ADCI | (flags & SET_FLAGS) | RD4(dst) | RN4(reg) | imm);
			break;
		case SLJIT_SUB:
			compiler->status_flags_state = SLJIT_CURRENT_FLAGS_SUB;
			if (flags & ARG1_IMM) {
				if (imm == 0 && IS_2_LO_REGS(reg, dst))
					return push_inst16(compiler, RSBSI | RD3(dst) | RN3(reg));
				imm = get_imm(imm);
				if (imm != INVALID_IMM)
					return push_inst32(compiler, RSB_WI | (flags & SET_FLAGS) | RD4(dst) | RN4(reg) | imm);
				break;
			}
			if (flags & UNUSED_RETURN) {
				if (imm <= 0xff && reg_map[reg] <= 7)
					return push_inst16(compiler, CMPI | IMM8(imm) | RDN3(reg));
				nimm = get_imm(imm);
				if (nimm != INVALID_IMM)
					return push_inst32(compiler, CMPI_W | RN4(reg) | nimm);
				nimm = get_imm(NEGATE(imm));
				if (nimm != INVALID_IMM)
					return push_inst32(compiler, CMNI_W | RN4(reg) | nimm);
				break;
			}
			nimm = NEGATE(imm);
			if (IS_2_LO_REGS(reg, dst)) {
				if (imm <= 0x7)
					return push_inst16(compiler, SUBSI3 | IMM3(imm) | RD3(dst) | RN3(reg));
				if (nimm <= 0x7)
					return push_inst16(compiler, ADDSI3 | IMM3(nimm) | RD3(dst) | RN3(reg));
				if (reg == dst) {
					if (imm <= 0xff)
						return push_inst16(compiler, SUBSI8 | IMM8(imm) | RDN3(dst));
					if (nimm <= 0xff)
						return push_inst16(compiler, ADDSI8 | IMM8(nimm) | RDN3(dst));
				}
			}
			if (!(flags & SET_FLAGS)) {
				if (imm <= 0xfff)
					return push_inst32(compiler, SUBWI | RD4(dst) | RN4(reg) | IMM12(imm));
				if (nimm <= 0xfff)
					return push_inst32(compiler, ADDWI | RD4(dst) | RN4(reg) | IMM12(nimm));
			}
			nimm = get_imm(imm);
			if (nimm != INVALID_IMM)
				return push_inst32(compiler, SUB_WI | (flags & SET_FLAGS) | RD4(dst) | RN4(reg) | nimm);
			nimm = get_imm(NEGATE(imm));
			if (nimm != INVALID_IMM)
				return push_inst32(compiler, ADD_WI | (flags & SET_FLAGS) | RD4(dst) | RN4(reg) | nimm);
			break;
		case SLJIT_SUBC:
			compiler->status_flags_state = SLJIT_CURRENT_FLAGS_SUB;
			if (flags & ARG1_IMM)
				break;
			imm = get_imm(imm);
			if (imm != INVALID_IMM)
				return push_inst32(compiler, SBCI | (flags & SET_FLAGS) | RD4(dst) | RN4(reg) | imm);
			break;
		case SLJIT_AND:
			nimm = get_imm(imm);
			if (nimm != INVALID_IMM)
				return push_inst32(compiler, ((flags & UNUSED_RETURN) ? TSTI : ANDI) | (flags & SET_FLAGS) | RD4(dst) | RN4(reg) | nimm);
			imm = get_imm(~imm);
			if (imm != INVALID_IMM)
				return push_inst32(compiler, BICI | (flags & SET_FLAGS) | RD4(dst) | RN4(reg) | imm);
			break;
		case SLJIT_OR:
			nimm = get_imm(imm);
			if (nimm != INVALID_IMM)
				return push_inst32(compiler, ORRI | (flags & SET_FLAGS) | RD4(dst) | RN4(reg) | nimm);
			imm = get_imm(~imm);
			if (imm != INVALID_IMM)
				return push_inst32(compiler, ORNI | (flags & SET_FLAGS) | RD4(dst) | RN4(reg) | imm);
			break;
		case SLJIT_XOR:
			imm = get_imm(imm);
			if (imm != INVALID_IMM)
				return push_inst32(compiler, EORI | (flags & SET_FLAGS) | RD4(dst) | RN4(reg) | imm);
			break;
		case SLJIT_SHL:
		case SLJIT_LSHR:
		case SLJIT_ASHR:
			if (flags & ARG1_IMM)
				break;
			imm &= 0x1f;
			if (imm == 0) {
				if (!(flags & SET_FLAGS))
					return push_inst16(compiler, MOV | SET_REGS44(dst, reg));
				if (IS_2_LO_REGS(dst, reg))
					return push_inst16(compiler, MOVS | RD3(dst) | RN3(reg));
				return push_inst32(compiler, MOV_W | SET_FLAGS | RD4(dst) | RM4(reg));
			}
			switch (flags & 0xffff) {
			case SLJIT_SHL:
				if (IS_2_LO_REGS(dst, reg))
					return push_inst16(compiler, LSLSI | RD3(dst) | RN3(reg) | (imm << 6));
				return push_inst32(compiler, LSL_WI | (flags & SET_FLAGS) | RD4(dst) | RM4(reg) | IMM5(imm));
			case SLJIT_LSHR:
				if (IS_2_LO_REGS(dst, reg))
					return push_inst16(compiler, LSRSI | RD3(dst) | RN3(reg) | (imm << 6));
				return push_inst32(compiler, LSR_WI | (flags & SET_FLAGS) | RD4(dst) | RM4(reg) | IMM5(imm));
			default: /* SLJIT_ASHR */
				if (IS_2_LO_REGS(dst, reg))
					return push_inst16(compiler, ASRSI | RD3(dst) | RN3(reg) | (imm << 6));
				return push_inst32(compiler, ASR_WI | (flags & SET_FLAGS) | RD4(dst) | RM4(reg) | IMM5(imm));
			}
		default:
			SLJIT_UNREACHABLE();
			break;
		}

		if (flags & ARG2_IMM) {
			imm = arg2;
			arg2 = (arg1 == TMP_REG1) ? TMP_REG2 : TMP_REG1;
			FAIL_IF(load_immediate(compiler, (sljit_s32)arg2, imm));
		}
		else {
			imm = arg1;
			arg1 = (arg2 == TMP_REG1) ? TMP_REG2 : TMP_REG1;
			FAIL_IF(load_immediate(compiler, (sljit_s32)arg1, imm));
		}

		SLJIT_ASSERT(arg1 != arg2);
	}

	/* Both arguments are registers. */
	switch (flags & 0xffff) {
	case SLJIT_MOV:
	case SLJIT_MOV_U32:
	case SLJIT_MOV_S32:
	case SLJIT_MOV32:
	case SLJIT_MOV_P:
		SLJIT_ASSERT(!(flags & SET_FLAGS) && arg1 == TMP_REG2);
		if (dst == (sljit_s32)arg2)
			return SLJIT_SUCCESS;
		return push_inst16(compiler, MOV | SET_REGS44(dst, arg2));
	case SLJIT_MOV_U8:
		SLJIT_ASSERT(!(flags & SET_FLAGS) && arg1 == TMP_REG2);
		if (IS_2_LO_REGS(dst, arg2))
			return push_inst16(compiler, UXTB | RD3(dst) | RN3(arg2));
		return push_inst32(compiler, UXTB_W | RD4(dst) | RM4(arg2));
	case SLJIT_MOV_S8:
		SLJIT_ASSERT(!(flags & SET_FLAGS) && arg1 == TMP_REG2);
		if (IS_2_LO_REGS(dst, arg2))
			return push_inst16(compiler, SXTB | RD3(dst) | RN3(arg2));
		return push_inst32(compiler, SXTB_W | RD4(dst) | RM4(arg2));
	case SLJIT_MOV_U16:
		SLJIT_ASSERT(!(flags & SET_FLAGS) && arg1 == TMP_REG2);
		if (IS_2_LO_REGS(dst, arg2))
			return push_inst16(compiler, UXTH | RD3(dst) | RN3(arg2));
		return push_inst32(compiler, UXTH_W | RD4(dst) | RM4(arg2));
	case SLJIT_MOV_S16:
		SLJIT_ASSERT(!(flags & SET_FLAGS) && arg1 == TMP_REG2);
		if (IS_2_LO_REGS(dst, arg2))
			return push_inst16(compiler, SXTH | RD3(dst) | RN3(arg2));
		return push_inst32(compiler, SXTH_W | RD4(dst) | RM4(arg2));
	case SLJIT_NOT:
		SLJIT_ASSERT(arg1 == TMP_REG2);
		if (IS_2_LO_REGS(dst, arg2))
			return push_inst16(compiler, MVNS | RD3(dst) | RN3(arg2));
		return push_inst32(compiler, MVN_W | (flags & SET_FLAGS) | RD4(dst) | RM4(arg2));
	case SLJIT_CLZ:
		SLJIT_ASSERT(arg1 == TMP_REG2);
		FAIL_IF(push_inst32(compiler, CLZ | RN4(arg2) | RD4(dst) | RM4(arg2)));
		return SLJIT_SUCCESS;
	case SLJIT_ADD:
		compiler->status_flags_state = SLJIT_CURRENT_FLAGS_ADD;
		if (IS_3_LO_REGS(dst, arg1, arg2))
			return push_inst16(compiler, ADDS | RD3(dst) | RN3(arg1) | RM3(arg2));
		if (dst == (sljit_s32)arg1 && !(flags & SET_FLAGS))
			return push_inst16(compiler, ADD | SET_REGS44(dst, arg2));
		return push_inst32(compiler, ADD_W | (flags & SET_FLAGS) | RD4(dst) | RN4(arg1) | RM4(arg2));
	case SLJIT_ADDC:
		compiler->status_flags_state = SLJIT_CURRENT_FLAGS_ADD;
		if (dst == (sljit_s32)arg1 && IS_2_LO_REGS(dst, arg2))
			return push_inst16(compiler, ADCS | RD3(dst) | RN3(arg2));
		return push_inst32(compiler, ADC_W | (flags & SET_FLAGS) | RD4(dst) | RN4(arg1) | RM4(arg2));
	case SLJIT_SUB:
		compiler->status_flags_state = SLJIT_CURRENT_FLAGS_SUB;
		if (flags & UNUSED_RETURN) {
			if (IS_2_LO_REGS(arg1, arg2))
				return push_inst16(compiler, CMP | RD3(arg1) | RN3(arg2));
			return push_inst16(compiler, CMP_X | SET_REGS44(arg1, arg2));
		}
		if (IS_3_LO_REGS(dst, arg1, arg2))
			return push_inst16(compiler, SUBS | RD3(dst) | RN3(arg1) | RM3(arg2));
		return push_inst32(compiler, SUB_W | (flags & SET_FLAGS) | RD4(dst) | RN4(arg1) | RM4(arg2));
	case SLJIT_SUBC:
		compiler->status_flags_state = SLJIT_CURRENT_FLAGS_SUB;
		if (dst == (sljit_s32)arg1 && IS_2_LO_REGS(dst, arg2))
			return push_inst16(compiler, SBCS | RD3(dst) | RN3(arg2));
		return push_inst32(compiler, SBC_W | (flags & SET_FLAGS) | RD4(dst) | RN4(arg1) | RM4(arg2));
	case SLJIT_MUL:
		compiler->status_flags_state = 0;
		if (!(flags & SET_FLAGS))
			return push_inst32(compiler, MUL | RD4(dst) | RN4(arg1) | RM4(arg2));
		SLJIT_ASSERT(dst != TMP_REG2);
		FAIL_IF(push_inst32(compiler, SMULL | RT4(dst) | RD4(TMP_REG2) | RN4(arg1) | RM4(arg2)));
		/* cmp TMP_REG2, dst asr #31. */
		return push_inst32(compiler, CMP_W | RN4(TMP_REG2) | 0x70e0 | RM4(dst));
	case SLJIT_AND:
		if (dst == (sljit_s32)arg1 && IS_2_LO_REGS(dst, arg2))
			return push_inst16(compiler, ANDS | RD3(dst) | RN3(arg2));
		if ((flags & UNUSED_RETURN) && IS_2_LO_REGS(arg1, arg2))
			return push_inst16(compiler, TST | RD3(arg1) | RN3(arg2));
		return push_inst32(compiler, ((flags & UNUSED_RETURN) ? TST_W : AND_W) | (flags & SET_FLAGS) | RD4(dst) | RN4(arg1) | RM4(arg2));
	case SLJIT_OR:
		if (dst == (sljit_s32)arg1 && IS_2_LO_REGS(dst, arg2))
			return push_inst16(compiler, ORRS | RD3(dst) | RN3(arg2));
		return push_inst32(compiler, ORR_W | (flags & SET_FLAGS) | RD4(dst) | RN4(arg1) | RM4(arg2));
	case SLJIT_XOR:
		if (dst == (sljit_s32)arg1 && IS_2_LO_REGS(dst, arg2))
			return push_inst16(compiler, EORS | RD3(dst) | RN3(arg2));
		return push_inst32(compiler, EOR_W | (flags & SET_FLAGS) | RD4(dst) | RN4(arg1) | RM4(arg2));
	case SLJIT_SHL:
		if (dst == (sljit_s32)arg1 && IS_2_LO_REGS(dst, arg2))
			return push_inst16(compiler, LSLS | RD3(dst) | RN3(arg2));
		return push_inst32(compiler, LSL_W | (flags & SET_FLAGS) | RD4(dst) | RN4(arg1) | RM4(arg2));
	case SLJIT_LSHR:
		if (dst == (sljit_s32)arg1 && IS_2_LO_REGS(dst, arg2))
			return push_inst16(compiler, LSRS | RD3(dst) | RN3(arg2));
		return push_inst32(compiler, LSR_W | (flags & SET_FLAGS) | RD4(dst) | RN4(arg1) | RM4(arg2));
	case SLJIT_ASHR:
		if (dst == (sljit_s32)arg1 && IS_2_LO_REGS(dst, arg2))
			return push_inst16(compiler, ASRS | RD3(dst) | RN3(arg2));
		return push_inst32(compiler, ASR_W | (flags & SET_FLAGS) | RD4(dst) | RN4(arg1) | RM4(arg2));
	}

	SLJIT_UNREACHABLE();
	return SLJIT_SUCCESS;
}

#define STORE		0x01
#define SIGNED		0x02

#define WORD_SIZE	0x00
#define BYTE_SIZE	0x04
#define HALF_SIZE	0x08
#define PRELOAD		0x0c

#define IS_WORD_SIZE(flags)		(!(flags & (BYTE_SIZE | HALF_SIZE)))
#define OFFSET_CHECK(imm, shift)	(!(argw & ~(imm << shift)))

/*
  1st letter:
  w = word
  b = byte
  h = half

  2nd letter:
  s = signed
  u = unsigned

  3rd letter:
  l = load
  s = store
*/

static const sljit_ins sljit_mem16[12] = {
/* w u l */ 0x5800 /* ldr */,
/* w u s */ 0x5000 /* str */,
/* w s l */ 0x5800 /* ldr */,
/* w s s */ 0x5000 /* str */,

/* b u l */ 0x5c00 /* ldrb */,
/* b u s */ 0x5400 /* strb */,
/* b s l */ 0x5600 /* ldrsb */,
/* b s s */ 0x5400 /* strb */,

/* h u l */ 0x5a00 /* ldrh */,
/* h u s */ 0x5200 /* strh */,
/* h s l */ 0x5e00 /* ldrsh */,
/* h s s */ 0x5200 /* strh */,
};

static const sljit_ins sljit_mem16_imm5[12] = {
/* w u l */ 0x6800 /* ldr imm5 */,
/* w u s */ 0x6000 /* str imm5 */,
/* w s l */ 0x6800 /* ldr imm5 */,
/* w s s */ 0x6000 /* str imm5 */,

/* b u l */ 0x7800 /* ldrb imm5 */,
/* b u s */ 0x7000 /* strb imm5 */,
/* b s l */ 0x0000 /* not allowed */,
/* b s s */ 0x7000 /* strb imm5 */,

/* h u l */ 0x8800 /* ldrh imm5 */,
/* h u s */ 0x8000 /* strh imm5 */,
/* h s l */ 0x0000 /* not allowed */,
/* h s s */ 0x8000 /* strh imm5 */,
};

#define MEM_IMM8	0xc00
#define MEM_IMM12	0x800000
static const sljit_ins sljit_mem32[13] = {
/* w u l */ 0xf8500000 /* ldr.w */,
/* w u s */ 0xf8400000 /* str.w */,
/* w s l */ 0xf8500000 /* ldr.w */,
/* w s s */ 0xf8400000 /* str.w */,

/* b u l */ 0xf8100000 /* ldrb.w */,
/* b u s */ 0xf8000000 /* strb.w */,
/* b s l */ 0xf9100000 /* ldrsb.w */,
/* b s s */ 0xf8000000 /* strb.w */,

/* h u l */ 0xf8300000 /* ldrh.w */,
/* h u s */ 0xf8200000 /* strsh.w */,
/* h s l */ 0xf9300000 /* ldrsh.w */,
/* h s s */ 0xf8200000 /* strsh.w */,

/* p u l */ 0xf8100000 /* pld */,
};

/* Helper function. Dst should be reg + value, using at most 1 instruction, flags does not set. */
static sljit_s32 emit_set_delta(struct sljit_compiler *compiler, sljit_s32 dst, sljit_s32 reg, sljit_sw value)
{
	sljit_uw imm;

	if (value >= 0) {
		if (value <= 0xfff)
			return push_inst32(compiler, ADDWI | RD4(dst) | RN4(reg) | IMM12(value));
		imm = get_imm((sljit_uw)value);
		if (imm != INVALID_IMM)
			return push_inst32(compiler, ADD_WI | RD4(dst) | RN4(reg) | imm);
	}
	else {
		value = -value;
		if (value <= 0xfff)
			return push_inst32(compiler, SUBWI | RD4(dst) | RN4(reg) | IMM12(value));
		imm = get_imm((sljit_uw)value);
		if (imm != INVALID_IMM)
			return push_inst32(compiler, SUB_WI | RD4(dst) | RN4(reg) | imm);
	}
	return SLJIT_ERR_UNSUPPORTED;
}

static SLJIT_INLINE sljit_s32 emit_op_mem(struct sljit_compiler *compiler, sljit_s32 flags, sljit_s32 reg,
	sljit_s32 arg, sljit_sw argw, sljit_s32 tmp_reg)
{
	sljit_s32 other_r;
	sljit_uw tmp;

	SLJIT_ASSERT(arg & SLJIT_MEM);
	SLJIT_ASSERT((arg & REG_MASK) != tmp_reg);
	arg &= ~SLJIT_MEM;

	if (SLJIT_UNLIKELY(!(arg & REG_MASK))) {
		tmp = get_imm((sljit_uw)argw & ~(sljit_uw)0xfff);
		if (tmp != INVALID_IMM) {
			FAIL_IF(push_inst32(compiler, MOV_WI | RD4(tmp_reg) | tmp));
			return push_inst32(compiler, sljit_mem32[flags] | MEM_IMM12 | RT4(reg) | RN4(tmp_reg) | (argw & 0xfff));
		}

		FAIL_IF(load_immediate(compiler, tmp_reg, (sljit_uw)argw));
		if (IS_2_LO_REGS(reg, tmp_reg) && sljit_mem16_imm5[flags])
			return push_inst16(compiler, sljit_mem16_imm5[flags] | RD3(reg) | RN3(tmp_reg));
		return push_inst32(compiler, sljit_mem32[flags] | MEM_IMM12 | RT4(reg) | RN4(tmp_reg));
	}

	if (SLJIT_UNLIKELY(arg & OFFS_REG_MASK)) {
		argw &= 0x3;
		other_r = OFFS_REG(arg);
		arg &= 0xf;

		if (!argw && IS_3_LO_REGS(reg, arg, other_r))
			return push_inst16(compiler, sljit_mem16[flags] | RD3(reg) | RN3(arg) | RM3(other_r));
		return push_inst32(compiler, sljit_mem32[flags] | RT4(reg) | RN4(arg) | RM4(other_r) | ((sljit_ins)argw << 4));
	}

	if (argw > 0xfff) {
		tmp = get_imm((sljit_uw)argw & ~(sljit_uw)0xfff);
		if (tmp != INVALID_IMM) {
			push_inst32(compiler, ADD_WI | RD4(tmp_reg) | RN4(arg) | tmp);
			arg = tmp_reg;
			argw = argw & 0xfff;
		}
	}
	else if (argw < -0xff) {
		tmp = get_imm((sljit_uw)-argw & ~(sljit_uw)0xff);
		if (tmp != INVALID_IMM) {
			push_inst32(compiler, SUB_WI | RD4(tmp_reg) | RN4(arg) | tmp);
			arg = tmp_reg;
			argw = -(-argw & 0xff);
		}
	}

	if (IS_2_LO_REGS(reg, arg) && sljit_mem16_imm5[flags]) {
		tmp = 3;
		if (IS_WORD_SIZE(flags)) {
			if (OFFSET_CHECK(0x1f, 2))
				tmp = 2;
		}
		else if (flags & BYTE_SIZE)
		{
			if (OFFSET_CHECK(0x1f, 0))
				tmp = 0;
		}
		else {
			SLJIT_ASSERT(flags & HALF_SIZE);
			if (OFFSET_CHECK(0x1f, 1))
				tmp = 1;
		}

		if (tmp < 3)
			return push_inst16(compiler, sljit_mem16_imm5[flags] | RD3(reg) | RN3(arg) | ((sljit_ins)argw << (6 - tmp)));
	}
	else if (SLJIT_UNLIKELY(arg == SLJIT_SP) && IS_WORD_SIZE(flags) && OFFSET_CHECK(0xff, 2) && reg_map[reg] <= 7) {
		/* SP based immediate. */
		return push_inst16(compiler, STR_SP | (sljit_ins)((flags & STORE) ? 0 : 0x800) | RDN3(reg) | ((sljit_ins)argw >> 2));
	}

	if (argw >= 0 && argw <= 0xfff)
		return push_inst32(compiler, sljit_mem32[flags] | MEM_IMM12 | RT4(reg) | RN4(arg) | (sljit_ins)argw);
	else if (argw < 0 && argw >= -0xff)
		return push_inst32(compiler, sljit_mem32[flags] | MEM_IMM8 | RT4(reg) | RN4(arg) | (sljit_ins)-argw);

	SLJIT_ASSERT(arg != tmp_reg);

	FAIL_IF(load_immediate(compiler, tmp_reg, (sljit_uw)argw));
	if (IS_3_LO_REGS(reg, arg, tmp_reg))
		return push_inst16(compiler, sljit_mem16[flags] | RD3(reg) | RN3(arg) | RM3(tmp_reg));
	return push_inst32(compiler, sljit_mem32[flags] | RT4(reg) | RN4(arg) | RM4(tmp_reg));
}

/* --------------------------------------------------------------------- */
/*  Entry, exit                                                          */
/* --------------------------------------------------------------------- */

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_enter(struct sljit_compiler *compiler,
	sljit_s32 options, sljit_s32 arg_types, sljit_s32 scratches, sljit_s32 saveds,
	sljit_s32 fscratches, sljit_s32 fsaveds, sljit_s32 local_size)
{
	sljit_s32 size, i, tmp, word_arg_count, saved_arg_count;
	sljit_uw offset;
	sljit_uw imm = 0;
#ifdef __SOFTFP__
	sljit_u32 float_arg_count;
#else
	sljit_u32 old_offset, f32_offset;
	sljit_u32 remap[3];
	sljit_u32 *remap_ptr = remap;
#endif

	CHECK_ERROR();
	CHECK(check_sljit_emit_enter(compiler, options, arg_types, scratches, saveds, fscratches, fsaveds, local_size));
	set_emit_enter(compiler, options, arg_types, scratches, saveds, fscratches, fsaveds, local_size);

	tmp = SLJIT_S0 - saveds;
	for (i = SLJIT_S0; i > tmp; i--)
		imm |= (sljit_uw)1 << reg_map[i];

	for (i = scratches; i >= SLJIT_FIRST_SAVED_REG; i--)
		imm |= (sljit_uw)1 << reg_map[i];

	/* At least two registers must be set for PUSH_W and one for PUSH instruction. */
	FAIL_IF((imm & 0xff00)
		? push_inst32(compiler, PUSH_W | (1 << 14) | imm)
		: push_inst16(compiler, PUSH | (1 << 8) | imm));

	/* Stack must be aligned to 8 bytes: (LR, R4) */
	size = GET_SAVED_REGISTERS_SIZE(scratches, saveds, 1);

	if (fsaveds > 0 || fscratches >= SLJIT_FIRST_SAVED_FLOAT_REG) {
		if ((size & SSIZE_OF(sw)) != 0) {
			FAIL_IF(push_inst16(compiler, SUB_SP_I | (sizeof(sljit_sw) >> 2)));
			size += SSIZE_OF(sw);
		}

		if (fsaveds + fscratches >= SLJIT_NUMBER_OF_FLOAT_REGISTERS) {
			FAIL_IF(push_inst32(compiler, VPUSH | DD4(SLJIT_FS0) | ((sljit_uw)SLJIT_NUMBER_OF_SAVED_FLOAT_REGISTERS << 1)));
		} else {
			if (fsaveds > 0)
				FAIL_IF(push_inst32(compiler, VPUSH | DD4(SLJIT_FS0) | ((sljit_uw)fsaveds << 1)));
			if (fscratches >= SLJIT_FIRST_SAVED_FLOAT_REG)
				FAIL_IF(push_inst32(compiler, VPUSH | DD4(fscratches) | ((sljit_uw)(fscratches - (SLJIT_FIRST_SAVED_FLOAT_REG - 1)) << 1)));
		}
	}

	local_size = ((size + local_size + 0x7) & ~0x7) - size;
	compiler->local_size = local_size;

	arg_types >>= SLJIT_ARG_SHIFT;
	word_arg_count = 0;
	saved_arg_count = 0;
#ifdef __SOFTFP__
	SLJIT_COMPILE_ASSERT(SLJIT_FR0 == 1, float_register_index_start);

	offset = 0;
	float_arg_count = 0;

	while (arg_types) {
		switch (arg_types & SLJIT_ARG_MASK) {
		case SLJIT_ARG_TYPE_F64:
			if (offset & 0x7)
				offset += sizeof(sljit_sw);

			if (offset < 4 * sizeof(sljit_sw))
				FAIL_IF(push_inst32(compiler, VMOV2 | (offset << 10) | ((offset + sizeof(sljit_sw)) << 14) | float_arg_count));
			else
				FAIL_IF(push_inst32(compiler, VLDR_F32 | 0x800100 | RN4(SLJIT_SP)
					| (float_arg_count << 12) | ((offset + (sljit_uw)size - 4 * sizeof(sljit_sw)) >> 2)));
			float_arg_count++;
			offset += sizeof(sljit_f64) - sizeof(sljit_sw);
			break;
		case SLJIT_ARG_TYPE_F32:
			if (offset < 4 * sizeof(sljit_sw))
				FAIL_IF(push_inst32(compiler, VMOV | (float_arg_count << 16) | (offset << 10)));
			else
				FAIL_IF(push_inst32(compiler, VLDR_F32 | 0x800000 | RN4(SLJIT_SP)
					| (float_arg_count << 12) | ((offset + (sljit_uw)size - 4 * sizeof(sljit_sw)) >> 2)));
			float_arg_count++;
			break;
		default:
			word_arg_count++;

			if (!(arg_types & SLJIT_ARG_TYPE_SCRATCH_REG)) {
				tmp = SLJIT_S0 - saved_arg_count;
				saved_arg_count++;
			} else if (word_arg_count - 1 != (sljit_s32)(offset >> 2))
				tmp = word_arg_count;
			else
				break;

			SLJIT_ASSERT(reg_map[tmp] <= 7);

			if (offset < 4 * sizeof(sljit_sw))
				FAIL_IF(push_inst16(compiler, MOV | RD3(tmp) | (offset << 1)));
			else
				FAIL_IF(push_inst16(compiler, LDR_SP | RDN3(tmp)
					| ((offset + (sljit_uw)size - 4 * sizeof(sljit_sw)) >> 2)));
			break;
		}

		offset += sizeof(sljit_sw);
		arg_types >>= SLJIT_ARG_SHIFT;
	}

	compiler->args_size = offset;
#else
	offset = SLJIT_FR0;
	old_offset = SLJIT_FR0;
	f32_offset = 0;

	while (arg_types) {
		switch (arg_types & SLJIT_ARG_MASK) {
		case SLJIT_ARG_TYPE_F64:
			if (offset != old_offset)
				*remap_ptr++ = VMOV_F32 | SLJIT_32 | DD4(offset) | DM4(old_offset);
			old_offset++;
			offset++;
			break;
		case SLJIT_ARG_TYPE_F32:
			if (f32_offset != 0) {
				*remap_ptr++ = VMOV_F32 | 0x20 | DD4(offset) | DM4(f32_offset);
				f32_offset = 0;
			} else {
				if (offset != old_offset)
					*remap_ptr++ = VMOV_F32 | DD4(offset) | DM4(old_offset);
				f32_offset = old_offset;
				old_offset++;
			}
			offset++;
			break;
		default:
			if (!(arg_types & SLJIT_ARG_TYPE_SCRATCH_REG)) {
				FAIL_IF(push_inst16(compiler, MOV | SET_REGS44(SLJIT_S0 - saved_arg_count, SLJIT_R0 + word_arg_count)));
				saved_arg_count++;
			}

			word_arg_count++;
			break;
		}
		arg_types >>= SLJIT_ARG_SHIFT;
	}

	SLJIT_ASSERT((sljit_uw)(remap_ptr - remap) <= sizeof(remap));

	while (remap_ptr > remap)
		FAIL_IF(push_inst32(compiler, *(--remap_ptr)));
#endif

#ifdef _WIN32
	if (local_size >= 4096) {
		imm = get_imm(4096);
		SLJIT_ASSERT(imm != INVALID_IMM);

		FAIL_IF(push_inst32(compiler, SUB_WI | RD4(SLJIT_SP) | RN4(SLJIT_SP) | imm));

		if (local_size < 4 * 4096) {
			if (local_size > 2 * 4096) {
				if (local_size > 3 * 4096) {
					FAIL_IF(push_inst32(compiler, LDRI | 0x400 | RT4(TMP_REG1) | RN4(SLJIT_SP)));
					FAIL_IF(push_inst32(compiler, SUB_WI | RD4(SLJIT_SP) | RN4(SLJIT_SP) | imm));
				}

				FAIL_IF(push_inst32(compiler, LDRI | 0x400 | RT4(TMP_REG1) | RN4(SLJIT_SP)));
				FAIL_IF(push_inst32(compiler, SUB_WI | RD4(SLJIT_SP) | RN4(SLJIT_SP) | imm));
			}
		} else {
			FAIL_IF(load_immediate(compiler, TMP_REG2, ((sljit_uw)local_size >> 12) - 1));
			FAIL_IF(push_inst32(compiler, LDRI | 0x400 | RT4(TMP_REG1) | RN4(SLJIT_SP)));
			FAIL_IF(push_inst32(compiler, SUB_WI | RD4(SLJIT_SP) | RN4(SLJIT_SP) | imm));
			FAIL_IF(push_inst32(compiler, SUB_WI | SET_FLAGS | RD4(TMP_REG2) | RN4(TMP_REG2) | 1));
			FAIL_IF(push_inst16(compiler, BCC | (0x1 << 8) /* not-equal */ | (-8 & 0xff)));
		}

		FAIL_IF(push_inst32(compiler, LDRI | 0x400 | RT4(TMP_REG1) | RN4(SLJIT_SP)));
		local_size &= 0xfff;
	}

	if (local_size >= 256) {
		SLJIT_ASSERT(local_size < 4096);

		if (local_size <= (127 << 2))
			FAIL_IF(push_inst16(compiler, SUB_SP_I | ((sljit_uw)local_size >> 2)));
		else
			FAIL_IF(emit_op_imm(compiler, SLJIT_SUB | ARG2_IMM, SLJIT_SP, SLJIT_SP, (sljit_uw)local_size));

		FAIL_IF(push_inst32(compiler, LDRI | 0x400 | RT4(TMP_REG1) | RN4(SLJIT_SP)));
	} else if (local_size > 0)
		FAIL_IF(push_inst32(compiler, LDRI | 0x500 | RT4(TMP_REG1) | RN4(SLJIT_SP) | (sljit_uw)local_size));
#else /* !_WIN32 */
	if (local_size > 0) {
		if (local_size <= (127 << 2))
			FAIL_IF(push_inst16(compiler, SUB_SP_I | ((sljit_uw)local_size >> 2)));
		else
			FAIL_IF(emit_op_imm(compiler, SLJIT_SUB | ARG2_IMM, SLJIT_SP, SLJIT_SP, (sljit_uw)local_size));
	}
#endif /* _WIN32 */

	return SLJIT_SUCCESS;
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_set_context(struct sljit_compiler *compiler,
	sljit_s32 options, sljit_s32 arg_types, sljit_s32 scratches, sljit_s32 saveds,
	sljit_s32 fscratches, sljit_s32 fsaveds, sljit_s32 local_size)
{
	sljit_s32 size;

	CHECK_ERROR();
	CHECK(check_sljit_set_context(compiler, options, arg_types, scratches, saveds, fscratches, fsaveds, local_size));
	set_set_context(compiler, options, arg_types, scratches, saveds, fscratches, fsaveds, local_size);

	size = GET_SAVED_REGISTERS_SIZE(scratches, saveds, 1);

	if ((size & SSIZE_OF(sw)) != 0 && (fsaveds > 0 || fscratches >= SLJIT_FIRST_SAVED_FLOAT_REG))
		size += SSIZE_OF(sw);

	compiler->local_size = ((size + local_size + 0x7) & ~0x7) - size;
	return SLJIT_SUCCESS;
}

static sljit_s32 emit_add_sp(struct sljit_compiler *compiler, sljit_uw imm)
{
	sljit_uw imm2;

	/* The TMP_REG1 register must keep its value. */
	if (imm <= (127u << 2))
		return push_inst16(compiler, ADD_SP_I | (imm >> 2));

	if (imm <= 0xfff)
		return push_inst32(compiler, ADDWI | RD4(SLJIT_SP) | RN4(SLJIT_SP) | IMM12(imm));

	imm2 = get_imm(imm);

	if (imm2 != INVALID_IMM)
		return push_inst32(compiler, ADD_WI | RD4(SLJIT_SP) | RN4(SLJIT_SP) | imm2);

	FAIL_IF(load_immediate(compiler, TMP_REG2, imm));
	return push_inst16(compiler, ADD_SP | RN3(TMP_REG2));
}

static sljit_s32 emit_stack_frame_release(struct sljit_compiler *compiler, sljit_s32 frame_size)
{
	sljit_s32 local_size, fscratches, fsaveds, i, tmp;
	sljit_s32 lr_dst = TMP_PC;
	sljit_uw reg_list;

	SLJIT_ASSERT(reg_map[TMP_REG2] == 14 && frame_size <= 128);

	local_size = compiler->local_size;
	fscratches = compiler->fscratches;
	fsaveds = compiler->fsaveds;

	if (fsaveds > 0 || fscratches >= SLJIT_FIRST_SAVED_FLOAT_REG) {
		if (local_size > 0)
			FAIL_IF(emit_add_sp(compiler, (sljit_uw)local_size));

		if (fsaveds + fscratches >= SLJIT_NUMBER_OF_FLOAT_REGISTERS) {
			FAIL_IF(push_inst32(compiler, VPOP | DD4(SLJIT_FS0) | ((sljit_uw)SLJIT_NUMBER_OF_SAVED_FLOAT_REGISTERS << 1)));
		} else {
			if (fscratches >= SLJIT_FIRST_SAVED_FLOAT_REG)
				FAIL_IF(push_inst32(compiler, VPOP | DD4(fscratches) | ((sljit_uw)(fscratches - (SLJIT_FIRST_SAVED_FLOAT_REG - 1)) << 1)));
			if (fsaveds > 0)
				FAIL_IF(push_inst32(compiler, VPOP | DD4(SLJIT_FS0) | ((sljit_uw)fsaveds << 1)));
		}

		local_size = GET_SAVED_REGISTERS_SIZE(compiler->scratches, compiler->saveds, 1) & 0x7;
	}

	if (frame_size < 0) {
		lr_dst = TMP_REG2;
		frame_size = 0;
	} else if (frame_size > 0)
		lr_dst = 0;

	reg_list = 0;
	tmp = SLJIT_S0 - compiler->saveds;
	for (i = SLJIT_S0; i > tmp; i--)
		reg_list |= (sljit_uw)1 << reg_map[i];

	for (i = compiler->scratches; i >= SLJIT_FIRST_SAVED_REG; i--)
		reg_list |= (sljit_uw)1 << reg_map[i];

	if (lr_dst == 0 && (reg_list & (reg_list - 1)) == 0) {
		/* The local_size does not include the saved registers. */
		local_size += SSIZE_OF(sw);

		if (reg_list != 0)
			local_size += SSIZE_OF(sw);

		if (frame_size > local_size)
			FAIL_IF(push_inst16(compiler, SUB_SP_I | ((sljit_uw)(frame_size - local_size) >> 2)));
		else if (frame_size < local_size)
			FAIL_IF(emit_add_sp(compiler, (sljit_uw)(local_size - frame_size)));

		if (reg_list == 0)
			return SLJIT_SUCCESS;

		if (compiler->saveds > 0) {
			SLJIT_ASSERT(reg_list == ((sljit_uw)1 << reg_map[SLJIT_S0]));
			lr_dst = SLJIT_S0;
		} else {
			SLJIT_ASSERT(reg_list == ((sljit_uw)1 << reg_map[SLJIT_FIRST_SAVED_REG]));
			lr_dst = SLJIT_FIRST_SAVED_REG;
		}

		frame_size -= 2 * SSIZE_OF(sw);

		if (reg_map[lr_dst] <= 7)
			return push_inst16(compiler, STR_SP | 0x800 | RDN3(lr_dst) | (sljit_uw)(frame_size >> 2));

		return push_inst32(compiler, LDR | RT4(lr_dst) | RN4(SLJIT_SP) | (sljit_uw)frame_size);
	}

	if (local_size > 0)
		FAIL_IF(emit_add_sp(compiler, (sljit_uw)local_size));

	if (!(reg_list & 0xff00) && lr_dst != TMP_REG2) {
		if (lr_dst == TMP_PC)
			reg_list |= 1u << 8;

		/* At least one register must be set for POP instruction. */
		SLJIT_ASSERT(reg_list != 0);

		FAIL_IF(push_inst16(compiler, POP | reg_list));
	} else {
		if (lr_dst != 0) {
			if (reg_list == 0)
				return push_inst32(compiler, 0xf85d0b04 | RT4(lr_dst));

			reg_list |= (sljit_uw)1 << reg_map[lr_dst];
		}

		/* At least two registers must be set for POP_W instruction. */
		SLJIT_ASSERT((reg_list & (reg_list - 1)) != 0);

		FAIL_IF(push_inst32(compiler, POP_W | reg_list));
	}

	if (frame_size > 0)
		return push_inst16(compiler, SUB_SP_I | (((sljit_uw)frame_size - sizeof(sljit_sw)) >> 2));
	return SLJIT_SUCCESS;
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_return_void(struct sljit_compiler *compiler)
{
	CHECK_ERROR();
	CHECK(check_sljit_emit_return_void(compiler));

	return emit_stack_frame_release(compiler, 0);
}

/* --------------------------------------------------------------------- */
/*  Operators                                                            */
/* --------------------------------------------------------------------- */

#if !(defined __ARM_FEATURE_IDIV) && !(defined __ARM_ARCH_EXT_IDIV__)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
extern unsigned long long __rt_udiv(unsigned int denominator, unsigned int numerator);
extern long long __rt_sdiv(int denominator, int numerator);
#elif defined(__GNUC__)
extern unsigned int __aeabi_uidivmod(unsigned int numerator, int unsigned denominator);
extern int __aeabi_idivmod(int numerator, int denominator);
#else
#error "Software divmod functions are needed"
#endif

#ifdef __cplusplus
}
#endif

#endif /* !__ARM_FEATURE_IDIV && !__ARM_ARCH_EXT_IDIV__ */

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_op0(struct sljit_compiler *compiler, sljit_s32 op)
{
#if !(defined __ARM_FEATURE_IDIV) && !(defined __ARM_ARCH_EXT_IDIV__)
	sljit_uw saved_reg_list[3];
	sljit_uw saved_reg_count;
#endif

	CHECK_ERROR();
	CHECK(check_sljit_emit_op0(compiler, op));

	op = GET_OPCODE(op);
	switch (op) {
	case SLJIT_BREAKPOINT:
		return push_inst16(compiler, BKPT);
	case SLJIT_NOP:
		return push_inst16(compiler, NOP);
	case SLJIT_LMUL_UW:
	case SLJIT_LMUL_SW:
		return push_inst32(compiler, (op == SLJIT_LMUL_UW ? UMULL : SMULL)
			| RD4(SLJIT_R1) | RT4(SLJIT_R0) | RN4(SLJIT_R0) | RM4(SLJIT_R1));
#if (defined __ARM_FEATURE_IDIV) || (defined __ARM_ARCH_EXT_IDIV__)
	case SLJIT_DIVMOD_UW:
	case SLJIT_DIVMOD_SW:
		FAIL_IF(push_inst16(compiler, MOV | SET_REGS44(TMP_REG1, SLJIT_R0)));
		FAIL_IF(push_inst32(compiler, (op == SLJIT_DIVMOD_UW ? UDIV : SDIV) | RD4(SLJIT_R0) | RN4(SLJIT_R0) | RM4(SLJIT_R1)));
		FAIL_IF(push_inst32(compiler, MUL | RD4(SLJIT_R1) | RN4(SLJIT_R0) | RM4(SLJIT_R1)));
		return push_inst32(compiler, SUB_W | RD4(SLJIT_R1) | RN4(TMP_REG1) | RM4(SLJIT_R1));
	case SLJIT_DIV_UW:
	case SLJIT_DIV_SW:
		return push_inst32(compiler, (op == SLJIT_DIV_UW ? UDIV : SDIV) | RD4(SLJIT_R0) | RN4(SLJIT_R0) | RM4(SLJIT_R1));
#else /* !__ARM_FEATURE_IDIV && !__ARM_ARCH_EXT_IDIV__ */
	case SLJIT_DIVMOD_UW:
	case SLJIT_DIVMOD_SW:
	case SLJIT_DIV_UW:
	case SLJIT_DIV_SW:
		SLJIT_COMPILE_ASSERT((SLJIT_DIVMOD_UW & 0x2) == 0 && SLJIT_DIV_UW - 0x2 == SLJIT_DIVMOD_UW, bad_div_opcode_assignments);
		SLJIT_ASSERT(reg_map[2] == 1 && reg_map[3] == 2 && reg_map[4] == 3);

		saved_reg_count = 0;
		if (compiler->scratches >= 4)
			saved_reg_list[saved_reg_count++] = 3;
		if (compiler->scratches >= 3)
			saved_reg_list[saved_reg_count++] = 2;
		if (op >= SLJIT_DIV_UW)
			saved_reg_list[saved_reg_count++] = 1;

		if (saved_reg_count > 0) {
			FAIL_IF(push_inst32(compiler, 0xf84d0d00 | (saved_reg_count >= 3 ? 16 : 8)
						| (saved_reg_list[0] << 12) /* str rX, [sp, #-8/-16]! */));
			if (saved_reg_count >= 2) {
				SLJIT_ASSERT(saved_reg_list[1] < 8);
				FAIL_IF(push_inst16(compiler, 0x9001 | (saved_reg_list[1] << 8) /* str rX, [sp, #4] */));
			}
			if (saved_reg_count >= 3) {
				SLJIT_ASSERT(saved_reg_list[2] < 8);
				FAIL_IF(push_inst16(compiler, 0x9002 | (saved_reg_list[2] << 8) /* str rX, [sp, #8] */));
			}
		}

#ifdef _WIN32
		FAIL_IF(push_inst16(compiler, MOV | SET_REGS44(TMP_REG1, SLJIT_R0)));
		FAIL_IF(push_inst16(compiler, MOV | SET_REGS44(SLJIT_R0, SLJIT_R1)));
		FAIL_IF(push_inst16(compiler, MOV | SET_REGS44(SLJIT_R1, TMP_REG1)));
		FAIL_IF(sljit_emit_ijump(compiler, SLJIT_FAST_CALL, SLJIT_IMM,
			((op | 0x2) == SLJIT_DIV_UW ? SLJIT_FUNC_ADDR(__rt_udiv) : SLJIT_FUNC_ADDR(__rt_sdiv))));
#elif defined(__GNUC__)
		FAIL_IF(sljit_emit_ijump(compiler, SLJIT_FAST_CALL, SLJIT_IMM,
			((op | 0x2) == SLJIT_DIV_UW ? SLJIT_FUNC_ADDR(__aeabi_uidivmod) : SLJIT_FUNC_ADDR(__aeabi_idivmod))));
#else
#error "Software divmod functions are needed"
#endif

		if (saved_reg_count > 0) {
			if (saved_reg_count >= 3) {
				SLJIT_ASSERT(saved_reg_list[2] < 8);
				FAIL_IF(push_inst16(compiler, 0x9802 | (saved_reg_list[2] << 8) /* ldr rX, [sp, #8] */));
			}
			if (saved_reg_count >= 2) {
				SLJIT_ASSERT(saved_reg_list[1] < 8);
				FAIL_IF(push_inst16(compiler, 0x9801 | (saved_reg_list[1] << 8) /* ldr rX, [sp, #4] */));
			}
			return push_inst32(compiler, 0xf85d0b00 | (saved_reg_count >= 3 ? 16 : 8)
						| (saved_reg_list[0] << 12) /* ldr rX, [sp], #8/16 */);
		}
		return SLJIT_SUCCESS;
#endif /* __ARM_FEATURE_IDIV || __ARM_ARCH_EXT_IDIV__ */
	case SLJIT_ENDBR:
	case SLJIT_SKIP_FRAMES_BEFORE_RETURN:
		return SLJIT_SUCCESS;
	}

	return SLJIT_SUCCESS;
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_op1(struct sljit_compiler *compiler, sljit_s32 op,
	sljit_s32 dst, sljit_sw dstw,
	sljit_s32 src, sljit_sw srcw)
{
	sljit_s32 dst_r, flags;
	sljit_s32 op_flags = GET_ALL_FLAGS(op);

	CHECK_ERROR();
	CHECK(check_sljit_emit_op1(compiler, op, dst, dstw, src, srcw));
	ADJUST_LOCAL_OFFSET(dst, dstw);
	ADJUST_LOCAL_OFFSET(src, srcw);

	dst_r = FAST_IS_REG(dst) ? dst : TMP_REG1;

	op = GET_OPCODE(op);
	if (op >= SLJIT_MOV && op <= SLJIT_MOV_P) {
		switch (op) {
		case SLJIT_MOV:
		case SLJIT_MOV_U32:
		case SLJIT_MOV_S32:
		case SLJIT_MOV32:
		case SLJIT_MOV_P:
			flags = WORD_SIZE;
			break;
		case SLJIT_MOV_U8:
			flags = BYTE_SIZE;
			if (src & SLJIT_IMM)
				srcw = (sljit_u8)srcw;
			break;
		case SLJIT_MOV_S8:
			flags = BYTE_SIZE | SIGNED;
			if (src & SLJIT_IMM)
				srcw = (sljit_s8)srcw;
			break;
		case SLJIT_MOV_U16:
			flags = HALF_SIZE;
			if (src & SLJIT_IMM)
				srcw = (sljit_u16)srcw;
			break;
		case SLJIT_MOV_S16:
			flags = HALF_SIZE | SIGNED;
			if (src & SLJIT_IMM)
				srcw = (sljit_s16)srcw;
			break;
		default:
			SLJIT_UNREACHABLE();
			flags = 0;
			break;
		}

		if (src & SLJIT_IMM)
			FAIL_IF(emit_op_imm(compiler, SLJIT_MOV | ARG2_IMM, dst_r, TMP_REG2, (sljit_uw)srcw));
		else if (src & SLJIT_MEM) {
			FAIL_IF(emit_op_mem(compiler, flags, dst_r, src, srcw, TMP_REG1));
		} else {
			if (dst_r != TMP_REG1)
				return emit_op_imm(compiler, op, dst_r, TMP_REG2, (sljit_uw)src);
			dst_r = src;
		}

		if (!(dst & SLJIT_MEM))
			return SLJIT_SUCCESS;

		return emit_op_mem(compiler, flags | STORE, dst_r, dst, dstw, TMP_REG2);
	}

	flags = HAS_FLAGS(op_flags) ? SET_FLAGS : 0;

	if (src & SLJIT_MEM) {
		FAIL_IF(emit_op_mem(compiler, WORD_SIZE, TMP_REG1, src, srcw, TMP_REG1));
		src = TMP_REG1;
	}

	emit_op_imm(compiler, flags | op, dst_r, TMP_REG2, (sljit_uw)src);

	if (SLJIT_UNLIKELY(dst & SLJIT_MEM))
		return emit_op_mem(compiler, flags | STORE, dst_r, dst, dstw, TMP_REG2);
	return SLJIT_SUCCESS;
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_op2(struct sljit_compiler *compiler, sljit_s32 op,
	sljit_s32 dst, sljit_sw dstw,
	sljit_s32 src1, sljit_sw src1w,
	sljit_s32 src2, sljit_sw src2w)
{
	sljit_s32 dst_reg, flags, src2_reg;

	CHECK_ERROR();
	CHECK(check_sljit_emit_op2(compiler, op, 0, dst, dstw, src1, src1w, src2, src2w));
	ADJUST_LOCAL_OFFSET(dst, dstw);
	ADJUST_LOCAL_OFFSET(src1, src1w);
	ADJUST_LOCAL_OFFSET(src2, src2w);

	dst_reg = FAST_IS_REG(dst) ? dst : TMP_REG1;
	flags = HAS_FLAGS(op) ? SET_FLAGS : 0;

	if (dst == TMP_REG1)
		flags |= UNUSED_RETURN;

	if (src1 & SLJIT_IMM)
		flags |= ARG1_IMM;
	else if (src1 & SLJIT_MEM) {
		emit_op_mem(compiler, WORD_SIZE, TMP_REG1, src1, src1w, TMP_REG1);
		src1w = TMP_REG1;
	}
	else
		src1w = src1;

	if (src2 & SLJIT_IMM)
		flags |= ARG2_IMM;
	else if (src2 & SLJIT_MEM) {
		src2_reg = (!(flags & ARG1_IMM) && (src1w == TMP_REG1)) ? TMP_REG2 : TMP_REG1;
		emit_op_mem(compiler, WORD_SIZE, src2_reg, src2, src2w, src2_reg);
		src2w = src2_reg;
	}
	else
		src2w = src2;

	emit_op_imm(compiler, flags | GET_OPCODE(op), dst_reg, (sljit_uw)src1w, (sljit_uw)src2w);

	if (!(dst & SLJIT_MEM))
		return SLJIT_SUCCESS;
	return emit_op_mem(compiler, WORD_SIZE | STORE, dst_reg, dst, dstw, TMP_REG2);
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_op2u(struct sljit_compiler *compiler, sljit_s32 op,
	sljit_s32 src1, sljit_sw src1w,
	sljit_s32 src2, sljit_sw src2w)
{
	CHECK_ERROR();
	CHECK(check_sljit_emit_op2(compiler, op, 1, 0, 0, src1, src1w, src2, src2w));

#if (defined SLJIT_VERBOSE && SLJIT_VERBOSE) \
		|| (defined SLJIT_ARGUMENT_CHECKS && SLJIT_ARGUMENT_CHECKS)
	compiler->skip_checks = 1;
#endif
	return sljit_emit_op2(compiler, op, TMP_REG1, 0, src1, src1w, src2, src2w);
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_op_src(struct sljit_compiler *compiler, sljit_s32 op,
	sljit_s32 src, sljit_sw srcw)
{
	CHECK_ERROR();
	CHECK(check_sljit_emit_op_src(compiler, op, src, srcw));
	ADJUST_LOCAL_OFFSET(src, srcw);

	switch (op) {
	case SLJIT_FAST_RETURN:
		SLJIT_ASSERT(reg_map[TMP_REG2] == 14);

		if (FAST_IS_REG(src))
			FAIL_IF(push_inst16(compiler, MOV | SET_REGS44(TMP_REG2, src)));
		else
			FAIL_IF(emit_op_mem(compiler, WORD_SIZE, TMP_REG2, src, srcw, TMP_REG2));

		return push_inst16(compiler, BX | RN3(TMP_REG2));
	case SLJIT_SKIP_FRAMES_BEFORE_FAST_RETURN:
		return SLJIT_SUCCESS;
	case SLJIT_PREFETCH_L1:
	case SLJIT_PREFETCH_L2:
	case SLJIT_PREFETCH_L3:
	case SLJIT_PREFETCH_ONCE:
		return emit_op_mem(compiler, PRELOAD, TMP_PC, src, srcw, TMP_REG1);
	}

	return SLJIT_SUCCESS;
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_get_register_index(sljit_s32 reg)
{
	CHECK_REG_INDEX(check_sljit_get_register_index(reg));
	return reg_map[reg];
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_get_float_register_index(sljit_s32 reg)
{
	CHECK_REG_INDEX(check_sljit_get_float_register_index(reg));
	return (freg_map[reg] << 1);
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_op_custom(struct sljit_compiler *compiler,
	void *instruction, sljit_u32 size)
{
	CHECK_ERROR();
	CHECK(check_sljit_emit_op_custom(compiler, instruction, size));

	if (size == 2)
		return push_inst16(compiler, *(sljit_u16*)instruction);
	return push_inst32(compiler, *(sljit_ins*)instruction);
}

/* --------------------------------------------------------------------- */
/*  Floating point operators                                             */
/* --------------------------------------------------------------------- */

#define FPU_LOAD (1 << 20)

static sljit_s32 emit_fop_mem(struct sljit_compiler *compiler, sljit_s32 flags, sljit_s32 reg, sljit_s32 arg, sljit_sw argw)
{
	sljit_uw imm;
	sljit_ins inst = VSTR_F32 | (flags & (SLJIT_32 | FPU_LOAD));

	SLJIT_ASSERT(arg & SLJIT_MEM);

	/* Fast loads and stores. */
	if (SLJIT_UNLIKELY(arg & OFFS_REG_MASK)) {
		FAIL_IF(push_inst32(compiler, ADD_W | RD4(TMP_REG1) | RN4(arg & REG_MASK) | RM4(OFFS_REG(arg)) | (((sljit_uw)argw & 0x3) << 6)));
		arg = SLJIT_MEM | TMP_REG1;
		argw = 0;
	}

	if ((arg & REG_MASK) && (argw & 0x3) == 0) {
		if (!(argw & ~0x3fc))
			return push_inst32(compiler, inst | 0x800000 | RN4(arg & REG_MASK) | DD4(reg) | ((sljit_uw)argw >> 2));
		if (!(-argw & ~0x3fc))
			return push_inst32(compiler, inst | RN4(arg & REG_MASK) | DD4(reg) | ((sljit_uw)-argw >> 2));
	}

	if (arg & REG_MASK) {
		if (emit_set_delta(compiler, TMP_REG1, arg & REG_MASK, argw) != SLJIT_ERR_UNSUPPORTED) {
			FAIL_IF(compiler->error);
			return push_inst32(compiler, inst | 0x800000 | RN4(TMP_REG1) | DD4(reg));
		}

		imm = get_imm((sljit_uw)argw & ~(sljit_uw)0x3fc);
		if (imm != INVALID_IMM) {
			FAIL_IF(push_inst32(compiler, ADD_WI | RD4(TMP_REG1) | RN4(arg & REG_MASK) | imm));
			return push_inst32(compiler, inst | 0x800000 | RN4(TMP_REG1) | DD4(reg) | (((sljit_uw)argw & 0x3fc) >> 2));
		}

		imm = get_imm((sljit_uw)-argw & ~(sljit_uw)0x3fc);
		if (imm != INVALID_IMM) {
			argw = -argw;
			FAIL_IF(push_inst32(compiler, SUB_WI | RD4(TMP_REG1) | RN4(arg & REG_MASK) | imm));
			return push_inst32(compiler, inst | RN4(TMP_REG1) | DD4(reg) | (((sljit_uw)argw & 0x3fc) >> 2));
		}
	}

	FAIL_IF(load_immediate(compiler, TMP_REG1, (sljit_uw)argw));
	if (arg & REG_MASK)
		FAIL_IF(push_inst16(compiler, ADD | SET_REGS44(TMP_REG1, (arg & REG_MASK))));
	return push_inst32(compiler, inst | 0x800000 | RN4(TMP_REG1) | DD4(reg));
}

static SLJIT_INLINE sljit_s32 sljit_emit_fop1_conv_sw_from_f64(struct sljit_compiler *compiler, sljit_s32 op,
	sljit_s32 dst, sljit_sw dstw,
	sljit_s32 src, sljit_sw srcw)
{
	op ^= SLJIT_32;

	if (src & SLJIT_MEM) {
		FAIL_IF(emit_fop_mem(compiler, (op & SLJIT_32) | FPU_LOAD, TMP_FREG1, src, srcw));
		src = TMP_FREG1;
	}

	FAIL_IF(push_inst32(compiler, VCVT_S32_F32 | (op & SLJIT_32) | DD4(TMP_FREG1) | DM4(src)));

	if (FAST_IS_REG(dst))
		return push_inst32(compiler, VMOV | (1 << 20) | RT4(dst) | DN4(TMP_FREG1));

	/* Store the integer value from a VFP register. */
	return emit_fop_mem(compiler, 0, TMP_FREG1, dst, dstw);
}

static SLJIT_INLINE sljit_s32 sljit_emit_fop1_conv_f64_from_sw(struct sljit_compiler *compiler, sljit_s32 op,
	sljit_s32 dst, sljit_sw dstw,
	sljit_s32 src, sljit_sw srcw)
{
	sljit_s32 dst_r = FAST_IS_REG(dst) ? dst : TMP_FREG1;

	op ^= SLJIT_32;

	if (FAST_IS_REG(src))
		FAIL_IF(push_inst32(compiler, VMOV | RT4(src) | DN4(TMP_FREG1)));
	else if (src & SLJIT_MEM) {
		/* Load the integer value into a VFP register. */
		FAIL_IF(emit_fop_mem(compiler, FPU_LOAD, TMP_FREG1, src, srcw));
	}
	else {
		FAIL_IF(load_immediate(compiler, TMP_REG1, (sljit_uw)srcw));
		FAIL_IF(push_inst32(compiler, VMOV | RT4(TMP_REG1) | DN4(TMP_FREG1)));
	}

	FAIL_IF(push_inst32(compiler, VCVT_F32_S32 | (op & SLJIT_32) | DD4(dst_r) | DM4(TMP_FREG1)));

	if (dst & SLJIT_MEM)
		return emit_fop_mem(compiler, (op & SLJIT_32), TMP_FREG1, dst, dstw);
	return SLJIT_SUCCESS;
}

static SLJIT_INLINE sljit_s32 sljit_emit_fop1_cmp(struct sljit_compiler *compiler, sljit_s32 op,
	sljit_s32 src1, sljit_sw src1w,
	sljit_s32 src2, sljit_sw src2w)
{
	op ^= SLJIT_32;

	if (src1 & SLJIT_MEM) {
		emit_fop_mem(compiler, (op & SLJIT_32) | FPU_LOAD, TMP_FREG1, src1, src1w);
		src1 = TMP_FREG1;
	}

	if (src2 & SLJIT_MEM) {
		emit_fop_mem(compiler, (op & SLJIT_32) | FPU_LOAD, TMP_FREG2, src2, src2w);
		src2 = TMP_FREG2;
	}

	FAIL_IF(push_inst32(compiler, VCMP_F32 | (op & SLJIT_32) | DD4(src1) | DM4(src2)));
	return push_inst32(compiler, VMRS);
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_fop1(struct sljit_compiler *compiler, sljit_s32 op,
	sljit_s32 dst, sljit_sw dstw,
	sljit_s32 src, sljit_sw srcw)
{
	sljit_s32 dst_r;

	CHECK_ERROR();

	SLJIT_COMPILE_ASSERT((SLJIT_32 == 0x100), float_transfer_bit_error);
	SELECT_FOP1_OPERATION_WITH_CHECKS(compiler, op, dst, dstw, src, srcw);

	dst_r = FAST_IS_REG(dst) ? dst : TMP_FREG1;

	if (GET_OPCODE(op) != SLJIT_CONV_F64_FROM_F32)
		op ^= SLJIT_32;

	if (src & SLJIT_MEM) {
		emit_fop_mem(compiler, (op & SLJIT_32) | FPU_LOAD, dst_r, src, srcw);
		src = dst_r;
	}

	switch (GET_OPCODE(op)) {
	case SLJIT_MOV_F64:
		if (src != dst_r) {
			if (dst_r != TMP_FREG1)
				FAIL_IF(push_inst32(compiler, VMOV_F32 | (op & SLJIT_32) | DD4(dst_r) | DM4(src)));
			else
				dst_r = src;
		}
		break;
	case SLJIT_NEG_F64:
		FAIL_IF(push_inst32(compiler, VNEG_F32 | (op & SLJIT_32) | DD4(dst_r) | DM4(src)));
		break;
	case SLJIT_ABS_F64:
		FAIL_IF(push_inst32(compiler, VABS_F32 | (op & SLJIT_32) | DD4(dst_r) | DM4(src)));
		break;
	case SLJIT_CONV_F64_FROM_F32:
		FAIL_IF(push_inst32(compiler, VCVT_F64_F32 | (op & SLJIT_32) | DD4(dst_r) | DM4(src)));
		op ^= SLJIT_32;
		break;
	}

	if (dst & SLJIT_MEM)
		return emit_fop_mem(compiler, (op & SLJIT_32), dst_r, dst, dstw);
	return SLJIT_SUCCESS;
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_fop2(struct sljit_compiler *compiler, sljit_s32 op,
	sljit_s32 dst, sljit_sw dstw,
	sljit_s32 src1, sljit_sw src1w,
	sljit_s32 src2, sljit_sw src2w)
{
	sljit_s32 dst_r;

	CHECK_ERROR();
	CHECK(check_sljit_emit_fop2(compiler, op, dst, dstw, src1, src1w, src2, src2w));
	ADJUST_LOCAL_OFFSET(dst, dstw);
	ADJUST_LOCAL_OFFSET(src1, src1w);
	ADJUST_LOCAL_OFFSET(src2, src2w);

	op ^= SLJIT_32;

	dst_r = FAST_IS_REG(dst) ? dst : TMP_FREG1;
	if (src1 & SLJIT_MEM) {
		emit_fop_mem(compiler, (op & SLJIT_32) | FPU_LOAD, TMP_FREG1, src1, src1w);
		src1 = TMP_FREG1;
	}
	if (src2 & SLJIT_MEM) {
		emit_fop_mem(compiler, (op & SLJIT_32) | FPU_LOAD, TMP_FREG2, src2, src2w);
		src2 = TMP_FREG2;
	}

	switch (GET_OPCODE(op)) {
	case SLJIT_ADD_F64:
		FAIL_IF(push_inst32(compiler, VADD_F32 | (op & SLJIT_32) | DD4(dst_r) | DN4(src1) | DM4(src2)));
		break;
	case SLJIT_SUB_F64:
		FAIL_IF(push_inst32(compiler, VSUB_F32 | (op & SLJIT_32) | DD4(dst_r) | DN4(src1) | DM4(src2)));
		break;
	case SLJIT_MUL_F64:
		FAIL_IF(push_inst32(compiler, VMUL_F32 | (op & SLJIT_32) | DD4(dst_r) | DN4(src1) | DM4(src2)));
		break;
	case SLJIT_DIV_F64:
		FAIL_IF(push_inst32(compiler, VDIV_F32 | (op & SLJIT_32) | DD4(dst_r) | DN4(src1) | DM4(src2)));
		break;
	}

	if (!(dst & SLJIT_MEM))
		return SLJIT_SUCCESS;
	return emit_fop_mem(compiler, (op & SLJIT_32), TMP_FREG1, dst, dstw);
}

#undef FPU_LOAD

/* --------------------------------------------------------------------- */
/*  Other instructions                                                   */
/* --------------------------------------------------------------------- */

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_fast_enter(struct sljit_compiler *compiler, sljit_s32 dst, sljit_sw dstw)
{
	CHECK_ERROR();
	CHECK(check_sljit_emit_fast_enter(compiler, dst, dstw));
	ADJUST_LOCAL_OFFSET(dst, dstw);

	SLJIT_ASSERT(reg_map[TMP_REG2] == 14);

	if (FAST_IS_REG(dst))
		return push_inst16(compiler, MOV | SET_REGS44(dst, TMP_REG2));

	/* Memory. */
	return emit_op_mem(compiler, WORD_SIZE | STORE, TMP_REG2, dst, dstw, TMP_REG1);
}

/* --------------------------------------------------------------------- */
/*  Conditional instructions                                             */
/* --------------------------------------------------------------------- */

static sljit_uw get_cc(struct sljit_compiler *compiler, sljit_s32 type)
{
	switch (type) {
	case SLJIT_EQUAL:
	case SLJIT_EQUAL_F64:
		return 0x0;

	case SLJIT_NOT_EQUAL:
	case SLJIT_NOT_EQUAL_F64:
		return 0x1;

	case SLJIT_CARRY:
		if (compiler->status_flags_state & SLJIT_CURRENT_FLAGS_ADD)
			return 0x2;
		/* fallthrough */

	case SLJIT_LESS:
	case SLJIT_LESS_F64:
		return 0x3;

	case SLJIT_NOT_CARRY:
		if (compiler->status_flags_state & SLJIT_CURRENT_FLAGS_ADD)
			return 0x3;
		/* fallthrough */

	case SLJIT_GREATER_EQUAL:
	case SLJIT_GREATER_EQUAL_F64:
		return 0x2;

	case SLJIT_GREATER:
	case SLJIT_GREATER_F64:
		return 0x8;

	case SLJIT_LESS_EQUAL:
	case SLJIT_LESS_EQUAL_F64:
		return 0x9;

	case SLJIT_SIG_LESS:
		return 0xb;

	case SLJIT_SIG_GREATER_EQUAL:
		return 0xa;

	case SLJIT_SIG_GREATER:
		return 0xc;

	case SLJIT_SIG_LESS_EQUAL:
		return 0xd;

	case SLJIT_OVERFLOW:
		if (!(compiler->status_flags_state & (SLJIT_CURRENT_FLAGS_ADD | SLJIT_CURRENT_FLAGS_SUB)))
			return 0x1;
		/* fallthrough */

	case SLJIT_UNORDERED_F64:
		return 0x6;

	case SLJIT_NOT_OVERFLOW:
		if (!(compiler->status_flags_state & (SLJIT_CURRENT_FLAGS_ADD | SLJIT_CURRENT_FLAGS_SUB)))
			return 0x0;
		/* fallthrough */

	case SLJIT_ORDERED_F64:
		return 0x7;

	default: /* SLJIT_JUMP */
		SLJIT_UNREACHABLE();
		return 0xe;
	}
}

SLJIT_API_FUNC_ATTRIBUTE struct sljit_label* sljit_emit_label(struct sljit_compiler *compiler)
{
	struct sljit_label *label;

	CHECK_ERROR_PTR();
	CHECK_PTR(check_sljit_emit_label(compiler));

	if (compiler->last_label && compiler->last_label->size == compiler->size)
		return compiler->last_label;

	label = (struct sljit_label*)ensure_abuf(compiler, sizeof(struct sljit_label));
	PTR_FAIL_IF(!label);
	set_label(label, compiler);
	return label;
}

SLJIT_API_FUNC_ATTRIBUTE struct sljit_jump* sljit_emit_jump(struct sljit_compiler *compiler, sljit_s32 type)
{
	struct sljit_jump *jump;
	sljit_ins cc;

	CHECK_ERROR_PTR();
	CHECK_PTR(check_sljit_emit_jump(compiler, type));

	jump = (struct sljit_jump*)ensure_abuf(compiler, sizeof(struct sljit_jump));
	PTR_FAIL_IF(!jump);
	set_jump(jump, compiler, type & SLJIT_REWRITABLE_JUMP);
	type &= 0xff;

	PTR_FAIL_IF(emit_imm32_const(compiler, TMP_REG1, 0));
	if (type < SLJIT_JUMP) {
		jump->flags |= IS_COND;
		cc = get_cc(compiler, type);
		jump->flags |= cc << 8;
		PTR_FAIL_IF(push_inst16(compiler, IT | (cc << 4) | 0x8));
	}

	jump->addr = compiler->size;
	if (type <= SLJIT_JUMP)
		PTR_FAIL_IF(push_inst16(compiler, BX | RN3(TMP_REG1)));
	else {
		jump->flags |= IS_BL;
		PTR_FAIL_IF(push_inst16(compiler, BLX | RN3(TMP_REG1)));
	}

	return jump;
}

#ifdef __SOFTFP__

static sljit_s32 softfloat_call_with_args(struct sljit_compiler *compiler, sljit_s32 arg_types, sljit_s32 *src, sljit_u32 *extra_space)
{
	sljit_u32 is_tail_call = *extra_space & SLJIT_CALL_RETURN;
	sljit_u32 offset = 0;
	sljit_u32 word_arg_offset = 0;
	sljit_u32 float_arg_count = 0;
	sljit_s32 types = 0;
	sljit_u32 src_offset = 4 * sizeof(sljit_sw);
	sljit_u8 offsets[4];
	sljit_u8 *offset_ptr = offsets;

	if (src && FAST_IS_REG(*src))
		src_offset = (sljit_u32)reg_map[*src] * sizeof(sljit_sw);

	arg_types >>= SLJIT_ARG_SHIFT;

	while (arg_types) {
		types = (types << SLJIT_ARG_SHIFT) | (arg_types & SLJIT_ARG_MASK);

		switch (arg_types & SLJIT_ARG_MASK) {
		case SLJIT_ARG_TYPE_F64:
			if (offset & 0x7)
				offset += sizeof(sljit_sw);
			*offset_ptr++ = (sljit_u8)offset;
			offset += sizeof(sljit_f64);
			float_arg_count++;
			break;
		case SLJIT_ARG_TYPE_F32:
			*offset_ptr++ = (sljit_u8)offset;
			offset += sizeof(sljit_f32);
			float_arg_count++;
			break;
		default:
			*offset_ptr++ = (sljit_u8)offset;
			offset += sizeof(sljit_sw);
			word_arg_offset += sizeof(sljit_sw);
			break;
		}

		arg_types >>= SLJIT_ARG_SHIFT;
	}

	if (offset > 4 * sizeof(sljit_sw) && (!is_tail_call || offset > compiler->args_size)) {
		/* Keep lr register on the stack. */
		if (is_tail_call)
			offset += sizeof(sljit_sw);

		offset = ((offset - 4 * sizeof(sljit_sw)) + 0x7) & ~(sljit_uw)0x7;

		*extra_space = offset;

		if (is_tail_call)
			FAIL_IF(emit_stack_frame_release(compiler, (sljit_s32)offset));
		else
			FAIL_IF(push_inst16(compiler, SUB_SP_I | (offset >> 2)));
	} else {
		if (is_tail_call)
			FAIL_IF(emit_stack_frame_release(compiler, -1));
		*extra_space = 0;
	}

	SLJIT_ASSERT(reg_map[TMP_REG1] == 12);

	/* Process arguments in reversed direction. */
	while (types) {
		switch (types & SLJIT_ARG_MASK) {
		case SLJIT_ARG_TYPE_F64:
			float_arg_count--;
			offset = *(--offset_ptr);

			SLJIT_ASSERT((offset & 0x7) == 0);

			if (offset < 4 * sizeof(sljit_sw)) {
				if (src_offset == offset || src_offset == offset + sizeof(sljit_sw)) {
					FAIL_IF(push_inst16(compiler, MOV | (src_offset << 1) | 4 | (1 << 7)));
					*src = TMP_REG1;
				}
				FAIL_IF(push_inst32(compiler, VMOV2 | 0x100000 | (offset << 10) | ((offset + sizeof(sljit_sw)) << 14) | float_arg_count));
			} else
				FAIL_IF(push_inst32(compiler, VSTR_F32 | 0x800100 | RN4(SLJIT_SP)
						| (float_arg_count << 12) | ((offset - 4 * sizeof(sljit_sw)) >> 2)));
			break;
		case SLJIT_ARG_TYPE_F32:
			float_arg_count--;
			offset = *(--offset_ptr);

			if (offset < 4 * sizeof(sljit_sw)) {
				if (src_offset == offset) {
					FAIL_IF(push_inst16(compiler, MOV | (src_offset << 1) | 4 | (1 << 7)));
					*src = TMP_REG1;
				}
				FAIL_IF(push_inst32(compiler, VMOV | 0x100000 | (float_arg_count << 16) | (offset << 10)));
			} else
				FAIL_IF(push_inst32(compiler, VSTR_F32 | 0x800000 | RN4(SLJIT_SP)
						| (float_arg_count << 12) | ((offset - 4 * sizeof(sljit_sw)) >> 2)));
			break;
		default:
			word_arg_offset -= sizeof(sljit_sw);
			offset = *(--offset_ptr);

			SLJIT_ASSERT(offset >= word_arg_offset);

			if (offset != word_arg_offset) {
				if (offset < 4 * sizeof(sljit_sw)) {
					if (src_offset == offset) {
						FAIL_IF(push_inst16(compiler, MOV | (src_offset << 1) | 4 | (1 << 7)));
						*src = TMP_REG1;
					}
					else if (src_offset == word_arg_offset) {
						*src = (sljit_s32)(1 + (offset >> 2));
						src_offset = offset;
					}
					FAIL_IF(push_inst16(compiler, MOV | (offset >> 2) | (word_arg_offset << 1)));
				} else
					FAIL_IF(push_inst16(compiler, STR_SP | (word_arg_offset << 6) | ((offset - 4 * sizeof(sljit_sw)) >> 2)));
			}
			break;
		}

		types >>= SLJIT_ARG_SHIFT;
	}

	return SLJIT_SUCCESS;
}

static sljit_s32 softfloat_post_call_with_args(struct sljit_compiler *compiler, sljit_s32 arg_types)
{
	if ((arg_types & SLJIT_ARG_MASK) == SLJIT_ARG_TYPE_F64)
		FAIL_IF(push_inst32(compiler, VMOV2 | (1 << 16) | (0 << 12) | 0));
	if ((arg_types & SLJIT_ARG_MASK) == SLJIT_ARG_TYPE_F32)
		FAIL_IF(push_inst32(compiler, VMOV | (0 << 16) | (0 << 12)));

	return SLJIT_SUCCESS;
}

#else

static sljit_s32 hardfloat_call_with_args(struct sljit_compiler *compiler, sljit_s32 arg_types)
{
	sljit_u32 offset = SLJIT_FR0;
	sljit_u32 new_offset = SLJIT_FR0;
	sljit_u32 f32_offset = 0;

	/* Remove return value. */
	arg_types >>= SLJIT_ARG_SHIFT;

	while (arg_types) {
		switch (arg_types & SLJIT_ARG_MASK) {
		case SLJIT_ARG_TYPE_F64:
			if (offset != new_offset)
				FAIL_IF(push_inst32(compiler, VMOV_F32 | SLJIT_32 | DD4(new_offset) | DM4(offset)));

			new_offset++;
			offset++;
			break;
		case SLJIT_ARG_TYPE_F32:
			if (f32_offset != 0) {
				FAIL_IF(push_inst32(compiler, VMOV_F32 | 0x400000 | DD4(f32_offset) | DM4(offset)));
				f32_offset = 0;
			} else {
				if (offset != new_offset)
					FAIL_IF(push_inst32(compiler, VMOV_F32 | 0x400000 | DD4(new_offset) | DM4(offset)));
				f32_offset = new_offset;
				new_offset++;
			}
			offset++;
			break;
		}
		arg_types >>= SLJIT_ARG_SHIFT;
	}

	return SLJIT_SUCCESS;
}

#endif

SLJIT_API_FUNC_ATTRIBUTE struct sljit_jump* sljit_emit_call(struct sljit_compiler *compiler, sljit_s32 type,
	sljit_s32 arg_types)
{
#ifdef __SOFTFP__
	struct sljit_jump *jump;
	sljit_u32 extra_space = (sljit_u32)type;
#endif

	CHECK_ERROR_PTR();
	CHECK_PTR(check_sljit_emit_call(compiler, type, arg_types));

#ifdef __SOFTFP__
	PTR_FAIL_IF(softfloat_call_with_args(compiler, arg_types, NULL, &extra_space));
	SLJIT_ASSERT((extra_space & 0x7) == 0);

	if ((type & SLJIT_CALL_RETURN) && extra_space == 0)
		type = SLJIT_JUMP | (type & SLJIT_REWRITABLE_JUMP);

#if (defined SLJIT_VERBOSE && SLJIT_VERBOSE) \
		|| (defined SLJIT_ARGUMENT_CHECKS && SLJIT_ARGUMENT_CHECKS)
	compiler->skip_checks = 1;
#endif

	jump = sljit_emit_jump(compiler, type);
	PTR_FAIL_IF(jump == NULL);

	if (extra_space > 0) {
		if (type & SLJIT_CALL_RETURN)
			PTR_FAIL_IF(push_inst32(compiler, LDR | RT4(TMP_REG2)
				| RN4(SLJIT_SP) | (extra_space - sizeof(sljit_sw))));

		PTR_FAIL_IF(push_inst16(compiler, ADD_SP_I | (extra_space >> 2)));

		if (type & SLJIT_CALL_RETURN) {
			PTR_FAIL_IF(push_inst16(compiler, BX | RN3(TMP_REG2)));
			return jump;
		}
	}

	SLJIT_ASSERT(!(type & SLJIT_CALL_RETURN));
	PTR_FAIL_IF(softfloat_post_call_with_args(compiler, arg_types));
	return jump;
#else
	if (type & SLJIT_CALL_RETURN) {
		/* ldmia sp!, {..., lr} */
		PTR_FAIL_IF(emit_stack_frame_release(compiler, -1));
		type = SLJIT_JUMP | (type & SLJIT_REWRITABLE_JUMP);
	}

	PTR_FAIL_IF(hardfloat_call_with_args(compiler, arg_types));

#if (defined SLJIT_VERBOSE && SLJIT_VERBOSE) \
		|| (defined SLJIT_ARGUMENT_CHECKS && SLJIT_ARGUMENT_CHECKS)
	compiler->skip_checks = 1;
#endif

	return sljit_emit_jump(compiler, type);
#endif
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_ijump(struct sljit_compiler *compiler, sljit_s32 type, sljit_s32 src, sljit_sw srcw)
{
	struct sljit_jump *jump;

	CHECK_ERROR();
	CHECK(check_sljit_emit_ijump(compiler, type, src, srcw));
	ADJUST_LOCAL_OFFSET(src, srcw);

	SLJIT_ASSERT(reg_map[TMP_REG1] != 14);

	if (!(src & SLJIT_IMM)) {
		if (FAST_IS_REG(src)) {
			SLJIT_ASSERT(reg_map[src] != 14);
			return push_inst16(compiler, (type <= SLJIT_JUMP ? BX : BLX) | RN3(src));
		}

		FAIL_IF(emit_op_mem(compiler, WORD_SIZE, type <= SLJIT_JUMP ? TMP_PC : TMP_REG1, src, srcw, TMP_REG1));
		if (type >= SLJIT_FAST_CALL)
			return push_inst16(compiler, BLX | RN3(TMP_REG1));
	}

	/* These jumps are converted to jump/call instructions when possible. */
	jump = (struct sljit_jump*)ensure_abuf(compiler, sizeof(struct sljit_jump));
	FAIL_IF(!jump);
	set_jump(jump, compiler, JUMP_ADDR | ((type >= SLJIT_FAST_CALL) ? IS_BL : 0));
	jump->u.target = (sljit_uw)srcw;

	FAIL_IF(emit_imm32_const(compiler, TMP_REG1, 0));
	jump->addr = compiler->size;
	return push_inst16(compiler, (type <= SLJIT_JUMP ? BX : BLX) | RN3(TMP_REG1));
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_icall(struct sljit_compiler *compiler, sljit_s32 type,
	sljit_s32 arg_types,
	sljit_s32 src, sljit_sw srcw)
{
#ifdef __SOFTFP__
	sljit_u32 extra_space = (sljit_u32)type;
#endif

	CHECK_ERROR();
	CHECK(check_sljit_emit_icall(compiler, type, arg_types, src, srcw));

	if (src & SLJIT_MEM) {
		FAIL_IF(emit_op_mem(compiler, WORD_SIZE, TMP_REG1, src, srcw, TMP_REG1));
		src = TMP_REG1;
	}

	if ((type & SLJIT_CALL_RETURN) && (src >= SLJIT_FIRST_SAVED_REG && src <= SLJIT_S0)) {
		FAIL_IF(push_inst16(compiler, MOV | SET_REGS44(TMP_REG1, src)));
		src = TMP_REG1;
	}

#ifdef __SOFTFP__
	FAIL_IF(softfloat_call_with_args(compiler, arg_types, &src, &extra_space));
	SLJIT_ASSERT((extra_space & 0x7) == 0);

	if ((type & SLJIT_CALL_RETURN) && extra_space == 0)
		type = SLJIT_JUMP;

#if (defined SLJIT_VERBOSE && SLJIT_VERBOSE) \
		|| (defined SLJIT_ARGUMENT_CHECKS && SLJIT_ARGUMENT_CHECKS)
	compiler->skip_checks = 1;
#endif

	FAIL_IF(sljit_emit_ijump(compiler, type, src, srcw));

	if (extra_space > 0) {
		if (type & SLJIT_CALL_RETURN)
			FAIL_IF(push_inst32(compiler, LDR | RT4(TMP_REG2)
				| RN4(SLJIT_SP) | (extra_space - sizeof(sljit_sw))));

		FAIL_IF(push_inst16(compiler, ADD_SP_I | (extra_space >> 2)));

		if (type & SLJIT_CALL_RETURN)
			return push_inst16(compiler, BX | RN3(TMP_REG2));
	}

	SLJIT_ASSERT(!(type & SLJIT_CALL_RETURN));
	return softfloat_post_call_with_args(compiler, arg_types);
#else /* !__SOFTFP__ */
	if (type & SLJIT_CALL_RETURN) {
		/* ldmia sp!, {..., lr} */
		FAIL_IF(emit_stack_frame_release(compiler, -1));
		type = SLJIT_JUMP;
	}

	FAIL_IF(hardfloat_call_with_args(compiler, arg_types));

#if (defined SLJIT_VERBOSE && SLJIT_VERBOSE) \
		|| (defined SLJIT_ARGUMENT_CHECKS && SLJIT_ARGUMENT_CHECKS)
	compiler->skip_checks = 1;
#endif

	return sljit_emit_ijump(compiler, type, src, srcw);
#endif /* __SOFTFP__ */
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_op_flags(struct sljit_compiler *compiler, sljit_s32 op,
	sljit_s32 dst, sljit_sw dstw,
	sljit_s32 type)
{
	sljit_s32 dst_r, flags = GET_ALL_FLAGS(op);
	sljit_ins cc;

	CHECK_ERROR();
	CHECK(check_sljit_emit_op_flags(compiler, op, dst, dstw, type));
	ADJUST_LOCAL_OFFSET(dst, dstw);

	op = GET_OPCODE(op);
	cc = get_cc(compiler, type & 0xff);
	dst_r = FAST_IS_REG(dst) ? dst : TMP_REG1;

	if (op < SLJIT_ADD) {
		FAIL_IF(push_inst16(compiler, IT | (cc << 4) | (((cc & 0x1) ^ 0x1) << 3) | 0x4));
		if (reg_map[dst_r] > 7) {
			FAIL_IF(push_inst32(compiler, MOV_WI | RD4(dst_r) | 1));
			FAIL_IF(push_inst32(compiler, MOV_WI | RD4(dst_r) | 0));
		} else {
			/* The movsi (immediate) instruction does not set flags in IT block. */
			FAIL_IF(push_inst16(compiler, MOVSI | RDN3(dst_r) | 1));
			FAIL_IF(push_inst16(compiler, MOVSI | RDN3(dst_r) | 0));
		}
		if (!(dst & SLJIT_MEM))
			return SLJIT_SUCCESS;
		return emit_op_mem(compiler, WORD_SIZE | STORE, TMP_REG1, dst, dstw, TMP_REG2);
	}

	if (dst & SLJIT_MEM)
		FAIL_IF(emit_op_mem(compiler, WORD_SIZE, TMP_REG1, dst, dstw, TMP_REG2));

	if (op == SLJIT_AND) {
		FAIL_IF(push_inst16(compiler, IT | (cc << 4) | (((cc & 0x1) ^ 0x1) << 3) | 0x4));
		FAIL_IF(push_inst32(compiler, ANDI | RN4(dst_r) | RD4(dst_r) | 1));
		FAIL_IF(push_inst32(compiler, ANDI | RN4(dst_r) | RD4(dst_r) | 0));
	}
	else {
		FAIL_IF(push_inst16(compiler, IT | (cc << 4) | 0x8));
		FAIL_IF(push_inst32(compiler, ((op == SLJIT_OR) ? ORRI : EORI) | RN4(dst_r) | RD4(dst_r) | 1));
	}

	if (dst & SLJIT_MEM)
		FAIL_IF(emit_op_mem(compiler, WORD_SIZE | STORE, TMP_REG1, dst, dstw, TMP_REG2));

	if (!(flags & SLJIT_SET_Z))
		return SLJIT_SUCCESS;

	/* The condition must always be set, even if the ORR/EORI is not executed above. */
	return push_inst32(compiler, MOV_W | SET_FLAGS | RD4(TMP_REG1) | RM4(dst_r));
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_cmov(struct sljit_compiler *compiler, sljit_s32 type,
	sljit_s32 dst_reg,
	sljit_s32 src, sljit_sw srcw)
{
	sljit_uw cc, tmp;

	CHECK_ERROR();
	CHECK(check_sljit_emit_cmov(compiler, type, dst_reg, src, srcw));

	dst_reg &= ~SLJIT_32;

	cc = get_cc(compiler, type & 0xff);

	if (!(src & SLJIT_IMM)) {
		FAIL_IF(push_inst16(compiler, IT | (cc << 4) | 0x8));
		return push_inst16(compiler, MOV | SET_REGS44(dst_reg, src));
	}

	tmp = (sljit_uw) srcw;

	if (tmp < 0x10000) {
		/* set low 16 bits, set hi 16 bits to 0. */
		FAIL_IF(push_inst16(compiler, IT | (cc << 4) | 0x8));
		return push_inst32(compiler, MOVW | RD4(dst_reg)
			| COPY_BITS(tmp, 12, 16, 4) | COPY_BITS(tmp, 11, 26, 1) | COPY_BITS(tmp, 8, 12, 3) | (tmp & 0xff));
	}

	tmp = get_imm((sljit_uw)srcw);
	if (tmp != INVALID_IMM) {
		FAIL_IF(push_inst16(compiler, IT | (cc << 4) | 0x8));
		return push_inst32(compiler, MOV_WI | RD4(dst_reg) | tmp);
	}

	tmp = get_imm(~(sljit_uw)srcw);
	if (tmp != INVALID_IMM) {
		FAIL_IF(push_inst16(compiler, IT | (cc << 4) | 0x8));
		return push_inst32(compiler, MVN_WI | RD4(dst_reg) | tmp);
	}

	FAIL_IF(push_inst16(compiler, IT | (cc << 4) | ((cc & 0x1) << 3) | 0x4));

	tmp = (sljit_uw) srcw;
	FAIL_IF(push_inst32(compiler, MOVW | RD4(dst_reg)
		| COPY_BITS(tmp, 12, 16, 4) | COPY_BITS(tmp, 11, 26, 1) | COPY_BITS(tmp, 8, 12, 3) | (tmp & 0xff)));
	return push_inst32(compiler, MOVT | RD4(dst_reg)
		| COPY_BITS(tmp, 12 + 16, 16, 4) | COPY_BITS(tmp, 11 + 16, 26, 1) | COPY_BITS(tmp, 8 + 16, 12, 3) | ((tmp & 0xff0000) >> 16));
}

SLJIT_API_FUNC_ATTRIBUTE sljit_s32 sljit_emit_mem(struct sljit_compiler *compiler, sljit_s32 type,
	sljit_s32 reg,
	sljit_s32 mem, sljit_sw memw)
{
	sljit_s32 flags;
	sljit_ins inst;

	CHECK_ERROR();
	CHECK(check_sljit_emit_mem(compiler, type, reg, mem, memw));

	if ((mem & OFFS_REG_MASK) || (memw > 255 || memw < -255))
		return SLJIT_ERR_UNSUPPORTED;

	if (type & SLJIT_MEM_SUPP)
		return SLJIT_SUCCESS;

	switch (type & 0xff) {
	case SLJIT_MOV:
	case SLJIT_MOV_U32:
	case SLJIT_MOV_S32:
	case SLJIT_MOV32:
	case SLJIT_MOV_P:
		flags = WORD_SIZE;
		break;
	case SLJIT_MOV_U8:
		flags = BYTE_SIZE;
		break;
	case SLJIT_MOV_S8:
		flags = BYTE_SIZE | SIGNED;
		break;
	case SLJIT_MOV_U16:
		flags = HALF_SIZE;
		break;
	case SLJIT_MOV_S16:
		flags = HALF_SIZE | SIGNED;
		break;
	default:
		SLJIT_UNREACHABLE();
		flags = WORD_SIZE;
		break;
	}

	if (type & SLJIT_MEM_STORE)
		flags |= STORE;

	inst = sljit_mem32[flags] | 0x900;

	if (type & SLJIT_MEM_PRE)
		inst |= 0x400;

	if (memw >= 0)
		inst |= 0x200;
	else
		memw = -memw;

	return push_inst32(compiler, inst | RT4(reg) | RN4(mem & REG_MASK) | (sljit_ins)memw);
}

SLJIT_API_FUNC_ATTRIBUTE struct sljit_const* sljit_emit_const(struct sljit_compiler *compiler, sljit_s32 dst, sljit_sw dstw, sljit_sw init_value)
{
	struct sljit_const *const_;
	sljit_s32 dst_r;

	CHECK_ERROR_PTR();
	CHECK_PTR(check_sljit_emit_const(compiler, dst, dstw, init_value));
	ADJUST_LOCAL_OFFSET(dst, dstw);

	const_ = (struct sljit_const*)ensure_abuf(compiler, sizeof(struct sljit_const));
	PTR_FAIL_IF(!const_);
	set_const(const_, compiler);

	dst_r = FAST_IS_REG(dst) ? dst : TMP_REG1;
	PTR_FAIL_IF(emit_imm32_const(compiler, dst_r, (sljit_uw)init_value));

	if (dst & SLJIT_MEM)
		PTR_FAIL_IF(emit_op_mem(compiler, WORD_SIZE | STORE, dst_r, dst, dstw, TMP_REG2));
	return const_;
}

SLJIT_API_FUNC_ATTRIBUTE struct sljit_put_label* sljit_emit_put_label(struct sljit_compiler *compiler, sljit_s32 dst, sljit_sw dstw)
{
	struct sljit_put_label *put_label;
	sljit_s32 dst_r;

	CHECK_ERROR_PTR();
	CHECK_PTR(check_sljit_emit_put_label(compiler, dst, dstw));
	ADJUST_LOCAL_OFFSET(dst, dstw);

	put_label = (struct sljit_put_label*)ensure_abuf(compiler, sizeof(struct sljit_put_label));
	PTR_FAIL_IF(!put_label);
	set_put_label(put_label, compiler, 0);

	dst_r = FAST_IS_REG(dst) ? dst : TMP_REG1;
	PTR_FAIL_IF(emit_imm32_const(compiler, dst_r, 0));

	if (dst & SLJIT_MEM)
		PTR_FAIL_IF(emit_op_mem(compiler, WORD_SIZE | STORE, dst_r, dst, dstw, TMP_REG2));
	return put_label;
}

SLJIT_API_FUNC_ATTRIBUTE void sljit_set_jump_addr(sljit_uw addr, sljit_uw new_target, sljit_sw executable_offset)
{
	sljit_u16 *inst = (sljit_u16*)addr;
	SLJIT_UNUSED_ARG(executable_offset);

	SLJIT_UPDATE_WX_FLAGS(inst, inst + 4, 0);
	modify_imm32_const(inst, new_target);
	SLJIT_UPDATE_WX_FLAGS(inst, inst + 4, 1);
	inst = (sljit_u16 *)SLJIT_ADD_EXEC_OFFSET(inst, executable_offset);
	SLJIT_CACHE_FLUSH(inst, inst + 4);
}

SLJIT_API_FUNC_ATTRIBUTE void sljit_set_const(sljit_uw addr, sljit_sw new_constant, sljit_sw executable_offset)
{
	sljit_set_jump_addr(addr, (sljit_uw)new_constant, executable_offset);
}
