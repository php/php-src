/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2009 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_OPERATORS_H
#define ZEND_OPERATORS_H

#include <errno.h>
#include <math.h>

#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif

#include "zend_strtod.h"
#include "zend_unicode.h"

#if 0&&HAVE_BCMATH
#include "ext/bcmath/libbcmath/src/bcmath.h"
#endif

#if SIZEOF_LONG == 4
#define MAX_LENGTH_OF_LONG 11
static const char long_min_digits[] = "2147483648";
#elif SIZEOF_LONG == 8
#define MAX_LENGTH_OF_LONG 20
static const char long_min_digits[] = "9223372036854775808";
#else
#error "Unknown SIZEOF_LONG"
#endif

#define MAX_LENGTH_OF_DOUBLE 32

BEGIN_EXTERN_C()
ZEND_API int add_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int sub_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int mul_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int div_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int mod_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int boolean_xor_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int boolean_not_function(zval *result, zval *op1 TSRMLS_DC);
ZEND_API int bitwise_not_function(zval *result, zval *op1 TSRMLS_DC);
ZEND_API int bitwise_or_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int bitwise_and_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int bitwise_xor_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int shift_left_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int shift_right_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int concat_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);

ZEND_API int is_equal_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int is_identical_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int is_not_identical_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int is_not_equal_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int is_smaller_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int is_smaller_or_equal_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);

ZEND_API zend_bool instanceof_function_ex(const zend_class_entry *instance_ce, const zend_class_entry *ce, zend_bool interfaces_only TSRMLS_DC);
ZEND_API zend_bool instanceof_function(const zend_class_entry *instance_ce, const zend_class_entry *ce TSRMLS_DC);
ZEND_API long zend_u_strtol(const UChar *nptr, UChar **endptr, int base);
ZEND_API unsigned long zend_u_strtoul(const UChar *nptr, UChar **endptr, int base);
ZEND_API double zend_u_strtod(const UChar *nptr, UChar **endptr);
END_EXTERN_C()

static inline zend_uchar is_numeric_string(char *str, int length, long *lval, double *dval, int allow_errors)
{
	long local_lval;
	double local_dval;
	char *end_ptr_long, *end_ptr_double;
	int conv_base=10;

	if (!length) {
		return 0;
	}

	/* handle hex numbers */
	if (length>=2 && str[0]=='0' && (str[1]=='x' || str[1]=='X')) {
		conv_base=16;
	}
	errno=0;
	local_lval = strtol(str, &end_ptr_long, conv_base);
	if (errno!=ERANGE) {
		if (end_ptr_long == str+length) { /* integer string */
			if (lval) {
				*lval = local_lval;
			}
			return IS_LONG;
		} else if (end_ptr_long == str && *end_ptr_long != '\0' && *str != '.' && *str != '-') { /* ignore partial string matches */
			return 0;
		}
	} else {
		end_ptr_long=NULL;
	}

	if (conv_base==16) { /* hex string, under UNIX strtod() messes it up */
		return 0;
	}

	errno=0;
	local_dval = zend_strtod(str, &end_ptr_double);
	if (errno != ERANGE) {
		if (end_ptr_double == str+length) { /* floating point string */
			if (!zend_finite(local_dval)) {
				/* "inf","nan" and maybe other weird ones */
				return 0;
			}

			if (dval) {
				*dval = local_dval;
			}
			return IS_DOUBLE;
		}
	} else {
		end_ptr_double=NULL;
	}

	if (!allow_errors) {
		return 0;
	}
	if (allow_errors == -1) {
		zend_error(E_NOTICE, "A non well formed numeric value encountered");
	}

	if (end_ptr_double>end_ptr_long && dval) {
		*dval = local_dval;
		return IS_DOUBLE;
	} else if (end_ptr_long && lval) {
		*lval = local_lval;
		return IS_LONG;
	}
	return 0;
}

