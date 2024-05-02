#!/usr/bin/env php
<?php
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

const HEADER_TEXT = <<< DATA
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/


DATA;

/*
    This script creates zend_vm_execute.h and zend_vm_opcodes.{h,c}
    from existing zend_vm_def.h and zend_vm_execute.skl
*/

error_reporting(E_ALL);

const ZEND_VM_KIND_CALL   = 1;
const ZEND_VM_KIND_SWITCH = 2;
const ZEND_VM_KIND_GOTO   = 3;
const ZEND_VM_KIND_HYBRID = 4;

$vm_op_flags = array(
    "ZEND_VM_OP_SPEC"         => 1<<0,
    "ZEND_VM_OP_CONST"        => 1<<1,
    "ZEND_VM_OP_TMPVAR"       => 1<<2,
    "ZEND_VM_OP_TMPVARCV"     => 1<<3,
    "ZEND_VM_OP_MASK"         => 0xf0,
    "ZEND_VM_OP_NUM"          => 0x10,
    "ZEND_VM_OP_JMP_ADDR"     => 0x20,
    "ZEND_VM_OP_TRY_CATCH"    => 0x30,
    // unused 0x40
    "ZEND_VM_OP_THIS"         => 0x50,
    "ZEND_VM_OP_NEXT"         => 0x60,
    "ZEND_VM_OP_CLASS_FETCH"  => 0x70,
    "ZEND_VM_OP_CONSTRUCTOR"  => 0x80,
    "ZEND_VM_OP_CONST_FETCH"  => 0x90,
    "ZEND_VM_OP_CACHE_SLOT"   => 0xa0,

    "ZEND_VM_EXT_VAR_FETCH"   => 1<<16,
    "ZEND_VM_EXT_ISSET"       => 1<<17,
    "ZEND_VM_EXT_CACHE_SLOT"  => 1<<18,
    "ZEND_VM_EXT_ARRAY_INIT"  => 1<<19,
    "ZEND_VM_EXT_REF"         => 1<<20,
    "ZEND_VM_EXT_FETCH_REF"   => 1<<21,
    "ZEND_VM_EXT_DIM_WRITE"    => 1<<22,
    "ZEND_VM_EXT_MASK"        => 0x0f000000,
    "ZEND_VM_EXT_NUM"         => 0x01000000,
    "ZEND_VM_EXT_LAST_CATCH"  => 0x02000000,
    "ZEND_VM_EXT_JMP_ADDR"    => 0x03000000,
    "ZEND_VM_EXT_OP"          => 0x04000000,
    // unused 0x5000000
    // unused 0x6000000
    "ZEND_VM_EXT_TYPE"        => 0x07000000,
    "ZEND_VM_EXT_EVAL"        => 0x08000000,
    "ZEND_VM_EXT_TYPE_MASK"   => 0x09000000,
    // unused 0x0a000000,
    "ZEND_VM_EXT_SRC"         => 0x0b000000,
    // unused 0x0c000000,
    "ZEND_VM_NO_CONST_CONST"  => 0x40000000,
    "ZEND_VM_COMMUTATIVE"     => 0x80000000,
);

foreach ($vm_op_flags as $name => $val) {
    define($name, $val);
}

$vm_op_decode = array(
    "ANY"                  => 0,
    "CONST"                => ZEND_VM_OP_SPEC | ZEND_VM_OP_CONST,
    "TMP"                  => ZEND_VM_OP_SPEC,
    "VAR"                  => ZEND_VM_OP_SPEC,
    "UNUSED"               => ZEND_VM_OP_SPEC,
    "CV"                   => ZEND_VM_OP_SPEC,
    "TMPVAR"               => ZEND_VM_OP_SPEC | ZEND_VM_OP_TMPVAR,
    "TMPVARCV"             => ZEND_VM_OP_SPEC | ZEND_VM_OP_TMPVARCV,
    "NUM"                  => ZEND_VM_OP_NUM,
    "JMP_ADDR"             => ZEND_VM_OP_JMP_ADDR,
    "TRY_CATCH"            => ZEND_VM_OP_TRY_CATCH,
    "THIS"                 => ZEND_VM_OP_THIS,
    "NEXT"                 => ZEND_VM_OP_NEXT,
    "CLASS_FETCH"          => ZEND_VM_OP_CLASS_FETCH,
    "CONSTRUCTOR"          => ZEND_VM_OP_CONSTRUCTOR,
    "CONST_FETCH"          => ZEND_VM_OP_CONST_FETCH,
    "CACHE_SLOT"           => ZEND_VM_OP_CACHE_SLOT,
);

$vm_ext_decode = array(
    "NUM"                  => ZEND_VM_EXT_NUM,
    "LAST_CATCH"           => ZEND_VM_EXT_LAST_CATCH,
    "JMP_ADDR"             => ZEND_VM_EXT_JMP_ADDR,
    "OP"                   => ZEND_VM_EXT_OP,
    "VAR_FETCH"            => ZEND_VM_EXT_VAR_FETCH,
    "ARRAY_INIT"           => ZEND_VM_EXT_ARRAY_INIT,
    "TYPE"                 => ZEND_VM_EXT_TYPE,
    "EVAL"                 => ZEND_VM_EXT_EVAL,
    "TYPE_MASK"            => ZEND_VM_EXT_TYPE_MASK,
    "ISSET"                => ZEND_VM_EXT_ISSET,
    "REF"                  => ZEND_VM_EXT_REF,
    "FETCH_REF"            => ZEND_VM_EXT_FETCH_REF,
    "SRC"                  => ZEND_VM_EXT_SRC,
    "CACHE_SLOT"           => ZEND_VM_EXT_CACHE_SLOT,
    "DIM_WRITE"            => ZEND_VM_EXT_DIM_WRITE,
);

$vm_kind_name = array(
    ZEND_VM_KIND_CALL      => "ZEND_VM_KIND_CALL",
    ZEND_VM_KIND_SWITCH    => "ZEND_VM_KIND_SWITCH",
    ZEND_VM_KIND_GOTO      => "ZEND_VM_KIND_GOTO",
    ZEND_VM_KIND_HYBRID    => "ZEND_VM_KIND_HYBRID",
);

$op_types = array(
    "ANY",
    "CONST",
    "TMP",
    "VAR",
    "UNUSED",
    "CV",
);

$op_types_ex = array(
    "ANY",
    "CONST",
    "TMPVARCV",
    "TMPVAR",
    "TMP",
    "VAR",
    "UNUSED",
    "CV",
);

$prefix = array(
    "ANY"      => "",
    "TMP"      => "_TMP",
    "VAR"      => "_VAR",
    "CONST"    => "_CONST",
    "UNUSED"   => "_UNUSED",
    "CV"       => "_CV",
    "TMPVAR"   => "_TMPVAR",
    "TMPVARCV" => "_TMPVARCV",
);

$commutative_order = array(
    "ANY"      => 0,
    "TMP"      => 1,
    "VAR"      => 2,
    "CONST"    => 0,
    "UNUSED"   => 0,
    "CV"       => 4,
    "TMPVAR"   => 2,
    "TMPVARCV" => 4,
);

$op1_type = array(
    "ANY"      => "opline->op1_type",
    "TMP"      => "IS_TMP_VAR",
    "VAR"      => "IS_VAR",
    "CONST"    => "IS_CONST",
    "UNUSED"   => "IS_UNUSED",
    "CV"       => "IS_CV",
    "TMPVAR"   => "(IS_TMP_VAR|IS_VAR)",
    "TMPVARCV" => "(IS_TMP_VAR|IS_VAR|IS_CV)",
);

$op2_type = array(
    "ANY"      => "opline->op2_type",
    "TMP"      => "IS_TMP_VAR",
    "VAR"      => "IS_VAR",
    "CONST"    => "IS_CONST",
    "UNUSED"   => "IS_UNUSED",
    "CV"       => "IS_CV",
    "TMPVAR"   => "(IS_TMP_VAR|IS_VAR)",
    "TMPVARCV" => "(IS_TMP_VAR|IS_VAR|IS_CV)",
);

$op1_get_zval_ptr = array(
    "ANY"      => "get_zval_ptr(opline->op1_type, opline->op1, \\1)",
    "TMP"      => "_get_zval_ptr_tmp(opline->op1.var EXECUTE_DATA_CC)",
    "VAR"      => "_get_zval_ptr_var(opline->op1.var EXECUTE_DATA_CC)",
    "CONST"    => "RT_CONSTANT(opline, opline->op1)",
    "UNUSED"   => "NULL",
    "CV"       => "_get_zval_ptr_cv_\\1(opline->op1.var EXECUTE_DATA_CC)",
    "TMPVAR"   => "_get_zval_ptr_var(opline->op1.var EXECUTE_DATA_CC)",
    "TMPVARCV" => "???",
);

$op2_get_zval_ptr = array(
    "ANY"      => "get_zval_ptr(opline->op2_type, opline->op2, \\1)",
    "TMP"      => "_get_zval_ptr_tmp(opline->op2.var EXECUTE_DATA_CC)",
    "VAR"      => "_get_zval_ptr_var(opline->op2.var EXECUTE_DATA_CC)",
    "CONST"    => "RT_CONSTANT(opline, opline->op2)",
    "UNUSED"   => "NULL",
    "CV"       => "_get_zval_ptr_cv_\\1(opline->op2.var EXECUTE_DATA_CC)",
    "TMPVAR"   => "_get_zval_ptr_var(opline->op2.var EXECUTE_DATA_CC)",
    "TMPVARCV" => "???",
);

$op1_get_zval_ptr_ptr = array(
    "ANY"      => "get_zval_ptr_ptr(opline->op1_type, opline->op1, \\1)",
    "TMP"      => "zend_get_bad_ptr()",
    "VAR"      => "_get_zval_ptr_ptr_var(opline->op1.var EXECUTE_DATA_CC)",
    "CONST"    => "zend_get_bad_ptr()",
    "UNUSED"   => "NULL",
    "CV"       => "_get_zval_ptr_cv_\\1(opline->op1.var EXECUTE_DATA_CC)",
    "TMPVAR"   => "???",
    "TMPVARCV" => "???",
);

$op2_get_zval_ptr_ptr = array(
    "ANY"      => "get_zval_ptr_ptr(opline->op2_type, opline->op2, \\1)",
    "TMP"      => "zend_get_bad_ptr()",
    "VAR"      => "_get_zval_ptr_ptr_var(opline->op2.var EXECUTE_DATA_CC)",
    "CONST"    => "zend_get_bad_ptr()",
    "UNUSED"   => "NULL",
    "CV"       => "_get_zval_ptr_cv_\\1(opline->op2.var EXECUTE_DATA_CC)",
    "TMPVAR"   => "???",
    "TMPVARCV" => "???",
);

$op1_get_zval_ptr_deref = array(
    "ANY"      => "get_zval_ptr_deref(opline->op1_type, opline->op1, \\1)",
    "TMP"      => "_get_zval_ptr_tmp(opline->op1.var EXECUTE_DATA_CC)",
    "VAR"      => "_get_zval_ptr_var_deref(opline->op1.var EXECUTE_DATA_CC)",
    "CONST"    => "RT_CONSTANT(opline, opline->op1)",
    "UNUSED"   => "NULL",
    "CV"       => "_get_zval_ptr_cv_deref_\\1(opline->op1.var EXECUTE_DATA_CC)",
    "TMPVAR"   => "???",
    "TMPVARCV" => "_get_zval_ptr_tmpvarcv(opline->op1_type, opline->op1, \\1 EXECUTE_DATA_CC)",
);

$op2_get_zval_ptr_deref = array(
    "ANY"      => "get_zval_ptr_deref(opline->op2_type, opline->op2, \\1)",
    "TMP"      => "_get_zval_ptr_tmp(opline->op2.var EXECUTE_DATA_CC)",
    "VAR"      => "_get_zval_ptr_var_deref(opline->op2.var EXECUTE_DATA_CC)",
    "CONST"    => "RT_CONSTANT(opline, opline->op2)",
    "UNUSED"   => "NULL",
    "CV"       => "_get_zval_ptr_cv_deref_\\1(opline->op2.var EXECUTE_DATA_CC)",
    "TMPVAR"   => "???",
    "TMPVARCV" => "_get_zval_ptr_tmpvarcv(opline->op2_type, opline->op2, \\1 EXECUTE_DATA_CC)",
);

$op1_get_zval_ptr_undef = array(
    "ANY"      => "get_zval_ptr_undef(opline->op1_type, opline->op1, \\1)",
    "TMP"      => "_get_zval_ptr_tmp(opline->op1.var EXECUTE_DATA_CC)",
    "VAR"      => "_get_zval_ptr_var(opline->op1.var EXECUTE_DATA_CC)",
    "CONST"    => "RT_CONSTANT(opline, opline->op1)",
    "UNUSED"   => "NULL",
    "CV"       => "EX_VAR(opline->op1.var)",
    "TMPVAR"   => "_get_zval_ptr_var(opline->op1.var EXECUTE_DATA_CC)",
    "TMPVARCV" => "EX_VAR(opline->op1.var)",
);

$op2_get_zval_ptr_undef = array(
    "ANY"      => "get_zval_ptr_undef(opline->op2_type, opline->op2, \\1)",
    "TMP"      => "_get_zval_ptr_tmp(opline->op2.var EXECUTE_DATA_CC)",
    "VAR"      => "_get_zval_ptr_var(opline->op2.var EXECUTE_DATA_CC)",
    "CONST"    => "RT_CONSTANT(opline, opline->op2)",
    "UNUSED"   => "NULL",
    "CV"       => "EX_VAR(opline->op2.var)",
    "TMPVAR"   => "_get_zval_ptr_var(opline->op2.var EXECUTE_DATA_CC)",
    "TMPVARCV" => "EX_VAR(opline->op2.var)",
);

$op1_get_zval_ptr_ptr_undef = array(
    "ANY"      => "get_zval_ptr_ptr_undef(opline->op1_type, opline->op1, \\1)",
    "TMP"      => "zend_get_bad_ptr()",
    "VAR"      => "_get_zval_ptr_ptr_var(opline->op1.var EXECUTE_DATA_CC)",
    "CONST"    => "zend_get_bad_ptr()",
    "UNUSED"   => "NULL",
    "CV"       => "EX_VAR(opline->op1.var)",
    "TMPVAR"   => "???",
    "TMPVARCV" => "???",
);

$op2_get_zval_ptr_ptr_undef = array(
    "ANY"      => "get_zval_ptr_ptr_undef(opline->op2_type, opline->op2, \\1)",
    "TMP"      => "zend_get_bad_ptr()",
    "VAR"      => "_get_zval_ptr_ptr_var(opline->op2.var EXECUTE_DATA_CC)",
    "CONST"    => "zend_get_bad_ptr()",
    "UNUSED"   => "NULL",
    "CV"       => "EX_VAR(opline->op2.var)",
    "TMPVAR"   => "???",
    "TMPVARCV" => "???",
);

$op1_get_obj_zval_ptr = array(
    "ANY"      => "get_obj_zval_ptr(opline->op1_type, opline->op1, \\1)",
    "TMP"      => "_get_zval_ptr_tmp(opline->op1.var EXECUTE_DATA_CC)",
    "VAR"      => "_get_zval_ptr_var(opline->op1.var EXECUTE_DATA_CC)",
    "CONST"    => "RT_CONSTANT(opline, opline->op1)",
    "UNUSED"   => "&EX(This)",
    "CV"       => "_get_zval_ptr_cv_\\1(opline->op1.var EXECUTE_DATA_CC)",
    "TMPVAR"   => "_get_zval_ptr_var(opline->op1.var EXECUTE_DATA_CC)",
    "TMPVARCV" => "???",
);

$op2_get_obj_zval_ptr = array(
    "ANY"      => "get_obj_zval_ptr(opline->op2_type, opline->op2, \\1)",
    "TMP"      => "_get_zval_ptr_tmp(opline->op2.var EXECUTE_DATA_CC)",
    "VAR"      => "_get_zval_ptr_var(opline->op2.var EXECUTE_DATA_CC)",
    "CONST"    => "RT_CONSTANT(opline, opline->op2)",
    "UNUSED"   => "&EX(This)",
    "CV"       => "_get_zval_ptr_cv_\\1(opline->op2.var EXECUTE_DATA_CC)",
    "TMPVAR"   => "_get_zval_ptr_var(opline->op2.var EXECUTE_DATA_CC)",
    "TMPVARCV" => "???",
);

$op1_get_obj_zval_ptr_undef = array(
    "ANY"      => "get_obj_zval_ptr_undef(opline->op1_type, opline->op1, \\1)",
    "TMP"      => "_get_zval_ptr_tmp(opline->op1.var EXECUTE_DATA_CC)",
    "VAR"      => "_get_zval_ptr_var(opline->op1.var EXECUTE_DATA_CC)",
    "CONST"    => "RT_CONSTANT(opline, opline->op1)",
    "UNUSED"   => "&EX(This)",
    "CV"       => "EX_VAR(opline->op1.var)",
    "TMPVAR"   => "_get_zval_ptr_var(opline->op1.var EXECUTE_DATA_CC)",
    "TMPVARCV" => "EX_VAR(opline->op1.var)",
);

$op2_get_obj_zval_ptr_undef = array(
    "ANY"      => "get_obj_zval_ptr_undef(opline->op2_type, opline->op2, \\1)",
    "TMP"      => "_get_zval_ptr_tmp(opline->op2.var EXECUTE_DATA_CC)",
    "VAR"      => "_get_zval_ptr_var(opline->op2.var EXECUTE_DATA_CC)",
    "CONST"    => "RT_CONSTANT(opline, opline->op2)",
    "UNUSED"   => "&EX(This)",
    "CV"       => "EX_VAR(opline->op2.var)",
    "TMPVAR"   => "_get_zval_ptr_var(opline->op2.var EXECUTE_DATA_CC)",
    "TMPVARCV" => "EX_VAR(opline->op2.var)",
);

$op1_get_obj_zval_ptr_deref = array(
    "ANY"      => "get_obj_zval_ptr(opline->op1_type, opline->op1, \\1)",
    "TMP"      => "_get_zval_ptr_tmp(opline->op1.var EXECUTE_DATA_CC)",
    "VAR"      => "_get_zval_ptr_var_deref(opline->op1.var EXECUTE_DATA_CC)",
    "CONST"    => "RT_CONSTANT(opline, opline->op1)",
    "UNUSED"   => "&EX(This)",
    "CV"       => "_get_zval_ptr_cv_deref_\\1(opline->op1.var EXECUTE_DATA_CC)",
    "TMPVAR"   => "???",
    "TMPVARCV" => "???",
);

$op2_get_obj_zval_ptr_deref = array(
    "ANY"      => "get_obj_zval_ptr(opline->op2_type, opline->op2, \\1)",
    "TMP"      => "_get_zval_ptr_tmp(opline->op2.var EXECUTE_DATA_CC)",
    "VAR"      => "_get_zval_ptr_var_deref(opline->op2.var EXECUTE_DATA_CC)",
    "CONST"    => "RT_CONSTANT(opline, opline->op2)",
    "UNUSED"   => "&EX(This)",
    "CV"       => "_get_zval_ptr_cv_deref_\\1(opline->op2.var EXECUTE_DATA_CC)",
    "TMPVAR"   => "???",
    "TMPVARCV" => "???",
);

$op1_get_obj_zval_ptr_ptr = array(
    "ANY"      => "get_obj_zval_ptr_ptr(opline->op1_type, opline->op1, \\1)",
    "TMP"      => "zend_get_bad_ptr()",
    "VAR"      => "_get_zval_ptr_ptr_var(opline->op1.var EXECUTE_DATA_CC)",
    "CONST"    => "zend_get_bad_ptr()",
    "UNUSED"   => "&EX(This)",
    "CV"       => "_get_zval_ptr_cv_\\1(opline->op1.var EXECUTE_DATA_CC)",
    "TMPVAR"   => "???",
    "TMPVARCV" => "???",
);

