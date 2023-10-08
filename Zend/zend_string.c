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

#include "zend.h"
#include "zend_globals.h"

#ifdef HAVE_VALGRIND
# include "valgrind/callgrind.h"
#endif

ZEND_API zend_new_interned_string_func_t zend_new_interned_string;
ZEND_API zend_string_init_interned_func_t zend_string_init_interned;
ZEND_API zend_string_init_existing_interned_func_t zend_string_init_existing_interned;

static zend_string* ZEND_FASTCALL zend_new_interned_string_permanent(zend_string *str);
static zend_string* ZEND_FASTCALL zend_new_interned_string_request(zend_string *str);
static zend_string* ZEND_FASTCALL zend_string_init_interned_permanent(const char *str, size_t size, bool permanent);
static zend_string* ZEND_FASTCALL zend_string_init_existing_interned_permanent(const char *str, size_t size, bool permanent);
static zend_string* ZEND_FASTCALL zend_string_init_interned_request(const char *str, size_t size, bool permanent);
static zend_string* ZEND_FASTCALL zend_string_init_existing_interned_request(const char *str, size_t size, bool permanent);

/* Any strings interned in the startup phase. Common to all the threads,
   won't be free'd until process exit. If we want an ability to
   add permanent strings even after startup, it would be still
   possible on costs of locking in the thread safe builds. */
static HashTable interned_strings_permanent;

static zend_new_interned_string_func_t interned_string_request_handler = zend_new_interned_string_request;
static zend_string_init_interned_func_t interned_string_init_request_handler = zend_string_init_interned_request;
static zend_string_init_existing_interned_func_t interned_string_init_existing_request_handler = zend_string_init_existing_interned_request;

ZEND_API zend_string  *zend_empty_string = NULL;
ZEND_API zend_string  *zend_one_char_string[256];
ZEND_API zend_string **zend_known_strings = NULL;

ZEND_API zend_ulong ZEND_FASTCALL zend_string_hash_func(zend_string *str)
{
	return ZSTR_H(str) = zend_hash_func(ZSTR_VAL(str), ZSTR_LEN(str));
}

ZEND_API zend_ulong ZEND_FASTCALL zend_hash_func(const char *str, size_t len)
{
	return zend_inline_hash_func(str, len);
}

static void _str_dtor(zval *zv)
{
	zend_string *str = Z_STR_P(zv);
	pefree(str, GC_FLAGS(str) & IS_STR_PERSISTENT);
}

static const char *known_strings[] = {
#define _ZEND_STR_DSC(id, str) str,
ZEND_KNOWN_STRINGS(_ZEND_STR_DSC)
#undef _ZEND_STR_DSC
	NULL
};

static zend_always_inline void zend_init_interned_strings_ht(HashTable *interned_strings, bool permanent)
{
	zend_hash_init(interned_strings, 1024, NULL, _str_dtor, permanent);
	if (permanent) {
		zend_hash_real_init_mixed(interned_strings);
	}
}

ZEND_API void zend_interned_strings_init(void)
{
	char s[2];
	unsigned int i;
	zend_string *str;

	interned_string_request_handler = zend_new_interned_string_request;
	interned_string_init_request_handler = zend_string_init_interned_request;
	interned_string_init_existing_request_handler = zend_string_init_existing_interned_request;

	zend_empty_string = NULL;
	zend_known_strings = NULL;

	zend_init_interned_strings_ht(&interned_strings_permanent, 1);

	zend_new_interned_string = zend_new_interned_string_permanent;
	zend_string_init_interned = zend_string_init_interned_permanent;
	zend_string_init_existing_interned = zend_string_init_existing_interned_permanent;

	/* interned empty string */
	str = zend_string_alloc(sizeof("")-1, 1);
	ZSTR_VAL(str)[0] = '\000';
	zend_empty_string = zend_new_interned_string_permanent(str);
	GC_ADD_FLAGS(zend_empty_string, IS_STR_VALID_UTF8);

	s[1] = 0;
	for (i = 0; i < 256; i++) {
		s[0] = i;
		zend_one_char_string[i] = zend_new_interned_string_permanent(zend_string_init(s, 1, 1));
		if (i < 0x80) {
			GC_ADD_FLAGS(zend_one_char_string[i], IS_STR_VALID_UTF8);
		}
	}

	/* known strings */
	zend_known_strings = pemalloc(sizeof(zend_string*) * ((sizeof(known_strings) / sizeof(known_strings[0]) - 1)), 1);
	for (i = 0; i < (sizeof(known_strings) / sizeof(known_strings[0])) - 1; i++) {
		str = zend_string_init(known_strings[i], strlen(known_strings[i]), 1);
		zend_known_strings[i] = zend_new_interned_string_permanent(str);
		GC_ADD_FLAGS(zend_known_strings[i], IS_STR_VALID_UTF8);
	}
}

