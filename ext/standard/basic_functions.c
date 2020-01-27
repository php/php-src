/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_streams.h"
#include "php_main.h"
#include "php_globals.h"
#include "php_variables.h"
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
#include "ext/standard/php_mt_rand.h"

#ifdef PHP_WIN32
#include "win32/php_win32_globals.h"
#include "win32/time.h"
#include "win32/ioutil.h"
#endif

typedef struct yy_buffer_state *YY_BUFFER_STATE;

#include "zend.h"
#include "zend_ini_scanner.h"
#include "zend_language_scanner.h"
#include <zend_language_parser.h>

#include "zend_portability.h"

#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#ifndef PHP_WIN32
#include <sys/types.h>
#include <sys/stat.h>
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

#include <string.h>
#include <locale.h>

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
#define INADDR_NONE ((zend_ulong) -1)
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
	zval *arguments;
	int arg_count;
	int calling;
} user_tick_function_entry;

/* some prototypes for local functions */
static void user_shutdown_function_dtor(zval *zv);
static void user_tick_function_dtor(user_tick_function_entry *tick_function_entry);

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

ZEND_BEGIN_ARG_INFO(arginfo_current, 0)
	ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_key, 0)
	ZEND_ARG_INFO(0, arg)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_min, 0, 0, 1)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_max, 0, 0, 1)
	ZEND_ARG_VARIADIC_INFO(0, args)
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
	ZEND_ARG_VARIADIC_INFO(0, var_names)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_push, 0, 0, 1)
	ZEND_ARG_INFO(1, stack) /* ARRAY_INFO(1, stack, 0) */
	ZEND_ARG_VARIADIC_INFO(0, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_pop, 0)
	ZEND_ARG_INFO(1, stack) /* ARRAY_INFO(1, stack, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_shift, 0)
	ZEND_ARG_INFO(1, stack) /* ARRAY_INFO(1, stack, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_unshift, 0, 0, 1)
	ZEND_ARG_INFO(1, stack) /* ARRAY_INFO(1, stack, 0) */
	ZEND_ARG_VARIADIC_INFO(0, vars)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_merge, 0, 0, 0)
	ZEND_ARG_VARIADIC_INFO(0, arrays)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_merge_recursive, 0, 0, 0)
	ZEND_ARG_VARIADIC_INFO(0, arrays)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_replace, 0, 0, 1)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_VARIADIC_INFO(0, arrays)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_replace_recursive, 0, 0, 1)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_VARIADIC_INFO(0, arrays)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_keys, 0, 0, 1)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, search_value)
	ZEND_ARG_INFO(0, strict)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_key_first, 0)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_key_last, 0)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO(arginfo_array_values, 0)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_count_values, 0)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_column, 0, 0, 2)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, column_key)
	ZEND_ARG_INFO(0, index_key)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_unique, 0, 0, 1)
	ZEND_ARG_INFO(0, arg) /* ARRAY_INFO(0, arg, 0) */
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_intersect_key, 0, 0, 2)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_VARIADIC_INFO(0, arrays)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_intersect_ukey, 0)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, callback_key_compare_func)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_intersect, 0, 0, 2)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_VARIADIC_INFO(0, arrays)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_uintersect, 0)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, callback_data_compare_func)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_intersect_assoc, 0, 0, 2)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_VARIADIC_INFO(0, arrays)
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
	ZEND_ARG_VARIADIC_INFO(0, arrays)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_diff_ukey, 0)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(0, arr2) /* ARRAY_INFO(0, arg2, 0) */
	ZEND_ARG_INFO(0, callback_key_comp_func)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_diff, 0, 0, 2)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_VARIADIC_INFO(0, arrays)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_array_udiff, 0)
	ZEND_ARG_INFO(0, arr1)
	ZEND_ARG_INFO(0, arr2)
	ZEND_ARG_INFO(0, callback_data_comp_func)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_diff_assoc, 0, 0, 2)
	ZEND_ARG_INFO(0, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_VARIADIC_INFO(0, arrays)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_multisort, 0, 0, 1)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, arr1) /* ARRAY_INFO(0, arg1, 0) */
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, sort_order)
	ZEND_ARG_INFO(ZEND_SEND_PREFER_REF, sort_flags)
	ZEND_ARG_VARIADIC_INFO(ZEND_SEND_PREFER_REF, arr2)
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
    ZEND_ARG_INFO(0, use_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_array_map, 0, 0, 2)
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_VARIADIC_INFO(0, arrays)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_getenv, 0, 0, 0)
	ZEND_ARG_INFO(0, varname)
	ZEND_ARG_INFO(0, local_only)
ZEND_END_ARG_INFO()

#ifdef HAVE_PUTENV
ZEND_BEGIN_ARG_INFO(arginfo_putenv, 0)
	ZEND_ARG_INFO(0, setting)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_getopt, 0, 0, 1)
	ZEND_ARG_INFO(0, options)
	ZEND_ARG_INFO(0, opts) /* ARRAY_INFO(0, opts, 1) */
	ZEND_ARG_INFO(1, optind)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_error_clear_last, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_call_user_func, 0, 0, 1)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_VARIADIC_INFO(0, parameters)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_call_user_func_array, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, parameters) /* ARRAY_INFO(0, parameters, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_forward_static_call, 0, 0, 1)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_VARIADIC_INFO(0, parameters)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_forward_static_call_array, 0, 0, 2)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_INFO(0, parameters) /* ARRAY_INFO(0, parameters, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_register_shutdown_function, 0, 0, 1)
	ZEND_ARG_INFO(0, function_name)
	ZEND_ARG_VARIADIC_INFO(0, parameters)
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
	ZEND_ARG_INFO(0, details)
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
	ZEND_ARG_VARIADIC_INFO(0, parameters)
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
ZEND_BEGIN_ARG_INFO_EX(arginfo_assert, 0, 0, 1)
	ZEND_ARG_INFO(0, assertion)
	ZEND_ARG_INFO(0, description)
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
ZEND_BEGIN_ARG_INFO_EX(arginfo_crypt, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, salt)
ZEND_END_ARG_INFO()
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

ZEND_BEGIN_ARG_INFO(arginfo_net_get_interfaces, 0)
ZEND_END_ARG_INFO()

#if defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC
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

#endif /* defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC */
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_fscanf, 0, 0, 2)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_VARIADIC_INFO(1, vars)
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
ZEND_BEGIN_ARG_INFO_EX(arginfo_copy, 0, 0, 2)
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
	ZEND_ARG_INFO(0, escape_char)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fgetcsv, 0, 0, 1)
	ZEND_ARG_INFO(0, fp)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(0, delimiter)
	ZEND_ARG_INFO(0, enclosure)
	ZEND_ARG_INFO(0, escape)
ZEND_END_ARG_INFO()

#if HAVE_REALPATH || defined(ZTS)
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

ZEND_BEGIN_ARG_INFO(arginfo_chgrp, 0)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, group)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_chown, 0)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, user)
ZEND_END_ARG_INFO()

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
ZEND_BEGIN_ARG_INFO_EX(arginfo_sprintf, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_vsprintf, 0)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, args) /* ARRAY_INFO(0, args, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_printf, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_vprintf, 0)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, args) /* ARRAY_INFO(0, args, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fprintf, 0, 0, 2)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_vfprintf, 0)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, args) /* ARRAY_INFO(0, args, 1) */
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ fsock.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_fsockopen, 0, 0, 1)
	ZEND_ARG_INFO(0, hostname)
	ZEND_ARG_INFO(0, port)
	ZEND_ARG_INFO(1, errno)
	ZEND_ARG_INFO(1, errstr)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pfsockopen, 0, 0, 1)
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
	ZEND_ARG_INFO(0, expires_or_options)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, secure)
	ZEND_ARG_INFO(0, httponly)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_setrawcookie, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
	ZEND_ARG_INFO(0, expires_or_options)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, secure)
	ZEND_ARG_INFO(0, httponly)
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
/* {{{ hrtime.c */
ZEND_BEGIN_ARG_INFO(arginfo_hrtime, 0)
	ZEND_ARG_INFO(0, get_as_number)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ html.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_htmlspecialchars, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, quote_style)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_INFO(0, double_encode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_htmlspecialchars_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, quote_style)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_html_entity_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, quote_style)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_htmlentities, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, quote_style)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_INFO(0, double_encode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_get_html_translation_table, 0, 0, 0)
	ZEND_ARG_INFO(0, table)
	ZEND_ARG_INFO(0, quote_style)
	ZEND_ARG_INFO(0, encoding)
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

ZEND_BEGIN_ARG_INFO(arginfo_php_sapi_name, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_php_uname, 0, 0, 0)
	ZEND_ARG_INFO(0, mode)
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
ZEND_BEGIN_ARG_INFO_EX(arginfo_levenshtein, 0, 0, 2)
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
	ZEND_ARG_INFO(0, dec_separator)
	ZEND_ARG_INFO(0, thousands_separator)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fmod, 0)
	ZEND_ARG_INFO(0, x)
	ZEND_ARG_INFO(0, y)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_intdiv, 0)
	ZEND_ARG_INFO(0, dividend)
	ZEND_ARG_INFO(0, divisor)
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
ZEND_BEGIN_ARG_INFO_EX(arginfo_pack, 0, 0, 1)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_VARIADIC_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_unpack, 0, 0, 2)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_INFO(0, input)
	ZEND_ARG_INFO(0, offset)
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
/* {{{ password.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_password_hash, 0, 0, 2)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_password_get_info, 0, 0, 1)
	ZEND_ARG_INFO(0, hash)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_password_needs_rehash, 0, 0, 2)
	ZEND_ARG_INFO(0, hash)
	ZEND_ARG_INFO(0, algo)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_password_verify, 0, 0, 2)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, hash)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO(arginfo_password_algos, 0)
ZEND_END_ARG_INFO();
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
/* {{{ mt_rand.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_mt_srand, 0, 0, 0)
	ZEND_ARG_INFO(0, seed)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mt_rand, 0, 0, 0)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_mt_getrandmax, 0)
ZEND_END_ARG_INFO()
/* }}} */
/* {{{ random.c */
ZEND_BEGIN_ARG_INFO_EX(arginfo_random_bytes, 0, 0, 1)
	ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_random_int, 0, 0, 2)
	ZEND_ARG_INFO(0, min)
	ZEND_ARG_INFO(0, max)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_isatty, 0, 0, 1)
	ZEND_ARG_INFO(0, stream)
ZEND_END_ARG_INFO()

#ifdef PHP_WIN32
ZEND_BEGIN_ARG_INFO_EX(arginfo_sapi_windows_vt100_support, 0, 0, 1)
	ZEND_ARG_INFO(0, stream)
	ZEND_ARG_INFO(0, enable)
ZEND_END_ARG_INFO()
#endif

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

ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_context_set_option, 0, 0, 2)
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
ZEND_BEGIN_ARG_INFO_EX(arginfo_stream_set_timeout, 0, 0, 2)
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

ZEND_BEGIN_ARG_INFO(arginfo_strcoll, 0)
	ZEND_ARG_INFO(0, str1)
	ZEND_ARG_INFO(0, str2)
