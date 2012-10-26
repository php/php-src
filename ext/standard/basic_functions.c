/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "php_streams.h"
#include "php_main.h"
#include "php_globals.h"
#include "php_ini.h"
#include "php_standard.h"
#include "php_math.h"
#include "php_http.h"
#include "php_incomplete_class.h"
#include "php_getopt.h"
#include "ext/standard/info.h"
#include "ext/session/php_session.h"
#include "zend_operators.h"
#include "ext/standard/php_dns.h"
#include "ext/standard/php_uuencode.h"

#ifdef PHP_WIN32
#include "win32/php_win32_globals.h"
#include "win32/time.h"
#endif

typedef struct yy_buffer_state *YY_BUFFER_STATE;

#include "zend.h"
#include "zend_ini_scanner.h"
#include "zend_language_scanner.h"
#include <zend_language_parser.h>

#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#ifndef PHP_WIN32
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef NETWARE
#include <netinet/in.h>
#endif

#ifndef PHP_WIN32
# include <netdb.h>
#else
#include "win32/inet.h"
#endif

#if HAVE_ARPA_INET_H
# include <arpa/inet.h>
#endif

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#if HAVE_LOCALE_H
# include <locale.h>
#endif

#if HAVE_SYS_MMAN_H
# include <sys/mman.h>
#endif

#if HAVE_SYS_LOADAVG_H
# include <sys/loadavg.h>
#endif

#ifdef PHP_WIN32
# include "win32/unistd.h"
#endif

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long int) -1)
#endif

#include "zend_globals.h"
#include "php_globals.h"
#include "SAPI.h"
#include "php_ticks.h"

#ifdef ZTS
PHPAPI int basic_globals_id;
#else
PHPAPI php_basic_globals basic_globals;
#endif

#include "php_fopen_wrappers.h"
#include "streamsfuncs.h"

static zend_class_entry *incomplete_class_entry = NULL;

typedef struct _user_tick_function_entry {
	zval **arguments;
	int arg_count;
	int calling;
} user_tick_function_entry;

/* some prototypes for local functions */
static void user_shutdown_function_dtor(php_shutdown_function_entry *shutdown_function_entry);
static void user_tick_function_dtor(user_tick_function_entry *tick_function_entry);

#undef sprintf

/* {{{ arginfo */
/* {{{ main/main.c */
ZEND_BEGIN_ARG_INFO(arginfo_set_time_limit, 0)
	ZEND_ARG_INFO(0, seconds)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ main/sapi.c */
ZEND_BEGIN_ARG_INFO(arginfo_header_register_callback, 0)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ main/output.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_ob_start, 0, 0, 0)
	ZEND_ARG_INFO(0, user_function)
	ZEND_ARG_INFO(0, chunk_size)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ob_flush, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ob_clean, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ob_end_flush, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ob_end_clean, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ob_get_flush, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ob_get_clean, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ob_get_contents, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ob_get_level, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ob_get_length, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ob_list_handlers, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ob_get_status, 0, 0, 0)
	ZEND_ARG_INFO(0, full_status)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ob_implicit_flush, 0, 0, 0)
	ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_output_reset_rewrite_vars, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_output_add_rewrite_var, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ main/streams/userspace.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_wrapper_register, 0, 0, 2)
	ZEND_ARG_INFO(0, protocol)
	ZEND_ARG_INFO(0, classname)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_wrapper_unregister, 0)
	ZEND_ARG_INFO(0, protocol)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_wrapper_restore, 0)
	ZEND_ARG_INFO(0, protocol)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ array.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_krsort, 0, 0, 1)
	ZEND_ARG_INFO(1, arg) /* ARRAY_INFO(1, arg, 0) */
	ZEND_ARG_INFO(0, sort_flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ksort, 0, 0, 1)
	ZEND_ARG_INFO(1, arg) /* ARRAY_INFO(1, arg, 0) */
	ZEND_ARG_INFO(0, sort_flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_count, 0, 0, 1)
	ZEND_ARG_INFO(0, var)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_natsort, 0)
	ZEND_ARG_INFO(1, arg) /* ARRAY_INFO(1, arg, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_natcasesort, 0)
	ZEND_ARG_INFO(1, arg) /* ARRAY_INFO(1, arg, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_asort, 0, 0, 1)
	ZEND_ARG_INFO(1, arg) /* ARRAY_INFO(1, arg, 0) */
	ZEND_ARG_INFO(0, sort_flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_arsort, 0, 0, 1)
	ZEND_ARG_INFO(1, arg) /* ARRAY_INFO(1, arg, 0) */
	ZEND_ARG_INFO(0, sort_flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sort, 0, 0, 1)
	ZEND_ARG_INFO(1, arg) /* ARRAY_INFO(1, arg, 0) */
	ZEND_ARG_INFO(0, sort_flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rsort, 0, 0, 1)
	ZEND_ARG_INFO(1, arg) /* ARRAY_INFO(1, arg, 0) */
	ZEND_ARG_INFO(0, sort_flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_usort, 0)
	ZEND_ARG_INFO(1, arg) /* ARRAY_INFO(1, arg, 0) */
	ZEND_ARG_INFO(0, cmp_function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_uasort, 0)
	ZEND_ARG_INFO(1, arg) /* ARRAY_INFO(1, arg, 0) */
	ZEND_ARG_INFO(0, cmp_function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_uksort, 0)
	ZEND_ARG_INFO(1, arg) /* ARRAY_INFO(1, arg, 0) */
	ZEND_ARG_INFO(0, cmp_function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_end, 0)
	ZEND_ARG_INFO(1, arg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_prev, 0)
	ZEND_ARG_INFO(1, arg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_next, 0)
	ZEND_ARG_INFO(1, arg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reset, 0)
	ZEND_ARG_INFO(1, arg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_current, ZEND_SEND_PREFER_REF)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, arg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_key, ZEND_SEND_PREFER_REF)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, arg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_min, 0, 0, 1)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_max, 0, 0, 1)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, arg2)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_walk, 0, 0, 2)
	ZEND_ARG_INFO(1, input) /* ARRAY_INFO(1, arg, 0) */
	ZEND_ARG_INFO(0, funcname)
	ZEND_ARG_INFO(0, userdata)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_walk_recursive, 0, 0, 2)
	ZEND_ARG_INFO(1, input) /* ARRAY_INFO(1, arg, 0) */
	ZEND_ARG_INFO(0, funcname)
	ZEND_ARG_INFO(0, userdata)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_in_array, 0, 0, 2)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, haystack) /* ARRAY_INFO(0, haystack, 0) */
	ZEND_ARG_INFO(0, strict)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_search, 0, 0, 2)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, haystack) /* ARRAY_INFO(0, haystack, 0) */
	ZEND_ARG_INFO(0, strict)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_extract, 0, 0, 1)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, arg) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, extract_type)
	ZEND_ARG_INFO(0, prefix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_compact, 0, 0, 1)
	ZEND_ARG_INFO(0, var_names)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_fill, 0)
	ZEND_ARG_INFO(0, start_key)
	ZEND_ARG_INFO(0, num)
	ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_fill_keys, 0)
	ZEND_ARG_INFO(0, keys) /* ARRAY_INFO(0, keys, 0) */
	ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_range, 0, 0, 2)
	ZEND_ARG_INFO(0, low)
	ZEND_ARG_INFO(0, high)
	ZEND_ARG_INFO(0, step)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_shuffle, 0)
	ZEND_ARG_INFO(1, arg) /* ARRAY_INFO(1, arg, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_push, 0, 0, 2)
	ZEND_ARG_INFO(1, stack) /* ARRAY_INFO(1, stack, 0) */
	ZEND_ARG_INFO(0, var)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_pop, 0)
	ZEND_ARG_INFO(1, stack) /* ARRAY_INFO(1, stack, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_shift, 0)
	ZEND_ARG_INFO(1, stack) /* ARRAY_INFO(1, stack, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_unshift, 0, 0, 2)
	ZEND_ARG_INFO(1, stack) /* ARRAY_INFO(1, stack, 0) */
	ZEND_ARG_INFO(0, var)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_splice, 0, 0, 2)
	ZEND_ARG_INFO(1, arg) /* ARRAY_INFO(1, arg, 0) */
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(0, replacement) /* ARRAY_INFO(0, arg, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_slice, 0, 0, 2)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(1, arg, 0) */
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(0, preserve_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_merge, 0, 0, 2)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, ...)  /* ARRAY_INFO(0, ..., 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_merge_recursive, 0, 0, 2)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, ...)  /* ARRAY_INFO(0, arg, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_replace, 0, 0, 2)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, ...)  /* ARRAY_INFO(0, ..., 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_replace_recursive, 0, 0, 2)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, ...)  /* ARRAY_INFO(0, arg, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_keys, 0, 0, 1)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, search_value)
	ZEND_ARG_INFO(0, strict)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_values, 0)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_count_values, 0)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_reverse, 0, 0, 1)
	ZEND_ARG_INFO(0, input) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, preserve_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_pad, 0)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, pad_size)
	ZEND_ARG_INFO(0, pad_value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_flip, 0)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_change_key_case, 0, 0, 1)
	ZEND_ARG_INFO(0, input) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, case)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_unique, 0)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_intersect_key, 0, 0, 2)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, ...)  /* ARRAY_INFO(0, ..., 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_intersect_ukey, 0)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, callback_key_compare_func)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_intersect, 0, 0, 2)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, ...)  /* ARRAY_INFO(0, ..., 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_uintersect, 0)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, callback_data_compare_func)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_intersect_assoc, 0, 0, 2)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, ...)  /* ARRAY_INFO(0, ..., 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_uintersect_assoc, 0)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, callback_data_compare_func)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_intersect_uassoc, 0)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, callback_key_compare_func)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_uintersect_uassoc, 0)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, callback_data_compare_func)
	ZEND_ARG_INFO(0, callback_key_compare_func)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_diff_key, 0, 0, 2)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, ...)  /* ARRAY_INFO(0, ..., 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_diff_ukey, 0)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, callback_key_comp_func)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_diff, 0, 0, 2)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, ...)  /* ARRAY_INFO(0, ..., 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_udiff, 0)
	ZEND_ARG_INFO(0, arr1)
	ZEND_ARG_INFO(0, arr2)
	ZEND_ARG_INFO(0, callback_data_comp_func)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_diff_assoc, 0, 0, 2)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, ...)  /* ARRAY_INFO(0, ..., 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_diff_uassoc, 0)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, callback_data_comp_func)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_udiff_assoc, 0)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, callback_key_comp_func)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_udiff_uassoc, 0)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, callback_data_comp_func)
	ZEND_ARG_INFO(0, callback_key_comp_func)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_multisort, ZEND_SEND_PREFER_REF, 0, 1)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, SORT_ASC_or_SORT_DESC)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, SORT_REGULAR_or_SORT_NUMERIC_or_SORT_STRING)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, arr2)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, SORT_ASC_or_SORT_DESC)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, SORT_REGULAR_or_SORT_NUMERIC_or_SORT_STRING)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_rand, 0, 0, 1)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, num_req)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_sum, 0)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_product, 0)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_reduce, 0, 0, 2)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, initial)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_filter, 0, 0, 1)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_map, 0, 0, 2)
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_key_exists, 0)
	ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, search)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_chunk, 0, 0, 2)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, size)
	ZEND_ARG_INFO(0, preserve_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_combine, 0)
	ZEND_ARG_INFO(0, keys)   /* ARRAY_INFO(0, keys, 0) */
	ZEND_ARG_INFO(0, values) /* ARRAY_INFO(0, values, 0) */
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ basic_functions.c */
ZEND_BEGIN_ARG_INFO(arginfo_get_magic_quotes_gpc, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_get_magic_quotes_runtime, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_set_magic_quotes_runtime, 0, 0, 1)
	ZEND_ARG_INFO(0, new_setting)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_constant, 0)
	ZEND_ARG_INFO(0, const_name)
ZEND_END_ARG_INFO()

#ifdef HAVE_INET_NTOP
ZEND_BEGIN_ARG_INFO(arginfo_inet_ntop, 0)
	ZEND_ARG_INFO(0, in_addr)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_INET_PTON
ZEND_BEGIN_ARG_INFO(arginfo_inet_pton, 0)
	ZEND_ARG_INFO(0, ip_address)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO(arginfo_ip2long, 0)
	ZEND_ARG_INFO(0, ip_address)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_long2ip, 0)
	ZEND_ARG_INFO(0, proper_address)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_getenv, 0)
	ZEND_ARG_INFO(0, varname)
ZEND_END_ARG_INFO()

#ifdef HAVE_PUTENV
ZEND_BEGIN_ARG_INFO(arginfo_putenv, 0)
	ZEND_ARG_INFO(0, setting)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_getopt, 0, 0, 1)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, opts) /* ARRAY_INFO(0, opts, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_flush, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_sleep, 0)
	ZEND_ARG_INFO(0, seconds)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_usleep, 0)
	ZEND_ARG_INFO(0, micro_seconds)
ZEND_END_ARG_INFO()

#if HAVE_NANOSLEEP
ZEND_BEGIN_ARG_INFO(arginfo_time_nanosleep, 0)
	ZEND_ARG_INFO(0, seconds)
	ZEND_ARG_INFO(0, nanoseconds)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_time_sleep_until, 0)
	ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO(arginfo_get_current_user, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_get_cfg_var, 0)
	ZEND_ARG_INFO(0, option_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_error_log, 0, 0, 1)
	ZEND_ARG_INFO(0, message)
	ZEND_ARG_INFO(0, message_type)
	ZEND_ARG_INFO(0, destination)
	ZEND_ARG_INFO(0, extra_headers)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_error_get_last, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_call_user_func, 0, 0, 1)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, parmeter)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_call_user_func_array, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, parameters) /* ARRAY_INFO(0, parameters, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_call_user_method, 0, 0, 2)
	ZEND_ARG_INFO(0, method_name)
	ZEND_ARG_INFO(1, object)
	ZEND_ARG_INFO(0, parameter)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_call_user_method_array, 0)
	ZEND_ARG_INFO(0, method_name)
	ZEND_ARG_INFO(1, object)
	ZEND_ARG_INFO(0, params) /* ARRAY_INFO(0, params, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_forward_static_call, 0, 0, 1)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, parameter)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_forward_static_call_array, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, parameters) /* ARRAY_INFO(0, parameters, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_register_shutdown_function, 0)
	ZEND_ARG_INFO(0, function_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_highlight_file, 0, 0, 1)
	ZEND_ARG_INFO(0, file_name)
	ZEND_ARG_INFO(0, return)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_php_strip_whitespace, 0)
	ZEND_ARG_INFO(0, file_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_highlight_string, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, return)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ini_get, 0)
	ZEND_ARG_INFO(0, varname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ini_get_all, 0, 0, 0)
	ZEND_ARG_INFO(0, extension)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ini_set, 0)
	ZEND_ARG_INFO(0, varname)
	ZEND_ARG_INFO(0, newvalue)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ini_restore, 0)
	ZEND_ARG_INFO(0, varname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_set_include_path, 0)
	ZEND_ARG_INFO(0, new_include_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_get_include_path, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_restore_include_path, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_print_r, 0, 0, 1)
	ZEND_ARG_INFO(0, var)
	ZEND_ARG_INFO(0, return)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_connection_aborted, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_connection_status, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ignore_user_abort, 0, 0, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

#if HAVE_GETSERVBYNAME
ZEND_BEGIN_ARG_INFO(arginfo_getservbyname, 0)
	ZEND_ARG_INFO(0, service)
	ZEND_ARG_INFO(0, protocol)
ZEND_END_ARG_INFO()
#endif

#if HAVE_GETSERVBYPORT
ZEND_BEGIN_ARG_INFO(arginfo_getservbyport, 0)
	ZEND_ARG_INFO(0, port)
	ZEND_ARG_INFO(0, protocol)
ZEND_END_ARG_INFO()
#endif

#if HAVE_GETPROTOBYNAME
ZEND_BEGIN_ARG_INFO(arginfo_getprotobyname, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
#endif

#if HAVE_GETPROTOBYNUMBER
ZEND_BEGIN_ARG_INFO(arginfo_getprotobynumber, 0)
	ZEND_ARG_INFO(0, proto)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_register_tick_function, 0, 0, 1)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, arg)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_unregister_tick_function, 0)
	ZEND_ARG_INFO(0, function_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_uploaded_file, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_move_uploaded_file, 0)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, new_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_parse_ini_file, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, process_sections)
	ZEND_ARG_INFO(0, scanner_mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_parse_ini_string, 0, 0, 1)
    ZEND_ARG_INFO(0, ini_string)
    ZEND_ARG_INFO(0, process_sections)
    ZEND_ARG_INFO(0, scanner_mode)
ZEND_END_ARG_INFO()

#if ZEND_DEBUG
ZEND_BEGIN_ARG_INFO(arginfo_config_get_hash, 0)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_GETLOADAVG
ZEND_BEGIN_ARG_INFO(arginfo_sys_getloadavg, 0)
ZEND_END_ARG_INFO()
#endif
/* }}} */
/* {{{ assert.c */
ZEND_BEGIN_ARG_INFO(arginfo_assert, 0)
	ZEND_ARG_INFO(0, assertion)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_assert_options, 0, 0, 1)
	ZEND_ARG_INFO(0, what)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ base64.c */
ZEND_BEGIN_ARG_INFO(arginfo_base64_encode, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_base64_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, strict)
ZEND_END_ARG_INFO()

/* }}} */
/* {{{ browscap.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_browser, 0, 0, 0)
	ZEND_ARG_INFO(0, browser_name)
	ZEND_ARG_INFO(0, return_array)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ crc32.c */
ZEND_BEGIN_ARG_INFO(arginfo_crc32, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

/* }}} */
/* {{{ crypt.c */
#if HAVE_CRYPT
ZEND_BEGIN_ARG_INFO_EX(arginfo_crypt, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, salt)
ZEND_END_ARG_INFO()
#endif
/* }}} */
/* {{{ cyr_convert.c */
ZEND_BEGIN_ARG_INFO(arginfo_convert_cyr_string, 0)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, from)
	ZEND_ARG_INFO(0, to)
ZEND_END_ARG_INFO()

/* }}} */
/* {{{ datetime.c */
#if HAVE_STRPTIME
ZEND_BEGIN_ARG_INFO(arginfo_strptime, 0)
	ZEND_ARG_INFO(0, timestamp)
	ZEND_ARG_INFO(0, format)
ZEND_END_ARG_INFO()
#endif
/* }}} */
/* {{{ dir.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_opendir, 0, 0, 1)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dir, 0, 0, 1)
	ZEND_ARG_INFO(0, directory)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_closedir, 0, 0, 0)
	ZEND_ARG_INFO(0, dir_handle)
ZEND_END_ARG_INFO()

#if defined(HAVE_CHROOT) && !defined(ZTS) && ENABLE_CHROOT_FUNC
ZEND_BEGIN_ARG_INFO(arginfo_chroot, 0)
	ZEND_ARG_INFO(0, directory)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO(arginfo_chdir, 0)
	ZEND_ARG_INFO(0, directory)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_getcwd, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rewinddir, 0, 0, 0)
	ZEND_ARG_INFO(0, dir_handle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_readdir, 0, 0, 0)
	ZEND_ARG_INFO(0, dir_handle)
ZEND_END_ARG_INFO()

#ifdef HAVE_GLOB
ZEND_BEGIN_ARG_INFO_EX(arginfo_glob, 0, 0, 1)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_scandir, 0, 0, 1)
	ZEND_ARG_INFO(0, dir)
	ZEND_ARG_INFO(0, sorting_order)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ arginfo ext/standard/dl.c */
ZEND_BEGIN_ARG_INFO(arginfo_dl, 0)
	ZEND_ARG_INFO(0, extension_filename)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ dns.c */
ZEND_BEGIN_ARG_INFO(arginfo_gethostbyaddr, 0)
	ZEND_ARG_INFO(0, ip_address)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gethostbyname, 0)
	ZEND_ARG_INFO(0, hostname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gethostbynamel, 0)
	ZEND_ARG_INFO(0, hostname)
ZEND_END_ARG_INFO()

#ifdef HAVE_GETHOSTNAME
ZEND_BEGIN_ARG_INFO(arginfo_gethostname, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(PHP_WIN32) || (HAVE_DNS_SEARCH_FUNC && !(defined(__BEOS__) || defined(NETWARE)))
ZEND_BEGIN_ARG_INFO_EX(arginfo_dns_check_record, 0, 0, 1)
	ZEND_ARG_INFO(0, host)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

# if defined(PHP_WIN32) || HAVE_FULL_DNS_FUNCS
ZEND_BEGIN_ARG_INFO_EX(arginfo_dns_get_record, 0, 0, 1)
	ZEND_ARG_INFO(0, hostname)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_ARRAY_INFO(1, authns, 1)
	ZEND_ARG_ARRAY_INFO(1, addtl, 1)
	ZEND_ARG_INFO(0, raw)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_dns_get_mx, 0, 0, 2)
	ZEND_ARG_INFO(0, hostname)
	ZEND_ARG_INFO(1, mxhosts) /* ARRAY_INFO(1, mxhosts, 1) */
	ZEND_ARG_INFO(1, weight) /* ARRAY_INFO(1, weight, 1) */
