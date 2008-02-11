/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2008 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Georg Richter <georg@mysql.com>                             |
  |          Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef MYSQLND_DEBUG_H
#define MYSQLND_DEBUG_H

#include "zend_stack.h"

#define MYSQLND_DEBUG_MEMORY 1

struct st_mysqlnd_debug_methods
{
	enum_func_status (*open)(MYSQLND_DEBUG *self, zend_bool reopen);
	void			 (*set_mode)(MYSQLND_DEBUG *self, const char * const mode);
	enum_func_status (*log)(MYSQLND_DEBUG *self, unsigned int line, const char * const file,
							unsigned int level, const char * type, const char *message);
	enum_func_status (*log_va)(MYSQLND_DEBUG *self, unsigned int line, const char * const file,
							   unsigned int level, const char * type, const char *format, ...);
	zend_bool (*func_enter)(MYSQLND_DEBUG *self, unsigned int line, const char * const file,
							char * func_name, uint func_name_len);
	enum_func_status (*func_leave)(MYSQLND_DEBUG *self, unsigned int line, const char * const file);
	enum_func_status (*close)(MYSQLND_DEBUG *self);
	enum_func_status (*free_handle)(MYSQLND_DEBUG *self);
};

struct st_mysqlnd_debug
{
	php_stream	*stream;
#ifdef ZTS
	TSRMLS_D;
#endif
	unsigned int flags;
	unsigned int nest_level_limit;
	int pid;
	char * file_name;
	zend_stack call_stack;
	HashTable not_filtered_functions;
	struct st_mysqlnd_debug_methods *m;
};


MYSQLND_DEBUG *mysqlnd_debug_init(TSRMLS_D);

#define MYSQLND_MEM_D 	TSRMLS_DC ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC


void *	_mysqlnd_emalloc(size_t size MYSQLND_MEM_D);
void *	_mysqlnd_pemalloc(size_t size, zend_bool persistent MYSQLND_MEM_D);
void *	_mysqlnd_ecalloc(uint nmemb, size_t size MYSQLND_MEM_D);
void *	_mysqlnd_pecalloc(uint nmemb, size_t size, zend_bool persistent MYSQLND_MEM_D);
void *	_mysqlnd_erealloc(void *ptr, size_t new_size MYSQLND_MEM_D);
void *	_mysqlnd_perealloc(void *ptr, size_t new_size, zend_bool persistent MYSQLND_MEM_D);
void 	_mysqlnd_efree(void *ptr MYSQLND_MEM_D);
void 	_mysqlnd_pefree(void *ptr, zend_bool persistent MYSQLND_MEM_D);
void *	_mysqlnd_malloc(size_t size MYSQLND_MEM_D);
void *	_mysqlnd_calloc(uint nmemb, size_t size MYSQLND_MEM_D);
void *	_mysqlnd_realloc(void *ptr, size_t new_size MYSQLND_MEM_D);
void	_mysqlnd_free(void *ptr MYSQLND_MEM_D);

char *	mysqlnd_get_backtrace(TSRMLS_D);

#if MYSQLND_DBG_ENABLED == 1

#define DBG_INF(msg) do { if (dbg_skip_trace == FALSE) MYSQLND_G(dbg)->m->log(MYSQLND_G(dbg), __LINE__, __FILE__, -1, "info : ", (msg)); } while (0)
#define DBG_ERR(msg) do { if (dbg_skip_trace == FALSE) MYSQLND_G(dbg)->m->log(MYSQLND_G(dbg), __LINE__, __FILE__, -1, "error: ", (msg)); } while (0)
#define DBG_INF_FMT(...) do { if (dbg_skip_trace == FALSE) MYSQLND_G(dbg)->m->log_va(MYSQLND_G(dbg), __LINE__, __FILE__, -1, "info : ", __VA_ARGS__); } while (0)
#define DBG_ERR_FMT(...) do { if (dbg_skip_trace == FALSE) MYSQLND_G(dbg)->m->log_va(MYSQLND_G(dbg), __LINE__, __FILE__, -1, "error: ", __VA_ARGS__); } while (0)

#define DBG_ENTER(func_name) zend_bool dbg_skip_trace = TRUE; if (MYSQLND_G(dbg)) dbg_skip_trace = !MYSQLND_G(dbg)->m->func_enter(MYSQLND_G(dbg), __LINE__, __FILE__, func_name, strlen(func_name));
#define DBG_RETURN(value)	do { if (MYSQLND_G(dbg)) MYSQLND_G(dbg)->m->func_leave(MYSQLND_G(dbg), __LINE__, __FILE__); return (value); } while (0)
#define DBG_VOID_RETURN		do { if (MYSQLND_G(dbg)) MYSQLND_G(dbg)->m->func_leave(MYSQLND_G(dbg), __LINE__, __FILE__); return; } while (0)



#elif MYSQLND_DBG_ENABLED == 0


static inline void DBG_INF(char *msg) {}
static inline void DBG_ERR(char *msg) {}
static inline void DBG_INF_FMT(char *format, ...) {}
static inline void DBG_ERR_FMT(char *format, ...) {}
static inline void DBG_ENTER(char *func_name) {}
#define DBG_RETURN(value)	return (value)
#define DBG_VOID_RETURN		return;
#endif


#if MYSQLND_DEBUG_MEMORY

#define mnd_emalloc(size)				_mysqlnd_emalloc((size) TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define mnd_pemalloc(size, pers)		_mysqlnd_pemalloc((size), (pers) TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define mnd_ecalloc(nmemb, size)		_mysqlnd_ecalloc((nmemb), (size) TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define mnd_pecalloc(nmemb, size, p)	_mysqlnd_pecalloc((nmemb), (size), (p) TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define mnd_erealloc(ptr, new_size)		_mysqlnd_erealloc((ptr), (new_size) TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define mnd_perealloc(ptr, new_size, p)	_mysqlnd_perealloc((ptr), (new_size), (p) TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define mnd_efree(ptr)					_mysqlnd_efree((ptr) TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define mnd_pefree(ptr, pers)			_mysqlnd_pefree((ptr), (pers) TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define mnd_malloc(size)				_mysqlnd_malloc((size) TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define mnd_calloc(nmemb, size)			_mysqlnd_calloc((nmemb), (size) TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define mnd_realloc(ptr, new_size)		_mysqlnd_realloc((ptr), (new_size) TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)
#define mnd_free(ptr)					_mysqlnd_free((ptr) TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC)

#else

#define mnd_emalloc(size)				emalloc((size))
#define mnd_pemalloc(size, pers)		pemalloc((size), (pers))
#define mnd_ecalloc(nmemb, size)		ecalloc((nmemb), (size))
#define mnd_pecalloc(nmemb, size, p)	pecalloc((nmemb), (size), (p))
#define mnd_erealloc(ptr, new_size)		erealloc((ptr), (new_size))
#define mnd_perealloc(ptr, new_size, p)	perealloc((ptr), (new_size), (p))
#define mnd_efree(ptr)					efree((ptr))
#define mnd_pefree(ptr, pers)			pefree((ptr), (pers))
#define mnd_malloc(size)				malloc((size))
#define mnd_calloc(nmemb, size)			calloc((nmemb), (size))
#define mnd_realloc(ptr, new_size)		realloc((ptr), (new_size))
#define mnd_free(ptr)					free((ptr))

#endif

#endif /* MYSQLND_DEBUG_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
