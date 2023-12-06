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
*/

#ifndef ZEND_FRAMELESS_FUNCTION_H
#define ZEND_FRAMELESS_FUNCTION_H

#include <stdint.h>
#include <stddef.h>

#ifdef PHP_WIN32
# include <config.w32.h>
#else
# include <php_config.h>
#endif

#include "zend_portability.h"

#define ZEND_FRAMELESS_FUNCTION_PARAMETERS_0 zval *return_value
#define ZEND_FRAMELESS_FUNCTION_PARAMETERS_1 zval *return_value, zval *arg1
#define ZEND_FRAMELESS_FUNCTION_PARAMETERS_2 zval *return_value, zval *arg1, zval *arg2
#define ZEND_FRAMELESS_FUNCTION_PARAMETERS_3 zval *return_value, zval *arg1, zval *arg2, zval *arg3

#define ZEND_FRAMELESS_FUNCTION_NAME(name, arity) zflf_##name##_##arity
#define ZEND_OP_IS_FRAMELESS_ICALL(opcode) ((opcode) >= ZEND_FRAMELESS_ICALL_0 && (opcode) <= ZEND_FRAMELESS_ICALL_3)
#define ZEND_FLF_NUM_ARGS(opcode) ((opcode) - ZEND_FRAMELESS_ICALL_0)
#define ZEND_FLF_FUNC(opline) (zend_flf_functions[(opline)->extended_value])
#define ZEND_FLF_HANDLER(opline) (zend_flf_handlers[(opline)->extended_value])

#define ZEND_FRAMELESS_FUNCTION(name, arity) \
	void ZEND_FRAMELESS_FUNCTION_NAME(name, arity)(ZEND_FRAMELESS_FUNCTION_PARAMETERS_##arity)

#define Z_FLF_PARAM_ZVAL(arg_num, dest) \
	dest = arg ## arg_num;
#define Z_FLF_PARAM_ARRAY(arg_num, dest) \
	if (!zend_parse_arg_array(arg ## arg_num, &dest, /* null_check */ false, /* or_object */ false)) { \
		zend_wrong_parameter_type_error(arg_num, Z_EXPECTED_ARRAY, arg ## arg_num); \
		goto flf_clean; \
	}
#define Z_FLF_PARAM_ARRAY_OR_NULL(arg_num, dest) \
	if (!zend_parse_arg_array(arg ## arg_num, &dest, /* null_check */ true, /* or_object */ false)) { \
		zend_wrong_parameter_type_error(arg_num, Z_EXPECTED_ARRAY_OR_NULL, arg ## arg_num); \
		goto flf_clean; \
	}
#define Z_FLF_PARAM_ARRAY_HT_OR_STR(arg_num, dest_ht, dest_str, str_tmp) \
	if (Z_TYPE_P(arg ## arg_num) == IS_STRING) { \
		dest_ht = NULL; \
		dest_str = Z_STR_P(arg ## arg_num); \
	} else if (EXPECTED(Z_TYPE_P(arg ## arg_num) == IS_ARRAY)) { \
		dest_ht = Z_ARRVAL_P(arg ## arg_num); \
		dest_str = NULL; \
	} else { \
		dest_ht = NULL; \
		ZVAL_COPY(&str_tmp, arg ## arg_num); \
		arg ## arg_num = &str_tmp; \
		if (!zend_flf_parse_arg_str_slow(arg ## arg_num, &dest_str, arg_num)) { \
			zend_wrong_parameter_type_error(arg_num, Z_EXPECTED_ARRAY_OR_STRING, arg ## arg_num); \
			goto flf_clean; \
		} \
	}
#define Z_FLF_PARAM_BOOL(arg_num, dest) \
	if (!zend_parse_arg_bool_ex(arg ## arg_num, &dest, /* is_null */ NULL, /* null_check */ false, arg_num, /* frameless */ true)) { \
		zend_wrong_parameter_type_error(arg_num, Z_EXPECTED_BOOL, arg ## arg_num); \
		goto flf_clean; \
	}
#define Z_FLF_PARAM_LONG(arg_num, dest) \
	if (!zend_parse_arg_long_ex(arg ## arg_num, &dest, /* is_null */ NULL, /* null_check */ false, arg_num, /* frameless */ true)) { \
		zend_wrong_parameter_type_error(arg_num, Z_EXPECTED_LONG, arg ## arg_num); \
		goto flf_clean; \
	}
#define Z_FLF_PARAM_LONG_OR_NULL(arg_num, is_null, dest) \
	if (!zend_parse_arg_long_ex(arg ## arg_num, &dest, &is_null, /* null_check */ true, arg_num, /* frameless */ true)) { \
		zend_wrong_parameter_type_error(arg_num, Z_EXPECTED_LONG_OR_NULL, arg ## arg_num); \
		goto flf_clean; \
	}
#define Z_FLF_PARAM_STR(arg_num, dest, tmp) \
	if (Z_TYPE_P(arg ## arg_num) == IS_STRING) { \
		dest = Z_STR_P(arg ## arg_num); \
	} else { \
		ZVAL_COPY(&tmp, arg ## arg_num); \
		arg ## arg_num = &tmp; \
		if (!zend_parse_arg_str_ex(arg ## arg_num, &dest, /* null_check */ false, arg_num, /* frameless */ true)) { \
			zend_wrong_parameter_type_error(arg_num, Z_EXPECTED_STRING, arg ## arg_num); \
			goto flf_clean; \
		} \
	}
#define Z_FLF_PARAM_FREE_STR(arg_num, tmp) \
	if (UNEXPECTED(arg ## arg_num == &tmp)) { \
		zval_ptr_dtor(arg ## arg_num); \
	}

BEGIN_EXTERN_C()

typedef struct _zval_struct zval;
typedef struct _zend_op zend_op;
typedef union _zend_function zend_function;

typedef void (*zend_frameless_function_0)(zval *return_value);
typedef void (*zend_frameless_function_1)(zval *return_value, zval *op1);
typedef void (*zend_frameless_function_2)(zval *return_value, zval *op1, zval *op2);
typedef void (*zend_frameless_function_3)(zval *return_value, zval *op1, zval *op2, zval *op3);

extern size_t zend_flf_count;
extern size_t zend_flf_capacity;
ZEND_API extern void **zend_flf_handlers;
extern zend_function **zend_flf_functions;

typedef struct {
	void *handler;
	uint32_t num_args;
} zend_frameless_function_info;

typedef enum {
	ZEND_JMP_FL_UNPRIMED = 0,
	ZEND_JMP_FL_MISS = 1,
	ZEND_JMP_FL_HIT = 2,
} zend_jmp_fl_result;

END_EXTERN_C()

#endif
