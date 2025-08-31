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
   | Author: Stanislav Malyshev <stas@php.net>                            |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "php_gmp.h"
#include "php_gmp_int.h"
#include "ext/standard/info.h"
#include "ext/standard/php_var.h"
#include "zend_smart_str_public.h"
#include "zend_exceptions.h"

#include <gmp.h>

/* Needed for gmp_random() */
#include "ext/random/php_random.h"
#include "ext/random/php_random_csprng.h"

#ifndef mpz_fits_si_p
# define mpz_fits_si_p mpz_fits_slong_p
#endif

#define GMP_ROUND_ZERO      0
#define GMP_ROUND_PLUSINF   1
#define GMP_ROUND_MINUSINF  2

#ifdef mpir_version
#define GMP_MPIR_VERSION_STRING ((char *) mpir_version)
#endif
#define GMP_VERSION_STRING ((char *) gmp_version)

#define GMP_MSW_FIRST     (1 << 0)
#define GMP_LSW_FIRST     (1 << 1)
#define GMP_LITTLE_ENDIAN (1 << 2)
#define GMP_BIG_ENDIAN    (1 << 3)
#define GMP_NATIVE_ENDIAN (1 << 4)

#include "gmp_arginfo.h"

ZEND_DECLARE_MODULE_GLOBALS(gmp)
static ZEND_GINIT_FUNCTION(gmp);
static ZEND_GSHUTDOWN_FUNCTION(gmp);

/* {{{ gmp_module_entry */
zend_module_entry gmp_module_entry = {
	STANDARD_MODULE_HEADER,
	"gmp",
	ext_functions,
	ZEND_MODULE_STARTUP_N(gmp),
	NULL,
	NULL,
	ZEND_MODULE_DEACTIVATE_N(gmp),
	ZEND_MODULE_INFO_N(gmp),
	PHP_GMP_VERSION,
	ZEND_MODULE_GLOBALS(gmp),
	ZEND_GINIT(gmp),
	ZEND_GSHUTDOWN(gmp),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_GMP
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(gmp)
#endif

static zend_class_entry *gmp_ce;
static zend_object_handlers gmp_object_handlers;

PHP_GMP_API zend_class_entry *php_gmp_class_entry(void) {
	return gmp_ce;
}

#define GMP_MAX_BASE 62

#define GMP_51_OR_NEWER \
	((__GNU_MP_VERSION >= 6) || (__GNU_MP_VERSION >= 5 && __GNU_MP_VERSION_MINOR >= 1))

#define IS_GMP(zval) \
	(Z_TYPE_P(zval) == IS_OBJECT && Z_OBJCE_P(zval) == gmp_ce)

#define GET_GMP_OBJECT_FROM_OBJ(obj) \
	php_gmp_object_from_zend_object(obj)
#define GET_GMP_OBJECT_FROM_ZVAL(zv) \
	GET_GMP_OBJECT_FROM_OBJ(Z_OBJ_P(zv))

#define GET_GMP_FROM_ZVAL(zval) \
	GET_GMP_OBJECT_FROM_OBJ(Z_OBJ_P(zval))->num

static void gmp_strval(zval *result, mpz_t gmpnum, int base);
static zend_result convert_zstr_to_gmp(mpz_t gmp_number, const zend_string *val, zend_long base, uint32_t arg_pos);

static bool gmp_zend_parse_arg_into_mpz_ex(
	zval *arg,
	mpz_ptr *destination_mpz_ptr,
	uint32_t arg_num,
	bool is_operator
) {
	if (EXPECTED(Z_TYPE_P(arg) == IS_OBJECT)) {
		if (EXPECTED(Z_OBJCE_P(arg) == gmp_ce)) {
			*destination_mpz_ptr = GET_GMP_FROM_ZVAL(arg);
			return true;
		}
		return false;
	}

	*destination_mpz_ptr = GMPG(zpp_arg[arg_num-1]);
	if (Z_TYPE_P(arg) == IS_STRING) {
		return convert_zstr_to_gmp(*destination_mpz_ptr, Z_STR_P(arg), /* base */ 0, is_operator ? 0 : arg_num) != FAILURE;
	}

	if (Z_TYPE_P(arg) == IS_LONG) {
		mpz_set_si(*destination_mpz_ptr, Z_LVAL_P(arg));
		return true;
	}

	/* This function is also used by the do_operation object hook,
	 * but operator overloading with objects should behave as if a
	 * method was called, thus strict types should apply. */
	if (!ZEND_ARG_USES_STRICT_TYPES()) {
		zend_long lval = 0;
		if (is_operator && Z_TYPE_P(arg) == IS_NULL) {
			return false;
		}
		if (!zend_parse_arg_long_weak(arg, &lval, arg_num)) {
			return false;
		}

		mpz_set_si(*destination_mpz_ptr, lval);

		return true;
	}

	return false;
}

static bool gmp_zend_parse_arg_into_mpz(zval *arg, mpz_ptr *destination_mpz_ptr, uint32_t arg_num)
{
	return gmp_zend_parse_arg_into_mpz_ex(arg, destination_mpz_ptr, arg_num, false);
}

#define GMP_Z_PARAM_INTO_MPZ_PTR(destination_mpz_ptr) \
	Z_PARAM_PROLOGUE(0, 0); \
	if (UNEXPECTED(!gmp_zend_parse_arg_into_mpz(_arg, &destination_mpz_ptr, _i))) { \
		_error_code = ZPP_ERROR_FAILURE; \
		if (!EG(exception)) { \
			zend_argument_type_error(_i, "must be of type GMP|string|int, %s given", zend_zval_value_name(_arg)); \
		} \
		break; \
	}

#define INIT_GMP_RETVAL(gmpnumber) \
	gmp_create(return_value, &gmpnumber)

#define GMP_FN_NAME(name) gmp_##name
#define GMP_MPZ_FN_NAME(name) mpz_##name

#define GMP_UNARY_OP_FUNCTION(name) \
	ZEND_FUNCTION(gmp_##name) { \
		mpz_ptr gmpnum_a, gmpnum_result; \
		ZEND_PARSE_PARAMETERS_START(1, 1) \
			GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a) \
		ZEND_PARSE_PARAMETERS_END(); \
		INIT_GMP_RETVAL(gmpnum_result); \
		GMP_MPZ_FN_NAME(name)(gmpnum_result, gmpnum_a); \
	}

#define GMP_BINARY_OP_FUNCTION_EX(gmp_name, mpz_name) \
	ZEND_FUNCTION(gmp_name) { \
		mpz_ptr gmpnum_a, gmpnum_b, gmpnum_result; \
		ZEND_PARSE_PARAMETERS_START(2, 2) \
			GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a) \
			GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_b) \
		ZEND_PARSE_PARAMETERS_END(); \
		INIT_GMP_RETVAL(gmpnum_result); \
		mpz_name(gmpnum_result, gmpnum_a, gmpnum_b); \
	}

