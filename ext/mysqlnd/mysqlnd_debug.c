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

#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_palloc.h"
#include "mysqlnd_statistics.h"
#include "zend_builtin_functions.h"


static const char * const mysqlnd_debug_default_trace_file = "/tmp/mysqlnd.trace";

#ifdef ZTS 
#define MYSQLND_ZTS(self) TSRMLS_D = (self)->TSRMLS_C
#else
#define MYSQLND_ZTS(self)
#endif

#define MYSQLND_DEBUG_DUMP_TIME				1
#define MYSQLND_DEBUG_DUMP_TRACE			2
#define MYSQLND_DEBUG_DUMP_PID				4
#define MYSQLND_DEBUG_DUMP_LINE				8
#define MYSQLND_DEBUG_DUMP_FILE				16
#define MYSQLND_DEBUG_DUMP_LEVEL			32
#define MYSQLND_DEBUG_APPEND				64
#define MYSQLND_DEBUG_FLUSH					128
#define MYSQLND_DEBUG_TRACE_MEMORY_CALLS	256

static char * mysqlnd_emalloc_name	= "_mysqlnd_emalloc";
static char * mysqlnd_pemalloc_name	= "_mysqlnd_pemalloc";
static char * mysqlnd_ecalloc_name	= "_mysqlnd_ecalloc";
static char * mysqlnd_pecalloc_name	= "_mysqlnd_pecalloc";
static char * mysqlnd_erealloc_name	= "_mysqlnd_erealloc";
static char * mysqlnd_perealloc_name= "_mysqlnd_perealloc";
static char * mysqlnd_efree_name	= "_mysqlnd_efree";
static char * mysqlnd_pefree_name	= "_mysqlnd_pefree";
static char * mysqlnd_malloc_name	= "_mysqlnd_malloc";
static char * mysqlnd_calloc_name	= "_mysqlnd_calloc";
static char * mysqlnd_realloc_name	= "_mysqlnd_realloc";
static char * mysqlnd_free_name		= "_mysqlnd_free";

/* {{{ mysqlnd_debug::open */
static enum_func_status
MYSQLND_METHOD(mysqlnd_debug, open)(MYSQLND_DEBUG * self, zend_bool reopen)
{
	MYSQLND_ZTS(self);

	if (!self->file_name) {
		return FAIL;
	}

	self->stream = php_stream_open_wrapper(self->file_name,
										   reopen == TRUE || self->flags & MYSQLND_DEBUG_APPEND? "ab":"wb",
										   REPORT_ERRORS, NULL);
	return self->stream? PASS:FAIL;
}
/* }}} */


/* {{{ mysqlnd_debug::log */
static enum_func_status
MYSQLND_METHOD(mysqlnd_debug, log)(MYSQLND_DEBUG * self,
								   unsigned int line, const char * const file,
								   unsigned int level, const char * type, const char * message)
{
	char pipe_buffer[512];
	enum_func_status ret;
	int i;
	char * message_line;
	uint message_line_len;
	unsigned int flags = self->flags;
	char pid_buffer[10], time_buffer[30], file_buffer[200],
		 line_buffer[6], level_buffer[7];
	MYSQLND_ZTS(self);

#ifdef ZTS
	if (MYSQLND_G(thread_id) != tsrm_thread_id()) {
		return PASS; /* don't trace background threads */
	}
#endif

	if (!self->stream) {
		if (FAIL == self->m->open(self, FALSE)) {
			return FAIL;
		}
	}

	if (level == -1) {
		level = zend_stack_count(&self->call_stack);
	}
	i = MIN(level, sizeof(pipe_buffer) / 2  - 1);
	pipe_buffer[i*2] = '\0';
	for (;i > 0;i--) {
		pipe_buffer[i*2 - 1] = ' ';
		pipe_buffer[i*2 - 2] = '|';
	}


	if (flags & MYSQLND_DEBUG_DUMP_PID) {
		snprintf(pid_buffer, sizeof(pid_buffer) - 1, "%5u: ", self->pid);
		pid_buffer[sizeof(pid_buffer) - 1 ] = '\0';
	}
	if (flags & MYSQLND_DEBUG_DUMP_TIME) {
		/* The following from FF's DBUG library, which is in the public domain */
#if defined(PHP_WIN32)
		/* FIXME This doesn't give microseconds as in Unix case, and the resolution is
		in system ticks, 10 ms intervals. See my_getsystime.c for high res */
		SYSTEMTIME loc_t;
		GetLocalTime(&loc_t);
		snprintf(time_buffer, sizeof(time_buffer) - 1,
				 /* "%04d-%02d-%02d " */
				 "%02d:%02d:%02d.%06d ",
				 /*tm_p->tm_year + 1900, tm_p->tm_mon + 1, tm_p->tm_mday,*/
				 loc_t.wHour, loc_t.wMinute, loc_t.wSecond, loc_t.wMilliseconds);
		time_buffer[sizeof(time_buffer) - 1 ] = '\0';
#else
		struct timeval tv;
		struct tm *tm_p;
		if (gettimeofday(&tv, NULL) != -1) {
			if ((tm_p= localtime((const time_t *)&tv.tv_sec))) {
				snprintf(time_buffer, sizeof(time_buffer) - 1,
						 /* "%04d-%02d-%02d " */
						 "%02d:%02d:%02d.%06d ",
						 /*tm_p->tm_year + 1900, tm_p->tm_mon + 1, tm_p->tm_mday,*/
						 tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec,
						 (int) (tv.tv_usec));
				time_buffer[sizeof(time_buffer) - 1 ] = '\0';
			}
		}
#endif
	}
	if (flags & MYSQLND_DEBUG_DUMP_FILE) {
		snprintf(file_buffer, sizeof(file_buffer) - 1, "%14s: ", file);
		file_buffer[sizeof(file_buffer) - 1 ] = '\0';
	}
	if (flags & MYSQLND_DEBUG_DUMP_LINE) {
		snprintf(line_buffer, sizeof(line_buffer) - 1, "%5u: ", line);
		line_buffer[sizeof(line_buffer) - 1 ] = '\0';
	}
	if (flags & MYSQLND_DEBUG_DUMP_LEVEL) {
		snprintf(level_buffer, sizeof(level_buffer) - 1, "%4u: ", level);
		level_buffer[sizeof(level_buffer) - 1 ] = '\0';
	}

	message_line_len = spprintf(&message_line, 0, "%s%s%s%s%s%s%s%s\n",
								flags & MYSQLND_DEBUG_DUMP_PID? pid_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_TIME? time_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_FILE? file_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_LINE? line_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_LEVEL? level_buffer:"",
								pipe_buffer, type? type:"", message);

	ret = php_stream_write(self->stream, message_line, message_line_len)? PASS:FAIL;
	efree(message_line);
	if (flags & MYSQLND_DEBUG_FLUSH) {
		self->m->close(self);
		self->m->open(self, TRUE);	
	}
	return ret;
}
/* }}} */


