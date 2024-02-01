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
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_STRING_H
#define ZEND_STRING_H

#include "zend.h"

BEGIN_EXTERN_C()

typedef void (*zend_string_copy_storage_func_t)(void);
typedef zend_string *(ZEND_FASTCALL *zend_new_interned_string_func_t)(zend_string *str);
typedef zend_string *(ZEND_FASTCALL *zend_string_init_interned_func_t)(const char *str, size_t size, bool permanent);
typedef zend_string *(ZEND_FASTCALL *zend_string_init_existing_interned_func_t)(const char *str, size_t size, bool permanent);

ZEND_API extern zend_new_interned_string_func_t zend_new_interned_string;
ZEND_API extern zend_string_init_interned_func_t zend_string_init_interned;
/* Init an interned string if it already exists, but do not create a new one if it does not. */
ZEND_API extern zend_string_init_existing_interned_func_t zend_string_init_existing_interned;

ZEND_API zend_ulong ZEND_FASTCALL zend_string_hash_func(zend_string *str);
ZEND_API zend_ulong ZEND_FASTCALL zend_hash_func(const char *str, size_t len);
ZEND_API zend_string* ZEND_FASTCALL zend_interned_string_find_permanent(zend_string *str);

ZEND_API zend_string *zend_string_concat2(
	const char *str1, size_t str1_len,
	const char *str2, size_t str2_len);
ZEND_API zend_string *zend_string_concat3(
	const char *str1, size_t str1_len,
	const char *str2, size_t str2_len,
	const char *str3, size_t str3_len);

ZEND_API void zend_interned_strings_init(void);
ZEND_API void zend_interned_strings_dtor(void);
ZEND_API void zend_interned_strings_activate(void);
ZEND_API void zend_interned_strings_deactivate(void);
ZEND_API void zend_interned_strings_set_request_storage_handlers(
	zend_new_interned_string_func_t handler,
	zend_string_init_interned_func_t init_handler,
	zend_string_init_existing_interned_func_t init_existing_handler);
ZEND_API void zend_interned_strings_switch_storage(bool request);

ZEND_API extern zend_string  *zend_empty_string;
ZEND_API extern zend_string  *zend_one_char_string[256];
ZEND_API extern zend_string **zend_known_strings;

END_EXTERN_C()

/* Shortcuts */

#define ZSTR_VAL(zstr)  (zstr)->val
#define ZSTR_LEN(zstr)  (zstr)->len
#define ZSTR_H(zstr)    (zstr)->h
#define ZSTR_HASH(zstr) zend_string_hash_val(zstr)

/* Compatibility macros */

#define IS_INTERNED(s)	ZSTR_IS_INTERNED(s)
#define STR_EMPTY_ALLOC()	ZSTR_EMPTY_ALLOC()
#define _STR_HEADER_SIZE _ZSTR_HEADER_SIZE
#define STR_ALLOCA_ALLOC(str, _len, use_heap) ZSTR_ALLOCA_ALLOC(str, _len, use_heap)
#define STR_ALLOCA_INIT(str, s, len, use_heap) ZSTR_ALLOCA_INIT(str, s, len, use_heap)
#define STR_ALLOCA_FREE(str, use_heap) ZSTR_ALLOCA_FREE(str, use_heap)

/*---*/

#define ZSTR_IS_INTERNED(s)					(GC_FLAGS(s) & IS_STR_INTERNED)

#define ZSTR_EMPTY_ALLOC() zend_empty_string
#define ZSTR_CHAR(c) zend_one_char_string[c]
#define ZSTR_KNOWN(idx) zend_known_strings[idx]

#define _ZSTR_HEADER_SIZE XtOffsetOf(zend_string, val)

#define _ZSTR_STRUCT_SIZE(len) (_ZSTR_HEADER_SIZE + len + 1)

#define ZSTR_MAX_OVERHEAD (ZEND_MM_ALIGNED_SIZE(_ZSTR_HEADER_SIZE + 1))
#define ZSTR_MAX_LEN (SIZE_MAX - ZSTR_MAX_OVERHEAD)

#define ZSTR_ALLOCA_ALLOC(str, _len, use_heap) do { \
	(str) = (zend_string *)do_alloca(ZEND_MM_ALIGNED_SIZE_EX(_ZSTR_STRUCT_SIZE(_len), 8), (use_heap)); \
	GC_SET_REFCOUNT(str, 1); \
	GC_TYPE_INFO(str) = GC_STRING; \
	ZSTR_H(str) = 0; \
	ZSTR_LEN(str) = _len; \
} while (0)

