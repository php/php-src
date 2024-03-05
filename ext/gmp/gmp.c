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
#include "php_ini.h"
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
	NULL,
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

typedef struct _gmp_temp {
	mpz_t num;
	bool is_used;
} gmp_temp_t;

#define GMP_MAX_BASE 62

#define GMP_51_OR_NEWER \
	((__GNU_MP_VERSION >= 6) || (__GNU_MP_VERSION >= 5 && __GNU_MP_VERSION_MINOR >= 1))

#define IS_GMP(zval) \
	(Z_TYPE_P(zval) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zval), gmp_ce))

#define GET_GMP_OBJECT_FROM_OBJ(obj) \
	php_gmp_object_from_zend_object(obj)
#define GET_GMP_OBJECT_FROM_ZVAL(zv) \
	GET_GMP_OBJECT_FROM_OBJ(Z_OBJ_P(zv))

#define GET_GMP_FROM_ZVAL(zval) \
	GET_GMP_OBJECT_FROM_OBJ(Z_OBJ_P(zval))->num

/* The FETCH_GMP_ZVAL_* family of macros is used to fetch a gmp number
 * (mpz_ptr) from a zval. If the zval is not a GMP instance, then we
 * try to convert the value to a temporary gmp number using convert_to_gmp.
 * This temporary number is stored in the temp argument, which is of type
 * gmp_temp_t. This temporary value needs to be freed lateron using the
 * FREE_GMP_TEMP macro.
 *
 * If the conversion to a gmp number fails, the macros RETURN_THROWS() due to TypeError.
 * The _DEP / _DEP_DEP variants additionally free the temporary values
 * passed in the last / last two arguments.
 *
 * If one zval can sometimes be fetched as a long you have to set the
 * is_used member of the corresponding gmp_temp_t value to 0, otherwise
 * the FREE_GMP_TEMP and *_DEP macros will not work properly.
 *
 * The three FETCH_GMP_ZVAL_* macros below are mostly copy & paste code
 * as I couldn't find a way to combine them.
 */

#define FREE_GMP_TEMP(temp)  \
	if (temp.is_used) {      \
		mpz_clear(temp.num); \
	}

#define FETCH_GMP_ZVAL_DEP_DEP(gmpnumber, zval, temp, dep1, dep2, arg_pos) \
if (IS_GMP(zval)) {                                               \
	gmpnumber = GET_GMP_FROM_ZVAL(zval);                          \
	temp.is_used = 0;                                             \
} else {                                                          \
	mpz_init(temp.num);                                           \
	if (convert_to_gmp(temp.num, zval, 0, arg_pos) == FAILURE) {  \
		mpz_clear(temp.num);                                      \
		FREE_GMP_TEMP(dep1);                                      \
		FREE_GMP_TEMP(dep2);                                      \
		RETURN_THROWS();                                          \
	}                                                             \
	temp.is_used = 1;                                             \
	gmpnumber = temp.num;                                         \
}

#define FETCH_GMP_ZVAL_DEP(gmpnumber, zval, temp, dep, arg_pos)   \
if (IS_GMP(zval)) {                                               \
	gmpnumber = GET_GMP_FROM_ZVAL(zval);                          \
	temp.is_used = 0;                                             \
} else {                                                          \
	mpz_init(temp.num);                                           \
	if (convert_to_gmp(temp.num, zval, 0, arg_pos) == FAILURE) {  \
		mpz_clear(temp.num);                                      \
		FREE_GMP_TEMP(dep);                                       \
		RETURN_THROWS();                                          \
	}                                                             \
	temp.is_used = 1;                                             \
	gmpnumber = temp.num;                                         \
}

#define FETCH_GMP_ZVAL(gmpnumber, zval, temp, arg_pos)            \
if (IS_GMP(zval)) {                                               \
	gmpnumber = GET_GMP_FROM_ZVAL(zval);                          \
	temp.is_used = 0;                                             \
} else {                                                          \
	mpz_init(temp.num);                                           \
	if (convert_to_gmp(temp.num, zval, 0, arg_pos) == FAILURE) {  \
		mpz_clear(temp.num);                                      \
		RETURN_THROWS();                                          \
	}                                                             \
	temp.is_used = 1;                                             \
	gmpnumber = temp.num;                                         \
}

#define INIT_GMP_RETVAL(gmpnumber) \
	gmp_create(return_value, &gmpnumber)

static void gmp_strval(zval *result, mpz_t gmpnum, int base);
static zend_result convert_to_gmp(mpz_t gmpnumber, zval *val, zend_long base, uint32_t arg_pos);
static void gmp_cmp(zval *return_value, zval *a_arg, zval *b_arg, bool is_operator);

/*
 * The gmp_*_op functions provide an implementation for several common types
 * of GMP functions. The gmp_zval_(unary|binary)_*_op functions have to be manually
 * passed zvals to work on, whereas the gmp_(unary|binary)_*_op macros already
 * include parameter parsing.
 */
typedef void (*gmp_unary_op_t)(mpz_ptr, mpz_srcptr);
typedef mp_bitcnt_t (*gmp_unary_opl_t)(mpz_srcptr);

typedef void (*gmp_unary_ui_op_t)(mpz_ptr, gmp_ulong);

typedef void (*gmp_binary_op_t)(mpz_ptr, mpz_srcptr, mpz_srcptr);

typedef void (*gmp_binary_ui_op_t)(mpz_ptr, mpz_srcptr, gmp_ulong);
typedef void (*gmp_binary_op2_t)(mpz_ptr, mpz_ptr, mpz_srcptr, mpz_srcptr);
typedef gmp_ulong (*gmp_binary_ui_op2_t)(mpz_ptr, mpz_ptr, mpz_srcptr, gmp_ulong);

static inline void gmp_zval_binary_ui_op(zval *return_value, zval *a_arg, zval *b_arg, gmp_binary_op_t gmp_op, gmp_binary_ui_op_t gmp_ui_op, bool check_b_zero, bool is_operator);
static inline void gmp_zval_binary_ui_op2(zval *return_value, zval *a_arg, zval *b_arg, gmp_binary_op2_t gmp_op, gmp_binary_ui_op2_t gmp_ui_op, int check_b_zero);
static inline void gmp_zval_unary_op(zval *return_value, zval *a_arg, gmp_unary_op_t gmp_op);

