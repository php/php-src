/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   |          Andrei Zmievski <andrei@php.net>                            |
   |          Stig Venaas <venaas@php.net>                                |
   |          Jason Greene <jason@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "php_ini.h"
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#ifdef PHP_WIN32
#include "win32/unistd.h"
#endif
#include "zend_globals.h"
#include "zend_interfaces.h"
#include "php_globals.h"
#include "php_array.h"
#include "basic_functions.h"
#include "php_string.h"
#include "php_rand.h"
#include "php_math.h"
#include "zend_smart_str.h"
#include "zend_bitset.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_array.h"

/* {{{ defines */
#define EXTR_OVERWRITE			0
#define EXTR_SKIP				1
#define EXTR_PREFIX_SAME		2
#define	EXTR_PREFIX_ALL			3
#define	EXTR_PREFIX_INVALID		4
#define	EXTR_PREFIX_IF_EXISTS	5
#define	EXTR_IF_EXISTS			6

#define EXTR_REFS				0x100

#define CASE_LOWER				0
#define CASE_UPPER				1

#define DIFF_NORMAL			1
#define DIFF_KEY			2
#define DIFF_ASSOC			6
#define DIFF_COMP_DATA_NONE    -1
#define DIFF_COMP_DATA_INTERNAL 0
#define DIFF_COMP_DATA_USER     1
#define DIFF_COMP_KEY_INTERNAL  0
#define DIFF_COMP_KEY_USER      1

#define INTERSECT_NORMAL		1
#define INTERSECT_KEY			2
#define INTERSECT_ASSOC			6
#define INTERSECT_COMP_DATA_NONE    -1
#define INTERSECT_COMP_DATA_INTERNAL 0
#define INTERSECT_COMP_DATA_USER     1
#define INTERSECT_COMP_KEY_INTERNAL  0
#define INTERSECT_COMP_KEY_USER      1
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(array)

/* {{{ php_array_init_globals */
static void php_array_init_globals(zend_array_globals *array_globals)
{
	memset(array_globals, 0, sizeof(zend_array_globals));
}
/* }}} */

PHP_MINIT_FUNCTION(array) /* {{{ */
{
	ZEND_INIT_MODULE_GLOBALS(array, php_array_init_globals, NULL);

	REGISTER_LONG_CONSTANT("EXTR_OVERWRITE", EXTR_OVERWRITE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_SKIP", EXTR_SKIP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_SAME", EXTR_PREFIX_SAME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_ALL", EXTR_PREFIX_ALL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_INVALID", EXTR_PREFIX_INVALID, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_IF_EXISTS", EXTR_PREFIX_IF_EXISTS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_IF_EXISTS", EXTR_IF_EXISTS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_REFS", EXTR_REFS, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SORT_ASC", PHP_SORT_ASC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_DESC", PHP_SORT_DESC, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("SORT_REGULAR", PHP_SORT_REGULAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_NUMERIC", PHP_SORT_NUMERIC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_STRING", PHP_SORT_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_LOCALE_STRING", PHP_SORT_LOCALE_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_NATURAL", PHP_SORT_NATURAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SORT_FLAG_CASE", PHP_SORT_FLAG_CASE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("CASE_LOWER", CASE_LOWER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CASE_UPPER", CASE_UPPER, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("COUNT_NORMAL", COUNT_NORMAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("COUNT_RECURSIVE", COUNT_RECURSIVE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("ARRAY_FILTER_USE_BOTH", ARRAY_FILTER_USE_BOTH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("ARRAY_FILTER_USE_KEY", ARRAY_FILTER_USE_KEY, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(array) /* {{{ */
{
#ifdef ZTS
	ts_free_id(array_globals_id);
#endif

	return SUCCESS;
}
/* }}} */

static zend_never_inline ZEND_COLD int stable_sort_fallback(Bucket *a, Bucket *b) {
	if (Z_EXTRA(a->val) > Z_EXTRA(b->val)) {
		return 1;
	} else if (Z_EXTRA(a->val) < Z_EXTRA(b->val)) {
		return -1;
	} else {
		return 0;
	}
}

#define RETURN_STABLE_SORT(a, b, result) do { \
	int _result = (result); \
	if (EXPECTED(_result)) { \
		return _result; \
	} \
	return stable_sort_fallback((a), (b)); \
} while (0)

/* Generate inlined unstable and stable variants, and non-inlined reversed variants. */
#define DEFINE_SORT_VARIANTS(name) \
	static zend_never_inline int php_array_##name##_unstable(Bucket *a, Bucket *b) { \
		return php_array_##name##_unstable_i(a, b); \
	} \
	static zend_never_inline int php_array_##name(Bucket *a, Bucket *b) { \
		RETURN_STABLE_SORT(a, b, php_array_##name##_unstable_i(a, b)); \
	} \
	static zend_never_inline int php_array_reverse_##name##_unstable(Bucket *a, Bucket *b) { \
		return php_array_##name##_unstable(a, b) * -1; \
	} \
	static zend_never_inline int php_array_reverse_##name(Bucket *a, Bucket *b) { \
		RETURN_STABLE_SORT(a, b, php_array_reverse_##name##_unstable(a, b)); \
	} \

static zend_always_inline int php_array_key_compare_unstable_i(Bucket *f, Bucket *s) /* {{{ */
{
	zend_uchar t;
	zend_long l1, l2;
	double d;

	if (f->key == NULL) {
		if (s->key == NULL) {
			return (zend_long)f->h > (zend_long)s->h ? 1 : -1;
		} else {
			l1 = (zend_long)f->h;
			t = is_numeric_string(s->key->val, s->key->len, &l2, &d, 1);
			if (t == IS_LONG) {
				/* pass */
			} else if (t == IS_DOUBLE) {
				return ZEND_NORMALIZE_BOOL((double)l1 - d);
			} else {
				l2 = 0;
			}
		}
	} else {
		if (s->key) {
			return zendi_smart_strcmp(f->key, s->key);
		} else {
			l2 = (zend_long)s->h;
			t = is_numeric_string(f->key->val, f->key->len, &l1, &d, 1);
			if (t == IS_LONG) {
				/* pass */
			} else if (t == IS_DOUBLE) {
				return ZEND_NORMALIZE_BOOL(d - (double)l2);
			} else {
				l1 = 0;
			}
		}
	}
	return ZEND_NORMALIZE_BOOL(l1 - l2);
}
/* }}} */

static zend_always_inline int php_array_key_compare_numeric_unstable_i(Bucket *f, Bucket *s) /* {{{ */
{
	if (f->key == NULL && s->key == NULL) {
		return (zend_long)f->h > (zend_long)s->h ? 1 : -1;
	} else {
		double d1, d2;
		if (f->key) {
			d1 = zend_strtod(f->key->val, NULL);
		} else {
			d1 = (double)(zend_long)f->h;
		}
		if (s->key) {
			d2 = zend_strtod(s->key->val, NULL);
		} else {
			d2 = (double)(zend_long)s->h;
		}
		return ZEND_NORMALIZE_BOOL(d1 - d2);
	}
}
/* }}} */

static zend_always_inline int php_array_key_compare_string_case_unstable_i(Bucket *f, Bucket *s) /* {{{ */
{
	const char *s1, *s2;
	size_t l1, l2;
	char buf1[MAX_LENGTH_OF_LONG + 1];
	char buf2[MAX_LENGTH_OF_LONG + 1];

	if (f->key) {
		s1 = f->key->val;
		l1 = f->key->len;
	} else {
		s1 = zend_print_long_to_buf(buf1 + sizeof(buf1) - 1, f->h);
		l1 = buf1 + sizeof(buf1) - 1 - s1;
	}
	if (s->key) {
		s2 = s->key->val;
		l2 = s->key->len;
	} else {
		s2 = zend_print_long_to_buf(buf2 + sizeof(buf2) - 1, s->h);
		l2 = buf2 + sizeof(buf2) - 1 - s1;
	}
	return zend_binary_strcasecmp_l(s1, l1, s2, l2);
}
/* }}} */

static zend_always_inline int php_array_key_compare_string_unstable_i(Bucket *f, Bucket *s) /* {{{ */
{
	const char *s1, *s2;
	size_t l1, l2;
	char buf1[MAX_LENGTH_OF_LONG + 1];
	char buf2[MAX_LENGTH_OF_LONG + 1];

	if (f->key) {
		s1 = f->key->val;
		l1 = f->key->len;
	} else {
		s1 = zend_print_long_to_buf(buf1 + sizeof(buf1) - 1, f->h);
		l1 = buf1 + sizeof(buf1) - 1 - s1;
	}
	if (s->key) {
		s2 = s->key->val;
		l2 = s->key->len;
	} else {
		s2 = zend_print_long_to_buf(buf2 + sizeof(buf2) - 1, s->h);
		l2 = buf2 + sizeof(buf2) - 1 - s2;
	}
	return zend_binary_strcmp(s1, l1, s2, l2);
}
/* }}} */

static int php_array_key_compare_string_natural_general(Bucket *f, Bucket *s, int fold_case) /* {{{ */
{
	const char *s1, *s2;
	size_t l1, l2;
	char buf1[MAX_LENGTH_OF_LONG + 1];
	char buf2[MAX_LENGTH_OF_LONG + 1];

	if (f->key) {
		s1 = f->key->val;
		l1 = f->key->len;
	} else {
		s1 = zend_print_long_to_buf(buf1 + sizeof(buf1) - 1, f->h);
		l1 = buf1 + sizeof(buf1) - 1 - s1;
	}
	if (s->key) {
		s2 = s->key->val;
		l2 = s->key->len;
	} else {
		s2 = zend_print_long_to_buf(buf2 + sizeof(buf2) - 1, s->h);
		l2 = buf2 + sizeof(buf2) - 1 - s1;
	}
	return strnatcmp_ex(s1, l1, s2, l2, fold_case);
}
/* }}} */

static int php_array_key_compare_string_natural_case(Bucket *a, Bucket *b) /* {{{ */
{
	RETURN_STABLE_SORT(a, b, php_array_key_compare_string_natural_general(a, b, 1));
}
/* }}} */

static int php_array_reverse_key_compare_string_natural_case(Bucket *a, Bucket *b) /* {{{ */
{
	RETURN_STABLE_SORT(a, b, php_array_key_compare_string_natural_general(b, a, 1));
}
/* }}} */

static int php_array_key_compare_string_natural(Bucket *a, Bucket *b) /* {{{ */
{
	RETURN_STABLE_SORT(a, b, php_array_key_compare_string_natural_general(a, b, 0));
}
/* }}} */

static int php_array_reverse_key_compare_string_natural(Bucket *a, Bucket *b) /* {{{ */
{
	RETURN_STABLE_SORT(a, b, php_array_key_compare_string_natural_general(b, a, 0));
}
/* }}} */

static zend_always_inline int php_array_key_compare_string_locale_unstable_i(Bucket *f, Bucket *s) /* {{{ */
{
	const char *s1, *s2;
	char buf1[MAX_LENGTH_OF_LONG + 1];
	char buf2[MAX_LENGTH_OF_LONG + 1];

	if (f->key) {
		s1 = f->key->val;
	} else {
		s1 = zend_print_long_to_buf(buf1 + sizeof(buf1) - 1, f->h);
	}
	if (s->key) {
		s2 = s->key->val;
	} else {
		s2 = zend_print_long_to_buf(buf2 + sizeof(buf2) - 1, s->h);
	}
	return strcoll(s1, s2);
}
/* }}} */

static zend_always_inline int php_array_data_compare_unstable_i(Bucket *f, Bucket *s) /* {{{ */
{
	zval *first = &f->val;
	zval *second = &s->val;

	if (UNEXPECTED(Z_TYPE_P(first) == IS_INDIRECT)) {
		first = Z_INDIRECT_P(first);
	}
	if (UNEXPECTED(Z_TYPE_P(second) == IS_INDIRECT)) {
		second = Z_INDIRECT_P(second);
	}
	return zend_compare(first, second);
}
/* }}} */

static zend_always_inline int php_array_data_compare_numeric_unstable_i(Bucket *f, Bucket *s) /* {{{ */
{
	zval *first = &f->val;
	zval *second = &s->val;

	if (UNEXPECTED(Z_TYPE_P(first) == IS_INDIRECT)) {
		first = Z_INDIRECT_P(first);
	}
	if (UNEXPECTED(Z_TYPE_P(second) == IS_INDIRECT)) {
		second = Z_INDIRECT_P(second);
	}

	return numeric_compare_function(first, second);
}
/* }}} */

static zend_always_inline int php_array_data_compare_string_case_unstable_i(Bucket *f, Bucket *s) /* {{{ */
{
	zval *first = &f->val;
	zval *second = &s->val;

	if (UNEXPECTED(Z_TYPE_P(first) == IS_INDIRECT)) {
		first = Z_INDIRECT_P(first);
	}
	if (UNEXPECTED(Z_TYPE_P(second) == IS_INDIRECT)) {
		second = Z_INDIRECT_P(second);
	}

	return string_case_compare_function(first, second);
}
/* }}} */

static zend_always_inline int php_array_data_compare_string_unstable_i(Bucket *f, Bucket *s) /* {{{ */
{
	zval *first = &f->val;
	zval *second = &s->val;

	if (UNEXPECTED(Z_TYPE_P(first) == IS_INDIRECT)) {
		first = Z_INDIRECT_P(first);
	}
	if (UNEXPECTED(Z_TYPE_P(second) == IS_INDIRECT)) {
		second = Z_INDIRECT_P(second);
	}

	return string_compare_function(first, second);
}
/* }}} */

static int php_array_natural_general_compare(Bucket *f, Bucket *s, int fold_case) /* {{{ */
{
	zend_string *tmp_str1, *tmp_str2;
	zend_string *str1 = zval_get_tmp_string(&f->val, &tmp_str1);
	zend_string *str2 = zval_get_tmp_string(&s->val, &tmp_str2);

	int result = strnatcmp_ex(ZSTR_VAL(str1), ZSTR_LEN(str1), ZSTR_VAL(str2), ZSTR_LEN(str2), fold_case);

	zend_tmp_string_release(tmp_str1);
	zend_tmp_string_release(tmp_str2);
	return result;
}
/* }}} */

static zend_always_inline int php_array_natural_compare_unstable_i(Bucket *a, Bucket *b) /* {{{ */
{
	return php_array_natural_general_compare(a, b, 0);
}
/* }}} */

static zend_always_inline int php_array_natural_case_compare_unstable_i(Bucket *a, Bucket *b) /* {{{ */
{
	return php_array_natural_general_compare(a, b, 1);
}
/* }}} */

static int php_array_data_compare_string_locale_unstable_i(Bucket *f, Bucket *s) /* {{{ */
{
	zval *first = &f->val;
	zval *second = &s->val;

	if (UNEXPECTED(Z_TYPE_P(first) == IS_INDIRECT)) {
		first = Z_INDIRECT_P(first);
	}
	if (UNEXPECTED(Z_TYPE_P(second) == IS_INDIRECT)) {
		second = Z_INDIRECT_P(second);
	}

	return string_locale_compare_function(first, second);
}
/* }}} */

DEFINE_SORT_VARIANTS(key_compare);
DEFINE_SORT_VARIANTS(key_compare_numeric);
DEFINE_SORT_VARIANTS(key_compare_string_case);
DEFINE_SORT_VARIANTS(key_compare_string);
DEFINE_SORT_VARIANTS(key_compare_string_locale);
DEFINE_SORT_VARIANTS(data_compare);
DEFINE_SORT_VARIANTS(data_compare_numeric);
DEFINE_SORT_VARIANTS(data_compare_string_case);
DEFINE_SORT_VARIANTS(data_compare_string);
DEFINE_SORT_VARIANTS(data_compare_string_locale);
DEFINE_SORT_VARIANTS(natural_compare);
DEFINE_SORT_VARIANTS(natural_case_compare);

static bucket_compare_func_t php_get_key_compare_func(zend_long sort_type, int reverse) /* {{{ */
{
	switch (sort_type & ~PHP_SORT_FLAG_CASE) {
		case PHP_SORT_NUMERIC:
			if (reverse) {
				return php_array_reverse_key_compare_numeric;
			} else {
				return php_array_key_compare_numeric;
			}
			break;

		case PHP_SORT_STRING:
			if (sort_type & PHP_SORT_FLAG_CASE) {
				if (reverse) {
					return php_array_reverse_key_compare_string_case;
				} else {
					return php_array_key_compare_string_case;
				}
			} else {
				if (reverse) {
					return php_array_reverse_key_compare_string;
				} else {
					return php_array_key_compare_string;
				}
			}
			break;

		case PHP_SORT_NATURAL:
			if (sort_type & PHP_SORT_FLAG_CASE) {
				if (reverse) {
					return php_array_reverse_key_compare_string_natural_case;
				} else {
					return php_array_key_compare_string_natural_case;
				}
			} else {
				if (reverse) {
					return php_array_reverse_key_compare_string_natural;
				} else {
					return php_array_key_compare_string_natural;
				}
			}
			break;

		case PHP_SORT_LOCALE_STRING:
			if (reverse) {
				return php_array_reverse_key_compare_string_locale;
			} else {
				return php_array_key_compare_string_locale;
			}
			break;

		case PHP_SORT_REGULAR:
		default:
			if (reverse) {
				return php_array_reverse_key_compare;
			} else {
				return php_array_key_compare;
			}
			break;
	}
	return NULL;
}
/* }}} */

static bucket_compare_func_t php_get_data_compare_func(zend_long sort_type, int reverse) /* {{{ */
{
	switch (sort_type & ~PHP_SORT_FLAG_CASE) {
		case PHP_SORT_NUMERIC:
			if (reverse) {
				return php_array_reverse_data_compare_numeric;
			} else {
				return php_array_data_compare_numeric;
			}
			break;

		case PHP_SORT_STRING:
			if (sort_type & PHP_SORT_FLAG_CASE) {
				if (reverse) {
					return php_array_reverse_data_compare_string_case;
				} else {
					return php_array_data_compare_string_case;
				}
			} else {
				if (reverse) {
					return php_array_reverse_data_compare_string;
				} else {
					return php_array_data_compare_string;
				}
			}
			break;

		case PHP_SORT_NATURAL:
			if (sort_type & PHP_SORT_FLAG_CASE) {
				if (reverse) {
					return php_array_reverse_natural_case_compare;
				} else {
					return php_array_natural_case_compare;
				}
			} else {
				if (reverse) {
					return php_array_reverse_natural_compare;
				} else {
					return php_array_natural_compare;
				}
			}
			break;

		case PHP_SORT_LOCALE_STRING:
			if (reverse) {
				return php_array_reverse_data_compare_string_locale;
			} else {
				return php_array_data_compare_string_locale;
			}
			break;

		case PHP_SORT_REGULAR:
		default:
			if (reverse) {
				return php_array_reverse_data_compare;
			} else {
				return php_array_data_compare;
			}
			break;
	}
	return NULL;
}
/* }}} */

static bucket_compare_func_t php_get_data_compare_func_unstable(zend_long sort_type, int reverse) /* {{{ */
{
	switch (sort_type & ~PHP_SORT_FLAG_CASE) {
		case PHP_SORT_NUMERIC:
			if (reverse) {
				return php_array_reverse_data_compare_numeric_unstable;
			} else {
				return php_array_data_compare_numeric_unstable;
			}
			break;

		case PHP_SORT_STRING:
			if (sort_type & PHP_SORT_FLAG_CASE) {
				if (reverse) {
					return php_array_reverse_data_compare_string_case_unstable;
				} else {
					return php_array_data_compare_string_case_unstable;
				}
			} else {
				if (reverse) {
					return php_array_reverse_data_compare_string_unstable;
				} else {
					return php_array_data_compare_string_unstable;
				}
			}
			break;

		case PHP_SORT_NATURAL:
			if (sort_type & PHP_SORT_FLAG_CASE) {
				if (reverse) {
					return php_array_reverse_natural_case_compare_unstable;
				} else {
					return php_array_natural_case_compare_unstable;
				}
			} else {
				if (reverse) {
					return php_array_reverse_natural_compare_unstable;
				} else {
					return php_array_natural_compare_unstable;
				}
			}
			break;

		case PHP_SORT_LOCALE_STRING:
			if (reverse) {
				return php_array_reverse_data_compare_string_locale_unstable;
			} else {
				return php_array_data_compare_string_locale_unstable;
			}
			break;

		case PHP_SORT_REGULAR:
		default:
			if (reverse) {
				return php_array_reverse_data_compare_unstable;
			} else {
				return php_array_data_compare_unstable;
			}
			break;
	}
	return NULL;
}
/* }}} */

