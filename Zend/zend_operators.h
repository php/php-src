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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_OPERATORS_H
#define ZEND_OPERATORS_H

#include <errno.h>
#include <math.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

#include "zend_portability.h"
#include "zend_strtod.h"
#include "zend_multiply.h"
#include "zend_object_handlers.h"

#define LONG_SIGN_MASK ZEND_LONG_MIN

BEGIN_EXTERN_C()
ZEND_API zend_result ZEND_FASTCALL add_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL sub_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL mul_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL pow_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL div_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL mod_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL boolean_xor_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL boolean_not_function(zval *result, zval *op1);
ZEND_API zend_result ZEND_FASTCALL bitwise_not_function(zval *result, zval *op1);
ZEND_API zend_result ZEND_FASTCALL bitwise_or_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL bitwise_and_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL bitwise_xor_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL shift_left_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL shift_right_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL concat_function(zval *result, zval *op1, zval *op2);

ZEND_API bool ZEND_FASTCALL zend_is_identical(const zval *op1, const zval *op2);

ZEND_API zend_result ZEND_FASTCALL is_equal_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL is_identical_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL is_not_identical_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL is_not_equal_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL is_smaller_function(zval *result, zval *op1, zval *op2);
ZEND_API zend_result ZEND_FASTCALL is_smaller_or_equal_function(zval *result, zval *op1, zval *op2);

ZEND_API bool ZEND_FASTCALL zend_class_implements_interface(const zend_class_entry *class_ce, const zend_class_entry *interface_ce);
ZEND_API bool ZEND_FASTCALL instanceof_function_slow(const zend_class_entry *instance_ce, const zend_class_entry *ce);

static zend_always_inline bool instanceof_function(
		const zend_class_entry *instance_ce, const zend_class_entry *ce) {
	return instance_ce == ce || instanceof_function_slow(instance_ce, ce);
}

ZEND_API bool zend_string_only_has_ascii_alphanumeric(const zend_string *str);

/**
 * Checks whether the string "str" with length "length" is numeric. The value
 * of allow_errors determines whether it's required to be entirely numeric, or
 * just its prefix. Leading whitespace is allowed.
 *
 * The function returns 0 if the string did not contain a valid number; IS_LONG
 * if it contained a number that fits within the range of a long; or IS_DOUBLE
 * if the number was out of long range or contained a decimal point/exponent.
 * The number's value is returned into the respective pointer, *lval or *dval,
 * if that pointer is not NULL.
 *
 * This variant also gives information if a string that represents an integer
 * could not be represented as such due to overflow. It writes 1 to oflow_info
 * if the integer is larger than ZEND_LONG_MAX and -1 if it's smaller than ZEND_LONG_MIN.
 */
ZEND_API uint8_t ZEND_FASTCALL _is_numeric_string_ex(const char *str, size_t length, zend_long *lval,
	double *dval, bool allow_errors, int *oflow_info, bool *trailing_data);

ZEND_API const char* ZEND_FASTCALL zend_memnstr_ex(const char *haystack, const char *needle, size_t needle_len, const char *end);
ZEND_API const char* ZEND_FASTCALL zend_memnrstr_ex(const char *haystack, const char *needle, size_t needle_len, const char *end);

#if SIZEOF_ZEND_LONG == 4
#	define ZEND_DOUBLE_FITS_LONG(d) (!((d) > (double)ZEND_LONG_MAX || (d) < (double)ZEND_LONG_MIN))
#else
	/* >= as (double)ZEND_LONG_MAX is outside signed range */
#	define ZEND_DOUBLE_FITS_LONG(d) (!((d) >= (double)ZEND_LONG_MAX || (d) < (double)ZEND_LONG_MIN))
#endif

ZEND_API zend_long ZEND_FASTCALL zend_dval_to_lval_slow(double d);

static zend_always_inline zend_long zend_dval_to_lval(double d)
{
	if (UNEXPECTED(!zend_finite(d)) || UNEXPECTED(zend_isnan(d))) {
		return 0;
	} else if (!ZEND_DOUBLE_FITS_LONG(d)) {
		return zend_dval_to_lval_slow(d);
	}
	return (zend_long)d;
}

/* Used to convert a string float to integer during an (int) cast */
static zend_always_inline zend_long zend_dval_to_lval_cap(double d)
{
	if (UNEXPECTED(!zend_finite(d)) || UNEXPECTED(zend_isnan(d))) {
		return 0;
	} else if (!ZEND_DOUBLE_FITS_LONG(d)) {
		return (d > 0 ? ZEND_LONG_MAX : ZEND_LONG_MIN);
	}
	return (zend_long)d;
}
/* }}} */

static zend_always_inline bool zend_is_long_compatible(double d, zend_long l) {
	return (double)l == d;
}

ZEND_API void zend_incompatible_double_to_long_error(double d);
ZEND_API void zend_incompatible_string_to_long_error(const zend_string *s);

static zend_always_inline zend_long zend_dval_to_lval_safe(double d)
{
	zend_long l = zend_dval_to_lval(d);
	if (!zend_is_long_compatible(d, l)) {
		zend_incompatible_double_to_long_error(d);
	}
	return l;
}

