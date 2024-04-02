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
  | Authors: George Peter Banyard <girgias@php.net>                      |
  +----------------------------------------------------------------------+
*/

#include "object_handlers.h"
#include "zend_API.h"
#include "object_handlers_arginfo.h"

/* donc refers to DoOperationNoCast */
static zend_class_entry *donc_ce;
static zend_object_handlers donc_object_handlers;

static zend_object* donc_object_create_ex(zend_class_entry* ce, zend_long l) {
	zend_object *obj = zend_objects_new(ce);
	object_properties_init(obj, ce);
	obj->handlers = &donc_object_handlers;
	ZVAL_LONG(OBJ_PROP_NUM(obj, 0), l);
	return obj;
}
static zend_object *donc_object_create(zend_class_entry *ce) /* {{{ */
{
	return donc_object_create_ex(ce, 0);
}
/* }}} */

static inline void donc_create(zval *target, zend_long l) /* {{{ */
{
	ZVAL_OBJ(target, donc_object_create_ex(donc_ce, l));
}

#define IS_DONC(zval) \
	(Z_TYPE_P(zval) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zval), donc_ce))

static void donc_add(zval *result, zval *op1, zval *op2)
{
	zend_long val_1;
	zend_long val_2;
	if (IS_DONC(op1)) {
		val_1 = Z_LVAL_P(OBJ_PROP_NUM(Z_OBJ_P(op1), 0));
	} else {
		val_1 = zval_get_long(op1);
	}
	if (IS_DONC(op2)) {
		val_2 = Z_LVAL_P(OBJ_PROP_NUM(Z_OBJ_P(op2), 0));
	} else {
		val_2 = zval_get_long(op2);
	}

	donc_create(result, val_1 + val_2);
}
static void donc_mul(zval *result, zval *op1, zval *op2)
{
	zend_long val_1;
	zend_long val_2;
	if (IS_DONC(op1)) {
		val_1 = Z_LVAL_P(OBJ_PROP_NUM(Z_OBJ_P(op1), 0));
	} else {
		val_1 = zval_get_long(op1);
	}
	if (IS_DONC(op2)) {
		val_2 = Z_LVAL_P(OBJ_PROP_NUM(Z_OBJ_P(op2), 0));
	} else {
		val_2 = zval_get_long(op2);
	}

	donc_create(result, val_1 * val_2);
}

static zend_result donc_do_operation(zend_uchar opcode, zval *result, zval *op1, zval *op2)
{
	zval op1_copy;
	zend_result status;

	if (result == op1) {
		ZVAL_COPY_VALUE(&op1_copy, op1);
		op1 = &op1_copy;
	}

	switch (opcode) {
		case ZEND_ADD:
			donc_add(result, op1, op2);
			if (UNEXPECTED(EG(exception))) { status = FAILURE; }
			status = SUCCESS;
			break;
		case ZEND_MUL:
			donc_mul(result, op1, op2);
			if (UNEXPECTED(EG(exception))) { status = FAILURE; }
			status = SUCCESS;
			break;
		default:
			status = FAILURE;
			break;
	}

	if (status == SUCCESS && op1 == &op1_copy) {
		zval_ptr_dtor(op1);
	}

	return status;
}

ZEND_METHOD(DoOperationNoCast, __construct)
{
	zend_long l;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(l)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_LONG(OBJ_PROP_NUM(Z_OBJ_P(ZEND_THIS), 0), l);
}

static zend_class_entry *long_castable_no_operation_ce;
static zend_object_handlers long_castable_no_operation_object_handlers;

static zend_object* long_castable_no_operation_object_create_ex(zend_class_entry* ce, zend_long l) {
	zend_object *obj = zend_objects_new(ce);
	object_properties_init(obj, ce);
	obj->handlers = &long_castable_no_operation_object_handlers;
	ZVAL_LONG(OBJ_PROP_NUM(obj, 0), l);
	return obj;
}

static zend_object *long_castable_no_operation_object_create(zend_class_entry *ce)
{
	return long_castable_no_operation_object_create_ex(ce, 0);
}

static zend_result long_castable_no_operation_cast_object(zend_object *obj, zval *result, int type)
{
	if (type == IS_LONG) {
		ZVAL_COPY(result, OBJ_PROP_NUM(obj, 0));
		return SUCCESS;
	}
	return FAILURE;
}

ZEND_METHOD(LongCastableNoOperations, __construct)
{
	zend_long l;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(l)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_LONG(OBJ_PROP_NUM(Z_OBJ_P(ZEND_THIS), 0), l);
}

static zend_class_entry *float_castable_no_operation_ce;
static zend_object_handlers float_castable_no_operation_object_handlers;

static zend_object* float_castable_no_operation_object_create_ex(zend_class_entry* ce, double d) {
	zend_object *obj = zend_objects_new(ce);
	object_properties_init(obj, ce);
	obj->handlers = &float_castable_no_operation_object_handlers;
	ZVAL_DOUBLE(OBJ_PROP_NUM(obj, 0), d);
	return obj;
}

static zend_object *float_castable_no_operation_object_create(zend_class_entry *ce)
{
	return float_castable_no_operation_object_create_ex(ce, 0.0);
}

static zend_result float_castable_no_operation_cast_object(zend_object *obj, zval *result, int type)
{
	if (type == IS_DOUBLE) {
		ZVAL_COPY(result, OBJ_PROP_NUM(obj, 0));
		return SUCCESS;
	}
	return FAILURE;
}