#define GMP_BINARY_OP_FUNCTION(name) GMP_BINARY_OP_FUNCTION_EX(GMP_FN_NAME(name), GMP_MPZ_FN_NAME(name))

static void gmp_free_object_storage(zend_object *obj) /* {{{ */
{
	gmp_object *intern = GET_GMP_OBJECT_FROM_OBJ(obj);

	mpz_clear(intern->num);
	zend_object_std_dtor(&intern->std);
}
/* }}} */

static inline zend_object *gmp_create_object_ex(zend_class_entry *ce, mpz_ptr *gmpnum_target) /* {{{ */
{
	gmp_object *intern = zend_object_alloc(sizeof(gmp_object), ce);

	zend_object_std_init(&intern->std, ce);
	object_properties_init(&intern->std, ce);

	mpz_init(intern->num);
	*gmpnum_target = intern->num;

	return &intern->std;
}
/* }}} */

static zend_object *gmp_create_object(zend_class_entry *ce) /* {{{ */
{
	mpz_ptr gmpnum_dummy;
	return gmp_create_object_ex(ce, &gmpnum_dummy);
}
/* }}} */

static inline void gmp_create(zval *target, mpz_ptr *gmpnum_target) /* {{{ */
{
	ZVAL_OBJ(target, gmp_create_object_ex(gmp_ce, gmpnum_target));
}
/* }}} */

static zend_result gmp_cast_object(zend_object *readobj, zval *writeobj, int type) /* {{{ */
{
	mpz_ptr gmpnum;
	switch (type) {
	case IS_STRING:
		gmpnum = GET_GMP_OBJECT_FROM_OBJ(readobj)->num;
		gmp_strval(writeobj, gmpnum, 10);
		return SUCCESS;
	case IS_LONG:
		gmpnum = GET_GMP_OBJECT_FROM_OBJ(readobj)->num;
		ZVAL_LONG(writeobj, mpz_get_si(gmpnum));
		return SUCCESS;
	case IS_DOUBLE:
		gmpnum = GET_GMP_OBJECT_FROM_OBJ(readobj)->num;
		ZVAL_DOUBLE(writeobj, mpz_get_d(gmpnum));
		return SUCCESS;
	case _IS_NUMBER:
		gmpnum = GET_GMP_OBJECT_FROM_OBJ(readobj)->num;
		if (mpz_fits_si_p(gmpnum)) {
			ZVAL_LONG(writeobj, mpz_get_si(gmpnum));
		} else {
			ZVAL_DOUBLE(writeobj, mpz_get_d(gmpnum));
		}
		return SUCCESS;
	case _IS_BOOL:
		gmpnum = GET_GMP_OBJECT_FROM_OBJ(readobj)->num;
		ZVAL_BOOL(writeobj, mpz_sgn(gmpnum) != 0);
		return SUCCESS;
	default:
		return FAILURE;
	}
}
/* }}} */

static HashTable *gmp_get_debug_info(zend_object *obj, int *is_temp) /* {{{ */
{
	HashTable *ht, *props = zend_std_get_properties(obj);
	mpz_ptr gmpnum = GET_GMP_OBJECT_FROM_OBJ(obj)->num;
	zval zv;

	*is_temp = 1;
	ht = zend_array_dup(props);

	gmp_strval(&zv, gmpnum, 10);
	zend_hash_str_update(ht, "num", sizeof("num")-1, &zv);

	return ht;
}
/* }}} */

static zend_object *gmp_clone_obj(zend_object *obj) /* {{{ */
{
	gmp_object *old_object = GET_GMP_OBJECT_FROM_OBJ(obj);
	gmp_object *new_object = GET_GMP_OBJECT_FROM_OBJ(gmp_create_object(obj->ce));

	zend_objects_clone_members( &new_object->std, &old_object->std);

	mpz_set(new_object->num, old_object->num);

	return &new_object->std;
}
/* }}} */

typedef void (*gmp_binary_op_t)(mpz_ptr, mpz_srcptr, mpz_srcptr);

static zend_result binop_operator_helper(gmp_binary_op_t gmp_op, zval *return_value, zval *op1, zval *op2) {
	mpz_ptr gmp_op1, gmp_op2, gmp_result;
	if (!gmp_zend_parse_arg_into_mpz_ex(op1, &gmp_op1, 1, true)) {
		if (!EG(exception)) {
			zend_type_error("Number must be of type GMP|string|int, %s given", zend_zval_value_name(op1));
		}
		return FAILURE;
	}
	if (!gmp_zend_parse_arg_into_mpz_ex(op2, &gmp_op2, 2, true)) {
		if (!EG(exception)) {
			zend_type_error("Number must be of type GMP|string|int, %s given", zend_zval_value_name(op2));
		}
		return FAILURE;
	}
	/* Check special requirements for op2 */
	if (gmp_op == mpz_tdiv_q && mpz_cmp_ui(gmp_op2, 0) == 0) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
		return FAILURE;
	}
	if (gmp_op == mpz_mod && mpz_cmp_ui(gmp_op2, 0) == 0) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
		return FAILURE;
	}

	gmp_create(return_value, &gmp_result);
	gmp_op(gmp_result, gmp_op1, gmp_op2);
	return SUCCESS;
}

typedef void (*gmp_binary_ui_op_t)(mpz_ptr, mpz_srcptr, gmp_ulong);