#define ZEND_IS_DIGIT(c) ((c) >= '0' && (c) <= '9')
#define ZEND_IS_XDIGIT(c) (((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))

static zend_always_inline uint8_t is_numeric_string_ex(const char *str, size_t length, zend_long *lval,
	double *dval, bool allow_errors, int *oflow_info, bool *trailing_data)
{
	if (*str > '9') {
		return 0;
	}
	return _is_numeric_string_ex(str, length, lval, dval, allow_errors, oflow_info, trailing_data);
}

static zend_always_inline uint8_t is_numeric_string(const char *str, size_t length, zend_long *lval, double *dval, bool allow_errors) {
    return is_numeric_string_ex(str, length, lval, dval, allow_errors, NULL, NULL);
}

ZEND_API uint8_t ZEND_FASTCALL is_numeric_str_function(const zend_string *str, zend_long *lval, double *dval);

static zend_always_inline const char *
zend_memnstr(const char *haystack, const char *needle, size_t needle_len, const char *end)
{
	const char *p = haystack;
	size_t off_s;

	ZEND_ASSERT(end >= p);

	if (needle_len == 1) {
		return (const char *)memchr(p, *needle, (end-p));
	} else if (UNEXPECTED(needle_len == 0)) {
		return p;
	}

	off_s = (size_t)(end - p);

	if (needle_len > off_s) {
		return NULL;
	}

	if (EXPECTED(off_s < 1024 || needle_len < 9)) {	/* glibc memchr is faster when needle is too short */
		const char ne = needle[needle_len-1];
		end -= needle_len;

		while (p <= end) {
			if ((p = (const char *)memchr(p, *needle, (end-p+1)))) {
				if (ne == p[needle_len-1] && !memcmp(needle+1, p+1, needle_len-2)) {
					return p;
				}
			} else {
				return NULL;
			}
			p++;
		}

		return NULL;
	} else {
		return zend_memnstr_ex(haystack, needle, needle_len, end);
	}
}

static zend_always_inline const void *zend_memrchr(const void *s, int c, size_t n)
{
#if defined(HAVE_MEMRCHR) && !defined(i386)
	/* On x86 memrchr() doesn't use SSE/AVX, so inlined version is faster */
	return (const void*)memrchr(s, c, n);
#else
	const unsigned char *e;
	if (0 == n) {
		return NULL;
	}

	for (e = (const unsigned char *)s + n - 1; e >= (const unsigned char *)s; e--) {
		if (*e == (unsigned char)c) {
			return (const void *)e;
		}
	}
	return NULL;
#endif
}


static zend_always_inline const char *
zend_memnrstr(const char *haystack, const char *needle, size_t needle_len, const char *end)
{
    const char *p = end;
    ptrdiff_t off_p;
    size_t off_s;

	if (needle_len == 0) {
		return p;
	}

    if (needle_len == 1) {
        return (const char *)zend_memrchr(haystack, *needle, (p - haystack));
    }

    off_p = end - haystack;
    off_s = (off_p > 0) ? (size_t)off_p : 0;

    if (needle_len > off_s) {
        return NULL;
    }

	if (EXPECTED(off_s < 1024 || needle_len < 3)) {
		const char ne = needle[needle_len-1];
		p -= needle_len;

		do {
			p = (const char *)zend_memrchr(haystack, *needle, (p - haystack) + 1);
			if (!p) {
				return NULL;
			}
			if (ne == p[needle_len-1] && !memcmp(needle + 1, p + 1, needle_len - 2)) {
				return p;
			}
		} while (p-- >= haystack);

		return NULL;
	} else {
		return zend_memnrstr_ex(haystack, needle, needle_len, end);
	}
}

static zend_always_inline size_t zend_strnlen(const char* s, size_t maxlen)
{
#if defined(HAVE_STRNLEN)
	return strnlen(s, maxlen);
#else
	const char *p = (const char *)memchr(s, '\0', maxlen);
	return p ? p-s : maxlen;
#endif
}

ZEND_API zend_result ZEND_FASTCALL increment_function(zval *op1);
ZEND_API zend_result ZEND_FASTCALL decrement_function(zval *op2);

ZEND_API void ZEND_FASTCALL convert_scalar_to_number(zval *op);
ZEND_API void ZEND_FASTCALL _convert_to_string(zval *op);
ZEND_API void ZEND_FASTCALL convert_to_long(zval *op);
ZEND_API void ZEND_FASTCALL convert_to_double(zval *op);
ZEND_API void ZEND_FASTCALL convert_to_null(zval *op);
ZEND_API void ZEND_FASTCALL convert_to_boolean(zval *op);
ZEND_API void ZEND_FASTCALL convert_to_array(zval *op);
ZEND_API void ZEND_FASTCALL convert_to_object(zval *op);

ZEND_API zend_long    ZEND_FASTCALL zval_get_long_func(const zval *op, bool is_strict);
ZEND_API zend_long    ZEND_FASTCALL zval_try_get_long(const zval *op, bool *failed);
ZEND_API double       ZEND_FASTCALL zval_get_double_func(const zval *op);
ZEND_API zend_string* ZEND_FASTCALL zval_get_string_func(zval *op);
ZEND_API zend_string* ZEND_FASTCALL zval_try_get_string_func(zval *op);