static void gmp_mpz_tdiv_q_ui(mpz_ptr a, mpz_srcptr b, gmp_ulong c) {
	mpz_tdiv_q_ui(a, b, c);
}
static void gmp_mpz_tdiv_r_ui(mpz_ptr a, mpz_srcptr b, gmp_ulong c) {
	mpz_tdiv_r_ui(a, b, c);
}
static void gmp_mpz_fdiv_q_ui(mpz_ptr a, mpz_srcptr b, gmp_ulong c) {
	mpz_fdiv_q_ui(a, b, c);
}
static void gmp_mpz_fdiv_r_ui(mpz_ptr a, mpz_srcptr b, gmp_ulong c) {
	mpz_fdiv_r_ui(a, b, c);
}
static void gmp_mpz_cdiv_r_ui(mpz_ptr a, mpz_srcptr b, gmp_ulong c) {
	mpz_cdiv_r_ui(a, b, c);
}
static void gmp_mpz_cdiv_q_ui(mpz_ptr a, mpz_srcptr b, gmp_ulong c) {
	mpz_cdiv_q_ui(a, b, c);
}
static void gmp_mpz_mod_ui(mpz_ptr a, mpz_srcptr b, gmp_ulong c) {
	mpz_mod_ui(a, b, c);
}
static void gmp_mpz_gcd_ui(mpz_ptr a, mpz_srcptr b, gmp_ulong c) {
	mpz_gcd_ui(a, b, c);
}

/* Binary operations */
#define gmp_binary_ui_op(op, uop) _gmp_binary_ui_op(INTERNAL_FUNCTION_PARAM_PASSTHRU, op, uop, 0)
#define gmp_binary_op(op)         _gmp_binary_ui_op(INTERNAL_FUNCTION_PARAM_PASSTHRU, op, NULL, 0)
#define gmp_binary_ui_op_no_zero(op, uop) \
	_gmp_binary_ui_op(INTERNAL_FUNCTION_PARAM_PASSTHRU, op, uop, 1)

/* Unary operations */
#define gmp_unary_op(op)         _gmp_unary_op(INTERNAL_FUNCTION_PARAM_PASSTHRU, op)
#define gmp_unary_opl(op)         _gmp_unary_opl(INTERNAL_FUNCTION_PARAM_PASSTHRU, op)

static void gmp_free_object_storage(zend_object *obj) /* {{{ */
{
	gmp_object *intern = GET_GMP_OBJECT_FROM_OBJ(obj);

	mpz_clear(intern->num);
	zend_object_std_dtor(&intern->std);
}
/* }}} */

static inline zend_object *gmp_create_object_ex(zend_class_entry *ce, mpz_ptr *gmpnum_target) /* {{{ */
{
	gmp_object *intern = emalloc(sizeof(gmp_object) + zend_object_properties_size(ce));

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
		if (mpz_fits_slong_p(gmpnum)) {
			ZVAL_LONG(writeobj, mpz_get_si(gmpnum));
		} else {
			ZVAL_DOUBLE(writeobj, mpz_get_d(gmpnum));
		}
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

static void shift_operator_helper(gmp_binary_ui_op_t op, zval *return_value, zval *op1, zval *op2, uint8_t opcode) {
	zend_long shift = zval_get_long(op2);

	if (shift < 0) {
		zend_throw_error(
			zend_ce_value_error, "%s must be greater than or equal to 0",
			opcode == ZEND_POW ? "Exponent" : "Shift"
		);
		ZVAL_UNDEF(return_value);
		return;
	} else {
		mpz_ptr gmpnum_op, gmpnum_result;
		gmp_temp_t temp;

		FETCH_GMP_ZVAL(gmpnum_op, op1, temp, 1);
		INIT_GMP_RETVAL(gmpnum_result);
		op(gmpnum_result, gmpnum_op, (gmp_ulong) shift);
		FREE_GMP_TEMP(temp);
	}
}

#define DO_BINARY_UI_OP_EX(op, uop, check_b_zero) \
	gmp_zval_binary_ui_op( \
		result, op1, op2, op, uop, check_b_zero, /* is_operator */ true); \
	if (UNEXPECTED(EG(exception))) { return FAILURE; } \
	return SUCCESS;

#define DO_BINARY_UI_OP(op) DO_BINARY_UI_OP_EX(op, op ## _ui, 0)
#define DO_BINARY_OP(op) DO_BINARY_UI_OP_EX(op, NULL, 0)

#define DO_UNARY_OP(op)                 \
	gmp_zval_unary_op(result, op1, op); \
	if (UNEXPECTED(EG(exception))) {    \
		return FAILURE;                 \
	}                                   \
	return SUCCESS;

static zend_result gmp_do_operation_ex(uint8_t opcode, zval *result, zval *op1, zval *op2) /* {{{ */
{
	switch (opcode) {
	case ZEND_ADD:
		DO_BINARY_UI_OP(mpz_add);
	case ZEND_SUB:
		DO_BINARY_UI_OP(mpz_sub);
	case ZEND_MUL:
		DO_BINARY_UI_OP(mpz_mul);
	case ZEND_POW:
		shift_operator_helper(mpz_pow_ui, result, op1, op2, opcode);
		return SUCCESS;
	case ZEND_DIV:
		DO_BINARY_UI_OP_EX(mpz_tdiv_q, gmp_mpz_tdiv_q_ui, 1);
	case ZEND_MOD:
		DO_BINARY_UI_OP_EX(mpz_mod, gmp_mpz_mod_ui, 1);
	case ZEND_SL:
		shift_operator_helper(mpz_mul_2exp, result, op1, op2, opcode);
		return SUCCESS;
	case ZEND_SR:
		shift_operator_helper(mpz_fdiv_q_2exp, result, op1, op2, opcode);
		return SUCCESS;
	case ZEND_BW_OR:
		DO_BINARY_OP(mpz_ior);
	case ZEND_BW_AND:
		DO_BINARY_OP(mpz_and);
	case ZEND_BW_XOR:
		DO_BINARY_OP(mpz_xor);
	case ZEND_BW_NOT:
		DO_UNARY_OP(mpz_com);

	default:
		return FAILURE;
	}
}
/* }}} */

static zend_result gmp_do_operation(uint8_t opcode, zval *result, zval *op1, zval *op2) /* {{{ */
{
	zval op1_copy;
	int retval;

	if (result == op1) {
		ZVAL_COPY_VALUE(&op1_copy, op1);
		op1 = &op1_copy;
	}

	retval = gmp_do_operation_ex(opcode, result, op1, op2);

	if (retval == SUCCESS && op1 == &op1_copy) {
		zval_ptr_dtor(op1);
	}

	return retval;
}
/* }}} */

static int gmp_compare(zval *op1, zval *op2) /* {{{ */
{
	zval result;

	gmp_cmp(&result, op1, op2, /* is_operator */ true);

	/* An error/exception occurs if one of the operands is not a numeric string
	 * or an object which is different from GMP */
	if (EG(exception)) {
		return 1;
	}
	/* result can only be a zend_long if gmp_cmp hasn't thrown an Error */
	ZEND_ASSERT(Z_TYPE(result) == IS_LONG);
	return Z_LVAL(result);
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
	int retval = FAILURE;
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
		|| convert_to_gmp(gmpnum, zv, 10, 0) == FAILURE
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
}
/* }}} */

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

	if (ZSTR_LEN(val) >= 2 && num_str[0] == '0') {
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

/* {{{ convert_to_gmp
 * Convert zval to be gmp number */
static zend_result convert_to_gmp(mpz_t gmpnumber, zval *val, zend_long base, uint32_t arg_pos)
{
	switch (Z_TYPE_P(val)) {
	case IS_LONG:
		mpz_set_si(gmpnumber, Z_LVAL_P(val));
		return SUCCESS;
	case IS_STRING: {
		return convert_zstr_to_gmp(gmpnumber, Z_STR_P(val), base, arg_pos);
	}
	default: {
		zend_long lval;
		if (!zend_parse_arg_long_slow(val, &lval, arg_pos)) {
			if (arg_pos == 0) {
				zend_type_error(
					"Number must be of type GMP|string|int, %s given", zend_zval_value_name(val));
			} else {
				zend_argument_type_error(arg_pos,
					"must be of type GMP|string|int, %s given", zend_zval_value_name(val));
			}
			return FAILURE;
		}

		mpz_set_si(gmpnumber, lval);
		return SUCCESS;
	}
	}
}
/* }}} */

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

static void gmp_cmp(zval *return_value, zval *a_arg, zval *b_arg, bool is_operator) /* {{{ */
{
	mpz_ptr gmpnum_a, gmpnum_b;
	gmp_temp_t temp_a, temp_b;
	bool use_si = 0;
	zend_long res;

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, is_operator ? 0 : 1);

	if (Z_TYPE_P(b_arg) == IS_LONG) {
		use_si = 1;
		temp_b.is_used = 0;
	} else {
		FETCH_GMP_ZVAL_DEP(gmpnum_b, b_arg, temp_b, temp_a, is_operator ? 0 : 2);
	}

	if (use_si) {
		res = mpz_cmp_si(gmpnum_a, Z_LVAL_P(b_arg));
	} else {
		res = mpz_cmp(gmpnum_a, gmpnum_b);
	}

	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);

	RETURN_LONG(res);
}
/* }}} */