#define ZSTR_ALLOCA_INIT(str, s, len, use_heap) do { \
	ZSTR_ALLOCA_ALLOC(str, len, use_heap); \
	memcpy(ZSTR_VAL(str), (s), (len)); \
	ZSTR_VAL(str)[(len)] = '\0'; \
} while (0)

#define ZSTR_ALLOCA_FREE(str, use_heap) free_alloca(str, use_heap)

#define ZSTR_INIT_LITERAL(s, persistent) (zend_string_init((s), strlen(s), (persistent)))

/*---*/

static zend_always_inline zend_ulong zend_string_hash_val(zend_string *s)
{
	return ZSTR_H(s) ? ZSTR_H(s) : zend_string_hash_func(s);
}

static zend_always_inline void zend_string_forget_hash_val(zend_string *s)
{
	ZSTR_H(s) = 0;
	GC_DEL_FLAGS(s, IS_STR_VALID_UTF8);
}

static zend_always_inline uint32_t zend_string_refcount(const zend_string *s)
{
	if (!ZSTR_IS_INTERNED(s)) {
		return GC_REFCOUNT(s);
	}
	return 1;
}

static zend_always_inline uint32_t zend_string_addref(zend_string *s)
{
	if (!ZSTR_IS_INTERNED(s)) {
		return GC_ADDREF(s);
	}
	return 1;
}

static zend_always_inline uint32_t zend_string_delref(zend_string *s)
{
	if (!ZSTR_IS_INTERNED(s)) {
		return GC_DELREF(s);
	}
	return 1;
}

static zend_always_inline zend_string *zend_string_alloc(size_t len, bool persistent)
{
	zend_string *ret = (zend_string *)pemalloc(ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(len)), persistent);

	GC_SET_REFCOUNT(ret, 1);
	GC_TYPE_INFO(ret) = GC_STRING | ((persistent ? IS_STR_PERSISTENT : 0) << GC_FLAGS_SHIFT);
	ZSTR_H(ret) = 0;
	ZSTR_LEN(ret) = len;
	return ret;
}

static zend_always_inline zend_string *zend_string_safe_alloc(size_t n, size_t m, size_t l, bool persistent)
{
	zend_string *ret = (zend_string *)safe_pemalloc(n, m, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(l)), persistent);

	GC_SET_REFCOUNT(ret, 1);
	GC_TYPE_INFO(ret) = GC_STRING | ((persistent ? IS_STR_PERSISTENT : 0) << GC_FLAGS_SHIFT);
	ZSTR_H(ret) = 0;
	ZSTR_LEN(ret) = (n * m) + l;
	return ret;
}

static zend_always_inline zend_string *zend_string_init(const char *str, size_t len, bool persistent)
{
	zend_string *ret = zend_string_alloc(len, persistent);

	memcpy(ZSTR_VAL(ret), str, len);
	ZSTR_VAL(ret)[len] = '\0';
	return ret;
}

static zend_always_inline zend_string *zend_string_init_fast(const char *str, size_t len)
{
	if (len > 1) {
		return zend_string_init(str, len, 0);
	} else if (len == 0) {
		return zend_empty_string;
	} else /* if (len == 1) */ {
		return ZSTR_CHAR((zend_uchar) *str);
	}
}

static zend_always_inline zend_string *zend_string_copy(zend_string *s)
{
	if (!ZSTR_IS_INTERNED(s)) {
		GC_ADDREF(s);
	}
	return s;
}

static zend_always_inline zend_string *zend_string_dup(zend_string *s, bool persistent)
{
	if (ZSTR_IS_INTERNED(s)) {
		return s;
	} else {
		return zend_string_init(ZSTR_VAL(s), ZSTR_LEN(s), persistent);
	}
}

static zend_always_inline zend_string *zend_string_separate(zend_string *s, bool persistent)
{
	if (ZSTR_IS_INTERNED(s) || GC_REFCOUNT(s) > 1) {
		if (!ZSTR_IS_INTERNED(s)) {
			GC_DELREF(s);
		}
		return zend_string_init(ZSTR_VAL(s), ZSTR_LEN(s), persistent);
	}

	zend_string_forget_hash_val(s);
	return s;
}

