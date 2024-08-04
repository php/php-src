/*
 * IR - Lightweight JIT Compilation Framework
 * (Folding engine rules)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 *
 * Based on Mike Pall's implementation for LuaJIT.
 */

/* Constant Folding */
IR_FOLD(EQ(C_BOOL, C_BOOL))
IR_FOLD(EQ(C_U8, C_U8))
IR_FOLD(EQ(C_U16, C_U16))
IR_FOLD(EQ(C_U32, C_U32))
IR_FOLD(EQ(C_U64, C_U64))
IR_FOLD(EQ(C_ADDR, C_ADDR))
IR_FOLD(EQ(C_CHAR, C_CHAR))
IR_FOLD(EQ(C_I8, C_I8))
IR_FOLD(EQ(C_I16, C_I16))
IR_FOLD(EQ(C_I32, C_I32))
IR_FOLD(EQ(C_I64, C_I64))
{
	IR_FOLD_BOOL(op1_insn->val.u64 == op2_insn->val.u64);
}

IR_FOLD(EQ(C_DOUBLE, C_DOUBLE))
{
	IR_FOLD_BOOL(op1_insn->val.d == op2_insn->val.d);
}

IR_FOLD(EQ(C_FLOAT, C_FLOAT))
{
	IR_FOLD_BOOL(op1_insn->val.d == op2_insn->val.d);
}

IR_FOLD(NE(C_BOOL, C_BOOL))
IR_FOLD(NE(C_U8, C_U8))
IR_FOLD(NE(C_U16, C_U16))
IR_FOLD(NE(C_U32, C_U32))
IR_FOLD(NE(C_U64, C_U64))
IR_FOLD(NE(C_ADDR, C_ADDR))
IR_FOLD(NE(C_CHAR, C_CHAR))
IR_FOLD(NE(C_I8, C_I8))
IR_FOLD(NE(C_I16, C_I16))
IR_FOLD(NE(C_I32, C_I32))
IR_FOLD(NE(C_I64, C_I64))
{
	IR_FOLD_BOOL(op1_insn->val.u64 != op2_insn->val.u64);
}

IR_FOLD(NE(C_DOUBLE, C_DOUBLE))
{
	IR_FOLD_BOOL(op1_insn->val.d != op2_insn->val.d);
}

IR_FOLD(NE(C_FLOAT, C_FLOAT))
{
	IR_FOLD_BOOL(op1_insn->val.f != op2_insn->val.f);
}

IR_FOLD(LT(C_BOOL, C_BOOL))
IR_FOLD(LT(C_U8, C_U8))
IR_FOLD(LT(C_U16, C_U16))
IR_FOLD(LT(C_U32, C_U32))
IR_FOLD(LT(C_U64, C_U64))
IR_FOLD(LT(C_ADDR, C_ADDR))
{
	IR_FOLD_BOOL(op1_insn->val.u64 < op2_insn->val.u64);
}

IR_FOLD(LT(C_CHAR, C_CHAR))
IR_FOLD(LT(C_I8, C_I8))
IR_FOLD(LT(C_I16, C_I16))
IR_FOLD(LT(C_I32, C_I32))
IR_FOLD(LT(C_I64, C_I64))
{
	IR_FOLD_BOOL(op1_insn->val.i64 < op2_insn->val.i64);
}

IR_FOLD(LT(C_DOUBLE, C_DOUBLE))
{
	IR_FOLD_BOOL(op1_insn->val.d < op2_insn->val.d);
}

IR_FOLD(LT(C_FLOAT, C_FLOAT))
{
	IR_FOLD_BOOL(op1_insn->val.f < op2_insn->val.f);
}

IR_FOLD(GE(C_BOOL, C_BOOL))
IR_FOLD(GE(C_U8, C_U8))
IR_FOLD(GE(C_U16, C_U16))
IR_FOLD(GE(C_U32, C_U32))
IR_FOLD(GE(C_U64, C_U64))
IR_FOLD(GE(C_ADDR, C_ADDR))
{
	IR_FOLD_BOOL(op1_insn->val.u64 >= op2_insn->val.u64);
}

IR_FOLD(GE(C_CHAR, C_CHAR))
IR_FOLD(GE(C_I8, C_I8))
IR_FOLD(GE(C_I16, C_I16))
IR_FOLD(GE(C_I32, C_I32))
IR_FOLD(GE(C_I64, C_I64))
{
	IR_FOLD_BOOL(op1_insn->val.i64 >= op2_insn->val.i64);
}

IR_FOLD(GE(C_DOUBLE, C_DOUBLE))
{
	IR_FOLD_BOOL(op1_insn->val.d >= op2_insn->val.d);
}

IR_FOLD(GE(C_FLOAT, C_FLOAT))
{
	IR_FOLD_BOOL(op1_insn->val.f >= op2_insn->val.f);
}

IR_FOLD(LE(C_BOOL, C_BOOL))
IR_FOLD(LE(C_U8, C_U8))
IR_FOLD(LE(C_U16, C_U16))
IR_FOLD(LE(C_U32, C_U32))
IR_FOLD(LE(C_U64, C_U64))
IR_FOLD(LE(C_ADDR, C_ADDR))
{
	IR_FOLD_BOOL(op1_insn->val.u64 <= op2_insn->val.u64);
}

IR_FOLD(LE(C_CHAR, C_CHAR))
IR_FOLD(LE(C_I8, C_I8))
IR_FOLD(LE(C_I16, C_I16))
IR_FOLD(LE(C_I32, C_I32))
IR_FOLD(LE(C_I64, C_I64))
{
	IR_FOLD_BOOL(op1_insn->val.i64 <= op2_insn->val.i64);
}

IR_FOLD(LE(C_DOUBLE, C_DOUBLE))
{
	IR_FOLD_BOOL(op1_insn->val.d <= op2_insn->val.d);
}

IR_FOLD(LE(C_FLOAT, C_FLOAT))
{
	IR_FOLD_BOOL(op1_insn->val.f <= op2_insn->val.f);
}

IR_FOLD(GT(C_BOOL, C_BOOL))
IR_FOLD(GT(C_U8, C_U8))
IR_FOLD(GT(C_U16, C_U16))
IR_FOLD(GT(C_U32, C_U32))
IR_FOLD(GT(C_U64, C_U64))
IR_FOLD(GT(C_ADDR, C_ADDR))
{
	IR_FOLD_BOOL(op1_insn->val.u64 > op2_insn->val.u64);
}

IR_FOLD(GT(C_CHAR, C_CHAR))
IR_FOLD(GT(C_I8, C_I8))
IR_FOLD(GT(C_I16, C_I16))
IR_FOLD(GT(C_I32, C_I32))
IR_FOLD(GT(C_I64, C_I64))
{
	IR_FOLD_BOOL(op1_insn->val.i64 > op2_insn->val.i64);
}

IR_FOLD(GT(C_DOUBLE, C_DOUBLE))
{
	IR_FOLD_BOOL(op1_insn->val.d > op2_insn->val.d);
}

IR_FOLD(GT(C_FLOAT, C_FLOAT))
{
	IR_FOLD_BOOL(op1_insn->val.f > op2_insn->val.f);
}

IR_FOLD(ULT(C_BOOL, C_BOOL))
IR_FOLD(ULT(C_U8, C_U8))
IR_FOLD(ULT(C_U16, C_U16))
IR_FOLD(ULT(C_U32, C_U32))
IR_FOLD(ULT(C_U64, C_U64))
IR_FOLD(ULT(C_ADDR, C_ADDR))
IR_FOLD(ULT(C_CHAR, C_CHAR))
IR_FOLD(ULT(C_I8, C_I8))
IR_FOLD(ULT(C_I16, C_I16))
IR_FOLD(ULT(C_I32, C_I32))
IR_FOLD(ULT(C_I64, C_I64))
{
	IR_FOLD_BOOL(op1_insn->val.u64 < op2_insn->val.u64);
}

IR_FOLD(ULT(C_DOUBLE, C_DOUBLE))
{
	IR_FOLD_BOOL(!(op1_insn->val.d >= op2_insn->val.d));
}

IR_FOLD(ULT(C_FLOAT, C_FLOAT))
{
	IR_FOLD_BOOL(!(op1_insn->val.f >= op2_insn->val.f));
}

IR_FOLD(UGE(C_BOOL, C_BOOL))
IR_FOLD(UGE(C_U8, C_U8))
IR_FOLD(UGE(C_U16, C_U16))
IR_FOLD(UGE(C_U32, C_U32))
IR_FOLD(UGE(C_U64, C_U64))
IR_FOLD(UGE(C_ADDR, C_ADDR))
IR_FOLD(UGE(C_CHAR, C_CHAR))
IR_FOLD(UGE(C_I8, C_I8))
IR_FOLD(UGE(C_I16, C_I16))
IR_FOLD(UGE(C_I32, C_I32))
IR_FOLD(UGE(C_I64, C_I64))
{
	IR_FOLD_BOOL(op1_insn->val.u64 >= op2_insn->val.u64);
}

IR_FOLD(UGE(C_DOUBLE, C_DOUBLE))
{
	IR_FOLD_BOOL(!(op1_insn->val.d < op2_insn->val.d));
}

IR_FOLD(UGE(C_FLOAT, C_FLOAT))
{
	IR_FOLD_BOOL(!(op1_insn->val.f < op2_insn->val.f));
}

IR_FOLD(ULE(C_BOOL, C_BOOL))
IR_FOLD(ULE(C_U8, C_U8))
IR_FOLD(ULE(C_U16, C_U16))
IR_FOLD(ULE(C_U32, C_U32))
IR_FOLD(ULE(C_U64, C_U64))
IR_FOLD(ULE(C_ADDR, C_ADDR))
IR_FOLD(ULE(C_CHAR, C_CHAR))
IR_FOLD(ULE(C_I8, C_I8))
IR_FOLD(ULE(C_I16, C_I16))
IR_FOLD(ULE(C_I32, C_I32))
IR_FOLD(ULE(C_I64, C_I64))
{
	IR_FOLD_BOOL(op1_insn->val.u64 <= op2_insn->val.u64);
}

IR_FOLD(ULE(C_DOUBLE, C_DOUBLE))
{
	IR_FOLD_BOOL(!(op1_insn->val.d > op2_insn->val.d));
}

IR_FOLD(ULE(C_FLOAT, C_FLOAT))
{
	IR_FOLD_BOOL(!(op1_insn->val.f > op2_insn->val.f));
}

IR_FOLD(UGT(C_BOOL, C_BOOL))
IR_FOLD(UGT(C_U8, C_U8))
IR_FOLD(UGT(C_U16, C_U16))
IR_FOLD(UGT(C_U32, C_U32))
IR_FOLD(UGT(C_U64, C_U64))
IR_FOLD(UGT(C_ADDR, C_ADDR))
IR_FOLD(UGT(C_CHAR, C_CHAR))
IR_FOLD(UGT(C_I8, C_I8))
IR_FOLD(UGT(C_I16, C_I16))
IR_FOLD(UGT(C_I32, C_I32))
IR_FOLD(UGT(C_I64, C_I64))
{
	IR_FOLD_BOOL(op1_insn->val.u64 > op2_insn->val.u64);
}

IR_FOLD(UGT(C_DOUBLE, C_DOUBLE))
{
	IR_FOLD_BOOL(!(op1_insn->val.d <= op2_insn->val.d));
}

IR_FOLD(UGT(C_FLOAT, C_FLOAT))
{
	IR_FOLD_BOOL(!(op1_insn->val.f <= op2_insn->val.f));
}

IR_FOLD(ADD(C_U8, C_U8))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u8 + op2_insn->val.u8);
}

IR_FOLD(ADD(C_U16, C_U16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u16 + op2_insn->val.u16);
}

IR_FOLD(ADD(C_U32, C_U32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type || (sizeof(void*) == 4 && IR_OPT_TYPE(opt) == IR_ADDR));
	IR_FOLD_CONST_U(op1_insn->val.u32 + op2_insn->val.u32);
}

IR_FOLD(ADD(C_U64, C_U64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type || (sizeof(void*) == 8 && IR_OPT_TYPE(opt) == IR_ADDR));
	IR_FOLD_CONST_U(op1_insn->val.u64 + op2_insn->val.u64);
}

IR_FOLD(ADD(C_ADDR, C_ADDR))
IR_FOLD(ADD(C_ADDR, C_INTPTR))
IR_FOLD(ADD(C_ADDR, C_UINTPTR))
IR_FOLD(ADD(C_INTPTR, C_ADDR))
IR_FOLD(ADD(C_UINTPTR, C_ADDR))
{
//	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.addr + op2_insn->val.addr);
}

