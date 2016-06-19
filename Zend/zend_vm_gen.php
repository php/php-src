<?php
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+

	 $Id$
*/

$header_text = <<< DATA
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/


DATA;

/*
	This script creates zend_vm_execute.h and zend_vm_opcodes.h
	from existing zend_vm_def.h and zend_vm_execute.skl
*/

error_reporting(E_ALL);

define("ZEND_VM_KIND_CALL",   1);
define("ZEND_VM_KIND_SWITCH", 2);
define("ZEND_VM_KIND_GOTO",   3);

$vm_op_flags = array(
	"ZEND_VM_OP_SPEC"         => 1<<0,
	"ZEND_VM_OP_CONST"        => 1<<1,
	"ZEND_VM_OP_TMPVAR"       => 1<<2,
	"ZEND_VM_OP_TMPVARCV"     => 1<<3,
	"ZEND_VM_OP_MASK"         => 0xf0,
	"ZEND_VM_OP_NUM"          => 0x10,
	"ZEND_VM_OP_JMP_ADDR"     => 0x20,
	"ZEND_VM_OP_TRY_CATCH"    => 0x30,
	"ZEND_VM_OP_LIVE_RANGE"   => 0x40,
	"ZEND_VM_OP_THIS"         => 0x50,
	"ZEND_VM_OP_NEXT"         => 0x60,
	"ZEND_VM_OP_CLASS_FETCH"  => 0x70,
	"ZEND_VM_OP_CONSTRUCTOR"  => 0x80,

	"ZEND_VM_EXT_VAR_FETCH"   => 1<<16,
	"ZEND_VM_EXT_ISSET"       => 1<<17,
	"ZEND_VM_EXT_ARG_NUM"     => 1<<18,
	"ZEND_VM_EXT_ARRAY_INIT"  => 1<<19,
	"ZEND_VM_EXT_REF"         => 1<<20,
	"ZEND_VM_EXT_MASK"        => 0x0f000000,
	"ZEND_VM_EXT_NUM"         => 0x01000000,
    // unused 0x2000000
	"ZEND_VM_EXT_JMP_ADDR"    => 0x03000000,
	"ZEND_VM_EXT_DIM_OBJ"     => 0x04000000,
	"ZEND_VM_EXT_CLASS_FETCH" => 0x05000000,
	"ZEND_VM_EXT_CONST_FETCH" => 0x06000000,
	"ZEND_VM_EXT_TYPE"        => 0x07000000,
	"ZEND_VM_EXT_EVAL"        => 0x08000000,
	// unused 0x09000000,
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
	"LIVE_RANGE"           => ZEND_VM_OP_LIVE_RANGE,
	"THIS"                 => ZEND_VM_OP_THIS,
	"NEXT"                 => ZEND_VM_OP_NEXT,
	"CLASS_FETCH"          => ZEND_VM_OP_CLASS_FETCH,
	"CONSTRUCTOR"          => ZEND_VM_OP_CONSTRUCTOR,
);

$vm_ext_decode = array(
	"NUM"                  => ZEND_VM_EXT_NUM,
	"JMP_ADDR"             => ZEND_VM_EXT_JMP_ADDR,
	"DIM_OBJ"              => ZEND_VM_EXT_DIM_OBJ,
	"CLASS_FETCH"          => ZEND_VM_EXT_CLASS_FETCH,
	"CONST_FETCH"          => ZEND_VM_EXT_CONST_FETCH,
	"VAR_FETCH"            => ZEND_VM_EXT_VAR_FETCH,
	"ARRAY_INIT"           => ZEND_VM_EXT_ARRAY_INIT,
	"TYPE"                 => ZEND_VM_EXT_TYPE,
	"EVAL"                 => ZEND_VM_EXT_EVAL,
	"ISSET"                => ZEND_VM_EXT_ISSET,
	"ARG_NUM"              => ZEND_VM_EXT_ARG_NUM,
	"REF"                  => ZEND_VM_EXT_REF,
	"SRC"                  => ZEND_VM_EXT_SRC,
);

$vm_kind_name = array(
	ZEND_VM_KIND_CALL => "ZEND_VM_KIND_CALL",
	ZEND_VM_KIND_SWITCH => "ZEND_VM_KIND_SWITCH",
	ZEND_VM_KIND_GOTO => "ZEND_VM_KIND_GOTO",
);

$op_types = array(
	"ANY",
	"CONST",
	"TMP",
	"VAR",
	"UNUSED",
	"CV"
);