ZEND_END_ARG_INFO()
# endif

#endif /* defined(PHP_WIN32) || (HAVE_DNS_SEARCH_FUNC && !(defined(__BEOS__) || defined(NETWARE))) */
/* }}} */

/* {{{ exec.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_exec, 0, 0, 1)
	ZEND_ARG_INFO(0, command)
	ZEND_ARG_INFO(1, output) /* ARRAY_INFO(1, output, 1) */
	ZEND_ARG_INFO(1, return_value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_system, 0, 0, 1)
	ZEND_ARG_INFO(0, command)
	ZEND_ARG_INFO(1, return_value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_passthru, 0, 0, 1)
	ZEND_ARG_INFO(0, command)
	ZEND_ARG_INFO(1, return_value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_escapeshellcmd, 0)
	ZEND_ARG_INFO(0, command)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_escapeshellarg, 0)
	ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_shell_exec, 0)
	ZEND_ARG_INFO(0, cmd)
ZEND_END_ARG_INFO()

#ifdef HAVE_NICE
ZEND_BEGIN_ARG_INFO(arginfo_proc_nice, 0)
	ZEND_ARG_INFO(0, priority)
ZEND_END_ARG_INFO()
#endif
/* }}} */
/* {{{ file.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_flock, 0, 0, 2)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, operation)
	ZEND_ARG_INFO(1, wouldblock)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_meta_tags, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, use_include_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_file_get_contents, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, context)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, maxlen)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_file_put_contents, 0, 0, 2)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_file, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tempnam, 0)
	ZEND_ARG_INFO(0, dir)
	ZEND_ARG_INFO(0, prefix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tmpfile, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fopen, 0, 0, 2)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, use_include_path)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fclose, 0)
	ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_popen, 0)
	ZEND_ARG_INFO(0, command)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_pclose, 0)
	ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_feof, 0)
	ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fgets, 0, 0, 1)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fgetc, 0)
	ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fgetss, 0, 0, 1)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(0, allowable_tags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fscanf, 1, 0, 2)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(1, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fwrite, 0, 0, 2)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fflush, 0)
	ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rewind, 0)
	ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ftell, 0)
	ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fseek, 0, 0, 2)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, whence)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mkdir, 0, 0, 1)
	ZEND_ARG_INFO(0, pathname)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, recursive)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rmdir, 0, 0, 1)
	ZEND_ARG_INFO(0, dirname)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_readfile, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_umask, 0, 0, 0)
	ZEND_ARG_INFO(0, mask)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fpassthru, 0)
	ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rename, 0, 0, 2)
	ZEND_ARG_INFO(0, old_name)
	ZEND_ARG_INFO(0, new_name)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_unlink, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ftruncate, 0)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fstat, 0)
	ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO(arginfo_copy, 0)
	ZEND_ARG_INFO(0, source_file)
	ZEND_ARG_INFO(0, destination_file)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fread, 0)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fputcsv, 0, 0, 2)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, fields) /* ARRAY_INFO(0, fields, 1) */
	ZEND_ARG_INFO(0, delimiter)
	ZEND_ARG_INFO(0, enclosure)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fgetcsv, 0, 0, 1)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(0, delimiter)
	ZEND_ARG_INFO(0, enclosure)
	ZEND_ARG_INFO(0, escape)
ZEND_END_ARG_INFO()

#if (!defined(__BEOS__) && !defined(NETWARE) && HAVE_REALPATH) || defined(ZTS)
ZEND_BEGIN_ARG_INFO(arginfo_realpath, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_FNMATCH
ZEND_BEGIN_ARG_INFO_EX(arginfo_fnmatch, 0, 0, 2)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO(arginfo_sys_get_temp_dir, 0)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ filestat.c */
ZEND_BEGIN_ARG_INFO(arginfo_disk_total_space, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_disk_free_space, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

#ifndef NETWARE
ZEND_BEGIN_ARG_INFO(arginfo_chgrp, 0)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, group)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_chown, 0)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, user)
ZEND_END_ARG_INFO()
#endif

#if HAVE_LCHOWN
ZEND_BEGIN_ARG_INFO(arginfo_lchgrp, 0)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, group)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_lchown, 0)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, user)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO(arginfo_chmod, 0)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

#if HAVE_UTIME
ZEND_BEGIN_ARG_INFO_EX(arginfo_touch, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, time)
	ZEND_ARG_INFO(0, atime)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_clearstatcache, 0, 0, 0)
	ZEND_ARG_INFO(0, clear_realpath_cache)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_realpath_cache_size, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_realpath_cache_get, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fileperms, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fileinode, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_filesize, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fileowner, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_filegroup, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fileatime, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_filemtime, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_filectime, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_filetype, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_writable, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_readable, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_executable, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_file, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_dir, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_link, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_file_exists, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_lstat, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stat, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ formatted_print.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_sprintf, 0, 0, 2)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_vsprintf, 0)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, args) /* ARRAY_INFO(0, args, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_printf, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_vprintf, 0)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, args) /* ARRAY_INFO(0, args, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fprintf, 0, 0, 2)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_vfprintf, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, args) /* ARRAY_INFO(0, args, 1) */
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ fsock.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_fsockopen, 0, 0, 2)
	ZEND_ARG_INFO(0, hostname)
	ZEND_ARG_INFO(0, port)
	ZEND_ARG_INFO(1, errno)
	ZEND_ARG_INFO(1, errstr)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pfsockopen, 0, 0, 2)
	ZEND_ARG_INFO(0, hostname)
	ZEND_ARG_INFO(0, port)
	ZEND_ARG_INFO(1, errno)
	ZEND_ARG_INFO(1, errstr)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ ftok.c */
#if HAVE_FTOK
ZEND_BEGIN_ARG_INFO(arginfo_ftok, 0)
	ZEND_ARG_INFO(0, pathname)
	ZEND_ARG_INFO(0, proj)
ZEND_END_ARG_INFO()
#endif
/* }}} */
/* {{{ head.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_header, 0, 0, 1)
	ZEND_ARG_INFO(0, header)
	ZEND_ARG_INFO(0, replace)
	ZEND_ARG_INFO(0, http_response_code)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_header_remove, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_setcookie, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, expires)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, secure)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_setrawcookie, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, expires)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, secure)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_headers_sent, 0, 0, 0)
	ZEND_ARG_INFO(1, file)
	ZEND_ARG_INFO(1, line)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_headers_list, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_http_response_code, 0, 0, 0)
	ZEND_ARG_INFO(0, response_code)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ html.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_htmlspecialchars, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, quote_style)
	ZEND_ARG_INFO(0, charset)
	ZEND_ARG_INFO(0, double_encode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_htmlspecialchars_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, quote_style)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_html_entity_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, quote_style)
	ZEND_ARG_INFO(0, charset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_htmlentities, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, quote_style)
	ZEND_ARG_INFO(0, charset)
	ZEND_ARG_INFO(0, double_encode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_html_translation_table, 0, 0, 0)
	ZEND_ARG_INFO(0, table)
	ZEND_ARG_INFO(0, quote_style)
ZEND_END_ARG_INFO()

/* }}} */
/* {{{ http.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_http_build_query, 0, 0, 1)
	ZEND_ARG_INFO(0, formdata)
	ZEND_ARG_INFO(0, prefix)
	ZEND_ARG_INFO(0, arg_separator)
	ZEND_ARG_INFO(0, enc_type)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ image.c */
ZEND_BEGIN_ARG_INFO(arginfo_image_type_to_mime_type, 0)
	ZEND_ARG_INFO(0, imagetype)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_image_type_to_extension, 0, 0, 1)
	ZEND_ARG_INFO(0, imagetype)
	ZEND_ARG_INFO(0, include_dot)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_getimagesize, 0, 0, 1)
	ZEND_ARG_INFO(0, imagefile)
	ZEND_ARG_INFO(1, info) /* ARRAY_INFO(1, info, 1) */
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ info.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_phpinfo, 0, 0, 0)
	ZEND_ARG_INFO(0, what)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phpversion, 0, 0, 0)
	ZEND_ARG_INFO(0, extension)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_phpcredits, 0, 0, 0)
	ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_php_logo_guid, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_php_real_logo_guid, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_php_egg_logo_guid, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_zend_logo_guid, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_php_sapi_name, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_php_uname, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_php_ini_scanned_files, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_php_ini_loaded_file, 0)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ iptc.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_iptcembed, 0, 0, 2)
	ZEND_ARG_INFO(0, iptcdata)
	ZEND_ARG_INFO(0, jpeg_file_name)
	ZEND_ARG_INFO(0, spool)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_iptcparse, 0)
	ZEND_ARG_INFO(0, iptcdata)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ lcg.c */
ZEND_BEGIN_ARG_INFO(arginfo_lcg_value, 0)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ levenshtein.c */
ZEND_BEGIN_ARG_INFO(arginfo_levenshtein, 0)
	ZEND_ARG_INFO(0, str1)
	ZEND_ARG_INFO(0, str2)
	ZEND_ARG_INFO(0, cost_ins)
	ZEND_ARG_INFO(0, cost_rep)
	ZEND_ARG_INFO(0, cost_del)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ link.c */
#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
ZEND_BEGIN_ARG_INFO(arginfo_readlink, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_linkinfo, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_symlink, 0)
	ZEND_ARG_INFO(0, target)
	ZEND_ARG_INFO(0, link)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_link, 0)
	ZEND_ARG_INFO(0, target)
	ZEND_ARG_INFO(0, link)
ZEND_END_ARG_INFO()
#endif
/* }}} */
/* {{{ mail.c */
ZEND_BEGIN_ARG_INFO(arginfo_ezmlm_hash, 0)
	ZEND_ARG_INFO(0, addr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mail, 0, 0, 3)
	ZEND_ARG_INFO(0, to)
	ZEND_ARG_INFO(0, subject)
	ZEND_ARG_INFO(0, message)
	ZEND_ARG_INFO(0, additional_headers)
	ZEND_ARG_INFO(0, additional_parameters)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ math.c */
ZEND_BEGIN_ARG_INFO(arginfo_abs, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ceil, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_floor, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_round, 0, 0, 1)
	ZEND_ARG_INFO(0, number)
	ZEND_ARG_INFO(0, precision)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_sin, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_cos, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tan, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_asin, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_acos, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_atan, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_atan2, 0)
	ZEND_ARG_INFO(0, y)
	ZEND_ARG_INFO(0, x)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_sinh, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_cosh, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tanh, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_asinh, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_acosh, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_atanh, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_pi, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_finite, 0)
	ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_infinite, 0)
	ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_nan, 0)
	ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_pow, 0)
	ZEND_ARG_INFO(0, base)
	ZEND_ARG_INFO(0, exponent)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_exp, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_expm1, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_log1p, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_log, 0, 0, 1)
	ZEND_ARG_INFO(0, number)
	ZEND_ARG_INFO(0, base)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_log10, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_sqrt, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_hypot, 0)
	ZEND_ARG_INFO(0, num1)
	ZEND_ARG_INFO(0, num2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_deg2rad, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rad2deg, 0)
	ZEND_ARG_INFO(0, number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_bindec, 0)
	ZEND_ARG_INFO(0, binary_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_hexdec, 0)
	ZEND_ARG_INFO(0, hexadecimal_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_octdec, 0)
	ZEND_ARG_INFO(0, octal_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_decbin, 0)
	ZEND_ARG_INFO(0, decimal_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_decoct, 0)
	ZEND_ARG_INFO(0, decimal_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dechex, 0)
	ZEND_ARG_INFO(0, decimal_number)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_base_convert, 0)
	ZEND_ARG_INFO(0, number)
	ZEND_ARG_INFO(0, frombase)
	ZEND_ARG_INFO(0, tobase)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_number_format, 0, 0, 1)
	ZEND_ARG_INFO(0, number)
	ZEND_ARG_INFO(0, num_decimal_places)
	ZEND_ARG_INFO(0, dec_seperator)
	ZEND_ARG_INFO(0, thousands_seperator)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fmod, 0)
	ZEND_ARG_INFO(0, x)
	ZEND_ARG_INFO(0, y)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ md5.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_md5, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_md5_file, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ metaphone.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_metaphone, 0, 0, 1)
	ZEND_ARG_INFO(0, text)
	ZEND_ARG_INFO(0, phones)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ microtime.c */
#ifdef HAVE_GETTIMEOFDAY
ZEND_BEGIN_ARG_INFO_EX(arginfo_microtime, 0, 0, 0)
	ZEND_ARG_INFO(0, get_as_float)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_gettimeofday, 0, 0, 0)
	ZEND_ARG_INFO(0, get_as_float)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_GETRUSAGE
ZEND_BEGIN_ARG_INFO_EX(arginfo_getrusage, 0, 0, 0)
	ZEND_ARG_INFO(0, who)
ZEND_END_ARG_INFO()
#endif
/* }}} */
/* {{{ pack.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_pack, 0, 0, 2)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, arg1)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_unpack, 0)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, input)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ pageinfo.c */
ZEND_BEGIN_ARG_INFO(arginfo_getmyuid, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_getmygid, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_getmypid, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_getmyinode, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_getlastmod, 0)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ proc_open.c */
#ifdef PHP_CAN_SUPPORT_PROC_OPEN
ZEND_BEGIN_ARG_INFO_EX(arginfo_proc_terminate, 0, 0, 1)
	ZEND_ARG_INFO(0, process)
	ZEND_ARG_INFO(0, signal)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_proc_close, 0)
	ZEND_ARG_INFO(0, process)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_proc_get_status, 0)
	ZEND_ARG_INFO(0, process)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_proc_open, 0, 0, 3)
	ZEND_ARG_INFO(0, command)
	ZEND_ARG_INFO(0, descriptorspec) /* ARRAY_INFO(0, descriptorspec, 1) */
	ZEND_ARG_INFO(1, pipes) /* ARRAY_INFO(1, pipes, 1) */
	ZEND_ARG_INFO(0, cwd)
	ZEND_ARG_INFO(0, env) /* ARRAY_INFO(0, env, 1) */
	ZEND_ARG_INFO(0, other_options) /* ARRAY_INFO(0, other_options, 1) */
ZEND_END_ARG_INFO()
#endif
/* }}} */
/* {{{ quot_print.c */
ZEND_BEGIN_ARG_INFO(arginfo_quoted_printable_decode, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ quot_print.c */
ZEND_BEGIN_ARG_INFO(arginfo_quoted_printable_encode, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ rand.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_srand, 0, 0, 0)
	ZEND_ARG_INFO(0, seed)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mt_srand, 0, 0, 0)
	ZEND_ARG_INFO(0, seed)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rand, 0, 0, 0)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mt_rand, 0, 0, 0)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_getrandmax, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_mt_getrandmax, 0)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ sha1.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_sha1, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sha1_file, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, raw_output)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ soundex.c */
ZEND_BEGIN_ARG_INFO(arginfo_soundex, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ streamsfuncs.c */
#if HAVE_SOCKETPAIR
ZEND_BEGIN_ARG_INFO(arginfo_stream_socket_pair, 0)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, protocol)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_socket_client, 0, 0, 1)
	ZEND_ARG_INFO(0, remoteaddress)
	ZEND_ARG_INFO(1, errcode)
	ZEND_ARG_INFO(1, errstring)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_socket_server, 0, 0, 1)
	ZEND_ARG_INFO(0, localaddress)
	ZEND_ARG_INFO(1, errcode)
	ZEND_ARG_INFO(1, errstring)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_socket_accept, 0, 0, 1)
	ZEND_ARG_INFO(0, serverstream)
	ZEND_ARG_INFO(0, timeout)
	ZEND_ARG_INFO(1, peername)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_socket_get_name, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, want_peer)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_socket_sendto, 0, 0, 2)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(0, target_addr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_socket_recvfrom, 0, 0, 2)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, amount)
	ZEND_ARG_INFO(0, flags)
	ZEND_ARG_INFO(1, remote_addr)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_get_contents, 0, 0, 1)
	ZEND_ARG_INFO(0, source)
	ZEND_ARG_INFO(0, maxlen)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_copy_to_stream, 0, 0, 2)
	ZEND_ARG_INFO(0, source)
	ZEND_ARG_INFO(0, dest)
	ZEND_ARG_INFO(0, maxlen)
	ZEND_ARG_INFO(0, pos)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_get_meta_data, 0)
	ZEND_ARG_INFO(0, fp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_get_transports, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_get_wrappers, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_resolve_include_path, 0)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_is_local, 0)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_supports_lock, 0, 0, 1)
    ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_select, 0, 0, 4)
	ZEND_ARG_INFO(1, read_streams) /* ARRAY_INFO(1, read_streams, 1) */
	ZEND_ARG_INFO(1, write_streams) /* ARRAY_INFO(1, write_streams, 1) */
	ZEND_ARG_INFO(1, except_streams) /* ARRAY_INFO(1, except_streams, 1) */
	ZEND_ARG_INFO(0, tv_sec)
	ZEND_ARG_INFO(0, tv_usec)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_context_get_options, 0)
	ZEND_ARG_INFO(0, stream_or_context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_context_set_option, 0)
	ZEND_ARG_INFO(0, stream_or_context)
	ZEND_ARG_INFO(0, wrappername)
	ZEND_ARG_INFO(0, optionname)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_context_set_params, 0)
	ZEND_ARG_INFO(0, stream_or_context)
	ZEND_ARG_INFO(0, options) /* ARRAY_INFO(0, options, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_context_get_params, 0, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, stream_or_context)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_context_get_default, 0, 0, 0)
	ZEND_ARG_INFO(0, options) /* ARRAY_INFO(0, options, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_context_set_default, 0)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_context_create, 0, 0, 0)
	ZEND_ARG_INFO(0, options) /* ARRAY_INFO(0, options, 1) */
	ZEND_ARG_INFO(0, params) /* ARRAY_INFO(0, params, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_filter_prepend, 0, 0, 2)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, filtername)
	ZEND_ARG_INFO(0, read_write)
	ZEND_ARG_INFO(0, filterparams)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_filter_append, 0, 0, 2)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, filtername)
	ZEND_ARG_INFO(0, read_write)
	ZEND_ARG_INFO(0, filterparams)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_filter_remove, 0)
	ZEND_ARG_INFO(0, stream_filter)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_get_line, 0, 0, 2)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, maxlen)
	ZEND_ARG_INFO(0, ending)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_set_blocking, 0)
	ZEND_ARG_INFO(0, socket)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

#if HAVE_SYS_TIME_H || defined(PHP_WIN32)
ZEND_BEGIN_ARG_INFO(arginfo_stream_set_timeout, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, seconds)
	ZEND_ARG_INFO(0, microseconds)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO(arginfo_stream_set_read_buffer, 0)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, buffer)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_set_write_buffer, 0)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, buffer)
ZEND_END_ARG_INFO()
		
ZEND_BEGIN_ARG_INFO(arginfo_stream_set_chunk_size, 0)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, chunk_size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_socket_enable_crypto, 0, 0, 2)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, enable)
	ZEND_ARG_INFO(0, cryptokind)
	ZEND_ARG_INFO(0, sessionstream)
ZEND_END_ARG_INFO()

#ifdef HAVE_SHUTDOWN
ZEND_BEGIN_ARG_INFO(arginfo_stream_socket_shutdown, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, how)
ZEND_END_ARG_INFO()
#endif
/* }}} */
/* {{{ string.c */
ZEND_BEGIN_ARG_INFO(arginfo_bin2hex, 0)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_hex2bin, 0)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_strspn, 0, 0, 2)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, mask)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, len)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_strcspn, 0, 0, 2)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, mask)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, len)
ZEND_END_ARG_INFO()

#if HAVE_NL_LANGINFO
ZEND_BEGIN_ARG_INFO(arginfo_nl_langinfo, 0)
	ZEND_ARG_INFO(0, item)
ZEND_END_ARG_INFO()
#endif

#ifdef HAVE_STRCOLL
ZEND_BEGIN_ARG_INFO(arginfo_strcoll, 0)
	ZEND_ARG_INFO(0, str1)
	ZEND_ARG_INFO(0, str2)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_trim, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, character_mask)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rtrim, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, character_mask)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ltrim, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, character_mask)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_wordwrap, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, width)
	ZEND_ARG_INFO(0, break)
	ZEND_ARG_INFO(0, cut)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_explode, 0, 0, 2)
	ZEND_ARG_INFO(0, separator)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, limit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_implode, 0)
	ZEND_ARG_INFO(0, glue)
	ZEND_ARG_INFO(0, pieces)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_strtok, 0)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, token)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_strtoupper, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_strtolower, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_basename, 0, 0, 1)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, suffix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dirname, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pathinfo, 0, 0, 1)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stristr, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, part)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_strstr, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, part)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_strpos, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_stripos, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_strrpos, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_strripos, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_strrchr, 0)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_chunk_split, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, chunklen)
	ZEND_ARG_INFO(0, ending)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_substr, 0, 0, 2)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_substr_replace, 0, 0, 3)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, replace)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_quotemeta, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ord, 0)
	ZEND_ARG_INFO(0, character)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_chr, 0)
	ZEND_ARG_INFO(0, codepoint)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ucfirst, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_lcfirst, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()
	