IR_FOLD(ADD(C_I8, C_I8))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i8 + op2_insn->val.i8);
}

IR_FOLD(ADD(C_I16, C_I16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i16 + op2_insn->val.i16);
}

IR_FOLD(ADD(C_I32, C_I32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type || (sizeof(void*) == 4 && IR_OPT_TYPE(opt) == IR_ADDR));
	IR_FOLD_CONST_I(op1_insn->val.i32 + op2_insn->val.i32);
}

IR_FOLD(ADD(C_I64, C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type || (sizeof(void*) == 8 && IR_OPT_TYPE(opt) == IR_ADDR));
	IR_FOLD_CONST_I(op1_insn->val.i64 + op2_insn->val.i64);
}

IR_FOLD(ADD(C_DOUBLE, C_DOUBLE))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_D(op1_insn->val.d + op2_insn->val.d);
}

IR_FOLD(ADD(C_FLOAT, C_FLOAT))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_F(op1_insn->val.f + op2_insn->val.f);
}

IR_FOLD(SUB(C_U8, C_U8))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u8 - op2_insn->val.u8);
}

IR_FOLD(SUB(C_U16, C_U16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u16 - op2_insn->val.u16);
}

IR_FOLD(SUB(C_U32, C_U32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u32 - op2_insn->val.u32);
}

IR_FOLD(SUB(C_U64, C_U64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u64 - op2_insn->val.u64);
}

IR_FOLD(SUB(C_ADDR, C_ADDR))
IR_FOLD(SUB(C_ADDR, C_INTPTR))
IR_FOLD(SUB(C_ADDR, C_UINTPTR))
IR_FOLD(SUB(C_INTPTR, C_ADDR))
IR_FOLD(SUB(C_UINTPTR, C_ADDR))
{
//	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.addr - op2_insn->val.addr);
}

IR_FOLD(SUB(C_I8, C_I8))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i8 - op2_insn->val.i8);
}

IR_FOLD(SUB(C_I16, C_I16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i16 - op2_insn->val.i16);
}

IR_FOLD(SUB(C_I32, C_I32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i32 - op2_insn->val.i32);
}

IR_FOLD(SUB(C_I64, C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i64 - op2_insn->val.i64);
}

IR_FOLD(SUB(C_DOUBLE, C_DOUBLE))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_D(op1_insn->val.d - op2_insn->val.d);
}

IR_FOLD(SUB(C_FLOAT, C_FLOAT))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_F(op1_insn->val.f - op2_insn->val.f);
}

IR_FOLD(MUL(C_U8, C_U8))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u8 * op2_insn->val.u8);
}

IR_FOLD(MUL(C_U16, C_U16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u16 * op2_insn->val.u16);
}

IR_FOLD(MUL(C_U32, C_U32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u32 * op2_insn->val.u32);
}

IR_FOLD(MUL(C_U64, C_U64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u64 * op2_insn->val.u64);
}

IR_FOLD(MUL(C_ADDR, C_ADDR))
IR_FOLD(MUL(C_ADDR, C_INTPTR))
IR_FOLD(MUL(C_ADDR, C_UINTPTR))
IR_FOLD(MUL(C_INTPTR, C_ADDR))
IR_FOLD(MUL(C_UINTPTR, C_ADDR))
{
//	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.addr * op2_insn->val.addr);
}

IR_FOLD(MUL(C_I8, C_I8))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i8 * op2_insn->val.i8);
}

IR_FOLD(MUL(C_I16, C_I16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i16 * op2_insn->val.i16);
}

IR_FOLD(MUL(C_I32, C_I32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i32 * op2_insn->val.i32);
}

IR_FOLD(MUL(C_I64, C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i64 * op2_insn->val.i64);
}

IR_FOLD(MUL(C_DOUBLE, C_DOUBLE))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_D(op1_insn->val.d * op2_insn->val.d);
}

IR_FOLD(MUL(C_FLOAT, C_FLOAT))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_F(op1_insn->val.f * op2_insn->val.f);
}

IR_FOLD(DIV(C_U8, C_U8))
IR_FOLD(DIV(C_U16, C_U16))
IR_FOLD(DIV(C_U32, C_U32))
IR_FOLD(DIV(C_U64, C_U64))
IR_FOLD(DIV(C_ADDR, C_ADDR))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	if (op2_insn->val.u64 == 0) {
		/* division by zero */
		IR_FOLD_EMIT;
	}
	IR_FOLD_CONST_U(op1_insn->val.u64 / op2_insn->val.u64);
}

IR_FOLD(DIV(C_I8, C_I8))
IR_FOLD(DIV(C_I16, C_I16))
IR_FOLD(DIV(C_I32, C_I32))
IR_FOLD(DIV(C_I64, C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	if (op2_insn->val.i64 == 0) {
		/* division by zero */
		IR_FOLD_EMIT;
	}
	IR_FOLD_CONST_I(op1_insn->val.i64 / op2_insn->val.i64);
}

IR_FOLD(DIV(C_DOUBLE, C_DOUBLE))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_D(op1_insn->val.d / op2_insn->val.d);
}

IR_FOLD(DIV(C_FLOAT, C_FLOAT))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_F(op1_insn->val.f / op2_insn->val.f);
}

IR_FOLD(MOD(C_U8, C_U8))
IR_FOLD(MOD(C_U16, C_U16))
IR_FOLD(MOD(C_U32, C_U32))
IR_FOLD(MOD(C_U64, C_U64))
IR_FOLD(MOD(C_ADDR, C_ADDR))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	if (op2_insn->val.u64 == 0) {
		/* division by zero */
		IR_FOLD_EMIT;
	}
	IR_FOLD_CONST_U(op1_insn->val.u64 % op2_insn->val.u64);
}

IR_FOLD(MOD(C_I8, C_I8))
IR_FOLD(MOD(C_I16, C_I16))
IR_FOLD(MOD(C_I32, C_I32))
IR_FOLD(MOD(C_I64, C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	if (op2_insn->val.i64 == 0) {
		/* division by zero */
		IR_FOLD_EMIT;
	}
	IR_FOLD_CONST_I(op1_insn->val.i64 % op2_insn->val.i64);
}

IR_FOLD(NEG(C_I8))
IR_FOLD(NEG(C_I16))
IR_FOLD(NEG(C_I32))
IR_FOLD(NEG(C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(-op1_insn->val.i64);
}

IR_FOLD(NEG(C_DOUBLE))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_D(-op1_insn->val.d);
}

IR_FOLD(NEG(C_FLOAT))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_F(-op1_insn->val.f);
}

IR_FOLD(ABS(C_I8))
IR_FOLD(ABS(C_I16))
IR_FOLD(ABS(C_I32))
IR_FOLD(ABS(C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	if (op1_insn->val.i64 >= 0) {
		IR_FOLD_COPY(op1);
	} else {
		IR_FOLD_CONST_I(-op1_insn->val.i64);
	}
}

IR_FOLD(ABS(C_DOUBLE))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_D(fabs(op1_insn->val.d));
}

IR_FOLD(ABS(C_FLOAT))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_F(fabsf(op1_insn->val.f));
}

IR_FOLD(ADD_OV(C_U8, C_U8))
IR_FOLD(ADD_OV(C_U16, C_U16))
IR_FOLD(ADD_OV(C_U32, C_U32))
IR_FOLD(ADD_OV(C_U64, C_U64))
{
	ir_type type = IR_OPT_TYPE(opt);
	uint64_t max = ((uint64_t)0xffffffffffffffff) >> (64 - ir_type_size[type] * 8);
	IR_ASSERT(type == op1_insn->type);
	if (op1_insn->val.u64 > max - op2_insn->val.u64) {
		IR_FOLD_NEXT;
	}
	IR_FOLD_CONST_U(op1_insn->val.u64 + op2_insn->val.u64);
}

IR_FOLD(ADD_OV(C_I8, C_I8))
IR_FOLD(ADD_OV(C_I16, C_I16))
IR_FOLD(ADD_OV(C_I32, C_I32))
IR_FOLD(ADD_OV(C_I64, C_I64))
{
	ir_type type = IR_OPT_TYPE(opt);
	int64_t max = ((uint64_t)0x7fffffffffffffff) >> (64 - ir_type_size[type] * 8);
	int64_t min = - max - 1;
	IR_ASSERT(type == op1_insn->type);
	if ((op2_insn->val.i64 > 0 && op1_insn->val.i64 > max - op2_insn->val.i64)
	 || (op2_insn->val.i64 < 0 && op1_insn->val.i64 < min - op2_insn->val.i64)) {
		IR_FOLD_NEXT;
	}
	IR_FOLD_CONST_I(op1_insn->val.i64 + op2_insn->val.i64);
}

IR_FOLD(SUB_OV(C_U8, C_U8))
IR_FOLD(SUB_OV(C_U16, C_U16))
IR_FOLD(SUB_OV(C_U32, C_U32))
IR_FOLD(SUB_OV(C_U64, C_U64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	if (op2_insn->val.u64 > op1_insn->val.u64) {
		IR_FOLD_NEXT;
	}
	IR_FOLD_CONST_U(op1_insn->val.u64 - op2_insn->val.u64);
}

IR_FOLD(SUB_OV(C_I8, C_I8))
IR_FOLD(SUB_OV(C_I16, C_I16))
IR_FOLD(SUB_OV(C_I32, C_I32))
IR_FOLD(SUB_OV(C_I64, C_I64))
{
	ir_type type = IR_OPT_TYPE(opt);
	int64_t max = ((uint64_t)0x7fffffffffffffff) >> (64 - ir_type_size[type] * 8);
	int64_t min = - max - 1;
	IR_ASSERT(type == op1_insn->type);
	if ((op2_insn->val.i64 > 0 && op1_insn->val.i64 < min + op2_insn->val.i64)
	 || (op2_insn->val.i64 < 0 && op1_insn->val.i64 > max + op2_insn->val.i64)) {
		IR_FOLD_NEXT;
	}
	IR_FOLD_CONST_I(op1_insn->val.i64 - op2_insn->val.i64);
}

IR_FOLD(MUL_OV(C_U8, C_U8))
IR_FOLD(MUL_OV(C_U16, C_U16))
IR_FOLD(MUL_OV(C_U32, C_U32))
IR_FOLD(MUL_OV(C_U64, C_U64))
{
	ir_type type = IR_OPT_TYPE(opt);
	uint64_t max = ((uint64_t)0xffffffffffffffff) >> (64 - ir_type_size[type] * 8);
	uint64_t res;
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	res = op1_insn->val.u64 * op2_insn->val.u64;
	if (op1_insn->val.u64 != 0 && res / op1_insn->val.u64 != op2_insn->val.u64 && res <= max) {
		IR_FOLD_NEXT;
	}
	IR_FOLD_CONST_U(res);
}

IR_FOLD(MUL_OV(C_I8, C_I8))
IR_FOLD(MUL_OV(C_I16, C_I16))
IR_FOLD(MUL_OV(C_I32, C_I32))
IR_FOLD(MUL_OV(C_I64, C_I64))
{
	ir_type type = IR_OPT_TYPE(opt);
	int64_t max = ((uint64_t)0x7fffffffffffffff) >> (64 - ir_type_size[type] * 8);
	int64_t min = - max - 1;
	int64_t res;
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	res = op1_insn->val.i64 * op2_insn->val.i64;
	if (op1_insn->val.i64 != 0 && res / op1_insn->val.i64 != op2_insn->val.i64 && res >= min && res <= max) {
		IR_FOLD_NEXT;
	}
	IR_FOLD_CONST_U(res);
}

IR_FOLD(OVERFLOW(_))
{
	if (op1_insn->op != IR_ADD_OV && op1_insn->op != IR_SUB_OV && op1_insn->op != IR_MUL_OV) {
		IR_FOLD_COPY(IR_FALSE);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(NOT(C_BOOL))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_BOOL(!op1_insn->val.u64);
}

IR_FOLD(NOT(C_U8))
IR_FOLD(NOT(C_CHAR))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(~op1_insn->val.u8);
}

IR_FOLD(NOT(C_I8))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(~op1_insn->val.i8);
}