static zend_result shift_operator_helper(gmp_binary_ui_op_t op, zval *return_value, zval *op1, zval *op2, uint8_t opcode) {
	zend_long shift = 0;

	if (UNEXPECTED(Z_TYPE_P(op2) != IS_LONG)) {
		if (UNEXPECTED(!IS_GMP(op2))) {
			// For PHP 8.3 and up use zend_try_get_long()
			switch (Z_TYPE_P(op2)) {
				case IS_DOUBLE:
					shift = zval_get_long(op2);
					if (UNEXPECTED(EG(exception))) {
						return FAILURE;
					}
					break;
				case IS_STRING:
					if (is_numeric_str_function(Z_STR_P(op2), &shift, NULL) != IS_LONG) {
						goto valueof_op_failure;
					}
					break;
				default:
					goto typeof_op_failure;
			}
		} else {
			// TODO We shouldn't cast the GMP object to int here
			shift = zval_get_long(op2);
		}
	} else {
		shift = Z_LVAL_P(op2);
	}

	if (shift < 0) {
		zend_throw_error(
			zend_ce_value_error, "%s must be greater than or equal to 0",
			opcode == ZEND_POW ? "Exponent" : "Shift"
		);
		ZVAL_UNDEF(return_value);
		return FAILURE;
	} else {
		mpz_ptr gmpnum_op, gmpnum_result;

		if (!gmp_zend_parse_arg_into_mpz_ex(op1, &gmpnum_op, 1, true)) {
			goto typeof_op_failure;
		}

		INIT_GMP_RETVAL(gmpnum_result);
		op(gmpnum_result, gmpnum_op, (gmp_ulong) shift);
		return SUCCESS;
	}

typeof_op_failure: ;
	/* Returning FAILURE without throwing an exception would emit the
	 * Unsupported operand types: GMP OP TypeOfOp2
	 * However, this leads to the engine trying to interpret the GMP object as an integer
	 * and doing the operation that way, which is not something we want. */
	const char *op_sigil;
	switch (opcode) {
		case ZEND_POW:
			op_sigil = "**";
			break;
		case ZEND_SL:
			op_sigil = "<<";
			break;
		case ZEND_SR:
			op_sigil = ">>";
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
	zend_type_error("Unsupported operand types: %s %s %s", zend_zval_type_name(op1), op_sigil, zend_zval_type_name(op2));
	return FAILURE;
valueof_op_failure:
	zend_value_error("Number is not an integer string");
	return FAILURE;
}

static zend_result gmp_do_operation_ex(uint8_t opcode, zval *result, zval *op1, zval *op2) /* {{{ */
{
	mpz_ptr gmp_result;
	switch (opcode) {
	case ZEND_ADD:
		return binop_operator_helper(mpz_add, result, op1, op2);
	case ZEND_SUB:
		return binop_operator_helper(mpz_sub, result, op1, op2);
	case ZEND_MUL:
		return binop_operator_helper(mpz_mul, result, op1, op2);
	case ZEND_POW:
		return shift_operator_helper(mpz_pow_ui, result, op1, op2, opcode);
	case ZEND_DIV:
		return binop_operator_helper(mpz_tdiv_q, result, op1, op2);
	case ZEND_MOD:
		return binop_operator_helper(mpz_mod, result, op1, op2);
	case ZEND_SL:
		return shift_operator_helper(mpz_mul_2exp, result, op1, op2, opcode);
	case ZEND_SR:
		return shift_operator_helper(mpz_fdiv_q_2exp, result, op1, op2, opcode);
	case ZEND_BW_OR:
		return binop_operator_helper(mpz_ior, result, op1, op2);
	case ZEND_BW_AND:
		return binop_operator_helper(mpz_and, result, op1, op2);
	case ZEND_BW_XOR:
		return binop_operator_helper(mpz_xor, result, op1, op2);
	case ZEND_BW_NOT: {
		ZEND_ASSERT(Z_TYPE_P(op1) == IS_OBJECT && Z_OBJCE_P(op1) == gmp_ce);
		gmp_create(result, &gmp_result);
		mpz_com(gmp_result, GET_GMP_FROM_ZVAL(op1));
		return SUCCESS;
	}

	default:
		return FAILURE;
	}
}
/* }}} */

static zend_result gmp_do_operation(uint8_t opcode, zval *result, zval *op1, zval *op2) /* {{{ */
{
	zval op1_copy;

	if (result == op1) {
		ZVAL_COPY_VALUE(&op1_copy, op1);
		op1 = &op1_copy;
	}

	zend_result retval = gmp_do_operation_ex(opcode, result, op1, op2);

	if (retval == SUCCESS && op1 == &op1_copy) {
		zval_ptr_dtor(op1);
	}

	return retval;
}
/* }}} */

static int gmp_compare(zval *op1, zval *op2) /* {{{ */
{
	mpz_ptr gmp_op1, gmp_op2;
	bool status = false;

	status = gmp_zend_parse_arg_into_mpz_ex(op1, &gmp_op1, 1, true);
	if (!status) {
		if (!EG(exception)) {
			zend_type_error("Number must be of type GMP|string|int, %s given", zend_zval_value_name(op1));
		}
		return ZEND_UNCOMPARABLE;
	}
	status = gmp_zend_parse_arg_into_mpz_ex(op2, &gmp_op2, 2, true);
	if (!status) {
		if (!EG(exception)) {
			zend_type_error("Number must be of type GMP|string|int, %s given", zend_zval_value_name(op2));
		}
		return ZEND_UNCOMPARABLE;
	}

	int ret = mpz_cmp(gmp_op1, gmp_op2); /* avoid multiple evaluations */
	return ZEND_THREEWAY_COMPARE(ret, 0);
}
/* }}} */

static int gmp_serialize(zval *object, unsigned char **buffer, size_t *buf_len, zend_serialize_data *data) /* {{{ */
{
	mpz_ptr gmpnum = GET_GMP_FROM_ZVAL(object);
	smart_str buf = {0};
	zval zv;
	php_serialize_data_t serialize_data;

	PHP_VAR_SERIALIZE_INIT(serialize_data);

	gmp_strval(&zv, gmpnum, 10);
	php_var_serialize(&buf, &zv, &serialize_data);
	zval_ptr_dtor_str(&zv);

	ZVAL_ARR(&zv, zend_std_get_properties(Z_OBJ_P(object)));
	php_var_serialize(&buf, &zv, &serialize_data);

	PHP_VAR_SERIALIZE_DESTROY(serialize_data);
	*buffer = (unsigned char *) estrndup(ZSTR_VAL(buf.s), ZSTR_LEN(buf.s));
	*buf_len = ZSTR_LEN(buf.s);
	zend_string_release_ex(buf.s, 0);

	return SUCCESS;
}
/* }}} */

static int gmp_unserialize(zval *object, zend_class_entry *ce, const unsigned char *buf, size_t buf_len, zend_unserialize_data *data) /* {{{ */
{
	mpz_ptr gmpnum;
	const unsigned char *p, *max;
	zval *zv;
	zend_result retval = FAILURE;
	php_unserialize_data_t unserialize_data;
	zend_object *zobj;

	PHP_VAR_UNSERIALIZE_INIT(unserialize_data);
	gmp_create(object, &gmpnum);

	zobj = Z_OBJ_P(object);

	p = buf;
	max = buf + buf_len;

	zv = var_tmp_var(&unserialize_data);
	if (!php_var_unserialize(zv, &p, max, &unserialize_data)
		|| Z_TYPE_P(zv) != IS_STRING
		|| convert_zstr_to_gmp(gmpnum, Z_STR_P(zv), 10, 0) == FAILURE
	) {
		zend_throw_exception(NULL, "Could not unserialize number", 0);
		goto exit;
	}

	zv = var_tmp_var(&unserialize_data);
	if (!php_var_unserialize(zv, &p, max, &unserialize_data)
		|| Z_TYPE_P(zv) != IS_ARRAY
	) {
		zend_throw_exception(NULL, "Could not unserialize properties", 0);
		goto exit;
	}

	if (zend_hash_num_elements(Z_ARRVAL_P(zv)) != 0) {
		zend_hash_copy(
			zend_std_get_properties(zobj), Z_ARRVAL_P(zv),
			(copy_ctor_func_t) zval_add_ref
		);
	}

	retval = SUCCESS;
exit:
	PHP_VAR_UNSERIALIZE_DESTROY(unserialize_data);
	return retval;
}
/* }}} */

/* {{{ ZEND_GINIT_FUNCTION */
static ZEND_GINIT_FUNCTION(gmp)
{
#if defined(COMPILE_DL_GMP) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	gmp_globals->rand_initialized = 0;
	mpz_init(gmp_globals->zpp_arg[0]);
	mpz_init(gmp_globals->zpp_arg[1]);
	mpz_init(gmp_globals->zpp_arg[2]);
}
/* }}} */

static ZEND_GSHUTDOWN_FUNCTION(gmp)
{
	mpz_clear(gmp_globals->zpp_arg[0]);
	mpz_clear(gmp_globals->zpp_arg[1]);
	mpz_clear(gmp_globals->zpp_arg[2]);
}

/* {{{ ZEND_MINIT_FUNCTION */
ZEND_MINIT_FUNCTION(gmp)
{
	gmp_ce = register_class_GMP();
	gmp_ce->create_object = gmp_create_object;
	gmp_ce->default_object_handlers = &gmp_object_handlers;
	gmp_ce->serialize = gmp_serialize;
	gmp_ce->unserialize = gmp_unserialize;

	memcpy(&gmp_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	gmp_object_handlers.offset = XtOffsetOf(gmp_object, std);
	gmp_object_handlers.free_obj = gmp_free_object_storage;
	gmp_object_handlers.cast_object = gmp_cast_object;
	gmp_object_handlers.get_debug_info = gmp_get_debug_info;
	gmp_object_handlers.clone_obj = gmp_clone_obj;
	gmp_object_handlers.do_operation = gmp_do_operation;
	gmp_object_handlers.compare = gmp_compare;

	register_gmp_symbols(module_number);

	return SUCCESS;
}
/* }}} */

/* {{{ ZEND_RSHUTDOWN_FUNCTION */
ZEND_MODULE_DEACTIVATE_D(gmp)
{
	if (GMPG(rand_initialized)) {
		gmp_randclear(GMPG(rand_state));
		GMPG(rand_initialized) = 0;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ ZEND_MINFO_FUNCTION */
ZEND_MODULE_INFO_D(gmp)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "gmp support", "enabled");
#ifdef mpir_version
	php_info_print_table_row(2, "MPIR version", mpir_version);
#else
	php_info_print_table_row(2, "GMP version", gmp_version);
#endif
	php_info_print_table_end();
}
/* }}} */

static zend_result convert_zstr_to_gmp(mpz_t gmp_number, const zend_string *val, zend_long base, uint32_t arg_pos)
{
	const char *num_str = ZSTR_VAL(val);
	bool skip_lead = false;

	size_t num_len = ZSTR_LEN(val);
	while (isspace(*num_str)) {
		++num_str;
		--num_len;
	}

	if (num_len >= 2 && num_str[0] == '0') {
		if ((base == 0 || base == 16) && (num_str[1] == 'x' || num_str[1] == 'X')) {
			base = 16;
			skip_lead = true;
		} else if ((base == 0 || base == 8) && (num_str[1] == 'o' || num_str[1] == 'O')) {
			base = 8;
			skip_lead = true;
		} else if ((base == 0 || base == 2) && (num_str[1] == 'b' || num_str[1] == 'B')) {
			base = 2;
			skip_lead = true;
		}
	}

	int gmp_ret = mpz_set_str(gmp_number, (skip_lead ? &num_str[2] : num_str), (int) base);
	if (-1 == gmp_ret) {
		if (arg_pos == 0) {
			zend_value_error("Number is not an integer string");
		} else {
			zend_argument_value_error(arg_pos, "is not an integer string");
		}
		return FAILURE;
	}

	return SUCCESS;
}

static void gmp_strval(zval *result, mpz_t gmpnum, int base) /* {{{ */
{
	size_t num_len;
	zend_string *str;

	num_len = mpz_sizeinbase(gmpnum, abs(base));
	if (mpz_sgn(gmpnum) < 0) {
		num_len++;
	}

	str = zend_string_alloc(num_len, 0);
	mpz_get_str(ZSTR_VAL(str), base, gmpnum);

	/*
	 * From GMP documentation for mpz_sizeinbase():
	 * The returned value will be exact or 1 too big.  If base is a power of
	 * 2, the returned value will always be exact.
	 *
	 * So let's check to see if we already have a \0 byte...
	 */

	if (ZSTR_VAL(str)[ZSTR_LEN(str) - 1] == '\0') {
		ZSTR_LEN(str)--;
	} else {
		ZSTR_VAL(str)[ZSTR_LEN(str)] = '\0';
	}

	ZVAL_NEW_STR(result, str);
}
/* }}} */

static bool gmp_verify_base(zend_long base, uint32_t arg_num)
{
	if (base && (base < 2 || base > GMP_MAX_BASE)) {
		zend_argument_value_error(arg_num, "must be 0 or between 2 and %d", GMP_MAX_BASE);
		return false;
	}

	return true;
}

static zend_result gmp_initialize_number(mpz_ptr gmp_number, const zend_string *arg_str, zend_long arg_l, zend_long base)
{
	if (arg_str) {
		return convert_zstr_to_gmp(gmp_number, arg_str, base, 1);
	}

	mpz_set_si(gmp_number, arg_l);
	return SUCCESS;
}

/* {{{ Initializes GMP number */
ZEND_FUNCTION(gmp_init)
{
	mpz_ptr gmp_number;
	zend_string *arg_str = NULL;
	zend_long arg_l = 0;
	zend_long base = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR_OR_LONG(arg_str, arg_l)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(base)
	ZEND_PARSE_PARAMETERS_END();

	if (!gmp_verify_base(base, 2)) {
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmp_number);

	if (gmp_initialize_number(gmp_number, arg_str, arg_l, base) == FAILURE) {
		RETURN_THROWS();
	}
}
/* }}} */

static bool gmp_import_export_validate(zend_long size, zend_long options, int *order, int *endian)
{
	if (size < 1) {
		/* size argument is in second position */
		zend_argument_value_error(2, "must be greater than or equal to 1");
		return false;
	}

	switch (options & (GMP_LSW_FIRST | GMP_MSW_FIRST)) {
		case GMP_LSW_FIRST:
			*order = -1;
			break;
		case GMP_MSW_FIRST:
		case 0: /* default */
			*order = 1;
			break;
		default:
			/* options argument is in third position */
			zend_argument_value_error(3, "cannot use multiple word order options");
			return false;
	}

	switch (options & (GMP_LITTLE_ENDIAN | GMP_BIG_ENDIAN | GMP_NATIVE_ENDIAN)) {
		case GMP_LITTLE_ENDIAN:
			*endian = -1;
			break;
		case GMP_BIG_ENDIAN:
			*endian = 1;
			break;
		case GMP_NATIVE_ENDIAN:
		case 0: /* default */
			*endian = 0;
			break;
		default:
			/* options argument is in third position */
			zend_argument_value_error(3, "cannot use multiple endian options");
			return false;
	}

	return true;
}

/* {{{ Imports a GMP number from a binary string */
ZEND_FUNCTION(gmp_import)
{
	char *data;
	size_t data_len;
	zend_long size = 1;
	zend_long options = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN;
	int order, endian;
	mpz_ptr gmpnumber;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|ll", &data, &data_len, &size, &options) == FAILURE) {
		RETURN_THROWS();
	}

	if (!gmp_import_export_validate(size, options, &order, &endian)) {
		RETURN_THROWS();
	}

	if ((data_len % size) != 0) {
		zend_argument_value_error(1, "must be a multiple of argument #2 ($word_size)");
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnumber);

	mpz_import(gmpnumber, data_len / size, order, size, endian, 0, data);
}
/* }}} */

/* {{{ Exports a GMP number to a binary string */
ZEND_FUNCTION(gmp_export)
{
	zend_long size = 1;
	zend_long options = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN;
	int order, endian;
	mpz_ptr gmpnumber;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnumber)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(size)
		Z_PARAM_LONG(options)
	ZEND_PARSE_PARAMETERS_END();

	if (!gmp_import_export_validate(size, options, &order, &endian)) {
		RETURN_THROWS();
	}

	if (mpz_sgn(gmpnumber) == 0) {
		RETVAL_EMPTY_STRING();
	} else {
		ZEND_ASSERT(size > 0);
		size_t size_in_base_2 = mpz_sizeinbase(gmpnumber, 2);
		if (size > ZEND_LONG_MAX / 4 || size_in_base_2 > SIZE_MAX - (size_t) size * 8 + 1) {
			zend_argument_value_error(2, "is too large for argument #1 ($num)");
			RETURN_THROWS();
		}
		size_t bits_per_word = (size_t) size * 8;
		size_t count = (size_in_base_2 + bits_per_word - 1) / bits_per_word;

		zend_string *out_string = zend_string_safe_alloc(count, size, 0, 0);
		mpz_export(ZSTR_VAL(out_string), NULL, order, size, endian, 0, gmpnumber);
		ZSTR_VAL(out_string)[ZSTR_LEN(out_string)] = '\0';

		RETVAL_NEW_STR(out_string);
	}
}
/* }}} */