$op2_get_obj_zval_ptr_ptr = array(
    "ANY"      => "get_obj_zval_ptr_ptr(opline->op2_type, opline->op2, \\1)",
    "TMP"      => "zend_get_bad_ptr()",
    "VAR"      => "_get_zval_ptr_ptr_var(opline->op2.var EXECUTE_DATA_CC)",
    "CONST"    => "zend_get_bad_ptr()",
    "UNUSED"   => "&EX(This)",
    "CV"       => "_get_zval_ptr_cv_\\1(opline->op2.var EXECUTE_DATA_CC)",
    "TMPVAR"   => "???",
    "TMPVARCV" => "???",
);

$op1_get_obj_zval_ptr_ptr_undef = array(
    "ANY"      => "get_obj_zval_ptr_ptr(opline->op1_type, opline->op1, \\1)",
    "TMP"      => "zend_get_bad_ptr()",
    "VAR"      => "_get_zval_ptr_ptr_var(opline->op1.var EXECUTE_DATA_CC)",
    "CONST"    => "zend_get_bad_ptr()",
    "UNUSED"   => "&EX(This)",
    "CV"       => "EX_VAR(opline->op1.var)",
    "TMPVAR"   => "???",
    "TMPVARCV" => "???",
);

$op2_get_obj_zval_ptr_ptr_undef = array(
    "ANY"      => "get_obj_zval_ptr_ptr(opline->op2_type, opline->op2, \\1)",
    "TMP"      => "zend_get_bad_ptr()",
    "VAR"      => "_get_zval_ptr_ptr_var(opline->op2.var EXECUTE_DATA_CC)",
    "CONST"    => "zend_get_bad_ptr()",
    "UNUSED"   => "&EX(This)",
    "CV"       => "EX_VAR(opline->op2.var)",
    "TMPVAR"   => "???",
    "TMPVARCV" => "???",
);

$op1_free_op = array(
    "ANY"      => "FREE_OP(opline->op1_type, opline->op1.var)",
    "TMP"      => "zval_ptr_dtor_nogc(EX_VAR(opline->op1.var))",
    "VAR"      => "zval_ptr_dtor_nogc(EX_VAR(opline->op1.var))",
    "CONST"    => "",
    "UNUSED"   => "",
    "CV"       => "",
    "TMPVAR"   => "zval_ptr_dtor_nogc(EX_VAR(opline->op1.var))",
    "TMPVARCV" => "FREE_OP(opline->op1_type, opline->op1.var)",
);

$op2_free_op = array(
    "ANY"      => "FREE_OP(opline->op2_type, opline->op2.var)",
    "TMP"      => "zval_ptr_dtor_nogc(EX_VAR(opline->op2.var))",
    "VAR"      => "zval_ptr_dtor_nogc(EX_VAR(opline->op2.var))",
    "CONST"    => "",
    "UNUSED"   => "",
    "CV"       => "",
    "TMPVAR"   => "zval_ptr_dtor_nogc(EX_VAR(opline->op2.var))",
    "TMPVARCV" => "FREE_OP(opline->op2_type, opline->op2.var)",
);

$op1_free_op_if_var = array(
    "ANY"      => "if (opline->op1_type == IS_VAR) {zval_ptr_dtor_nogc(EX_VAR(opline->op1.var));}",
    "TMP"      => "",
    "VAR"      => "zval_ptr_dtor_nogc(EX_VAR(opline->op1.var))",
    "CONST"    => "",
    "UNUSED"   => "",
    "CV"       => "",
    "TMPVAR"   => "???",
    "TMPVARCV" => "???",
);

$op2_free_op_if_var = array(
    "ANY"      => "if (opline->op2_type == IS_VAR) {zval_ptr_dtor_nogc(EX_VAR(opline->op2.var));}",
    "TMP"      => "",
    "VAR"      => "zval_ptr_dtor_nogc(EX_VAR(opline->op2.var))",
    "CONST"    => "",
    "UNUSED"   => "",
    "CV"       => "",
    "TMPVAR"   => "???",
    "TMPVARCV" => "???",
);

$op_data_type = array(
    "ANY"      => "(opline+1)->op1_type",
    "TMP"      => "IS_TMP_VAR",
    "VAR"      => "IS_VAR",
    "CONST"    => "IS_CONST",
    "UNUSED"   => "IS_UNUSED",
    "CV"       => "IS_CV",
    "TMPVAR"   => "(IS_TMP_VAR|IS_VAR)",
    "TMPVARCV" => "(IS_TMP_VAR|IS_VAR|IS_CV)",
);

$op_data_get_zval_ptr = array(
    "ANY"      => "get_op_data_zval_ptr_r((opline+1)->op1_type, (opline+1)->op1)",
    "TMP"      => "_get_zval_ptr_tmp((opline+1)->op1.var EXECUTE_DATA_CC)",
    "VAR"      => "_get_zval_ptr_var((opline+1)->op1.var EXECUTE_DATA_CC)",
    "CONST"    => "RT_CONSTANT((opline+1), (opline+1)->op1)",
    "UNUSED"   => "NULL",
    "CV"       => "_get_zval_ptr_cv_\\1((opline+1)->op1.var EXECUTE_DATA_CC)",
    "TMPVAR"   => "_get_zval_ptr_var((opline+1)->op1.var EXECUTE_DATA_CC)",
    "TMPVARCV" => "???",
);

$op_data_get_zval_ptr_undef = array(
    "ANY"      => "get_op_data_zval_ptr_undef((opline+1)->op1_type, (opline+1)->op1)",
    "TMP"      => "_get_zval_ptr_tmp((opline+1)->op1.var EXECUTE_DATA_CC)",
    "VAR"      => "_get_zval_ptr_var((opline+1)->op1.var EXECUTE_DATA_CC)",
    "CONST"    => "RT_CONSTANT((opline+1), (opline+1)->op1)",
    "UNUSED"   => "NULL",
    "CV"       => "EX_VAR((opline+1)->op1.var)",
    "TMPVAR"   => "_get_zval_ptr_var((opline+1)->op1.var EXECUTE_DATA_CC)",
    "TMPVARCV" => "EX_VAR((opline+1)->op1.var)",
);

$op_data_get_zval_ptr_deref = array(
    "ANY"      => "get_op_data_zval_ptr_deref_r((opline+1)->op1_type, (opline+1)->op1)",
    "TMP"      => "_get_zval_ptr_tmp((opline+1)->op1.var EXECUTE_DATA_CC)",
    "VAR"      => "_get_zval_ptr_var_deref((opline+1)->op1.var EXECUTE_DATA_CC)",
    "CONST"    => "RT_CONSTANT((opline+1), (opline+1)->op1)",
    "UNUSED"   => "NULL",
    "CV"       => "_get_zval_ptr_cv_deref_\\1((opline+1)->op1.var EXECUTE_DATA_CC)",
    "TMPVAR"   => "???",
    "TMPVARCV" => "???",
);

$op_data_get_zval_ptr_ptr = array(
    "ANY"      => "get_zval_ptr_ptr((opline+1)->op1_type, (opline+1)->op1, \\1)",
    "TMP"      => "zend_get_bad_ptr()",
    "VAR"      => "_get_zval_ptr_ptr_var((opline+1)->op1.var EXECUTE_DATA_CC)",
    "CONST"    => "zend_get_bad_ptr()",
    "UNUSED"   => "NULL",
    "CV"       => "_get_zval_ptr_cv_\\1((opline+1)->op1.var EXECUTE_DATA_CC)",
    "TMPVAR"   => "???",
    "TMPVARCV" => "???",
);

$op_data_free_op = array(
    "ANY"      => "FREE_OP((opline+1)->op1_type, (opline+1)->op1.var)",
    "TMP"      => "zval_ptr_dtor_nogc(EX_VAR((opline+1)->op1.var))",
    "VAR"      => "zval_ptr_dtor_nogc(EX_VAR((opline+1)->op1.var))",
    "CONST"    => "",
    "UNUSED"   => "",
    "CV"       => "",
    "TMPVAR"   => "zval_ptr_dtor_nogc(EX_VAR((opline+1)->op1.var))",
    "TMPVARCV" => "???",
);

$list    = array(); // list of opcode handlers and helpers in original order
$opcodes = array(); // opcode handlers by code
$helpers = array(); // opcode helpers by name
$params  = array(); // parameters of helpers
$opnames = array(); // opcode name to code mapping
$line_no = 1;

$used_extra_spec = array();

// Writes $s into resulting executor
function out($f, $s) {
    global $line_no;

    fputs($f,$s);
    $line_no += substr_count($s, "\n");
}

// Resets #line directives in resulting executor
function out_line($f) {
    global $line_no, $executor_file;

    fputs($f,"#line ".($line_no+1)." \"".$executor_file."\"\n");
    ++$line_no;
}

function is_hot_helper($name) {
    global $helpers;

    if (isset($helpers[$name]["hot"])) {
        return $helpers[$name]["hot"];
    }

    return false;
}

// Returns name of specialized helper
function helper_name($name, $spec, $op1, $op2, $extra_spec) {
    global $prefix, $helpers;

    $extra = "";

    if (isset($helpers[$name])) {
        // If we have no helper with specified specialized operands then
        // using unspecialized helper
        if (!isset($helpers[$name]["op1"][$op1])) {
            if (($op1 == 'TMP' || $op1 == 'VAR') &&
                isset($helpers[$name]["op1"]["TMPVAR"])) {
                $op1 = "TMPVAR";
            } else if (($op1 == 'TMP' || $op1 == 'VAR') &&
                isset($helpers[$name]["op1"]["TMPVARCV"])) {
                $op1 = "TMPVARCV";
            } else if ($op1 == 'CV' &&
                isset($helpers[$name]["op1"]["TMPVARCV"])) {
                $op1 = "TMPVARCV";
            } else if (isset($helpers[$name]["op1"]["ANY"])) {
                $op1 = "ANY";
            }
        }
        if (!isset($helpers[$name]["op2"][$op2])) {
            if (($op2 == 'TMP' || $op2 == 'VAR') &&
                isset($helpers[$name]["op2"]["TMPVAR"])) {
                $op2 = "TMPVAR";
            } else if (($op2 == 'TMP' || $op2 == 'VAR') &&
                isset($helpers[$name]["op2"]["TMPVARCV"])) {
                $op2 = "TMPVARCV";
            } else if ($op2 == 'CV' &&
                isset($helpers[$name]["op2"]["TMPVARCV"])) {
                $op2 = "TMPVARCV";
            } else if (isset($helpers[$name]["op2"]["ANY"])) {
                $op2 = "ANY";
            }
        }
        /* forward common specs (e.g. in ZEND_VM_DISPATCH_TO_HELPER) */
        if (isset($extra_spec, $helpers[$name]["spec"])) {
            $extra = extra_spec_name(array_intersect_key($extra_spec, $helpers[$name]["spec"]));
        }
    }

    return $name . ($spec ? "_SPEC" : "") . $prefix[$op1] . $prefix[$op2] . $extra;
}

function opcode_name($name, $spec, $op1, $op2, $extra_spec) {
    global $prefix, $opnames, $opcodes;

    $extra = "";

    if (isset($opnames[$name])) {
        $opcode = $opcodes[$opnames[$name]];
        // If we have no helper with specified specialized operands then
        // using unspecialized helper
        if (!isset($opcode["op1"][$op1])) {
            if (($op1 == 'TMP' || $op1 == 'VAR') &&
                isset($opcode["op1"]["TMPVAR"])) {
                $op1 = "TMPVAR";
            } else if (($op1 == 'TMP' || $op1 == 'VAR') &&
                isset($opcode["op1"]["TMPVARCV"])) {
                $op1 = "TMPVARCV";
            } else if ($op1 == 'CV' &&
                isset($opcode["op1"]["TMPVARCV"])) {
                $op1 = "TMPVARCV";
            } else if (isset($opcode["op1"]["ANY"])) {
                $op1 = "ANY";
            } else if ($spec) {
                /* dispatch to invalid handler from unreachable code */
                return "ZEND_NULL";
            }
        }
        if (!isset($opcode["op2"][$op2])) {
            if (($op2 == 'TMP' || $op2 == 'VAR') &&
                isset($opcode["op2"]["TMPVAR"])) {
                $op2 = "TMPVAR";
            } else if (($op2 == 'TMP' || $op2 == 'VAR') &&
                isset($opcode["op2"]["TMPVARCV"])) {
                $op2 = "TMPVARCV";
            } else if ($op2 == 'CV' &&
                isset($opcode["op2"]["TMPVARCV"])) {
                $op2 = "TMPVARCV";
            } else if (isset($opcode["op2"]["ANY"])) {
                $op2 = "ANY";
            } else if ($spec) {
                /* dispatch to unknown handler in unreachable code */
                return "ZEND_NULL";
            }
        }
        /* forward common specs (e.g. in ZEND_VM_DISPATCH_TO_HANDLER) */
        if (isset($extra_spec, $opcode["spec"])) {
            $extra = extra_spec_name(array_intersect_key($extra_spec, $opcode["spec"]));
        }
    }

    return $name . ($spec ? "_SPEC" : "") . $prefix[$op1] . $prefix[$op2] . $extra;
}

// Formats condition, protecting it by parentheses when needed.
function format_condition($condition) {
    if ($condition === "") {
        throw new InvalidArgumentException("A non empty string condition was expected.");
    }

    if ($condition[0] === "(" && substr($condition, -1) === ")") {
        return $condition;
    }

    return "(" . $condition . ")";
}