/* {{{ Sort an array by key value in reverse order */
PHP_FUNCTION(krsort)
{
	zval *array;
	zend_long sort_type = PHP_SORT_REGULAR;
	bucket_compare_func_t cmp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_EX(array, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(sort_type)
	ZEND_PARSE_PARAMETERS_END();

	cmp = php_get_key_compare_func(sort_type, 1);

	zend_hash_sort(Z_ARRVAL_P(array), cmp, 0);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Sort an array by key */
PHP_FUNCTION(ksort)
{
	zval *array;
	zend_long sort_type = PHP_SORT_REGULAR;
	bucket_compare_func_t cmp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_EX(array, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(sort_type)
	ZEND_PARSE_PARAMETERS_END();

	cmp = php_get_key_compare_func(sort_type, 0);

	zend_hash_sort(Z_ARRVAL_P(array), cmp, 0);

	RETURN_TRUE;
}
/* }}} */

PHPAPI zend_long php_count_recursive(HashTable *ht) /* {{{ */
{
	zend_long cnt = 0;
	zval *element;

	if (!(GC_FLAGS(ht) & GC_IMMUTABLE)) {
		if (GC_IS_RECURSIVE(ht)) {
			php_error_docref(NULL, E_WARNING, "Recursion detected");
			return 0;
		}
		GC_PROTECT_RECURSION(ht);
	}

	cnt = zend_array_count(ht);
	ZEND_HASH_FOREACH_VAL(ht, element) {
		ZVAL_DEREF(element);
		if (Z_TYPE_P(element) == IS_ARRAY) {
			cnt += php_count_recursive(Z_ARRVAL_P(element));
		}
	} ZEND_HASH_FOREACH_END();

	GC_TRY_UNPROTECT_RECURSION(ht);
	return cnt;
}
/* }}} */

/* {{{ Count the number of elements in a variable (usually an array) */
PHP_FUNCTION(count)
{
	zval *array;
	zend_long mode = COUNT_NORMAL;
	zend_long cnt;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(array)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(mode)
	ZEND_PARSE_PARAMETERS_END();

	if (mode != COUNT_NORMAL && mode != COUNT_RECURSIVE) {
		zend_argument_value_error(2, "must be either COUNT_NORMAL or COUNT_RECURSIVE");
		RETURN_THROWS();
	}

	switch (Z_TYPE_P(array)) {
		case IS_ARRAY:
			if (mode != COUNT_RECURSIVE) {
				cnt = zend_array_count(Z_ARRVAL_P(array));
			} else {
				cnt = php_count_recursive(Z_ARRVAL_P(array));
			}
			RETURN_LONG(cnt);
			break;
		case IS_OBJECT: {
			zval retval;
			/* first, we check if the handler is defined */
			if (Z_OBJ_HT_P(array)->count_elements) {
				RETVAL_LONG(1);
				if (SUCCESS == Z_OBJ_HT(*array)->count_elements(Z_OBJ_P(array), &Z_LVAL_P(return_value))) {
					return;
				}
				if (EG(exception)) {
					RETURN_THROWS();
				}
			}
			/* if not and the object implements Countable we call its count() method */
			if (instanceof_function(Z_OBJCE_P(array), zend_ce_countable)) {
				zend_call_method_with_0_params(Z_OBJ_P(array), NULL, NULL, "count", &retval);
				if (Z_TYPE(retval) != IS_UNDEF) {
					RETVAL_LONG(zval_get_long(&retval));
					zval_ptr_dtor(&retval);
				}
				return;
			}
		}
		/* fallthrough */
		default:
			zend_argument_type_error(1, "must be of type Countable|array, %s given", zend_zval_type_name(array));
			RETURN_THROWS();
	}
}
/* }}} */

static void php_natsort(INTERNAL_FUNCTION_PARAMETERS, int fold_case) /* {{{ */
{
	zval *array;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_EX(array, 0, 1)
	ZEND_PARSE_PARAMETERS_END();

	if (fold_case) {
		zend_hash_sort(Z_ARRVAL_P(array), php_array_natural_case_compare, 0);
	} else {
		zend_hash_sort(Z_ARRVAL_P(array), php_array_natural_compare, 0);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ Sort an array using natural sort */
PHP_FUNCTION(natsort)
{
	php_natsort(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Sort an array using case-insensitive natural sort */
PHP_FUNCTION(natcasesort)
{
	php_natsort(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Sort an array and maintain index association */
PHP_FUNCTION(asort)
{
	zval *array;
	zend_long sort_type = PHP_SORT_REGULAR;
	bucket_compare_func_t cmp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_EX(array, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(sort_type)
	ZEND_PARSE_PARAMETERS_END();

	cmp = php_get_data_compare_func(sort_type, 0);

	zend_hash_sort(Z_ARRVAL_P(array), cmp, 0);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Sort an array in reverse order and maintain index association */
PHP_FUNCTION(arsort)
{
	zval *array;
	zend_long sort_type = PHP_SORT_REGULAR;
	bucket_compare_func_t cmp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_EX(array, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(sort_type)
	ZEND_PARSE_PARAMETERS_END();

	cmp = php_get_data_compare_func(sort_type, 1);

	zend_hash_sort(Z_ARRVAL_P(array), cmp, 0);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Sort an array */
PHP_FUNCTION(sort)
{
	zval *array;
	zend_long sort_type = PHP_SORT_REGULAR;
	bucket_compare_func_t cmp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_EX(array, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(sort_type)
	ZEND_PARSE_PARAMETERS_END();

	cmp = php_get_data_compare_func(sort_type, 0);

	zend_hash_sort(Z_ARRVAL_P(array), cmp, 1);

	RETURN_TRUE;
}
/* }}} */

/* {{{ Sort an array in reverse order */
PHP_FUNCTION(rsort)
{
	zval *array;
	zend_long sort_type = PHP_SORT_REGULAR;
	bucket_compare_func_t cmp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY_EX(array, 0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(sort_type)
	ZEND_PARSE_PARAMETERS_END();

	cmp = php_get_data_compare_func(sort_type, 1);

	zend_hash_sort(Z_ARRVAL_P(array), cmp, 1);

	RETURN_TRUE;
}
/* }}} */

static inline int php_array_user_compare_unstable(Bucket *f, Bucket *s) /* {{{ */
{
	zval args[2];
	zval retval;
	zend_bool call_failed;

	ZVAL_COPY(&args[0], &f->val);
	ZVAL_COPY(&args[1], &s->val);

	BG(user_compare_fci).param_count = 2;
	BG(user_compare_fci).params = args;
	BG(user_compare_fci).retval = &retval;
	call_failed = zend_call_function(&BG(user_compare_fci), &BG(user_compare_fci_cache)) == FAILURE || Z_TYPE(retval) == IS_UNDEF;
	zval_ptr_dtor(&args[1]);
	zval_ptr_dtor(&args[0]);
	if (UNEXPECTED(call_failed)) {
		return 0;
	}

	if (UNEXPECTED(Z_TYPE(retval) == IS_FALSE || Z_TYPE(retval) == IS_TRUE)) {
		if (!ARRAYG(compare_deprecation_thrown)) {
			php_error_docref(NULL, E_DEPRECATED,
				"Returning bool from comparison function is deprecated, "
				"return an integer less than, equal to, or greater than zero");
			ARRAYG(compare_deprecation_thrown) = 1;
		}

		if (Z_TYPE(retval) == IS_FALSE) {
			/* Retry with swapped operands. */
			ZVAL_COPY(&args[0], &s->val);
			ZVAL_COPY(&args[1], &f->val);
			call_failed = zend_call_function(&BG(user_compare_fci), &BG(user_compare_fci_cache)) == FAILURE || Z_TYPE(retval) == IS_UNDEF;
			zval_ptr_dtor(&args[1]);
			zval_ptr_dtor(&args[0]);
			if (call_failed) {
				return 0;
			}

			zend_long ret = zval_get_long(&retval);
			zval_ptr_dtor(&retval);
			return -ZEND_NORMALIZE_BOOL(ret);
		}
	}

	zend_long ret = zval_get_long(&retval);
	zval_ptr_dtor(&retval);
	return ZEND_NORMALIZE_BOOL(ret);
}
/* }}} */

static int php_array_user_compare(Bucket *a, Bucket *b) /* {{{ */
{
	RETURN_STABLE_SORT(a, b, php_array_user_compare_unstable(a, b));
}
/* }}} */

#define PHP_ARRAY_CMP_FUNC_VARS \
	zend_fcall_info old_user_compare_fci; \
	zend_fcall_info_cache old_user_compare_fci_cache \

#define PHP_ARRAY_CMP_FUNC_BACKUP() \
	old_user_compare_fci = BG(user_compare_fci); \
	old_user_compare_fci_cache = BG(user_compare_fci_cache); \
	ARRAYG(compare_deprecation_thrown) = 0; \
	BG(user_compare_fci_cache) = empty_fcall_info_cache; \

#define PHP_ARRAY_CMP_FUNC_RESTORE() \
	BG(user_compare_fci) = old_user_compare_fci; \
	BG(user_compare_fci_cache) = old_user_compare_fci_cache; \

static void php_usort(INTERNAL_FUNCTION_PARAMETERS, bucket_compare_func_t compare_func, zend_bool renumber) /* {{{ */
{
	zval *array;
	zend_array *arr;
	PHP_ARRAY_CMP_FUNC_VARS;

	PHP_ARRAY_CMP_FUNC_BACKUP();

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ARRAY_EX2(array, 0, 1, 0)
		Z_PARAM_FUNC(BG(user_compare_fci), BG(user_compare_fci_cache))
	ZEND_PARSE_PARAMETERS_END_EX( PHP_ARRAY_CMP_FUNC_RESTORE(); return );

	arr = Z_ARR_P(array);
	if (zend_hash_num_elements(arr) == 0)  {
		PHP_ARRAY_CMP_FUNC_RESTORE();
		RETURN_TRUE;
	}

	/* Copy array, so the in-place modifications will not be visible to the callback function */
	arr = zend_array_dup(arr);

	zend_hash_sort(arr, compare_func, renumber);

	zval_ptr_dtor(array);
	ZVAL_ARR(array, arr);

	PHP_ARRAY_CMP_FUNC_RESTORE();
	RETURN_TRUE;
}
/* }}} */

/* {{{ Sort an array by values using a user-defined comparison function */
PHP_FUNCTION(usort)
{
	php_usort(INTERNAL_FUNCTION_PARAM_PASSTHRU, php_array_user_compare, 1);
}
/* }}} */

/* {{{ Sort an array with a user-defined comparison function and maintain index association */
PHP_FUNCTION(uasort)
{
	php_usort(INTERNAL_FUNCTION_PARAM_PASSTHRU, php_array_user_compare, 0);
}
/* }}} */

static inline int php_array_user_key_compare_unstable(Bucket *f, Bucket *s) /* {{{ */
{
	zval args[2];
	zval retval;
	zend_bool call_failed;

	if (f->key == NULL) {
		ZVAL_LONG(&args[0], f->h);
	} else {
		ZVAL_STR_COPY(&args[0], f->key);
	}
	if (s->key == NULL) {
		ZVAL_LONG(&args[1], s->h);
	} else {
		ZVAL_STR_COPY(&args[1], s->key);
	}

	BG(user_compare_fci).param_count = 2;
	BG(user_compare_fci).params = args;
	BG(user_compare_fci).retval = &retval;
	call_failed = zend_call_function(&BG(user_compare_fci), &BG(user_compare_fci_cache)) == FAILURE || Z_TYPE(retval) == IS_UNDEF;
	zval_ptr_dtor(&args[1]);
	zval_ptr_dtor(&args[0]);
	if (UNEXPECTED(call_failed)) {
		return 0;
	}

	if (UNEXPECTED(Z_TYPE(retval) == IS_FALSE || Z_TYPE(retval) == IS_TRUE)) {
		if (!ARRAYG(compare_deprecation_thrown)) {
			php_error_docref(NULL, E_DEPRECATED,
				"Returning bool from comparison function is deprecated, "
				"return an integer less than, equal to, or greater than zero");
			ARRAYG(compare_deprecation_thrown) = 1;
		}

		if (Z_TYPE(retval) == IS_FALSE) {
			/* Retry with swapped operands. */
			if (s->key == NULL) {
				ZVAL_LONG(&args[0], s->h);
			} else {
				ZVAL_STR_COPY(&args[0], s->key);
			}
			if (f->key == NULL) {
				ZVAL_LONG(&args[1], f->h);
			} else {
				ZVAL_STR_COPY(&args[1], f->key);
			}

			call_failed = zend_call_function(&BG(user_compare_fci), &BG(user_compare_fci_cache)) == FAILURE || Z_TYPE(retval) == IS_UNDEF;
			zval_ptr_dtor(&args[1]);
			zval_ptr_dtor(&args[0]);
			if (call_failed) {
				return 0;
			}

			zend_long ret = zval_get_long(&retval);
			zval_ptr_dtor(&retval);
			return -ZEND_NORMALIZE_BOOL(ret);
		}
	}

	zend_long result = zval_get_long(&retval);
	zval_ptr_dtor(&retval);
	return ZEND_NORMALIZE_BOOL(result);
}
/* }}} */

static int php_array_user_key_compare(Bucket *a, Bucket *b) /* {{{ */
{
	RETURN_STABLE_SORT(a, b, php_array_user_key_compare_unstable(a, b));
}
/* }}} */

/* {{{ Sort an array by keys using a user-defined comparison function */
PHP_FUNCTION(uksort)
{
	php_usort(INTERNAL_FUNCTION_PARAM_PASSTHRU, php_array_user_key_compare, 0);
}
/* }}} */

/* {{{ Advances array argument's internal pointer to the last element and return it */
PHP_FUNCTION(end)
{
	HashTable *array;
	zval *entry;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_OR_OBJECT_HT_EX(array, 0, 1)
	ZEND_PARSE_PARAMETERS_END();

	zend_hash_internal_pointer_end(array);

	if (USED_RET()) {
		if ((entry = zend_hash_get_current_data(array)) == NULL) {
			RETURN_FALSE;
		}

		if (Z_TYPE_P(entry) == IS_INDIRECT) {
			entry = Z_INDIRECT_P(entry);
		}

		ZVAL_COPY_DEREF(return_value, entry);
	}
}
/* }}} */

/* {{{ Move array argument's internal pointer to the previous element and return it */
PHP_FUNCTION(prev)
{
	HashTable *array;
	zval *entry;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_OR_OBJECT_HT_EX(array, 0, 1)
	ZEND_PARSE_PARAMETERS_END();

	zend_hash_move_backwards(array);

	if (USED_RET()) {
		if ((entry = zend_hash_get_current_data(array)) == NULL) {
			RETURN_FALSE;
		}

		if (Z_TYPE_P(entry) == IS_INDIRECT) {
			entry = Z_INDIRECT_P(entry);
		}

		ZVAL_COPY_DEREF(return_value, entry);
	}
}
/* }}} */

/* {{{ Move array argument's internal pointer to the next element and return it */
PHP_FUNCTION(next)
{
	HashTable *array;
	zval *entry;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_OR_OBJECT_HT_EX(array, 0, 1)
	ZEND_PARSE_PARAMETERS_END();

	zend_hash_move_forward(array);

	if (USED_RET()) {
		if ((entry = zend_hash_get_current_data(array)) == NULL) {
			RETURN_FALSE;
		}

		if (Z_TYPE_P(entry) == IS_INDIRECT) {
			entry = Z_INDIRECT_P(entry);
		}

		ZVAL_COPY_DEREF(return_value, entry);
	}
}
/* }}} */

/* {{{ Set array argument's internal pointer to the first element and return it */
PHP_FUNCTION(reset)
{
	HashTable *array;
	zval *entry;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_OR_OBJECT_HT_EX(array, 0, 1)
	ZEND_PARSE_PARAMETERS_END();

	zend_hash_internal_pointer_reset(array);

	if (USED_RET()) {
		if ((entry = zend_hash_get_current_data(array)) == NULL) {
			RETURN_FALSE;
		}

		if (Z_TYPE_P(entry) == IS_INDIRECT) {
			entry = Z_INDIRECT_P(entry);
		}

		ZVAL_COPY_DEREF(return_value, entry);
	}
}
/* }}} */

/* {{{ Return the element currently pointed to by the internal array pointer */
PHP_FUNCTION(current)
{
	HashTable *array;
	zval *entry;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_OR_OBJECT_HT(array)
	ZEND_PARSE_PARAMETERS_END();

	if ((entry = zend_hash_get_current_data(array)) == NULL) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(entry) == IS_INDIRECT) {
		entry = Z_INDIRECT_P(entry);
	}

	ZVAL_COPY_DEREF(return_value, entry);
}
/* }}} */

/* {{{ Return the key of the element currently pointed to by the internal array pointer */
PHP_FUNCTION(key)
{
	HashTable *array;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_OR_OBJECT_HT(array)
	ZEND_PARSE_PARAMETERS_END();

	zend_hash_get_current_key_zval(array, return_value);
}
/* }}} */

/* {{{
 * proto mixed min(array values)
 * proto mixed min(mixed arg1 [, mixed arg2 [, mixed ...]])
   Return the lowest value in an array or a series of arguments */
PHP_FUNCTION(min)
{
	int argc;
	zval *args = NULL;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	/* mixed min ( array $values ) */
	if (argc == 1) {
		if (Z_TYPE(args[0]) != IS_ARRAY) {
			zend_argument_type_error(1, "must be of type array, %s given", zend_zval_type_name(&args[0]));
			RETURN_THROWS();
		} else {
			zval *result = zend_hash_minmax(Z_ARRVAL(args[0]), php_array_data_compare_unstable, 0);
			if (result) {
				ZVAL_COPY_DEREF(return_value, result);
			} else {
				zend_argument_value_error(1, "must contain at least one element");
				RETURN_THROWS();
			}
		}
	} else {
		/* mixed min ( mixed $value1 , mixed $value2 [, mixed $value3... ] ) */
		zval *min, result;
		int i;

		min = &args[0];

		for (i = 1; i < argc; i++) {
			is_smaller_function(&result, &args[i], min);
			if (Z_TYPE(result) == IS_TRUE) {
				min = &args[i];
			}
		}

		ZVAL_COPY(return_value, min);
	}
}
/* }}} */

/* {{{
 * proto mixed max(array values)
 * proto mixed max(mixed arg1 [, mixed arg2 [, mixed ...]])
   Return the highest value in an array or a series of arguments */
PHP_FUNCTION(max)
{
	zval *args = NULL;
	int argc;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	/* mixed max ( array $values ) */
	if (argc == 1) {
		if (Z_TYPE(args[0]) != IS_ARRAY) {
			zend_argument_type_error(1, "must be of type array, %s given", zend_zval_type_name(&args[0]));
			RETURN_THROWS();
		} else {
			zval *result = zend_hash_minmax(Z_ARRVAL(args[0]), php_array_data_compare_unstable, 1);
			if (result) {
				ZVAL_COPY_DEREF(return_value, result);
			} else {
				zend_argument_value_error(1, "must contain at least one element");
				RETURN_THROWS();
			}
		}
	} else {
		/* mixed max ( mixed $value1 , mixed $value2 [, mixed $value3... ] ) */
		zval *max, result;
		int i;

		max = &args[0];

		for (i = 1; i < argc; i++) {
			is_smaller_or_equal_function(&result, &args[i], max);
			if (Z_TYPE(result) == IS_FALSE) {
				max = &args[i];
			}
		}

		ZVAL_COPY(return_value, max);
	}
}
/* }}} */

static int php_array_walk(zval *array, zval *userdata, int recursive) /* {{{ */
{
	zval args[3],		/* Arguments to userland function */
		 retval,		/* Return value - unused */
		 *zv;
	HashTable *target_hash = HASH_OF(array);
	HashPosition pos;
	uint32_t ht_iter;
	int result = SUCCESS;

	/* Set up known arguments */
	ZVAL_UNDEF(&args[1]);
	if (userdata) {
		ZVAL_COPY(&args[2], userdata);
	}

	BG(array_walk_fci).retval = &retval;
	BG(array_walk_fci).param_count = userdata ? 3 : 2;
	BG(array_walk_fci).params = args;

	zend_hash_internal_pointer_reset_ex(target_hash, &pos);
	ht_iter = zend_hash_iterator_add(target_hash, pos);

	/* Iterate through hash */
	do {
		/* Retrieve value */
		zv = zend_hash_get_current_data_ex(target_hash, &pos);
		if (zv == NULL) {
			break;
		}

		/* Skip undefined indirect elements */
		if (Z_TYPE_P(zv) == IS_INDIRECT) {
			zv = Z_INDIRECT_P(zv);
			if (Z_TYPE_P(zv) == IS_UNDEF) {
				zend_hash_move_forward_ex(target_hash, &pos);
				continue;
			}

			/* Add type source for property references. */
			if (Z_TYPE_P(zv) != IS_REFERENCE && Z_TYPE_P(array) == IS_OBJECT) {
				zend_property_info *prop_info =
					zend_get_typed_property_info_for_slot(Z_OBJ_P(array), zv);
				if (prop_info) {
					ZVAL_NEW_REF(zv, zv);
					ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(zv), prop_info);
				}
			}
		}

		/* Ensure the value is a reference. Otherwise the location of the value may be freed. */
		ZVAL_MAKE_REF(zv);

		/* Retrieve key */
		zend_hash_get_current_key_zval_ex(target_hash, &args[1], &pos);

		/* Move to next element already now -- this mirrors the approach used by foreach
		 * and ensures proper behavior with regard to modifications. */
		zend_hash_move_forward_ex(target_hash, &pos);

		/* Back up hash position, as it may change */
		EG(ht_iterators)[ht_iter].pos = pos;

		if (recursive && Z_TYPE_P(Z_REFVAL_P(zv)) == IS_ARRAY) {
			HashTable *thash;
			zend_fcall_info orig_array_walk_fci;
			zend_fcall_info_cache orig_array_walk_fci_cache;
			zval ref;
			ZVAL_COPY_VALUE(&ref, zv);

			ZVAL_DEREF(zv);
			SEPARATE_ARRAY(zv);
			thash = Z_ARRVAL_P(zv);
			if (GC_IS_RECURSIVE(thash)) {
				zend_throw_error(NULL, "Recursion detected");
				result = FAILURE;
				break;
			}

			/* backup the fcall info and cache */
			orig_array_walk_fci = BG(array_walk_fci);
			orig_array_walk_fci_cache = BG(array_walk_fci_cache);

			Z_ADDREF(ref);
			GC_PROTECT_RECURSION(thash);
			result = php_array_walk(zv, userdata, recursive);
			if (Z_TYPE_P(Z_REFVAL(ref)) == IS_ARRAY && thash == Z_ARRVAL_P(Z_REFVAL(ref))) {
				/* If the hashtable changed in the meantime, we'll "leak" this apply count
				 * increment -- our reference to thash is no longer valid. */
				GC_UNPROTECT_RECURSION(thash);
			}
			zval_ptr_dtor(&ref);

			/* restore the fcall info and cache */
			BG(array_walk_fci) = orig_array_walk_fci;
			BG(array_walk_fci_cache) = orig_array_walk_fci_cache;
		} else {
			ZVAL_COPY(&args[0], zv);

			/* Call the userland function */
			result = zend_call_function(&BG(array_walk_fci), &BG(array_walk_fci_cache));
			if (result == SUCCESS) {
				zval_ptr_dtor(&retval);
			}

			zval_ptr_dtor(&args[0]);
		}

		if (Z_TYPE(args[1]) != IS_UNDEF) {
			zval_ptr_dtor(&args[1]);
			ZVAL_UNDEF(&args[1]);
		}

		if (result == FAILURE) {
			break;
		}

		/* Reload array and position -- both may have changed */
		if (Z_TYPE_P(array) == IS_ARRAY) {
			pos = zend_hash_iterator_pos_ex(ht_iter, array);
			target_hash = Z_ARRVAL_P(array);
		} else if (Z_TYPE_P(array) == IS_OBJECT) {
			target_hash = Z_OBJPROP_P(array);
			pos = zend_hash_iterator_pos(ht_iter, target_hash);
		} else {
			zend_type_error("Iterated value is no longer an array or object");
			result = FAILURE;
			break;
		}
	} while (!EG(exception));

	if (userdata) {
		zval_ptr_dtor(&args[2]);
	}
	zend_hash_iterator_del(ht_iter);
	return result;
}
/* }}} */

/* {{{ Apply a user function to every member of an array */
PHP_FUNCTION(array_walk)
{
	zval *array;
	zval *userdata = NULL;
	zend_fcall_info orig_array_walk_fci;
	zend_fcall_info_cache orig_array_walk_fci_cache;

	orig_array_walk_fci = BG(array_walk_fci);
	orig_array_walk_fci_cache = BG(array_walk_fci_cache);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ARRAY_OR_OBJECT_EX(array, 0, 1)
		Z_PARAM_FUNC(BG(array_walk_fci), BG(array_walk_fci_cache))
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(userdata)
	ZEND_PARSE_PARAMETERS_END_EX(
		BG(array_walk_fci) = orig_array_walk_fci;
		BG(array_walk_fci_cache) = orig_array_walk_fci_cache;
		return
	);

	php_array_walk(array, userdata, 0);
	BG(array_walk_fci) = orig_array_walk_fci;
	BG(array_walk_fci_cache) = orig_array_walk_fci_cache;
	RETURN_TRUE;
}
/* }}} */

/* {{{ Apply a user function recursively to every member of an array */
PHP_FUNCTION(array_walk_recursive)
{
	zval *array;
	zval *userdata = NULL;
	zend_fcall_info orig_array_walk_fci;
	zend_fcall_info_cache orig_array_walk_fci_cache;

	orig_array_walk_fci = BG(array_walk_fci);
	orig_array_walk_fci_cache = BG(array_walk_fci_cache);

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ARRAY_OR_OBJECT_EX(array, 0, 1)
		Z_PARAM_FUNC(BG(array_walk_fci), BG(array_walk_fci_cache))
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(userdata)
	ZEND_PARSE_PARAMETERS_END_EX(
		BG(array_walk_fci) = orig_array_walk_fci;
		BG(array_walk_fci_cache) = orig_array_walk_fci_cache;
		return
	);

	php_array_walk(array, userdata, 1);
	BG(array_walk_fci) = orig_array_walk_fci;
	BG(array_walk_fci_cache) = orig_array_walk_fci_cache;
	RETURN_TRUE;
}
/* }}} */

/* void php_search_array(INTERNAL_FUNCTION_PARAMETERS, int behavior)
 * 0 = return boolean
 * 1 = return key
 */
static inline void php_search_array(INTERNAL_FUNCTION_PARAMETERS, int behavior) /* {{{ */
{
	zval *value,				/* value to check for */
		 *array,				/* array to check in */
		 *entry;				/* pointer to array entry */
	zend_ulong num_idx;
	zend_string *str_idx;
	zend_bool strict = 0;		/* strict comparison or not */

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ZVAL(value)
		Z_PARAM_ARRAY(array)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(strict)
	ZEND_PARSE_PARAMETERS_END();

	if (strict) {
		if (Z_TYPE_P(value) == IS_LONG) {
			ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL_P(array), num_idx, str_idx, entry) {
				ZVAL_DEREF(entry);
				if (Z_TYPE_P(entry) == IS_LONG && Z_LVAL_P(entry) == Z_LVAL_P(value)) {
					if (behavior == 0) {
						RETURN_TRUE;
					} else {
						if (str_idx) {
							RETVAL_STR_COPY(str_idx);
						} else {
							RETVAL_LONG(num_idx);
						}
						return;
					}
				}
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL_P(array), num_idx, str_idx, entry) {
				ZVAL_DEREF(entry);
				if (fast_is_identical_function(value, entry)) {
					if (behavior == 0) {
						RETURN_TRUE;
					} else {
						if (str_idx) {
							RETVAL_STR_COPY(str_idx);
						} else {
							RETVAL_LONG(num_idx);
						}
						return;
					}
				}
			} ZEND_HASH_FOREACH_END();
		}
	} else {
		if (Z_TYPE_P(value) == IS_LONG) {
			ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL_P(array), num_idx, str_idx, entry) {
				if (fast_equal_check_long(value, entry)) {
					if (behavior == 0) {
						RETURN_TRUE;
					} else {
						if (str_idx) {
							RETVAL_STR_COPY(str_idx);
						} else {
							RETVAL_LONG(num_idx);
						}
						return;
					}
				}
			} ZEND_HASH_FOREACH_END();
		} else if (Z_TYPE_P(value) == IS_STRING) {
			ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL_P(array), num_idx, str_idx, entry) {
				if (fast_equal_check_string(value, entry)) {
					if (behavior == 0) {
						RETURN_TRUE;
					} else {
						if (str_idx) {
							RETVAL_STR_COPY(str_idx);
						} else {
							RETVAL_LONG(num_idx);
						}
						return;
					}
				}
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL_P(array), num_idx, str_idx, entry) {
				if (fast_equal_check_function(value, entry)) {
					if (behavior == 0) {
						RETURN_TRUE;
					} else {
						if (str_idx) {
							RETVAL_STR_COPY(str_idx);
						} else {
							RETVAL_LONG(num_idx);
						}
						return;
					}
				}
			} ZEND_HASH_FOREACH_END();
 		}
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ Checks if the given value exists in the array */
PHP_FUNCTION(in_array)
{
	php_search_array(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Searches the array for a given value and returns the corresponding key if successful */
PHP_FUNCTION(array_search)
{
	php_search_array(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

static zend_always_inline int php_valid_var_name(const char *var_name, size_t var_name_len) /* {{{ */
{
#if 1
	/* first 256 bits for first character, and second 256 bits for the next */
	static const uint32_t charset[8] = {
	     /*  31      0   63     32   95     64   127    96 */
			0x00000000, 0x00000000, 0x87fffffe, 0x07fffffe,
			0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
	static const uint32_t charset2[8] = {
	     /*  31      0   63     32   95     64   127    96 */
			0x00000000, 0x03ff0000, 0x87fffffe, 0x07fffffe,
			0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
#endif
	size_t i;
	uint32_t ch;

	if (UNEXPECTED(!var_name_len)) {
		return 0;
	}

	/* These are allowed as first char: [a-zA-Z_\x7f-\xff] */
	ch = (uint32_t)((unsigned char *)var_name)[0];
#if 1
	if (UNEXPECTED(!ZEND_BIT_TEST(charset, ch))) {
#else
	if (var_name[0] != '_' &&
		(ch < 65  /* A    */ || /* Z    */ ch > 90)  &&
		(ch < 97  /* a    */ || /* z    */ ch > 122) &&
		(ch < 127 /* 0x7f */ || /* 0xff */ ch > 255)
	) {
#endif
		return 0;
	}

	/* And these as the rest: [a-zA-Z0-9_\x7f-\xff] */
	if (var_name_len > 1) {
		i = 1;
		do {
			ch = (uint32_t)((unsigned char *)var_name)[i];
#if 1
			if (UNEXPECTED(!ZEND_BIT_TEST(charset2, ch))) {
#else
			if (var_name[i] != '_' &&
				(ch < 48  /* 0    */ || /* 9    */ ch > 57)  &&
				(ch < 65  /* A    */ || /* Z    */ ch > 90)  &&
				(ch < 97  /* a    */ || /* z    */ ch > 122) &&
				(ch < 127 /* 0x7f */ || /* 0xff */ ch > 255)
			) {
#endif
				return 0;
			}
		} while (++i < var_name_len);
	}
	return 1;
}
/* }}} */

PHPAPI int php_prefix_varname(zval *result, zend_string *prefix, const char *var_name, size_t var_name_len, zend_bool add_underscore) /* {{{ */
{
	ZVAL_NEW_STR(result, zend_string_alloc(ZSTR_LEN(prefix) + (add_underscore ? 1 : 0) + var_name_len, 0));
	memcpy(Z_STRVAL_P(result), ZSTR_VAL(prefix), ZSTR_LEN(prefix));

	if (add_underscore) {
		Z_STRVAL_P(result)[ZSTR_LEN(prefix)] = '_';
	}

	memcpy(Z_STRVAL_P(result) + ZSTR_LEN(prefix) + (add_underscore ? 1 : 0), var_name, var_name_len + 1);

	return SUCCESS;
}
/* }}} */

static zend_long php_extract_ref_if_exists(zend_array *arr, zend_array *symbol_table) /* {{{ */
{
	zend_long count = 0;
	zend_string *var_name;
	zval *entry, *orig_var;

	ZEND_HASH_FOREACH_STR_KEY_VAL_IND(arr, var_name, entry) {
		if (!var_name) {
			continue;
		}
		orig_var = zend_hash_find_ex(symbol_table, var_name, 1);
		if (orig_var) {
			if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
				orig_var = Z_INDIRECT_P(orig_var);
				if (Z_TYPE_P(orig_var) == IS_UNDEF) {
					continue;
				}
			}
			if (!php_valid_var_name(ZSTR_VAL(var_name), ZSTR_LEN(var_name))) {
				continue;
			}
			if (zend_string_equals_literal(var_name, "GLOBALS")) {
				continue;
			}
			if (zend_string_equals_literal(var_name, "this")) {
				zend_throw_error(NULL, "Cannot re-assign $this");
				return -1;
			}
			if (Z_ISREF_P(entry)) {
				Z_ADDREF_P(entry);
			} else {
				ZVAL_MAKE_REF_EX(entry, 2);
			}
			zval_ptr_dtor(orig_var);
			ZVAL_REF(orig_var, Z_REF_P(entry));
			count++;
		}
	} ZEND_HASH_FOREACH_END();

	return count;
}
/* }}} */

static zend_long php_extract_if_exists(zend_array *arr, zend_array *symbol_table) /* {{{ */
{
	zend_long count = 0;
	zend_string *var_name;
	zval *entry, *orig_var;

	ZEND_HASH_FOREACH_STR_KEY_VAL_IND(arr, var_name, entry) {
		if (!var_name) {
			continue;
		}
		orig_var = zend_hash_find_ex(symbol_table, var_name, 1);
		if (orig_var) {
			if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
				orig_var = Z_INDIRECT_P(orig_var);
				if (Z_TYPE_P(orig_var) == IS_UNDEF) {
					continue;
				}
			}
			if (!php_valid_var_name(ZSTR_VAL(var_name), ZSTR_LEN(var_name))) {
				continue;
			}
			if (zend_string_equals_literal(var_name, "GLOBALS")) {
				continue;
			}
			if (zend_string_equals_literal(var_name, "this")) {
				zend_throw_error(NULL, "Cannot re-assign $this");
				return -1;
			}
			ZVAL_DEREF(entry);
			ZEND_TRY_ASSIGN_COPY_EX(orig_var, entry, 0);
			if (UNEXPECTED(EG(exception))) {
				return -1;
			}
			count++;
		}
	} ZEND_HASH_FOREACH_END();

	return count;
}
/* }}} */

static zend_long php_extract_ref_overwrite(zend_array *arr, zend_array *symbol_table) /* {{{ */
{
	zend_long count = 0;
	zend_string *var_name;
	zval *entry, *orig_var;

	ZEND_HASH_FOREACH_STR_KEY_VAL_IND(arr, var_name, entry) {
		if (!var_name) {
			continue;
		}
		if (!php_valid_var_name(ZSTR_VAL(var_name), ZSTR_LEN(var_name))) {
			continue;
		}
		if (zend_string_equals_literal(var_name, "this")) {
			zend_throw_error(NULL, "Cannot re-assign $this");
			return -1;
		}
		orig_var = zend_hash_find_ex(symbol_table, var_name, 1);
		if (orig_var) {
			if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
				orig_var = Z_INDIRECT_P(orig_var);
			}
			if (zend_string_equals_literal(var_name, "GLOBALS")) {
				continue;
			}
			if (Z_ISREF_P(entry)) {
				Z_ADDREF_P(entry);
			} else {
				ZVAL_MAKE_REF_EX(entry, 2);
			}
			zval_ptr_dtor(orig_var);
			ZVAL_REF(orig_var, Z_REF_P(entry));
		} else {
			if (Z_ISREF_P(entry)) {
				Z_ADDREF_P(entry);
			} else {
				ZVAL_MAKE_REF_EX(entry, 2);
			}
			zend_hash_add_new(symbol_table, var_name, entry);
		}
		count++;
	} ZEND_HASH_FOREACH_END();

	return count;
}
/* }}} */

static zend_long php_extract_overwrite(zend_array *arr, zend_array *symbol_table) /* {{{ */
{
	zend_long count = 0;
	zend_string *var_name;
	zval *entry, *orig_var;

	ZEND_HASH_FOREACH_STR_KEY_VAL_IND(arr, var_name, entry) {
		if (!var_name) {
			continue;
		}
		if (!php_valid_var_name(ZSTR_VAL(var_name), ZSTR_LEN(var_name))) {
			continue;
		}
		if (zend_string_equals_literal(var_name, "this")) {
			zend_throw_error(NULL, "Cannot re-assign $this");
			return -1;
		}
		orig_var = zend_hash_find_ex(symbol_table, var_name, 1);
		if (orig_var) {
			if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
				orig_var = Z_INDIRECT_P(orig_var);
			}
			if (zend_string_equals_literal(var_name, "GLOBALS")) {
				continue;
			}
			ZVAL_DEREF(entry);
			ZEND_TRY_ASSIGN_COPY_EX(orig_var, entry, 0);
			if (UNEXPECTED(EG(exception))) {
				return -1;
			}
		} else {
			ZVAL_DEREF(entry);
			Z_TRY_ADDREF_P(entry);
			zend_hash_add_new(symbol_table, var_name, entry);
		}
		count++;
	} ZEND_HASH_FOREACH_END();

	return count;
}
/* }}} */

static zend_long php_extract_ref_prefix_if_exists(zend_array *arr, zend_array *symbol_table, zend_string *prefix) /* {{{ */
{
	zend_long count = 0;
	zend_string *var_name;
	zval *entry, *orig_var, final_name;

	ZEND_HASH_FOREACH_STR_KEY_VAL_IND(arr, var_name, entry) {
		if (!var_name) {
			continue;
		}
		orig_var = zend_hash_find_ex(symbol_table, var_name, 1);
		if (orig_var) {
			if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
				orig_var = Z_INDIRECT_P(orig_var);
				if (Z_TYPE_P(orig_var) == IS_UNDEF) {
					if (Z_ISREF_P(entry)) {
						Z_ADDREF_P(entry);
					} else {
						ZVAL_MAKE_REF_EX(entry, 2);
					}
					ZVAL_REF(orig_var, Z_REF_P(entry));
					count++;
					continue;
				}
			}
			php_prefix_varname(&final_name, prefix, ZSTR_VAL(var_name), ZSTR_LEN(var_name), 1);
			if (php_valid_var_name(Z_STRVAL(final_name), Z_STRLEN(final_name))) {
				if (zend_string_equals_literal(Z_STR(final_name), "this")) {
					zend_throw_error(NULL, "Cannot re-assign $this");
					return -1;
				} else {
					if (Z_ISREF_P(entry)) {
						Z_ADDREF_P(entry);
					} else {
						ZVAL_MAKE_REF_EX(entry, 2);
					}
					if ((orig_var = zend_hash_find(symbol_table, Z_STR(final_name))) != NULL) {
						if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
							orig_var = Z_INDIRECT_P(orig_var);
						}
						zval_ptr_dtor(orig_var);
						ZVAL_REF(orig_var, Z_REF_P(entry));
					} else {
						zend_hash_add_new(symbol_table, Z_STR(final_name), entry);
					}
					count++;
				}
			}
			zval_ptr_dtor_str(&final_name);
		}
	} ZEND_HASH_FOREACH_END();

	return count;
}
/* }}} */

static zend_long php_extract_prefix_if_exists(zend_array *arr, zend_array *symbol_table, zend_string *prefix) /* {{{ */
{
	zend_long count = 0;
	zend_string *var_name;
	zval *entry, *orig_var, final_name;

	ZEND_HASH_FOREACH_STR_KEY_VAL_IND(arr, var_name, entry) {
		if (!var_name) {
			continue;
		}
		orig_var = zend_hash_find_ex(symbol_table, var_name, 1);
		if (orig_var) {
			if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
				orig_var = Z_INDIRECT_P(orig_var);
				if (Z_TYPE_P(orig_var) == IS_UNDEF) {
					ZVAL_COPY_DEREF(orig_var, entry);
					count++;
					continue;
				}
			}
			php_prefix_varname(&final_name, prefix, ZSTR_VAL(var_name), ZSTR_LEN(var_name), 1);
			if (php_valid_var_name(Z_STRVAL(final_name), Z_STRLEN(final_name))) {
				if (zend_string_equals_literal(Z_STR(final_name), "this")) {
					zend_throw_error(NULL, "Cannot re-assign $this");
					return -1;
				} else {
					ZVAL_DEREF(entry);
					if ((orig_var = zend_hash_find(symbol_table, Z_STR(final_name))) != NULL) {
						if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
							orig_var = Z_INDIRECT_P(orig_var);
						}
						ZEND_TRY_ASSIGN_COPY_EX(orig_var, entry, 0);
						if (UNEXPECTED(EG(exception))) {
							zend_string_release_ex(Z_STR(final_name), 0);
							return -1;
						}
					} else {
						Z_TRY_ADDREF_P(entry);
						zend_hash_add_new(symbol_table, Z_STR(final_name), entry);
					}
					count++;
				}
			}
			zval_ptr_dtor_str(&final_name);
		}
	} ZEND_HASH_FOREACH_END();

	return count;
}
/* }}} */

static zend_long php_extract_ref_prefix_same(zend_array *arr, zend_array *symbol_table, zend_string *prefix) /* {{{ */
{
	zend_long count = 0;
	zend_string *var_name;
	zval *entry, *orig_var, final_name;

	ZEND_HASH_FOREACH_STR_KEY_VAL_IND(arr, var_name, entry) {
		if (!var_name) {
			continue;
		}
		if (ZSTR_LEN(var_name) == 0) {
			continue;
		}
		orig_var = zend_hash_find_ex(symbol_table, var_name, 1);
		if (orig_var) {
			if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
				orig_var = Z_INDIRECT_P(orig_var);
				if (Z_TYPE_P(orig_var) == IS_UNDEF) {
					if (Z_ISREF_P(entry)) {
						Z_ADDREF_P(entry);
					} else {
						ZVAL_MAKE_REF_EX(entry, 2);
					}
					ZVAL_REF(orig_var, Z_REF_P(entry));
					count++;
					continue;
				}
			}
prefix:
			php_prefix_varname(&final_name, prefix, ZSTR_VAL(var_name), ZSTR_LEN(var_name), 1);
			if (php_valid_var_name(Z_STRVAL(final_name), Z_STRLEN(final_name))) {
				if (zend_string_equals_literal(Z_STR(final_name), "this")) {
					zend_throw_error(NULL, "Cannot re-assign $this");
					return -1;
				} else {
					if (Z_ISREF_P(entry)) {
						Z_ADDREF_P(entry);
					} else {
						ZVAL_MAKE_REF_EX(entry, 2);
					}
					if ((orig_var = zend_hash_find(symbol_table, Z_STR(final_name))) != NULL) {
						if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
							orig_var = Z_INDIRECT_P(orig_var);
						}
						zval_ptr_dtor(orig_var);
						ZVAL_REF(orig_var, Z_REF_P(entry));
					} else {
						zend_hash_add_new(symbol_table, Z_STR(final_name), entry);
					}
					count++;
				}
			}
			zval_ptr_dtor_str(&final_name);
		} else {
			if (!php_valid_var_name(ZSTR_VAL(var_name), ZSTR_LEN(var_name))) {
				continue;
			}
			if (zend_string_equals_literal(var_name, "this")) {
				goto prefix;
			}
			if (Z_ISREF_P(entry)) {
				Z_ADDREF_P(entry);
			} else {
				ZVAL_MAKE_REF_EX(entry, 2);
			}
			zend_hash_add_new(symbol_table, var_name, entry);
			count++;
		}
	} ZEND_HASH_FOREACH_END();

	return count;
}
/* }}} */

static zend_long php_extract_prefix_same(zend_array *arr, zend_array *symbol_table, zend_string *prefix) /* {{{ */
{
	zend_long count = 0;
	zend_string *var_name;
	zval *entry, *orig_var, final_name;

	ZEND_HASH_FOREACH_STR_KEY_VAL_IND(arr, var_name, entry) {
		if (!var_name) {
			continue;
		}
		if (ZSTR_LEN(var_name) == 0) {
			continue;
		}
		orig_var = zend_hash_find_ex(symbol_table, var_name, 1);
		if (orig_var) {
			if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
				orig_var = Z_INDIRECT_P(orig_var);
				if (Z_TYPE_P(orig_var) == IS_UNDEF) {
					ZVAL_COPY_DEREF(orig_var, entry);
					count++;
					continue;
				}
			}
prefix:
			php_prefix_varname(&final_name, prefix, ZSTR_VAL(var_name), ZSTR_LEN(var_name), 1);
			if (php_valid_var_name(Z_STRVAL(final_name), Z_STRLEN(final_name))) {
				if (zend_string_equals_literal(Z_STR(final_name), "this")) {
					zend_throw_error(NULL, "Cannot re-assign $this");
					return -1;
				} else {
					ZVAL_DEREF(entry);
					if ((orig_var = zend_hash_find(symbol_table, Z_STR(final_name))) != NULL) {
						if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
							orig_var = Z_INDIRECT_P(orig_var);
						}
						ZEND_TRY_ASSIGN_COPY_EX(orig_var, entry, 0);
						if (UNEXPECTED(EG(exception))) {
							zend_string_release_ex(Z_STR(final_name), 0);
							return -1;
						}
					} else {
						Z_TRY_ADDREF_P(entry);
						zend_hash_add_new(symbol_table, Z_STR(final_name), entry);
					}
					count++;
				}
			}
			zval_ptr_dtor_str(&final_name);
		} else {
			if (!php_valid_var_name(ZSTR_VAL(var_name), ZSTR_LEN(var_name))) {
				continue;
			}
			if (zend_string_equals_literal(var_name, "this")) {
				goto prefix;
			}
			ZVAL_DEREF(entry);
			Z_TRY_ADDREF_P(entry);
			zend_hash_add_new(symbol_table, var_name, entry);
			count++;
		}
	} ZEND_HASH_FOREACH_END();

	return count;
}
/* }}} */

static zend_long php_extract_ref_prefix_all(zend_array *arr, zend_array *symbol_table, zend_string *prefix) /* {{{ */
{
	zend_long count = 0;
	zend_string *var_name;
	zend_ulong num_key;
	zval *entry, *orig_var, final_name;

	ZEND_HASH_FOREACH_KEY_VAL_IND(arr, num_key, var_name, entry) {
		if (var_name) {
			if (ZSTR_LEN(var_name) == 0) {
				continue;
			}
			php_prefix_varname(&final_name, prefix, ZSTR_VAL(var_name), ZSTR_LEN(var_name), 1);
		} else {
			zend_string *str = zend_long_to_str(num_key);
			php_prefix_varname(&final_name, prefix, ZSTR_VAL(str), ZSTR_LEN(str), 1);
			zend_string_release_ex(str, 0);
		}
		if (php_valid_var_name(Z_STRVAL(final_name), Z_STRLEN(final_name))) {
			if (zend_string_equals_literal(Z_STR(final_name), "this")) {
				zend_throw_error(NULL, "Cannot re-assign $this");
				return -1;
			} else {
				if (Z_ISREF_P(entry)) {
					Z_ADDREF_P(entry);
				} else {
					ZVAL_MAKE_REF_EX(entry, 2);
				}
				if ((orig_var = zend_hash_find(symbol_table, Z_STR(final_name))) != NULL) {
					if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
						orig_var = Z_INDIRECT_P(orig_var);
					}
					zval_ptr_dtor(orig_var);
					ZVAL_REF(orig_var, Z_REF_P(entry));
				} else {
					zend_hash_add_new(symbol_table, Z_STR(final_name), entry);
				}
				count++;
			}
		}
		zval_ptr_dtor_str(&final_name);
	} ZEND_HASH_FOREACH_END();

	return count;
}
/* }}} */

static zend_long php_extract_prefix_all(zend_array *arr, zend_array *symbol_table, zend_string *prefix) /* {{{ */
{
	zend_long count = 0;
	zend_string *var_name;
	zend_ulong num_key;
	zval *entry, *orig_var, final_name;

	ZEND_HASH_FOREACH_KEY_VAL_IND(arr, num_key, var_name, entry) {
		if (var_name) {
			if (ZSTR_LEN(var_name) == 0) {
				continue;
			}
			php_prefix_varname(&final_name, prefix, ZSTR_VAL(var_name), ZSTR_LEN(var_name), 1);
		} else {
			zend_string *str = zend_long_to_str(num_key);
			php_prefix_varname(&final_name, prefix, ZSTR_VAL(str), ZSTR_LEN(str), 1);
			zend_string_release_ex(str, 0);
		}
		if (php_valid_var_name(Z_STRVAL(final_name), Z_STRLEN(final_name))) {
			if (zend_string_equals_literal(Z_STR(final_name), "this")) {
				zend_throw_error(NULL, "Cannot re-assign $this");
				return -1;
			} else {
				ZVAL_DEREF(entry);
				if ((orig_var = zend_hash_find(symbol_table, Z_STR(final_name))) != NULL) {
					if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
						orig_var = Z_INDIRECT_P(orig_var);
					}
					ZEND_TRY_ASSIGN_COPY_EX(orig_var, entry, 0);
					if (UNEXPECTED(EG(exception))) {
						zend_string_release_ex(Z_STR(final_name), 0);
						return -1;
					}
				} else {
					Z_TRY_ADDREF_P(entry);
					zend_hash_add_new(symbol_table, Z_STR(final_name), entry);
				}
				count++;
			}
		}
		zval_ptr_dtor_str(&final_name);
	} ZEND_HASH_FOREACH_END();

	return count;
}
/* }}} */

static zend_long php_extract_ref_prefix_invalid(zend_array *arr, zend_array *symbol_table, zend_string *prefix) /* {{{ */
{
	zend_long count = 0;
	zend_string *var_name;
	zend_ulong num_key;
	zval *entry, *orig_var, final_name;

	ZEND_HASH_FOREACH_KEY_VAL_IND(arr, num_key, var_name, entry) {
		if (var_name) {
			if (!php_valid_var_name(ZSTR_VAL(var_name), ZSTR_LEN(var_name))
			 || zend_string_equals_literal(var_name, "this")) {
				php_prefix_varname(&final_name, prefix, ZSTR_VAL(var_name), ZSTR_LEN(var_name), 1);
				if (!php_valid_var_name(Z_STRVAL(final_name), Z_STRLEN(final_name))) {
					zval_ptr_dtor_str(&final_name);
					continue;
				}
			} else {
				ZVAL_STR_COPY(&final_name, var_name);
			}
		} else {
			zend_string *str = zend_long_to_str(num_key);
			php_prefix_varname(&final_name, prefix, ZSTR_VAL(str), ZSTR_LEN(str), 1);
			zend_string_release_ex(str, 0);
			if (!php_valid_var_name(Z_STRVAL(final_name), Z_STRLEN(final_name))) {
				zval_ptr_dtor_str(&final_name);
				continue;
			}
		}
		if (zend_string_equals_literal(Z_STR(final_name), "this")) {
			zend_throw_error(NULL, "Cannot re-assign $this");
			return -1;
		} else {
			if (Z_ISREF_P(entry)) {
				Z_ADDREF_P(entry);
			} else {
				ZVAL_MAKE_REF_EX(entry, 2);
			}
			if ((orig_var = zend_hash_find(symbol_table, Z_STR(final_name))) != NULL) {
				if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
					orig_var = Z_INDIRECT_P(orig_var);
				}
				zval_ptr_dtor(orig_var);
				ZVAL_REF(orig_var, Z_REF_P(entry));
			} else {
				zend_hash_add_new(symbol_table, Z_STR(final_name), entry);
			}
			count++;
		}
		zval_ptr_dtor_str(&final_name);
	} ZEND_HASH_FOREACH_END();

	return count;
}
/* }}} */

static zend_long php_extract_prefix_invalid(zend_array *arr, zend_array *symbol_table, zend_string *prefix) /* {{{ */
{
	zend_long count = 0;
	zend_string *var_name;
	zend_ulong num_key;
	zval *entry, *orig_var, final_name;

	ZEND_HASH_FOREACH_KEY_VAL_IND(arr, num_key, var_name, entry) {
		if (var_name) {
			if (!php_valid_var_name(ZSTR_VAL(var_name), ZSTR_LEN(var_name))
			 || zend_string_equals_literal(var_name, "this")) {
				php_prefix_varname(&final_name, prefix, ZSTR_VAL(var_name), ZSTR_LEN(var_name), 1);
				if (!php_valid_var_name(Z_STRVAL(final_name), Z_STRLEN(final_name))) {
					zval_ptr_dtor_str(&final_name);
					continue;
				}
			} else {
				ZVAL_STR_COPY(&final_name, var_name);
			}
		} else {
			zend_string *str = zend_long_to_str(num_key);
			php_prefix_varname(&final_name, prefix, ZSTR_VAL(str), ZSTR_LEN(str), 1);
			zend_string_release_ex(str, 0);
			if (!php_valid_var_name(Z_STRVAL(final_name), Z_STRLEN(final_name))) {
				zval_ptr_dtor_str(&final_name);
				continue;
			}
		}
		if (zend_string_equals_literal(Z_STR(final_name), "this")) {
			zend_throw_error(NULL, "Cannot re-assign $this");
			return -1;
		} else {
			ZVAL_DEREF(entry);
			if ((orig_var = zend_hash_find(symbol_table, Z_STR(final_name))) != NULL) {
				if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
					orig_var = Z_INDIRECT_P(orig_var);
				}
				ZEND_TRY_ASSIGN_COPY_EX(orig_var, entry, 0);
				if (UNEXPECTED(EG(exception))) {
					zend_string_release_ex(Z_STR(final_name), 0);
					return -1;
				}
			} else {
				Z_TRY_ADDREF_P(entry);
				zend_hash_add_new(symbol_table, Z_STR(final_name), entry);
			}
			count++;
		}
		zval_ptr_dtor_str(&final_name);
	} ZEND_HASH_FOREACH_END();

	return count;
}
/* }}} */

static zend_long php_extract_ref_skip(zend_array *arr, zend_array *symbol_table) /* {{{ */
{
	zend_long count = 0;
	zend_string *var_name;
	zval *entry, *orig_var;

	ZEND_HASH_FOREACH_STR_KEY_VAL_IND(arr, var_name, entry) {
		if (!var_name) {
			continue;
		}
		if (!php_valid_var_name(ZSTR_VAL(var_name), ZSTR_LEN(var_name))) {
			continue;
		}
		if (zend_string_equals_literal(var_name, "this")) {
			continue;
		}
		orig_var = zend_hash_find_ex(symbol_table, var_name, 1);
		if (orig_var) {
			if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
				orig_var = Z_INDIRECT_P(orig_var);
				if (Z_TYPE_P(orig_var) == IS_UNDEF) {
					if (Z_ISREF_P(entry)) {
						Z_ADDREF_P(entry);
					} else {
						ZVAL_MAKE_REF_EX(entry, 2);
					}
					ZVAL_REF(orig_var, Z_REF_P(entry));
					count++;
				}
			}
		} else {
			if (Z_ISREF_P(entry)) {
				Z_ADDREF_P(entry);
			} else {
				ZVAL_MAKE_REF_EX(entry, 2);
			}
			zend_hash_add_new(symbol_table, var_name, entry);
			count++;
		}
	} ZEND_HASH_FOREACH_END();

	return count;
}
/* }}} */

static zend_long php_extract_skip(zend_array *arr, zend_array *symbol_table) /* {{{ */
{
	zend_long count = 0;
	zend_string *var_name;
	zval *entry, *orig_var;

	ZEND_HASH_FOREACH_STR_KEY_VAL_IND(arr, var_name, entry) {
		if (!var_name) {
			continue;
		}
		if (!php_valid_var_name(ZSTR_VAL(var_name), ZSTR_LEN(var_name))) {
			continue;
		}
		if (zend_string_equals_literal(var_name, "this")) {
			continue;
		}
		orig_var = zend_hash_find_ex(symbol_table, var_name, 1);
		if (orig_var) {
			if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
				orig_var = Z_INDIRECT_P(orig_var);
				if (Z_TYPE_P(orig_var) == IS_UNDEF) {
					ZVAL_COPY_DEREF(orig_var, entry);
					count++;
				}
			}
		} else {
			ZVAL_DEREF(entry);
			Z_TRY_ADDREF_P(entry);
			zend_hash_add_new(symbol_table, var_name, entry);
			count++;
		}
	} ZEND_HASH_FOREACH_END();

	return count;
}
/* }}} */

/* {{{ Imports variables into symbol table from an array */
PHP_FUNCTION(extract)
{
	zval *var_array_param;
	zend_long extract_refs;
	zend_long extract_type = EXTR_OVERWRITE;
	zend_string *prefix = NULL;
	zend_long count;
	zend_array *symbol_table;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_ARRAY_EX2(var_array_param, 0, 1, 0)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(extract_type)
		Z_PARAM_STR(prefix)
	ZEND_PARSE_PARAMETERS_END();

	extract_refs = (extract_type & EXTR_REFS);
	if (extract_refs) {
		SEPARATE_ARRAY(var_array_param);
	}
	extract_type &= 0xff;

	if (extract_type < EXTR_OVERWRITE || extract_type > EXTR_IF_EXISTS) {
		zend_argument_value_error(2, "must be a valid extract type");
		RETURN_THROWS();
	}

	if (extract_type > EXTR_SKIP && extract_type <= EXTR_PREFIX_IF_EXISTS && ZEND_NUM_ARGS() < 3) {
		zend_argument_value_error(3, "is required when using this extract type");
		RETURN_THROWS();
	}

	if (prefix) {
		if (ZSTR_LEN(prefix) && !php_valid_var_name(ZSTR_VAL(prefix), ZSTR_LEN(prefix))) {
			zend_argument_value_error(3, "must be a valid identifier");
			RETURN_THROWS();
		}
	}

	if (zend_forbid_dynamic_call("extract()") == FAILURE) {
		return;
	}

	symbol_table = zend_rebuild_symbol_table();
	ZEND_ASSERT(symbol_table && "A symbol table should always be available here");

	if (extract_refs) {
		switch (extract_type) {
			case EXTR_IF_EXISTS:
				count = php_extract_ref_if_exists(Z_ARRVAL_P(var_array_param), symbol_table);
				break;
			case EXTR_OVERWRITE:
				count = php_extract_ref_overwrite(Z_ARRVAL_P(var_array_param), symbol_table);
				break;
			case EXTR_PREFIX_IF_EXISTS:
				count = php_extract_ref_prefix_if_exists(Z_ARRVAL_P(var_array_param), symbol_table, prefix);
				break;
			case EXTR_PREFIX_SAME:
				count = php_extract_ref_prefix_same(Z_ARRVAL_P(var_array_param), symbol_table, prefix);
				break;
			case EXTR_PREFIX_ALL:
				count = php_extract_ref_prefix_all(Z_ARRVAL_P(var_array_param), symbol_table, prefix);
				break;
			case EXTR_PREFIX_INVALID:
				count = php_extract_ref_prefix_invalid(Z_ARRVAL_P(var_array_param), symbol_table, prefix);
				break;
			default:
				count = php_extract_ref_skip(Z_ARRVAL_P(var_array_param), symbol_table);
				break;
		}
	} else {
		/* The array might be stored in a local variable that will be overwritten */
		zval array_copy;
		ZVAL_COPY(&array_copy, var_array_param);
		switch (extract_type) {
			case EXTR_IF_EXISTS:
				count = php_extract_if_exists(Z_ARRVAL(array_copy), symbol_table);
				break;
			case EXTR_OVERWRITE:
				count = php_extract_overwrite(Z_ARRVAL(array_copy), symbol_table);
				break;
			case EXTR_PREFIX_IF_EXISTS:
				count = php_extract_prefix_if_exists(Z_ARRVAL(array_copy), symbol_table, prefix);
				break;
			case EXTR_PREFIX_SAME:
				count = php_extract_prefix_same(Z_ARRVAL(array_copy), symbol_table, prefix);
				break;
			case EXTR_PREFIX_ALL:
				count = php_extract_prefix_all(Z_ARRVAL(array_copy), symbol_table, prefix);
				break;
			case EXTR_PREFIX_INVALID:
				count = php_extract_prefix_invalid(Z_ARRVAL(array_copy), symbol_table, prefix);
				break;
			default:
				count = php_extract_skip(Z_ARRVAL(array_copy), symbol_table);
				break;
		}
		zval_ptr_dtor(&array_copy);
	}

	RETURN_LONG(count);
}
/* }}} */

static void php_compact_var(HashTable *eg_active_symbol_table, zval *return_value, zval *entry) /* {{{ */
{
	zval *value_ptr, data;

	ZVAL_DEREF(entry);
	if (Z_TYPE_P(entry) == IS_STRING) {
		if ((value_ptr = zend_hash_find_ind(eg_active_symbol_table, Z_STR_P(entry))) != NULL) {
			ZVAL_DEREF(value_ptr);
			Z_TRY_ADDREF_P(value_ptr);
			zend_hash_update(Z_ARRVAL_P(return_value), Z_STR_P(entry), value_ptr);
		} else if (zend_string_equals_literal(Z_STR_P(entry), "this")) {
			zend_object *object = zend_get_this_object(EG(current_execute_data));
			if (object) {
				ZVAL_OBJ_COPY(&data, object);
				zend_hash_update(Z_ARRVAL_P(return_value), Z_STR_P(entry), &data);
			}
		} else {
			php_error_docref(NULL, E_WARNING, "Undefined variable $%s", ZSTR_VAL(Z_STR_P(entry)));
		}
	} else if (Z_TYPE_P(entry) == IS_ARRAY) {
	    if (Z_REFCOUNTED_P(entry)) {
			if (Z_IS_RECURSIVE_P(entry)) {
				zend_throw_error(NULL, "Recursion detected");
				return;
			}
			Z_PROTECT_RECURSION_P(entry);
		}
		ZEND_HASH_FOREACH_VAL_IND(Z_ARRVAL_P(entry), value_ptr) {
			php_compact_var(eg_active_symbol_table, return_value, value_ptr);
		} ZEND_HASH_FOREACH_END();
	    if (Z_REFCOUNTED_P(entry)) {
			Z_UNPROTECT_RECURSION_P(entry);
		}
	}
}
/* }}} */

/* {{{ Creates a hash containing variables and their values */
PHP_FUNCTION(compact)
{
	zval *args = NULL;	/* function arguments array */
	uint32_t num_args, i;
	zend_array *symbol_table;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_VARIADIC('+', args, num_args)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_forbid_dynamic_call("compact()") == FAILURE) {
		return;
	}

	symbol_table = zend_rebuild_symbol_table();
	ZEND_ASSERT(symbol_table && "A symbol table should always be available here");

	/* compact() is probably most used with a single array of var_names
	   or multiple string names, rather than a combination of both.
	   So quickly guess a minimum result size based on that */
	if (num_args && Z_TYPE(args[0]) == IS_ARRAY) {
		array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL(args[0])));
	} else {
		array_init_size(return_value, num_args);
	}

	for (i = 0; i < num_args; i++) {
		php_compact_var(symbol_table, return_value, &args[i]);
	}
}
/* }}} */

