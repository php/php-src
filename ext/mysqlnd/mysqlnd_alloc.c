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
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_statistics.h"

#define MYSQLND_DEBUG_MEMORY 1

static const char mysqlnd_emalloc_name[]	= "_mysqlnd_emalloc";
static const char mysqlnd_pemalloc_name[]	= "_mysqlnd_pemalloc";
static const char mysqlnd_ecalloc_name[]	= "_mysqlnd_ecalloc";
static const char mysqlnd_pecalloc_name[]	= "_mysqlnd_pecalloc";
static const char mysqlnd_erealloc_name[]	= "_mysqlnd_erealloc";
static const char mysqlnd_perealloc_name[]	= "_mysqlnd_perealloc";
static const char mysqlnd_efree_name[]		= "_mysqlnd_efree";
static const char mysqlnd_pefree_name[]		= "_mysqlnd_pefree";
static const char mysqlnd_pememdup_name[]	= "_mysqlnd_pememdup";
static const char mysqlnd_pestrndup_name[]	= "_mysqlnd_pestrndup";
static const char mysqlnd_pestrdup_name[]	= "_mysqlnd_pestrdup";

PHPAPI const char * mysqlnd_debug_std_no_trace_funcs[] =
{
	mysqlnd_emalloc_name,
	mysqlnd_ecalloc_name,
	mysqlnd_efree_name,
	mysqlnd_erealloc_name,
	mysqlnd_pemalloc_name,
	mysqlnd_pecalloc_name,
	mysqlnd_pefree_name,
	mysqlnd_perealloc_name,
	mysqlnd_pestrndup_name,
	mysqlnd_read_header_name,
	mysqlnd_read_body_name,
	NULL /* must be always last */
};

#if MYSQLND_DEBUG_MEMORY


#if ZEND_DEBUG
#else
#define __zend_orig_filename "/unknown/unknown"
#define __zend_orig_lineno   0
#endif

#define EXTRA_SIZE ZEND_MM_ALIGNED_SIZE(sizeof(size_t))
#define REAL_SIZE(s) (collect_memory_statistics? (s) + EXTRA_SIZE : (s))
#define REAL_PTR(p) (collect_memory_statistics && (p)? (((char *)(p)) - EXTRA_SIZE) : (p))
#define FAKE_PTR(p) (collect_memory_statistics && (p)? (((char *)(p)) + EXTRA_SIZE) : (p))

/* {{{ _mysqlnd_emalloc */
static void * _mysqlnd_emalloc(size_t size MYSQLND_MEM_D)
{
	void *ret;
	bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	TRACE_ALLOC_ENTER(mysqlnd_emalloc_name);
	ret = emalloc_rel(REAL_SIZE(size));

	TRACE_ALLOC_INF_FMT("size=%zu ptr=%p", size, ret);

	if (collect_memory_statistics) {
		*(size_t *) ret = size;
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(STAT_MEM_EMALLOC_COUNT, 1, STAT_MEM_EMALLOC_AMOUNT, size);
	}
	TRACE_ALLOC_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_pemalloc */
static void * _mysqlnd_pemalloc(size_t size, bool persistent MYSQLND_MEM_D)
{
	void *ret;
	bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	TRACE_ALLOC_ENTER(mysqlnd_pemalloc_name);
	ret = pemalloc_rel(REAL_SIZE(size), persistent);

	TRACE_ALLOC_INF_FMT("size=%zu ptr=%p persistent=%u", size, ret, persistent);

	if (collect_memory_statistics) {
		enum mysqlnd_collected_stats s1 = persistent? STAT_MEM_MALLOC_COUNT:STAT_MEM_EMALLOC_COUNT;
		enum mysqlnd_collected_stats s2 = persistent? STAT_MEM_MALLOC_AMOUNT:STAT_MEM_EMALLOC_AMOUNT;
		*(size_t *) ret = size;
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(s1, 1, s2, size);
	}

	TRACE_ALLOC_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_ecalloc */
static void * _mysqlnd_ecalloc(unsigned int nmemb, size_t size MYSQLND_MEM_D)
{
	void *ret;
	bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	TRACE_ALLOC_ENTER(mysqlnd_ecalloc_name);
	TRACE_ALLOC_INF_FMT("before: %zu", zend_memory_usage(FALSE));
	ret = ecalloc_rel(nmemb, REAL_SIZE(size));

	TRACE_ALLOC_INF_FMT("after : %zu", zend_memory_usage(FALSE));
	TRACE_ALLOC_INF_FMT("size=%zu ptr=%p", size, ret);
	if (collect_memory_statistics) {
		*(size_t *) ret = size;
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(STAT_MEM_ECALLOC_COUNT, 1, STAT_MEM_ECALLOC_AMOUNT, size);
	}
	TRACE_ALLOC_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_pecalloc */
static void * _mysqlnd_pecalloc(unsigned int nmemb, size_t size, bool persistent MYSQLND_MEM_D)
{
	void *ret;
	bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	TRACE_ALLOC_ENTER(mysqlnd_pecalloc_name);
	ret = pecalloc_rel(nmemb, REAL_SIZE(size), persistent);

	TRACE_ALLOC_INF_FMT("size=%zu ptr=%p", size, ret);

	if (collect_memory_statistics) {
		enum mysqlnd_collected_stats s1 = persistent? STAT_MEM_CALLOC_COUNT:STAT_MEM_ECALLOC_COUNT;
		enum mysqlnd_collected_stats s2 = persistent? STAT_MEM_CALLOC_AMOUNT:STAT_MEM_ECALLOC_AMOUNT;
		*(size_t *) ret = size;
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(s1, 1, s2, size);
	}

	TRACE_ALLOC_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_erealloc */
static void * _mysqlnd_erealloc(void *ptr, size_t new_size MYSQLND_MEM_D)
{
	void *ret;
	bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	size_t old_size = collect_memory_statistics && ptr? *(size_t *) (((char*)ptr) - sizeof(size_t)) : 0;
	TRACE_ALLOC_ENTER(mysqlnd_erealloc_name);
	TRACE_ALLOC_INF_FMT("ptr=%p old_size=%zu, new_size=%zu", ptr, old_size, new_size);
	ret = erealloc_rel(REAL_PTR(ptr), REAL_SIZE(new_size));

	TRACE_ALLOC_INF_FMT("new_ptr=%p", (char*)ret);
	if (collect_memory_statistics) {
		*(size_t *) ret = new_size;
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(STAT_MEM_EREALLOC_COUNT, 1, STAT_MEM_EREALLOC_AMOUNT, new_size);
	}
	TRACE_ALLOC_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_perealloc */
static void * _mysqlnd_perealloc(void *ptr, size_t new_size, bool persistent MYSQLND_MEM_D)
{
	void *ret;
	bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	size_t old_size = collect_memory_statistics && ptr? *(size_t *) (((char*)ptr) - sizeof(size_t)) : 0;
	TRACE_ALLOC_ENTER(mysqlnd_perealloc_name);
	TRACE_ALLOC_INF_FMT("ptr=%p old_size=%zu new_size=%zu   persistent=%u", ptr, old_size, new_size, persistent);
	ret = perealloc_rel(REAL_PTR(ptr), REAL_SIZE(new_size), persistent);

	TRACE_ALLOC_INF_FMT("new_ptr=%p", (char*)ret);

	if (collect_memory_statistics) {
		enum mysqlnd_collected_stats s1 = persistent? STAT_MEM_REALLOC_COUNT:STAT_MEM_EREALLOC_COUNT;
		enum mysqlnd_collected_stats s2 = persistent? STAT_MEM_REALLOC_AMOUNT:STAT_MEM_EREALLOC_AMOUNT;
		*(size_t *) ret = new_size;
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(s1, 1, s2, new_size);
	}
	TRACE_ALLOC_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_efree */
static void _mysqlnd_efree(void *ptr MYSQLND_MEM_D)
{
	size_t free_amount = 0;
	bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	TRACE_ALLOC_ENTER(mysqlnd_efree_name);

#if PHP_DEBUG
	{
		char * fn = strrchr(__zend_filename, PHP_DIR_SEPARATOR);
		TRACE_ALLOC_INF_FMT("file=%-15s line=%4d", fn? fn + 1:__zend_filename, __zend_lineno);
	}
#endif
	TRACE_ALLOC_INF_FMT("ptr=%p", ptr);

	if (ptr) {
		if (collect_memory_statistics) {
			free_amount = *(size_t *)(((char*)ptr) - sizeof(size_t));
			TRACE_ALLOC_INF_FMT("ptr=%p size=%zu", ((char*)ptr) - sizeof(size_t), free_amount);
		}
		efree_rel(REAL_PTR(ptr));
	}

	if (collect_memory_statistics) {
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(STAT_MEM_EFREE_COUNT, 1, STAT_MEM_EFREE_AMOUNT, free_amount);
	}
	TRACE_ALLOC_VOID_RETURN;
}
/* }}} */


/* {{{ _mysqlnd_pefree */
static void _mysqlnd_pefree(void *ptr, bool persistent MYSQLND_MEM_D)
{
	size_t free_amount = 0;
	bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	TRACE_ALLOC_ENTER(mysqlnd_pefree_name);

#if PHP_DEBUG
	{
		char * fn = strrchr(__zend_filename, PHP_DIR_SEPARATOR);
		TRACE_ALLOC_INF_FMT("file=%-15s line=%4d", fn? fn + 1:__zend_filename, __zend_lineno);
	}
#endif
	TRACE_ALLOC_INF_FMT("ptr=%p persistent=%u", ptr, persistent);

	if (ptr) {
		if (collect_memory_statistics) {
			free_amount = *(size_t *)(((char*)ptr) - sizeof(size_t));
			TRACE_ALLOC_INF_FMT("ptr=%p size=%zu", ((char*)ptr) - sizeof(size_t), free_amount);
		}
		pefree_rel(REAL_PTR(ptr), persistent);
	}

	if (collect_memory_statistics) {
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(persistent? STAT_MEM_FREE_COUNT:STAT_MEM_EFREE_COUNT, 1,
											  persistent? STAT_MEM_FREE_AMOUNT:STAT_MEM_EFREE_AMOUNT, free_amount);
	}
	TRACE_ALLOC_VOID_RETURN;
}
/* }}} */


/* {{{ _mysqlnd_pememdup */
static char * _mysqlnd_pememdup(const char * const ptr, size_t length, bool persistent MYSQLND_MEM_D)
{
	char * ret;
	bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	TRACE_ALLOC_ENTER(mysqlnd_pememdup_name);

#if PHP_DEBUG
	{
		char * fn = strrchr(__zend_filename, PHP_DIR_SEPARATOR);
		TRACE_ALLOC_INF_FMT("file=%-15s line=%4d", fn? fn + 1:__zend_filename, __zend_lineno);
	}
#endif
	TRACE_ALLOC_INF_FMT("ptr=%p", ptr);

	ret = pemalloc_rel(REAL_SIZE(length + 1), persistent);
	{
		char * dest = (char *) FAKE_PTR(ret);
		memcpy(dest, ptr, length);
	}

	if (collect_memory_statistics) {
		*(size_t *) ret = length;
		MYSQLND_INC_GLOBAL_STATISTIC(persistent? STAT_MEM_DUP_COUNT : STAT_MEM_EDUP_COUNT);
	}

	TRACE_ALLOC_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_pestrndup */
static char * _mysqlnd_pestrndup(const char * const ptr, size_t length, bool persistent MYSQLND_MEM_D)
{
	char * ret;
	bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	TRACE_ALLOC_ENTER(mysqlnd_pestrndup_name);

#if PHP_DEBUG
	{
		char * fn = strrchr(__zend_filename, PHP_DIR_SEPARATOR);
		TRACE_ALLOC_INF_FMT("file=%-15s line=%4d", fn? fn + 1:__zend_filename, __zend_lineno);
	}
#endif
	TRACE_ALLOC_INF_FMT("ptr=%p", ptr);

	ret = pemalloc_rel(REAL_SIZE(length + 1), persistent);
	{
		size_t l = length;
		char * p = (char *) ptr;
		char * dest = (char *) FAKE_PTR(ret);
		while (*p && l--) {
			*dest++ = *p++;
		}
		*dest = '\0';
	}

	if (collect_memory_statistics) {
		*(size_t *) ret = length;
		MYSQLND_INC_GLOBAL_STATISTIC(persistent? STAT_MEM_STRNDUP_COUNT : STAT_MEM_ESTRNDUP_COUNT);
	}

	TRACE_ALLOC_RETURN(FAKE_PTR(ret));
}
/* }}} */


#define SMART_STR_START_SIZE 2048
#define SMART_STR_PREALLOC 512
#include "zend_smart_str.h"


/* {{{ _mysqlnd_pestrdup */
static char * _mysqlnd_pestrdup(const char * const ptr, bool persistent MYSQLND_MEM_D)
{
	char * ret;
	smart_str tmp_str = {0, 0};
	const char * p = ptr;
	bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	TRACE_ALLOC_ENTER(mysqlnd_pestrdup_name);
#if PHP_DEBUG
	{
		char * fn = strrchr(__zend_filename, PHP_DIR_SEPARATOR);
		TRACE_ALLOC_INF_FMT("file=%-15s line=%4d", fn? fn + 1:__zend_filename, __zend_lineno);
	}
#endif
	TRACE_ALLOC_INF_FMT("ptr=%p", ptr);
	do {
		smart_str_appendc(&tmp_str, *p);
	} while (*p++);

	ret = pemalloc_rel(REAL_SIZE(ZSTR_LEN(tmp_str.s)), persistent);
	memcpy(FAKE_PTR(ret), ZSTR_VAL(tmp_str.s), ZSTR_LEN(tmp_str.s));

	if (ret && collect_memory_statistics) {
		*(size_t *) ret = ZSTR_LEN(tmp_str.s);
		MYSQLND_INC_GLOBAL_STATISTIC(persistent? STAT_MEM_STRDUP_COUNT : STAT_MEM_ESTRDUP_COUNT);
	}
	smart_str_free(&tmp_str);

	TRACE_ALLOC_RETURN(FAKE_PTR(ret));
}
/* }}} */


#else

/* {{{ mysqlnd_zend_mm_emalloc */
static void * mysqlnd_zend_mm_emalloc(size_t size MYSQLND_MEM_D)
{
	return emalloc_rel(size);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_pemalloc */
static void * mysqlnd_zend_mm_pemalloc(size_t size, bool persistent MYSQLND_MEM_D)
{
	return pemalloc_rel(size, persistent);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_ecalloc */
static void * mysqlnd_zend_mm_ecalloc(unsigned int nmemb, size_t size MYSQLND_MEM_D)
{
	return ecalloc_rel(nmemb, size);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_pecalloc */
static void * mysqlnd_zend_mm_pecalloc(unsigned int nmemb, size_t size, bool persistent MYSQLND_MEM_D)
{
	return pecalloc_rel(nmemb, size, persistent);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_erealloc */
static void * mysqlnd_zend_mm_erealloc(void *ptr, size_t new_size MYSQLND_MEM_D)
{
	return erealloc_rel(ptr, new_size);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_perealloc */
static void * mysqlnd_zend_mm_perealloc(void *ptr, size_t new_size, bool persistent MYSQLND_MEM_D)
{
	return perealloc_rel(ptr, new_size, persistent);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_efree */
static void mysqlnd_zend_mm_efree(void * ptr MYSQLND_MEM_D)
{
	efree_rel(ptr);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_pefree */
static void mysqlnd_zend_mm_pefree(void * ptr, bool persistent MYSQLND_MEM_D)
{
	pefree_rel(ptr, persistent);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_pememdup */
static char * mysqlnd_zend_mm_pememdup(const char * const ptr, size_t length, bool persistent MYSQLND_MEM_D)
{
	char * dest = pemalloc_rel(length, persistent);
	if (dest) {
		memcpy(dest, ptr, length);
	}
	return dest;
}
/* }}} */


/* {{{ mysqlnd_zend_mm_pestrndup */
static char * mysqlnd_zend_mm_pestrndup(const char * const ptr, size_t length, bool persistent MYSQLND_MEM_D)
{
	return persistent? zend_strndup(ptr, length ) : estrndup_rel(ptr, length);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_pestrdup */
static char * mysqlnd_zend_mm_pestrdup(const char * const ptr, bool persistent MYSQLND_MEM_D)
{
	return pestrdup_rel(ptr, persistent);
}
/* }}} */

#endif /* MYSQLND_DEBUG_MEMORY */


PHPAPI struct st_mysqlnd_allocator_methods mysqlnd_allocator =
{
#if MYSQLND_DEBUG_MEMORY == 1
	_mysqlnd_emalloc,
	_mysqlnd_pemalloc,
	_mysqlnd_ecalloc,
	_mysqlnd_pecalloc,
	_mysqlnd_erealloc,
	_mysqlnd_perealloc,
	_mysqlnd_efree,
	_mysqlnd_pefree,
	_mysqlnd_pememdup,
	_mysqlnd_pestrndup,
	_mysqlnd_pestrdup
#else
	mysqlnd_zend_mm_emalloc,
	mysqlnd_zend_mm_pemalloc,
	mysqlnd_zend_mm_ecalloc,
	mysqlnd_zend_mm_pecalloc,
	mysqlnd_zend_mm_erealloc,
	mysqlnd_zend_mm_perealloc,
	mysqlnd_zend_mm_efree,
	mysqlnd_zend_mm_pefree,
	mysqlnd_zend_mm_pememdup,
	mysqlnd_zend_mm_pestrndup,
	mysqlnd_zend_mm_pestrdup
#endif
};