ZEND_END_ARG_INFO()

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

ZEND_BEGIN_ARG_INFO_EX(arginfo_strtok, 0, 0, 1)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_dirname, 0, 0, 1)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, levels)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_ucwords, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, delimiters)
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
	ZEND_ARG_VARIADIC_INFO(0, locales)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_sscanf, 0, 0, 2)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, format)
	ZEND_ARG_VARIADIC_INFO(1, vars)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_strpbrk, 0, 0, 2)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_utf8_encode, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_utf8_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
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

ZEND_BEGIN_ARG_INFO(arginfo_boolval, 0)
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

ZEND_BEGIN_ARG_INFO(arginfo_is_int, 0)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_is_iterable, 0, 0, 1)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_is_countable, 0)
	ZEND_ARG_INFO(0, var)
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
	ZEND_ARG_INFO(0, context)
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
	ZEND_ARG_VARIADIC_INFO(0, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_debug_zval_dump, 0, 0, 1)
	ZEND_ARG_VARIADIC_INFO(0, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_var_export, 0, 0, 1)
	ZEND_ARG_INFO(0, var)
	ZEND_ARG_INFO(0, return)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_serialize, 0)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_unserialize, 0, 0, 1)
	ZEND_ARG_INFO(0, variable_representation)
	ZEND_ARG_INFO(0, allowed_classes)
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
/* {{{ win32/codepage.c */
#ifdef PHP_WIN32
ZEND_BEGIN_ARG_INFO_EX(arginfo_sapi_windows_cp_set, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, code_page, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sapi_windows_cp_get, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sapi_windows_cp_is_utf8, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sapi_windows_cp_conv, 0, 0, 3)
	ZEND_ARG_INFO(0, in_codepage)
	ZEND_ARG_INFO(0, out_codepage)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sapi_windows_set_ctrl_handler, 0, 0, 1)
	ZEND_ARG_INFO(0, callable)
	ZEND_ARG_INFO(0, add)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_sapi_windows_generate_ctrl_event, 0, 0, 1)
	ZEND_ARG_INFO(0, event)
	ZEND_ARG_INFO(0, pid)
ZEND_END_ARG_INFO()
#endif
/* }}} */
/* }}} */