// Generates code for opcode handler or helper
function gen_code($f, $spec, $kind, $code, $op1, $op2, $name, $extra_spec=null) {
    global $op1_type, $op2_type, $op1_get_zval_ptr, $op2_get_zval_ptr,
        $op1_get_zval_ptr_deref, $op2_get_zval_ptr_deref,
        $op1_get_zval_ptr_undef, $op2_get_zval_ptr_undef,
        $op1_get_zval_ptr_ptr, $op2_get_zval_ptr_ptr,
        $op1_get_zval_ptr_ptr_undef, $op2_get_zval_ptr_ptr_undef,
        $op1_get_obj_zval_ptr, $op2_get_obj_zval_ptr,
        $op1_get_obj_zval_ptr_undef, $op2_get_obj_zval_ptr_undef,
        $op1_get_obj_zval_ptr_deref, $op2_get_obj_zval_ptr_deref,
        $op1_get_obj_zval_ptr_ptr, $op2_get_obj_zval_ptr_ptr,
        $op1_get_obj_zval_ptr_ptr_undef, $op2_get_obj_zval_ptr_ptr_undef,
        $op1_free_op, $op2_free_op, $op1_free_op_if_var, $op2_free_op_if_var,
        $prefix,
        $op_data_type, $op_data_get_zval_ptr, $op_data_get_zval_ptr_undef,
        $op_data_get_zval_ptr_deref, $op_data_get_zval_ptr_ptr,
        $op_data_free_op;

    // Specializing
    $specialized_replacements = array(
        "/OP1_TYPE/" => $op1_type[$op1],
        "/OP2_TYPE/" => $op2_type[$op2],
        "/GET_OP1_ZVAL_PTR\(([^)]*)\)/" => $op1_get_zval_ptr[$op1],
        "/GET_OP2_ZVAL_PTR\(([^)]*)\)/" => $op2_get_zval_ptr[$op2],
        "/GET_OP1_ZVAL_PTR_DEREF\(([^)]*)\)/" => $op1_get_zval_ptr_deref[$op1],
        "/GET_OP2_ZVAL_PTR_DEREF\(([^)]*)\)/" => $op2_get_zval_ptr_deref[$op2],
        "/GET_OP1_ZVAL_PTR_UNDEF\(([^)]*)\)/" => $op1_get_zval_ptr_undef[$op1],
        "/GET_OP2_ZVAL_PTR_UNDEF\(([^)]*)\)/" => $op2_get_zval_ptr_undef[$op2],
        "/GET_OP1_ZVAL_PTR_PTR\(([^)]*)\)/" => $op1_get_zval_ptr_ptr[$op1],
        "/GET_OP2_ZVAL_PTR_PTR\(([^)]*)\)/" => $op2_get_zval_ptr_ptr[$op2],
        "/GET_OP1_ZVAL_PTR_PTR_UNDEF\(([^)]*)\)/" => $op1_get_zval_ptr_ptr_undef[$op1],
        "/GET_OP2_ZVAL_PTR_PTR_UNDEF\(([^)]*)\)/" => $op2_get_zval_ptr_ptr_undef[$op2],
        "/GET_OP1_OBJ_ZVAL_PTR\(([^)]*)\)/" => $op1_get_obj_zval_ptr[$op1],
        "/GET_OP2_OBJ_ZVAL_PTR\(([^)]*)\)/" => $op2_get_obj_zval_ptr[$op2],
        "/GET_OP1_OBJ_ZVAL_PTR_UNDEF\(([^)]*)\)/" => $op1_get_obj_zval_ptr_undef[$op1],
        "/GET_OP2_OBJ_ZVAL_PTR_UNDEF\(([^)]*)\)/" => $op2_get_obj_zval_ptr_undef[$op2],
        "/GET_OP1_OBJ_ZVAL_PTR_DEREF\(([^)]*)\)/" => $op1_get_obj_zval_ptr_deref[$op1],
        "/GET_OP2_OBJ_ZVAL_PTR_DEREF\(([^)]*)\)/" => $op2_get_obj_zval_ptr_deref[$op2],
        "/GET_OP1_OBJ_ZVAL_PTR_PTR\(([^)]*)\)/" => $op1_get_obj_zval_ptr_ptr[$op1],
        "/GET_OP2_OBJ_ZVAL_PTR_PTR\(([^)]*)\)/" => $op2_get_obj_zval_ptr_ptr[$op2],
        "/GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF\(([^)]*)\)/" => $op1_get_obj_zval_ptr_ptr_undef[$op1],
        "/GET_OP2_OBJ_ZVAL_PTR_PTR_UNDEF\(([^)]*)\)/" => $op2_get_obj_zval_ptr_ptr_undef[$op2],
        "/FREE_OP1\(\)/" => $op1_free_op[$op1],
        "/FREE_OP2\(\)/" => $op2_free_op[$op2],
        "/FREE_OP1_IF_VAR\(\)/" => $op1_free_op_if_var[$op1],
        "/FREE_OP2_IF_VAR\(\)/" => $op2_free_op_if_var[$op2],
        "/\!ZEND_VM_SPEC/m" => ($op1!="ANY"||$op2!="ANY")?"0":"1",
        "/ZEND_VM_SPEC/m" => ($op1!="ANY"||$op2!="ANY")?"1":"0",
        "/ZEND_VM_C_LABEL\(\s*([A-Za-z_]*)\s*\)/m" => "\\1".(($spec && $kind != ZEND_VM_KIND_CALL)?("_SPEC".$prefix[$op1].$prefix[$op2].extra_spec_name($extra_spec)):""),
        "/ZEND_VM_C_GOTO\(\s*([A-Za-z_]*)\s*\)/m" => "goto \\1".(($spec && $kind != ZEND_VM_KIND_CALL)?("_SPEC".$prefix[$op1].$prefix[$op2].extra_spec_name($extra_spec)):""),
        "/^#(\s*)if\s+1\s*\\|\\|.*[^\\\\]$/m" => "#\\1if 1",
        "/^#(\s*)if\s+0\s*&&.*[^\\\\]$/m" => "#\\1if 0",
        "/^#(\s*)elif\s+1\s*\\|\\|.*[^\\\\]$/m" => "#\\1elif 1",
        "/^#(\s*)elif\s+0\s*&&.*[^\\\\]$/m" => "#\\1elif 0",
        "/OP_DATA_TYPE/" => $op_data_type[isset($extra_spec['OP_DATA']) ? $extra_spec['OP_DATA'] : "ANY"],
        "/GET_OP_DATA_ZVAL_PTR\(([^)]*)\)/" => $op_data_get_zval_ptr[isset($extra_spec['OP_DATA']) ? $extra_spec['OP_DATA'] : "ANY"],
        "/GET_OP_DATA_ZVAL_PTR_UNDEF\(([^)]*)\)/" => $op_data_get_zval_ptr_undef[isset($extra_spec['OP_DATA']) ? $extra_spec['OP_DATA'] : "ANY"],
        "/GET_OP_DATA_ZVAL_PTR_DEREF\(([^)]*)\)/" => $op_data_get_zval_ptr_deref[isset($extra_spec['OP_DATA']) ? $extra_spec['OP_DATA'] : "ANY"],
        "/GET_OP_DATA_ZVAL_PTR_PTR\(([^)]*)\)/" => $op_data_get_zval_ptr_ptr[isset($extra_spec['OP_DATA']) ? $extra_spec['OP_DATA'] : "ANY"],
        "/FREE_OP_DATA\(\)/" => $op_data_free_op[isset($extra_spec['OP_DATA']) ? $extra_spec['OP_DATA'] : "ANY"],
        "/RETURN_VALUE_USED\(opline\)/" => isset($extra_spec['RETVAL']) ? $extra_spec['RETVAL'] : "RETURN_VALUE_USED(opline)",
        "/arg_num <= MAX_ARG_FLAG_NUM/" => isset($extra_spec['QUICK_ARG']) ? $extra_spec['QUICK_ARG'] : "arg_num <= MAX_ARG_FLAG_NUM",
        "/ZEND_VM_SMART_BRANCH\(\s*([^,)]*)\s*,\s*([^)]*)\s*\)/" => isset($extra_spec['SMART_BRANCH']) ?
            ($extra_spec['SMART_BRANCH'] == 1 ?
                    "ZEND_VM_SMART_BRANCH_JMPZ(\\1, \\2)"
                :	($extra_spec['SMART_BRANCH'] == 2 ?
                        "ZEND_VM_SMART_BRANCH_JMPNZ(\\1, \\2)" : "ZEND_VM_SMART_BRANCH_NONE(\\1, \\2)"))
            :	"ZEND_VM_SMART_BRANCH(\\1, \\2)",
        "/ZEND_VM_SMART_BRANCH_TRUE\(\s*\)/" => isset($extra_spec['SMART_BRANCH']) ?
            ($extra_spec['SMART_BRANCH'] == 1 ?
                    "ZEND_VM_SMART_BRANCH_TRUE_JMPZ()"
                :	($extra_spec['SMART_BRANCH'] == 2 ?
                        "ZEND_VM_SMART_BRANCH_TRUE_JMPNZ()" : "ZEND_VM_SMART_BRANCH_TRUE_NONE()"))
            :	"ZEND_VM_SMART_BRANCH_TRUE()",
        "/ZEND_VM_SMART_BRANCH_FALSE\(\s*\)/" => isset($extra_spec['SMART_BRANCH']) ?
            ($extra_spec['SMART_BRANCH'] == 1 ?
                    "ZEND_VM_SMART_BRANCH_FALSE_JMPZ()"
                :	($extra_spec['SMART_BRANCH'] == 2 ?
                        "ZEND_VM_SMART_BRANCH_FALSE_JMPNZ()" : "ZEND_VM_SMART_BRANCH_FALSE_NONE()"))
            :	"ZEND_VM_SMART_BRANCH_FALSE()",
        "/opline->extended_value\s*&\s*ZEND_ISEMPTY/" => isset($extra_spec['ISSET']) ?
            ($extra_spec['ISSET'] == 0 ? "0" : "1")
            : "\\0",
        "/opline->extended_value\s*&\s*~\s*ZEND_ISEMPTY/" => isset($extra_spec['ISSET']) ?
            ($extra_spec['ISSET'] == 0 ? "\\0" : "opline->extended_value")
            : "\\0",
        "/ZEND_OBSERVER_ENABLED/" => isset($extra_spec['OBSERVER']) && $extra_spec['OBSERVER'] == 1 ? "1" : "0",
        "/ZEND_OBSERVER_USE_RETVAL/" => isset($extra_spec['OBSERVER']) && $extra_spec['OBSERVER'] == 1 ? "zval observer_retval" : "",
        "/ZEND_OBSERVER_SET_RETVAL\(\)/" => isset($extra_spec['OBSERVER']) && $extra_spec['OBSERVER'] == 1 ? "if (!return_value) { return_value = &observer_retval; }" : "",
        "/ZEND_OBSERVER_FREE_RETVAL\(\)/" => isset($extra_spec['OBSERVER']) && $extra_spec['OBSERVER'] == 1 ? "if (return_value == &observer_retval) { zval_ptr_dtor_nogc(&observer_retval); }" : "",
        "/ZEND_OBSERVER_SAVE_OPLINE\(\)/" => isset($extra_spec['OBSERVER']) && $extra_spec['OBSERVER'] == 1 ? "SAVE_OPLINE()" : "",
        "/ZEND_OBSERVER_FCALL_BEGIN\(\s*(.*)\s*\)/" => isset($extra_spec['OBSERVER']) ?
            ($extra_spec['OBSERVER'] == 0 ? "" : "zend_observer_fcall_begin(\\1)")
            : "",
        "/ZEND_OBSERVER_FCALL_END\(\s*([^,]*)\s*,\s*(.*)\s*\)/" => isset($extra_spec['OBSERVER']) ?
            ($extra_spec['OBSERVER'] == 0 ? "" : "zend_observer_fcall_end(\\1, \\2)")
            : "",
    );
    $code = preg_replace(array_keys($specialized_replacements), array_values($specialized_replacements), $code);

    if (0 && strpos($code, '{') === 0) {
        $code = "{\n\tfprintf(stderr, \"$name\\n\");\n" . substr($code, 1);
    }
    // Updating code according to selected threading model
    switch ($kind) {
        case ZEND_VM_KIND_HYBRID:
            $code = preg_replace_callback(
                array(
                    "/EXECUTE_DATA(?=[^_])/m",
                    "/ZEND_VM_DISPATCH_TO_HANDLER\(\s*([A-Z_]*)\s*\)/m",
                    "/ZEND_VM_DISPATCH_TO_HELPER\(\s*([A-Za-z_]*)\s*(,[^)]*)?\)/m",
                ),
                function($matches) use ($spec, $prefix, $op1, $op2, $extra_spec) {
                    if (strncasecmp($matches[0], "EXECUTE_DATA", strlen("EXECUTE_DATA")) == 0) {
                        return "execute_data";
                    } else if (strncasecmp($matches[0], "ZEND_VM_DISPATCH_TO_HANDLER", strlen("ZEND_VM_DISPATCH_TO_HANDLER")) == 0) {
                        global $opcodes, $opnames;

                        $name = $matches[1];
                        $opcode = $opcodes[$opnames[$name]];
                        return "goto " . opcode_name($name, $spec, $op1, $op2, $extra_spec) . "_LABEL";
                    } else {
                        // ZEND_VM_DISPATCH_TO_HELPER
                        if (is_hot_helper($matches[1])) {
                            if (isset($matches[2])) {
                                // extra args
                                $args = preg_replace("/,\s*([A-Za-z0-9_]*)\s*,\s*([^,)\s]*)\s*/", "$1 = $2; ", $matches[2]);
                                return $args . "goto " . helper_name($matches[1], $spec, $op1, $op2, $extra_spec) . "_LABEL";
                            }
                            return "goto " . helper_name($matches[1], $spec, $op1, $op2, $extra_spec) . "_LABEL";
                        }
                        if (isset($matches[2])) {
                            // extra args
                            $args = substr(preg_replace("/,\s*[A-Za-z0-9_]*\s*,\s*([^,)\s]*)\s*/", ", $1", $matches[2]), 2);
                            return "ZEND_VM_TAIL_CALL(" . helper_name($matches[1], $spec, $op1, $op2, $extra_spec) . "(" . $args. " ZEND_OPCODE_HANDLER_ARGS_PASSTHRU_CC))";
                        }
                        return "ZEND_VM_TAIL_CALL(" . helper_name($matches[1], $spec, $op1, $op2, $extra_spec) . "(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU))";
                    }
                },
                $code);
            break;
        case ZEND_VM_KIND_CALL:
            $code = preg_replace_callback(
                array(
                    "/EXECUTE_DATA(?=[^_])/m",
                    "/ZEND_VM_DISPATCH_TO_HANDLER\(\s*([A-Z_]*)\s*\)/m",
                    "/ZEND_VM_DISPATCH_TO_HELPER\(\s*([A-Za-z_]*)\s*(,[^)]*)?\)/m",
                ),
                function($matches) use ($spec, $prefix, $op1, $op2, $extra_spec, $name) {
                    if (strncasecmp($matches[0], "EXECUTE_DATA", strlen("EXECUTE_DATA")) == 0) {
                        return "execute_data";
                    } else if (strncasecmp($matches[0], "ZEND_VM_DISPATCH_TO_HANDLER", strlen("ZEND_VM_DISPATCH_TO_HANDLER")) == 0) {
                        global $opcodes, $opnames;

                        $handler = $matches[1];
                        $opcode = $opcodes[$opnames[$handler]];
                        $inline =
                            ZEND_VM_KIND == ZEND_VM_KIND_HYBRID &&
                            isset($opcode["use"]) &&
                            is_hot_handler($opcode["hot"], $op1, $op2, $extra_spec) &&
                            is_hot_handler($opcodes[$opnames[$name]]["hot"], $op1, $op2, $extra_spec) ?
                            "_INLINE" : "";
                        return "ZEND_VM_TAIL_CALL(" . opcode_name($handler, $spec, $op1, $op2, $extra_spec) . $inline . "_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU))";
                    } else {
                        // ZEND_VM_DISPATCH_TO_HELPER
                        if (isset($matches[2])) {
                            // extra args
                            $args = substr(preg_replace("/,\s*[A-Za-z0-9_]*\s*,\s*([^,)\s]*)\s*/", ", $1", $matches[2]), 2);
                            return "ZEND_VM_TAIL_CALL(" . helper_name($matches[1], $spec, $op1, $op2, $extra_spec) . "(" . $args. " ZEND_OPCODE_HANDLER_ARGS_PASSTHRU_CC))";
                        }
                        return "ZEND_VM_TAIL_CALL(" . helper_name($matches[1], $spec, $op1, $op2, $extra_spec) . "(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU))";
                    }
                },
                $code);
            break;
        case ZEND_VM_KIND_SWITCH:
            $code = preg_replace_callback(
                array(
                    "/EXECUTE_DATA(?=[^_])/m",
                    "/ZEND_VM_DISPATCH_TO_HANDLER\(\s*([A-Z_]*)\s*\)/m",
                    "/ZEND_VM_DISPATCH_TO_HELPER\(\s*([A-Za-z_]*)\s*(,[^)]*)?\)/m",
                ),
                function($matches) use ($spec, $prefix, $op1, $op2, $extra_spec) {
                    if (strncasecmp($matches[0], "EXECUTE_DATA", strlen("EXECUTE_DATA")) == 0) {
                        return "execute_data";
                    } else if (strncasecmp($matches[0], "ZEND_VM_DISPATCH_TO_HANDLER", strlen("ZEND_VM_DISPATCH_TO_HANDLER")) == 0) {
                        return "goto " . opcode_name($matches[1], $spec, $op1, $op2, $extra_spec) . "_LABEL";
                    } else {
                        // ZEND_VM_DISPATCH_TO_HELPER
                        if (isset($matches[2])) {
                            // extra args
                            $args = preg_replace("/,\s*([A-Za-z0-9_]*)\s*,\s*([^,)\s]*)\s*/", "$1 = $2; ", $matches[2]);
                            return $args .  "goto " . helper_name($matches[1], $spec, $op1, $op2, $extra_spec);
                        }
                        return "goto " . helper_name($matches[1], $spec, $op1, $op2, $extra_spec);
                    }
                },
                    $code);
            break;
        case ZEND_VM_KIND_GOTO:
            $code = preg_replace_callback(
                array(
                    "/EXECUTE_DATA(?=[^_])/m",
                    "/ZEND_VM_DISPATCH_TO_HANDLER\(\s*([A-Z_]*)\s*\)/m",
                    "/ZEND_VM_DISPATCH_TO_HELPER\(\s*([A-Za-z_]*)\s*(,[^)]*)?\)/m",
                ),
                function($matches) use ($spec, $prefix, $op1, $op2, $extra_spec) {
                    if (strncasecmp($matches[0], "EXECUTE_DATA", strlen("EXECUTE_DATA")) == 0) {
                        return "execute_data";
                    } else if (strncasecmp($matches[0], "ZEND_VM_DISPATCH_TO_HANDLER", strlen("ZEND_VM_DISPATCH_TO_HANDLER")) == 0) {
                        return "goto " . opcode_name($matches[1], $spec, $op1, $op2, $extra_spec) . "_LABEL";
                    } else {
                        // ZEND_VM_DISPATCH_TO_HELPER
                        if (isset($matches[2])) {
                            // extra args
                            $args = preg_replace("/,\s*([A-Za-z0-9_]*)\s*,\s*([^,)\s]*)\s*/", "$1 = $2; ", $matches[2]);
                            return $args .  "goto " . helper_name($matches[1], $spec, $op1, $op2, $extra_spec);
                        }
                        return "goto " . helper_name($matches[1], $spec, $op1, $op2, $extra_spec);
                    }
                },
                    $code);
            break;
    }

    /* Remove unnecessary ';' */
    $code = preg_replace('/^\s*;\s*$/m', '', $code);

    /* Remove WS */
    $code = preg_replace('/[ \t]+\n/m', "\n", $code);

    out($f, $code);
}

function skip_extra_spec_function($op1, $op2, $extra_spec) {
    global $commutative_order;

    if (isset($extra_spec["NO_CONST_CONST"]) &&
        $op1 == "CONST" && $op2 == "CONST") {
        // Skip useless constant handlers
        return true;
    }

    if (isset($extra_spec["COMMUTATIVE"]) &&
        $commutative_order[$op1] < $commutative_order[$op2]) {
        // Skip duplicate commutative handlers
        return true;
    }

    return false;
}

function is_hot_handler($hot, $op1, $op2, $extra_spec) {
    if (isset($extra_spec["SMART_BRANCH"]) && $extra_spec["SMART_BRANCH"] == 0) {
        return false;
    }
    if (isset($extra_spec["OBSERVER"]) && $extra_spec["OBSERVER"] == 1) {
        return false;
    }
    if ($hot === 'HOT_' || $hot === 'INLINE_') {
        return true;
    } else if ($hot === 'HOT_NOCONST_') {
        return ($op1 !== 'CONST');
    } else if ($hot === 'HOT_NOCONSTCONST_') {
        return (($op1 !== 'CONST') || ($op2 !== 'CONST')) ;
    } else if ($hot === 'HOT_OBJ_') {
        return (($op1 === 'UNUSED') || ($op1 === 'CV')) && ($op2 === 'CONST');
    } else if ($hot === 'HOT_SEND_') {
        return !empty($extra_spec["QUICK_ARG"]);
    } else {
        return false;
    }
}

function is_cold_handler($hot, $op1, $op2, $extra_spec) {
    if ($hot === 'COLD_') {
        return true;
    } else if (isset($extra_spec["OBSERVER"]) && $extra_spec["OBSERVER"] == 1) {
        return true;
    } else if ($hot === 'COLD_CONST_') {
        return ($op1 === 'CONST');
    } else if ($hot === 'COLD_CONSTCONST_') {
        return ($op1 === 'CONST' && $op2 === 'CONST');
    } else if ($hot === 'HOT_OBJ_') {
        return ($op1 === 'CONST');
    } else if ($hot === 'HOT_NOCONST_') {
        return ($op1 === 'CONST');
    } else if ($hot === 'HOT_NOCONSTCONST_') {
        return ($op1 === 'CONST' && $op2 === 'CONST');
    } else {
        return false;
    }
}

function is_inline_hybrid_handler($name, $hot, $op1, $op2, $extra_spec) {
    return ($hot === 'INLINE_');
}

// Generates opcode handler
function gen_handler($f, $spec, $kind, $name, $op1, $op2, $use, $code, $lineno, $opcode, $extra_spec = null, &$switch_labels = array()) {
    global $definition_file, $prefix, $opnames, $gen_order;

    static $used_observer_handlers = array();

    if (isset($opcode['alias']) && ($spec || $kind != ZEND_VM_KIND_SWITCH)) {
        return;
    }

    if ($spec && skip_extra_spec_function($op1, $op2, $extra_spec)) {
        return;
    }

    /* Skip SMART_BRANCH specialization for "cold" CONST_CONST instructions */
    if (isset($extra_spec["SMART_BRANCH"])) {
        if ($opcode["hot"] === 'HOT_NOCONSTCONST_'
         || $opcode["hot"] === 'COLD_CONSTCONST_') {
            if (($op1 === 'CONST') && ($op2 === 'CONST')) {
                if ($extra_spec["SMART_BRANCH"] == 0) {
                    unset($extra_spec["SMART_BRANCH"]);
                } else {
                    return;
                }
            }
        }
    }

    /* Skip QUICK_ARG specialization for named parameters */
    if (isset($extra_spec["QUICK_ARG"])) {
        if ($op2 === "CONST") {
            if ($extra_spec["QUICK_ARG"] == 0) {
                unset($extra_spec["QUICK_ARG"]);
            } else {
                return;
            }
        }
    }

    /* Skip all specialization for OBSERVER handlers */
    if (isset($extra_spec["OBSERVER"]) && $extra_spec["OBSERVER"] == 1) {
        if (isset($extra_spec["RETVAL"])) {
            if ($extra_spec["RETVAL"] == 0) {
                unset($extra_spec["RETVAL"]);
            } else {
                return;
            }
        }
        if ($op1 != "ANY" || $op2 != "ANY") {
            if (!isset($used_observer_handlers[$kind][$opcode["op"]])) {
                $used_observer_handlers[$kind][$opcode["op"]] = true;
                $op1 = "ANY";
                $op2 = "ANY";
            } else {
                return;
            }
        }
    }

    if (ZEND_VM_LINES) {
        out($f, "#line $lineno \"$definition_file\"\n");
    }

    // Generate opcode handler's entry point according to selected threading model
    $additional_func = false;
    $spec_name = $name.($spec?"_SPEC":"").$prefix[$op1].$prefix[$op2].($spec?extra_spec_name($extra_spec):"");
    switch ($kind) {
        case ZEND_VM_KIND_HYBRID:
            if (is_inline_hybrid_handler($name, $opcode["hot"], $op1, $op2, $extra_spec)) {
                $out = fopen('php://memory', 'w+');
                gen_code($out, $spec, $kind, $code, $op1, $op2, $name, $extra_spec);
                rewind($out);
                $code =
                      "\t\t\tHYBRID_CASE({$spec_name}):\n"
                    . "\t\t\t\tVM_TRACE($spec_name)\n"
                    . stream_get_contents($out)
                    . "\t\t\t\tVM_TRACE_OP_END($spec_name)\n";
                fclose($out);
            } else {
                $inline =
                    isset($opcode["use"]) &&
                    is_hot_handler($opcode["hot"], $op1, $op2, $extra_spec) ?
                    "_INLINE" : "";
                $code =
                      "\t\t\tHYBRID_CASE({$spec_name}):\n"
                    . "\t\t\t\tVM_TRACE($spec_name)\n"
                    . "\t\t\t\t{$spec_name}{$inline}_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);\n"
                    . "\t\t\t\tVM_TRACE_OP_END($spec_name)\n"
                    . "\t\t\t\tHYBRID_BREAK();\n";
            }
            if (is_array($gen_order)) {
                $gen_order[$spec_name] = $code;
            } else {
                out($f, $code);
            }
            return;
        case ZEND_VM_KIND_CALL:
            if ($opcode["hot"] && ZEND_VM_KIND == ZEND_VM_KIND_HYBRID && is_hot_handler($opcode["hot"], $op1, $op2, $extra_spec)) {
                if (isset($opcode["use"])) {
                    out($f,"static zend_always_inline ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL {$spec_name}_INLINE_HANDLER(ZEND_OPCODE_HANDLER_ARGS)\n");
                    $additional_func = true;
                } else {
                    out($f,"static ZEND_VM_HOT ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL {$spec_name}_HANDLER(ZEND_OPCODE_HANDLER_ARGS)\n");
                }
            } else if ($opcode["hot"] && is_cold_handler($opcode["hot"], $op1, $op2, $extra_spec)) {
                out($f,"static ZEND_VM_COLD ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL {$spec_name}_HANDLER(ZEND_OPCODE_HANDLER_ARGS)\n");
            } else {
                out($f,"static ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL {$spec_name}_HANDLER(ZEND_OPCODE_HANDLER_ARGS)\n");
            }
            break;
        case ZEND_VM_KIND_SWITCH:
            if ($spec) {
                $cur = $switch_labels ? end($switch_labels) + 1 : 0;
                out($f,"case $cur: /* $spec_name */");
                $switch_labels[$spec_name] = $cur;
            } else {
                out($f,"case ".$name.":");
            }
            if ($use) {
                // This handler is used by other handlers. We will add label to call it.
                out($f," {$spec_name}_LABEL: ZEND_ATTRIBUTE_UNUSED_LABEL\n");
            } else {
                out($f,"\n");
            }
            break;
        case ZEND_VM_KIND_GOTO:
            out($f,"{$spec_name}_LABEL: ZEND_VM_GUARD($spec_name);\n");
            break;
    }

    // Generate opcode handler's code
    gen_code($f, $spec, $kind, $code, $op1, $op2, $name, $extra_spec);

    if ($additional_func) {
        out($f,"static ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL {$spec_name}_HANDLER(ZEND_OPCODE_HANDLER_ARGS)\n");
        out($f,"{\n");
        out($f,"\tZEND_VM_TAIL_CALL({$spec_name}_INLINE_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU));\n");
        out($f,"}\n");
        out($f,"\n");
    }
}