/* {{{ Gets signed long value of GMP number */
ZEND_FUNCTION(gmp_intval)
{
	mpz_ptr gmpnum;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum)
	ZEND_PARSE_PARAMETERS_END();

	RETVAL_LONG(mpz_get_si(gmpnum));
}
/* }}} */

/* {{{ Gets string representation of GMP number  */
ZEND_FUNCTION(gmp_strval)
{
	zend_long base = 10;
	mpz_ptr gmpnum;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(base)
	ZEND_PARSE_PARAMETERS_END();

	/* Although the maximum base in general in GMP is 62, mpz_get_str()
	 * is explicitly limited to -36 when dealing with negative bases. */
	if ((base < 2 && base > -2) || base > GMP_MAX_BASE || base < -36) {
		zend_argument_value_error(2, "must be between 2 and %d, or -2 and -36", GMP_MAX_BASE);
		RETURN_THROWS();
	}

	gmp_strval(return_value, gmpnum, (int)base);
}
/* }}} */

/* {{{ Divide a by b, returns quotient and reminder */
ZEND_FUNCTION(gmp_div_qr)
{
	mpz_ptr gmpnum_a, gmpnum_b;
	zend_long round = GMP_ROUND_ZERO;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_b)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(round)
	ZEND_PARSE_PARAMETERS_END();

	if (mpz_cmp_ui(gmpnum_b, 0) == 0) {
		zend_argument_error(zend_ce_division_by_zero_error, 2, "Division by zero");
		RETURN_THROWS();
	}

	if (round != GMP_ROUND_ZERO && round != GMP_ROUND_PLUSINF && round != GMP_ROUND_MINUSINF) {
		zend_argument_value_error(3, "must be one of GMP_ROUND_ZERO, GMP_ROUND_PLUSINF, or GMP_ROUND_MINUSINF");
		RETURN_THROWS();
	}

	zval result1, result2;
	mpz_ptr gmpnum_result1, gmpnum_result2;
	gmp_create(&result1, &gmpnum_result1);
	gmp_create(&result2, &gmpnum_result2);

	RETVAL_ARR(zend_new_pair(&result1, &result2));

	switch (round) {
		case GMP_ROUND_ZERO:
			mpz_tdiv_qr(gmpnum_result1, gmpnum_result2, gmpnum_a, gmpnum_b);
			break;
		case GMP_ROUND_PLUSINF:
			mpz_cdiv_qr(gmpnum_result1, gmpnum_result2, gmpnum_a, gmpnum_b);
			break;
		case GMP_ROUND_MINUSINF:
			mpz_fdiv_qr(gmpnum_result1, gmpnum_result2, gmpnum_a, gmpnum_b);
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}