/* {{{ mysqlnd_debug::log_va */
static enum_func_status
MYSQLND_METHOD(mysqlnd_debug, log_va)(MYSQLND_DEBUG *self,
									  unsigned int line, const char * const file,
									  unsigned int level, const char * type,
									  const char *format, ...)
{
	char pipe_buffer[512];
	int i;
	enum_func_status ret;
	char * message_line, *buffer;
	uint message_line_len;
	va_list args;
	unsigned int flags = self->flags;
	char pid_buffer[10], time_buffer[30], file_buffer[200],
		 line_buffer[6], level_buffer[7];
	MYSQLND_ZTS(self);

#ifdef ZTS
	if (MYSQLND_G(thread_id) != tsrm_thread_id()) {
		return PASS; /* don't trace background threads */
	}
#endif

	if (!self->stream) {
		if (FAIL == self->m->open(self, FALSE)) {
			return FAIL;
		}
	}

	if (level == -1) {
		level = zend_stack_count(&self->call_stack);
	}
	i = MIN(level, sizeof(pipe_buffer) / 2  - 1);
	pipe_buffer[i*2] = '\0';
	for (;i > 0;i--) {
		pipe_buffer[i*2 - 1] = ' ';
		pipe_buffer[i*2 - 2] = '|';
	}


	if (flags & MYSQLND_DEBUG_DUMP_PID) {
		snprintf(pid_buffer, sizeof(pid_buffer) - 1, "%5u: ", self->pid);
		pid_buffer[sizeof(pid_buffer) - 1 ] = '\0';
	}
	if (flags & MYSQLND_DEBUG_DUMP_TIME) {
		/* The following from FF's DBUG library, which is in the public domain */
#if defined(PHP_WIN32)
		/* FIXME This doesn't give microseconds as in Unix case, and the resolution is
		in system ticks, 10 ms intervals. See my_getsystime.c for high res */
		SYSTEMTIME loc_t;
		GetLocalTime(&loc_t);
		snprintf(time_buffer, sizeof(time_buffer) - 1,
				 /* "%04d-%02d-%02d " */
				 "%02d:%02d:%02d.%06d ",
				 /*tm_p->tm_year + 1900, tm_p->tm_mon + 1, tm_p->tm_mday,*/
				 loc_t.wHour, loc_t.wMinute, loc_t.wSecond, loc_t.wMilliseconds);
		time_buffer[sizeof(time_buffer) - 1 ] = '\0';
#else
		struct timeval tv;
		struct tm *tm_p;
		if (gettimeofday(&tv, NULL) != -1) {
			if ((tm_p= localtime((const time_t *)&tv.tv_sec))) {
				snprintf(time_buffer, sizeof(time_buffer) - 1,
						 /* "%04d-%02d-%02d " */
						 "%02d:%02d:%02d.%06d ",
						 /*tm_p->tm_year + 1900, tm_p->tm_mon + 1, tm_p->tm_mday,*/
						 tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec,
						 (int) (tv.tv_usec));
				time_buffer[sizeof(time_buffer) - 1 ] = '\0';
			}
		}
#endif
	}
	if (flags & MYSQLND_DEBUG_DUMP_FILE) {
		snprintf(file_buffer, sizeof(file_buffer) - 1, "%14s: ", file);
		file_buffer[sizeof(file_buffer) - 1 ] = '\0';
	}
	if (flags & MYSQLND_DEBUG_DUMP_LINE) {
		snprintf(line_buffer, sizeof(line_buffer) - 1, "%5u: ", line);
		line_buffer[sizeof(line_buffer) - 1 ] = '\0';
	}
	if (flags & MYSQLND_DEBUG_DUMP_LEVEL) {
		snprintf(level_buffer, sizeof(level_buffer) - 1, "%4u: ", level);
		level_buffer[sizeof(level_buffer) - 1 ] = '\0';
	}


	va_start(args, format);
	vspprintf(&buffer, 0, format, args);
	va_end(args);

	message_line_len = spprintf(&message_line, 0, "%s%s%s%s%s%s%s%s\n",
								flags & MYSQLND_DEBUG_DUMP_PID? pid_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_TIME? time_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_FILE? file_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_LINE? line_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_LEVEL? level_buffer:"",
								pipe_buffer, type? type:"", buffer);
	efree(buffer);

	ret = php_stream_write(self->stream, message_line, message_line_len)? PASS:FAIL;
	efree(message_line);

	if (flags & MYSQLND_DEBUG_FLUSH) {
		self->m->close(self);
		self->m->open(self, TRUE);	
	}
	return ret;
}
/* }}} */