static zend_always_inline zend_long zval_get_long(const zval *op) {
	return EXPECTED(Z_TYPE_P(op) == IS_LONG) ? Z_LVAL_P(op) : zval_get_long_func(op, false);
}
static zend_always_inline zend_long zval_get_long_ex(const zval *op, bool is_strict) {
	return EXPECTED(Z_TYPE_P(op) == IS_LONG) ? Z_LVAL_P(op) : zval_get_long_func(op, is_strict);
}
static zend_always_inline double zval_get_double(const zval *op) {
	return EXPECTED(Z_TYPE_P(op) == IS_DOUBLE) ? Z_DVAL_P(op) : zval_get_double_func(op);
}
static zend_always_inline zend_string *zval_get_string(zval *op) {
	return EXPECTED(Z_TYPE_P(op) == IS_STRING) ? zend_string_copy(Z_STR_P(op)) : zval_get_string_func(op);
}

static zend_always_inline zend_string *zval_get_tmp_string(zval *op, zend_string **tmp) {
	if (EXPECTED(Z_TYPE_P(op) == IS_STRING)) {
		*tmp = NULL;
		return Z_STR_P(op);
	} else {
		return *tmp = zval_get_string_func(op);
	}
}
static zend_always_inline void zend_tmp_string_release(zend_string *tmp) {
	if (UNEXPECTED(tmp)) {
		zend_string_release_ex(tmp, 0);
	}
}

/* Like zval_get_string, but returns NULL if the conversion fails with an exception. */
static zend_always_inline zend_string *zval_try_get_string(zval *op) {
	if (EXPECTED(Z_TYPE_P(op) == IS_STRING)) {
		zend_string *ret = zend_string_copy(Z_STR_P(op));
		ZEND_ASSUME(ret != NULL);
		return ret;
	} else {
		return zval_try_get_string_func(op);
	}
}

/* Like zval_get_tmp_string, but returns NULL if the conversion fails with an exception. */
static zend_always_inline zend_string *zval_try_get_tmp_string(zval *op, zend_string **tmp) {
	if (EXPECTED(Z_TYPE_P(op) == IS_STRING)) {
		zend_string *ret = Z_STR_P(op);
		*tmp = NULL;
		ZEND_ASSUME(ret != NULL);
		return ret;
	} else {
		return *tmp = zval_try_get_string_func(op);
	}
}

/* Like convert_to_string(), but returns whether the conversion succeeded and does not modify the
 * zval in-place if it fails. */
ZEND_API bool ZEND_FASTCALL _try_convert_to_string(zval *op);
static zend_always_inline bool try_convert_to_string(zval *op) {
	if (Z_TYPE_P(op) == IS_STRING) {
		return 1;
	}
	return _try_convert_to_string(op);
}

/* Compatibility macros for 7.2 and below */
#define _zval_get_long(op) zval_get_long(op)
#define _zval_get_double(op) zval_get_double(op)
#define _zval_get_string(op) zval_get_string(op)
#define _zval_get_long_func(op) zval_get_long_func(op)
#define _zval_get_double_func(op) zval_get_double_func(op)
#define _zval_get_string_func(op) zval_get_string_func(op)

#define convert_to_string(op) if (Z_TYPE_P(op) != IS_STRING) { _convert_to_string((op)); }


ZEND_API int ZEND_FASTCALL zend_is_true(const zval *op);
ZEND_API bool ZEND_FASTCALL zend_object_is_true(const zval *op);

#define zval_is_true(op) \
	zend_is_true(op)

static zend_always_inline bool i_zend_is_true(const zval *op)
{
	bool result = 0;

again:
	switch (Z_TYPE_P(op)) {
		case IS_TRUE:
			result = 1;
			break;
		case IS_LONG:
			if (Z_LVAL_P(op)) {
				result = 1;
			}
			break;
		case IS_DOUBLE:
			if (Z_DVAL_P(op)) {
				result = 1;
			}
			break;
		case IS_STRING:
			if (Z_STRLEN_P(op) > 1 || (Z_STRLEN_P(op) && Z_STRVAL_P(op)[0] != '0')) {
				result = 1;
			}
			break;
		case IS_ARRAY:
			if (zend_hash_num_elements(Z_ARRVAL_P(op))) {
				result = 1;
			}
			break;
		case IS_OBJECT:
			if (EXPECTED(Z_OBJ_HT_P(op)->cast_object == zend_std_cast_object_tostring)) {
				result = 1;
			} else {
				result = zend_object_is_true(op);
			}
			break;
		case IS_RESOURCE:
			if (EXPECTED(Z_RES_HANDLE_P(op))) {
				result = 1;
			}
			break;
		case IS_REFERENCE:
			op = Z_REFVAL_P(op);
			goto again;
			break;
		default:
			break;
	}
	return result;
}

/* Indicate that two values cannot be compared. This value should be returned for both orderings
 * of the operands. This implies that all of ==, <, <= and >, >= will return false, because we
 * canonicalize >/>= to </<= with swapped operands. */