ZEND_BEGIN_ARG_INFO(arginfo_ucwords, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_strtr, 0, 0, 2)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, from)
	ZEND_ARG_INFO(0, to)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_strrev, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_similar_text, 0, 0, 2)
	ZEND_ARG_INFO(0, str1)
	ZEND_ARG_INFO(0, str2)
	ZEND_ARG_INFO(1, percent)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_addcslashes, 0)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, charlist)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_addslashes, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stripcslashes, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stripslashes, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_str_replace, 0, 0, 3)
	ZEND_ARG_INFO(0, search)
	ZEND_ARG_INFO(0, replace)
	ZEND_ARG_INFO(0, subject)
	ZEND_ARG_INFO(1, replace_count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_str_ireplace, 0, 0, 3)
	ZEND_ARG_INFO(0, search)
	ZEND_ARG_INFO(0, replace)
	ZEND_ARG_INFO(0, subject)
	ZEND_ARG_INFO(1, replace_count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hebrev, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, max_chars_per_line)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_hebrevc, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, max_chars_per_line)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_nl2br, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, is_xhtml)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_strip_tags, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, allowable_tags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_setlocale, 0, 0, 2)
	ZEND_ARG_INFO(0, category)
	ZEND_ARG_INFO(0, locale)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_parse_str, 0, 0, 1)
	ZEND_ARG_INFO(0, encoded_string)
	ZEND_ARG_INFO(1, result)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_str_getcsv, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, delimiter)
	ZEND_ARG_INFO(0, enclosure)
	ZEND_ARG_INFO(0, escape)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_str_repeat, 0)
	ZEND_ARG_INFO(0, input)
	ZEND_ARG_INFO(0, mult)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_count_chars, 0, 0, 1)
	ZEND_ARG_INFO(0, input)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_strnatcmp, 0)
	ZEND_ARG_INFO(0, s1)
	ZEND_ARG_INFO(0, s2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_localeconv, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_strnatcasecmp, 0)
	ZEND_ARG_INFO(0, s1)
	ZEND_ARG_INFO(0, s2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_substr_count, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_str_pad, 0, 0, 2)
	ZEND_ARG_INFO(0, input)
	ZEND_ARG_INFO(0, pad_length)
	ZEND_ARG_INFO(0, pad_string)
	ZEND_ARG_INFO(0, pad_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sscanf, 1, 0, 2)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(1, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_str_rot13, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_str_shuffle, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_str_word_count, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, charlist)
ZEND_END_ARG_INFO()

#ifdef HAVE_STRFMON
ZEND_BEGIN_ARG_INFO(arginfo_money_format, 0)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_str_split, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, split_length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_strpbrk, 0, 0, 1)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, char_list)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_substr_compare, 0, 0, 3)
	ZEND_ARG_INFO(0, main_str)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(0, case_sensitivity)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ syslog.c */
#ifdef HAVE_SYSLOG_H
ZEND_BEGIN_ARG_INFO(arginfo_openlog, 0)
	ZEND_ARG_INFO(0, ident)
	ZEND_ARG_INFO(0, option)
	ZEND_ARG_INFO(0, facility)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_closelog, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_syslog, 0)
	ZEND_ARG_INFO(0, priority)
	ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()
#endif
/* }}} */
/* {{{ type.c */
ZEND_BEGIN_ARG_INFO(arginfo_gettype, 0)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_settype, 0)
	ZEND_ARG_INFO(1, var)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_intval, 0, 0, 1)
	ZEND_ARG_INFO(0, var)
	ZEND_ARG_INFO(0, base)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_floatval, 0)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_strval, 0)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_null, 0)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_resource, 0)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_bool, 0)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_long, 0)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_float, 0)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_string, 0)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_array, 0)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_object, 0)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_numeric, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_scalar, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_is_callable, 0, 0, 1)
	ZEND_ARG_INFO(0, var)
	ZEND_ARG_INFO(0, syntax_only)
	ZEND_ARG_INFO(1, callable_name)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ uniqid.c */
#ifdef HAVE_GETTIMEOFDAY
ZEND_BEGIN_ARG_INFO_EX(arginfo_uniqid, 0, 0, 0)
	ZEND_ARG_INFO(0, prefix)
	ZEND_ARG_INFO(0, more_entropy)
ZEND_END_ARG_INFO()
#endif
/* }}} */
/* {{{ url.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_parse_url, 0, 0, 1)
	ZEND_ARG_INFO(0, url)
	ZEND_ARG_INFO(0, component)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_urlencode, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_urldecode, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rawurlencode, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_rawurldecode, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_headers, 0, 0, 1)
	ZEND_ARG_INFO(0, url)
	ZEND_ARG_INFO(0, format)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ user_filters.c */
ZEND_BEGIN_ARG_INFO(arginfo_stream_bucket_make_writeable, 0)
	ZEND_ARG_INFO(0, brigade)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_bucket_prepend, 0)
	ZEND_ARG_INFO(0, brigade)
	ZEND_ARG_INFO(0, bucket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_bucket_append, 0)
	ZEND_ARG_INFO(0, brigade)
	ZEND_ARG_INFO(0, bucket)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_bucket_new, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, buffer)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_get_filters, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_stream_filter_register, 0)
	ZEND_ARG_INFO(0, filtername)
	ZEND_ARG_INFO(0, classname)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ uuencode.c */
ZEND_BEGIN_ARG_INFO(arginfo_convert_uuencode, 0)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_convert_uudecode, 0)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ var.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_var_dump, 0, 0, 1)
	ZEND_ARG_INFO(0, var)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_debug_zval_dump, 0, 0, 1)
	ZEND_ARG_INFO(0, var)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_var_export, 0, 0, 1)
	ZEND_ARG_INFO(0, var)
	ZEND_ARG_INFO(0, return)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_serialize, 0)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_unserialize, 0)
	ZEND_ARG_INFO(0, variable_representation)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_memory_get_usage, 0, 0, 0)
	ZEND_ARG_INFO(0, real_usage)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_memory_get_peak_usage, 0, 0, 0)
	ZEND_ARG_INFO(0, real_usage)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ versioning.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_version_compare, 0, 0, 2)
	ZEND_ARG_INFO(0, ver1)
	ZEND_ARG_INFO(0, ver2)
	ZEND_ARG_INFO(0, oper)
ZEND_END_ARG_INFO()
/* }}} */
/* }}} */