// Generates helper
function gen_helper($f, $spec, $kind, $name, $op1, $op2, $param, $code, $lineno, $inline, $cold = false, $hot = false, $extra_spec = null) {
    global $definition_file, $prefix;

    if ($kind == ZEND_VM_KIND_HYBRID && !$hot) {
        return;
    }

    if ($spec && skip_extra_spec_function($op1, $op2, $extra_spec)) {
        return;
    }

    if (ZEND_VM_LINES) {
        out($f, "#line $lineno \"$definition_file\"\n");
    }

    $spec_name = $name.($spec?"_SPEC":"").$prefix[$op1].$prefix[$op2].($spec?extra_spec_name($extra_spec):"");

    // Generate helper's entry point according to selected threading model
    switch ($kind) {
        case ZEND_VM_KIND_HYBRID:
            out($f, $spec_name . "_LABEL:\n");
            break;
        case ZEND_VM_KIND_CALL:
            if ($inline) {
                $zend_attributes = " zend_always_inline";
                $zend_fastcall = "";
            } else {
                if ($cold) {
                    $zend_attributes = " zend_never_inline ZEND_COLD";
                } else {
                    $zend_attributes = " zend_never_inline";
                }
                $zend_fastcall = " ZEND_FASTCALL";
            }
            if ($param == null) {
              // Helper without parameters
                out($f, "static$zend_attributes ZEND_OPCODE_HANDLER_RET$zend_fastcall $spec_name(ZEND_OPCODE_HANDLER_ARGS)\n");
            } else {
              // Helper with parameter
                out($f, "static$zend_attributes ZEND_OPCODE_HANDLER_RET$zend_fastcall $spec_name($param ZEND_OPCODE_HANDLER_ARGS_DC)\n");
            }
            break;
        case ZEND_VM_KIND_SWITCH:
            out($f, "$spec_name:\n");
            break;
        case ZEND_VM_KIND_GOTO:
            out($f, "$spec_name:\n");
            break;
    }

    // Generate helper's code
    gen_code($f, $spec, $kind, $code, $op1, $op2, $name, $extra_spec);
}


function gen_null_label($f, $kind, $prolog) {
    switch ($kind) {
        case ZEND_VM_KIND_CALL:
            out($f,$prolog."ZEND_NULL_HANDLER,\n");
            break;
        case ZEND_VM_KIND_SWITCH:
            out($f,$prolog."(void*)(uintptr_t)-1,\n");
            break;
        case ZEND_VM_KIND_GOTO:
            out($f,$prolog."(void*)&&ZEND_NULL_LABEL,\n");
            break;
    }
}

// Generates array of opcode handlers (specialized or unspecialized)
function gen_labels($f, $spec, $kind, $prolog, &$specs, $switch_labels = array()) {
    global $opcodes, $opnames, $op_types, $prefix, $op_types_ex;

    $list = [];
    $next = 0;
    $label = 0;
    if ($spec) {
      // Emit labels for specialized executor

      // For each opcode in opcode number order
        foreach ($opcodes as $num => $dsc) {
            if (isset($dsc['alias'])) {
                $specs[$num] = $specs[$opnames[$dsc['alias']]];
                continue;
            }
            $specs[$num] = "$label";
            $spec_op1 = $spec_op2 = $spec_extra = false;
            $def_op1_type = $def_op2_type = "ANY";
            $next = $num + 1;
            if (isset($dsc["op1"]) && !isset($dsc["op1"]["ANY"])) {
                $count = 0;
                foreach ($op_types_ex as $t) {
                    if (isset($dsc["op1"][$t])) {
                        $def_op1_type = $t;
                        $count++;
                    }
                }
                if ($count > 1) {
                    $spec_op1 = true;
                    $specs[$num] .= " | SPEC_RULE_OP1";
                    $def_op1_type = "ANY";
                }
            }
            if (isset($dsc["op2"]) && !isset($dsc["op2"]["ANY"])) {
                $count = 0;
                foreach ($op_types_ex as $t) {
                    if (isset($dsc["op2"][$t])) {
                        $def_op2_type = $t;
                        $count++;
                    }
                }
                if ($count > 1) {
                    $spec_op2 = true;
                    $specs[$num] .= " | SPEC_RULE_OP2";
                    $def_op2_type = "ANY";
                }
            }
            $spec_extra = call_user_func_array("array_merge", extra_spec_handler($dsc) ?: array(array()));
            $flags = extra_spec_flags($spec_extra);
            if ($flags) {
                $specs[$num] .= " | " . implode(" | ", $flags);
            }
            if ($num >= 256) {
                $opcodes[$num]['spec_code'] = $specs[$num];
                unset($specs[$num]);
            }

            $foreach_op1 = function($do) use ($dsc, $op_types) {
                return function($_, $op2) use ($do, $dsc, $op_types) {
                    // For each op1.op_type except ANY
                    foreach ($op_types as $op1) {
                        if ($op1 != "ANY") {
                            if (!isset($dsc["op1"][$op1])) {
                                if ($op1 == "TMP" || $op1 == "VAR") {
                                    if (isset($dsc["op1"]["TMPVAR"])) {
                                        $op1 = "TMPVAR";
                                    } else if (isset($dsc["op1"]["TMPVARCV"])) {
                                        $op1 = "TMPVARCV";
                                    } else {
                                        $op1 = "ANY";
                                    }
                                } else if ($op1 == "CV" && isset($dsc["op1"]["TMPVARCV"])) {
                                    $op1 = "TMPVARCV";
                                } else {
                                    // Try to use unspecialized handler
                                    $op1 = "ANY";
                                }
                            }
                            $do($op1, $op2);
                        }
                    }
                };
            };
            $foreach_op2 = function($do) use ($dsc, $op_types) {
                return function($op1, $_) use ($do, $dsc, $op_types) {
                    // For each op2.op_type except ANY
                    foreach ($op_types as $op2) {
                        if ($op2 != "ANY") {
                            if (!isset($dsc["op2"][$op2])) {
                                if ($op2 == "TMP" || $op2 == "VAR") {
                                    if (isset($dsc["op2"]["TMPVAR"])) {
                                        $op2 = "TMPVAR";
                                    } else if (isset($dsc["op2"]["TMPVARCV"])) {
                                        $op2 = "TMPVARCV";
                                    } else {
                                        $op2 = "ANY";
                                    }
                                } else if ($op2 == "CV" && isset($dsc["op2"]["TMPVARCV"])) {
                                    $op2 = "TMPVARCV";
                                } else {
                                    // Try to use unspecialized handler
                                    $op2 = "ANY";
                                }
                            }
                            $do($op1, $op2);
                        }
                    }
                };
            };
            $foreach_op_data = function($do) use ($dsc, $op_types) {
                return function($op1, $op2, $extra_spec = array()) use ($do, $dsc, $op_types) {
                    // For each op_data.op_type except ANY
                    foreach ($op_types as $op_data) {
                        if ($op_data != "ANY") {
                            if (!isset($dsc["spec"]["OP_DATA"][$op_data])) {
                                if ($op_data == "TMP" || $op_data == "VAR") {
                                    if (isset($dsc["spec"]["OP_DATA"]["TMPVAR"])) {
                                        $op_data = "TMPVAR";
                                    } else if (isset($dsc["spec"]["OP_DATA"]["TMPVARCV"])) {
                                        $op_data = "TMPVARCV";
                                    } else {
                                        // Try to use unspecialized handler
                                        $op_data = "ANY";
                                    }
                                } else if ($op_data == "CV" && isset($dsc["OP_DATA"]["TMPVARCV"])) {
                                    $op_data = "TMPVARCV";
                                } else {
                                    // Try to use unspecialized handler
                                    $op_data = "ANY";
                                }
                            }
                            $do($op1, $op2, array("OP_DATA" => $op_data) + $extra_spec);
                        }
                    }
                };
            };
            $foreach_extra_spec = function($do, $spec) use ($dsc) {
                return function($op1, $op2, $extra_spec = array()) use ($do, $spec, $dsc) {
                    foreach ($dsc["spec"][$spec] as $val) {
                        $do($op1, $op2, array($spec => $val) + $extra_spec);
                    }
                };
            };
            $generate = function ($op1, $op2, $extra_spec = array()) use ($f, $kind, $dsc, $prefix, $prolog, $num, $switch_labels, &$label, &$list) {
                global $commutative_order;

                // Check if specialized handler is defined
                /* TODO: figure out better way to signal "specialized and not defined" than an extra lookup */
                if (isset($dsc["op1"][$op1]) &&
                    isset($dsc["op2"][$op2]) &&
                    (!isset($extra_spec["OP_DATA"]) || isset($dsc["spec"]["OP_DATA"][$extra_spec["OP_DATA"]]))) {
                    if (skip_extra_spec_function($op1, $op2, $extra_spec)) {
                        gen_null_label($f, $kind, $prolog);
                        $list[$label] = null;
                        $label++;
                        return;
                    }

                    /* Skip SMART_BRANCH specialization for "cold" CONST_CONST instructions */
                    if (isset($extra_spec["SMART_BRANCH"])) {
                        if ($dsc["hot"] === 'HOT_NOCONSTCONST_'
                         || $dsc["hot"] === 'COLD_CONSTCONST_') {
                            if (($op1 === 'CONST') && ($op2 === 'CONST')) {
                                unset($extra_spec["SMART_BRANCH"]);
                            }
                        }
                    }

                    /* Skip QUICK_ARG specialization for named parameters */
                    if (isset($extra_spec["QUICK_ARG"])) {
                        if ($op2 === "CONST") {
                            unset($extra_spec["QUICK_ARG"]);
                        }
                    }

                    /* Skip all specialization for OBSERVER handlers */
                    if (isset($extra_spec["OBSERVER"]) && $extra_spec["OBSERVER"] == 1) {
                        if (isset($extra_spec["RETVAL"])) {
                            unset($extra_spec["RETVAL"]);
                        }
                        if ($op1 != "ANY" || $op2 != "ANY") {
                            $op1 = "ANY";
                            $op2 = "ANY";
                        }
                    }

                    // Emit pointer to specialized handler
                    $spec_name = $dsc["op"]."_SPEC".$prefix[$op1].$prefix[$op2].extra_spec_name($extra_spec);
                    switch ($kind) {
                        case ZEND_VM_KIND_CALL:
                            out($f,"$prolog{$spec_name}_HANDLER,\n");
                            break;
                        case ZEND_VM_KIND_SWITCH:
                            out($f,$prolog."(void*)(uintptr_t)$switch_labels[$spec_name],\n");
                            break;
                        case ZEND_VM_KIND_GOTO:
                            out($f,$prolog."(void*)&&{$spec_name}_LABEL,\n");
                            break;
                    }
                    $list[$label] = $spec_name;
                    $label++;
                } else {
                    // Emit pointer to handler of undefined opcode
                    gen_null_label($f, $kind, $prolog);
                    $list[$label] = null;
                    $label++;
                }
            };

            $do = $generate;
            if ($spec_extra) {
                foreach ($spec_extra as $extra => $devnull) {
                    if ($extra == "OP_DATA") {
                        $do = $foreach_op_data($do);
                    } else {
                        $do = $foreach_extra_spec($do, $extra);
                    }
                }
            }
            if ($spec_op2) {
                $do = $foreach_op2($do);
            }
            if ($spec_op1) {
                $do = $foreach_op1($do);
            }

            $do($def_op1_type, $def_op2_type);
        }
    } else {
      // Emit labels for unspecialized executor

      // For each opcode in opcode number order
        foreach ($opcodes as $num => $dsc) {
            while ($next != $num) {
              // If some opcode numbers are not used then fill hole with pointers
              // to handler of undefined opcode
                switch ($kind) {
                    case ZEND_VM_KIND_CALL:
                        out($f,$prolog."ZEND_NULL_HANDLER,\n");
                        break;
                    case ZEND_VM_KIND_SWITCH:
                        out($f,$prolog."(void*)(uintptr_t)-1,\n");
                        break;
                    case ZEND_VM_KIND_GOTO:
                        out($f,$prolog."(void*)&&ZEND_NULL_LABEL,\n");
                        break;
                }
                $next++;
            }
            if ($num >= 256) {
                continue;
            }
            $next = $num+1;

            if (isset($dsc['alias']) && $kind != ZEND_VM_KIND_SWITCH) {
                // Emit pointer to unspecialized handler
                switch ($kind) {
                case ZEND_VM_KIND_CALL:
                    out($f,$prolog.$dsc['alias']."_HANDLER,\n");
                    break;
                case ZEND_VM_KIND_GOTO:
                    out($f,$prolog."(void*)&&".$dsc['alias']."_LABEL,\n");
                    break;
                }
                $list[] = $dsc["op"];
            } else if ($dsc["code"]) { //ugly trick for ZEND_VM_DEFINE_OP
                // Emit pointer to unspecialized handler
                switch ($kind) {
                case ZEND_VM_KIND_CALL:
                    out($f,$prolog.$dsc["op"]."_HANDLER,\n");
                    break;
                case ZEND_VM_KIND_SWITCH:
                    out($f,$prolog."(void*)(uintptr_t)".((string)$num).",\n");
                    break;
                case ZEND_VM_KIND_GOTO:
                    out($f,$prolog."(void*)&&".$dsc["op"]."_LABEL,\n");
                    break;
                }
                $list[] = $dsc["op"];
            } else {
                switch ($kind) {
                    case ZEND_VM_KIND_CALL:
                        out($f,$prolog."ZEND_NULL_HANDLER,\n");
                        break;
                    case ZEND_VM_KIND_SWITCH:
                        out($f,$prolog."(void*)(uintptr_t)-1,\n");
                        break;
                    case ZEND_VM_KIND_GOTO:
                        out($f,$prolog."(void*)&&ZEND_NULL_LABEL,\n");
                        break;
                }
                $list[] = null;
            }
        }
    }

    // Emit last handler's label (undefined opcode)
    switch ($kind) {
        case ZEND_VM_KIND_CALL:
            out($f,$prolog."ZEND_NULL_HANDLER\n");
            break;
        case ZEND_VM_KIND_SWITCH:
            out($f,$prolog."(void*)(uintptr_t)-1\n");
            break;
        case ZEND_VM_KIND_GOTO:
            out($f,$prolog."(void*)&&ZEND_NULL_LABEL\n");
            break;
    }
    $specs[$num + 1] = "$label";

    $l = fopen(__DIR__ . "/zend_vm_handlers.h", "w+") or die("ERROR: Cannot create zend_vm_handlers.h\n");
    out($l, "#define VM_HANDLERS(_) \\\n");
    foreach ($list as $n => $name) {
        if (null !== $name) {
            out($l, "\t_($n, $name) \\\n");
        }
    }
    out($l, "\t_($n+1, ZEND_NULL)\n");
    fclose($l);
}

// Generates specialized offsets
function gen_specs($f, $prolog, $specs) {
    $lastdef = array_pop($specs);
    $last = 0;
    foreach ($specs as $num => $def) {
        while (++$last < $num) {
            out($f, "$prolog$lastdef,\n");
        }
        $last = $num;
        out($f, "$prolog$def,\n");
    }
    while ($last++ < 255) {
        out($f, "$prolog$lastdef,\n");
    }
}

// Generates handler for undefined opcodes (CALL threading model)
function gen_null_handler($f) {
    static $done = 0;

    // New and all executors with CALL threading model can use the same handler
    // for undefined opcodes, do we emit code for it only once
    if (!$done) {
        $done = 1;
        out($f,"static ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL ZEND_NULL_HANDLER(ZEND_OPCODE_HANDLER_ARGS)\n");
        out($f,"{\n");
        out($f,"\tUSE_OPLINE\n");
        out($f,"\n");
        out($f,"\tSAVE_OPLINE();\n");
        out($f,"\tzend_error_noreturn(E_ERROR, \"Invalid opcode %d/%d/%d.\", OPLINE->opcode, OPLINE->op1_type, OPLINE->op2_type);\n");
        out($f,"\tZEND_VM_NEXT_OPCODE(); /* Never reached */\n");
        out($f,"}\n\n");
    }
}

function extra_spec_name($extra_spec) {
    global $prefix;

    $s = "";
    if (isset($extra_spec["OP_DATA"])) {
        $s .= "_OP_DATA" . $prefix[$extra_spec["OP_DATA"]];
    }
    if (isset($extra_spec["RETVAL"])) {
        $s .= "_RETVAL_".($extra_spec["RETVAL"] ? "USED" : "UNUSED");
    }
    if (isset($extra_spec["QUICK_ARG"])) {
        if ($extra_spec["QUICK_ARG"]) {
            $s .= "_QUICK";
        }
    }
    if (isset($extra_spec["SMART_BRANCH"])) {
        if ($extra_spec["SMART_BRANCH"] == 1) {
            $s .= "_JMPZ";
        } else if ($extra_spec["SMART_BRANCH"] == 2) {
            $s .= "_JMPNZ";
        }
    }
    if (isset($extra_spec["ISSET"])) {
        if ($extra_spec["ISSET"] == 0) {
            $s .= "_SET";
        } else {
            $s .= "_EMPTY";
        }
    }
    if (isset($extra_spec["OBSERVER"])) {
        if ($extra_spec["OBSERVER"]) {
            $s .= "_OBSERVER";
        }
    }
    return $s;
}

function extra_spec_flags($extra_spec) {
    $s = array();
    if (isset($extra_spec["OP_DATA"])) {
        $s[] = "SPEC_RULE_OP_DATA";
    }
    if (isset($extra_spec["RETVAL"])) {
        $s[] = "SPEC_RULE_RETVAL";
    }
    if (isset($extra_spec["QUICK_ARG"])) {
        $s[] = "SPEC_RULE_QUICK_ARG";
    }
    if (isset($extra_spec["SMART_BRANCH"])) {
        $s[] = "SPEC_RULE_SMART_BRANCH";
    }
    if (isset($extra_spec["COMMUTATIVE"])) {
        $s[] = "SPEC_RULE_COMMUTATIVE";
    }
    if (isset($extra_spec["ISSET"])) {
        $s[] = "SPEC_RULE_ISSET";
    }
    if (isset($extra_spec["OBSERVER"])) {
        $s[] = "SPEC_RULE_OBSERVER";
    }
    return $s;
}