// TODO: Use a different value to allow an actual distinction here.
#define ZEND_UNCOMPARABLE 1

ZEND_API int ZEND_FASTCALL zend_compare(zval *op1, zval *op2);

ZEND_API zend_result ZEND_FASTCALL compare_function(zval *result, zval *op1, zval *op2);

ZEND_API int ZEND_FASTCALL numeric_compare_function(zval *op1, zval *op2);
ZEND_API int ZEND_FASTCALL string_compare_function_ex(zval *op1, zval *op2, bool case_insensitive);
ZEND_API int ZEND_FASTCALL string_compare_function(zval *op1, zval *op2);
ZEND_API int ZEND_FASTCALL string_case_compare_function(zval *op1, zval *op2);
ZEND_API int ZEND_FASTCALL string_locale_compare_function(zval *op1, zval *op2);

ZEND_API extern const unsigned char zend_tolower_map[256];
ZEND_API extern const unsigned char zend_toupper_map[256];

#define zend_tolower_ascii(c) (zend_tolower_map[(unsigned char)(c)])
#define zend_toupper_ascii(c) (zend_toupper_map[(unsigned char)(c)])

ZEND_API void         ZEND_FASTCALL zend_str_tolower(char *str, size_t length);
ZEND_API void         ZEND_FASTCALL zend_str_toupper(char *str, size_t length);
ZEND_API char*        ZEND_FASTCALL zend_str_tolower_copy(char *dest, const char *source, size_t length);
ZEND_API char*        ZEND_FASTCALL zend_str_toupper_copy(char *dest, const char *source, size_t length);
ZEND_API char*        ZEND_FASTCALL zend_str_tolower_dup(const char *source, size_t length);
ZEND_API char*        ZEND_FASTCALL zend_str_toupper_dup(const char *source, size_t length);
ZEND_API char*        ZEND_FASTCALL zend_str_tolower_dup_ex(const char *source, size_t length);
ZEND_API char*        ZEND_FASTCALL zend_str_toupper_dup_ex(const char *source, size_t length);
ZEND_API zend_string* ZEND_FASTCALL zend_string_tolower_ex(zend_string *str, bool persistent);
ZEND_API zend_string* ZEND_FASTCALL zend_string_toupper_ex(zend_string *str, bool persistent);

static zend_always_inline zend_string* zend_string_tolower(zend_string *str) {
	return zend_string_tolower_ex(str, false);
}
static zend_always_inline zend_string* zend_string_toupper(zend_string *str) {
	return zend_string_toupper_ex(str, false);
}

ZEND_API int ZEND_FASTCALL zend_binary_zval_strcmp(zval *s1, zval *s2);
ZEND_API int ZEND_FASTCALL zend_binary_zval_strncmp(zval *s1, zval *s2, zval *s3);
ZEND_API int ZEND_FASTCALL zend_binary_strcmp(const char *s1, size_t len1, const char *s2, size_t len2);
ZEND_API int ZEND_FASTCALL zend_binary_strncmp(const char *s1, size_t len1, const char *s2, size_t len2, size_t length);
ZEND_API int ZEND_FASTCALL zend_binary_strcasecmp(const char *s1, size_t len1, const char *s2, size_t len2);
ZEND_API int ZEND_FASTCALL zend_binary_strncasecmp(const char *s1, size_t len1, const char *s2, size_t len2, size_t length);
ZEND_API int ZEND_FASTCALL zend_binary_strcasecmp_l(const char *s1, size_t len1, const char *s2, size_t len2);
ZEND_API int ZEND_FASTCALL zend_binary_strncasecmp_l(const char *s1, size_t len1, const char *s2, size_t len2, size_t length);

ZEND_API bool ZEND_FASTCALL zendi_smart_streq(zend_string *s1, zend_string *s2);
ZEND_API int ZEND_FASTCALL zendi_smart_strcmp(zend_string *s1, zend_string *s2);
ZEND_API int ZEND_FASTCALL zend_compare_symbol_tables(HashTable *ht1, HashTable *ht2);
ZEND_API int ZEND_FASTCALL zend_compare_arrays(zval *a1, zval *a2);
ZEND_API int ZEND_FASTCALL zend_compare_objects(zval *o1, zval *o2);

/** Deprecatd in favor of ZEND_STRTOL() */
ZEND_ATTRIBUTE_DEPRECATED ZEND_API int ZEND_FASTCALL zend_atoi(const char *str, size_t str_len);

/** Deprecatd in favor of ZEND_STRTOL() */
ZEND_ATTRIBUTE_DEPRECATED ZEND_API zend_long ZEND_FASTCALL zend_atol(const char *str, size_t str_len);

#define convert_to_null_ex(zv) convert_to_null(zv)
#define convert_to_boolean_ex(zv) convert_to_boolean(zv)
#define convert_to_long_ex(zv) convert_to_long(zv)
#define convert_to_double_ex(zv) convert_to_double(zv)
#define convert_to_string_ex(zv) convert_to_string(zv)
#define convert_to_array_ex(zv) convert_to_array(zv)
#define convert_to_object_ex(zv) convert_to_object(zv)
#define convert_scalar_to_number_ex(zv) convert_scalar_to_number(zv)