IR_FOLD(NOT(C_U16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(~op1_insn->val.u16);
}

IR_FOLD(NOT(C_I16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(~op1_insn->val.i16);
}

IR_FOLD(NOT(C_U32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(~op1_insn->val.u32);
}

IR_FOLD(NOT(C_I32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(~op1_insn->val.i32);
}

IR_FOLD(NOT(C_U64))
IR_FOLD(NOT(C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(~op1_insn->val.u64);
}

IR_FOLD(OR(C_BOOL, C_BOOL))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_BOOL(op1_insn->val.b || op2_insn->val.b);
}

IR_FOLD(OR(C_CHAR, C_CHAR))
IR_FOLD(OR(C_U8, C_U8))
IR_FOLD(OR(C_U16, C_U16))
IR_FOLD(OR(C_U32, C_U32))
IR_FOLD(OR(C_U64, C_U64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u64 | op2_insn->val.u64);
}

IR_FOLD(OR(C_I8, C_I8))
IR_FOLD(OR(C_I16, C_I16))
IR_FOLD(OR(C_I32, C_I32))
IR_FOLD(OR(C_I64, C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i64 | op2_insn->val.i64);
}

IR_FOLD(AND(C_BOOL, C_BOOL))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_BOOL(op1_insn->val.b && op2_insn->val.b);
}

IR_FOLD(AND(C_CHAR, C_CHAR))
IR_FOLD(AND(C_U8, C_U8))
IR_FOLD(AND(C_U16, C_U16))
IR_FOLD(AND(C_U32, C_U32))
IR_FOLD(AND(C_U64, C_U64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u64 & op2_insn->val.u64);
}

IR_FOLD(AND(C_I8, C_I8))
IR_FOLD(AND(C_I16, C_I16))
IR_FOLD(AND(C_I32, C_I32))
IR_FOLD(AND(C_I64, C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i64 & op2_insn->val.i64);
}

IR_FOLD(XOR(C_BOOL, C_BOOL))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_BOOL(op1_insn->val.b != op2_insn->val.b);
}

IR_FOLD(XOR(C_U8, C_U8))
IR_FOLD(XOR(C_CHAR, C_CHAR))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u8 ^ op2_insn->val.u8);
}

IR_FOLD(XOR(C_I8, C_I8))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i8 ^ op2_insn->val.i8);
}

IR_FOLD(XOR(C_U16, C_U16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u16 ^ op2_insn->val.u16);
}

IR_FOLD(XOR(C_I16, C_I16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i16 ^ op2_insn->val.i16);
}

IR_FOLD(XOR(C_U32, C_U32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u32 ^ op2_insn->val.u32);
}

IR_FOLD(XOR(C_I32, C_I32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i32 ^ op2_insn->val.i32);
}

IR_FOLD(XOR(C_U64, C_U64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u64 ^ op2_insn->val.u64);
}

IR_FOLD(XOR(C_I64, C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i64 ^ op2_insn->val.i64);
}

IR_FOLD(SHL(C_U8, C_U8))
IR_FOLD(SHL(C_CHAR, C_CHAR))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u8 << op2_insn->val.u8);
}

IR_FOLD(SHL(C_I8, C_I8))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i8 << op2_insn->val.i8);
}

IR_FOLD(SHL(C_U16, C_U16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u16 << op2_insn->val.u16);
}

IR_FOLD(SHL(C_I16, C_I16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i16 << op2_insn->val.i16);
}

IR_FOLD(SHL(C_U32, C_U32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u32 << op2_insn->val.u32);
}

IR_FOLD(SHL(C_I32, C_I32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i32 << op2_insn->val.i32);
}

IR_FOLD(SHL(C_U64, C_U64))
IR_FOLD(SHL(C_I64, C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u64 << op2_insn->val.u64);
}

IR_FOLD(SHR(C_U8, C_U8))
IR_FOLD(SHR(C_CHAR, C_CHAR))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u8 >> op2_insn->val.u8);
}

IR_FOLD(SHR(C_I8, C_I8))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I((int8_t)(op1_insn->val.u8 >> op2_insn->val.u8));
}

IR_FOLD(SHR(C_U16, C_U16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u16 >> op2_insn->val.u16);
}

IR_FOLD(SHR(C_I16, C_I16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U((int16_t)(op1_insn->val.u16 >> op2_insn->val.u16));
}

IR_FOLD(SHR(C_U32, C_U32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u32 >> op2_insn->val.u32);
}

IR_FOLD(SHR(C_I32, C_I32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U((int32_t)(op1_insn->val.u32 >> op2_insn->val.u32));
}

IR_FOLD(SHR(C_U64, C_U64))
IR_FOLD(SHR(C_I64, C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(op1_insn->val.u64 >> op2_insn->val.u64);
}

IR_FOLD(SAR(C_U8, C_U8))
IR_FOLD(SAR(C_CHAR, C_CHAR))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U((uint8_t)(op1_insn->val.i8 >> op2_insn->val.i8));
}

IR_FOLD(SAR(C_I8, C_I8))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i8 >> op2_insn->val.i8);
}