function extra_spec_handler($dsc) {
    global $op_types_ex;

    if (!isset($dsc["spec"])) {
        return array(array());
    }
    $specs = $dsc["spec"];

    if (isset($specs["OP_DATA"])) {
        $op_data_specs = $specs["OP_DATA"];
        $specs["OP_DATA"] = array();
        foreach ($op_types_ex as $op_data) {
            if (isset($dsc["spec"]["OP_DATA"][$op_data])) {
                $specs["OP_DATA"][] = $op_data;
            }
        }
    }

    $f = function($specs) use (&$f) {
        $spec = key($specs);
        $top = array_shift($specs);
        if ($specs) {
            $next = $f($specs);
        } else {
            $next = array(array());
        }
        $ret = array();
        foreach ($next as $existing) {
            foreach ($top as $mode) {
                $ret[] = array($spec => $mode) + $existing;
            }
        }
        return $ret;
    };
    return $f($specs);
}

function read_order_file($fn) {
    $f = fopen($fn, "r");
    if (!is_resource($f)) {
        return false;
    }
    $order = [];
    while (!feof($f)) {
        $op = trim(fgets($f));
        if ($op !== "") {
            $order[$op] = null;
        }
    }
    fclose($f);
    return $order;
}

// Generates all opcode handlers and helpers (specialized or unspecialized)
function gen_executor_code($f, $spec, $kind, $prolog, &$switch_labels = array()) {
    global $list, $opcodes, $helpers, $op_types_ex, $gen_order;

    if ($spec) {
        // Produce specialized executor
        $op1t = $op_types_ex;
        // for each op1.op_type
        foreach ($op1t as $op1) {
            $op2t = $op_types_ex;
            // for each op2.op_type
            foreach ($op2t as $op2) {
                // for each handlers in helpers in original order
                foreach ($list as $lineno => $dsc) {
                    if (isset($dsc["handler"])) {
                        $num = $dsc["handler"];
                        foreach (extra_spec_handler($opcodes[$num]) as $extra_spec) {
                            // Check if handler accepts such types of operands (op1 and op2)
                            if (isset($opcodes[$num]["op1"][$op1]) &&
                                isset($opcodes[$num]["op2"][$op2])) {
                              // Generate handler code
                                gen_handler($f, 1, $kind, $opcodes[$num]["op"], $op1, $op2, isset($opcodes[$num]["use"]), $opcodes[$num]["code"], $lineno, $opcodes[$num], $extra_spec, $switch_labels);
                            }
                        }
                    } else if (isset($dsc["helper"])) {
                        $num = $dsc["helper"];
                        foreach (extra_spec_handler($helpers[$num]) as $extra_spec) {
                            // Check if handler accepts such types of operands (op1 and op2)
                            if (isset($helpers[$num]["op1"][$op1]) &&
                                isset($helpers[$num]["op2"][$op2])) {
                              // Generate helper code
                                gen_helper($f, 1, $kind, $num, $op1, $op2, $helpers[$num]["param"], $helpers[$num]["code"], $lineno, $helpers[$num]["inline"], $helpers[$num]["cold"], $helpers[$num]["hot"], $extra_spec);
                            }
                        }
                    } else {
                        var_dump($dsc);
                        die("??? $kind:$num\n");
                    }
                }
            }
        }
    } else {
        // Produce unspecialized executor

        // for each handlers in helpers in original order
        foreach ($list as $lineno => $dsc) {
            if (isset($dsc["handler"])) {
                $num = $dsc["handler"];
                // Generate handler code
                if ($num < 256) {
                    gen_handler($f, 0, $kind, $opcodes[$num]["op"], "ANY", "ANY", isset($opcodes[$num]["use"]), $opcodes[$num]["code"], $lineno, $opcodes[$num]);
                }
            } else if (isset($dsc["helper"])) {
                $num = $dsc["helper"];
                // Generate helper code
                gen_helper($f, 0, $kind, $num, "ANY", "ANY", $helpers[$num]["param"], $helpers[$num]["code"], $lineno, $helpers[$num]["inline"], $helpers[$num]["cold"], $helpers[$num]["hot"]);
            } else {
                var_dump($dsc);
                die("??? $kind:$num\n");
            }
        }
    }

    if (is_array($gen_order)) {
        foreach ($gen_order as $txt) {
            if ($txt !== null) {
                out($f, $txt);
            }
        }
    }

    if (ZEND_VM_LINES) {
        // Reset #line directives
        out_line($f);
    }

    // Generate handler for undefined opcodes
    switch ($kind) {
        case ZEND_VM_KIND_CALL:
            gen_null_handler($f);
            break;
        case ZEND_VM_KIND_SWITCH:
            out($f,"default: ZEND_NULL_LABEL:\n");
            out($f,"\tzend_error_noreturn(E_ERROR, \"Invalid opcode %d/%d/%d.\", OPLINE->opcode, OPLINE->op1_type, OPLINE->op2_type);\n");
            out($f,"\tZEND_VM_NEXT_OPCODE(); /* Never reached */\n");
            break;
        case ZEND_VM_KIND_GOTO:
            out($f,"ZEND_NULL_LABEL:\n");
            out($f,"\tzend_error_noreturn(E_ERROR, \"Invalid opcode %d/%d/%d.\", OPLINE->opcode, OPLINE->op1_type, OPLINE->op2_type);\n");
            out($f,"\tZEND_VM_NEXT_OPCODE(); /* Never reached */\n");
            break;
        case ZEND_VM_KIND_HYBRID:
            out($f,"\t\t\tHYBRID_CASE(HYBRID_HALT):\n");
            out($f,"#ifdef ZEND_VM_FP_GLOBAL_REG\n");
            out($f,"\t\t\t\texecute_data = vm_stack_data.orig_execute_data;\n");
            out($f,"#endif\n");
            out($f,"#ifdef ZEND_VM_IP_GLOBAL_REG\n");
            out($f,"\t\t\t\topline = vm_stack_data.orig_opline;\n");
            out($f,"#endif\n");
            out($f,"\t\t\t\treturn;\n");
            out($f,"\t\t\tHYBRID_DEFAULT:\n");
            out($f,"\t\t\t\tVM_TRACE(ZEND_NULL)\n");
            out($f,"\t\t\t\tZEND_NULL_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);\n");
            out($f,"\t\t\t\tVM_TRACE_OP_END(ZEND_NULL)\n");
            out($f,"\t\t\t\tHYBRID_BREAK(); /* Never reached */\n");
            break;
    }
}

function skip_blanks($f, $prolog, $epilog) {
    if (trim($prolog) != "" || trim($epilog) != "") {
        out($f, $prolog.$epilog);
    }
}

