/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2016 The PHP Group                                |
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

static const char * const mysqlnd_debug_default_trace_file = "/tmp/mysqlnd.trace";

#ifdef ZTS 
#define MYSQLND_ZTS(self) TSRMLS_D = (self)->TSRMLS_C
#else
#define MYSQLND_ZTS(self)
#endif


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

	message_line_len = mnd_sprintf(&message_line, 0, "%s%s%s%s%s%s%s%s\n",
								flags & MYSQLND_DEBUG_DUMP_PID? pid_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_TIME? time_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_FILE? file_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_LINE? line_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_LEVEL? level_buffer:"",
								pipe_buffer, type? type:"", message);

	ret = php_stream_write(self->stream, message_line, message_line_len)? PASS:FAIL;
	mnd_sprintf_free(message_line);
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
	mnd_vsprintf(&buffer, 0, format, args);
	va_end(args);

	message_line_len = mnd_sprintf(&message_line, 0, "%s%s%s%s%s%s%s%s\n",
								flags & MYSQLND_DEBUG_DUMP_PID? pid_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_TIME? time_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_FILE? file_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_LINE? line_buffer:"",
								flags & MYSQLND_DEBUG_DUMP_LEVEL? level_buffer:"",
								pipe_buffer, type? type:"", buffer);
	mnd_sprintf_free(buffer);
	ret = php_stream_write(self->stream, message_line, message_line_len)? PASS:FAIL;
	mnd_sprintf_free(message_line);

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
	free(self);
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
	unsigned int mode_len, i;
	enum mysqlnd_debug_parser_state state = PARSER_WAIT_MODIFIER;

	mode_len = mode? strlen(mode) : 0;

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
	MYSQLND_DEBUG *ret = calloc(1, sizeof(MYSQLND_DEBUG));
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
#if PHP_DEBUG
	MYSQLND_DEBUG * dbg = MYSQLND_G(dbg);
	if (!dbg) {
		struct st_mysqlnd_plugin_trace_log * trace_log_plugin = mysqlnd_plugin_find("debug_trace");
		if (trace_log_plugin) {
			dbg = trace_log_plugin->methods.trace_instance_init(mysqlnd_debug_std_no_trace_funcs TSRMLS_CC);
			if (!dbg) {
				return;
			}
			MYSQLND_G(dbg) = dbg;
		}
	}
	if (dbg) {
		dbg->m->close(dbg);
		dbg->m->set_mode(dbg, mode);
		while (zend_stack_count(&dbg->call_stack)) {
			zend_stack_del_top(&dbg->call_stack);
		}
		while (zend_stack_count(&dbg->call_time_stack)) {
			zend_stack_del_top(&dbg->call_time_stack);
		}
	}
#endif
}
/* }}} */


static struct st_mysqlnd_plugin_trace_log mysqlnd_plugin_trace_log_plugin =
{
	{
		MYSQLND_PLUGIN_API_VERSION,
		"debug_trace",
		MYSQLND_VERSION_ID,
		MYSQLND_VERSION,
		"PHP License 3.01",
		"Andrey Hristov <andrey@mysql.com>,  Ulf Wendel <uwendel@mysql.com>, Georg Richter <georg@mysql.com>",
		{
			NULL, /* no statistics , will be filled later if there are some */
			NULL, /* no statistics */
		},
		{
			NULL /* plugin shutdown */
		}
	},
	{/* methods */
		mysqlnd_debug_init,
		mysqlnd_get_backtrace
	}
};


/* {{{ mysqlnd_debug_trace_plugin_register */
void
mysqlnd_debug_trace_plugin_register(TSRMLS_D)
{
	mysqlnd_plugin_register_ex((struct st_mysqlnd_plugin_header *) &mysqlnd_plugin_trace_log_plugin TSRMLS_CC);
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