const zend_function_entry basic_functions[] = { /* {{{ */
	PHP_FE(constant,														arginfo_constant)
	PHP_FE(bin2hex,															arginfo_bin2hex)
	PHP_FE(hex2bin,															arginfo_hex2bin)
	PHP_FE(sleep,															arginfo_sleep)
	PHP_FE(usleep,															arginfo_usleep)
#if HAVE_NANOSLEEP
	PHP_FE(time_nanosleep,													arginfo_time_nanosleep)
	PHP_FE(time_sleep_until,												arginfo_time_sleep_until)
#endif

#if HAVE_STRPTIME
	PHP_FE(strptime,														arginfo_strptime)
#endif

	PHP_FE(flush,															arginfo_flush)
	PHP_FE(wordwrap,														arginfo_wordwrap)
	PHP_FE(htmlspecialchars,												arginfo_htmlspecialchars)
	PHP_FE(htmlentities,													arginfo_htmlentities)
	PHP_FE(html_entity_decode,												arginfo_html_entity_decode)
	PHP_FE(htmlspecialchars_decode,											arginfo_htmlspecialchars_decode)
	PHP_FE(get_html_translation_table,										arginfo_get_html_translation_table)
	PHP_FE(sha1,															arginfo_sha1)
	PHP_FE(sha1_file,														arginfo_sha1_file)
	PHP_NAMED_FE(md5,php_if_md5,											arginfo_md5)
	PHP_NAMED_FE(md5_file,php_if_md5_file,									arginfo_md5_file)
	PHP_NAMED_FE(crc32,php_if_crc32,										arginfo_crc32)

	PHP_FE(iptcparse,														arginfo_iptcparse)
	PHP_FE(iptcembed,														arginfo_iptcembed)
	PHP_FE(getimagesize,													arginfo_getimagesize)
	PHP_FE(getimagesizefromstring,											arginfo_getimagesize)
	PHP_FE(image_type_to_mime_type,											arginfo_image_type_to_mime_type)
	PHP_FE(image_type_to_extension,											arginfo_image_type_to_extension)

	PHP_FE(phpinfo,															arginfo_phpinfo)
	PHP_FE(phpversion,														arginfo_phpversion)
	PHP_FE(phpcredits,														arginfo_phpcredits)
	PHP_FE(php_logo_guid,													arginfo_php_logo_guid)
	PHP_FE(php_real_logo_guid,												arginfo_php_real_logo_guid)
	PHP_FE(php_egg_logo_guid,												arginfo_php_egg_logo_guid)
	PHP_FE(zend_logo_guid,													arginfo_zend_logo_guid)
	PHP_FE(php_sapi_name,													arginfo_php_sapi_name)
	PHP_FE(php_uname,														arginfo_php_uname)
	PHP_FE(php_ini_scanned_files,											arginfo_php_ini_scanned_files)
	PHP_FE(php_ini_loaded_file,												arginfo_php_ini_loaded_file)

	PHP_FE(strnatcmp,														arginfo_strnatcmp)
	PHP_FE(strnatcasecmp,													arginfo_strnatcasecmp)
	PHP_FE(substr_count,													arginfo_substr_count)
	PHP_FE(strspn,															arginfo_strspn)
	PHP_FE(strcspn,															arginfo_strcspn)
	PHP_FE(strtok,															arginfo_strtok)
	PHP_FE(strtoupper,														arginfo_strtoupper)
	PHP_FE(strtolower,														arginfo_strtolower)
	PHP_FE(strpos,															arginfo_strpos)
	PHP_FE(stripos,															arginfo_stripos)
	PHP_FE(strrpos,															arginfo_strrpos)
	PHP_FE(strripos,														arginfo_strripos)
	PHP_FE(strrev,															arginfo_strrev)
	PHP_FE(hebrev,															arginfo_hebrev)
	PHP_FE(hebrevc,															arginfo_hebrevc)
	PHP_FE(nl2br,															arginfo_nl2br)
	PHP_FE(basename,														arginfo_basename)
	PHP_FE(dirname,															arginfo_dirname)
	PHP_FE(pathinfo,														arginfo_pathinfo)
	PHP_FE(stripslashes,													arginfo_stripslashes)
	PHP_FE(stripcslashes,													arginfo_stripcslashes)
	PHP_FE(strstr,															arginfo_strstr)
	PHP_FE(stristr,															arginfo_stristr)
	PHP_FE(strrchr,															arginfo_strrchr)
	PHP_FE(str_shuffle,														arginfo_str_shuffle)
	PHP_FE(str_word_count,													arginfo_str_word_count)
	PHP_FE(str_split,														arginfo_str_split)
	PHP_FE(strpbrk,															arginfo_strpbrk)
	PHP_FE(substr_compare,													arginfo_substr_compare)

#ifdef HAVE_STRCOLL
	PHP_FE(strcoll,															arginfo_strcoll)
#endif

#ifdef HAVE_STRFMON
	PHP_FE(money_format,													arginfo_money_format)
#endif

	PHP_FE(substr,															arginfo_substr)
	PHP_FE(substr_replace,													arginfo_substr_replace)
	PHP_FE(quotemeta,														arginfo_quotemeta)
	PHP_FE(ucfirst,															arginfo_ucfirst)
	PHP_FE(lcfirst,															arginfo_lcfirst)
	PHP_FE(ucwords,															arginfo_ucwords)
	PHP_FE(strtr,															arginfo_strtr)
	PHP_FE(addslashes,														arginfo_addslashes)
	PHP_FE(addcslashes,														arginfo_addcslashes)
	PHP_FE(rtrim,															arginfo_rtrim)
	PHP_FE(str_replace,														arginfo_str_replace)
	PHP_FE(str_ireplace,													arginfo_str_ireplace)
	PHP_FE(str_repeat,														arginfo_str_repeat)
	PHP_FE(count_chars,														arginfo_count_chars)
	PHP_FE(chunk_split,														arginfo_chunk_split)
	PHP_FE(trim,															arginfo_trim)
	PHP_FE(ltrim,															arginfo_ltrim)
	PHP_FE(strip_tags,														arginfo_strip_tags)
	PHP_FE(similar_text,													arginfo_similar_text)
	PHP_FE(explode,															arginfo_explode)
	PHP_FE(implode,															arginfo_implode)
	PHP_FALIAS(join,				implode,								arginfo_implode)
	PHP_FE(setlocale,														arginfo_setlocale)
	PHP_FE(localeconv,														arginfo_localeconv)

#if HAVE_NL_LANGINFO
	PHP_FE(nl_langinfo,														arginfo_nl_langinfo)
#endif

	PHP_FE(soundex,															arginfo_soundex)
	PHP_FE(levenshtein,														arginfo_levenshtein)
	PHP_FE(chr,																arginfo_chr)
	PHP_FE(ord,																arginfo_ord)
	PHP_FE(parse_str,														arginfo_parse_str)
	PHP_FE(str_getcsv,														arginfo_str_getcsv)
	PHP_FE(str_pad,															arginfo_str_pad)
	PHP_FALIAS(chop,				rtrim,									arginfo_rtrim)
	PHP_FALIAS(strchr,				strstr,									arginfo_strstr)
	PHP_NAMED_FE(sprintf,			PHP_FN(user_sprintf),					arginfo_sprintf)
	PHP_NAMED_FE(printf,			PHP_FN(user_printf),					arginfo_printf)
	PHP_FE(vprintf,															arginfo_vprintf)
	PHP_FE(vsprintf,														arginfo_vsprintf)
	PHP_FE(fprintf,															arginfo_fprintf)
	PHP_FE(vfprintf,														arginfo_vfprintf)
	PHP_FE(sscanf,															arginfo_sscanf)
	PHP_FE(fscanf,															arginfo_fscanf)
	PHP_FE(parse_url,														arginfo_parse_url)
	PHP_FE(urlencode,														arginfo_urlencode)
	PHP_FE(urldecode,														arginfo_urldecode)
	PHP_FE(rawurlencode,													arginfo_rawurlencode)
	PHP_FE(rawurldecode,													arginfo_rawurldecode)
	PHP_FE(http_build_query,												arginfo_http_build_query)

#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
	PHP_FE(readlink,														arginfo_readlink)
	PHP_FE(linkinfo,														arginfo_linkinfo)
	PHP_FE(symlink,															arginfo_symlink)
	PHP_FE(link,															arginfo_link)
#endif

	PHP_FE(unlink,															arginfo_unlink)
	PHP_FE(exec,															arginfo_exec)
	PHP_FE(system,															arginfo_system)
	PHP_FE(escapeshellcmd,													arginfo_escapeshellcmd)
	PHP_FE(escapeshellarg,													arginfo_escapeshellarg)
	PHP_FE(passthru,														arginfo_passthru)
	PHP_FE(shell_exec,														arginfo_shell_exec)
#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	PHP_FE(proc_open,														arginfo_proc_open)
	PHP_FE(proc_close,														arginfo_proc_close)
	PHP_FE(proc_terminate,													arginfo_proc_terminate)
	PHP_FE(proc_get_status,													arginfo_proc_get_status)
#endif

#ifdef HAVE_NICE
	PHP_FE(proc_nice,														arginfo_proc_nice)
#endif

	PHP_FE(rand,															arginfo_rand)
	PHP_FE(srand,															arginfo_srand)
	PHP_FE(getrandmax,													arginfo_getrandmax)
	PHP_FE(mt_rand,														arginfo_mt_rand)
	PHP_FE(mt_srand,														arginfo_mt_srand)
	PHP_FE(mt_getrandmax,													arginfo_mt_getrandmax)

#if HAVE_GETSERVBYNAME
	PHP_FE(getservbyname,													arginfo_getservbyname)
#endif

#if HAVE_GETSERVBYPORT
	PHP_FE(getservbyport,													arginfo_getservbyport)
#endif

#if HAVE_GETPROTOBYNAME
	PHP_FE(getprotobyname,													arginfo_getprotobyname)
#endif

#if HAVE_GETPROTOBYNUMBER
	PHP_FE(getprotobynumber,												arginfo_getprotobynumber)
#endif

	PHP_FE(getmyuid,														arginfo_getmyuid)
	PHP_FE(getmygid,														arginfo_getmygid)
	PHP_FE(getmypid,														arginfo_getmypid)
	PHP_FE(getmyinode,														arginfo_getmyinode)
	PHP_FE(getlastmod,														arginfo_getlastmod)

	PHP_FE(base64_decode,													arginfo_base64_decode)
	PHP_FE(base64_encode,													arginfo_base64_encode)

	PHP_FE(convert_uuencode,												arginfo_convert_uuencode)
	PHP_FE(convert_uudecode,												arginfo_convert_uudecode)

	PHP_FE(abs,																arginfo_abs)
	PHP_FE(ceil,															arginfo_ceil)
	PHP_FE(floor,															arginfo_floor)
	PHP_FE(round,															arginfo_round)
	PHP_FE(sin,																arginfo_sin)
	PHP_FE(cos,																arginfo_cos)
	PHP_FE(tan,																arginfo_tan)
	PHP_FE(asin,															arginfo_asin)
	PHP_FE(acos,															arginfo_acos)
	PHP_FE(atan,															arginfo_atan)
	PHP_FE(atanh,															arginfo_atanh)
	PHP_FE(atan2,															arginfo_atan2)
	PHP_FE(sinh,															arginfo_sinh)
	PHP_FE(cosh,															arginfo_cosh)
	PHP_FE(tanh,															arginfo_tanh)
	PHP_FE(asinh,															arginfo_asinh)
	PHP_FE(acosh,															arginfo_acosh)
	PHP_FE(expm1,															arginfo_expm1)
	PHP_FE(log1p,															arginfo_log1p)
	PHP_FE(pi,																arginfo_pi)
	PHP_FE(is_finite,														arginfo_is_finite)
	PHP_FE(is_nan,															arginfo_is_nan)
	PHP_FE(is_infinite,														arginfo_is_infinite)
	PHP_FE(pow,																arginfo_pow)
	PHP_FE(exp,																arginfo_exp)
	PHP_FE(log,																arginfo_log)
	PHP_FE(log10,															arginfo_log10)
	PHP_FE(sqrt,															arginfo_sqrt)
	PHP_FE(hypot,															arginfo_hypot)
	PHP_FE(deg2rad,															arginfo_deg2rad)
	PHP_FE(rad2deg,															arginfo_rad2deg)
	PHP_FE(bindec,															arginfo_bindec)
	PHP_FE(hexdec,															arginfo_hexdec)
	PHP_FE(octdec,															arginfo_octdec)
	PHP_FE(decbin,															arginfo_decbin)
	PHP_FE(decoct,															arginfo_decoct)
	PHP_FE(dechex,															arginfo_dechex)
	PHP_FE(base_convert,													arginfo_base_convert)
	PHP_FE(number_format,													arginfo_number_format)
	PHP_FE(fmod,															arginfo_fmod)
#ifdef HAVE_INET_NTOP
	PHP_RAW_NAMED_FE(inet_ntop,		php_inet_ntop,								arginfo_inet_ntop)
#endif
#ifdef HAVE_INET_PTON
	PHP_RAW_NAMED_FE(inet_pton,		php_inet_pton,								arginfo_inet_pton)
#endif
	PHP_FE(ip2long,															arginfo_ip2long)
	PHP_FE(long2ip,															arginfo_long2ip)

	PHP_FE(getenv,															arginfo_getenv)
#ifdef HAVE_PUTENV
	PHP_FE(putenv,															arginfo_putenv)
#endif

	PHP_FE(getopt,															arginfo_getopt)

#ifdef HAVE_GETLOADAVG
	PHP_FE(sys_getloadavg,													arginfo_sys_getloadavg)
#endif
#ifdef HAVE_GETTIMEOFDAY
	PHP_FE(microtime,														arginfo_microtime)
	PHP_FE(gettimeofday,													arginfo_gettimeofday)
#endif

#ifdef HAVE_GETRUSAGE
	PHP_FE(getrusage,														arginfo_getrusage)
#endif

#ifdef HAVE_GETTIMEOFDAY
	PHP_FE(uniqid,															arginfo_uniqid)
#endif

	PHP_FE(quoted_printable_decode,											arginfo_quoted_printable_decode)
	PHP_FE(quoted_printable_encode,											arginfo_quoted_printable_encode)
	PHP_FE(convert_cyr_string,												arginfo_convert_cyr_string)
	PHP_FE(get_current_user,												arginfo_get_current_user)
	PHP_FE(set_time_limit,													arginfo_set_time_limit)
	PHP_FE(header_register_callback,										arginfo_header_register_callback)
	PHP_FE(get_cfg_var,														arginfo_get_cfg_var)

	PHP_DEP_FALIAS(magic_quotes_runtime,	set_magic_quotes_runtime,		arginfo_set_magic_quotes_runtime)
	PHP_DEP_FE(set_magic_quotes_runtime,									arginfo_set_magic_quotes_runtime)
	PHP_FE(get_magic_quotes_gpc,										arginfo_get_magic_quotes_gpc)
	PHP_FE(get_magic_quotes_runtime,									arginfo_get_magic_quotes_runtime)

	PHP_FE(error_log,														arginfo_error_log)
	PHP_FE(error_get_last,													arginfo_error_get_last)
	PHP_FE(call_user_func,													arginfo_call_user_func)
	PHP_FE(call_user_func_array,											arginfo_call_user_func_array)
	PHP_DEP_FE(call_user_method,											arginfo_call_user_method)
	PHP_DEP_FE(call_user_method_array,										arginfo_call_user_method_array)
	PHP_FE(forward_static_call,											arginfo_forward_static_call)
	PHP_FE(forward_static_call_array,										arginfo_forward_static_call_array)
	PHP_FE(serialize,														arginfo_serialize)
	PHP_FE(unserialize,														arginfo_unserialize)

	PHP_FE(var_dump,														arginfo_var_dump)
	PHP_FE(var_export,														arginfo_var_export)
	PHP_FE(debug_zval_dump,													arginfo_debug_zval_dump)
	PHP_FE(print_r,															arginfo_print_r)
	PHP_FE(memory_get_usage,												arginfo_memory_get_usage)
	PHP_FE(memory_get_peak_usage,											arginfo_memory_get_peak_usage)

	PHP_FE(register_shutdown_function,										arginfo_register_shutdown_function)
	PHP_FE(register_tick_function,											arginfo_register_tick_function)
	PHP_FE(unregister_tick_function,										arginfo_unregister_tick_function)

	PHP_FE(highlight_file,													arginfo_highlight_file)
	PHP_FALIAS(show_source,			highlight_file,							arginfo_highlight_file)
	PHP_FE(highlight_string,												arginfo_highlight_string)
	PHP_FE(php_strip_whitespace,											arginfo_php_strip_whitespace)

	PHP_FE(ini_get,															arginfo_ini_get)
	PHP_FE(ini_get_all,														arginfo_ini_get_all)
	PHP_FE(ini_set,															arginfo_ini_set)
	PHP_FALIAS(ini_alter,			ini_set,								arginfo_ini_set)
	PHP_FE(ini_restore,														arginfo_ini_restore)
	PHP_FE(get_include_path,												arginfo_get_include_path)
	PHP_FE(set_include_path,												arginfo_set_include_path)
	PHP_FE(restore_include_path,											arginfo_restore_include_path)

	PHP_FE(setcookie,														arginfo_setcookie)
	PHP_FE(setrawcookie,													arginfo_setrawcookie)
	PHP_FE(header,															arginfo_header)
	PHP_FE(header_remove,													arginfo_header_remove)
	PHP_FE(headers_sent,													arginfo_headers_sent)
	PHP_FE(headers_list,													arginfo_headers_list)
	PHP_FE(http_response_code,												arginfo_http_response_code)

	PHP_FE(connection_aborted,												arginfo_connection_aborted)
	PHP_FE(connection_status,												arginfo_connection_status)
	PHP_FE(ignore_user_abort,												arginfo_ignore_user_abort)
	PHP_FE(parse_ini_file,													arginfo_parse_ini_file)
	PHP_FE(parse_ini_string,												arginfo_parse_ini_string)
#if ZEND_DEBUG
	PHP_FE(config_get_hash,													arginfo_config_get_hash)
#endif
	PHP_FE(is_uploaded_file,												arginfo_is_uploaded_file)
	PHP_FE(move_uploaded_file,												arginfo_move_uploaded_file)

	/* functions from dns.c */
	PHP_FE(gethostbyaddr,													arginfo_gethostbyaddr)
	PHP_FE(gethostbyname,													arginfo_gethostbyname)
	PHP_FE(gethostbynamel,													arginfo_gethostbynamel)

#ifdef HAVE_GETHOSTNAME
	PHP_FE(gethostname,													arginfo_gethostname)
#endif

#if defined(PHP_WIN32) || (HAVE_DNS_SEARCH_FUNC && !(defined(__BEOS__) || defined(NETWARE)))

	PHP_FE(dns_check_record,												arginfo_dns_check_record)
	PHP_FALIAS(checkdnsrr,			dns_check_record,						arginfo_dns_check_record)

# if defined(PHP_WIN32) || HAVE_FULL_DNS_FUNCS
	PHP_FE(dns_get_mx,														arginfo_dns_get_mx)
	PHP_FALIAS(getmxrr,				dns_get_mx,					arginfo_dns_get_mx)
	PHP_FE(dns_get_record,													arginfo_dns_get_record)
# endif
#endif

	/* functions from type.c */
	PHP_FE(intval,															arginfo_intval)
	PHP_FE(floatval,														arginfo_floatval)
	PHP_FALIAS(doubleval,			floatval,								arginfo_floatval)
	PHP_FE(strval,															arginfo_strval)
	PHP_FE(gettype,															arginfo_gettype)
	PHP_FE(settype,															arginfo_settype)
	PHP_FE(is_null,															arginfo_is_null)
	PHP_FE(is_resource,														arginfo_is_resource)
	PHP_FE(is_bool,															arginfo_is_bool)
	PHP_FE(is_long,															arginfo_is_long)
	PHP_FE(is_float,														arginfo_is_float)
	PHP_FALIAS(is_int,				is_long,								arginfo_is_long)
	PHP_FALIAS(is_integer,			is_long,								arginfo_is_long)
	PHP_FALIAS(is_double,			is_float,								arginfo_is_float)
	PHP_FALIAS(is_real,				is_float,								arginfo_is_float)
	PHP_FE(is_numeric,														arginfo_is_numeric)
	PHP_FE(is_string,														arginfo_is_string)
	PHP_FE(is_array,														arginfo_is_array)
	PHP_FE(is_object,														arginfo_is_object)
	PHP_FE(is_scalar,														arginfo_is_scalar)
	PHP_FE(is_callable,														arginfo_is_callable)

	/* functions from file.c */
	PHP_FE(pclose,															arginfo_pclose)
	PHP_FE(popen,															arginfo_popen)
	PHP_FE(readfile,														arginfo_readfile)
	PHP_FE(rewind,															arginfo_rewind)
	PHP_FE(rmdir,															arginfo_rmdir)
	PHP_FE(umask,															arginfo_umask)
	PHP_FE(fclose,															arginfo_fclose)
	PHP_FE(feof,															arginfo_feof)
	PHP_FE(fgetc,															arginfo_fgetc)
	PHP_FE(fgets,															arginfo_fgets)
	PHP_FE(fgetss,															arginfo_fgetss)
	PHP_FE(fread,															arginfo_fread)
	PHP_NAMED_FE(fopen,				php_if_fopen,							arginfo_fopen)
	PHP_FE(fpassthru,														arginfo_fpassthru)
	PHP_NAMED_FE(ftruncate,			php_if_ftruncate,						arginfo_ftruncate)
	PHP_NAMED_FE(fstat,				php_if_fstat,							arginfo_fstat)
	PHP_FE(fseek,															arginfo_fseek)
	PHP_FE(ftell,															arginfo_ftell)
	PHP_FE(fflush,															arginfo_fflush)
	PHP_FE(fwrite,															arginfo_fwrite)
	PHP_FALIAS(fputs,				fwrite,									arginfo_fwrite)
	PHP_FE(mkdir,															arginfo_mkdir)
	PHP_FE(rename,															arginfo_rename)
	PHP_FE(copy,															arginfo_copy)
	PHP_FE(tempnam,															arginfo_tempnam)
	PHP_NAMED_FE(tmpfile,			php_if_tmpfile,							arginfo_tmpfile)
	PHP_FE(file,															arginfo_file)
	PHP_FE(file_get_contents,												arginfo_file_get_contents)
	PHP_FE(file_put_contents,												arginfo_file_put_contents)
	PHP_FE(stream_select,													arginfo_stream_select)
	PHP_FE(stream_context_create,											arginfo_stream_context_create)
	PHP_FE(stream_context_set_params,										arginfo_stream_context_set_params)
	PHP_FE(stream_context_get_params,										arginfo_stream_context_get_params)
	PHP_FE(stream_context_set_option,										arginfo_stream_context_set_option)
	PHP_FE(stream_context_get_options,										arginfo_stream_context_get_options)
	PHP_FE(stream_context_get_default,										arginfo_stream_context_get_default)
	PHP_FE(stream_context_set_default,										arginfo_stream_context_set_default)
	PHP_FE(stream_filter_prepend,											arginfo_stream_filter_prepend)
	PHP_FE(stream_filter_append,											arginfo_stream_filter_append)
	PHP_FE(stream_filter_remove,											arginfo_stream_filter_remove)
	PHP_FE(stream_socket_client,											arginfo_stream_socket_client)
	PHP_FE(stream_socket_server,											arginfo_stream_socket_server)
	PHP_FE(stream_socket_accept,											arginfo_stream_socket_accept)
	PHP_FE(stream_socket_get_name,											arginfo_stream_socket_get_name)
	PHP_FE(stream_socket_recvfrom,											arginfo_stream_socket_recvfrom)
	PHP_FE(stream_socket_sendto,											arginfo_stream_socket_sendto)
	PHP_FE(stream_socket_enable_crypto,										arginfo_stream_socket_enable_crypto)
#ifdef HAVE_SHUTDOWN
	PHP_FE(stream_socket_shutdown,											arginfo_stream_socket_shutdown)
#endif
#if HAVE_SOCKETPAIR
	PHP_FE(stream_socket_pair,												arginfo_stream_socket_pair)
#endif
	PHP_FE(stream_copy_to_stream,											arginfo_stream_copy_to_stream)
	PHP_FE(stream_get_contents,												arginfo_stream_get_contents)
	PHP_FE(stream_supports_lock,											arginfo_stream_supports_lock)
	PHP_FE(fgetcsv,															arginfo_fgetcsv)
	PHP_FE(fputcsv,															arginfo_fputcsv)
	PHP_FE(flock,															arginfo_flock)
	PHP_FE(get_meta_tags,													arginfo_get_meta_tags)
	PHP_FE(stream_set_read_buffer,											arginfo_stream_set_read_buffer)
	PHP_FE(stream_set_write_buffer,											arginfo_stream_set_write_buffer)
	PHP_FALIAS(set_file_buffer, stream_set_write_buffer,					arginfo_stream_set_write_buffer)
	PHP_FE(stream_set_chunk_size,											arginfo_stream_set_chunk_size)

	PHP_DEP_FALIAS(set_socket_blocking, stream_set_blocking,				arginfo_stream_set_blocking)
	PHP_FE(stream_set_blocking,												arginfo_stream_set_blocking)
	PHP_FALIAS(socket_set_blocking, stream_set_blocking,					arginfo_stream_set_blocking)

	PHP_FE(stream_get_meta_data,											arginfo_stream_get_meta_data)
	PHP_FE(stream_get_line,													arginfo_stream_get_line)
	PHP_FE(stream_wrapper_register,											arginfo_stream_wrapper_register)
	PHP_FALIAS(stream_register_wrapper, stream_wrapper_register,			arginfo_stream_wrapper_register)
	PHP_FE(stream_wrapper_unregister,										arginfo_stream_wrapper_unregister)
	PHP_FE(stream_wrapper_restore,											arginfo_stream_wrapper_restore)
	PHP_FE(stream_get_wrappers,												arginfo_stream_get_wrappers)
	PHP_FE(stream_get_transports,											arginfo_stream_get_transports)
	PHP_FE(stream_resolve_include_path,										arginfo_stream_resolve_include_path)
	PHP_FE(stream_is_local,												arginfo_stream_is_local)
	PHP_FE(get_headers,														arginfo_get_headers)

#if HAVE_SYS_TIME_H || defined(PHP_WIN32)
	PHP_FE(stream_set_timeout,												arginfo_stream_set_timeout)
	PHP_FALIAS(socket_set_timeout, stream_set_timeout,						arginfo_stream_set_timeout)
#endif

	PHP_FALIAS(socket_get_status, stream_get_meta_data,						arginfo_stream_get_meta_data)

#if (!defined(__BEOS__) && !defined(NETWARE) && HAVE_REALPATH) || defined(ZTS)
	PHP_FE(realpath,														arginfo_realpath)
#endif

#ifdef HAVE_FNMATCH
	PHP_FE(fnmatch,															arginfo_fnmatch)
#endif

	/* functions from fsock.c */
	PHP_FE(fsockopen,														arginfo_fsockopen)
	PHP_FE(pfsockopen,														arginfo_pfsockopen)

	/* functions from pack.c */
	PHP_FE(pack,															arginfo_pack)
	PHP_FE(unpack,															arginfo_unpack)

	/* functions from browscap.c */
	PHP_FE(get_browser,														arginfo_get_browser)

#if HAVE_CRYPT
	/* functions from crypt.c */
	PHP_FE(crypt,															arginfo_crypt)
#endif

	/* functions from dir.c */
	PHP_FE(opendir,															arginfo_opendir)
	PHP_FE(closedir,														arginfo_closedir)
	PHP_FE(chdir,															arginfo_chdir)

#if defined(HAVE_CHROOT) && !defined(ZTS) && ENABLE_CHROOT_FUNC
	PHP_FE(chroot,															arginfo_chroot)
#endif

	PHP_FE(getcwd,															arginfo_getcwd)
	PHP_FE(rewinddir,														arginfo_rewinddir)
	PHP_NAMED_FE(readdir,			php_if_readdir,							arginfo_readdir)
	PHP_FALIAS(dir,					getdir,									arginfo_dir)
	PHP_FE(scandir,															arginfo_scandir)
#ifdef HAVE_GLOB
	PHP_FE(glob,															arginfo_glob)
#endif
	/* functions from filestat.c */
	PHP_FE(fileatime,														arginfo_fileatime)
	PHP_FE(filectime,														arginfo_filectime)
	PHP_FE(filegroup,														arginfo_filegroup)
	PHP_FE(fileinode,														arginfo_fileinode)
	PHP_FE(filemtime,														arginfo_filemtime)
	PHP_FE(fileowner,														arginfo_fileowner)
	PHP_FE(fileperms,														arginfo_fileperms)
	PHP_FE(filesize,														arginfo_filesize)
	PHP_FE(filetype,														arginfo_filetype)
	PHP_FE(file_exists,														arginfo_file_exists)
	PHP_FE(is_writable,														arginfo_is_writable)
	PHP_FALIAS(is_writeable,		is_writable,							arginfo_is_writable)
	PHP_FE(is_readable,														arginfo_is_readable)
	PHP_FE(is_executable,													arginfo_is_executable)
	PHP_FE(is_file,															arginfo_is_file)
	PHP_FE(is_dir,															arginfo_is_dir)
	PHP_FE(is_link,															arginfo_is_link)
	PHP_NAMED_FE(stat,				php_if_stat,							arginfo_stat)
	PHP_NAMED_FE(lstat,				php_if_lstat,							arginfo_lstat)
#ifndef NETWARE
	PHP_FE(chown,															arginfo_chown)
	PHP_FE(chgrp,															arginfo_chgrp)
#endif
#if HAVE_LCHOWN
	PHP_FE(lchown,															arginfo_lchown)
#endif
#if HAVE_LCHOWN
	PHP_FE(lchgrp,															arginfo_lchgrp)
#endif
	PHP_FE(chmod,															arginfo_chmod)
#if HAVE_UTIME
	PHP_FE(touch,															arginfo_touch)
#endif
	PHP_FE(clearstatcache,													arginfo_clearstatcache)
	PHP_FE(disk_total_space,												arginfo_disk_total_space)
	PHP_FE(disk_free_space,													arginfo_disk_free_space)
	PHP_FALIAS(diskfreespace,		disk_free_space,						arginfo_disk_free_space)
	PHP_FE(realpath_cache_size,												arginfo_realpath_cache_size)
	PHP_FE(realpath_cache_get,												arginfo_realpath_cache_get)

	/* functions from mail.c */
	PHP_FE(mail,															arginfo_mail)
	PHP_FE(ezmlm_hash,														arginfo_ezmlm_hash)

	/* functions from syslog.c */
#ifdef HAVE_SYSLOG_H
	PHP_FE(openlog,															arginfo_openlog)
	PHP_FE(syslog,															arginfo_syslog)
	PHP_FE(closelog,														arginfo_closelog)
#endif

	/* functions from lcg.c */
	PHP_FE(lcg_value,														arginfo_lcg_value)

	/* functions from metaphone.c */
	PHP_FE(metaphone,														arginfo_metaphone)

	/* functions from output.c */
	PHP_FE(ob_start,														arginfo_ob_start)
	PHP_FE(ob_flush,														arginfo_ob_flush)
	PHP_FE(ob_clean,														arginfo_ob_clean)
	PHP_FE(ob_end_flush,													arginfo_ob_end_flush)
	PHP_FE(ob_end_clean,													arginfo_ob_end_clean)
	PHP_FE(ob_get_flush,													arginfo_ob_get_flush)
	PHP_FE(ob_get_clean,													arginfo_ob_get_clean)
	PHP_FE(ob_get_length,													arginfo_ob_get_length)
	PHP_FE(ob_get_level,													arginfo_ob_get_level)
	PHP_FE(ob_get_status,													arginfo_ob_get_status)
	PHP_FE(ob_get_contents,													arginfo_ob_get_contents)
	PHP_FE(ob_implicit_flush,												arginfo_ob_implicit_flush)
	PHP_FE(ob_list_handlers,												arginfo_ob_list_handlers)

	/* functions from array.c */
	PHP_FE(ksort,															arginfo_ksort)
	PHP_FE(krsort,															arginfo_krsort)
	PHP_FE(natsort,															arginfo_natsort)
	PHP_FE(natcasesort,														arginfo_natcasesort)
	PHP_FE(asort,															arginfo_asort)
	PHP_FE(arsort,															arginfo_arsort)
	PHP_FE(sort,															arginfo_sort)
	PHP_FE(rsort,															arginfo_rsort)
	PHP_FE(usort,															arginfo_usort)
	PHP_FE(uasort,															arginfo_uasort)
	PHP_FE(uksort,															arginfo_uksort)
	PHP_FE(shuffle,															arginfo_shuffle)
	PHP_FE(array_walk,														arginfo_array_walk)
	PHP_FE(array_walk_recursive,											arginfo_array_walk_recursive)
	PHP_FE(count,															arginfo_count)
	PHP_FE(end,																arginfo_end)
	PHP_FE(prev,															arginfo_prev)
	PHP_FE(next,															arginfo_next)
	PHP_FE(reset,															arginfo_reset)
	PHP_FE(current,															arginfo_current)
	PHP_FE(key,																arginfo_key)
	PHP_FE(min,																arginfo_min)
	PHP_FE(max,																arginfo_max)
	PHP_FE(in_array,														arginfo_in_array)
	PHP_FE(array_search,													arginfo_array_search)
	PHP_FE(extract,															arginfo_extract)
	PHP_FE(compact,															arginfo_compact)
	PHP_FE(array_fill,														arginfo_array_fill)
	PHP_FE(array_fill_keys,													arginfo_array_fill_keys)
	PHP_FE(range,															arginfo_range)
	PHP_FE(array_multisort,													arginfo_array_multisort)
	PHP_FE(array_push,														arginfo_array_push)
	PHP_FE(array_pop,														arginfo_array_pop)
	PHP_FE(array_shift,														arginfo_array_shift)
	PHP_FE(array_unshift,													arginfo_array_unshift)
	PHP_FE(array_splice,													arginfo_array_splice)
	PHP_FE(array_slice,														arginfo_array_slice)
	PHP_FE(array_merge,														arginfo_array_merge)
	PHP_FE(array_merge_recursive,											arginfo_array_merge_recursive)
	PHP_FE(array_replace,													arginfo_array_replace)
	PHP_FE(array_replace_recursive,											arginfo_array_replace_recursive)
	PHP_FE(array_keys,														arginfo_array_keys)
	PHP_FE(array_values,													arginfo_array_values)
	PHP_FE(array_count_values,												arginfo_array_count_values)
	PHP_FE(array_reverse,													arginfo_array_reverse)
	PHP_FE(array_reduce,													arginfo_array_reduce)
	PHP_FE(array_pad,														arginfo_array_pad)
	PHP_FE(array_flip,														arginfo_array_flip)
	PHP_FE(array_change_key_case,											arginfo_array_change_key_case)
	PHP_FE(array_rand,														arginfo_array_rand)
	PHP_FE(array_unique,													arginfo_array_unique)
	PHP_FE(array_intersect,													arginfo_array_intersect)
	PHP_FE(array_intersect_key,												arginfo_array_intersect_key)
	PHP_FE(array_intersect_ukey,											arginfo_array_intersect_ukey)
	PHP_FE(array_uintersect,												arginfo_array_uintersect)
	PHP_FE(array_intersect_assoc,											arginfo_array_intersect_assoc)
	PHP_FE(array_uintersect_assoc,											arginfo_array_uintersect_assoc)
	PHP_FE(array_intersect_uassoc,											arginfo_array_intersect_uassoc)
	PHP_FE(array_uintersect_uassoc,											arginfo_array_uintersect_uassoc)
	PHP_FE(array_diff,														arginfo_array_diff)
	PHP_FE(array_diff_key,													arginfo_array_diff_key)
	PHP_FE(array_diff_ukey,													arginfo_array_diff_ukey)
	PHP_FE(array_udiff,														arginfo_array_udiff)
	PHP_FE(array_diff_assoc,												arginfo_array_diff_assoc)
	PHP_FE(array_udiff_assoc,												arginfo_array_udiff_assoc)
	PHP_FE(array_diff_uassoc,												arginfo_array_diff_uassoc)
	PHP_FE(array_udiff_uassoc,												arginfo_array_udiff_uassoc)
	PHP_FE(array_sum,														arginfo_array_sum)
	PHP_FE(array_product,													arginfo_array_product)
	PHP_FE(array_filter,													arginfo_array_filter)
	PHP_FE(array_map,														arginfo_array_map)
	PHP_FE(array_chunk,														arginfo_array_chunk)
	PHP_FE(array_combine,													arginfo_array_combine)
	PHP_FE(array_key_exists,												arginfo_array_key_exists)

	/* aliases from array.c */
	PHP_FALIAS(pos,					current,								arginfo_current)
	PHP_FALIAS(sizeof,				count,									arginfo_count)
	PHP_FALIAS(key_exists,			array_key_exists,						arginfo_array_key_exists)

	/* functions from assert.c */
	PHP_FE(assert,															arginfo_assert)
	PHP_FE(assert_options,													arginfo_assert_options)

	/* functions from versioning.c */
	PHP_FE(version_compare,													arginfo_version_compare)

	/* functions from ftok.c*/
#if HAVE_FTOK
	PHP_FE(ftok,															arginfo_ftok)
#endif

	PHP_FE(str_rot13,														arginfo_str_rot13)
	PHP_FE(stream_get_filters,												arginfo_stream_get_filters)
	PHP_FE(stream_filter_register,											arginfo_stream_filter_register)
	PHP_FE(stream_bucket_make_writeable,									arginfo_stream_bucket_make_writeable)
	PHP_FE(stream_bucket_prepend,											arginfo_stream_bucket_prepend)
	PHP_FE(stream_bucket_append,											arginfo_stream_bucket_append)
	PHP_FE(stream_bucket_new,												arginfo_stream_bucket_new)

	PHP_FE(output_add_rewrite_var,											arginfo_output_add_rewrite_var)
	PHP_FE(output_reset_rewrite_vars,										arginfo_output_reset_rewrite_vars)

	PHP_FE(sys_get_temp_dir,												arginfo_sys_get_temp_dir)

	PHP_FE_END
};
/* }}} */