/* {{{ Create an array containing num elements starting with index start_key each initialized to val */
PHP_FUNCTION(array_fill)
{
	zval *val;
	zend_long start_key, num;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(start_key)
		Z_PARAM_LONG(num)
		Z_PARAM_ZVAL(val)
	ZEND_PARSE_PARAMETERS_END();

	if (EXPECTED(num > 0)) {
		if (sizeof(num) > 4 && UNEXPECTED(EXPECTED(num > 0x7fffffff))) {
			zend_argument_value_error(2, "is too large");
			RETURN_THROWS();
		} else if (UNEXPECTED(start_key > ZEND_LONG_MAX - num + 1)) {
			zend_throw_error(NULL, "Cannot add element to the array as the next element is already occupied");
			RETURN_THROWS();
		} else if (EXPECTED(start_key >= 0) && EXPECTED(start_key < num)) {
			/* create packed array */
			Bucket *p;
			zend_long n;

			array_init_size(return_value, (uint32_t)(start_key + num));
			zend_hash_real_init_packed(Z_ARRVAL_P(return_value));
			Z_ARRVAL_P(return_value)->nNumUsed = (uint32_t)(start_key + num);
			Z_ARRVAL_P(return_value)->nNumOfElements = (uint32_t)num;
			Z_ARRVAL_P(return_value)->nNextFreeElement = (zend_long)(start_key + num);

			if (Z_REFCOUNTED_P(val)) {
				GC_ADDREF_EX(Z_COUNTED_P(val), (uint32_t)num);
			}

			p = Z_ARRVAL_P(return_value)->arData;
			n = start_key;

			while (start_key--) {
				ZVAL_UNDEF(&p->val);
				p++;
			}
			while (num--) {
				ZVAL_COPY_VALUE(&p->val, val);
				p->h = n++;
				p->key = NULL;
				p++;
			}
		} else {
			/* create hash */
			array_init_size(return_value, (uint32_t)num);
			zend_hash_real_init_mixed(Z_ARRVAL_P(return_value));
			if (Z_REFCOUNTED_P(val)) {
				GC_ADDREF_EX(Z_COUNTED_P(val), (uint32_t)num);
			}
			zend_hash_index_add_new(Z_ARRVAL_P(return_value), start_key, val);
			while (--num) {
				zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), val);
				start_key++;
			}
		}
	} else if (EXPECTED(num == 0)) {
		RETURN_EMPTY_ARRAY();
	} else {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ Create an array using the elements of the first parameter as keys each initialized to val */
PHP_FUNCTION(array_fill_keys)
{
	zval *keys, *val, *entry;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ARRAY(keys)
		Z_PARAM_ZVAL(val)
	ZEND_PARSE_PARAMETERS_END();

	/* Initialize return array */
	array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_P(keys)));

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(keys), entry) {
		ZVAL_DEREF(entry);
		Z_TRY_ADDREF_P(val);
		if (Z_TYPE_P(entry) == IS_LONG) {
			zend_hash_index_update(Z_ARRVAL_P(return_value), Z_LVAL_P(entry), val);
		} else {
			zend_string *tmp_key;
			zend_string *key = zval_get_tmp_string(entry, &tmp_key);
			zend_symtable_update(Z_ARRVAL_P(return_value), key, val);
			zend_tmp_string_release(tmp_key);
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

#define RANGE_CHECK_DOUBLE_INIT_ARRAY(start, end) do { \
		double __calc_size = ((start - end) / step) + 1; \
		if (__calc_size >= (double)HT_MAX_SIZE) { \
			zend_value_error(\
					"The supplied range exceeds the maximum array size: start=%0.0f end=%0.0f", end, start); \
			RETURN_THROWS(); \
		} \
		size = (uint32_t)_php_math_round(__calc_size, 0, PHP_ROUND_HALF_UP); \
		array_init_size(return_value, size); \
		zend_hash_real_init_packed(Z_ARRVAL_P(return_value)); \
	} while (0)

#define RANGE_CHECK_LONG_INIT_ARRAY(start, end) do { \
		zend_ulong __calc_size = ((zend_ulong) start - end) / lstep; \
		if (__calc_size >= HT_MAX_SIZE - 1) { \
			zend_value_error(\
					"The supplied range exceeds the maximum array size: start=" ZEND_LONG_FMT " end=" ZEND_LONG_FMT, end, start); \
			RETURN_THROWS(); \
		} \
		size = (uint32_t)(__calc_size + 1); \
		array_init_size(return_value, size); \
		zend_hash_real_init_packed(Z_ARRVAL_P(return_value)); \
	} while (0)