/* Divide a by b, returns reminder only */
ZEND_FUNCTION(gmp_div_r)
{
	mpz_ptr gmpnum_a, gmpnum_b, gmpnum_result;
	zend_long round = GMP_ROUND_ZERO;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_b)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(round)
	ZEND_PARSE_PARAMETERS_END();

	if (mpz_cmp_ui(gmpnum_b, 0) == 0) {
		zend_argument_error(zend_ce_division_by_zero_error, 2, "Division by zero");
		RETURN_THROWS();
	}

	switch (round) {
		case GMP_ROUND_ZERO: {
			INIT_GMP_RETVAL(gmpnum_result);
			mpz_tdiv_r(gmpnum_result, gmpnum_a, gmpnum_b);
			return;
		}
		case GMP_ROUND_PLUSINF: {
			INIT_GMP_RETVAL(gmpnum_result);
			mpz_cdiv_r(gmpnum_result, gmpnum_a, gmpnum_b);
			return;
		}
		case GMP_ROUND_MINUSINF: {
			INIT_GMP_RETVAL(gmpnum_result);
			mpz_fdiv_r(gmpnum_result, gmpnum_a, gmpnum_b);
			return;
		}
		default:
			zend_argument_value_error(3, "must be one of GMP_ROUND_ZERO, GMP_ROUND_PLUSINF, or GMP_ROUND_MINUSINF");
			RETURN_THROWS();
	}
}