ZEND_API void zend_interned_strings_dtor(void)
{
	zend_hash_destroy(&interned_strings_permanent);

	free(zend_known_strings);
	zend_known_strings = NULL;
}

static zend_always_inline zend_string *zend_interned_string_ht_lookup_ex(zend_ulong h, const char *str, size_t size, HashTable *interned_strings)
{
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;

	nIndex = h | interned_strings->nTableMask;
	idx = HT_HASH(interned_strings, nIndex);
	while (idx != HT_INVALID_IDX) {
		p = HT_HASH_TO_BUCKET(interned_strings, idx);
		if ((p->h == h) && zend_string_equals_cstr(p->key, str, size)) {
			return p->key;
		}
		idx = Z_NEXT(p->val);
	}

	return NULL;
}

static zend_always_inline zend_string *zend_interned_string_ht_lookup(zend_string *str, HashTable *interned_strings)
{
	zend_ulong h = ZSTR_H(str);
	uint32_t nIndex;
	uint32_t idx;
	Bucket *p;

	nIndex = h | interned_strings->nTableMask;
	idx = HT_HASH(interned_strings, nIndex);
	while (idx != HT_INVALID_IDX) {
		p = HT_HASH_TO_BUCKET(interned_strings, idx);
		if ((p->h == h) && zend_string_equal_content(p->key, str)) {
			return p->key;
		}
		idx = Z_NEXT(p->val);
	}

	return NULL;
}

/* This function might be not thread safe at least because it would update the
   hash val in the passed string. Be sure it is called in the appropriate context. */
static zend_always_inline zend_string *zend_add_interned_string(zend_string *str, HashTable *interned_strings, uint32_t flags)
{
	zval val;

	GC_SET_REFCOUNT(str, 1);
	GC_ADD_FLAGS(str, IS_STR_INTERNED | flags);

	ZVAL_INTERNED_STR(&val, str);

	zend_hash_add_new(interned_strings, str, &val);

	return str;
}

ZEND_API zend_string* ZEND_FASTCALL zend_interned_string_find_permanent(zend_string *str)
{
	zend_string_hash_val(str);
	return zend_interned_string_ht_lookup(str, &interned_strings_permanent);
}

static zend_string* ZEND_FASTCALL zend_init_string_for_interning(zend_string *str, bool persistent)
{
	uint32_t flags = ZSTR_GET_COPYABLE_CONCAT_PROPERTIES(str);
	zend_ulong h = ZSTR_H(str);
	zend_string_delref(str);
	str = zend_string_init(ZSTR_VAL(str), ZSTR_LEN(str), persistent);
	GC_ADD_FLAGS(str, flags);
	ZSTR_H(str) = h;
	return str;
}

static zend_string* ZEND_FASTCALL zend_new_interned_string_permanent(zend_string *str)
{
	zend_string *ret;

	if (ZSTR_IS_INTERNED(str)) {
		return str;
	}

	zend_string_hash_val(str);
	ret = zend_interned_string_ht_lookup(str, &interned_strings_permanent);
	if (ret) {
		zend_string_release(str);
		return ret;
	}

	ZEND_ASSERT(GC_FLAGS(str) & GC_PERSISTENT);
	if (GC_REFCOUNT(str) > 1) {
		str = zend_init_string_for_interning(str, true);
	}

	return zend_add_interned_string(str, &interned_strings_permanent, IS_STR_PERMANENT);
}

static zend_string* ZEND_FASTCALL zend_new_interned_string_request(zend_string *str)
{
	zend_string *ret;

	if (ZSTR_IS_INTERNED(str)) {
		return str;
	}

	zend_string_hash_val(str);

	/* Check for permanent strings, the table is readonly at this point. */
	ret = zend_interned_string_ht_lookup(str, &interned_strings_permanent);
	if (ret) {
		zend_string_release(str);
		return ret;
	}

	ret = zend_interned_string_ht_lookup(str, &CG(interned_strings));
	if (ret) {
		zend_string_release(str);
		return ret;
	}

	/* Create a short living interned, freed after the request. */
#if ZEND_RC_DEBUG
	if (zend_rc_debug) {
		/* PHP shouldn't create persistent interned string during request,
		 * but at least dl() may do this */
		ZEND_ASSERT(!(GC_FLAGS(str) & GC_PERSISTENT));
	}
#endif
	if (GC_REFCOUNT(str) > 1) {
		str = zend_init_string_for_interning(str, false);
	}

	ret = zend_add_interned_string(str, &CG(interned_strings), 0);

	return ret;
}

