/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
*/
#ifndef ZEND_API_HELPER_H
#define ZEND_API_HELPER_H

/* Overloaded functions in C++ or _Generic parametric polymorphism in C */
#if __cplusplus
static inline void add_index_long(zval *arg, zend_ulong index, zend_long n) {
	add_index_long_ex(Z_ARR_P(arg), index, n);
}
static inline void add_index_long(HashTable *arg, zend_ulong index, zend_long n) {
	add_index_long_ex(arg, index, n);
}

static inline void add_index_null(zval *arg, zend_ulong index) {
	add_index_null_ex(Z_ARR_P(arg), index);
}
static inline void add_index_null(HashTable *arg, zend_ulong index) {
	add_index_null_ex(arg, index);
}

static inline void add_index_bool(zval *arg, zend_ulong index, bool b) {
	add_index_bool_ex(Z_ARR_P(arg), index, b);
}
static inline void add_index_bool(HashTable *arg, zend_ulong index, bool b) {
	add_index_bool_ex(arg, index, b);
}

static inline void add_index_resource(zval *arg, zend_ulong index, zend_resource *r) {
	add_index_resource_ex(Z_ARR_P(arg), index, r);
}
static inline void add_index_resource(HashTable *arg, zend_ulong index, zend_resource *r) {
	add_index_resource_ex(arg, index, r);
}

static inline void add_index_double(zval *arg, zend_ulong index, double d) {
	add_index_double_ex(Z_ARR_P(arg), index, d);
}
static inline void add_index_double(HashTable *arg, zend_ulong index, double d) {
	add_index_double_ex(arg, index, d);
}

static inline void add_index_str(zval *arg, zend_ulong index, zend_string *str) {
	add_index_str_ex(Z_ARR_P(arg), index, str);
}
static inline void add_index_str(HashTable *arg, zend_ulong index, zend_string *str) {
	add_index_str_ex(arg, index, str);
}

static inline void add_index_string(zval *arg, zend_ulong index, const char *str) {
	add_index_string_ex(Z_ARR_P(arg), index, str);
}
static inline void add_index_string(HashTable *arg, zend_ulong index, const char *str) {
	add_index_string_ex(arg, index, str);
}

static inline void add_index_stringl(zval *arg, zend_ulong index, const char *str, size_t length) {
	add_index_stringl_ex(Z_ARR_P(arg), index, str, length);
}
static inline void add_index_stringl(HashTable *arg, zend_ulong index, const char *str, size_t length) {
	add_index_stringl_ex(arg, index, str, length);
}

static inline void add_index_array(zval *arg, zend_ulong index, zend_array *arr) {
	add_index_array_ex(Z_ARR_P(arg), index, arr);
}
static inline void add_index_array(HashTable *arg, zend_ulong index, zend_array *arr) {
	add_index_array_ex(arg, index, arr);
}

static inline void add_index_object(zval *arg, zend_ulong index, zend_object *obj) {
	add_index_object_ex(Z_ARR_P(arg), index, obj);
}
static inline void add_index_object(HashTable *arg, zend_ulong index, zend_object *obj) {
	add_index_object_ex(arg, index, obj);
}

static inline void add_index_reference(zval *arg, zend_ulong index, zend_reference *ref) {
	add_index_reference_ex(Z_ARR_P(arg), index, ref);
}
static inline void add_index_reference(HashTable *arg, zend_ulong index, zend_reference *ref) {
	add_index_reference_ex(arg, index, ref);
}

static inline zend_result add_next_index_long(zval *arg, zend_long n) {
	return add_next_index_long_ex(Z_ARR_P(arg), n);
}
static inline zend_result add_next_index_long(HashTable *arg, zend_long n) {
	return add_next_index_long_ex(arg, n);
}

static inline zend_result add_next_index_null(zval *arg) {
	return add_next_index_null_ex(Z_ARR_P(arg));
}
static inline zend_result add_next_index_null(HashTable *arg) {
	return add_next_index_null_ex(arg);
}

static inline zend_result add_next_index_bool(zval *arg, bool b) {
	return add_next_index_bool_ex(Z_ARR_P(arg), b);
}
static inline zend_result add_next_index_bool(HashTable *arg, bool b) {
	return add_next_index_bool_ex(arg, b);
}

static inline zend_result add_next_index_resource(zval *arg, zend_resource *r) {
	return add_next_index_resource_ex(Z_ARR_P(arg), r);
}
static inline zend_result add_next_index_resource(HashTable *arg, zend_resource *r) {
	return add_next_index_resource_ex(arg, r);
}

static inline zend_result add_next_index_double(zval *arg, double d) {
	return add_next_index_double_ex(Z_ARR_P(arg), d);
}
static inline zend_result add_next_index_double(HashTable *arg, double d) {
	return add_next_index_double_ex(arg, d);
}

static inline zend_result add_next_index_str(zval *arg, zend_string *str) {
	return add_next_index_str_ex(Z_ARR_P(arg), str);
}
static inline zend_result add_next_index_str(HashTable *arg, zend_string *str) {
	return add_next_index_str_ex(arg, str);
}