IR_FOLD(SAR(C_U16, C_U16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U((uint16_t)(op1_insn->val.i16 >> op2_insn->val.i16));
}

IR_FOLD(SAR(C_I16, C_I16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i16 >> op2_insn->val.i16);
}

IR_FOLD(SAR(C_U32, C_U32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U((uint32_t)(op1_insn->val.i32 >> op2_insn->val.i32));
}

IR_FOLD(SAR(C_I32, C_I32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i32 >> op2_insn->val.i32);
}

IR_FOLD(SAR(C_U64, C_U64))
IR_FOLD(SAR(C_I64, C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I(op1_insn->val.i64 >> op2_insn->val.i64);
}

IR_FOLD(ROL(C_U8, C_U8))
IR_FOLD(ROL(C_CHAR, C_CHAR))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(ir_rol8(op1_insn->val.u8, op2_insn->val.u8));
}

IR_FOLD(ROL(C_I8, C_I8))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I((int8_t)ir_rol8(op1_insn->val.u8, op2_insn->val.u8));
}

IR_FOLD(ROL(C_U16, C_U16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(ir_rol16(op1_insn->val.u16, op2_insn->val.u16));
}

IR_FOLD(ROL(C_I16, C_I16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I((int16_t)ir_rol16(op1_insn->val.u16, op2_insn->val.u16));
}

IR_FOLD(ROL(C_U32, C_U32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(ir_rol32(op1_insn->val.u32, op2_insn->val.u32));
}

IR_FOLD(ROL(C_I32, C_I32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I((int32_t)ir_rol32(op1_insn->val.u32, op2_insn->val.u32));
}

IR_FOLD(ROL(C_U64, C_U64))
IR_FOLD(ROL(C_I64, C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(ir_rol64(op1_insn->val.u64, op2_insn->val.u64));
}

IR_FOLD(ROR(C_U8, C_U8))
IR_FOLD(ROR(C_CHAR, C_CHAR))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(ir_ror8(op1_insn->val.u8, op2_insn->val.u8));
}

IR_FOLD(ROR(C_I8, C_I8))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I((int8_t)ir_ror8(op1_insn->val.u8, op2_insn->val.u8));
}

IR_FOLD(ROR(C_U16, C_U16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(ir_ror16(op1_insn->val.u16, op2_insn->val.u16));
}

IR_FOLD(ROR(C_I16, C_I16))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I((int16_t)ir_ror16(op1_insn->val.u16, op2_insn->val.u16));
}

IR_FOLD(ROR(C_U32, C_U32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(ir_ror32(op1_insn->val.u32, op2_insn->val.u32));
}

IR_FOLD(ROR(C_I32, C_I32))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_I((int32_t)ir_ror32(op1_insn->val.u32, op2_insn->val.u32));
}

IR_FOLD(ROR(C_U64, C_U64))
IR_FOLD(ROR(C_I64, C_I64))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	IR_FOLD_CONST_U(ir_ror64(op1_insn->val.u64, op2_insn->val.u64));
}

//IR_FOLD(BSWAP(CONST))
//TODO: bswap

IR_FOLD(MIN(C_BOOL, C_BOOL))
IR_FOLD(MIN(C_U8, C_U8))
IR_FOLD(MIN(C_U16, C_U16))
IR_FOLD(MIN(C_U32, C_U32))
IR_FOLD(MIN(C_U64, C_U64))
IR_FOLD(MIN(C_ADDR, C_ADDR))
{
	IR_FOLD_COPY(op1_insn->val.u64 <= op2_insn->val.u64 ? op1 : op2);
}

IR_FOLD(MIN(C_CHAR, C_CHAR))
IR_FOLD(MIN(C_I8, C_U8))
IR_FOLD(MIN(C_I16, C_U16))
IR_FOLD(MIN(C_I32, C_U32))
IR_FOLD(MIN(C_I64, C_U64))
{
	IR_FOLD_COPY(op1_insn->val.i64 <= op2_insn->val.i64 ? op1 : op2);
}

IR_FOLD(MIN(C_DOUBLE, C_DOUBLE))
{
	IR_FOLD_COPY(op1_insn->val.d <= op2_insn->val.d ? op1 : op2);
}

IR_FOLD(MIN(C_FLOAT, C_FLOAT))
{
	IR_FOLD_COPY(op1_insn->val.f <= op2_insn->val.f ? op1 : op2);
}

IR_FOLD(MAX(C_BOOL, C_BOOL))
IR_FOLD(MAX(C_U8, C_U8))
IR_FOLD(MAX(C_U16, C_U16))
IR_FOLD(MAX(C_U32, C_U32))
IR_FOLD(MAX(C_U64, C_U64))
IR_FOLD(MAX(C_ADDR, C_ADDR))
{
	IR_FOLD_COPY(op1_insn->val.u64 >= op2_insn->val.u64 ? op1 : op2);
}

IR_FOLD(MAX(C_CHAR, C_CHAR))
IR_FOLD(MAX(C_I8, C_U8))
IR_FOLD(MAX(C_I16, C_U16))
IR_FOLD(MAX(C_I32, C_U32))
IR_FOLD(MAX(C_I64, C_U64))
{
	IR_FOLD_COPY(op1_insn->val.i64 >= op2_insn->val.i64 ? op1 : op2);
}

IR_FOLD(MAX(C_DOUBLE, C_DOUBLE))
{
	IR_FOLD_COPY(op1_insn->val.d >= op2_insn->val.d ? op1 : op2);
}

IR_FOLD(MAX(C_FLOAT, C_FLOAT))
{
	IR_FOLD_COPY(op1_insn->val.f >= op2_insn->val.f ? op1 : op2);
}

IR_FOLD(SEXT(C_I8))
IR_FOLD(SEXT(C_U8))
IR_FOLD(SEXT(C_BOOL))
{
	IR_ASSERT(IR_IS_TYPE_INT(IR_OPT_TYPE(opt)));
	IR_ASSERT(ir_type_size[IR_OPT_TYPE(opt)] > ir_type_size[op1_insn->type]);
	IR_FOLD_CONST_I((int64_t)op1_insn->val.i8);
}

IR_FOLD(SEXT(C_I16))
IR_FOLD(SEXT(C_U16))
{
	IR_ASSERT(IR_IS_TYPE_INT(IR_OPT_TYPE(opt)));
	IR_ASSERT(ir_type_size[IR_OPT_TYPE(opt)] > ir_type_size[op1_insn->type]);
	IR_FOLD_CONST_I((int64_t)op1_insn->val.i16);
}

IR_FOLD(SEXT(C_I32))
IR_FOLD(SEXT(C_U32))
{
	IR_ASSERT(IR_IS_TYPE_INT(IR_OPT_TYPE(opt)));
	IR_ASSERT(ir_type_size[IR_OPT_TYPE(opt)] > ir_type_size[op1_insn->type]);
	IR_FOLD_CONST_I((int64_t)op1_insn->val.i32);
}

IR_FOLD(ZEXT(C_I8))
IR_FOLD(ZEXT(C_U8))
IR_FOLD(ZEXT(C_BOOL))
{
	IR_ASSERT(IR_IS_TYPE_INT(IR_OPT_TYPE(opt)));
	IR_ASSERT(ir_type_size[IR_OPT_TYPE(opt)] > ir_type_size[op1_insn->type]);
	IR_FOLD_CONST_U((uint64_t)op1_insn->val.u8);
}

IR_FOLD(ZEXT(C_I16))
IR_FOLD(ZEXT(C_U16))
{
	IR_ASSERT(IR_IS_TYPE_INT(IR_OPT_TYPE(opt)));
	IR_ASSERT(ir_type_size[IR_OPT_TYPE(opt)] > ir_type_size[op1_insn->type]);
	IR_FOLD_CONST_U((uint64_t)op1_insn->val.u16);
}

IR_FOLD(ZEXT(C_I32))
IR_FOLD(ZEXT(C_U32))
{
	IR_ASSERT(IR_IS_TYPE_INT(IR_OPT_TYPE(opt)));
	IR_ASSERT(ir_type_size[IR_OPT_TYPE(opt)] > ir_type_size[op1_insn->type]);
	IR_FOLD_CONST_U((uint64_t)op1_insn->val.u32);
}

IR_FOLD(TRUNC(C_I16))
IR_FOLD(TRUNC(C_I32))
IR_FOLD(TRUNC(C_I64))
IR_FOLD(TRUNC(C_U16))
IR_FOLD(TRUNC(C_U32))
IR_FOLD(TRUNC(C_U64))
{
	IR_ASSERT(IR_IS_TYPE_INT(IR_OPT_TYPE(opt)));
	IR_ASSERT(ir_type_size[IR_OPT_TYPE(opt)] < ir_type_size[op1_insn->type]);
	switch (IR_OPT_TYPE(opt)) {
		default:
			IR_ASSERT(0);
		case IR_I8:
			IR_FOLD_CONST_I(op1_insn->val.i8);
		case IR_I16:
			IR_FOLD_CONST_I(op1_insn->val.i16);
		case IR_I32:
			IR_FOLD_CONST_I(op1_insn->val.i32);
		case IR_U8:
			IR_FOLD_CONST_U(op1_insn->val.u8);
		case IR_U16:
			IR_FOLD_CONST_U(op1_insn->val.u16);
		case IR_U32:
			IR_FOLD_CONST_U(op1_insn->val.u32);
	}
}


IR_FOLD(BITCAST(C_I8))
IR_FOLD(BITCAST(C_I16))
IR_FOLD(BITCAST(C_I32))
IR_FOLD(BITCAST(C_I64))
IR_FOLD(BITCAST(C_U8))
IR_FOLD(BITCAST(C_U16))
IR_FOLD(BITCAST(C_U32))
IR_FOLD(BITCAST(C_U64))
IR_FOLD(BITCAST(C_FLOAT))
IR_FOLD(BITCAST(C_DOUBLE))
IR_FOLD(BITCAST(C_BOOL))
IR_FOLD(BITCAST(C_CHAR))
IR_FOLD(BITCAST(C_ADDR))
{
	IR_ASSERT(ir_type_size[IR_OPT_TYPE(opt)] == ir_type_size[op1_insn->type]);
	switch (IR_OPT_TYPE(opt)) {
		default:
			IR_ASSERT(0);
		case IR_BOOL:
			IR_FOLD_BOOL(op1_insn->val.i8 != 0);
		case IR_I8:
			IR_FOLD_CONST_I(op1_insn->val.i8);
		case IR_I16:
			IR_FOLD_CONST_I(op1_insn->val.i16);
		case IR_I32:
			IR_FOLD_CONST_I(op1_insn->val.i32);
		case IR_I64:
			IR_FOLD_CONST_I(op1_insn->val.i64);
		case IR_U8:
			IR_FOLD_CONST_U(op1_insn->val.u8);
		case IR_U16:
			IR_FOLD_CONST_U(op1_insn->val.u16);
		case IR_U32:
			IR_FOLD_CONST_U(op1_insn->val.u32);
		case IR_U64:
			IR_FOLD_CONST_U(op1_insn->val.u64);
		case IR_FLOAT:
			IR_FOLD_CONST_F(op1_insn->val.f);
		case IR_DOUBLE:
			IR_FOLD_CONST_D(op1_insn->val.d);
		case IR_CHAR:
			IR_FOLD_CONST_I(op1_insn->val.c);
		case IR_ADDR:
			IR_FOLD_CONST_U(op1_insn->val.addr);
	}
}

IR_FOLD(INT2FP(C_I8))
IR_FOLD(INT2FP(C_I16))
IR_FOLD(INT2FP(C_I32))
IR_FOLD(INT2FP(C_I64))
{
	if (IR_OPT_TYPE(opt) == IR_DOUBLE) {
		IR_FOLD_CONST_D((double)op1_insn->val.i64);
	} else {
		IR_ASSERT(IR_OPT_TYPE(opt) == IR_FLOAT);
		IR_FOLD_CONST_F((float)op1_insn->val.i64);
	}
}

IR_FOLD(INT2FP(C_U8))
IR_FOLD(INT2FP(C_U16))
IR_FOLD(INT2FP(C_U32))
IR_FOLD(INT2FP(C_U64))
{
	if (IR_OPT_TYPE(opt) == IR_DOUBLE) {
		IR_FOLD_CONST_D((double)op1_insn->val.u64);
	} else {
		IR_ASSERT(IR_OPT_TYPE(opt) == IR_FLOAT);
		IR_FOLD_CONST_F((float)op1_insn->val.u64);
	}
}

IR_FOLD(FP2INT(C_FLOAT))
{
	IR_ASSERT(IR_IS_TYPE_INT(IR_OPT_TYPE(opt)));
	switch (IR_OPT_TYPE(opt)) {
		default:
			IR_ASSERT(0);
		case IR_I8:
			IR_FOLD_CONST_I((int8_t)op1_insn->val.f);
		case IR_I16:
			IR_FOLD_CONST_I((int16_t)op1_insn->val.f);
		case IR_I32:
			IR_FOLD_CONST_I((int32_t)op1_insn->val.f);
		case IR_I64:
			IR_FOLD_CONST_I((int64_t)op1_insn->val.f);
		case IR_U8:
			IR_FOLD_CONST_U((uint8_t)op1_insn->val.f);
		case IR_U16:
			IR_FOLD_CONST_U((uint16_t)op1_insn->val.f);
		case IR_U32:
			IR_FOLD_CONST_U((uint32_t)op1_insn->val.f);
		case IR_U64:
			IR_FOLD_CONST_U((uint64_t)op1_insn->val.f);
	}
}

IR_FOLD(FP2INT(C_DOUBLE))
{
	IR_ASSERT(IR_IS_TYPE_INT(IR_OPT_TYPE(opt)));
	switch (IR_OPT_TYPE(opt)) {
		default:
			IR_ASSERT(0);
		case IR_I8:
			IR_FOLD_CONST_I((int8_t)op1_insn->val.d);
		case IR_I16:
			IR_FOLD_CONST_I((int16_t)op1_insn->val.d);
		case IR_I32:
			IR_FOLD_CONST_I((int32_t)op1_insn->val.d);
		case IR_I64:
			IR_FOLD_CONST_I((int64_t)op1_insn->val.d);
		case IR_U8:
			IR_FOLD_CONST_U((uint8_t)op1_insn->val.d);
		case IR_U16:
			IR_FOLD_CONST_U((uint16_t)op1_insn->val.d);
		case IR_U32:
			IR_FOLD_CONST_U((uint32_t)op1_insn->val.d);
		case IR_U64:
			IR_FOLD_CONST_U((uint64_t)op1_insn->val.d);
	}
}

IR_FOLD(FP2FP(C_FLOAT))
{
	if (IR_OPT_TYPE(opt) == IR_DOUBLE) {
		IR_FOLD_CONST_D((double)op1_insn->val.f);
	} else {
		IR_ASSERT(IR_OPT_TYPE(opt) == IR_FLOAT);
		IR_FOLD_COPY(op1);
	}
}

IR_FOLD(FP2FP(C_DOUBLE))
{
	if (IR_OPT_TYPE(opt) == IR_DOUBLE) {
		IR_FOLD_COPY(op1);
	} else {
		IR_ASSERT(IR_OPT_TYPE(opt) == IR_FLOAT);
		IR_FOLD_CONST_F((float)op1_insn->val.d);
	}
}

// TODO: constant functions (e.g.  sin, cos)

/* Copy Propagation */
IR_FOLD(COPY(_))
{
	IR_ASSERT(IR_OPT_TYPE(opt) == op1_insn->type);
	if (!op2) {
		IR_FOLD_COPY(op1);
	}
	/* skip CSE */
	IR_FOLD_EMIT;
}

IR_FOLD(PHI(_, _)) // TODO: PHI(_, _, _)
{
	if (op2 == op3 && op3 != IR_UNUSED) {
		IR_FOLD_COPY(op2);
	}
	/* skip CSE */
	opt = opt | (3 << IR_OPT_INPUTS_SHIFT);
	IR_FOLD_EMIT;
}

IR_FOLD(COND(C_BOOL, _)) // TODO: COND(CONST, _, _)
IR_FOLD(COND(C_U8, _))
IR_FOLD(COND(C_U16, _))
IR_FOLD(COND(C_U32, _))
IR_FOLD(COND(C_U64, _))
IR_FOLD(COND(C_ADDR, _))
IR_FOLD(COND(C_CHAR, _))
IR_FOLD(COND(C_I8, _))
IR_FOLD(COND(C_I16, _))
IR_FOLD(COND(C_I32, _))
IR_FOLD(COND(C_I64, _))
IR_FOLD(COND(C_DOUBLE, _))
IR_FOLD(COND(C_FLOAT, _))
{
	if (ir_const_is_true(op1_insn)) {
		IR_FOLD_COPY(op2);
	} else {
		IR_FOLD_COPY(op3);
	}
}

IR_FOLD(BITCAST(_))
{
	if (IR_OPT_TYPE(opt) == op1_insn->type) {
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

/* Algebraic simplifications */
IR_FOLD(ABS(ABS))
{
	/* abs(x = abs(y)) => x */
	IR_FOLD_COPY(op1);
}

IR_FOLD(ABS(NEG))
{
	/* abs(neg(y)) => abs(y) */
	op1 = op1_insn->op1;
	IR_FOLD_RESTART;
}

IR_FOLD(NEG(NEG))
IR_FOLD(NOT(NOT))
IR_FOLD(BSWAP(BSWAP))
{
	/* f(f(y)) => y */
	IR_FOLD_COPY(op1_insn->op1);
}

IR_FOLD(EQ(_, C_BOOL))
{
	if (op2 == IR_TRUE) {
		IR_FOLD_COPY(op1);
	} else {
		opt = IR_OPT(IR_NOT, IR_BOOL);
		op2 = IR_UNUSED;
		IR_FOLD_RESTART;
	}
}

IR_FOLD(NE(_, C_BOOL))
{
	if (op2 != IR_TRUE) {
		IR_FOLD_COPY(op1);
	} else {
		opt = IR_OPT(IR_NOT, IR_BOOL);
		op2 = IR_UNUSED;
		IR_FOLD_RESTART;
	}
}

IR_FOLD(EQ(ZEXT, C_U16))
IR_FOLD(EQ(ZEXT, C_U32))
IR_FOLD(EQ(ZEXT, C_U64))
IR_FOLD(EQ(ZEXT, C_I16))
IR_FOLD(EQ(ZEXT, C_I32))
IR_FOLD(EQ(ZEXT, C_I64))
IR_FOLD(EQ(ZEXT, C_ADDR))
IR_FOLD(EQ(SEXT, C_U16))
IR_FOLD(EQ(SEXT, C_U32))
IR_FOLD(EQ(SEXT, C_U64))
IR_FOLD(EQ(SEXT, C_I16))
IR_FOLD(EQ(SEXT, C_I32))
IR_FOLD(EQ(SEXT, C_I64))
IR_FOLD(EQ(SEXT, C_ADDR))
{
	if (op2_insn->val.u64 == 0 && ctx->ir_base[op1_insn->op1].type == IR_BOOL) {
		opt = IR_OPT(IR_NOT, IR_BOOL);
		op1 = op1_insn->op1;
		op2 = IR_UNUSED;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(NE(ZEXT, C_U16))
IR_FOLD(NE(ZEXT, C_U32))
IR_FOLD(NE(ZEXT, C_U64))
IR_FOLD(NE(ZEXT, C_I16))
IR_FOLD(NE(ZEXT, C_I32))
IR_FOLD(NE(ZEXT, C_I64))
IR_FOLD(NE(ZEXT, C_ADDR))
IR_FOLD(NE(SEXT, C_U16))
IR_FOLD(NE(SEXT, C_U32))
IR_FOLD(NE(SEXT, C_U64))
IR_FOLD(NE(SEXT, C_I16))
IR_FOLD(NE(SEXT, C_I32))
IR_FOLD(NE(SEXT, C_I64))
IR_FOLD(NE(SEXT, C_ADDR))
{
	if (op2_insn->val.u64 == 0 && ctx->ir_base[op1_insn->op1].type == IR_BOOL) {
		IR_FOLD_COPY(op1_insn->op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(NOT(EQ))
IR_FOLD(NOT(NE))
IR_FOLD(NOT(LT))
IR_FOLD(NOT(GE))
IR_FOLD(NOT(LE))
IR_FOLD(NOT(GT))
IR_FOLD(NOT(ULT))
IR_FOLD(NOT(UGE))
IR_FOLD(NOT(ULE))
IR_FOLD(NOT(UGT))
{
	if (IR_IS_TYPE_INT(ctx->ir_base[op1_insn->op1].type)) {
		opt = op1_insn->opt ^ 1;
		op1 = op1_insn->op1;
		op2 = op1_insn->op2;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(ADD(_, C_U8))
IR_FOLD(ADD(_, C_U16))
IR_FOLD(ADD(_, C_U32))
IR_FOLD(ADD(_, C_U64))
IR_FOLD(ADD(_, C_I8))
IR_FOLD(ADD(_, C_I16))
IR_FOLD(ADD(_, C_I32))
IR_FOLD(ADD(_, C_I64))
IR_FOLD(ADD(_, C_ADDR))
IR_FOLD(SUB(_, C_U8))
IR_FOLD(SUB(_, C_U16))
IR_FOLD(SUB(_, C_U32))
IR_FOLD(SUB(_, C_U64))
IR_FOLD(SUB(_, C_I8))
IR_FOLD(SUB(_, C_I16))
IR_FOLD(SUB(_, C_I32))
IR_FOLD(SUB(_, C_I64))
IR_FOLD(SUB(_, C_ADDR))
IR_FOLD(ADD_OV(_, C_U8))
IR_FOLD(ADD_OV(_, C_U16))
IR_FOLD(ADD_OV(_, C_U32))
IR_FOLD(ADD_OV(_, C_U64))
IR_FOLD(ADD_OV(_, C_I8))
IR_FOLD(ADD_OV(_, C_I16))
IR_FOLD(ADD_OV(_, C_I32))
IR_FOLD(ADD_OV(_, C_I64))
IR_FOLD(ADD_OV(_, C_ADDR))
IR_FOLD(SUB_OV(_, C_U8))
IR_FOLD(SUB_OV(_, C_U16))
IR_FOLD(SUB_OV(_, C_U32))
IR_FOLD(SUB_OV(_, C_U64))
IR_FOLD(SUB_OV(_, C_I8))
IR_FOLD(SUB_OV(_, C_I16))
IR_FOLD(SUB_OV(_, C_I32))
IR_FOLD(SUB_OV(_, C_I64))
IR_FOLD(SUB_OV(_, C_ADDR))
{
	if (op2_insn->val.u64 == 0) {
		/* a +/- 0 => a */
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SUB(C_I8, _))
IR_FOLD(SUB(C_I16, _))
IR_FOLD(SUB(C_I32, _))
IR_FOLD(SUB(C_I64, _))
{
	if (op1_insn->val.u64 == 0) {
		/* 0 - a => -a (invalid for +0.0) */
		opt = IR_NEG | (opt & IR_OPT_TYPE_MASK);
		op1 = op2;
		op2 = IR_UNUSED;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(UGE(_, C_U8))
IR_FOLD(UGE(_, C_U16))
IR_FOLD(UGE(_, C_U32))
IR_FOLD(UGE(_, C_U64))
IR_FOLD(UGE(_, C_I8))
IR_FOLD(UGE(_, C_I16))
IR_FOLD(UGE(_, C_I32))
IR_FOLD(UGE(_, C_I64))
IR_FOLD(UGE(_, C_ADDR))
{
	if (op2_insn->val.u64 == 0) {
		IR_FOLD_COPY(IR_TRUE);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(UGT(_, C_U8))
IR_FOLD(UGT(_, C_U16))
IR_FOLD(UGT(_, C_U32))
IR_FOLD(UGT(_, C_U64))
IR_FOLD(UGT(_, C_I8))
IR_FOLD(UGT(_, C_I16))
IR_FOLD(UGT(_, C_I32))
IR_FOLD(UGT(_, C_I64))
IR_FOLD(UGT(_, C_ADDR))
{
	if (op2_insn->val.u64 == 0) {
		opt = IR_OPT(IR_NE, IR_BOOL);
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(ULT(_, C_U8))
IR_FOLD(ULT(_, C_U16))
IR_FOLD(ULT(_, C_U32))
IR_FOLD(ULT(_, C_U64))
IR_FOLD(ULT(_, C_I8))
IR_FOLD(ULT(_, C_I16))
IR_FOLD(ULT(_, C_I32))
IR_FOLD(ULT(_, C_I64))
IR_FOLD(ULT(_, C_ADDR))
{
	if (op2_insn->val.u64 == 0) {
		IR_FOLD_COPY(IR_FALSE);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(ULE(_, C_U8))
IR_FOLD(ULE(_, C_U16))
IR_FOLD(ULE(_, C_U32))
IR_FOLD(ULE(_, C_U64))
IR_FOLD(ULE(_, C_I8))
IR_FOLD(ULE(_, C_I16))
IR_FOLD(ULE(_, C_I32))
IR_FOLD(ULE(_, C_I64))
IR_FOLD(ULE(_, C_ADDR))
{
	if (op2_insn->val.u64 == 0) {
		opt = IR_OPT(IR_EQ, IR_BOOL);
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(ADD(NEG, _))
{
	/* (-a) + b => b - a */
	opt++; /* ADD -> SUB */
	op1 = op2;
	op2 = op1_insn->op1;
	IR_FOLD_RESTART;
}

IR_FOLD(ADD(_, NEG))
IR_FOLD(SUB(_,NEG))
{
	/* a + (-b) => a - b */
	opt ^= 1; /* ADD <-> SUB */
	op2 = op2_insn->op1;
	IR_FOLD_RESTART;
}

IR_FOLD(ADD(SUB, _))
{
	if (IR_IS_TYPE_INT(IR_OPT_TYPE(opt))) {
		if (op1_insn->op2 == op2) {
			/* (a - b) + b => a */
			IR_FOLD_COPY(op1_insn->op1);
		}
	}
	IR_FOLD_NEXT;
}

IR_FOLD(ADD(_, SUB))
{
	if (IR_IS_TYPE_INT(IR_OPT_TYPE(opt))) {
		if (op2_insn->op2 == op1) {
			/* a + (b - a) => b */
			IR_FOLD_COPY(op2_insn->op1);
		}
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SUB(ADD, _))
{
	if (IR_IS_TYPE_INT(IR_OPT_TYPE(opt))) {
		if (op1_insn->op1 == op2) {
			/* (a + b) - a => b */
			IR_FOLD_COPY(op1_insn->op2);
		} else if (op1_insn->op2 == op2) {
			/* (a + b) - a => b */
			IR_FOLD_COPY(op1_insn->op1);
		}
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SUB(_, ADD))
{
	if (IR_IS_TYPE_INT(IR_OPT_TYPE(opt))) {
		if (op2_insn->op1 == op1) {
			/* a - (a + b) => -b */
			opt = IR_NEG | (opt & IR_OPT_TYPE_MASK);
			op1 = op2_insn->op2;
			op2 = IR_UNUSED;
			IR_FOLD_RESTART;
		} else if (op2_insn->op2 == op1) {
			/* b - (a + b) => -a */
			opt = IR_NEG | (opt & IR_OPT_TYPE_MASK);
			op1 = op2_insn->op1;
			op2 = IR_UNUSED;
			IR_FOLD_RESTART;
		}
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SUB(SUB, _))
{
	if (IR_IS_TYPE_INT(IR_OPT_TYPE(opt))) {
		if (op1_insn->op1 == op2) {
			/* (a - b) - a => -b */
			opt = IR_NEG | (opt & IR_OPT_TYPE_MASK);
			op1 = op1_insn->op2;
			op2 = IR_UNUSED;
			IR_FOLD_RESTART;
		}
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SUB(_, SUB))
{
	if (IR_IS_TYPE_INT(IR_OPT_TYPE(opt))) {
		if (op2_insn->op1 == op1) {
			/* a - (a - b) => b */
			IR_FOLD_COPY(op2_insn->op2);
		}
	}
    IR_FOLD_NEXT;
}

IR_FOLD(SUB(ADD, ADD))
{
	if (IR_IS_TYPE_INT(IR_OPT_TYPE(opt))) {
		if (op1_insn->op1 == op2_insn->op1) {
			/* (a + b) - (a + c) => b - c */
			op1 = op1_insn->op2;
			op2 = op2_insn->op2;
			IR_FOLD_RESTART;
		} else if (op1_insn->op1 == op2_insn->op2) {
			/* (a + b) - (c + a) => b - c */
			op1 = op1_insn->op2;
			op2 = op2_insn->op1;
			IR_FOLD_RESTART;
		} else if (op1_insn->op2 == op2_insn->op1) {
			/* (a + b) - (b + c) => a - c */
			op1 = op1_insn->op1;
			op2 = op2_insn->op2;
			IR_FOLD_RESTART;
		} else if (op1_insn->op2 == op2_insn->op2) {
			/* (a + b) - (c + b) => a - c */
			op1 = op1_insn->op1;
			op2 = op2_insn->op1;
			IR_FOLD_RESTART;
		}
	}
    IR_FOLD_NEXT;
}

// IR_FOLD(SUB(NEG, CONST))  TODO: -a - b => -b - a
// IR_FOLD(MUL(NEG, CONST))  TODO: -a * b => a * -b
// IR_FOLD(DIV(NEG, CONST))  TODO: -a / b => a / -b

IR_FOLD(MUL(_, C_U8))
IR_FOLD(MUL(_, C_U16))
IR_FOLD(MUL(_, C_U32))
IR_FOLD(MUL(_, C_U64))
IR_FOLD(MUL(_, C_ADDR))
{
	if (op2_insn->val.u64 == 0) {
		/* a * 0 => 0 */
		IR_FOLD_COPY(op2);
	} else if (op2_insn->val.u64 == 1) {
		IR_FOLD_COPY(op1);
	} else if (op2_insn->val.u64 == 2 && IR_OPT_TYPE(opt) != IR_ADDR) {
		opt = IR_ADD | (opt & IR_OPT_TYPE_MASK);
		op2 = op1;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(MUL(_, C_I8))
IR_FOLD(MUL(_, C_I16))
IR_FOLD(MUL(_, C_I32))
IR_FOLD(MUL(_, C_I64))
{
	if (op2_insn->val.i64 == 0) {
		/* a * 0 => 0 */
		IR_FOLD_COPY(op2);
	} else if (op2_insn->val.i64 == 1) {
		/* a * 1 => a */
		IR_FOLD_COPY(op1);
	} else if (op2_insn->val.i64 == 2) {
		/* a * 2 => a + a */
		opt = IR_ADD | (opt & IR_OPT_TYPE_MASK);
		op2 = op1;
		IR_FOLD_RESTART;
	} else if (op2_insn->val.i64 == -1) {
		/* a * -1 => -a */
		opt = IR_NEG | (opt & IR_OPT_TYPE_MASK);
		op2 = IR_UNUSED;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(MUL(_, C_DOUBLE))
{
	if (op2_insn->val.d == 1.0) {
		/* a * 1.0 => a */
		IR_FOLD_COPY(op1);
	} else if (op2_insn->val.d == 2.0) {
		/* a * 2.0 => a + a */
		opt = IR_ADD | (opt & IR_OPT_TYPE_MASK);
		op2 = op1;
		IR_FOLD_RESTART;
	} else if (op2_insn->val.d == -1.0) {
		/* a * -1.0 => -a */
		opt = IR_NEG | (opt & IR_OPT_TYPE_MASK);
		op2 = IR_UNUSED;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(MUL(_, C_FLOAT))
{
	if (op2_insn->val.f == 1.0) {
		/* a * 1.0 => a */
		IR_FOLD_COPY(op1);
	} else if (op2_insn->val.f == 2.0) {
		/* a * 2.0 => a + a */
		opt = IR_ADD | (opt & IR_OPT_TYPE_MASK);
		op2 = op1;
		IR_FOLD_RESTART;
	} else if (op2_insn->val.f == -1.0) {
		/* a * -1.0 => -a */
		opt = IR_NEG | (opt & IR_OPT_TYPE_MASK);
		op2 = IR_UNUSED;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(DIV(_, C_U8))
IR_FOLD(DIV(_, C_U16))
IR_FOLD(DIV(_, C_U32))
IR_FOLD(DIV(_, C_U64))
{
	if (op2_insn->val.u64 == 1) {
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(DIV(_, C_I8))
IR_FOLD(DIV(_, C_I16))
IR_FOLD(DIV(_, C_I32))
IR_FOLD(DIV(_, C_I64))
{
	if (op2_insn->val.i64 == 1) {
		/* a / 1 => a */
		IR_FOLD_COPY(op1);
	} else if (op2_insn->val.i64 == -1) {
		/* a / -1 => -a */
		opt = IR_NEG | (opt & IR_OPT_TYPE_MASK);
		op2 = IR_UNUSED;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(MOD(_, C_U8))
IR_FOLD(MOD(_, C_U16))
IR_FOLD(MOD(_, C_U32))
IR_FOLD(MOD(_, C_U64))
IR_FOLD(MOD(_, C_I8))
IR_FOLD(MOD(_, C_I16))
IR_FOLD(MOD(_, C_I32))
IR_FOLD(MOD(_, C_I64))
{
	if (op2_insn->val.i64 == 1) {
		/* a % 1 => 0 */
		IR_FOLD_CONST_U(0);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(DIV(_, C_DOUBLE))
{
	if (op2_insn->val.d == 1.0) {
		/* a / 1.0 => a */
		IR_FOLD_COPY(op1);
	} else if (op2_insn->val.d == -1.0) {
		/* a / -1.0 => -a */
		opt = IR_NEG | (opt & IR_OPT_TYPE_MASK);
		op2 = IR_UNUSED;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(DIV(_, C_FLOAT))
{
	if (op2_insn->val.f == 1.0) {
		/* a / 1.0 => a */
		IR_FOLD_COPY(op1);
	} else if (op2_insn->val.f == -1.0) {
		/* a / -1.0 => -a */
		opt = IR_NEG | (opt & IR_OPT_TYPE_MASK);
		op2 = IR_UNUSED;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(MUL(NEG, NEG))
IR_FOLD(DIV(NEG, NEG))
{
	op1 = op1_insn->op1;
	op2 = op2_insn->op1;
	IR_FOLD_RESTART;
}

IR_FOLD(AND(_, C_BOOL))
{
	IR_FOLD_COPY(op2_insn->val.b ? op1 : op2);
}

IR_FOLD(AND(_, C_U8))
IR_FOLD(AND(_, C_I8))
IR_FOLD(AND(_, C_CHAR))
{
	if (op2_insn->val.i8 == 0) {
		/* a & 0 => 0 */
		IR_FOLD_COPY(op2);
	} else if (op2_insn->val.i8 == -1) {
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(AND(_, C_U16))
IR_FOLD(AND(_, C_I16))
{
	if (op2_insn->val.i16 == 0) {
		/* a & 0 => 0 */
		IR_FOLD_COPY(op2);
	} else if (op2_insn->val.i16 == -1) {
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(AND(_, C_U32))
IR_FOLD(AND(_, C_I32))
{
	if (op2_insn->val.i32 == 0) {
		/* a & 0 => 0 */
		IR_FOLD_COPY(op2);
	} else if (op2_insn->val.i32 == -1) {
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(AND(_, C_U64))
IR_FOLD(AND(_, C_I64))
{
	if (op2_insn->val.i64 == 0) {
		/* a & 0 => 0 */
		IR_FOLD_COPY(op2);
	} else if (op2_insn->val.i64 == -1) {
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(OR(_, C_BOOL))
{
	IR_FOLD_COPY(op2_insn->val.b ? op2 : op1);
}

IR_FOLD(OR(_, C_U8))
IR_FOLD(OR(_, C_I8))
IR_FOLD(OR(_, C_CHAR))
{
	if (op2_insn->val.i8 == -1) {
		/* a | 1 => 1 */
		IR_FOLD_COPY(op2);
	} else if (op2_insn->val.i8 == 0) {
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(OR(_, C_U16))
IR_FOLD(OR(_, C_I16))
{
	if (op2_insn->val.i16 == -1) {
		/* a | 1 => 1 */
		IR_FOLD_COPY(op2);
	} else if (op2_insn->val.i16 == 0) {
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(OR(_, C_U32))
IR_FOLD(OR(_, C_I32))
{
	if (op2_insn->val.i32 == -1) {
		/* a | 1 => 1 */
		IR_FOLD_COPY(op2);
	} else if (op2_insn->val.i32 == -0) {
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(OR(_, C_U64))
IR_FOLD(OR(_, C_I64))
{
	if (op2_insn->val.i64 == -1) {
		/* a | 1 => 1 */
		IR_FOLD_COPY(op2);
	} else if (op2_insn->val.i64 == 0) {
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(XOR(_, C_BOOL))
{
	if (!op2_insn->val.b) {
		/* a ^ 0 => a */
		IR_FOLD_COPY(op1);
	} else {
		/* a ^ 1 => !a */
		opt = IR_NOT | (opt & IR_OPT_TYPE_MASK);
		op2 = IR_UNUSED;
		IR_FOLD_RESTART;
	}
}

IR_FOLD(XOR(_, C_U8))
IR_FOLD(XOR(_, C_I8))
IR_FOLD(XOR(_, C_CHAR))
{
	if (op2_insn->val.i8 == 0) {
		/* a ^ 0 => a */
		IR_FOLD_COPY(op1);
	} else if (op2_insn->val.i8 == -1) {
		/* a ^ 1 => ~a */
		opt = IR_NOT | (opt & IR_OPT_TYPE_MASK);
		op2 = IR_UNUSED;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(XOR(_, C_U16))
IR_FOLD(XOR(_, C_I16))
{
	if (op2_insn->val.i16 == 0) {
		/* a ^ 0 => a */
		IR_FOLD_COPY(op1);
	} else if (op2_insn->val.i16 == -1) {
		/* a ^ 1 => ~a */
		opt = IR_NOT | (opt & IR_OPT_TYPE_MASK);
		op2 = IR_UNUSED;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(XOR(_, C_U32))
IR_FOLD(XOR(_, C_I32))
{
	if (op2_insn->val.i32 == 0) {
		/* a ^ 0 => a */
		IR_FOLD_COPY(op1);
	} else if (op2_insn->val.i32 == -1) {
		/* a ^ 1 => ~a */
		opt = IR_NOT | (opt & IR_OPT_TYPE_MASK);
		op2 = IR_UNUSED;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(XOR(_, C_U64))
IR_FOLD(XOR(_, C_I64))
{
	if (op2_insn->val.i64 == 0) {
		/* a ^ 0 => a */
		IR_FOLD_COPY(op1);
	} else if (op2_insn->val.i64 == -1) {
		/* a ^ 1 => ~a */
		opt = IR_NOT | (opt & IR_OPT_TYPE_MASK);
		op2 = IR_UNUSED;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SHL(_, C_U8))
IR_FOLD(SHL(_, C_U16))
IR_FOLD(SHL(_, C_U32))
IR_FOLD(SHL(_, C_U64))
IR_FOLD(SHL(_, C_I8))
IR_FOLD(SHL(_, C_I16))
IR_FOLD(SHL(_, C_I32))
IR_FOLD(SHL(_, C_I64))
{
	if (op2_insn->val.u64 == 0) {
		/* a << 0 => a */
		IR_FOLD_COPY(op1);
	} else if (op2_insn->val.u64 == 1) {
		/* a << 1 => a + a */
		opt = IR_ADD | (opt & IR_OPT_TYPE_MASK);
		op2 = op1;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SHR(_, C_U8))
IR_FOLD(SHR(_, C_U16))
IR_FOLD(SHR(_, C_U32))
IR_FOLD(SHR(_, C_U64))
IR_FOLD(SHR(_, C_I8))
IR_FOLD(SHR(_, C_I16))
IR_FOLD(SHR(_, C_I32))
IR_FOLD(SHR(_, C_I64))
IR_FOLD(SAR(_, C_U8))
IR_FOLD(SAR(_, C_U16))
IR_FOLD(SAR(_, C_U32))
IR_FOLD(SAR(_, C_U64))
IR_FOLD(SAR(_, C_I8))
IR_FOLD(SAR(_, C_I16))
IR_FOLD(SAR(_, C_I32))
IR_FOLD(SAR(_, C_I64))
IR_FOLD(ROL(_, C_U8))
IR_FOLD(ROL(_, C_U16))
IR_FOLD(ROL(_, C_U32))
IR_FOLD(ROL(_, C_U64))
IR_FOLD(ROL(_, C_I8))
IR_FOLD(ROL(_, C_I16))
IR_FOLD(ROL(_, C_I32))
IR_FOLD(ROL(_, C_I64))
IR_FOLD(ROR(_, C_U8))
IR_FOLD(ROR(_, C_U16))
IR_FOLD(ROR(_, C_U32))
IR_FOLD(ROR(_, C_U64))
IR_FOLD(ROR(_, C_I8))
IR_FOLD(ROR(_, C_I16))
IR_FOLD(ROR(_, C_I32))
IR_FOLD(ROR(_, C_I64))
{
	if (op2_insn->val.u64 == 0) {
		/* a >> 0 => a */
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SHL(C_U8, _))
IR_FOLD(SHL(C_U16, _))
IR_FOLD(SHL(C_U32, _))
IR_FOLD(SHL(C_U64, _))
IR_FOLD(SHL(C_I8, _))
IR_FOLD(SHL(C_I16, _))
IR_FOLD(SHL(C_I32, _))
IR_FOLD(SHL(C_I64, _))
IR_FOLD(SHR(C_U8, _))
IR_FOLD(SHR(C_U16, _))
IR_FOLD(SHR(C_U32, _))
IR_FOLD(SHR(C_U64, _))
IR_FOLD(SHR(C_I8, _))
IR_FOLD(SHR(C_I16, _))
IR_FOLD(SHR(C_I32, _))
IR_FOLD(SHR(C_I64, _))
{
	if (op1_insn->val.u64 == 0) {
		/* 0 << a => 0 */
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SAR(C_U8, _))
IR_FOLD(SAR(C_I8, _))
IR_FOLD(ROL(C_U8, _))
IR_FOLD(ROL(C_I8, _))
IR_FOLD(ROR(C_U8, _))
IR_FOLD(ROR(C_I8, _))
{
	if (op1_insn->val.i8 == 0 || op1_insn->val.i8 == -1) {
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SAR(C_U16, _))
IR_FOLD(SAR(C_I16, _))
IR_FOLD(ROL(C_U16, _))
IR_FOLD(ROL(C_I16, _))
IR_FOLD(ROR(C_U16, _))
IR_FOLD(ROR(C_I16, _))
{
	if (op1_insn->val.i16 == 0 || op1_insn->val.i16 == -1) {
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SAR(C_U32, _))
IR_FOLD(SAR(C_I32, _))
IR_FOLD(ROL(C_U32, _))
IR_FOLD(ROL(C_I32, _))
IR_FOLD(ROR(C_U32, _))
IR_FOLD(ROR(C_I32, _))
{
	if (op1_insn->val.i32 == 0 || op1_insn->val.i32 == -1) {
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SAR(C_U64, _))
IR_FOLD(SAR(C_I64, _))
IR_FOLD(ROL(C_U64, _))
IR_FOLD(ROL(C_I64, _))
IR_FOLD(ROR(C_U64, _))
IR_FOLD(ROR(C_I64, _))
{
	if (op1_insn->val.i64 == 0 || op1_insn->val.i64 == -1) {
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(LT(ABS, C_I8))
IR_FOLD(LT(ABS, C_I16))
IR_FOLD(LT(ABS, C_I32))
IR_FOLD(LT(ABS, C_I64))
IR_FOLD(LT(ABS, C_FLOAT))
IR_FOLD(LT(ABS, C_DOUBLE))
{
	if (op2_insn->val.u64 == 0) {
		/* abs() < 0 => false */
		IR_FOLD_COPY(IR_FALSE);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(GE(ABS, C_I8))
IR_FOLD(GE(ABS, C_I16))
IR_FOLD(GE(ABS, C_I32))
IR_FOLD(GE(ABS, C_I64))
IR_FOLD(GE(ABS, C_FLOAT))
IR_FOLD(GE(ABS, C_DOUBLE))
{
	if (op2_insn->val.u64 == 0) {
		/* abs() >= 0 => true */
		IR_FOLD_COPY(IR_TRUE);
	}
	IR_FOLD_NEXT;
}

// TODO: conversions
IR_FOLD(FP2FP(FP2FP))
{
	if (IR_OPT_TYPE(opt) == IR_FLOAT) {
		/* (float)(double)f => f */
		IR_ASSERT(op1_insn->type == IR_DOUBLE);
		IR_ASSERT(ctx->ir_base[op1_insn->op1].type == IR_FLOAT);
		IR_FOLD_COPY(op1_insn->op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(FP2INT(INT2FP))
{
	ir_type dst_type = IR_OPT_TYPE(opt);
	ir_type src_type = ctx->ir_base[op1_insn->op1].type;

	if (ir_type_size[src_type] >= ir_type_size[op1_insn->type]) {
		/* source integer type can not fit into intermediate floating point */
		IR_FOLD_NEXT;
	}
	/* (int)(double)i => i */
	if (src_type == dst_type) {
		IR_FOLD_COPY(op1_insn->op1);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(TRUNC(ZEXT))
IR_FOLD(TRUNC(SEXT))
{
	ir_type dst_type = IR_OPT_TYPE(opt);
	ir_type src_type = ctx->ir_base[op1_insn->op1].type;

	/* (int32_t)(int64_t)i => i */
	if (src_type == dst_type) {
		IR_FOLD_COPY(op1_insn->op1);
	} else if (ir_type_size[src_type] == ir_type_size[dst_type]) {
		opt = IR_OPT(IR_BITCAST, dst_type);
		op1 = op1_insn->op1;
		IR_FOLD_RESTART;
	} else if (ir_type_size[src_type] > ir_type_size[dst_type]) {
		opt = IR_OPT(IR_TRUNC, dst_type);
		op1 = op1_insn->op1;
		IR_FOLD_RESTART;
	} else {
		opt = IR_OPT(op1_insn->op, dst_type);
		op1 = op1_insn->op1;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(TRUNC(BITCAST))
IR_FOLD(ZEXT(BITCAST))
IR_FOLD(SEXT(BITCAST))
{
	if (IR_IS_TYPE_INT(ctx->ir_base[op1_insn->op1].type)) {
		op1 = op1_insn->op1;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(BITCAST(BITCAST))
{
	ir_type dst_type = IR_OPT_TYPE(opt);
	ir_type src_type = ctx->ir_base[op1_insn->op1].type;

	if (src_type == dst_type) {
		IR_FOLD_COPY(op1_insn->op1);
	} else if (IR_IS_TYPE_INT(src_type) == IR_IS_TYPE_INT(dst_type)) {
		op1 = op1_insn->op1;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(TRUNC(TRUNC))
IR_FOLD(ZEXT(ZEXT))
IR_FOLD(SEXT(SEXT))
{
	op1 = op1_insn->op1;
	IR_FOLD_RESTART;
}

IR_FOLD(SEXT(ZEXT))
{
	op1 = op1_insn->op1;
	opt = IR_OPT(IR_ZEXT, IR_OPT_TYPE(opt));
	IR_FOLD_RESTART;
}

IR_FOLD(SEXT(AND))
{
	if (IR_IS_CONST_REF(op1_insn->op2)
	 && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op2].op)
	 && !(ctx->ir_base[op1_insn->op2].val.u64
			& (1ULL << ((ir_type_size[op1_insn->type] * 8) - 1)))) {
		/* SEXT(AND(_, 0b0*)) -> ZEXT(AND(_, 0b0*)) */
		opt = IR_OPT(IR_ZEXT, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(TRUNC(AND))
{
	if (IR_IS_CONST_REF(op1_insn->op2)) {
		size_t size = ir_type_size[IR_OPT_TYPE(opt)];
		uint64_t mask = ctx->ir_base[op1_insn->op2].val.u64;

		if (size == 1) {
			if (mask == 0xff) {
				op1 = op1_insn->op1;
				IR_FOLD_RESTART;
			}
		} else if (size == 2) {
			if (mask == 0xffff) {
				op1 = op1_insn->op1;
				IR_FOLD_RESTART;
			}
		} else if (size == 4) {
			if (mask == 0xffffffff) {
				op1 = op1_insn->op1;
				IR_FOLD_RESTART;
			}
		}
	}
	IR_FOLD_NEXT;
}

IR_FOLD(AND(SHR, C_I8))
IR_FOLD(AND(SHR, C_U8))
{
	if (IR_IS_CONST_REF(op1_insn->op2)) {
		if (((uint8_t)-1) >> ctx->ir_base[op1_insn->op2].val.u8 == op2_insn->val.u8) {
			IR_FOLD_COPY(op1);
		}
	}
	IR_FOLD_NEXT;
}

IR_FOLD(AND(SHR, C_I16))
IR_FOLD(AND(SHR, C_U16))
{
	if (IR_IS_CONST_REF(op1_insn->op2)) {
		if (((uint16_t)-1) >> ctx->ir_base[op1_insn->op2].val.u16 == op2_insn->val.u16) {
			IR_FOLD_COPY(op1);
		}
	}
	IR_FOLD_NEXT;
}

IR_FOLD(AND(SHR, C_I32))
IR_FOLD(AND(SHR, C_U32))
{
	if (IR_IS_CONST_REF(op1_insn->op2)) {
		if (((uint32_t)-1) >> ctx->ir_base[op1_insn->op2].val.u32 == op2_insn->val.u32) {
			IR_FOLD_COPY(op1);
		}
	}
	IR_FOLD_NEXT;
}

IR_FOLD(AND(SHR, C_I64))
IR_FOLD(AND(SHR, C_U64))
{
	if (IR_IS_CONST_REF(op1_insn->op2)) {
		if (((uint64_t)-1) >> ctx->ir_base[op1_insn->op2].val.u64 == op2_insn->val.u64) {
			IR_FOLD_COPY(op1);
		}
	}
	IR_FOLD_NEXT;
}

IR_FOLD(EQ(FP2FP, C_DOUBLE))
IR_FOLD(NE(FP2FP, C_DOUBLE))
IR_FOLD(LT(FP2FP, C_DOUBLE))
IR_FOLD(GE(FP2FP, C_DOUBLE))
IR_FOLD(LE(FP2FP, C_DOUBLE))
IR_FOLD(GT(FP2FP, C_DOUBLE))
IR_FOLD(ULT(FP2FP, C_DOUBLE))
IR_FOLD(UGE(FP2FP, C_DOUBLE))
IR_FOLD(ULE(FP2FP, C_DOUBLE))
IR_FOLD(UGT(FP2FP, C_DOUBLE))
{
	IR_ASSERT(op1_insn->type == IR_DOUBLE);
	IR_ASSERT(ctx->ir_base[op1_insn->op1].type == IR_FLOAT);
	if (op2_insn->val.d == (double)(float)op2_insn->val.d) {
		op1 = op1_insn->op1;
		op2 = ir_const_float(ctx, (float)op2_insn->val.d);
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

// TODO: Reassociation
IR_FOLD(ADD(ADD, C_U8))
IR_FOLD(ADD(ADD, C_U16))
IR_FOLD(ADD(ADD, C_U32))
IR_FOLD(ADD(ADD, C_U64))
IR_FOLD(ADD(ADD, C_ADDR))
{
	if (IR_IS_CONST_REF(op1_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op2].op)) {
		/* (x + c1) + c2  => x + (c1 + c2) */
		val.u64 = ctx->ir_base[op1_insn->op2].val.u64 + op2_insn->val.u64;
		op1 = op1_insn->op1;
		op2 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(ADD(ADD, C_I8))
IR_FOLD(ADD(ADD, C_I16))
IR_FOLD(ADD(ADD, C_I32))
IR_FOLD(ADD(ADD, C_I64))
{
	if (IR_IS_CONST_REF(op1_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op2].op)) {
		/* (x + c1) + c2  => x + (c1 + c2) */
		val.i64 = ctx->ir_base[op1_insn->op2].val.i64 + op2_insn->val.i64;
		op1 = op1_insn->op1;
		op2 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(ADD(SUB, C_U8))
IR_FOLD(ADD(SUB, C_U16))
IR_FOLD(ADD(SUB, C_U32))
IR_FOLD(ADD(SUB, C_U64))
IR_FOLD(ADD(SUB, C_ADDR))
{
	if (IR_IS_CONST_REF(op1_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op2].op)) {
		/* (x - c1) + c2  => x + (c2 - c1) */
		val.u64 = op2_insn->val.u64 - ctx->ir_base[op1_insn->op2].val.u64;
		op1 = op1_insn->op1;
		op2 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	} else if (IR_IS_CONST_REF(op1_insn->op1) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op1].op)) {
		/* (c1 - x) + c2  => (c1 + c2) - x */
		val.u64 = ctx->ir_base[op1_insn->op1].val.u64 + op2_insn->val.u64;
		opt++; /* ADD -> SUB */
		op1 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		op2 = op1_insn->op2;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(ADD(SUB, C_I8))
IR_FOLD(ADD(SUB, C_I16))
IR_FOLD(ADD(SUB, C_I32))
IR_FOLD(ADD(SUB, C_I64))
{
	if (IR_IS_CONST_REF(op1_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op2].op)) {
		/* (x - c1) + c2  => x + (c2 - c1) */
		val.i64 = op2_insn->val.i64 - ctx->ir_base[op1_insn->op2].val.i64;
		if (val.i64 < 0 && val.i64 - 1 < 0) {
			val.i64 = -val.i64;
			opt++; /* ADD -> SUB */
		}
		op1 = op1_insn->op1;
		op2 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	} else if (IR_IS_CONST_REF(op1_insn->op1) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op1].op)) {
		/* (c1 - x) + c2  => (c1 + c2) - x */
		val.i64 = ctx->ir_base[op1_insn->op1].val.i64 + op2_insn->val.i64;
		opt++; /* ADD -> SUB */
		op1 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		op2 = op1_insn->op2;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SUB(ADD, C_U8))
IR_FOLD(SUB(ADD, C_U16))
IR_FOLD(SUB(ADD, C_U32))
IR_FOLD(SUB(ADD, C_U64))
IR_FOLD(SUB(ADD, C_ADDR))
{
	if (IR_IS_CONST_REF(op1_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op2].op)) {
		/* (x + c1) - c2  => x + (c1 - c2) */
		val.u64 = ctx->ir_base[op1_insn->op2].val.u64 - op2_insn->val.u64;
		opt--; /* SUB -> ADD */
		op1 = op1_insn->op1;
		op2 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SUB(ADD, C_I8))
IR_FOLD(SUB(ADD, C_I16))
IR_FOLD(SUB(ADD, C_I32))
IR_FOLD(SUB(ADD, C_I64))
{
	if (IR_IS_CONST_REF(op1_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op2].op)) {
		/* (x + c1) - c2  => x + (c1 - c2) */
		val.i64 = ctx->ir_base[op1_insn->op2].val.i64 - op2_insn->val.i64;
		if (val.i64 < 0 && val.i64 - 1 < 0) {
			val.i64 = -val.i64;
		} else {
			opt--; /* SUB -> ADD */
		}
		op1 = op1_insn->op1;
		op2 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SUB(C_U8, ADD))
IR_FOLD(SUB(C_U16, ADD))
IR_FOLD(SUB(C_U32, ADD))
IR_FOLD(SUB(C_U64, ADD))
IR_FOLD(SUB(C_ADDR, ADD))
{
	if (IR_IS_CONST_REF(op2_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op2_insn->op2].op)) {
		/* c1 - (x + c2) => (c1 - c2) - x */
		val.u64 = op1_insn->val.u64 - ctx->ir_base[op2_insn->op2].val.u64;
		op1 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		op2 = op2_insn->op1;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SUB(C_I8, ADD))
IR_FOLD(SUB(C_I16, ADD))
IR_FOLD(SUB(C_I32, ADD))
IR_FOLD(SUB(C_I64, ADD))
{
	if (IR_IS_CONST_REF(op2_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op2_insn->op2].op)) {
		/* c1 - (x + c2) => (c1 - c2) - x */
		val.i64 = op1_insn->val.i64 - ctx->ir_base[op2_insn->op2].val.i64;
		op1 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		op2 = op2_insn->op1;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SUB(SUB, C_U8))
IR_FOLD(SUB(SUB, C_U16))
IR_FOLD(SUB(SUB, C_U32))
IR_FOLD(SUB(SUB, C_U64))
IR_FOLD(SUB(SUB, C_ADDR))
{
	if (IR_IS_CONST_REF(op1_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op2].op)) {
		/* (x - c1) - c2  => x - (c1 + c2) */
		val.u64 = ctx->ir_base[op1_insn->op2].val.u64 + op2_insn->val.u64;
		if (val.i64 < 0 && val.i64 - 1 < 0) {
			val.i64 = -val.i64;
			opt--; /* SUB -> ADD */
		}
		op1 = op1_insn->op1;
		op2 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	} else if (IR_IS_CONST_REF(op1_insn->op1) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op1].op)) {
		/* (c1 - x) - c2  => (c1 - c2) - x */
		val.u64 = ctx->ir_base[op1_insn->op1].val.u64 - op2_insn->val.u64;
		op1 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		op2 = op1_insn->op2;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SUB(SUB, C_I8))
IR_FOLD(SUB(SUB, C_I16))
IR_FOLD(SUB(SUB, C_I32))
IR_FOLD(SUB(SUB, C_I64))
{
	if (IR_IS_CONST_REF(op1_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op2].op)) {
		/* (x - c1) - c2  => x - (c1 + c2) */
		val.i64 = ctx->ir_base[op1_insn->op2].val.i64 + op2_insn->val.i64;
		if (val.i64 < 0 && val.i64 - 1 < 0) {
			val.i64 = -val.i64;
			opt--; /* SUB -> ADD */
		}
		op1 = op1_insn->op1;
		op2 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	} else if (IR_IS_CONST_REF(op1_insn->op1) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op1].op)) {
		/* (c1 - x) - c2  => (c1 - c2) - x */
		val.i64 = ctx->ir_base[op1_insn->op1].val.i64 - op2_insn->val.i64;
		op1 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		op2 = op1_insn->op2;
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SUB(C_U8, SUB))
IR_FOLD(SUB(C_U16, SUB))
IR_FOLD(SUB(C_U32, SUB))
IR_FOLD(SUB(C_U64, SUB))
IR_FOLD(SUB(C_ADDR, SUB))
{
	if (IR_IS_CONST_REF(op2_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op2_insn->op2].op)) {
		/* c1 - (x - c2) => (c1 + c2) - x */
		val.u64 = op1_insn->val.u64 + ctx->ir_base[op2_insn->op2].val.u64;
		op1 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		op2 = op2_insn->op1;
		IR_FOLD_RESTART;
	} else if (IR_IS_CONST_REF(op2_insn->op1) && !IR_IS_SYM_CONST(ctx->ir_base[op2_insn->op1].op)) {
		/* c1 - (c2 - x) => x + (c1 - c2) */
		val.u64 = op1_insn->val.u64 - ctx->ir_base[op2_insn->op1].val.u64;
		if (val.i64 < 0 && val.i64 - 1 < 0) {
			val.i64 = -val.i64;
			opt++; /* ADD -> SUB */
		}
		op1 = op2_insn->op2;
		op2 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SUB(C_I8, SUB))
IR_FOLD(SUB(C_I16, SUB))
IR_FOLD(SUB(C_I32, SUB))
IR_FOLD(SUB(C_I64, SUB))
{
	if (IR_IS_CONST_REF(op2_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op2_insn->op2].op)) {
		/* c1 - (x - c2) => (c1 + c2) - x */
		val.i64 = op1_insn->val.i64 + ctx->ir_base[op2_insn->op2].val.i64;
		op1 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		op2 = op2_insn->op1;
		IR_FOLD_RESTART;
	} else if (IR_IS_CONST_REF(op2_insn->op1) && !IR_IS_SYM_CONST(ctx->ir_base[op2_insn->op1].op)) {
		/* c1 - (c2 - x) => x + (c1 - c2) */
		val.i64 = op1_insn->val.i64 - ctx->ir_base[op2_insn->op1].val.i64;
		if (val.i64 < 0 && val.i64 - 1 < 0) {
			val.i64 = -val.i64;
			opt++; /* ADD -> SUB */
		}
		op1 = op2_insn->op2;
		op2 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(MUL(MUL, C_U8))
IR_FOLD(MUL(MUL, C_U16))
IR_FOLD(MUL(MUL, C_U32))
IR_FOLD(MUL(MUL, C_U64))
IR_FOLD(MUL(MUL, C_ADDR))
{
	if (IR_IS_CONST_REF(op1_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op2].op)) {
		/* (x * c1) * c2  => x * (c1 * c2) */
		val.u64 = ctx->ir_base[op1_insn->op2].val.u64 * op2_insn->val.u64;
		op1 = op1_insn->op1;
		op2 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(MUL(MUL, C_I8))
IR_FOLD(MUL(MUL, C_I16))
IR_FOLD(MUL(MUL, C_I32))
IR_FOLD(MUL(MUL, C_I64))
{
	if (IR_IS_CONST_REF(op1_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op2].op)) {
		/* (x * c1) * c2  => x * (c1 * c2) */
		val.i64 = ctx->ir_base[op1_insn->op2].val.i64 * op2_insn->val.i64;
		op1 = op1_insn->op1;
		op2 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(AND(AND, C_U8))
IR_FOLD(AND(AND, C_U16))
IR_FOLD(AND(AND, C_U32))
IR_FOLD(AND(AND, C_U64))
IR_FOLD(AND(AND, C_I8))
IR_FOLD(AND(AND, C_I16))
IR_FOLD(AND(AND, C_I32))
IR_FOLD(AND(AND, C_I64))
IR_FOLD(AND(AND, C_ADDR))
{
	if (IR_IS_CONST_REF(op1_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op2].op)) {
		/* (x & c1) & c2  => x & (c1 & c2) */
		val.u64 = ctx->ir_base[op1_insn->op2].val.u64 & op2_insn->val.u64;
		op1 = op1_insn->op1;
		op2 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(OR(OR, C_U8))
IR_FOLD(OR(OR, C_U16))
IR_FOLD(OR(OR, C_U32))
IR_FOLD(OR(OR, C_U64))
IR_FOLD(OR(OR, C_I8))
IR_FOLD(OR(OR, C_I16))
IR_FOLD(OR(OR, C_I32))
IR_FOLD(OR(OR, C_I64))
IR_FOLD(OR(OR, C_ADDR))
{
	if (IR_IS_CONST_REF(op1_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op2].op)) {
		/* (x | c1) | c2  => x | (c1 | c2) */
		val.u64 = ctx->ir_base[op1_insn->op2].val.u64 | op2_insn->val.u64;
		op1 = op1_insn->op1;
		op2 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(XOR(XOR, C_U8))
IR_FOLD(XOR(XOR, C_U16))
IR_FOLD(XOR(XOR, C_U32))
IR_FOLD(XOR(XOR, C_U64))
IR_FOLD(XOR(XOR, C_I8))
IR_FOLD(XOR(XOR, C_I16))
IR_FOLD(XOR(XOR, C_I32))
IR_FOLD(XOR(XOR, C_I64))
IR_FOLD(XOR(XOR, C_ADDR))
{
	if (IR_IS_CONST_REF(op1_insn->op2) && !IR_IS_SYM_CONST(ctx->ir_base[op1_insn->op2].op)) {
		/* (x ^ c1) ^ c2  => x ^ (c1 ^ c2) */
		val.u64 = ctx->ir_base[op1_insn->op2].val.u64 ^ op2_insn->val.u64;
		op1 = op1_insn->op1;
		op2 = ir_const(ctx, val, IR_OPT_TYPE(opt));
		IR_FOLD_RESTART;
	}
	IR_FOLD_NEXT;
}

IR_FOLD(AND(AND, _))
IR_FOLD(OR(OR, _))
IR_FOLD(MIN(MIN, _))
IR_FOLD(MAX(MAX, _))
{
	if (op1_insn->op1 == op2 || op1_insn->op2 == op2) {
		IR_FOLD_COPY(op2);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(XOR(XOR, _))
{
	if (op1_insn->op1 == op2) {
		IR_FOLD_COPY(op1_insn->op2);
	} else if (op1_insn->op2 == op2) {
		IR_FOLD_COPY(op1_insn->op1);
	}
	IR_FOLD_NEXT;
}

/* ROL/ROR */
IR_FOLD(OR(SHL, SHR))
IR_FOLD(OR(SHR, SHL))
IR_FOLD(ADD(SHL, SHR))
IR_FOLD(ADD(SHR, SHL))
{
	if (op1_insn->op1 == op2_insn->op1) {
		if (IR_IS_CONST_REF(op1_insn->op2) && IR_IS_CONST_REF(op2_insn->op2)) {
			if (ctx->ir_base[op1_insn->op2].val.u64 + ctx->ir_base[op2_insn->op2].val.u64 ==
					ir_type_size[IR_OPT_TYPE(opt)] * 8) {
				/* (x << c) | (x >> (32 - c)) -> ROL(x, c) */
				op1 = op1_insn->op1;
				op2 = op1_insn->op2;
				opt = op1_insn->opt + 3; /* SHL -> ROL, SHR -> ROR */
				IR_FOLD_RESTART;
			}
		} else if (ctx->ir_base[op2_insn->op2].op == IR_SUB
				&& IR_IS_CONST_REF(ctx->ir_base[op2_insn->op2].op1)
				&& ctx->ir_base[op2_insn->op2].op2 == op1_insn->op2
				&& ctx->ir_base[ctx->ir_base[op2_insn->op2].op1].val.u64 == ir_type_size[IR_OPT_TYPE(opt)] * 8) {
			/* (x << y) | (x >> (32 - y)) -> ROL(x, y) */
			op1 = op1_insn->op1;
			op2 = op1_insn->op2;
			opt = op1_insn->opt + 3; /* SHL -> ROL, SHR -> ROR */
			IR_FOLD_RESTART;
		} else if (ctx->ir_base[op1_insn->op2].op == IR_SUB
				&& IR_IS_CONST_REF(ctx->ir_base[op1_insn->op2].op1)
				&& ctx->ir_base[op1_insn->op2].op2 == op2_insn->op2
				&& ctx->ir_base[ctx->ir_base[op1_insn->op2].op1].val.u64 == ir_type_size[IR_OPT_TYPE(opt)] * 8) {
			/* (x << (32 - y)) | (x >> y) -> ROR(x, y) */
			op1 = op2_insn->op1;
			op2 = op2_insn->op2;
			opt = op2_insn->opt + 3; /* SHL -> ROL, SHR -> ROR */
			IR_FOLD_RESTART;
		}
	}
	IR_FOLD_NEXT;
}


/* Swap operands (move lower ref to op2) for better CSE */
IR_FOLD(ADD(_, _))
IR_FOLD(MUL(_, _))
IR_FOLD_NAMED(swap_ops)
{
	if (op1 < op2) {  /* move lower ref to op2 */
		SWAP_REFS(op1, op2);
		IR_FOLD_RESTART;
	}
    IR_FOLD_NEXT;
}

IR_FOLD(ADD_OV(_, _))
IR_FOLD(MUL_OV(_, _))
{
	if (op1 < op2) {  /* move lower ref to op2 */
		SWAP_REFS(op1, op2);
		IR_FOLD_RESTART;
	}
	/* skip CSE ??? */
	IR_FOLD_EMIT;
}

IR_FOLD(SUB(_, _))
{
	if (IR_IS_TYPE_INT(IR_OPT_TYPE(opt)) && op1 == op2) {
		IR_FOLD_CONST_U(0);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(SUB_OV(_, _))
{
	if (op1 == op2) {
		IR_FOLD_CONST_U(0);
	}
	/* skip CSE ??? */
	IR_FOLD_EMIT;
}

/* Binary operations with op1 == op2 */
IR_FOLD(AND(_,_))
IR_FOLD(OR(_,_))
IR_FOLD(MIN(_, _))
IR_FOLD(MAX(_, _))
{
	/* a & a => a */
	if (op1 == op2) {
		IR_FOLD_COPY(op1);
	}
	IR_FOLD_DO_NAMED(swap_ops);
}

IR_FOLD(XOR(_,_))
{
	/* a xor a => 0 */
	if (op1 == op2) {
		IR_FOLD_CONST_U(0);
	}
	IR_FOLD_DO_NAMED(swap_ops);
}

IR_FOLD(EQ(_, _))
IR_FOLD(NE(_, _))
{
	if (op1 != op2) {
		IR_FOLD_DO_NAMED(swap_ops);
	} else if (IR_IS_TYPE_INT(op1_insn->type)) {
		/* a == a => true */
		IR_FOLD_BOOL((opt & IR_OPT_OP_MASK) == IR_EQ);
	}
	IR_FOLD_NEXT;
}

IR_FOLD(LT(_, _))
IR_FOLD(GE(_, _))
IR_FOLD(LE(_, _))
IR_FOLD(GT(_, _))
{
	if (op1 == op2) {
		if (IR_IS_TYPE_INT(op1_insn->type)) {
			/* a >= a => true (two low bits are differ) */
			IR_FOLD_BOOL((opt ^ (opt >> 1)) & 1);
		}
	} else if (op1 < op2) {  /* move lower ref to op2 */
		SWAP_REFS(op1, op2);
		opt ^= 3; /* [U]LT <-> [U]GT, [U]LE <-> [U]GE */
		IR_FOLD_RESTART;
	}
    IR_FOLD_NEXT;
}

IR_FOLD(ULT(_, _))
IR_FOLD(UGE(_, _))
IR_FOLD(ULE(_, _))
IR_FOLD(UGT(_, _))
{
	if (op1 == op2) {
		/* a >= a => true (two low bits are differ) */
		IR_FOLD_BOOL((opt ^ (opt >> 1)) & 1);
	} else if (op1 < op2) {  /* move lower ref to op2 */
		SWAP_REFS(op1, op2);
		opt ^= 3; /* [U]LT <-> [U]GT, [U]LE <-> [U]GE */
	}
	IR_FOLD_NEXT;
}

IR_FOLD(COND(_, _)) // TODO: COND(_, _, _)
{
	if (op2 == op3) {
		IR_FOLD_COPY(op2);
	}
	IR_FOLD_NEXT;
}