/* Divide a by b, returns quotient only */
ZEND_FUNCTION(gmp_div_q)
{
	mpz_ptr gmpnum_a, gmpnum_b, gmpnum_result;
	zend_long round = GMP_ROUND_ZERO;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_b)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(round)
	ZEND_PARSE_PARAMETERS_END();

	if (mpz_cmp_ui(gmpnum_b, 0) == 0) {
		zend_argument_error(zend_ce_division_by_zero_error, 2, "Division by zero");
		RETURN_THROWS();
	}

	switch (round) {
		case GMP_ROUND_ZERO: {
			INIT_GMP_RETVAL(gmpnum_result);
			mpz_tdiv_q(gmpnum_result, gmpnum_a, gmpnum_b);
			return;
		}
		case GMP_ROUND_PLUSINF: {
			INIT_GMP_RETVAL(gmpnum_result);
			mpz_cdiv_q(gmpnum_result, gmpnum_a, gmpnum_b);
			return;
		}
		case GMP_ROUND_MINUSINF: {
			INIT_GMP_RETVAL(gmpnum_result);
			mpz_fdiv_q(gmpnum_result, gmpnum_a, gmpnum_b);
			return;
		}
		default:
			zend_argument_value_error(3, "must be one of GMP_ROUND_ZERO, GMP_ROUND_PLUSINF, or GMP_ROUND_MINUSINF");
			RETURN_THROWS();
	}
}

/* {{{ Computes a modulo b */
ZEND_FUNCTION(gmp_mod)
{
	mpz_ptr gmpnum_a, gmpnum_b, gmpnum_result;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_b)
	ZEND_PARSE_PARAMETERS_END();

	if (mpz_cmp_ui(gmpnum_b, 0) == 0) {
		zend_argument_error(zend_ce_division_by_zero_error, 2, "Modulo by zero");
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);
	mpz_mod(gmpnum_result, gmpnum_a, gmpnum_b);
}
/* }}} */

/* {{{ Divide a by b using exact division algorithm */
ZEND_FUNCTION(gmp_divexact)
{
	mpz_ptr gmpnum_a, gmpnum_b, gmpnum_result;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_b)
	ZEND_PARSE_PARAMETERS_END();

	if (mpz_cmp_ui(gmpnum_b, 0) == 0) {
		zend_argument_error(zend_ce_division_by_zero_error, 2, "Division by zero");
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);
	mpz_divexact(gmpnum_result, gmpnum_a, gmpnum_b);
}
/* }}} */

/* {{{ Negates a number */
GMP_UNARY_OP_FUNCTION(neg);
/* {{{ Calculates absolute value */
GMP_UNARY_OP_FUNCTION(abs);
/* {{{ Calculates one's complement of a */
GMP_UNARY_OP_FUNCTION(com);
/* {{{ Finds next prime of a */
GMP_UNARY_OP_FUNCTION(nextprime);

/* Add a and b */
GMP_BINARY_OP_FUNCTION(add);
/* Subtract b from a */
GMP_BINARY_OP_FUNCTION(sub);
/* Multiply a and b */
GMP_BINARY_OP_FUNCTION(mul);
/* Computes greatest common denominator (gcd) of a and b */
GMP_BINARY_OP_FUNCTION(gcd);
/* Computes least common multiple (lcm) of a and b */
GMP_BINARY_OP_FUNCTION(lcm);
/* {Calculates logical AND of a and b */
GMP_BINARY_OP_FUNCTION(and);
/* Calculates logical exclusive OR of a and b */
GMP_BINARY_OP_FUNCTION(xor);
/* Calculates logical OR of a and b */
GMP_BINARY_OP_FUNCTION_EX(gmp_or, mpz_ior);

/* {{{ Calculates factorial function */
ZEND_FUNCTION(gmp_fact)
{
	mpz_ptr gmpnum;
	mpz_ptr gmpnum_result;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum)
	ZEND_PARSE_PARAMETERS_END();

	if (mpz_sgn(gmpnum) < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	// TODO: Check that we don't an int that is larger than an unsigned long?
	// Could use mpz_fits_slong_p() if we revert to using mpz_get_si()

	INIT_GMP_RETVAL(gmpnum_result);
	mpz_fac_ui(gmpnum_result, mpz_get_ui(gmpnum));
}
/* }}} */

/* {{{ Calculates binomial coefficient */
ZEND_FUNCTION(gmp_binomial)
{
	mpz_ptr gmpnum_n;
	zend_long k;
	mpz_ptr gmpnum_result;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_n)
		Z_PARAM_LONG(k)
	ZEND_PARSE_PARAMETERS_END();

	if (k < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);
	mpz_bin_ui(gmpnum_result, gmpnum_n, (gmp_ulong) k);
}
/* }}} */

/* {{{ Raise base to power exp */
ZEND_FUNCTION(gmp_pow)
{
	mpz_ptr gmpnum_result;
	mpz_ptr gmpnum_base;
	zend_long exp;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_base)
		Z_PARAM_LONG(exp)
	ZEND_PARSE_PARAMETERS_END();

	if (exp < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);
	mpz_pow_ui(gmpnum_result, gmpnum_base, exp);
}
/* }}} */

/* {{{ Raise base to power exp and take result modulo mod */
ZEND_FUNCTION(gmp_powm)
{
	mpz_ptr gmpnum_base, gmpnum_exp, gmpnum_mod, gmpnum_result;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_base)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_exp)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_mod)
	ZEND_PARSE_PARAMETERS_END();

	if (mpz_sgn(gmpnum_exp) < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (!mpz_cmp_ui(gmpnum_mod, 0)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);
	mpz_powm(gmpnum_result, gmpnum_base, gmpnum_exp, gmpnum_mod);
}
/* }}} */

/* {{{ Takes integer part of square root of a */
ZEND_FUNCTION(gmp_sqrt)
{
	mpz_ptr gmpnum_a, gmpnum_result;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
	ZEND_PARSE_PARAMETERS_END();

	if (mpz_sgn(gmpnum_a) < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);
	mpz_sqrt(gmpnum_result, gmpnum_a);
}
/* }}} */