static const zend_module_dep standard_deps[] = { /* {{{ */
	ZEND_MOD_OPTIONAL("session")
	ZEND_MOD_END
};
/* }}} */

zend_module_entry basic_functions_module = { /* {{{ */
	STANDARD_MODULE_HEADER_EX,
	NULL,
	standard_deps,
	"standard",					/* extension name */
	basic_functions,			/* function list */
	PHP_MINIT(basic),			/* process startup */
	PHP_MSHUTDOWN(basic),		/* process shutdown */
	PHP_RINIT(basic),			/* request startup */
	PHP_RSHUTDOWN(basic),		/* request shutdown */
	PHP_MINFO(basic),			/* extension info */
	PHP_VERSION,				/* extension version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#if defined(HAVE_PUTENV)
static void php_putenv_destructor(putenv_entry *pe) /* {{{ */
{
	if (pe->previous_value) {
#if _MSC_VER >= 1300
		/* VS.Net has a bug in putenv() when setting a variable that
		 * is already set; if the SetEnvironmentVariable() API call
		 * fails, the Crt will double free() a string.
		 * We try to avoid this by setting our own value first */
		SetEnvironmentVariable(pe->key, "bugbug");
#endif
		putenv(pe->previous_value);
# if defined(PHP_WIN32)
		efree(pe->previous_value);
# endif
	} else {
# if HAVE_UNSETENV
		unsetenv(pe->key);
# elif defined(PHP_WIN32)
		SetEnvironmentVariable(pe->key, NULL);
# else
		char **env;

		for (env = environ; env != NULL && *env != NULL; env++) {
			if (!strncmp(*env, pe->key, pe->key_len) && (*env)[pe->key_len] == '=') {	/* found it */
				*env = "";
				break;
			}
		}
# endif
	}
#ifdef HAVE_TZSET
	/* don't forget to reset the various libc globals that
	 * we might have changed by an earlier call to tzset(). */
	if (!strncmp(pe->key, "TZ", pe->key_len)) {
		tzset();
	}
#endif

	efree(pe->putenv_string);
	efree(pe->key);
}
/* }}} */
#endif

static void basic_globals_ctor(php_basic_globals *basic_globals_p TSRMLS_DC) /* {{{ */
{
	BG(rand_is_seeded) = 0;
	BG(mt_rand_is_seeded) = 0;
	BG(umask) = -1;
	BG(next) = NULL;
	BG(left) = -1;
	BG(user_tick_functions) = NULL;
	BG(user_filter_map) = NULL;
	BG(serialize_lock) = 0;
	
	memset(&BG(serialize), 0, sizeof(BG(serialize)));
	memset(&BG(unserialize), 0, sizeof(BG(unserialize)));

	memset(&BG(url_adapt_state_ex), 0, sizeof(BG(url_adapt_state_ex)));

#if defined(_REENTRANT) && defined(HAVE_MBRLEN) && defined(HAVE_MBSTATE_T)
	memset(&BG(mblen_state), 0, sizeof(BG(mblen_state)));
#endif

	BG(incomplete_class) = incomplete_class_entry;
	BG(page_uid) = -1;
	BG(page_gid) = -1;
}
/* }}} */

static void basic_globals_dtor(php_basic_globals *basic_globals_p TSRMLS_DC) /* {{{ */
{
	if (BG(url_adapt_state_ex).tags) {
		zend_hash_destroy(BG(url_adapt_state_ex).tags);
		free(BG(url_adapt_state_ex).tags);
	}
}
/* }}} */

#define PHP_DOUBLE_INFINITY_HIGH       0x7ff00000
#define PHP_DOUBLE_QUIET_NAN_HIGH      0xfff80000

PHPAPI double php_get_nan(void) /* {{{ */
{
#if HAVE_HUGE_VAL_NAN
	return HUGE_VAL + -HUGE_VAL;
#elif defined(__i386__) || defined(_X86_) || defined(ALPHA) || defined(_ALPHA) || defined(__alpha)
	double val = 0.0;
	((php_uint32*)&val)[1] = PHP_DOUBLE_QUIET_NAN_HIGH;
	((php_uint32*)&val)[0] = 0;
	return val;
#elif HAVE_ATOF_ACCEPTS_NAN
	return atof("NAN");
#else
	return 0.0/0.0;
#endif
}
/* }}} */

PHPAPI double php_get_inf(void) /* {{{ */
{
#if HAVE_HUGE_VAL_INF
	return HUGE_VAL;
#elif defined(__i386__) || defined(_X86_) || defined(ALPHA) || defined(_ALPHA) || defined(__alpha)
	double val = 0.0;
	((php_uint32*)&val)[1] = PHP_DOUBLE_INFINITY_HIGH;
	((php_uint32*)&val)[0] = 0;
	return val;
#elif HAVE_ATOF_ACCEPTS_INF
	return atof("INF");
#else
	return 1.0/0.0;
#endif
}
/* }}} */

PHP_MINIT_FUNCTION(basic) /* {{{ */
{
#ifdef ZTS
	ts_allocate_id(&basic_globals_id, sizeof(php_basic_globals), (ts_allocate_ctor) basic_globals_ctor, (ts_allocate_dtor) basic_globals_dtor);
#ifdef PHP_WIN32
	ts_allocate_id(&php_win32_core_globals_id, sizeof(php_win32_core_globals), (ts_allocate_ctor)php_win32_core_globals_ctor, (ts_allocate_dtor)php_win32_core_globals_dtor );
#endif
#else
	basic_globals_ctor(&basic_globals TSRMLS_CC);
#ifdef PHP_WIN32
	php_win32_core_globals_ctor(&the_php_win32_core_globals TSRMLS_CC);
#endif
#endif

	BG(incomplete_class) = incomplete_class_entry = php_create_incomplete_class(TSRMLS_C);

	REGISTER_LONG_CONSTANT("CONNECTION_ABORTED", PHP_CONNECTION_ABORTED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CONNECTION_NORMAL",  PHP_CONNECTION_NORMAL,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CONNECTION_TIMEOUT", PHP_CONNECTION_TIMEOUT, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("INI_USER",   ZEND_INI_USER,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_PERDIR", ZEND_INI_PERDIR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_SYSTEM", ZEND_INI_SYSTEM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_ALL",    ZEND_INI_ALL,    CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("INI_SCANNER_NORMAL", ZEND_INI_SCANNER_NORMAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_SCANNER_RAW",    ZEND_INI_SCANNER_RAW,    CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("PHP_URL_SCHEME", PHP_URL_SCHEME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_URL_HOST", PHP_URL_HOST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_URL_PORT", PHP_URL_PORT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_URL_USER", PHP_URL_USER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_URL_PASS", PHP_URL_PASS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_URL_PATH", PHP_URL_PATH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_URL_QUERY", PHP_URL_QUERY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_URL_FRAGMENT", PHP_URL_FRAGMENT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_QUERY_RFC1738", PHP_QUERY_RFC1738, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_QUERY_RFC3986", PHP_QUERY_RFC3986, CONST_CS | CONST_PERSISTENT);

#define REGISTER_MATH_CONSTANT(x)  REGISTER_DOUBLE_CONSTANT(#x, x, CONST_CS | CONST_PERSISTENT)
	REGISTER_MATH_CONSTANT(M_E);
	REGISTER_MATH_CONSTANT(M_LOG2E);
	REGISTER_MATH_CONSTANT(M_LOG10E);
	REGISTER_MATH_CONSTANT(M_LN2);
	REGISTER_MATH_CONSTANT(M_LN10);
	REGISTER_MATH_CONSTANT(M_PI);
	REGISTER_MATH_CONSTANT(M_PI_2);
	REGISTER_MATH_CONSTANT(M_PI_4);
	REGISTER_MATH_CONSTANT(M_1_PI);
	REGISTER_MATH_CONSTANT(M_2_PI);
	REGISTER_MATH_CONSTANT(M_SQRTPI);
	REGISTER_MATH_CONSTANT(M_2_SQRTPI);
	REGISTER_MATH_CONSTANT(M_LNPI);
	REGISTER_MATH_CONSTANT(M_EULER);
	REGISTER_MATH_CONSTANT(M_SQRT2);
	REGISTER_MATH_CONSTANT(M_SQRT1_2);
	REGISTER_MATH_CONSTANT(M_SQRT3);
	REGISTER_DOUBLE_CONSTANT("INF", php_get_inf(), CONST_CS | CONST_PERSISTENT);
	REGISTER_DOUBLE_CONSTANT("NAN", php_get_nan(), CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("PHP_ROUND_HALF_UP", PHP_ROUND_HALF_UP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_ROUND_HALF_DOWN", PHP_ROUND_HALF_DOWN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_ROUND_HALF_EVEN", PHP_ROUND_HALF_EVEN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PHP_ROUND_HALF_ODD", PHP_ROUND_HALF_ODD, CONST_CS | CONST_PERSISTENT);

#if ENABLE_TEST_CLASS
	test_class_startup();
#endif

	register_phpinfo_constants(INIT_FUNC_ARGS_PASSTHRU);
	register_html_constants(INIT_FUNC_ARGS_PASSTHRU);
	register_string_constants(INIT_FUNC_ARGS_PASSTHRU);

	PHP_MINIT(file)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(pack)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(browscap)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(standard_filters)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(user_filters)(INIT_FUNC_ARGS_PASSTHRU);

#if defined(HAVE_LOCALECONV) && defined(ZTS)
	PHP_MINIT(localeconv)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#if defined(HAVE_NL_LANGINFO)
	PHP_MINIT(nl_langinfo)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#if HAVE_CRYPT
	PHP_MINIT(crypt)(INIT_FUNC_ARGS_PASSTHRU);
#endif

	PHP_MINIT(lcg)(INIT_FUNC_ARGS_PASSTHRU);

	PHP_MINIT(dir)(INIT_FUNC_ARGS_PASSTHRU);
#ifdef HAVE_SYSLOG_H
	PHP_MINIT(syslog)(INIT_FUNC_ARGS_PASSTHRU);
#endif
	PHP_MINIT(array)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(assert)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(url_scanner_ex)(INIT_FUNC_ARGS_PASSTHRU);
#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	PHP_MINIT(proc_open)(INIT_FUNC_ARGS_PASSTHRU);
#endif

	PHP_MINIT(user_streams)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(imagetypes)(INIT_FUNC_ARGS_PASSTHRU);

	php_register_url_stream_wrapper("php", &php_stream_php_wrapper TSRMLS_CC);
	php_register_url_stream_wrapper("file", &php_plain_files_wrapper TSRMLS_CC);
#ifdef HAVE_GLOB
	php_register_url_stream_wrapper("glob", &php_glob_stream_wrapper TSRMLS_CC);
#endif
	php_register_url_stream_wrapper("data", &php_stream_rfc2397_wrapper TSRMLS_CC);
#ifndef PHP_CURL_URL_WRAPPERS
	php_register_url_stream_wrapper("http", &php_stream_http_wrapper TSRMLS_CC);
	php_register_url_stream_wrapper("ftp", &php_stream_ftp_wrapper TSRMLS_CC);
#endif

#if defined(PHP_WIN32) || (HAVE_DNS_SEARCH_FUNC && !(defined(__BEOS__) || defined(NETWARE)))
# if defined(PHP_WIN32) || HAVE_FULL_DNS_FUNCS
	PHP_MINIT(dns)(INIT_FUNC_ARGS_PASSTHRU);
# endif
#endif

	return SUCCESS;
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(basic) /* {{{ */
{
#ifdef HAVE_SYSLOG_H
	PHP_MSHUTDOWN(syslog)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#endif
#ifdef ZTS
	ts_free_id(basic_globals_id);
#ifdef PHP_WIN32
	ts_free_id(php_win32_core_globals_id);
#endif
#else
	basic_globals_dtor(&basic_globals TSRMLS_CC);
#ifdef PHP_WIN32
	php_win32_core_globals_dtor(&the_php_win32_core_globals TSRMLS_CC);
#endif
#endif

	php_unregister_url_stream_wrapper("php" TSRMLS_CC);
#ifndef PHP_CURL_URL_WRAPPERS
	php_unregister_url_stream_wrapper("http" TSRMLS_CC);
	php_unregister_url_stream_wrapper("ftp" TSRMLS_CC);
#endif

	PHP_MSHUTDOWN(browscap)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_MSHUTDOWN(array)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_MSHUTDOWN(assert)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_MSHUTDOWN(url_scanner_ex)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_MSHUTDOWN(file)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_MSHUTDOWN(standard_filters)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#if defined(HAVE_LOCALECONV) && defined(ZTS)
	PHP_MSHUTDOWN(localeconv)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#endif
#if HAVE_CRYPT
	PHP_MSHUTDOWN(crypt)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#endif

	return SUCCESS;
}
/* }}} */

PHP_RINIT_FUNCTION(basic) /* {{{ */
{
	memset(BG(strtok_table), 0, 256);

	BG(serialize_lock) = 0;
	memset(&BG(serialize), 0, sizeof(BG(serialize)));
	memset(&BG(unserialize), 0, sizeof(BG(unserialize)));

	BG(strtok_string) = NULL;
	BG(strtok_zval) = NULL;
	BG(strtok_last) = NULL;
	BG(locale_string) = NULL;
	BG(array_walk_fci) = empty_fcall_info;
	BG(array_walk_fci_cache) = empty_fcall_info_cache;
	BG(user_compare_fci) = empty_fcall_info;
	BG(user_compare_fci_cache) = empty_fcall_info_cache;
	BG(page_uid) = -1;
	BG(page_gid) = -1;
	BG(page_inode) = -1;
	BG(page_mtime) = -1;
#ifdef HAVE_PUTENV
	if (zend_hash_init(&BG(putenv_ht), 1, NULL, (void (*)(void *)) php_putenv_destructor, 0) == FAILURE) {
		return FAILURE;
	}
#endif
	BG(user_shutdown_function_names) = NULL;

	PHP_RINIT(filestat)(INIT_FUNC_ARGS_PASSTHRU);
#ifdef HAVE_SYSLOG_H
	PHP_RINIT(syslog)(INIT_FUNC_ARGS_PASSTHRU);
#endif
	PHP_RINIT(dir)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_RINIT(url_scanner_ex)(INIT_FUNC_ARGS_PASSTHRU);

	/* Setup default context */
	FG(default_context) = NULL;

	/* Default to global wrappers only */
	FG(stream_wrappers) = NULL;

	/* Default to global filters only */
	FG(stream_filters) = NULL;

	return SUCCESS;
}
/* }}} */

PHP_RSHUTDOWN_FUNCTION(basic) /* {{{ */
{
	if (BG(strtok_zval)) {
		zval_ptr_dtor(&BG(strtok_zval));
	}
	BG(strtok_string) = NULL;
	BG(strtok_zval) = NULL;
#ifdef HAVE_PUTENV
	zend_hash_destroy(&BG(putenv_ht));
#endif

	if (BG(umask) != -1) {
		umask(BG(umask));
	}

	/* Check if locale was changed and change it back
	 * to the value in startup environment */
	if (BG(locale_string) != NULL) {
		setlocale(LC_ALL, "C");
		setlocale(LC_CTYPE, "");
		zend_update_current_locale();
	}
	STR_FREE(BG(locale_string));
	BG(locale_string) = NULL;

	/* FG(stream_wrappers) and FG(stream_filters) are destroyed
	 * during php_request_shutdown() */

	PHP_RSHUTDOWN(filestat)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#ifdef HAVE_SYSLOG_H
#ifdef PHP_WIN32
	PHP_RSHUTDOWN(syslog)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#endif
#endif
	PHP_RSHUTDOWN(assert)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_RSHUTDOWN(url_scanner_ex)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_RSHUTDOWN(streams)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#ifdef PHP_WIN32
	PHP_RSHUTDOWN(win32_core_globals)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#endif

	if (BG(user_tick_functions)) {
		zend_llist_destroy(BG(user_tick_functions));
		efree(BG(user_tick_functions));
		BG(user_tick_functions) = NULL;
	}

	PHP_RSHUTDOWN(user_filters)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	PHP_RSHUTDOWN(browscap)(SHUTDOWN_FUNC_ARGS_PASSTHRU);

 	BG(page_uid) = -1;
 	BG(page_gid) = -1;
	return SUCCESS;
}
/* }}} */

PHP_MINFO_FUNCTION(basic) /* {{{ */
{
	php_info_print_table_start();
	PHP_MINFO(dl)(ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU);
	PHP_MINFO(mail)(ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU);
	php_info_print_table_end();
	PHP_MINFO(assert)(ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU);
}
/* }}} */

/* {{{ proto mixed constant(string const_name)
   Given the name of a constant this function will return the constant's associated value */
PHP_FUNCTION(constant)
{
	char *const_name;
	int const_name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &const_name, &const_name_len) == FAILURE) {
		return;
	}

	if (!zend_get_constant_ex(const_name, const_name_len, return_value, NULL, ZEND_FETCH_CLASS_SILENT TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Couldn't find constant %s", const_name);
		RETURN_NULL();
	}
}
/* }}} */

#ifdef HAVE_INET_NTOP
/* {{{ proto string inet_ntop(string in_addr)
   Converts a packed inet address to a human readable IP address string */
PHP_NAMED_FUNCTION(php_inet_ntop)
{
	char *address;
	int address_len, af = AF_INET;
	char buffer[40];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &address, &address_len) == FAILURE) {
		RETURN_FALSE;
	}

#ifdef HAVE_IPV6
	if (address_len == 16) {
		af = AF_INET6;
	} else
#endif
	if (address_len != 4) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid in_addr value");
		RETURN_FALSE;
	}

	if (!inet_ntop(af, address, buffer, sizeof(buffer))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "An unknown error occurred");
		RETURN_FALSE;
	}

	RETURN_STRING(buffer, 1);
}
/* }}} */
#endif /* HAVE_INET_NTOP */

#ifdef HAVE_INET_PTON
/* {{{ proto string inet_pton(string ip_address)
   Converts a human readable IP address to a packed binary string */
PHP_NAMED_FUNCTION(php_inet_pton)
{
	int ret, af = AF_INET;
	char *address;
	int address_len;
	char buffer[17];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &address, &address_len) == FAILURE) {
		RETURN_FALSE;
	}

	memset(buffer, 0, sizeof(buffer));

#ifdef HAVE_IPV6
	if (strchr(address, ':')) {
		af = AF_INET6;
	} else
#endif
	if (!strchr(address, '.')) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unrecognized address %s", address);
		RETURN_FALSE;
	}

	ret = inet_pton(af, address, buffer);

	if (ret <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unrecognized address %s", address);
		RETURN_FALSE;
	}

	RETURN_STRINGL(buffer, af == AF_INET ? 4 : 16, 1);
}
/* }}} */
#endif /* HAVE_INET_PTON */

/* {{{ proto int ip2long(string ip_address)
   Converts a string containing an (IPv4) Internet Protocol dotted address into a proper address */