/* {{{ gmp_zval_binary_ui_op
   Execute GMP binary operation.
*/
static inline void gmp_zval_binary_ui_op(zval *return_value, zval *a_arg, zval *b_arg, gmp_binary_op_t gmp_op, gmp_binary_ui_op_t gmp_ui_op, bool check_b_zero, bool is_operator)
{
	mpz_ptr gmpnum_a, gmpnum_b, gmpnum_result;
	gmp_temp_t temp_a, temp_b;

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, is_operator ? 0 : 1);

	if (gmp_ui_op && Z_TYPE_P(b_arg) == IS_LONG && Z_LVAL_P(b_arg) >= 0) {
		gmpnum_b = NULL;
		temp_b.is_used = 0;
	} else {
		FETCH_GMP_ZVAL_DEP(gmpnum_b, b_arg, temp_b, temp_a, is_operator ? 0 : 2);
	}

	if (check_b_zero) {
		int b_is_zero = 0;
		if (!gmpnum_b) {
			b_is_zero = (Z_LVAL_P(b_arg) == 0);
		} else {
			b_is_zero = !mpz_cmp_ui(gmpnum_b, 0);
		}

		if (b_is_zero) {
			if ((gmp_binary_op_t) mpz_mod == gmp_op) {
				zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
			} else {
				zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
			}
			FREE_GMP_TEMP(temp_a);
			FREE_GMP_TEMP(temp_b);
			RETURN_THROWS();
		}
	}

	INIT_GMP_RETVAL(gmpnum_result);

	if (!gmpnum_b) {
		gmp_ui_op(gmpnum_result, gmpnum_a, (gmp_ulong) Z_LVAL_P(b_arg));
	} else {
		gmp_op(gmpnum_result, gmpnum_a, gmpnum_b);
	}

	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
}
/* }}} */

/* {{{ gmp_zval_binary_ui_op2
   Execute GMP binary operation which returns 2 values.
*/
static inline void gmp_zval_binary_ui_op2(zval *return_value, zval *a_arg, zval *b_arg, gmp_binary_op2_t gmp_op, gmp_binary_ui_op2_t gmp_ui_op, int check_b_zero)
{
	mpz_ptr gmpnum_a, gmpnum_b, gmpnum_result1, gmpnum_result2;
	gmp_temp_t temp_a, temp_b;
	zval result1, result2;

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);

	if (gmp_ui_op && Z_TYPE_P(b_arg) == IS_LONG && Z_LVAL_P(b_arg) >= 0) {
		gmpnum_b = NULL;
		temp_b.is_used = 0;
	} else {
		FETCH_GMP_ZVAL_DEP(gmpnum_b, b_arg, temp_b, temp_a, 2);
	}

	if (check_b_zero) {
		int b_is_zero = 0;
		if (!gmpnum_b) {
			b_is_zero = (Z_LVAL_P(b_arg) == 0);
		} else {
			b_is_zero = !mpz_cmp_ui(gmpnum_b, 0);
		}

		if (b_is_zero) {
			zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
			FREE_GMP_TEMP(temp_a);
			FREE_GMP_TEMP(temp_b);
			RETURN_THROWS();
		}
	}

	gmp_create(&result1, &gmpnum_result1);
	gmp_create(&result2, &gmpnum_result2);

	array_init(return_value);
	add_next_index_zval(return_value, &result1);
	add_next_index_zval(return_value, &result2);

	if (!gmpnum_b) {
		gmp_ui_op(gmpnum_result1, gmpnum_result2, gmpnum_a, (gmp_ulong) Z_LVAL_P(b_arg));
	} else {
		gmp_op(gmpnum_result1, gmpnum_result2, gmpnum_a, gmpnum_b);
	}

	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
}
/* }}} */

