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
*/

#include "iterators.h"
#include "zend_API.h"
#include "iterators_arginfo.h"

#include <zend_interfaces.h>
#include "php.h"

#define DUMP(s) php_output_write((s), sizeof((s)) - 1)

static zend_class_entry *traversable_test_ce;

// Dummy iterator that yields numbers from 0..4,
// while printing operations to the output buffer
typedef struct {
	zend_object_iterator intern;
	zval current;
} test_traversable_it;

static test_traversable_it *test_traversable_it_fetch(zend_object_iterator *iter) {
	return (test_traversable_it *)iter;
}

static void test_traversable_it_dtor(zend_object_iterator *iter) {
	DUMP("TraversableTest::drop\n");
	test_traversable_it *iterator = test_traversable_it_fetch(iter);
	zval_ptr_dtor(&iterator->intern.data);
}

static void test_traversable_it_rewind(zend_object_iterator *iter) {
	DUMP("TraversableTest::rewind\n");
	test_traversable_it *iterator = test_traversable_it_fetch(iter);
	ZVAL_LONG(&iterator->current, 0);
}

static void test_traversable_it_next(zend_object_iterator *iter) {
	DUMP("TraversableTest::next\n");
	test_traversable_it *iterator = test_traversable_it_fetch(iter);
	ZVAL_LONG(&iterator->current, Z_LVAL(iterator->current) + 1);
}

static int test_traversable_it_valid(zend_object_iterator *iter) {
	DUMP("TraversableTest::valid\n");
	test_traversable_it *iterator = test_traversable_it_fetch(iter);
	if (Z_LVAL(iterator->current) < 4) {
		return SUCCESS;
	}
	return FAILURE;
}

static void test_traversable_it_key(zend_object_iterator *iter, zval *return_value) {
	DUMP("TraversableTest::key\n");
	test_traversable_it *iterator = test_traversable_it_fetch(iter);
	ZVAL_LONG(return_value, Z_LVAL(iterator->current));
}

static zval *test_traversable_it_current(zend_object_iterator *iter) {
	DUMP("TraversableTest::current\n");
	test_traversable_it *iterator = test_traversable_it_fetch(iter);
	return &iterator->current;
}

static const zend_object_iterator_funcs test_traversable_it_vtable = {
	test_traversable_it_dtor,
	test_traversable_it_valid,
	test_traversable_it_current,
	test_traversable_it_key,
	test_traversable_it_next,
	test_traversable_it_rewind,
	NULL, // invalidate_current
	NULL, // get_gc
};

static zend_object_iterator *test_traversable_get_iterator(
	zend_class_entry *ce,
	zval *object,
	int by_ref
) {
	test_traversable_it *iterator;

	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(test_traversable_it));
	zend_iterator_init((zend_object_iterator*)iterator);

	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	iterator->intern.funcs = &test_traversable_it_vtable;
	ZVAL_LONG(&iterator->current, 0);

	return (zend_object_iterator*)iterator;
}

ZEND_METHOD(ZendTest_Iterators_TraversableTest, __construct) {
	ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_METHOD(ZendTest_Iterators_TraversableTest, getIterator) {
	ZEND_PARSE_PARAMETERS_NONE();
    zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

void zend_test_iterators_init(void) {
	traversable_test_ce = register_class_ZendTest_Iterators_TraversableTest(zend_ce_aggregate);
	traversable_test_ce->get_iterator = test_traversable_get_iterator;
}