ZEND_API void zend_update_current_locale(void);

ZEND_API void zend_reset_lc_ctype_locale(void);

/* The offset in bytes between the value and type fields of a zval */
#define ZVAL_OFFSETOF_TYPE	\
	(offsetof(zval, u1.type_info) - offsetof(zval, value))

#if defined(HAVE_ASM_GOTO) && !__has_feature(memory_sanitizer)
# define ZEND_USE_ASM_ARITHMETIC 1
#else
# define ZEND_USE_ASM_ARITHMETIC 0
#endif

static zend_always_inline void fast_long_increment_function(zval *op1)
{
#if ZEND_USE_ASM_ARITHMETIC && defined(__i386__) && !(4 == __GNUC__ && 8 == __GNUC_MINOR__)
	__asm__ goto(
		"addl $1,(%0)\n\t"
		"jo  %l1\n"
		:
		: "r"(&op1->value)
		: "cc", "memory"
		: overflow);
	return;
overflow: ZEND_ATTRIBUTE_COLD_LABEL
	ZVAL_DOUBLE(op1, (double)ZEND_LONG_MAX + 1.0);
#elif ZEND_USE_ASM_ARITHMETIC && defined(__x86_64__)
	__asm__ goto(
		"addq $1,(%0)\n\t"
		"jo  %l1\n"
		:
		: "r"(&op1->value)
		: "cc", "memory"
		: overflow);
	return;
overflow: ZEND_ATTRIBUTE_COLD_LABEL
	ZVAL_DOUBLE(op1, (double)ZEND_LONG_MAX + 1.0);
#elif ZEND_USE_ASM_ARITHMETIC && defined(__aarch64__)
	__asm__ goto (
		"ldr x5, [%0]\n\t"
		"adds x5, x5, 1\n\t"
		"bvs %l1\n"
		"str x5, [%0]"
		:
		: "r"(&op1->value)
		: "x5", "cc", "memory"
		: overflow);
	return;
overflow: ZEND_ATTRIBUTE_COLD_LABEL
	ZVAL_DOUBLE(op1, (double)ZEND_LONG_MAX + 1.0);
#elif PHP_HAVE_BUILTIN_SADDL_OVERFLOW && SIZEOF_LONG == SIZEOF_ZEND_LONG
	long lresult;
	if (UNEXPECTED(__builtin_saddl_overflow(Z_LVAL_P(op1), 1, &lresult))) {
		/* switch to double */
		ZVAL_DOUBLE(op1, (double)ZEND_LONG_MAX + 1.0);
	} else {
		Z_LVAL_P(op1) = lresult;
	}
#elif PHP_HAVE_BUILTIN_SADDLL_OVERFLOW && SIZEOF_LONG_LONG == SIZEOF_ZEND_LONG
	long long llresult;
	if (UNEXPECTED(__builtin_saddll_overflow(Z_LVAL_P(op1), 1, &llresult))) {
		/* switch to double */
		ZVAL_DOUBLE(op1, (double)ZEND_LONG_MAX + 1.0);
	} else {
		Z_LVAL_P(op1) = llresult;
	}
#else
	if (UNEXPECTED(Z_LVAL_P(op1) == ZEND_LONG_MAX)) {
		/* switch to double */
		ZVAL_DOUBLE(op1, (double)ZEND_LONG_MAX + 1.0);
	} else {
		Z_LVAL_P(op1)++;
	}
#endif
}

static zend_always_inline void fast_long_decrement_function(zval *op1)
{
#if ZEND_USE_ASM_ARITHMETIC && defined(__i386__) && !(4 == __GNUC__ && 8 == __GNUC_MINOR__)
	__asm__ goto(
		"subl $1,(%0)\n\t"
		"jo  %l1\n"
		:
		: "r"(&op1->value)
		: "cc", "memory"
		: overflow);
	return;
overflow: ZEND_ATTRIBUTE_COLD_LABEL
	ZVAL_DOUBLE(op1, (double)ZEND_LONG_MIN - 1.0);
#elif ZEND_USE_ASM_ARITHMETIC && defined(__x86_64__)
	__asm__ goto(
		"subq $1,(%0)\n\t"
		"jo  %l1\n"
		:
		: "r"(&op1->value)
		: "cc", "memory"
		: overflow);
	return;
overflow: ZEND_ATTRIBUTE_COLD_LABEL
	ZVAL_DOUBLE(op1, (double)ZEND_LONG_MIN - 1.0);
#elif ZEND_USE_ASM_ARITHMETIC && defined(__aarch64__)
	__asm__ goto (
		"ldr x5, [%0]\n\t"
		"subs x5 ,x5, 1\n\t"
		"bvs %l1\n"
		"str x5, [%0]"
		:
		: "r"(&op1->value)
		: "x5", "cc", "memory"
		: overflow);
	return;
overflow: ZEND_ATTRIBUTE_COLD_LABEL
	ZVAL_DOUBLE(op1, (double)ZEND_LONG_MIN - 1.0);
#elif PHP_HAVE_BUILTIN_SSUBL_OVERFLOW && SIZEOF_LONG == SIZEOF_ZEND_LONG
	long lresult;
	if (UNEXPECTED(__builtin_ssubl_overflow(Z_LVAL_P(op1), 1, &lresult))) {
		/* switch to double */
		ZVAL_DOUBLE(op1, (double)ZEND_LONG_MIN - 1.0);
	} else {
		Z_LVAL_P(op1) = lresult;
	}
#elif PHP_HAVE_BUILTIN_SSUBLL_OVERFLOW && SIZEOF_LONG_LONG == SIZEOF_ZEND_LONG
	long long llresult;
	if (UNEXPECTED(__builtin_ssubll_overflow(Z_LVAL_P(op1), 1, &llresult))) {
		/* switch to double */
		ZVAL_DOUBLE(op1, (double)ZEND_LONG_MIN - 1.0);
	} else {
		Z_LVAL_P(op1) = llresult;
	}
#else
	if (UNEXPECTED(Z_LVAL_P(op1) == ZEND_LONG_MIN)) {
		/* switch to double */
		ZVAL_DOUBLE(op1, (double)ZEND_LONG_MIN - 1.0);
	} else {
		Z_LVAL_P(op1)--;
	}
#endif
}