/* {{{ _gmp_binary_ui_op */
static inline void _gmp_binary_ui_op(INTERNAL_FUNCTION_PARAMETERS, gmp_binary_op_t gmp_op, gmp_binary_ui_op_t gmp_ui_op, int check_b_zero)
{
	zval *a_arg, *b_arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a_arg, &b_arg) == FAILURE){
		RETURN_THROWS();
	}

	gmp_zval_binary_ui_op(
		return_value, a_arg, b_arg, gmp_op, gmp_ui_op, check_b_zero, /* is_operator */ false);
}
/* }}} */

/* Unary operations */

/* {{{ gmp_zval_unary_op */
static inline void gmp_zval_unary_op(zval *return_value, zval *a_arg, gmp_unary_op_t gmp_op)
{
	mpz_ptr gmpnum_a, gmpnum_result;
	gmp_temp_t temp_a;

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);

	INIT_GMP_RETVAL(gmpnum_result);
	gmp_op(gmpnum_result, gmpnum_a);

	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ _gmp_unary_op */
static inline void _gmp_unary_op(INTERNAL_FUNCTION_PARAMETERS, gmp_unary_op_t gmp_op)
{
	zval *a_arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		RETURN_THROWS();
	}

	gmp_zval_unary_op(return_value, a_arg, gmp_op);
}
/* }}} */

/* {{{ _gmp_unary_opl */
static inline void _gmp_unary_opl(INTERNAL_FUNCTION_PARAMETERS, gmp_unary_opl_t gmp_op)
{
	zval *a_arg;
	mpz_ptr gmpnum_a;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);
	RETVAL_LONG(gmp_op(gmpnum_a));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

