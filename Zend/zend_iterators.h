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
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   |         Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
*/

/* These iterators were designed to operate within the foreach()
 * structures provided by the engine, but could be extended for use
 * with other iterative engine opcodes.
 * These methods have similar semantics to the zend_hash API functions
 * with similar names.
 * */

typedef struct _zend_object_iterator zend_object_iterator;

typedef struct _zend_object_iterator_funcs {
	/* release all resources associated with this iterator instance */
	void (*dtor)(zend_object_iterator *iter);

	/* check for end of iteration (FAILURE or SUCCESS if data is valid) */
	int (*valid)(zend_object_iterator *iter);

	/* fetch the item data for the current element */
	zval *(*get_current_data)(zend_object_iterator *iter);

	/* fetch the key for the current element (optional, may be NULL). The key
	 * should be written into the provided zval* using the ZVAL_* macros. If
	 * this handler is not provided auto-incrementing integer keys will be
	 * used. */
	void (*get_current_key)(zend_object_iterator *iter, zval *key);

	/* step forwards to next element */
	void (*move_forward)(zend_object_iterator *iter);

	/* rewind to start of data (optional, may be NULL) */
	void (*rewind)(zend_object_iterator *iter);

	/* invalidate current value/key (optional, may be NULL) */
	void (*invalidate_current)(zend_object_iterator *iter);

	/* Expose owned values to GC.
	 * This has the same semantics as the corresponding object handler. */
	HashTable *(*get_gc)(zend_object_iterator *iter, zval **table, int *n);
} zend_object_iterator_funcs;

struct _zend_object_iterator {
	zend_object std;
	zval data;
	const zend_object_iterator_funcs *funcs;
	zend_ulong index; /* private to fe_reset/fe_fetch opcodes */
};

typedef struct _zend_class_iterator_funcs {
	zend_function *zf_new_iterator;
	zend_function *zf_valid;
	zend_function *zf_current;
	zend_function *zf_key;
	zend_function *zf_next;
	zend_function *zf_rewind;
} zend_class_iterator_funcs;

typedef struct _zend_class_arrayaccess_funcs {
	zend_function *zf_offsetget;
	zend_function *zf_offsetexists;
	zend_function *zf_offsetset;
	zend_function *zf_offsetunset;
} zend_class_arrayaccess_funcs;

BEGIN_EXTERN_C()
/* given a zval, returns stuff that can be used to iterate it. */
ZEND_API zend_object_iterator* zend_iterator_unwrap(zval *array_ptr);

/* given an iterator, wrap it up as a zval for use by the engine opcodes */
ZEND_API void zend_iterator_init(zend_object_iterator *iter);
ZEND_API void zend_iterator_dtor(zend_object_iterator *iter);

ZEND_API void zend_register_iterator_wrapper(void);
END_EXTERN_C()