/* {{{ Create an array containing the range of integers or characters from low to high (inclusive) */
PHP_FUNCTION(range)
{
	zval *zlow, *zhigh, *zstep = NULL, tmp;
	int err = 0, is_step_double = 0;
	double step = 1.0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ZVAL(zlow)
		Z_PARAM_ZVAL(zhigh)
		Z_PARAM_OPTIONAL
		Z_PARAM_NUMBER(zstep)
	ZEND_PARSE_PARAMETERS_END();

	if (zstep) {
		is_step_double = Z_TYPE_P(zstep) == IS_DOUBLE;
		step = zval_get_double(zstep);

		/* We only want positive step values. */
		if (step < 0.0) {
			step *= -1;
		}
	}

	/* If the range is given as strings, generate an array of characters. */
	if (Z_TYPE_P(zlow) == IS_STRING && Z_TYPE_P(zhigh) == IS_STRING && Z_STRLEN_P(zlow) >= 1 && Z_STRLEN_P(zhigh) >= 1) {
		int type1, type2;
		unsigned char low, high;
		zend_long lstep = (zend_long) step;

		type1 = is_numeric_string(Z_STRVAL_P(zlow), Z_STRLEN_P(zlow), NULL, NULL, 0);
		type2 = is_numeric_string(Z_STRVAL_P(zhigh), Z_STRLEN_P(zhigh), NULL, NULL, 0);

		if (type1 == IS_DOUBLE || type2 == IS_DOUBLE || is_step_double) {
			goto double_str;
		} else if (type1 == IS_LONG || type2 == IS_LONG) {
			goto long_str;
		}

		low = (unsigned char)Z_STRVAL_P(zlow)[0];
		high = (unsigned char)Z_STRVAL_P(zhigh)[0];

		if (low > high) {		/* Negative Steps */
			if (lstep <= 0) {
				err = 1;
				goto err;
			}
			/* Initialize the return_value as an array. */
			array_init_size(return_value, (uint32_t)(((low - high) / lstep) + 1));
			zend_hash_real_init_packed(Z_ARRVAL_P(return_value));
			ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
				for (; low >= high; low -= (unsigned int)lstep) {
					ZEND_HASH_FILL_SET_INTERNED_STR(ZSTR_CHAR(low));
					ZEND_HASH_FILL_NEXT();
					if (((signed int)low - lstep) < 0) {
						break;
					}
				}
			} ZEND_HASH_FILL_END();
		} else if (high > low) {	/* Positive Steps */
			if (lstep <= 0) {
				err = 1;
				goto err;
			}
			array_init_size(return_value, (uint32_t)(((high - low) / lstep) + 1));
			zend_hash_real_init_packed(Z_ARRVAL_P(return_value));
			ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
				for (; low <= high; low += (unsigned int)lstep) {
					ZEND_HASH_FILL_SET_INTERNED_STR(ZSTR_CHAR(low));
					ZEND_HASH_FILL_NEXT();
					if (((signed int)low + lstep) > 255) {
						break;
					}
				}
			} ZEND_HASH_FILL_END();
		} else {
			array_init(return_value);
			ZVAL_CHAR(&tmp, low);
			zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &tmp);
		}
	} else if (Z_TYPE_P(zlow) == IS_DOUBLE || Z_TYPE_P(zhigh) == IS_DOUBLE || is_step_double) {
		double low, high, element;
		uint32_t i, size;
double_str:
		low = zval_get_double(zlow);
		high = zval_get_double(zhigh);

		if (zend_isinf(high) || zend_isinf(low)) {
			zend_value_error("Invalid range supplied: start=%0.0f end=%0.0f", low, high);
			RETURN_THROWS();
		}

		if (low > high) { 		/* Negative steps */
			if (low - high < step || step <= 0) {
				err = 1;
				goto err;
			}

			RANGE_CHECK_DOUBLE_INIT_ARRAY(low, high);

			ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
				for (i = 0, element = low; i < size && element >= high; ++i, element = low - (i * step)) {
					ZEND_HASH_FILL_SET_DOUBLE(element);
					ZEND_HASH_FILL_NEXT();
				}
			} ZEND_HASH_FILL_END();
		} else if (high > low) { 	/* Positive steps */
			if (high - low < step || step <= 0) {
				err = 1;
				goto err;
			}

			RANGE_CHECK_DOUBLE_INIT_ARRAY(high, low);

			ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
				for (i = 0, element = low; i < size && element <= high; ++i, element = low + (i * step)) {
					ZEND_HASH_FILL_SET_DOUBLE(element);
					ZEND_HASH_FILL_NEXT();
				}
			} ZEND_HASH_FILL_END();
		} else {
			array_init(return_value);
			ZVAL_DOUBLE(&tmp, low);
			zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &tmp);
		}
	} else {
		zend_long low, high;
		/* lstep is a zend_ulong so that comparisons to it don't overflow, i.e. low - high < lstep */
		zend_ulong lstep;
		uint32_t i, size;
long_str:
		low = zval_get_long(zlow);
		high = zval_get_long(zhigh);

		if (step <= 0) {
			err = 1;
			goto err;
		}

		lstep = (zend_ulong)step;
		if (step <= 0) {
			err = 1;
			goto err;
		}

		if (low > high) { 		/* Negative steps */
			if ((zend_ulong)low - high < lstep) {
				err = 1;
				goto err;
			}

			RANGE_CHECK_LONG_INIT_ARRAY(low, high);

			ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
				for (i = 0; i < size; ++i) {
					ZEND_HASH_FILL_SET_LONG(low - (i * lstep));
					ZEND_HASH_FILL_NEXT();
				}
			} ZEND_HASH_FILL_END();
		} else if (high > low) { 	/* Positive steps */
			if ((zend_ulong)high - low < lstep) {
				err = 1;
				goto err;
			}

			RANGE_CHECK_LONG_INIT_ARRAY(high, low);

			ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
				for (i = 0; i < size; ++i) {
					ZEND_HASH_FILL_SET_LONG(low + (i * lstep));
					ZEND_HASH_FILL_NEXT();
				}
			} ZEND_HASH_FILL_END();
		} else {
			array_init(return_value);
			ZVAL_LONG(&tmp, low);
			zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &tmp);
		}
	}
err:
	if (err) {
		zend_argument_value_error(3, "must not exceed the specified range");
		RETURN_THROWS();
	}
}
/* }}} */

#undef RANGE_CHECK_DOUBLE_INIT_ARRAY
#undef RANGE_CHECK_LONG_INIT_ARRAY

static void php_array_data_shuffle(zval *array) /* {{{ */
{
	uint32_t idx, j, n_elems;
	Bucket *p, temp;
	HashTable *hash;
	zend_long rnd_idx;
	uint32_t n_left;

	n_elems = zend_hash_num_elements(Z_ARRVAL_P(array));

	if (n_elems < 1) {
		return;
	}

	hash = Z_ARRVAL_P(array);
	n_left = n_elems;

	if (EXPECTED(!HT_HAS_ITERATORS(hash))) {
		if (hash->nNumUsed != hash->nNumOfElements) {
			for (j = 0, idx = 0; idx < hash->nNumUsed; idx++) {
				p = hash->arData + idx;
				if (Z_TYPE(p->val) == IS_UNDEF) continue;
				if (j != idx) {
					hash->arData[j] = *p;
				}
				j++;
			}
		}
		while (--n_left) {
			rnd_idx = php_mt_rand_range(0, n_left);
			if (rnd_idx != n_left) {
				temp = hash->arData[n_left];
				hash->arData[n_left] = hash->arData[rnd_idx];
				hash->arData[rnd_idx] = temp;
			}
		}
	} else {
		uint32_t iter_pos = zend_hash_iterators_lower_pos(hash, 0);

		if (hash->nNumUsed != hash->nNumOfElements) {
			for (j = 0, idx = 0; idx < hash->nNumUsed; idx++) {
				p = hash->arData + idx;
				if (Z_TYPE(p->val) == IS_UNDEF) continue;
				if (j != idx) {
					hash->arData[j] = *p;
					if (idx == iter_pos) {
						zend_hash_iterators_update(hash, idx, j);
						iter_pos = zend_hash_iterators_lower_pos(hash, iter_pos + 1);
					}
				}
				j++;
			}
		}
		while (--n_left) {
			rnd_idx = php_mt_rand_range(0, n_left);
			if (rnd_idx != n_left) {
				temp = hash->arData[n_left];
				hash->arData[n_left] = hash->arData[rnd_idx];
				hash->arData[rnd_idx] = temp;
				zend_hash_iterators_update(hash, (uint32_t)rnd_idx, n_left);
			}
		}
	}
	hash->nNumUsed = n_elems;
	hash->nInternalPointer = 0;

	for (j = 0; j < n_elems; j++) {
		p = hash->arData + j;
		if (p->key) {
			zend_string_release_ex(p->key, 0);
		}
		p->h = j;
		p->key = NULL;
	}
	hash->nNextFreeElement = n_elems;
	if (!(HT_FLAGS(hash) & HASH_FLAG_PACKED)) {
		zend_hash_to_packed(hash);
	}
}
/* }}} */

/* {{{ Randomly shuffle the contents of an array */
PHP_FUNCTION(shuffle)
{
	zval *array;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_EX(array, 0, 1)
	ZEND_PARSE_PARAMETERS_END();

	php_array_data_shuffle(array);

	RETURN_TRUE;
}
/* }}} */

static void php_splice(HashTable *in_hash, zend_long offset, zend_long length, HashTable *replace, HashTable *removed) /* {{{ */
{
	HashTable 	 out_hash;			/* Output hashtable */
	zend_long	 num_in;			/* Number of entries in the input hashtable */
	zend_long	 pos;				/* Current position in the hashtable */
	uint32_t     idx;
	Bucket		*p;					/* Pointer to hash bucket */
	zval		*entry;				/* Hash entry */
	uint32_t    iter_pos = zend_hash_iterators_lower_pos(in_hash, 0);

	/* Get number of entries in the input hash */
	num_in = zend_hash_num_elements(in_hash);

	/* Clamp the offset.. */
	if (offset > num_in) {
		offset = num_in;
	} else if (offset < 0 && (offset = (num_in + offset)) < 0) {
		offset = 0;
	}

	/* ..and the length */
	if (length < 0) {
		length = num_in - offset + length;
	} else if (((unsigned)offset + (unsigned)length) > (unsigned)num_in) {
		length = num_in - offset;
	}

	/* Create and initialize output hash */
	zend_hash_init(&out_hash, (length > 0 ? num_in - length : 0) + (replace ? zend_hash_num_elements(replace) : 0), NULL, ZVAL_PTR_DTOR, 0);

	/* Start at the beginning of the input hash and copy entries to output hash until offset is reached */
	for (pos = 0, idx = 0; pos < offset && idx < in_hash->nNumUsed; idx++) {
		p = in_hash->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		/* Get entry and increase reference count */
		entry = &p->val;

		/* Update output hash depending on key type */
		if (p->key == NULL) {
			zend_hash_next_index_insert_new(&out_hash, entry);
		} else {
			zend_hash_add_new(&out_hash, p->key, entry);
		}
		if (idx == iter_pos) {
			if ((zend_long)idx != pos) {
				zend_hash_iterators_update(in_hash, idx, pos);
			}
			iter_pos = zend_hash_iterators_lower_pos(in_hash, iter_pos + 1);
		}
		pos++;
	}

	/* If hash for removed entries exists, go until offset+length and copy the entries to it */
	if (removed != NULL) {
		for ( ; pos < offset + length && idx < in_hash->nNumUsed; idx++) {
			p = in_hash->arData + idx;
			if (Z_TYPE(p->val) == IS_UNDEF) continue;
			pos++;
			entry = &p->val;
			Z_TRY_ADDREF_P(entry);
			if (p->key == NULL) {
				zend_hash_next_index_insert_new(removed, entry);
				zend_hash_del_bucket(in_hash, p);
			} else {
				zend_hash_add_new(removed, p->key, entry);
				if (in_hash == &EG(symbol_table)) {
					zend_delete_global_variable(p->key);
				} else {
					zend_hash_del_bucket(in_hash, p);
				}
			}
		}
	} else { /* otherwise just skip those entries */
		int pos2 = pos;

		for ( ; pos2 < offset + length && idx < in_hash->nNumUsed; idx++) {
			p = in_hash->arData + idx;
			if (Z_TYPE(p->val) == IS_UNDEF) continue;
			pos2++;
			if (p->key && in_hash == &EG(symbol_table)) {
				zend_delete_global_variable(p->key);
			} else {
				zend_hash_del_bucket(in_hash, p);
			}
		}
	}
	iter_pos = zend_hash_iterators_lower_pos(in_hash, iter_pos);

	/* If there are entries to insert.. */
	if (replace) {
		ZEND_HASH_FOREACH_VAL_IND(replace, entry) {
			Z_TRY_ADDREF_P(entry);
			zend_hash_next_index_insert_new(&out_hash, entry);
			pos++;
		} ZEND_HASH_FOREACH_END();
	}

	/* Copy the remaining input hash entries to the output hash */
	for ( ; idx < in_hash->nNumUsed ; idx++) {
		p = in_hash->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		entry = &p->val;
		if (p->key == NULL) {
			zend_hash_next_index_insert_new(&out_hash, entry);
		} else {
			zend_hash_add_new(&out_hash, p->key, entry);
		}
		if (idx == iter_pos) {
			if ((zend_long)idx != pos) {
				zend_hash_iterators_update(in_hash, idx, pos);
			}
			iter_pos = zend_hash_iterators_lower_pos(in_hash, iter_pos + 1);
		}
		pos++;
	}

	/* replace HashTable data */
	HT_SET_ITERATORS_COUNT(&out_hash, HT_ITERATORS_COUNT(in_hash));
	HT_SET_ITERATORS_COUNT(in_hash, 0);
	in_hash->pDestructor = NULL;
	zend_hash_destroy(in_hash);

	HT_FLAGS(in_hash)          = HT_FLAGS(&out_hash);
	in_hash->nTableSize        = out_hash.nTableSize;
	in_hash->nTableMask        = out_hash.nTableMask;
	in_hash->nNumUsed          = out_hash.nNumUsed;
	in_hash->nNumOfElements    = out_hash.nNumOfElements;
	in_hash->nNextFreeElement  = out_hash.nNextFreeElement;
	in_hash->arData            = out_hash.arData;
	in_hash->pDestructor       = out_hash.pDestructor;

	zend_hash_internal_pointer_reset(in_hash);
}
/* }}} */

/* {{{ Pushes elements onto the end of the array */
PHP_FUNCTION(array_push)
{
	zval   *args,		/* Function arguments array */
		   *stack,		/* Input array */
		    new_var;	/* Variable to be pushed */
	int i,				/* Loop counter */
		argc;			/* Number of function arguments */


	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_ARRAY_EX(stack, 0, 1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	/* For each subsequent argument, make it a reference, increase refcount, and add it to the end of the array */
	for (i = 0; i < argc; i++) {
		ZVAL_COPY(&new_var, &args[i]);

		if (zend_hash_next_index_insert(Z_ARRVAL_P(stack), &new_var) == NULL) {
			Z_TRY_DELREF(new_var);
			zend_throw_error(NULL, "Cannot add element to the array as the next element is already occupied");
			RETURN_THROWS();
		}
	}

	/* Clean up and return the number of values in the stack */
	RETVAL_LONG(zend_hash_num_elements(Z_ARRVAL_P(stack)));
}
/* }}} */

/* {{{ Pops an element off the end of the array */
PHP_FUNCTION(array_pop)
{
	zval *stack,	/* Input stack */
		 *val;		/* Value to be popped */
	uint32_t idx;
	Bucket *p;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_EX(stack, 0, 1)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_hash_num_elements(Z_ARRVAL_P(stack)) == 0) {
		return;
	}

	/* Get the last value and copy it into the return value */
	idx = Z_ARRVAL_P(stack)->nNumUsed;
	while (1) {
		if (idx == 0) {
			return;
		}
		idx--;
		p = Z_ARRVAL_P(stack)->arData + idx;
		val = &p->val;
		if (Z_TYPE_P(val) == IS_INDIRECT) {
			val = Z_INDIRECT_P(val);
		}
		if (Z_TYPE_P(val) != IS_UNDEF) {
			break;
		}
	}
	ZVAL_COPY_DEREF(return_value, val);

	if (!p->key && (zend_long)p->h == (Z_ARRVAL_P(stack)->nNextFreeElement - 1)) {
		Z_ARRVAL_P(stack)->nNextFreeElement = Z_ARRVAL_P(stack)->nNextFreeElement - 1;
	}

	/* Delete the last value */
	if (p->key && Z_ARRVAL_P(stack) == &EG(symbol_table)) {
		zend_delete_global_variable(p->key);
	} else {
		zend_hash_del_bucket(Z_ARRVAL_P(stack), p);
	}

	zend_hash_internal_pointer_reset(Z_ARRVAL_P(stack));
}
/* }}} */