/* FALSE - The DBG_ calls won't be traced, TRUE - will be traced */
/* {{{ mysqlnd_res_meta::func_enter */
static zend_bool
MYSQLND_METHOD(mysqlnd_debug, func_enter)(MYSQLND_DEBUG * self,
										  unsigned int line, const char * const file,
										  char * func_name, uint func_name_len)
{
	MYSQLND_ZTS(self);
	if ((self->flags & MYSQLND_DEBUG_DUMP_TRACE) == 0 || self->file_name == NULL) {
		return FALSE;
	}
#ifdef ZTS
	if (MYSQLND_G(thread_id) != tsrm_thread_id()) {
		return FALSE; /* don't trace background threads */
	}
#endif
	if (zend_stack_count(&self->call_stack) >= self->nest_level_limit) {
		return FALSE;
	}

	if ((self->flags & MYSQLND_DEBUG_TRACE_MEMORY_CALLS) == 0 && 
		(func_name == mysqlnd_emalloc_name	|| func_name == mysqlnd_pemalloc_name	||
		 func_name == mysqlnd_ecalloc_name	|| func_name == mysqlnd_pecalloc_name	||
		 func_name == mysqlnd_erealloc_name || func_name == mysqlnd_perealloc_name	||
		 func_name == mysqlnd_efree_name	|| func_name == mysqlnd_pefree_name		|| 
		 func_name == mysqlnd_malloc_name	|| func_name == mysqlnd_calloc_name		|| 
		 func_name == mysqlnd_realloc_name	|| func_name == mysqlnd_free_name		||
		 func_name == mysqlnd_palloc_zval_ptr_dtor_name	|| func_name == mysqlnd_palloc_get_zval_name ||
		 func_name == mysqlnd_read_header_name || func_name == mysqlnd_read_body_name)) {
		zend_stack_push(&self->call_stack, "", sizeof(""));
	   	return FALSE;
	}

	zend_stack_push(&self->call_stack, func_name, func_name_len + 1);

	if (zend_hash_num_elements(&self->not_filtered_functions) &&
		0 == zend_hash_exists(&self->not_filtered_functions, func_name, strlen(func_name) + 1))
	{
		return FALSE;
	}

	self->m->log_va(self, line, file, zend_stack_count(&self->call_stack) - 1, NULL, ">%s", func_name);
	return TRUE;
}
/* }}} */


/* {{{ mysqlnd_res_meta::func_leave */
static enum_func_status
MYSQLND_METHOD(mysqlnd_debug, func_leave)(MYSQLND_DEBUG * self, unsigned int line,
										  const char * const file)
{
	char *func_name;
	MYSQLND_ZTS(self);

	if ((self->flags & MYSQLND_DEBUG_DUMP_TRACE) == 0 || self->file_name == NULL) {
		return PASS;
	}
#ifdef ZTS
	if (MYSQLND_G(thread_id) != tsrm_thread_id()) {
		return PASS; /* don't trace background threads */
	}
#endif
	if (zend_stack_count(&self->call_stack) >= self->nest_level_limit) {
		return PASS;
	}

	zend_stack_top(&self->call_stack, (void **)&func_name);

	if (func_name[0] == '\0') {
		; /* don't log that function */
	} else if (!zend_hash_num_elements(&self->not_filtered_functions) ||
		1 == zend_hash_exists(&self->not_filtered_functions, func_name, strlen(func_name) + 1))
	{
		self->m->log_va(self, line, file, zend_stack_count(&self->call_stack) - 1, NULL, "<%s", func_name);
	}

	return zend_stack_del_top(&self->call_stack) == SUCCESS? PASS:FAIL;
}
/* }}} */


/* {{{ mysqlnd_res_meta::close */
static enum_func_status
MYSQLND_METHOD(mysqlnd_debug, close)(MYSQLND_DEBUG * self)
{
	MYSQLND_ZTS(self);
	if (self->stream) {
		php_stream_free(self->stream, PHP_STREAM_FREE_CLOSE);
		self->stream = NULL;
	}
	return PASS;
}
/* }}} */


/* {{{ mysqlnd_res_meta::free */
static enum_func_status
MYSQLND_METHOD(mysqlnd_debug, free)(MYSQLND_DEBUG * self)
{
	if (self->file_name && self->file_name != mysqlnd_debug_default_trace_file) {
		efree(self->file_name);
		self->file_name = NULL;
	}
	zend_stack_destroy(&self->call_stack);
	zend_hash_destroy(&self->not_filtered_functions);
	efree(self);
	return PASS;
}
/* }}} */

enum mysqlnd_debug_parser_state
{
	PARSER_WAIT_MODIFIER,
	PARSER_WAIT_COLON,
	PARSER_WAIT_VALUE
};


/* {{{ mysqlnd_res_meta::set_mode */
static void
MYSQLND_METHOD(mysqlnd_debug, set_mode)(MYSQLND_DEBUG * self, const char * const mode)
{
	uint mode_len = strlen(mode), i;
	enum mysqlnd_debug_parser_state state = PARSER_WAIT_MODIFIER;

	self->flags = 0;
	self->nest_level_limit = 0;
	if (self->file_name && self->file_name != mysqlnd_debug_default_trace_file) {
		efree(self->file_name);
		self->file_name = NULL;
	}
	if (zend_hash_num_elements(&self->not_filtered_functions)) {
		zend_hash_destroy(&self->not_filtered_functions);
		zend_hash_init(&self->not_filtered_functions, 0, NULL, NULL, 0);
	}

	for (i = 0; i < mode_len; i++) {
		switch (mode[i]) {
			case 'O':
			case 'A':
				self->flags |= MYSQLND_DEBUG_FLUSH;
			case 'a':
			case 'o':
				if (mode[i] == 'a' || mode[i] == 'A') {
					self->flags |= MYSQLND_DEBUG_APPEND;
				}
				if (i + 1 < mode_len && mode[i+1] == ',') {
					unsigned int j = i + 2;
					while (j < mode_len) {
						if (mode[j] == ':') {
							break;
						}
						j++;
					}
					if (j > i + 2) {
						self->file_name = estrndup(mode + i + 2, j - i - 2);
					}
					i = j;
				} else {
					if (!self->file_name)
						self->file_name = (char *) mysqlnd_debug_default_trace_file;
				}
				state = PARSER_WAIT_COLON;
				break;
			case ':':
#if 0
				if (state != PARSER_WAIT_COLON) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Consecutive semicolons at position %u", i);
				}
#endif
				state = PARSER_WAIT_MODIFIER;
				break;
			case 'f': /* limit output to these functions */
				if (i + 1 < mode_len && mode[i+1] == ',') {
					unsigned int j = i + 2;
					i++;
					while (j < mode_len) {
						if (mode[j] == ':') {
							/* function names with :: */
							if ((j + 1 < mode_len) && mode[j+1] == ':') {
								j += 2;
								continue;
							}
						}
						if (mode[j] == ',' || mode[j] == ':') {
							if (j > i + 2) {
								char func_name[1024];
								uint func_name_len = MIN(sizeof(func_name) - 1, j - i - 1);
								memcpy(func_name, mode + i + 1, func_name_len);
								func_name[func_name_len] = '\0'; 

								zend_hash_add_empty_element(&self->not_filtered_functions,
															func_name, func_name_len + 1);
								i = j;
							}
							if (mode[j] == ':') {
								break;
							}
						}
						j++;
					}
					i = j;
				} else {
#if 0
					php_error_docref(NULL TSRMLS_CC, E_WARNING,
									 "Expected list of functions for '%c' found none", mode[i]);				
#endif
				}
				state = PARSER_WAIT_COLON;
				break;
			case 'D':
			case 'd':
			case 'g':
			case 'p':
				/* unsupported */
				if ((i + 1) < mode_len && mode[i+1] == ',') {
					i+= 2;
					while (i < mode_len) {
						if (mode[i] == ':') {
							break;
						}
						i++;
					}
				}
				state = PARSER_WAIT_COLON;
				break;
			case 'F':
				self->flags |= MYSQLND_DEBUG_DUMP_FILE;
				state = PARSER_WAIT_COLON;
				break;
			case 'i':
				self->flags |= MYSQLND_DEBUG_DUMP_PID;
				state = PARSER_WAIT_COLON;
				break;
			case 'L':
				self->flags |= MYSQLND_DEBUG_DUMP_LINE;
				state = PARSER_WAIT_COLON;
				break;
			case 'n':
				self->flags |= MYSQLND_DEBUG_DUMP_LEVEL;
				state = PARSER_WAIT_COLON;
				break;
			case 't':
				if (mode[i+1] == ',') {
					unsigned int j = i + 2;
					while (j < mode_len) {
						if (mode[j] == ':') {
							break;
						}
						j++;
					}
					if (j > i + 2) {
						char *value_str = estrndup(mode + i + 2, j - i - 2);
						self->nest_level_limit = atoi(value_str);
						efree(value_str);
					}
					i = j;
				} else {
					self->nest_level_limit = 200; /* default value for FF DBUG */
				}
				self->flags |= MYSQLND_DEBUG_DUMP_TRACE;
				state = PARSER_WAIT_COLON;
				break;
			case 'T':
				self->flags |= MYSQLND_DEBUG_DUMP_TIME;
				state = PARSER_WAIT_COLON;
				break;
			case 'N':
			case 'P':
			case 'r':
			case 'S':
				state = PARSER_WAIT_COLON;
				break;
			case 'm': /* mysqlnd extension - trace memory functions */
				self->flags |= MYSQLND_DEBUG_TRACE_MEMORY_CALLS;
				state = PARSER_WAIT_COLON;
				break;
			default:
				if (state == PARSER_WAIT_MODIFIER) {
#if 0
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unrecognized format '%c'", mode[i]);
#endif
					if (i+1 < mode_len && mode[i+1] == ',') {
						i+= 2;
						while (i < mode_len) {
							if (mode[i] == ':') {
								break;
							}
							i++;
						}
					}
					state = PARSER_WAIT_COLON;
				} else if (state == PARSER_WAIT_COLON) {
#if 0
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Colon expected, '%c' found", mode[i]);				
#endif
				}
				break;
		}
	}
}
/* }}} */