static zend_always_inline zend_string *zend_string_realloc(zend_string *s, size_t len, bool persistent)
{
	zend_string *ret;

	if (!ZSTR_IS_INTERNED(s)) {
		if (EXPECTED(GC_REFCOUNT(s) == 1)) {
			ret = (zend_string *)perealloc(s, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(len)), persistent);
			ZSTR_LEN(ret) = len;
			zend_string_forget_hash_val(ret);
			return ret;
		}
	}
	ret = zend_string_alloc(len, persistent);
	memcpy(ZSTR_VAL(ret), ZSTR_VAL(s), MIN(len, ZSTR_LEN(s)) + 1);
	if (!ZSTR_IS_INTERNED(s)) {
		GC_DELREF(s);
	}
	return ret;
}

static zend_always_inline zend_string *zend_string_extend(zend_string *s, size_t len, bool persistent)
{
	zend_string *ret;

	ZEND_ASSERT(len >= ZSTR_LEN(s));
	if (!ZSTR_IS_INTERNED(s)) {
		if (EXPECTED(GC_REFCOUNT(s) == 1)) {
			ret = (zend_string *)perealloc(s, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(len)), persistent);
			ZSTR_LEN(ret) = len;
			zend_string_forget_hash_val(ret);
			return ret;
		}
	}
	ret = zend_string_alloc(len, persistent);
	memcpy(ZSTR_VAL(ret), ZSTR_VAL(s), ZSTR_LEN(s) + 1);
	if (!ZSTR_IS_INTERNED(s)) {
		GC_DELREF(s);
	}
	return ret;
}

static zend_always_inline zend_string *zend_string_truncate(zend_string *s, size_t len, bool persistent)
{
	zend_string *ret;

	ZEND_ASSERT(len <= ZSTR_LEN(s));
	if (!ZSTR_IS_INTERNED(s)) {
		if (EXPECTED(GC_REFCOUNT(s) == 1)) {
			ret = (zend_string *)perealloc(s, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(len)), persistent);
			ZSTR_LEN(ret) = len;
			zend_string_forget_hash_val(ret);
			return ret;
		}
	}
	ret = zend_string_alloc(len, persistent);
	memcpy(ZSTR_VAL(ret), ZSTR_VAL(s), len + 1);
	if (!ZSTR_IS_INTERNED(s)) {
		GC_DELREF(s);
	}
	return ret;
}

static zend_always_inline zend_string *zend_string_safe_realloc(zend_string *s, size_t n, size_t m, size_t l, bool persistent)
{
	zend_string *ret;

	if (!ZSTR_IS_INTERNED(s)) {
		if (GC_REFCOUNT(s) == 1) {
			ret = (zend_string *)safe_perealloc(s, n, m, ZEND_MM_ALIGNED_SIZE(_ZSTR_STRUCT_SIZE(l)), persistent);
			ZSTR_LEN(ret) = (n * m) + l;
			zend_string_forget_hash_val(ret);
			return ret;
		}
	}
	ret = zend_string_safe_alloc(n, m, l, persistent);
	memcpy(ZSTR_VAL(ret), ZSTR_VAL(s), MIN((n * m) + l, ZSTR_LEN(s)) + 1);
	if (!ZSTR_IS_INTERNED(s)) {
		GC_DELREF(s);
	}
	return ret;
}

static zend_always_inline void zend_string_free(zend_string *s)
{
	if (!ZSTR_IS_INTERNED(s)) {
		ZEND_ASSERT(GC_REFCOUNT(s) <= 1);
		pefree(s, GC_FLAGS(s) & IS_STR_PERSISTENT);
	}
}

static zend_always_inline void zend_string_efree(zend_string *s)
{
	ZEND_ASSERT(!ZSTR_IS_INTERNED(s));
	ZEND_ASSERT(GC_REFCOUNT(s) <= 1);
	ZEND_ASSERT(!(GC_FLAGS(s) & IS_STR_PERSISTENT));
	efree(s);
}

static zend_always_inline void zend_string_release(zend_string *s)
{
	if (!ZSTR_IS_INTERNED(s)) {
		if (GC_DELREF(s) == 0) {
			pefree(s, GC_FLAGS(s) & IS_STR_PERSISTENT);
		}
	}
}