/* {{{ Pops an element off the beginning of the array */
PHP_FUNCTION(array_shift)
{
	zval *stack,	/* Input stack */
		 *val;		/* Value to be popped */
	uint32_t idx;
	Bucket *p;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_EX(stack, 0, 1)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_hash_num_elements(Z_ARRVAL_P(stack)) == 0) {
		return;
	}

	/* Get the first value and copy it into the return value */
	idx = 0;
	while (1) {
		if (idx == Z_ARRVAL_P(stack)->nNumUsed) {
			return;
		}
		p = Z_ARRVAL_P(stack)->arData + idx;
		val = &p->val;
		if (Z_TYPE_P(val) == IS_INDIRECT) {
			val = Z_INDIRECT_P(val);
		}
		if (Z_TYPE_P(val) != IS_UNDEF) {
			break;
		}
		idx++;
	}
	ZVAL_COPY_DEREF(return_value, val);

	/* Delete the first value */
	if (p->key && Z_ARRVAL_P(stack) == &EG(symbol_table)) {
		zend_delete_global_variable(p->key);
	} else {
		zend_hash_del_bucket(Z_ARRVAL_P(stack), p);
	}

	/* re-index like it did before */
	if (HT_FLAGS(Z_ARRVAL_P(stack)) & HASH_FLAG_PACKED) {
		uint32_t k = 0;

		if (EXPECTED(!HT_HAS_ITERATORS(Z_ARRVAL_P(stack)))) {
			for (idx = 0; idx < Z_ARRVAL_P(stack)->nNumUsed; idx++) {
				p = Z_ARRVAL_P(stack)->arData + idx;
				if (Z_TYPE(p->val) == IS_UNDEF) continue;
				if (idx != k) {
					Bucket *q = Z_ARRVAL_P(stack)->arData + k;
					q->h = k;
					q->key = NULL;
					ZVAL_COPY_VALUE(&q->val, &p->val);
					ZVAL_UNDEF(&p->val);
				}
				k++;
			}
		} else {
			uint32_t iter_pos = zend_hash_iterators_lower_pos(Z_ARRVAL_P(stack), 0);

			for (idx = 0; idx < Z_ARRVAL_P(stack)->nNumUsed; idx++) {
				p = Z_ARRVAL_P(stack)->arData + idx;
				if (Z_TYPE(p->val) == IS_UNDEF) continue;
				if (idx != k) {
					Bucket *q = Z_ARRVAL_P(stack)->arData + k;
					q->h = k;
					q->key = NULL;
					ZVAL_COPY_VALUE(&q->val, &p->val);
					ZVAL_UNDEF(&p->val);
					if (idx == iter_pos) {
						zend_hash_iterators_update(Z_ARRVAL_P(stack), idx, k);
						iter_pos = zend_hash_iterators_lower_pos(Z_ARRVAL_P(stack), iter_pos + 1);
					}
				}
				k++;
			}
		}
		Z_ARRVAL_P(stack)->nNumUsed = k;
		Z_ARRVAL_P(stack)->nNextFreeElement = k;
	} else {
		uint32_t k = 0;
		int should_rehash = 0;

		for (idx = 0; idx < Z_ARRVAL_P(stack)->nNumUsed; idx++) {
			p = Z_ARRVAL_P(stack)->arData + idx;
			if (Z_TYPE(p->val) == IS_UNDEF) continue;
			if (p->key == NULL) {
				if (p->h != k) {
					p->h = k++;
					should_rehash = 1;
				} else {
					k++;
				}
			}
		}
		Z_ARRVAL_P(stack)->nNextFreeElement = k;
		if (should_rehash) {
			zend_hash_rehash(Z_ARRVAL_P(stack));
		}
	}

	zend_hash_internal_pointer_reset(Z_ARRVAL_P(stack));
}
/* }}} */

/* {{{ Pushes elements onto the beginning of the array */
PHP_FUNCTION(array_unshift)
{
	zval   *args,			/* Function arguments array */
		   *stack;			/* Input stack */
	HashTable new_hash;		/* New hashtable for the stack */
	int argc;				/* Number of function arguments */
	int i;
	zend_string *key;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_ARRAY_EX(stack, 0, 1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	zend_hash_init(&new_hash, zend_hash_num_elements(Z_ARRVAL_P(stack)) + argc, NULL, ZVAL_PTR_DTOR, 0);
	for (i = 0; i < argc; i++) {
		Z_TRY_ADDREF(args[i]);
		zend_hash_next_index_insert_new(&new_hash, &args[i]);
	}

	ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(stack), key, value) {
		if (key) {
			zend_hash_add_new(&new_hash, key, value);
		} else {
			zend_hash_next_index_insert_new(&new_hash, value);
		}
	} ZEND_HASH_FOREACH_END();

	if (UNEXPECTED(HT_HAS_ITERATORS(Z_ARRVAL_P(stack)))) {
		zend_hash_iterators_advance(Z_ARRVAL_P(stack), argc);
		HT_SET_ITERATORS_COUNT(&new_hash, HT_ITERATORS_COUNT(Z_ARRVAL_P(stack)));
		HT_SET_ITERATORS_COUNT(Z_ARRVAL_P(stack), 0);
	}

	/* replace HashTable data */
	Z_ARRVAL_P(stack)->pDestructor = NULL;
	zend_hash_destroy(Z_ARRVAL_P(stack));

	HT_FLAGS(Z_ARRVAL_P(stack))          = HT_FLAGS(&new_hash);
	Z_ARRVAL_P(stack)->nTableSize        = new_hash.nTableSize;
	Z_ARRVAL_P(stack)->nTableMask        = new_hash.nTableMask;
	Z_ARRVAL_P(stack)->nNumUsed          = new_hash.nNumUsed;
	Z_ARRVAL_P(stack)->nNumOfElements    = new_hash.nNumOfElements;
	Z_ARRVAL_P(stack)->nNextFreeElement  = new_hash.nNextFreeElement;
	Z_ARRVAL_P(stack)->arData            = new_hash.arData;
	Z_ARRVAL_P(stack)->pDestructor       = new_hash.pDestructor;

	zend_hash_internal_pointer_reset(Z_ARRVAL_P(stack));

	/* Clean up and return the number of elements in the stack */
	RETVAL_LONG(zend_hash_num_elements(Z_ARRVAL_P(stack)));
}
/* }}} */

/* {{{ Removes the elements designated by offset and length and replace them with supplied array */
PHP_FUNCTION(array_splice)
{
	zval *array,				/* Input array */
		 *repl_array = NULL;	/* Replacement array */
	HashTable  *rem_hash = NULL;
	zend_long offset,
			length = 0;
	zend_bool length_is_null = 1;
	int		num_in;				/* Number of elements in the input array */

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_ARRAY_EX(array, 0, 1)
		Z_PARAM_LONG(offset)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(length, length_is_null)
		Z_PARAM_ZVAL(repl_array)
	ZEND_PARSE_PARAMETERS_END();

	num_in = zend_hash_num_elements(Z_ARRVAL_P(array));

	if (length_is_null) {
		length = num_in;
	}

	if (ZEND_NUM_ARGS() == 4) {
		/* Make sure the last argument, if passed, is an array */
		convert_to_array_ex(repl_array);
	}

	/* Don't create the array of removed elements if it's not going
	 * to be used; e.g. only removing and/or replacing elements */
	if (USED_RET()) {
		zend_long size = length;

		/* Clamp the offset.. */
		if (offset > num_in) {
			offset = num_in;
		} else if (offset < 0 && (offset = (num_in + offset)) < 0) {
			offset = 0;
		}

		/* ..and the length */
		if (length < 0) {
			size = num_in - offset + length;
		} else if (((zend_ulong) offset + (zend_ulong) length) > (uint32_t) num_in) {
			size = num_in - offset;
		}

		/* Initialize return value */
		array_init_size(return_value, size > 0 ? (uint32_t)size : 0);
		rem_hash = Z_ARRVAL_P(return_value);
	} else {
		/* The return value will not be used, but make sure it still has the correct type. */
		RETVAL_EMPTY_ARRAY();
	}

	/* Perform splice */
	php_splice(Z_ARRVAL_P(array), offset, length, repl_array ? Z_ARRVAL_P(repl_array) : NULL, rem_hash);
}
/* }}} */

/* {{{ find_bucket_at_offset(HashTable* ht, zend_long offset)
   Finds the bucket at the given valid offset */
static inline Bucket* find_bucket_at_offset(HashTable* ht, zend_long offset)
{
	zend_long pos;
	Bucket *bucket;
	ZEND_ASSERT(offset >= 0 && offset <= ht->nNumOfElements);
	if (HT_IS_WITHOUT_HOLES(ht)) {
		/* There's no need to iterate over the array to filter out holes if there are no holes */
		/* This properly handles both packed and unpacked arrays. */
		return ht->arData + offset;
	}
	/* Otherwise, this code has to iterate over the HashTable and skip holes in the array. */
	pos = 0;
	ZEND_HASH_FOREACH_BUCKET(ht, bucket) {
		if (pos >= offset) {
			/* This is the bucket of the array element at the requested offset */
			return bucket;
		}
		++pos;
	} ZEND_HASH_FOREACH_END();

	/* Return a pointer to the end of the bucket array. */
	return ht->arData + ht->nNumUsed;
}
/* }}} */

/* {{{ Returns elements specified by offset and length */
PHP_FUNCTION(array_slice)
{
	zval *input;                  /* Input array */
	zval *entry;                  /* An array entry */
	zend_long offset;             /* Offset to get elements from */
	zend_long length = 0;         /* How many elements to get */
	zend_bool length_is_null = 1; /* Whether an explicit length has been omitted */
	zend_bool preserve_keys = 0;  /* Whether to preserve keys while copying to the new array */
	uint32_t num_in;              /* Number of elements in the input array */
	zend_string *string_key;
	zend_ulong num_key;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_ARRAY(input)
		Z_PARAM_LONG(offset)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(length, length_is_null)
		Z_PARAM_BOOL(preserve_keys)
	ZEND_PARSE_PARAMETERS_END();

	/* Get number of entries in the input hash */
	num_in = zend_hash_num_elements(Z_ARRVAL_P(input));

	/* We want all entries from offset to the end if length is not passed or is null */
	if (length_is_null) {
		length = num_in;
	}

	/* Clamp the offset.. */
	if (offset > (zend_long) num_in) {
		RETURN_EMPTY_ARRAY();
	} else if (offset < 0 && (offset = (num_in + offset)) < 0) {
		offset = 0;
	}

	/* ..and the length */
	if (length < 0) {
		length = num_in - offset + length;
	} else if (((zend_ulong) offset + (zend_ulong) length) > (unsigned) num_in) {
		length = num_in - offset;
	}

	if (length <= 0) {
		RETURN_EMPTY_ARRAY();
	}

	/* Initialize returned array */
	array_init_size(return_value, (uint32_t)length);

	// Contains modified variants of ZEND_HASH_FOREACH_VAL
	{
		HashTable *ht = Z_ARRVAL_P(input);
		Bucket *p = find_bucket_at_offset(ht, offset);
		Bucket *end = ht->arData + ht->nNumUsed;

		/* Start at the beginning and go until we hit offset */
		if (HT_IS_PACKED(Z_ARRVAL_P(input)) &&
			(!preserve_keys ||
			 (offset == 0 && HT_IS_WITHOUT_HOLES(Z_ARRVAL_P(input))))) {

			zend_hash_real_init_packed(Z_ARRVAL_P(return_value));
			ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
				for (; p != end; p++) {
					if (__fill_idx >= length) {
						break;
					}
					entry = &p->val;
					if (UNEXPECTED(Z_TYPE_P(entry) == IS_UNDEF)) {
						continue;
					}
					if (UNEXPECTED(Z_ISREF_P(entry)) &&
						UNEXPECTED(Z_REFCOUNT_P(entry) == 1)) {
						entry = Z_REFVAL_P(entry);
					}
					Z_TRY_ADDREF_P(entry);
					ZEND_HASH_FILL_ADD(entry);
				}
			} ZEND_HASH_FILL_END();
		} else {
			zend_long n = 0;  /* Current number of elements */
			for (; p != end; p++) {
				entry = &p->val;
				if (UNEXPECTED(Z_TYPE_P(entry) == IS_UNDEF)) {
					continue;
				}
				if (n >= length) {
					break;
				}
				n++;
				num_key = p->h;
				string_key = p->key;

				if (string_key) {
					entry = zend_hash_add_new(Z_ARRVAL_P(return_value), string_key, entry);
				} else {
					if (preserve_keys) {
						entry = zend_hash_index_add_new(Z_ARRVAL_P(return_value), num_key, entry);
					} else {
						entry = zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), entry);
					}
				}
				zval_add_ref(entry);
			}
		}
	}
}
/* }}} */

PHPAPI int php_array_merge_recursive(HashTable *dest, HashTable *src) /* {{{ */
{
	zval *src_entry, *dest_entry;
	zend_string *string_key;

	ZEND_HASH_FOREACH_STR_KEY_VAL(src, string_key, src_entry) {
		if (string_key) {
			if ((dest_entry = zend_hash_find_ex(dest, string_key, 1)) != NULL) {
				zval *src_zval = src_entry;
				zval *dest_zval = dest_entry;
				HashTable *thash;
				zval tmp;
				int ret;

				ZVAL_DEREF(src_zval);
				ZVAL_DEREF(dest_zval);
				thash = Z_TYPE_P(dest_zval) == IS_ARRAY ? Z_ARRVAL_P(dest_zval) : NULL;
				if ((thash && GC_IS_RECURSIVE(thash)) || (src_entry == dest_entry && Z_ISREF_P(dest_entry) && (Z_REFCOUNT_P(dest_entry) % 2))) {
					zend_throw_error(NULL, "Recursion detected");
					return 0;
				}

				ZEND_ASSERT(!Z_ISREF_P(dest_entry) || Z_REFCOUNT_P(dest_entry) > 1);
				SEPARATE_ZVAL(dest_entry);
				dest_zval = dest_entry;

				if (Z_TYPE_P(dest_zval) == IS_NULL) {
					convert_to_array_ex(dest_zval);
					add_next_index_null(dest_zval);
				} else {
					convert_to_array_ex(dest_zval);
				}
				ZVAL_UNDEF(&tmp);
				if (Z_TYPE_P(src_zval) == IS_OBJECT) {
					ZVAL_COPY(&tmp, src_zval);
					convert_to_array(&tmp);
					src_zval = &tmp;
				}
				if (Z_TYPE_P(src_zval) == IS_ARRAY) {
					if (thash) {
						GC_TRY_PROTECT_RECURSION(thash);
					}
					ret = php_array_merge_recursive(Z_ARRVAL_P(dest_zval), Z_ARRVAL_P(src_zval));
					if (thash) {
						GC_TRY_UNPROTECT_RECURSION(thash);
					}
					if (!ret) {
						return 0;
					}
				} else {
					Z_TRY_ADDREF_P(src_zval);
					zend_hash_next_index_insert(Z_ARRVAL_P(dest_zval), src_zval);
				}
				zval_ptr_dtor(&tmp);
			} else {
				zval *zv = zend_hash_add_new(dest, string_key, src_entry);
				zval_add_ref(zv);
			}
		} else {
			zval *zv = zend_hash_next_index_insert(dest, src_entry);
			zval_add_ref(zv);
		}
	} ZEND_HASH_FOREACH_END();
	return 1;
}
/* }}} */

PHPAPI int php_array_merge(HashTable *dest, HashTable *src) /* {{{ */
{
	zval *src_entry;
	zend_string *string_key;

	if ((HT_FLAGS(dest) & HASH_FLAG_PACKED) && (HT_FLAGS(src) & HASH_FLAG_PACKED)) {
		zend_hash_extend(dest, zend_hash_num_elements(dest) + zend_hash_num_elements(src), 1);
		ZEND_HASH_FILL_PACKED(dest) {
			ZEND_HASH_FOREACH_VAL(src, src_entry) {
				if (UNEXPECTED(Z_ISREF_P(src_entry)) &&
					UNEXPECTED(Z_REFCOUNT_P(src_entry) == 1)) {
					src_entry = Z_REFVAL_P(src_entry);
				}
				Z_TRY_ADDREF_P(src_entry);
				ZEND_HASH_FILL_ADD(src_entry);
			} ZEND_HASH_FOREACH_END();
		} ZEND_HASH_FILL_END();
	} else {
		ZEND_HASH_FOREACH_STR_KEY_VAL(src, string_key, src_entry) {
			if (UNEXPECTED(Z_ISREF_P(src_entry) &&
				Z_REFCOUNT_P(src_entry) == 1)) {
				src_entry = Z_REFVAL_P(src_entry);
			}
			Z_TRY_ADDREF_P(src_entry);
			if (string_key) {
				zend_hash_update(dest, string_key, src_entry);
			} else {
				zend_hash_next_index_insert_new(dest, src_entry);
			}
		} ZEND_HASH_FOREACH_END();
	}
	return 1;
}
/* }}} */

PHPAPI int php_array_replace_recursive(HashTable *dest, HashTable *src) /* {{{ */
{
	zval *src_entry, *dest_entry, *src_zval, *dest_zval;
	zend_string *string_key;
	zend_ulong num_key;
	int ret;

	ZEND_HASH_FOREACH_KEY_VAL(src, num_key, string_key, src_entry) {
		src_zval = src_entry;
		ZVAL_DEREF(src_zval);
		if (string_key) {
			if (Z_TYPE_P(src_zval) != IS_ARRAY ||
				(dest_entry = zend_hash_find_ex(dest, string_key, 1)) == NULL ||
				(Z_TYPE_P(dest_entry) != IS_ARRAY &&
				 (!Z_ISREF_P(dest_entry) || Z_TYPE_P(Z_REFVAL_P(dest_entry)) != IS_ARRAY))) {

				zval *zv = zend_hash_update(dest, string_key, src_entry);
				zval_add_ref(zv);
				continue;
			}
		} else {
			if (Z_TYPE_P(src_zval) != IS_ARRAY ||
				(dest_entry = zend_hash_index_find(dest, num_key)) == NULL ||
				(Z_TYPE_P(dest_entry) != IS_ARRAY &&
				 (!Z_ISREF_P(dest_entry) || Z_TYPE_P(Z_REFVAL_P(dest_entry)) != IS_ARRAY))) {

				zval *zv = zend_hash_index_update(dest, num_key, src_entry);
				zval_add_ref(zv);
				continue;
			}
		}

		dest_zval = dest_entry;
		ZVAL_DEREF(dest_zval);
		if (Z_IS_RECURSIVE_P(dest_zval) ||
		    Z_IS_RECURSIVE_P(src_zval) ||
		    (Z_ISREF_P(src_entry) && Z_ISREF_P(dest_entry) && Z_REF_P(src_entry) == Z_REF_P(dest_entry) && (Z_REFCOUNT_P(dest_entry) % 2))) {
			zend_throw_error(NULL, "Recursion detected");
			return 0;
		}

		ZEND_ASSERT(!Z_ISREF_P(dest_entry) || Z_REFCOUNT_P(dest_entry) > 1);
		SEPARATE_ZVAL(dest_entry);
		dest_zval = dest_entry;

		if (Z_REFCOUNTED_P(dest_zval)) {
			Z_PROTECT_RECURSION_P(dest_zval);
		}
		if (Z_REFCOUNTED_P(src_zval)) {
			Z_PROTECT_RECURSION_P(src_zval);
		}

		ret = php_array_replace_recursive(Z_ARRVAL_P(dest_zval), Z_ARRVAL_P(src_zval));

		if (Z_REFCOUNTED_P(dest_zval)) {
			Z_UNPROTECT_RECURSION_P(dest_zval);
		}
		if (Z_REFCOUNTED_P(src_zval)) {
			Z_UNPROTECT_RECURSION_P(src_zval);
		}

		if (!ret) {
			return 0;
		}
	} ZEND_HASH_FOREACH_END();

	return 1;
}
/* }}} */

static zend_always_inline void php_array_replace_wrapper(INTERNAL_FUNCTION_PARAMETERS, int recursive) /* {{{ */
{
	zval *args = NULL;
	zval *arg;
	int argc, i;
	HashTable *dest;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();


	for (i = 0; i < argc; i++) {
		zval *arg = args + i;

		if (Z_TYPE_P(arg) != IS_ARRAY) {
			zend_argument_type_error(i + 1, "must be of type array, %s given", zend_zval_type_name(arg));
			RETURN_THROWS();
		}
	}

	/* copy first array */
	arg = args;
	dest = zend_array_dup(Z_ARRVAL_P(arg));
	ZVAL_ARR(return_value, dest);
	if (recursive) {
		for (i = 1; i < argc; i++) {
			arg = args + i;
			php_array_replace_recursive(dest, Z_ARRVAL_P(arg));
		}
	} else {
		for (i = 1; i < argc; i++) {
			arg = args + i;
			zend_hash_merge(dest, Z_ARRVAL_P(arg), zval_add_ref, 1);
		}
	}
}
/* }}} */

static zend_always_inline void php_array_merge_wrapper(INTERNAL_FUNCTION_PARAMETERS, int recursive) /* {{{ */
{
	zval *args = NULL;
	zval *arg;
	int argc, i;
	zval *src_entry;
	HashTable *src, *dest;
	uint32_t count = 0;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	if (argc == 0) {
		RETURN_EMPTY_ARRAY();
	}

	for (i = 0; i < argc; i++) {
		zval *arg = args + i;

		if (Z_TYPE_P(arg) != IS_ARRAY) {
			zend_argument_type_error(i + 1, "must be of type array, %s given", zend_zval_type_name(arg));
			RETURN_THROWS();
		}
		count += zend_hash_num_elements(Z_ARRVAL_P(arg));
	}

	if (argc == 2) {
		zval *ret = NULL;

		if (zend_hash_num_elements(Z_ARRVAL(args[0])) == 0) {
			ret = &args[1];
		} else if (zend_hash_num_elements(Z_ARRVAL(args[1])) == 0) {
			ret = &args[0];
		}
		if (ret) {
			if (HT_FLAGS(Z_ARRVAL_P(ret)) & HASH_FLAG_PACKED) {
				if (HT_IS_WITHOUT_HOLES(Z_ARRVAL_P(ret))) {
					ZVAL_COPY(return_value, ret);
					return;
				}
			} else {
				zend_bool copy = 1;
				zend_string *string_key;

				ZEND_HASH_FOREACH_STR_KEY(Z_ARRVAL_P(ret), string_key) {
					if (!string_key) {
						copy = 0;
						break;
					}
				} ZEND_HASH_FOREACH_END();
				if (copy) {
					ZVAL_COPY(return_value, ret);
					return;
				}
			}
		}
	}

	arg = args;
	src  = Z_ARRVAL_P(arg);
	/* copy first array */
	array_init_size(return_value, count);
	dest = Z_ARRVAL_P(return_value);
	if (HT_FLAGS(src) & HASH_FLAG_PACKED) {
		zend_hash_real_init_packed(dest);
		ZEND_HASH_FILL_PACKED(dest) {
			ZEND_HASH_FOREACH_VAL(src, src_entry) {
				if (UNEXPECTED(Z_ISREF_P(src_entry) &&
					Z_REFCOUNT_P(src_entry) == 1)) {
					src_entry = Z_REFVAL_P(src_entry);
				}
				Z_TRY_ADDREF_P(src_entry);
				ZEND_HASH_FILL_ADD(src_entry);
			} ZEND_HASH_FOREACH_END();
		} ZEND_HASH_FILL_END();
	} else {
		zend_string *string_key;
		zend_hash_real_init_mixed(dest);
		ZEND_HASH_FOREACH_STR_KEY_VAL(src, string_key, src_entry) {
			if (UNEXPECTED(Z_ISREF_P(src_entry) &&
				Z_REFCOUNT_P(src_entry) == 1)) {
				src_entry = Z_REFVAL_P(src_entry);
			}
			Z_TRY_ADDREF_P(src_entry);
			if (EXPECTED(string_key)) {
				_zend_hash_append(dest, string_key, src_entry);
			} else {
				zend_hash_next_index_insert_new(dest, src_entry);
			}
		} ZEND_HASH_FOREACH_END();
	}
	if (recursive) {
		for (i = 1; i < argc; i++) {
			arg = args + i;
			php_array_merge_recursive(dest, Z_ARRVAL_P(arg));
		}
	} else {
		for (i = 1; i < argc; i++) {
			arg = args + i;
			php_array_merge(dest, Z_ARRVAL_P(arg));
		}
	}
}
/* }}} */