PHP_FUNCTION(ip2long)
{
	char *addr;
	int addr_len;
#ifdef HAVE_INET_PTON
	struct in_addr ip;
#else
	unsigned long int ip;
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &addr, &addr_len) == FAILURE) {
		return;
	}

#ifdef HAVE_INET_PTON
	if (addr_len == 0 || inet_pton(AF_INET, addr, &ip) != 1) {
		RETURN_FALSE;
	}
	RETURN_LONG(ntohl(ip.s_addr));
#else
	if (addr_len == 0 || (ip = inet_addr(addr)) == INADDR_NONE) {
		/* The only special case when we should return -1 ourselves,
		 * because inet_addr() considers it wrong. We return 0xFFFFFFFF and
		 * not -1 or ~0 because of 32/64bit issues. */
		if (addr_len == sizeof("255.255.255.255") - 1 &&
			!memcmp(addr, "255.255.255.255", sizeof("255.255.255.255") - 1)
		) {
			RETURN_LONG(0xFFFFFFFF);
		}
		RETURN_FALSE;
	}
	RETURN_LONG(ntohl(ip));
#endif
}
/* }}} */

/* {{{ proto string long2ip(int proper_address)
   Converts an (IPv4) Internet network address into a string in Internet standard dotted format */
PHP_FUNCTION(long2ip)
{
	/* "It's a long but it's not, PHP ints are signed */
	char *ip;
	int ip_len;
	unsigned long n;
	struct in_addr myaddr;
#ifdef HAVE_INET_PTON
	char str[40];
#endif

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ip, &ip_len) == FAILURE) {
		return;
	}

	n = strtoul(ip, NULL, 0);

	myaddr.s_addr = htonl(n);
#ifdef HAVE_INET_PTON
	if (inet_ntop(AF_INET, &myaddr, str, sizeof(str))) {
		RETURN_STRING(str, 1);
	} else {
		RETURN_FALSE;
	}
#else
	RETURN_STRING(inet_ntoa(myaddr), 1);
#endif
}
/* }}} */

/********************
 * System Functions *
 ********************/

/* {{{ proto string getenv(string varname)
   Get the value of an environment variable */
PHP_FUNCTION(getenv)
{
	char *ptr, *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		RETURN_FALSE;
	}

	/* SAPI method returns an emalloc()'d string */
	ptr = sapi_getenv(str, str_len TSRMLS_CC);
	if (ptr) {
		RETURN_STRING(ptr, 0);
	}
#ifdef PHP_WIN32
	{
		char dummybuf;
		int size;

		SetLastError(0);
		/*If the given bugger is not large enough to hold the data, the return value is 
		the buffer size,  in characters, required to hold the string and its terminating 
		null character. We use this return value to alloc the final buffer. */
		size = GetEnvironmentVariableA(str, &dummybuf, 0);
		if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
				/* The environment variable doesn't exist. */
				RETURN_FALSE;
		}

		if (size == 0) {
				/* env exists, but it is empty */
				RETURN_EMPTY_STRING();
		}

		ptr = emalloc(size);
		size = GetEnvironmentVariableA(str, ptr, size);
		if (size == 0) {
				/* has been removed between the two calls */
				efree(ptr);
				RETURN_EMPTY_STRING();
		} else {
			RETURN_STRING(ptr, 0);
		}
	}
#else
	/* system method returns a const */
	ptr = getenv(str);
	if (ptr) {
		RETURN_STRING(ptr, 1);
	}
#endif
	RETURN_FALSE;
}
/* }}} */

#ifdef HAVE_PUTENV
/* {{{ proto bool putenv(string setting)
   Set the value of an environment variable */
PHP_FUNCTION(putenv)
{
	char *setting;
	int setting_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &setting, &setting_len) == FAILURE) {
		return;
	}

	if (setting_len) {
		char *p, **env;
		putenv_entry pe;
#ifdef PHP_WIN32
		char *value = NULL;
		int equals = 0;
		int error_code;
#endif

		pe.putenv_string = estrndup(setting, setting_len);
		pe.key = estrndup(setting, setting_len);
		if ((p = strchr(pe.key, '='))) {	/* nullify the '=' if there is one */
			*p = '\0';
#ifdef PHP_WIN32
			equals = 1;
#endif
		}

		pe.key_len = strlen(pe.key);
#ifdef PHP_WIN32
		if (equals) {
			if (pe.key_len < setting_len - 1) {
				value = p + 1;
			} else {
				/* empty string*/
				value = p;
			}
		}
#endif

		zend_hash_del(&BG(putenv_ht), pe.key, pe.key_len+1);

		/* find previous value */
		pe.previous_value = NULL;
		for (env = environ; env != NULL && *env != NULL; env++) {
			if (!strncmp(*env, pe.key, pe.key_len) && (*env)[pe.key_len] == '=') {	/* found it */
#if defined(PHP_WIN32)
				/* must copy previous value because MSVCRT's putenv can free the string without notice */
				pe.previous_value = estrdup(*env);
#else
				pe.previous_value = *env;
#endif
				break;
			}
		}

#if HAVE_UNSETENV
		if (!p) { /* no '=' means we want to unset it */
			unsetenv(pe.putenv_string);
		}
		if (!p || putenv(pe.putenv_string) == 0) { /* success */
#else
# ifndef PHP_WIN32
		if (putenv(pe.putenv_string) == 0) { /* success */
# else
		error_code = SetEnvironmentVariable(pe.key, value);
#  if _MSC_VER < 1500
		/* Yet another VC6 bug, unset may return env not found */
		if (error_code != 0 || 
			(error_code == 0 && GetLastError() == ERROR_ENVVAR_NOT_FOUND)) {
#  else
		if (error_code != 0) { /* success */
#  endif
# endif
#endif
			zend_hash_add(&BG(putenv_ht), pe.key, pe.key_len + 1, (void **) &pe, sizeof(putenv_entry), NULL);
#ifdef HAVE_TZSET
			if (!strncmp(pe.key, "TZ", pe.key_len)) {
				tzset();
			}
#endif
			RETURN_TRUE;
		} else {
			efree(pe.putenv_string);
			efree(pe.key);
			RETURN_FALSE;
		}
	}

	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parameter syntax");
	RETURN_FALSE;
}
/* }}} */
#endif

/* {{{ free_argv()
   Free the memory allocated to an argv array. */
static void free_argv(char **argv, int argc)
{
	int i;

	if (argv) {
		for (i = 0; i < argc; i++) {
			if (argv[i]) {
				efree(argv[i]);
			}
		}
		efree(argv);
	}
}
/* }}} */

/* {{{ free_longopts()
   Free the memory allocated to an longopt array. */
static void free_longopts(opt_struct *longopts)
{
	opt_struct *p;

	if (longopts) {
		for (p = longopts; p && p->opt_char != '-'; p++) {
			if (p->opt_name != NULL) {
				efree((char *)(p->opt_name));
			}
		}
	}
}
/* }}} */

/* {{{ parse_opts()
   Convert the typical getopt input characters to the php_getopt struct array */
static int parse_opts(char * opts, opt_struct ** result)
{
	opt_struct * paras = NULL;
	unsigned int i, count = 0;

	for (i = 0; i < strlen(opts); i++) {
		if ((opts[i] >= 48 && opts[i] <= 57) ||
			(opts[i] >= 65 && opts[i] <= 90) ||
			(opts[i] >= 97 && opts[i] <= 122)
		) {
			count++;
		}
	}

	paras = safe_emalloc(sizeof(opt_struct), count, 0);
	memset(paras, 0, sizeof(opt_struct) * count);
	*result = paras;
	while ( (*opts >= 48 && *opts <= 57) || /* 0 - 9 */
			(*opts >= 65 && *opts <= 90) || /* A - Z */
			(*opts >= 97 && *opts <= 122)   /* a - z */
	) {
		paras->opt_char = *opts;
		paras->need_param = (*(++opts) == ':') ? 1 : 0;
		paras->opt_name = NULL;
		if (paras->need_param == 1) {
			opts++;
			if (*opts == ':') {
				paras->need_param++;
				opts++;
			}
		}
		paras++;
	}
	return count;
}
/* }}} */

/* {{{ proto array getopt(string options [, array longopts])
   Get options from the command line argument list */
PHP_FUNCTION(getopt)
{
	char *options = NULL, **argv = NULL;
	char opt[2] = { '\0' };
	char *optname;
	int argc = 0, options_len = 0, len, o;
	char *php_optarg = NULL;
	int php_optind = 1;
	zval *val, **args = NULL, *p_longopts = NULL;
	int optname_len = 0;
	opt_struct *opts, *orig_opts;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|a", &options, &options_len, &p_longopts) == FAILURE) {
		RETURN_FALSE;
	}

	/* Get argv from the global symbol table. We calculate argc ourselves
	 * in order to be on the safe side, even though it is also available
	 * from the symbol table. */
	if (PG(http_globals)[TRACK_VARS_SERVER] &&
		(zend_hash_find(HASH_OF(PG(http_globals)[TRACK_VARS_SERVER]), "argv", sizeof("argv"), (void **) &args) != FAILURE ||
		zend_hash_find(&EG(symbol_table), "argv", sizeof("argv"), (void **) &args) != FAILURE) && Z_TYPE_PP(args) == IS_ARRAY
	) {
		int pos = 0;
		zval **entry;

		argc = zend_hash_num_elements(Z_ARRVAL_PP(args));

		/* Attempt to allocate enough memory to hold all of the arguments
		 * and a trailing NULL */
		argv = (char **) safe_emalloc(sizeof(char *), (argc + 1), 0);

		/* Reset the array indexes. */
		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(args));

		/* Iterate over the hash to construct the argv array. */
		while (zend_hash_get_current_data(Z_ARRVAL_PP(args), (void **)&entry) == SUCCESS) {
			zval arg, *arg_ptr = *entry;

			if (Z_TYPE_PP(entry) != IS_STRING) {
				arg = **entry;
				zval_copy_ctor(&arg);
				convert_to_string(&arg);
				arg_ptr = &arg;
			}

			argv[pos++] = estrdup(Z_STRVAL_P(arg_ptr));

			if (arg_ptr != *entry) {
				zval_dtor(&arg);
			}

			zend_hash_move_forward(Z_ARRVAL_PP(args));
		}

		/* The C Standard requires argv[argc] to be NULL - this might
		 * keep some getopt implementations happy. */
		argv[argc] = NULL;
	} else {
		/* Return false if we can't find argv. */
		RETURN_FALSE;
	}

	len = parse_opts(options, &opts);

	if (p_longopts) {
		int count;
		zval **entry;

		count = zend_hash_num_elements(Z_ARRVAL_P(p_longopts));

		/* the first <len> slots are filled by the one short ops
		 * we now extend our array and jump to the new added structs */
		opts = (opt_struct *) erealloc(opts, sizeof(opt_struct) * (len + count + 1));
		orig_opts = opts;
		opts += len;

		memset(opts, 0, count * sizeof(opt_struct));

		/* Reset the array indexes. */
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(p_longopts));

		/* Iterate over the hash to construct the argv array. */
		while (zend_hash_get_current_data(Z_ARRVAL_P(p_longopts), (void **)&entry) == SUCCESS) {
			zval arg, *arg_ptr = *entry;

			if (Z_TYPE_PP(entry) != IS_STRING) {
				arg = **entry;
				zval_copy_ctor(&arg);
				convert_to_string(&arg);
				arg_ptr = &arg;
			}

			opts->need_param = 0;
			opts->opt_name = estrdup(Z_STRVAL_P(arg_ptr));
			len = strlen(opts->opt_name);
			if ((len > 0) && (opts->opt_name[len - 1] == ':')) {
				opts->need_param++;
				opts->opt_name[len - 1] = '\0';
				if ((len > 1) && (opts->opt_name[len - 2] == ':')) {
					opts->need_param++;
					opts->opt_name[len - 2] = '\0';
				}
			}
			opts->opt_char = 0;
			opts++;

			if (arg_ptr != *entry) {
				zval_dtor(&arg);
			}

			zend_hash_move_forward(Z_ARRVAL_P(p_longopts));
		}
	} else {
		opts = (opt_struct*) erealloc(opts, sizeof(opt_struct) * (len + 1));
		orig_opts = opts;
		opts += len;
	}

	/* php_getopt want to identify the last param */
	opts->opt_char   = '-';
	opts->need_param = 0;
	opts->opt_name   = NULL;

	/* Initialize the return value as an array. */
	array_init(return_value);

	/* after our pointer arithmetic jump back to the first element */
	opts = orig_opts;

	while ((o = php_getopt(argc, argv, opts, &php_optarg, &php_optind, 0, 1)) != -1) {
		/* Skip unknown arguments. */
		if (o == '?') {
			continue;
		}

		/* Prepare the option character and the argument string. */
		if (o == 0) {
			optname = opts[php_optidx].opt_name;
		} else {
			if (o == 1) {
				o = '-';
			}
			opt[0] = o;
			optname = opt;
		}

		MAKE_STD_ZVAL(val);
		if (php_optarg != NULL) {
			/* keep the arg as binary, since the encoding is not known */
			ZVAL_STRING(val, php_optarg, 1);
		} else {
			ZVAL_FALSE(val);
		}

		/* Add this option / argument pair to the result hash. */
		optname_len = strlen(optname);
		if (!(optname_len > 1 && optname[0] == '0') && is_numeric_string(optname, optname_len, NULL, NULL, 0) == IS_LONG) {
			/* numeric string */
			int optname_int = atoi(optname);
			if (zend_hash_index_find(HASH_OF(return_value), optname_int, (void **)&args) != FAILURE) {
				if (Z_TYPE_PP(args) != IS_ARRAY) {
					convert_to_array_ex(args);
				}
				zend_hash_next_index_insert(HASH_OF(*args), (void *)&val, sizeof(zval *), NULL);
			} else {
				zend_hash_index_update(HASH_OF(return_value), optname_int, &val, sizeof(zval *), NULL);
			}
		} else {
			/* other strings */
			if (zend_hash_find(HASH_OF(return_value), optname, strlen(optname)+1, (void **)&args) != FAILURE) {
				if (Z_TYPE_PP(args) != IS_ARRAY) {
					convert_to_array_ex(args);
				}
				zend_hash_next_index_insert(HASH_OF(*args), (void *)&val, sizeof(zval *), NULL);
			} else {
				zend_hash_add(HASH_OF(return_value), optname, strlen(optname)+1, (void *)&val, sizeof(zval *), NULL);
			}
		}

		php_optarg = NULL;
	}

	free_longopts(orig_opts);
	efree(orig_opts);
	free_argv(argv, argc);
}
/* }}} */

/* {{{ proto void flush(void)
   Flush the output buffer */
PHP_FUNCTION(flush)
{
	sapi_flush(TSRMLS_C);
}
/* }}} */

/* {{{ proto void sleep(int seconds)
   Delay for a given number of seconds */
PHP_FUNCTION(sleep)
{
	long num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &num) == FAILURE) {
		RETURN_FALSE;
	}
	if (num < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of seconds must be greater than or equal to 0");
		RETURN_FALSE;
	}
#ifdef PHP_SLEEP_NON_VOID
	RETURN_LONG(php_sleep(num));
#else
	php_sleep(num);
#endif

}
/* }}} */

/* {{{ proto void usleep(int micro_seconds)
   Delay for a given number of micro seconds */
PHP_FUNCTION(usleep)
{
#if HAVE_USLEEP
	long num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &num) == FAILURE) {
		return;
	}
	if (num < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of microseconds must be greater than or equal to 0");
		RETURN_FALSE;
	}
	usleep(num);
#endif
}
/* }}} */

#if HAVE_NANOSLEEP
/* {{{ proto mixed time_nanosleep(long seconds, long nanoseconds)
   Delay for a number of seconds and nano seconds */
PHP_FUNCTION(time_nanosleep)
{
	long tv_sec, tv_nsec;
	struct timespec php_req, php_rem;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &tv_sec, &tv_nsec) == FAILURE) {
		return;
	}

	if (tv_sec < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The seconds value must be greater than 0");
		RETURN_FALSE;
	}
	if (tv_nsec < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The nanoseconds value must be greater than 0");
		RETURN_FALSE;
	}

	php_req.tv_sec = (time_t) tv_sec;
	php_req.tv_nsec = tv_nsec;
	if (!nanosleep(&php_req, &php_rem)) {
		RETURN_TRUE;
	} else if (errno == EINTR) {
		array_init(return_value);
		add_assoc_long_ex(return_value, "seconds", sizeof("seconds"), php_rem.tv_sec);
		add_assoc_long_ex(return_value, "nanoseconds", sizeof("nanoseconds"), php_rem.tv_nsec);
		return;
	} else if (errno == EINVAL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "nanoseconds was not in the range 0 to 999 999 999 or seconds was negative");
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto mixed time_sleep_until(float timestamp)
   Make the script sleep until the specified time */
PHP_FUNCTION(time_sleep_until)
{
	double d_ts, c_ts;
	struct timeval tm;
	struct timespec php_req, php_rem;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &d_ts) == FAILURE) {
		return;
	}

	if (gettimeofday((struct timeval *) &tm, NULL) != 0) {
		RETURN_FALSE;
	}

	c_ts = (double)(d_ts - tm.tv_sec - tm.tv_usec / 1000000.00);
	if (c_ts < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Sleep until to time is less than current time");
		RETURN_FALSE;
	}

	php_req.tv_sec = (time_t) c_ts;
	if (php_req.tv_sec > c_ts) { /* rounding up occurred */
		php_req.tv_sec--;
	}
	/* 1sec = 1000000000 nanoseconds */
	php_req.tv_nsec = (long) ((c_ts - php_req.tv_sec) * 1000000000.00);

	while (nanosleep(&php_req, &php_rem)) {
		if (errno == EINTR) {
			php_req.tv_sec = php_rem.tv_sec;
			php_req.tv_nsec = php_rem.tv_nsec;
		} else {
			RETURN_FALSE;
		}
	}

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto string get_current_user(void)
   Get the name of the owner of the current PHP script */
PHP_FUNCTION(get_current_user)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_STRING(php_get_current_user(TSRMLS_C), 1);
}
/* }}} */

/* {{{ add_config_entry_cb
 */
static int add_config_entry_cb(zval *entry TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *retval = (zval *)va_arg(args, zval*);
	zval *tmp;

	if (Z_TYPE_P(entry) == IS_STRING) {
		if (hash_key->nKeyLength > 0) {
			add_assoc_stringl_ex(retval, hash_key->arKey, hash_key->nKeyLength, Z_STRVAL_P(entry), Z_STRLEN_P(entry), 1);
		} else {
			add_index_stringl(retval, hash_key->h, Z_STRVAL_P(entry), Z_STRLEN_P(entry), 1);
		}
	} else if (Z_TYPE_P(entry) == IS_ARRAY) {
		MAKE_STD_ZVAL(tmp);
		array_init(tmp);
		zend_hash_apply_with_arguments(Z_ARRVAL_P(entry) TSRMLS_CC, (apply_func_args_t) add_config_entry_cb, 1, tmp);
		add_assoc_zval_ex(retval, hash_key->arKey, hash_key->nKeyLength, tmp);
	}
	return 0;
}
/* }}} */

/* {{{ proto mixed get_cfg_var(string option_name)
   Get the value of a PHP configuration option */