static zend_always_inline void zend_string_release_ex(zend_string *s, bool persistent)
{
	if (!ZSTR_IS_INTERNED(s)) {
		if (GC_DELREF(s) == 0) {
			if (persistent) {
				ZEND_ASSERT(GC_FLAGS(s) & IS_STR_PERSISTENT);
				free(s);
			} else {
				ZEND_ASSERT(!(GC_FLAGS(s) & IS_STR_PERSISTENT));
				efree(s);
			}
		}
	}
}

static zend_always_inline bool zend_string_equals_cstr(const zend_string *s1, const char *s2, size_t s2_length)
{
	return ZSTR_LEN(s1) == s2_length && !memcmp(ZSTR_VAL(s1), s2, s2_length);
}

#if defined(__GNUC__) && (defined(__i386__) || (defined(__x86_64__) && !defined(__ILP32__)))
BEGIN_EXTERN_C()
ZEND_API bool ZEND_FASTCALL zend_string_equal_val(const zend_string *s1, const zend_string *s2);
END_EXTERN_C()
#else
static zend_always_inline bool zend_string_equal_val(const zend_string *s1, const zend_string *s2)
{
	return !memcmp(ZSTR_VAL(s1), ZSTR_VAL(s2), ZSTR_LEN(s1));
}
#endif

static zend_always_inline bool zend_string_equal_content(const zend_string *s1, const zend_string *s2)
{
	return ZSTR_LEN(s1) == ZSTR_LEN(s2) && zend_string_equal_val(s1, s2);
}

static zend_always_inline bool zend_string_equals(const zend_string *s1, const zend_string *s2)
{
	return s1 == s2 || zend_string_equal_content(s1, s2);
}

#define zend_string_equals_ci(s1, s2) \
	(ZSTR_LEN(s1) == ZSTR_LEN(s2) && !zend_binary_strcasecmp(ZSTR_VAL(s1), ZSTR_LEN(s1), ZSTR_VAL(s2), ZSTR_LEN(s2)))

#define zend_string_equals_literal_ci(str, c) \
	(ZSTR_LEN(str) == sizeof(c) - 1 && !zend_binary_strcasecmp(ZSTR_VAL(str), ZSTR_LEN(str), (c), sizeof(c) - 1))

#define zend_string_equals_literal(str, literal) \
	zend_string_equals_cstr(str, literal, strlen(literal))

static zend_always_inline bool zend_string_starts_with_cstr(const zend_string *str, const char *prefix, size_t prefix_length)
{
	return ZSTR_LEN(str) >= prefix_length && !memcmp(ZSTR_VAL(str), prefix, prefix_length);
}

static zend_always_inline bool zend_string_starts_with(const zend_string *str, const zend_string *prefix)
{
	return zend_string_starts_with_cstr(str, ZSTR_VAL(prefix), ZSTR_LEN(prefix));
}

#define zend_string_starts_with_literal(str, prefix) \
	zend_string_starts_with_cstr(str, prefix, strlen(prefix))

/*
 * DJBX33A (Daniel J. Bernstein, Times 33 with Addition)
 *
 * This is Daniel J. Bernstein's popular `times 33' hash function as
 * posted by him years ago on comp.lang.c. It basically uses a function
 * like ``hash(i) = hash(i-1) * 33 + str[i]''. This is one of the best
 * known hash functions for strings. Because it is both computed very
 * fast and distributes very well.
 *
 * The magic of number 33, i.e. why it works better than many other
 * constants, prime or not, has never been adequately explained by
 * anyone. So I try an explanation: if one experimentally tests all
 * multipliers between 1 and 256 (as RSE did now) one detects that even
 * numbers are not usable at all. The remaining 128 odd numbers
 * (except for the number 1) work more or less all equally well. They
 * all distribute in an acceptable way and this way fill a hash table
 * with an average percent of approx. 86%.
 *
 * If one compares the Chi^2 values of the variants, the number 33 not
 * even has the best value. But the number 33 and a few other equally
 * good numbers like 17, 31, 63, 127 and 129 have nevertheless a great
 * advantage to the remaining numbers in the large set of possible
 * multipliers: their multiply operation can be replaced by a faster
 * operation based on just one shift plus either a single addition
 * or subtraction operation. And because a hash function has to both
 * distribute good _and_ has to be very fast to compute, those few
 * numbers should be preferred and seems to be the reason why Daniel J.
 * Bernstein also preferred it.
 *
 *
 *                  -- Ralf S. Engelschall <rse@engelschall.com>
 */