static inline zend_result add_next_index_string(zval *arg, const char *str) {
	return add_next_index_string_ex(Z_ARR_P(arg), str);
}
static inline zend_result add_next_index_string(HashTable *arg, const char *str) {
	return add_next_index_string_ex(arg, str);
}

static inline zend_result add_next_index_stringl(zval *arg, const char *str, size_t length) {
	return add_next_index_stringl_ex(Z_ARR_P(arg), str, length);
}
static inline zend_result add_next_index_stringl(HashTable *arg, const char *str, size_t length) {
	return add_next_index_stringl_ex(arg, str, length);
}

static inline zend_result add_next_index_array(zval *arg, zend_array *arr) {
	return add_next_index_array_ex(Z_ARR_P(arg), arr);
}
static inline zend_result add_next_index_array(HashTable *arg, zend_array *arr) {
	return add_next_index_array_ex(arg, arr);
}

static inline zend_result add_next_index_object(zval *arg, zend_object *obj) {
	return add_next_index_object_ex(Z_ARR_P(arg), obj);
}
static inline zend_result add_next_index_object(HashTable *arg, zend_object *obj) {
	return add_next_index_object_ex(arg, obj);
}

static inline zend_result add_next_index_reference(zval *arg, zend_reference *ref) {
	return add_next_index_reference_ex(Z_ARR_P(arg), ref);
}
static inline zend_result add_next_index_reference(HashTable *arg, zend_reference *ref) {
	return add_next_index_reference_ex(arg, ref);
}

#else

BEGIN_EXTERN_C()
/* One might expect C _Generic to properly branch on parameters of function so that one can do something like:
#define add_index_long(a, i, val) \
	add_index_long_ex( \
		_Generic((a), HashTable *: a, zval *: Z_ARR_P(a) ), \
		i, val)
 * or
#define add_index_long(a, val) \
	_Generic( \
		(a), \
		HashTable *: add_index_long_ex((a), (val)), \
		zval *: add_index_long_ex(Z_ARR_P(a), (val)))

 * to work, however it turns out this is not possible as explained in [1] "the discarded expressions still
 * have to be semantically valid" which is not possible as a zval* is not a HashTable*
 * and a HashTable* is not compatible with the Z_ARR_P macro.
 * To have semantically correct branches of *all* generic expressions we create helper functions
 * that take a void* and return a HashTable*.
 * [1] https://www.chiark.greenend.org.uk/~sgtatham/quasiblog/c11-generic/#bigbug
 */

static inline HashTable *htid(void *ptr) { return ptr; }
static inline HashTable *z2h(void *ptr) { return Z_ARR_P((zval*)ptr); }

#define give_me_a_ht(ptr) _Generic(ptr, HashTable* : htid, zval* : z2h) (ptr)

#define add_index_long(a, i, val) \
	add_index_long_ex(give_me_a_ht(a), i, val)
#define add_index_null(a, i) \
	add_index_null_ex(give_me_a_ht(a), i)
#define add_index_bool(a, i, val) \
	add_index_bool_ex(give_me_a_ht(a), i, val)
#define add_index_resource(a, i, val) \
	add_index_resource_ex(give_me_a_ht(a), i, val)
#define add_index_double(a, i, val) \
	add_index_double_ex(give_me_a_ht(a), i, val)
#define add_index_str(a, i, val) \
	add_index_str_ex(give_me_a_ht(a), i, val)
#define add_index_string(a, i, val) \
	add_index_string_ex(give_me_a_ht(a), i, val)
#define add_index_stringl(a, i, val, len) \
	add_index_stringl_ex(give_me_a_ht(a), i, val, len)
#define add_index_array(a, i, val) \
	add_index_array_ex(give_me_a_ht(a), i, val)
#define add_index_object(a, i, val) \
	add_index_object_ex(give_me_a_ht(a), i, val)
#define add_index_reference(a, i, val) \
	add_index_reference_ex(give_me_a_ht(a), i, val)

#define add_next_index_long(a, val) \
	add_next_index_long_ex(give_me_a_ht(a), val)
#define add_next_index_null(a) \
	add_next_index_null_ex(give_me_a_ht(a))
#define add_next_index_bool(a, val) \
	add_next_index_bool_ex(give_me_a_ht(a), val)
#define add_next_index_resource(a, val) \
	add_next_index_resource_ex(give_me_a_ht(a), val)
#define add_next_index_double(a, val) \
	add_next_index_double_ex(give_me_a_ht(a), val)
#define add_next_index_str(a, val) \
	add_next_index_str_ex(give_me_a_ht(a), val)
#define add_next_index_string(a, val) \
	add_next_index_string_ex(give_me_a_ht(a), val)
#define add_next_index_stringl(a, val, len) \
	add_next_index_stringl_ex(give_me_a_ht(a), val, len)
#define add_next_index_array(a, val) \
	add_next_index_array_ex(give_me_a_ht(a), val)
#define add_next_index_object(a, val) \
	add_next_index_object_ex(give_me_a_ht(a), val)
#define add_next_index_reference(a, val) \
	add_next_index_reference_ex(give_me_a_ht(a), val)

END_EXTERN_C()

#endif

#endif