static bool gmp_verify_base(zend_long base, uint32_t arg_num)
{
	if (base && (base < 2 || base > GMP_MAX_BASE)) {
		zend_argument_value_error(arg_num, "must be between 2 and %d", GMP_MAX_BASE);
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

int gmp_import_export_validate(zend_long size, zend_long options, int *order, int *endian)
{
	if (size < 1) {
		/* size argument is in second position */
		zend_argument_value_error(2, "must be greater than or equal to 1");
		return FAILURE;
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
			/* options argument is in second position */
			zend_argument_value_error(3, "cannot use multiple word order options");
			return FAILURE;
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
			/* options argument is in second position */
			zend_argument_value_error(3, "cannot use multiple endian options");
			return FAILURE;
	}

	return SUCCESS;
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

	if (gmp_import_export_validate(size, options, &order, &endian) == FAILURE) {
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
	zval *gmpnumber_arg;
	zend_long size = 1;
	zend_long options = GMP_MSW_FIRST | GMP_NATIVE_ENDIAN;
	int order, endian;
	mpz_ptr gmpnumber;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|ll", &gmpnumber_arg, &size, &options) == FAILURE) {
		RETURN_THROWS();
	}

	if (gmp_import_export_validate(size, options, &order, &endian) == FAILURE) {
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnumber, gmpnumber_arg, temp_a, 1);

	if (mpz_sgn(gmpnumber) == 0) {
		RETVAL_EMPTY_STRING();
	} else {
		size_t bits_per_word = size * 8;
		size_t count = (mpz_sizeinbase(gmpnumber, 2) + bits_per_word - 1) / bits_per_word;

		zend_string *out_string = zend_string_safe_alloc(count, size, 0, 0);
		mpz_export(ZSTR_VAL(out_string), NULL, order, size, endian, 0, gmpnumber);
		ZSTR_VAL(out_string)[ZSTR_LEN(out_string)] = '\0';

		RETVAL_NEW_STR(out_string);
	}

	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ Gets signed long value of GMP number */
ZEND_FUNCTION(gmp_intval)
{
	zval *gmpnumber_arg;
	mpz_ptr gmpnum;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &gmpnumber_arg) == FAILURE){
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum, gmpnumber_arg, temp_a, 1);
	RETVAL_LONG(mpz_get_si(gmpnum));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ Gets string representation of GMP number  */
ZEND_FUNCTION(gmp_strval)
{
	zval *gmpnumber_arg;
	zend_long base = 10;
	mpz_ptr gmpnum;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|l", &gmpnumber_arg, &base) == FAILURE) {
		RETURN_THROWS();
	}

	/* Although the maximum base in general in GMP is 62, mpz_get_str()
	 * is explicitly limited to -36 when dealing with negative bases. */
	if ((base < 2 && base > -2) || base > GMP_MAX_BASE || base < -36) {
		zend_argument_value_error(2, "must be between 2 and %d, or -2 and -36", GMP_MAX_BASE);
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum, gmpnumber_arg, temp_a, 1);

	gmp_strval(return_value, gmpnum, (int)base);

	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ Add a and b */
ZEND_FUNCTION(gmp_add)
{
	gmp_binary_ui_op(mpz_add, mpz_add_ui);
}
/* }}} */

/* {{{ Subtract b from a */
ZEND_FUNCTION(gmp_sub)
{
	gmp_binary_ui_op(mpz_sub, mpz_sub_ui);
}
/* }}} */

/* {{{ Multiply a and b */
ZEND_FUNCTION(gmp_mul)
{
	gmp_binary_ui_op(mpz_mul, mpz_mul_ui);
}
/* }}} */

/* {{{ Divide a by b, returns quotient and reminder */
ZEND_FUNCTION(gmp_div_qr)
{
	zval *a_arg, *b_arg;
	zend_long round = GMP_ROUND_ZERO;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz|l", &a_arg, &b_arg, &round) == FAILURE) {
		RETURN_THROWS();
	}

	switch (round) {
	case GMP_ROUND_ZERO:
		gmp_zval_binary_ui_op2(return_value, a_arg, b_arg, mpz_tdiv_qr, mpz_tdiv_qr_ui, 1);
		break;
	case GMP_ROUND_PLUSINF:
		gmp_zval_binary_ui_op2(return_value, a_arg, b_arg, mpz_cdiv_qr, mpz_cdiv_qr_ui, 1);
		break;
	case GMP_ROUND_MINUSINF:
		gmp_zval_binary_ui_op2(return_value, a_arg, b_arg, mpz_fdiv_qr, mpz_fdiv_qr_ui, 1);
		break;
	default:
		zend_argument_value_error(3, "must be one of GMP_ROUND_ZERO, GMP_ROUND_PLUSINF, or GMP_ROUND_MINUSINF");
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Divide a by b, returns reminder only */
ZEND_FUNCTION(gmp_div_r)
{
	zval *a_arg, *b_arg;
	zend_long round = GMP_ROUND_ZERO;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz|l", &a_arg, &b_arg, &round) == FAILURE) {
		RETURN_THROWS();
	}

	switch (round) {
	case GMP_ROUND_ZERO:
		gmp_zval_binary_ui_op(
			return_value, a_arg, b_arg, mpz_tdiv_r, gmp_mpz_tdiv_r_ui, 1, /* is_operator */ false);
		break;
	case GMP_ROUND_PLUSINF:
		gmp_zval_binary_ui_op(
			return_value, a_arg, b_arg, mpz_cdiv_r, gmp_mpz_cdiv_r_ui, 1, /* is_operator */ false);
		break;
	case GMP_ROUND_MINUSINF:
		gmp_zval_binary_ui_op(
			return_value, a_arg, b_arg, mpz_fdiv_r, gmp_mpz_fdiv_r_ui, 1, /* is_operator */ false);
		break;
	default:
		zend_argument_value_error(3, "must be one of GMP_ROUND_ZERO, GMP_ROUND_PLUSINF, or GMP_ROUND_MINUSINF");
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Divide a by b, returns quotient only */
ZEND_FUNCTION(gmp_div_q)
{
	zval *a_arg, *b_arg;
	zend_long round = GMP_ROUND_ZERO;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz|l", &a_arg, &b_arg, &round) == FAILURE) {
		RETURN_THROWS();
	}

	switch (round) {
	case GMP_ROUND_ZERO:
		gmp_zval_binary_ui_op(
			return_value, a_arg, b_arg, mpz_tdiv_q, gmp_mpz_tdiv_q_ui, 1, /* is_operator */ false);
		break;
	case GMP_ROUND_PLUSINF:
		gmp_zval_binary_ui_op(
			return_value, a_arg, b_arg, mpz_cdiv_q, gmp_mpz_cdiv_q_ui, 1, /* is_operator */ false);
		break;
	case GMP_ROUND_MINUSINF:
		gmp_zval_binary_ui_op(
			return_value, a_arg, b_arg, mpz_fdiv_q, gmp_mpz_fdiv_q_ui, 1, /* is_operator */ false);
		break;
	default:
		zend_argument_value_error(3, "must be one of GMP_ROUND_ZERO, GMP_ROUND_PLUSINF, or GMP_ROUND_MINUSINF");
		RETURN_THROWS();
	}

}
/* }}} */

/* {{{ Computes a modulo b */
ZEND_FUNCTION(gmp_mod)
{
	gmp_binary_ui_op_no_zero(mpz_mod, gmp_mpz_mod_ui);
}
/* }}} */

/* {{{ Divide a by b using exact division algorithm */
ZEND_FUNCTION(gmp_divexact)
{
	gmp_binary_ui_op_no_zero(mpz_divexact, NULL);
}
/* }}} */

/* {{{ Negates a number */
ZEND_FUNCTION(gmp_neg)
{
	gmp_unary_op(mpz_neg);
}
/* }}} */

/* {{{ Calculates absolute value */
ZEND_FUNCTION(gmp_abs)
{
	gmp_unary_op(mpz_abs);
}
/* }}} */

/* {{{ Calculates factorial function */
ZEND_FUNCTION(gmp_fact)
{
	zval *a_arg;
	mpz_ptr gmpnum_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		RETURN_THROWS();
	}

	if (Z_TYPE_P(a_arg) == IS_LONG) {
		if (Z_LVAL_P(a_arg) < 0) {
			zend_argument_value_error(1, "must be greater than or equal to 0");
			RETURN_THROWS();
		}
	} else {
		mpz_ptr gmpnum;
		gmp_temp_t temp_a;

		FETCH_GMP_ZVAL(gmpnum, a_arg, temp_a, 1);
		FREE_GMP_TEMP(temp_a);

		if (mpz_sgn(gmpnum) < 0) {
			zend_argument_value_error(1, "must be greater than or equal to 0");
			RETURN_THROWS();
		}
	}

	INIT_GMP_RETVAL(gmpnum_result);
	mpz_fac_ui(gmpnum_result, zval_get_long(a_arg));
}
/* }}} */

/* {{{ Calculates binomial coefficient */
ZEND_FUNCTION(gmp_binomial)
{
	zval *n_arg;
	zend_long k;
	mpz_ptr gmpnum_result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &n_arg, &k) == FAILURE) {
		RETURN_THROWS();
	}

	if (k < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);
	if (Z_TYPE_P(n_arg) == IS_LONG && Z_LVAL_P(n_arg) >= 0) {
		mpz_bin_uiui(gmpnum_result, (gmp_ulong) Z_LVAL_P(n_arg), (gmp_ulong) k);
	} else {
		mpz_ptr gmpnum_n;
		gmp_temp_t temp_n;
		FETCH_GMP_ZVAL(gmpnum_n, n_arg, temp_n, 1);
		mpz_bin_ui(gmpnum_result, gmpnum_n, (gmp_ulong) k);
		FREE_GMP_TEMP(temp_n);
	}
}
/* }}} */

/* {{{ Raise base to power exp */
ZEND_FUNCTION(gmp_pow)
{
	zval *base_arg;
	mpz_ptr gmpnum_result;
	gmp_temp_t temp_base;
	zend_long exp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &base_arg, &exp) == FAILURE) {
		RETURN_THROWS();
	}

	if (exp < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (Z_TYPE_P(base_arg) == IS_LONG && Z_LVAL_P(base_arg) >= 0) {
		INIT_GMP_RETVAL(gmpnum_result);
		mpz_ui_pow_ui(gmpnum_result, Z_LVAL_P(base_arg), exp);
	} else {
		mpz_ptr gmpnum_base;
		FETCH_GMP_ZVAL(gmpnum_base, base_arg, temp_base, 1);
		INIT_GMP_RETVAL(gmpnum_result);
		mpz_pow_ui(gmpnum_result, gmpnum_base, exp);
		FREE_GMP_TEMP(temp_base);
	}
}
/* }}} */