static zend_always_inline zend_ulong zend_inline_hash_func(const char *str, size_t len)
{
	zend_ulong hash = Z_UL(5381);

#if defined(_WIN32) || defined(__i386__) || defined(__x86_64__) || defined(__aarch64__)
	/* Version with multiplication works better on modern CPU */
	for (; len >= 8; len -= 8, str += 8) {
# if defined(__aarch64__) && !defined(WORDS_BIGENDIAN)
		/* On some architectures it is beneficial to load 8 bytes at a
		   time and extract each byte with a bit field extract instr. */
		uint64_t chunk;

		memcpy(&chunk, str, sizeof(chunk));
		hash =
			hash                        * 33 * 33 * 33 * 33 +
			((chunk >> (8 * 0)) & 0xff) * 33 * 33 * 33 +
			((chunk >> (8 * 1)) & 0xff) * 33 * 33 +
			((chunk >> (8 * 2)) & 0xff) * 33 +
			((chunk >> (8 * 3)) & 0xff);
		hash =
			hash                        * 33 * 33 * 33 * 33 +
			((chunk >> (8 * 4)) & 0xff) * 33 * 33 * 33 +
			((chunk >> (8 * 5)) & 0xff) * 33 * 33 +
			((chunk >> (8 * 6)) & 0xff) * 33 +
			((chunk >> (8 * 7)) & 0xff);
# else
		hash =
			hash   * Z_L(33 * 33 * 33 * 33) +
			str[0] * Z_L(33 * 33 * 33) +
			str[1] * Z_L(33 * 33) +
			str[2] * Z_L(33) +
			str[3];
		hash =
			hash   * Z_L(33 * 33 * 33 * 33) +
			str[4] * Z_L(33 * 33 * 33) +
			str[5] * Z_L(33 * 33) +
			str[6] * Z_L(33) +
			str[7];
# endif
	}
	if (len >= 4) {
		hash =
			hash   * Z_L(33 * 33 * 33 * 33) +
			str[0] * Z_L(33 * 33 * 33) +
			str[1] * Z_L(33 * 33) +
			str[2] * Z_L(33) +
			str[3];
		len -= 4;
		str += 4;
	}
	if (len >= 2) {
		if (len > 2) {
			hash =
				hash   * Z_L(33 * 33 * 33) +
				str[0] * Z_L(33 * 33) +
				str[1] * Z_L(33) +
				str[2];
		} else {
			hash =
				hash   * Z_L(33 * 33) +
				str[0] * Z_L(33) +
				str[1];
		}
	} else if (len != 0) {
		hash = hash * Z_L(33) + *str;
	}
#else
	/* variant with the hash unrolled eight times */
	for (; len >= 8; len -= 8) {
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
		hash = ((hash << 5) + hash) + *str++;
	}
	switch (len) {
		case 7: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 6: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 5: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 4: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 3: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 2: hash = ((hash << 5) + hash) + *str++; /* fallthrough... */
		case 1: hash = ((hash << 5) + hash) + *str++; break;
		case 0: break;
EMPTY_SWITCH_DEFAULT_CASE()
	}
#endif

	/* Hash value can't be zero, so we always set the high bit */
#if SIZEOF_ZEND_LONG == 8
	return hash | Z_UL(0x8000000000000000);
#elif SIZEOF_ZEND_LONG == 4
	return hash | Z_UL(0x80000000);
#else
# error "Unknown SIZEOF_ZEND_LONG"
#endif
}

