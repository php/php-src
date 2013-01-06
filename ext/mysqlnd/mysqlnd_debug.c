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

#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_statistics.h"
#include "zend_builtin_functions.h"

static const char * const mysqlnd_debug_default_trace_file = "/tmp/mysqlnd.trace";

#ifdef ZTS 
#define MYSQLND_ZTS(self) TSRMLS_D = (self)->TSRMLS_C
#else
#define MYSQLND_ZTS(self)
#endif

static const char mysqlnd_emalloc_name[]	= "_mysqlnd_emalloc";
static const char mysqlnd_pemalloc_name[]	= "_mysqlnd_pemalloc";
static const char mysqlnd_ecalloc_name[]	= "_mysqlnd_ecalloc";
static const char mysqlnd_pecalloc_name[]	= "_mysqlnd_pecalloc";
static const char mysqlnd_erealloc_name[]	= "_mysqlnd_erealloc";
static const char mysqlnd_perealloc_name[]	= "_mysqlnd_perealloc";
static const char mysqlnd_efree_name[]		= "_mysqlnd_efree";
static const char mysqlnd_pefree_name[]		= "_mysqlnd_pefree";
static const char mysqlnd_malloc_name[]		= "_mysqlnd_malloc";
static const char mysqlnd_calloc_name[]		= "_mysqlnd_calloc";
static const char mysqlnd_realloc_name[]	= "_mysqlnd_realloc";
static const char mysqlnd_free_name[]		= "_mysqlnd_free";
static const char mysqlnd_pestrndup_name[]	= "_mysqlnd_pestrndup";
static const char mysqlnd_pestrdup_name[]	= "_mysqlnd_pestrdup";