static inline zend_uchar is_numeric_unicode(UChar *str, int length, long *lval, double *dval, int allow_errors)
{
	long local_lval;
	double local_dval;
	UChar *end_ptr_long, *end_ptr_double;
	int conv_base=10;

	if (!length) {
		return 0;
	}

	/* handle hex numbers */
	if (length>=2 && str[0]=='0' && (str[1]=='x' || str[1]=='X')) {
		conv_base=16;
	}

	errno=0;
	local_lval = zend_u_strtol(str, &end_ptr_long, conv_base);
	if (errno != ERANGE) {
		if (end_ptr_long == str+length) { /* integer string */
			if (lval) {
				*lval = local_lval;
			}
			return IS_LONG;
		} else if (end_ptr_long == str && *end_ptr_long != '\0' && *str != '.' && *str != '-') { /* ignore partial string matches */
			return 0;
		}
	} else {
		end_ptr_long = NULL;
	}

	if (conv_base == 16) { /* hex string, under UNIX strtod() messes it up */
		/* UTODO: keep compatibility with is_numeric_string() here? */
		return 0;
	}

	local_dval = zend_u_strtod(str, &end_ptr_double);
	if (local_dval == 0 && end_ptr_double == str) {
		end_ptr_double = NULL;
	} else {
		if (end_ptr_double == str+length) { /* floating point string */
			if (!zend_finite(local_dval)) {
				/* "inf","nan" and maybe other weird ones */
				return 0;
			}

			if (dval) {
				*dval = local_dval;
			}
			return IS_DOUBLE;
		}
	}

	if (!allow_errors) {
		return 0;
	}
	if (allow_errors == -1) {
		zend_error(E_NOTICE, "A non well formed numeric value encountered");
	}

	if (allow_errors) {
		if (end_ptr_double > end_ptr_long && dval) {
			*dval = local_dval;
			return IS_DOUBLE;
		} else if (end_ptr_long && lval) {
			*lval = local_lval;
			return IS_LONG;
		}
	}
	return 0;
}

static inline UChar*
zend_u_memnstr(UChar *haystack, UChar *needle, int needle_len, UChar *end)
{
	return u_strFindFirst(haystack, end - haystack, needle, needle_len);
}

static inline char *
zend_memnstr(char *haystack, char *needle, int needle_len, char *end)
{
	char *p = haystack;
	char ne = needle[needle_len-1];

	if (needle_len == 1) {
		return (char *)memchr(p, *needle, (end-p));
	}
	if(needle_len > end-haystack) {
		return NULL;
	}

	end -= needle_len;

	while (p <= end) {
		if ((p = (char *)memchr(p, *needle, (end-p+1))) && ne == p[needle_len-1]) {
			if (!memcmp(needle, p, needle_len-1)) {
				return p;
			}
		}

		if (p == NULL) {
			return NULL;
		}

		p++;
	}

	return NULL;
}

static inline void *zend_memrchr(const void *s, int c, size_t n)
{
	register unsigned char *e;

	if (n <= 0) {
		return NULL;
	}

	for (e = (unsigned char *)s + n - 1; e >= (unsigned char *)s; e--) {
		if (*e == (unsigned char)c) {
			return (void *)e;
		}
	}

	return NULL;
}

BEGIN_EXTERN_C()
ZEND_API int increment_function(zval *op1);
ZEND_API int decrement_function(zval *op2);

ZEND_API int convert_scalar_to_number(zval *op TSRMLS_DC);
ZEND_API int _convert_to_string(zval *op ZEND_FILE_LINE_DC);
ZEND_API int _convert_to_string_with_converter(zval *op, UConverter *conv TSRMLS_DC ZEND_FILE_LINE_DC);
ZEND_API int _convert_to_unicode(zval *op TSRMLS_DC ZEND_FILE_LINE_DC);
ZEND_API int _convert_to_unicode_with_converter(zval *op, UConverter *conv TSRMLS_DC ZEND_FILE_LINE_DC);
ZEND_API int convert_to_long(zval *op);
ZEND_API int convert_to_double(zval *op);
ZEND_API int convert_to_long_base(zval *op, int base);
ZEND_API int convert_to_null(zval *op);
ZEND_API int convert_to_boolean(zval *op);
ZEND_API int convert_to_array(zval *op);
ZEND_API int convert_to_object(zval *op);
ZEND_API void multi_convert_to_long_ex(int argc, ...);
ZEND_API void multi_convert_to_double_ex(int argc, ...);
ZEND_API void multi_convert_to_string_ex(int argc, ...);
ZEND_API int add_char_to_string(zval *result, const zval *op1, const zval *op2);
ZEND_API int add_string_to_string(zval *result, const zval *op1, const zval *op2);
#define convert_to_string(op)		     	 _convert_to_string((op) ZEND_FILE_LINE_CC)
#define convert_to_string_with_converter(op, conv) _convert_to_string_with_converter((op), (conv) TSRMLS_CC ZEND_FILE_LINE_CC)
#define convert_to_unicode(op)               _convert_to_unicode((op) TSRMLS_CC ZEND_FILE_LINE_CC)
#define convert_to_unicode_with_converter(op, conv) _convert_to_unicode_with_converter((op), (conv) TSRMLS_CC ZEND_FILE_LINE_CC)
#define convert_to_text(op) (UG(unicode)?convert_to_unicode(op):convert_to_string(op))