ZEND_METHOD(FloatCastableNoOperations, __construct)
{
	double d;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(d)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_DOUBLE(OBJ_PROP_NUM(Z_OBJ_P(ZEND_THIS), 0), d);
}

static zend_class_entry *numeric_castable_no_operation_ce;
static zend_object_handlers numeric_castable_no_operation_object_handlers;

static zend_object* numeric_castable_no_operation_object_create_ex(zend_class_entry* ce, const zval *n) {
	zend_object *obj = zend_objects_new(ce);
	object_properties_init(obj, ce);
	obj->handlers = &numeric_castable_no_operation_object_handlers;
	ZVAL_COPY(OBJ_PROP_NUM(obj, 0), n);
	return obj;
}

static zend_object *numeric_castable_no_operation_object_create(zend_class_entry *ce)
{
	zval tmp;
	ZVAL_LONG(&tmp, 0);
	return numeric_castable_no_operation_object_create_ex(ce, &tmp);
}

static zend_result numeric_castable_no_operation_cast_object(zend_object *obj, zval *result, int type)
{
	if (type == _IS_NUMBER) {
		ZVAL_COPY(result, OBJ_PROP_NUM(obj, 0));
		return SUCCESS;
	}
	return FAILURE;
}

ZEND_METHOD(NumericCastableNoOperations, __construct)
{
	zval *n;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_NUMBER(n)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_COPY(OBJ_PROP_NUM(Z_OBJ_P(ZEND_THIS), 0), n);
}

static zend_class_entry *dimension_handlers_no_ArrayAccess_ce;
static zend_object_handlers dimension_handlers_no_ArrayAccess_object_handlers;

static zend_object* dimension_handlers_no_ArrayAccess_object_create(zend_class_entry* ce) {
	zend_object *object = zend_objects_new(ce);
	object_properties_init(object, ce);
	object->handlers = &dimension_handlers_no_ArrayAccess_object_handlers;
	return object;
}

static void dimension_common_helper(zend_object *object, zval *offset, int prop_access_type) {
	ZVAL_BOOL(OBJ_PROP_NUM(object, prop_access_type), true);
	/* hasOffset */
	ZVAL_BOOL(OBJ_PROP_NUM(object, 5), offset != NULL);
	if (offset) {
		ZVAL_COPY(OBJ_PROP_NUM(object, 7), offset);
	}
}

static zval* dimension_handlers_no_ArrayAccess_read_dimension(zend_object *object, zval *offset, int type, zval *rv) {
	dimension_common_helper(object, offset, 0);
	/* ReadType */
	ZVAL_LONG(OBJ_PROP_NUM(object, 4), type);

	/* Normal logic */
	ZVAL_BOOL(rv, true);
	return rv;
}

static void dimension_handlers_no_ArrayAccess_write_dimension(zend_object *object, zval *offset, zval *value) {
	dimension_common_helper(object, offset, 1);
}

static int dimension_handlers_no_ArrayAccess_has_dimension(zend_object *object, zval *offset, int check_empty) {
	/* checkEmpty */
	ZVAL_LONG(OBJ_PROP_NUM(object, 6), check_empty);
	dimension_common_helper(object, offset, 2);

	/* Normal logic */
	return 1;
}

static void dimension_handlers_no_ArrayAccess_unset_dimension(zend_object *object, zval *offset) {
	dimension_common_helper(object, offset, 3);
}

void zend_test_object_handlers_init(void)
{
	/* DoOperationNoCast class */
	donc_ce = register_class_DoOperationNoCast();
	donc_ce->create_object = donc_object_create;
	memcpy(&donc_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	donc_object_handlers.do_operation = donc_do_operation;

	/* CastableNoOperation classes */
	long_castable_no_operation_ce = register_class_LongCastableNoOperations();
	long_castable_no_operation_ce->create_object = long_castable_no_operation_object_create;
	memcpy(&long_castable_no_operation_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	long_castable_no_operation_object_handlers.cast_object = long_castable_no_operation_cast_object;

	float_castable_no_operation_ce = register_class_FloatCastableNoOperations();
	float_castable_no_operation_ce->create_object = float_castable_no_operation_object_create;
	memcpy(&float_castable_no_operation_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	float_castable_no_operation_object_handlers.cast_object = float_castable_no_operation_cast_object;

	numeric_castable_no_operation_ce = register_class_NumericCastableNoOperations();
	numeric_castable_no_operation_ce->create_object = numeric_castable_no_operation_object_create;
	memcpy(&numeric_castable_no_operation_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	numeric_castable_no_operation_object_handlers.cast_object = numeric_castable_no_operation_cast_object;

	dimension_handlers_no_ArrayAccess_ce = register_class_DimensionHandlersNoArrayAccess();
	dimension_handlers_no_ArrayAccess_ce->create_object = dimension_handlers_no_ArrayAccess_object_create;
	memcpy(&dimension_handlers_no_ArrayAccess_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	dimension_handlers_no_ArrayAccess_object_handlers.read_dimension = dimension_handlers_no_ArrayAccess_read_dimension;
	dimension_handlers_no_ArrayAccess_object_handlers.write_dimension = dimension_handlers_no_ArrayAccess_write_dimension;
	dimension_handlers_no_ArrayAccess_object_handlers.has_dimension = dimension_handlers_no_ArrayAccess_has_dimension;
	dimension_handlers_no_ArrayAccess_object_handlers.unset_dimension = dimension_handlers_no_ArrayAccess_unset_dimension;
}
