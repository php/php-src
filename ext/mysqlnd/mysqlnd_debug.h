/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2013 The PHP Group                                |
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

struct st_mysqlnd_debug_methods
{
	enum_func_status (*open)(MYSQLND_DEBUG * self, zend_bool reopen);
	void			 (*set_mode)(MYSQLND_DEBUG * self, const char * const mode);
	enum_func_status (*log)(MYSQLND_DEBUG * self, unsigned int line, const char * const file,
							unsigned int level, const char * type, const char *message);
	enum_func_status (*log_va)(MYSQLND_DEBUG * self, unsigned int line, const char * const file,
							   unsigned int level, const char * type, const char *format, ...);
	zend_bool (*func_enter)(MYSQLND_DEBUG * self, unsigned int line, const char * const file,
							const char * const func_name, unsigned int func_name_len);
	enum_func_status (*func_leave)(MYSQLND_DEBUG * self, unsigned int line, const char * const file, uint64_t call_time);
	enum_func_status (*close)(MYSQLND_DEBUG * self);
	enum_func_status (*free_handle)(MYSQLND_DEBUG * self);
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
	zend_stack call_time_stack;	
	HashTable not_filtered_functions;
	HashTable function_profiles;
	struct st_mysqlnd_debug_methods *m;
	const char ** skip_functions;
};

PHPAPI extern const char * mysqlnd_debug_std_no_trace_funcs[];

PHPAPI MYSQLND_DEBUG * mysqlnd_debug_init(const char * skip_functions[] TSRMLS_DC);

PHPAPI char *	mysqlnd_get_backtrace(uint max_levels, size_t * length TSRMLS_DC);

#if defined(__GNUC__) || (defined(_MSC_VER) && (_MSC_VER >= 1400))
#ifdef PHP_WIN32
#include "win32/time.h"
#elif defined(NETWARE)
#include <sys/timeval.h>
#include <sys/time.h>
#else
#include <sys/time.h>
#endif

#ifndef MYSQLND_PROFILING_DISABLED
#define DBG_PROFILE_TIMEVAL_TO_DOUBLE(tp)	((tp.tv_sec * 1000000LL)+ tp.tv_usec)
#define DBG_PROFILE_START_TIME()		gettimeofday(&__dbg_prof_tp, NULL); __dbg_prof_start = DBG_PROFILE_TIMEVAL_TO_DOUBLE(__dbg_prof_tp);
#define DBG_PROFILE_END_TIME(duration)	gettimeofday(&__dbg_prof_tp, NULL); (duration) = (DBG_PROFILE_TIMEVAL_TO_DOUBLE(__dbg_prof_tp) - __dbg_prof_start);
#else
#define DBG_PROFILE_TIMEVAL_TO_DOUBLE(tp)
#define DBG_PROFILE_START_TIME()
#define DBG_PROFILE_END_TIME(duration)
#endif

#define DBG_INF_EX(dbg_obj, msg)		do { if (dbg_skip_trace == FALSE) (dbg_obj)->m->log((dbg_obj), __LINE__, __FILE__, -1, "info : ", (msg)); } while (0)
#define DBG_ERR_EX(dbg_obj, msg)		do { if (dbg_skip_trace == FALSE) (dbg_obj)->m->log((dbg_obj), __LINE__, __FILE__, -1, "error: ", (msg)); } while (0)
#define DBG_INF_FMT_EX(dbg_obj, ...)	do { if (dbg_skip_trace == FALSE) (dbg_obj)->m->log_va((dbg_obj), __LINE__, __FILE__, -1, "info : ", __VA_ARGS__); } while (0)
#define DBG_ERR_FMT_EX(dbg_obj, ...)	do { if (dbg_skip_trace == FALSE) (dbg_obj)->m->log_va((dbg_obj), __LINE__, __FILE__, -1, "error: ", __VA_ARGS__); } while (0)

#define DBG_BLOCK_ENTER_EX(dbg_obj, block_name) \
		{ \
			DBG_ENTER_EX(dbg_obj, (block_name));

#define DBG_BLOCK_LEAVE_EX(dbg_obj) \
			DBG_LEAVE_EX((dbg_obj), ;) \
		} \
	

#define DBG_ENTER_EX(dbg_obj, func_name) \
					struct timeval __dbg_prof_tp = {0}; \
					uint64_t __dbg_prof_start = 0; /* initialization is needed */ \
					zend_bool dbg_skip_trace = TRUE; \
					if ((dbg_obj)) { \
						dbg_skip_trace = !(dbg_obj)->m->func_enter((dbg_obj), __LINE__, __FILE__, func_name, strlen(func_name)); \
					} \
					do { \
						if ((dbg_obj) && (dbg_obj)->flags & MYSQLND_DEBUG_PROFILE_CALLS) { \
							DBG_PROFILE_START_TIME(); \
						} \
					} while (0); 