ZEND_API double zend_string_to_double(const char *number, zend_uint length);

ZEND_API int zval_is_true(zval *op);
ZEND_API int compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int numeric_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
ZEND_API int string_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
#if HAVE_STRCOLL
ZEND_API int string_locale_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC);
#endif

ZEND_API void zend_str_tolower(char *str, unsigned int length);
ZEND_API char *zend_str_tolower_copy(char *dest, const char *source, unsigned int length);
ZEND_API char *zend_str_tolower_dup(const char *source, unsigned int length);

ZEND_API void zend_u_str_tolower(zend_uchar type, zstr str, unsigned int length);
ZEND_API zstr zend_u_str_tolower_copy(zend_uchar type, zstr dest, zstr source, unsigned int length);
ZEND_API zstr zend_u_str_tolower_dup(zend_uchar type, zstr source, unsigned int length);

ZEND_API zstr zend_u_str_case_fold(zend_uchar type, zstr source, unsigned int length, zend_bool normalize, unsigned int *new_len);

ZEND_API int zend_binary_zval_strcmp(zval *s1, zval *s2);
ZEND_API int zend_binary_zval_strncmp(zval *s1, zval *s2, zval *s3);
ZEND_API int zend_binary_zval_strcasecmp(zval *s1, zval *s2);
ZEND_API int zend_binary_zval_strncasecmp(zval *s1, zval *s2, zval *s3);
ZEND_API int zend_binary_strcmp(const char *s1, uint len1, const char *s2, uint len2);
ZEND_API int zend_binary_strncmp(const char *s1, uint len1, const char *s2, uint len2, uint length);
ZEND_API int zend_binary_strcasecmp(const char *s1, uint len1, const char *s2, uint len2);
ZEND_API int zend_binary_strncasecmp(const char *s1, uint len1, const char *s2, uint len2, uint length);

ZEND_API int zend_u_binary_zval_strcmp(zval *s1, zval *s2);
ZEND_API int zend_u_binary_strcmp(UChar *s1, int len1, UChar *s2, int len2);
ZEND_API int zend_u_binary_strncmp(UChar *s1, int len1, UChar *s2, int len2, uint length);
ZEND_API int zend_u_binary_strcasecmp(UChar *s1, int len1, UChar *s2, int len2);
ZEND_API int zend_u_binary_strncasecmp(UChar *s1, int len1, UChar *s2, int len2, uint length);

ZEND_API void zendi_smart_strcmp(zval *result, zval *s1, zval *s2);
ZEND_API void zendi_u_smart_strcmp(zval *result, zval *s1, zval *s2);
ZEND_API void zend_compare_symbol_tables(zval *result, HashTable *ht1, HashTable *ht2 TSRMLS_DC);
ZEND_API void zend_compare_arrays(zval *result, zval *a1, zval *a2 TSRMLS_DC);
ZEND_API void zend_compare_objects(zval *result, zval *o1, zval *o2 TSRMLS_DC);

ZEND_API int zend_atoi(const char *str, int str_len);
ZEND_API long zend_atol(const char *str, int str_len);

ZEND_API void zend_locale_sprintf_double(zval *op ZEND_FILE_LINE_DC);
END_EXTERN_C()
#define convert_to_ex_master(ppzv, lower_type, upper_type)	\
	if (Z_TYPE_PP(ppzv)!=IS_##upper_type) {					\
		SEPARATE_ZVAL_IF_NOT_REF(ppzv);						\
		convert_to_##lower_type(*ppzv);						\
	}

#define convert_to_explicit_type(pzv, type)		\
    do {										\
		switch (type) {							\
			case IS_NULL:						\
				convert_to_null(pzv);			\
				break;							\
			case IS_LONG:						\
				convert_to_long(pzv);			\
				break;							\
			case IS_DOUBLE: 					\
				convert_to_double(pzv); 		\
				break; 							\
			case IS_BOOL: 						\
				convert_to_boolean(pzv); 		\
				break; 							\
			case IS_ARRAY: 						\
				convert_to_array(pzv); 			\
				break; 							\
			case IS_OBJECT: 					\
				convert_to_object(pzv); 		\
				break; 							\
			case IS_STRING: 					\
				convert_to_string(pzv); 		\
				break; 							\
			case IS_UNICODE: 					\
				convert_to_unicode(pzv);		\
				break; 							\
			default: 							\
				assert(0); 						\
				break; 							\
		}										\
	} while (0);								\