static const zend_function_entry basic_functions[] = { /* {{{ */
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
	PHP_DEP_FE(hebrevc,														arginfo_hebrevc)
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
	PHP_FE(utf8_encode, 													arginfo_utf8_encode)
	PHP_FE(utf8_decode, 													arginfo_utf8_decode)
	PHP_FE(strcoll,															arginfo_strcoll)

#ifdef HAVE_STRFMON
	PHP_DEP_FE(money_format,												arginfo_money_format)
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

	PHP_FE(rand,															arginfo_mt_rand)
	PHP_FALIAS(srand, mt_srand,												arginfo_mt_srand)
	PHP_FALIAS(getrandmax, mt_getrandmax,									arginfo_mt_getrandmax)
	PHP_FE(mt_rand,															arginfo_mt_rand)
	PHP_FE(mt_srand,														arginfo_mt_srand)
	PHP_FE(mt_getrandmax,													arginfo_mt_getrandmax)

	PHP_FE(random_bytes,													arginfo_random_bytes)
	PHP_FE(random_int,													arginfo_random_int)

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

	PHP_FE(password_hash,													arginfo_password_hash)
	PHP_FE(password_get_info,												arginfo_password_get_info)
	PHP_FE(password_needs_rehash,											arginfo_password_needs_rehash)
	PHP_FE(password_verify,													arginfo_password_verify)
	PHP_FE(password_algos,													arginfo_password_algos)
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
	PHP_FE(intdiv,															arginfo_intdiv)
#ifdef HAVE_INET_NTOP
	PHP_RAW_NAMED_FE(inet_ntop,		zif_inet_ntop,								arginfo_inet_ntop)
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

	PHP_FE(hrtime,															arginfo_hrtime)

#ifdef HAVE_GETTIMEOFDAY
	PHP_FE(uniqid,															arginfo_uniqid)
#endif

	PHP_FE(quoted_printable_decode,											arginfo_quoted_printable_decode)
	PHP_FE(quoted_printable_encode,											arginfo_quoted_printable_encode)
	PHP_DEP_FE(convert_cyr_string,												arginfo_convert_cyr_string)
	PHP_FE(get_current_user,												arginfo_get_current_user)
	PHP_FE(set_time_limit,													arginfo_set_time_limit)
	PHP_FE(header_register_callback,										arginfo_header_register_callback)
	PHP_FE(get_cfg_var,														arginfo_get_cfg_var)

	PHP_DEP_FE(get_magic_quotes_gpc,										arginfo_get_magic_quotes_gpc)
	PHP_DEP_FE(get_magic_quotes_runtime,									arginfo_get_magic_quotes_runtime)

	PHP_FE(error_log,														arginfo_error_log)
	PHP_FE(error_get_last,													arginfo_error_get_last)
	PHP_FE(error_clear_last,													arginfo_error_clear_last)
	PHP_FE(call_user_func,													arginfo_call_user_func)
	PHP_FE(call_user_func_array,											arginfo_call_user_func_array)
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
	PHP_DEP_FE(restore_include_path,											arginfo_restore_include_path)

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

#if defined(PHP_WIN32) || HAVE_GETIFADDRS
	PHP_FE(net_get_interfaces,												arginfo_net_get_interfaces)
#endif

#if defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC

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
	PHP_FE(boolval,															arginfo_boolval)
	PHP_FE(gettype,															arginfo_gettype)
	PHP_FE(settype,															arginfo_settype)
	PHP_FE(is_null,															arginfo_is_null)
	PHP_FE(is_resource,														arginfo_is_resource)
	PHP_FE(is_bool,															arginfo_is_bool)
	PHP_FE(is_int,															arginfo_is_int)
	PHP_FE(is_float,														arginfo_is_float)
	PHP_FALIAS(is_integer,			is_int,									arginfo_is_int)
	PHP_FALIAS(is_long,				is_int,									arginfo_is_int)
	PHP_FALIAS(is_double,			is_float,								arginfo_is_float)
	PHP_DEP_FALIAS(is_real,			is_float,								arginfo_is_float)
	PHP_FE(is_numeric,														arginfo_is_numeric)
	PHP_FE(is_string,														arginfo_is_string)
	PHP_FE(is_array,														arginfo_is_array)
	PHP_FE(is_object,														arginfo_is_object)
	PHP_FE(is_scalar,														arginfo_is_scalar)
	PHP_FE(is_callable,														arginfo_is_callable)
	PHP_FE(is_iterable,														arginfo_is_iterable)
	PHP_FE(is_countable,													arginfo_is_countable)

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
	PHP_DEP_FE(fgetss,														arginfo_fgetss)
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
	PHP_FE(stream_isatty,													arginfo_stream_isatty)
#ifdef PHP_WIN32
	PHP_FE(sapi_windows_vt100_support,										arginfo_sapi_windows_vt100_support)
#endif
	PHP_FE(fgetcsv,															arginfo_fgetcsv)
	PHP_FE(fputcsv,															arginfo_fputcsv)
	PHP_FE(flock,															arginfo_flock)
	PHP_FE(get_meta_tags,													arginfo_get_meta_tags)
	PHP_FE(stream_set_read_buffer,											arginfo_stream_set_read_buffer)
	PHP_FE(stream_set_write_buffer,											arginfo_stream_set_write_buffer)
	PHP_FALIAS(set_file_buffer, stream_set_write_buffer,					arginfo_stream_set_write_buffer)
	PHP_FE(stream_set_chunk_size,											arginfo_stream_set_chunk_size)

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

#if HAVE_REALPATH || defined(ZTS)
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

	/* functions from crypt.c */
	PHP_FE(crypt,															arginfo_crypt)

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
	PHP_FE(chown,															arginfo_chown)
	PHP_FE(chgrp,															arginfo_chgrp)
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
	PHP_DEP_FE(ezmlm_hash,													arginfo_ezmlm_hash)

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
	PHP_FE(array_key_first,													arginfo_array_key_first)
	PHP_FE(array_key_last,													arginfo_array_key_last)
	PHP_FE(array_values,													arginfo_array_values)
	PHP_FE(array_count_values,												arginfo_array_count_values)
	PHP_FE(array_column,													arginfo_array_column)
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

#ifdef PHP_WIN32
	PHP_FE(sapi_windows_cp_set, arginfo_sapi_windows_cp_set)
	PHP_FE(sapi_windows_cp_get, arginfo_sapi_windows_cp_get)
	PHP_FE(sapi_windows_cp_is_utf8, arginfo_sapi_windows_cp_is_utf8)
	PHP_FE(sapi_windows_cp_conv, arginfo_sapi_windows_cp_conv)
	PHP_FE(sapi_windows_set_ctrl_handler, arginfo_sapi_windows_set_ctrl_handler)
	PHP_FE(sapi_windows_generate_ctrl_event, arginfo_sapi_windows_generate_ctrl_event)
#endif
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
	PHP_STANDARD_VERSION,		/* extension version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#if defined(HAVE_PUTENV)
static void php_putenv_destructor(zval *zv) /* {{{ */
{
	putenv_entry *pe = Z_PTR_P(zv);

	if (pe->previous_value) {
# if defined(PHP_WIN32)
		/* MSVCRT has a bug in putenv() when setting a variable that
		 * is already set; if the SetEnvironmentVariable() API call
		 * fails, the Crt will double free() a string.
		 * We try to avoid this by setting our own value first */
		SetEnvironmentVariable(pe->key, "bugbug");
# endif
		putenv(pe->previous_value);
# if defined(PHP_WIN32)
		efree(pe->previous_value);
# endif
	} else {
# if HAVE_UNSETENV
		unsetenv(pe->key);
# elif defined(PHP_WIN32)
		SetEnvironmentVariable(pe->key, NULL);
# ifndef ZTS
		_putenv_s(pe->key, "");
# endif
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
	efree(pe);
}
/* }}} */
#endif

static void basic_globals_ctor(php_basic_globals *basic_globals_p) /* {{{ */
{
	BG(mt_rand_is_seeded) = 0;
	BG(mt_rand_mode) = MT_RAND_MT19937;
	BG(umask) = -1;
	BG(next) = NULL;
	BG(left) = -1;
	BG(user_tick_functions) = NULL;
	BG(user_filter_map) = NULL;
	BG(serialize_lock) = 0;

	memset(&BG(serialize), 0, sizeof(BG(serialize)));
	memset(&BG(unserialize), 0, sizeof(BG(unserialize)));

	memset(&BG(url_adapt_session_ex), 0, sizeof(BG(url_adapt_session_ex)));
	memset(&BG(url_adapt_output_ex), 0, sizeof(BG(url_adapt_output_ex)));

	BG(url_adapt_session_ex).type = 1;
	BG(url_adapt_output_ex).type  = 0;

	zend_hash_init(&BG(url_adapt_session_hosts_ht), 0, NULL, NULL, 1);
	zend_hash_init(&BG(url_adapt_output_hosts_ht), 0, NULL, NULL, 1);

#if defined(_REENTRANT) && defined(HAVE_MBRLEN) && defined(HAVE_MBSTATE_T)
	memset(&BG(mblen_state), 0, sizeof(BG(mblen_state)));
#endif

	BG(incomplete_class) = incomplete_class_entry;
	BG(page_uid) = -1;
	BG(page_gid) = -1;
}
/* }}} */

static void basic_globals_dtor(php_basic_globals *basic_globals_p) /* {{{ */
{
	if (basic_globals_p->url_adapt_session_ex.tags) {
		zend_hash_destroy(basic_globals_p->url_adapt_session_ex.tags);
		free(basic_globals_p->url_adapt_session_ex.tags);
	}
	if (basic_globals_p->url_adapt_output_ex.tags) {
		zend_hash_destroy(basic_globals_p->url_adapt_output_ex.tags);
		free(basic_globals_p->url_adapt_output_ex.tags);
	}

	zend_hash_destroy(&basic_globals_p->url_adapt_session_hosts_ht);
	zend_hash_destroy(&basic_globals_p->url_adapt_output_hosts_ht);
}
/* }}} */

PHPAPI double php_get_nan(void) /* {{{ */
{
	return ZEND_NAN;
}
/* }}} */

PHPAPI double php_get_inf(void) /* {{{ */
{
	return ZEND_INFINITY;
}
/* }}} */

#define BASIC_MINIT_SUBMODULE(module) \
	if (PHP_MINIT(module)(INIT_FUNC_ARGS_PASSTHRU) != SUCCESS) {\
		return FAILURE; \
	}

#define BASIC_RINIT_SUBMODULE(module) \
	PHP_RINIT(module)(INIT_FUNC_ARGS_PASSTHRU);

#define BASIC_MINFO_SUBMODULE(module) \
	PHP_MINFO(module)(ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU);

#define BASIC_RSHUTDOWN_SUBMODULE(module) \
	PHP_RSHUTDOWN(module)(SHUTDOWN_FUNC_ARGS_PASSTHRU);

#define BASIC_MSHUTDOWN_SUBMODULE(module) \
	PHP_MSHUTDOWN(module)(SHUTDOWN_FUNC_ARGS_PASSTHRU);

PHP_MINIT_FUNCTION(basic) /* {{{ */
{
#ifdef ZTS
	ts_allocate_id(&basic_globals_id, sizeof(php_basic_globals), (ts_allocate_ctor) basic_globals_ctor, (ts_allocate_dtor) basic_globals_dtor);
#ifdef PHP_WIN32
	ts_allocate_id(&php_win32_core_globals_id, sizeof(php_win32_core_globals), (ts_allocate_ctor)php_win32_core_globals_ctor, (ts_allocate_dtor)php_win32_core_globals_dtor );
#endif
#else
	basic_globals_ctor(&basic_globals);
#ifdef PHP_WIN32
	php_win32_core_globals_ctor(&the_php_win32_core_globals);
#endif
#endif

	BG(incomplete_class) = incomplete_class_entry = php_create_incomplete_class();

	REGISTER_LONG_CONSTANT("CONNECTION_ABORTED", PHP_CONNECTION_ABORTED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CONNECTION_NORMAL",  PHP_CONNECTION_NORMAL,  CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CONNECTION_TIMEOUT", PHP_CONNECTION_TIMEOUT, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("INI_USER",   ZEND_INI_USER,   CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_PERDIR", ZEND_INI_PERDIR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_SYSTEM", ZEND_INI_SYSTEM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_ALL",    ZEND_INI_ALL,    CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("INI_SCANNER_NORMAL", ZEND_INI_SCANNER_NORMAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_SCANNER_RAW",    ZEND_INI_SCANNER_RAW,    CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("INI_SCANNER_TYPED",  ZEND_INI_SCANNER_TYPED,  CONST_CS | CONST_PERSISTENT);

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
	REGISTER_DOUBLE_CONSTANT("INF", ZEND_INFINITY, CONST_CS | CONST_PERSISTENT);
	REGISTER_DOUBLE_CONSTANT("NAN", ZEND_NAN, CONST_CS | CONST_PERSISTENT);

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

	BASIC_MINIT_SUBMODULE(var)
	BASIC_MINIT_SUBMODULE(file)
	BASIC_MINIT_SUBMODULE(pack)
	BASIC_MINIT_SUBMODULE(browscap)
	BASIC_MINIT_SUBMODULE(standard_filters)
	BASIC_MINIT_SUBMODULE(user_filters)
	BASIC_MINIT_SUBMODULE(password)
	BASIC_MINIT_SUBMODULE(mt_rand)

#if defined(ZTS)
	BASIC_MINIT_SUBMODULE(localeconv)
#endif

#if defined(HAVE_NL_LANGINFO)
	BASIC_MINIT_SUBMODULE(nl_langinfo)
#endif

#if ZEND_INTRIN_SSE4_2_FUNC_PTR
	BASIC_MINIT_SUBMODULE(string_intrin)
#endif

#if ZEND_INTRIN_AVX2_FUNC_PTR || ZEND_INTRIN_SSSE3_FUNC_PTR
	BASIC_MINIT_SUBMODULE(base64_intrin)
#endif

	BASIC_MINIT_SUBMODULE(crypt)
	BASIC_MINIT_SUBMODULE(lcg)

	BASIC_MINIT_SUBMODULE(dir)
#ifdef HAVE_SYSLOG_H
	BASIC_MINIT_SUBMODULE(syslog)
#endif
	BASIC_MINIT_SUBMODULE(array)
	BASIC_MINIT_SUBMODULE(assert)
	BASIC_MINIT_SUBMODULE(url_scanner_ex)
#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	BASIC_MINIT_SUBMODULE(proc_open)
#endif
	BASIC_MINIT_SUBMODULE(exec)

	BASIC_MINIT_SUBMODULE(user_streams)
	BASIC_MINIT_SUBMODULE(imagetypes)

	php_register_url_stream_wrapper("php", &php_stream_php_wrapper);
	php_register_url_stream_wrapper("file", &php_plain_files_wrapper);
#ifdef HAVE_GLOB
	php_register_url_stream_wrapper("glob", &php_glob_stream_wrapper);
#endif
	php_register_url_stream_wrapper("data", &php_stream_rfc2397_wrapper);
	php_register_url_stream_wrapper("http", &php_stream_http_wrapper);
	php_register_url_stream_wrapper("ftp", &php_stream_ftp_wrapper);

#if defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC
# if defined(PHP_WIN32) || HAVE_FULL_DNS_FUNCS
	BASIC_MINIT_SUBMODULE(dns)
# endif
#endif

	BASIC_MINIT_SUBMODULE(random)

	BASIC_MINIT_SUBMODULE(hrtime)

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
	basic_globals_dtor(&basic_globals);
#ifdef PHP_WIN32
	php_win32_core_globals_dtor(&the_php_win32_core_globals);
#endif
#endif

	php_unregister_url_stream_wrapper("php");
	php_unregister_url_stream_wrapper("http");
	php_unregister_url_stream_wrapper("ftp");

	BASIC_MSHUTDOWN_SUBMODULE(browscap)
	BASIC_MSHUTDOWN_SUBMODULE(array)
	BASIC_MSHUTDOWN_SUBMODULE(assert)
	BASIC_MSHUTDOWN_SUBMODULE(url_scanner_ex)
	BASIC_MSHUTDOWN_SUBMODULE(file)
	BASIC_MSHUTDOWN_SUBMODULE(standard_filters)
#if defined(ZTS)
	BASIC_MSHUTDOWN_SUBMODULE(localeconv)
#endif
	BASIC_MSHUTDOWN_SUBMODULE(crypt)
	BASIC_MSHUTDOWN_SUBMODULE(random)
	BASIC_MSHUTDOWN_SUBMODULE(password)

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
	ZVAL_UNDEF(&BG(strtok_zval));
	BG(strtok_last) = NULL;
	BG(locale_string) = NULL;
	BG(locale_changed) = 0;
	BG(array_walk_fci) = empty_fcall_info;
	BG(array_walk_fci_cache) = empty_fcall_info_cache;
	BG(user_compare_fci) = empty_fcall_info;
	BG(user_compare_fci_cache) = empty_fcall_info_cache;
	BG(page_uid) = -1;
	BG(page_gid) = -1;
	BG(page_inode) = -1;
	BG(page_mtime) = -1;
#ifdef HAVE_PUTENV
	zend_hash_init(&BG(putenv_ht), 1, NULL, php_putenv_destructor, 0);
#endif
	BG(user_shutdown_function_names) = NULL;

	PHP_RINIT(filestat)(INIT_FUNC_ARGS_PASSTHRU);
#ifdef HAVE_SYSLOG_H
	BASIC_RINIT_SUBMODULE(syslog)
#endif
	BASIC_RINIT_SUBMODULE(dir)
	BASIC_RINIT_SUBMODULE(url_scanner_ex)

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
	zval_ptr_dtor(&BG(strtok_zval));
	ZVAL_UNDEF(&BG(strtok_zval));
	BG(strtok_string) = NULL;
#ifdef HAVE_PUTENV
	tsrm_env_lock();
	zend_hash_destroy(&BG(putenv_ht));
	tsrm_env_unlock();
#endif

	BG(mt_rand_is_seeded) = 0;

	if (BG(umask) != -1) {
		umask(BG(umask));
	}

	/* Check if locale was changed and change it back
	 * to the value in startup environment */
	if (BG(locale_changed)) {
		setlocale(LC_ALL, "C");
		setlocale(LC_CTYPE, "");
		zend_update_current_locale();
		if (BG(locale_string)) {
			zend_string_release_ex(BG(locale_string), 0);
			BG(locale_string) = NULL;
		}
	}

	/* FG(stream_wrappers) and FG(stream_filters) are destroyed
	 * during php_request_shutdown() */

	PHP_RSHUTDOWN(filestat)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#ifdef HAVE_SYSLOG_H
#ifdef PHP_WIN32
	BASIC_RSHUTDOWN_SUBMODULE(syslog)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#endif
#endif
	BASIC_RSHUTDOWN_SUBMODULE(assert)
	BASIC_RSHUTDOWN_SUBMODULE(url_scanner_ex)
	BASIC_RSHUTDOWN_SUBMODULE(streams)
#ifdef PHP_WIN32
	BASIC_RSHUTDOWN_SUBMODULE(win32_core_globals)
#endif

	if (BG(user_tick_functions)) {
		zend_llist_destroy(BG(user_tick_functions));
		efree(BG(user_tick_functions));
		BG(user_tick_functions) = NULL;
	}

	BASIC_RSHUTDOWN_SUBMODULE(user_filters)
	BASIC_RSHUTDOWN_SUBMODULE(browscap)

 	BG(page_uid) = -1;
 	BG(page_gid) = -1;
	return SUCCESS;
}
/* }}} */

PHP_MINFO_FUNCTION(basic) /* {{{ */
{
	php_info_print_table_start();
	BASIC_MINFO_SUBMODULE(dl)
	BASIC_MINFO_SUBMODULE(mail)
	php_info_print_table_end();
	BASIC_MINFO_SUBMODULE(assert)
}
/* }}} */

/* {{{ proto mixed constant(string const_name)
   Given the name of a constant this function will return the constant's associated value */
PHP_FUNCTION(constant)
{
	zend_string *const_name;
	zval *c;
	zend_class_entry *scope;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(const_name)
	ZEND_PARSE_PARAMETERS_END();

	scope = zend_get_executed_scope();
	c = zend_get_constant_ex(const_name, scope, ZEND_FETCH_CLASS_SILENT);
	if (c) {
		ZVAL_COPY_OR_DUP(return_value, c);
		if (Z_TYPE_P(return_value) == IS_CONSTANT_AST) {
			if (UNEXPECTED(zval_update_constant_ex(return_value, scope) != SUCCESS)) {
				return;
			}
		}
	} else {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Couldn't find constant %s", ZSTR_VAL(const_name));
		}
		RETURN_NULL();
	}
}
/* }}} */

#ifdef HAVE_INET_NTOP
/* {{{ proto string inet_ntop(string in_addr)
   Converts a packed inet address to a human readable IP address string */
PHP_NAMED_FUNCTION(zif_inet_ntop)
{
	char *address;
	size_t address_len;
	int af = AF_INET;
	char buffer[40];

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(address, address_len)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

#ifdef HAVE_IPV6
	if (address_len == 16) {
		af = AF_INET6;
	} else
#endif
	if (address_len != 4) {
		RETURN_FALSE;
	}

	if (!inet_ntop(af, address, buffer, sizeof(buffer))) {
		RETURN_FALSE;
	}

	RETURN_STRING(buffer);
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
	size_t address_len;
	char buffer[17];

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(address, address_len)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	memset(buffer, 0, sizeof(buffer));

#ifdef HAVE_IPV6
	if (strchr(address, ':')) {
		af = AF_INET6;
	} else
#endif
	if (!strchr(address, '.')) {
		RETURN_FALSE;
	}

	ret = inet_pton(af, address, buffer);

	if (ret <= 0) {
		RETURN_FALSE;
	}

	RETURN_STRINGL(buffer, af == AF_INET ? 4 : 16);
}
/* }}} */
#endif /* HAVE_INET_PTON */

/* {{{ proto int ip2long(string ip_address)
   Converts a string containing an (IPv4) Internet Protocol dotted address into a proper address */
PHP_FUNCTION(ip2long)
{
	char *addr;
	size_t addr_len;
#ifdef HAVE_INET_PTON
	struct in_addr ip;
#else
	zend_ulong ip;
#endif

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(addr, addr_len)
	ZEND_PARSE_PARAMETERS_END();

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
	zend_ulong ip;
	zend_long sip;
	struct in_addr myaddr;
#ifdef HAVE_INET_PTON
	char str[40];
#endif

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(sip)
	ZEND_PARSE_PARAMETERS_END();

	/* autoboxes on 32bit platforms, but that's expected */
	ip = (zend_ulong)sip;

	myaddr.s_addr = htonl(ip);
#ifdef HAVE_INET_PTON
	if (inet_ntop(AF_INET, &myaddr, str, sizeof(str))) {
		RETURN_STRING(str);
	} else {
		RETURN_FALSE;
	}
#else
	RETURN_STRING(inet_ntoa(myaddr));
#endif
}
/* }}} */

/********************
 * System Functions *
 ********************/

/* {{{ proto string getenv(string varname[, bool local_only]
   Get the value of an environment variable or every available environment variable
   if no varname is present  */
PHP_FUNCTION(getenv)
{
	char *ptr, *str = NULL;
	size_t str_len;
	zend_bool local_only = 0;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_BOOL(local_only)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (!str) {
		array_init(return_value);
		php_import_environment_variables(return_value);
		return;
	}

	if (!local_only) {
		/* SAPI method returns an emalloc()'d string */
		ptr = sapi_getenv(str, str_len);
		if (ptr) {
			// TODO: avoid realocation ???
			RETVAL_STRING(ptr);
			efree(ptr);
			return;
		}
	}
#ifdef PHP_WIN32
	{
		wchar_t dummybuf;
		DWORD size;
		wchar_t *keyw, *valw;

		keyw = php_win32_cp_conv_any_to_w(str, str_len, PHP_WIN32_CP_IGNORE_LEN_P);
		if (!keyw) {
				RETURN_FALSE;
		}

		SetLastError(0);
		/*If the given bugger is not large enough to hold the data, the return value is
		the buffer size,  in characters, required to hold the string and its terminating
		null character. We use this return value to alloc the final buffer. */
		size = GetEnvironmentVariableW(keyw, &dummybuf, 0);
		if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
				/* The environment variable doesn't exist. */
				free(keyw);
				RETURN_FALSE;
		}

		if (size == 0) {
				/* env exists, but it is empty */
				free(keyw);
				RETURN_EMPTY_STRING();
		}

		valw = emalloc((size + 1) * sizeof(wchar_t));
		size = GetEnvironmentVariableW(keyw, valw, size);
		if (size == 0) {
				/* has been removed between the two calls */
				free(keyw);
				efree(valw);
				RETURN_EMPTY_STRING();
		} else {
			ptr = php_win32_cp_w_to_any(valw);
			RETVAL_STRING(ptr);
			free(ptr);
			free(keyw);
			efree(valw);
			return;
		}
	}
#else

    tsrm_env_lock();

	/* system method returns a const */
	ptr = getenv(str);

	if (ptr) {
		RETVAL_STRING(ptr);
	}

    tsrm_env_unlock();

    if (ptr) {
        return;
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
	size_t setting_len;
	char *p, **env;
	putenv_entry pe;
#ifdef PHP_WIN32
	char *value = NULL;
	int equals = 0;
	int error_code;
#endif

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(setting, setting_len)
	ZEND_PARSE_PARAMETERS_END();

    if(setting_len == 0 || setting[0] == '=') {
    	php_error_docref(NULL, E_WARNING, "Invalid parameter syntax");
    	RETURN_FALSE;
    }

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

	tsrm_env_lock();
	zend_hash_str_del(&BG(putenv_ht), pe.key, pe.key_len);

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
		wchar_t *keyw, *valw = NULL;

		keyw = php_win32_cp_any_to_w(pe.key);
		if (value) {
			valw = php_win32_cp_any_to_w(value);
		}
		/* valw may be NULL, but the failed conversion still needs to be checked. */
		if (!keyw || !valw && value) {
			efree(pe.putenv_string);
			efree(pe.key);
			free(keyw);
			free(valw);
			RETURN_FALSE;
		}

	error_code = SetEnvironmentVariableW(keyw, valw);

	if (error_code != 0
# ifndef ZTS
	/* We need both SetEnvironmentVariable and _putenv here as some
		dependency lib could use either way to read the environment.
		Obviously the CRT version will be useful more often. But
		generally, doing both brings us on the safe track at least
		in NTS build. */
	&& _wputenv_s(keyw, valw ? valw : L"") == 0
# endif
	) { /* success */
# endif
#endif
		zend_hash_str_add_mem(&BG(putenv_ht), pe.key, pe.key_len, &pe, sizeof(putenv_entry));
#ifdef HAVE_TZSET
		if (!strncmp(pe.key, "TZ", pe.key_len)) {
			tzset();
		}
#endif
		tsrm_env_unlock();
#if defined(PHP_WIN32)
		free(keyw);
		free(valw);
#endif
		RETURN_TRUE;
	} else {
		efree(pe.putenv_string);
		efree(pe.key);
#if defined(PHP_WIN32)
		free(keyw);
		free(valw);
#endif
		RETURN_FALSE;
	}
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
	unsigned int opts_len = (unsigned int)strlen(opts);

	for (i = 0; i < opts_len; i++) {
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
		paras->need_param = *(++opts) == ':';
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

/* {{{ proto array getopt(string options [, array longopts [, int &optind]])
   Get options from the command line argument list */
PHP_FUNCTION(getopt)
{
	char *options = NULL, **argv = NULL;
	char opt[2] = { '\0' };
	char *optname;
	int argc = 0, o;
	size_t options_len = 0, len;
	char *php_optarg = NULL;
	int php_optind = 1;
	zval val, *args = NULL, *p_longopts = NULL;
	zval *zoptind = NULL;
	size_t optname_len = 0;
	opt_struct *opts, *orig_opts;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STRING(options, options_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY(p_longopts)
		Z_PARAM_ZVAL(zoptind)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	/* Init zoptind to 1 */
	if (zoptind) {
		ZEND_TRY_ASSIGN_REF_LONG(zoptind, 1);
	}

	/* Get argv from the global symbol table. We calculate argc ourselves
	 * in order to be on the safe side, even though it is also available
	 * from the symbol table. */
	if ((Z_TYPE(PG(http_globals)[TRACK_VARS_SERVER]) == IS_ARRAY || zend_is_auto_global_str(ZEND_STRL("_SERVER"))) &&
		((args = zend_hash_find_ex_ind(Z_ARRVAL_P(&PG(http_globals)[TRACK_VARS_SERVER]), ZSTR_KNOWN(ZEND_STR_ARGV), 1)) != NULL ||
		(args = zend_hash_find_ex_ind(&EG(symbol_table), ZSTR_KNOWN(ZEND_STR_ARGV), 1)) != NULL)
	) {
		int pos = 0;
		zval *entry;

 		if (Z_TYPE_P(args) != IS_ARRAY) {
 			RETURN_FALSE;
 		}
 		argc = zend_hash_num_elements(Z_ARRVAL_P(args));

		/* Attempt to allocate enough memory to hold all of the arguments
		 * and a trailing NULL */
		argv = (char **) safe_emalloc(sizeof(char *), (argc + 1), 0);

		/* Iterate over the hash to construct the argv array. */
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(args), entry) {
			zend_string *tmp_arg_str;
			zend_string *arg_str = zval_get_tmp_string(entry, &tmp_arg_str);

			argv[pos++] = estrdup(ZSTR_VAL(arg_str));

			zend_tmp_string_release(tmp_arg_str);
		} ZEND_HASH_FOREACH_END();

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
		zval *entry;

		count = zend_hash_num_elements(Z_ARRVAL_P(p_longopts));

		/* the first <len> slots are filled by the one short ops
		 * we now extend our array and jump to the new added structs */
		opts = (opt_struct *) erealloc(opts, sizeof(opt_struct) * (len + count + 1));
		orig_opts = opts;
		opts += len;

		memset(opts, 0, count * sizeof(opt_struct));

		/* Iterate over the hash to construct the argv array. */
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(p_longopts), entry) {
			zend_string *tmp_arg_str;
			zend_string *arg_str = zval_get_tmp_string(entry, &tmp_arg_str);

			opts->need_param = 0;
			opts->opt_name = estrdup(ZSTR_VAL(arg_str));
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

			zend_tmp_string_release(tmp_arg_str);
		} ZEND_HASH_FOREACH_END();
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
		if (o == PHP_GETOPT_INVALID_ARG) {
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

		if (php_optarg != NULL) {
			/* keep the arg as binary, since the encoding is not known */
			ZVAL_STRING(&val, php_optarg);
		} else {
			ZVAL_FALSE(&val);
		}

		/* Add this option / argument pair to the result hash. */
		optname_len = strlen(optname);
		if (!(optname_len > 1 && optname[0] == '0') && is_numeric_string(optname, optname_len, NULL, NULL, 0) == IS_LONG) {
			/* numeric string */
			int optname_int = atoi(optname);
			if ((args = zend_hash_index_find(Z_ARRVAL_P(return_value), optname_int)) != NULL) {
				if (Z_TYPE_P(args) != IS_ARRAY) {
					convert_to_array_ex(args);
				}
				zend_hash_next_index_insert(Z_ARRVAL_P(args), &val);
			} else {
				zend_hash_index_update(Z_ARRVAL_P(return_value), optname_int, &val);
			}
		} else {
			/* other strings */
			if ((args = zend_hash_str_find(Z_ARRVAL_P(return_value), optname, strlen(optname))) != NULL) {
				if (Z_TYPE_P(args) != IS_ARRAY) {
					convert_to_array_ex(args);
				}
				zend_hash_next_index_insert(Z_ARRVAL_P(args), &val);
			} else {
				zend_hash_str_add(Z_ARRVAL_P(return_value), optname, strlen(optname), &val);
			}
		}

		php_optarg = NULL;
	}

	/* Set zoptind to php_optind */
	if (zoptind) {
		ZEND_TRY_ASSIGN_REF_LONG(zoptind, php_optind);
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
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	sapi_flush();
}
/* }}} */

/* {{{ proto void sleep(int seconds)
   Delay for a given number of seconds */
PHP_FUNCTION(sleep)
{
	zend_long num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(num)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (num < 0) {
		php_error_docref(NULL, E_WARNING, "Number of seconds must be greater than or equal to 0");
		RETURN_FALSE;
	}
#ifdef PHP_SLEEP_NON_VOID
	RETURN_LONG(php_sleep((unsigned int)num));
#else
	php_sleep((unsigned int)num);
#endif

}
/* }}} */

/* {{{ proto void usleep(int micro_seconds)
   Delay for a given number of micro seconds */
PHP_FUNCTION(usleep)
{
#if HAVE_USLEEP
	zend_long num;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(num)
	ZEND_PARSE_PARAMETERS_END();

	if (num < 0) {
		php_error_docref(NULL, E_WARNING, "Number of microseconds must be greater than or equal to 0");
		RETURN_FALSE;
	}
	usleep((unsigned int)num);
#endif
}
/* }}} */

#if HAVE_NANOSLEEP
/* {{{ proto mixed time_nanosleep(int seconds, int nanoseconds)
   Delay for a number of seconds and nano seconds */
PHP_FUNCTION(time_nanosleep)
{
	zend_long tv_sec, tv_nsec;
	struct timespec php_req, php_rem;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(tv_sec)
		Z_PARAM_LONG(tv_nsec)
	ZEND_PARSE_PARAMETERS_END();

	if (tv_sec < 0) {
		php_error_docref(NULL, E_WARNING, "The seconds value must be greater than 0");
		RETURN_FALSE;
	}
	if (tv_nsec < 0) {
		php_error_docref(NULL, E_WARNING, "The nanoseconds value must be greater than 0");
		RETURN_FALSE;
	}

	php_req.tv_sec = (time_t) tv_sec;
	php_req.tv_nsec = (long)tv_nsec;
	if (!nanosleep(&php_req, &php_rem)) {
		RETURN_TRUE;
	} else if (errno == EINTR) {
		array_init(return_value);
		add_assoc_long_ex(return_value, "seconds", sizeof("seconds")-1, php_rem.tv_sec);
		add_assoc_long_ex(return_value, "nanoseconds", sizeof("nanoseconds")-1, php_rem.tv_nsec);
		return;
	} else if (errno == EINVAL) {
		php_error_docref(NULL, E_WARNING, "nanoseconds was not in the range 0 to 999 999 999 or seconds was negative");
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto mixed time_sleep_until(float timestamp)
   Make the script sleep until the specified time */
PHP_FUNCTION(time_sleep_until)
{
	double target_secs;
	struct timeval tm;
	struct timespec php_req, php_rem;
	uint64_t current_ns, target_ns, diff_ns;
	const uint64_t ns_per_sec = 1000000000;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_DOUBLE(target_secs)
	ZEND_PARSE_PARAMETERS_END();

	if (gettimeofday((struct timeval *) &tm, NULL) != 0) {
		RETURN_FALSE;
	}

	target_ns = (uint64_t) (target_secs * ns_per_sec);
	current_ns = ((uint64_t) tm.tv_sec) * ns_per_sec + ((uint64_t) tm.tv_usec) * 1000;
	if (target_ns < current_ns) {
		php_error_docref(NULL, E_WARNING, "Sleep until to time is less than current time");
		RETURN_FALSE;
	}

	diff_ns = target_ns - current_ns;
	php_req.tv_sec = (time_t) (diff_ns / ns_per_sec);
	php_req.tv_nsec = (long) (diff_ns % ns_per_sec);

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

	RETURN_STRING(php_get_current_user());
}
/* }}} */

static void add_config_entries(HashTable *hash, zval *return_value);

/* {{{ add_config_entry
 */
static void add_config_entry(zend_ulong h, zend_string *key, zval *entry, zval *retval)
{
	if (Z_TYPE_P(entry) == IS_STRING) {
		zend_string *str = Z_STR_P(entry);
		if (!ZSTR_IS_INTERNED(str)) {
			if (!(GC_FLAGS(str) & GC_PERSISTENT)) {
				zend_string_addref(str);
			} else {
				str = zend_string_init(ZSTR_VAL(str), ZSTR_LEN(str), 0);
			}
		}

		if (key) {
			add_assoc_str_ex(retval, ZSTR_VAL(key), ZSTR_LEN(key), str);
		} else {
			add_index_str(retval, h, str);
		}
	} else if (Z_TYPE_P(entry) == IS_ARRAY) {
		zval tmp;
		array_init(&tmp);
		add_config_entries(Z_ARRVAL_P(entry), &tmp);
		zend_hash_update(Z_ARRVAL_P(retval), key, &tmp);
	}
}
/* }}} */

/* {{{ add_config_entries
 */
static void add_config_entries(HashTable *hash, zval *return_value) /* {{{ */
{
	zend_ulong h;
	zend_string *key;
	zval *zv;

	ZEND_HASH_FOREACH_KEY_VAL(hash, h, key, zv)
		add_config_entry(h, key, zv, return_value);
	ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ proto mixed get_cfg_var(string option_name)
   Get the value of a PHP configuration option */
PHP_FUNCTION(get_cfg_var)
{
	char *varname;
	size_t varname_len;
	zval *retval;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(varname, varname_len)
	ZEND_PARSE_PARAMETERS_END();

	retval = cfg_get_entry(varname, (uint32_t)varname_len);

	if (retval) {
		if (Z_TYPE_P(retval) == IS_ARRAY) {
			array_init(return_value);
			add_config_entries(Z_ARRVAL_P(retval), return_value);
			return;
		} else {
			RETURN_STRING(Z_STRVAL_P(retval));
		}
	} else {
		RETURN_FALSE;
	}
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
	size_t message_len, opt_len = 0, headers_len = 0;
	int opt_err = 0, argc = ZEND_NUM_ARGS();
	zend_long erropt = 0;

	ZEND_PARSE_PARAMETERS_START(1, 4)
		Z_PARAM_STRING(message, message_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(erropt)
		Z_PARAM_PATH(opt, opt_len)
		Z_PARAM_STRING(headers, headers_len)
	ZEND_PARSE_PARAMETERS_END();

	if (argc > 1) {
		opt_err = (int)erropt;
	}

	if (_php_error_log_ex(opt_err, message, message_len, opt, headers) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* For BC (not binary-safe!) */
PHPAPI int _php_error_log(int opt_err, char *message, char *opt, char *headers) /* {{{ */
{
	return _php_error_log_ex(opt_err, message, (opt_err == 3) ? strlen(message) : 0, opt, headers);
}
/* }}} */

PHPAPI int _php_error_log_ex(int opt_err, char *message, size_t message_len, char *opt, char *headers) /* {{{ */
{
	php_stream *stream = NULL;
	size_t nbytes;

	switch (opt_err)
	{
		case 1:		/*send an email */
			if (!php_mail(opt, "PHP error_log message", message, headers, NULL)) {
				return FAILURE;
			}
			break;

		case 2:		/*send to an address */
			php_error_docref(NULL, E_WARNING, "TCP/IP option not available!");
			return FAILURE;
			break;

		case 3:		/*save to a file */
			stream = php_stream_open_wrapper(opt, "a", IGNORE_URL_WIN | REPORT_ERRORS, NULL);
			if (!stream) {
				return FAILURE;
			}
			nbytes = php_stream_write(stream, message, message_len);
			php_stream_close(stream);
			if (nbytes != message_len) {
				return FAILURE;
			}
			break;

		case 4: /* send to SAPI */
			if (sapi_module.log_message) {
				sapi_module.log_message(message, -1);
			} else {
				return FAILURE;
			}
			break;

		default:
			php_log_err_with_severity(message, LOG_NOTICE);
			break;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ proto array error_get_last()
   Get the last occurred error as associative array. Returns NULL if there hasn't been an error yet. */
PHP_FUNCTION(error_get_last)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PG(last_error_message)) {
		array_init(return_value);
		add_assoc_long_ex(return_value, "type", sizeof("type")-1, PG(last_error_type));
		add_assoc_string_ex(return_value, "message", sizeof("message")-1, PG(last_error_message));
		add_assoc_string_ex(return_value, "file", sizeof("file")-1, PG(last_error_file)?PG(last_error_file):"-");
		add_assoc_long_ex(return_value, "line", sizeof("line")-1, PG(last_error_lineno));
	}
}
/* }}} */

/* {{{ proto void error_clear_last(void)
   Clear the last occurred error. */
PHP_FUNCTION(error_clear_last)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PG(last_error_message)) {
		PG(last_error_type) = 0;
		PG(last_error_lineno) = 0;

		free(PG(last_error_message));
		PG(last_error_message) = NULL;

		if (PG(last_error_file)) {
			free(PG(last_error_file));
			PG(last_error_file) = NULL;
		}
	}
}
/* }}} */

/* {{{ proto mixed call_user_func(mixed function_name [, mixed parameter] [, mixed ...])
   Call a user function which is the first parameter
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
PHP_FUNCTION(call_user_func)
{
	zval retval;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_FUNC(fci, fci_cache)
		Z_PARAM_VARIADIC('*', fci.params, fci.param_count)
	ZEND_PARSE_PARAMETERS_END();

	fci.retval = &retval;

	if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		if (Z_ISREF(retval)) {
			zend_unwrap_reference(&retval);
		}
		ZVAL_COPY_VALUE(return_value, &retval);
	}
}
/* }}} */

/* {{{ proto mixed call_user_func_array(string function_name, array parameters)
   Call a user function which is the first parameter with the arguments contained in array
   Warning: This function is special-cased by zend_compile.c and so is usually bypassed */
PHP_FUNCTION(call_user_func_array)
{
	zval *params, retval;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_FUNC(fci, fci_cache)
		Z_PARAM_ARRAY(params)
	ZEND_PARSE_PARAMETERS_END();

	zend_fcall_info_args(&fci, params);
	fci.retval = &retval;

	if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		if (Z_ISREF(retval)) {
			zend_unwrap_reference(&retval);
		}
		ZVAL_COPY_VALUE(return_value, &retval);
	}

	zend_fcall_info_args_clear(&fci, 1);
}
/* }}} */

/* {{{ proto mixed forward_static_call(mixed function_name [, mixed parameter] [, mixed ...]) U
   Call a user function which is the first parameter */
PHP_FUNCTION(forward_static_call)
{
	zval retval;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	zend_class_entry *called_scope;

	ZEND_PARSE_PARAMETERS_START(1, -1)
		Z_PARAM_FUNC(fci, fci_cache)
		Z_PARAM_VARIADIC('*', fci.params, fci.param_count)
	ZEND_PARSE_PARAMETERS_END();

	if (!EX(prev_execute_data)->func->common.scope) {
		zend_throw_error(NULL, "Cannot call forward_static_call() when no class scope is active");
		return;
	}

	fci.retval = &retval;

	called_scope = zend_get_called_scope(execute_data);
	if (called_scope && fci_cache.calling_scope &&
		instanceof_function(called_scope, fci_cache.calling_scope)) {
			fci_cache.called_scope = called_scope;
	}

	if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		if (Z_ISREF(retval)) {
			zend_unwrap_reference(&retval);
		}
		ZVAL_COPY_VALUE(return_value, &retval);
	}
}
/* }}} */

/* {{{ proto mixed call_user_func_array(string function_name, array parameters) U
   Call a user function which is the first parameter with the arguments contained in array */
PHP_FUNCTION(forward_static_call_array)
{
	zval *params, retval;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	zend_class_entry *called_scope;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_FUNC(fci, fci_cache)
		Z_PARAM_ARRAY(params)
	ZEND_PARSE_PARAMETERS_END();

	zend_fcall_info_args(&fci, params);
	fci.retval = &retval;

	called_scope = zend_get_called_scope(execute_data);
	if (called_scope && fci_cache.calling_scope &&
		instanceof_function(called_scope, fci_cache.calling_scope)) {
			fci_cache.called_scope = called_scope;
	}

	if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		if (Z_ISREF(retval)) {
			zend_unwrap_reference(&retval);
		}
		ZVAL_COPY_VALUE(return_value, &retval);
	}

	zend_fcall_info_args_clear(&fci, 1);
}
/* }}} */

void user_shutdown_function_dtor(zval *zv) /* {{{ */
{
	int i;
	php_shutdown_function_entry *shutdown_function_entry = Z_PTR_P(zv);

	for (i = 0; i < shutdown_function_entry->arg_count; i++) {
		zval_ptr_dtor(&shutdown_function_entry->arguments[i]);
	}
	efree(shutdown_function_entry->arguments);
	efree(shutdown_function_entry);
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

static int user_shutdown_function_call(zval *zv) /* {{{ */
{
    php_shutdown_function_entry *shutdown_function_entry = Z_PTR_P(zv);
	zval retval;

	if (!zend_is_callable(&shutdown_function_entry->arguments[0], 0, NULL)) {
		zend_string *function_name
			= zend_get_callable_name(&shutdown_function_entry->arguments[0]);
		php_error(E_WARNING, "(Registered shutdown functions) Unable to call %s() - function does not exist", ZSTR_VAL(function_name));
		zend_string_release_ex(function_name, 0);
		return 0;
	}

	if (call_user_function(NULL, NULL,
				&shutdown_function_entry->arguments[0],
				&retval,
				shutdown_function_entry->arg_count - 1,
				shutdown_function_entry->arguments + 1) == SUCCESS)
	{
		zval_ptr_dtor(&retval);
	}
	return 0;
}
/* }}} */

static void user_tick_function_call(user_tick_function_entry *tick_fe) /* {{{ */
{
	zval retval;
	zval *function = &tick_fe->arguments[0];

	/* Prevent reentrant calls to the same user ticks function */
	if (! tick_fe->calling) {
		tick_fe->calling = 1;

		if (call_user_function(NULL, NULL,
								function,
								&retval,
								tick_fe->arg_count - 1,
								tick_fe->arguments + 1
								) == SUCCESS) {
			zval_ptr_dtor(&retval);

		} else {
			zval *obj, *method;

			if (Z_TYPE_P(function) == IS_STRING) {
				php_error_docref(NULL, E_WARNING, "Unable to call %s() - function does not exist", Z_STRVAL_P(function));
			} else if (	Z_TYPE_P(function) == IS_ARRAY
						&& (obj = zend_hash_index_find(Z_ARRVAL_P(function), 0)) != NULL
						&& (method = zend_hash_index_find(Z_ARRVAL_P(function), 1)) != NULL
						&& Z_TYPE_P(obj) == IS_OBJECT
						&& Z_TYPE_P(method) == IS_STRING) {
				php_error_docref(NULL, E_WARNING, "Unable to call %s::%s() - function does not exist", ZSTR_VAL(Z_OBJCE_P(obj)->name), Z_STRVAL_P(method));
			} else {
				php_error_docref(NULL, E_WARNING, "Unable to call tick function");
			}
		}

		tick_fe->calling = 0;
	}
}
/* }}} */

static void run_user_tick_functions(int tick_count, void *arg) /* {{{ */
{
	zend_llist_apply(BG(user_tick_functions), (llist_apply_func_t) user_tick_function_call);
}
/* }}} */

static int user_tick_function_compare(user_tick_function_entry * tick_fe1, user_tick_function_entry * tick_fe2) /* {{{ */
{
	zval *func1 = &tick_fe1->arguments[0];
	zval *func2 = &tick_fe2->arguments[0];
	int ret;

	if (Z_TYPE_P(func1) == IS_STRING && Z_TYPE_P(func2) == IS_STRING) {
		ret = zend_binary_zval_strcmp(func1, func2) == 0;
	} else if (Z_TYPE_P(func1) == IS_ARRAY && Z_TYPE_P(func2) == IS_ARRAY) {
		ret = zend_compare_arrays(func1, func2) == 0;
	} else if (Z_TYPE_P(func1) == IS_OBJECT && Z_TYPE_P(func2) == IS_OBJECT) {
		ret = zend_compare_objects(func1, func2) == 0;
	} else {
		ret = 0;
	}

	if (ret && tick_fe1->calling) {
		php_error_docref(NULL, E_WARNING, "Unable to delete tick function executed at the moment");
		return 0;
	}
	return ret;
}
/* }}} */

PHPAPI void php_call_shutdown_functions(void) /* {{{ */
{
	if (BG(user_shutdown_function_names)) {
		zend_try {
			zend_hash_apply(BG(user_shutdown_function_names), user_shutdown_function_call);
		}
		zend_end_try();
	}
}
/* }}} */

PHPAPI void php_free_shutdown_functions(void) /* {{{ */
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

/* {{{ proto void register_shutdown_function(callback function) U
   Register a user-level function to be called on request termination */
PHP_FUNCTION(register_shutdown_function)
{
	php_shutdown_function_entry shutdown_function_entry;
	int i;

	shutdown_function_entry.arg_count = ZEND_NUM_ARGS();

	if (shutdown_function_entry.arg_count < 1) {
		WRONG_PARAM_COUNT;
	}

	shutdown_function_entry.arguments = (zval *) safe_emalloc(sizeof(zval), shutdown_function_entry.arg_count, 0);

	if (zend_get_parameters_array(ZEND_NUM_ARGS(), shutdown_function_entry.arg_count, shutdown_function_entry.arguments) == FAILURE) {
		efree(shutdown_function_entry.arguments);
		RETURN_FALSE;
	}

	/* Prevent entering of anything but valid callback (syntax check only!) */
	if (!zend_is_callable(&shutdown_function_entry.arguments[0], 0, NULL)) {
		zend_string *callback_name
			= zend_get_callable_name(&shutdown_function_entry.arguments[0]);
		php_error_docref(NULL, E_WARNING, "Invalid shutdown callback '%s' passed", ZSTR_VAL(callback_name));
		efree(shutdown_function_entry.arguments);
		zend_string_release_ex(callback_name, 0);
		RETVAL_FALSE;
	} else {
		if (!BG(user_shutdown_function_names)) {
			ALLOC_HASHTABLE(BG(user_shutdown_function_names));
			zend_hash_init(BG(user_shutdown_function_names), 0, NULL, user_shutdown_function_dtor, 0);
		}

		for (i = 0; i < shutdown_function_entry.arg_count; i++) {
			Z_TRY_ADDREF(shutdown_function_entry.arguments[i]);
		}
		zend_hash_next_index_insert_mem(BG(user_shutdown_function_names), &shutdown_function_entry, sizeof(php_shutdown_function_entry));
	}
}
/* }}} */

PHPAPI zend_bool register_user_shutdown_function(char *function_name, size_t function_len, php_shutdown_function_entry *shutdown_function_entry) /* {{{ */
{
	if (!BG(user_shutdown_function_names)) {
		ALLOC_HASHTABLE(BG(user_shutdown_function_names));
		zend_hash_init(BG(user_shutdown_function_names), 0, NULL, user_shutdown_function_dtor, 0);
	}

	zend_hash_str_update_mem(BG(user_shutdown_function_names), function_name, function_len, shutdown_function_entry, sizeof(php_shutdown_function_entry));
	return 1;
}
/* }}} */

PHPAPI zend_bool remove_user_shutdown_function(char *function_name, size_t function_len) /* {{{ */
{
	if (BG(user_shutdown_function_names)) {
		return zend_hash_str_del(BG(user_shutdown_function_names), function_name, function_len) != FAILURE;
	}

	return 0;
}
/* }}} */

PHPAPI zend_bool append_user_shutdown_function(php_shutdown_function_entry shutdown_function_entry) /* {{{ */
{
	if (!BG(user_shutdown_function_names)) {
		ALLOC_HASHTABLE(BG(user_shutdown_function_names));
		zend_hash_init(BG(user_shutdown_function_names), 0, NULL, user_shutdown_function_dtor, 0);
	}

	return zend_hash_next_index_insert_mem(BG(user_shutdown_function_names), &shutdown_function_entry, sizeof(php_shutdown_function_entry)) != NULL;
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
	size_t filename_len;
	int ret;
	zend_syntax_highlighter_ini syntax_highlighter_ini;
	zend_bool i = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(i)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (php_check_open_basedir(filename)) {
		RETURN_FALSE;
	}

	if (i) {
		php_output_start_default();
	}

	php_get_highlight_struct(&syntax_highlighter_ini);

	ret = highlight_file(filename, &syntax_highlighter_ini);

	if (ret == FAILURE) {
		if (i) {
			php_output_end();
		}
		RETURN_FALSE;
	}

	if (i) {
		php_output_get_contents(return_value);
		php_output_discard();
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
	size_t filename_len;
	zend_lex_state original_lex_state;
	zend_file_handle file_handle;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH(filename, filename_len)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	php_output_start_default();

	zend_stream_init_filename(&file_handle, filename);
	zend_save_lexical_state(&original_lex_state);
	if (open_file_for_scanning(&file_handle) == FAILURE) {
		zend_restore_lexical_state(&original_lex_state);
		php_output_end();
		RETURN_EMPTY_STRING();
	}

	zend_strip();

	zend_destroy_file_handle(&file_handle);
	zend_restore_lexical_state(&original_lex_state);

	php_output_get_contents(return_value);
	php_output_discard();
}
/* }}} */

/* {{{ proto bool highlight_string(string string [, bool return] )
   Syntax highlight a string or optionally return it */
PHP_FUNCTION(highlight_string)
{
	zval *expr;
	zend_syntax_highlighter_ini syntax_highlighter_ini;
	char *hicompiled_string_description;
	zend_bool i = 0;
	int old_error_reporting = EG(error_reporting);

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(expr)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(i)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (!try_convert_to_string(expr)) {
		return;
	}

	if (i) {
		php_output_start_default();
	}

	EG(error_reporting) = E_ERROR;

	php_get_highlight_struct(&syntax_highlighter_ini);

	hicompiled_string_description = zend_make_compiled_string_description("highlighted code");

	if (highlight_string(expr, &syntax_highlighter_ini, hicompiled_string_description) == FAILURE) {
		efree(hicompiled_string_description);
		EG(error_reporting) = old_error_reporting;
		if (i) {
			php_output_end();
		}
		RETURN_FALSE;
	}
	efree(hicompiled_string_description);

	EG(error_reporting) = old_error_reporting;

	if (i) {
		php_output_get_contents(return_value);
		php_output_discard();
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto string ini_get(string varname)
   Get a configuration option */
PHP_FUNCTION(ini_get)
{
	zend_string *varname, *val;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(varname)
	ZEND_PARSE_PARAMETERS_END();

	val = zend_ini_get_value(varname);

	if (!val) {
		RETURN_FALSE;
	}

	if (ZSTR_IS_INTERNED(val)) {
		RETVAL_INTERNED_STR(val);
	} else if (ZSTR_LEN(val) == 0) {
		RETVAL_EMPTY_STRING();
	} else if (ZSTR_LEN(val) == 1) {
		RETVAL_INTERNED_STR(ZSTR_CHAR((zend_uchar)ZSTR_VAL(val)[0]));
	} else if (!(GC_FLAGS(val) & GC_PERSISTENT)) {
		ZVAL_NEW_STR(return_value, zend_string_copy(val));
	} else {
		ZVAL_NEW_STR(return_value, zend_string_init(ZSTR_VAL(val), ZSTR_LEN(val), 0));
	}
}
/* }}} */

/* {{{ proto array ini_get_all([string extension[, bool details = true]])
   Get all configuration options */
PHP_FUNCTION(ini_get_all)
{
	char *extname = NULL;
	size_t extname_len = 0, module_number = 0;
	zend_module_entry *module;
	zend_bool details = 1;
	zend_string *key;
	zend_ini_entry *ini_entry;


	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_EX(extname, extname_len, 1, 0)
		Z_PARAM_BOOL(details)
	ZEND_PARSE_PARAMETERS_END();

	zend_ini_sort_entries();

	if (extname) {
		if ((module = zend_hash_str_find_ptr(&module_registry, extname, extname_len)) == NULL) {
			php_error_docref(NULL, E_WARNING, "Unable to find extension '%s'", extname);
			RETURN_FALSE;
		}
		module_number = module->module_number;
	}

	array_init(return_value);
	ZEND_HASH_FOREACH_STR_KEY_PTR(EG(ini_directives), key, ini_entry) {
		zval option;

		if (module_number != 0 && ini_entry->module_number != module_number) {
			continue;
		}

		if (key == NULL || ZSTR_VAL(key)[0] != 0) {
			if (details) {
				array_init(&option);

				if (ini_entry->orig_value) {
					add_assoc_str(&option, "global_value", zend_string_copy(ini_entry->orig_value));
				} else if (ini_entry->value) {
					add_assoc_str(&option, "global_value", zend_string_copy(ini_entry->value));
				} else {
					add_assoc_null(&option, "global_value");
				}

				if (ini_entry->value) {
					add_assoc_str(&option, "local_value", zend_string_copy(ini_entry->value));
				} else {
					add_assoc_null(&option, "local_value");
				}

				add_assoc_long(&option, "access", ini_entry->modifiable);

				zend_symtable_update(Z_ARRVAL_P(return_value), ini_entry->name, &option);
			} else {
				if (ini_entry->value) {
					zval zv;

					ZVAL_STR_COPY(&zv, ini_entry->value);
					zend_symtable_update(Z_ARRVAL_P(return_value), ini_entry->name, &zv);
				} else {
					zend_symtable_update(Z_ARRVAL_P(return_value), ini_entry->name, &EG(uninitialized_zval));
				}
			}
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

static int php_ini_check_path(char *option_name, size_t option_len, char *new_option_name, size_t new_option_len) /* {{{ */
{
	if (option_len + 1 != new_option_len) {
		return 0;
	}

	return !strncmp(option_name, new_option_name, option_len);
}
/* }}} */

/* {{{ proto string ini_set(string varname, string newvalue)
   Set a configuration option, returns false on error and the old value of the configuration option on success */
PHP_FUNCTION(ini_set)
{
	zend_string *varname;
	zend_string *new_value;
	zend_string *val;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(varname)
		Z_PARAM_STR(new_value)
	ZEND_PARSE_PARAMETERS_END();

	val = zend_ini_get_value(varname);

	/* copy to return here, because alter might free it! */
	if (val) {
		if (ZSTR_IS_INTERNED(val)) {
			RETVAL_INTERNED_STR(val);
		} else if (ZSTR_LEN(val) == 0) {
			RETVAL_EMPTY_STRING();
		} else if (ZSTR_LEN(val) == 1) {
			RETVAL_INTERNED_STR(ZSTR_CHAR((zend_uchar)ZSTR_VAL(val)[0]));
		} else if (!(GC_FLAGS(val) & GC_PERSISTENT)) {
			ZVAL_NEW_STR(return_value, zend_string_copy(val));
		} else {
			ZVAL_NEW_STR(return_value, zend_string_init(ZSTR_VAL(val), ZSTR_LEN(val), 0));
		}
	} else {
		RETVAL_FALSE;
	}

#define _CHECK_PATH(var, var_len, ini) php_ini_check_path(var, var_len, ini, sizeof(ini))
	/* open basedir check */
	if (PG(open_basedir)) {
		if (_CHECK_PATH(ZSTR_VAL(varname), ZSTR_LEN(varname), "error_log") ||
			_CHECK_PATH(ZSTR_VAL(varname), ZSTR_LEN(varname), "java.class.path") ||
			_CHECK_PATH(ZSTR_VAL(varname), ZSTR_LEN(varname), "java.home") ||
			_CHECK_PATH(ZSTR_VAL(varname), ZSTR_LEN(varname), "mail.log") ||
			_CHECK_PATH(ZSTR_VAL(varname), ZSTR_LEN(varname), "java.library.path") ||
			_CHECK_PATH(ZSTR_VAL(varname), ZSTR_LEN(varname), "vpopmail.directory")) {
			if (php_check_open_basedir(ZSTR_VAL(new_value))) {
				zval_ptr_dtor_str(return_value);
				RETURN_FALSE;
			}
		}
	}
#undef _CHECK_PATH

	if (zend_alter_ini_entry_ex(varname, new_value, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0) == FAILURE) {
		zval_ptr_dtor_str(return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto void ini_restore(string varname)
   Restore the value of a configuration option specified by varname */
PHP_FUNCTION(ini_restore)
{
	zend_string *varname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(varname)
	ZEND_PARSE_PARAMETERS_END();

	zend_restore_ini_entry(varname, PHP_INI_STAGE_RUNTIME);
}
/* }}} */

/* {{{ proto string set_include_path(string new_include_path)
   Sets the include_path configuration option */
PHP_FUNCTION(set_include_path)
{
	zend_string *new_value;
	char *old_value;
	zend_string *key;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR(new_value)
	ZEND_PARSE_PARAMETERS_END();

	old_value = zend_ini_string("include_path", sizeof("include_path") - 1, 0);
	/* copy to return here, because alter might free it! */
	if (old_value) {
		RETVAL_STRING(old_value);
	} else {
		RETVAL_FALSE;
	}

	key = zend_string_init("include_path", sizeof("include_path") - 1, 0);
	if (zend_alter_ini_entry_ex(key, new_value, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0) == FAILURE) {
		zend_string_release_ex(key, 0);
		zval_ptr_dtor_str(return_value);
		RETURN_FALSE;
	}
	zend_string_release_ex(key, 0);
}
/* }}} */

/* {{{ proto string get_include_path()
   Get the current include_path configuration option */
PHP_FUNCTION(get_include_path)
{
	char *str;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	str = zend_ini_string("include_path", sizeof("include_path") - 1, 0);

	if (str == NULL) {
		RETURN_FALSE;
	}

	RETURN_STRING(str);
}
/* }}} */

/* {{{ proto void restore_include_path()
   Restore the value of the include_path configuration option */
PHP_FUNCTION(restore_include_path)
{
	zend_string *key;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	key = zend_string_init("include_path", sizeof("include_path")-1, 0);
	zend_restore_ini_entry(key, PHP_INI_STAGE_RUNTIME);
	zend_string_efree(key);
}
/* }}} */

/* {{{ proto mixed print_r(mixed var [, bool return])
   Prints out or returns information about the specified variable */
PHP_FUNCTION(print_r)
{
	zval *var;
	zend_bool do_return = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(var)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(do_return)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (do_return) {
		RETURN_STR(zend_print_zval_r_to_str(var, 0));
	} else {
		zend_print_zval_r(var, 0);
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

/* {{{ proto int ignore_user_abort([bool value])
   Set whether we want to ignore a user abort event or not */
PHP_FUNCTION(ignore_user_abort)
{
	zend_bool arg = 0;
	int old_setting;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(arg)
	ZEND_PARSE_PARAMETERS_END();

	old_setting = (unsigned short)PG(ignore_user_abort);

	if (ZEND_NUM_ARGS()) {
		zend_string *key = zend_string_init("ignore_user_abort", sizeof("ignore_user_abort") - 1, 0);
		zend_alter_ini_entry_chars(key, arg ? "1" : "0", 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release_ex(key, 0);
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
	size_t name_len, proto_len;
	struct servent *serv;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(name, name_len)
		Z_PARAM_STRING(proto, proto_len)
	ZEND_PARSE_PARAMETERS_END();


/* empty string behaves like NULL on windows implementation of
   getservbyname. Let be portable instead. */
#ifdef PHP_WIN32
	if (proto_len == 0) {
		RETURN_FALSE;
	}
#endif

	serv = getservbyname(name, proto);

#if defined(_AIX)
	/*
        On AIX, imap is only known as imap2 in /etc/services, while on Linux imap is an alias for imap2.
        If a request for imap gives no result, we try again with imap2.
        */
	if (serv == NULL && strcmp(name,  "imap") == 0) {
		serv = getservbyname("imap2", proto);
	}
#endif
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
	size_t proto_len;
	zend_long port;
	struct servent *serv;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(port)
		Z_PARAM_STRING(proto, proto_len)
	ZEND_PARSE_PARAMETERS_END();

	serv = getservbyport(htons((unsigned short) port), proto);

	if (serv == NULL) {
		RETURN_FALSE;
	}

	RETURN_STRING(serv->s_name);
}
/* }}} */
#endif

#if HAVE_GETPROTOBYNAME
/* {{{ proto int getprotobyname(string name)
   Returns protocol number associated with name as per /etc/protocols */
PHP_FUNCTION(getprotobyname)
{
	char *name;
	size_t name_len;
	struct protoent *ent;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(name, name_len)
	ZEND_PARSE_PARAMETERS_END();

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
	zend_long proto;
	struct protoent *ent;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(proto)
	ZEND_PARSE_PARAMETERS_END();

	ent = getprotobynumber((int)proto);

	if (ent == NULL) {
		RETURN_FALSE;
	}

	RETURN_STRING(ent->p_name);
}
/* }}} */
#endif

/* {{{ proto bool register_tick_function(string function_name [, mixed arg [, mixed ... ]])
   Registers a tick callback function */
PHP_FUNCTION(register_tick_function)
{
	user_tick_function_entry tick_fe;
	int i;
	zend_string *function_name = NULL;

	tick_fe.calling = 0;
	tick_fe.arg_count = ZEND_NUM_ARGS();

	if (tick_fe.arg_count < 1) {
		WRONG_PARAM_COUNT;
	}

	tick_fe.arguments = (zval *) safe_emalloc(sizeof(zval), tick_fe.arg_count, 0);

	if (zend_get_parameters_array(ZEND_NUM_ARGS(), tick_fe.arg_count, tick_fe.arguments) == FAILURE) {
		efree(tick_fe.arguments);
		RETURN_FALSE;
	}

	if (!zend_is_callable(&tick_fe.arguments[0], 0, &function_name)) {
		efree(tick_fe.arguments);
		php_error_docref(NULL, E_WARNING, "Invalid tick callback '%s' passed", ZSTR_VAL(function_name));
		zend_string_release_ex(function_name, 0);
		RETURN_FALSE;
	} else if (function_name) {
		zend_string_release_ex(function_name, 0);
	}

	if (Z_TYPE(tick_fe.arguments[0]) != IS_ARRAY && Z_TYPE(tick_fe.arguments[0]) != IS_OBJECT) {
		convert_to_string_ex(&tick_fe.arguments[0]);
	}

	if (!BG(user_tick_functions)) {
		BG(user_tick_functions) = (zend_llist *) emalloc(sizeof(zend_llist));
		zend_llist_init(BG(user_tick_functions),
						sizeof(user_tick_function_entry),
						(llist_dtor_func_t) user_tick_function_dtor, 0);
		php_add_tick_function(run_user_tick_functions, NULL);
	}

	for (i = 0; i < tick_fe.arg_count; i++) {
		Z_TRY_ADDREF(tick_fe.arguments[i]);
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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(function)
	ZEND_PARSE_PARAMETERS_END();

	if (!BG(user_tick_functions)) {
		return;
	}

	if (Z_TYPE_P(function) != IS_ARRAY && Z_TYPE_P(function) != IS_OBJECT) {
		convert_to_string(function);
	}

	tick_fe.arguments = (zval *) emalloc(sizeof(zval));
	ZVAL_COPY_VALUE(&tick_fe.arguments[0], function);
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
	size_t path_len;

	if (!SG(rfc1867_uploaded_files)) {
		RETURN_FALSE;
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(path, path_len)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_hash_str_exists(SG(rfc1867_uploaded_files), path, path_len)) {
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
	size_t path_len, new_path_len;
	zend_bool successful = 0;

#ifndef PHP_WIN32
	int oldmask; int ret;
#endif

	if (!SG(rfc1867_uploaded_files)) {
		RETURN_FALSE;
	}

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(path, path_len)
		Z_PARAM_PATH(new_path, new_path_len)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_hash_str_exists(SG(rfc1867_uploaded_files), path, path_len)) {
		RETURN_FALSE;
	}

	if (php_check_open_basedir(new_path)) {
		RETURN_FALSE;
	}

	if (VCWD_RENAME(path, new_path) == 0) {
		successful = 1;
#ifndef PHP_WIN32
		oldmask = umask(077);
		umask(oldmask);

		ret = VCWD_CHMOD(new_path, 0666 & ~oldmask);

		if (ret == -1) {
			php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
		}
#endif
	} else if (php_copy_file_ex(path, new_path, STREAM_DISABLE_OPEN_BASEDIR) == SUCCESS) {
		VCWD_UNLINK(path);
		successful = 1;
	}

	if (successful) {
		zend_hash_str_del(SG(rfc1867_uploaded_files), path, path_len);
	} else {
		php_error_docref(NULL, E_WARNING, "Unable to move '%s' to '%s'", path, new_path);
	}

	RETURN_BOOL(successful);
}
/* }}} */

/* {{{ php_simple_ini_parser_cb
 */
static void php_simple_ini_parser_cb(zval *arg1, zval *arg2, zval *arg3, int callback_type, zval *arr)
{
	switch (callback_type) {

		case ZEND_INI_PARSER_ENTRY:
			if (!arg2) {
				/* bare string - nothing to do */
				break;
			}
			Z_TRY_ADDREF_P(arg2);
			zend_symtable_update(Z_ARRVAL_P(arr), Z_STR_P(arg1), arg2);
			break;

		case ZEND_INI_PARSER_POP_ENTRY:
		{
			zval hash, *find_hash;

			if (!arg2) {
				/* bare string - nothing to do */
				break;
			}

			if (!(Z_STRLEN_P(arg1) > 1 && Z_STRVAL_P(arg1)[0] == '0') && is_numeric_string(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1), NULL, NULL, 0) == IS_LONG) {
				zend_ulong key = (zend_ulong) zend_atol(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1));
				if ((find_hash = zend_hash_index_find(Z_ARRVAL_P(arr), key)) == NULL) {
					array_init(&hash);
					find_hash = zend_hash_index_add_new(Z_ARRVAL_P(arr), key, &hash);
				}
			} else {
				if ((find_hash = zend_hash_find(Z_ARRVAL_P(arr), Z_STR_P(arg1))) == NULL) {
					array_init(&hash);
					find_hash = zend_hash_add_new(Z_ARRVAL_P(arr), Z_STR_P(arg1), &hash);
				}
			}

			if (Z_TYPE_P(find_hash) != IS_ARRAY) {
				zval_ptr_dtor_nogc(find_hash);
				array_init(find_hash);
			}

			if (!arg3 || (Z_TYPE_P(arg3) == IS_STRING && Z_STRLEN_P(arg3) == 0)) {
				Z_TRY_ADDREF_P(arg2);
				add_next_index_zval(find_hash, arg2);
			} else {
				array_set_zval_key(Z_ARRVAL_P(find_hash), arg3, arg2);
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
static void php_ini_parser_cb_with_sections(zval *arg1, zval *arg2, zval *arg3, int callback_type, zval *arr)
{
	if (callback_type == ZEND_INI_PARSER_SECTION) {
		array_init(&BG(active_ini_file_section));
		zend_symtable_update(Z_ARRVAL_P(arr), Z_STR_P(arg1), &BG(active_ini_file_section));
	} else if (arg2) {
		zval *active_arr;

		if (Z_TYPE(BG(active_ini_file_section)) != IS_UNDEF) {
			active_arr = &BG(active_ini_file_section);
		} else {
			active_arr = arr;
		}

		php_simple_ini_parser_cb(arg1, arg2, arg3, callback_type, active_arr);
	}
}
/* }}} */

/* {{{ proto array parse_ini_file(string filename [, bool process_sections [, int scanner_mode]])
   Parse configuration file */
PHP_FUNCTION(parse_ini_file)
{
	char *filename = NULL;
	size_t filename_len = 0;
	zend_bool process_sections = 0;
	zend_long scanner_mode = ZEND_INI_SCANNER_NORMAL;
	zend_file_handle fh;
	zend_ini_parser_cb_t ini_parser_cb;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_PATH(filename, filename_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(process_sections)
		Z_PARAM_LONG(scanner_mode)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (filename_len == 0) {
		php_error_docref(NULL, E_WARNING, "Filename cannot be empty!");
		RETURN_FALSE;
	}

	/* Set callback function */
	if (process_sections) {
		ZVAL_UNDEF(&BG(active_ini_file_section));
		ini_parser_cb = (zend_ini_parser_cb_t) php_ini_parser_cb_with_sections;
	} else {
		ini_parser_cb = (zend_ini_parser_cb_t) php_simple_ini_parser_cb;
	}

	/* Setup filehandle */
	zend_stream_init_filename(&fh, filename);

	array_init(return_value);
	if (zend_parse_ini_file(&fh, 0, (int)scanner_mode, ini_parser_cb, return_value) == FAILURE) {
		zend_array_destroy(Z_ARR_P(return_value));
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array parse_ini_string(string ini_string [, bool process_sections [, int scanner_mode]])
   Parse configuration string */
PHP_FUNCTION(parse_ini_string)
{
	char *string = NULL, *str = NULL;
	size_t str_len = 0;
	zend_bool process_sections = 0;
	zend_long scanner_mode = ZEND_INI_SCANNER_NORMAL;
	zend_ini_parser_cb_t ini_parser_cb;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(process_sections)
		Z_PARAM_LONG(scanner_mode)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

	if (INT_MAX - str_len < ZEND_MMAP_AHEAD) {
		RETVAL_FALSE;
	}

	/* Set callback function */
	if (process_sections) {
		ZVAL_UNDEF(&BG(active_ini_file_section));
		ini_parser_cb = (zend_ini_parser_cb_t) php_ini_parser_cb_with_sections;
	} else {
		ini_parser_cb = (zend_ini_parser_cb_t) php_simple_ini_parser_cb;
	}

	/* Setup string */
	string = (char *) emalloc(str_len + ZEND_MMAP_AHEAD);
	memcpy(string, str, str_len);
	memset(string + str_len, 0, ZEND_MMAP_AHEAD);

	array_init(return_value);
	if (zend_parse_ini_string(string, 0, (int)scanner_mode, ini_parser_cb, return_value) == FAILURE) {
		zend_array_destroy(Z_ARR_P(return_value));
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
	add_config_entries(hash, return_value);
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