PHP_FUNCTION(get_cfg_var)
{
	char *varname;
	int varname_len;
	zval *retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &varname, &varname_len) == FAILURE) {
		return;
	}

	retval = cfg_get_entry(varname, varname_len + 1);

	if (retval) {
		if (Z_TYPE_P(retval) == IS_ARRAY) {
			array_init(return_value);
			zend_hash_apply_with_arguments(Z_ARRVAL_P(retval) TSRMLS_CC, (apply_func_args_t) add_config_entry_cb, 1, return_value);
			return;
		} else {
			RETURN_STRING(Z_STRVAL_P(retval), 1);
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool set_magic_quotes_runtime(int new_setting)
   magic_quotes_runtime is not supported anymore */
PHP_FUNCTION(set_magic_quotes_runtime)
{
	zend_bool new_setting;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &new_setting) == FAILURE) {
		return;
	}
	
	if (new_setting) {
		php_error_docref(NULL TSRMLS_CC, E_CORE_ERROR, "magic_quotes_runtime is not supported anymore");
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int get_magic_quotes_runtime(void)
   Get the current active configuration setting of magic_quotes_runtime */
PHP_FUNCTION(get_magic_quotes_runtime)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto int get_magic_quotes_gpc(void)
   Get the current active configuration setting of magic_quotes_gpc */
PHP_FUNCTION(get_magic_quotes_gpc)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	RETURN_FALSE;
}
/* }}} */

/*
	1st arg = error message
	2nd arg = error option
	3rd arg = optional parameters (email address or tcp address)
	4th arg = used for additional headers if email

error options:
	0 = send to php_error_log (uses syslog or file depending on ini setting)
	1 = send via email to 3rd parameter 4th option = additional headers
	2 = send via tcp/ip to 3rd parameter (name or ip:port)
	3 = save to file in 3rd parameter
	4 = send to SAPI logger directly
*/

/* {{{ proto bool error_log(string message [, int message_type [, string destination [, string extra_headers]]])
   Send an error message somewhere */
PHP_FUNCTION(error_log)
{
	char *message, *opt = NULL, *headers = NULL;
	int message_len, opt_len = 0, headers_len = 0;
	int opt_err = 0, argc = ZEND_NUM_ARGS();
	long erropt = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "s|lps", &message, &message_len, &erropt, &opt, &opt_len, &headers, &headers_len) == FAILURE) {
		return;
	}

	if (argc > 1) {
		opt_err = erropt;
	}

	if (_php_error_log_ex(opt_err, message, message_len, opt, headers TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* For BC (not binary-safe!) */
PHPAPI int _php_error_log(int opt_err, char *message, char *opt, char *headers TSRMLS_DC) /* {{{ */
{
	return _php_error_log_ex(opt_err, message, (opt_err == 3) ? strlen(message) : 0, opt, headers TSRMLS_CC);
}
/* }}} */

PHPAPI int _php_error_log_ex(int opt_err, char *message, int message_len, char *opt, char *headers TSRMLS_DC) /* {{{ */
{
	php_stream *stream = NULL;

	switch (opt_err)
	{
		case 1:		/*send an email */
			if (!php_mail(opt, "PHP error_log message", message, headers, NULL TSRMLS_CC)) {
				return FAILURE;
			}
			break;

		case 2:		/*send to an address */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "TCP/IP option not available!");
			return FAILURE;
			break;

		case 3:		/*save to a file */
			stream = php_stream_open_wrapper(opt, "a", IGNORE_URL_WIN | REPORT_ERRORS, NULL);
			if (!stream) {
				return FAILURE;
			}
			php_stream_write(stream, message, message_len);
			php_stream_close(stream);
			break;

		case 4: /* send to SAPI */
			if (sapi_module.log_message) {
				sapi_module.log_message(message TSRMLS_CC);
			} else {
				return FAILURE;
			}
			break;

		default:
			php_log_err(message TSRMLS_CC);
			break;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ proto array error_get_last()
   Get the last occurred error as associative array. Returns NULL if there hasn't been an error yet. */
PHP_FUNCTION(error_get_last)
{
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	if (PG(last_error_message)) {
		array_init(return_value);
		add_assoc_long_ex(return_value, "type", sizeof("type"), PG(last_error_type));
		add_assoc_string_ex(return_value, "message", sizeof("message"), PG(last_error_message), 1);
		add_assoc_string_ex(return_value, "file", sizeof("file"), PG(last_error_file)?PG(last_error_file):"-", 1 );
		add_assoc_long_ex(return_value, "line", sizeof("line"), PG(last_error_lineno));
	}
}
/* }}} */

/* {{{ proto mixed call_user_func(mixed function_name [, mixed parmeter] [, mixed ...])
   Call a user function which is the first parameter */
PHP_FUNCTION(call_user_func)
{
	zval *retval_ptr = NULL;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f*", &fci, &fci_cache, &fci.params, &fci.param_count) == FAILURE) {
		return;
	}

	fci.retval_ptr_ptr = &retval_ptr;

	if (zend_call_function(&fci, &fci_cache TSRMLS_CC) == SUCCESS && fci.retval_ptr_ptr && *fci.retval_ptr_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, *fci.retval_ptr_ptr);
	}

	if (fci.params) {
		efree(fci.params);
	}
}
/* }}} */

/* {{{ proto mixed call_user_func_array(string function_name, array parameters)
   Call a user function which is the first parameter with the arguments contained in array */
PHP_FUNCTION(call_user_func_array)
{
	zval *params, *retval_ptr = NULL;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "fa/", &fci, &fci_cache, &params) == FAILURE) {
		return;
	}

	zend_fcall_info_args(&fci, params TSRMLS_CC);
	fci.retval_ptr_ptr = &retval_ptr;

	if (zend_call_function(&fci, &fci_cache TSRMLS_CC) == SUCCESS && fci.retval_ptr_ptr && *fci.retval_ptr_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, *fci.retval_ptr_ptr);
	}

	zend_fcall_info_args_clear(&fci, 1);
}
/* }}} */

/* {{{ proto mixed call_user_method(string method_name, mixed object [, mixed parameter] [, mixed ...])
   Call a user method on a specific object or class */
PHP_FUNCTION(call_user_method)
{
	zval ***params = NULL;
	int n_params = 0;
	zval *retval_ptr;
	zval *callback, *object;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z/z*", &callback, &object, &params, &n_params) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(object) != IS_OBJECT &&
		Z_TYPE_P(object) != IS_STRING
	) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Second argument is not an object or class name");
		if (params) {
			efree(params);
		}
		RETURN_FALSE;
	}

	convert_to_string(callback);

	if (call_user_function_ex(EG(function_table), &object, callback, &retval_ptr, n_params, params, 0, NULL TSRMLS_CC) == SUCCESS) {
		if (retval_ptr) {
			COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call %s()", Z_STRVAL_P(callback));
	}
	if (n_params) {
		efree(params);
	}
}
/* }}} */

/* {{{ proto mixed call_user_method_array(string method_name, mixed object, array params)
   Call a user method on a specific object or class using a parameter array */
PHP_FUNCTION(call_user_method_array)
{
	zval *params, ***method_args = NULL, *retval_ptr;
	zval *callback, *object;
	HashTable *params_ar;
	int num_elems, element = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z/zA/", &callback, &object, &params) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(object) != IS_OBJECT &&
		Z_TYPE_P(object) != IS_STRING
	) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Second argument is not an object or class name");
		RETURN_FALSE;
	}

	convert_to_string(callback);

	params_ar = HASH_OF(params);
	num_elems = zend_hash_num_elements(params_ar);
	method_args = (zval ***) safe_emalloc(sizeof(zval **), num_elems, 0);

	for (zend_hash_internal_pointer_reset(params_ar);
		zend_hash_get_current_data(params_ar, (void **) &(method_args[element])) == SUCCESS;
		zend_hash_move_forward(params_ar)
	) {
		element++;
	}

	if (call_user_function_ex(EG(function_table), &object, callback, &retval_ptr, num_elems, method_args, 0, NULL TSRMLS_CC) == SUCCESS) {
		if (retval_ptr) {
			COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call %s()", Z_STRVAL_P(callback));
	}

	efree(method_args);
}
/* }}} */

/* {{{ proto mixed forward_static_call(mixed function_name [, mixed parmeter] [, mixed ...]) U
   Call a user function which is the first parameter */
PHP_FUNCTION(forward_static_call)
{
	zval *retval_ptr = NULL;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f*", &fci, &fci_cache, &fci.params, &fci.param_count) == FAILURE) {
		return;
	}

	if (!EG(active_op_array)->scope) {
		zend_error(E_ERROR, "Cannot call forward_static_call() when no class scope is active");
	}

	fci.retval_ptr_ptr = &retval_ptr;

	if (EG(called_scope) &&
		instanceof_function(EG(called_scope), fci_cache.calling_scope TSRMLS_CC)) {
			fci_cache.called_scope = EG(called_scope);
	}
	
	if (zend_call_function(&fci, &fci_cache TSRMLS_CC) == SUCCESS && fci.retval_ptr_ptr && *fci.retval_ptr_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, *fci.retval_ptr_ptr);
	}

	if (fci.params) {
		efree(fci.params);
	}
}
/* }}} */

/* {{{ proto mixed call_user_func_array(string function_name, array parameters) U
   Call a user function which is the first parameter with the arguments contained in array */
PHP_FUNCTION(forward_static_call_array)
{
	zval *params, *retval_ptr = NULL;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "fa/", &fci, &fci_cache, &params) == FAILURE) {
		return;
	}

	zend_fcall_info_args(&fci, params TSRMLS_CC);
	fci.retval_ptr_ptr = &retval_ptr;

	if (EG(called_scope) &&
		instanceof_function(EG(called_scope), fci_cache.calling_scope TSRMLS_CC)) {
			fci_cache.called_scope = EG(called_scope);
	}

	if (zend_call_function(&fci, &fci_cache TSRMLS_CC) == SUCCESS && fci.retval_ptr_ptr && *fci.retval_ptr_ptr) {
		COPY_PZVAL_TO_ZVAL(*return_value, *fci.retval_ptr_ptr);
	}

	zend_fcall_info_args_clear(&fci, 1);
}
/* }}} */

void user_shutdown_function_dtor(php_shutdown_function_entry *shutdown_function_entry) /* {{{ */
{
	int i;

	for (i = 0; i < shutdown_function_entry->arg_count; i++) {
		zval_ptr_dtor(&shutdown_function_entry->arguments[i]);
	}
	efree(shutdown_function_entry->arguments);
}
/* }}} */

void user_tick_function_dtor(user_tick_function_entry *tick_function_entry) /* {{{ */
{
	int i;

	for (i = 0; i < tick_function_entry->arg_count; i++) {
		zval_ptr_dtor(&tick_function_entry->arguments[i]);
	}
	efree(tick_function_entry->arguments);
}
/* }}} */

static int user_shutdown_function_call(php_shutdown_function_entry *shutdown_function_entry TSRMLS_DC) /* {{{ */
{
	zval retval;
	char *function_name;

	if (!zend_is_callable(shutdown_function_entry->arguments[0], 0, &function_name TSRMLS_CC)) {
		php_error(E_WARNING, "(Registered shutdown functions) Unable to call %s() - function does not exist", function_name);
		if (function_name) {
			efree(function_name);
		}
		return 0;
	}
	if (function_name) {
		efree(function_name);
	}

	if (call_user_function(EG(function_table), NULL,
				shutdown_function_entry->arguments[0],
				&retval,
				shutdown_function_entry->arg_count - 1,
				shutdown_function_entry->arguments + 1
				TSRMLS_CC ) == SUCCESS)
	{
		zval_dtor(&retval);
	}
	return 0;
}
/* }}} */

static void user_tick_function_call(user_tick_function_entry *tick_fe TSRMLS_DC) /* {{{ */
{
	zval retval;
	zval *function = tick_fe->arguments[0];

	/* Prevent reentrant calls to the same user ticks function */
	if (! tick_fe->calling) {
		tick_fe->calling = 1;

		if (call_user_function(	EG(function_table), NULL,
								function,
								&retval,
								tick_fe->arg_count - 1,
								tick_fe->arguments + 1
								TSRMLS_CC) == SUCCESS) {
			zval_dtor(&retval);

		} else {
			zval **obj, **method;

			if (Z_TYPE_P(function) == IS_STRING) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call %s() - function does not exist", Z_STRVAL_P(function));
			} else if (	Z_TYPE_P(function) == IS_ARRAY
						&& zend_hash_index_find(Z_ARRVAL_P(function), 0, (void **) &obj) == SUCCESS
						&& zend_hash_index_find(Z_ARRVAL_P(function), 1, (void **) &method) == SUCCESS
						&& Z_TYPE_PP(obj) == IS_OBJECT
						&& Z_TYPE_PP(method) == IS_STRING) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call %s::%s() - function does not exist", Z_OBJCE_PP(obj)->name, Z_STRVAL_PP(method));
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call tick function");
			}
		}

		tick_fe->calling = 0;
	}
}
/* }}} */

static void run_user_tick_functions(int tick_count) /* {{{ */
{
	TSRMLS_FETCH();

	zend_llist_apply(BG(user_tick_functions), (llist_apply_func_t) user_tick_function_call TSRMLS_CC);
}
/* }}} */

static int user_tick_function_compare(user_tick_function_entry * tick_fe1, user_tick_function_entry * tick_fe2) /* {{{ */
{
	zval *func1 = tick_fe1->arguments[0];
	zval *func2 = tick_fe2->arguments[0];
	int ret;
	TSRMLS_FETCH();

	if (Z_TYPE_P(func1) == IS_STRING && Z_TYPE_P(func2) == IS_STRING) {
		ret = (zend_binary_zval_strcmp(func1, func2) == 0);
	} else if (Z_TYPE_P(func1) == IS_ARRAY && Z_TYPE_P(func2) == IS_ARRAY) {
		zval result;
		zend_compare_arrays(&result, func1, func2 TSRMLS_CC);
		ret = (Z_LVAL(result) == 0);
	} else if (Z_TYPE_P(func1) == IS_OBJECT && Z_TYPE_P(func2) == IS_OBJECT) {
		zval result;
		zend_compare_objects(&result, func1, func2 TSRMLS_CC);
		ret = (Z_LVAL(result) == 0);
	} else {
		ret = 0;
	}

	if (ret && tick_fe1->calling) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to delete tick function executed at the moment");
		return 0;
	}
	return ret;
}
/* }}} */

void php_call_shutdown_functions(TSRMLS_D) /* {{{ */
{
	if (BG(user_shutdown_function_names)) {
		zend_try {
			zend_hash_apply(BG(user_shutdown_function_names), (apply_func_t) user_shutdown_function_call TSRMLS_CC);
		}
		zend_end_try();
		php_free_shutdown_functions(TSRMLS_C);
	}
}
/* }}} */

void php_free_shutdown_functions(TSRMLS_D) /* {{{ */
{
	if (BG(user_shutdown_function_names))
		zend_try {
			zend_hash_destroy(BG(user_shutdown_function_names));
			FREE_HASHTABLE(BG(user_shutdown_function_names));
			BG(user_shutdown_function_names) = NULL;
		} zend_catch {
			/* maybe shutdown method call exit, we just ignore it */
			FREE_HASHTABLE(BG(user_shutdown_function_names));
			BG(user_shutdown_function_names) = NULL;
		} zend_end_try();
}
/* }}} */

/* {{{ proto void register_shutdown_function(string function_name) U
   Register a user-level function to be called on request termination */
PHP_FUNCTION(register_shutdown_function)
{
	php_shutdown_function_entry shutdown_function_entry;
	char *function_name = NULL;
	int i;

	shutdown_function_entry.arg_count = ZEND_NUM_ARGS();

	if (shutdown_function_entry.arg_count < 1) {
		WRONG_PARAM_COUNT;
	}

	shutdown_function_entry.arguments = (zval **) safe_emalloc(sizeof(zval *), shutdown_function_entry.arg_count, 0);

	if (zend_get_parameters_array(ht, shutdown_function_entry.arg_count, shutdown_function_entry.arguments) == FAILURE) {
		efree(shutdown_function_entry.arguments);
		RETURN_FALSE;
	}

	/* Prevent entering of anything but valid callback (syntax check only!) */
	if (!zend_is_callable(shutdown_function_entry.arguments[0], 0, &function_name TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid shutdown callback '%s' passed", function_name);
		efree(shutdown_function_entry.arguments);
		RETVAL_FALSE;
	} else {
		if (!BG(user_shutdown_function_names)) {
			ALLOC_HASHTABLE(BG(user_shutdown_function_names));
			zend_hash_init(BG(user_shutdown_function_names), 0, NULL, (void (*)(void *)) user_shutdown_function_dtor, 0);
		}

		for (i = 0; i < shutdown_function_entry.arg_count; i++) {
			Z_ADDREF_P(shutdown_function_entry.arguments[i]);
		}
		zend_hash_next_index_insert(BG(user_shutdown_function_names), &shutdown_function_entry, sizeof(php_shutdown_function_entry), NULL);
	}
	if (function_name) {
		efree(function_name);
	}
}
/* }}} */

PHPAPI zend_bool register_user_shutdown_function(char *function_name, size_t function_len, php_shutdown_function_entry *shutdown_function_entry TSRMLS_DC) /* {{{ */
{
	if (!BG(user_shutdown_function_names)) {
		ALLOC_HASHTABLE(BG(user_shutdown_function_names));
		zend_hash_init(BG(user_shutdown_function_names), 0, NULL, (void (*)(void *)) user_shutdown_function_dtor, 0);
	}

	return zend_hash_update(BG(user_shutdown_function_names), function_name, function_len, shutdown_function_entry, sizeof(php_shutdown_function_entry), NULL) != FAILURE;
}
/* }}} */

PHPAPI zend_bool remove_user_shutdown_function(char *function_name, size_t function_len TSRMLS_DC) /* {{{ */
{
	if (BG(user_shutdown_function_names)) {
		return zend_hash_del_key_or_index(BG(user_shutdown_function_names), function_name, function_len, 0, HASH_DEL_KEY) != FAILURE;
	}

	return 0;
}
/* }}} */

PHPAPI zend_bool append_user_shutdown_function(php_shutdown_function_entry shutdown_function_entry TSRMLS_DC) /* {{{ */
{
	if (!BG(user_shutdown_function_names)) {
		ALLOC_HASHTABLE(BG(user_shutdown_function_names));
		zend_hash_init(BG(user_shutdown_function_names), 0, NULL, (void (*)(void *)) user_shutdown_function_dtor, 0);
	}

	return zend_hash_next_index_insert(BG(user_shutdown_function_names), &shutdown_function_entry, sizeof(php_shutdown_function_entry), NULL) != FAILURE;
}
/* }}} */

ZEND_API void php_get_highlight_struct(zend_syntax_highlighter_ini *syntax_highlighter_ini) /* {{{ */
{
	syntax_highlighter_ini->highlight_comment = INI_STR("highlight.comment");
	syntax_highlighter_ini->highlight_default = INI_STR("highlight.default");
	syntax_highlighter_ini->highlight_html    = INI_STR("highlight.html");
	syntax_highlighter_ini->highlight_keyword = INI_STR("highlight.keyword");
	syntax_highlighter_ini->highlight_string  = INI_STR("highlight.string");
}
/* }}} */

/* {{{ proto bool highlight_file(string file_name [, bool return] )
   Syntax highlight a source file */
PHP_FUNCTION(highlight_file)
{
	char *filename;
	int filename_len, ret;
	zend_syntax_highlighter_ini syntax_highlighter_ini;
	zend_bool i = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|b", &filename, &filename_len, &i) == FAILURE) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(filename TSRMLS_CC)) {
		RETURN_FALSE;
	}

	if (i) {
		php_output_start_default(TSRMLS_C);
	}

	php_get_highlight_struct(&syntax_highlighter_ini);

	ret = highlight_file(filename, &syntax_highlighter_ini TSRMLS_CC);

	if (ret == FAILURE) {
		if (i) {
			php_output_end(TSRMLS_C);
		}
		RETURN_FALSE;
	}

	if (i) {
		php_output_get_contents(return_value TSRMLS_CC);
		php_output_discard(TSRMLS_C);
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto string php_strip_whitespace(string file_name)
   Return source with stripped comments and whitespace */
PHP_FUNCTION(php_strip_whitespace)
{
	char *filename;
	int filename_len;
	zend_lex_state original_lex_state;
	zend_file_handle file_handle = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p", &filename, &filename_len) == FAILURE) {
		RETURN_FALSE;
	}

	php_output_start_default(TSRMLS_C);

	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.filename = filename;
	file_handle.free_filename = 0;
	file_handle.opened_path = NULL;
	zend_save_lexical_state(&original_lex_state TSRMLS_CC);
	if (open_file_for_scanning(&file_handle TSRMLS_CC) == FAILURE) {
		zend_restore_lexical_state(&original_lex_state TSRMLS_CC);
		php_output_end(TSRMLS_C);
		RETURN_EMPTY_STRING();
	}

	zend_strip(TSRMLS_C);

	zend_destroy_file_handle(&file_handle TSRMLS_CC);
	zend_restore_lexical_state(&original_lex_state TSRMLS_CC);

	php_output_get_contents(return_value TSRMLS_CC);
	php_output_discard(TSRMLS_C);
}
/* }}} */

/* {{{ proto bool highlight_string(string string [, bool return] )
   Syntax highlight a string or optionally return it */
PHP_FUNCTION(highlight_string)
{
	zval **expr;
	zend_syntax_highlighter_ini syntax_highlighter_ini;
	char *hicompiled_string_description;
	zend_bool i = 0;
	int old_error_reporting = EG(error_reporting);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|b", &expr, &i) == FAILURE) {
		RETURN_FALSE;
	}
	convert_to_string_ex(expr);

	if (i) {
		php_output_start_default(TSRMLS_C);
	}

	EG(error_reporting) = E_ERROR;

	php_get_highlight_struct(&syntax_highlighter_ini);

	hicompiled_string_description = zend_make_compiled_string_description("highlighted code" TSRMLS_CC);

	if (highlight_string(*expr, &syntax_highlighter_ini, hicompiled_string_description TSRMLS_CC) == FAILURE) {
		efree(hicompiled_string_description);
		EG(error_reporting) = old_error_reporting;
		if (i) {
			php_output_end(TSRMLS_C);
		}
		RETURN_FALSE;
	}
	efree(hicompiled_string_description);

	EG(error_reporting) = old_error_reporting;

	if (i) {
		php_output_get_contents(return_value TSRMLS_CC);
		php_output_discard(TSRMLS_C);
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto string ini_get(string varname)
   Get a configuration option */
PHP_FUNCTION(ini_get)
{
	char *varname, *str;
	int varname_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &varname, &varname_len) == FAILURE) {
		return;
	}

	str = zend_ini_string(varname, varname_len + 1, 0);

	if (!str) {
		RETURN_FALSE;
	}

	RETURN_STRING(str, 1);
}
/* }}} */