static zend_always_inline void fast_long_add_function(zval *result, zval *op1, zval *op2)
{
#if ZEND_USE_ASM_ARITHMETIC && defined(__i386__) && !(4 == __GNUC__ && 8 == __GNUC_MINOR__)
	__asm__ goto(
		"movl	(%1), %%eax\n\t"
		"addl   (%2), %%eax\n\t"
		"jo     %l5\n\t"
		"movl   %%eax, (%0)\n\t"
		"movl   %3, %c4(%0)\n"
		:
		: "r"(&result->value),
		  "r"(&op1->value),
		  "r"(&op2->value),
		  "n"(IS_LONG),
		  "n"(ZVAL_OFFSETOF_TYPE)
		: "eax","cc", "memory"
		: overflow);
	return;
overflow: ZEND_ATTRIBUTE_COLD_LABEL
	ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) + (double) Z_LVAL_P(op2));
#elif ZEND_USE_ASM_ARITHMETIC && defined(__x86_64__)
	__asm__ goto(
		"movq	(%1), %%rax\n\t"
		"addq   (%2), %%rax\n\t"
		"jo     %l5\n\t"
		"movq   %%rax, (%0)\n\t"
		"movl   %3, %c4(%0)\n"
		:
		: "r"(&result->value),
		  "r"(&op1->value),
		  "r"(&op2->value),
		  "n"(IS_LONG),
		  "n"(ZVAL_OFFSETOF_TYPE)
		: "rax","cc", "memory"
		: overflow);
	return;
overflow: ZEND_ATTRIBUTE_COLD_LABEL
	ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) + (double) Z_LVAL_P(op2));
#elif ZEND_USE_ASM_ARITHMETIC && defined(__aarch64__)
	__asm__ goto(
		"ldr    x5, [%1]\n\t"
		"ldr    x6, [%2]\n\t"
		"adds	x5, x5, x6\n\t"
		"bvs	%l5\n\t"
		"mov	w6, %3\n\t"
		"str	x5, [%0]\n\t"
		"str	w6, [%0, %c4]\n"
		:
		: "r"(&result->value),
		  "r"(&op1->value),
		  "r"(&op2->value),
		  "n"(IS_LONG),
		  "n"(ZVAL_OFFSETOF_TYPE)
		: "x5", "x6", "cc", "memory"
		: overflow);
	return;
overflow: ZEND_ATTRIBUTE_COLD_LABEL
	ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) + (double) Z_LVAL_P(op2));
#elif PHP_HAVE_BUILTIN_SADDL_OVERFLOW && SIZEOF_LONG == SIZEOF_ZEND_LONG
	long lresult;
	if (UNEXPECTED(__builtin_saddl_overflow(Z_LVAL_P(op1), Z_LVAL_P(op2), &lresult))) {
		ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) + (double) Z_LVAL_P(op2));
	} else {
		ZVAL_LONG(result, lresult);
	}
#elif PHP_HAVE_BUILTIN_SADDLL_OVERFLOW && SIZEOF_LONG_LONG == SIZEOF_ZEND_LONG
	long long llresult;
	if (UNEXPECTED(__builtin_saddll_overflow(Z_LVAL_P(op1), Z_LVAL_P(op2), &llresult))) {
		ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) + (double) Z_LVAL_P(op2));
	} else {
		ZVAL_LONG(result, llresult);
	}
#else
	/*
	 * 'result' may alias with op1 or op2, so we need to
	 * ensure that 'result' is not updated until after we
	 * have read the values of op1 and op2.
	 */

	if (UNEXPECTED((Z_LVAL_P(op1) & LONG_SIGN_MASK) == (Z_LVAL_P(op2) & LONG_SIGN_MASK)
		&& (Z_LVAL_P(op1) & LONG_SIGN_MASK) != ((Z_LVAL_P(op1) + Z_LVAL_P(op2)) & LONG_SIGN_MASK))) {
		ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) + (double) Z_LVAL_P(op2));
	} else {
		ZVAL_LONG(result, Z_LVAL_P(op1) + Z_LVAL_P(op2));
	}
#endif
}