MYSQLND_CLASS_METHODS_START(mysqlnd_debug)
	MYSQLND_METHOD(mysqlnd_debug, open),
	MYSQLND_METHOD(mysqlnd_debug, set_mode),
	MYSQLND_METHOD(mysqlnd_debug, log),
	MYSQLND_METHOD(mysqlnd_debug, log_va),
	MYSQLND_METHOD(mysqlnd_debug, func_enter),
	MYSQLND_METHOD(mysqlnd_debug, func_leave),
	MYSQLND_METHOD(mysqlnd_debug, close),
	MYSQLND_METHOD(mysqlnd_debug, free),
MYSQLND_CLASS_METHODS_END;



/* {{{ mysqlnd_debug_init */
MYSQLND_DEBUG *mysqlnd_debug_init(TSRMLS_D)
{
	MYSQLND_DEBUG *ret = ecalloc(1, sizeof(MYSQLND_DEBUG));
#ifdef ZTS
	ret->TSRMLS_C = TSRMLS_C;
#endif
	ret->nest_level_limit = 0;
	ret->pid = getpid();
	zend_stack_init(&ret->call_stack);
	zend_hash_init(&ret->not_filtered_functions, 0, NULL, NULL, 0);

	ret->m = & mysqlnd_mysqlnd_debug_methods;
	
	return ret;
}
/* }}} */


/* {{{ _mysqlnd_debug */
void _mysqlnd_debug(const char *mode TSRMLS_DC)
{
#ifdef PHP_DEBUG
	MYSQLND_DEBUG *dbg = MYSQLND_G(dbg);
	if (!dbg) {
		MYSQLND_G(dbg) = dbg = mysqlnd_debug_init(TSRMLS_C);
		if (!dbg) {
			return;
		}
	}
	
	dbg->m->close(dbg);
	dbg->m->set_mode(dbg, mode);
	while (zend_stack_count(&dbg->call_stack)) {
		zend_stack_del_top(&dbg->call_stack);
	}
#endif
}
/* }}} */


#if ZEND_DEBUG
#else
#define __zend_filename "/unknown/unknown"
#define __zend_lineno   0
#endif