static int php_ini_get_option(zend_ini_entry *ini_entry TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	zval *ini_array = va_arg(args, zval *);
	int module_number = va_arg(args, int);
	int details = va_arg(args, int);
	zval *option;

	if (module_number != 0 && ini_entry->module_number != module_number) {
		return 0;
	}

	if (hash_key->nKeyLength == 0 ||
		hash_key->arKey[0] != 0
	) {
		if (details) {
			MAKE_STD_ZVAL(option);
			array_init(option);

			if (ini_entry->orig_value) {
				add_assoc_stringl(option, "global_value", ini_entry->orig_value, ini_entry->orig_value_length, 1);
			} else if (ini_entry->value) {
				add_assoc_stringl(option, "global_value", ini_entry->value, ini_entry->value_length, 1);
			} else {
				add_assoc_null(option, "global_value");
			}

			if (ini_entry->value) {
				add_assoc_stringl(option, "local_value", ini_entry->value, ini_entry->value_length, 1);
			} else {
				add_assoc_null(option, "local_value");
			}

			add_assoc_long(option, "access", ini_entry->modifiable);

			add_assoc_zval_ex(ini_array, ini_entry->name, ini_entry->name_length, option);
		} else {
			if (ini_entry->value) {
				add_assoc_stringl(ini_array, ini_entry->name, ini_entry->value, ini_entry->value_length, 1);
			} else {
				add_assoc_null(ini_array, ini_entry->name);
			}
		}
	}
	return 0;
}
/* }}} */

/* {{{ proto array ini_get_all([string extension[, bool details = true]])
   Get all configuration options */
PHP_FUNCTION(ini_get_all)
{
	char *extname = NULL;
	int extname_len = 0, extnumber = 0;
	zend_module_entry *module;
	zend_bool details = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!b", &extname, &extname_len, &details) == FAILURE) {
		return;
	}

	zend_ini_sort_entries(TSRMLS_C);

	if (extname) {
		if (zend_hash_find(&module_registry, extname, extname_len+1, (void **) &module) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to find extension '%s'", extname);
			RETURN_FALSE;
		}
		extnumber = module->module_number;
	}

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(ini_directives) TSRMLS_CC, (apply_func_args_t) php_ini_get_option, 2, return_value, extnumber, details);
}
/* }}} */

static int php_ini_check_path(char *option_name, int option_len, char *new_option_name, int new_option_len) /* {{{ */
{
	if (option_len != (new_option_len - 1)) {
		return 0;
	}

	return !strncmp(option_name, new_option_name, option_len);
}
/* }}} */

/* {{{ proto string ini_set(string varname, string newvalue)
   Set a configuration option, returns false on error and the old value of the configuration option on success */
PHP_FUNCTION(ini_set)
{
	char *varname, *new_value;
	int varname_len, new_value_len;
	char *old_value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &varname, &varname_len, &new_value, &new_value_len) == FAILURE) {
		return;
	}

	old_value = zend_ini_string(varname, varname_len + 1, 0);

	/* copy to return here, because alter might free it! */
	if (old_value) {
		RETVAL_STRING(old_value, 1);
	} else {
		RETVAL_FALSE;
	}

#define _CHECK_PATH(var, var_len, ini) php_ini_check_path(var, var_len, ini, sizeof(ini))
	/* open basedir check */
	if (PG(open_basedir)) {
		if (_CHECK_PATH(varname, varname_len, "error_log") ||
			_CHECK_PATH(varname, varname_len, "java.class.path") ||
			_CHECK_PATH(varname, varname_len, "java.home") ||
			_CHECK_PATH(varname, varname_len, "mail.log") ||
			_CHECK_PATH(varname, varname_len, "java.library.path") ||
			_CHECK_PATH(varname, varname_len, "vpopmail.directory")) {
			if (php_check_open_basedir(new_value TSRMLS_CC)) {
				zval_dtor(return_value);
				RETURN_FALSE;
			}
		}
	}

	if (zend_alter_ini_entry_ex(varname, varname_len + 1, new_value, new_value_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0 TSRMLS_CC) == FAILURE) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void ini_restore(string varname)
   Restore the value of a configuration option specified by varname */
PHP_FUNCTION(ini_restore)
{
	char *varname;
	int varname_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &varname, &varname_len) == FAILURE) {
		return;
	}

	zend_restore_ini_entry(varname, varname_len+1, PHP_INI_STAGE_RUNTIME);
}
/* }}} */

/* {{{ proto string set_include_path(string new_include_path)
   Sets the include_path configuration option */
PHP_FUNCTION(set_include_path)
{
	char *new_value;
	int new_value_len;
	char *old_value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &new_value, &new_value_len) == FAILURE) {
		return;
	}

	old_value = zend_ini_string("include_path", sizeof("include_path"), 0);
	/* copy to return here, because alter might free it! */
	if (old_value) {
		RETVAL_STRING(old_value, 1);
	} else {
		RETVAL_FALSE;
	}

	if (zend_alter_ini_entry_ex("include_path", sizeof("include_path"), new_value, new_value_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0 TSRMLS_CC) == FAILURE) {
		zval_dtor(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string get_include_path()
   Get the current include_path configuration option */
PHP_FUNCTION(get_include_path)
{
	char *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	str = zend_ini_string("include_path", sizeof("include_path"), 0);

	if (str == NULL) {
		RETURN_FALSE;
	}

	RETURN_STRING(str, 1);
}
/* }}} */

/* {{{ proto void restore_include_path()
   Restore the value of the include_path configuration option */
PHP_FUNCTION(restore_include_path)
{
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}
	zend_restore_ini_entry("include_path", sizeof("include_path"), PHP_INI_STAGE_RUNTIME);
}
/* }}} */

/* {{{ proto mixed print_r(mixed var [, bool return])
   Prints out or returns information about the specified variable */
PHP_FUNCTION(print_r)
{
	zval *var;
	zend_bool do_return = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &var, &do_return) == FAILURE) {
		RETURN_FALSE;
	}

	if (do_return) {
		php_output_start_default(TSRMLS_C);
	}

	zend_print_zval_r(var, 0 TSRMLS_CC);

	if (do_return) {
		php_output_get_contents(return_value TSRMLS_CC);
		php_output_discard(TSRMLS_C);
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto int connection_aborted(void)
   Returns true if client disconnected */
PHP_FUNCTION(connection_aborted)
{
	RETURN_LONG(PG(connection_status) & PHP_CONNECTION_ABORTED);
}
/* }}} */

/* {{{ proto int connection_status(void)
   Returns the connection status bitfield */
PHP_FUNCTION(connection_status)
{
	RETURN_LONG(PG(connection_status));
}
/* }}} */

/* {{{ proto int ignore_user_abort([string value])
   Set whether we want to ignore a user abort event or not */
PHP_FUNCTION(ignore_user_abort)
{
	char *arg = NULL;
	int arg_len = 0;
	int old_setting;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &arg, &arg_len) == FAILURE) {
		return;
	}

	old_setting = PG(ignore_user_abort);

	if (arg) {
		zend_alter_ini_entry_ex("ignore_user_abort", sizeof("ignore_user_abort"), arg, arg_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0 TSRMLS_CC);
	}

	RETURN_LONG(old_setting);
}
/* }}} */

#if HAVE_GETSERVBYNAME
/* {{{ proto int getservbyname(string service, string protocol)
   Returns port associated with service. Protocol must be "tcp" or "udp" */
PHP_FUNCTION(getservbyname)
{
	char *name, *proto;
	int name_len, proto_len;
	struct servent *serv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &proto, &proto_len) == FAILURE) {
		return;
	}


/* empty string behaves like NULL on windows implementation of 
   getservbyname. Let be portable instead. */
#ifdef PHP_WIN32
	if (proto_len == 0) {
		RETURN_FALSE;
	}
#endif

	serv = getservbyname(name, proto);

	if (serv == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(ntohs(serv->s_port));
}
/* }}} */
#endif

#if HAVE_GETSERVBYPORT
/* {{{ proto string getservbyport(int port, string protocol)
   Returns service name associated with port. Protocol must be "tcp" or "udp" */
PHP_FUNCTION(getservbyport)
{
	char *proto;
	int proto_len;
	long port;
	struct servent *serv;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &port, &proto, &proto_len) == FAILURE) {
		return;
	}

	serv = getservbyport(htons((unsigned short) port), proto);

	if (serv == NULL) {
		RETURN_FALSE;
	}

	RETURN_STRING(serv->s_name, 1);
}
/* }}} */
#endif

#if HAVE_GETPROTOBYNAME
/* {{{ proto int getprotobyname(string name)
   Returns protocol number associated with name as per /etc/protocols */
PHP_FUNCTION(getprotobyname)
{
	char *name;
	int name_len;
	struct protoent *ent;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	ent = getprotobyname(name);

	if (ent == NULL) {
		RETURN_FALSE;
	}

	RETURN_LONG(ent->p_proto);
}
/* }}} */
#endif

#if HAVE_GETPROTOBYNUMBER
/* {{{ proto string getprotobynumber(int proto)
   Returns protocol name associated with protocol number proto */
PHP_FUNCTION(getprotobynumber)
{
	long proto;
	struct protoent *ent;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &proto) == FAILURE) {
		return;
	}

	ent = getprotobynumber(proto);

	if (ent == NULL) {
		RETURN_FALSE;
	}

	RETURN_STRING(ent->p_name, 1);
}
/* }}} */
#endif

/* {{{ proto bool register_tick_function(string function_name [, mixed arg [, mixed ... ]])
   Registers a tick callback function */
PHP_FUNCTION(register_tick_function)
{
	user_tick_function_entry tick_fe;
	int i;
	char *function_name = NULL;

	tick_fe.calling = 0;
	tick_fe.arg_count = ZEND_NUM_ARGS();

	if (tick_fe.arg_count < 1) {
		WRONG_PARAM_COUNT;
	}

	tick_fe.arguments = (zval **) safe_emalloc(sizeof(zval *), tick_fe.arg_count, 0);

	if (zend_get_parameters_array(ht, tick_fe.arg_count, tick_fe.arguments) == FAILURE) {
		efree(tick_fe.arguments);
		RETURN_FALSE;
	}

	if (!zend_is_callable(tick_fe.arguments[0], 0, &function_name TSRMLS_CC)) {
		efree(tick_fe.arguments);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid tick callback '%s' passed", function_name);
		efree(function_name);
		RETURN_FALSE;
	} else if (function_name) {
		efree(function_name);
	}

	if (Z_TYPE_P(tick_fe.arguments[0]) != IS_ARRAY && Z_TYPE_P(tick_fe.arguments[0]) != IS_OBJECT) {
		convert_to_string_ex(&tick_fe.arguments[0]);
	}

	if (!BG(user_tick_functions)) {
		BG(user_tick_functions) = (zend_llist *) emalloc(sizeof(zend_llist));
		zend_llist_init(BG(user_tick_functions),
						sizeof(user_tick_function_entry),
						(llist_dtor_func_t) user_tick_function_dtor, 0);
		php_add_tick_function(run_user_tick_functions);
	}

	for (i = 0; i < tick_fe.arg_count; i++) {
		Z_ADDREF_P(tick_fe.arguments[i]);
	}

	zend_llist_add_element(BG(user_tick_functions), &tick_fe);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void unregister_tick_function(string function_name)
   Unregisters a tick callback function */
PHP_FUNCTION(unregister_tick_function)
{
	zval *function;
	user_tick_function_entry tick_fe;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z/", &function) == FAILURE) {
		return;
	}

	if (!BG(user_tick_functions)) {
		return;
	}

	if (Z_TYPE_P(function) != IS_ARRAY) {
		convert_to_string(function);
	}

	tick_fe.arguments = (zval **) emalloc(sizeof(zval *));
	tick_fe.arguments[0] = function;
	tick_fe.arg_count = 1;
	zend_llist_del_element(BG(user_tick_functions), &tick_fe, (int (*)(void *, void *)) user_tick_function_compare);
	efree(tick_fe.arguments);
}
/* }}} */

/* {{{ proto bool is_uploaded_file(string path)
   Check if file was created by rfc1867 upload */
PHP_FUNCTION(is_uploaded_file)
{
	char *path;
	int path_len;

	if (!SG(rfc1867_uploaded_files)) {
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &path_len) == FAILURE) {
		return;
	}

	if (zend_hash_exists(SG(rfc1867_uploaded_files), path, path_len + 1)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool move_uploaded_file(string path, string new_path)
   Move a file if and only if it was created by an upload */
PHP_FUNCTION(move_uploaded_file)
{
	char *path, *new_path;
	int path_len, new_path_len;
	zend_bool successful = 0;

#ifndef PHP_WIN32
	int oldmask; int ret;
#endif

	if (!SG(rfc1867_uploaded_files)) {
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &path, &path_len, &new_path, &new_path_len) == FAILURE) {
		return;
	}

	if (!zend_hash_exists(SG(rfc1867_uploaded_files), path, path_len + 1)) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(new_path TSRMLS_CC)) {
		RETURN_FALSE;
	}

	if (VCWD_RENAME(path, new_path) == 0) {
		successful = 1;
#ifndef PHP_WIN32
		oldmask = umask(077);
		umask(oldmask);

		ret = VCWD_CHMOD(new_path, 0666 & ~oldmask);

		if (ret == -1) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", strerror(errno));
		}
#endif
	} else if (php_copy_file_ex(path, new_path, STREAM_DISABLE_OPEN_BASEDIR TSRMLS_CC) == SUCCESS) {
		VCWD_UNLINK(path);
		successful = 1;
	}

	if (successful) {
		zend_hash_del(SG(rfc1867_uploaded_files), path, path_len + 1);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to move '%s' to '%s'", path, new_path);
	}

	RETURN_BOOL(successful);
}
/* }}} */

/* {{{ php_simple_ini_parser_cb
 */
static void php_simple_ini_parser_cb(zval *arg1, zval *arg2, zval *arg3, int callback_type, zval *arr TSRMLS_DC)
{
	zval *element;

	switch (callback_type) {

		case ZEND_INI_PARSER_ENTRY:
			if (!arg2) {
				/* bare string - nothing to do */
				break;
			}
			ALLOC_ZVAL(element);
			MAKE_COPY_ZVAL(&arg2, element);
			zend_symtable_update(Z_ARRVAL_P(arr), Z_STRVAL_P(arg1), Z_STRLEN_P(arg1) + 1, &element, sizeof(zval *), NULL);
			break;

		case ZEND_INI_PARSER_POP_ENTRY:
		{
			zval *hash, **find_hash;

			if (!arg2) {
				/* bare string - nothing to do */
				break;
			}

			if (!(Z_STRLEN_P(arg1) > 1 && Z_STRVAL_P(arg1)[0] == '0') && is_numeric_string(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1), NULL, NULL, 0) == IS_LONG) {
				ulong key = (ulong) zend_atol(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1));
				if (zend_hash_index_find(Z_ARRVAL_P(arr), key, (void **) &find_hash) == FAILURE) {
					ALLOC_ZVAL(hash);
					INIT_PZVAL(hash);
					array_init(hash);

					zend_hash_index_update(Z_ARRVAL_P(arr), key, &hash, sizeof(zval *), NULL);
				} else {
					hash = *find_hash;
				}
			} else {
				if (zend_hash_find(Z_ARRVAL_P(arr), Z_STRVAL_P(arg1), Z_STRLEN_P(arg1) + 1, (void **) &find_hash) == FAILURE) {
					ALLOC_ZVAL(hash);
					INIT_PZVAL(hash);
					array_init(hash);

					zend_hash_update(Z_ARRVAL_P(arr), Z_STRVAL_P(arg1), Z_STRLEN_P(arg1) + 1, &hash, sizeof(zval *), NULL);
				} else {
					hash = *find_hash;
				}
			}

			if (Z_TYPE_P(hash) != IS_ARRAY) {
				zval_dtor(hash);
				INIT_PZVAL(hash);
				array_init(hash);
			}

			ALLOC_ZVAL(element);
			MAKE_COPY_ZVAL(&arg2, element);

			if (arg3 && Z_STRLEN_P(arg3) > 0) {
				add_assoc_zval_ex(hash, Z_STRVAL_P(arg3), Z_STRLEN_P(arg3) + 1, element);
			} else {
				add_next_index_zval(hash, element);
			}
		}
		break;

		case ZEND_INI_PARSER_SECTION:
			break;
	}
}
/* }}} */

/* {{{ php_ini_parser_cb_with_sections
 */
static void php_ini_parser_cb_with_sections(zval *arg1, zval *arg2, zval *arg3, int callback_type, zval *arr TSRMLS_DC)
{
	if (callback_type == ZEND_INI_PARSER_SECTION) {
		MAKE_STD_ZVAL(BG(active_ini_file_section));
		array_init(BG(active_ini_file_section));
		zend_symtable_update(Z_ARRVAL_P(arr), Z_STRVAL_P(arg1), Z_STRLEN_P(arg1) + 1, &BG(active_ini_file_section), sizeof(zval *), NULL);
	} else if (arg2) {
		zval *active_arr;

		if (BG(active_ini_file_section)) {
			active_arr = BG(active_ini_file_section);
		} else {
			active_arr = arr;
		}

		php_simple_ini_parser_cb(arg1, arg2, arg3, callback_type, active_arr TSRMLS_CC);
	}
}
/* }}} */

/* {{{ proto array parse_ini_file(string filename [, bool process_sections [, int scanner_mode]])
   Parse configuration file */
PHP_FUNCTION(parse_ini_file)
{
	char *filename = NULL;
	int filename_len = 0;
	zend_bool process_sections = 0;
	long scanner_mode = ZEND_INI_SCANNER_NORMAL;
	zend_file_handle fh;
	zend_ini_parser_cb_t ini_parser_cb;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|bl", &filename, &filename_len, &process_sections, &scanner_mode) == FAILURE) {
		RETURN_FALSE;
	}

	if (filename_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Filename cannot be empty!");
		RETURN_FALSE;
	}

	/* Set callback function */
	if (process_sections) {
		BG(active_ini_file_section) = NULL;
		ini_parser_cb = (zend_ini_parser_cb_t) php_ini_parser_cb_with_sections;
	} else {
		ini_parser_cb = (zend_ini_parser_cb_t) php_simple_ini_parser_cb;
	}

	/* Setup filehandle */
	memset(&fh, 0, sizeof(fh));
	fh.filename = filename;
	fh.type = ZEND_HANDLE_FILENAME;

	array_init(return_value);
	if (zend_parse_ini_file(&fh, 0, scanner_mode, ini_parser_cb, return_value TSRMLS_CC) == FAILURE) {
		zend_hash_destroy(Z_ARRVAL_P(return_value));
		efree(Z_ARRVAL_P(return_value));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array parse_ini_string(string ini_string [, bool process_sections [, int scanner_mode]])
   Parse configuration string */
PHP_FUNCTION(parse_ini_string)
{
	char *string = NULL, *str = NULL;
	int str_len = 0;
	zend_bool process_sections = 0;
	long scanner_mode = ZEND_INI_SCANNER_NORMAL;
	zend_ini_parser_cb_t ini_parser_cb;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|bl", &str, &str_len, &process_sections, &scanner_mode) == FAILURE) {
		RETURN_FALSE;
	}

	if (INT_MAX - str_len < ZEND_MMAP_AHEAD) {
		RETVAL_FALSE;
	}

	/* Set callback function */
	if (process_sections) {
		BG(active_ini_file_section) = NULL;
		ini_parser_cb = (zend_ini_parser_cb_t) php_ini_parser_cb_with_sections;
	} else {
		ini_parser_cb = (zend_ini_parser_cb_t) php_simple_ini_parser_cb;
	}

	/* Setup string */
	string = (char *) emalloc(str_len + ZEND_MMAP_AHEAD);
	memcpy(string, str, str_len);
	memset(string + str_len, 0, ZEND_MMAP_AHEAD);

	array_init(return_value);
	if (zend_parse_ini_string(string, 0, scanner_mode, ini_parser_cb, return_value TSRMLS_CC) == FAILURE) {
		zend_hash_destroy(Z_ARRVAL_P(return_value));
		efree(Z_ARRVAL_P(return_value));
		RETVAL_FALSE;
	}
	efree(string);
}
/* }}} */

#if ZEND_DEBUG
/* This function returns an array of ALL valid ini options with values and 
 *  is not the same as ini_get_all() which returns only registered ini options. Only useful for devs to debug php.ini scanner/parser! */
PHP_FUNCTION(config_get_hash) /* {{{ */
{
	HashTable *hash = php_ini_get_configuration_hash();

	array_init(return_value);
	zend_hash_apply_with_arguments(hash TSRMLS_CC, (apply_func_args_t) add_config_entry_cb, 1, return_value);
}
/* }}} */
#endif

#ifdef HAVE_GETLOADAVG
/* {{{ proto array sys_getloadavg()
*/
PHP_FUNCTION(sys_getloadavg)
{
	double load[3];

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (getloadavg(load, 3) == -1) {
		RETURN_FALSE;
	} else {
		array_init(return_value);
		add_index_double(return_value, 0, load[0]);
		add_index_double(return_value, 1, load[1]);
		add_index_double(return_value, 2, load[2]);
	}
}
/* }}} */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