// Generates executor from skeleton file and definition (specialized or unspecialized)
function gen_executor($f, $skl, $spec, $kind, $executor_name, $initializer_name) {
    global $params, $skeleton_file, $line_no, $gen_order;

    if ($kind == ZEND_VM_KIND_HYBRID && file_exists(__DIR__ . "/zend_vm_order.txt")) {
        $gen_order = read_order_file(__DIR__ . "/zend_vm_order.txt");
    } else {
        $gen_order = null;
    }

    $switch_labels = array();
    $lineno = 0;
    foreach ($skl as $line) {
      // Skeleton file contains special markers in form %NAME% those are
      // substituted by custom code
        if (preg_match("/(.*)[{][%]([A-Z_]*)[%][}](.*)/", $line, $m)) {
            switch ($m[2]) {
                case "DEFINES":
                    out($f,"#define SPEC_START_MASK        0x0000ffff\n");
                    out($f,"#define SPEC_EXTRA_MASK        0xfffc0000\n");
                    out($f,"#define SPEC_RULE_OP1          0x00010000\n");
                    out($f,"#define SPEC_RULE_OP2          0x00020000\n");
                    out($f,"#define SPEC_RULE_OP_DATA      0x00040000\n");
                    out($f,"#define SPEC_RULE_RETVAL       0x00080000\n");
                    out($f,"#define SPEC_RULE_QUICK_ARG    0x00100000\n");
                    out($f,"#define SPEC_RULE_SMART_BRANCH 0x00200000\n");
                    out($f,"#define SPEC_RULE_COMMUTATIVE  0x00800000\n");
                    out($f,"#define SPEC_RULE_ISSET        0x01000000\n");
                    out($f,"#define SPEC_RULE_OBSERVER     0x02000000\n");
                    out($f,"\n");
                    out($f,"static const uint32_t *zend_spec_handlers;\n");
                    out($f,"static const void * const *zend_opcode_handlers;\n");
                    out($f,"static int zend_handlers_count;\n");
                    if ($kind == ZEND_VM_KIND_HYBRID) {
                        out($f,"#if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID)\n");
                        out($f,"static const void * const * zend_opcode_handler_funcs;\n");
                        out($f,"static zend_op hybrid_halt_op;\n");
                        out($f,"#endif\n");
                    }
                    out($f,"#if (ZEND_VM_KIND != ZEND_VM_KIND_HYBRID) || !ZEND_VM_SPEC\n");
                    out($f,"static const void *zend_vm_get_opcode_handler(uint8_t opcode, const zend_op* op);\n");
                    out($f,"#endif\n\n");
                    if ($kind == ZEND_VM_KIND_HYBRID) {
                        out($f,"#if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID)\n");
                        out($f,"static const void *zend_vm_get_opcode_handler_func(uint8_t opcode, const zend_op* op);\n");
                        out($f,"#else\n");
                        out($f,"# define zend_vm_get_opcode_handler_func zend_vm_get_opcode_handler\n");
                        out($f,"#endif\n\n");
                    }
                    out($f,"#ifndef VM_TRACE\n");
                    if (is_array($gen_order)) {
                        out($f,"# define VM_TRACE(op) ZEND_VM_GUARD(op);\n");
                    } else {
                        out($f,"# define VM_TRACE(op)\n");
                    }
                    out($f,"#endif\n");
                    out($f,"#ifndef VM_TRACE_OP_END\n");
                    out($f,"# define VM_TRACE_OP_END(op)\n");
                    out($f,"#endif\n");
                    out($f,"#ifndef VM_TRACE_START\n");
                    out($f,"# define VM_TRACE_START()\n");
                    out($f,"#endif\n");
                    out($f,"#ifndef VM_TRACE_END\n");
                    out($f,"# define VM_TRACE_END()\n");
                    out($f,"#endif\n");
                    switch ($kind) {
                        case ZEND_VM_KIND_HYBRID:
                            out($f,"#if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID)\n");
                            out($f,"# if defined(__GNUC__) && defined(__i386__)\n");
                            out($f,"#  define HYBRID_JIT_GUARD() __asm__ __volatile__ (\"\"::: \"ebx\")\n");
                            out($f,"# elif defined(__GNUC__) && defined(__x86_64__)\n");
                            out($f,"#  define HYBRID_JIT_GUARD() __asm__ __volatile__ (\"\"::: \"rbx\",\"r12\",\"r13\")\n");
                            out($f,"# elif defined(__GNUC__) && defined(__aarch64__)\n");
                            out($f,"#  define HYBRID_JIT_GUARD() __asm__ __volatile__ (\"\"::: \"x19\",\"x20\",\"x21\",\"x22\",\"x23\",\"x24\",\"x25\",\"x26\")\n");
                            out($f,"# else\n");
                            out($f,"#  define HYBRID_JIT_GUARD()\n");
                            out($f,"# endif\n");
                            out($f,"#define HYBRID_NEXT()     HYBRID_JIT_GUARD(); goto *(void**)(OPLINE->handler)\n");
                            out($f,"#define HYBRID_SWITCH()   HYBRID_NEXT();\n");
                            out($f,"#define HYBRID_CASE(op)   op ## _LABEL\n");
                            out($f,"#define HYBRID_BREAK()    HYBRID_NEXT()\n");
                            out($f,"#define HYBRID_DEFAULT    ZEND_NULL_LABEL\n");
                            out($f,"#endif\n");
                        case ZEND_VM_KIND_CALL:
                            out($f,"\n");
                            out($f,"#ifdef ZEND_VM_FP_GLOBAL_REG\n");
                            out($f,"# define ZEND_OPCODE_HANDLER_ARGS void\n");
                            out($f,"# define ZEND_OPCODE_HANDLER_ARGS_PASSTHRU\n");
                            out($f,"# define ZEND_OPCODE_HANDLER_ARGS_DC\n");
                            out($f,"# define ZEND_OPCODE_HANDLER_ARGS_PASSTHRU_CC\n");
                            out($f,"#else\n");
                            out($f,"# define ZEND_OPCODE_HANDLER_ARGS zend_execute_data *execute_data\n");
                            out($f,"# define ZEND_OPCODE_HANDLER_ARGS_PASSTHRU execute_data\n");
                            out($f,"# define ZEND_OPCODE_HANDLER_ARGS_DC , ZEND_OPCODE_HANDLER_ARGS\n");
                            out($f,"# define ZEND_OPCODE_HANDLER_ARGS_PASSTHRU_CC , ZEND_OPCODE_HANDLER_ARGS_PASSTHRU\n");
                            out($f,"#endif\n");
                            out($f,"\n");
                            out($f,"#if defined(ZEND_VM_FP_GLOBAL_REG) && defined(ZEND_VM_IP_GLOBAL_REG)\n");
                            out($f,"# define ZEND_OPCODE_HANDLER_RET void\n");
                            out($f,"# define ZEND_VM_TAIL_CALL(call) call; return\n");
                            out($f,"# ifdef ZEND_VM_TAIL_CALL_DISPATCH\n");
                            out($f,"#  define ZEND_VM_CONTINUE()     ((opcode_handler_t)OPLINE->handler)(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU); return\n");
                            out($f,"# else\n");
                            out($f,"#  define ZEND_VM_CONTINUE()     return\n");
                            out($f,"# endif\n");
                            if ($kind == ZEND_VM_KIND_HYBRID) {
                                out($f,"# if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID)\n");
                                out($f,"#  define ZEND_VM_RETURN()        opline = &hybrid_halt_op; return\n");
                                out($f,"#  define ZEND_VM_HOT             zend_always_inline ZEND_COLD ZEND_OPT_SIZE\n");
                                out($f,"#  define ZEND_VM_COLD            ZEND_COLD ZEND_OPT_SIZE\n");
                                out($f,"# else\n");
                                out($f,"#  define ZEND_VM_RETURN()        opline = NULL; return\n");
                                out($f,"#  define ZEND_VM_HOT\n");
                                out($f,"#  define ZEND_VM_COLD            ZEND_COLD ZEND_OPT_SIZE\n");
                                out($f,"# endif\n");
                            } else {
                                out($f,"# define ZEND_VM_RETURN()        opline = NULL; return\n");
                                out($f,"# define ZEND_VM_COLD            ZEND_COLD ZEND_OPT_SIZE\n");
                            }
                            out($f,"#else\n");
                            out($f,"# define ZEND_OPCODE_HANDLER_RET int\n");
                            out($f,"# define ZEND_VM_TAIL_CALL(call) return call\n");
                            out($f,"# define ZEND_VM_CONTINUE()      return  0\n");
                            out($f,"# define ZEND_VM_RETURN()        return -1\n");
                            if ($kind == ZEND_VM_KIND_HYBRID) {
                                out($f,"# define ZEND_VM_HOT\n");
                            }
                            out($f,"# define ZEND_VM_COLD            ZEND_COLD ZEND_OPT_SIZE\n");
                            out($f,"#endif\n");
                            out($f,"\n");
                            out($f,"typedef ZEND_OPCODE_HANDLER_RET (ZEND_FASTCALL *opcode_handler_t) (ZEND_OPCODE_HANDLER_ARGS);\n");
                            out($f,"\n");
                            out($f,"#define DCL_OPLINE\n");
                            out($f,"#ifdef ZEND_VM_IP_GLOBAL_REG\n");
                            out($f,"# define OPLINE opline\n");
                            out($f,"# define USE_OPLINE\n");
                            out($f,"# define LOAD_OPLINE() opline = EX(opline)\n");
                            out($f,"# define LOAD_OPLINE_EX()\n");
                            out($f,"# define LOAD_NEXT_OPLINE() opline = EX(opline) + 1\n");
                            out($f,"# define SAVE_OPLINE() EX(opline) = opline\n");
                            out($f,"# define SAVE_OPLINE_EX() SAVE_OPLINE()\n");
                            out($f,"#else\n");
                            out($f,"# define OPLINE EX(opline)\n");
                            out($f,"# define USE_OPLINE const zend_op *opline = EX(opline);\n");
                            out($f,"# define LOAD_OPLINE()\n");
                            out($f,"# define LOAD_OPLINE_EX()\n");
                            out($f,"# define LOAD_NEXT_OPLINE() ZEND_VM_INC_OPCODE()\n");
                            out($f,"# define SAVE_OPLINE()\n");
                            out($f,"# define SAVE_OPLINE_EX()\n");
                            out($f,"#endif\n");
                            out($f,"#define HANDLE_EXCEPTION() ZEND_ASSERT(EG(exception)); LOAD_OPLINE(); ZEND_VM_CONTINUE()\n");
                            out($f,"#define HANDLE_EXCEPTION_LEAVE() ZEND_ASSERT(EG(exception)); LOAD_OPLINE(); ZEND_VM_LEAVE()\n");
                            out($f,"#if defined(ZEND_VM_FP_GLOBAL_REG)\n");
                            out($f,"# define ZEND_VM_ENTER_EX()        ZEND_VM_INTERRUPT_CHECK(); ZEND_VM_CONTINUE()\n");
                            out($f,"# define ZEND_VM_ENTER()           execute_data = EG(current_execute_data); LOAD_OPLINE(); ZEND_VM_ENTER_EX()\n");
                            out($f,"# define ZEND_VM_LEAVE()           ZEND_VM_CONTINUE()\n");
                            out($f,"#elif defined(ZEND_VM_IP_GLOBAL_REG)\n");
                            out($f,"# define ZEND_VM_ENTER_EX()        return  1\n");
                            out($f,"# define ZEND_VM_ENTER()           opline = EG(current_execute_data)->opline; ZEND_VM_ENTER_EX()\n");
                            out($f,"# define ZEND_VM_LEAVE()           return  2\n");
                            out($f,"#else\n");
                            out($f,"# define ZEND_VM_ENTER_EX()        return  1\n");
                            out($f,"# define ZEND_VM_ENTER()           return  1\n");
                            out($f,"# define ZEND_VM_LEAVE()           return  2\n");
                            out($f,"#endif\n");
                            out($f,"#define ZEND_VM_INTERRUPT()      ZEND_VM_TAIL_CALL(zend_interrupt_helper".($spec?"_SPEC":"")."(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU));\n");
                            out($f,"#define ZEND_VM_LOOP_INTERRUPT() zend_interrupt_helper".($spec?"_SPEC":"")."(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);\n");
                            if ($kind == ZEND_VM_KIND_HYBRID) {
                                out($f,"#define ZEND_VM_DISPATCH(opcode, opline) ZEND_VM_TAIL_CALL(((opcode_handler_t)zend_vm_get_opcode_handler_func(opcode, opline))(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU));\n");
                            } else {
                                out($f,"#define ZEND_VM_DISPATCH(opcode, opline) ZEND_VM_TAIL_CALL(((opcode_handler_t)zend_vm_get_opcode_handler(opcode, opline))(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU));\n");
                            }
                            out($f,"\n");
                            out($f,"static ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL zend_interrupt_helper".($spec?"_SPEC":"")."(ZEND_OPCODE_HANDLER_ARGS);\n");
                            out($f,"static ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL ZEND_NULL_HANDLER(ZEND_OPCODE_HANDLER_ARGS);\n");
                            out($f,"\n");
                            break;
                        case ZEND_VM_KIND_SWITCH:
                            out($f,"\n");
                            out($f,"#define OPLINE opline\n");
                            out($f,"#ifdef ZEND_VM_IP_GLOBAL_REG\n");
                            out($f,"# define DCL_OPLINE register const zend_op *opline __asm__(ZEND_VM_IP_GLOBAL_REG);\n");
                            out($f,"#else\n");
                            out($f,"# define DCL_OPLINE const zend_op *opline;\n");
                            out($f,"#endif\n");
                            out($f,"#define USE_OPLINE\n");
                            out($f,"#define LOAD_OPLINE() opline = EX(opline)\n");
                            out($f,"# define LOAD_OPLINE_EX() LOAD_OPLINE()\n");
                            out($f,"#define LOAD_NEXT_OPLINE() opline = EX(opline) + 1\n");
                            out($f,"#define SAVE_OPLINE() EX(opline) = opline\n");
                            out($f,"#define SAVE_OPLINE_EX()\n");
                            out($f,"#define HANDLE_EXCEPTION() ZEND_ASSERT(EG(exception)); LOAD_OPLINE(); ZEND_VM_CONTINUE()\n");
                            out($f,"#define HANDLE_EXCEPTION_LEAVE() ZEND_ASSERT(EG(exception)); LOAD_OPLINE(); ZEND_VM_LEAVE()\n");
                            out($f,"#define ZEND_VM_CONTINUE() goto zend_vm_continue\n");
                            out($f,"#define ZEND_VM_RETURN()   return\n");
                            out($f,"#define ZEND_VM_ENTER_EX() ZEND_VM_INTERRUPT_CHECK(); ZEND_VM_CONTINUE()\n");
                            out($f,"#define ZEND_VM_ENTER()    execute_data = EG(current_execute_data); LOAD_OPLINE(); ZEND_VM_ENTER_EX()\n");
                            out($f,"#define ZEND_VM_LEAVE()    ZEND_VM_CONTINUE()\n");
                            out($f,"#define ZEND_VM_INTERRUPT()              goto zend_interrupt_helper".($spec?"_SPEC":"").";\n");
                            out($f,"#define ZEND_VM_LOOP_INTERRUPT()         goto zend_interrupt_helper".($spec?"_SPEC":"").";\n");
                            out($f,"#define ZEND_VM_DISPATCH(opcode, opline) dispatch_handler = zend_vm_get_opcode_handler(opcode, opline); goto zend_vm_dispatch;\n");
                            out($f,"\n");
                            break;
                        case ZEND_VM_KIND_GOTO:
                            out($f,"\n");
                            out($f,"#define OPLINE opline\n");
                            out($f,"#ifdef ZEND_VM_IP_GLOBAL_REG\n");
                            out($f,"# define DCL_OPLINE register const zend_op *opline __asm__(ZEND_VM_IP_GLOBAL_REG);\n");
                            out($f,"#else\n");
                            out($f,"# define DCL_OPLINE const zend_op *opline;\n");
                            out($f,"#endif\n");
                            out($f,"#define USE_OPLINE\n");
                            out($f,"#define LOAD_OPLINE() opline = EX(opline)\n");
                            out($f,"#define LOAD_OPLINE_EX() LOAD_OPLINE()\n");
                            out($f,"#define LOAD_NEXT_OPLINE() opline = EX(opline) + 1\n");
                            out($f,"#define SAVE_OPLINE() EX(opline) = opline\n");
                            out($f,"#define SAVE_OPLINE_EX()\n");
                            if (ZEND_VM_SPEC) {
                                out($f,"#define HANDLE_EXCEPTION() ZEND_ASSERT(EG(exception)); goto ZEND_HANDLE_EXCEPTION_SPEC_LABEL\n");
                                out($f,"#define HANDLE_EXCEPTION_LEAVE() ZEND_ASSERT(EG(exception)); goto ZEND_HANDLE_EXCEPTION_SPEC_LABEL\n");
                            } else {
                                out($f,"#define HANDLE_EXCEPTION() ZEND_ASSERT(EG(exception)); goto ZEND_HANDLE_EXCEPTION_LABEL\n");
                                out($f,"#define HANDLE_EXCEPTION_LEAVE() ZEND_ASSERT(EG(exception)); goto ZEND_HANDLE_EXCEPTION_LABEL\n");
                            }
                            out($f,"#define ZEND_VM_CONTINUE() goto *(void**)(OPLINE->handler)\n");
                            out($f,"#define ZEND_VM_RETURN()   return\n");
                            out($f,"#define ZEND_VM_ENTER_EX() ZEND_VM_INTERRUPT_CHECK(); ZEND_VM_CONTINUE()\n");
                            out($f,"#define ZEND_VM_ENTER()    execute_data = EG(current_execute_data); LOAD_OPLINE(); ZEND_VM_ENTER_EX()\n");
                            out($f,"#define ZEND_VM_LEAVE()    ZEND_VM_CONTINUE()\n");
                            out($f,"#define ZEND_VM_INTERRUPT()              goto zend_interrupt_helper".($spec?"_SPEC":"").";\n");
                            out($f,"#define ZEND_VM_LOOP_INTERRUPT()         goto zend_interrupt_helper".($spec?"_SPEC":"").";\n");
                            out($f,"#define ZEND_VM_DISPATCH(opcode, opline) goto *(void**)(zend_vm_get_opcode_handler(opcode, opline));\n");
                            out($f,"\n");
                            break;
                    }
                    if ($kind == ZEND_VM_KIND_HYBRID) {
                        gen_executor_code($f, $spec, ZEND_VM_KIND_CALL, $m[1]);
                        out($f,"\n");
                        out($f,"#if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID)\n");
                        out($f,"# undef ZEND_VM_TAIL_CALL\n");
                        out($f,"# undef ZEND_VM_CONTINUE\n");
                        out($f,"# undef ZEND_VM_RETURN\n");
//						out($f,"# undef ZEND_VM_INTERRUPT\n");
                        out($f,"\n");
                        out($f,"# define ZEND_VM_TAIL_CALL(call) call; ZEND_VM_CONTINUE()\n");
                        out($f,"# define ZEND_VM_CONTINUE()      HYBRID_NEXT()\n");
                        out($f,"# define ZEND_VM_RETURN()        goto HYBRID_HALT_LABEL\n");
//						out($f,"# define ZEND_VM_INTERRUPT()     goto zend_interrupt_helper_SPEC_LABEL\n");
                        out($f,"#endif\n\n");
                    }
                    break;
                case "EXECUTOR_NAME":
                    out($f, $m[1].$executor_name.$m[3]."\n");
                    break;
                case "HELPER_VARS":
                    if ($kind == ZEND_VM_KIND_SWITCH) {
                        out($f,$m[1]."const void *dispatch_handler;\n");
                    }
                    if ($kind != ZEND_VM_KIND_CALL && count($params)) {
                        if ($kind == ZEND_VM_KIND_HYBRID) {
                            out($f, "#if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID)\n");
                        }
                        // Emit local variables those are used for helpers' parameters
                        foreach ($params as $param => $x) {
                            out($f,$m[1].$param.";\n");
                        }
                        if ($kind == ZEND_VM_KIND_HYBRID) {
                            out($f, "#endif\n");
                        }
                    }
                    if ($kind != ZEND_VM_KIND_CALL && $kind != ZEND_VM_KIND_HYBRID) {
                        out($f,"#ifdef ZEND_VM_FP_GLOBAL_REG\n");
                        out($f,$m[1]."register zend_execute_data *execute_data __asm__(ZEND_VM_FP_GLOBAL_REG) = ex;\n");
                        out($f,"#else\n");
                        out($f,$m[1]."zend_execute_data *execute_data = ex;\n");
                        out($f,"#endif\n");
                    } else {
                        out($f,"#if defined(ZEND_VM_IP_GLOBAL_REG) || defined(ZEND_VM_FP_GLOBAL_REG)\n");
                        out($f,$m[1]."struct {\n");
                        out($f,"#ifdef ZEND_VM_HYBRID_JIT_RED_ZONE_SIZE\n");
                        out($f,$m[1]."\tchar hybrid_jit_red_zone[ZEND_VM_HYBRID_JIT_RED_ZONE_SIZE];\n");
                        out($f,"#endif\n");
                        out($f,"#ifdef ZEND_VM_IP_GLOBAL_REG\n");
                        out($f,$m[1]."\tconst zend_op *orig_opline;\n");
                        out($f,"#endif\n");
                        out($f,"#ifdef ZEND_VM_FP_GLOBAL_REG\n");
                        out($f,$m[1]."\tzend_execute_data *orig_execute_data;\n");
                        out($f,"#endif\n");
                        out($f,$m[1]."} vm_stack_data;\n");
                        out($f,"#endif\n");
                        out($f,"#ifdef ZEND_VM_IP_GLOBAL_REG\n");
                        out($f,$m[1]."vm_stack_data.orig_opline = opline;\n");
                        out($f,"#endif\n");
                        out($f,"#ifdef ZEND_VM_FP_GLOBAL_REG\n");
                        out($f,$m[1]."vm_stack_data.orig_execute_data = execute_data;\n");
                        out($f,$m[1]."execute_data = ex;\n");
                        out($f,"#else\n");
                        out($f,$m[1]."zend_execute_data *execute_data = ex;\n");
                        out($f,"#endif\n");
                    }
                    break;
                case "INTERNAL_LABELS":
                    if ($kind == ZEND_VM_KIND_GOTO || $kind == ZEND_VM_KIND_HYBRID) {
                      // Emit array of labels of opcode handlers and code for
                      // zend_opcode_handlers initialization
                        if ($kind == ZEND_VM_KIND_HYBRID) {
                            out($f,"#if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID)\n");
                        }
                        $prolog = $m[1];
                        out($f,$prolog."if (UNEXPECTED(execute_data == NULL)) {\n");
                        out($f,$prolog."\tstatic const void * const labels[] = {\n");
                        gen_labels($f, $spec, ($kind == ZEND_VM_KIND_HYBRID) ? ZEND_VM_KIND_GOTO : $kind, $prolog."\t\t", $specs);
                        out($f,$prolog."\t};\n");
                        out($f,$prolog."\tzend_opcode_handlers = (const void **) labels;\n");
                        out($f,$prolog."\tzend_handlers_count = sizeof(labels) / sizeof(void*);\n");
                        if ($kind == ZEND_VM_KIND_HYBRID) {
                            out($f,$prolog."\tmemset(&hybrid_halt_op, 0, sizeof(hybrid_halt_op));\n");
                            out($f,$prolog."\thybrid_halt_op.handler = (void*)&&HYBRID_HALT_LABEL;\n");
	                        out($f,"#ifdef ZEND_VM_HYBRID_JIT_RED_ZONE_SIZE\n");
	                        out($f,$prolog."\tmemset(vm_stack_data.hybrid_jit_red_zone, 0, ZEND_VM_HYBRID_JIT_RED_ZONE_SIZE);\n");
	                        out($f,"#endif\n");
	                        out($f,$prolog."\tif (zend_touch_vm_stack_data) {\n");
	                        out($f,$prolog."\t\tzend_touch_vm_stack_data(&vm_stack_data);\n");
	                        out($f,$prolog."\t}\n");
                            out($f,$prolog."\tgoto HYBRID_HALT_LABEL;\n");
                        } else {
                            out($f,$prolog."\treturn;\n");
                        }
                        out($f,$prolog."}\n");
                        if ($kind == ZEND_VM_KIND_HYBRID) {
                            out($f,"#endif\n");
                        }
                    } else {
                        skip_blanks($f, $m[1], $m[3]);
                    }
                    break;
                case "ZEND_VM_CONTINUE_LABEL":
                    if ($kind == ZEND_VM_KIND_CALL || $kind == ZEND_VM_KIND_HYBRID) {
                      // Only SWITCH dispatch method use it
                        out($f,"#if !defined(ZEND_VM_FP_GLOBAL_REG) || !defined(ZEND_VM_IP_GLOBAL_REG)\n");
                        out($f,$m[1]."\tint ret;".$m[3]."\n");
                        out($f,"#endif\n");
                    } else if ($kind == ZEND_VM_KIND_SWITCH) {
                      // Only SWITCH dispatch method use it
                        out($f,"zend_vm_continue:".$m[3]."\n");
                    } else {
                        skip_blanks($f, $m[1], $m[3]);
                    }
                    break;
                case "ZEND_VM_DISPATCH":
                  // Emit code that dispatches to opcode handler
                    switch ($kind) {
                        case ZEND_VM_KIND_SWITCH:
                            out($f, $m[1]."dispatch_handler = OPLINE->handler;\nzend_vm_dispatch:\n".$m[1]."switch ((int)(uintptr_t)dispatch_handler)".$m[3]."\n");
                            break;
                        case ZEND_VM_KIND_GOTO:
                            out($f, $m[1]."goto *(void**)(OPLINE->handler);".$m[3]."\n");
                            break;
                        case ZEND_VM_KIND_HYBRID:
                            out($f,"#if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID)\n");
                            out($f, $m[1]."HYBRID_SWITCH()".$m[3]."\n");
                            out($f,"#else\n");
                        case ZEND_VM_KIND_CALL:
                            out($f,"#if defined(ZEND_VM_FP_GLOBAL_REG) && defined(ZEND_VM_IP_GLOBAL_REG)\n");
                            out($f, $m[1]."((opcode_handler_t)OPLINE->handler)(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);\n");
                            out($f, $m[1]."if (UNEXPECTED(!OPLINE))".$m[3]."\n");
                            out($f,"#else\n");
                            out($f, $m[1]."if (UNEXPECTED((ret = ((opcode_handler_t)OPLINE->handler)(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU)) != 0))".$m[3]."\n");
                            out($f,"#endif\n");
                            if ($kind == ZEND_VM_KIND_HYBRID) {
                                out($f,"#endif\n");
                            }
                            break;
                    }
                    break;
                case "INTERNAL_EXECUTOR":
                    if ($kind != ZEND_VM_KIND_CALL) {
                        // Emit executor code
                        if ($kind == ZEND_VM_KIND_HYBRID) {
                            out($f,"#if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID)\n");
                        }
                        gen_executor_code($f, $spec, $kind, $m[1], $switch_labels);
                    }
                    if ($kind == ZEND_VM_KIND_CALL || $kind == ZEND_VM_KIND_HYBRID) {
                        // Executor is defined as a set of functions
                        if ($kind == ZEND_VM_KIND_HYBRID) {
                            out($f,"#else\n");
                        }
                        out($f,
                                "#ifdef ZEND_VM_FP_GLOBAL_REG\n" .
                                $m[1]."execute_data = vm_stack_data.orig_execute_data;\n" .
                                "# ifdef ZEND_VM_IP_GLOBAL_REG\n" .
                                $m[1]."opline = vm_stack_data.orig_opline;\n" .
                                "# endif\n" .
                                $m[1]."return;\n" .
                                "#else\n" .
                                $m[1]."if (EXPECTED(ret > 0)) {\n" .
                                $m[1]."\texecute_data = EG(current_execute_data);\n".
                                $m[1]."\tZEND_VM_LOOP_INTERRUPT_CHECK();\n".
                                $m[1]."} else {\n" .
                                "# ifdef ZEND_VM_IP_GLOBAL_REG\n" .
                                $m[1]."\topline = vm_stack_data.orig_opline;\n" .
                                "# endif\n".
                                $m[1]."\treturn;\n".
                                $m[1]."}\n".
                                "#endif\n");
                        if ($kind == ZEND_VM_KIND_HYBRID) {
                            out($f,"#endif\n");
                        }
                    }
                    break;
                case "EXTERNAL_EXECUTOR":
                    if ($kind == ZEND_VM_KIND_CALL) {
                        gen_executor_code($f, $spec, $kind, $m[1]);
                    }
                    break;
                case "INITIALIZER_NAME":
                    out($f, $m[1].$initializer_name.$m[3]."\n");
                    break;
                case "EXTERNAL_LABELS":
                  // Emit code that initializes zend_opcode_handlers array
                    $prolog = $m[1];
                    if ($kind == ZEND_VM_KIND_GOTO) {
                      // Labels are defined in the executor itself, so we call it
                      // with execute_data NULL and it sets zend_opcode_handlers array
                        out($f,$prolog."static const uint32_t specs[] = {\n");
                        gen_specs($f, $prolog."\t", $specs);
                        out($f,$prolog."};\n");
                        out($f,$prolog."zend_spec_handlers = specs;\n");
                        out($f,$prolog.$executor_name."_ex(NULL);\n");
                    } else {
                        out($f,$prolog."static const void * const labels[] = {\n");
                        gen_labels($f, $spec, ($kind == ZEND_VM_KIND_HYBRID) ? ZEND_VM_KIND_CALL : $kind, $prolog."\t", $specs, $switch_labels);
                        out($f,$prolog."};\n");
                        out($f,$prolog."static const uint32_t specs[] = {\n");
                        gen_specs($f, $prolog."\t", $specs);
                        out($f,$prolog."};\n");
                        if ($kind == ZEND_VM_KIND_HYBRID) {
                            out($f,"#if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID)\n");
                            out($f,$prolog."zend_opcode_handler_funcs = labels;\n");
                            out($f,$prolog."zend_spec_handlers = specs;\n");
                            out($f,$prolog.$executor_name."_ex(NULL);\n");
                            out($f,"#else\n");
                        }
                        out($f,$prolog."zend_opcode_handlers = labels;\n");
                        out($f,$prolog."zend_handlers_count = sizeof(labels) / sizeof(void*);\n");
                        out($f,$prolog."zend_spec_handlers = specs;\n");
                        if ($kind == ZEND_VM_KIND_HYBRID) {
                            out($f,"#endif\n");
                        }
                    }
                    break;
                default:
                    die("ERROR: Unknown keyword ".$m[2]." in skeleton file.\n");
            }
        } else {
          // Copy the line as is
            out($f, $line);
        }
    }
}

function parse_operand_spec($def, $lineno, $str, &$flags) {
    global $vm_op_decode;

    $flags = 0;
    $a = explode("|",$str);
    foreach ($a as $val) {
        if (isset($vm_op_decode[$val])) {
            $flags |= $vm_op_decode[$val];
        } else {
            die("ERROR ($def:$lineno): Wrong operand type '$str'\n");
        }
    }
    if (!($flags & ZEND_VM_OP_SPEC)) {
        if (count($a) != 1) {
            die("ERROR ($def:$lineno): Wrong operand type '$str'\n");
        }
        $a = array("ANY");
    }
    return array_flip($a);
}

function parse_ext_spec($def, $lineno, $str) {
    global $vm_ext_decode;

    $flags = 0;
    $a = explode("|",$str);
    foreach ($a as $val) {
        if (isset($vm_ext_decode[$val])) {
            $flags |= $vm_ext_decode[$val];
        } else {
            die("ERROR ($def:$lineno): Wrong extended_value type '$str'\n");
        }
    }
    return $flags;
}

function parse_spec_rules($def, $lineno, $str) {
    global $used_extra_spec;

    $ret = array();
    $a = explode(",", $str);
    foreach ($a as $rule) {
        $n = strpos($rule, "=");
        if ($n !== false) {
            $id = trim(substr($rule, 0, $n));
            $val = trim(substr($rule, $n+1));
            switch ($id) {
                case "OP_DATA":
                    $ret["OP_DATA"] = parse_operand_spec($def, $lineno, $val, $devnull);
                    break;
                default:
                    die("ERROR ($def:$lineno): Wrong specialization rules '$str'\n");
            }
            $used_extra_spec[$id] = 1;
        } else {
            switch ($rule) {
                case "RETVAL":
                    $ret["RETVAL"] = array(0, 1);
                    break;
                case "QUICK_ARG":
                    $ret["QUICK_ARG"] = array(0, 1);
                    break;
                case "SMART_BRANCH":
                    $ret["SMART_BRANCH"] = array(0, 1, 2);
                    break;
                case "NO_CONST_CONST":
                    $ret["NO_CONST_CONST"] = array(1);
                    break;
                case "COMMUTATIVE":
                    $ret["COMMUTATIVE"] = array(1);
                    break;
                case "ISSET":
                    $ret["ISSET"] = array(0, 1);
                    break;
                case "OBSERVER":
                    $ret["OBSERVER"] = array(0, 1);
                    break;
                default:
                    die("ERROR ($def:$lineno): Wrong specialization rules '$str'\n");
            }
            $used_extra_spec[$rule] = 1;
        }
    }
    return $ret;
}