/* {{{ Merges elements from passed arrays into one array */
PHP_FUNCTION(array_merge)
{
	php_array_merge_wrapper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Recursively merges elements from passed arrays into one array */
PHP_FUNCTION(array_merge_recursive)
{
	php_array_merge_wrapper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Replaces elements from passed arrays into one array */
PHP_FUNCTION(array_replace)
{
	php_array_replace_wrapper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Recursively replaces elements from passed arrays into one array */
PHP_FUNCTION(array_replace_recursive)
{
	php_array_replace_wrapper(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Return just the keys from the input array, optionally only for the specified search_value */
PHP_FUNCTION(array_keys)
{
	zval *input,				/* Input array */
	     *search_value = NULL,	/* Value to search for */
	     *entry,				/* An entry in the input array */
	       new_val;				/* New value */
	zend_bool strict = 0;		/* do strict comparison */
	zend_ulong num_idx;
	zend_string *str_idx;
	zend_array *arrval;
	zend_ulong elem_count;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_ARRAY(input)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(search_value)
		Z_PARAM_BOOL(strict)
	ZEND_PARSE_PARAMETERS_END();
	arrval = Z_ARRVAL_P(input);
	elem_count = zend_hash_num_elements(arrval);

	/* Base case: empty input */
	if (!elem_count) {
		RETURN_COPY(input);
	}

	/* Initialize return array */
	if (search_value != NULL) {
		array_init(return_value);

		if (strict) {
			ZEND_HASH_FOREACH_KEY_VAL_IND(arrval, num_idx, str_idx, entry) {
				ZVAL_DEREF(entry);
				if (fast_is_identical_function(search_value, entry)) {
					if (str_idx) {
						ZVAL_STR_COPY(&new_val, str_idx);
					} else {
						ZVAL_LONG(&new_val, num_idx);
					}
					zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &new_val);
				}
			} ZEND_HASH_FOREACH_END();
		} else {
			ZEND_HASH_FOREACH_KEY_VAL_IND(arrval, num_idx, str_idx, entry) {
				if (fast_equal_check_function(search_value, entry)) {
					if (str_idx) {
						ZVAL_STR_COPY(&new_val, str_idx);
					} else {
						ZVAL_LONG(&new_val, num_idx);
					}
					zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &new_val);
				}
			} ZEND_HASH_FOREACH_END();
		}
	} else {
		array_init_size(return_value, elem_count);
		zend_hash_real_init_packed(Z_ARRVAL_P(return_value));
		ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
			if (HT_IS_PACKED(arrval) && HT_IS_WITHOUT_HOLES(arrval)) {
				/* Optimistic case: range(0..n-1) for vector-like packed array */
				zend_ulong lval = 0;

				for (; lval < elem_count; ++lval) {
					ZEND_HASH_FILL_SET_LONG(lval);
					ZEND_HASH_FILL_NEXT();
				}
			} else {
				/* Go through input array and add keys to the return array */
				ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL_P(input), num_idx, str_idx, entry) {
					if (str_idx) {
						ZEND_HASH_FILL_SET_STR_COPY(str_idx);
					} else {
						ZEND_HASH_FILL_SET_LONG(num_idx);
					}
					ZEND_HASH_FILL_NEXT();
				} ZEND_HASH_FOREACH_END();
			}
		} ZEND_HASH_FILL_END();
	}
}
/* }}} */

/* {{{ Get the key of the first element of the array */
PHP_FUNCTION(array_key_first)
{
	zval *stack;    /* Input stack */

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(stack)
	ZEND_PARSE_PARAMETERS_END();

	HashTable *target_hash = Z_ARRVAL_P (stack);
	HashPosition pos = 0;
	zend_hash_get_current_key_zval_ex(target_hash, return_value, &pos);
}
/* }}} */

/* {{{ Get the key of the last element of the array */
PHP_FUNCTION(array_key_last)
{
	zval *stack;    /* Input stack */
	HashPosition pos;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(stack)
	ZEND_PARSE_PARAMETERS_END();

	HashTable *target_hash = Z_ARRVAL_P (stack);
	zend_hash_internal_pointer_end_ex(target_hash, &pos);
	zend_hash_get_current_key_zval_ex(target_hash, return_value, &pos);
}
/* }}} */

/* {{{ Return just the values from the input array */
PHP_FUNCTION(array_values)
{
	zval	 *input,		/* Input array */
			 *entry;		/* An entry in the input array */
	zend_array *arrval;
	zend_long arrlen;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(input)
	ZEND_PARSE_PARAMETERS_END();

	arrval = Z_ARRVAL_P(input);

	/* Return empty input as is */
	arrlen = zend_hash_num_elements(arrval);
	if (!arrlen) {
		RETURN_EMPTY_ARRAY();
	}

	/* Return vector-like packed arrays as-is */
	if (HT_IS_PACKED(arrval) && HT_IS_WITHOUT_HOLES(arrval) &&
		arrval->nNextFreeElement == arrlen) {
		RETURN_COPY(input);
	}

	/* Initialize return array */
	array_init_size(return_value, arrlen);
	zend_hash_real_init_packed(Z_ARRVAL_P(return_value));

	/* Go through input array and add values to the return array */
	ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
		ZEND_HASH_FOREACH_VAL(arrval, entry) {
			if (UNEXPECTED(Z_ISREF_P(entry) && Z_REFCOUNT_P(entry) == 1)) {
				entry = Z_REFVAL_P(entry);
			}
			Z_TRY_ADDREF_P(entry);
			ZEND_HASH_FILL_ADD(entry);
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FILL_END();
}
/* }}} */

/* {{{ Return the value as key and the frequency of that value in input as value */
PHP_FUNCTION(array_count_values)
{
	zval	*input,		/* Input array */
			*entry,		/* An entry in the input array */
			*tmp;
	HashTable *myht;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(input)
	ZEND_PARSE_PARAMETERS_END();

	/* Initialize return array */
	array_init(return_value);

	/* Go through input array and add values to the return array */
	myht = Z_ARRVAL_P(input);
	ZEND_HASH_FOREACH_VAL(myht, entry) {
		ZVAL_DEREF(entry);
		if (Z_TYPE_P(entry) == IS_LONG) {
			if ((tmp = zend_hash_index_find(Z_ARRVAL_P(return_value), Z_LVAL_P(entry))) == NULL) {
				zval data;
				ZVAL_LONG(&data, 1);
				zend_hash_index_update(Z_ARRVAL_P(return_value), Z_LVAL_P(entry), &data);
			} else {
				Z_LVAL_P(tmp)++;
			}
		} else if (Z_TYPE_P(entry) == IS_STRING) {
			if ((tmp = zend_symtable_find(Z_ARRVAL_P(return_value), Z_STR_P(entry))) == NULL) {
				zval data;
				ZVAL_LONG(&data, 1);
				zend_symtable_update(Z_ARRVAL_P(return_value), Z_STR_P(entry), &data);
			} else {
				Z_LVAL_P(tmp)++;
			}
		} else {
			php_error_docref(NULL, E_WARNING, "Can only count string and integer values, entry skipped");
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

static inline zval *array_column_fetch_prop(zval *data, zend_string *name_str, zend_long name_long, zval *rv) /* {{{ */
{
	zval *prop = NULL;

	if (Z_TYPE_P(data) == IS_OBJECT) {
		zend_string *tmp_str;
		/* If name is an integer convert integer to string */
		if (name_str == NULL) {
			tmp_str = zend_long_to_str(name_long);
		} else {
			tmp_str = zend_string_copy(name_str);
		}
		/* The has_property check is first performed in "exists" mode (which returns true for
		 * properties that are null but exist) and then in "has" mode to handle objects that
		 * implement __isset (which is not called in "exists" mode). */
		if (Z_OBJ_HANDLER_P(data, has_property)(Z_OBJ_P(data), tmp_str, ZEND_PROPERTY_EXISTS, NULL)
				|| Z_OBJ_HANDLER_P(data, has_property)(Z_OBJ_P(data), tmp_str, ZEND_PROPERTY_ISSET, NULL)) {
			prop = Z_OBJ_HANDLER_P(data, read_property)(Z_OBJ_P(data), tmp_str, BP_VAR_R, NULL, rv);
			if (prop) {
				ZVAL_DEREF(prop);
				if (prop != rv) {
					Z_TRY_ADDREF_P(prop);
				}
			}
		}
		zend_string_release(tmp_str);
	} else if (Z_TYPE_P(data) == IS_ARRAY) {
		/* Name is a string */
		if (name_str != NULL) {
			prop = zend_symtable_find(Z_ARRVAL_P(data), name_str);
		} else {
			prop = zend_hash_index_find(Z_ARRVAL_P(data), name_long);
		}
		if (prop) {
			ZVAL_DEREF(prop);
			Z_TRY_ADDREF_P(prop);
		}
	}

	return prop;
}
/* }}} */

/* {{{ Return the values from a single column in the input array, identified by the
   value_key and optionally indexed by the index_key */
PHP_FUNCTION(array_column)
{
	HashTable *input;
	zval *colval, *data, rv;
	zend_string *column_str = NULL;
	zend_long column_long;
	zend_bool column_is_null = 0;
	zend_string *index_str = NULL;
	zend_long index_long;
	zend_bool index_is_null = 1;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ARRAY_HT(input)
		Z_PARAM_STR_OR_LONG_OR_NULL(column_str, column_long, column_is_null)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_LONG_OR_NULL(index_str, index_long, index_is_null)
	ZEND_PARSE_PARAMETERS_END();

	array_init_size(return_value, zend_hash_num_elements(input));
	/* Index param is not passed */
	if (index_is_null) {
		zend_hash_real_init_packed(Z_ARRVAL_P(return_value));
		ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
			ZEND_HASH_FOREACH_VAL(input, data) {
				ZVAL_DEREF(data);
				if (column_is_null) {
					Z_TRY_ADDREF_P(data);
					colval = data;
				} else if ((colval = array_column_fetch_prop(data, column_str, column_long, &rv)) == NULL) {
					continue;
				}
				ZEND_HASH_FILL_ADD(colval);
			} ZEND_HASH_FOREACH_END();
		} ZEND_HASH_FILL_END();
	} else {
		ZEND_HASH_FOREACH_VAL(input, data) {
			ZVAL_DEREF(data);

			if (column_is_null) {
				Z_TRY_ADDREF_P(data);
				colval = data;
			} else if ((colval = array_column_fetch_prop(data, column_str, column_long, &rv)) == NULL) {
				continue;
			}

			zval rv;
			zval *keyval = array_column_fetch_prop(data, index_str, index_long, &rv);
			if (keyval) {
				array_set_zval_key(Z_ARRVAL_P(return_value), keyval, colval);
				zval_ptr_dtor(colval);
				zval_ptr_dtor(keyval);
			} else {
				zend_hash_next_index_insert(Z_ARRVAL_P(return_value), colval);
			}
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

/* {{{ Return input as a new array with the order of the entries reversed */
PHP_FUNCTION(array_reverse)
{
	zval	 *input,				/* Input array */
			 *entry;				/* An entry in the input array */
	zend_string *string_key;
	zend_ulong	  num_key;
	zend_bool preserve_keys = 0;	/* whether to preserve keys */

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY(input)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(preserve_keys)
	ZEND_PARSE_PARAMETERS_END();

	/* Initialize return array */
	array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_P(input)));
	if ((HT_FLAGS(Z_ARRVAL_P(input)) & HASH_FLAG_PACKED) && !preserve_keys) {
		zend_hash_real_init_packed(Z_ARRVAL_P(return_value));
		ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
			ZEND_HASH_REVERSE_FOREACH_VAL(Z_ARRVAL_P(input), entry) {
				if (UNEXPECTED(Z_ISREF_P(entry) &&
					Z_REFCOUNT_P(entry) == 1)) {
					entry = Z_REFVAL_P(entry);
				}
				Z_TRY_ADDREF_P(entry);
				ZEND_HASH_FILL_ADD(entry);
			} ZEND_HASH_FOREACH_END();
		} ZEND_HASH_FILL_END();
	} else {
		ZEND_HASH_REVERSE_FOREACH_KEY_VAL(Z_ARRVAL_P(input), num_key, string_key, entry) {
			if (string_key) {
				entry = zend_hash_add_new(Z_ARRVAL_P(return_value), string_key, entry);
			} else {
				if (preserve_keys) {
					entry = zend_hash_index_add_new(Z_ARRVAL_P(return_value), num_key, entry);
				} else {
					entry = zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), entry);
				}
			}
			zval_add_ref(entry);
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

/* {{{ Returns a copy of input array padded with pad_value to size pad_size */
PHP_FUNCTION(array_pad)
{
	zval  *input;		/* Input array */
	zval  *pad_value;	/* Padding value obviously */
	zend_long pad_size;		/* Size to pad to */
	zend_long pad_size_abs;	/* Absolute value of pad_size */
	zend_long input_size;		/* Size of the input array */
	zend_long num_pads;		/* How many pads do we need */
	zend_long i;
	zend_string *key;
	zval *value;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_ARRAY(input)
		Z_PARAM_LONG(pad_size)
		Z_PARAM_ZVAL(pad_value)
	ZEND_PARSE_PARAMETERS_END();

	/* Do some initial calculations */
	input_size = zend_hash_num_elements(Z_ARRVAL_P(input));
	pad_size_abs = ZEND_ABS(pad_size);
	if (pad_size_abs < 0 || pad_size_abs - input_size > Z_L(1048576)) {
		zend_argument_value_error(2, "must be less than or equal to 1048576");
		RETURN_THROWS();
	}

	if (input_size >= pad_size_abs) {
		/* Copy the original array */
		ZVAL_COPY(return_value, input);
		return;
	}

	num_pads = pad_size_abs - input_size;
	if (Z_REFCOUNTED_P(pad_value)) {
		GC_ADDREF_EX(Z_COUNTED_P(pad_value), num_pads);
	}

	array_init_size(return_value, pad_size_abs);
	if (HT_FLAGS(Z_ARRVAL_P(input)) & HASH_FLAG_PACKED) {
		zend_hash_real_init_packed(Z_ARRVAL_P(return_value));

		if (pad_size < 0) {
			ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
				for (i = 0; i < num_pads; i++) {
					ZEND_HASH_FILL_ADD(pad_value);
				}
			} ZEND_HASH_FILL_END();
		}

		ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(input), value) {
				Z_TRY_ADDREF_P(value);
				ZEND_HASH_FILL_ADD(value);
			} ZEND_HASH_FOREACH_END();
		} ZEND_HASH_FILL_END();

		if (pad_size > 0) {
			ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
				for (i = 0; i < num_pads; i++) {
					ZEND_HASH_FILL_ADD(pad_value);
				}
			} ZEND_HASH_FILL_END();
		}
	} else {
		if (pad_size < 0) {
			for (i = 0; i < num_pads; i++) {
				zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), pad_value);
			}
		}

		ZEND_HASH_FOREACH_STR_KEY_VAL_IND(Z_ARRVAL_P(input), key, value) {
			Z_TRY_ADDREF_P(value);
			if (key) {
				zend_hash_add_new(Z_ARRVAL_P(return_value), key, value);
			} else {
				zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), value);
			}
		} ZEND_HASH_FOREACH_END();

		if (pad_size > 0) {
			for (i = 0; i < num_pads; i++) {
				zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), pad_value);
			}
		}
	}
}
/* }}} */

/* {{{ Return array with key <-> value flipped */
PHP_FUNCTION(array_flip)
{
	zval *array, *entry, data;
	zend_ulong num_idx;
	zend_string *str_idx;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(array)
	ZEND_PARSE_PARAMETERS_END();

	array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_P(array)));

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(array), num_idx, str_idx, entry) {
		ZVAL_DEREF(entry);
		if (Z_TYPE_P(entry) == IS_LONG) {
			if (str_idx) {
				ZVAL_STR_COPY(&data, str_idx);
			} else {
				ZVAL_LONG(&data, num_idx);
			}
			zend_hash_index_update(Z_ARRVAL_P(return_value), Z_LVAL_P(entry), &data);
		} else if (Z_TYPE_P(entry) == IS_STRING) {
			if (str_idx) {
				ZVAL_STR_COPY(&data, str_idx);
			} else {
				ZVAL_LONG(&data, num_idx);
			}
			zend_symtable_update(Z_ARRVAL_P(return_value), Z_STR_P(entry), &data);
		} else {
			php_error_docref(NULL, E_WARNING, "Can only flip string and integer values, entry skipped");
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Returns an array with all string keys lowercased [or uppercased] */
PHP_FUNCTION(array_change_key_case)
{
	zval *array, *entry;
	zend_string *string_key;
	zend_string *new_key;
	zend_ulong num_key;
	zend_long change_to_upper=0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY(array)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(change_to_upper)
	ZEND_PARSE_PARAMETERS_END();

	array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_P(array)));

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(array), num_key, string_key, entry) {
		if (!string_key) {
			entry = zend_hash_index_update(Z_ARRVAL_P(return_value), num_key, entry);
		} else {
			if (change_to_upper) {
				new_key = php_string_toupper(string_key);
			} else {
				new_key = php_string_tolower(string_key);
			}
			entry = zend_hash_update(Z_ARRVAL_P(return_value), new_key, entry);
			zend_string_release_ex(new_key, 0);
		}

		zval_add_ref(entry);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

struct bucketindex {
	Bucket b;
	unsigned int i;
};

static void array_bucketindex_swap(void *p, void *q)
{
	struct bucketindex *f = (struct bucketindex *)p;
	struct bucketindex *g = (struct bucketindex *)q;
	struct bucketindex t;
	t = *f;
	*f = *g;
	*g = t;
}

/* {{{ Removes duplicate values from array */
PHP_FUNCTION(array_unique)
{
	zval *array;
	Bucket *p;
	zend_long sort_type = PHP_SORT_STRING;
	bucket_compare_func_t cmp;
	struct bucketindex *arTmp, *cmpdata, *lastkept;
	uint32_t i, idx;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY(array)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(sort_type)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_ARRVAL_P(array)->nNumOfElements <= 1) {	/* nothing to do */
		ZVAL_COPY(return_value, array);
		return;
	}

	if (sort_type == PHP_SORT_STRING) {
		HashTable seen;
		zend_long num_key;
		zend_string *str_key;
		zval *val;

		zend_hash_init(&seen, zend_hash_num_elements(Z_ARRVAL_P(array)), NULL, NULL, 0);
		array_init(return_value);

		ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL_P(array), num_key, str_key, val) {
			zval *retval;
			if (Z_TYPE_P(val) == IS_STRING) {
				retval = zend_hash_add_empty_element(&seen, Z_STR_P(val));
			} else {
				zend_string *tmp_str_val;
				zend_string *str_val = zval_get_tmp_string(val, &tmp_str_val);
				retval = zend_hash_add_empty_element(&seen, str_val);
				zend_tmp_string_release(tmp_str_val);
			}

			if (retval) {
				/* First occurrence of the value */
				if (UNEXPECTED(Z_ISREF_P(val) && Z_REFCOUNT_P(val) == 1)) {
					ZVAL_DEREF(val);
				}
				Z_TRY_ADDREF_P(val);

				if (str_key) {
					zend_hash_add_new(Z_ARRVAL_P(return_value), str_key, val);
				} else {
					zend_hash_index_add_new(Z_ARRVAL_P(return_value), num_key, val);
				}
			}
		} ZEND_HASH_FOREACH_END();

		zend_hash_destroy(&seen);
		return;
	}

	cmp = php_get_data_compare_func_unstable(sort_type, 0);

	RETVAL_ARR(zend_array_dup(Z_ARRVAL_P(array)));

	/* create and sort array with pointers to the target_hash buckets */
	arTmp = pemalloc((Z_ARRVAL_P(array)->nNumOfElements + 1) * sizeof(struct bucketindex), GC_FLAGS(Z_ARRVAL_P(array)) & IS_ARRAY_PERSISTENT);
	for (i = 0, idx = 0; idx < Z_ARRVAL_P(array)->nNumUsed; idx++) {
		p = Z_ARRVAL_P(array)->arData + idx;
		if (Z_TYPE(p->val) == IS_UNDEF) continue;
		if (Z_TYPE(p->val) == IS_INDIRECT && Z_TYPE_P(Z_INDIRECT(p->val)) == IS_UNDEF) continue;
		arTmp[i].b = *p;
		arTmp[i].i = i;
		i++;
	}
	ZVAL_UNDEF(&arTmp[i].b.val);
	zend_sort((void *) arTmp, i, sizeof(struct bucketindex),
			(compare_func_t) cmp, (swap_func_t) array_bucketindex_swap);
	/* go through the sorted array and delete duplicates from the copy */
	lastkept = arTmp;
	for (cmpdata = arTmp + 1; Z_TYPE(cmpdata->b.val) != IS_UNDEF; cmpdata++) {
		if (cmp(&lastkept->b, &cmpdata->b)) {
			lastkept = cmpdata;
		} else {
			if (lastkept->i > cmpdata->i) {
				p = &lastkept->b;
				lastkept = cmpdata;
			} else {
				p = &cmpdata->b;
			}
			if (p->key == NULL) {
				zend_hash_index_del(Z_ARRVAL_P(return_value), p->h);
			} else {
				if (Z_ARRVAL_P(return_value) == &EG(symbol_table)) {
					zend_delete_global_variable(p->key);
				} else {
					zend_hash_del(Z_ARRVAL_P(return_value), p->key);
				}
			}
		}
	}
	pefree(arTmp, GC_FLAGS(Z_ARRVAL_P(array)) & IS_ARRAY_PERSISTENT);
}
/* }}} */

static int zval_compare(zval *first, zval *second) /* {{{ */
{
	return string_compare_function(first, second);
}
/* }}} */

static int zval_user_compare(zval *a, zval *b) /* {{{ */
{
	zval args[2];
	zval retval;

	ZVAL_COPY_VALUE(&args[0], a);
	ZVAL_COPY_VALUE(&args[1], b);

	BG(user_compare_fci).param_count = 2;
	BG(user_compare_fci).params = args;
	BG(user_compare_fci).retval = &retval;

	if (zend_call_function(&BG(user_compare_fci), &BG(user_compare_fci_cache)) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		zend_long ret = zval_get_long(&retval);
		zval_ptr_dtor(&retval);
		return ZEND_NORMALIZE_BOOL(ret);
	} else {
		return 0;
	}
}
/* }}} */