/* {{{ Raise base to power exp and take result modulo mod */
ZEND_FUNCTION(gmp_powm)
{
	zval *base_arg, *exp_arg, *mod_arg;
	mpz_ptr gmpnum_base, gmpnum_exp, gmpnum_mod, gmpnum_result;
	int use_ui = 0;
	gmp_temp_t temp_base, temp_exp, temp_mod;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zzz", &base_arg, &exp_arg, &mod_arg) == FAILURE){
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_base, base_arg, temp_base, 1);

	if (Z_TYPE_P(exp_arg) == IS_LONG && Z_LVAL_P(exp_arg) >= 0) {
		use_ui = 1;
		temp_exp.is_used = 0;
	} else {
		FETCH_GMP_ZVAL_DEP(gmpnum_exp, exp_arg, temp_exp, temp_base, 2);
		if (mpz_sgn(gmpnum_exp) < 0) {
			zend_argument_value_error(2, "must be greater than or equal to 0");
			FREE_GMP_TEMP(temp_base);
			FREE_GMP_TEMP(temp_exp);
			RETURN_THROWS();
		}
	}
	FETCH_GMP_ZVAL_DEP_DEP(gmpnum_mod, mod_arg, temp_mod, temp_exp, temp_base, 3);

	if (!mpz_cmp_ui(gmpnum_mod, 0)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Modulo by zero");
		FREE_GMP_TEMP(temp_base);
		FREE_GMP_TEMP(temp_exp);
		FREE_GMP_TEMP(temp_mod);
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);
	if (use_ui) {
		mpz_powm_ui(gmpnum_result, gmpnum_base, (zend_ulong) Z_LVAL_P(exp_arg), gmpnum_mod);
	} else {
		mpz_powm(gmpnum_result, gmpnum_base, gmpnum_exp, gmpnum_mod);
		FREE_GMP_TEMP(temp_exp);
	}

	FREE_GMP_TEMP(temp_base);
	FREE_GMP_TEMP(temp_mod);
}
/* }}} */

/* {{{ Takes integer part of square root of a */
ZEND_FUNCTION(gmp_sqrt)
{
	zval *a_arg;
	mpz_ptr gmpnum_a, gmpnum_result;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);

	if (mpz_sgn(gmpnum_a) < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		FREE_GMP_TEMP(temp_a);
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);
	mpz_sqrt(gmpnum_result, gmpnum_a);
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ Square root with remainder */
ZEND_FUNCTION(gmp_sqrtrem)
{
	zval *a_arg;
	mpz_ptr gmpnum_a, gmpnum_result1, gmpnum_result2;
	gmp_temp_t temp_a;
	zval result1, result2;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);

	if (mpz_sgn(gmpnum_a) < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		FREE_GMP_TEMP(temp_a);
		RETURN_THROWS();
	}

	gmp_create(&result1, &gmpnum_result1);
	gmp_create(&result2, &gmpnum_result2);

	array_init(return_value);
	add_next_index_zval(return_value, &result1);
	add_next_index_zval(return_value, &result2);

	mpz_sqrtrem(gmpnum_result1, gmpnum_result2, gmpnum_a);
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ Takes integer part of nth root */
ZEND_FUNCTION(gmp_root)
{
	zval *a_arg;
	zend_long nth;
	mpz_ptr gmpnum_a, gmpnum_result;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &a_arg, &nth) == FAILURE) {
		RETURN_THROWS();
	}

	if (nth <= 0) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);

	if (nth % 2 == 0 && mpz_sgn(gmpnum_a) < 0) {
		zend_argument_value_error(2, "must be odd if argument #1 ($a) is negative");
		FREE_GMP_TEMP(temp_a);
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);
	mpz_root(gmpnum_result, gmpnum_a, (gmp_ulong) nth);
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ Calculates integer part of nth root and remainder */
ZEND_FUNCTION(gmp_rootrem)
{
	zval *a_arg;
	zend_long nth;
	mpz_ptr gmpnum_a, gmpnum_result1, gmpnum_result2;
	gmp_temp_t temp_a;
	zval result1, result2;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &a_arg, &nth) == FAILURE) {
		RETURN_THROWS();
	}

	if (nth <= 0) {
		zend_argument_value_error(2, "must be greater than or equal to 1");
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);

	if (nth % 2 == 0 && mpz_sgn(gmpnum_a) < 0) {
		zend_argument_value_error(2, "must be odd if argument #1 ($a) is negative");
		FREE_GMP_TEMP(temp_a);
		RETURN_THROWS();
	}

	gmp_create(&result1, &gmpnum_result1);
	gmp_create(&result2, &gmpnum_result2);

	array_init(return_value);
	add_next_index_zval(return_value, &result1);
	add_next_index_zval(return_value, &result2);

#if GMP_51_OR_NEWER
	/* mpz_rootrem() is supported since GMP 4.2, but buggy wrt odd roots
	 * of negative numbers */
	mpz_rootrem(gmpnum_result1, gmpnum_result2, gmpnum_a, (gmp_ulong) nth);
#else
	mpz_root(gmpnum_result1, gmpnum_a, (gmp_ulong) nth);
	mpz_pow_ui(gmpnum_result2, gmpnum_result1, (gmp_ulong) nth);
	mpz_sub(gmpnum_result2, gmpnum_a, gmpnum_result2);
#endif

	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ Checks if a is an exact square */
ZEND_FUNCTION(gmp_perfect_square)
{
	zval *a_arg;
	mpz_ptr gmpnum_a;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);

	RETVAL_BOOL((mpz_perfect_square_p(gmpnum_a) != 0));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ Checks if a is a perfect power */
ZEND_FUNCTION(gmp_perfect_power)
{
	zval *a_arg;
	mpz_ptr gmpnum_a;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);

	RETVAL_BOOL((mpz_perfect_power_p(gmpnum_a) != 0));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ Checks if a is "probably prime" */
ZEND_FUNCTION(gmp_prob_prime)
{
	zval *gmpnumber_arg;
	mpz_ptr gmpnum_a;
	zend_long reps = 10;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|l", &gmpnumber_arg, &reps) == FAILURE) {
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, gmpnumber_arg, temp_a, 1);

	RETVAL_LONG(mpz_probab_prime_p(gmpnum_a, (int)reps));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ Computes greatest common denominator (gcd) of a and b */
ZEND_FUNCTION(gmp_gcd)
{
	gmp_binary_ui_op(mpz_gcd, gmp_mpz_gcd_ui);
}
/* }}} */

/* {{{ Computes least common multiple (lcm) of a and b */
ZEND_FUNCTION(gmp_lcm)
{
	gmp_binary_ui_op(mpz_lcm, mpz_lcm_ui);
}
/* }}} */