function gen_vm_opcodes_header(
    array $opcodes, int $max_opcode, int $max_opcode_len, array $vm_op_flags
): string {
    $str = HEADER_TEXT;
    $str .= "#ifndef ZEND_VM_OPCODES_H\n#define ZEND_VM_OPCODES_H\n\n";
    $str .= "#define ZEND_VM_SPEC\t\t" . ZEND_VM_SPEC . "\n";
    $str .= "#define ZEND_VM_LINES\t\t" . ZEND_VM_LINES . "\n";
    $str .= "#define ZEND_VM_KIND_CALL\t" . ZEND_VM_KIND_CALL . "\n";
    $str .= "#define ZEND_VM_KIND_SWITCH\t" . ZEND_VM_KIND_SWITCH . "\n";
    $str .= "#define ZEND_VM_KIND_GOTO\t" . ZEND_VM_KIND_GOTO . "\n";
    $str .= "#define ZEND_VM_KIND_HYBRID\t" . ZEND_VM_KIND_HYBRID . "\n";
    if ($GLOBALS["vm_kind_name"][ZEND_VM_KIND] === "ZEND_VM_KIND_HYBRID") {
        $str .= "/* HYBRID requires support for computed GOTO and global register variables*/\n";
        $str .= "#if (defined(__GNUC__) && defined(HAVE_GCC_GLOBAL_REGS))\n";
        $str .= "# define ZEND_VM_KIND\t\tZEND_VM_KIND_HYBRID\n";
        $str .= "#else\n";
        $str .= "# define ZEND_VM_KIND\t\tZEND_VM_KIND_CALL\n";
        $str .= "#endif\n";
    } else {
        $str .= "#define ZEND_VM_KIND\t\t" . $GLOBALS["vm_kind_name"][ZEND_VM_KIND] . "\n";
    }
    $str .= "\n";
    $str .= "#if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID) && !defined(__SANITIZE_ADDRESS__)\n";
    $str .= "# if ((defined(i386) && !defined(__PIC__)) || defined(__x86_64__) || defined(_M_X64))\n";
    $str .= "#  define ZEND_VM_HYBRID_JIT_RED_ZONE_SIZE 48\n";
    $str .= "# endif\n";
    $str .= "#endif\n";
    $str .= "\n";
    foreach ($vm_op_flags as $name => $val) {
        $str .= sprintf("#define %-24s 0x%08x\n", $name, $val);
    }
    $str .= "#define ZEND_VM_OP1_FLAGS(flags) (flags & 0xff)\n";
    $str .= "#define ZEND_VM_OP2_FLAGS(flags) ((flags >> 8) & 0xff)\n";
    $str .= "\n";
    $str .= "BEGIN_EXTERN_C()\n\n";
    $str .= "ZEND_API const char* ZEND_FASTCALL zend_get_opcode_name(uint8_t opcode);\n";
    $str .= "ZEND_API uint32_t ZEND_FASTCALL zend_get_opcode_flags(uint8_t opcode);\n";
    $str .= "ZEND_API uint8_t zend_get_opcode_id(const char *name, size_t length);\n\n";
    $str .= "END_EXTERN_C()\n\n";

    $code_len = strlen((string) $max_opcode);
    foreach ($opcodes as $code => $dsc) {
        $code = str_pad((string)$code, $code_len, " ", STR_PAD_LEFT);
        $op = str_pad($dsc["op"], $max_opcode_len);
        if ($code <= $max_opcode) {
            $str .= "#define $op $code\n";
        }
    }

    $code = str_pad((string)$max_opcode, $code_len, " ", STR_PAD_LEFT);
    $op = str_pad("ZEND_VM_LAST_OPCODE", $max_opcode_len);
    $str .= "\n#define $op $code\n";

    $str .= "\n#endif\n";
    return $str;
}