static zend_string* ZEND_FASTCALL zend_string_init_interned_permanent(const char *str, size_t size, bool permanent)
{
	zend_string *ret;
	zend_ulong h = zend_inline_hash_func(str, size);

	ret = zend_interned_string_ht_lookup_ex(h, str, size, &interned_strings_permanent);
	if (ret) {
		return ret;
	}

	ZEND_ASSERT(permanent);
	ret = zend_string_init(str, size, permanent);
	ZSTR_H(ret) = h;
	return zend_add_interned_string(ret, &interned_strings_permanent, IS_STR_PERMANENT);
}

static zend_string* ZEND_FASTCALL zend_string_init_existing_interned_permanent(const char *str, size_t size, bool permanent)
{
	zend_ulong h = zend_inline_hash_func(str, size);
	zend_string *ret = zend_interned_string_ht_lookup_ex(h, str, size, &interned_strings_permanent);
	if (ret) {
		return ret;
	}

	ZEND_ASSERT(permanent);
	ret = zend_string_init(str, size, permanent);
	ZSTR_H(ret) = h;
	return ret;
}

static zend_string* ZEND_FASTCALL zend_string_init_interned_request(const char *str, size_t size, bool permanent)
{
	zend_string *ret;
	zend_ulong h = zend_inline_hash_func(str, size);

	/* Check for permanent strings, the table is readonly at this point. */
	ret = zend_interned_string_ht_lookup_ex(h, str, size, &interned_strings_permanent);
	if (ret) {
		return ret;
	}

	ret = zend_interned_string_ht_lookup_ex(h, str, size, &CG(interned_strings));
	if (ret) {
		return ret;
	}

#if ZEND_RC_DEBUG
	if (zend_rc_debug) {
		/* PHP shouldn't create persistent interned string during request,
		 * but at least dl() may do this */
		ZEND_ASSERT(!permanent);
	}
#endif
	ret = zend_string_init(str, size, permanent);
	ZSTR_H(ret) = h;

	/* Create a short living interned, freed after the request. */
	return zend_add_interned_string(ret, &CG(interned_strings), 0);
}

static zend_string* ZEND_FASTCALL zend_string_init_existing_interned_request(const char *str, size_t size, bool permanent)
{
	zend_ulong h = zend_inline_hash_func(str, size);
	zend_string *ret = zend_interned_string_ht_lookup_ex(h, str, size, &interned_strings_permanent);
	if (ret) {
		return ret;
	}

	ret = zend_interned_string_ht_lookup_ex(h, str, size, &CG(interned_strings));
	if (ret) {
		return ret;
	}

	ZEND_ASSERT(!permanent);
	ret = zend_string_init(str, size, permanent);
	ZSTR_H(ret) = h;
	return ret;
}

ZEND_API void zend_interned_strings_activate(void)
{
	zend_init_interned_strings_ht(&CG(interned_strings), 0);
}

ZEND_API void zend_interned_strings_deactivate(void)
{
	zend_hash_destroy(&CG(interned_strings));
}

ZEND_API void zend_interned_strings_set_request_storage_handlers(zend_new_interned_string_func_t handler, zend_string_init_interned_func_t init_handler, zend_string_init_existing_interned_func_t init_existing_handler)
{
	interned_string_request_handler = handler;
	interned_string_init_request_handler = init_handler;
	interned_string_init_existing_request_handler = init_existing_handler;
}

ZEND_API void zend_interned_strings_switch_storage(bool request)
{
	if (request) {
		zend_new_interned_string = interned_string_request_handler;
		zend_string_init_interned = interned_string_init_request_handler;
		zend_string_init_existing_interned = interned_string_init_existing_request_handler;
	} else {
		zend_new_interned_string = zend_new_interned_string_permanent;
		zend_string_init_interned = zend_string_init_interned_permanent;
		zend_string_init_existing_interned = zend_string_init_existing_interned_permanent;
	}
}

/* Even if we don't build with valgrind support, include the symbol so that valgrind available
 * only at runtime will not result in false positives. */
#ifndef I_REPLACE_SONAME_FNNAME_ZU
# define I_REPLACE_SONAME_FNNAME_ZU(soname, fnname) _vgr00000ZU_ ## soname ## _ ## fnname
#endif

/* See GH-9068 */
#if defined(__GNUC__) && (__GNUC__ >= 11 || defined(__clang__)) && __has_attribute(no_caller_saved_registers)
# define NO_CALLER_SAVED_REGISTERS __attribute__((no_caller_saved_registers))
# ifndef __clang__
#  pragma GCC push_options
#  pragma GCC target ("general-regs-only")
#  define POP_OPTIONS
# endif
#else
# define NO_CALLER_SAVED_REGISTERS
#endif