$op_types_ex = array(
	"ANY",
	"CONST",
	"TMP",
	"VAR",
	"UNUSED",
	"CV",
	"TMPVAR",
	"TMPVARCV",
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

$typecode = array(
	"ANY"      => 0,
	"TMP"      => 1,
	"VAR"      => 2,
	"CONST"    => 0,
	"UNUSED"   => 3,
	"CV"       => 4,
	"TMPVAR"   => 0,
	"TMPVARCV" => 0,
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

$op1_free = array(
	"ANY"      => "(free_op1 != NULL)",
	"TMP"      => "1",
	"VAR"      => "(free_op1 != NULL)",
	"CONST"    => "0",
	"UNUSED"   => "0",
	"CV"       => "0",
	"TMPVAR"   => "???",
	"TMPVARCV" => "???",
);

$op2_free = array(
	"ANY"      => "(free_op2 != NULL)",
	"TMP"      => "1",
	"VAR"      => "(free_op2 != NULL)",
	"CONST"    => "0",
	"UNUSED"   => "0",
	"CV"       => "0",
	"TMPVAR"   => "???",
	"TMPVARCV" => "???",
);

$op1_get_zval_ptr = array(
	"ANY"      => "get_zval_ptr(opline->op1_type, opline->op1, execute_data, &free_op1, \\1)",
	"TMP"      => "_get_zval_ptr_tmp(opline->op1.var, execute_data, &free_op1)",
	"VAR"      => "_get_zval_ptr_var(opline->op1.var, execute_data, &free_op1)",
	"CONST"    => "EX_CONSTANT(opline->op1)",
	"UNUSED"   => "NULL",
	"CV"       => "_get_zval_ptr_cv_\\1(execute_data, opline->op1.var)",
	"TMPVAR"   => "_get_zval_ptr_var(opline->op1.var, execute_data, &free_op1)",
	"TMPVARCV" => "???",
);

$op2_get_zval_ptr = array(
	"ANY"      => "get_zval_ptr(opline->op2_type, opline->op2, execute_data, &free_op2, \\1)",
	"TMP"      => "_get_zval_ptr_tmp(opline->op2.var, execute_data, &free_op2)",
	"VAR"      => "_get_zval_ptr_var(opline->op2.var, execute_data, &free_op2)",
	"CONST"    => "EX_CONSTANT(opline->op2)",
	"UNUSED"   => "NULL",
	"CV"       => "_get_zval_ptr_cv_\\1(execute_data, opline->op2.var)",
	"TMPVAR"   => "_get_zval_ptr_var(opline->op2.var, execute_data, &free_op2)",
	"TMPVARCV" => "???",
);

$op1_get_zval_ptr_ptr = array(
	"ANY"      => "get_zval_ptr_ptr(opline->op1_type, opline->op1, execute_data, &free_op1, \\1)",
	"TMP"      => "NULL",
	"VAR"      => "_get_zval_ptr_ptr_var(opline->op1.var, execute_data, &free_op1)",
	"CONST"    => "NULL",
	"UNUSED"   => "NULL",
	"CV"       => "_get_zval_ptr_cv_\\1(execute_data, opline->op1.var)",
	"TMPVAR"   => "???",
	"TMPVARCV" => "???",
);

$op2_get_zval_ptr_ptr = array(
	"ANY"      => "get_zval_ptr_ptr(opline->op2_type, opline->op2, execute_data, &free_op2, \\1)",
	"TMP"      => "NULL",
	"VAR"      => "_get_zval_ptr_ptr_var(opline->op2.var, execute_data, &free_op2)",
	"CONST"    => "NULL",
	"UNUSED"   => "NULL",
	"CV"       => "_get_zval_ptr_cv_\\1(execute_data, opline->op2.var)",
	"TMPVAR"   => "???",
	"TMPVARCV" => "???",
);

$op1_get_zval_ptr_deref = array(
	"ANY"      => "get_zval_ptr_deref(opline->op1_type, opline->op1, execute_data, &free_op1, \\1)",
	"TMP"      => "_get_zval_ptr_tmp(opline->op1.var, execute_data, &free_op1)",
	"VAR"      => "_get_zval_ptr_var_deref(opline->op1.var, execute_data, &free_op1)",
	"CONST"    => "EX_CONSTANT(opline->op1)",
	"UNUSED"   => "NULL",
	"CV"       => "_get_zval_ptr_cv_deref_\\1(execute_data, opline->op1.var)",
	"TMPVAR"   => "???",
	"TMPVARCV" => "???",
);

$op2_get_zval_ptr_deref = array(
	"ANY"      => "get_zval_ptr_deref(opline->op2_type, opline->op2, execute_data, &free_op2, \\1)",
	"TMP"      => "_get_zval_ptr_tmp(opline->op2.var, execute_data, &free_op2)",
	"VAR"      => "_get_zval_ptr_var_deref(opline->op2.var, execute_data, &free_op2)",
	"CONST"    => "EX_CONSTANT(opline->op2)",
	"UNUSED"   => "NULL",
	"CV"       => "_get_zval_ptr_cv_deref_\\1(execute_data, opline->op2.var)",
	"TMPVAR"   => "???",
	"TMPVARCV" => "???",
);

$op1_get_zval_ptr_undef = array(
	"ANY"      => "get_zval_ptr_undef(opline->op1_type, opline->op1, execute_data, &free_op1, \\1)",
	"TMP"      => "_get_zval_ptr_tmp(opline->op1.var, execute_data, &free_op1)",
	"VAR"      => "_get_zval_ptr_var(opline->op1.var, execute_data, &free_op1)",
	"CONST"    => "EX_CONSTANT(opline->op1)",
	"UNUSED"   => "NULL",
	"CV"       => "_get_zval_ptr_cv_undef(execute_data, opline->op1.var)",
	"TMPVAR"   => "_get_zval_ptr_var(opline->op1.var, execute_data, &free_op1)",
	"TMPVARCV" => "EX_VAR(opline->op1.var)",
);

$op2_get_zval_ptr_undef = array(
	"ANY"      => "get_zval_ptr_undef(opline->op2_type, opline->op2, execute_data, &free_op2, \\1)",
	"TMP"      => "_get_zval_ptr_tmp(opline->op2.var, execute_data, &free_op2)",
	"VAR"      => "_get_zval_ptr_var(opline->op2.var, execute_data, &free_op2)",
	"CONST"    => "EX_CONSTANT(opline->op2)",
	"UNUSED"   => "NULL",
	"CV"       => "_get_zval_ptr_cv_undef(execute_data, opline->op2.var)",
	"TMPVAR"   => "_get_zval_ptr_var(opline->op2.var, execute_data, &free_op2)",
	"TMPVARCV" => "EX_VAR(opline->op2.var)",
);

$op1_get_zval_ptr_ptr_undef = array(
	"ANY"      => "get_zval_ptr_ptr_undef(opline->op1_type, opline->op1, execute_data, &free_op1, \\1)",
	"TMP"      => "NULL",
	"VAR"      => "_get_zval_ptr_ptr_var(opline->op1.var, execute_data, &free_op1)",
	"CONST"    => "NULL",
	"UNUSED"   => "NULL",
	"CV"       => "_get_zval_ptr_cv_undef_\\1(execute_data, opline->op1.var)",
	"TMPVAR"   => "???",
	"TMPVARCV" => "EX_VAR(opline->op1.var)",
);

$op2_get_zval_ptr_ptr_undef = array(
	"ANY"      => "get_zval_ptr_ptr_undef(opline->op2_type, opline->op2, execute_data, &free_op2, \\1)",
	"TMP"      => "NULL",
	"VAR"      => "_get_zval_ptr_ptr_var(opline->op2.var, execute_data, &free_op2)",
	"CONST"    => "NULL",
	"UNUSED"   => "NULL",
	"CV"       => "_get_zval_ptr_cv_undef_\\1(execute_data, opline->op2.var)",
	"TMPVAR"   => "???",
	"TMPVARCV" => "EX_VAR(opline->op2.var)",
);

$op1_get_obj_zval_ptr = array(
	"ANY"      => "get_obj_zval_ptr(opline->op1_type, opline->op1, execute_data, &free_op1, \\1)",
	"TMP"      => "_get_zval_ptr_tmp(opline->op1.var, execute_data, &free_op1)",
	"VAR"      => "_get_zval_ptr_var(opline->op1.var, execute_data, &free_op1)",
	"CONST"    => "EX_CONSTANT(opline->op1)",
	"UNUSED"   => "_get_obj_zval_ptr_unused(execute_data)",
	"CV"       => "_get_zval_ptr_cv_\\1(execute_data, opline->op1.var)",
	"TMPVAR"   => "_get_zval_ptr_var(opline->op1.var, execute_data, &free_op1)",
	"TMPVARCV" => "???",
);

$op2_get_obj_zval_ptr = array(
	"ANY"      => "get_obj_zval_ptr(opline->op2_type, opline->op2, execute_data, &free_op2, \\1)",
	"TMP"      => "_get_zval_ptr_tmp(opline->op2.var, execute_data, &free_op2)",
	"VAR"      => "_get_zval_ptr_var(opline->op2.var, execute_data, &free_op2)",
	"CONST"    => "EX_CONSTANT(opline->op2)",
	"UNUSED"   => "_get_obj_zval_ptr_unused(execute_data)",
	"CV"       => "_get_zval_ptr_cv_\\1(execute_data, opline->op2.var)",
	"TMPVAR"   => "_get_zval_ptr_var(opline->op2.var, execute_data, &free_op2)",
	"TMPVARCV" => "???",
);

$op1_get_obj_zval_ptr_undef = array(
	"ANY"      => "get_obj_zval_ptr_undef(opline->op1_type, opline->op1, execute_data, &free_op1, \\1)",
	"TMP"      => "_get_zval_ptr_tmp(opline->op1.var, execute_data, &free_op1)",
	"VAR"      => "_get_zval_ptr_var(opline->op1.var, execute_data, &free_op1)",
	"CONST"    => "EX_CONSTANT(opline->op1)",
	"UNUSED"   => "_get_obj_zval_ptr_unused(execute_data)",
	"CV"       => "_get_zval_ptr_cv_undef(execute_data, opline->op1.var)",
	"TMPVAR"   => "_get_zval_ptr_var(opline->op1.var, execute_data, &free_op1)",
	"TMPVARCV" => "EX_VAR(opline->op1.var)",
);

$op2_get_obj_zval_ptr_undef = array(
	"ANY"      => "get_obj_zval_ptr_undef(opline->op2_type, opline->op2, execute_data, &free_op2, \\1)",
	"TMP"      => "_get_zval_ptr_tmp(opline->op2.var, execute_data, &free_op2)",
	"VAR"      => "_get_zval_ptr_var(opline->op2.var, execute_data, &free_op2)",
	"CONST"    => "EX_CONSTANT(opline->op2)",
	"UNUSED"   => "_get_obj_zval_ptr_unused(execute_data)",
	"CV"       => "_get_zval_ptr_cv_undef(execute_data, opline->op2.var)",
	"TMPVAR"   => "_get_zval_ptr_var(opline->op2.var, execute_data, &free_op2)",
	"TMPVARCV" => "EX_VAR(opline->op2.var)",
);

$op1_get_obj_zval_ptr_deref = array(
	"ANY"      => "get_obj_zval_ptr(opline->op1_type, opline->op1, execute_data, &free_op1, \\1)",
	"TMP"      => "_get_zval_ptr_tmp(opline->op1.var, execute_data, &free_op1)",
	"VAR"      => "_get_zval_ptr_var_deref(opline->op1.var, execute_data, &free_op1)",
	"CONST"    => "EX_CONSTANT(opline->op1)",
	"UNUSED"   => "_get_obj_zval_ptr_unused(execute_data)",
	"CV"       => "_get_zval_ptr_cv_deref_\\1(execute_data, opline->op1.var)",
	"TMPVAR"   => "???",
	"TMPVARCV" => "???",
);

$op2_get_obj_zval_ptr_deref = array(
	"ANY"      => "get_obj_zval_ptr(opline->op2_type, opline->op2, execute_data, &free_op2, \\1)",
	"TMP"      => "_get_zval_ptr_tmp(opline->op2.var, execute_data, &free_op2)",
	"VAR"      => "_get_zval_ptr_var_deref(opline->op2.var, execute_data, &free_op2)",
	"CONST"    => "EX_CONSTANT(opline->op2)",
	"UNUSED"   => "_get_obj_zval_ptr_unused(execute_data)",
	"CV"       => "_get_zval_ptr_cv_deref_\\1(execute_data, opline->op2.var)",
	"TMPVAR"   => "???",
	"TMPVARCV" => "???",
);

$op1_get_obj_zval_ptr_ptr = array(
	"ANY"      => "get_obj_zval_ptr_ptr(opline->op1_type, opline->op1, execute_data, &free_op1, \\1)",
	"TMP"      => "NULL",
	"VAR"      => "_get_zval_ptr_ptr_var(opline->op1.var, execute_data, &free_op1)",
	"CONST"    => "NULL",
	"UNUSED"   => "_get_obj_zval_ptr_unused(execute_data)",
	"CV"       => "_get_zval_ptr_cv_\\1(execute_data, opline->op1.var)",
	"TMPVAR"   => "???",
	"TMPVARCV" => "???",
);

$op2_get_obj_zval_ptr_ptr = array(
	"ANY"      => "get_obj_zval_ptr_ptr(opline->op2_type, opline->op2, execute_data, &free_op2, \\1)",
	"TMP"      => "NULL",
	"VAR"      => "_get_zval_ptr_ptr_var(opline->op2.var, execute_data, &free_op2)",
	"CONST"    => "NULL",
	"UNUSED"   => "_get_obj_zval_ptr_unused(execute_data)",
	"CV"       => "_get_zval_ptr_cv_\\1(execute_data, opline->op2.var)",
	"TMPVAR"   => "???",
	"TMPVARCV" => "???",
);

$op1_get_obj_zval_ptr_ptr_undef = array(
	"ANY"      => "get_obj_zval_ptr_ptr(opline->op1_type, opline->op1, execute_data, &free_op1, \\1)",
	"TMP"      => "NULL",
	"VAR"      => "_get_zval_ptr_ptr_var(opline->op1.var, execute_data, &free_op1)",
	"CONST"    => "NULL",
	"UNUSED"   => "_get_obj_zval_ptr_unused(execute_data)",
	"CV"       => "_get_zval_ptr_cv_undef_\\1(execute_data, opline->op1.var)",
	"TMPVAR"   => "???",
	"TMPVARCV" => "EX_VAR(opline->op1.var)",
);

$op2_get_obj_zval_ptr_ptr_undef = array(
	"ANY"      => "get_obj_zval_ptr_ptr(opline->op2_type, opline->op2, execute_data, &free_op2, \\1)",
	"TMP"      => "NULL",
	"VAR"      => "_get_zval_ptr_ptr_var(opline->op2.var, execute_data, &free_op2)",
	"CONST"    => "NULL",
	"UNUSED"   => "_get_obj_zval_ptr_unused(execute_data)",
	"CV"       => "_get_zval_ptr_cv_undef_\\1(execute_data, opline->op2.var)",
	"TMPVAR"   => "???",
	"TMPVARCV" => "EX_VAR(opline->op2.var)",
);

$op1_free_op = array(
	"ANY"      => "FREE_OP(free_op1)",
	"TMP"      => "zval_ptr_dtor_nogc(free_op1)",
	"VAR"      => "zval_ptr_dtor_nogc(free_op1)",
	"CONST"    => "",
	"UNUSED"   => "",
	"CV"       => "",
	"TMPVAR"   => "zval_ptr_dtor_nogc(free_op1)",
	"TMPVARCV" => "???",
);

$op2_free_op = array(
	"ANY"      => "FREE_OP(free_op2)",
	"TMP"      => "zval_ptr_dtor_nogc(free_op2)",
	"VAR"      => "zval_ptr_dtor_nogc(free_op2)",
	"CONST"    => "",
	"UNUSED"   => "",
	"CV"       => "",
	"TMPVAR"   => "zval_ptr_dtor_nogc(free_op2)",
	"TMPVARCV" => "???",
);

$op1_free_op_if_var = array(
	"ANY"      => "if (opline->op1_type == IS_VAR) {zval_ptr_dtor_nogc(free_op1);}",
	"TMP"      => "",
	"VAR"      => "zval_ptr_dtor_nogc(free_op1)",
	"CONST"    => "",
	"UNUSED"   => "",
	"CV"       => "",
	"TMPVAR"   => "???",
	"TMPVARCV" => "???",
);

$op2_free_op_if_var = array(
	"ANY"      => "if (opline->op2_type == IS_VAR) {zval_ptr_dtor_nogc(free_op2);}",
	"TMP"      => "",
	"VAR"      => "zval_ptr_dtor_nogc(free_op2)",
	"CONST"    => "",
	"UNUSED"   => "",
	"CV"       => "",
	"TMPVAR"   => "???",
	"TMPVARCV" => "???",
);

$op1_free_op_var_ptr = array(
	"ANY"      => "if (free_op1) {zval_ptr_dtor_nogc(free_op1);}",
	"TMP"      => "",
	"VAR"      => "if (UNEXPECTED(free_op1)) {zval_ptr_dtor_nogc(free_op1);}",
	"CONST"    => "",
	"UNUSED"   => "",
	"CV"       => "",
	"TMPVAR"   => "???",
	"TMPVARCV" => "???",
);

$op2_free_op_var_ptr = array(
	"ANY"      => "if (free_op2) {zval_ptr_dtor_nogc(free_op2);}",
	"TMP"      => "",
	"VAR"      => "if (UNEXPECTED(free_op2)) {zval_ptr_dtor_nogc(free_op2);}",
	"CONST"    => "",
	"UNUSED"   => "",
	"CV"       => "",
	"TMPVAR"   => "???",
	"TMPVARCV" => "???",
);

$op1_free_unfetched = array(
	"ANY"      => "FREE_UNFETCHED_OP(opline->op1_type, opline->op1.var)",
	"TMP"      => "zval_ptr_dtor_nogc(EX_VAR(opline->op1.var))",
	"VAR"      => "zval_ptr_dtor_nogc(EX_VAR(opline->op1.var))",
	"CONST"    => "",
	"UNUSED"   => "",
	"CV"       => "",
	"TMPVAR"   => "zval_ptr_dtor_nogc(EX_VAR(opline->op1.var))",
	"TMPVARCV" => "???",
);

$op2_free_unfetched = array(
	"ANY"      => "FREE_UNFETCHED_OP(opline->op2_type, opline->op2.var)",
	"TMP"      => "zval_ptr_dtor_nogc(EX_VAR(opline->op2.var))",
	"VAR"      => "zval_ptr_dtor_nogc(EX_VAR(opline->op2.var))",
	"CONST"    => "",
	"UNUSED"   => "",
	"CV"       => "",
	"TMPVAR"   => "zval_ptr_dtor_nogc(EX_VAR(opline->op2.var))",
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
	"ANY"      => "get_zval_ptr((opline+1)->op1_type, (opline+1)->op1, execute_data, &free_op_data, \\1)",
	"TMP"      => "_get_zval_ptr_tmp((opline+1)->op1.var, execute_data, &free_op_data)",
	"VAR"      => "_get_zval_ptr_var((opline+1)->op1.var, execute_data, &free_op_data)",
	"CONST"    => "EX_CONSTANT((opline+1)->op1)",
	"UNUSED"   => "NULL",
	"CV"       => "_get_zval_ptr_cv_\\1(execute_data, (opline+1)->op1.var)",
	"TMPVAR"   => "_get_zval_ptr_var((opline+1)->op1.var, execute_data, &free_op_data)",
	"TMPVARCV" => "???",
);

$op_data_get_zval_ptr_deref = array(
	"ANY"      => "get_zval_ptr((opline+1)->op1_type, (opline+1)->op1, execute_data, &free_op_data, \\1)",
	"TMP"      => "_get_zval_ptr_tmp((opline+1)->op1.var, execute_data, &free_op_data)",
	"VAR"      => "_get_zval_ptr_var_deref((opline+1)->op1.var, execute_data, &free_op_data)",
	"CONST"    => "EX_CONSTANT((opline+1)->op1)",
	"UNUSED"   => "NULL",
	"CV"       => "_get_zval_ptr_cv_deref_\\1(execute_data, (opline+1)->op1.var)",
	"TMPVAR"   => "???",
	"TMPVARCV" => "???",
);

$op_data_free_op = array(
	"ANY"      => "FREE_OP(free_op_data)",
	"TMP"      => "zval_ptr_dtor_nogc(free_op_data)",
	"VAR"      => "zval_ptr_dtor_nogc(free_op_data)",
	"CONST"    => "",
	"UNUSED"   => "",
	"CV"       => "",
	"TMPVAR"   => "zval_ptr_dtor_nogc(free_op_data)",
	"TMPVARCV" => "???",
);

$op_data_free_unfetched = array(
	"ANY"      => "FREE_UNFETCHED_OP((opline+1)->op1_type, (opline+1)->op1.var)",
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

// Returns name of specialized helper
function helper_name($name, $spec, $op1, $op2) {
	global $prefix, $helpers;

	if (isset($helpers[$name])) {
		// If we haven't helper with specified spicialized operands then
		// using unspecialized helper
		if (!isset($helpers[$name]["op1"][$op1]) &&
		    isset($helpers[$name]["op1"]["ANY"])) {
			$op1 = "ANY";
		}
		if (!isset($helpers[$name]["op2"][$op2]) &&
		    isset($helpers[$name]["op2"]["ANY"])) {
			$op2 = "ANY";
		}
	}
	return $name.($spec?"_SPEC":"").$prefix[$op1].$prefix[$op2];
}

function opcode_name($name, $spec, $op1, $op2) {
	global $prefix, $opnames, $opcodes;

	if (isset($opnames[$name])) {
		$opcode = $opcodes[$opnames[$name]];
		// If we haven't helper with specified spicialized operands then
		// using unspecialized helper
		if (!isset($opcode["op1"][$op1]) &&
		    isset($opcode["op1"]["ANY"])) {
			$op1 = "ANY";
		}
		if (!isset($opcode["op2"][$op2]) &&
		    isset($opcode["op2"]["ANY"])) {
			$op2 = "ANY";
		}
	}
	return $name.($spec?"_SPEC":"").$prefix[$op1].$prefix[$op2];
}

// Generates code for opcode handler or helper
function gen_code($f, $spec, $kind, $export, $code, $op1, $op2, $name, $extra_spec=null) {
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
		$op1_free, $op2_free, $op1_free_unfetched, $op2_free_unfetched,
		$op1_free_op, $op2_free_op, $op1_free_op_if_var, $op2_free_op_if_var,
		$op1_free_op_var_ptr, $op2_free_op_var_ptr, $prefix,
		$op_data_type, $op_data_get_zval_ptr, $op_data_get_zval_ptr_deref,
		$op_data_free_op, $op_data_free_unfetched;

	// Specializing
	$code = preg_replace(
		array(
			"/OP1_TYPE/",
			"/OP2_TYPE/",
			"/OP1_FREE/",
			"/OP2_FREE/",
			"/GET_OP1_ZVAL_PTR\(([^)]*)\)/",
			"/GET_OP2_ZVAL_PTR\(([^)]*)\)/",
			"/GET_OP1_ZVAL_PTR_DEREF\(([^)]*)\)/",
			"/GET_OP2_ZVAL_PTR_DEREF\(([^)]*)\)/",
			"/GET_OP1_ZVAL_PTR_UNDEF\(([^)]*)\)/",
			"/GET_OP2_ZVAL_PTR_UNDEF\(([^)]*)\)/",
			"/GET_OP1_ZVAL_PTR_PTR\(([^)]*)\)/",
			"/GET_OP2_ZVAL_PTR_PTR\(([^)]*)\)/",
			"/GET_OP1_ZVAL_PTR_PTR_UNDEF\(([^)]*)\)/",
			"/GET_OP2_ZVAL_PTR_PTR_UNDEF\(([^)]*)\)/",
			"/GET_OP1_OBJ_ZVAL_PTR\(([^)]*)\)/",
			"/GET_OP2_OBJ_ZVAL_PTR\(([^)]*)\)/",
			"/GET_OP1_OBJ_ZVAL_PTR_UNDEF\(([^)]*)\)/",
			"/GET_OP2_OBJ_ZVAL_PTR_UNDEF\(([^)]*)\)/",
			"/GET_OP1_OBJ_ZVAL_PTR_DEREF\(([^)]*)\)/",
			"/GET_OP2_OBJ_ZVAL_PTR_DEREF\(([^)]*)\)/",
			"/GET_OP1_OBJ_ZVAL_PTR_PTR\(([^)]*)\)/",
			"/GET_OP2_OBJ_ZVAL_PTR_PTR\(([^)]*)\)/",
			"/GET_OP1_OBJ_ZVAL_PTR_PTR_UNDEF\(([^)]*)\)/",
			"/GET_OP2_OBJ_ZVAL_PTR_PTR_UNDEF\(([^)]*)\)/",
			"/FREE_OP1\(\)/",
			"/FREE_OP2\(\)/",
			"/FREE_OP1_IF_VAR\(\)/",
			"/FREE_OP2_IF_VAR\(\)/",
			"/FREE_OP1_VAR_PTR\(\)/",
			"/FREE_OP2_VAR_PTR\(\)/",
			"/FREE_UNFETCHED_OP1\(\)/",
			"/FREE_UNFETCHED_OP2\(\)/",
			"/^#(\s*)ifdef\s+ZEND_VM_SPEC\s*\n/m",
			"/^#(\s*)ifndef\s+ZEND_VM_SPEC\s*\n/m",
			"/\!defined\(ZEND_VM_SPEC\)/m",
			"/defined\(ZEND_VM_SPEC\)/m",
			"/ZEND_VM_C_LABEL\(\s*([A-Za-z_]*)\s*\)/m",
			"/ZEND_VM_C_GOTO\(\s*([A-Za-z_]*)\s*\)/m",
			"/^#(\s*)if\s+1\s*\\|\\|.*[^\\\\]$/m",
			"/^#(\s*)if\s+0\s*&&.*[^\\\\]$/m",
			"/^#(\s*)ifdef\s+ZEND_VM_EXPORT\s*\n/m",
			"/^#(\s*)ifndef\s+ZEND_VM_EXPORT\s*\n/m",
			"/OP_DATA_TYPE/",
			"/GET_OP_DATA_ZVAL_PTR\(([^)]*)\)/",
			"/GET_OP_DATA_ZVAL_PTR_DEREF\(([^)]*)\)/",
			"/FREE_OP_DATA\(\)/",
			"/FREE_UNFETCHED_OP_DATA\(\)/",
			"/RETURN_VALUE_USED\(opline\)/",
			"/arg_num <= MAX_ARG_FLAG_NUM/",
			"/ZEND_VM_SMART_BRANCH\(\s*([^,)]*)\s*,\s*([^)]*)\s*\)/",
			"/opline->extended_value\s*==\s*0/",
			"/opline->extended_value\s*==\s*ZEND_ASSIGN_DIM/",
			"/opline->extended_value\s*==\s*ZEND_ASSIGN_OBJ/",
		),
		array(
			$op1_type[$op1],
			$op2_type[$op2],
			$op1_free[$op1],
			$op2_free[$op2],
			$op1_get_zval_ptr[$op1],
			$op2_get_zval_ptr[$op2],
			$op1_get_zval_ptr_deref[$op1],
			$op2_get_zval_ptr_deref[$op2],
			$op1_get_zval_ptr_undef[$op1],
			$op2_get_zval_ptr_undef[$op2],
			$op1_get_zval_ptr_ptr[$op1],
			$op2_get_zval_ptr_ptr[$op2],
			$op1_get_zval_ptr_ptr_undef[$op1],
			$op2_get_zval_ptr_ptr_undef[$op2],
			$op1_get_obj_zval_ptr[$op1],
			$op2_get_obj_zval_ptr[$op2],
			$op1_get_obj_zval_ptr_undef[$op1],
			$op2_get_obj_zval_ptr_undef[$op2],
			$op1_get_obj_zval_ptr_deref[$op1],
			$op2_get_obj_zval_ptr_deref[$op2],
			$op1_get_obj_zval_ptr_ptr[$op1],
			$op2_get_obj_zval_ptr_ptr[$op2],
			$op1_get_obj_zval_ptr_ptr_undef[$op1],
			$op2_get_obj_zval_ptr_ptr_undef[$op2],
			$op1_free_op[$op1],
			$op2_free_op[$op2],
			$op1_free_op_if_var[$op1],
			$op2_free_op_if_var[$op2],
			$op1_free_op_var_ptr[$op1],
			$op2_free_op_var_ptr[$op2],
			$op1_free_unfetched[$op1],
			$op2_free_unfetched[$op2],
			($op1!="ANY"||$op2!="ANY")?"#\\1if 1\n":"#\\1if 0\n",
			($op1!="ANY"||$op2!="ANY")?"#\\1if 0\n":"#\\1if 1\n",
			($op1!="ANY"||$op2!="ANY")?"0":"1",
			($op1!="ANY"||$op2!="ANY")?"1":"0",
			"\\1".(($spec && $kind != ZEND_VM_KIND_CALL)?("_SPEC".$prefix[$op1].$prefix[$op2].extra_spec_name($extra_spec)):""),
			"goto \\1".(($spec && $kind != ZEND_VM_KIND_CALL)?("_SPEC".$prefix[$op1].$prefix[$op2].extra_spec_name($extra_spec)):""),
			"#\\1if 1",
			"#\\1if 0",
			$export?"#\\1if 1\n":"#\\1if 0\n",
			$export?"#\\1if 0\n":"#\\1if 1\n",
			$op_data_type[isset($extra_spec['OP_DATA']) ? $extra_spec['OP_DATA'] : "ANY"],
			$op_data_get_zval_ptr[isset($extra_spec['OP_DATA']) ? $extra_spec['OP_DATA'] : "ANY"],
			$op_data_get_zval_ptr_deref[isset($extra_spec['OP_DATA']) ? $extra_spec['OP_DATA'] : "ANY"],
			$op_data_free_op[isset($extra_spec['OP_DATA']) ? $extra_spec['OP_DATA'] : "ANY"],
			$op_data_free_unfetched[isset($extra_spec['OP_DATA']) ? $extra_spec['OP_DATA'] : "ANY"],
			isset($extra_spec['RETVAL']) ? $extra_spec['RETVAL'] : "RETURN_VALUE_USED(opline)",
			isset($extra_spec['QUICK_ARG']) ? $extra_spec['QUICK_ARG'] : "arg_num <= MAX_ARG_FLAG_NUM",
			isset($extra_spec['SMART_BRANCH']) ?
				($extra_spec['SMART_BRANCH'] == 1 ?
						"ZEND_VM_SMART_BRANCH_JMPZ(\\1, \\2)"
					:	($extra_spec['SMART_BRANCH'] == 2 ?
							"ZEND_VM_SMART_BRANCH_JMPNZ(\\1, \\2)" : ""))
				: 	"ZEND_VM_SMART_BRANCH(\\1, \\2)",
			isset($extra_spec['DIM_OBJ']) ?
				($extra_spec['DIM_OBJ'] == 0 ? "1" : "0")
				: "\\0",
			isset($extra_spec['DIM_OBJ']) ?
				($extra_spec['DIM_OBJ'] == 1 ? "1" : "0")
				: "\\0",
			isset($extra_spec['DIM_OBJ']) ?
				($extra_spec['DIM_OBJ'] == 2 ? "1" : "0")
				: "\\0",
		),
		$code);

	if (0 && strpos($code, '{') === 0) {
		$code = "{\n\tfprintf(stderr, \"$name\\n\");\n" . substr($code, 1);
	}
	// Updating code according to selected threading model
	switch($kind) {
		case ZEND_VM_KIND_CALL:
			$code = preg_replace_callback(
				array(
					"/EXECUTE_DATA/m",
					"/ZEND_VM_DISPATCH_TO_HANDLER\(\s*([A-Z_]*)\s*\)/m",
					"/ZEND_VM_DISPATCH_TO_HELPER\(\s*([A-Za-z_]*)\s*(,[^)]*)?\)/m",
				),
				function($matches) use ($spec, $prefix, $op1, $op2) {
					if (strncasecmp($matches[0], "EXECUTE_DATA", strlen("EXECUTE_DATA")) == 0) {
						return "execute_data";
					} else if (strncasecmp($matches[0], "ZEND_VM_DISPATCH_TO_HANDLER", strlen("ZEND_VM_DISPATCH_TO_HANDLER")) == 0) {
						return "ZEND_VM_TAIL_CALL(" . opcode_name($matches[1], $spec, $op1, $op2) . "_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU))";
					} else {
						// ZEND_VM_DISPATCH_TO_HELPER
						if (isset($matches[2])) {
							// extra args
							$args = substr(preg_replace("/,\s*[A-Za-z_]*\s*,\s*([^,)\s]*)\s*/", ", $1", $matches[2]), 2);
							return "ZEND_VM_TAIL_CALL(" . helper_name($matches[1], $spec, $op1, $op2) . "(" . $args. " ZEND_OPCODE_HANDLER_ARGS_PASSTHRU_CC))";
						}
						return "ZEND_VM_TAIL_CALL(" . helper_name($matches[1], $spec, $op1, $op2) . "(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU))";
					}
				},
				$code);
			break;
		case ZEND_VM_KIND_SWITCH:
			$code = preg_replace_callback(
				array(
					"/EXECUTE_DATA/m",
					"/ZEND_VM_DISPATCH_TO_HANDLER\(\s*([A-Z_]*)\s*\)/m",
					"/ZEND_VM_DISPATCH_TO_HELPER\(\s*([A-Za-z_]*)\s*(,[^)]*)?\)/m",
				),
				function($matches) use ($spec, $prefix, $op1, $op2) {
					if (strncasecmp($matches[0], "EXECUTE_DATA", strlen("EXECUTE_DATA")) == 0) {
						return "execute_data";
					} else if (strncasecmp($matches[0], "ZEND_VM_DISPATCH_TO_HANDLER", strlen("ZEND_VM_DISPATCH_TO_HANDLER")) == 0) {
						return "goto " . opcode_name($matches[1], $spec, $op1, $op2) . "_LABEL";
					} else {
						// ZEND_VM_DISPATCH_TO_HELPER
						if (isset($matches[2])) {
							// extra args
							$args = preg_replace("/,\s*([A-Za-z_]*)\s*,\s*([^,)\s]*)\s*/", "$1 = $2; ", $matches[2]);
							return $args .  "goto " . helper_name($matches[1], $spec, $op1, $op2);
						}
						return "goto " . helper_name($matches[1], $spec, $op1, $op2);
					}
				},
					$code);
			break;
		case ZEND_VM_KIND_GOTO:
			$code = preg_replace_callback(
				array(
					"/EXECUTE_DATA/m",
					"/ZEND_VM_DISPATCH_TO_HANDLER\(\s*([A-Z_]*)\s*\)/m",
					"/ZEND_VM_DISPATCH_TO_HELPER\(\s*([A-Za-z_]*)\s*(,[^)]*)?\)/m",
				),
				function($matches) use ($spec, $prefix, $op1, $op2) {
					if (strncasecmp($matches[0], "EXECUTE_DATA", strlen("EXECUTE_DATA")) == 0) {
						return "execute_data";
					} else if (strncasecmp($matches[0], "ZEND_VM_DISPATCH_TO_HANDLER", strlen("ZEND_VM_DISPATCH_TO_HANDLER")) == 0) {
						return "goto " . opcode_name($matches[1], $spec, $op1, $op2) . "_HANDLER";
					} else {
						// ZEND_VM_DISPATCH_TO_HELPER
						if (isset($matches[2])) {
							// extra args
							$args = preg_replace("/,\s*([A-Za-z_]*)\s*,\s*([^,)\s]*)\s*/", "$1 = $2; ", $matches[2]);
							return $args .  "goto " . helper_name($matches[1], $spec, $op1, $op2);
						}
						return "goto " . helper_name($matches[1], $spec, $op1, $op2);
					}
				},
					$code);
			break;
	}

	/* Remove unused free_op1 and free_op2 declarations */
	if ($spec && preg_match_all('/^\s*zend_free_op\s+[^;]+;\s*$/me', $code, $matches, PREG_SET_ORDER)) {
		$n = 0;
		foreach ($matches as $match) {
		  $code = preg_replace('/'.preg_quote($match[0],'/').'/', "\$D$n", $code);
		  ++$n;
		}
		$del_free_op1 = (strpos($code, "free_op1") === false);
		$del_free_op2 = (strpos($code, "free_op2") === false);
		$del_free_op_data = (strpos($code, "free_op_data") === false);
		$n = 0;
		foreach ($matches as $match) {
			$dcl = $match[0];
			$changed = 0;
			if ($del_free_op1 && strpos($dcl, "free_op1") !== false) {
				$dcl = preg_replace("/free_op1\s*,\s*/", "", $dcl);
				$dcl = preg_replace("/free_op1\s*;/", ";", $dcl);
				$changed = 1;
			}
			if ($del_free_op2 && strpos($dcl, "free_op2") !== false) {
				$dcl = preg_replace("/free_op2\s*,\s*/", "", $dcl);
				$dcl = preg_replace("/free_op2\s*;/", ";", $dcl);
				$changed = 1;
			}
			if ($del_free_op_data && strpos($dcl, "free_op_data") !== false) {
				$dcl = preg_replace("/free_op_data\s*,\s*/", "", $dcl);
				$dcl = preg_replace("/free_op_data\s*;/", ";", $dcl);
				$changed = 1;
			}
			if ($changed) {
				$dcl = preg_replace("/,\s*;/", ";", $dcl);
				$dcl = preg_replace("/zend_free_op\s*;/", "", $dcl);
			}
		  $code = preg_replace("/\\\$D$n/", $dcl, $code);
		  ++$n;
		}
	}

	/* Remove unnecessary ';' */
	$code = preg_replace('/^\s*;\s*$/m', '', $code);

	/* Remove WS */
	$code = preg_replace('/[ \t]+\n/m', "\n", $code);

	out($f, $code);
}

// Generates opcode handler
function gen_handler($f, $spec, $kind, $name, $op1, $op2, $use, $code, $lineno, $extra_spec = null, &$switch_labels = array()) {
	global $definition_file, $prefix, $typecode, $opnames, $commutative_order;

	if ($spec &&
	    isset($extra_spec["NO_CONST_CONST"]) &&
	    $op1 == "CONST" && $op2 == "CONST") {
	    // Skip useless constant handlers
		return;
	}

	if ($spec &&
	    isset($extra_spec["COMMUTATIVE"]) &&
	    $commutative_order[$op1] > $commutative_order[$op2]) {
	    // Skip duplicate commutative handlers
		return;
	}

	if ($spec &&
	    isset($extra_spec["DIM_OBJ"]) &&
		(($op2 == "UNUSED" && $extra_spec["DIM_OBJ"] != 1) ||
		 ($op1 == "UNUSED" && $extra_spec["DIM_OBJ"] != 2))) {
	    // Skip useless handlers
		return;
	}

	if (ZEND_VM_LINES) {
		out($f, "#line $lineno \"$definition_file\"\n");
	}

	// Generate opcode handler's entry point according to selected threading model
	$spec_name = $name.($spec?"_SPEC":"").$prefix[$op1].$prefix[$op2].($spec?extra_spec_name($extra_spec):"");
	switch($kind) {
		case ZEND_VM_KIND_CALL:
			out($f,"static ZEND_OPCODE_HANDLER_RET ZEND_FASTCALL {$spec_name}_HANDLER(ZEND_OPCODE_HANDLER_ARGS)\n");
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
				out($f," {$spec_name}_LABEL:\n");
			} else {
				out($f,"\n");
			}
			break;
		case ZEND_VM_KIND_GOTO:
			out($f,"{$spec_name}_HANDLER: ZEND_VM_GUARD($spec_name);\n");
			break;
	}

	// Generate opcode handler's code
	gen_code($f, $spec, $kind, 0, $code, $op1, $op2, $name, $extra_spec);
}

// Generates helper
function gen_helper($f, $spec, $kind, $name, $op1, $op2, $param, $code, $lineno, $inline) {
	global $definition_file, $prefix;

	if (ZEND_VM_LINES) {
		out($f, "#line $lineno \"$definition_file\"\n");
	}

	// Generate helper's entry point according to selected threading model
	switch($kind) {
		case ZEND_VM_KIND_CALL:
			if ($inline) {
				$zend_always_inline = " zend_always_inline";
				$zend_fastcall = "";
			} else {
				$zend_always_inline = "";
				$zend_fastcall = " ZEND_FASTCALL";
			}
			if ($param == null) {
			  // Helper without parameters
				out($f, "static$zend_always_inline ZEND_OPCODE_HANDLER_RET$zend_fastcall ".$name.($spec?"_SPEC":"").$prefix[$op1].$prefix[$op2]."(ZEND_OPCODE_HANDLER_ARGS)\n");
			} else {
			  // Helper with parameter
				out($f, "static$zend_always_inline ZEND_OPCODE_HANDLER_RET$zend_fastcall ".$name.($spec?"_SPEC":"").$prefix[$op1].$prefix[$op2]."(".$param." ZEND_OPCODE_HANDLER_ARGS_DC)\n");
			}
			break;
		case ZEND_VM_KIND_SWITCH:
			out($f, $name.($spec?"_SPEC":"").$prefix[$op1].$prefix[$op2].":\n");
			break;
		case ZEND_VM_KIND_GOTO:
			out($f, $name.($spec?"_SPEC":"").$prefix[$op1].$prefix[$op2].":\n");
			break;
	}

	// Generate helper's code
	gen_code($f, $spec, $kind, 0, $code, $op1, $op2, $name);
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
			out($f,$prolog."(void*)&&ZEND_NULL_HANDLER,\n");
			break;
	}
}

// Generates array of opcode handlers (specialized or unspecialized)
function gen_labels($f, $spec, $kind, $prolog, &$specs, $switch_labels = array()) {
	global $opcodes, $op_types, $prefix;

	$next = 0;
	$label = 0;
	if ($spec) {
	  // Emit labels for specialized executor

	  // For each opcode in opcode number order
		foreach($opcodes as $num => $dsc) {
			$specs[$num] = "$label";
			$spec_op1 = $spec_op2 = $spec_extra = false;
			$next = $num + 1;
			$diff = array_diff_key(array_flip($op_types), isset($dsc["op1"]) ? $dsc["op1"] : array());
			if ((count($diff) == count($op_types) - 1 ? isset($diff["ANY"]) : count($diff) != count($op_types)) || isset($dsc["op1"]["TMPVAR"]) || isset($dsc["op1"]["TMPVARCV"])) {
				$spec_op1 = true;
				$specs[$num] .= " | SPEC_RULE_OP1";
			}
			$diff = array_diff_key(array_flip($op_types), isset($dsc["op2"]) ? $dsc["op2"] : array());
			if ((count($diff) == count($op_types) - 1 ? isset($diff["ANY"]) : count($diff) != count($op_types)) || isset($dsc["op2"]["TMPVAR"]) || isset($dsc["op2"]["TMPVARCV"])) {
				$spec_op2 = true;
				$specs[$num] .= " | SPEC_RULE_OP2";
			}
			$spec_extra = call_user_func_array("array_merge", extra_spec_handler($dsc) ?: array(array()));
			$flags = extra_spec_flags($spec_extra);
			if ($flags) {
				$specs[$num] .= " | ".implode("|", $flags);
			}
			if ($num >= 256) {
				$opcodes[$num]['spec_code'] = $specs[$num];
				unset($specs[$num]);
			}

			$foreach_op1 = function($do) use ($dsc, $op_types) {
				return function() use ($do, $dsc, $op_types) {
					// For each op1.op_type except ANY
					foreach($op_types as $op1) {
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
							$do($op1, "ANY");
						}
					}
				};
			};
			$foreach_op2 = function($do) use ($dsc, $op_types) {
				return function($op1) use ($do, $dsc, $op_types) {
					// For each op2.op_type except ANY
					foreach($op_types as $op2) {
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
					foreach($op_types as $op_data) {
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
			$generate = function ($op1, $op2, $extra_spec = array()) use ($f, $kind, $dsc, $prefix, $prolog, $num, $switch_labels, &$label) {
				global $typecode, $commutative_order;

				// Check if specialized handler is defined
				/* TODO: figure out better way to signal "specialized and not defined" than an extra lookup */
				if (isset($dsc["op1"][$op1]) &&
				    isset($dsc["op2"][$op2]) &&
				    (!isset($extra_spec["OP_DATA"]) || isset($dsc["spec"]["OP_DATA"][$extra_spec["OP_DATA"]]))) {

					if (isset($extra_spec["NO_CONST_CONST"]) &&
					    $op1 == "CONST" && $op2 == "CONST") {
					    // Skip useless constant handlers
						gen_null_label($f, $kind, $prolog);
						$label++;
						return;
					} else if (isset($extra_spec["COMMUTATIVE"]) &&
					    $commutative_order[$op1] > $commutative_order[$op2]) {
					    // Skip duplicate commutative handlers
						gen_null_label($f, $kind, $prolog);
						$label++;
						return;
					} else if (isset($extra_spec["DIM_OBJ"]) &&
						(($op2 == "UNUSED" && $extra_spec["DIM_OBJ"] != 1) ||
						 ($op1 == "UNUSED" && $extra_spec["DIM_OBJ"] != 2))) {
					    // Skip useless handlers
						gen_null_label($f, $kind, $prolog);
						$label++;
						return;
					}
					
					// Emit pointer to specialized handler
					$spec_name = $dsc["op"]."_SPEC".$prefix[$op1].$prefix[$op2].extra_spec_name($extra_spec);
					switch ($kind) {
						case ZEND_VM_KIND_CALL:
							out($f,"$prolog{$spec_name}_HANDLER,\n");
							$label++;
							break;
						case ZEND_VM_KIND_SWITCH:
							out($f,$prolog."(void*)(uintptr_t)$switch_labels[$spec_name],\n");
							$label++;
							break;
						case ZEND_VM_KIND_GOTO:
							out($f,$prolog."(void*)&&{$spec_name}_HANDLER,\n");
							$label++;
							break;
					}
				} else {
					// Emit pointer to handler of undefined opcode
					gen_null_label($f, $kind, $prolog);
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

			$do("ANY", "ANY");
		}
	} else {
	  // Emit labels for unspecialized executor

	  // For each opcode in opcode number order
		foreach($opcodes as $num => $dsc) {
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
						out($f,$prolog."(void*)&&ZEND_NULL_HANDLER,\n");
						break;
				}
				$next++;
			}
			if ($num >= 256) {
				continue;
			}
			$next = $num+1;

			//ugly trick for ZEND_VM_DEFINE_OP
			if ($dsc["code"]) {
				// Emit pointer to unspecialized handler
				switch ($kind) {
				case ZEND_VM_KIND_CALL:
					out($f,$prolog.$dsc["op"]."_HANDLER,\n");
					break;
				case ZEND_VM_KIND_SWITCH:
					out($f,$prolog."(void*)(uintptr_t)".((string)$num).",\n");
					break;
				case ZEND_VM_KIND_GOTO:
					out($f,$prolog."(void*)&&".$dsc["op"]."_HANDLER,\n");
					break;
				}
			} else {
				switch ($kind) {
					case ZEND_VM_KIND_CALL:
						out($f,$prolog."ZEND_NULL_HANDLER,\n");
						break;
					case ZEND_VM_KIND_SWITCH:
						out($f,$prolog."(void*)(uintptr_t)-1,\n");
						break;
					case ZEND_VM_KIND_GOTO:
						out($f,$prolog."(void*)&&ZEND_NULL_HANDLER,\n");
						break;
				}
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
			out($f,$prolog."(void*)&&ZEND_NULL_HANDLER\n");
			break;
	}
	$specs[$num + 1] = "$label";
}

// Generates specialized offsets
function gen_specs($f, $spec, $kind, $prolog, $specs) {
	$lastdef = array_pop($specs);
	$last = 0;
	foreach ($specs as $num => $def) {
		while (++$last < $num) {
			out($f, "$prolog$lastdef,\n");
		}
		$last = $num;
		out($f, "$prolog$def,\n");
	}
	out($f, "$prolog$lastdef\n");
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
	if (isset($extra_spec["DIM_OBJ"])) {
		if ($extra_spec["DIM_OBJ"] == 1) {
			$s .= "_DIM";
		} else if ($extra_spec["DIM_OBJ"] == 2) {
			$s .= "_OBJ";
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
	if (isset($extra_spec["DIM_OBJ"])) {
		$s[] = "SPEC_RULE_DIM_OBJ";
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
		foreach($op_types_ex as $op_data) {
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

// Generates all opcode handlers and helpers (specialized or unspecilaized)
function gen_executor_code($f, $spec, $kind, $prolog, &$switch_labels = array()) {
	global $list, $opcodes, $helpers, $op_types_ex;

	if ($spec) {
		// Produce specialized executor
		$op1t = $op_types_ex;
		// for each op1.op_type
		foreach($op1t as $op1) {
			$op2t = $op_types_ex;
			// for each op2.op_type
			foreach($op2t as $op2) {
				// for each handlers in helpers in original order
				foreach ($list as $lineno => $dsc) {
					if (isset($dsc["handler"])) {
						$num = $dsc["handler"];
						foreach (extra_spec_handler($opcodes[$num]) as $extra_spec) {
							// Check if handler accepts such types of operands (op1 and op2)
							if (isset($opcodes[$num]["op1"][$op1]) &&
							    isset($opcodes[$num]["op2"][$op2])) {
							  // Generate handler code
								gen_handler($f, 1, $kind, $opcodes[$num]["op"], $op1, $op2, isset($opcodes[$num]["use"]), $opcodes[$num]["code"], $lineno, $extra_spec, $switch_labels);
							}
						}
					} else if (isset($dsc["helper"])) {
						$num = $dsc["helper"];
						// Check if handler accepts such types of operands (op1 and op2)
						if (isset($helpers[$num]["op1"][$op1]) &&
						    isset($helpers[$num]["op2"][$op2])) {
						  // Generate helper code
							gen_helper($f, 1, $kind, $num, $op1, $op2, $helpers[$num]["param"], $helpers[$num]["code"], $lineno, $helpers[$num]["inline"]);
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
					gen_handler($f, 0, $kind, $opcodes[$num]["op"], "ANY", "ANY", isset($opcodes[$num]["use"]), $opcodes[$num]["code"], $lineno);
				}
			} else if (isset($dsc["helper"])) {
				$num = $dsc["helper"];
				// Generate helper code
				gen_helper($f, 0, $kind, $num, "ANY", "ANY", $helpers[$num]["param"], $helpers[$num]["code"], $lineno, $helpers[$num]["inline"]);
			} else {
				var_dump($dsc);
				die("??? $kind:$num\n");
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
			out($f,"default:\n");
			out($f,"\tzend_error_noreturn(E_ERROR, \"Invalid opcode %d/%d/%d.\", OPLINE->opcode, OPLINE->op1_type, OPLINE->op2_type);\n");
			out($f,"\tZEND_VM_NEXT_OPCODE(); /* Never reached */\n");
			break;
		case ZEND_VM_KIND_GOTO:
			out($f,"ZEND_NULL_HANDLER:\n");
			out($f,"\tzend_error_noreturn(E_ERROR, \"Invalid opcode %d/%d/%d.\", OPLINE->opcode, OPLINE->op1_type, OPLINE->op2_type);\n");
			out($f,"\tZEND_VM_NEXT_OPCODE(); /* Never reached */\n");
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
	global $params, $skeleton_file, $line_no;

	$switch_labels = array();
	$lineno = 0;
	foreach ($skl as $line) {
	  // Skeleton file contains special markers in form %NAME% those are
	  // substituted by custom code
		if (preg_match("/(.*)[{][%]([A-Z_]*)[%][}](.*)/", $line, $m)) {
			switch ($m[2]) {
				case "DEFINES":
					out($f,"#define SPEC_START_MASK        0x0000ffff\n");
					out($f,"#define SPEC_RULE_OP1          0x00010000\n");
					out($f,"#define SPEC_RULE_OP2          0x00020000\n");
					out($f,"#define SPEC_RULE_OP_DATA      0x00040000\n");
					out($f,"#define SPEC_RULE_RETVAL       0x00080000\n");
					out($f,"#define SPEC_RULE_QUICK_ARG    0x00100000\n");
					out($f,"#define SPEC_RULE_SMART_BRANCH 0x00200000\n");
					out($f,"#define SPEC_RULE_DIM_OBJ      0x00400000\n");
					out($f,"\n");
					out($f,"static const uint32_t *zend_spec_handlers;\n");
					out($f,"static const void **zend_opcode_handlers;\n");
					out($f,"static int zend_handlers_count;\n");
					out($f,"static const void *zend_vm_get_opcode_handler(zend_uchar opcode, const zend_op* op);\n\n");
					switch ($kind) {
						case ZEND_VM_KIND_CALL:
							out($f,"\n");
							out($f,"#ifdef ZEND_VM_FP_GLOBAL_REG\n");
							out($f,"#pragma GCC diagnostic ignored \"-Wvolatile-register-var\"\n");
							out($f,"register zend_execute_data* volatile execute_data __asm__(ZEND_VM_FP_GLOBAL_REG);\n");
							out($f,"#pragma GCC diagnostic warning \"-Wvolatile-register-var\"\n");
							out($f,"#endif\n");
							out($f,"\n");
							out($f,"#ifdef ZEND_VM_IP_GLOBAL_REG\n");
							out($f,"#pragma GCC diagnostic ignored \"-Wvolatile-register-var\"\n");
							out($f,"register const zend_op* volatile opline __asm__(ZEND_VM_IP_GLOBAL_REG);\n");
							out($f,"#pragma GCC diagnostic warning \"-Wvolatile-register-var\"\n");
							out($f,"#endif\n");
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
							out($f,"# define ZEND_VM_RETURN()        opline = NULL; return\n");
							out($f,"#else\n");
							out($f,"# define ZEND_OPCODE_HANDLER_RET int\n");
							out($f,"# define ZEND_VM_TAIL_CALL(call) return call\n");
							out($f,"# define ZEND_VM_CONTINUE()      return  0\n");
							out($f,"# define ZEND_VM_RETURN()        return -1\n");
							out($f,"#endif\n");
							out($f,"\n");
							out($f,"typedef ZEND_OPCODE_HANDLER_RET (ZEND_FASTCALL *opcode_handler_t) (ZEND_OPCODE_HANDLER_ARGS);\n");
							out($f,"\n");
							out($f,"#undef OPLINE\n");
							out($f,"#undef DCL_OPLINE\n");
							out($f,"#undef USE_OPLINE\n");
							out($f,"#undef LOAD_OPLINE\n");
							out($f,"#undef LOAD_OPLINE_EX\n");
							out($f,"#undef SAVE_OPLINE\n");
							out($f,"#define DCL_OPLINE\n");
							out($f,"#ifdef ZEND_VM_IP_GLOBAL_REG\n");
							out($f,"# define OPLINE opline\n");
							out($f,"# define USE_OPLINE\n");
							out($f,"# define LOAD_OPLINE() opline = EX(opline)\n");
							out($f,"# define LOAD_NEXT_OPLINE() opline = EX(opline) + 1\n");
							out($f,"# define SAVE_OPLINE() EX(opline) = opline\n");
							out($f,"#else\n");
							out($f,"# define OPLINE EX(opline)\n");
							out($f,"# define USE_OPLINE const zend_op *opline = EX(opline);\n");
							out($f,"# define LOAD_OPLINE()\n");
							out($f,"# define LOAD_NEXT_OPLINE() ZEND_VM_INC_OPCODE()\n");
							out($f,"# define SAVE_OPLINE()\n");
							out($f,"#endif\n");
							out($f,"#undef HANDLE_EXCEPTION\n");
							out($f,"#undef HANDLE_EXCEPTION_LEAVE\n");
							out($f,"#define HANDLE_EXCEPTION() LOAD_OPLINE(); ZEND_VM_CONTINUE()\n");
							out($f,"#define HANDLE_EXCEPTION_LEAVE() LOAD_OPLINE(); ZEND_VM_LEAVE()\n");
							out($f,"#if defined(ZEND_VM_FP_GLOBAL_REG)\n");
							out($f,"# define ZEND_VM_ENTER()           execute_data = EG(current_execute_data); LOAD_OPLINE(); ZEND_VM_INTERRUPT_CHECK(); ZEND_VM_CONTINUE()\n");
							out($f,"# define ZEND_VM_LEAVE()           ZEND_VM_CONTINUE()\n");
							out($f,"#elif defined(ZEND_VM_IP_GLOBAL_REG)\n");
							out($f,"# define ZEND_VM_ENTER()           opline = EG(current_execute_data)->opline; return 1\n");
							out($f,"# define ZEND_VM_LEAVE()           return  2\n");
							out($f,"#else\n");
							out($f,"# define ZEND_VM_ENTER()           return  1\n");
							out($f,"# define ZEND_VM_LEAVE()           return  2\n");
							out($f,"#endif\n");
							out($f,"#define ZEND_VM_DISPATCH(opcode, opline) ZEND_VM_TAIL_CALL(((opcode_handler_t)zend_vm_get_opcode_handler(opcode, opline))(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU));\n");
							out($f,"\n");
							break;
						case ZEND_VM_KIND_SWITCH:
							out($f,"\n");
							out($f,"#undef OPLINE\n");
							out($f,"#undef DCL_OPLINE\n");
							out($f,"#undef USE_OPLINE\n");
							out($f,"#undef LOAD_OPLINE\n");
							out($f,"#undef LOAD_NEXT_OPLINE\n");
							out($f,"#undef SAVE_OPLINE\n");
							out($f,"#define OPLINE opline\n");
							out($f,"#ifdef ZEND_VM_IP_GLOBAL_REG\n");
							out($f,"# define DCL_OPLINE register const zend_op *opline __asm__(ZEND_VM_IP_GLOBAL_REG);\n");
							out($f,"#else\n");
							out($f,"# define DCL_OPLINE const zend_op *opline;\n");
							out($f,"#endif\n");
							out($f,"#define USE_OPLINE\n");
							out($f,"#define LOAD_OPLINE() opline = EX(opline)\n");
							out($f,"#define LOAD_NEXT_OPLINE() opline = EX(opline) + 1\n");
							out($f,"#define SAVE_OPLINE() EX(opline) = opline\n");
							out($f,"#undef HANDLE_EXCEPTION\n");
							out($f,"#undef HANDLE_EXCEPTION_LEAVE\n");
							out($f,"#define HANDLE_EXCEPTION() LOAD_OPLINE(); ZEND_VM_CONTINUE()\n");
							out($f,"#define HANDLE_EXCEPTION_LEAVE() LOAD_OPLINE(); ZEND_VM_LEAVE()\n");
							out($f,"#define ZEND_VM_CONTINUE() goto zend_vm_continue\n");
							out($f,"#define ZEND_VM_RETURN()   return\n");
							out($f,"#define ZEND_VM_ENTER()    execute_data = EG(current_execute_data); LOAD_OPLINE(); ZEND_VM_INTERRUPT_CHECK(); ZEND_VM_CONTINUE()\n");
							out($f,"#define ZEND_VM_LEAVE()    ZEND_VM_CONTINUE()\n");
							out($f,"#define ZEND_VM_DISPATCH(opcode, opline) dispatch_handler = zend_vm_get_opcode_handler(opcode, opline); goto zend_vm_dispatch;\n");
							out($f,"\n");
							break;
						case ZEND_VM_KIND_GOTO:
							out($f,"\n");
							out($f,"#undef OPLINE\n");
							out($f,"#undef DCL_OPLINE\n");
							out($f,"#undef USE_OPLINE\n");
							out($f,"#undef LOAD_OPLINE\n");
							out($f,"#undef LOAD_NEXT_OPLINE\n");
							out($f,"#undef SAVE_OPLINE\n");
							out($f,"#define OPLINE opline\n");
							out($f,"#ifdef ZEND_VM_IP_GLOBAL_REG\n");
							out($f,"# define DCL_OPLINE register const zend_op *opline __asm__(ZEND_VM_IP_GLOBAL_REG);\n");
							out($f,"#else\n");
							out($f,"# define DCL_OPLINE const zend_op *opline;\n");
							out($f,"#endif\n");
							out($f,"#define USE_OPLINE\n");
							out($f,"#define LOAD_OPLINE() opline = EX(opline)\n");
							out($f,"#define LOAD_NEXT_OPLINE() opline = EX(opline) + 1\n");
							out($f,"#define SAVE_OPLINE() EX(opline) = opline\n");
							out($f,"#undef HANDLE_EXCEPTION\n");
							out($f,"#undef HANDLE_EXCEPTION_LEAVE\n");
							if (ZEND_VM_SPEC) {
								out($f,"#define HANDLE_EXCEPTION() goto ZEND_HANDLE_EXCEPTION_SPEC_HANDLER\n");
								out($f,"#define HANDLE_EXCEPTION_LEAVE() goto ZEND_HANDLE_EXCEPTION_SPEC_HANDLER\n");
							} else {
								out($f,"#define HANDLE_EXCEPTION() goto ZEND_HANDLE_EXCEPTION_HANDLER\n");
								out($f,"#define HANDLE_EXCEPTION_LEAVE() goto ZEND_HANDLE_EXCEPTION_HANDLER\n");
							}
							out($f,"#define ZEND_VM_CONTINUE() goto *(void**)(OPLINE->handler)\n");
							out($f,"#define ZEND_VM_RETURN()   return\n");
							out($f,"#define ZEND_VM_ENTER()    execute_data = EG(current_execute_data); LOAD_OPLINE(); ZEND_VM_INTERRUPT_CHECK(); ZEND_VM_CONTINUE()\n");
							out($f,"#define ZEND_VM_LEAVE()    ZEND_VM_CONTINUE()\n");
							out($f,"#define ZEND_VM_DISPATCH(opcode, opline) goto *(void**)(zend_vm_get_opcode_handler(opcode, opline));\n");
							out($f,"\n");
							break;
					}
					break;
				case "EXECUTOR_NAME":
					out($f, $m[1].$executor_name.$m[3]."\n");
					break;
				case "HELPER_VARS":
					if ($kind != ZEND_VM_KIND_CALL) {
						if ($kind == ZEND_VM_KIND_SWITCH) {
							out($f,$m[1]."const void *dispatch_handler;\n");
						}
						// Emit local variables those are used for helpers' parameters
						foreach ($params as $param => $x) {
							out($f,$m[1].$param.";\n");
						}
						out($f,"#ifdef ZEND_VM_FP_GLOBAL_REG\n");
						out($f,$m[1]."register zend_execute_data *execute_data __asm__(ZEND_VM_FP_GLOBAL_REG) = ex;\n");
						out($f,"#else\n");
						out($f,$m[1]."zend_execute_data *execute_data = ex;\n");
						out($f,"#endif\n");
					} else {
						out($f,"#ifdef ZEND_VM_IP_GLOBAL_REG\n");
						out($f,$m[1]."const zend_op *orig_opline = opline;\n");
						out($f,"#endif\n");
						out($f,"#ifdef ZEND_VM_FP_GLOBAL_REG\n");
						out($f,$m[1]."zend_execute_data *orig_execute_data = execute_data;\n");
						out($f,$m[1]."execute_data = ex;\n");
						out($f,"#else\n");
						out($f,$m[1]."zend_execute_data *execute_data = ex;\n");
						out($f,"#endif\n");
					}
					break;
				case "INTERNAL_LABELS":
					if ($kind == ZEND_VM_KIND_GOTO) {
					  // Emit array of labels of opcode handlers and code for
					  // zend_opcode_handlers initialization
						$prolog = $m[1];
						out($f,$prolog."if (UNEXPECTED(execute_data == NULL)) {\n");
						out($f,$prolog."\tstatic const void* labels[] = {\n");
						gen_labels($f, $spec, $kind, $prolog."\t\t", $specs);
						out($f,$prolog."\t};\n");
						out($f,$prolog."static const uint32_t specs[] = {\n");
						gen_specs($f, $spec, $kind, $prolog."\t", $specs);
						out($f,$prolog."};\n");
						out($f,$prolog."\tzend_opcode_handlers = (const void **) labels;\n");
						out($f,$prolog."\tzend_handlers_count = sizeof(labels) / sizeof(void*);\n");
						out($f,$prolog."\tzend_spec_handlers = (const uint32_t *) specs;\n");
						out($f,$prolog."\treturn;\n");
						out($f,$prolog."}\n");
					} else {
						skip_blanks($f, $m[1], $m[3]);
					}
					break;
				case "ZEND_VM_CONTINUE_LABEL":
					if ($kind == ZEND_VM_KIND_CALL) {
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
						case ZEND_VM_KIND_CALL:
							out($f,"#if defined(ZEND_VM_FP_GLOBAL_REG) && defined(ZEND_VM_IP_GLOBAL_REG)\n");
							out($f, $m[1]."((opcode_handler_t)OPLINE->handler)(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);\n");
							out($f, $m[1]."if (UNEXPECTED(!OPLINE))".$m[3]."\n");
							out($f,"#else\n");
							out($f, $m[1]."if (UNEXPECTED((ret = ((opcode_handler_t)OPLINE->handler)(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU)) != 0))".$m[3]."\n");
							out($f,"#endif\n");
							break;
						case ZEND_VM_KIND_SWITCH:
							out($f, $m[1]."dispatch_handler = OPLINE->handler;\nzend_vm_dispatch:\n".$m[1]."switch ((int)(uintptr_t)dispatch_handler)".$m[3]."\n");
							break;
						case ZEND_VM_KIND_GOTO:
							out($f, $m[1]."goto *(void**)(OPLINE->handler);".$m[3]."\n");
							break;
					}
					break;
				case "INTERNAL_EXECUTOR":
					if ($kind == ZEND_VM_KIND_CALL) {
					  // Executor is defined as a set of functions
						out($f,
								"#ifdef ZEND_VM_FP_GLOBAL_REG\n" .
								$m[1]."execute_data = orig_execute_data;\n" .
								"# ifdef ZEND_VM_IP_GLOBAL_REG\n" .
								$m[1]."opline = orig_opline;\n" .
								"# endif\n" .
								$m[1]."return;\n" .
								"#else\n" .
								$m[1]."if (EXPECTED(ret > 0)) {\n" . 
						        $m[1]."\texecute_data = EG(current_execute_data);\n".
								$m[1]."\tZEND_VM_INTERRUPT_CHECK();\n".
						        $m[1]."} else {\n" .
								"# ifdef ZEND_VM_IP_GLOBAL_REG\n" .
						        $m[1]."\topline = orig_opline;\n" .
								"# endif\n".
						        $m[1]."\treturn;\n".
						        $m[1]."}\n".
								"#endif\n");
					} else {
					  // Emit executor code
						gen_executor_code($f, $spec, $kind, $m[1], $switch_labels);
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
						out($f,$prolog."");
						out($f,$prolog.$executor_name."_ex(NULL);\n");
					} else {
						out($f,$prolog."static const void *labels[] = {\n");
						gen_labels($f, $spec, $kind, $prolog."\t", $specs, $switch_labels);
						out($f,$prolog."};\n");
						out($f,$prolog."static const uint32_t specs[] = {\n");
						gen_specs($f, $spec, $kind, $prolog."\t", $specs);
						out($f,$prolog."};\n");
						out($f,$prolog."zend_opcode_handlers = labels;\n");
						out($f,$prolog."\tzend_handlers_count = sizeof(labels) / sizeof(void*);\n");
						out($f,$prolog."zend_spec_handlers = specs;\n");
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
	foreach($a as $val) {
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
	foreach($a as $val) {
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
	foreach($a as $rule) {
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
				case "DIM_OBJ":
					$ret["DIM_OBJ"] = array(0, 1, 2);
					break;
				case "NO_CONST_CONST":
					$ret["NO_CONST_CONST"] = array(1);
					break;
				case "COMMUTATIVE":
					$ret["COMMUTATIVE"] = array(1);
					break;
				default:
					die("ERROR ($def:$lineno): Wrong specialization rules '$str'\n");
			}
			$used_extra_spec[$rule] = 1;
		}
	}
	return $ret;
}

function gen_vm($def, $skel) {
	global $definition_file, $skeleton_file, $executor_file,
		$op_types, $list, $opcodes, $helpers, $params, $opnames,
		$vm_op_flags, $used_extra_spec;

	// Load definition file
	$in = @file($def);
	if (!$in) {
		die("ERROR: Can not open definition file '$def'\n");
	}
	// We need absolute path to definition file to use it in #line directives
	$definition_file = realpath($def);

	// Load skeleton file
	$skl = @file($skel);
	if (!$skl) {
		die("ERROR: Can not open skeleton file '$skel'\n");
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
	$export         = array();
	foreach ($in as $line) {
		++$lineno;
		if (strpos($line,"ZEND_VM_HANDLER(") === 0) {
		  // Parsing opcode handler's definition
			if (preg_match(
					"/^ZEND_VM_HANDLER\(\s*([0-9]+)\s*,\s*([A-Z_]+)\s*,\s*([A-Z_|]+)\s*,\s*([A-Z_|]+)\s*(,\s*([A-Z_|]+)\s*)?(,\s*SPEC\(([A-Z_|=,]+)\)\s*)?\)/",
					$line,
					$m) == 0) {
				die("ERROR ($def:$lineno): Invalid ZEND_VM_HANDLER definition.\n");
			}
			$code = (int)$m[1];
			$op   = $m[2];
			$len  = strlen($op);
			$op1  = parse_operand_spec($def, $lineno, $m[3], $flags1);
			$op2  = parse_operand_spec($def, $lineno, $m[4], $flags2);
			$flags = $flags1 | ($flags2 << 8);
			if (!empty($m[6])) {
				$flags |= parse_ext_spec($def, $lineno, $m[6]);
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
			$opcodes[$code] = array("op"=>$op,"op1"=>$op1,"op2"=>$op2,"code"=>"","flags"=>$flags);
			if (isset($m[8])) {
				$opcodes[$code]["spec"] = parse_spec_rules($def, $lineno, $m[8]);
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
		} else if (strpos($line,"ZEND_VM_TYPE_SPEC_HANDLER(") === 0) {
		  // Parsing opcode handler's definition
			if (preg_match(
					"/^ZEND_VM_TYPE_SPEC_HANDLER\(\s*([A-Z_]+)\s*,\s*([^,]+),\s*([A-Za-z_]+)\s*,\s*([A-Z_|]+)\s*,\s*([A-Z_|]+)\s*(,\s*([A-Z_|]+)\s*)?(,\s*SPEC\(([A-Z_|=,]+)\)\s*)?\)/",
					$line,
					$m) == 0) {
				die("ERROR ($def:$lineno): Invalid ZEND_VM_TYPE_HANDLER_HANDLER definition.\n");
			}
			$orig_op = $m[1];
			if (!isset($opnames[$orig_op])) {
				die("ERROR ($def:$lineno): Opcode with name '$orig_op' is not defined.\n");
			}
			$orig_code = $opnames[$orig_op];
			$condition = $m[2];
			$code = $extra_num++;
			$op = $m[3];
			$op1  = parse_operand_spec($def, $lineno, $m[4], $flags1);
			$op2  = parse_operand_spec($def, $lineno, $m[5], $flags2);
			$flags = $flags1 | ($flags2 << 8);
			if (!empty($m[7])) {
				$flags |= parse_ext_spec($def, $lineno, $m[7]);
			}

			if (isset($opcodes[$code])) {
				die("ERROR ($def:$lineno): Opcode with name '$code' is already defined.\n");
			}
			$opcodes[$orig_code]['type_spec'][$code] = $condition;
			$used_extra_spec["TYPE"] = 1;
			$opcodes[$code] = array("op"=>$op,"op1"=>$op1,"op2"=>$op2,"code"=>"","flags"=>$flags);
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
		} else if (strpos($line,"ZEND_VM_HELPER(") === 0 || strpos($line,"ZEND_VM_INLINE_HELPER(") === 0) {
		  // Parsing helper's definition
			if (preg_match(
					"/^ZEND_VM(_INLINE)?_HELPER\(\s*([A-Za-z_]+)\s*,\s*([A-Z_|]+)\s*,\s*([A-Z_|]+)\s*(?:,\s*([^)]*))?\s*\)/",
					$line,
					$m) == 0) {
				die("ERROR ($def:$lineno): Invalid ZEND_VM_HELPER definition.\n");
			}
			$inline = !empty($m[1]);
			$helper = $m[2];
			$op1    = parse_operand_spec($def, $lineno, $m[3], $flags1);
			$op2    = parse_operand_spec($def, $lineno, $m[4], $flags2);
			$param  = isset($m[5]) ? $m[5] : null;
			if (isset($helpers[$helper])) {
				die("ERROR ($def:$lineno): Helper with name '$helper' is already defined.\n");
			}

			// Store parameters
			foreach (explode(",", $param) as $p) {
				$p = trim($p);
				if ($p !== "") {
					$params[$p] = 1;
				}
			}

			$helpers[$helper] = array("op1"=>$op1,"op2"=>$op2,"param"=>$param,"code"=>"","inline"=>$inline);
			$handler = null;
			$list[$lineno] = array("helper"=>$helper);
		} else if (strpos($line,"ZEND_VM_EXPORT_HANDLER(") === 0) {
			if (preg_match(
					"/^ZEND_VM_EXPORT_HANDLER\(\s*([A-Za-z_]+)\s*,\s*([A-Z_]+)\s*\)/",
					$line,
					$m) == 0) {
				die("ERROR ($def:$lineno): Invalid ZEND_VM_EXPORT_HANDLER definition.\n");
			}
			if (!isset($opnames[$m[2]])) {
				die("ERROR ($def:$lineno): opcode '{$m[2]}' is not defined.\n");
			}
			$export[] = array("handler",$m[1],$m[2]);
		} else if (strpos($line,"ZEND_VM_EXPORT_HELPER(") === 0) {
			if (preg_match(
					"/^ZEND_VM_EXPORT_HELPER\(\s*([A-Za-z_]+)\s*,\s*([A-Za-z_]+)\s*\)/",
					$line,
					$m) == 0) {
				die("ERROR ($def:$lineno): Invalid ZEND_VM_EXPORT_HELPER definition.\n");
			}
			if (!isset($helpers[$m[2]])) {
				die("ERROR ($def:$lineno): helper '{$m[2]}' is not defined.\n");
			}
			$export[] = array("helper",$m[1],$m[2]);
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
		if (preg_match("/ZEND_VM_DISPATCH_TO_HANDLER\(\s*([A-Z_]*)\s*\)/m", $dsc["code"], $m)) {
			$op = $m[1];
			if (!isset($opnames[$op])) {
				die("ERROR ($def:$lineno): Opcode with name '$op' is not defined.\n");
			}
			$code = $opnames[$op];
			$opcodes[$code]['use'] = 1;
		}
	}

	// Generate opcode #defines (zend_vm_opcodes.h)
	$code_len = strlen((string)$max_opcode);
	$f = fopen(__DIR__ . "/zend_vm_opcodes.h", "w+") or die("ERROR: Cannot create zend_vm_opcodes.h\n");

	// Insert header
	out($f, $GLOBALS['header_text']);
	fputs($f, "#ifndef ZEND_VM_OPCODES_H\n#define ZEND_VM_OPCODES_H\n\n");
	fputs($f, "#define ZEND_VM_SPEC\t\t" . ZEND_VM_SPEC . "\n");
	fputs($f, "#define ZEND_VM_LINES\t\t" . ZEND_VM_LINES . "\n");
	fputs($f, "#define ZEND_VM_KIND_CALL\t" . ZEND_VM_KIND_CALL . "\n");
	fputs($f, "#define ZEND_VM_KIND_SWITCH\t" . ZEND_VM_KIND_SWITCH . "\n");
	fputs($f, "#define ZEND_VM_KIND_GOTO\t" . ZEND_VM_KIND_GOTO . "\n");
	fputs($f, "#define ZEND_VM_KIND\t\t" . $GLOBALS["vm_kind_name"][ZEND_VM_KIND] . "\n");
	fputs($f, "\n");
	foreach($vm_op_flags as $name => $val) {
		fprintf($f, "#define %-24s 0x%08x\n", $name, $val);
	}
	fputs($f, "#define ZEND_VM_OP1_FLAGS(flags) (flags & 0xff)\n");
	fputs($f, "#define ZEND_VM_OP2_FLAGS(flags) ((flags >> 8) & 0xff)\n");
	fputs($f, "\n");
	fputs($f, "BEGIN_EXTERN_C()\n\n");
	fputs($f, "ZEND_API const char *zend_get_opcode_name(zend_uchar opcode);\n");
	fputs($f, "ZEND_API uint32_t zend_get_opcode_flags(zend_uchar opcode);\n\n");
	fputs($f, "END_EXTERN_C()\n\n");
	
	foreach ($opcodes as $code => $dsc) {
		$code = str_pad((string)$code,$code_len," ",STR_PAD_LEFT);
		$op = str_pad($dsc["op"],$max_opcode_len);
		if ($code <= $max_opcode) {
			fputs($f,"#define $op $code\n");
		}
	}

	$code = str_pad((string)$max_opcode,$code_len," ",STR_PAD_LEFT);
	$op = str_pad("ZEND_VM_LAST_OPCODE",$max_opcode_len);
	fputs($f,"\n#define $op $code\n");

	fputs($f, "\n#endif\n");
	fclose($f);
	echo "zend_vm_opcodes.h generated successfully.\n";

	// zend_vm_opcodes.c
	$f = fopen(__DIR__ . "/zend_vm_opcodes.c", "w+") or die("ERROR: Cannot create zend_vm_opcodes.c\n");

	// Insert header
	out($f, $GLOBALS['header_text']);
	fputs($f,"#include <stdio.h>\n");
	fputs($f,"#include <zend.h>\n\n");
	
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

	fputs($f, "ZEND_API const char* zend_get_opcode_name(zend_uchar opcode) {\n");
	fputs($f, "\treturn zend_vm_opcodes_names[opcode];\n");
	fputs($f, "}\n");

	fputs($f, "ZEND_API uint32_t zend_get_opcode_flags(zend_uchar opcode) {\n");
	fputs($f, "\treturn zend_vm_opcodes_flags[opcode];\n");
	fputs($f, "}\n");
    
	fclose($f);
	echo "zend_vm_opcodes.c generated successfully.\n";

	// Generate zend_vm_execute.h
	$f = fopen(__DIR__ . "/zend_vm_execute.h", "w+") or die("ERROR: Cannot create zend_vm_execute.h\n");
	$executor_file = realpath(__DIR__ . "/zend_vm_execute.h");

	// Insert header
	out($f, $GLOBALS['header_text']);

	out($f, "#ifdef ZEND_WIN32\n");
	// Suppress free_op1 warnings on Windows
	out($f, "# pragma warning(once : 4101)\n");
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

	out($f, "static zend_uchar zend_user_opcodes[256] = {");
	for ($i = 0; $i < 255; ++$i) {
		if ($i % 16 == 1) out($f, "\n\t");
		out($f, "$i,");
	}
	out($f, "255\n};\n\n");

	// Generate specialized executor
	gen_executor($f, $skl, ZEND_VM_SPEC, ZEND_VM_KIND, "execute", "zend_init_opcodes_handlers");

	// Generate zend_vm_get_opcode_handler() function
	out($f, "static const void *zend_vm_get_opcode_handler_ex(uint32_t spec, const zend_op* op)\n");
	out($f, "{\n");
	if (!ZEND_VM_SPEC) {
		out($f, "\treturn zend_opcode_handlers[spec];\n");
	} else {
		out($f, "\tstatic const int zend_vm_decode[] = {\n");
		out($f, "\t\t_UNUSED_CODE, /* 0              */\n");
		out($f, "\t\t_CONST_CODE,  /* 1 = IS_CONST   */\n");
		out($f, "\t\t_TMP_CODE,    /* 2 = IS_TMP_VAR */\n");
		out($f, "\t\t_UNUSED_CODE, /* 3              */\n");
		out($f, "\t\t_VAR_CODE,    /* 4 = IS_VAR     */\n");
		out($f, "\t\t_UNUSED_CODE, /* 5              */\n");
		out($f, "\t\t_UNUSED_CODE, /* 6              */\n");
		out($f, "\t\t_UNUSED_CODE, /* 7              */\n");
		out($f, "\t\t_UNUSED_CODE, /* 8 = IS_UNUSED  */\n");
		out($f, "\t\t_UNUSED_CODE, /* 9              */\n");
		out($f, "\t\t_UNUSED_CODE, /* 10             */\n");
		out($f, "\t\t_UNUSED_CODE, /* 11             */\n");
		out($f, "\t\t_UNUSED_CODE, /* 12             */\n");
		out($f, "\t\t_UNUSED_CODE, /* 13             */\n");
		out($f, "\t\t_UNUSED_CODE, /* 14             */\n");
		out($f, "\t\t_UNUSED_CODE, /* 15             */\n");
		out($f, "\t\t_CV_CODE      /* 16 = IS_CV     */\n");
		out($f, "\t};\n");
		out($f, "\tuint32_t offset = 0;\n");
		out($f, "\tif (spec & SPEC_RULE_OP1) offset = offset * 5 + zend_vm_decode[op->op1_type];\n");
		out($f, "\tif (spec & SPEC_RULE_OP2) offset = offset * 5 + zend_vm_decode[op->op2_type];\n");
		if (isset($used_extra_spec["OP_DATA"])) {
			out($f, "\tif (spec & SPEC_RULE_OP_DATA) offset = offset * 5 + zend_vm_decode[(op + 1)->op1_type];\n");
		}
		if (isset($used_extra_spec["RETVAL"])) {
			out($f, "\tif (spec & SPEC_RULE_RETVAL) offset = offset * 2 + (op->result_type != IS_UNUSED);\n");
		}
		if (isset($used_extra_spec["QUICK_ARG"])) {
			out($f, "\tif (spec & SPEC_RULE_QUICK_ARG) offset = offset * 2 + (op->op2.num < MAX_ARG_FLAG_NUM);\n");
		}
		if (isset($used_extra_spec["SMART_BRANCH"])) {
			out($f, "\tif (spec & SPEC_RULE_SMART_BRANCH) {\n");
			out($f,	"\t\toffset = offset * 3;\n");
			out($f, "\t\tif ((op+1)->opcode == ZEND_JMPZ) {\n");
			out($f,	"\t\t\toffset += 1;\n");
			out($f, "\t\t} else if ((op+1)->opcode == ZEND_JMPNZ) {\n");
			out($f,	"\t\t\toffset += 2;\n");
			out($f, "\t\t}\n");
			out($f, "\t}\n");
		}
		if (isset($used_extra_spec["DIM_OBJ"])) {
			out($f, "\tif (spec & SPEC_RULE_DIM_OBJ) {\n");
			out($f,	"\t\toffset = offset * 3;\n");
			out($f, "\t\tif (op->extended_value == ZEND_ASSIGN_DIM) {\n");
			out($f,	"\t\t\toffset += 1;\n");
			out($f, "\t\t} else if (op->extended_value == ZEND_ASSIGN_OBJ) {\n");
			out($f,	"\t\t\toffset += 2;\n");
			out($f, "\t\t}\n");
			out($f, "\t}\n");
		}
		out($f, "\treturn zend_opcode_handlers[(spec & SPEC_START_MASK) + offset];\n");
	}
	out($f, "}\n\n");
	out($f, "static const void *zend_vm_get_opcode_handler(zend_uchar opcode, const zend_op* op)\n");
	out($f, "{\n");
	if (!ZEND_VM_SPEC) {
		out($f, "\treturn zend_vm_get_opcode_handler_ex(opcode, op);\n");
	} else {
		out($f, "\treturn zend_vm_get_opcode_handler_ex(zend_spec_handlers[opcode], op);\n");
	}
	out($f, "}\n\n");

	// Generate zend_vm_get_opcode_handler() function
	out($f, "ZEND_API void zend_vm_set_opcode_handler(zend_op* op)\n");
	out($f, "{\n");
	out($f, "\top->handler = zend_vm_get_opcode_handler(zend_user_opcodes[op->opcode], op);\n");
	out($f, "}\n\n");

	// Generate zend_vm_set_opcode_handler_ex() function
	out($f, "ZEND_API void zend_vm_set_opcode_handler_ex(zend_op* op, uint32_t op1_info, uint32_t op2_info, uint32_t res_info)\n");
	out($f, "{\n");
	out($f, "\tzend_uchar opcode = zend_user_opcodes[op->opcode];\n");
	if (!ZEND_VM_SPEC) {
		out($f, "\top->handler = zend_vm_get_opcode_handler_ex(opcode, op);\n");
	} else {
		out($f, "\tuint32_t spec = zend_spec_handlers[opcode];\n");
		if (isset($used_extra_spec["TYPE"])) {
			out($f, "\tswitch (opcode) {\n");
			foreach($opcodes as $code => $dsc) {
				if (isset($dsc['type_spec'])) {
					$orig_op = $dsc['op'];
					out($f, "\t\tcase $orig_op:\n");
					$first = true;
					foreach($dsc['type_spec'] as $code => $condition) {
						if ($first) {
							out($f, "\t\t\tif ($condition) {\n");
							$first = false;
						} else {
							out($f, "\t\t\t} else if ($condition) {\n");
						}
						$spec_dsc = $opcodes[$code];
						if (isset($spec_dsc["spec"]["NO_CONST_CONST"])) {
							out($f, "\t\t\t\tif (op->op1_type == IS_CONST && op->op2_type == IS_CONST) {\n");
							out($f, "\t\t\t\t\tbreak;\n");
							out($f, "\t\t\t\t}\n");
						}
						out($f, "\t\t\t\tspec = ${spec_dsc['spec_code']};\n");
						if (isset($spec_dsc["spec"]["COMMUTATIVE"])) {
							out($f, "\t\t\t\tif (op->op1_type > op->op2_type) {\n");
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
			out($f, "\t\tdefault:\n");
			out($f, "\t\t\tbreak;\n");
			out($f, "\t}\n");
		}
		out($f, "\top->handler = zend_vm_get_opcode_handler_ex(spec, op);\n");
	}
	out($f, "}\n\n");

	// Generate zend_vm_call_opcode_handler() function
	if (ZEND_VM_KIND == ZEND_VM_KIND_CALL) {
		out($f, "ZEND_API int zend_vm_call_opcode_handler(zend_execute_data* ex)\n");
		out($f, "{\n");
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
		out($f, "\t((opcode_handler_t)OPLINE->handler)(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);\n");
		out($f, "\tif (EXPECTED(opline)) {\n");
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
		out($f, "ZEND_API int zend_vm_call_opcode_handler(zend_execute_data* ex)\n");
		out($f, "{\n");
		out($f, "\tzend_error_noreturn(E_CORE_ERROR, \"zend_vm_call_opcode_handler() is not supported\");\n");
		out($f, "\treturn 0;\n");
		out($f, "}\n\n");
	}

	// Export handlers and helpers
	if (count($export) > 0 &&
	    ZEND_VM_KIND != ZEND_VM_KIND_CALL) {
		out($f,"#undef OPLINE\n");
		out($f,"#undef DCL_OPLINE\n");
		out($f,"#undef USE_OPLINE\n");
		out($f,"#undef LOAD_OPLINE\n");
		out($f,"#undef LOAD_NEXT_OPLINE\n");
		out($f,"#undef SAVE_OPLINE\n");
		out($f,"#define OPLINE EX(opline)\n");
		out($f,"#define DCL_OPLINE\n");
		out($f,"#define USE_OPLINE const zend_op *opline = EX(opline);\n");
		out($f,"#define LOAD_OPLINE()\n");
		out($f,"#define LOAD_NEXT_OPLINE() ZEND_VM_INC_OPCODE()\n");
		out($f,"#define SAVE_OPLINE()\n");
		out($f,"#undef HANDLE_EXCEPTION\n");
		out($f,"#undef HANDLE_EXCEPTION_LEAVE\n");
		out($f,"#define HANDLE_EXCEPTION() LOAD_OPLINE(); ZEND_VM_CONTINUE()\n");
		out($f,"#define HANDLE_EXCEPTION_LEAVE() LOAD_OPLINE(); ZEND_VM_LEAVE()\n");
		out($f,"#undef ZEND_VM_CONTINUE\n");
		out($f,"#undef ZEND_VM_RETURN\n");
		out($f,"#undef ZEND_VM_ENTER\n");
		out($f,"#undef ZEND_VM_LEAVE\n");
		out($f,"#undef ZEND_VM_DISPATCH\n");
		out($f,"#define ZEND_VM_CONTINUE()   return  0\n");
		out($f,"#define ZEND_VM_RETURN()     return -1\n");
		out($f,"#define ZEND_VM_ENTER()      return  1\n");
		out($f,"#define ZEND_VM_LEAVE()      return  2\n");
		out($f,"#define ZEND_VM_DISPATCH(opcode, opline) return zend_vm_get_opcode_handler(opcode, opline)(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);\n\n");
		out($f,"\n");
	}
	foreach ($export as $dsk) {
		list($kind, $func, $name) = $dsk;
		out($f, "ZEND_API int $func(");
		if ($kind == "handler") {
			out($f, "ZEND_OPCODE_HANDLER_ARGS)\n");
			$code = $opcodes[$opnames[$name]]['code'];
		} else {
			$h = $helpers[$name];
			if ($h['param'] == null) {
				out($f, "ZEND_OPCODE_HANDLER_ARGS)\n");
			} else {
				out($f, $h['param']. " ZEND_OPCODE_HANDLER_ARGS_DC)\n");
			}
			$code = $h['code'];
		}
		$done = 0;
		if (ZEND_VM_KIND == ZEND_VM_KIND_CALL) {
			if ($kind == "handler") {
				$op = $opcodes[$opnames[$name]];
				if (isset($op['op1']["ANY"]) && isset($op['op2']["ANY"])) {
					out($f, "{\n\treturn ".$name.(ZEND_VM_SPEC?"_SPEC":"")."_HANDLER(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);\n}\n\n");
					$done = 1;
				}
			} else if ($helpers[$name]["param"] == null) {
				$h = $helpers[$name];
				if (isset($h['op1']["ANY"]) && isset($h['op2']["ANY"])) {
					out($f, "{\n\treturn ".$name.(ZEND_VM_SPEC?"_SPEC":"")."(ZEND_OPCODE_HANDLER_ARGS_PASSTHRU);\n}\n\n");
					$done = 1;
				}
			}
		}
		if (!$done) {
			gen_code($f, 0, ZEND_VM_KIND_CALL, 1, $code, 'ANY', 'ANY', $name);
		}
	}

	fclose($f);
	echo "zend_vm_execute.h generated successfully.\n";
}

function usage() {
	echo("\nUsage: php zend_vm_gen.php [options]\n".
	     "\nOptions:".
	     "\n  --with-vm-kind=CALL|SWITCH|GOTO - select threading model (default is CALL)".
	     "\n  --without-specializer           - disable executor specialization".
	     "\n  --with-lines                    - enable #line directives".
	     "\n\n");
}

// Parse arguments
for ($i = 1;  $i < $argc; $i++) {
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
	define("ZEND_VM_KIND", ZEND_VM_KIND_CALL);
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

?>