/* {{{ Computes G, S, and T, such that AS + BT = G = `gcd' (A, B) */
ZEND_FUNCTION(gmp_gcdext)
{
	zval *a_arg, *b_arg;
	mpz_ptr gmpnum_a, gmpnum_b, gmpnum_t, gmpnum_s, gmpnum_g;
	gmp_temp_t temp_a, temp_b;
	zval result_g, result_s, result_t;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a_arg, &b_arg) == FAILURE){
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);
	FETCH_GMP_ZVAL_DEP(gmpnum_b, b_arg, temp_b, temp_a, 2);

	gmp_create(&result_g, &gmpnum_g);
	gmp_create(&result_s, &gmpnum_s);
	gmp_create(&result_t, &gmpnum_t);

	array_init(return_value);
	add_assoc_zval(return_value, "g", &result_g);
	add_assoc_zval(return_value, "s", &result_s);
	add_assoc_zval(return_value, "t", &result_t);

	mpz_gcdext(gmpnum_g, gmpnum_s, gmpnum_t, gmpnum_a, gmpnum_b);
	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
}
/* }}} */

/* {{{ Computes the inverse of a modulo b */
ZEND_FUNCTION(gmp_invert)
{
	zval *a_arg, *b_arg;
	mpz_ptr gmpnum_a, gmpnum_b, gmpnum_result;
	gmp_temp_t temp_a, temp_b;
	int res;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a_arg, &b_arg) == FAILURE){
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);
	FETCH_GMP_ZVAL_DEP(gmpnum_b, b_arg, temp_b, temp_a, 2);

	// TODO Early check if b_arg IS_LONG?
	if (0 == mpz_cmp_ui(gmpnum_b, 0)) {
		zend_throw_exception_ex(zend_ce_division_by_zero_error, 0, "Division by zero");
		FREE_GMP_TEMP(temp_a);
		FREE_GMP_TEMP(temp_b);
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);
	res = mpz_invert(gmpnum_result, gmpnum_a, gmpnum_b);
	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
	if (!res) {
		zval_ptr_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Computes Jacobi symbol */
ZEND_FUNCTION(gmp_jacobi)
{
	zval *a_arg, *b_arg;
	mpz_ptr gmpnum_a, gmpnum_b;
	gmp_temp_t temp_a, temp_b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a_arg, &b_arg) == FAILURE){
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);
	FETCH_GMP_ZVAL_DEP(gmpnum_b, b_arg, temp_b, temp_a, 2);

	RETVAL_LONG(mpz_jacobi(gmpnum_a, gmpnum_b));

	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
}
/* }}} */

/* {{{ Computes Legendre symbol */
ZEND_FUNCTION(gmp_legendre)
{
	zval *a_arg, *b_arg;
	mpz_ptr gmpnum_a, gmpnum_b;
	gmp_temp_t temp_a, temp_b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a_arg, &b_arg) == FAILURE){
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);
	FETCH_GMP_ZVAL_DEP(gmpnum_b, b_arg, temp_b, temp_a, 2);

	RETVAL_LONG(mpz_legendre(gmpnum_a, gmpnum_b));

	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
}
/* }}} */

/* {{{ Computes the Kronecker symbol */
ZEND_FUNCTION(gmp_kronecker)
{
	zval *a_arg, *b_arg;
	mpz_ptr gmpnum_a, gmpnum_b;
	gmp_temp_t temp_a, temp_b;
	bool use_a_si = 0, use_b_si = 0;
	int result;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a_arg, &b_arg) == FAILURE){
		RETURN_THROWS();
	}

	if (Z_TYPE_P(a_arg) == IS_LONG && Z_TYPE_P(b_arg) != IS_LONG) {
		use_a_si = 1;
		temp_a.is_used = 0;
	} else {
		FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);
	}

	if (Z_TYPE_P(b_arg) == IS_LONG) {
		use_b_si = 1;
		temp_b.is_used = 0;
	} else {
		FETCH_GMP_ZVAL_DEP(gmpnum_b, b_arg, temp_b, temp_a, 2);
	}

	if (use_a_si) {
		ZEND_ASSERT(use_b_si == 0);
		result = mpz_si_kronecker((gmp_long) Z_LVAL_P(a_arg), gmpnum_b);
	} else if (use_b_si) {
		result = mpz_kronecker_si(gmpnum_a, (gmp_long) Z_LVAL_P(b_arg));
	} else {
		result = mpz_kronecker(gmpnum_a, gmpnum_b);
	}

	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);

	RETURN_LONG(result);
}
/* }}} */

/* {{{ Compares two numbers */
ZEND_FUNCTION(gmp_cmp)
{
	zval *a_arg, *b_arg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a_arg, &b_arg) == FAILURE){
		RETURN_THROWS();
	}

	gmp_cmp(return_value, a_arg, b_arg, /* is_operator */ false);
}
/* }}} */

/* {{{ Gets the sign of the number */
ZEND_FUNCTION(gmp_sign)
{
	/* Can't use gmp_unary_opl here, because mpz_sgn is a macro */
	zval *a_arg;
	mpz_ptr gmpnum_a;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &a_arg) == FAILURE){
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);

	RETVAL_LONG(mpz_sgn(gmpnum_a));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

static void gmp_init_random(void)
{
	if (!GMPG(rand_initialized)) {
		/* Initialize */
		gmp_randinit_mt(GMPG(rand_state));
		/* Seed */
		zend_long seed = 0;
		if (php_random_bytes_silent(&seed, sizeof(zend_long)) == FAILURE) {
			seed = GENERATE_SEED();
		}
		gmp_randseed_ui(GMPG(rand_state), seed);

		GMPG(rand_initialized) = 1;
	}
}

/* {{{ Seed the RNG */
ZEND_FUNCTION(gmp_random_seed)
{
	zval *seed;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &seed) == FAILURE) {
		RETURN_THROWS();
	}

	gmp_init_random();

	if (Z_TYPE_P(seed) == IS_LONG && Z_LVAL_P(seed) >= 0) {
		gmp_randseed_ui(GMPG(rand_state), Z_LVAL_P(seed));
	}
	else {
		mpz_ptr gmpnum_seed;

		FETCH_GMP_ZVAL(gmpnum_seed, seed, temp_a, 1);

		gmp_randseed(GMPG(rand_state), gmpnum_seed);

		FREE_GMP_TEMP(temp_a);
	}
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

	if (bits <= 0) {
		zend_argument_value_error(1, "must be greater than or equal to 1");
		RETURN_THROWS();
	}

	INIT_GMP_RETVAL(gmpnum_result);
	gmp_init_random();

	mpz_urandomb(gmpnum_result, GMPG(rand_state), bits);
}
/* }}} */

