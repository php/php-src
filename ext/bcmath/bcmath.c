/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andi Gutmans <andi@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#ifdef HAVE_BCMATH

#include "php_ini.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "bcmath_arginfo.h"
#include "ext/standard/info.h"
#include "php_bcmath.h"
#include "libbcmath/src/bcmath.h"

/* Always pair SETUP with TEARDOWN, and do so in the outer scope!
 * Should not be used when data can escape the function. */
#define BC_ARENA_SETUP \
	char bc_arena[BC_ARENA_SIZE]; \
	BCG(arena) = bc_arena;

#define BC_ARENA_TEARDOWN \
	BCG(arena) = NULL; \
	BCG(arena_offset) = 0;

ZEND_DECLARE_MODULE_GLOBALS(bcmath)
static PHP_GINIT_FUNCTION(bcmath);
static PHP_GSHUTDOWN_FUNCTION(bcmath);
static PHP_MINIT_FUNCTION(bcmath);
static PHP_MSHUTDOWN_FUNCTION(bcmath);
static PHP_MINFO_FUNCTION(bcmath);

zend_module_entry bcmath_module_entry = {
	STANDARD_MODULE_HEADER,
	"bcmath",
	ext_functions,
	PHP_MINIT(bcmath),
	PHP_MSHUTDOWN(bcmath),
	NULL,
	NULL,
	PHP_MINFO(bcmath),
	PHP_BCMATH_VERSION,
	PHP_MODULE_GLOBALS(bcmath),
	PHP_GINIT(bcmath),
	PHP_GSHUTDOWN(bcmath),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_BCMATH
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(bcmath)
#endif

ZEND_INI_MH(OnUpdateScale)
{
	int *p;
	zend_long tmp;

	tmp = zend_ini_parse_quantity_warn(new_value, entry->name);
	if (tmp < 0 || tmp > INT_MAX) {
		return FAILURE;
	}

	p = (int *) ZEND_INI_GET_ADDR();
	*p = (int) tmp;

	return SUCCESS;
}

/* {{{ PHP_INI */
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("bcmath.scale", "0", PHP_INI_ALL, OnUpdateScale, bc_precision, zend_bcmath_globals, bcmath_globals)
PHP_INI_END()
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(bcmath)
{
#if defined(COMPILE_DL_BCMATH) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	bcmath_globals->bc_precision = 0;
	bcmath_globals->arena = NULL;
	bcmath_globals->arena_offset = 0;
	bc_init_numbers();
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION */
static PHP_GSHUTDOWN_FUNCTION(bcmath)
{
	_bc_free_num_ex(&bcmath_globals->_zero_, 1);
	_bc_free_num_ex(&bcmath_globals->_one_, 1);
	_bc_free_num_ex(&bcmath_globals->_two_, 1);
	bcmath_globals->arena = NULL;
	bcmath_globals->arena_offset = 0;
}
/* }}} */

static void bcmath_number_register_class(void);

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(bcmath)
{
	REGISTER_INI_ENTRIES();
	bcmath_number_register_class();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(bcmath)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(bcmath)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "BCMath support", "enabled");
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}
/* }}} */

static inline void php_long2num(bc_num *num, zend_long lval)
{
	bc_long2num(num, lval);
}

static inline zend_result php_str2num_ex(bc_num *num, const zend_string *str, size_t *full_scale)
{
	if (!bc_str2num(num, ZSTR_VAL(str), ZSTR_VAL(str) + ZSTR_LEN(str), 0, full_scale, true)) {
		return FAILURE;
	}

	return SUCCESS;
}

/* {{{ php_str2num
   Convert to bc_num detecting scale */
static inline zend_result php_str2num(bc_num *num, const zend_string *str)
{
	return php_str2num_ex(num, str, NULL);
}
/* }}} */

/* {{{ Returns the sum of two arbitrary precision numbers */
PHP_FUNCTION(bcadd)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first = NULL, second = NULL, result = NULL;
	int scale;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(3, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	if (php_str2num(&first, left) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, right) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	result = bc_add (first, second, scale);

	RETVAL_NEW_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Returns the difference between two arbitrary precision numbers */
PHP_FUNCTION(bcsub)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first = NULL, second = NULL, result = NULL;
	int scale;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(3, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	if (php_str2num(&first, left) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, right) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	result = bc_sub (first, second, scale);

	RETVAL_NEW_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Returns the multiplication of two arbitrary precision numbers */
PHP_FUNCTION(bcmul)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first = NULL, second = NULL, result = NULL;
	int scale;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(3, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	if (php_str2num(&first, left) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, right) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	result = bc_multiply (first, second, scale);

	RETVAL_NEW_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Returns the quotient of two arbitrary precision numbers (division) */
PHP_FUNCTION(bcdiv)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first = NULL, second = NULL, result;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(3, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	bc_init_num(&result);

	if (php_str2num(&first, left) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, right) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	if (!bc_divide(first, second, &result, scale)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
		goto cleanup;
	}

	RETVAL_NEW_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Returns the modulus of the two arbitrary precision operands */
PHP_FUNCTION(bcmod)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first = NULL, second = NULL, result;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(3, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	bc_init_num(&result);

	if (php_str2num(&first, left) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, right) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	if (!bc_modulo(first, second, &result, scale)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
		goto cleanup;
	}

	RETVAL_NEW_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Returns the value of an arbitrary precision number raised to the power of another reduced by a modulus */
PHP_FUNCTION(bcpowmod)
{
	zend_string *base_str, *exponent_str, *modulus_str;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num bc_base = NULL, bc_expo = NULL, bc_modulus = NULL, result;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(3, 4)
		Z_PARAM_STR(base_str)
		Z_PARAM_STR(exponent_str)
		Z_PARAM_STR(modulus_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(4, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	bc_init_num(&result);

	if (php_str2num(&bc_base, base_str) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&bc_expo, exponent_str) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&bc_modulus, modulus_str) == FAILURE) {
		zend_argument_value_error(3, "is not well-formed");
		goto cleanup;
	}

	raise_mod_status status = bc_raisemod(bc_base, bc_expo, bc_modulus, &result, scale);
	switch (status) {
		case BASE_HAS_FRACTIONAL:
			zend_argument_value_error(1, "cannot have a fractional part");
			goto cleanup;
		case EXPO_HAS_FRACTIONAL:
			zend_argument_value_error(2, "cannot have a fractional part");
			goto cleanup;
		case EXPO_IS_NEGATIVE:
			zend_argument_value_error(2, "must be greater than or equal to 0");
			goto cleanup;
		case MOD_HAS_FRACTIONAL:
			zend_argument_value_error(3, "cannot have a fractional part");
			goto cleanup;
		case MOD_IS_ZERO:
			zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
			goto cleanup;
		case OK:
			RETVAL_NEW_STR(bc_num2str_ex(result, scale));
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}

	cleanup: {
		bc_free_num(&bc_base);
		bc_free_num(&bc_expo);
		bc_free_num(&bc_modulus);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Returns the value of an arbitrary precision number raised to the power of another */
PHP_FUNCTION(bcpow)
{
	zend_string *base_str, *exponent_str;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first = NULL, bc_exponent = NULL, result;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(base_str)
		Z_PARAM_STR(exponent_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(3, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	bc_init_num(&result);

	if (php_str2num(&first, base_str) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&bc_exponent, exponent_str) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	/* Check the exponent for scale digits and convert to a long. */
	if (bc_exponent->n_scale != 0) {
		zend_argument_value_error(2, "cannot have a fractional part");
		goto cleanup;
	}
	long exponent = bc_num2long(bc_exponent);
	if (exponent == 0 && (bc_exponent->n_len > 1 || bc_exponent->n_value[0] != 0)) {
		zend_argument_value_error(2, "is too large");
		goto cleanup;
	}

	bc_raise(first, exponent, &result, scale);

	RETVAL_NEW_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&bc_exponent);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Returns the square root of an arbitrary precision number */
PHP_FUNCTION(bcsqrt)
{
	zend_string *left;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num result = NULL;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(left)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(2, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	if (php_str2num(&result, left) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (bc_sqrt (&result, scale) != 0) {
		RETVAL_NEW_STR(bc_num2str_ex(result, scale));
	} else {
		zend_argument_value_error(1, "must be greater than or equal to 0");
	}

	cleanup: {
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Compares two arbitrary precision numbers */
PHP_FUNCTION(bccomp)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first = NULL, second = NULL;
	int scale = BCG(bc_precision);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(left)
		Z_PARAM_STR(right)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(scale_param, scale_param_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (scale_param_is_null) {
		scale = BCG(bc_precision);
	} else if (scale_param < 0 || scale_param > INT_MAX) {
		zend_argument_value_error(3, "must be between 0 and %d", INT_MAX);
		RETURN_THROWS();
	} else {
		scale = (int) scale_param;
	}

	BC_ARENA_SETUP;

	if (!bc_str2num(&first, ZSTR_VAL(left), ZSTR_VAL(left) + ZSTR_LEN(left), scale, NULL, false)) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (!bc_str2num(&second, ZSTR_VAL(right), ZSTR_VAL(right) + ZSTR_LEN(right), scale, NULL, false)) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	RETVAL_LONG(bc_compare(first, second));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ floor or ceil */
static void bcfloor_or_bcceil(INTERNAL_FUNCTION_PARAMETERS, bool is_floor)
{
	zend_string *numstr;
	bc_num num = NULL, result = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(numstr)
	ZEND_PARSE_PARAMETERS_END();

	BC_ARENA_SETUP;

	if (php_str2num(&num, numstr) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	result = bc_floor_or_ceil(num, is_floor);
	RETVAL_NEW_STR(bc_num2str_ex(result, 0));

	cleanup: {
		bc_free_num(&num);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Returns floor of num */
PHP_FUNCTION(bcfloor)
{
	bcfloor_or_bcceil(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}
/* }}} */

/* {{{ Returns ceil of num */
PHP_FUNCTION(bcceil)
{
	bcfloor_or_bcceil(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}
/* }}} */

/* {{{ Returns num rounded to the digits specified by precision. */
PHP_FUNCTION(bcround)
{
	zend_string *numstr;
	zend_long precision = 0;
	zend_long mode = PHP_ROUND_HALF_UP;
	bc_num num = NULL, result;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(numstr)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(precision)
		Z_PARAM_LONG(mode)
	ZEND_PARSE_PARAMETERS_END();

	switch (mode) {
		case PHP_ROUND_HALF_UP:
		case PHP_ROUND_HALF_DOWN:
		case PHP_ROUND_HALF_EVEN:
		case PHP_ROUND_HALF_ODD:
		case PHP_ROUND_CEILING:
		case PHP_ROUND_FLOOR:
		case PHP_ROUND_TOWARD_ZERO:
		case PHP_ROUND_AWAY_FROM_ZERO:
			break;
		default:
			zend_argument_value_error(3, "must be a valid rounding mode (PHP_ROUND_*)");
			return;
	}

	BC_ARENA_SETUP;

	bc_init_num(&result);

	if (php_str2num(&num, numstr) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	bc_round(num, precision, mode, &result);
	RETVAL_NEW_STR(bc_num2str_ex(result, result->n_scale));

	cleanup: {
		bc_free_num(&num);
		bc_free_num(&result);
		BC_ARENA_TEARDOWN;
	};
}
/* }}} */

/* {{{ Sets default scale parameter for all bc math functions */
PHP_FUNCTION(bcscale)
{
	zend_long old_scale, new_scale;
	bool new_scale_is_null = 1;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(new_scale, new_scale_is_null)
	ZEND_PARSE_PARAMETERS_END();

	old_scale = BCG(bc_precision);

	if (!new_scale_is_null) {
		if (new_scale < 0 || new_scale > INT_MAX) {
			zend_argument_value_error(1, "must be between 0 and %d", INT_MAX);
			RETURN_THROWS();
		}

		zend_string *ini_name = ZSTR_INIT_LITERAL("bcmath.scale", 0);
		zend_string *new_scale_str = zend_long_to_str(new_scale);
		zend_alter_ini_entry(ini_name, new_scale_str, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release(new_scale_str);
		zend_string_release(ini_name);
	}

	RETURN_LONG(old_scale);
}
/* }}} */



static zend_class_entry *bcmath_number_ce;
static zend_object_handlers bcmath_number_obj_handlers;

static inline bcmath_number_obj_t *get_bcmath_number_from_obj(zend_object *obj) {
	return (bcmath_number_obj_t*)((char*)(obj) - XtOffsetOf(bcmath_number_obj_t, std));
}

static inline bcmath_number_obj_t *get_bcmath_number_from_zval(zval *zv) {
	return get_bcmath_number_from_obj(Z_OBJ_P(zv));
}

static zend_object *bcmath_number_create(zend_class_entry *ce)
{
	bcmath_number_obj_t *intern;

	intern = zend_object_alloc(sizeof(bcmath_number_obj_t), ce);
	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);
	rebuild_object_properties(&intern->std);
	intern->num = NULL;
	intern->value = NULL;
	intern->scale = 1;

	return &intern->std;
}

static void bcmath_number_free(zend_object *object)
{
	bcmath_number_obj_t *intern = get_bcmath_number_from_obj(object);
	if (intern->num) {
		bc_free_num(&intern->num);
		intern->num = NULL;
	}
	if (intern->value) {
		zend_string_release(intern->value);
		intern->value = NULL;
	}
	zend_object_std_dtor(&intern->std);
}

static zend_object *bcmath_number_clone(zend_object *object)
{
	bcmath_number_obj_t *original = get_bcmath_number_from_obj(object);
	bcmath_number_obj_t *clone = get_bcmath_number_from_obj(bcmath_number_create(bcmath_number_ce));

	zend_objects_clone_members(&clone->std, &original->std);
	clone->num = bc_copy_num(original->num);
	if (original->value) {
		clone->value = zend_string_copy(original->value);
	}
	clone->scale = original->scale;

	return &clone->std;
}

static zend_result bc_num_from_zval(zval *zv, bc_num *num, size_t *full_scale, uint32_t arg_num)
{
	switch (Z_TYPE_P(zv)) {
		case IS_LONG:
			php_long2num(num, Z_LVAL_P(zv));
			if (full_scale) {
				*full_scale = 0;
			}
			break;

		case IS_STRING:
			if (php_str2num_ex(num, Z_STR_P(zv), full_scale) == FAILURE) {
				zend_argument_value_error(arg_num, "is not well-formed");
				return FAILURE;
			}
			break;

		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(zv), bcmath_number_ce)) {
				bcmath_number_obj_t *intern = get_bcmath_number_from_zval(zv);
				*num = intern->num;
				if (full_scale) {
					*full_scale = intern->scale;
				}
				break;
			}
			ZEND_FALLTHROUGH;

		default:
			zend_argument_type_error(arg_num, "must be of type int, string, or BcMath\\Number, %s given", zend_zval_value_name(zv));
			return FAILURE;
	}

	return SUCCESS;
}

static zend_result bcmath_number_do_calc(
	bc_num n1, bc_num n2, bc_num *ret,
	size_t n1_full_scale, size_t n2_full_scale, size_t *scale,
	bool auto_scale, bcmath_calc_type type, bool is_op
) {
	switch (type) {
		case BC_ADD:
			if (auto_scale) {
				*scale = MAX(n1_full_scale, n2_full_scale);
			}
			*ret = bc_add(n1, n2, *scale);
			break;
		case BC_SUB:
			if (auto_scale) {
				*scale = MAX(n1_full_scale, n2_full_scale);
			}
			*ret = bc_sub(n1, n2, *scale);
			break;
		case BC_MUL:
			if (auto_scale) {
				*scale = n1_full_scale + n2_full_scale;
			}
			*ret = bc_multiply(n1, n2, *scale);
			break;
		case BC_DIV:
			if (auto_scale) {
				*scale = n1_full_scale + BC_MATH_NUMBER_MAX_EX_SCALE;
			}
			if (UNEXPECTED(!bc_divide(n1, n2, ret, *scale))) {
				zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
				return FAILURE;
			}
			break;
		case BC_MOD:
			if (auto_scale) {
				*scale = MAX(n1_full_scale, n2_full_scale);
			}
			if (UNEXPECTED(!bc_modulo(n1, n2, ret, *scale))) {
				zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
				return FAILURE;
			}
			break;
		case BC_POW:
			/* Check the exponent for scale digits and convert to a long. */
			if (UNEXPECTED(n2->n_scale != 0)) {
				if (is_op) {
					zend_value_error("exponent cannot have a fractional part");
				} else{
					zend_argument_value_error(1, "exponent cannot have a fractional part");
				}
				return FAILURE;
			}
			long exponent = bc_num2long(n2);

			if (auto_scale) {
				if (exponent > 0) {
					*scale = n1_full_scale * exponent;
				} else if (exponent < 0) {
					*scale = n1_full_scale + BC_MATH_NUMBER_MAX_EX_SCALE;
				} else {
					*scale = 0;
				}
			}

			if (UNEXPECTED(exponent == 0 && (n2->n_len > 1 || n2->n_value[0] != 0))) {
				if (is_op) {
					zend_value_error("exponent is too large");
				} else {
					zend_argument_value_error(1, "exponent is too large");
				}
				return FAILURE;
			}
			bc_raise(n1, exponent, ret, *scale);
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	return SUCCESS;
}

static inline bcmath_number_obj_t *bcmath_number_new_obj(bc_num ret, size_t scale)
{
	bcmath_number_obj_t *intern = get_bcmath_number_from_obj(bcmath_number_create(bcmath_number_ce));
	intern->num = ret;
	intern->scale = scale;
	return intern;
}

static zend_result bcmath_number_do_operation(uint8_t opcode, zval *ret_val, zval *op1, zval *op2)
{
	bc_num n1 = NULL, n2 = NULL;
	size_t n1_full_scale, n2_full_scale;

	if (UNEXPECTED(bc_num_from_zval(op1, &n1, &n1_full_scale, 0) == FAILURE || bc_num_from_zval(op2, &n2, &n2_full_scale, 0) == FAILURE)) {
		goto fail;
	}

	/* For increment and decrement, etc */
	zval op1_copy;
	if (ret_val == op1) {
		ZVAL_COPY_VALUE(&op1_copy, op1);
		op1 = &op1_copy;
	}

	bcmath_calc_type type;
	switch (opcode) {
		case ZEND_ADD:
			type = BC_ADD;
			break;
		case ZEND_SUB:
			type = BC_SUB;
			break;
		case ZEND_MUL:
			type = BC_MUL;
			break;
		case ZEND_DIV:
			type = BC_DIV;
			break;
		case ZEND_MOD:
			type = BC_MOD;
			break;
		case ZEND_POW:
			type = BC_POW;
			break;
		default:
			return FAILURE;
	}

	bc_num ret = NULL;
	size_t scale = 0;
	if (UNEXPECTED(bcmath_number_do_calc(n1, n2, &ret, n1_full_scale, n2_full_scale, &scale, true, type, true) == FAILURE)) {
		goto fail;
	}

	if (n1 && Z_TYPE_P(op1) != IS_OBJECT) {
		bc_free_num(&n1);
	}
	if (n2 && Z_TYPE_P(op2) != IS_OBJECT) {
		bc_free_num(&n2);
	}

	bcmath_number_obj_t *intern = bcmath_number_new_obj(ret, scale);
	ZVAL_OBJ(ret_val, &intern->std);

	/* For increment and decrement, etc */
	if (ret == SUCCESS && op1 == &op1_copy) {
		zval_ptr_dtor(op1);
	}

	return SUCCESS;

fail:
	if (n1 && Z_TYPE_P(op1) != IS_OBJECT) {
		bc_free_num(&n1);
	}
	if (n2 && Z_TYPE_P(op2) != IS_OBJECT) {
		bc_free_num(&n2);
	}
	return FAILURE;
}

static int bcmath_number_compare(zval *z1, zval *z2)
{
	ZEND_COMPARE_OBJECTS_FALLBACK(z1, z2);

	bcmath_number_obj_t *n1 = get_bcmath_number_from_zval(z1);
	bcmath_number_obj_t *n2 = get_bcmath_number_from_zval(z2);

	return bc_compare(n1->num, n2->num);
}

static inline zend_string *bcmath_number_value_to_str(bcmath_number_obj_t *intern)
{
	if (intern->value == NULL) {
		intern->value = bc_num2str_ex(intern->num, intern->scale);
	}
	return intern->value;
}

static HashTable *bcmath_number_get_properties(zend_object *obj, zend_prop_purpose purpose)
{
	switch (purpose) {
		case ZEND_PROP_PURPOSE_DEBUG:
		case ZEND_PROP_PURPOSE_SERIALIZE:
		case ZEND_PROP_PURPOSE_VAR_EXPORT:
		case ZEND_PROP_PURPOSE_JSON:
		case ZEND_PROP_PURPOSE_ARRAY_CAST:
			break;
		default:
			return zend_std_get_properties_for(obj, purpose);
	}

	zval zv;
	bcmath_number_obj_t *intern = get_bcmath_number_from_obj(obj);
	HashTable *props = zend_array_dup(zend_std_get_properties(obj));

	ZVAL_STR(&zv, bcmath_number_value_to_str(intern));
	zend_hash_str_update(props, "value", sizeof("value")-1, &zv);
	ZVAL_LONG(&zv, intern->scale);
	zend_hash_str_update(props, "scale", sizeof("scale")-1, &zv);

	return props;
}

static zval *bcmath_number_read_property(zend_object *obj, zend_string *name, int type, void **cache_slot, zval *rv)
{
	bcmath_number_obj_t *intern = get_bcmath_number_from_obj(obj);

	if (zend_string_equals_literal(name, "value")) {
		ZVAL_STR_COPY(rv, bcmath_number_value_to_str(intern));
		return rv;
	}

	if (zend_string_equals_literal(name, "scale")) {
		ZVAL_LONG(rv, intern->scale);
		return rv;
	}

	// fallback
	return zend_std_read_property(obj, name, type, cache_slot, rv);
}

static zend_result bcmath_number_cast_object(zend_object *obj, zval *ret, int type)
{
	if (type == _IS_BOOL) {
		bcmath_number_obj_t *intern = get_bcmath_number_from_obj(obj);
		ZVAL_BOOL(ret, !bc_is_zero(intern->num));
		return SUCCESS;
	}

	return zend_std_cast_object_tostring(obj, ret, type);
}

static HashTable *bcmath_number_get_gc(zend_object *obj, zval **table, int *n)
{
	*table = NULL;
	*n = 0;
	return zend_std_get_properties(obj);
}

static void bcmath_number_register_class(void)
{
	bcmath_number_ce = register_class_BcMath_Number(zend_ce_stringable);
	bcmath_number_ce->create_object = bcmath_number_create;
	bcmath_number_ce->default_object_handlers = &bcmath_number_obj_handlers;

	memcpy(&bcmath_number_obj_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	bcmath_number_obj_handlers.offset = XtOffsetOf(bcmath_number_obj_t, std);
	bcmath_number_obj_handlers.free_obj = bcmath_number_free;
	bcmath_number_obj_handlers.clone_obj = bcmath_number_clone;
	bcmath_number_obj_handlers.do_operation = bcmath_number_do_operation;
	bcmath_number_obj_handlers.compare = bcmath_number_compare;
	bcmath_number_obj_handlers.read_property = bcmath_number_read_property;
	bcmath_number_obj_handlers.get_properties_for = bcmath_number_get_properties;
	bcmath_number_obj_handlers.cast_object = bcmath_number_cast_object;
	bcmath_number_obj_handlers.get_gc = bcmath_number_get_gc;
}

PHP_METHOD(BcMath_Number, __construct)
{
	zval *zv;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(zv);
	ZEND_PARSE_PARAMETERS_END();

	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);

	bc_num num = NULL;
	size_t scale;
	if (bc_num_from_zval(zv, &num, &scale, 1) == FAILURE) {
		RETURN_THROWS();
	}

	intern->num = num;
	intern->scale = scale;
}

static inline zend_result bc_check_scale(zend_long scale, bool scale_is_null, uint32_t arg_num)
{
	if (UNEXPECTED(!scale_is_null && (scale < 0 || scale > INT_MAX))) {
		zend_argument_value_error(arg_num, "must be between 0 and %d", INT_MAX);
		return FAILURE;
	}
	return SUCCESS;
}

static void bcmath_number_calc_method(INTERNAL_FUNCTION_PARAMETERS, bcmath_calc_type type)
{
	zval *zv;
	zend_long arg_scale;
	bool scale_is_null = true;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(zv);
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(arg_scale, scale_is_null);
	ZEND_PARSE_PARAMETERS_END();

	bc_num num = NULL;
	size_t num_full_scale;
	if (bc_num_from_zval(zv, &num, &num_full_scale, 1) == FAILURE) {
		RETURN_THROWS();
	}

	if (bc_check_scale(arg_scale, scale_is_null, 2) == FAILURE) {
		bc_free_num(&num);
		RETURN_THROWS();
	}

	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);

	bc_num ret = NULL;
	size_t scale = scale_is_null ? 0 : arg_scale;
	zend_result calc_ret = bcmath_number_do_calc(
		intern->num, num, &ret,
		intern->scale, num_full_scale, &scale,
		scale_is_null, type, false
	);
	if (Z_TYPE_P(zv) != IS_OBJECT) {
		bc_free_num(&num);
	}
	if (UNEXPECTED(calc_ret == FAILURE)) {
		bc_free_num(&ret);
		RETURN_THROWS();
	}

	bcmath_number_obj_t *new_intern = bcmath_number_new_obj(ret, scale);
	RETURN_OBJ(&new_intern->std);
}

PHP_METHOD(BcMath_Number, add)
{
	bcmath_number_calc_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, BC_ADD);
}

PHP_METHOD(BcMath_Number, sub)
{
	bcmath_number_calc_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, BC_SUB);
}

PHP_METHOD(BcMath_Number, mul)
{
	bcmath_number_calc_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, BC_MUL);
}

PHP_METHOD(BcMath_Number, div)
{
	bcmath_number_calc_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, BC_DIV);
}

PHP_METHOD(BcMath_Number, mod)
{
	bcmath_number_calc_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, BC_MOD);
}

PHP_METHOD(BcMath_Number, pow)
{
	bcmath_number_calc_method(INTERNAL_FUNCTION_PARAM_PASSTHRU, BC_POW);
}

PHP_METHOD(BcMath_Number, powmod)
{
	zval *exponent;
	zval *modulus;
	zend_long arg_scale;
	bool scale_is_null = true;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ZVAL(exponent);
		Z_PARAM_ZVAL(modulus);
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(arg_scale, scale_is_null);
	ZEND_PARSE_PARAMETERS_END();

	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);

	bc_num exponent_num = NULL;
	bc_num modulus_num = NULL;

	if (bc_num_from_zval(exponent, &exponent_num, NULL, 1) == FAILURE) {
		goto cleanup;
	}
	if (bc_num_from_zval(modulus, &modulus_num, NULL, 2) == FAILURE) {
		goto cleanup;
	}
	if (bc_check_scale(arg_scale, scale_is_null, 3) == FAILURE) {
		goto cleanup;
	}

	size_t scale = 0;

	bc_num ret = NULL;
	raise_mod_status status = bc_raisemod(intern->num, exponent_num, modulus_num, &ret, scale);
	switch (status) {
		case BASE_HAS_FRACTIONAL:
			zend_value_error("Base number cannot have a fractional part");
			goto cleanup;
		case EXPO_HAS_FRACTIONAL:
			zend_argument_value_error(1, "cannot have a fractional part");
			goto cleanup;
		case EXPO_IS_NEGATIVE:
			zend_argument_value_error(1, "must be greater than or equal to 0");
			goto cleanup;
		case MOD_HAS_FRACTIONAL:
			zend_argument_value_error(2, "cannot have a fractional part");
			goto cleanup;
		case MOD_IS_ZERO:
			zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
			goto cleanup;
		case OK:
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}

	if (Z_TYPE_P(exponent) != IS_OBJECT) {
		bc_free_num(&exponent_num);
	}
	if (Z_TYPE_P(modulus) != IS_OBJECT) {
		bc_free_num(&modulus_num);
	}

	bcmath_number_obj_t *new_intern = bcmath_number_new_obj(ret, scale);
	RETURN_OBJ(&new_intern->std);

cleanup:
	if (exponent_num && Z_TYPE_P(exponent) != IS_OBJECT) {
		bc_free_num(&exponent_num);
	}
	if (modulus_num && Z_TYPE_P(modulus) != IS_OBJECT) {
		bc_free_num(&modulus_num);
	}
	RETURN_THROWS();
}

PHP_METHOD(BcMath_Number, sqrt)
{
	zend_long arg_scale;
	bool scale_is_null = true;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(arg_scale, scale_is_null);
	ZEND_PARSE_PARAMETERS_END();

	if (bc_check_scale(arg_scale, scale_is_null, 1) == FAILURE) {
		RETURN_THROWS();
	}

	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);

	size_t scale;
	if (scale_is_null) {
		scale = intern->num->n_scale;
	} else {
		scale = arg_scale;
	}

	bc_num ret = bc_copy_num(intern->num);
	if (!bc_sqrt (&ret, scale)) {
		zend_value_error("Base number must be greater than or equal to 0");
		bc_free_num(&ret);
		RETURN_THROWS();
	}

	bcmath_number_obj_t *new_intern = bcmath_number_new_obj(ret, scale);
	RETURN_OBJ(&new_intern->std);
}

PHP_METHOD(BcMath_Number, comp)
{
	zval *zv;
	zend_long arg_scale;
	bool scale_is_null = true;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(zv);
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(arg_scale, scale_is_null);
	ZEND_PARSE_PARAMETERS_END();

	bc_num num = NULL;
	if (bc_num_from_zval(zv, &num, NULL, 1) == FAILURE) {
		RETURN_THROWS();
	}

	if (bc_check_scale(arg_scale, scale_is_null, 2) == FAILURE) {
		bc_free_num(&num);
		RETURN_THROWS();
	}

	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);

	size_t scale;
	if (scale_is_null) {
		scale = MAX(intern->num->n_scale, num->n_scale);
	} else {
		scale = arg_scale;
	}

	zend_long ret = bc_compare(intern->num, num);
	if (Z_TYPE_P(zv) != IS_OBJECT) {
		bc_free_num(&num);
	}

	RETURN_LONG(ret);
}

static void bcmath_number_floor_or_ceil(INTERNAL_FUNCTION_PARAMETERS, bool is_floor)
{
	ZEND_PARSE_PARAMETERS_NONE();

	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);

	bc_num ret = bc_floor_or_ceil(intern->num, is_floor);

	bcmath_number_obj_t *new_intern = bcmath_number_new_obj(ret, 0);
	RETURN_OBJ(&new_intern->std);
}

PHP_METHOD(BcMath_Number, floor)
{
	bcmath_number_floor_or_ceil(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

PHP_METHOD(BcMath_Number, ceil)
{
	bcmath_number_floor_or_ceil(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(BcMath_Number, round)
{
	zend_long precision = 0;
	zend_long rounding_mode = PHP_ROUND_HALF_UP;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(precision);
		Z_PARAM_LONG(rounding_mode);
	ZEND_PARSE_PARAMETERS_END();

	switch (rounding_mode) {
		case PHP_ROUND_HALF_UP:
		case PHP_ROUND_HALF_DOWN:
		case PHP_ROUND_HALF_EVEN:
		case PHP_ROUND_HALF_ODD:
		case PHP_ROUND_CEILING:
		case PHP_ROUND_FLOOR:
		case PHP_ROUND_TOWARD_ZERO:
		case PHP_ROUND_AWAY_FROM_ZERO:
			break;
		default:
			zend_argument_value_error(2, "must be a valid rounding mode (PHP_ROUND_*)");
			RETURN_THROWS();
	}

	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);

	bc_num ret = NULL;
	bc_round(intern->num, precision, rounding_mode, &ret);

	bcmath_number_obj_t *new_intern = bcmath_number_new_obj(ret, ret->n_scale);
	RETURN_OBJ(&new_intern->std);
}

PHP_METHOD(BcMath_Number, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();
	bcmath_number_obj_t *intern = get_bcmath_number_from_zval(ZEND_THIS);
	RETURN_STR_COPY(bcmath_number_value_to_str(intern));
}

#endif