static zend_always_inline void fast_long_sub_function(zval *result, zval *op1, zval *op2)
{
#if ZEND_USE_ASM_ARITHMETIC && defined(__i386__) && !(4 == __GNUC__ && 8 == __GNUC_MINOR__)
	__asm__ goto(
		"movl	(%1), %%eax\n\t"
		"subl   (%2), %%eax\n\t"
		"jo     %l5\n\t"
		"movl   %%eax, (%0)\n\t"
		"movl   %3, %c4(%0)\n"
		:
		: "r"(&result->value),
		  "r"(&op1->value),
		  "r"(&op2->value),
		  "n"(IS_LONG),
		  "n"(ZVAL_OFFSETOF_TYPE)
		: "eax","cc", "memory"
		: overflow);
	return;
overflow: ZEND_ATTRIBUTE_COLD_LABEL
	ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) - (double) Z_LVAL_P(op2));
#elif ZEND_USE_ASM_ARITHMETIC && defined(__x86_64__)
	__asm__ goto(
		"movq	(%1), %%rax\n\t"
		"subq   (%2), %%rax\n\t"
		"jo     %l5\n\t"
		"movq   %%rax, (%0)\n\t"
		"movl   %3, %c4(%0)\n"
		:
		: "r"(&result->value),
		  "r"(&op1->value),
		  "r"(&op2->value),
		  "n"(IS_LONG),
		  "n"(ZVAL_OFFSETOF_TYPE)
		: "rax","cc", "memory"
		: overflow);
	return;
overflow: ZEND_ATTRIBUTE_COLD_LABEL
	ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) - (double) Z_LVAL_P(op2));
#elif ZEND_USE_ASM_ARITHMETIC && defined(__aarch64__)
	__asm__ goto(
		"ldr    x5, [%1]\n\t"
		"ldr    x6, [%2]\n\t"
		"subs	x5, x5, x6\n\t"
		"bvs	%l5\n\t"
		"mov	w6, %3\n\t"
		"str	x5, [%0]\n\t"
		"str	w6, [%0, %c4]\n"
		:
		: "r"(&result->value),
		  "r"(&op1->value),
		  "r"(&op2->value),
		  "n"(IS_LONG),
		  "n"(ZVAL_OFFSETOF_TYPE)
		: "x5", "x6", "cc", "memory"
		: overflow);
	return;
overflow: ZEND_ATTRIBUTE_COLD_LABEL
	ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) - (double) Z_LVAL_P(op2));
#elif PHP_HAVE_BUILTIN_SSUBL_OVERFLOW && SIZEOF_LONG == SIZEOF_ZEND_LONG
	long lresult;
	if (UNEXPECTED(__builtin_ssubl_overflow(Z_LVAL_P(op1), Z_LVAL_P(op2), &lresult))) {
		ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) - (double) Z_LVAL_P(op2));
	} else {
		ZVAL_LONG(result, lresult);
	}
#elif PHP_HAVE_BUILTIN_SSUBLL_OVERFLOW && SIZEOF_LONG_LONG == SIZEOF_ZEND_LONG
	long long llresult;
	if (UNEXPECTED(__builtin_ssubll_overflow(Z_LVAL_P(op1), Z_LVAL_P(op2), &llresult))) {
		ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) - (double) Z_LVAL_P(op2));
	} else {
		ZVAL_LONG(result, llresult);
	}
#else
	ZVAL_LONG(result, Z_LVAL_P(op1) - Z_LVAL_P(op2));

	if (UNEXPECTED((Z_LVAL_P(op1) & LONG_SIGN_MASK) != (Z_LVAL_P(op2) & LONG_SIGN_MASK)
		&& (Z_LVAL_P(op1) & LONG_SIGN_MASK) != (Z_LVAL_P(result) & LONG_SIGN_MASK))) {
		ZVAL_DOUBLE(result, (double) Z_LVAL_P(op1) - (double) Z_LVAL_P(op2));
	}
#endif
}

static zend_always_inline bool zend_fast_equal_strings(zend_string *s1, zend_string *s2)
{
	if (s1 == s2) {
		return 1;
	} else if (ZSTR_VAL(s1)[0] > '9' || ZSTR_VAL(s2)[0] > '9') {
		return zend_string_equal_content(s1, s2);
	} else {
		return zendi_smart_streq(s1, s2);
	}
}

static zend_always_inline bool fast_equal_check_function(zval *op1, zval *op2)
{
	if (EXPECTED(Z_TYPE_P(op1) == IS_LONG)) {
		if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
			return Z_LVAL_P(op1) == Z_LVAL_P(op2);
		} else if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
			return ((double)Z_LVAL_P(op1)) == Z_DVAL_P(op2);
		}
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_DOUBLE)) {
		if (EXPECTED(Z_TYPE_P(op2) == IS_DOUBLE)) {
			return Z_DVAL_P(op1) == Z_DVAL_P(op2);
		} else if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
			return Z_DVAL_P(op1) == ((double)Z_LVAL_P(op2));
		}
	} else if (EXPECTED(Z_TYPE_P(op1) == IS_STRING)) {
		if (EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
			return zend_fast_equal_strings(Z_STR_P(op1), Z_STR_P(op2));
		}
	}
	return zend_compare(op1, op2) == 0;
}

