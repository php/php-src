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
#include "bcmath_arginfo.h"
#include "ext/standard/info.h"
#include "php_bcmath.h"
#include "libbcmath/src/bcmath.h"

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
	bc_init_numbers();
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION */
static PHP_GSHUTDOWN_FUNCTION(bcmath)
{
	_bc_free_num_ex(&bcmath_globals->_zero_, 1);
	_bc_free_num_ex(&bcmath_globals->_one_, 1);
	_bc_free_num_ex(&bcmath_globals->_two_, 1);
}
/* }}} */

static void bc_num_register_class(void);

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(bcmath)
{
	REGISTER_INI_ENTRIES();
	bc_num_register_class();

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

/* {{{ php_str2num
   Convert to bc_num detecting scale */
static zend_result php_str2num(bc_num *num, char *str)
{
	char *p;

	if (!(p = strchr(str, '.'))) {
		if (!bc_str2num(num, str, 0)) {
			return FAILURE;
		}

		return SUCCESS;
	}

	if (!bc_str2num(num, str, strlen(p+1))) {
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ Returns the sum of two arbitrary precision numbers */
PHP_FUNCTION(bcadd)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first, second, result;
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

	bc_init_num(&first);
	bc_init_num(&second);
	bc_init_num(&result);

	if (php_str2num(&first, ZSTR_VAL(left)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, ZSTR_VAL(right)) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	bc_add (first, second, &result, scale);

	RETVAL_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Returns the difference between two arbitrary precision numbers */
PHP_FUNCTION(bcsub)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first, second, result;
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

	bc_init_num(&first);
	bc_init_num(&second);
	bc_init_num(&result);

	if (php_str2num(&first, ZSTR_VAL(left)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, ZSTR_VAL(right)) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	bc_sub (first, second, &result, scale);

	RETVAL_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Returns the multiplication of two arbitrary precision numbers */
PHP_FUNCTION(bcmul)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first, second, result;
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

	bc_init_num(&first);
	bc_init_num(&second);
	bc_init_num(&result);

	if (php_str2num(&first, ZSTR_VAL(left)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, ZSTR_VAL(right)) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	bc_multiply (first, second, &result, scale);

	RETVAL_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Returns the quotient of two arbitrary precision numbers (division) */
PHP_FUNCTION(bcdiv)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first, second, result;
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

	bc_init_num(&first);
	bc_init_num(&second);
	bc_init_num(&result);

	if (php_str2num(&first, ZSTR_VAL(left)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, ZSTR_VAL(right)) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	if (!bc_divide(first, second, &result, scale)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
		goto cleanup;
	}

	RETVAL_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Returns the modulus of the two arbitrary precision operands */
PHP_FUNCTION(bcmod)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first, second, result;
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

	bc_init_num(&first);
	bc_init_num(&second);
	bc_init_num(&result);

	if (php_str2num(&first, ZSTR_VAL(left)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&second, ZSTR_VAL(right)) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	if (!bc_modulo(first, second, &result, scale)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
		goto cleanup;
	}

	RETVAL_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Returns the value of an arbitrary precision number raised to the power of another reduced by a modulus */
PHP_FUNCTION(bcpowmod)
{
	zend_string *base_str, *exponent_str, *modulus_str;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num bc_base, bc_expo, bc_modulus, result;
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

	bc_init_num(&bc_base);
	bc_init_num(&bc_expo);
	bc_init_num(&bc_modulus);
	bc_init_num(&result);

	if (php_str2num(&bc_base, ZSTR_VAL(base_str)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&bc_expo, ZSTR_VAL(exponent_str)) == FAILURE) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&bc_modulus, ZSTR_VAL(modulus_str)) == FAILURE) {
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
			RETVAL_STR(bc_num2str_ex(result, scale));
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}

	cleanup: {
		bc_free_num(&bc_base);
		bc_free_num(&bc_expo);
		bc_free_num(&bc_modulus);
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Returns the value of an arbitrary precision number raised to the power of another */
PHP_FUNCTION(bcpow)
{
	zend_string *base_str, *exponent_str;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first, bc_exponent, result;
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

	bc_init_num(&first);
	bc_init_num(&bc_exponent);
	bc_init_num(&result);

	if (php_str2num(&first, ZSTR_VAL(base_str)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (php_str2num(&bc_exponent, ZSTR_VAL(exponent_str)) == FAILURE) {
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

	RETVAL_STR(bc_num2str_ex(result, scale));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&bc_exponent);
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Returns the square root of an arbitrary precision number */
PHP_FUNCTION(bcsqrt)
{
	zend_string *left;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num result;
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

	bc_init_num(&result);

	if (php_str2num(&result, ZSTR_VAL(left)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (bc_sqrt (&result, scale) != 0) {
		RETVAL_STR(bc_num2str_ex(result, scale));
	} else {
		zend_argument_value_error(1, "must be greater than or equal to 0");
	}

	cleanup: {
		bc_free_num(&result);
	};
}
/* }}} */

/* {{{ Compares two arbitrary precision numbers */
PHP_FUNCTION(bccomp)
{
	zend_string *left, *right;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	bc_num first, second;
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

	bc_init_num(&first);
	bc_init_num(&second);

	if (!bc_str2num(&first, ZSTR_VAL(left), scale)) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	if (!bc_str2num(&second, ZSTR_VAL(right), scale)) {
		zend_argument_value_error(2, "is not well-formed");
		goto cleanup;
	}

	RETVAL_LONG(bc_compare(first, second));

	cleanup: {
		bc_free_num(&first);
		bc_free_num(&second);
	};
}
/* }}} */

/* {{{ floor or ceil */
static void bcfloor_or_bcceil(INTERNAL_FUNCTION_PARAMETERS, bool is_floor)
{
	zend_string *numstr;
	bc_num num, result;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(numstr)
	ZEND_PARSE_PARAMETERS_END();

	bc_init_num(&num);
	bc_init_num(&result);

	if (php_str2num(&num, ZSTR_VAL(numstr)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	bc_floor_or_ceil(num, is_floor, &result);
	RETVAL_STR(bc_num2str_ex(result, 0));

	cleanup: {
		bc_free_num(&num);
		bc_free_num(&result);
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
	bc_num num, result;

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

	bc_init_num(&num);
	bc_init_num(&result);

	if (php_str2num(&num, ZSTR_VAL(numstr)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		goto cleanup;
	}

	bc_round(num, precision, mode, &result);
	RETVAL_STR(bc_num2str_ex(result, result->n_scale));

	cleanup: {
		bc_free_num(&num);
		bc_free_num(&result);
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

static zend_class_entry *bc_num_ce;
static zend_object_handlers bc_num_obj_handlers;
#define IS_BC_NUM(zval) (Z_TYPE_P(zval) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zval), bc_num_ce))

static inline bc_num_obj *bc_num_obj_from_obj(zend_object *obj) {
	return (bc_num_obj*)((char*)(obj) - XtOffsetOf(bc_num_obj, std));
}

static inline bc_num_obj *bc_num_obj_from_zval(zval *zv) {
	return bc_num_obj_from_obj(Z_OBJ_P(zv));
}

/* {{{ bc_num_ce->create_object */
static zend_object *bc_num_create_obj(zend_class_entry *ce)
{
	bc_num_obj *intern;

	intern = zend_object_alloc(sizeof(bc_num_obj), ce);
	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);
	rebuild_object_properties(&intern->std);

	return &intern->std;
}
/* }}} */

/* {{{ bc_num_obj_handlers.free_obj */
static void bc_num_free_obj(zend_object *object)
{
	bc_num_obj *intern = bc_num_obj_from_obj(object);
	if (intern->bc_num) {
		bc_free_num(&intern->bc_num);
	}
	efree(intern->bc_num);
	zend_object_std_dtor(&intern->std);
}
/* }}} */

/* {{{ bc_num_obj_handlers.clone_obj */
static zend_object *bc_num_clone_obj(zend_object *object)
{
	bc_num_obj *old_obj = bc_num_obj_from_obj(object);
	bc_num_obj *new_obj = bc_num_obj_from_obj(bc_num_create_obj(old_obj->std.ce));

	zend_objects_clone_members(&new_obj->std, &old_obj->std);
	new_obj->bc_num = bc_copy_num(old_obj->bc_num);

	return &new_obj->std;
}
/* }}} */

static zend_result convert_zval_to_bc_num(zval *zv, bc_num *num)
{
	switch (Z_TYPE_P(zv)) {
		case IS_LONG:
		case IS_STRING:
			convert_to_string(zv);
			bc_init_num(&num1);
			if (!bc_str2num(num, Z_STRVAL_P(zv), 0)) {
				return FAILURE;
			}
			break;
		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(zv), bc_num_ce)) {
				bc_free_num(num);
				*num = bc_num_obj_from_zval(zv)->bc_num;
			} else {
				zend_argument_type_error(0, "must be of type int, string, or BcNum, %s given", zend_zval_value_name(zv));
				return FAILURE;
			}
			break;
	}

	return SUCCESS;
}

static zend_result bc_num_calculation(zval *result, zval *op1, zval *op2, bc_num_calculation_type type, bool is_operator)
{
	bc_num num1, num2;

	if (convert_zval_to_bc_num(op1, &num1) == FAILURE || convert_zval_to_bc_num(op2, &num2) == FAILURE) {
		return FAILURE;
	}

	bc_num_obj *result_obj = bc_num_obj_from_obj(bc_num_create_obj(bc_num_ce));
	bc_init_num(&result_obj->bc_num);

	size_t scale = MAX(num1->n_scale, num2->n_scale);

	switch (type) {
		case BC_NUM_ADD:
			bc_add(num1, num2, &result_obj->bc_num, scale);
			break;
		case BC_NUM_SUB:
			bc_sub(num1, num2, &result_obj->bc_num, scale);
			break;
		case BC_NUM_MUL:
			bc_multiply(num1, num2, &result_obj->bc_num, scale);
			break;
		case BC_NUM_DIV:
			bc_divide(num1, num2, &result_obj->bc_num, scale);
			break;
		case BC_NUM_MOD:
			bc_modulo(num1, num2, &result_obj->bc_num, scale);
			break;
		case BC_NUM_POW:
			{
				long exponent = bc_num2long(num2);
				if (exponent == 0 && (num2->n_len > 1 || num2->n_value[0] != 0)) {
					zend_argument_value_error(is_operator ? 0 : 1, "exponent is too large");
					return FAILURE;
				}
				bc_raise(num1, bc_num2long(num2), &result_obj->bc_num, scale);
			}
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}

	result_obj->bc_num->n_scale = scale;
	ZVAL_OBJ(result, &result_obj->std);
	return SUCCESS;
}

/* {{{ bc_num_obj_handlers.do_operation */
static int bc_num_do_operation(uint8_t opcode, zval *result, zval *op1, zval *op2)
{
	zend_result ret;

	switch (opcode) {
		case ZEND_ADD:
			ret = bc_num_calculation(result, op1, op2, BC_NUM_ADD, true);
			break;
		case ZEND_SUB:
			ret = bc_num_calculation(result, op1, op2, BC_NUM_SUB, true);
			break;
		case ZEND_MUL:
			ret = bc_num_calculation(result, op1, op2, BC_NUM_MUL, true);
			break;
		case ZEND_POW:
			ret = bc_num_calculation(result, op1, op2, BC_NUM_POW, true);
			break;
		case ZEND_DIV:
			ret = bc_num_calculation(result, op1, op2, BC_NUM_DIV, true);
			break;
		case ZEND_MOD:
			ret = bc_num_calculation(result, op1, op2, BC_NUM_MOD, true);
			break;
		default:
			return FAILURE;
	}

	return ret;
}
/* }}} */

/* {{{ bc_num_obj_handlers.compare */
static int bc_num_compare(zval *z1, zval *z2)
{
	bc_num_obj *obj1;
	bc_num_obj *obj2;

	ZEND_COMPARE_OBJECTS_FALLBACK(z1, z2);

	obj1 = bc_num_obj_from_zval(z1);
	obj2 = bc_num_obj_from_zval(z2);

	return bc_compare(obj1->bc_num, obj2->bc_num);
}
/* }}} */

/* {{{ bc_num_obj_handlers.get_properties_for */
static HashTable *bc_num_get_properties_for(zend_object *object, zend_prop_purpose purpose)
{
	HashTable *props;
	bc_num_obj *bc_num_obj;

	switch (purpose) {
		case ZEND_PROP_PURPOSE_DEBUG:
		case ZEND_PROP_PURPOSE_SERIALIZE:
		case ZEND_PROP_PURPOSE_VAR_EXPORT:
		case ZEND_PROP_PURPOSE_JSON:
		case ZEND_PROP_PURPOSE_ARRAY_CAST:
			break;
		default:
			return zend_std_get_properties_for(object, purpose);
	}

	zval zv;
	bc_num_obj = bc_num_obj_from_obj(object);
	props = zend_array_dup(zend_std_get_properties(object));

	ZVAL_STR(&zv, bc_num2str(bc_num_obj->bc_num));
	zend_hash_str_update(props, "num", sizeof("num")-1, &zv);
	ZVAL_LONG(&zv, bc_num_obj->bc_num->n_scale);
	zend_hash_str_update(props, "scale", sizeof("scale")-1, &zv);

	return props;
}
/* }}} */

/* {{{ bc_num_obj_handlers.get_gc */
static HashTable *bc_num_get_gc(zend_object *object, zval **table, int *n)
{
	*table = NULL;
	*n = 0;
	return zend_std_get_properties(object);
}
/* }}} */

static void bc_num_register_class(void)
{
	bc_num_ce = register_class_BcNum();
	bc_num_ce->create_object = bc_num_create_obj;
	bc_num_ce->default_object_handlers = &bc_num_obj_handlers;

	memcpy(&bc_num_obj_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	bc_num_obj_handlers.offset = XtOffsetOf(bc_num_obj, std);
	bc_num_obj_handlers.free_obj = bc_num_free_obj;
	bc_num_obj_handlers.clone_obj = bc_num_clone_obj;
	bc_num_obj_handlers.do_operation = bc_num_do_operation;
	bc_num_obj_handlers.compare = bc_num_compare;
	bc_num_obj_handlers.get_properties_for = bc_num_get_properties_for;
	bc_num_obj_handlers.get_gc = bc_num_get_gc;
}

/* {{{ Creates new BcNum */
PHP_METHOD(BcNum, __construct)
{
	zend_string *num_str;
	zend_long scale_param;
	bool scale_param_is_null = 1;
	int scale;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(num_str)
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

	bc_num_obj *obj = bc_num_obj_from_zval(ZEND_THIS);
	bc_init_num(&obj->bc_num);

	if (php_str2num(&obj->bc_num, ZSTR_VAL(num_str)) == FAILURE) {
		zend_argument_value_error(1, "is not well-formed");
		bc_free_num(&obj->bc_num);
		RETURN_THROWS();
	}

	if (obj->bc_num->n_scale < scale) {
		char *nptr = (char *) (obj->bc_num->n_value + obj->bc_num->n_len + obj->bc_num->n_scale);
		for (int count = scale - obj->bc_num->n_scale; count > 0; count--) {
			*nptr++ = 0;
		}
	}
	obj->bc_num->n_scale = scale;
}
/* }}} */

#endif