#define DBG_LEAVE_EX(dbg_obj, leave)	\
			do {\
				if ((dbg_obj)) { \
					uint64_t this_call_duration = 0; \
					if ((dbg_obj)->flags & MYSQLND_DEBUG_PROFILE_CALLS) { \
						DBG_PROFILE_END_TIME(this_call_duration); \
					} \
					(dbg_obj)->m->func_leave((dbg_obj), __LINE__, __FILE__, this_call_duration); \
				} \
				leave \
			} while (0);

#define DBG_RETURN_EX(dbg_obj, value) DBG_LEAVE_EX(dbg_obj, return (value);)

#define DBG_VOID_RETURN_EX(dbg_obj) DBG_LEAVE_EX(dbg_obj, return;)



#else
static inline void DBG_INF_EX(MYSQLND_DEBUG * dbg_obj, const char * const msg) {}
static inline void DBG_ERR_EX(MYSQLND_DEBUG * dbg_obj, const char * const msg) {}
static inline void DBG_INF_FMT_EX(MYSQLND_DEBUG * dbg_obj, ...) {}
static inline void DBG_ERR_FMT_EX(MYSQLND_DEBUG * dbg_obj, ...) {}
static inline void DBG_ENTER_EX(MYSQLND_DEBUG * dbg_obj, const char * const func_name) {}
#define DBG_BLOCK_ENTER(bname)			{
#define DBG_RETURN_EX(dbg_obj, value)	return (value)
#define DBG_VOID_RETURN_EX(dbg_obj)		return
#define DBG_BLOCK_LEAVE_EX(dbg_obj)		}

#endif /* defined(__GNUC__) || (defined(_MSC_VER) && (_MSC_VER >= 1400)) */

#if MYSQLND_DBG_ENABLED == 1

#define DBG_INF(msg)		DBG_INF_EX(MYSQLND_G(dbg), (msg))
#define DBG_ERR(msg)		DBG_ERR_EX(MYSQLND_G(dbg), (msg))
#define DBG_INF_FMT(...)	DBG_INF_FMT_EX(MYSQLND_G(dbg), __VA_ARGS__)
#define DBG_ERR_FMT(...)	DBG_ERR_FMT_EX(MYSQLND_G(dbg), __VA_ARGS__)

#define DBG_ENTER(func_name)	DBG_ENTER_EX(MYSQLND_G(dbg), (func_name))
#define DBG_BLOCK_ENTER(bname)	DBG_BLOCK_ENTER_EX(MYSQLND_G(dbg), (bname))
#define DBG_RETURN(value)		DBG_RETURN_EX(MYSQLND_G(dbg), (value))
#define DBG_VOID_RETURN			DBG_VOID_RETURN_EX(MYSQLND_G(dbg))
#define DBG_BLOCK_LEAVE			DBG_BLOCK_LEAVE_EX(MYSQLND_G(dbg))

#elif MYSQLND_DBG_ENABLED == 0



static inline void DBG_INF(const char * const msg) {}
static inline void DBG_ERR(const char * const msg) {}
static inline void DBG_INF_FMT(const char * const format, ...) {}
static inline void DBG_ERR_FMT(const char * const format, ...) {}
static inline void DBG_ENTER(const char * const func_name) {}
#define DBG_BLOCK_ENTER(bname)	{
#define DBG_RETURN(value)		return (value)
#define DBG_VOID_RETURN			return
#define DBG_BLOCK_LEAVE			}

#endif


#define MYSQLND_MEM_D	TSRMLS_DC ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC
#define MYSQLND_MEM_C	TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC

struct st_mysqlnd_allocator_methods
{
	void *	(*m_emalloc)(size_t size MYSQLND_MEM_D);
	void *	(*m_pemalloc)(size_t size, zend_bool persistent MYSQLND_MEM_D);
	void *	(*m_ecalloc)(unsigned int nmemb, size_t size MYSQLND_MEM_D);
	void *	(*m_pecalloc)(unsigned int nmemb, size_t size, zend_bool persistent MYSQLND_MEM_D);
	void *	(*m_erealloc)(void *ptr, size_t new_size MYSQLND_MEM_D);
	void *	(*m_perealloc)(void *ptr, size_t new_size, zend_bool persistent MYSQLND_MEM_D);
	void	(*m_efree)(void *ptr MYSQLND_MEM_D);
	void	(*m_pefree)(void *ptr, zend_bool persistent MYSQLND_MEM_D);
	void *	(*m_malloc)(size_t size MYSQLND_MEM_D);
	void *	(*m_calloc)(unsigned int nmemb, size_t size MYSQLND_MEM_D);
	void *	(*m_realloc)(void *ptr, size_t new_size MYSQLND_MEM_D);
	void	(*m_free)(void *ptr MYSQLND_MEM_D);
	char *	(*m_pestrndup)(const char * const ptr, size_t size, zend_bool persistent MYSQLND_MEM_D);
	char *	(*m_pestrdup)(const char * const ptr, zend_bool persistent MYSQLND_MEM_D);
};

