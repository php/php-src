/*
 * IR - Lightweight JIT Compilation Framework
 * (IR Construction API)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 */

#ifndef IR_BUILDER_H
#define IR_BUILDER_H

#ifdef __cplusplus
extern "C" {
#endif

/* _ir_CTX may be redefined by the user */
#define _ir_CTX                           ctx

#define ir_NOP()                          ir_emit0(_ir_CTX, IR_NOP)

#define ir_CONST_BOOL(_val)               ir_const_bool(_ir_CTX, (_val))
#define ir_CONST_U8(_val)                 ir_const_u8(_ir_CTX, (_val))
#define ir_CONST_U16(_val)                ir_const_u16(_ir_CTX, (_val))
#define ir_CONST_U32(_val)                ir_const_u32(_ir_CTX, (_val))
#define ir_CONST_U64(_val)                ir_const_u64(_ir_CTX, (_val))
#define ir_CONST_ADDR(_val)               ir_const_addr(_ir_CTX, (uintptr_t)(_val))
#define ir_CONST_CHAR(_val)               ir_const_char(_ir_CTX, (_val))
#define ir_CONST_I8(_val)                 ir_const_i8(_ir_CTX, (_val))
#define ir_CONST_I16(_val)                ir_const_i16(_ir_CTX, (_val))
#define ir_CONST_I32(_val)                ir_const_i32(_ir_CTX, (_val))
#define ir_CONST_I64(_val)                ir_const_i64(_ir_CTX, (_val))
#define ir_CONST_DOUBLE(_val)             ir_const_double(_ir_CTX, (_val))
#define ir_CONST_FLOAT(_val)              ir_const_float(_ir_CTX, (_val))

#define ir_CMP_OP(_op, _op1, _op2)        ir_fold2(_ir_CTX, IR_OPT((_op), IR_BOOL), (_op1), (_op2))

#define ir_UNARY_OP(_op, _type, _op1)     ir_fold1(_ir_CTX, IR_OPT((_op), (_type)),   (_op1))
#define ir_UNARY_OP_B(_op, _op1)          ir_fold1(_ir_CTX, IR_OPT((_op), IR_BOOL),   (_op1))
#define ir_UNARY_OP_U8(_op, _op1)         ir_fold1(_ir_CTX, IR_OPT((_op), IR_U8),     (_op1))
#define ir_UNARY_OP_U16(_op, _op1)        ir_fold1(_ir_CTX, IR_OPT((_op), IR_U16),    (_op1))
#define ir_UNARY_OP_U32(_op, _op1)        ir_fold1(_ir_CTX, IR_OPT((_op), IR_U32),    (_op1))
#define ir_UNARY_OP_U64(_op, _op1)        ir_fold1(_ir_CTX, IR_OPT((_op), IR_U64),    (_op1))
#define ir_UNARY_OP_A(_op, _op1)          ir_fold1(_ir_CTX, IR_OPT((_op), IR_ADDR),   (_op1))
#define ir_UNARY_OP_C(_op, _op1)          ir_fold1(_ir_CTX, IR_OPT((_op), IR_CHAR),   (_op1))
#define ir_UNARY_OP_I8(_op, _op1)         ir_fold1(_ir_CTX, IR_OPT((_op), IR_I8),     (_op1))
#define ir_UNARY_OP_I16(_op, _op1)        ir_fold1(_ir_CTX, IR_OPT((_op), IR_I16),    (_op1))
#define ir_UNARY_OP_I32(_op, _op1)        ir_fold1(_ir_CTX, IR_OPT((_op), IR_I32),    (_op1))
#define ir_UNARY_OP_I64(_op, _op1)        ir_fold1(_ir_CTX, IR_OPT((_op), IR_I64),    (_op1))
#define ir_UNARY_OP_D(_op, _op1)          ir_fold1(_ir_CTX, IR_OPT((_op), IR_DOUBLE), (_op1))
#define ir_UNARY_OP_F(_op, _op1)          ir_fold1(_ir_CTX, IR_OPT((_op), IR_FLOAT),  (_op1))

#define ir_BINARY_OP(_op, _t, _op1, _op2) ir_fold2(_ir_CTX, IR_OPT((_op), (_t)),      (_op1), (_op2))
#define ir_BINARY_OP_B(_op, _op1, _op2)   ir_fold2(_ir_CTX, IR_OPT((_op), IR_BOOL),   (_op1), (_op2))
#define ir_BINARY_OP_U8(_op, _op1, _op2)  ir_fold2(_ir_CTX, IR_OPT((_op), IR_U8),     (_op1), (_op2))
#define ir_BINARY_OP_U16(_op, _op1, _op2) ir_fold2(_ir_CTX, IR_OPT((_op), IR_U16),    (_op1), (_op2))
#define ir_BINARY_OP_U32(_op, _op1, _op2) ir_fold2(_ir_CTX, IR_OPT((_op), IR_U32),    (_op1), (_op2))
#define ir_BINARY_OP_U64(_op, _op1, _op2) ir_fold2(_ir_CTX, IR_OPT((_op), IR_U64),    (_op1), (_op2))
#define ir_BINARY_OP_A(_op, _op1, _op2)   ir_fold2(_ir_CTX, IR_OPT((_op), IR_ADDR),   (_op1), (_op2))
#define ir_BINARY_OP_C(_op, _op1, _op2)   ir_fold2(_ir_CTX, IR_OPT((_op), IR_CHAR),   (_op1), (_op2))
#define ir_BINARY_OP_I8(_op, _op1, _op2)  ir_fold2(_ir_CTX, IR_OPT((_op), IR_I8),     (_op1), (_op2))
#define ir_BINARY_OP_I16(_op, _op1, _op2) ir_fold2(_ir_CTX, IR_OPT((_op), IR_I16),    (_op1), (_op2))
#define ir_BINARY_OP_I32(_op, _op1, _op2) ir_fold2(_ir_CTX, IR_OPT((_op), IR_I32),    (_op1), (_op2))
#define ir_BINARY_OP_I64(_op, _op1, _op2) ir_fold2(_ir_CTX, IR_OPT((_op), IR_I64),    (_op1), (_op2))
#define ir_BINARY_OP_D(_op, _op1, _op2)   ir_fold2(_ir_CTX, IR_OPT((_op), IR_DOUBLE), (_op1), (_op2))
#define ir_BINARY_OP_F(_op, _op1, _op2)   ir_fold2(_ir_CTX, IR_OPT((_op), IR_FLOAT),  (_op1), (_op2))

#define ir_EQ(_op1, _op2)                 ir_CMP_OP(IR_EQ, (_op1), (_op2))
#define ir_NE(_op1, _op2)                 ir_CMP_OP(IR_NE, (_op1), (_op2))

#define ir_LT(_op1, _op2)                 ir_CMP_OP(IR_LT, (_op1), (_op2))
#define ir_GE(_op1, _op2)                 ir_CMP_OP(IR_GE, (_op1), (_op2))
#define ir_LE(_op1, _op2)                 ir_CMP_OP(IR_LE, (_op1), (_op2))
#define ir_GT(_op1, _op2)                 ir_CMP_OP(IR_GT, (_op1), (_op2))

#define ir_ULT(_op1, _op2)                ir_CMP_OP(IR_ULT, (_op1), (_op2))
#define ir_UGE(_op1, _op2)                ir_CMP_OP(IR_UGE, (_op1), (_op2))
#define ir_ULE(_op1, _op2)                ir_CMP_OP(IR_ULE, (_op1), (_op2))
#define ir_UGT(_op1, _op2)                ir_CMP_OP(IR_UGT, (_op1), (_op2))

#define ir_ADD(_type, _op1, _op2)         ir_BINARY_OP(IR_ADD, (_type), (_op1), (_op2))
#define ir_ADD_U8(_op1, _op2)             ir_BINARY_OP_U8(IR_ADD, (_op1), (_op2))
#define ir_ADD_U16(_op1, _op2)            ir_BINARY_OP_U16(IR_ADD, (_op1), (_op2))
#define ir_ADD_U32(_op1, _op2)            ir_BINARY_OP_U32(IR_ADD, (_op1), (_op2))
#define ir_ADD_U64(_op1, _op2)            ir_BINARY_OP_U64(IR_ADD, (_op1), (_op2))
#define ir_ADD_A(_op1, _op2)              ir_BINARY_OP_A(IR_ADD, (_op1), (_op2))
#define ir_ADD_C(_op1, _op2)              ir_BINARY_OP_C(IR_ADD, (_op1), (_op2))
#define ir_ADD_I8(_op1, _op2)             ir_BINARY_OP_I8(IR_ADD, (_op1), (_op2))
#define ir_ADD_I16(_op1, _op2)            ir_BINARY_OP_I16(IR_ADD, (_op1), (_op2))
#define ir_ADD_I32(_op1, _op2)            ir_BINARY_OP_I32(IR_ADD, (_op1), (_op2))
#define ir_ADD_I64(_op1, _op2)            ir_BINARY_OP_I64(IR_ADD, (_op1), (_op2))
#define ir_ADD_D(_op1, _op2)              ir_BINARY_OP_D(IR_ADD, (_op1), (_op2))
#define ir_ADD_F(_op1, _op2)              ir_BINARY_OP_F(IR_ADD, (_op1), (_op2))

#define ir_SUB(_type, _op1, _op2)         ir_BINARY_OP(IR_SUB, (_type), (_op1), (_op2))
#define ir_SUB_U8(_op1, _op2)             ir_BINARY_OP_U8(IR_SUB, (_op1), (_op2))
#define ir_SUB_U16(_op1, _op2)            ir_BINARY_OP_U16(IR_SUB, (_op1), (_op2))
#define ir_SUB_U32(_op1, _op2)            ir_BINARY_OP_U32(IR_SUB, (_op1), (_op2))
#define ir_SUB_U64(_op1, _op2)            ir_BINARY_OP_U64(IR_SUB, (_op1), (_op2))
#define ir_SUB_A(_op1, _op2)              ir_BINARY_OP_A(IR_SUB, (_op1), (_op2))
#define ir_SUB_C(_op1, _op2)              ir_BINARY_OP_C(IR_SUB, (_op1), (_op2))
#define ir_SUB_I8(_op1, _op2)             ir_BINARY_OP_I8(IR_SUB, (_op1), (_op2))
#define ir_SUB_I16(_op1, _op2)            ir_BINARY_OP_I16(IR_SUB, (_op1), (_op2))
#define ir_SUB_I32(_op1, _op2)            ir_BINARY_OP_I32(IR_SUB, (_op1), (_op2))
#define ir_SUB_I64(_op1, _op2)            ir_BINARY_OP_I64(IR_SUB, (_op1), (_op2))
#define ir_SUB_D(_op1, _op2)              ir_BINARY_OP_D(IR_SUB, (_op1), (_op2))
#define ir_SUB_F(_op1, _op2)              ir_BINARY_OP_F(IR_SUB, (_op1), (_op2))

#define ir_MUL(_type, _op1, _op2)         ir_BINARY_OP(IR_MUL, (_type), (_op1), (_op2))
#define ir_MUL_U8(_op1, _op2)             ir_BINARY_OP_U8(IR_MUL, (_op1), (_op2))
#define ir_MUL_U16(_op1, _op2)            ir_BINARY_OP_U16(IR_MUL, (_op1), (_op2))
#define ir_MUL_U32(_op1, _op2)            ir_BINARY_OP_U32(IR_MUL, (_op1), (_op2))
#define ir_MUL_U64(_op1, _op2)            ir_BINARY_OP_U64(IR_MUL, (_op1), (_op2))
#define ir_MUL_A(_op1, _op2)              ir_BINARY_OP_A(IR_MUL, (_op1), (_op2))
#define ir_MUL_C(_op1, _op2)              ir_BINARY_OP_C(IR_MUL, (_op1), (_op2))
#define ir_NUL_I8(_op1, _op2)             ir_BINARY_OP_I8(IR_MUL, (_op1), (_op2))
#define ir_MUL_I16(_op1, _op2)            ir_BINARY_OP_I16(IR_MUL, (_op1), (_op2))
#define ir_MUL_I32(_op1, _op2)            ir_BINARY_OP_I32(IR_MUL, (_op1), (_op2))
#define ir_MUL_I64(_op1, _op2)            ir_BINARY_OP_I64(IR_MUL, (_op1), (_op2))
#define ir_MUL_D(_op1, _op2)              ir_BINARY_OP_D(IR_MUL, (_op1), (_op2))
#define ir_MUL_F(_op1, _op2)              ir_BINARY_OP_F(IR_MUL, (_op1), (_op2))

#define ir_DIV(_type, _op1, _op2)         ir_BINARY_OP(IR_DIV, (_type), (_op1), (_op2))
#define ir_DIV_U8(_op1, _op2)             ir_BINARY_OP_U8(IR_DIV, (_op1), (_op2))
#define ir_DIV_U16(_op1, _op2)            ir_BINARY_OP_U16(IR_DIV, (_op1), (_op2))
#define ir_DIV_U32(_op1, _op2)            ir_BINARY_OP_U32(IR_DIV, (_op1), (_op2))
#define ir_DIV_U64(_op1, _op2)            ir_BINARY_OP_U64(IR_DIV, (_op1), (_op2))
#define ir_DIV_A(_op1, _op2)              ir_BINARY_OP_A(IR_DIV, (_op1), (_op2))
#define ir_DIV_C(_op1, _op2)              ir_BINARY_OP_C(IR_DIV, (_op1), (_op2))
#define ir_DIV_I8(_op1, _op2)             ir_BINARY_OP_I8(IR_DIV, (_op1), (_op2))
#define ir_DIV_I16(_op1, _op2)            ir_BINARY_OP_I16(IR_DIV, (_op1), (_op2))
#define ir_DIV_I32(_op1, _op2)            ir_BINARY_OP_I32(IR_DIV, (_op1), (_op2))
#define ir_DIV_I64(_op1, _op2)            ir_BINARY_OP_I64(IR_DIV, (_op1), (_op2))
#define ir_DIV_D(_op1, _op2)              ir_BINARY_OP_D(IR_DIV, (_op1), (_op2))
#define ir_DIV_F(_op1, _op2)              ir_BINARY_OP_F(IR_DIV, (_op1), (_op2))

#define ir_MOD(_type, _op1, _op2)         ir_BINARY_OP(IR_MOD, (_type), (_op1), (_op2))
#define ir_MOD_U8(_op1, _op2)             ir_BINARY_OP_U8(IR_MOD, (_op1), (_op2))
#define ir_MOD_U16(_op1, _op2)            ir_BINARY_OP_U16(IR_MOD, (_op1), (_op2))
#define ir_MOD_U32(_op1, _op2)            ir_BINARY_OP_U32(IR_MOD, (_op1), (_op2))
#define ir_MOD_U64(_op1, _op2)            ir_BINARY_OP_U64(IR_MOD, (_op1), (_op2))
#define ir_MOD_A(_op1, _op2)              ir_BINARY_OP_A(IR_MOD, (_op1), (_op2))
#define ir_MOD_C(_op1, _op2)              ir_BINARY_OP_C(IR_MOD, (_op1), (_op2))
#define ir_MOD_I8(_op1, _op2)             ir_BINARY_OP_I8(IR_MOD, (_op1), (_op2))
#define ir_MOD_I16(_op1, _op2)            ir_BINARY_OP_I16(IR_MOD, (_op1), (_op2))
#define ir_MOD_I32(_op1, _op2)            ir_BINARY_OP_I32(IR_MOD, (_op1), (_op2))
#define ir_MOD_I64(_op1, _op2)            ir_BINARY_OP_I64(IR_MOD, (_op1), (_op2))

#define ir_NEG(_type, _op1)               ir_UNARY_OP(IR_NEG, (_type), (_op1))
#define ir_NEG_C(_op1)                    ir_UNARY_OP_C(IR_NEG, (_op1))
#define ir_NEG_I8(_op1)                   ir_UNARY_OP_I8(IR_NEG, (_op1))
#define ir_NEG_I16(_op1)                  ir_UNARY_OP_I16(IR_NEG, (_op1))
#define ir_NEG_I32(_op1)                  ir_UNARY_OP_I32(IR_NEG, (_op1))
#define ir_NEG_I64(_op1)                  ir_UNARY_OP_I64(IR_NEG, (_op1))
#define ir_NEG_D(_op1)                    ir_UNARY_OP_D(IR_NEG, (_op1))
#define ir_NEG_F(_op1)                    ir_UNARY_OP_F(IR_NEG, (_op1))

#define ir_ABS(_type, _op1)               ir_UNARY_OP(IR_ABS, (_type), (_op1))
#define ir_ABS_C(_op1)                    ir_UNARY_OP_C(IR_ABS, (_op1))
#define ir_ABS_I8(_op1)                   ir_UNARY_OP_I8(IR_ABS, (_op1))
#define ir_ABS_I16(_op1)                  ir_UNARY_OP_I16(IR_ABS, (_op1))
#define ir_ABS_I32(_op1)                  ir_UNARY_OP_I32(IR_ABS, (_op1))
#define ir_ABS_I64(_op1)                  ir_UNARY_OP_I64(IR_ABS, (_op1))
#define ir_ABS_D(_op1)                    ir_UNARY_OP_D(IR_ABS, (_op1))
#define ir_ABS_F(_op1)                    ir_UNARY_OP_F(IR_ABS, (_op1))

#define ir_SEXT(_type, _op1)              ir_UNARY_OP(IR_SEXT, (_type), (_op1))
#define ir_SEXT_U8(_op1)                  ir_UNARY_OP_U8(IR_SEXT, (_op1))
#define ir_SEXT_U16(_op1)                 ir_UNARY_OP_U16(IR_SEXT, (_op1))
#define ir_SEXT_U32(_op1)                 ir_UNARY_OP_U32(IR_SEXT, (_op1))
#define ir_SEXT_U64(_op1)                 ir_UNARY_OP_U64(IR_SEXT, (_op1))
#define ir_SEXT_A(_op1)                   ir_UNARY_OP_A(IR_SEXT, (_op1))
#define ir_SEXT_C(_op1)                   ir_UNARY_OP_C(IR_SEXT, (_op1))
#define ir_SEXT_I8(_op1)                  ir_UNARY_OP_I8(IR_SEXT, (_op1))
#define ir_SEXT_I16(_op1)                 ir_UNARY_OP_I16(IR_SEXT, (_op1))
#define ir_SEXT_I32(_op1)                 ir_UNARY_OP_I32(IR_SEXT, (_op1))
#define ir_SEXT_I64(_op1)                 ir_UNARY_OP_I64(IR_SEXT, (_op1))

#define ir_ZEXT(_type, _op1)              ir_UNARY_OP(IR_ZEXT, (_type), (_op1))
#define ir_ZEXT_U8(_op1)                  ir_UNARY_OP_U8(IR_ZEXT, (_op1))
#define ir_ZEXT_U16(_op1)                 ir_UNARY_OP_U16(IR_ZEXT, (_op1))
#define ir_ZEXT_U32(_op1)                 ir_UNARY_OP_U32(IR_ZEXT, (_op1))
#define ir_ZEXT_U64(_op1)                 ir_UNARY_OP_U64(IR_ZEXT, (_op1))
#define ir_ZEXT_A(_op1)                   ir_UNARY_OP_A(IR_ZEXT, (_op1))
#define ir_ZEXT_C(_op1)                   ir_UNARY_OP_C(IR_ZEXT, (_op1))
#define ir_ZEXT_I8(_op1)                  ir_UNARY_OP_I8(IR_ZEXT, (_op1))
#define ir_ZEXT_I16(_op1)                 ir_UNARY_OP_I16(IR_ZEXT, (_op1))
#define ir_ZEXT_I32(_op1)                 ir_UNARY_OP_I32(IR_ZEXT, (_op1))
#define ir_ZEXT_I64(_op1)                 ir_UNARY_OP_I64(IR_ZEXT, (_op1))

#define ir_TRUNC(_type, _op1)             ir_UNARY_OP(IR_TRUNC, (_type), (_op1))
#define ir_TRUNC_U8(_op1)                 ir_UNARY_OP_U8(IR_TRUNC, (_op1))
#define ir_TRUNC_U16(_op1)                ir_UNARY_OP_U16(IR_TRUNC, (_op1))
#define ir_TRUNC_U32(_op1)                ir_UNARY_OP_U32(IR_TRUNC, (_op1))
#define ir_TRUNC_U64(_op1)                ir_UNARY_OP_U64(IR_TRUNC, (_op1))
#define ir_TRUNC_A(_op1)                  ir_UNARY_OP_A(IR_TRUNC, (_op1))
#define ir_TRUNC_C(_op1)                  ir_UNARY_OP_C(IR_TRUNC, (_op1))
#define ir_TRUNC_I8(_op1)                 ir_UNARY_OP_I8(IR_TRUNC, (_op1))
#define ir_TRUNC_I16(_op1)                ir_UNARY_OP_I16(IR_TRUNC, (_op1))
#define ir_TRUNC_I32(_op1)                ir_UNARY_OP_I32(IR_TRUNC, (_op1))
#define ir_TRUNC_I64(_op1)                ir_UNARY_OP_I64(IR_TRUNC, (_op1))

#define ir_BITCAST(_type, _op1)           ir_UNARY_OP(IR_BITCAST, (_type), (_op1))
#define ir_BITCAST_U8(_op1)               ir_UNARY_OP_U8(IR_BITCAST, (_op1))
#define ir_BITCAST_U16(_op1)              ir_UNARY_OP_U16(IR_BITCAST, (_op1))
#define ir_BITCAST_U32(_op1)              ir_UNARY_OP_U32(IR_BITCAST, (_op1))
#define ir_BITCAST_U64(_op1)              ir_UNARY_OP_U64(IR_BITCAST, (_op1))
#define ir_BITCAST_A(_op1)                ir_UNARY_OP_A(IR_BITCAST, (_op1))
#define ir_BITCAST_C(_op1)                ir_UNARY_OP_C(IR_BITCAST, (_op1))
#define ir_BITCAST_I8(_op1)               ir_UNARY_OP_I8(IR_BITCAST, (_op1))
#define ir_BITCAST_I16(_op1)              ir_UNARY_OP_I16(IR_BITCAST, (_op1))
#define ir_BITCAST_I32(_op1)              ir_UNARY_OP_I32(IR_BITCAST, (_op1))
#define ir_BITCAST_I64(_op1)              ir_UNARY_OP_I64(IR_BITCAST, (_op1))
#define ir_BITCAST_D(_op1)                ir_UNARY_OP_D(IR_BITCAST, (_op1))
#define ir_BITCAST_F(_op1)                ir_UNARY_OP_F(IR_BITCAST, (_op1))

#define ir_INT2FP(_type, _op1)            ir_UNARY_OP(IR_INT2FP, (_type), (_op1))
#define ir_INT2D(_op1)                    ir_UNARY_OP_D(IR_INT2FP, (_op1))
#define ir_INT2F(_op1)                    ir_UNARY_OP_F(IR_INT2FP, (_op1))

#define ir_FP2INT(_type, _op1)            ir_UNARY_OP(IR_FP2INT, (_type), (_op1))
#define ir_FP2U8(_op1)                    ir_UNARY_OP_U8(IR_FP2INT, (_op1))
#define ir_FP2U16(_op1)                   ir_UNARY_OP_U16(IR_FP2INT, (_op1))
#define ir_FP2U32(_op1)                   ir_UNARY_OP_U32(IR_FP2INT, (_op1))
#define ir_FP2U64(_op1)                   ir_UNARY_OP_U64(IR_FP2INT, (_op1))
#define ir_FP2I8(_op1)                    ir_UNARY_OP_I8(IR_FP2INT, (_op1))
#define ir_FP2I16(_op1)                   ir_UNARY_OP_I16(IR_FP2INT, (_op1))
#define ir_FP2I32(_op1)                   ir_UNARY_OP_I32(IR_FP2INT, (_op1))
#define ir_FP2I64(_op1)                   ir_UNARY_OP_I64(IR_FP2INT, (_op1))

#define ir_FP2FP(_type, _op1)             ir_UNARY_OP(IR_FP2FP, (_type), (_op1))
#define ir_F2D(_op1)                      ir_UNARY_OP_D(IR_FP2FP, (_op1))
#define ir_D2F(_op1)                      ir_UNARY_OP_F(IR_FP2FP, (_op1))

#define ir_ADD_OV(_type, _op1, _op2)      ir_BINARY_OP(IR_ADD_OV, (_type), (_op1), (_op2))
#define ir_ADD_OV_U8(_op1, _op2)          ir_BINARY_OP_U8(IR_ADD_OV, (_op1), (_op2))
#define ir_ADD_OV_U16(_op1, _op2)         ir_BINARY_OP_U16(IR_ADD_OV, (_op1), (_op2))
#define ir_ADD_OV_U32(_op1, _op2)         ir_BINARY_OP_U32(IR_ADD_OV, (_op1), (_op2))
#define ir_ADD_OV_U64(_op1, _op2)         ir_BINARY_OP_U64(IR_ADD_OV, (_op1), (_op2))
#define ir_ADD_OV_A(_op1, _op2)           ir_BINARY_OP_A(IR_ADD_OV, (_op1), (_op2))
#define ir_ADD_OV_C(_op1, _op2)           ir_BINARY_OP_C(IR_ADD_OV, (_op1), (_op2))
#define ir_ADD_OV_I8(_op1, _op2)          ir_BINARY_OP_I8(IR_ADD_OV, (_op1), (_op2))
#define ir_ADD_OV_I16(_op1, _op2)         ir_BINARY_OP_I16(IR_ADD_OV, (_op1), (_op2))
#define ir_ADD_OV_I32(_op1, _op2)         ir_BINARY_OP_I32(IR_ADD_OV, (_op1), (_op2))
#define ir_ADD_OV_I64(_op1, _op2)         ir_BINARY_OP_I64(IR_ADD_OV, (_op1), (_op2))

#define ir_SUB_OV(_type, _op1, _op2)      ir_BINARY_OP(IR_SUB_OV, (_type), (_op1), (_op2))
#define ir_SUB_OV_U8(_op1, _op2)          ir_BINARY_OP_U8(IR_SUB_OV, (_op1), (_op2))
#define ir_SUB_OV_U16(_op1, _op2)         ir_BINARY_OP_U16(IR_SUB_OV, (_op1), (_op2))
#define ir_SUB_OV_U32(_op1, _op2)         ir_BINARY_OP_U32(IR_SUB_OV, (_op1), (_op2))
#define ir_SUB_OV_U64(_op1, _op2)         ir_BINARY_OP_U64(IR_SUB_OV, (_op1), (_op2))
#define ir_SUB_OV_A(_op1, _op2)           ir_BINARY_OP_A(IR_SUB_OV, (_op1), (_op2))
#define ir_SUB_OV_C(_op1, _op2)           ir_BINARY_OP_C(IR_SUB_OV, (_op1), (_op2))
#define ir_SUB_OV_I8(_op1, _op2)          ir_BINARY_OP_I8(IR_SUB_OV, (_op1), (_op2))
#define ir_SUB_OV_I16(_op1, _op2)         ir_BINARY_OP_I16(IR_SUB_OV, (_op1), (_op2))
#define ir_SUB_OV_I32(_op1, _op2)         ir_BINARY_OP_I32(IR_SUB_OV, (_op1), (_op2))
#define ir_SUB_OV_I64(_op1, _op2)         ir_BINARY_OP_I64(IR_SUB_OV, (_op1), (_op2))

#define ir_MUL_OV(_type, _op1, _op2)      ir_BINARY_OP(IR_MUL_OV, (_type), (_op1), (_op2))
#define ir_MUL_OV_U8(_op1, _op2)          ir_BINARY_OP_U8(IR_MUL_OV, (_op1), (_op2))
#define ir_MUL_OV_U16(_op1, _op2)         ir_BINARY_OP_U16(IR_MUL_OV, (_op1), (_op2))
#define ir_MUL_OV_U32(_op1, _op2)         ir_BINARY_OP_U32(IR_MUL_OV, (_op1), (_op2))
#define ir_MUL_OV_U64(_op1, _op2)         ir_BINARY_OP_U64(IR_MUL_OV, (_op1), (_op2))
#define ir_MUL_OV_A(_op1, _op2)           ir_BINARY_OP_A(IR_MUL_OV, (_op1), (_op2))
#define ir_MUL_OV_C(_op1, _op2)           ir_BINARY_OP_C(IR_MUL_OV, (_op1), (_op2))
#define ir_MUL_OV_I8(_op1, _op2)          ir_BINARY_OP_I8(IR_MUL_OV, (_op1), (_op2))
#define ir_MUL_OV_I16(_op1, _op2)         ir_BINARY_OP_I16(IR_MUL_OV, (_op1), (_op2))
#define ir_MUL_OV_I32(_op1, _op2)         ir_BINARY_OP_I32(IR_MUL_OV, (_op1), (_op2))
#define ir_MUL_OV_I64(_op1, _op2)         ir_BINARY_OP_I64(IR_MUL_OV, (_op1), (_op2))

#define ir_OVERFLOW(_op1)                 ir_fold1(_ir_CTX, IR_OPT(IR_OVERFLOW, IR_BOOL), (_op1))

#define ir_NOT(_type, _op1)               ir_UNARY_OP(IR_NOT, (_type), (_op1))
#define ir_NOT_B(_op1)                    ir_UNARY_OP_B(IR_NOT, (_op1))
#define ir_NOT_U8(_op1)                   ir_UNARY_OP_U8(IR_NOT, (_op1))
#define ir_NOT_U16(_op1)                  ir_UNARY_OP_U16(IR_NOT, (_op1))
#define ir_NOT_U32(_op1)                  ir_UNARY_OP_U32(IR_NOT, (_op1))
#define ir_NOT_U64(_op1)                  ir_UNARY_OP_U64(IR_NOT, (_op1))
#define ir_NOT_A(_op1)                    ir_UNARY_OP_A(IR_NOT, (_op1))
#define ir_NOT_C(_op1)                    ir_UNARY_OP_C(IR_NOT, (_op1))
#define ir_NOT_I8(_op1)                   ir_UNARY_OP_I8(IR_NOT, (_op1))
#define ir_NOT_I16(_op1)                  ir_UNARY_OP_I16(IR_NOT, (_op1))
#define ir_NOT_I32(_op1)                  ir_UNARY_OP_I32(IR_NOT, (_op1))
#define ir_NOT_I64(_op1)                  ir_UNARY_OP_I64(IR_NOT, (_op1))

#define ir_OR(_type, _op1, _op2)          ir_BINARY_OP(IR_OR, (_type), (_op1), (_op2))
#define ir_OR_B(_op1, _op2)               ir_BINARY_OP_B(IR_OR, (_op1), (_op2))
#define ir_OR_U8(_op1, _op2)              ir_BINARY_OP_U8(IR_OR, (_op1), (_op2))
#define ir_OR_U16(_op1, _op2)             ir_BINARY_OP_U16(IR_OR, (_op1), (_op2))
#define ir_OR_U32(_op1, _op2)             ir_BINARY_OP_U32(IR_OR, (_op1), (_op2))
#define ir_OR_U64(_op1, _op2)             ir_BINARY_OP_U64(IR_OR, (_op1), (_op2))
#define ir_OR_A(_op1, _op2)               ir_BINARY_OP_A(IR_OR, (_op1), (_op2))
#define ir_OR_C(_op1, _op2)               ir_BINARY_OP_C(IR_OR, (_op1), (_op2))
#define ir_OR_I8(_op1, _op2)              ir_BINARY_OP_I8(IR_OR, (_op1), (_op2))
#define ir_OR_I16(_op1, _op2)             ir_BINARY_OP_I16(IR_OR, (_op1), (_op2))
#define ir_OR_I32(_op1, _op2)             ir_BINARY_OP_I32(IR_OR, (_op1), (_op2))
#define ir_OR_I64(_op1, _op2)             ir_BINARY_OP_I64(IR_OR, (_op1), (_op2))

#define ir_AND(_type, _op1, _op2)         ir_BINARY_OP(IR_AND, (_type), (_op1), (_op2))
#define ir_AND_B(_op1, _op2)              ir_BINARY_OP_B(IR_AND, (_op1), (_op2))
#define ir_AND_U8(_op1, _op2)             ir_BINARY_OP_U8(IR_AND, (_op1), (_op2))
#define ir_AND_U16(_op1, _op2)            ir_BINARY_OP_U16(IR_AND, (_op1), (_op2))
#define ir_AND_U32(_op1, _op2)            ir_BINARY_OP_U32(IR_AND, (_op1), (_op2))
#define ir_AND_U64(_op1, _op2)            ir_BINARY_OP_U64(IR_AND, (_op1), (_op2))
#define ir_AND_A(_op1, _op2)              ir_BINARY_OP_A(IR_AND, (_op1), (_op2))
#define ir_AND_C(_op1, _op2)              ir_BINARY_OP_C(IR_AND, (_op1), (_op2))
#define ir_AND_I8(_op1, _op2)             ir_BINARY_OP_I8(IR_AND, (_op1), (_op2))
#define ir_AND_I16(_op1, _op2)            ir_BINARY_OP_I16(IR_AND, (_op1), (_op2))
#define ir_AND_I32(_op1, _op2)            ir_BINARY_OP_I32(IR_AND, (_op1), (_op2))
#define ir_AND_I64(_op1, _op2)            ir_BINARY_OP_I64(IR_AND, (_op1), (_op2))

#define ir_XOR(_type, _op1, _op2)         ir_BINARY_OP(IR_XOR, (_type), (_op1), (_op2))
#define ir_XOR_B(_op1, _op2)              ir_BINARY_OP_B(IR_XOR, (_op1), (_op2))
#define ir_XOR_U8(_op1, _op2)             ir_BINARY_OP_U8(IR_XOR, (_op1), (_op2))
#define ir_XOR_U16(_op1, _op2)            ir_BINARY_OP_U16(IR_XOR, (_op1), (_op2))
#define ir_XOR_U32(_op1, _op2)            ir_BINARY_OP_U32(IR_XOR, (_op1), (_op2))
#define ir_XOR_U64(_op1, _op2)            ir_BINARY_OP_U64(IR_XOR, (_op1), (_op2))
#define ir_XOR_A(_op1, _op2)              ir_BINARY_OP_A(IR_XOR, (_op1), (_op2))
#define ir_XOR_C(_op1, _op2)              ir_BINARY_OP_C(IR_XOR, (_op1), (_op2))
#define ir_XOR_I8(_op1, _op2)             ir_BINARY_OP_I8(IR_XOR, (_op1), (_op2))
#define ir_XOR_I16(_op1, _op2)            ir_BINARY_OP_I16(IR_XOR, (_op1), (_op2))
#define ir_XOR_I32(_op1, _op2)            ir_BINARY_OP_I32(IR_XOR, (_op1), (_op2))
#define ir_XOR_I64(_op1, _op2)            ir_BINARY_OP_I64(IR_XOR, (_op1), (_op2))

#define ir_SHL(_type, _op1, _op2)         ir_BINARY_OP(IR_SHL, (_type), (_op1), (_op2))
#define ir_SHL_U8(_op1, _op2)             ir_BINARY_OP_U8(IR_SHL, (_op1), (_op2))
#define ir_SHL_U16(_op1, _op2)            ir_BINARY_OP_U16(IR_SHL, (_op1), (_op2))
#define ir_SHL_U32(_op1, _op2)            ir_BINARY_OP_U32(IR_SHL, (_op1), (_op2))
#define ir_SHL_U64(_op1, _op2)            ir_BINARY_OP_U64(IR_SHL, (_op1), (_op2))
#define ir_SHL_A(_op1, _op2)              ir_BINARY_OP_A(IR_SHL, (_op1), (_op2))
#define ir_SHL_C(_op1, _op2)              ir_BINARY_OP_C(IR_SHL, (_op1), (_op2))
#define ir_SHL_I8(_op1, _op2)             ir_BINARY_OP_I8(IR_SHL, (_op1), (_op2))
#define ir_SHL_I16(_op1, _op2)            ir_BINARY_OP_I16(IR_SHL, (_op1), (_op2))
#define ir_SHL_I32(_op1, _op2)            ir_BINARY_OP_I32(IR_SHL, (_op1), (_op2))
#define ir_SHL_I64(_op1, _op2)            ir_BINARY_OP_I64(IR_SHL, (_op1), (_op2))

#define ir_SHR(_type, _op1, _op2)         ir_BINARY_OP(IR_SHR, (_type), (_op1), (_op2))
#define ir_SHR_U8(_op1, _op2)             ir_BINARY_OP_U8(IR_SHR, (_op1), (_op2))
#define ir_SHR_U16(_op1, _op2)            ir_BINARY_OP_U16(IR_SHR, (_op1), (_op2))
#define ir_SHR_U32(_op1, _op2)            ir_BINARY_OP_U32(IR_SHR, (_op1), (_op2))
#define ir_SHR_U64(_op1, _op2)            ir_BINARY_OP_U64(IR_SHR, (_op1), (_op2))
#define ir_SHR_A(_op1, _op2)              ir_BINARY_OP_A(IR_SHR, (_op1), (_op2))
#define ir_SHR_C(_op1, _op2)              ir_BINARY_OP_C(IR_SHR, (_op1), (_op2))
#define ir_SHR_I8(_op1, _op2)             ir_BINARY_OP_I8(IR_SHR, (_op1), (_op2))
#define ir_SHR_I16(_op1, _op2)            ir_BINARY_OP_I16(IR_SHR, (_op1), (_op2))
#define ir_SHR_I32(_op1, _op2)            ir_BINARY_OP_I32(IR_SHR, (_op1), (_op2))
#define ir_SHR_I64(_op1, _op2)            ir_BINARY_OP_I64(IR_SHR, (_op1), (_op2))

#define ir_SAR(_type, _op1, _op2)         ir_BINARY_OP(IR_SAR, (_type), (_op1), (_op2))
#define ir_SAR_U8(_op1, _op2)             ir_BINARY_OP_U8(IR_SAR, (_op1), (_op2))
#define ir_SAR_U16(_op1, _op2)            ir_BINARY_OP_U16(IR_SAR, (_op1), (_op2))
#define ir_SAR_U32(_op1, _op2)            ir_BINARY_OP_U32(IR_SAR, (_op1), (_op2))
#define ir_SAR_U64(_op1, _op2)            ir_BINARY_OP_U64(IR_SAR, (_op1), (_op2))
#define ir_SAR_A(_op1, _op2)              ir_BINARY_OP_A(IR_SAR, (_op1), (_op2))
#define ir_SAR_C(_op1, _op2)              ir_BINARY_OP_C(IR_SAR, (_op1), (_op2))
#define ir_SAR_I8(_op1, _op2)             ir_BINARY_OP_I8(IR_SAR, (_op1), (_op2))
#define ir_SAR_I16(_op1, _op2)            ir_BINARY_OP_I16(IR_SAR, (_op1), (_op2))
#define ir_SAR_I32(_op1, _op2)            ir_BINARY_OP_I32(IR_SAR, (_op1), (_op2))
#define ir_SAR_I64(_op1, _op2)            ir_BINARY_OP_I64(IR_SAR, (_op1), (_op2))

#define ir_ROL(_type, _op1, _op2)         ir_BINARY_OP(IR_ROL, (_type), (_op1), (_op2))
#define ir_ROL_U8(_op1, _op2)             ir_BINARY_OP_U8(IR_ROL, (_op1), (_op2))
#define ir_ROL_U16(_op1, _op2)            ir_BINARY_OP_U16(IR_ROL, (_op1), (_op2))
#define ir_ROL_U32(_op1, _op2)            ir_BINARY_OP_U32(IR_ROL, (_op1), (_op2))
#define ir_ROL_U64(_op1, _op2)            ir_BINARY_OP_U64(IR_ROL, (_op1), (_op2))
#define ir_ROL_A(_op1, _op2)              ir_BINARY_OP_A(IR_ROL, (_op1), (_op2))
#define ir_ROL_C(_op1, _op2)              ir_BINARY_OP_C(IR_ROL, (_op1), (_op2))
#define ir_ROL_I8(_op1, _op2)             ir_BINARY_OP_I8(IR_ROL, (_op1), (_op2))
#define ir_ROL_I16(_op1, _op2)            ir_BINARY_OP_I16(IR_ROL, (_op1), (_op2))
#define ir_ROL_I32(_op1, _op2)            ir_BINARY_OP_I32(IR_ROL, (_op1), (_op2))
#define ir_ROL_I64(_op1, _op2)            ir_BINARY_OP_I64(IR_ROL, (_op1), (_op2))

#define ir_ROR(_type, _op1, _op2)         ir_BINARY_OP(IR_ROR, (_type), (_op1), (_op2))
#define ir_ROR_U8(_op1, _op2)             ir_BINARY_OP_U8(IR_ROR, (_op1), (_op2))
#define ir_ROR_U16(_op1, _op2)            ir_BINARY_OP_U16(IR_ROR, (_op1), (_op2))
#define ir_ROR_U32(_op1, _op2)            ir_BINARY_OP_U32(IR_ROR, (_op1), (_op2))
#define ir_ROR_U64(_op1, _op2)            ir_BINARY_OP_U64(IR_ROR, (_op1), (_op2))
#define ir_ROR_A(_op1, _op2)              ir_BINARY_OP_A(IR_ROR, (_op1), (_op2))
#define ir_ROR_C(_op1, _op2)              ir_BINARY_OP_C(IR_ROR, (_op1), (_op2))
#define ir_ROR_I8(_op1, _op2)             ir_BINARY_OP_I8(IR_ROR, (_op1), (_op2))
#define ir_ROR_I16(_op1, _op2)            ir_BINARY_OP_I16(IR_ROR, (_op1), (_op2))
#define ir_ROR_I32(_op1, _op2)            ir_BINARY_OP_I32(IR_ROR, (_op1), (_op2))
#define ir_ROR_I64(_op1, _op2)            ir_BINARY_OP_I64(IR_ROR, (_op1), (_op2))

#define ir_BSWAP(_type, _op1)             ir_UNARY_OP(IR_BSWAP, (_type), (_op1))
#define ir_BSWAP_U16(_op1)                ir_UNARY_OP_U16(IR_BSWAP, (_op1))
#define ir_BSWAP_U32(_op1)                ir_UNARY_OP_U32(IR_BSWAP, (_op1))
#define ir_BSWAP_U64(_op1)                ir_UNARY_OP_U64(IR_BSWAP, (_op1))
#define ir_BSWAP_A(_op1)                  ir_UNARY_OP_A(IR_BSWAP, (_op1))
#define ir_BSWAP_I16(_op1)                ir_UNARY_OP_I16(IR_BSWAP, (_op1))
#define ir_BSWAP_I32(_op1)                ir_UNARY_OP_I32(IR_BSWAP, (_op1))
#define ir_BSWAP_I64(_op1)                ir_UNARY_OP_I64(IR_BSWAP, (_op1))

#define ir_CTPOP(_type, _op1)             ir_UNARY_OP(IR_CTPOP, (_type), (_op1))
#define ir_CTPOP_8(_op1)                  ir_UNARY_OP_U8(IR_CTPOP, (_op1))
#define ir_CTPOP_U16(_op1)                ir_UNARY_OP_U16(IR_CTPOP, (_op1))
#define ir_CTPOP_U32(_op1)                ir_UNARY_OP_U32(IR_CTPOP, (_op1))
#define ir_CTPOP_U64(_op1)                ir_UNARY_OP_U64(IR_CTPOP, (_op1))
#define ir_CTPOP_A(_op1)                  ir_UNARY_OP_A(IR_CTPOP, (_op1))
#define ir_CTPOP_C(_op1)                  ir_UNARY_OP_C(IR_CTPOP, (_op1))
#define ir_CTPOP_I8(_op1)                 ir_UNARY_OP_I8(IR_CTPOP, (_op1))
#define ir_CTPOP_I16(_op1)                ir_UNARY_OP_I16(IR_CTPOP, (_op1))
#define ir_CTPOP_I32(_op1)                ir_UNARY_OP_I32(IR_CTPOP, (_op1))
#define ir_CTPOP_I64(_op1)                ir_UNARY_OP_I64(IR_CTPOP, (_op1))

#define ir_CTLZ(_type, _op1)              ir_UNARY_OP(IR_CTLZ, (_type), (_op1))
#define ir_CTLZ_8(_op1)                   ir_UNARY_OP_U8(IR_CTLZ, (_op1))
#define ir_CTLZ_U16(_op1)                 ir_UNARY_OP_U16(IR_CTLZ, (_op1))
#define ir_CTLZ_U32(_op1)                 ir_UNARY_OP_U32(IR_CTLZ, (_op1))
#define ir_CTLZ_U64(_op1)                 ir_UNARY_OP_U64(IR_CTLZ, (_op1))
#define ir_CTLZ_A(_op1)                   ir_UNARY_OP_A(IR_CTLZ, (_op1))
#define ir_CTLZ_C(_op1)                   ir_UNARY_OP_C(IR_CTLZ, (_op1))
#define ir_CTLZ_I8(_op1)                  ir_UNARY_OP_I8(IR_CTLZ, (_op1))
#define ir_CTLZ_I16(_op1)                 ir_UNARY_OP_I16(IR_CTLZ, (_op1))
#define ir_CTLZ_I32(_op1)                 ir_UNARY_OP_I32(IR_CTLZ, (_op1))
#define ir_CTLZ_I64(_op1)                 ir_UNARY_OP_I64(IR_CTLZ, (_op1))

#define ir_CTTZ(_type, _op1)              ir_UNARY_OP(IR_CTTZ, (_type), (_op1))
#define ir_CTTZ_8(_op1)                   ir_UNARY_OP_U8(IR_CTTZ, (_op1))
#define ir_CTTZ_U16(_op1)                 ir_UNARY_OP_U16(IR_CTTZ, (_op1))
#define ir_CTTZ_U32(_op1)                 ir_UNARY_OP_U32(IR_CTTZ, (_op1))
#define ir_CTTZ_U64(_op1)                 ir_UNARY_OP_U64(IR_CTTZ, (_op1))
#define ir_CTTZ_A(_op1)                   ir_UNARY_OP_A(IR_CTTZ, (_op1))
#define ir_CTTZ_C(_op1)                   ir_UNARY_OP_C(IR_CTTZ, (_op1))
#define ir_CTTZ_I8(_op1)                  ir_UNARY_OP_I8(IR_CTTZ, (_op1))
#define ir_CTTZ_I16(_op1)                 ir_UNARY_OP_I16(IR_CTTZ, (_op1))
#define ir_CTTZ_I32(_op1)                 ir_UNARY_OP_I32(IR_CTTZ, (_op1))
#define ir_CTTZ_I64(_op1)                 ir_UNARY_OP_I64(IR_CTTZ, (_op1))

#define ir_MIN(_type, _op1, _op2)         ir_BINARY_OP(IR_MIN, (_type), (_op1), (_op2))
#define ir_MIN_U8(_op1, _op2)             ir_BINARY_OP_U8(IR_MIN, (_op1), (_op2))
#define ir_MIN_U16(_op1, _op2)            ir_BINARY_OP_U16(IR_MIN, (_op1), (_op2))
#define ir_MIN_U32(_op1, _op2)            ir_BINARY_OP_U32(IR_MIN, (_op1), (_op2))
#define ir_MIN_U64(_op1, _op2)            ir_BINARY_OP_U64(IR_MIN, (_op1), (_op2))
#define ir_MIN_A(_op1, _op2)              ir_BINARY_OP_A(IR_MIN, (_op1), (_op2))
#define ir_MIN_C(_op1, _op2)              ir_BINARY_OP_C(IR_MIN, (_op1), (_op2))
#define ir_MIN_I8(_op1, _op2)             ir_BINARY_OP_I8(IR_MIN, (_op1), (_op2))
#define ir_MIN_I16(_op1, _op2)            ir_BINARY_OP_I16(IR_MIN, (_op1), (_op2))
#define ir_MIN_I32(_op1, _op2)            ir_BINARY_OP_I32(IR_MIN, (_op1), (_op2))
#define ir_MIN_I64(_op1, _op2)            ir_BINARY_OP_I64(IR_MIN, (_op1), (_op2))
#define ir_MIN_D(_op1, _op2)              ir_BINARY_OP_D(IR_MIN, (_op1), (_op2))
#define ir_MIN_F(_op1, _op2)              ir_BINARY_OP_F(IR_MIN, (_op1), (_op2))

#define ir_MAX(_type, _op1, _op2)         ir_BINARY_OP(IR_MAX, (_type), (_op1), (_op2))
#define ir_MAX_U8(_op1, _op2)             ir_BINARY_OP_U8(IR_MAX, (_op1), (_op2))
#define ir_MAX_U16(_op1, _op2)            ir_BINARY_OP_U16(IR_MAX, (_op1), (_op2))
#define ir_MAX_U32(_op1, _op2)            ir_BINARY_OP_U32(IR_MAX, (_op1), (_op2))
#define ir_MAX_U64(_op1, _op2)            ir_BINARY_OP_U64(IR_MAX, (_op1), (_op2))
#define ir_MAX_A(_op1, _op2)              ir_BINARY_OP_A(IR_MAX, (_op1), (_op2))
#define ir_MAX_C(_op1, _op2)              ir_BINARY_OP_C(IR_MAX, (_op1), (_op2))
#define ir_MAX_I8(_op1, _op2)             ir_BINARY_OP_I8(IR_MAX, (_op1), (_op2))
#define ir_MAX_I16(_op1, _op2)            ir_BINARY_OP_I16(IR_MAX, (_op1), (_op2))
#define ir_MAX_I32(_op1, _op2)            ir_BINARY_OP_I32(IR_MAX, (_op1), (_op2))
#define ir_MAX_I64(_op1, _op2)            ir_BINARY_OP_I64(IR_MAX, (_op1), (_op2))
#define ir_MAX_D(_op1, _op2)              ir_BINARY_OP_D(IR_MAX, (_op1), (_op2))
#define ir_MAX_F(_op1, _op2)              ir_BINARY_OP_F(IR_MAX, (_op1), (_op2))

#define ir_COND(_type, _op1, _op2, _op3)  ir_fold3(_ir_CTX, IR_OPT(IR_COND, (_type)),   (_op1), (_op2), (_op3))
#define ir_COND_U8(_op1, _op2, _op3)      ir_fold3(_ir_CTX, IR_OPT(IR_COND, IR_U8),     (_op1), (_op2), (_op3))
#define ir_COND_U16(_op1, _op2, _op3)     ir_fold3(_ir_CTX, IR_OPT(IR_COND, IR_U16),    (_op1), (_op2), (_op3))
#define ir_COND_U32(_op1, _op2, _op3)     ir_fold3(_ir_CTX, IR_OPT(IR_COND, IR_U32),    (_op1), (_op2), (_op3))
#define ir_COND_U64(_op1, _op2, _op3)     ir_fold3(_ir_CTX, IR_OPT(IR_COND, IR_U64),    (_op1), (_op2), (_op3))
#define ir_COND_A(_op1, _op2, _op3)       ir_fold3(_ir_CTX, IR_OPT(IR_COND, IR_ADDR),   (_op1), (_op2), (_op3))
#define ir_COND_C(_op1, _op2, _op3)       ir_fold3(_ir_CTX, IR_OPT(IR_COND, IR_CHAR),   (_op1), (_op2), (_op3))
#define ir_COND_I8(_op1, _op2, _op3)      ir_fold3(_ir_CTX, IR_OPT(IR_COND, IR_I8),     (_op1), (_op2), (_op3))
#define ir_COND_I16(_op1, _op2, _op3)     ir_fold3(_ir_CTX, IR_OPT(IR_COMD, IR_I16),    (_op1), (_op2), (_op3))
#define ir_COND_I32(_op1, _op2, _op3)     ir_fold3(_ir_CTX, IR_OPT(IR_COND, IR_I32),    (_op1), (_op2), (_op3))
#define ir_COND_I64(_op1, _op2, _op3)     ir_fold3(_ir_CTX, IR_OPT(IR_COND, IR_I64),    (_op1), (_op2), (_op3))
#define ir_COND_D(_op1, _op2, _op3)       ir_fold3(_ir_CTX, IR_OPT(IR_COND, IR_DOUBLE), (_op1), (_op2), (_op3))
#define ir_COND_F(_op1, _op2, _op3)       ir_fold3(_ir_CTX, IR_OPT(IR_COND, IR_FLOAT),  (_op1), (_op2), (_op3))

#define ir_PHI_2(type, _src1, _src2)      _ir_PHI_2(_ir_CTX, type, (_src1), (_src2))
#define ir_PHI_N(type, _n, _inputs)       _ir_PHI_N(_ir_CTX, type, (_n), (_inputs))
#define ir_PHI_SET_OP(_ref, _pos, _src)   _ir_PHI_SET_OP(_ir_CTX, (_ref), (_pos), (_src))

#define ir_COPY(_type, _op1)              ir_UNARY_OP(IR_COPY, (_type), (_op1))
#define ir_COPY_B(_op1)                   ir_UNARY_OP_B(IR_COPY, (_op1))
#define ir_COPY_U8(_op1)                  ir_UNARY_OP_U8(IR_COPY, (_op1))
#define ir_COPY_U16(_op1)                 ir_UNARY_OP_U16(IR_COPY, (_op1))
#define ir_COPY_U32(_op1)                 ir_UNARY_OP_U32(IR_COPY, (_op1))
#define ir_COPY_U64(_op1)                 ir_UNARY_OP_U64(IR_COPY, (_op1))
#define ir_COPY_A(_op1)                   ir_UNARY_OP_A(IR_COPY, (_op1))
#define ir_COPY_C(_op1)                   ir_UNARY_OP_C(IR_COPY, (_op1))
#define ir_COPY_I8(_op1)                  ir_UNARY_OP_I8(IR_COPY, (_op1))
#define ir_COPY_I16(_op1)                 ir_UNARY_OP_I16(IR_COPY, (_op1))
#define ir_COPY_I32(_op1)                 ir_UNARY_OP_I32(IR_COPY, (_op1))
#define ir_COPY_I64(_op1)                 ir_UNARY_OP_I64(IR_COPY, (_op1))
#define ir_COPY_D(_op1)                   ir_UNARY_OP_D(IR_COPY, (_op1))
#define ir_COPY_F(_op1)                   ir_UNARY_OP_F(IR_COPY, (_op1))

/* Helper to add address with a constant offset */
#define ir_ADD_OFFSET(_addr, _offset)     _ir_ADD_OFFSET(_ir_CTX, (_addr), (_offset))

/* Unfoldable variant of COPY */
#define ir_HARD_COPY(_type, _op1)         ir_BINARY_OP(IR_COPY, (_type), (_op1), 1)
#define ir_HARD_COPY_B(_op1)              ir_BINARY_OP_B(IR_COPY, (_op1), 1)
#define ir_HARD_COPY_U8(_op1)             ir_BINARY_OP_U8(IR_COPY, (_op1), 1)
#define ir_HARD_COPY_U16(_op1)            ir_BINARY_OP_U16(IR_COPY, (_op1), 1)
#define ir_HARD_COPY_U32(_op1)            ir_BINARY_OP_U32(IR_COPY, (_op1), 1)
#define ir_HARD_COPY_U64(_op1)            ir_BINARY_OP_U64(IR_COPY, (_op1), 1)
#define ir_HARD_COPY_A(_op1)              ir_BINARY_OP_A(IR_COPY, (_op1), 1)
#define ir_HARD_COPY_C(_op1)              ir_BINARY_OP_C(IR_COPY, (_op1), 1)
#define ir_HARD_COPY_I8(_op1)             ir_BINARY_OP_I8(IR_COPY, (_op1), 1)
#define ir_HARD_COPY_I16(_op1)            ir_BINARY_OP_I16(IR_COPY, (_op1), 1)
#define ir_HARD_COPY_I32(_op1)            ir_BINARY_OP_I32(IR_COPY, (_op1), 1)
#define ir_HARD_COPY_I64(_op1)            ir_BINARY_OP_I64(IR_COPY, (_op1), 1)
#define ir_HARD_COPY_D(_op1)              ir_BINARY_OP_D(IR_COPY, (_op1), 1)
#define ir_HARD_COPY_F(_op1)              ir_BINARY_OP_F(IR_COPY, (_op1), 1)

#define ir_PARAM(_type, _name, _num)      _ir_PARAM(_ir_CTX, (_type), (_name), (_num))
#define ir_VAR(_type, _name)              _ir_VAR(_ir_CTX, (_type), (_name))

#define ir_CALL(type, func)                        _ir_CALL(_ir_CTX, type, func)
#define ir_CALL_1(type, func, a1)                  _ir_CALL_1(_ir_CTX, type, func, a1)
#define ir_CALL_2(type, func, a1, a2)              _ir_CALL_2(_ir_CTX, type, func, a1, a2)
#define ir_CALL_3(type, func, a1, a2, a3)          _ir_CALL_3(_ir_CTX, type, func, a1, a2, a3)
#define ir_CALL_4(type, func, a1, a2, a3, a4)      _ir_CALL_4(_ir_CTX, type, func, a1, a2, a3, a4)
#define ir_CALL_5(type, func, a1, a2, a3, a4, a5)  _ir_CALL_5(_ir_CTX, type, func, a1, a2, a3, a4, a5)
#define ir_CALL_N(type, func, count, args)         _ir_CALL_N(_ir_CTX, type, func, count, args)

#define ir_TAILCALL(type, func)                       _ir_TAILCALL(_ir_CTX, type, func)
#define ir_TAILCALL_1(type, func, a1)                 _ir_TAILCALL_1(_ir_CTX, type, func, a1)
#define ir_TAILCALL_2(type, func, a1, a2)             _ir_TAILCALL_2(_ir_CTX, type, func, a1, a2)
#define ir_TAILCALL_3(type, func, a1, a2, a3)         _ir_TAILCALL_3(_ir_CTX, type, func, a1, a2, a3)
#define ir_TAILCALL_4(type, func, a1, a2, a3, a4)     _ir_TAILCALL_4(_ir_CTX, type, func, a1, a2, a3, a4)
#define ir_TAILCALL_5(type, func, a1, a2, a3, a4, a5) _ir_TAILCALL_5(_ir_CTX, type, func, a1, a2, a3, a4, a5)
#define ir_TAILCALL_N(type, func, count, args)        _ir_TAILCALL_N(_ir_CTX, type, func, count, args)

#define ir_ALLOCA(_size)                  _ir_ALLOCA(_ir_CTX, (_size))
#define ir_AFREE(_size)                   _ir_AFREE(_ir_CTX, (_size))
#define ir_VADDR(_var)                    ir_emit1(_ir_CTX, IR_OPT(IR_VADDR, IR_ADDR), (_var))
#define ir_VLOAD(_type, _var)             _ir_VLOAD(_ir_CTX, (_type), (_var))
#define ir_VLOAD_B(_var)                  _ir_VLOAD(_ir_CTX, IR_BOOL, (_var))
#define ir_VLOAD_U8(_var)                 _ir_VLOAD(_ir_CTX, IR_U8, (_var))
#define ir_VLOAD_U16(_var)                _ir_VLOAD(_ir_CTX, IR_U16, (_var))
#define ir_VLOAD_U32(_var)                _ir_VLOAD(_ir_CTX, IR_U32, (_var))
#define ir_VLOAD_U64(_var)                _ir_VLOAD(_ir_CTX, IR_U64, (_var))
#define ir_VLOAD_A(_var)                  _ir_VLOAD(_ir_CTX, IR_ADDR, (_var))
#define ir_VLOAD_C(_var)                  _ir_VLOAD(_ir_CTX, IR_CHAR, (_var))
#define ir_VLOAD_I8(_var)                 _ir_VLOAD(_ir_CTX, IR_I8, (_var))
#define ir_VLOAD_I16(_var)                _ir_VLOAD(_ir_CTX, IR_I16, (_var))
#define ir_VLOAD_I32(_var)                _ir_VLOAD(_ir_CTX, IR_I32, (_var))
#define ir_VLOAD_I64(_var)                _ir_VLOAD(_ir_CTX, IR_I64, (_var))
#define ir_VLOAD_D(_var)                  _ir_VLOAD(_ir_CTX, IR_DOUBLE, (_var))
#define ir_VLOAD_F(_var)                  _ir_VLOAD(_ir_CTX, IR_FLOAT, (_var))
#define ir_VSTORE(_var, _val)             _ir_VSTORE(_ir_CTX, (_var), (_val))
#define ir_RLOAD(_type, _reg)             _ir_RLOAD(_ir_CTX, (_type), (_reg))
#define ir_RLOAD_B(_reg)                  _ir_RLOAD(_ir_CTX, IR_BOOL, (_reg))
#define ir_RLOAD_U8(_reg)                 _ir_RLOAD(_ir_CTX, IR_U8, (_reg))
#define ir_RLOAD_U16(_reg)                _ir_RLOAD(_ir_CTX, IR_U16, (_reg))
#define ir_RLOAD_U32(_reg)                _ir_RLOAD(_ir_CTX, IR_U32, (_reg))
#define ir_RLOAD_U64(_reg)                _ir_RLOAD(_ir_CTX, IR_U64, (_reg))
#define ir_RLOAD_A(_reg)                  _ir_RLOAD(_ir_CTX, IR_ADDR, (_reg))
#define ir_RLOAD_C(_reg)                  _ir_RLOAD(_ir_CTX, IR_CHAR, (_reg))
#define ir_RLOAD_I8(_reg)                 _ir_RLOAD(_ir_CTX, IR_I8, (_reg))
#define ir_RLOAD_I16(_reg)                _ir_RLOAD(_ir_CTX, IR_I16, (_reg))
#define ir_RLOAD_I32(_reg)                _ir_RLOAD(_ir_CTX, IR_I32, (_reg))
#define ir_RLOAD_I64(_reg)                _ir_RLOAD(_ir_CTX, IR_I64, (_reg))
#define ir_RLOAD_D(_reg)                  _ir_RLOAD(_ir_CTX, IR_DOUBLE, (_reg))
#define ir_RLOAD_F(_reg)                  _ir_RLOAD(_ir_CTX, IR_FLOAT, (_reg))
#define ir_RSTORE(_reg, _val)             _ir_RSTORE(_ir_CTX, (_reg), (_val))
#define ir_LOAD(_type, _addr)             _ir_LOAD(_ir_CTX, (_type), (_addr))
#define ir_LOAD_B(_addr)                  _ir_LOAD(_ir_CTX, IR_BOOL, (_addr))
#define ir_LOAD_U8(_addr)                 _ir_LOAD(_ir_CTX, IR_U8, (_addr))
#define ir_LOAD_U16(_addr)                _ir_LOAD(_ir_CTX, IR_U16, (_addr))
#define ir_LOAD_U32(_addr)                _ir_LOAD(_ir_CTX, IR_U32, (_addr))
#define ir_LOAD_U64(_addr)                _ir_LOAD(_ir_CTX, IR_U64, (_addr))
#define ir_LOAD_A(_addr)                  _ir_LOAD(_ir_CTX, IR_ADDR, (_addr))
#define ir_LOAD_C(_addr)                  _ir_LOAD(_ir_CTX, IR_CHAR, (_addr))
#define ir_LOAD_I8(_addr)                 _ir_LOAD(_ir_CTX, IR_I8, (_addr))
#define ir_LOAD_I16(_addr)                _ir_LOAD(_ir_CTX, IR_I16, (_addr))
#define ir_LOAD_I32(_addr)                _ir_LOAD(_ir_CTX, IR_I32, (_addr))
#define ir_LOAD_I64(_addr)                _ir_LOAD(_ir_CTX, IR_I64, (_addr))
#define ir_LOAD_D(_addr)                  _ir_LOAD(_ir_CTX, IR_DOUBLE, (_addr))
#define ir_LOAD_F(_addr)                  _ir_LOAD(_ir_CTX, IR_FLOAT, (_addr))
#define ir_STORE(_addr, _val)             _ir_STORE(_ir_CTX, (_addr), (_val))
#define ir_TLS(_index, _offset)           _ir_TLS(_ir_CTX, (_index), (_offset))
#define ir_TRAP()                         do {_ir_CTX->control = ir_emit1(_ir_CTX, IR_TRAP, _ir_CTX->control);} while (0)

#define ir_FRAME_ADDR()                   ir_fold0(_ir_CTX, IR_OPT(IR_FRAME_ADDR, IR_ADDR))

#define ir_VA_START(_list)                _ir_VA_START(_ir_CTX, _list)
#define ir_VA_END(_list)                  _ir_VA_END(_ir_CTX, _list)
#define ir_VA_COPY(_dst, _src)            _ir_VA_COPY(_ir_CTX, _dst, _src)
#define ir_VA_ARG(_list, _type)           _ir_VA_ARG(_ir_CTX, _type, _list)

#define ir_START()                        _ir_START(_ir_CTX)
#define ir_ENTRY(_src, _num)              _ir_ENTRY(_ir_CTX, (_src), (_num))
#define ir_BEGIN(_src)                    _ir_BEGIN(_ir_CTX, (_src))
#define ir_IF(_condition)                 _ir_IF(_ir_CTX, (_condition))
#define ir_IF_TRUE(_if)                   _ir_IF_TRUE(_ir_CTX, (_if))
#define ir_IF_TRUE_cold(_if)              _ir_IF_TRUE_cold(_ir_CTX, (_if))
#define ir_IF_FALSE(_if)                  _ir_IF_FALSE(_ir_CTX, (_if))
#define ir_IF_FALSE_cold(_if)             _ir_IF_FALSE_cold(_ir_CTX, (_if))
#define ir_END()                          _ir_END(_ir_CTX)
#define ir_MERGE_2(_src1, _src2)          _ir_MERGE_2(_ir_CTX, (_src1), (_src2))
#define ir_MERGE_N(_n, _inputs)           _ir_MERGE_N(_ir_CTX, (_n), (_inputs))
#define ir_MERGE_SET_OP(_ref, _pos, _src) _ir_MERGE_SET_OP(_ir_CTX, (_ref), (_pos), (_src))
#define ir_LOOP_BEGIN(_src1)              _ir_LOOP_BEGIN(_ir_CTX, (_src1))
#define ir_LOOP_END()                     _ir_LOOP_END(_ir_CTX)
#define ir_SWITCH(_val)                   _ir_SWITCH(_ir_CTX, (_val))
#define ir_CASE_VAL(_switch, _val)        _ir_CASE_VAL(_ir_CTX, (_switch), (_val))
#define ir_CASE_DEFAULT(_switch)          _ir_CASE_DEFAULT(_ir_CTX, (_switch))
#define ir_RETURN(_val)                   _ir_RETURN(_ir_CTX, (_val))
#define ir_IJMP(_addr)                    _ir_IJMP(_ir_CTX, (_addr))
#define ir_UNREACHABLE()                  _ir_UNREACHABLE(_ir_CTX)

#define ir_GUARD(_condition, _addr)       _ir_GUARD(_ir_CTX, (_condition), (_addr))
#define ir_GUARD_NOT(_condition, _addr)   _ir_GUARD_NOT(_ir_CTX, (_condition), (_addr))

#define ir_SNAPSHOT(_n)                   _ir_SNAPSHOT(_ir_CTX, (_n))
#define ir_SNAPSHOT_SET_OP(_s, _pos, _v)  _ir_SNAPSHOT_SET_OP(_ir_CTX, (_s), (_pos), (_v))

#define ir_EXITCALL(_func)                _ir_EXITCALL(_ir_CTX,(_func))

#define ir_END_list(_list)                do { _list = _ir_END_LIST(_ir_CTX, _list); } while (0)
#define ir_MERGE_list(_list)              _ir_MERGE_LIST(_ir_CTX, (_list))

#define ir_END_PHI_list(_list, _val)      do { _list = _ir_END_PHI_LIST(_ir_CTX, _list, _val); } while (0)
#define ir_PHI_list(_list)                _ir_PHI_LIST(_ir_CTX, (_list))

#define ir_MERGE_WITH(_src2)              do {ir_ref end = ir_END(); ir_MERGE_2(end, _src2);} while (0)
#define ir_MERGE_WITH_EMPTY_TRUE(_if)     do {ir_ref end = ir_END(); ir_IF_TRUE(_if); ir_MERGE_2(end, ir_END());} while (0)
#define ir_MERGE_WITH_EMPTY_FALSE(_if)    do {ir_ref end = ir_END(); ir_IF_FALSE(_if); ir_MERGE_2(end, ir_END());} while (0)

ir_ref _ir_ADD_OFFSET(ir_ctx *ctx, ir_ref addr, uintptr_t offset);
ir_ref _ir_PHI_2(ir_ctx *ctx, ir_type type, ir_ref src1, ir_ref src2);
ir_ref _ir_PHI_N(ir_ctx *ctx, ir_type type, ir_ref n, ir_ref *inputs);
void   _ir_PHI_SET_OP(ir_ctx *ctx, ir_ref phi, ir_ref pos, ir_ref src);
ir_ref _ir_PARAM(ir_ctx *ctx, ir_type type, const char* name, ir_ref num);
ir_ref _ir_VAR(ir_ctx *ctx, ir_type type, const char* name);
ir_ref _ir_CALL(ir_ctx *ctx, ir_type type, ir_ref func);
ir_ref _ir_CALL_1(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1);
ir_ref _ir_CALL_2(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2);
ir_ref _ir_CALL_3(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2, ir_ref arg3);
ir_ref _ir_CALL_4(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2, ir_ref arg3, ir_ref arg4);
ir_ref _ir_CALL_5(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2, ir_ref arg3, ir_ref arg4, ir_ref arg5);
ir_ref _ir_CALL_N(ir_ctx *ctx, ir_type type, ir_ref func, uint32_t count, ir_ref *args);
void   _ir_TAILCALL(ir_ctx *ctx, ir_type type, ir_ref func);
void   _ir_TAILCALL_1(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1);
void   _ir_TAILCALL_2(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2);
void   _ir_TAILCALL_3(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2, ir_ref arg3);
void   _ir_TAILCALL_4(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2, ir_ref arg3, ir_ref arg4);
void   _ir_TAILCALL_5(ir_ctx *ctx, ir_type type, ir_ref func, ir_ref arg1, ir_ref arg2, ir_ref arg3, ir_ref arg4, ir_ref arg5);
ir_ref _ir_TAILCALL_N(ir_ctx *ctx, ir_type type, ir_ref func, uint32_t count, ir_ref *args);
ir_ref _ir_ALLOCA(ir_ctx *ctx, ir_ref size);
void   _ir_AFREE(ir_ctx *ctx, ir_ref size);
ir_ref _ir_VLOAD(ir_ctx *ctx, ir_type type, ir_ref var);
void   _ir_VSTORE(ir_ctx *ctx, ir_ref var, ir_ref val);
ir_ref _ir_RLOAD(ir_ctx *ctx, ir_type type, ir_ref reg);
void   _ir_RSTORE(ir_ctx *ctx, ir_ref reg, ir_ref val);
ir_ref _ir_LOAD(ir_ctx *ctx, ir_type type, ir_ref addr);
void   _ir_STORE(ir_ctx *ctx, ir_ref addr, ir_ref val);
void   _ir_VA_START(ir_ctx *ctx, ir_ref list);
void   _ir_VA_END(ir_ctx *ctx, ir_ref list);
void   _ir_VA_COPY(ir_ctx *ctx, ir_ref dst, ir_ref src);
ir_ref _ir_VA_ARG(ir_ctx *ctx, ir_type type, ir_ref list);
void   _ir_START(ir_ctx *ctx);
void   _ir_ENTRY(ir_ctx *ctx, ir_ref src, ir_ref num);
void   _ir_BEGIN(ir_ctx *ctx, ir_ref src);
ir_ref _ir_END(ir_ctx *ctx);
ir_ref _ir_END_LIST(ir_ctx *ctx, ir_ref list);
ir_ref _ir_END_PHI_LIST(ir_ctx *ctx, ir_ref list, ir_ref val);
ir_ref _ir_IF(ir_ctx *ctx, ir_ref condition);
void   _ir_IF_TRUE(ir_ctx *ctx, ir_ref if_ref);
void   _ir_IF_TRUE_cold(ir_ctx *ctx, ir_ref if_ref);
void   _ir_IF_FALSE(ir_ctx *ctx, ir_ref if_ref);
void   _ir_IF_FALSE_cold(ir_ctx *ctx, ir_ref if_ref);
void   _ir_MERGE_2(ir_ctx *ctx, ir_ref src1, ir_ref src2);
void   _ir_MERGE_N(ir_ctx *ctx, ir_ref n, ir_ref *inputs);
void   _ir_MERGE_SET_OP(ir_ctx *ctx, ir_ref merge, ir_ref pos, ir_ref src);
void   _ir_MERGE_LIST(ir_ctx *ctx, ir_ref list);
ir_ref _ir_PHI_LIST(ir_ctx *ctx, ir_ref list);
ir_ref _ir_LOOP_BEGIN(ir_ctx *ctx, ir_ref src1);
ir_ref _ir_LOOP_END(ir_ctx *ctx);
ir_ref _ir_TLS(ir_ctx *ctx, ir_ref index, ir_ref offset);
void   _ir_UNREACHABLE(ir_ctx *ctx);
ir_ref _ir_SWITCH(ir_ctx *ctx, ir_ref val);
void   _ir_CASE_VAL(ir_ctx *ctx, ir_ref switch_ref, ir_ref val);
void   _ir_CASE_DEFAULT(ir_ctx *ctx, ir_ref switch_ref);
void   _ir_RETURN(ir_ctx *ctx, ir_ref val);
void   _ir_IJMP(ir_ctx *ctx, ir_ref addr);
void   _ir_GUARD(ir_ctx *ctx, ir_ref condition, ir_ref addr);
void   _ir_GUARD_NOT(ir_ctx *ctx, ir_ref condition, ir_ref addr);
ir_ref _ir_SNAPSHOT(ir_ctx *ctx, ir_ref n);
void   _ir_SNAPSHOT_SET_OP(ir_ctx *ctx, ir_ref snapshot, ir_ref pos, ir_ref val);
ir_ref _ir_EXITCALL(ir_ctx *ctx, ir_ref func);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* IR_BUILDER_H */