#define convert_to_explicit_type_ex(ppzv, str_type)	\
	if (Z_TYPE_PP(ppzv) != str_type) {				\
		SEPARATE_ZVAL_IF_NOT_REF(ppzv);				\
		convert_to_explicit_type(*ppzv, str_type);	\
	}

#define convert_to_boolean_ex(ppzv)	convert_to_ex_master(ppzv, boolean, BOOL)
#define convert_to_long_ex(ppzv)	convert_to_ex_master(ppzv, long, LONG)
#define convert_to_double_ex(ppzv)	convert_to_ex_master(ppzv, double, DOUBLE)
#define convert_to_string_ex(ppzv)	convert_to_ex_master(ppzv, string, STRING)
#define convert_to_unicode_ex(ppzv)	convert_to_ex_master(ppzv, unicode, UNICODE)
#define convert_to_array_ex(ppzv)	convert_to_ex_master(ppzv, array, ARRAY)
#define convert_to_object_ex(ppzv)	convert_to_ex_master(ppzv, object, OBJECT)
#define convert_to_null_ex(ppzv)	convert_to_ex_master(ppzv, null, NULL)
#define convert_to_text_ex(ppzv)	if (UG(unicode)) {convert_to_unicode_ex(ppzv);} else {convert_to_string_ex(ppzv);}

#define convert_to_string_with_converter_ex(ppzv, conv) \
	if (Z_TYPE_PP(ppzv) != IS_STRING) {					\
		SEPARATE_ZVAL_IF_NOT_REF(ppzv);					\
		convert_to_string_with_converter(*ppzv, conv);	\
	}

#define convert_scalar_to_number_ex(ppzv)							\
	if (Z_TYPE_PP(ppzv)!=IS_LONG && Z_TYPE_PP(ppzv)!=IS_DOUBLE) {		\
		if (!(*ppzv)->is_ref) {										\
			SEPARATE_ZVAL(ppzv);									\
		}															\
		convert_scalar_to_number(*ppzv TSRMLS_CC);					\
	}


#define Z_LVAL(zval)			(zval).value.lval
#define Z_BVAL(zval)			((zend_bool)(zval).value.lval)
#define Z_DVAL(zval)			(zval).value.dval
#define Z_STRVAL(zval)			(zval).value.str.val
#define Z_STRLEN(zval)			(zval).value.str.len
#define Z_USTRVAL(zval)			(zval).value.ustr.val
#define Z_USTRLEN(zval)			(zval).value.ustr.len
#define Z_USTRCPLEN(zval)		(u_countChar32((zval).value.ustr.val, (zval).value.ustr.len))
#define Z_ARRVAL(zval)			(zval).value.ht
#define Z_OBJVAL(zval)			(zval).value.obj
#define Z_OBJ_HANDLE(zval)		Z_OBJVAL(zval).handle
#define Z_OBJ_HT(zval)			Z_OBJVAL(zval).handlers
#define Z_OBJCE(zval)			zend_get_class_entry(&(zval) TSRMLS_CC)
#define Z_OBJPROP(zval)			Z_OBJ_HT((zval))->get_properties(&(zval) TSRMLS_CC)
#define Z_OBJ_HANDLER(zval, hf) Z_OBJ_HT((zval))->hf
#define Z_RESVAL(zval)			(zval).value.lval
#define Z_UNIVAL(zval)			(zval).value.uni.val
#define Z_UNILEN(zval)			(zval).value.uni.len
#define Z_UNISIZE(zval)         ((Z_TYPE(zval)==IS_UNICODE) ? Z_UNILEN(zval)*sizeof(UChar) : Z_UNILEN(zval))
#define Z_OBJDEBUG(zval,is_tmp)	(Z_OBJ_HANDLER((zval),get_debug_info)?Z_OBJ_HANDLER((zval),get_debug_info)(&(zval),&is_tmp TSRMLS_CC):(is_tmp=0,Z_OBJ_HANDLER((zval),get_properties)?Z_OBJPROP(zval):NULL))