/* {{{ _mysqlnd_emalloc */
void * _mysqlnd_emalloc(size_t size MYSQLND_MEM_D)
{
	void *ret;
	DBG_ENTER(mysqlnd_emalloc_name);
#ifdef ZTS
	if (MYSQLND_G(thread_id) != tsrm_thread_id()) {
		DBG_RETURN(_mysqlnd_pemalloc(size, 1 TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC));
	}
#endif

	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("before: %lu", zend_memory_usage(FALSE TSRMLS_CC));
	ret = emalloc(size);
	DBG_INF_FMT("after : %lu", zend_memory_usage(FALSE TSRMLS_CC));
	DBG_INF_FMT("size=%lu ptr=%p", size, ret); 

	if (MYSQLND_G(collect_memory_statistics)) {
		MYSQLND_INC_GLOBAL_STATISTIC2_W_VALUE(STAT_MEM_EMALLOC_COUNT, 1, STAT_MEM_EMALLOC_AMMOUNT, size);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ _mysqlnd_pemalloc */
void * _mysqlnd_pemalloc(size_t size, zend_bool persistent MYSQLND_MEM_D)
{
	void *ret;
	DBG_ENTER(mysqlnd_pemalloc_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	if (persistent == FALSE) {
		DBG_INF_FMT("before: %lu", zend_memory_usage(persistent TSRMLS_CC));
	}

	ret = pemalloc(size, persistent);
	DBG_INF_FMT("size=%lu ptr=%p persistent=%d", size, ret, persistent); 

	if (persistent == FALSE) {
		DBG_INF_FMT("after : %lu", zend_memory_usage(persistent TSRMLS_CC));
	}

	if (MYSQLND_G(collect_memory_statistics)) {
		enum mysqlnd_collected_stats s1 = persistent? STAT_MEM_MALLOC_COUNT:STAT_MEM_EMALLOC_COUNT;
		enum mysqlnd_collected_stats s2 = persistent? STAT_MEM_MALLOC_AMMOUNT:STAT_MEM_EMALLOC_AMMOUNT;
		MYSQLND_INC_GLOBAL_STATISTIC2_W_VALUE(s1, 1, s2, size);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ _mysqlnd_ecalloc */
void * _mysqlnd_ecalloc(uint nmemb, size_t size MYSQLND_MEM_D)
{
	void *ret;
	DBG_ENTER(mysqlnd_ecalloc_name);
#ifdef ZTS
	if (MYSQLND_G(thread_id) != tsrm_thread_id()) {
		DBG_RETURN(_mysqlnd_pecalloc(nmemb, size, 1 TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC));
	}
#endif
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("before: %lu", zend_memory_usage(FALSE TSRMLS_CC));

	ret = ecalloc(nmemb, size);

	DBG_INF_FMT("after : %lu", zend_memory_usage(FALSE TSRMLS_CC));
	DBG_INF_FMT("size=%lu ptr=%p", size, ret); 
	if (MYSQLND_G(collect_memory_statistics)) {
		MYSQLND_INC_GLOBAL_STATISTIC2_W_VALUE(STAT_MEM_ECALLOC_COUNT, 1, STAT_MEM_ECALLOC_AMMOUNT, size);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ _mysqlnd_pecalloc */
void * _mysqlnd_pecalloc(uint nmemb, size_t size, zend_bool persistent MYSQLND_MEM_D)
{
	void *ret;
	DBG_ENTER(mysqlnd_pecalloc_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	if (persistent == FALSE) {
		DBG_INF_FMT("before: %lu", zend_memory_usage(persistent TSRMLS_CC));
	}

	ret = pecalloc(nmemb, size, persistent);
	DBG_INF_FMT("size=%lu ptr=%p", size, ret); 

	if (persistent == FALSE) {
		DBG_INF_FMT("after : %lu", zend_memory_usage(persistent TSRMLS_CC));
	}

	if (MYSQLND_G(collect_memory_statistics)) {
		enum mysqlnd_collected_stats s1 = persistent? STAT_MEM_CALLOC_COUNT:STAT_MEM_ECALLOC_COUNT;
		enum mysqlnd_collected_stats s2 = persistent? STAT_MEM_CALLOC_AMMOUNT:STAT_MEM_ECALLOC_AMMOUNT;
		MYSQLND_INC_GLOBAL_STATISTIC2_W_VALUE(s1, 1, s2, size);
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ _mysqlnd_erealloc */
void * _mysqlnd_erealloc(void *ptr, size_t new_size MYSQLND_MEM_D)
{
	void *ret;
	DBG_ENTER(mysqlnd_erealloc_name);
#ifdef ZTS
	if (MYSQLND_G(thread_id) != tsrm_thread_id()) {
		DBG_RETURN(_mysqlnd_perealloc(ptr, new_size, 1 TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC));
	}
#endif
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("ptr=%p new_size=%lu", ptr, new_size); 
	DBG_INF_FMT("before: %lu", zend_memory_usage(FALSE TSRMLS_CC));

	ret = erealloc(ptr, new_size);

	DBG_INF_FMT("after : %lu", zend_memory_usage(FALSE TSRMLS_CC));
	DBG_INF_FMT("new_ptr=%p", ret); 
	if (MYSQLND_G(collect_memory_statistics)) {
		MYSQLND_INC_GLOBAL_STATISTIC2_W_VALUE(STAT_MEM_EREALLOC_COUNT, 1, STAT_MEM_EREALLOC_AMMOUNT, new_size);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ _mysqlnd_perealloc */
void * _mysqlnd_perealloc(void *ptr, size_t new_size, zend_bool persistent MYSQLND_MEM_D)
{
	void *ret;
	DBG_ENTER(mysqlnd_perealloc_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("ptr=%p new_size=%lu persist=%d", ptr, new_size, persistent); 
	if (persistent == FALSE) {
		DBG_INF_FMT("before: %lu", zend_memory_usage(persistent TSRMLS_CC));
	}

	ret = perealloc(ptr, new_size, persistent);

	DBG_INF_FMT("new_ptr=%p", ret); 

	if (persistent == FALSE) {
		DBG_INF_FMT("after : %lu", zend_memory_usage(persistent TSRMLS_CC));
	}
	MYSQLND_INC_GLOBAL_STATISTIC(persistent? STAT_MEM_EREALLOC_COUNT:STAT_MEM_REALLOC_COUNT);
	if (MYSQLND_G(collect_memory_statistics)) {
		enum mysqlnd_collected_stats s1 = persistent? STAT_MEM_REALLOC_COUNT:STAT_MEM_EREALLOC_COUNT;
		enum mysqlnd_collected_stats s2 = persistent? STAT_MEM_REALLOC_AMMOUNT:STAT_MEM_EREALLOC_AMMOUNT;
		MYSQLND_INC_GLOBAL_STATISTIC2_W_VALUE(s1, 1, s2, new_size);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ _mysqlnd_efree */
void _mysqlnd_efree(void *ptr MYSQLND_MEM_D)
{
	DBG_ENTER(mysqlnd_efree_name);
#ifdef ZTS
	if (MYSQLND_G(thread_id) != tsrm_thread_id()) {
		DBG_RETURN(_mysqlnd_pefree(ptr, 1 TSRMLS_CC ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC));
	}
#endif
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("ptr=%p", ptr); 
	DBG_INF_FMT("before: %lu", zend_memory_usage(FALSE TSRMLS_CC));
	MYSQLND_INC_GLOBAL_STATISTIC(STAT_MEM_EFREE_COUNT);

	efree(ptr);

	DBG_INF_FMT("after : %lu", zend_memory_usage(FALSE TSRMLS_CC));
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ _mysqlnd_pefree */
void _mysqlnd_pefree(void *ptr, zend_bool persistent MYSQLND_MEM_D)
{
	DBG_ENTER(mysqlnd_pefree_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("ptr=%p persistent=%d", ptr, persistent); 
	if (persistent == FALSE) {
		DBG_INF_FMT("before: %lu", zend_memory_usage(persistent TSRMLS_CC));
	}

	pefree(ptr, persistent);

	if (persistent == FALSE) {
		DBG_INF_FMT("after : %lu", zend_memory_usage(persistent TSRMLS_CC));
	}
	if (MYSQLND_G(collect_memory_statistics)) {
		MYSQLND_INC_GLOBAL_STATISTIC(persistent? STAT_MEM_FREE_COUNT:STAT_MEM_EFREE_COUNT);
	}
	DBG_VOID_RETURN;
}


/* {{{ _mysqlnd_malloc */
void * _mysqlnd_malloc(size_t size MYSQLND_MEM_D)
{
	void *ret;
	DBG_ENTER(mysqlnd_malloc_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);

	ret = malloc(size);

	DBG_INF_FMT("size=%lu ptr=%p", size, ret); 
	if (MYSQLND_G(collect_memory_statistics)) {
		MYSQLND_INC_GLOBAL_STATISTIC2_W_VALUE(STAT_MEM_MALLOC_COUNT, 1, STAT_MEM_MALLOC_AMMOUNT, size);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ _mysqlnd_calloc */
void * _mysqlnd_calloc(uint nmemb, size_t size MYSQLND_MEM_D)
{
	void *ret;
	DBG_ENTER(mysqlnd_calloc_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);

	ret = calloc(nmemb, size);

	DBG_INF_FMT("size=%lu ptr=%p", size, ret); 
	if (MYSQLND_G(collect_memory_statistics)) {
		MYSQLND_INC_GLOBAL_STATISTIC2_W_VALUE(STAT_MEM_CALLOC_COUNT, 1, STAT_MEM_CALLOC_AMMOUNT, size);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ _mysqlnd_realloc */
void * _mysqlnd_realloc(void *ptr, size_t new_size MYSQLND_MEM_D)
{
	void *ret;
	DBG_ENTER(mysqlnd_realloc_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("ptr=%p new_size=%lu ", new_size, ptr); 
	DBG_INF_FMT("before: %lu", zend_memory_usage(TRUE TSRMLS_CC));

	ret = realloc(ptr, new_size);

	DBG_INF_FMT("new_ptr=%p", ret);

	if (MYSQLND_G(collect_memory_statistics)) {
		MYSQLND_INC_GLOBAL_STATISTIC2_W_VALUE(STAT_MEM_REALLOC_COUNT, 1, STAT_MEM_REALLOC_AMMOUNT, new_size);
	}
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ _mysqlnd_free */
void _mysqlnd_free(void *ptr MYSQLND_MEM_D)
{
	DBG_ENTER(mysqlnd_free_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("ptr=%p", ptr); 

	free(ptr);

	if (MYSQLND_G(collect_memory_statistics)) {
		MYSQLND_INC_GLOBAL_STATISTIC(STAT_MEM_FREE_COUNT);
	}
	DBG_VOID_RETURN;
}
/* }}} */




/* Follows code borrowed from zend_builtin_functions.c because the functions there are static */

#if PHP_MAJOR_VERSION >= 6
/* {{{ gettraceasstring() macros */
#define TRACE_APPEND_CHR(chr)                                            \
	*str = (char*)erealloc(*str, *len + 1 + 1);                          \
	(*str)[(*len)++] = chr

#define TRACE_APPEND_STRL(val, vallen)                                   \
	{                                                                    \
		int l = vallen;                                                  \
		*str = (char*)erealloc(*str, *len + l + 1);                      \
		memcpy((*str) + *len, val, l);                                   \
		*len += l;                                                       \
	}

#define TRACE_APPEND_USTRL(val, vallen) \
	{ \
		zval tmp, copy; \
		int use_copy; \
		ZVAL_UNICODEL(&tmp, val, vallen, 1); \
		zend_make_printable_zval(&tmp, &copy, &use_copy); \
		TRACE_APPEND_STRL(Z_STRVAL(copy), Z_STRLEN(copy)); \
		zval_dtor(&copy); \
		zval_dtor(&tmp); \
	}

#define TRACE_APPEND_ZVAL(zv) \
	if (Z_TYPE_P((zv)) == IS_UNICODE) { \
		zval copy; \
		int use_copy; \
		zend_make_printable_zval((zv), &copy, &use_copy); \
		TRACE_APPEND_STRL(Z_STRVAL(copy), Z_STRLEN(copy)); \
		zval_dtor(&copy); \
	} else { \
		TRACE_APPEND_STRL(Z_STRVAL_P((zv)), Z_STRLEN_P((zv))); \
	}

#define TRACE_APPEND_STR(val)                                            \
	TRACE_APPEND_STRL(val, sizeof(val)-1)

#define TRACE_APPEND_KEY(key)                                            \
	if (zend_ascii_hash_find(ht, key, sizeof(key), (void**)&tmp) == SUCCESS) { \
		if (Z_TYPE_PP(tmp) == IS_UNICODE) { \
			zval copy; \
			int use_copy; \
			zend_make_printable_zval(*tmp, &copy, &use_copy); \
	    TRACE_APPEND_STRL(Z_STRVAL(copy), Z_STRLEN(copy)); \
	    zval_dtor(&copy); \
		} else { \
	    TRACE_APPEND_STRL(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));           \
	  } \
	}
/* }}} */

/* {{{ mysqlnd_build_trace_args */
static int mysqlnd_build_trace_args(zval **arg, int num_args, va_list args, zend_hash_key *hash_key)
{
	char **str;
	int *len;

	str = va_arg(args, char**);
	len = va_arg(args, int*);

	/* the trivial way would be to do:
	 * conver_to_string_ex(arg);
	 * append it and kill the now tmp arg.
	 * but that could cause some E_NOTICE and also damn long lines.
	 */

	switch (Z_TYPE_PP(arg)) {
		case IS_NULL:
			TRACE_APPEND_STR("NULL, ");
			break;
		case IS_STRING: {
			int l_added;
			TRACE_APPEND_CHR('\'');
			if (Z_STRLEN_PP(arg) > 15) {
				TRACE_APPEND_STRL(Z_STRVAL_PP(arg), 15);
				TRACE_APPEND_STR("...', ");
				l_added = 15 + 6 + 1; /* +1 because of while (--l_added) */
			} else {
				l_added = Z_STRLEN_PP(arg);
				TRACE_APPEND_STRL(Z_STRVAL_PP(arg), l_added);
				TRACE_APPEND_STR("', ");
				l_added += 3 + 1;
			}
			while (--l_added) {
				if ((unsigned char)(*str)[*len - l_added] < 32) {
					(*str)[*len - l_added] = '?';
				}
			}
			break;
		}
		case IS_UNICODE: {
			int l_added;
			TSRMLS_FETCH();

			/*
			 * We do not want to apply current error mode here, since
			 * zend_make_printable_zval() uses output encoding converter.
			 * Temporarily set output encoding converter to escape offending
			 * chars with \uXXXX notation.
			 */
			zend_set_converter_error_mode(ZEND_U_CONVERTER(UG(output_encoding_conv)), ZEND_FROM_UNICODE, ZEND_CONV_ERROR_ESCAPE_JAVA);
			TRACE_APPEND_CHR('\'');
			if (Z_USTRLEN_PP(arg) > 15) {
				TRACE_APPEND_USTRL(Z_USTRVAL_PP(arg), 15);
				TRACE_APPEND_STR("...', ");
				l_added = 15 + 6 + 1; /* +1 because of while (--l_added) */
			} else {
				l_added = Z_USTRLEN_PP(arg);
				TRACE_APPEND_USTRL(Z_USTRVAL_PP(arg), l_added);
				TRACE_APPEND_STR("', ");
				l_added += 3 + 1;
			}
			/*
			 * Reset output encoding converter error mode.
			 */
			zend_set_converter_error_mode(ZEND_U_CONVERTER(UG(output_encoding_conv)), ZEND_FROM_UNICODE, UG(from_error_mode));
			while (--l_added) {
				if ((unsigned char)(*str)[*len - l_added] < 32) {
					(*str)[*len - l_added] = '?';
				}
			}
			break;
		}
		case IS_BOOL:
			if (Z_LVAL_PP(arg)) {
				TRACE_APPEND_STR("true, ");
			} else {
				TRACE_APPEND_STR("false, ");
			}
			break;
		case IS_RESOURCE:
			TRACE_APPEND_STR("Resource id #");
			/* break; */
		case IS_LONG: {
			long lval = Z_LVAL_PP(arg);
			char s_tmp[MAX_LENGTH_OF_LONG + 1];
			int l_tmp = zend_sprintf(s_tmp, "%ld", lval);  /* SAFE */
			TRACE_APPEND_STRL(s_tmp, l_tmp);
			TRACE_APPEND_STR(", ");
			break;
		}
		case IS_DOUBLE: {
			double dval = Z_DVAL_PP(arg);
			char *s_tmp;
			int l_tmp;
			TSRMLS_FETCH();

			s_tmp = emalloc(MAX_LENGTH_OF_DOUBLE + EG(precision) + 1);
			l_tmp = zend_sprintf(s_tmp, "%.*G", (int) EG(precision), dval);  /* SAFE */
			TRACE_APPEND_STRL(s_tmp, l_tmp);
			/* %G already handles removing trailing zeros from the fractional part, yay */
			efree(s_tmp);
			TRACE_APPEND_STR(", ");
			break;
		}
		case IS_ARRAY:
			TRACE_APPEND_STR("Array, ");
			break;
		case IS_OBJECT: {
			zstr class_name;
			zend_uint class_name_len;
			int dup;
			TSRMLS_FETCH();

			TRACE_APPEND_STR("Object(");

			dup = zend_get_object_classname(*arg, &class_name, &class_name_len TSRMLS_CC);

			if (UG(unicode)) {
				zval tmp;

				ZVAL_UNICODEL(&tmp, class_name.u, class_name_len, 1);
				convert_to_string_with_converter(&tmp, ZEND_U_CONVERTER(UG(output_encoding_conv)));
				TRACE_APPEND_STRL(Z_STRVAL(tmp), Z_STRLEN(tmp));
				zval_dtor(&tmp);
			} else {
				TRACE_APPEND_STRL(class_name.s, class_name_len);
			}
			if(!dup) {
				efree(class_name.v);
			}

			TRACE_APPEND_STR("), ");
			break;
		}
		default:
			break;
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */


static int mysqlnd_build_trace_string(zval **frame, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	char *s_tmp, **str;
	int *len, *num;
	long line;
	HashTable *ht = Z_ARRVAL_PP(frame);
	zval **file, **tmp;

	str = va_arg(args, char**);
	len = va_arg(args, int*);
	num = va_arg(args, int*);

	s_tmp = emalloc(1 + MAX_LENGTH_OF_LONG + 1 + 1);
	sprintf(s_tmp, "#%d ", (*num)++);
	TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
	efree(s_tmp);
	if (zend_ascii_hash_find(ht, "file", sizeof("file"), (void**)&file) == SUCCESS) {
		if (zend_ascii_hash_find(ht, "line", sizeof("line"), (void**)&tmp) == SUCCESS) {
			line = Z_LVAL_PP(tmp);
		} else {
			line = 0;
		}
		TRACE_APPEND_ZVAL(*file);
		s_tmp = emalloc(MAX_LENGTH_OF_LONG + 2 + 1);
		sprintf(s_tmp, "(%ld): ", line);
		TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
		efree(s_tmp);
	} else {
		TRACE_APPEND_STR("[internal function]: ");
	}
	TRACE_APPEND_KEY("class");
	TRACE_APPEND_KEY("type");
	TRACE_APPEND_KEY("function");
	TRACE_APPEND_CHR('(');
	if (zend_ascii_hash_find(ht, "args", sizeof("args"), (void**)&tmp) == SUCCESS) {
		int last_len = *len;
		zend_hash_apply_with_arguments(Z_ARRVAL_PP(tmp), (apply_func_args_t)mysqlnd_build_trace_args, 2, str, len);
		if (last_len != *len) {
			*len -= 2; /* remove last ', ' */
		}
	}
	TRACE_APPEND_STR(")\n");
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */


#else /* PHP 5*/


/* {{{ gettraceasstring() macros */
#define TRACE_APPEND_CHR(chr)                                            \
	*str = (char*)erealloc(*str, *len + 1 + 1);                          \
	(*str)[(*len)++] = chr

#define TRACE_APPEND_STRL(val, vallen)                                   \
	{                                                                    \
		int l = vallen;                                                  \
		*str = (char*)erealloc(*str, *len + l + 1);                      \
		memcpy((*str) + *len, val, l);                                   \
		*len += l;                                                       \
	}

#define TRACE_APPEND_STR(val)                                            \
	TRACE_APPEND_STRL(val, sizeof(val)-1)

#define TRACE_APPEND_KEY(key)                                            \
	if (zend_hash_find(ht, key, sizeof(key), (void**)&tmp) == SUCCESS) { \
	    TRACE_APPEND_STRL(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));           \
	}

/* }}} */


static int mysqlnd_build_trace_args(zval **arg, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	char **str;
	int *len;

	str = va_arg(args, char**);
	len = va_arg(args, int*);

	/* the trivial way would be to do:
	 * conver_to_string_ex(arg);
	 * append it and kill the now tmp arg.
	 * but that could cause some E_NOTICE and also damn long lines.
	 */

	switch (Z_TYPE_PP(arg)) {
		case IS_NULL:
			TRACE_APPEND_STR("NULL, ");
			break;
		case IS_STRING: {
			int l_added;
			TRACE_APPEND_CHR('\'');
			if (Z_STRLEN_PP(arg) > 15) {
				TRACE_APPEND_STRL(Z_STRVAL_PP(arg), 15);
				TRACE_APPEND_STR("...', ");
				l_added = 15 + 6 + 1; /* +1 because of while (--l_added) */
			} else {
				l_added = Z_STRLEN_PP(arg);
				TRACE_APPEND_STRL(Z_STRVAL_PP(arg), l_added);
				TRACE_APPEND_STR("', ");
				l_added += 3 + 1;
			}
			while (--l_added) {
				if ((*str)[*len - l_added] < 32) {
					(*str)[*len - l_added] = '?';
				}
			}
			break;
		}
		case IS_BOOL:
			if (Z_LVAL_PP(arg)) {
				TRACE_APPEND_STR("true, ");
			} else {
				TRACE_APPEND_STR("false, ");
			}
			break;
		case IS_RESOURCE:
			TRACE_APPEND_STR("Resource id #");
			/* break; */
		case IS_LONG: {
			long lval = Z_LVAL_PP(arg);
			char s_tmp[MAX_LENGTH_OF_LONG + 1];
			int l_tmp = zend_sprintf(s_tmp, "%ld", lval);  /* SAFE */
			TRACE_APPEND_STRL(s_tmp, l_tmp);
			TRACE_APPEND_STR(", ");
			break;
		}
		case IS_DOUBLE: {
			double dval = Z_DVAL_PP(arg);
			char *s_tmp;
			int l_tmp;
			TSRMLS_FETCH();

			s_tmp = emalloc(MAX_LENGTH_OF_DOUBLE + EG(precision) + 1);
			l_tmp = zend_sprintf(s_tmp, "%.*G", (int) EG(precision), dval);  /* SAFE */
			TRACE_APPEND_STRL(s_tmp, l_tmp);
			/* %G already handles removing trailing zeros from the fractional part, yay */
			efree(s_tmp);
			TRACE_APPEND_STR(", ");
			break;
		}
		case IS_ARRAY:
			TRACE_APPEND_STR("Array, ");
			break;
		case IS_OBJECT: {
			char *class_name;
			zend_uint class_name_len;
			int dup;
			TSRMLS_FETCH();

			TRACE_APPEND_STR("Object(");

			dup = zend_get_object_classname(*arg, &class_name, &class_name_len TSRMLS_CC);

			TRACE_APPEND_STRL(class_name, class_name_len);
			if(!dup) {
				efree(class_name);
			}

			TRACE_APPEND_STR("), ");
			break;
		}
		default:
			break;
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static int mysqlnd_build_trace_string(zval **frame, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	char *s_tmp, **str;
	int *len, *num;
	long line;
	HashTable *ht = Z_ARRVAL_PP(frame);
	zval **file, **tmp;

	str = va_arg(args, char**);
	len = va_arg(args, int*);
	num = va_arg(args, int*);

	s_tmp = emalloc(1 + MAX_LENGTH_OF_LONG + 1 + 1);
	sprintf(s_tmp, "#%d ", (*num)++);
	TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
	efree(s_tmp);
	if (zend_hash_find(ht, "file", sizeof("file"), (void**)&file) == SUCCESS) {
		if (zend_hash_find(ht, "line", sizeof("line"), (void**)&tmp) == SUCCESS) {
			line = Z_LVAL_PP(tmp);
		} else {
			line = 0;
		}
		s_tmp = emalloc(Z_STRLEN_PP(file) + MAX_LENGTH_OF_LONG + 4 + 1);
		sprintf(s_tmp, "%s(%ld): ", Z_STRVAL_PP(file), line);
		TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
		efree(s_tmp);
	} else {
		TRACE_APPEND_STR("[internal function]: ");
	}
	TRACE_APPEND_KEY("class");
	TRACE_APPEND_KEY("type");
	TRACE_APPEND_KEY("function");
	TRACE_APPEND_CHR('(');
	if (zend_hash_find(ht, "args", sizeof("args"), (void**)&tmp) == SUCCESS) {
		int last_len = *len;
		zend_hash_apply_with_arguments(Z_ARRVAL_PP(tmp), (apply_func_args_t)mysqlnd_build_trace_args, 2, str, len);
		if (last_len != *len) {
			*len -= 2; /* remove last ', ' */
		}
	}
	TRACE_APPEND_STR(")\n");
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */
#endif


char * mysqlnd_get_backtrace(TSRMLS_D)
{
	zval *trace;
	char *res = estrdup(""), **str = &res, *s_tmp;
	int res_len = 0, *len = &res_len, num = 0;

	MAKE_STD_ZVAL(trace);
	zend_fetch_debug_backtrace(trace, 0, 0 TSRMLS_CC);

	zend_hash_apply_with_arguments(Z_ARRVAL_P(trace), (apply_func_args_t)mysqlnd_build_trace_string, 3, str, len, &num);
	zval_ptr_dtor(&trace);

	s_tmp = emalloc(1 + MAX_LENGTH_OF_LONG + 7 + 1);
	sprintf(s_tmp, "#%d {main}", num);
	TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
	efree(s_tmp);

	res[res_len] = '\0';

	return res;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