static zend_always_inline bool fast_equal_check_long(zval *op1, zval *op2)
{
	if (EXPECTED(Z_TYPE_P(op2) == IS_LONG)) {
		return Z_LVAL_P(op1) == Z_LVAL_P(op2);
	}
	return zend_compare(op1, op2) == 0;
}

static zend_always_inline bool fast_equal_check_string(zval *op1, zval *op2)
{
	if (EXPECTED(Z_TYPE_P(op2) == IS_STRING)) {
		return zend_fast_equal_strings(Z_STR_P(op1), Z_STR_P(op2));
	}
	return zend_compare(op1, op2) == 0;
}

static zend_always_inline bool fast_is_identical_function(zval *op1, zval *op2)
{
	if (Z_TYPE_P(op1) != Z_TYPE_P(op2)) {
		return 0;
	} else if (Z_TYPE_P(op1) <= IS_TRUE) {
		return 1;
	}
	return zend_is_identical(op1, op2);
}

static zend_always_inline bool fast_is_not_identical_function(zval *op1, zval *op2)
{
	if (Z_TYPE_P(op1) != Z_TYPE_P(op2)) {
		return 1;
	} else if (Z_TYPE_P(op1) <= IS_TRUE) {
		return 0;
	}
	return !zend_is_identical(op1, op2);
}

/* buf points to the END of the buffer */
static zend_always_inline char *zend_print_ulong_to_buf(char *buf, zend_ulong num) {
	*buf = '\0';
	do {
		*--buf = (char) (num % 10) + '0';
		num /= 10;
	} while (num > 0);
	return buf;
}

/* buf points to the END of the buffer */
static zend_always_inline char *zend_print_long_to_buf(char *buf, zend_long num) {
	if (num < 0) {
	    char *result = zend_print_ulong_to_buf(buf, ~((zend_ulong) num) + 1);
	    *--result = '-';
		return result;
	} else {
	    return zend_print_ulong_to_buf(buf, num);
	}
}

ZEND_API zend_string* ZEND_FASTCALL zend_long_to_str(zend_long num);
ZEND_API zend_string* ZEND_FASTCALL zend_ulong_to_str(zend_ulong num);
ZEND_API zend_string* ZEND_FASTCALL zend_u64_to_str(uint64_t num);
ZEND_API zend_string* ZEND_FASTCALL zend_i64_to_str(int64_t num);
ZEND_API zend_string* ZEND_FASTCALL zend_double_to_str(double num);

static zend_always_inline void zend_unwrap_reference(zval *op) /* {{{ */
{
	if (Z_REFCOUNT_P(op) == 1) {
		ZVAL_UNREF(op);
	} else {
		Z_DELREF_P(op);
		ZVAL_COPY(op, Z_REFVAL_P(op));
	}
}
/* }}} */

static zend_always_inline bool zend_strnieq(const char *ptr1, const char *ptr2, size_t num)
{
	const char *end = ptr1 + num;
	while (ptr1 < end) {
		if (zend_tolower_ascii(*ptr1++) != zend_tolower_ascii(*ptr2++)) {
			return 0;
		}
	}
	return 1;
}

static zend_always_inline const char *
zend_memnistr(const char *haystack, const char *needle, size_t needle_len, const char *end)
{
	ZEND_ASSERT(end >= haystack);

	if (UNEXPECTED(needle_len == 0)) {
		return haystack;
	}

	if (UNEXPECTED(needle_len > (size_t)(end - haystack))) {
		return NULL;
	}

	const char first_lower = zend_tolower_ascii(*needle);
	const char first_upper = zend_toupper_ascii(*needle);
	const char *p_lower = (const char *)memchr(haystack, first_lower, end - haystack);
	const char *p_upper = NULL;
	if (first_lower != first_upper) {
		// If the needle length is 1 we don't need to look beyond p_lower as it is a guaranteed match
		size_t upper_search_length = needle_len == 1 && p_lower != NULL ? p_lower - haystack : end - haystack;
		p_upper = (const char *)memchr(haystack, first_upper, upper_search_length);
	}
	const char *p = !p_upper || (p_lower && p_lower < p_upper) ? p_lower : p_upper;

	if (needle_len == 1) {
		return p;
	}

	const char needle_end_lower = zend_tolower_ascii(needle[needle_len - 1]);
	const char needle_end_upper = zend_toupper_ascii(needle[needle_len - 1]);
	end -= needle_len;

	while (p && p <= end) {
		if (needle_end_lower == p[needle_len - 1] || needle_end_upper == p[needle_len - 1]) {
			if (zend_strnieq(needle + 1, p + 1, needle_len - 2)) {
				return p;
			}
		}
		if (p_lower == p) {
			p_lower = (const char *)memchr(p_lower + 1, first_lower, end - p_lower);
		}
		if (p_upper == p) {
			p_upper = (const char *)memchr(p_upper + 1, first_upper, end - p_upper);
		}
		p = !p_upper || (p_lower && p_lower < p_upper) ? p_lower : p_upper;
	}

	return NULL;
}


END_EXTERN_C()

#endif
