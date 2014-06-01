/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2013 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   |         Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* These iterators were designed to operate within the foreach()
 * structures provided by the engine, but could be extended for use
 * with other iterative engine opcodes.
 * These methods have similar semantics to the zend_hash API functions
 * with similar names.
 * */

typedef struct _zend_object_iterator zend_object_iterator;

typedef struct _zend_object_iterator_funcs {
	/* release all resources associated with this iterator instance */
	void (*dtor)(zend_object_iterator *iter TSRMLS_DC);

	/* check for end of iteration (FAILURE or SUCCESS if data is valid) */
	int (*valid)(zend_object_iterator *iter TSRMLS_DC);

	/* fetch the item data for the current element */
	void (*get_current_data)(zend_object_iterator *iter, zval ***data TSRMLS_DC);

	/* fetch the key for the current element (optional, may be NULL). The key
	 * should be written into the provided zval* using the ZVAL_* macros. If
	 * this handler is not provided auto-incrementing integer keys will be
	 * used. */
	void (*get_current_key)(zend_object_iterator *iter, zval *key TSRMLS_DC);

	/* step forwards to next element */
	void (*move_forward)(zend_object_iterator *iter TSRMLS_DC);

	/* rewind to start of data (optional, may be NULL) */
	void (*rewind)(zend_object_iterator *iter TSRMLS_DC);

	/* invalidate current value/key (optional, may be NULL) */
	void (*invalidate_current)(zend_object_iterator *iter TSRMLS_DC);
} zend_object_iterator_funcs;

struct _zend_object_iterator {
	void *data;
	zend_object_iterator_funcs *funcs;
	ulong index; /* private to fe_reset/fe_fetch opcodes */
};

typedef struct _zend_class_iterator_funcs {
	zend_object_iterator_funcs  *funcs;
	union _zend_function *zf_new_iterator;
	union _zend_function *zf_valid;
	union _zend_function *zf_current;
	union _zend_function *zf_key;
	union _zend_function *zf_next;
	union _zend_function *zf_rewind;
} zend_class_iterator_funcs;

enum zend_object_iterator_kind {
	ZEND_ITER_INVALID,
	ZEND_ITER_PLAIN_ARRAY,
	ZEND_ITER_PLAIN_OBJECT,
	ZEND_ITER_OBJECT
};

BEGIN_EXTERN_C()
/* given a zval, returns stuff that can be used to iterate it. */
ZEND_API enum zend_object_iterator_kind zend_iterator_unwrap(zval *array_ptr, zend_object_iterator **iter TSRMLS_DC);

/* given an iterator, wrap it up as a zval for use by the engine opcodes */
ZEND_API zval *zend_iterator_wrap(zend_object_iterator *iter TSRMLS_DC);

ZEND_API void zend_register_iterator_wrapper(TSRMLS_D);
END_EXTERN_C()

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