/* {{{ Square root with remainder */
ZEND_FUNCTION(gmp_sqrtrem)
{
	mpz_ptr gmpnum_a, gmpnum_result1, gmpnum_result2;
	zval result1, result2;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
	ZEND_PARSE_PARAMETERS_END();

	if (mpz_sgn(gmpnum_a) < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	gmp_create(&result1, &gmpnum_result1);
	gmp_create(&result2, &gmpnum_result2);

	RETVAL_ARR(zend_new_pair(&result1, &result2));

	mpz_sqrtrem(gmpnum_result1, gmpnum_result2, gmpnum_a);
}
/* }}} */

/* {{{ Takes integer part of nth root */
ZEND_FUNCTION(gmp_root)
{
	zend_long nth;
	mpz_ptr gmpnum_a, gmpnum_result;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		Z_PARAM_LONG(nth)
	ZEND_PARSE_PARAMETERS_END();

	if (nth <= 0) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	if (nth % 2 == 0 && mpz_sgn(gmpnum_a) < 0) {
		zend_argument_value_error(2, "must be odd if argument #1 ($a) is negative");
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);
	mpz_root(gmpnum_result, gmpnum_a, (gmp_ulong) nth);
}
/* }}} */

/* {{{ Calculates integer part of nth root and remainder */
ZEND_FUNCTION(gmp_rootrem)
{
	zend_long nth;
	mpz_ptr gmpnum_a, gmpnum_result1, gmpnum_result2;
	zval result1, result2;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		Z_PARAM_LONG(nth)
	ZEND_PARSE_PARAMETERS_END();

	if (nth <= 0) {
		zend_argument_value_error(2, "must be greater than or equal to 1");
		RETURN_THROWS();
	}

	if (nth % 2 == 0 && mpz_sgn(gmpnum_a) < 0) {
		zend_argument_value_error(2, "must be odd if argument #1 ($a) is negative");
		RETURN_THROWS();
	}

	gmp_create(&result1, &gmpnum_result1);
	gmp_create(&result2, &gmpnum_result2);

	RETVAL_ARR(zend_new_pair(&result1, &result2));

#if GMP_51_OR_NEWER
	/* mpz_rootrem() is supported since GMP 4.2, but buggy wrt odd roots
	 * of negative numbers */
	mpz_rootrem(gmpnum_result1, gmpnum_result2, gmpnum_a, (gmp_ulong) nth);
#else
	mpz_root(gmpnum_result1, gmpnum_a, (gmp_ulong) nth);
	mpz_pow_ui(gmpnum_result2, gmpnum_result1, (gmp_ulong) nth);
	mpz_sub(gmpnum_result2, gmpnum_a, gmpnum_result2);
#endif
}
/* }}} */

/* {{{ Checks if a is an exact square */
ZEND_FUNCTION(gmp_perfect_square)
{
	mpz_ptr gmpnum_a;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(mpz_perfect_square_p(gmpnum_a) != 0);
}
/* }}} */

/* {{{ Checks if a is a perfect power */
ZEND_FUNCTION(gmp_perfect_power)
{
	mpz_ptr gmpnum_a;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(mpz_perfect_power_p(gmpnum_a) != 0);
}
/* }}} */

/* {{{ Checks if a is "probably prime" */
ZEND_FUNCTION(gmp_prob_prime)
{
	mpz_ptr gmpnum_a;
	zend_long reps = 10;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(reps)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(mpz_probab_prime_p(gmpnum_a, (int)reps));
}
/* }}} */

/* {{{ Computes G, S, and T, such that AS + BT = G = `gcd' (A, B) */
ZEND_FUNCTION(gmp_gcdext)
{
	mpz_ptr gmpnum_a, gmpnum_b, gmpnum_t, gmpnum_s, gmpnum_g;
	zval result_g, result_s, result_t;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_b)
	ZEND_PARSE_PARAMETERS_END();

	gmp_create(&result_g, &gmpnum_g);
	gmp_create(&result_s, &gmpnum_s);
	gmp_create(&result_t, &gmpnum_t);

	array_init(return_value);
	add_assoc_zval(return_value, "g", &result_g);
	add_assoc_zval(return_value, "s", &result_s);
	add_assoc_zval(return_value, "t", &result_t);

	mpz_gcdext(gmpnum_g, gmpnum_s, gmpnum_t, gmpnum_a, gmpnum_b);
}
/* }}} */

/* {{{ Computes the inverse of a modulo b */
ZEND_FUNCTION(gmp_invert)
{
	mpz_ptr gmpnum_a, gmpnum_b, gmpnum_result;
	int res;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_b)
	ZEND_PARSE_PARAMETERS_END();

	if (!mpz_cmp_ui(gmpnum_b, 0)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);
	res = mpz_invert(gmpnum_result, gmpnum_a, gmpnum_b);
	if (!res) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Computes Jacobi symbol */
ZEND_FUNCTION(gmp_jacobi)
{
	mpz_ptr gmpnum_a, gmpnum_b;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_b)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(mpz_jacobi(gmpnum_a, gmpnum_b));
}
/* }}} */

/* {{{ Computes Legendre symbol */
ZEND_FUNCTION(gmp_legendre)
{
	mpz_ptr gmpnum_a, gmpnum_b;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_b)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(mpz_legendre(gmpnum_a, gmpnum_b));
}
/* }}} */

/* {{{ Computes the Kronecker symbol */
ZEND_FUNCTION(gmp_kronecker)
{
	mpz_ptr gmpnum_a, gmpnum_b;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_b)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(mpz_kronecker(gmpnum_a, gmpnum_b));
}
/* }}} */

/* {{{ Compares two numbers */
ZEND_FUNCTION(gmp_cmp)
{
	mpz_ptr gmpnum_a, gmpnum_b;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_b)
	ZEND_PARSE_PARAMETERS_END();

	int ret = mpz_cmp(gmpnum_a, gmpnum_b); /* avoid multiple evaluations */
	RETURN_LONG(ZEND_THREEWAY_COMPARE(ret, 0));
}
/* }}} */

/* {{{ Gets the sign of the number */
ZEND_FUNCTION(gmp_sign)
{
	mpz_ptr gmpnum_a;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(mpz_sgn(gmpnum_a));
}
/* }}} */

static void gmp_init_random(void)
{
	if (!GMPG(rand_initialized)) {
		/* Initialize */
		gmp_randinit_mt(GMPG(rand_state));
		/* Seed */
		unsigned long int seed = 0;
		if (php_random_bytes_silent(&seed, sizeof(seed)) == FAILURE) {
			seed = (unsigned long int)php_random_generate_fallback_seed();
		}
		gmp_randseed_ui(GMPG(rand_state), seed);

		GMPG(rand_initialized) = 1;
	}
}

/* {{{ Seed the RNG */
ZEND_FUNCTION(gmp_random_seed)
{
	mpz_ptr gmpnum_seed;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_seed)
	ZEND_PARSE_PARAMETERS_END();

	gmp_init_random();
	gmp_randseed(GMPG(rand_state), gmpnum_seed);
}
/* }}} */

/* {{{ Gets a random number in the range 0 to (2 ** n) - 1 */
ZEND_FUNCTION(gmp_random_bits)
{
	zend_long bits;
	mpz_ptr gmpnum_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &bits) == FAILURE) {
		RETURN_THROWS();
	}

#if SIZEOF_SIZE_T == 4
	const zend_long maxbits = ULONG_MAX / GMP_NUMB_BITS;
#else
	const zend_long maxbits = INT_MAX;
#endif

	if (bits <= 0 || bits > maxbits) {
		zend_argument_value_error(1, "must be between 1 and " ZEND_LONG_FMT, maxbits);
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);
	gmp_init_random();

	mpz_urandomb(gmpnum_result, GMPG(rand_state), (mp_bitcnt_t)bits);
}
/* }}} */