function gen_vm($def, $skel) {
    global $definition_file, $skeleton_file, $executor_file,
        $op_types, $list, $opcodes, $helpers, $params, $opnames,
        $vm_op_flags, $used_extra_spec;

    // Load definition file
    $in = @file($def);
    if (!$in) {
        die("ERROR: Cannot open definition file '$def'\n");
    }
    // We need absolute path to definition file to use it in #line directives
    $definition_file = realpath($def);

    // Load skeleton file
    $skl = @file($skel);
    if (!$skl) {
        die("ERROR: Cannot open skeleton file '$skel'\n");
    }
    // We need absolute path to skeleton file to use it in #line directives
    $skeleton_file = realpath($skel);

    // Parse definition file into tree
    $lineno         = 0;
    $handler        = null;
    $helper         = null;
    $max_opcode_len = 0;
    $max_opcode     = 0;
    $extra_num      = 256;
    foreach ($in as $line) {
        ++$lineno;
        if (strpos($line,"ZEND_VM_HANDLER(") === 0 ||
            strpos($line,"ZEND_VM_INLINE_HANDLER(") === 0 ||
            strpos($line,"ZEND_VM_HOT_HANDLER(") === 0 ||
            strpos($line,"ZEND_VM_HOT_NOCONST_HANDLER(") === 0 ||
            strpos($line,"ZEND_VM_HOT_NOCONSTCONST_HANDLER(") === 0 ||
            strpos($line,"ZEND_VM_HOT_SEND_HANDLER(") === 0 ||
            strpos($line,"ZEND_VM_HOT_OBJ_HANDLER(") === 0 ||
            strpos($line,"ZEND_VM_COLD_HANDLER(") === 0 ||
            strpos($line,"ZEND_VM_COLD_CONST_HANDLER(") === 0 ||
            strpos($line,"ZEND_VM_COLD_CONSTCONST_HANDLER(") === 0) {
          // Parsing opcode handler's definition
            if (preg_match(
                    "/^
                        ZEND_VM_(HOT_|INLINE_|HOT_OBJ_|HOT_SEND_|HOT_NOCONST_|HOT_NOCONSTCONST_|COLD_|COLD_CONST_|COLD_CONSTCONST_)?HANDLER\(
                            \s*([0-9]+)\s*,
                            \s*([A-Z_][A-Z0-9_]*)\s*,
                            \s*([A-Z_|]+)\s*,
                            \s*([A-Z_|]+)\s*
                            (,\s*([A-Z_|]+)\s*)?
                            (,\s*SPEC\(([A-Z_|=,]+)\)\s*)?
                        \)
                    $/x",
                    $line,
                    $m) == 0) {
                die("ERROR ($def:$lineno): Invalid ZEND_VM_HANDLER definition.\n");
            }
            $hot = !empty($m[1]) ? $m[1] : false;
            $code = (int)$m[2];
            $op   = $m[3];
            $len  = strlen($op);
            $op1  = parse_operand_spec($def, $lineno, $m[4], $flags1);
            $op2  = parse_operand_spec($def, $lineno, $m[5], $flags2);
            $flags = $flags1 | ($flags2 << 8);
            if (!empty($m[7])) {
                $flags |= parse_ext_spec($def, $lineno, $m[7]);
            }

            if ($len > $max_opcode_len) {
                $max_opcode_len = $len;
            }
            if ($code > $max_opcode) {
                $max_opcode = $code;
            }
            if (isset($opcodes[$code])) {
                die("ERROR ($def:$lineno): Opcode with code '$code' is already defined.\n");
            }
            if (isset($opnames[$op])) {
                die("ERROR ($def:$lineno): Opcode with name '$op' is already defined.\n");
            }
            $opcodes[$code] = array("op"=>$op,"op1"=>$op1,"op2"=>$op2,"code"=>"","flags"=>$flags,"hot"=>$hot);
            if (isset($m[9])) {
                $opcodes[$code]["spec"] = parse_spec_rules($def, $lineno, $m[9]);
                if (isset($opcodes[$code]["spec"]["NO_CONST_CONST"])) {
                    $opcodes[$code]["flags"] |= $vm_op_flags["ZEND_VM_NO_CONST_CONST"];
                }
                if (isset($opcodes[$code]["spec"]["COMMUTATIVE"])) {
                    $opcodes[$code]["flags"] |= $vm_op_flags["ZEND_VM_COMMUTATIVE"];
                }
            }
            $opnames[$op] = $code;
            $handler = $code;
            $helper = null;
            $list[$lineno] = array("handler"=>$handler);
        } else if (strpos($line,"ZEND_VM_TYPE_SPEC_HANDLER(") === 0 ||
                   strpos($line,"ZEND_VM_INLINE_TYPE_SPEC_HANDLER(") === 0 ||
                   strpos($line,"ZEND_VM_HOT_TYPE_SPEC_HANDLER(") === 0 ||
                   strpos($line,"ZEND_VM_HOT_NOCONST_TYPE_SPEC_HANDLER(") === 0 ||
                   strpos($line,"ZEND_VM_HOT_NOCONSTCONST_TYPE_SPEC_HANDLER(") === 0 ||
                   strpos($line,"ZEND_VM_HOT_SEND_TYPE_SPEC_HANDLER(") === 0 ||
                   strpos($line,"ZEND_VM_HOT_OBJ_TYPE_SPEC_HANDLER(") === 0) {
          // Parsing opcode handler's definition
            if (preg_match(
                    "/^
                        ZEND_VM_(HOT_|INLINE_|HOT_OBJ_|HOT_SEND_|HOT_NOCONST_|HOT_NOCONSTCONST_)?TYPE_SPEC_HANDLER\(
                            \s*([A-Z_|]+)\s*,
                            \s*((?:[^(,]|\([^()]*|(?R)*\))*),
                            \s*([A-Za-z_]+)\s*,
                            \s*([A-Z_|]+)\s*,
                            \s*([A-Z_|]+)\s*
                            (,\s*([A-Z_|]+)\s*)?
                            (,\s*SPEC\(([A-Z_|=,]+)\)\s*)?
                        \)
                    $/x",
                    $line,
                    $m) == 0) {
                die("ERROR ($def:$lineno): Invalid ZEND_VM_TYPE_HANDLER_HANDLER definition.\n");
            }
            $hot = !empty($m[1]) ? $m[1] : false;
            $orig_op_list = $m[2];
            $code = $extra_num++;
            foreach (explode('|', $orig_op_list) as $orig_op) {
                if (!isset($opnames[$orig_op])) {
                    die("ERROR ($def:$lineno): Opcode with name '$orig_op' is not defined.\n");
                }
                $orig_code = $opnames[$orig_op];
                $condition = $m[3];
                $opcodes[$orig_code]['type_spec'][$code] = $condition;
            }
            $op = $m[4];
            $op1 = parse_operand_spec($def, $lineno, $m[5], $flags1);
            $op2 = parse_operand_spec($def, $lineno, $m[6], $flags2);
            $flags = $flags1 | ($flags2 << 8);
            if (!empty($m[8])) {
                $flags |= parse_ext_spec($def, $lineno, $m[8]);
            }

            if (isset($opcodes[$code])) {
                die("ERROR ($def:$lineno): Opcode with name '$code' is already defined.\n");
            }
            $used_extra_spec["TYPE"] = 1;
            $opcodes[$code] = array("op"=>$op,"op1"=>$op1,"op2"=>$op2,"code"=>"","flags"=>$flags,"hot"=>$hot,"is_type_spec"=>true);
            if (isset($m[10])) {
                $opcodes[$code]["spec"] = parse_spec_rules($def, $lineno, $m[10]);
                if (isset($opcodes[$code]["spec"]["NO_CONST_CONST"])) {
                    $opcodes[$code]["flags"] |= $vm_op_flags["ZEND_VM_NO_CONST_CONST"];
                }
                if (isset($opcodes[$code]["spec"]["COMMUTATIVE"])) {
                    $opcodes[$code]["flags"] |= $vm_op_flags["ZEND_VM_COMMUTATIVE"];
                }
            }
            $opnames[$op] = $code;
            $handler = $code;
            $helper = null;
            $list[$lineno] = array("handler"=>$handler);
        } else if (strpos($line,"ZEND_VM_HELPER(") === 0 ||
                   strpos($line,"ZEND_VM_INLINE_HELPER(") === 0 ||
                   strpos($line,"ZEND_VM_COLD_HELPER(") === 0 ||
                   strpos($line,"ZEND_VM_HOT_HELPER(") === 0) {
          // Parsing helper's definition
            if (preg_match(
                    "/^
                        ZEND_VM(_INLINE|_COLD|_HOT)?_HELPER\(
                            \s*([A-Za-z_]+)\s*,
                            \s*([A-Z_|]+)\s*,
                            \s*([A-Z_|]+)\s*
                            (?:,\s*SPEC\(([A-Z_|=,]+)\)\s*)?
                            (?:,\s*([^)]*)\s*)?
                        \)
                    $/x",
                    $line,
                    $m) == 0) {
                die("ERROR ($def:$lineno): Invalid ZEND_VM_HELPER definition.\n");
            }
            $inline = !empty($m[1]) && $m[1] === "_INLINE";
            $cold   = !empty($m[1]) && $m[1] === "_COLD";
            $hot    = !empty($m[1]) && $m[1] === "_HOT";
            $helper = $m[2];
            $op1    = parse_operand_spec($def, $lineno, $m[3], $flags1);
            $op2    = parse_operand_spec($def, $lineno, $m[4], $flags2);
            $param  = isset($m[6]) ? $m[6] : null;
            if (isset($helpers[$helper])) {
                die("ERROR ($def:$lineno): Helper with name '$helper' is already defined.\n");
            }

            // Store parameters
            if ((ZEND_VM_KIND == ZEND_VM_KIND_GOTO
             || ZEND_VM_KIND == ZEND_VM_KIND_SWITCH
             || (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID && $hot))
             && $param) {
                foreach (explode(",", $param ) as $p) {
                    $p = trim($p);
                    if ($p !== "") {
                        $params[$p] = 1;
                    }
                }
            }

            $helpers[$helper] = array("op1"=>$op1,"op2"=>$op2,"param"=>$param,"code"=>"","inline"=>$inline,"cold"=>$cold,"hot"=>$hot);

            if (!empty($m[5])) {
                $helpers[$helper]["spec"] = parse_spec_rules($def, $lineno, $m[5]);
            }

            $handler = null;
            $list[$lineno] = array("helper"=>$helper);
        } else if (strpos($line,"ZEND_VM_DEFINE_OP(") === 0) {
            if (preg_match(
                    "/^ZEND_VM_DEFINE_OP\(\s*([0-9]+)\s*,\s*([A-Z_]+)\s*\);/",
                    $line,
                    $m) == 0) {
                die("ERROR ($def:$lineno): Invalid ZEND_VM_DEFINE_OP definition.\n");
            }
            $code = (int)$m[1];
            $op   = $m[2];
            $len  = strlen($op);

            if ($len > $max_opcode_len) {
                $max_opcode_len = $len;
            }
            if ($code > $max_opcode) {
                $max_opcode = $code;
            }
            if (isset($opcodes[$code])) {
                die("ERROR ($def:$lineno): Opcode with code '$code' is already defined.\n");
            }
            if (isset($opnames[$op])) {
                die("ERROR ($def:$lineno): Opcode with name '$op' is already defined.\n");
            }
            $opcodes[$code] = array("op"=>$op,"code"=>"");
            $opnames[$op] = $code;
        } else if ($handler !== null) {
          // Add line of code to current opcode handler
            $opcodes[$handler]["code"] .= $line;
        } else if ($helper !== null) {
          // Add line of code to current helper
            $helpers[$helper]["code"] .= $line;
        }
    }

    ksort($opcodes);

    // Search for opcode handlers those are used by other opcode handlers
    foreach ($opcodes as $dsc) {
        if (preg_match("/^\s*{\s*ZEND_VM_DISPATCH_TO_HANDLER\(\s*([A-Z_]*)\s*\)\s*;\s*}\s*/", $dsc["code"], $m)) {
            $op = $m[1];
            if (!isset($opnames[$op])) {
                die("ERROR ($def:$lineno): Opcode with name '$op' is not defined.\n");
            }
            $opcodes[$opnames[$dsc['op']]]['alias'] = $op;
            if (!ZEND_VM_SPEC && ZEND_VM_KIND == ZEND_VM_KIND_SWITCH) {
                $code = $opnames[$op];
                $opcodes[$code]['use'] = 1;
            }
        } else if (preg_match_all("/ZEND_VM_DISPATCH_TO_HANDLER\(\s*([A-Z_]*)\s*\)/m", $dsc["code"], $mm, PREG_SET_ORDER)) {
            foreach ($mm as $m) {
                $op = $m[1];
                if (!isset($opnames[$op])) {
                    die("ERROR ($def:$lineno): Opcode with name '$op' is not defined.\n");
                }
                $code = $opnames[$op];
                $opcodes[$code]['use'] = 1;
            }
        }
    }

    // Generate opcode #defines (zend_vm_opcodes.h)
    $str = gen_vm_opcodes_header($opcodes, $max_opcode, $max_opcode_len, $vm_op_flags);
    write_file_if_changed(__DIR__ . "/zend_vm_opcodes.h", $str);
    echo "zend_vm_opcodes.h generated successfully.\n";

    // zend_vm_opcodes.c
    $f = fopen(__DIR__ . "/zend_vm_opcodes.c", "w+") or die("ERROR: Cannot create zend_vm_opcodes.c\n");

    // Insert header
    out($f, HEADER_TEXT);
    fputs($f,"#include <stdio.h>\n");
    fputs($f,"#include <zend.h>\n");
    fputs($f,"#include <zend_vm_opcodes.h>\n\n");

    fputs($f,"static const char *zend_vm_opcodes_names[".($max_opcode + 1)."] = {\n");
    for ($i = 0; $i <= $max_opcode; $i++) {
        fputs($f,"\t".(isset($opcodes[$i]["op"])?'"'.$opcodes[$i]["op"].'"':"NULL").",\n");
    }
    fputs($f, "};\n\n");

    fputs($f,"static uint32_t zend_vm_opcodes_flags[".($max_opcode + 1)."] = {\n");
    for ($i = 0; $i <= $max_opcode; $i++) {
        fprintf($f, "\t0x%08x,\n", isset($opcodes[$i]["flags"]) ? $opcodes[$i]["flags"] : 0);
    }
    fputs($f, "};\n\n");

    fputs($f, "ZEND_API const char* ZEND_FASTCALL zend_get_opcode_name(uint8_t opcode) {\n");
    fputs($f, "\tif (UNEXPECTED(opcode > ZEND_VM_LAST_OPCODE)) {\n");
    fputs($f, "\t\treturn NULL;\n");
    fputs($f, "\t}\n");
    fputs($f, "\treturn zend_vm_opcodes_names[opcode];\n");
    fputs($f, "}\n");

    fputs($f, "ZEND_API uint32_t ZEND_FASTCALL zend_get_opcode_flags(uint8_t opcode) {\n");
    fputs($f, "\tif (UNEXPECTED(opcode > ZEND_VM_LAST_OPCODE)) {\n");
    fputs($f, "\t\topcode = ZEND_NOP;\n");
    fputs($f, "\t}\n");
    fputs($f, "\treturn zend_vm_opcodes_flags[opcode];\n");
    fputs($f, "}\n");

    fputs($f, "ZEND_API uint8_t zend_get_opcode_id(const char *name, size_t length) {\n");
    fputs($f, "\tuint8_t opcode;\n");
    fputs($f, "\tfor (opcode = 0; opcode < (sizeof(zend_vm_opcodes_names) / sizeof(zend_vm_opcodes_names[0])) - 1; opcode++) {\n");
    fputs($f, "\t\tconst char *opcode_name = zend_vm_opcodes_names[opcode];\n");
    fputs($f, "\t\tif (opcode_name && strncmp(opcode_name, name, length) == 0) {\n");
    fputs($f, "\t\t\treturn opcode;\n");
    fputs($f, "\t\t}\n");
    fputs($f, "\t}\n");
    fputs($f, "\treturn ZEND_VM_LAST_OPCODE + 1;\n");
    fputs($f, "}\n");

    fclose($f);
    echo "zend_vm_opcodes.c generated successfully.\n";

    // Generate zend_vm_execute.h
    $f = fopen(__DIR__ . "/zend_vm_execute.h", "w+") or die("ERROR: Cannot create zend_vm_execute.h\n");
    $executor_file = realpath(__DIR__ . "/zend_vm_execute.h");

    // Insert header
    out($f, HEADER_TEXT);

    out($f, "#ifdef ZEND_WIN32\n");
    // Suppress free_op1 warnings on Windows
    out($f, "# pragma warning(disable : 4101)\n");
    if (ZEND_VM_SPEC) {
        // Suppress (<non-zero constant> || <expression>) warnings on windows
        out($f, "# pragma warning(once : 6235)\n");
        // Suppress (<zero> && <expression>) warnings on windows
        out($f, "# pragma warning(once : 6237)\n");
        // Suppress (<non-zero constant> && <expression>) warnings on windows
        out($f, "# pragma warning(once : 6239)\n");
        // Suppress (<expression> && <non-zero constant>) warnings on windows
        out($f, "# pragma warning(once : 6240)\n");
        // Suppress (<non-zero constant> || <non-zero constant>) warnings on windows
        out($f, "# pragma warning(once : 6285)\n");
        // Suppress (<non-zero constant> || <expression>) warnings on windows
        out($f, "# pragma warning(once : 6286)\n");
        // Suppress constant with constant comparison warnings on windows
        out($f, "# pragma warning(once : 6326)\n");
    }
    out($f, "#endif\n");

    // Support for ZEND_USER_OPCODE
    out($f, "static user_opcode_handler_t zend_user_opcode_handlers[256] = {\n");
    for ($i = 0; $i < 255; ++$i) {
        out($f, "\t(user_opcode_handler_t)NULL,\n");
    }
    out($f, "\t(user_opcode_handler_t)NULL\n};\n\n");

    out($f, "static uint8_t zend_user_opcodes[256] = {");
    for ($i = 0; $i < 255; ++$i) {
        if ($i % 16 == 1) out($f, "\n\t");
        out($f, "$i,");
    }
    out($f, "255\n};\n\n");

    // Generate specialized executor
    gen_executor($f, $skl, ZEND_VM_SPEC, ZEND_VM_KIND, "execute", "zend_vm_init");
    out($f, "\n");

    // Generate zend_vm_get_opcode_handler() function
    out($f, "static uint32_t ZEND_FASTCALL zend_vm_get_opcode_handler_idx(uint32_t spec, const zend_op* op)\n");
    out($f, "{\n");
    if (!ZEND_VM_SPEC) {
        out($f, "\treturn spec;\n");
    } else {
        out($f, "\tstatic const int zend_vm_decode[] = {\n");
        out($f, "\t\t_UNUSED_CODE, /* 0 = IS_UNUSED  */\n");
        out($f, "\t\t_CONST_CODE,  /* 1 = IS_CONST   */\n");
        out($f, "\t\t_TMP_CODE,    /* 2 = IS_TMP_VAR */\n");
        out($f, "\t\t_UNUSED_CODE, /* 3              */\n");
        out($f, "\t\t_VAR_CODE,    /* 4 = IS_VAR     */\n");
        out($f, "\t\t_UNUSED_CODE, /* 5              */\n");
        out($f, "\t\t_UNUSED_CODE, /* 6              */\n");
        out($f, "\t\t_UNUSED_CODE, /* 7              */\n");
        out($f, "\t\t_CV_CODE      /* 8 = IS_CV      */\n");
        out($f, "\t};\n");
        out($f, "\tuint32_t offset = 0;\n");
        out($f, "\tif (spec & SPEC_RULE_OP1) offset = offset * 5 + zend_vm_decode[op->op1_type];\n");
        out($f, "\tif (spec & SPEC_RULE_OP2) offset = offset * 5 + zend_vm_decode[op->op2_type];\n");

        if (isset($used_extra_spec["OP_DATA"]) ||
            isset($used_extra_spec["RETVAL"]) ||
            isset($used_extra_spec["QUICK_ARG"]) ||
            isset($used_extra_spec["SMART_BRANCH"]) ||
            isset($used_extra_spec["ISSET"]) ||
            isset($used_extra_spec["OBSERVER"])) {

            $else = "";
            out($f, "\tif (spec & SPEC_EXTRA_MASK) {\n");

            if (isset($used_extra_spec["RETVAL"])) {
                out($f, "\t\t{$else}if (spec & SPEC_RULE_RETVAL) {\n");
                out($f, "\t\t\toffset = offset * 2 + (op->result_type != IS_UNUSED);\n");
                out($f, "\t\t\tif ((spec & SPEC_RULE_OBSERVER) && ZEND_OBSERVER_ENABLED) {\n");
                out($f,	"\t\t\t\toffset += 2;\n");
                out($f, "\t\t\t}\n");
                $else = "} else ";
            }
            if (isset($used_extra_spec["QUICK_ARG"])) {
                out($f, "\t\t{$else}if (spec & SPEC_RULE_QUICK_ARG) {\n");
                out($f, "\t\t\toffset = offset * 2 + (op->op2.num <= MAX_ARG_FLAG_NUM);\n");
                $else = "} else ";
            }
            if (isset($used_extra_spec["OP_DATA"])) {
                out($f, "\t\t{$else}if (spec & SPEC_RULE_OP_DATA) {\n");
                out($f, "\t\t\toffset = offset * 5 + zend_vm_decode[(op + 1)->op1_type];\n");
                $else = "} else ";
            }
            if (isset($used_extra_spec["ISSET"])) {
                out($f, "\t\t{$else}if (spec & SPEC_RULE_ISSET) {\n");
                out($f, "\t\t\toffset = offset * 2 + (op->extended_value & ZEND_ISEMPTY);\n");
                $else = "} else ";
            }
            if (isset($used_extra_spec["SMART_BRANCH"])) {
                out($f, "\t\t{$else}if (spec & SPEC_RULE_SMART_BRANCH) {\n");
                out($f,	"\t\t\toffset = offset * 3;\n");
                out($f, "\t\t\tif (op->result_type == (IS_SMART_BRANCH_JMPZ|IS_TMP_VAR)) {\n");
                out($f,	"\t\t\t\toffset += 1;\n");
                out($f, "\t\t\t} else if (op->result_type == (IS_SMART_BRANCH_JMPNZ|IS_TMP_VAR)) {\n");
                out($f,	"\t\t\t\toffset += 2;\n");
                out($f, "\t\t\t}\n");
                $else = "} else ";
            }
            if (isset($used_extra_spec["OBSERVER"])) {
                out($f, "\t\t{$else}if (spec & SPEC_RULE_OBSERVER) {\n");
                out($f,	"\t\t\toffset = offset * 2;\n");
                out($f, "\t\t\tif (ZEND_OBSERVER_ENABLED) {\n");
                out($f,	"\t\t\t\toffset += 1;\n");
                out($f, "\t\t\t}\n");
                $else = "} else ";
            }
            if ($else !== "") {
                out($f, "\t\t}\n");
            }
            out($f, "\t}\n");
        }
        out($f, "\treturn (spec & SPEC_START_MASK) + offset;\n");
    }
    out($f, "}\n\n");
    out($f, "#if (ZEND_VM_KIND != ZEND_VM_KIND_HYBRID) || !ZEND_VM_SPEC\n");
    out($f, "static const void *zend_vm_get_opcode_handler(uint8_t opcode, const zend_op* op)\n");
    out($f, "{\n");
    if (!ZEND_VM_SPEC) {
        out($f, "\treturn zend_opcode_handlers[zend_vm_get_opcode_handler_idx(opcode, op)];\n");
    } else {
        out($f, "\treturn zend_opcode_handlers[zend_vm_get_opcode_handler_idx(zend_spec_handlers[opcode], op)];\n");
    }
    out($f, "}\n");
    out($f, "#endif\n\n");

    if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID) {
        // Generate zend_vm_get_opcode_handler_func() function
        out($f, "#if ZEND_VM_KIND == ZEND_VM_KIND_HYBRID\n");
        out($f,"static const void *zend_vm_get_opcode_handler_func(uint8_t opcode, const zend_op* op)\n");
        out($f, "{\n");
        out($f, "\tuint32_t spec = zend_spec_handlers[opcode];\n");
        if (!ZEND_VM_SPEC) {
            out($f, "\treturn zend_opcode_handler_funcs[spec];\n");
        } else {
            out($f, "\treturn zend_opcode_handler_funcs[zend_vm_get_opcode_handler_idx(spec, op)];\n");
        }
        out($f, "}\n\n");
        out($f, "#endif\n\n");
    }

    // Generate zend_vm_get_opcode_handler() function
    out($f, "ZEND_API void ZEND_FASTCALL zend_vm_set_opcode_handler(zend_op* op)\n");
    out($f, "{\n");
    out($f, "\tuint8_t opcode = zend_user_opcodes[op->opcode];\n");
    if (!ZEND_VM_SPEC) {
        out($f, "\top->handler = zend_opcode_handlers[zend_vm_get_opcode_handler_idx(opcode, op)];\n");
    } else {
        out($f, "\n");
        out($f, "\tif (zend_spec_handlers[op->opcode] & SPEC_RULE_COMMUTATIVE) {\n");
        out($f, "\t\tif (op->op1_type < op->op2_type) {\n");
        out($f, "\t\t\tzend_swap_operands(op);\n");
        out($f, "\t\t}\n");
        out($f, "\t}\n");
        out($f, "\top->handler = zend_opcode_handlers[zend_vm_get_opcode_handler_idx(zend_spec_handlers[opcode], op)];\n");
    }
    out($f, "}\n\n");

    // Generate zend_vm_set_opcode_handler_ex() function
    out($f, "ZEND_API void ZEND_FASTCALL zend_vm_set_opcode_handler_ex(zend_op* op, uint32_t op1_info, uint32_t op2_info, uint32_t res_info)\n");
    out($f, "{\n");
    out($f, "\tuint8_t opcode = zend_user_opcodes[op->opcode];\n");
    if (!ZEND_VM_SPEC) {
        out($f, "\top->handler = zend_opcode_handlers[zend_vm_get_opcode_handler_idx(opcode, op)];\n");
    } else {
        out($f, "\tuint32_t spec = zend_spec_handlers[opcode];\n");
        if (isset($used_extra_spec["TYPE"])) {
            out($f, "\tswitch (opcode) {\n");
            foreach ($opcodes as $code => $dsc) {
                if (isset($dsc['type_spec'])) {
                    $orig_op = $dsc['op'];
                    out($f, "\t\tcase $orig_op:\n");
                    if (isset($dsc["spec"]["COMMUTATIVE"])) {
                        out($f, "\t\t\tif (op->op1_type < op->op2_type) {\n");
                        out($f, "\t\t\t\tzend_swap_operands(op);\n");
                        out($f, "\t\t\t}\n");
                    }
                    $first = true;
                    foreach ($dsc['type_spec'] as $code => $condition) {
                        $condition = format_condition($condition);
                        if ($first) {
                            out($f, "\t\t\tif $condition {\n");
                            $first = false;
                        } else {
                            out($f, "\t\t\t} else if $condition {\n");
                        }
                        $spec_dsc = $opcodes[$code];
                        if (isset($spec_dsc["spec"]["NO_CONST_CONST"])) {
                            out($f, "\t\t\t\tif (op->op1_type == IS_CONST && op->op2_type == IS_CONST) {\n");
                            out($f, "\t\t\t\t\tbreak;\n");
                            out($f, "\t\t\t\t}\n");
                        }
                        out($f, "\t\t\t\tspec = {$spec_dsc['spec_code']};\n");
                        if (isset($spec_dsc["spec"]["COMMUTATIVE"]) && !isset($dsc["spec"]["COMMUTATIVE"])) {
                            out($f, "\t\t\t\tif (op->op1_type < op->op2_type) {\n");
                            out($f, "\t\t\t\t\tzend_swap_operands(op);\n");
                            out($f, "\t\t\t\t}\n");
                        }
                    }
                    if (!$first) {
                        out($f, "\t\t\t}\n");
                    }
                    out($f, "\t\t\tbreak;\n");
                }
            }
            $has_commutative = false;
            foreach ($opcodes as $code => $dsc) {
                if (!isset($dsc['is_type_spec']) &&
                    !isset($dsc['type_spec']) &&
                    isset($dsc["spec"]["COMMUTATIVE"])) {
                    $orig_op = $dsc['op'];
                    out($f, "\t\tcase $orig_op:\n");
                    $has_commutative = true;
                }
            }
            if ($has_commutative) {
                out($f, "\t\t\tif (op->op1_type < op->op2_type) {\n");
                out($f, "\t\t\t\tzend_swap_operands(op);\n");
                out($f, "\t\t\t}\n");
                out($f, "\t\t\tbreak;\n");
                out($f, "\t\tcase ZEND_USER_OPCODE:\n");
                out($f, "\t\t\tif (zend_spec_handlers[op->opcode] & SPEC_RULE_COMMUTATIVE) {\n");
                out($f, "\t\t\t\tif (op->op1_type < op->op2_type) {\n");
                out($f, "\t\t\t\t\tzend_swap_operands(op);\n");
                out($f, "\t\t\t\t}\n");
                out($f, "\t\t\t}\n");
                out($f, "\t\t\tbreak;\n");
            }
            out($f, "\t\tdefault:\n");
            out($f, "\t\t\tbreak;\n");
            out($f, "\t}\n");
        }
        out($f, "\top->handler = zend_opcode_handlers[zend_vm_get_opcode_handler_idx(spec, op)];\n");
    }
    out($f, "}\n\n");

    // Generate zend_vm_call_opcode_handler() function
    if (ZEND_VM_KIND == ZEND_VM_KIND_CALL || ZEND_VM_KIND == ZEND_VM_KIND_HYBRID) {
        out($f, "ZEND_API int ZEND_FASTCALL zend_vm_call_opcode_handler(zend_execute_data* ex)\n");
        out($f, "{\n");
        if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID) {
            out($f,"#if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID)\n");
            out($f, "\topcode_handler_t handler;\n");
            out($f,"#endif\n");
        }
        out($f, "\tint ret;\n");
        out($f, "#ifdef ZEND_VM_IP_GLOBAL_REG\n");
        out($f, "\tconst zend_op *orig_opline = opline;\n");
        out($f, "#endif\n");
        out($f, "#ifdef ZEND_VM_FP_GLOBAL_REG\n");
        out($f, "\tzend_execute_data *orig_execute_data = execute_data;\n");
        out($f, "\texecute_data = ex;\n");
        out($f, "#else\n");
        out($f, "\tzend_execute_data *execute_data = ex;\n");
        out($f, "#endif\n");
        out($f, "\n");
        out($f, "\tLOAD_OPLINE();\n");
        out($f,"#if defined(ZEND_VM_FP_GLOBAL_REG) && defined(ZEND_VM_IP_GLOBAL_REG)\n");
        if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID) {
            out($f,"#if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID)\n");
            out($f, "\thandler = (opcode_handler_t)zend_vm_get_opcode_handler_func(zend_user_opcodes[opline->opcode], opline);\n");
            out($f, "\thandler(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);\n");
            out($f, "\tif (EXPECTED(opline != &hybrid_halt_op)) {\n");
            out($f,"#else\n");
        }
        out($f, "\t((opcode_handler_t)OPLINE->handler)(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);\n");
        if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID) {
            out($f, "\tif (EXPECTED(opline)) {\n");
            out($f,"#endif\n");
        } else {
            out($f, "\tif (EXPECTED(opline)) {\n");
        }
        out($f, "\t\tret = execute_data != ex ? (int)(execute_data->prev_execute_data != ex) + 1 : 0;\n");
        out($f, "\t\tSAVE_OPLINE();\n");
        out($f, "\t} else {\n");
        out($f, "\t\tret = -1;\n");
        out($f, "\t}\n");
        out($f, "#else\n");
        out($f, "\tret = ((opcode_handler_t)OPLINE->handler)(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);\n");
        out($f, "\tSAVE_OPLINE();\n");
        out($f, "#endif\n");
        out($f, "#ifdef ZEND_VM_FP_GLOBAL_REG\n");
        out($f, "\texecute_data = orig_execute_data;\n");
        out($f, "#endif\n");
        out($f, "#ifdef ZEND_VM_IP_GLOBAL_REG\n");
        out($f, "\topline = orig_opline;\n");
        out($f, "#endif\n");
        out($f, "\treturn ret;\n");
        out($f, "}\n\n");
    } else {
        out($f, "ZEND_API int ZEND_FASTCALL zend_vm_call_opcode_handler(zend_execute_data* ex)\n");
        out($f, "{\n");
        out($f, "\tzend_error_noreturn(E_CORE_ERROR, \"zend_vm_call_opcode_handler() is not supported\");\n");
        out($f, "\treturn 0;\n");
        out($f, "}\n\n");
    }

    fclose($f);
    echo "zend_vm_execute.h generated successfully.\n";
}

function write_file_if_changed(string $filename, string $contents) {
    if (file_exists($filename)) {
        $orig_contents = file_get_contents($filename);
        if ($orig_contents === $contents) {
            // Unchanged, no need to write.
            return;
        }
    }

    file_put_contents($filename, $contents);
}

function usage() {
    echo("\nUsage: php zend_vm_gen.php [options]\n".
         "\nOptions:".
         "\n  --with-vm-kind=CALL|SWITCH|GOTO|HYBRID - select threading model (default is HYBRID)".
         "\n  --without-specializer                  - disable executor specialization".
         "\n  --with-lines                           - enable #line directives".
         "\n\n");
}

// Parse arguments
for ($i = 1; $i < $argc; $i++) {
    if (strpos($argv[$i],"--with-vm-kind=") === 0) {
        $kind = substr($argv[$i], strlen("--with-vm-kind="));
        switch ($kind) {
            case "CALL":
                define("ZEND_VM_KIND", ZEND_VM_KIND_CALL);
                break;
            case "SWITCH":
                define("ZEND_VM_KIND", ZEND_VM_KIND_SWITCH);
                break;
            case "GOTO":
                define("ZEND_VM_KIND", ZEND_VM_KIND_GOTO);
                break;
            case "HYBRID":
                define("ZEND_VM_KIND", ZEND_VM_KIND_HYBRID);
                break;
            default:
                echo("ERROR: Invalid vm kind '$kind'\n");
                usage();
                die();
        }
    } else if ($argv[$i] == "--without-specializer") {
        // Disabling specialization
        define("ZEND_VM_SPEC", 0);
    } else if ($argv[$i] == "--with-lines") {
        // Enabling debugging using original zend_vm_def.h
        define("ZEND_VM_LINES", 1);
    } else if ($argv[$i] == "--help") {
        usage();
        exit();
    } else {
        echo("ERROR: Invalid option '".$argv[$i]."'\n");
        usage();
        die();
    }
}

// Using defaults
if (!defined("ZEND_VM_KIND")) {
    // Using CALL threading by default
    define("ZEND_VM_KIND", ZEND_VM_KIND_HYBRID);
}
if (!defined("ZEND_VM_SPEC")) {
    // Using specialized executor by default
    define("ZEND_VM_SPEC", 1);
}
if (!defined("ZEND_VM_LINES")) {
    // Disabling #line directives
    define("ZEND_VM_LINES", 0);
}

gen_vm(__DIR__ . "/zend_vm_def.h", __DIR__ . "/zend_vm_execute.skl");