#define ZEND_KNOWN_STRINGS(_) \
	_(ZEND_STR_FILE,                   "file") \
	_(ZEND_STR_LINE,                   "line") \
	_(ZEND_STR_FUNCTION,               "function") \
	_(ZEND_STR_CLASS,                  "class") \
	_(ZEND_STR_OBJECT,                 "object") \
	_(ZEND_STR_TYPE,                   "type") \
	_(ZEND_STR_OBJECT_OPERATOR,        "->") \
	_(ZEND_STR_PAAMAYIM_NEKUDOTAYIM,   "::") \
	_(ZEND_STR_ARGS,                   "args") \
	_(ZEND_STR_UNKNOWN,                "unknown") \
	_(ZEND_STR_UNKNOWN_CAPITALIZED,    "Unknown") \
	_(ZEND_STR_EVAL,                   "eval") \
	_(ZEND_STR_INCLUDE,                "include") \
	_(ZEND_STR_REQUIRE,                "require") \
	_(ZEND_STR_INCLUDE_ONCE,           "include_once") \
	_(ZEND_STR_REQUIRE_ONCE,           "require_once") \
	_(ZEND_STR_SCALAR,                 "scalar") \
	_(ZEND_STR_ERROR_REPORTING,        "error_reporting") \
	_(ZEND_STR_STATIC,                 "static") \
	_(ZEND_STR_THIS,                   "this") \
	_(ZEND_STR_VALUE,                  "value") \
	_(ZEND_STR_KEY,                    "key") \
	_(ZEND_STR_MAGIC_INVOKE,           "__invoke") \
	_(ZEND_STR_PREVIOUS,               "previous") \
	_(ZEND_STR_CODE,                   "code") \
	_(ZEND_STR_MESSAGE,                "message") \
	_(ZEND_STR_SEVERITY,               "severity") \
	_(ZEND_STR_STRING,                 "string") \
	_(ZEND_STR_TRACE,                  "trace") \
	_(ZEND_STR_SCHEME,                 "scheme") \
	_(ZEND_STR_HOST,                   "host") \
	_(ZEND_STR_PORT,                   "port") \
	_(ZEND_STR_USER,                   "user") \
	_(ZEND_STR_PASS,                   "pass") \
	_(ZEND_STR_PATH,                   "path") \
	_(ZEND_STR_QUERY,                  "query") \
	_(ZEND_STR_FRAGMENT,               "fragment") \
	_(ZEND_STR_NULL,                   "NULL") \
	_(ZEND_STR_BOOLEAN,                "boolean") \
	_(ZEND_STR_INTEGER,                "integer") \
	_(ZEND_STR_DOUBLE,                 "double") \
	_(ZEND_STR_ARRAY,                  "array") \
	_(ZEND_STR_RESOURCE,               "resource") \
	_(ZEND_STR_CLOSED_RESOURCE,        "resource (closed)") \
	_(ZEND_STR_NAME,                   "name") \
	_(ZEND_STR_ARGV,                   "argv") \
	_(ZEND_STR_ARGC,                   "argc") \
	_(ZEND_STR_ARRAY_CAPITALIZED,      "Array") \
	_(ZEND_STR_BOOL,                   "bool") \
	_(ZEND_STR_INT,                    "int") \
	_(ZEND_STR_FLOAT,                  "float") \
	_(ZEND_STR_CALLABLE,               "callable") \
	_(ZEND_STR_ITERABLE,               "iterable") \
	_(ZEND_STR_VOID,                   "void") \
	_(ZEND_STR_NEVER,                  "never") \
	_(ZEND_STR_FALSE,                  "false") \
	_(ZEND_STR_TRUE,                   "true") \
	_(ZEND_STR_NULL_LOWERCASE,         "null") \
	_(ZEND_STR_MIXED,                  "mixed") \
	_(ZEND_STR_TRAVERSABLE,            "Traversable") \
	_(ZEND_STR_SLEEP,                  "__sleep") \
	_(ZEND_STR_WAKEUP,                 "__wakeup") \
	_(ZEND_STR_CASES,                  "cases") \
	_(ZEND_STR_FROM,                   "from") \
	_(ZEND_STR_TRYFROM,                "tryFrom") \
	_(ZEND_STR_TRYFROM_LOWERCASE,      "tryfrom") \
	_(ZEND_STR_AUTOGLOBAL_SERVER,      "_SERVER") \
	_(ZEND_STR_AUTOGLOBAL_ENV,         "_ENV") \
	_(ZEND_STR_AUTOGLOBAL_REQUEST,     "_REQUEST") \
	_(ZEND_STR_COUNT,                  "count") \
	_(ZEND_STR_SENSITIVEPARAMETER,     "SensitiveParameter") \


typedef enum _zend_known_string_id {
#define _ZEND_STR_ID(id, str) id,
ZEND_KNOWN_STRINGS(_ZEND_STR_ID)
#undef _ZEND_STR_ID
	ZEND_STR_LAST_KNOWN
} zend_known_string_id;

#endif /* ZEND_STRING_H */
