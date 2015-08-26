/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2015 The PHP Group                                |
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

#include "mysqlnd_alloc.h"
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

struct st_mysqlnd_plugin_trace_log
{
	struct st_mysqlnd_plugin_header plugin_header;
	struct
	{
		MYSQLND_DEBUG * (*trace_instance_init)(const char * skip_functions[]);
	} methods;
};

void mysqlnd_debug_trace_plugin_register(void);

PHPAPI MYSQLND_DEBUG * mysqlnd_debug_init(const char * skip_functions[]);


#if defined(__GNUC__) || defined(PHP_WIN32)
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

#define DBG_INF_EX(dbg_obj, msg)		do { if (dbg_skip_trace == FALSE && (dbg_obj)) (dbg_obj)->m->log((dbg_obj), __LINE__, __FILE__, -1, "info : ", (msg)); } while (0)
#define DBG_ERR_EX(dbg_obj, msg)		do { if (dbg_skip_trace == FALSE && (dbg_obj)) (dbg_obj)->m->log((dbg_obj), __LINE__, __FILE__, -1, "error: ", (msg)); } while (0)
#define DBG_INF_FMT_EX(dbg_obj, ...)	do { if (dbg_skip_trace == FALSE && (dbg_obj)) (dbg_obj)->m->log_va((dbg_obj), __LINE__, __FILE__, -1, "info : ", __VA_ARGS__); } while (0)
#define DBG_ERR_FMT_EX(dbg_obj, ...)	do { if (dbg_skip_trace == FALSE && (dbg_obj)) (dbg_obj)->m->log_va((dbg_obj), __LINE__, __FILE__, -1, "error: ", __VA_ARGS__); } while (0)

#define DBG_BLOCK_ENTER_EX(dbg_obj, block_name) DBG_BLOCK_ENTER_EX2((dbg_obj), (MYSQLND_DEBUG *) NULL, (block_name))
#define DBG_BLOCK_LEAVE_EX(dbg_obj)				DBG_BLOCK_LEAVE_EX2((dbg_obj), (MYSQLND_DEBUG *) NULL)

#define DBG_BLOCK_ENTER_EX2(dbg_obj1, dbg_obj2, block_name) \
		{ \
			DBG_ENTER_EX2((dbg_obj1), (dbg_obj2), (block_name));

#define DBG_BLOCK_LEAVE_EX2(dbg_obj1, dbg_obj2) \
			DBG_LEAVE_EX2((dbg_obj1), (dbg_obj2), ;) \
		} \


#define DBG_ENTER_EX(dbg_obj, func_name)	DBG_ENTER_EX2((dbg_obj), (MYSQLND_DEBUG *) NULL, (func_name))
#define DBG_LEAVE_EX(dbg_obj, leave)		DBG_LEAVE_EX2((dbg_obj), (MYSQLND_DEBUG *) NULL, leave)

#define DBG_ENTER_EX2(dbg_obj1, dbg_obj2, func_name) \
					struct timeval __dbg_prof_tp = {0}; \
					uint64_t __dbg_prof_start = 0; /* initialization is needed */ \
					zend_bool dbg_skip_trace = TRUE; \
					((void)__dbg_prof_start); \
					if ((dbg_obj1)) { \
						dbg_skip_trace = !(dbg_obj1)->m->func_enter((dbg_obj1), __LINE__, __FILE__, func_name, strlen(func_name)); \
					} \
					if ((dbg_obj2)) { \
						dbg_skip_trace |= !(dbg_obj2)->m->func_enter((dbg_obj2), __LINE__, __FILE__, func_name, strlen(func_name)); \
					} \
					if (dbg_skip_trace) \
						/* EMPTY */ ; /* shut compiler's mouth */	\
					do { \
						if (((dbg_obj1) && (dbg_obj1)->flags & MYSQLND_DEBUG_PROFILE_CALLS) || \
							((dbg_obj2) && (dbg_obj2)->flags & MYSQLND_DEBUG_PROFILE_CALLS)) \
						{ \
							DBG_PROFILE_START_TIME(); \
						} \
					} while (0);

#define DBG_LEAVE_EX2(dbg_obj1, dbg_obj2, leave)	\
			do {\
				uint64_t this_call_duration = 0; \
				if (((dbg_obj1) && (dbg_obj1)->flags & MYSQLND_DEBUG_PROFILE_CALLS) || \
					((dbg_obj2) && (dbg_obj2)->flags & MYSQLND_DEBUG_PROFILE_CALLS)) \
				{ \
					DBG_PROFILE_END_TIME(this_call_duration); \
				} \
				if ((dbg_obj1)) { \
					(dbg_obj1)->m->func_leave((dbg_obj1), __LINE__, __FILE__, this_call_duration); \
				} \
				if ((dbg_obj2)) { \
					(dbg_obj2)->m->func_leave((dbg_obj2), __LINE__, __FILE__, this_call_duration); \
				} \
				leave \
			} while (0);