PHPAPI extern struct st_mysqlnd_allocator_methods mysqlnd_allocator;


PHPAPI void *	_mysqlnd_emalloc(size_t size MYSQLND_MEM_D);
PHPAPI void *	_mysqlnd_pemalloc(size_t size, zend_bool persistent MYSQLND_MEM_D);
PHPAPI void *	_mysqlnd_ecalloc(unsigned int nmemb, size_t size MYSQLND_MEM_D);
PHPAPI void *	_mysqlnd_pecalloc(unsigned int nmemb, size_t size, zend_bool persistent MYSQLND_MEM_D);
PHPAPI void *	_mysqlnd_erealloc(void *ptr, size_t new_size MYSQLND_MEM_D);
PHPAPI void *	_mysqlnd_perealloc(void *ptr, size_t new_size, zend_bool persistent MYSQLND_MEM_D);
PHPAPI void		_mysqlnd_efree(void *ptr MYSQLND_MEM_D);
PHPAPI void		_mysqlnd_pefree(void *ptr, zend_bool persistent MYSQLND_MEM_D);
PHPAPI void *	_mysqlnd_malloc(size_t size MYSQLND_MEM_D);
PHPAPI void *	_mysqlnd_calloc(unsigned int nmemb, size_t size MYSQLND_MEM_D);
PHPAPI void *	_mysqlnd_realloc(void *ptr, size_t new_size MYSQLND_MEM_D);
PHPAPI void		_mysqlnd_free(void *ptr MYSQLND_MEM_D);
PHPAPI char *	_mysqlnd_pestrndup(const char * const ptr, size_t size, zend_bool persistent MYSQLND_MEM_D);
PHPAPI char *	_mysqlnd_pestrdup(const char * const ptr, zend_bool persistent MYSQLND_MEM_D);


#define mnd_emalloc(size)				mysqlnd_allocator.m_emalloc((size) MYSQLND_MEM_C)
#define mnd_pemalloc(size, pers)		mysqlnd_allocator.m_pemalloc((size), (pers) MYSQLND_MEM_C)
#define mnd_ecalloc(nmemb, size)		mysqlnd_allocator.m_ecalloc((nmemb), (size) MYSQLND_MEM_C)
#define mnd_pecalloc(nmemb, size, p)	mysqlnd_allocator.m_pecalloc((nmemb), (size), (p) MYSQLND_MEM_C)
#define mnd_erealloc(ptr, new_size)		mysqlnd_allocator.m_erealloc((ptr), (new_size) MYSQLND_MEM_C)
#define mnd_perealloc(ptr, new_size, p)	mysqlnd_allocator.m_perealloc((ptr), (new_size), (p) MYSQLND_MEM_C)
#define mnd_efree(ptr)					mysqlnd_allocator.m_efree((ptr) MYSQLND_MEM_C)
#define mnd_pefree(ptr, pers)			mysqlnd_allocator.m_pefree((ptr), (pers) MYSQLND_MEM_C)
#define mnd_malloc(size)				mysqlnd_allocator.m_malloc((size) MYSQLND_MEM_C)
#define mnd_calloc(nmemb, size)			mysqlnd_allocator.m_calloc((nmemb), (size) MYSQLND_MEM_C)
#define mnd_realloc(ptr, new_size)		mysqlnd_allocator.m_realloc((ptr), (new_size) MYSQLND_MEM_C)
#define mnd_free(ptr)					mysqlnd_allocator.m_free((ptr) MYSQLND_MEM_C)
#define mnd_pestrndup(ptr, size, pers)	mysqlnd_allocator.m_pestrndup((ptr), (size), (pers) MYSQLND_MEM_C)
#define mnd_pestrdup(ptr, pers)			mysqlnd_allocator.m_pestrdup((ptr), (pers) MYSQLND_MEM_C)

#endif /* MYSQLND_DEBUG_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