/* {{{ Gets a random number in the range min to max */
ZEND_FUNCTION(gmp_random_range)
{
	zval *min_arg, *max_arg;
	mpz_ptr gmpnum_max, gmpnum_result;
	mpz_t gmpnum_range;
	gmp_temp_t temp_a, temp_b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &min_arg, &max_arg) == FAILURE) {
		RETURN_THROWS();
	}

	gmp_init_random();

	FETCH_GMP_ZVAL(gmpnum_max, max_arg, temp_a, 2);

	if (Z_TYPE_P(min_arg) == IS_LONG && Z_LVAL_P(min_arg) >= 0) {
		if (mpz_cmp_ui(gmpnum_max, Z_LVAL_P(min_arg)) <= 0) {
			FREE_GMP_TEMP(temp_a);
			zend_argument_value_error(1, "must be less than argument #2 ($maximum)");
			RETURN_THROWS();
		}

		INIT_GMP_RETVAL(gmpnum_result);
		mpz_init(gmpnum_range);

		if (Z_LVAL_P(min_arg) != 0) {
			mpz_sub_ui(gmpnum_range, gmpnum_max, Z_LVAL_P(min_arg) - 1);
		} else {
			mpz_add_ui(gmpnum_range, gmpnum_max, 1);
		}

		mpz_urandomm(gmpnum_result, GMPG(rand_state), gmpnum_range);

		if (Z_LVAL_P(min_arg) != 0) {
			mpz_add_ui(gmpnum_result, gmpnum_result, Z_LVAL_P(min_arg));
		}

		mpz_clear(gmpnum_range);
		FREE_GMP_TEMP(temp_a);
	} else {
		mpz_ptr gmpnum_min;

		FETCH_GMP_ZVAL_DEP(gmpnum_min, min_arg, temp_b, temp_a, 1);

		if (mpz_cmp(gmpnum_max, gmpnum_min) <= 0) {
			FREE_GMP_TEMP(temp_b);
			FREE_GMP_TEMP(temp_a);
			zend_argument_value_error(1, "must be less than argument #2 ($maximum)");
			RETURN_THROWS();
		}

		INIT_GMP_RETVAL(gmpnum_result);
		mpz_init(gmpnum_range);

		mpz_sub(gmpnum_range, gmpnum_max, gmpnum_min);
		mpz_add_ui(gmpnum_range, gmpnum_range, 1);
		mpz_urandomm(gmpnum_result, GMPG(rand_state), gmpnum_range);
		mpz_add(gmpnum_result, gmpnum_result, gmpnum_min);

		mpz_clear(gmpnum_range);
		FREE_GMP_TEMP(temp_b);
		FREE_GMP_TEMP(temp_a);
	}
}
/* }}} */

/* {{{ Calculates logical AND of a and b */
ZEND_FUNCTION(gmp_and)
{
	gmp_binary_op(mpz_and);
}
/* }}} */

/* {{{ Calculates logical OR of a and b */
ZEND_FUNCTION(gmp_or)
{
	gmp_binary_op(mpz_ior);
}
/* }}} */

/* {{{ Calculates one's complement of a */
ZEND_FUNCTION(gmp_com)
{
	gmp_unary_op(mpz_com);
}
/* }}} */

/* {{{ Finds next prime of a */
ZEND_FUNCTION(gmp_nextprime)
{
	gmp_unary_op(mpz_nextprime);
}
/* }}} */

/* {{{ Calculates logical exclusive OR of a and b */
ZEND_FUNCTION(gmp_xor)
{
	gmp_binary_op(mpz_xor);
}
/* }}} */

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

	if (index < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}
	if (index / GMP_NUMB_BITS >= INT_MAX) {
		zend_argument_value_error(2, "must be less than %d * %d", INT_MAX, GMP_NUMB_BITS);
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

	if (index < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	gmpnum_a = GET_GMP_FROM_ZVAL(a_arg);
	mpz_clrbit(gmpnum_a, index);
}
/* }}} */

/* {{{ Tests if bit is set in a */
ZEND_FUNCTION(gmp_testbit)
{
	zval *a_arg;
	zend_long index;
	mpz_ptr gmpnum_a;
	gmp_temp_t temp_a;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &a_arg, &index) == FAILURE){
		RETURN_THROWS();
	}

	if (index < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);
	RETVAL_BOOL(mpz_tstbit(gmpnum_a, index));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ Calculates the population count of a */
ZEND_FUNCTION(gmp_popcount)
{
	gmp_unary_opl(mpz_popcount);
}
/* }}} */

/* {{{ Calculates hamming distance between a and b */
ZEND_FUNCTION(gmp_hamdist)
{
	zval *a_arg, *b_arg;
	mpz_ptr gmpnum_a, gmpnum_b;
	gmp_temp_t temp_a, temp_b;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &a_arg, &b_arg) == FAILURE){
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);
	FETCH_GMP_ZVAL_DEP(gmpnum_b, b_arg, temp_b, temp_a, 2);

	RETVAL_LONG(mpz_hamdist(gmpnum_a, gmpnum_b));

	FREE_GMP_TEMP(temp_a);
	FREE_GMP_TEMP(temp_b);
}
/* }}} */

/* {{{ Finds first zero bit */
ZEND_FUNCTION(gmp_scan0)
{
	zval *a_arg;
	mpz_ptr gmpnum_a;
	gmp_temp_t temp_a;
	zend_long start;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &a_arg, &start) == FAILURE){
		RETURN_THROWS();
	}

	if (start < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);

	RETVAL_LONG(mpz_scan0(gmpnum_a, start));
	FREE_GMP_TEMP(temp_a);
}
/* }}} */

/* {{{ Finds first non-zero bit */
ZEND_FUNCTION(gmp_scan1)
{
	zval *a_arg;
	mpz_ptr gmpnum_a;
	gmp_temp_t temp_a;
	zend_long start;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zl", &a_arg, &start) == FAILURE){
		RETURN_THROWS();
	}

	if (start < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	FETCH_GMP_ZVAL(gmpnum_a, a_arg, temp_a, 1);

	RETVAL_LONG(mpz_scan1(gmpnum_a, start));
	FREE_GMP_TEMP(temp_a);
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
			convert_to_gmp(GET_GMP_FROM_ZVAL(ZEND_THIS), num, 16, 0) == FAILURE) {
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