static void php_array_intersect_key(INTERNAL_FUNCTION_PARAMETERS, int data_compare_type) /* {{{ */
{
	int argc, i;
	zval *args;
	int (*intersect_data_compare_func)(zval *, zval *) = NULL;
	zend_bool ok;
	zval *val, *data;
	char *param_spec;
	zend_string *key;
	zend_ulong h;

	/* Get the argument count */
	argc = ZEND_NUM_ARGS();
	if (data_compare_type == INTERSECT_COMP_DATA_USER) {
		/* INTERSECT_COMP_DATA_USER - array_uintersect_assoc() */
		param_spec = "+f";
		intersect_data_compare_func = zval_user_compare;
	} else {
		/* 	INTERSECT_COMP_DATA_NONE - array_intersect_key()
			INTERSECT_COMP_DATA_INTERNAL - array_intersect_assoc() */
		param_spec = "+";

		if (data_compare_type == INTERSECT_COMP_DATA_INTERNAL) {
			intersect_data_compare_func = zval_compare;
		}
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), param_spec, &args, &argc, &BG(user_compare_fci), &BG(user_compare_fci_cache)) == FAILURE) {
		RETURN_THROWS();
	}

	for (i = 0; i < argc; i++) {
		if (Z_TYPE(args[i]) != IS_ARRAY) {
			zend_argument_type_error(i + 1, "must be of type array, %s given", zend_zval_type_name(&args[i]));
			RETURN_THROWS();
		}
	}

	array_init(return_value);

	/* Iterate over keys of the first array (handling possibility of indirects such as in $GLOBALS), to compute keys that are in all of the other arrays. */
	ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL(args[0]), h, key, val) {
		if (Z_ISREF_P(val) && Z_REFCOUNT_P(val) == 1) {
			val = Z_REFVAL_P(val);
		}
		if (key == NULL) {
			ok = 1;
			for (i = 1; i < argc; i++) {
				if ((data = zend_hash_index_find(Z_ARRVAL(args[i]), h)) == NULL ||
					(intersect_data_compare_func &&
					intersect_data_compare_func(val, data) != 0)
				) {
					ok = 0;
					break;
				}
			}
			if (ok) {
				Z_TRY_ADDREF_P(val);
				zend_hash_index_add_new(Z_ARRVAL_P(return_value), h, val);
			}
		} else {
			ok = 1;
			for (i = 1; i < argc; i++) {
				if ((data = zend_hash_find_ex_ind(Z_ARRVAL(args[i]), key, 1)) == NULL ||
					(intersect_data_compare_func &&
					intersect_data_compare_func(val, data) != 0)
				) {
					ok = 0;
					break;
				}
			}
			if (ok) {
				Z_TRY_ADDREF_P(val);
				zend_hash_add_new(Z_ARRVAL_P(return_value), key, val);
			}
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

static void php_array_intersect(INTERNAL_FUNCTION_PARAMETERS, int behavior, int data_compare_type, int key_compare_type) /* {{{ */
{
	zval *args = NULL;
	HashTable *hash;
	int arr_argc, i, c = 0;
	uint32_t idx;
	Bucket **lists, *list, **ptrs, *p;
	char *param_spec;
	zend_fcall_info fci1, fci2;
	zend_fcall_info_cache fci1_cache = empty_fcall_info_cache, fci2_cache = empty_fcall_info_cache;
	zend_fcall_info *fci_key = NULL, *fci_data;
	zend_fcall_info_cache *fci_key_cache = NULL, *fci_data_cache;
	PHP_ARRAY_CMP_FUNC_VARS;

	bucket_compare_func_t intersect_key_compare_func;
	bucket_compare_func_t intersect_data_compare_func;

	if (behavior == INTERSECT_NORMAL) {
		intersect_key_compare_func = php_array_key_compare_string;

		if (data_compare_type == INTERSECT_COMP_DATA_INTERNAL) {
			/* array_intersect() */
			param_spec = "+";
			intersect_data_compare_func = php_array_data_compare_string_unstable;
		} else if (data_compare_type == INTERSECT_COMP_DATA_USER) {
			/* array_uintersect() */
			param_spec = "+f";
			intersect_data_compare_func = php_array_user_compare_unstable;
		} else {
			ZEND_ASSERT(0 && "Invalid data_compare_type");
			return;
		}

		if (zend_parse_parameters(ZEND_NUM_ARGS(), param_spec, &args, &arr_argc, &fci1, &fci1_cache) == FAILURE) {
			RETURN_THROWS();
		}
		fci_data = &fci1;
		fci_data_cache = &fci1_cache;

	} else if (behavior & INTERSECT_ASSOC) { /* triggered also when INTERSECT_KEY */
		/* INTERSECT_KEY is subset of INTERSECT_ASSOC. When having the former
		 * no comparison of the data is done (part of INTERSECT_ASSOC) */

		if (data_compare_type == INTERSECT_COMP_DATA_INTERNAL && key_compare_type == INTERSECT_COMP_KEY_INTERNAL) {
			/* array_intersect_assoc() or array_intersect_key() */
			param_spec = "+";
			intersect_key_compare_func = php_array_key_compare_string_unstable;
			intersect_data_compare_func = php_array_data_compare_string_unstable;
		} else if (data_compare_type == INTERSECT_COMP_DATA_USER && key_compare_type == INTERSECT_COMP_KEY_INTERNAL) {
			/* array_uintersect_assoc() */
			param_spec = "+f";
			intersect_key_compare_func = php_array_key_compare_string_unstable;
			intersect_data_compare_func = php_array_user_compare_unstable;
			fci_data = &fci1;
			fci_data_cache = &fci1_cache;
		} else if (data_compare_type == INTERSECT_COMP_DATA_INTERNAL && key_compare_type == INTERSECT_COMP_KEY_USER) {
			/* array_intersect_uassoc() or array_intersect_ukey() */
			param_spec = "+f";
			intersect_key_compare_func = php_array_user_key_compare_unstable;
			intersect_data_compare_func = php_array_data_compare_string_unstable;
			fci_key = &fci1;
			fci_key_cache = &fci1_cache;
		} else if (data_compare_type == INTERSECT_COMP_DATA_USER && key_compare_type == INTERSECT_COMP_KEY_USER) {
			/* array_uintersect_uassoc() */
			param_spec = "+ff";
			intersect_key_compare_func = php_array_user_key_compare_unstable;
			intersect_data_compare_func = php_array_user_compare_unstable;
			fci_data = &fci1;
			fci_data_cache = &fci1_cache;
			fci_key = &fci2;
			fci_key_cache = &fci2_cache;
		} else {
			ZEND_ASSERT(0 && "Invalid data_compare_type / key_compare_type");
			return;
		}

		if (zend_parse_parameters(ZEND_NUM_ARGS(), param_spec, &args, &arr_argc, &fci1, &fci1_cache, &fci2, &fci2_cache) == FAILURE) {
			RETURN_THROWS();
		}

	} else {
		ZEND_ASSERT(0 && "Invalid behavior");
		return;
	}

	PHP_ARRAY_CMP_FUNC_BACKUP();

	/* for each argument, create and sort list with pointers to the hash buckets */
	lists = (Bucket **)safe_emalloc(arr_argc, sizeof(Bucket *), 0);
	ptrs = (Bucket **)safe_emalloc(arr_argc, sizeof(Bucket *), 0);

	if (behavior == INTERSECT_NORMAL && data_compare_type == INTERSECT_COMP_DATA_USER) {
		BG(user_compare_fci) = *fci_data;
		BG(user_compare_fci_cache) = *fci_data_cache;
	} else if ((behavior & INTERSECT_ASSOC) && key_compare_type == INTERSECT_COMP_KEY_USER) {
		BG(user_compare_fci) = *fci_key;
		BG(user_compare_fci_cache) = *fci_key_cache;
	}

	for (i = 0; i < arr_argc; i++) {
		if (Z_TYPE(args[i]) != IS_ARRAY) {
			zend_argument_type_error(i + 1, "must be of type array, %s given", zend_zval_type_name(&args[i]));
			arr_argc = i; /* only free up to i - 1 */
			goto out;
		}
		hash = Z_ARRVAL(args[i]);
		list = (Bucket *) pemalloc((hash->nNumOfElements + 1) * sizeof(Bucket), GC_FLAGS(hash) & IS_ARRAY_PERSISTENT);
		lists[i] = list;
		ptrs[i] = list;
		for (idx = 0; idx < hash->nNumUsed; idx++) {
			p = hash->arData + idx;
			if (Z_TYPE(p->val) == IS_UNDEF) continue;
			*list++ = *p;
		}
		ZVAL_UNDEF(&list->val);
		if (hash->nNumOfElements > 1) {
			if (behavior == INTERSECT_NORMAL) {
				zend_sort((void *) lists[i], hash->nNumOfElements,
						sizeof(Bucket), (compare_func_t) intersect_data_compare_func,
						(swap_func_t)zend_hash_bucket_swap);
			} else if (behavior & INTERSECT_ASSOC) { /* triggered also when INTERSECT_KEY */
				zend_sort((void *) lists[i], hash->nNumOfElements,
						sizeof(Bucket), (compare_func_t) intersect_key_compare_func,
						(swap_func_t)zend_hash_bucket_swap);
			}
		}
	}

	/* copy the argument array */
	RETVAL_ARR(zend_array_dup(Z_ARRVAL(args[0])));

	/* go through the lists and look for common values */
	while (Z_TYPE(ptrs[0]->val) != IS_UNDEF) {
		if ((behavior & INTERSECT_ASSOC) /* triggered also when INTERSECT_KEY */
			&& key_compare_type == INTERSECT_COMP_KEY_USER) {
			BG(user_compare_fci) = *fci_key;
			BG(user_compare_fci_cache) = *fci_key_cache;
		}

		for (i = 1; i < arr_argc; i++) {
			if (behavior & INTERSECT_NORMAL) {
				while (Z_TYPE(ptrs[i]->val) != IS_UNDEF && (0 < (c = intersect_data_compare_func(ptrs[0], ptrs[i])))) {
					ptrs[i]++;
				}
			} else if (behavior & INTERSECT_ASSOC) { /* triggered also when INTERSECT_KEY */
				while (Z_TYPE(ptrs[i]->val) != IS_UNDEF && (0 < (c = intersect_key_compare_func(ptrs[0], ptrs[i])))) {
					ptrs[i]++;
				}
				if ((!c && Z_TYPE(ptrs[i]->val) != IS_UNDEF) && (behavior == INTERSECT_ASSOC)) { /* only when INTERSECT_ASSOC */
					/* this means that ptrs[i] is not NULL so we can compare
					 * and "c==0" is from last operation
					 * in this branch of code we enter only when INTERSECT_ASSOC
					 * since when we have INTERSECT_KEY compare of data is not wanted. */
					if (data_compare_type == INTERSECT_COMP_DATA_USER) {
						BG(user_compare_fci) = *fci_data;
						BG(user_compare_fci_cache) = *fci_data_cache;
					}
					if (intersect_data_compare_func(ptrs[0], ptrs[i]) != 0) {
						c = 1;
						if (key_compare_type == INTERSECT_COMP_KEY_USER) {
							BG(user_compare_fci) = *fci_key;
							BG(user_compare_fci_cache) = *fci_key_cache;
							/* When KEY_USER, the last parameter is always the callback */
						}
						/* we are going to the break */
					} else {
						/* continue looping */
					}
				}
			}
			if (Z_TYPE(ptrs[i]->val) == IS_UNDEF) {
				/* delete any values corresponding to remains of ptrs[0] */
				/* and exit because they do not present in at least one of */
				/* the other arguments */
				for (;;) {
					p = ptrs[0]++;
					if (Z_TYPE(p->val) == IS_UNDEF) {
						goto out;
					}
					if (p->key == NULL) {
						zend_hash_index_del(Z_ARRVAL_P(return_value), p->h);
					} else {
						zend_hash_del(Z_ARRVAL_P(return_value), p->key);
					}
				}
			}
			if (c) /* here we get if not all are equal */
				break;
			ptrs[i]++;
		}
		if (c) {
			/* Value of ptrs[0] not in all arguments, delete all entries */
			/* with value < value of ptrs[i] */
			for (;;) {
				p = ptrs[0];
				if (p->key == NULL) {
					zend_hash_index_del(Z_ARRVAL_P(return_value), p->h);
				} else {
					zend_hash_del(Z_ARRVAL_P(return_value), p->key);
				}
				if (Z_TYPE((++ptrs[0])->val) == IS_UNDEF) {
					goto out;
				}
				if (behavior == INTERSECT_NORMAL) {
					if (0 <= intersect_data_compare_func(ptrs[0], ptrs[i])) {
						break;
					}
				} else if (behavior & INTERSECT_ASSOC) { /* triggered also when INTERSECT_KEY */
					/* no need of looping because indexes are unique */
					break;
				}
			}
		} else {
			/* ptrs[0] is present in all the arguments */
			/* Skip all entries with same value as ptrs[0] */
			for (;;) {
				if (Z_TYPE((++ptrs[0])->val) == IS_UNDEF) {
					goto out;
				}
				if (behavior == INTERSECT_NORMAL) {
					if (intersect_data_compare_func(ptrs[0] - 1, ptrs[0])) {
						break;
					}
				} else if (behavior & INTERSECT_ASSOC) { /* triggered also when INTERSECT_KEY */
					/* no need of looping because indexes are unique */
					break;
				}
			}
		}
	}
out:
	for (i = 0; i < arr_argc; i++) {
		hash = Z_ARRVAL(args[i]);
		pefree(lists[i], GC_FLAGS(hash) & IS_ARRAY_PERSISTENT);
	}

	PHP_ARRAY_CMP_FUNC_RESTORE();

	efree(ptrs);
	efree(lists);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have keys which are present in all the other arguments. Kind of equivalent to array_diff(array_keys($arr1), array_keys($arr2)[,array_keys(...)]). Equivalent of array_intersect_assoc() but does not do compare of the data. */
PHP_FUNCTION(array_intersect_key)
{
	php_array_intersect_key(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_COMP_DATA_NONE);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have keys which are present in all the other arguments. Kind of equivalent to array_diff(array_keys($arr1), array_keys($arr2)[,array_keys(...)]). The comparison of the keys is performed by a user supplied function. Equivalent of array_intersect_uassoc() but does not do compare of the data. */
PHP_FUNCTION(array_intersect_ukey)
{
	php_array_intersect(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_KEY, INTERSECT_COMP_DATA_INTERNAL, INTERSECT_COMP_KEY_USER);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have values which are present in all the other arguments */
PHP_FUNCTION(array_intersect)
{
	php_array_intersect(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_NORMAL, INTERSECT_COMP_DATA_INTERNAL, INTERSECT_COMP_KEY_INTERNAL);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have values which are present in all the other arguments. Data is compared by using a user-supplied callback. */
PHP_FUNCTION(array_uintersect)
{
	php_array_intersect(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_NORMAL, INTERSECT_COMP_DATA_USER, INTERSECT_COMP_KEY_INTERNAL);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have values which are present in all the other arguments. Keys are used to do more restrictive check */
PHP_FUNCTION(array_intersect_assoc)
{
	php_array_intersect_key(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_COMP_DATA_INTERNAL);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have values which are present in all the other arguments. Keys are used to do more restrictive check and they are compared by using a user-supplied callback. */
PHP_FUNCTION(array_intersect_uassoc)
{
	php_array_intersect(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_ASSOC, INTERSECT_COMP_DATA_INTERNAL, INTERSECT_COMP_KEY_USER);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have values which are present in all the other arguments. Keys are used to do more restrictive check. Data is compared by using a user-supplied callback. */
PHP_FUNCTION(array_uintersect_assoc)
{
	php_array_intersect_key(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_COMP_DATA_USER);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have values which are present in all the other arguments. Keys are used to do more restrictive check. Both data and keys are compared by using user-supplied callbacks. */
PHP_FUNCTION(array_uintersect_uassoc)
{
	php_array_intersect(INTERNAL_FUNCTION_PARAM_PASSTHRU, INTERSECT_ASSOC, INTERSECT_COMP_DATA_USER, INTERSECT_COMP_KEY_USER);
}
/* }}} */

static void php_array_diff_key(INTERNAL_FUNCTION_PARAMETERS, int data_compare_type) /* {{{ */
{
	int argc, i;
	zval *args;
	int (*diff_data_compare_func)(zval *, zval *) = NULL;
	zend_bool ok;
	zval *val, *data;
	zend_string *key;
	zend_ulong h;

	/* Get the argument count */
	argc = ZEND_NUM_ARGS();
	if (data_compare_type == DIFF_COMP_DATA_USER) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "+f", &args, &argc, &BG(user_compare_fci), &BG(user_compare_fci_cache)) == FAILURE) {
			RETURN_THROWS();
		}
		diff_data_compare_func = zval_user_compare;
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "+", &args, &argc) == FAILURE) {
			RETURN_THROWS();
		}
		if (data_compare_type == DIFF_COMP_DATA_INTERNAL) {
			diff_data_compare_func = zval_compare;
		}
	}

	for (i = 0; i < argc; i++) {
		if (Z_TYPE(args[i]) != IS_ARRAY) {
			zend_argument_type_error(i + 1, "must be of type array, %s given", zend_zval_type_name(&args[i]));
			RETURN_THROWS();
		}
	}

	array_init(return_value);

	/* Iterate over keys of the first array (handling possibility of indirects such as in $GLOBALS), to compute keys that aren't in the other arrays. */
	ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL(args[0]), h, key, val) {
		if (Z_ISREF_P(val) && Z_REFCOUNT_P(val) == 1) {
			val = Z_REFVAL_P(val);
		}
		if (key == NULL) {
			ok = 1;
			for (i = 1; i < argc; i++) {
				if ((data = zend_hash_index_find(Z_ARRVAL(args[i]), h)) != NULL &&
					(!diff_data_compare_func ||
					diff_data_compare_func(val, data) == 0)
				) {
					ok = 0;
					break;
				}
			}
			if (ok) {
				Z_TRY_ADDREF_P(val);
				zend_hash_index_add_new(Z_ARRVAL_P(return_value), h, val);
			}
		} else {
			ok = 1;
			for (i = 1; i < argc; i++) {
				if ((data = zend_hash_find_ex_ind(Z_ARRVAL(args[i]), key, 1)) != NULL &&
					(!diff_data_compare_func ||
					diff_data_compare_func(val, data) == 0)
				) {
					ok = 0;
					break;
				}
			}
			if (ok) {
				Z_TRY_ADDREF_P(val);
				zend_hash_add_new(Z_ARRVAL_P(return_value), key, val);
			}
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

static void php_array_diff(INTERNAL_FUNCTION_PARAMETERS, int behavior, int data_compare_type, int key_compare_type) /* {{{ */
{
	zval *args = NULL;
	HashTable *hash;
	int arr_argc, i, c;
	uint32_t idx;
	Bucket **lists, *list, **ptrs, *p;
	char *param_spec;
	zend_fcall_info fci1, fci2;
	zend_fcall_info_cache fci1_cache = empty_fcall_info_cache, fci2_cache = empty_fcall_info_cache;
	zend_fcall_info *fci_key = NULL, *fci_data;
	zend_fcall_info_cache *fci_key_cache = NULL, *fci_data_cache;
	PHP_ARRAY_CMP_FUNC_VARS;

	bucket_compare_func_t diff_key_compare_func;
	bucket_compare_func_t diff_data_compare_func;

	if (behavior == DIFF_NORMAL) {
		diff_key_compare_func = php_array_key_compare_string_unstable;

		if (data_compare_type == DIFF_COMP_DATA_INTERNAL) {
			/* array_diff */
			param_spec = "+";
			diff_data_compare_func = php_array_data_compare_string_unstable;
		} else if (data_compare_type == DIFF_COMP_DATA_USER) {
			/* array_udiff */
			param_spec = "+f";
			diff_data_compare_func = php_array_user_compare_unstable;
		} else {
			ZEND_ASSERT(0 && "Invalid data_compare_type");
			return;
		}

		if (zend_parse_parameters(ZEND_NUM_ARGS(), param_spec, &args, &arr_argc, &fci1, &fci1_cache) == FAILURE) {
			RETURN_THROWS();
		}
		fci_data = &fci1;
		fci_data_cache = &fci1_cache;

	} else if (behavior & DIFF_ASSOC) { /* triggered also if DIFF_KEY */
		/* DIFF_KEY is subset of DIFF_ASSOC. When having the former
		 * no comparison of the data is done (part of DIFF_ASSOC) */

		if (data_compare_type == DIFF_COMP_DATA_INTERNAL && key_compare_type == DIFF_COMP_KEY_INTERNAL) {
			/* array_diff_assoc() or array_diff_key() */
			param_spec = "+";
			diff_key_compare_func = php_array_key_compare_string_unstable;
			diff_data_compare_func = php_array_data_compare_string_unstable;
		} else if (data_compare_type == DIFF_COMP_DATA_USER && key_compare_type == DIFF_COMP_KEY_INTERNAL) {
			/* array_udiff_assoc() */
			param_spec = "+f";
			diff_key_compare_func = php_array_key_compare_string_unstable;
			diff_data_compare_func = php_array_user_compare_unstable;
			fci_data = &fci1;
			fci_data_cache = &fci1_cache;
		} else if (data_compare_type == DIFF_COMP_DATA_INTERNAL && key_compare_type == DIFF_COMP_KEY_USER) {
			/* array_diff_uassoc() or array_diff_ukey() */
			param_spec = "+f";
			diff_key_compare_func = php_array_user_key_compare_unstable;
			diff_data_compare_func = php_array_data_compare_string_unstable;
			fci_key = &fci1;
			fci_key_cache = &fci1_cache;
		} else if (data_compare_type == DIFF_COMP_DATA_USER && key_compare_type == DIFF_COMP_KEY_USER) {
			/* array_udiff_uassoc() */
			param_spec = "+ff";
			diff_key_compare_func = php_array_user_key_compare_unstable;
			diff_data_compare_func = php_array_user_compare_unstable;
			fci_data = &fci1;
			fci_data_cache = &fci1_cache;
			fci_key = &fci2;
			fci_key_cache = &fci2_cache;
		} else {
			ZEND_ASSERT(0 && "Invalid data_compare_type / key_compare_type");
			return;
		}

		if (zend_parse_parameters(ZEND_NUM_ARGS(), param_spec, &args, &arr_argc, &fci1, &fci1_cache, &fci2, &fci2_cache) == FAILURE) {
			RETURN_THROWS();
		}

	} else {
		ZEND_ASSERT(0 && "Invalid behavior");
		return;
	}

	PHP_ARRAY_CMP_FUNC_BACKUP();

	/* for each argument, create and sort list with pointers to the hash buckets */
	lists = (Bucket **)safe_emalloc(arr_argc, sizeof(Bucket *), 0);
	ptrs = (Bucket **)safe_emalloc(arr_argc, sizeof(Bucket *), 0);

	if (behavior == DIFF_NORMAL && data_compare_type == DIFF_COMP_DATA_USER) {
		BG(user_compare_fci) = *fci_data;
		BG(user_compare_fci_cache) = *fci_data_cache;
	} else if ((behavior & DIFF_ASSOC) && key_compare_type == DIFF_COMP_KEY_USER) {
		BG(user_compare_fci) = *fci_key;
		BG(user_compare_fci_cache) = *fci_key_cache;
	}

	for (i = 0; i < arr_argc; i++) {
		if (Z_TYPE(args[i]) != IS_ARRAY) {
			zend_argument_type_error(i + 1, "must be of type array, %s given", zend_zval_type_name(&args[i]));
			arr_argc = i; /* only free up to i - 1 */
			goto out;
		}
		hash = Z_ARRVAL(args[i]);
		list = (Bucket *) pemalloc((hash->nNumOfElements + 1) * sizeof(Bucket), GC_FLAGS(hash) & IS_ARRAY_PERSISTENT);
		lists[i] = list;
		ptrs[i] = list;
		for (idx = 0; idx < hash->nNumUsed; idx++) {
			p = hash->arData + idx;
			if (Z_TYPE(p->val) == IS_UNDEF) continue;
			*list++ = *p;
		}
		ZVAL_UNDEF(&list->val);
		if (hash->nNumOfElements > 1) {
			if (behavior == DIFF_NORMAL) {
				zend_sort((void *) lists[i], hash->nNumOfElements,
						sizeof(Bucket), (compare_func_t) diff_data_compare_func,
						(swap_func_t)zend_hash_bucket_swap);
			} else if (behavior & DIFF_ASSOC) { /* triggered also when DIFF_KEY */
				zend_sort((void *) lists[i], hash->nNumOfElements,
						sizeof(Bucket), (compare_func_t) diff_key_compare_func,
						(swap_func_t)zend_hash_bucket_swap);
			}
		}
	}

	/* copy the argument array */
	RETVAL_ARR(zend_array_dup(Z_ARRVAL(args[0])));

	/* go through the lists and look for values of ptr[0] that are not in the others */
	while (Z_TYPE(ptrs[0]->val) != IS_UNDEF) {
		if ((behavior & DIFF_ASSOC) /* triggered also when DIFF_KEY */
			&&
			key_compare_type == DIFF_COMP_KEY_USER
		) {
			BG(user_compare_fci) = *fci_key;
			BG(user_compare_fci_cache) = *fci_key_cache;
		}
		c = 1;
		for (i = 1; i < arr_argc; i++) {
			Bucket *ptr = ptrs[i];
			if (behavior == DIFF_NORMAL) {
				while (Z_TYPE(ptrs[i]->val) != IS_UNDEF && (0 < (c = diff_data_compare_func(ptrs[0], ptrs[i])))) {
					ptrs[i]++;
				}
			} else if (behavior & DIFF_ASSOC) { /* triggered also when DIFF_KEY */
				while (Z_TYPE(ptr->val) != IS_UNDEF && (0 != (c = diff_key_compare_func(ptrs[0], ptr)))) {
					ptr++;
				}
			}
			if (!c) {
				if (behavior == DIFF_NORMAL) {
					if (Z_TYPE(ptrs[i]->val) != IS_UNDEF) {
						ptrs[i]++;
					}
					break;
				} else if (behavior == DIFF_ASSOC) {  /* only when DIFF_ASSOC */
					/* In this branch is execute only when DIFF_ASSOC. If behavior == DIFF_KEY
					 * data comparison is not needed - skipped. */
					if (Z_TYPE(ptr->val) != IS_UNDEF) {
						if (data_compare_type == DIFF_COMP_DATA_USER) {
							BG(user_compare_fci) = *fci_data;
							BG(user_compare_fci_cache) = *fci_data_cache;
						}
						if (diff_data_compare_func(ptrs[0], ptr) != 0) {
							/* the data is not the same */
							c = -1;
							if (key_compare_type == DIFF_COMP_KEY_USER) {
								BG(user_compare_fci) = *fci_key;
								BG(user_compare_fci_cache) = *fci_key_cache;
							}
						} else {
							break;
							/* we have found the element in other arrays thus we don't want it
							 * in the return_value -> delete from there */
						}
					}
				} else if (behavior == DIFF_KEY) { /* only when DIFF_KEY */
					/* the behavior here differs from INTERSECT_KEY in php_intersect
					 * since in the "diff" case we have to remove the entry from
					 * return_value while when doing intersection the entry must not
					 * be deleted. */
					break; /* remove the key */
				}
			}
		}
		if (!c) {
			/* ptrs[0] in one of the other arguments */
			/* delete all entries with value as ptrs[0] */
			for (;;) {
				p = ptrs[0];
				if (p->key == NULL) {
					zend_hash_index_del(Z_ARRVAL_P(return_value), p->h);
				} else {
					zend_hash_del(Z_ARRVAL_P(return_value), p->key);
				}
				if (Z_TYPE((++ptrs[0])->val) == IS_UNDEF) {
					goto out;
				}
				if (behavior == DIFF_NORMAL) {
					if (diff_data_compare_func(ptrs[0] - 1, ptrs[0])) {
						break;
					}
				} else if (behavior & DIFF_ASSOC) { /* triggered also when DIFF_KEY */
					/* in this case no array_key_compare is needed */
					break;
				}
			}
		} else {
			/* ptrs[0] in none of the other arguments */
			/* skip all entries with value as ptrs[0] */
			for (;;) {
				if (Z_TYPE((++ptrs[0])->val) == IS_UNDEF) {
					goto out;
				}
				if (behavior == DIFF_NORMAL) {
					if (diff_data_compare_func(ptrs[0] - 1, ptrs[0])) {
						break;
					}
				} else if (behavior & DIFF_ASSOC) { /* triggered also when DIFF_KEY */
					/* in this case no array_key_compare is needed */
					break;
				}
			}
		}
	}
out:
	for (i = 0; i < arr_argc; i++) {
		hash = Z_ARRVAL(args[i]);
		pefree(lists[i], GC_FLAGS(hash) & IS_ARRAY_PERSISTENT);
	}

	PHP_ARRAY_CMP_FUNC_RESTORE();

	efree(ptrs);
	efree(lists);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have keys which are not present in any of the others arguments. This function is like array_diff() but works on the keys instead of the values. The associativity is preserved. */
PHP_FUNCTION(array_diff_key)
{
	php_array_diff_key(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIFF_COMP_DATA_NONE);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have keys which are not present in any of the others arguments. User supplied function is used for comparing the keys. This function is like array_udiff() but works on the keys instead of the values. The associativity is preserved. */
PHP_FUNCTION(array_diff_ukey)
{
	php_array_diff(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIFF_KEY, DIFF_COMP_DATA_INTERNAL, DIFF_COMP_KEY_USER);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have values which are not present in any of the others arguments. */
PHP_FUNCTION(array_diff)
{
	zval *args;
	int argc, i;
	uint32_t num;
	HashTable exclude;
	zval *value;
	zend_string *str, *tmp_str, *key;
	zend_long idx;
	zval dummy;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	if (Z_TYPE(args[0]) != IS_ARRAY) {
		zend_argument_type_error(1, "must be of type array, %s given", zend_zval_type_name(&args[0]));
		RETURN_THROWS();
	}

	num = zend_hash_num_elements(Z_ARRVAL(args[0]));
	if (num == 0) {
		for (i = 1; i < argc; i++) {
			if (Z_TYPE(args[i]) != IS_ARRAY) {
				zend_argument_type_error(i + 1, "must be of type array, %s given", zend_zval_type_name(&args[i]));
				RETURN_THROWS();
			}
		}
		RETURN_EMPTY_ARRAY();
	} else if (num == 1) {
		int found = 0;
		zend_string *search_str, *tmp_search_str;

		value = NULL;
		ZEND_HASH_FOREACH_VAL_IND(Z_ARRVAL(args[0]), value) {
			break;
		} ZEND_HASH_FOREACH_END();

		if (!value) {
			for (i = 1; i < argc; i++) {
				if (Z_TYPE(args[i]) != IS_ARRAY) {
					zend_argument_type_error(i + 1, "must be of type array, %s given", zend_zval_type_name(&args[i]));
					RETURN_THROWS();
				}
			}
			RETURN_EMPTY_ARRAY();
		}

		search_str = zval_get_tmp_string(value, &tmp_search_str);

		for (i = 1; i < argc; i++) {
			if (Z_TYPE(args[i]) != IS_ARRAY) {
				zend_argument_type_error(i + 1, "must be of type array, %s given", zend_zval_type_name(&args[i]));
				RETURN_THROWS();
			}
			if (!found) {
				ZEND_HASH_FOREACH_VAL_IND(Z_ARRVAL(args[i]), value) {
					str = zval_get_tmp_string(value, &tmp_str);
					if (zend_string_equals(search_str, str)) {
						zend_tmp_string_release(tmp_str);
						found = 1;
						break;
					}
					zend_tmp_string_release(tmp_str);
				} ZEND_HASH_FOREACH_END();
			}
		}

		zend_tmp_string_release(tmp_search_str);

		if (found) {
			RETVAL_EMPTY_ARRAY();
		} else {
			ZVAL_COPY(return_value, &args[0]);
		}
		return;
	}

	/* count number of elements */
	num = 0;
	for (i = 1; i < argc; i++) {
		if (Z_TYPE(args[i]) != IS_ARRAY) {
			zend_argument_type_error(i + 1, "must be of type array, %s given", zend_zval_type_name(&args[i]));
			RETURN_THROWS();
		}
		num += zend_hash_num_elements(Z_ARRVAL(args[i]));
	}

	if (num == 0) {
		ZVAL_COPY(return_value, &args[0]);
		return;
	}

	ZVAL_NULL(&dummy);
	/* create exclude map */
	zend_hash_init(&exclude, num, NULL, NULL, 0);
	for (i = 1; i < argc; i++) {
		ZEND_HASH_FOREACH_VAL_IND(Z_ARRVAL(args[i]), value) {
			str = zval_get_tmp_string(value, &tmp_str);
			zend_hash_add(&exclude, str, &dummy);
			zend_tmp_string_release(tmp_str);
		} ZEND_HASH_FOREACH_END();
	}

	/* copy all elements of first array that are not in exclude set */
	array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL(args[0])));
	ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL(args[0]), idx, key, value) {
		str = zval_get_tmp_string(value, &tmp_str);
		if (!zend_hash_exists(&exclude, str)) {
			if (key) {
				value = zend_hash_add_new(Z_ARRVAL_P(return_value), key, value);
			} else {
				value = zend_hash_index_add_new(Z_ARRVAL_P(return_value), idx, value);
			}
			zval_add_ref(value);
		}
		zend_tmp_string_release(tmp_str);
	} ZEND_HASH_FOREACH_END();

	zend_hash_destroy(&exclude);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have values which are not present in any of the others arguments. Elements are compared by user supplied function. */
PHP_FUNCTION(array_udiff)
{
	php_array_diff(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIFF_NORMAL, DIFF_COMP_DATA_USER, DIFF_COMP_KEY_INTERNAL);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have values which are not present in any of the others arguments but do additional checks whether the keys are equal */
PHP_FUNCTION(array_diff_assoc)
{
	php_array_diff_key(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIFF_COMP_DATA_INTERNAL);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have values which are not present in any of the others arguments but do additional checks whether the keys are equal. Elements are compared by user supplied function. */
PHP_FUNCTION(array_diff_uassoc)
{
	php_array_diff(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIFF_ASSOC, DIFF_COMP_DATA_INTERNAL, DIFF_COMP_KEY_USER);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have values which are not present in any of the others arguments but do additional checks whether the keys are equal. Keys are compared by user supplied function. */
PHP_FUNCTION(array_udiff_assoc)
{
	php_array_diff_key(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIFF_COMP_DATA_USER);
}
/* }}} */

/* {{{ Returns the entries of arr1 that have values which are not present in any of the others arguments but do additional checks whether the keys are equal. Keys and elements are compared by user supplied functions. */
PHP_FUNCTION(array_udiff_uassoc)
{
	php_array_diff(INTERNAL_FUNCTION_PARAM_PASSTHRU, DIFF_ASSOC, DIFF_COMP_DATA_USER, DIFF_COMP_KEY_USER);
}
/* }}} */

#define MULTISORT_ORDER	0
#define MULTISORT_TYPE	1
#define MULTISORT_LAST	2

PHPAPI int php_multisort_compare(const void *a, const void *b) /* {{{ */
{
	Bucket *ab = *(Bucket **)a;
	Bucket *bb = *(Bucket **)b;
	int r;
	zend_long result;

	r = 0;
	do {
		result = ARRAYG(multisort_func)[r](&ab[r], &bb[r]);
		if (result != 0) {
			return result > 0 ? 1 : -1;
		}
		r++;
	} while (Z_TYPE(ab[r].val) != IS_UNDEF);

	return stable_sort_fallback(&ab[r], &bb[r]);
}
/* }}} */

#define MULTISORT_ABORT				\
	efree(func);	\
	efree(arrays);					\
	return;

static void array_bucket_p_sawp(void *p, void *q) /* {{{ */ {
	Bucket *t;
	Bucket **f = (Bucket **)p;
	Bucket **g = (Bucket **)q;

	t = *f;
	*f = *g;
	*g = t;
}
/* }}} */

/* {{{ Sort multiple arrays at once similar to how ORDER BY clause works in SQL */
PHP_FUNCTION(array_multisort)
{
	zval*			args;
	zval**			arrays;
	Bucket**		indirect;
	uint32_t            idx;
	Bucket*			p;
	HashTable*		hash;
	int				argc;
	int				array_size;
	int				num_arrays = 0;
	int				parse_state[MULTISORT_LAST];   /* 0 - flag not allowed 1 - flag allowed */
	int				sort_order = PHP_SORT_ASC;
	int				sort_type  = PHP_SORT_REGULAR;
	int				i, k, n;
	bucket_compare_func_t *func;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	/* Allocate space for storing pointers to input arrays and sort flags. */
	arrays = (zval **)ecalloc(argc, sizeof(zval *));
	for (i = 0; i < MULTISORT_LAST; i++) {
		parse_state[i] = 0;
	}
	func = ARRAYG(multisort_func) = ecalloc(argc, sizeof(bucket_compare_func_t));

	/* Here we go through the input arguments and parse them. Each one can
	 * be either an array or a sort flag which follows an array. If not
	 * specified, the sort flags defaults to PHP_SORT_ASC and PHP_SORT_REGULAR
	 * accordingly. There can't be two sort flags of the same type after an
	 * array, and the very first argument has to be an array. */
	for (i = 0; i < argc; i++) {
		zval *arg = &args[i];

		ZVAL_DEREF(arg);
		if (Z_TYPE_P(arg) == IS_ARRAY) {
			SEPARATE_ARRAY(arg);
			/* We see the next array, so we update the sort flags of
			 * the previous array and reset the sort flags. */
			if (i > 0) {
				ARRAYG(multisort_func)[num_arrays - 1] = php_get_data_compare_func_unstable(sort_type, sort_order != PHP_SORT_ASC);
				sort_order = PHP_SORT_ASC;
				sort_type = PHP_SORT_REGULAR;
			}
			arrays[num_arrays++] = arg;

			/* Next one may be an array or a list of sort flags. */
			for (k = 0; k < MULTISORT_LAST; k++) {
				parse_state[k] = 1;
			}
		} else if (Z_TYPE_P(arg) == IS_LONG) {
			switch (Z_LVAL_P(arg) & ~PHP_SORT_FLAG_CASE) {
				case PHP_SORT_ASC:
				case PHP_SORT_DESC:
					/* flag allowed here */
					if (parse_state[MULTISORT_ORDER] == 1) {
						/* Save the flag and make sure then next arg is not the current flag. */
						sort_order = Z_LVAL_P(arg) == PHP_SORT_DESC ? PHP_SORT_DESC : PHP_SORT_ASC;
						parse_state[MULTISORT_ORDER] = 0;
					} else {
						zend_argument_type_error(i + 1, "must be an array or a sort flag that has not already been specified");
						MULTISORT_ABORT;
					}
					break;

				case PHP_SORT_REGULAR:
				case PHP_SORT_NUMERIC:
				case PHP_SORT_STRING:
				case PHP_SORT_NATURAL:
				case PHP_SORT_LOCALE_STRING:
					/* flag allowed here */
					if (parse_state[MULTISORT_TYPE] == 1) {
						/* Save the flag and make sure then next arg is not the current flag. */
						sort_type = (int)Z_LVAL_P(arg);
						parse_state[MULTISORT_TYPE] = 0;
					} else {
						zend_argument_type_error(i + 1, "must be an array or a sort flag that has not already been specified");
						MULTISORT_ABORT;
					}
					break;

				default:
					zend_argument_value_error(i + 1, "must be a valid sort flag");
					MULTISORT_ABORT;
					break;

			}
		} else {
			zend_argument_type_error(i + 1, "must be an array or a sort flag");
			MULTISORT_ABORT;
		}
	}
	/* Take care of the last array sort flags. */
	ARRAYG(multisort_func)[num_arrays - 1] = php_get_data_compare_func_unstable(sort_type, sort_order != PHP_SORT_ASC);

	/* Make sure the arrays are of the same size. */
	array_size = zend_hash_num_elements(Z_ARRVAL_P(arrays[0]));
	for (i = 0; i < num_arrays; i++) {
		if (zend_hash_num_elements(Z_ARRVAL_P(arrays[i])) != (uint32_t)array_size) {
			zend_value_error("Array sizes are inconsistent");
			MULTISORT_ABORT;
		}
	}

	/* If all arrays are empty we don't need to do anything. */
	if (array_size < 1) {
		efree(func);
		efree(arrays);
		RETURN_TRUE;
	}

	/* Create the indirection array. This array is of size MxN, where
	 * M is the number of entries in each input array and N is the number
	 * of the input arrays + 1. The last column is UNDEF to indicate the end
	 * of the row. It also stores the original position for stable sorting. */
	indirect = (Bucket **)safe_emalloc(array_size, sizeof(Bucket *), 0);
	for (i = 0; i < array_size; i++) {
		indirect[i] = (Bucket *)safe_emalloc((num_arrays + 1), sizeof(Bucket), 0);
	}
	for (i = 0; i < num_arrays; i++) {
		k = 0;
		for (idx = 0; idx < Z_ARRVAL_P(arrays[i])->nNumUsed; idx++) {
			p = Z_ARRVAL_P(arrays[i])->arData + idx;
			if (Z_TYPE(p->val) == IS_UNDEF) continue;
			indirect[k][i] = *p;
			k++;
		}
	}
	for (k = 0; k < array_size; k++) {
		ZVAL_UNDEF(&indirect[k][num_arrays].val);
		Z_EXTRA_P(&indirect[k][num_arrays].val) = k;
	}

	/* Do the actual sort magic - bada-bim, bada-boom. */
	zend_sort(indirect, array_size, sizeof(Bucket *), php_multisort_compare, (swap_func_t)array_bucket_p_sawp);

	/* Restructure the arrays based on sorted indirect - this is mostly taken from zend_hash_sort() function. */
	for (i = 0; i < num_arrays; i++) {
		int repack;

		hash = Z_ARRVAL_P(arrays[i]);
		hash->nNumUsed = array_size;
		hash->nInternalPointer = 0;
		repack = !(HT_FLAGS(hash) & HASH_FLAG_PACKED);

		for (n = 0, k = 0; k < array_size; k++) {
			hash->arData[k] = indirect[k][i];
			if (hash->arData[k].key == NULL) {
				hash->arData[k].h = n++;
			} else {
				repack = 0;
			}
		}
		hash->nNextFreeElement = array_size;
		if (repack) {
			zend_hash_to_packed(hash);
		} else if (!(HT_FLAGS(hash) & HASH_FLAG_PACKED)) {
			zend_hash_rehash(hash);
		}
	}

	/* Clean up. */
	for (i = 0; i < array_size; i++) {
		efree(indirect[i]);
	}
	efree(indirect);
	efree(func);
	efree(arrays);
	RETURN_TRUE;
}
/* }}} */

/* {{{ Return key/keys for random entry/entries in the array */
PHP_FUNCTION(array_rand)
{
	zval *input;
	zend_long num_req = 1;
	zend_string *string_key;
	zend_ulong num_key;
	int i;
	int num_avail;
	zend_bitset bitset;
	int negative_bitset = 0;
	uint32_t bitset_len;
	ALLOCA_FLAG(use_heap)

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ARRAY(input)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(num_req)
	ZEND_PARSE_PARAMETERS_END();

	num_avail = zend_hash_num_elements(Z_ARRVAL_P(input));

	if (num_avail == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	if (num_req == 1) {
		HashTable *ht = Z_ARRVAL_P(input);

		if ((uint32_t)num_avail < ht->nNumUsed - (ht->nNumUsed>>1)) {
			/* If less than 1/2 of elements are used, don't sample. Instead search for a
			 * specific offset using linear scan. */
			zend_long i = 0, randval = php_mt_rand_range(0, num_avail - 1);
			ZEND_HASH_FOREACH_KEY(Z_ARRVAL_P(input), num_key, string_key) {
				if (i == randval) {
					if (string_key) {
						RETURN_STR_COPY(string_key);
					} else {
						RETURN_LONG(num_key);
					}
				}
				i++;
			} ZEND_HASH_FOREACH_END();
		}

		/* Sample random buckets until we hit one that is not empty.
		 * The worst case probability of hitting an empty element is 1-1/2. The worst case
		 * probability of hitting N empty elements in a row is (1-1/2)**N.
		 * For N=10 this becomes smaller than 0.1%. */
		do {
			zend_long randval = php_mt_rand_range(0, ht->nNumUsed - 1);
			Bucket *bucket = &ht->arData[randval];
			if (!Z_ISUNDEF(bucket->val)) {
				if (bucket->key) {
					RETURN_STR_COPY(bucket->key);
				} else {
					RETURN_LONG(bucket->h);
				}
			}
		} while (1);
	}

	if (num_req <= 0 || num_req > num_avail) {
		zend_argument_value_error(2, "must be between 1 and the number of elements in argument #1 ($array)");
		RETURN_THROWS();
	}

	/* Make the return value an array only if we need to pass back more than one result. */
	array_init_size(return_value, (uint32_t)num_req);
	if (num_req > (num_avail >> 1)) {
		negative_bitset = 1;
		num_req = num_avail - num_req;
	}

	bitset_len = zend_bitset_len(num_avail);
	bitset = ZEND_BITSET_ALLOCA(bitset_len, use_heap);
	zend_bitset_clear(bitset, bitset_len);

	i = num_req;
	while (i) {
		zend_long randval = php_mt_rand_range(0, num_avail - 1);
		if (!zend_bitset_in(bitset, randval)) {
			zend_bitset_incl(bitset, randval);
			i--;
		}
	}
	/* i = 0; */

	zend_hash_real_init_packed(Z_ARRVAL_P(return_value));
	ZEND_HASH_FILL_PACKED(Z_ARRVAL_P(return_value)) {
		/* We can't use zend_hash_index_find()
		 * because the array may have string keys or gaps. */
		ZEND_HASH_FOREACH_KEY(Z_ARRVAL_P(input), num_key, string_key) {
			if (zend_bitset_in(bitset, i) ^ negative_bitset) {
				if (string_key) {
					ZEND_HASH_FILL_SET_STR_COPY(string_key);
				} else {
					ZEND_HASH_FILL_SET_LONG(num_key);
				}
				ZEND_HASH_FILL_NEXT();
			}
			i++;
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FILL_END();

	free_alloca(bitset, use_heap);
}
/* }}} */

/* {{{ Returns the sum of the array entries */
PHP_FUNCTION(array_sum)
{
	zval *input,
		 *entry,
		 entry_n;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(input)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_LONG(return_value, 0);

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(input), entry) {
		if (Z_TYPE_P(entry) == IS_ARRAY || Z_TYPE_P(entry) == IS_OBJECT) {
			continue;
		}
		ZVAL_COPY(&entry_n, entry);
		convert_scalar_to_number(&entry_n);
		fast_add_function(return_value, return_value, &entry_n);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Returns the product of the array entries */
PHP_FUNCTION(array_product)
{
	zval *input,
		 *entry,
		 entry_n;
	double dval;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(input)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_LONG(return_value, 1);
	if (!zend_hash_num_elements(Z_ARRVAL_P(input))) {
		return;
	}

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(input), entry) {
		if (Z_TYPE_P(entry) == IS_ARRAY || Z_TYPE_P(entry) == IS_OBJECT) {
			continue;
		}
		ZVAL_COPY(&entry_n, entry);
		convert_scalar_to_number(&entry_n);

		if (Z_TYPE(entry_n) == IS_LONG && Z_TYPE_P(return_value) == IS_LONG) {
			dval = (double)Z_LVAL_P(return_value) * (double)Z_LVAL(entry_n);
			if ( (double)ZEND_LONG_MIN <= dval && dval <= (double)ZEND_LONG_MAX ) {
				Z_LVAL_P(return_value) *= Z_LVAL(entry_n);
				continue;
			}
		}
		convert_to_double(return_value);
		convert_to_double(&entry_n);
		Z_DVAL_P(return_value) *= Z_DVAL(entry_n);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Iteratively reduce the array to a single value via the callback. */
PHP_FUNCTION(array_reduce)
{
	zval *input;
	zval args[2];
	zval *operand;
	zval retval;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;
	zval *initial = NULL;
	HashTable *htbl;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ARRAY(input)
		Z_PARAM_FUNC(fci, fci_cache)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL(initial)
	ZEND_PARSE_PARAMETERS_END();


	if (ZEND_NUM_ARGS() > 2) {
		ZVAL_COPY(return_value, initial);
	} else {
		ZVAL_NULL(return_value);
	}

	/* (zval **)input points to an element of argument stack
	 * the base pointer of which is subject to change.
	 * thus we need to keep the pointer to the hashtable for safety */
	htbl = Z_ARRVAL_P(input);

	if (zend_hash_num_elements(htbl) == 0) {
		return;
	}

	fci.retval = &retval;
	fci.param_count = 2;

	ZEND_HASH_FOREACH_VAL(htbl, operand) {
		ZVAL_COPY_VALUE(&args[0], return_value);
		ZVAL_COPY(&args[1], operand);
		fci.params = args;

		if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
			zval_ptr_dtor(&args[1]);
			zval_ptr_dtor(&args[0]);
			ZVAL_COPY_VALUE(return_value, &retval);
		} else {
			zval_ptr_dtor(&args[1]);
			zval_ptr_dtor(&args[0]);
			RETURN_NULL();
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Filters elements from the array via the callback. */
PHP_FUNCTION(array_filter)
{
	zval *array;
	zval *operand;
	zval *key;
	zval args[2];
	zval retval;
	zend_bool have_callback = 0;
	zend_long use_type = 0;
	zend_string *string_key;
	zend_fcall_info fci = empty_fcall_info;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;
	zend_ulong num_key;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_ARRAY(array)
		Z_PARAM_OPTIONAL
		Z_PARAM_FUNC_OR_NULL(fci, fci_cache)
		Z_PARAM_LONG(use_type)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_hash_num_elements(Z_ARRVAL_P(array)) == 0) {
		RETVAL_EMPTY_ARRAY();
		return;
	}
	array_init(return_value);

	if (ZEND_FCI_INITIALIZED(fci)) {
		have_callback = 1;
		fci.retval = &retval;
		if (use_type == ARRAY_FILTER_USE_BOTH) {
			fci.param_count = 2;
			key = &args[1];
		} else {
			fci.param_count = 1;
			key = &args[0];
		}
	}

	ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL_P(array), num_key, string_key, operand) {
		if (have_callback) {
			if (use_type) {
				/* Set up the key */
				if (!string_key) {
					ZVAL_LONG(key, num_key);
				} else {
					ZVAL_STR_COPY(key, string_key);
				}
			}
			if (use_type != ARRAY_FILTER_USE_KEY) {
				ZVAL_COPY(&args[0], operand);
			}
			fci.params = args;

			if (zend_call_function(&fci, &fci_cache) == SUCCESS) {
				int retval_true;

				zval_ptr_dtor(&args[0]);
				if (use_type == ARRAY_FILTER_USE_BOTH) {
					zval_ptr_dtor(&args[1]);
				}
				retval_true = zend_is_true(&retval);
				zval_ptr_dtor(&retval);
				if (!retval_true) {
					continue;
				}
			} else {
				zval_ptr_dtor(&args[0]);
				if (use_type == ARRAY_FILTER_USE_BOTH) {
					zval_ptr_dtor(&args[1]);
				}
				return;
			}
		} else if (!zend_is_true(operand)) {
			continue;
		}

		if (string_key) {
			operand = zend_hash_add_new(Z_ARRVAL_P(return_value), string_key, operand);
		} else {
			operand = zend_hash_index_add_new(Z_ARRVAL_P(return_value), num_key, operand);
		}
		zval_add_ref(operand);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Applies the callback to the elements in given arrays. */
PHP_FUNCTION(array_map)
{
	zval *arrays = NULL;
	int n_arrays = 0;
	zval result;
	zend_fcall_info fci = empty_fcall_info;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;
	int i;
	uint32_t k, maxlen = 0;

	ZEND_PARSE_PARAMETERS_START(2, -1)
		Z_PARAM_FUNC_OR_NULL(fci, fci_cache)
		Z_PARAM_VARIADIC('+', arrays, n_arrays)
	ZEND_PARSE_PARAMETERS_END();

	if (n_arrays == 1) {
		zend_ulong num_key;
		zend_string *str_key;
		zval *zv, arg;
		int ret;

		if (Z_TYPE(arrays[0]) != IS_ARRAY) {
			zend_argument_type_error(2, "must be of type array, %s given", zend_zval_type_name(&arrays[0]));
			RETURN_THROWS();
		}
		maxlen = zend_hash_num_elements(Z_ARRVAL(arrays[0]));

		/* Short-circuit: if no callback and only one array, just return it. */
		if (!ZEND_FCI_INITIALIZED(fci) || !maxlen) {
			ZVAL_COPY(return_value, &arrays[0]);
			return;
		}

		array_init_size(return_value, maxlen);
		zend_hash_real_init(Z_ARRVAL_P(return_value), HT_FLAGS(Z_ARRVAL(arrays[0])) & HASH_FLAG_PACKED);

		ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARRVAL(arrays[0]), num_key, str_key, zv) {
			fci.retval = &result;
			fci.param_count = 1;
			fci.params = &arg;

			ZVAL_COPY(&arg, zv);
			ret = zend_call_function(&fci, &fci_cache);
			i_zval_ptr_dtor(&arg);
			if (ret != SUCCESS || Z_TYPE(result) == IS_UNDEF) {
				zend_array_destroy(Z_ARR_P(return_value));
				RETURN_NULL();
			}
			if (str_key) {
				_zend_hash_append(Z_ARRVAL_P(return_value), str_key, &result);
			} else {
				zend_hash_index_add_new(Z_ARRVAL_P(return_value), num_key, &result);
			}
		} ZEND_HASH_FOREACH_END();
	} else {
		uint32_t *array_pos = (HashPosition *)ecalloc(n_arrays, sizeof(HashPosition));

		for (i = 0; i < n_arrays; i++) {
			if (Z_TYPE(arrays[i]) != IS_ARRAY) {
				zend_argument_type_error(i + 2, "must be of type array, %s given", zend_zval_type_name(&arrays[i]));
				efree(array_pos);
				RETURN_THROWS();
			}
			if (zend_hash_num_elements(Z_ARRVAL(arrays[i])) > maxlen) {
				maxlen = zend_hash_num_elements(Z_ARRVAL(arrays[i]));
			}
		}

		array_init_size(return_value, maxlen);

		if (!ZEND_FCI_INITIALIZED(fci)) {
			zval zv;

			/* We iterate through all the arrays at once. */
			for (k = 0; k < maxlen; k++) {

				/* If no callback, the result will be an array, consisting of current
				 * entries from all arrays. */
				array_init_size(&result, n_arrays);

				for (i = 0; i < n_arrays; i++) {
					/* If this array still has elements, add the current one to the
					 * parameter list, otherwise use null value. */
					uint32_t pos = array_pos[i];
					while (1) {
						if (pos >= Z_ARRVAL(arrays[i])->nNumUsed) {
							ZVAL_NULL(&zv);
							break;
						} else if (Z_TYPE(Z_ARRVAL(arrays[i])->arData[pos].val) != IS_UNDEF) {
							ZVAL_COPY(&zv, &Z_ARRVAL(arrays[i])->arData[pos].val);
							array_pos[i] = pos + 1;
							break;
						}
						pos++;
					}

					zend_hash_next_index_insert_new(Z_ARRVAL(result), &zv);
				}

				zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &result);
			}
		} else {
			zval *params = (zval *)safe_emalloc(n_arrays, sizeof(zval), 0);

			/* We iterate through all the arrays at once. */
			for (k = 0; k < maxlen; k++) {
				for (i = 0; i < n_arrays; i++) {
					/* If this array still has elements, add the current one to the
					 * parameter list, otherwise use null value. */
					uint32_t pos = array_pos[i];
					while (1) {
						if (pos >= Z_ARRVAL(arrays[i])->nNumUsed) {
							ZVAL_NULL(&params[i]);
							break;
						} else if (Z_TYPE(Z_ARRVAL(arrays[i])->arData[pos].val) != IS_UNDEF) {
							ZVAL_COPY(&params[i], &Z_ARRVAL(arrays[i])->arData[pos].val);
							array_pos[i] = pos + 1;
							break;
						}
						pos++;
					}
				}

				fci.retval = &result;
				fci.param_count = n_arrays;
				fci.params = params;

				if (zend_call_function(&fci, &fci_cache) != SUCCESS || Z_TYPE(result) == IS_UNDEF) {
					efree(array_pos);
					zend_array_destroy(Z_ARR_P(return_value));
					for (i = 0; i < n_arrays; i++) {
						zval_ptr_dtor(&params[i]);
					}
					efree(params);
					RETURN_NULL();
				} else {
					for (i = 0; i < n_arrays; i++) {
						zval_ptr_dtor(&params[i]);
					}
				}

				zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &result);
			}

			efree(params);
		}
		efree(array_pos);
	}
}
/* }}} */

/* {{{ Checks if the given key or index exists in the array */
PHP_FUNCTION(array_key_exists)
{
	zval *key;
	HashTable *ht;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(key)
		Z_PARAM_ARRAY_HT(ht)
	ZEND_PARSE_PARAMETERS_END();

	switch (Z_TYPE_P(key)) {
		case IS_STRING:
			RETVAL_BOOL(zend_symtable_exists_ind(ht, Z_STR_P(key)));
			break;
		case IS_LONG:
			RETVAL_BOOL(zend_hash_index_exists(ht, Z_LVAL_P(key)));
			break;
		case IS_NULL:
			RETVAL_BOOL(zend_hash_exists_ind(ht, ZSTR_EMPTY_ALLOC()));
			break;
		case IS_DOUBLE:
			RETVAL_BOOL(zend_hash_index_exists(ht, zend_dval_to_lval(Z_DVAL_P(key))));
			break;
		case IS_FALSE:
			RETVAL_BOOL(zend_hash_index_exists(ht, 0));
			break;
		case IS_TRUE:
			RETVAL_BOOL(zend_hash_index_exists(ht, 1));
			break;
		case IS_RESOURCE:
			zend_error(E_WARNING, "Resource ID#%d used as offset, casting to integer (%d)", Z_RES_HANDLE_P(key), Z_RES_HANDLE_P(key));
			RETVAL_BOOL(zend_hash_index_exists(ht, Z_RES_HANDLE_P(key)));
			break;
		default:
			zend_argument_type_error(1, "must be a valid array offset type");
			break;
	}
}
/* }}} */

/* {{{ Split array into chunks */
PHP_FUNCTION(array_chunk)
{
	int num_in;
	zend_long size, current = 0;
	zend_string *str_key;
	zend_ulong num_key;
	zend_bool preserve_keys = 0;
	zval *input = NULL;
	zval chunk;
	zval *entry;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ARRAY(input)
		Z_PARAM_LONG(size)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(preserve_keys)
	ZEND_PARSE_PARAMETERS_END();

	/* Do bounds checking for size parameter. */
	if (size < 1) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	num_in = zend_hash_num_elements(Z_ARRVAL_P(input));

	if (size > num_in) {
		if (num_in == 0) {
			RETVAL_EMPTY_ARRAY();
			return;
		}
		size = num_in;
	}

	array_init_size(return_value, (uint32_t)(((num_in - 1) / size) + 1));

	ZVAL_UNDEF(&chunk);

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(input), num_key, str_key, entry) {
		/* If new chunk, create and initialize it. */
		if (Z_TYPE(chunk) == IS_UNDEF) {
			array_init_size(&chunk, (uint32_t)size);
		}

		/* Add entry to the chunk, preserving keys if necessary. */
		if (preserve_keys) {
			if (str_key) {
				entry = zend_hash_add_new(Z_ARRVAL(chunk), str_key, entry);
			} else {
				entry = zend_hash_index_add_new(Z_ARRVAL(chunk), num_key, entry);
			}
		} else {
			entry = zend_hash_next_index_insert(Z_ARRVAL(chunk), entry);
		}
		zval_add_ref(entry);

		/* If reached the chunk size, add it to the result array, and reset the
		 * pointer. */
		if (!(++current % size)) {
			add_next_index_zval(return_value, &chunk);
			ZVAL_UNDEF(&chunk);
		}
	} ZEND_HASH_FOREACH_END();

	/* Add the final chunk if there is one. */
	if (Z_TYPE(chunk) != IS_UNDEF) {
		add_next_index_zval(return_value, &chunk);
	}
}
/* }}} */

/* {{{ Creates an array by using the elements of the first parameter as keys and the elements of the second as the corresponding values */
PHP_FUNCTION(array_combine)
{
	HashTable *values, *keys;
	uint32_t pos_values = 0;
	zval *entry_keys, *entry_values;
	int num_keys, num_values;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ARRAY_HT(keys)
		Z_PARAM_ARRAY_HT(values)
	ZEND_PARSE_PARAMETERS_END();

	num_keys = zend_hash_num_elements(keys);
	num_values = zend_hash_num_elements(values);

	if (num_keys != num_values) {
		zend_argument_value_error(1, "and argument #2 ($values) must have the same number of elements");
		RETURN_THROWS();
	}

	if (!num_keys) {
		RETURN_EMPTY_ARRAY();
	}

	array_init_size(return_value, num_keys);
	ZEND_HASH_FOREACH_VAL(keys, entry_keys) {
		while (1) {
			if (pos_values >= values->nNumUsed) {
				break;
			} else if (Z_TYPE(values->arData[pos_values].val) != IS_UNDEF) {
				entry_values = &values->arData[pos_values].val;
				if (Z_TYPE_P(entry_keys) == IS_LONG) {
					entry_values = zend_hash_index_update(Z_ARRVAL_P(return_value),
						Z_LVAL_P(entry_keys), entry_values);
				} else {
					zend_string *tmp_key;
					zend_string *key = zval_get_tmp_string(entry_keys, &tmp_key);
					entry_values = zend_symtable_update(Z_ARRVAL_P(return_value),
						key, entry_values);
					zend_tmp_string_release(tmp_key);
				}
				zval_add_ref(entry_values);
				pos_values++;
				break;
			}
			pos_values++;
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */
