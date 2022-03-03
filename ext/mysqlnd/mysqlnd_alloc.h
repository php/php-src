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

#ifndef MYSQLND_ALLOC_H
#define MYSQLND_ALLOC_H

PHPAPI extern const char * mysqlnd_debug_std_no_trace_funcs[];

#define MYSQLND_MEM_D	ZEND_FILE_LINE_DC
#define MYSQLND_MEM_C	ZEND_FILE_LINE_CC

struct st_mysqlnd_allocator_methods
{
	void *	(*m_emalloc)(size_t size MYSQLND_MEM_D);
	void *	(*m_pemalloc)(size_t size, bool persistent MYSQLND_MEM_D);
	void *	(*m_ecalloc)(unsigned int nmemb, size_t size MYSQLND_MEM_D);
	void *	(*m_pecalloc)(unsigned int nmemb, size_t size, bool persistent MYSQLND_MEM_D);
	void *	(*m_erealloc)(void *ptr, size_t new_size MYSQLND_MEM_D);
	void *	(*m_perealloc)(void *ptr, size_t new_size, bool persistent MYSQLND_MEM_D);
	void	(*m_efree)(void *ptr MYSQLND_MEM_D);
	void	(*m_pefree)(void *ptr, bool persistent MYSQLND_MEM_D);
	char *	(*m_pememdup)(const char * const ptr, size_t size, bool persistent MYSQLND_MEM_D);
	char *	(*m_pestrndup)(const char * const ptr, size_t size, bool persistent MYSQLND_MEM_D);
	char *	(*m_pestrdup)(const char * const ptr, bool persistent MYSQLND_MEM_D);
};

PHPAPI extern struct st_mysqlnd_allocator_methods mysqlnd_allocator;

#define mnd_emalloc(size)				mysqlnd_allocator.m_emalloc((size) MYSQLND_MEM_C)
#define mnd_pemalloc(size, pers)		mysqlnd_allocator.m_pemalloc((size), (pers) MYSQLND_MEM_C)
#define mnd_ecalloc(nmemb, size)		mysqlnd_allocator.m_ecalloc((nmemb), (size) MYSQLND_MEM_C)
#define mnd_pecalloc(nmemb, size, p)	mysqlnd_allocator.m_pecalloc((nmemb), (size), (p) MYSQLND_MEM_C)
#define mnd_erealloc(ptr, new_size)		mysqlnd_allocator.m_erealloc((ptr), (new_size) MYSQLND_MEM_C)
#define mnd_perealloc(ptr, new_size, p)	mysqlnd_allocator.m_perealloc((ptr), (new_size), (p) MYSQLND_MEM_C)
#define mnd_efree(ptr)					mysqlnd_allocator.m_efree((ptr) MYSQLND_MEM_C)
#define mnd_pefree(ptr, pers)			mysqlnd_allocator.m_pefree((ptr), (pers) MYSQLND_MEM_C)
#define mnd_pememdup(ptr, size, pers)	mysqlnd_allocator.m_pememdup((ptr), (size), (pers) MYSQLND_MEM_C)
#define mnd_pestrndup(ptr, size, pers)	mysqlnd_allocator.m_pestrndup((ptr), (size), (pers) MYSQLND_MEM_C)
#define mnd_pestrdup(ptr, pers)			mysqlnd_allocator.m_pestrdup((ptr), (pers) MYSQLND_MEM_C)
#define mnd_sprintf(p, mx_len, fmt,...) spprintf((p), (mx_len), (fmt), __VA_ARGS__)
#define mnd_vsprintf(p, mx_len, fmt,ap) vspprintf((p), (mx_len), (fmt), (ap))
#define mnd_sprintf_free(p)				efree((p))

static inline MYSQLND_STRING mnd_dup_cstring(const MYSQLND_CSTRING str, const bool persistent)
{
	const MYSQLND_STRING ret = {(char*) mnd_pemalloc(str.l + 1, persistent), str.l};
	if (ret.s) {
		memcpy(ret.s, str.s, str.l);
		ret.s[str.l] = '\0';
	}
	return ret;
}

static inline MYSQLND_CSTRING mnd_str2c(const MYSQLND_STRING str)
{
	const MYSQLND_CSTRING ret = {str.s, str.l};
	return ret;
}

static inline void mysqlnd_set_string(MYSQLND_STRING *buf, const char *string, size_t len) {
	if (buf->s) {
		mnd_efree(buf->s);
		buf->s = NULL;
		buf->l = 0;
	}
	if (string) {
		buf->s = mnd_pestrndup(string, len, 0);
		buf->l = len;
	}
}

static inline void mysqlnd_set_persistent_string(MYSQLND_STRING *buf, const char *string, size_t len, bool persistent) {
	if (buf->s) {
		mnd_pefree(buf->s, persistent);
		buf->s = NULL;
		buf->l = 0;
	}
	if (string) {
		buf->s = mnd_pestrndup(string, len, persistent);
		buf->l = len;
	}
}

#endif /* MYSQLND_ALLOC_H */