#define DBG_RETURN_EX(dbg_obj, value)		DBG_LEAVE_EX((dbg_obj), return (value);)
#define DBG_VOID_RETURN_EX(dbg_obj)			DBG_LEAVE_EX((dbg_obj), return;)

#define DBG_RETURN_EX2(dbg_obj1, dbg_obj2, value)	DBG_LEAVE_EX2((dbg_obj1), (dbg_obj2), return (value);)
#define DBG_VOID_RETURN_EX2(dbg_obj1, dbg_obj2)		DBG_LEAVE_EX2((dbg_obj1), (dbg_obj2), return;)



#else  /* defined(__GNUC__) || defined(PHP_WIN32) */
static inline void DBG_INF_EX(MYSQLND_DEBUG * dbg_obj, const char * const msg) {}
static inline void DBG_ERR_EX(MYSQLND_DEBUG * dbg_obj, const char * const msg) {}
static inline void DBG_INF_FMT_EX(MYSQLND_DEBUG * dbg_obj, ...) {}
static inline void DBG_ERR_FMT_EX(MYSQLND_DEBUG * dbg_obj, ...) {}
static inline void DBG_ENTER_EX(MYSQLND_DEBUG * dbg_obj, const char * const func_name) {}
#define DBG_BLOCK_ENTER(bname)			{
#define DBG_RETURN_EX(dbg_obj, value)	return (value)
#define DBG_VOID_RETURN_EX(dbg_obj)		return
#define DBG_BLOCK_LEAVE_EX(dbg_obj)		}

#endif

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


#define TRACE_ALLOC_INF(msg)			DBG_INF_EX(MYSQLND_G(trace_alloc), (msg))
#define TRACE_ALLOC_ERR(msg)			DBG_ERR_EX(MYSQLND_G(trace_alloc), (msg))
#define TRACE_ALLOC_INF_FMT(...)		DBG_INF_FMT_EX(MYSQLND_G(trace_alloc), __VA_ARGS__)
#define TRACE_ALLOC_ERR_FMT(...)		DBG_ERR_FMT_EX(MYSQLND_G(trace_alloc), __VA_ARGS__)

#define TRACE_ALLOC_ENTER(func_name)	DBG_ENTER_EX2(MYSQLND_G(dbg), MYSQLND_G(trace_alloc), (func_name))
#define TRACE_ALLOC_BLOCK_ENTER(bname)	DBG_BLOCK_ENTER_EX2(MYSQLND_G(dbg), MYSQLND_G(trace_alloc), (bname))
#define TRACE_ALLOC_RETURN(value)		DBG_RETURN_EX2(MYSQLND_G(dbg), MYSQLND_G(trace_alloc), (value))
#define TRACE_ALLOC_VOID_RETURN			DBG_VOID_RETURN_EX2(MYSQLND_G(dbg), MYSQLND_G(trace_alloc))
#define TRACE_ALLOC_BLOCK_LEAVE			DBG_BLOCK_LEAVE_EX2(MYSQLND_G(dbg), MYSQLND_G(trace_alloc))

#elif MYSQLND_DBG_ENABLED == 0

static inline void DBG_INF(const char * const msg) {}
static inline void DBG_ERR(const char * const msg) {}
static inline void DBG_INF_FMT(const char * const format, ...) {}
static inline void DBG_ERR_FMT(const char * const format, ...) {}
static inline void DBG_ENTER(const char * const func_name) {}
#define DBG_BLOCK_ENTER(bname)	{
#define DBG_RETURN(value)			return (value)
#define DBG_VOID_RETURN				return
#define DBG_BLOCK_LEAVE			}


static inline void TRACE_ALLOC_INF(const char * const msg) {}
static inline void TRACE_ALLOC_ERR(const char * const msg) {}
static inline void TRACE_ALLOC_INF_FMT(const char * const format, ...) {}
static inline void TRACE_ALLOC_ERR_FMT(const char * const format, ...) {}
static inline void TRACE_ALLOC_ENTER(const char * const func_name) {}
#define TRACE_ALLOC_BLOCK_ENTER(bname)	{
#define TRACE_ALLOC_RETURN(value)			return (value)
#define TRACE_ALLOC_VOID_RETURN				return
#define TRACE_ALLOC_BLOCK_LEAVE			}

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