#define Z_LVAL_P(zval_p)		Z_LVAL(*zval_p)
#define Z_BVAL_P(zval_p)		Z_BVAL(*zval_p)
#define Z_DVAL_P(zval_p)		Z_DVAL(*zval_p)
#define Z_STRVAL_P(zval_p)		Z_STRVAL(*zval_p)
#define Z_STRLEN_P(zval_p)		Z_STRLEN(*zval_p)
#define Z_USTRVAL_P(zval_p)		Z_USTRVAL(*zval_p)
#define Z_USTRLEN_P(zval_p)		Z_USTRLEN(*zval_p)
#define Z_USTRCPLEN_P(zval_p)	Z_USTRCPLEN(*zval_p)
#define Z_ARRVAL_P(zval_p)		Z_ARRVAL(*zval_p)
#define Z_OBJPROP_P(zval_p)		Z_OBJPROP(*zval_p)
#define Z_OBJCE_P(zval_p)		Z_OBJCE(*zval_p)
#define Z_RESVAL_P(zval_p)		Z_RESVAL(*zval_p)
#define Z_OBJVAL_P(zval_p)      Z_OBJVAL(*zval_p)
#define Z_OBJ_HANDLE_P(zval_p)  Z_OBJ_HANDLE(*zval_p)
#define Z_OBJ_HT_P(zval_p)      Z_OBJ_HT(*zval_p)
#define Z_OBJ_HANDLER_P(zval_p, h) Z_OBJ_HANDLER(*zval_p, h)
#define Z_UNIVAL_P(zval_p)		Z_UNIVAL(*zval_p)
#define Z_UNILEN_P(zval_p)		Z_UNILEN(*zval_p)
#define Z_UNISIZE_P(zval_p)     Z_UNISIZE(*zval_p)
#define Z_OBJDEBUG_P(zval_p,is_tmp) Z_OBJDEBUG(*zval_p,is_tmp)

#define Z_LVAL_PP(zval_pp)		Z_LVAL(**zval_pp)
#define Z_BVAL_PP(zval_pp)		Z_BVAL(**zval_pp)
#define Z_DVAL_PP(zval_pp)		Z_DVAL(**zval_pp)
#define Z_STRVAL_PP(zval_pp)	Z_STRVAL(**zval_pp)
#define Z_STRLEN_PP(zval_pp)	Z_STRLEN(**zval_pp)
#define Z_USTRVAL_PP(zval_pp)	Z_USTRVAL(**zval_pp)
#define Z_USTRLEN_PP(zval_pp)	Z_USTRLEN(**zval_pp)
#define Z_USTRCPLEN_PP(zval_pp)	Z_USTRCPLEN(**zval_pp)
#define Z_ARRVAL_PP(zval_pp)	Z_ARRVAL(**zval_pp)
#define Z_OBJPROP_PP(zval_pp)	Z_OBJPROP(**zval_pp)
#define Z_OBJCE_PP(zval_pp)		Z_OBJCE(**zval_pp)
#define Z_RESVAL_PP(zval_pp)	Z_RESVAL(**zval_pp)
#define Z_OBJVAL_PP(zval_pp)    Z_OBJVAL(**zval_pp)
#define Z_OBJ_HANDLE_PP(zval_p) Z_OBJ_HANDLE(**zval_p)
#define Z_OBJ_HT_PP(zval_p)     Z_OBJ_HT(**zval_p)
#define Z_OBJ_HANDLER_PP(zval_p, h) Z_OBJ_HANDLER(**zval_p, h)
#define Z_UNIVAL_PP(zval_pp)	Z_UNIVAL(**zval_pp)
#define Z_UNILEN_PP(zval_pp)	Z_UNILEN(**zval_pp)
#define Z_UNISIZE_PP(zval_pp)   Z_UNISIZE(**zval_pp)
#define Z_OBJDEBUG_PP(zval_pp,is_tmp) Z_OBJDEBUG(**zval_pp,is_tmp)

#define Z_TYPE(zval)		(zval).type
#define Z_TYPE_P(zval_p)	Z_TYPE(*zval_p)
#define Z_TYPE_PP(zval_pp)	Z_TYPE(**zval_pp)

#if HAVE_SETLOCALE && defined(ZEND_WIN32) && !defined(ZTS) && defined(_MSC_VER) && (_MSC_VER >= 1400)
/* This is performance improvement of tolower() on Windows and VC2005
 * Gives 10-18% on bench.php
 */
#define ZEND_USE_TOLOWER_L 1
#endif

#ifdef ZEND_USE_TOLOWER_L
ZEND_API void zend_update_current_locale(void);
#else
#define zend_update_current_locale()
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