/* {{{ Gets a random number in the range min to max */
ZEND_FUNCTION(gmp_random_range)
{
	mpz_ptr gmpnum_min, gmpnum_max, gmpnum_result;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_min)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_max)
	ZEND_PARSE_PARAMETERS_END();

	gmp_init_random();
	if (mpz_cmp(gmpnum_max, gmpnum_min) <= 0) {
		zend_argument_value_error(1, "must be less than argument #2 ($maximum)");
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);

	/* Use available 3rd ZPP slot for range num to prevent allocation and freeing */
	mpz_ptr gmpnum_range = GMPG(zpp_arg[2]);
	mpz_sub(gmpnum_range, gmpnum_max, gmpnum_min);
	mpz_add_ui(gmpnum_range, gmpnum_range, 1);
	mpz_urandomm(gmpnum_result, GMPG(rand_state), gmpnum_range);
	mpz_add(gmpnum_result, gmpnum_result, gmpnum_min);
}
/* }}} */

static bool gmp_is_bit_index_valid(zend_long index) {
	return index >= 0 && (index / GMP_NUMB_BITS < INT_MAX);
}

/* {{{ Sets or clear bit in a */
ZEND_FUNCTION(gmp_setbit)
{
	zval *a_arg;
	zend_long index;
	bool set = 1;
	mpz_ptr gmpnum_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol|b", &a_arg, gmp_ce, &index, &set) == FAILURE) {
		RETURN_THROWS();
	}

	if (!gmp_is_bit_index_valid(index)) {
		zend_argument_value_error(2, "must be between 0 and %d * %d", INT_MAX, GMP_NUMB_BITS);
		RETURN_THROWS();
	}

	gmpnum_a = GET_GMP_FROM_ZVAL(a_arg);

	if (set) {
		mpz_setbit(gmpnum_a, index);
	} else {
		mpz_clrbit(gmpnum_a, index);
	}
}
/* }}} */

/* {{{ Clears bit in a */
ZEND_FUNCTION(gmp_clrbit)
{
	zval *a_arg;
	zend_long index;
	mpz_ptr gmpnum_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ol", &a_arg, gmp_ce, &index) == FAILURE){
		RETURN_THROWS();
	}

	if (!gmp_is_bit_index_valid(index)) {
		zend_argument_value_error(2, "must be between 0 and %d * %d", INT_MAX, GMP_NUMB_BITS);
		RETURN_THROWS();
	}

	gmpnum_a = GET_GMP_FROM_ZVAL(a_arg);
	mpz_clrbit(gmpnum_a, index);
}
/* }}} */

/* {{{ Tests if bit is set in a */
ZEND_FUNCTION(gmp_testbit)
{
	zend_long index;
	mpz_ptr gmpnum_a;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		Z_PARAM_LONG(index)
	ZEND_PARSE_PARAMETERS_END();

	if (!gmp_is_bit_index_valid(index)) {
		zend_argument_value_error(2, "must be between 0 and %d * %d", INT_MAX, GMP_NUMB_BITS);
		RETURN_THROWS();
	}

	RETURN_BOOL(mpz_tstbit(gmpnum_a, index));
}
/* }}} */

/* {{{ Calculates the population count of a */
ZEND_FUNCTION(gmp_popcount)
{
	mpz_ptr gmpnum_a;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(mpz_popcount(gmpnum_a));
}
/* }}} */

/* {{{ Calculates hamming distance between a and b */
ZEND_FUNCTION(gmp_hamdist)
{
	mpz_ptr gmpnum_a, gmpnum_b;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_b)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(mpz_hamdist(gmpnum_a, gmpnum_b));
}
/* }}} */

/* {{{ Finds first zero bit */
ZEND_FUNCTION(gmp_scan0)
{
	mpz_ptr gmpnum_a;
	zend_long start;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		Z_PARAM_LONG(start)
	ZEND_PARSE_PARAMETERS_END();

	if (!gmp_is_bit_index_valid(start)) {
		zend_argument_value_error(2, "must be between 0 and %d * %d", INT_MAX, GMP_NUMB_BITS);
		RETURN_THROWS();
	}

	RETURN_LONG(mpz_scan0(gmpnum_a, start));
}
/* }}} */

/* {{{ Finds first non-zero bit */
ZEND_FUNCTION(gmp_scan1)
{
	mpz_ptr gmpnum_a;
	zend_long start;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		GMP_Z_PARAM_INTO_MPZ_PTR(gmpnum_a)
		Z_PARAM_LONG(start)
	ZEND_PARSE_PARAMETERS_END();

	if (!gmp_is_bit_index_valid(start)) {
		zend_argument_value_error(2, "must be between 0 and %d * %d", INT_MAX, GMP_NUMB_BITS);
		RETURN_THROWS();
	}

	RETURN_LONG(mpz_scan1(gmpnum_a, start));
}
/* }}} */

ZEND_METHOD(GMP, __construct)
{
	zend_string *arg_str = NULL;
	zend_long arg_l = 0;
	zend_long base = 0;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_LONG(arg_str, arg_l)
		Z_PARAM_LONG(base)
	ZEND_PARSE_PARAMETERS_END();

	if (!gmp_verify_base(base, 2)) {
		RETURN_THROWS();
	}

	return_value = ZEND_THIS;
	mpz_ptr gmp_number = GET_GMP_FROM_ZVAL(ZEND_THIS);

	if (gmp_initialize_number(gmp_number, arg_str, arg_l, base) == FAILURE) {
		RETURN_THROWS();
	}
}

ZEND_METHOD(GMP, __serialize)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval zv;
	array_init(return_value);

	mpz_ptr gmpnum = GET_GMP_FROM_ZVAL(ZEND_THIS);
	gmp_strval(&zv, gmpnum, 16);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &zv);

	HashTable *props = Z_OBJ_P(ZEND_THIS)->properties;
	if (props && zend_hash_num_elements(props) != 0) {
		ZVAL_ARR(&zv, zend_proptable_to_symtable(
			zend_std_get_properties(Z_OBJ_P(ZEND_THIS)), /* always duplicate */ 1));
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &zv);
	}
}

ZEND_METHOD(GMP, __unserialize)
{
	HashTable *data;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(data)
	ZEND_PARSE_PARAMETERS_END();

	zval *num = zend_hash_index_find(data, 0);
	if (!num || Z_TYPE_P(num) != IS_STRING ||
			convert_zstr_to_gmp(GET_GMP_FROM_ZVAL(ZEND_THIS), Z_STR_P(num), 16, /* arg_pos */ 0) == FAILURE) {
		zend_throw_exception(NULL, "Could not unserialize number", 0);
		RETURN_THROWS();
	}

	zval *props = zend_hash_index_find(data, 1);
	if (props) {
		if (Z_TYPE_P(props) != IS_ARRAY) {
			zend_throw_exception(NULL, "Could not unserialize properties", 0);
			RETURN_THROWS();
		}

		object_properties_load(Z_OBJ_P(ZEND_THIS), Z_ARRVAL_P(props));
	}
}