const char * mysqlnd_debug_std_no_trace_funcs[] =
{
	mysqlnd_emalloc_name,
	mysqlnd_ecalloc_name,
	mysqlnd_efree_name,
	mysqlnd_erealloc_name,
	mysqlnd_pemalloc_name,
	mysqlnd_pecalloc_name,
	mysqlnd_pefree_name,
	mysqlnd_perealloc_name,
	mysqlnd_malloc_name,
	mysqlnd_calloc_name,
	mysqlnd_realloc_name,
	mysqlnd_free_name,
	mysqlnd_pestrndup_name,
	mysqlnd_read_header_name,
	mysqlnd_read_body_name,
	NULL /* must be always last */
};


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
	unsigned int message_line_len;
	unsigned int flags = self->flags;
	char pid_buffer[10], time_buffer[30], file_buffer[200],
		 line_buffer[6], level_buffer[7];
	MYSQLND_ZTS(self);

	if (!self->stream && FAIL == self->m->open(self, FALSE)) {
		return FAIL;
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
	efree(message_line); /* allocated by spprintf */
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
	unsigned int message_line_len;
	va_list args;
	unsigned int flags = self->flags;
	char pid_buffer[10], time_buffer[30], file_buffer[200],
		 line_buffer[6], level_buffer[7];
	MYSQLND_ZTS(self);

	if (!self->stream && FAIL == self->m->open(self, FALSE)) {
		return FAIL;
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
	efree(message_line); /* allocated by spprintf */

	if (flags & MYSQLND_DEBUG_FLUSH) {
		self->m->close(self);
		self->m->open(self, TRUE);
	}
	return ret;
}
/* }}} */


/* FALSE - The DBG_ calls won't be traced, TRUE - will be traced */
/* {{{ mysqlnd_debug::func_enter */
static zend_bool
MYSQLND_METHOD(mysqlnd_debug, func_enter)(MYSQLND_DEBUG * self,
										  unsigned int line, const char * const file,
										  const char * const func_name, unsigned int func_name_len)
{
	if ((self->flags & MYSQLND_DEBUG_DUMP_TRACE) == 0 || self->file_name == NULL) {
		return FALSE;
	}
	if ((uint) zend_stack_count(&self->call_stack) >= self->nest_level_limit) {
		return FALSE;
	}

	if ((self->flags & MYSQLND_DEBUG_TRACE_MEMORY_CALLS) == 0 && self->skip_functions) {
		const char ** p = self->skip_functions;
		while (*p) {
			if (*p == func_name) {
				zend_stack_push(&self->call_stack, "", sizeof(""));
#ifndef MYSQLND_PROFILING_DISABLED
				if (self->flags & MYSQLND_DEBUG_PROFILE_CALLS) {
					uint64_t some_time = 0;
					zend_stack_push(&self->call_time_stack, &some_time, sizeof(some_time));
				}
#endif
				return FALSE;
			}
			p++;
		}
	}

	zend_stack_push(&self->call_stack, func_name, func_name_len + 1);
#ifndef MYSQLND_PROFILING_DISABLED
	if (self->flags & MYSQLND_DEBUG_PROFILE_CALLS) {
		uint64_t some_time = 0;
		zend_stack_push(&self->call_time_stack, &some_time, sizeof(some_time));
	}
#endif

	if (zend_hash_num_elements(&self->not_filtered_functions) &&
		0 == zend_hash_exists(&self->not_filtered_functions, func_name, strlen(func_name) + 1))
	{
		return FALSE;
	}

	self->m->log_va(self, line, file, zend_stack_count(&self->call_stack) - 1, NULL, ">%s", func_name);
	return TRUE;
}
/* }}} */

#ifndef MYSQLND_PROFILING_DISABLED
struct st_mysqlnd_dbg_function_profile {
	uint64_t calls;
	uint64_t min_own;
	uint64_t max_own;
	uint64_t avg_own;
	uint64_t own_underporm_calls;
	uint64_t min_in_calls;
	uint64_t max_in_calls;
	uint64_t avg_in_calls;
	uint64_t in_calls_underporm_calls;
	uint64_t min_total;
	uint64_t max_total;
	uint64_t avg_total;	
	uint64_t total_underporm_calls;
};
#define PROFILE_UNDERPERFORM_THRESHOLD 10
#endif

/* {{{ mysqlnd_debug::func_leave */
static enum_func_status
MYSQLND_METHOD(mysqlnd_debug, func_leave)(MYSQLND_DEBUG * self, unsigned int line, const char * const file, uint64_t call_time)
{
	char *func_name;
	uint64_t * parent_non_own_time_ptr = NULL, * mine_non_own_time_ptr = NULL;
	uint64_t mine_non_own_time = 0;
	zend_bool profile_calls = self->flags & MYSQLND_DEBUG_PROFILE_CALLS? TRUE:FALSE;

	if ((self->flags & MYSQLND_DEBUG_DUMP_TRACE) == 0 || self->file_name == NULL) {
		return PASS;
	}
	if ((uint) zend_stack_count(&self->call_stack) >= self->nest_level_limit) {
		return PASS;
	}

	zend_stack_top(&self->call_stack, (void **)&func_name);

#ifndef MYSQLND_PROFILING_DISABLED
	if (profile_calls) {
		zend_stack_top(&self->call_time_stack, (void **)&mine_non_own_time_ptr);
		mine_non_own_time = *mine_non_own_time_ptr;
		zend_stack_del_top(&self->call_time_stack); /* callee - removing ourselves */
	}
#endif

	if (func_name[0] == '\0') {
		; /* don't log that function */
	} else if (!zend_hash_num_elements(&self->not_filtered_functions) ||
			   1 == zend_hash_exists(&self->not_filtered_functions, func_name, strlen(func_name) + 1))
	{
#ifndef MYSQLND_PROFILING_DISABLED
		if (FALSE == profile_calls) {
#endif
			self->m->log_va(self, line, file, zend_stack_count(&self->call_stack) - 1, NULL, "<%s", func_name);

#ifndef MYSQLND_PROFILING_DISABLED
		} else {
			struct st_mysqlnd_dbg_function_profile f_profile_stack = {0};
			struct st_mysqlnd_dbg_function_profile * f_profile = NULL;
			uint64_t own_time = call_time - mine_non_own_time;
			uint func_name_len = strlen(func_name);

			self->m->log_va(self, line, file, zend_stack_count(&self->call_stack) - 1, NULL, "<%s (total=%u own=%u in_calls=%u)",
						func_name, (unsigned int) call_time, (unsigned int) own_time, (unsigned int) mine_non_own_time
					);

			if (SUCCESS == zend_hash_find(&self->function_profiles, func_name, func_name_len + 1, (void **) &f_profile)) {
				/* found */
					if (f_profile) {
					if (mine_non_own_time < f_profile->min_in_calls) {
						f_profile->min_in_calls = mine_non_own_time;
					} else if (mine_non_own_time > f_profile->max_in_calls) {
						f_profile->max_in_calls = mine_non_own_time;
					}
					f_profile->avg_in_calls = (f_profile->avg_in_calls * f_profile->calls + mine_non_own_time) / (f_profile->calls + 1);

					if (own_time < f_profile->min_own) {
						f_profile->min_own = own_time;
					} else if (own_time > f_profile->max_own) {
						f_profile->max_own = own_time;
					}
					f_profile->avg_own = (f_profile->avg_own * f_profile->calls + own_time) / (f_profile->calls + 1);

					if (call_time < f_profile->min_total) {
						f_profile->min_total = call_time;
					} else if (call_time > f_profile->max_total) {
						f_profile->max_total = call_time;
					}
					f_profile->avg_total = (f_profile->avg_total * f_profile->calls + call_time) / (f_profile->calls + 1);

					++f_profile->calls;
					if (f_profile->calls > PROFILE_UNDERPERFORM_THRESHOLD) {
						if (f_profile->avg_in_calls < mine_non_own_time) {
							f_profile->in_calls_underporm_calls++;
						}
						if (f_profile->avg_own < own_time) {
							f_profile->own_underporm_calls++;
						}
						if (f_profile->avg_total < call_time) {
							f_profile->total_underporm_calls++;
						}
					}
				}
			} else {
				/* add */
				f_profile = &f_profile_stack;
				f_profile->min_in_calls = f_profile->max_in_calls = f_profile->avg_in_calls = mine_non_own_time;
				f_profile->min_total = f_profile->max_total = f_profile->avg_total = call_time;
				f_profile->min_own = f_profile->max_own = f_profile->avg_own = own_time;
				f_profile->calls = 1;
				zend_hash_add(&self->function_profiles, func_name, func_name_len+1, f_profile, sizeof(struct st_mysqlnd_dbg_function_profile), NULL);
			}
			if ((uint) zend_stack_count(&self->call_time_stack)) {
				uint64_t parent_non_own_time = 0;

				zend_stack_top(&self->call_time_stack, (void **)&parent_non_own_time_ptr);
				parent_non_own_time = *parent_non_own_time_ptr;
				parent_non_own_time += call_time;
				zend_stack_del_top(&self->call_time_stack); /* the caller */
				zend_stack_push(&self->call_time_stack, &parent_non_own_time, sizeof(parent_non_own_time)); /* add back the caller */
			}
		}
#endif
	}

	return zend_stack_del_top(&self->call_stack) == SUCCESS? PASS:FAIL;
}
/* }}} */


/* {{{ mysqlnd_debug::close */
static enum_func_status
MYSQLND_METHOD(mysqlnd_debug, close)(MYSQLND_DEBUG * self)
{
	MYSQLND_ZTS(self);
	if (self->stream) {
#ifndef MYSQLND_PROFILING_DISABLED
		if (!(self->flags & MYSQLND_DEBUG_FLUSH) && (self->flags & MYSQLND_DEBUG_PROFILE_CALLS)) {
			struct st_mysqlnd_dbg_function_profile * f_profile;
			HashPosition pos_values;

			self->m->log_va(self, __LINE__, __FILE__, 0, "info : ",	
					"number of functions: %d", zend_hash_num_elements(&self->function_profiles));
			zend_hash_internal_pointer_reset_ex(&self->function_profiles, &pos_values);
			while (zend_hash_get_current_data_ex(&self->function_profiles, (void **) &f_profile, &pos_values) == SUCCESS) {
				char	*string_key = NULL;
				uint	string_key_len;
				ulong	num_key;

				zend_hash_get_current_key_ex(&self->function_profiles, &string_key, &string_key_len, &num_key, 0, &pos_values);

				self->m->log_va(self, __LINE__, __FILE__, -1, "info : ",
						"%-40s\tcalls=%5llu  own_slow=%5llu  in_calls_slow=%5llu  total_slow=%5llu"
						"   min_own=%5llu  max_own=%7llu  avg_own=%7llu   "
						"   min_in_calls=%5llu  max_in_calls=%7llu  avg_in_calls=%7llu"
						"   min_total=%5llu  max_total=%7llu  avg_total=%7llu"
						,string_key
						,(uint64_t) f_profile->calls
						,(uint64_t) f_profile->own_underporm_calls
						,(uint64_t) f_profile->in_calls_underporm_calls
						,(uint64_t) f_profile->total_underporm_calls
						
						,(uint64_t) f_profile->min_own
						,(uint64_t) f_profile->max_own
						,(uint64_t) f_profile->avg_own
						,(uint64_t) f_profile->min_in_calls
						,(uint64_t) f_profile->max_in_calls
						,(uint64_t) f_profile->avg_in_calls
						,(uint64_t) f_profile->min_total
						,(uint64_t) f_profile->max_total
						,(uint64_t) f_profile->avg_total
						);
				zend_hash_move_forward_ex(&self->function_profiles, &pos_values);
			}
		}
#endif	

		php_stream_free(self->stream, PHP_STREAM_FREE_CLOSE);
		self->stream = NULL;
	}
	/* no DBG_RETURN please */
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
	zend_stack_destroy(&self->call_time_stack);
	zend_hash_destroy(&self->not_filtered_functions);
	zend_hash_destroy(&self->function_profiles);
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
	unsigned int mode_len = strlen(mode), i;
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
#ifdef PHP_WIN32
					if (i+4 < mode_len && mode[i+3] == ':' && (mode[i+4] == '\\' || mode[i+5] == '/')) {
						j = i + 5;
					}
#endif
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
								unsigned int func_name_len = MIN(sizeof(func_name) - 1, j - i - 1);
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
			case 'x': /* mysqlnd extension - profile calls */
				self->flags |= MYSQLND_DEBUG_PROFILE_CALLS;
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
PHPAPI MYSQLND_DEBUG *
mysqlnd_debug_init(const char * skip_functions[] TSRMLS_DC)
{
	MYSQLND_DEBUG *ret = ecalloc(1, sizeof(MYSQLND_DEBUG));
#ifdef ZTS
	ret->TSRMLS_C = TSRMLS_C;
#endif
	ret->nest_level_limit = 0;
	ret->pid = getpid();
	zend_stack_init(&ret->call_stack);
	zend_stack_init(&ret->call_time_stack);
	zend_hash_init(&ret->not_filtered_functions, 0, NULL, NULL, 0);
	zend_hash_init(&ret->function_profiles, 0, NULL, NULL, 0);

	ret->m = & mysqlnd_mysqlnd_debug_methods;
	ret->skip_functions = skip_functions;

	return ret;
}
/* }}} */


/* {{{ _mysqlnd_debug */
PHPAPI void _mysqlnd_debug(const char * mode TSRMLS_DC)
{
#ifdef PHP_DEBUG
	MYSQLND_DEBUG *dbg = MYSQLND_G(dbg);
	if (!dbg) {
		MYSQLND_G(dbg) = dbg = mysqlnd_debug_init(mysqlnd_debug_std_no_trace_funcs TSRMLS_CC);
		if (!dbg) {
			return;
		}
	}

	dbg->m->close(dbg);
	dbg->m->set_mode(dbg, mode);
	while (zend_stack_count(&dbg->call_stack)) {
		zend_stack_del_top(&dbg->call_stack);
	}
	while (zend_stack_count(&dbg->call_time_stack)) {
		zend_stack_del_top(&dbg->call_time_stack);
	}
#endif
}
/* }}} */


#if ZEND_DEBUG
#else
#define __zend_filename "/unknown/unknown"
#define __zend_lineno   0
#endif

#define REAL_SIZE(s) (collect_memory_statistics? (s) + sizeof(size_t) : (s))
#define REAL_PTR(p) (collect_memory_statistics && (p)? (((char *)(p)) - sizeof(size_t)) : (p))
#define FAKE_PTR(p) (collect_memory_statistics && (p)? (((char *)(p)) + sizeof(size_t)) : (p))

/* {{{ _mysqlnd_emalloc */
void * _mysqlnd_emalloc(size_t size MYSQLND_MEM_D)
{
	void *ret;
	zend_bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	long * threshold = &MYSQLND_G(debug_emalloc_fail_threshold);
	DBG_ENTER(mysqlnd_emalloc_name);

	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);

#ifdef PHP_DEBUG
	/* -1 is also "true" */
	if (*threshold) {
#endif
		ret = emalloc(REAL_SIZE(size));
#ifdef PHP_DEBUG
		--*threshold;
	} else if (*threshold == 0) {
		ret = NULL;
	}
#endif

	DBG_INF_FMT("size=%lu ptr=%p", size, ret);

	if (ret && collect_memory_statistics) {
		*(size_t *) ret = size;
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(STAT_MEM_EMALLOC_COUNT, 1, STAT_MEM_EMALLOC_AMOUNT, size);
	}
	DBG_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_pemalloc */
void * _mysqlnd_pemalloc(size_t size, zend_bool persistent MYSQLND_MEM_D)
{
	void *ret;
	zend_bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	long * threshold = persistent? &MYSQLND_G(debug_malloc_fail_threshold):&MYSQLND_G(debug_emalloc_fail_threshold);
	DBG_ENTER(mysqlnd_pemalloc_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);

#ifdef PHP_DEBUG
	/* -1 is also "true" */
	if (*threshold) {
#endif
		ret = pemalloc(REAL_SIZE(size), persistent);
#ifdef PHP_DEBUG
		--*threshold;
	} else if (*threshold == 0) {
		ret = NULL;
	}
#endif

	DBG_INF_FMT("size=%lu ptr=%p persistent=%u", size, ret, persistent);

	if (ret && collect_memory_statistics) {
		enum mysqlnd_collected_stats s1 = persistent? STAT_MEM_MALLOC_COUNT:STAT_MEM_EMALLOC_COUNT;
		enum mysqlnd_collected_stats s2 = persistent? STAT_MEM_MALLOC_AMOUNT:STAT_MEM_EMALLOC_AMOUNT;
		*(size_t *) ret = size;
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(s1, 1, s2, size);
	}

	DBG_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_ecalloc */
void * _mysqlnd_ecalloc(unsigned int nmemb, size_t size MYSQLND_MEM_D)
{
	void *ret;
	zend_bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	long * threshold = &MYSQLND_G(debug_ecalloc_fail_threshold);
	DBG_ENTER(mysqlnd_ecalloc_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("before: %lu", zend_memory_usage(FALSE TSRMLS_CC));

#ifdef PHP_DEBUG
	/* -1 is also "true" */
	if (*threshold) {
#endif
		ret = ecalloc(nmemb, REAL_SIZE(size));
#ifdef PHP_DEBUG
		--*threshold;
	} else if (*threshold == 0) {
		ret = NULL;
	}
#endif

	DBG_INF_FMT("after : %lu", zend_memory_usage(FALSE TSRMLS_CC));
	DBG_INF_FMT("size=%lu ptr=%p", size, ret);
	if (ret && collect_memory_statistics) {
		*(size_t *) ret = size;
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(STAT_MEM_ECALLOC_COUNT, 1, STAT_MEM_ECALLOC_AMOUNT, size);
	}
	DBG_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_pecalloc */
void * _mysqlnd_pecalloc(unsigned int nmemb, size_t size, zend_bool persistent MYSQLND_MEM_D)
{
	void *ret;
	zend_bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	long * threshold = persistent? &MYSQLND_G(debug_calloc_fail_threshold):&MYSQLND_G(debug_ecalloc_fail_threshold);
	DBG_ENTER(mysqlnd_pecalloc_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);

#ifdef PHP_DEBUG
	/* -1 is also "true" */
	if (*threshold) {
#endif
		ret = pecalloc(nmemb, REAL_SIZE(size), persistent);
#ifdef PHP_DEBUG
		--*threshold;
	} else if (*threshold == 0) {
		ret = NULL;
	}
#endif

	DBG_INF_FMT("size=%lu ptr=%p", size, ret);

	if (ret && collect_memory_statistics) {
		enum mysqlnd_collected_stats s1 = persistent? STAT_MEM_CALLOC_COUNT:STAT_MEM_ECALLOC_COUNT;
		enum mysqlnd_collected_stats s2 = persistent? STAT_MEM_CALLOC_AMOUNT:STAT_MEM_ECALLOC_AMOUNT;
		*(size_t *) ret = size;
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(s1, 1, s2, size);
	}

	DBG_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_erealloc */
void * _mysqlnd_erealloc(void *ptr, size_t new_size MYSQLND_MEM_D)
{
	void *ret;
	zend_bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	size_t old_size = collect_memory_statistics && ptr? *(size_t *) (((char*)ptr) - sizeof(size_t)) : 0;
	long * threshold = &MYSQLND_G(debug_erealloc_fail_threshold);
	DBG_ENTER(mysqlnd_erealloc_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("ptr=%p old_size=%lu, new_size=%lu", ptr, old_size, new_size); 

#ifdef PHP_DEBUG
	/* -1 is also "true" */
	if (*threshold) {
#endif
		ret = erealloc(REAL_PTR(ptr), REAL_SIZE(new_size));
#ifdef PHP_DEBUG
		--*threshold;
	} else if (*threshold == 0) {
		ret = NULL;
	}
#endif

	DBG_INF_FMT("new_ptr=%p", (char*)ret);
	if (ret && collect_memory_statistics) {
		*(size_t *) ret = new_size;
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(STAT_MEM_EREALLOC_COUNT, 1, STAT_MEM_EREALLOC_AMOUNT, new_size);
	}
	DBG_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_perealloc */
void * _mysqlnd_perealloc(void *ptr, size_t new_size, zend_bool persistent MYSQLND_MEM_D)
{
	void *ret;
	zend_bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	size_t old_size = collect_memory_statistics && ptr? *(size_t *) (((char*)ptr) - sizeof(size_t)) : 0;
	long * threshold = persistent? &MYSQLND_G(debug_realloc_fail_threshold):&MYSQLND_G(debug_erealloc_fail_threshold);
	DBG_ENTER(mysqlnd_perealloc_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("ptr=%p old_size=%lu new_size=%lu persistent=%u", ptr, old_size, new_size, persistent); 

#ifdef PHP_DEBUG
	/* -1 is also "true" */
	if (*threshold) {
#endif
		ret = perealloc(REAL_PTR(ptr), REAL_SIZE(new_size), persistent);
#ifdef PHP_DEBUG
		--*threshold;
	} else if (*threshold == 0) {
		ret = NULL;
	}
#endif

	DBG_INF_FMT("new_ptr=%p", (char*)ret);

	if (ret && collect_memory_statistics) {
		enum mysqlnd_collected_stats s1 = persistent? STAT_MEM_REALLOC_COUNT:STAT_MEM_EREALLOC_COUNT;
		enum mysqlnd_collected_stats s2 = persistent? STAT_MEM_REALLOC_AMOUNT:STAT_MEM_EREALLOC_AMOUNT;
		*(size_t *) ret = new_size;
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(s1, 1, s2, new_size);
	}
	DBG_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_efree */
void _mysqlnd_efree(void *ptr MYSQLND_MEM_D)
{
	size_t free_amount = 0;
	zend_bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	DBG_ENTER(mysqlnd_efree_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("ptr=%p", ptr); 

	if (ptr) {
		if (collect_memory_statistics) {
			free_amount = *(size_t *)(((char*)ptr) - sizeof(size_t));
			DBG_INF_FMT("ptr=%p size=%u", ((char*)ptr) - sizeof(size_t), (unsigned int) free_amount);
		}
		efree(REAL_PTR(ptr));
	}

	if (collect_memory_statistics) {
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(STAT_MEM_EFREE_COUNT, 1, STAT_MEM_EFREE_AMOUNT, free_amount);
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ _mysqlnd_pefree */
void _mysqlnd_pefree(void *ptr, zend_bool persistent MYSQLND_MEM_D)
{
	size_t free_amount = 0;
	zend_bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	DBG_ENTER(mysqlnd_pefree_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("ptr=%p persistent=%u", ptr, persistent); 

	if (ptr) {
		if (collect_memory_statistics) {
			free_amount = *(size_t *)(((char*)ptr) - sizeof(size_t));
			DBG_INF_FMT("ptr=%p size=%u", ((char*)ptr) - sizeof(size_t), (unsigned int) free_amount);
		}
		pefree(REAL_PTR(ptr), persistent);
	}

	if (collect_memory_statistics) {
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(persistent? STAT_MEM_FREE_COUNT:STAT_MEM_EFREE_COUNT, 1,
											  persistent? STAT_MEM_FREE_AMOUNT:STAT_MEM_EFREE_AMOUNT, free_amount);
	}
	DBG_VOID_RETURN;
}


/* {{{ _mysqlnd_malloc */
void * _mysqlnd_malloc(size_t size MYSQLND_MEM_D)
{
	void *ret;
	zend_bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	long * threshold = &MYSQLND_G(debug_malloc_fail_threshold);
	DBG_ENTER(mysqlnd_malloc_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);

#ifdef PHP_DEBUG
	/* -1 is also "true" */
	if (*threshold) {
#endif
		ret = malloc(REAL_SIZE(size));
#ifdef PHP_DEBUG
		--*threshold;
	} else if (*threshold == 0) {
		ret = NULL;
	}
#endif

	DBG_INF_FMT("size=%lu ptr=%p", size, ret);
	if (ret && collect_memory_statistics) {
		*(size_t *) ret = size;
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(STAT_MEM_MALLOC_COUNT, 1, STAT_MEM_MALLOC_AMOUNT, size);
	}
	DBG_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_calloc */
void * _mysqlnd_calloc(unsigned int nmemb, size_t size MYSQLND_MEM_D)
{
	void *ret;
	zend_bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	long * threshold = &MYSQLND_G(debug_calloc_fail_threshold);
	DBG_ENTER(mysqlnd_calloc_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);

#ifdef PHP_DEBUG
	/* -1 is also "true" */
	if (*threshold) {
#endif
		ret = calloc(nmemb, REAL_SIZE(size));
#ifdef PHP_DEBUG
		--*threshold;
	} else if (*threshold == 0) {
		ret = NULL;
	}
#endif

	DBG_INF_FMT("size=%lu ptr=%p", size, ret);
	if (ret && collect_memory_statistics) {
		*(size_t *) ret = size;
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(STAT_MEM_CALLOC_COUNT, 1, STAT_MEM_CALLOC_AMOUNT, size);
	}
	DBG_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_realloc */
void * _mysqlnd_realloc(void *ptr, size_t new_size MYSQLND_MEM_D)
{
	void *ret;
	zend_bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	long * threshold = &MYSQLND_G(debug_realloc_fail_threshold);
	DBG_ENTER(mysqlnd_realloc_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("ptr=%p new_size=%lu ", new_size, ptr); 
	DBG_INF_FMT("before: %lu", zend_memory_usage(TRUE TSRMLS_CC));

#ifdef PHP_DEBUG
	/* -1 is also "true" */
	if (*threshold) {
#endif
		ret = realloc(REAL_PTR(ptr), REAL_SIZE(new_size));
#ifdef PHP_DEBUG
		--*threshold;
	} else if (*threshold == 0) {
		ret = NULL;
	}
#endif

	DBG_INF_FMT("new_ptr=%p", (char*)ret);

	if (ret && collect_memory_statistics) {
		*(size_t *) ret = new_size;
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(STAT_MEM_REALLOC_COUNT, 1, STAT_MEM_REALLOC_AMOUNT, new_size);
	}
	DBG_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_free */
void _mysqlnd_free(void *ptr MYSQLND_MEM_D)
{
	size_t free_amount = 0;
	zend_bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	DBG_ENTER(mysqlnd_free_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("ptr=%p", ptr); 

	if (ptr) {
		if (collect_memory_statistics) {
			free_amount = *(size_t *)(((char*)ptr) - sizeof(size_t));
			DBG_INF_FMT("ptr=%p size=%u", ((char*)ptr) - sizeof(size_t), (unsigned int) free_amount);
		}
		free(REAL_PTR(ptr));
	}

	if (collect_memory_statistics) {
		MYSQLND_INC_GLOBAL_STATISTIC_W_VALUE2(STAT_MEM_FREE_COUNT, 1, STAT_MEM_FREE_AMOUNT, free_amount);
	}
	DBG_VOID_RETURN;
}
/* }}} */

#define SMART_STR_START_SIZE 2048
#define SMART_STR_PREALLOC 512
#include "ext/standard/php_smart_str.h"


/* {{{ _mysqlnd_pestrndup */
char * _mysqlnd_pestrndup(const char * const ptr, size_t length, zend_bool persistent MYSQLND_MEM_D)
{
	char * ret;
	zend_bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	DBG_ENTER(mysqlnd_pestrndup_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("ptr=%p", ptr); 

	ret = pemalloc(REAL_SIZE(length) + 1, persistent);
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

	DBG_RETURN(FAKE_PTR(ret));
}
/* }}} */


/* {{{ _mysqlnd_pestrdup */
char * _mysqlnd_pestrdup(const char * const ptr, zend_bool persistent MYSQLND_MEM_D)
{
	char * ret;
	smart_str tmp_str = {0, 0, 0};
	const char * p = ptr;
	zend_bool collect_memory_statistics = MYSQLND_G(collect_memory_statistics);
	DBG_ENTER(mysqlnd_pestrdup_name);
	DBG_INF_FMT("file=%-15s line=%4d", strrchr(__zend_filename, PHP_DIR_SEPARATOR) + 1, __zend_lineno);
	DBG_INF_FMT("ptr=%p", ptr);
	do {
		smart_str_appendc(&tmp_str, *p);
	} while (*p++);

	ret = pemalloc(tmp_str.len + sizeof(size_t), persistent);
	memcpy(FAKE_PTR(ret), tmp_str.c, tmp_str.len);

	if (ret && collect_memory_statistics) {
		*(size_t *) ret = tmp_str.len;
		MYSQLND_INC_GLOBAL_STATISTIC(persistent? STAT_MEM_STRDUP_COUNT : STAT_MEM_ESTRDUP_COUNT);
	}
	smart_str_free(&tmp_str);

	DBG_RETURN(FAKE_PTR(ret));
}
/* }}} */

#define MYSQLND_DEBUG_MEMORY 1

#if MYSQLND_DEBUG_MEMORY == 0

/* {{{ mysqlnd_zend_mm_emalloc */
static void * mysqlnd_zend_mm_emalloc(size_t size MYSQLND_MEM_D)
{
	return emalloc(size);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_pemalloc */
static void * mysqlnd_zend_mm_pemalloc(size_t size, zend_bool persistent MYSQLND_MEM_D)
{
	return pemalloc(size, persistent);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_ecalloc */
static void * mysqlnd_zend_mm_ecalloc(unsigned int nmemb, size_t size MYSQLND_MEM_D)
{
	return ecalloc(nmemb, size);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_pecalloc */
static void * mysqlnd_zend_mm_pecalloc(unsigned int nmemb, size_t size, zend_bool persistent MYSQLND_MEM_D)
{
	return pecalloc(nmemb, size, persistent);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_erealloc */
static void * mysqlnd_zend_mm_erealloc(void *ptr, size_t new_size MYSQLND_MEM_D)
{
	return erealloc(ptr, new_size);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_perealloc */
static void * mysqlnd_zend_mm_perealloc(void *ptr, size_t new_size, zend_bool persistent MYSQLND_MEM_D)
{
	return perealloc(ptr, new_size, persistent);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_efree */
static void mysqlnd_zend_mm_efree(void * ptr MYSQLND_MEM_D)
{
	efree(ptr);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_pefree */
static void mysqlnd_zend_mm_pefree(void * ptr, zend_bool persistent MYSQLND_MEM_D)
{
	pefree(ptr, persistent);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_malloc */
static void * mysqlnd_zend_mm_malloc(size_t size MYSQLND_MEM_D)
{
	return malloc(size);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_calloc */
static void * mysqlnd_zend_mm_calloc(unsigned int nmemb, size_t size MYSQLND_MEM_D)
{
	return calloc(nmemb, size);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_realloc */
static void * mysqlnd_zend_mm_realloc(void * ptr, size_t new_size MYSQLND_MEM_D)
{
	return realloc(ptr, new_size);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_free */
static void mysqlnd_zend_mm_free(void * ptr MYSQLND_MEM_D)
{
	free(ptr);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_pestrndup */
static char * mysqlnd_zend_mm_pestrndup(const char * const ptr, size_t length, zend_bool persistent MYSQLND_MEM_D)
{
	return pestrndup(ptr, length, persistent);
}
/* }}} */


/* {{{ mysqlnd_zend_mm_pestrdup */
static char * mysqlnd_zend_mm_pestrdup(const char * const ptr, zend_bool persistent MYSQLND_MEM_D)
{
	return pestrdup(ptr, persistent);
}
/* }}} */

#endif


PHPAPI struct st_mysqlnd_allocator_methods mysqlnd_allocator = 
{
#if MYSQLND_DEBUG_MEMORY
	_mysqlnd_emalloc,
	_mysqlnd_pemalloc,
	_mysqlnd_ecalloc,
	_mysqlnd_pecalloc,
	_mysqlnd_erealloc,
	_mysqlnd_perealloc,
	_mysqlnd_efree,
	_mysqlnd_pefree,
	_mysqlnd_malloc,
	_mysqlnd_calloc,
	_mysqlnd_realloc,
	_mysqlnd_free,
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
	mysqlnd_zend_mm_malloc,
	mysqlnd_zend_mm_calloc,
	mysqlnd_zend_mm_realloc,
	mysqlnd_zend_mm_free,
	mysqlnd_zend_mm_pestrndup,
	mysqlnd_zend_mm_pestrdup
#endif
};



/* Follows code borrowed from zend_builtin_functions.c because the functions there are static */

#if MYSQLND_UNICODE
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
static int mysqlnd_build_trace_args(zval **arg TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key)
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
			zval tmp;
			zstr class_name;
			zend_uint class_name_len;
			int dup;

			TRACE_APPEND_STR("Object(");

			dup = zend_get_object_classname(*arg, &class_name, &class_name_len TSRMLS_CC);

			ZVAL_UNICODEL(&tmp, class_name.u, class_name_len, 1);
			convert_to_string_with_converter(&tmp, ZEND_U_CONVERTER(UG(output_encoding_conv)));
			TRACE_APPEND_STRL(Z_STRVAL(tmp), Z_STRLEN(tmp));
			zval_dtor(&tmp);

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


static int mysqlnd_build_trace_string(zval **frame TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	char *s_tmp, **str;
	int *len, *num;
	long line;
	HashTable *ht = Z_ARRVAL_PP(frame);
	zval **file, **tmp;
	uint * level;

	level = va_arg(args, uint *);
	str = va_arg(args, char**);
	len = va_arg(args, int*);
	num = va_arg(args, int*);

	if (!*level) {
		return ZEND_HASH_APPLY_KEEP;
	}
	--*level;

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
		zend_hash_apply_with_arguments(Z_ARRVAL_PP(tmp) TSRMLS_CC, (apply_func_args_t)mysqlnd_build_trace_args, 2, str, len);
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


static int mysqlnd_build_trace_args(zval **arg TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
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
			int dupl;

			TRACE_APPEND_STR("Object(");

			dupl = zend_get_object_classname(*arg, &class_name, &class_name_len TSRMLS_CC);

			TRACE_APPEND_STRL(class_name, class_name_len);
			if (!dupl) {
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

static int mysqlnd_build_trace_string(zval **frame TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	char *s_tmp, **str;
	int *len, *num;
	long line;
	HashTable *ht = Z_ARRVAL_PP(frame);
	zval **file, **tmp;
	uint * level;

	level = va_arg(args, uint *);
	str = va_arg(args, char**);
	len = va_arg(args, int*);
	num = va_arg(args, int*);

	if (!*level) {
		return ZEND_HASH_APPLY_KEEP;
	}
	--*level;

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
		zend_hash_apply_with_arguments(Z_ARRVAL_PP(tmp) TSRMLS_CC, (apply_func_args_t)mysqlnd_build_trace_args, 2, str, len);
		if (last_len != *len) {
			*len -= 2; /* remove last ', ' */
		}
	}
	TRACE_APPEND_STR(")\n");
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */
#endif


PHPAPI char * mysqlnd_get_backtrace(uint max_levels, size_t * length TSRMLS_DC)
{
	zval *trace;
	char *res = estrdup(""), **str = &res, *s_tmp;
	int res_len = 0, *len = &res_len, num = 0;
	if (max_levels == 0) {
		max_levels = 99999;
	}

	MAKE_STD_ZVAL(trace);
	zend_fetch_debug_backtrace(trace, 0, 0 TSRMLS_CC);

	zend_hash_apply_with_arguments(Z_ARRVAL_P(trace) TSRMLS_CC, (apply_func_args_t)mysqlnd_build_trace_string, 4, &max_levels, str, len, &num);
	zval_ptr_dtor(&trace);

	if (max_levels) {
		s_tmp = emalloc(1 + MAX_LENGTH_OF_LONG + 7 + 1);
		sprintf(s_tmp, "#%d {main}", num);
		TRACE_APPEND_STRL(s_tmp, strlen(s_tmp));
		efree(s_tmp);
	}

	res[res_len] = '\0';
	*length = res_len;

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