ZEND_API bool ZEND_FASTCALL NO_CALLER_SAVED_REGISTERS I_REPLACE_SONAME_FNNAME_ZU(NONE,zend_string_equal_val)(const zend_string *s1, const zend_string *s2)
{
	return !memcmp(ZSTR_VAL(s1), ZSTR_VAL(s2), ZSTR_LEN(s1));
}

#ifdef POP_OPTIONS
# pragma GCC pop_options
# undef POP_OPTIONS
#endif

#if defined(__GNUC__) && defined(__i386__)
ZEND_API bool ZEND_FASTCALL zend_string_equal_val(const zend_string *s1, const zend_string *s2)
{
	const char *ptr = ZSTR_VAL(s1);
	size_t delta = (const char*)s2 - (const char*)s1;
	size_t len = ZSTR_LEN(s1);
	zend_ulong ret;

	__asm__ (
		".LL0%=:\n\t"
		"movl (%2,%3), %0\n\t"
		"xorl (%2), %0\n\t"
		"jne .LL1%=\n\t"
		"addl $0x4, %2\n\t"
		"subl $0x4, %1\n\t"
		"ja .LL0%=\n\t"
		"movl $0x1, %0\n\t"
		"jmp .LL3%=\n\t"
		".LL1%=:\n\t"
		"cmpl $0x4,%1\n\t"
		"jb .LL2%=\n\t"
		"xorl %0, %0\n\t"
		"jmp .LL3%=\n\t"
		".LL2%=:\n\t"
		"negl %1\n\t"
		"lea 0x20(,%1,8), %1\n\t"
		"shll %b1, %0\n\t"
		"sete %b0\n\t"
		"movzbl %b0, %0\n\t"
		".LL3%=:\n"
		: "=&a"(ret),
		  "+c"(len),
		  "+r"(ptr)
		: "r"(delta)
		: "cc");
	return ret;
}

#elif defined(__GNUC__) && defined(__x86_64__) && !defined(__ILP32__)
ZEND_API bool ZEND_FASTCALL zend_string_equal_val(const zend_string *s1, const zend_string *s2)
{
	const char *ptr = ZSTR_VAL(s1);
	size_t delta = (const char*)s2 - (const char*)s1;
	size_t len = ZSTR_LEN(s1);
	zend_ulong ret;

	__asm__ (
		".LL0%=:\n\t"
		"movq (%2,%3), %0\n\t"
		"xorq (%2), %0\n\t"
		"jne .LL1%=\n\t"
		"addq $0x8, %2\n\t"
		"subq $0x8, %1\n\t"
		"ja .LL0%=\n\t"
		"movq $0x1, %0\n\t"
		"jmp .LL3%=\n\t"
		".LL1%=:\n\t"
		"cmpq $0x8,%1\n\t"
		"jb .LL2%=\n\t"
		"xorq %0, %0\n\t"
		"jmp .LL3%=\n\t"
		".LL2%=:\n\t"
		"negq %1\n\t"
		"lea 0x40(,%1,8), %1\n\t"
		"shlq %b1, %0\n\t"
		"sete %b0\n\t"
		"movzbq %b0, %0\n\t"
		".LL3%=:\n"
		: "=&a"(ret),
		  "+c"(len),
		  "+r"(ptr)
		: "r"(delta)
		: "cc");
	return ret;
}
#endif

ZEND_API zend_string *zend_string_concat2(
		const char *str1, size_t str1_len,
		const char *str2, size_t str2_len)
{
	size_t len = str1_len + str2_len;
	zend_string *res = zend_string_alloc(len, 0);

	memcpy(ZSTR_VAL(res), str1, str1_len);
	memcpy(ZSTR_VAL(res) + str1_len, str2, str2_len);
	ZSTR_VAL(res)[len] = '\0';

	return res;
}

ZEND_API zend_string *zend_string_concat3(
		const char *str1, size_t str1_len,
		const char *str2, size_t str2_len,
		const char *str3, size_t str3_len)
{
	size_t len = str1_len + str2_len + str3_len;
	zend_string *res = zend_string_alloc(len, 0);

	memcpy(ZSTR_VAL(res), str1, str1_len);
	memcpy(ZSTR_VAL(res) + str1_len, str2, str2_len);
	memcpy(ZSTR_VAL(res) + str1_len + str2_len, str3, str3_len);
	ZSTR_VAL(res)[len] = '\0';

	return res;
}
