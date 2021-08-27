/*
   +----------------------------------------------------------------------+
   | Zend Engine, Func Info                                               |
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
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   |          Xinchen Hui <laruence@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend_compile.h"
#include "zend_extensions.h"
#include "zend_ssa.h"
#include "zend_optimizer_internal.h"
#include "zend_inference.h"
#include "zend_call_graph.h"
#include "zend_func_info.h"
#include "zend_inference.h"
#ifdef _WIN32
#include "win32/ioutil.h"
#endif

typedef uint32_t (*info_func_t)(const zend_call_info *call_info, const zend_ssa *ssa);

typedef struct _func_info_t {
	const char *name;
	int         name_len;
	uint32_t    info;
	info_func_t info_func;
} func_info_t;

#define F0(name, info) \
	{name, sizeof(name)-1, (info), NULL}
#define F1(name, info) \
	{name, sizeof(name)-1, (MAY_BE_RC1 | (info)), NULL}
#define FN(name, info) \
	{name, sizeof(name)-1, (MAY_BE_RC1 | MAY_BE_RCN | (info)), NULL}
#define FC(name, callback) \
	{name, sizeof(name)-1, 0, callback}

#include "zend_func_infos.h"

static uint32_t zend_range_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	if (!call_info->send_unpack
	 && (call_info->num_args == 2 || call_info->num_args == 3)
	 && ssa
	 && !(ssa->cfg.flags & ZEND_SSA_TSSA)) {
		zend_op_array *op_array = call_info->caller_op_array;
		uint32_t t1 = _ssa_op1_info(op_array, ssa, call_info->arg_info[0].opline,
			&ssa->ops[call_info->arg_info[0].opline - op_array->opcodes]);
		uint32_t t2 = _ssa_op1_info(op_array, ssa, call_info->arg_info[1].opline,
			&ssa->ops[call_info->arg_info[1].opline - op_array->opcodes]);
		uint32_t t3 = 0;
		uint32_t tmp = MAY_BE_RC1 | MAY_BE_ARRAY | MAY_BE_ARRAY_PACKED;

		if (call_info->num_args == 3) {
			t3 = _ssa_op1_info(op_array, ssa, call_info->arg_info[2].opline,
				&ssa->ops[call_info->arg_info[2].opline - op_array->opcodes]);
		}
		if ((t1 & MAY_BE_STRING) && (t2 & MAY_BE_STRING)) {
			tmp |= MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING;
		}
		if ((t1 & (MAY_BE_DOUBLE|MAY_BE_STRING))
				|| (t2 & (MAY_BE_DOUBLE|MAY_BE_STRING))
				|| (t3 & (MAY_BE_DOUBLE|MAY_BE_STRING))) {
			tmp |= MAY_BE_ARRAY_OF_DOUBLE;
		}
		if ((t1 & (MAY_BE_ANY-(MAY_BE_STRING|MAY_BE_DOUBLE))) && (t2 & (MAY_BE_ANY-(MAY_BE_STRING|MAY_BE_DOUBLE)))) {
			if ((t3 & MAY_BE_ANY) != MAY_BE_DOUBLE) {
				tmp |= MAY_BE_ARRAY_OF_LONG;
			}
		}
		return tmp;
	} else {
		/* May throw */
		return MAY_BE_RC1 | MAY_BE_ARRAY | MAY_BE_ARRAY_PACKED | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING;
	}
}

static const func_info_t old_func_infos[] = {
	/* zend */
	F1("get_class_vars",          MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF),
	F1("get_defined_vars",        MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF),

	/* ext/standard */
	F1("base64_decode",                MAY_BE_FALSE | MAY_BE_STRING),
	F1("base64_encode",                MAY_BE_STRING),
	F1("password_hash",                MAY_BE_STRING),
	F1("password_get_info",            MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("convert_uuencode",             MAY_BE_STRING),
	F1("convert_uudecode",             MAY_BE_FALSE | MAY_BE_STRING),
	F1("pow",                          MAY_BE_LONG | MAY_BE_DOUBLE | MAY_BE_OBJECT),
	F1("decbin",                       MAY_BE_STRING),
	F1("decoct",                       MAY_BE_STRING),
	F1("dechex",                       MAY_BE_STRING),
	F1("base_convert",                 MAY_BE_STRING),
	F1("number_format",                MAY_BE_STRING),
#ifdef HAVE_INET_NTOP
	F1("inet_ntop",                    MAY_BE_FALSE | MAY_BE_STRING),
#endif
#ifdef HAVE_INET_PTON
	F1("inet_pton",                    MAY_BE_FALSE | MAY_BE_STRING),
#endif
	F1("long2ip",                      MAY_BE_FALSE | MAY_BE_STRING),
	F1("getenv",                       MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
#ifdef HAVE_PUTENV
#endif
	F1("getopt",                       MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
#ifdef HAVE_GETLOADAVG
	F1("sys_getloadavg",               MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_DOUBLE),
#endif
#ifdef HAVE_GETTIMEOFDAY
	F1("microtime",                    MAY_BE_DOUBLE | MAY_BE_STRING),
	F1("gettimeofday",                 MAY_BE_DOUBLE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG),
#endif
#ifdef HAVE_GETRUSAGE
	F1("getrusage",                    MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG),
#endif
#ifdef HAVE_GETTIMEOFDAY
	F1("uniqid",                       MAY_BE_STRING),
#endif
	F1("quoted_printable_decode",      MAY_BE_STRING),
	F1("quoted_printable_encode",      MAY_BE_STRING),
	F1("get_current_user",             MAY_BE_STRING),
	F1("get_cfg_var",                  MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("error_get_last",               MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("serialize",                    MAY_BE_STRING),
	F1("var_export",                   MAY_BE_NULL | MAY_BE_STRING),
	F1("print_r",                      MAY_BE_TRUE | MAY_BE_STRING),
	F0("register_shutdown_function",   MAY_BE_NULL | MAY_BE_FALSE),
	F1("highlight_file",               MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("show_source",                  MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("highlight_string",             MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("php_strip_whitespace",         MAY_BE_STRING),
	F1("ini_get_all",                  MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("ini_alter",                    MAY_BE_FALSE | MAY_BE_STRING),
	F1("get_include_path",             MAY_BE_FALSE | MAY_BE_STRING),
	F1("set_include_path",             MAY_BE_FALSE | MAY_BE_STRING),
	F1("headers_list",                 MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("parse_ini_file",               MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("parse_ini_string",             MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
#if ZEND_DEBUG
	F1("config_get_hash",              MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
#endif
	F1("gethostbyaddr",                MAY_BE_FALSE | MAY_BE_STRING),
	F1("gethostbyname",                MAY_BE_STRING),
	F1("gethostbynamel",               MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
#ifdef HAVE_GETHOSTNAME
	F1("gethostname",                  MAY_BE_FALSE | MAY_BE_STRING),
#endif
	F1("fgets",                        MAY_BE_FALSE | MAY_BE_STRING),
	F1("fread",                        MAY_BE_FALSE | MAY_BE_STRING),
	F1("fstat",                        MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG),
	F1("tempnam",                      MAY_BE_FALSE | MAY_BE_STRING),
	F1("file",                         MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("file_get_contents",            MAY_BE_FALSE | MAY_BE_STRING),
	F1("stream_context_get_params",    MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	FN("stream_context_get_options",   MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("stream_socket_get_name",       MAY_BE_FALSE | MAY_BE_STRING),
	F1("stream_socket_recvfrom",       MAY_BE_FALSE | MAY_BE_STRING),
#if HAVE_SOCKETPAIR
	F1("stream_socket_pair",           MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_RESOURCE),
#endif
	F1("stream_get_contents",          MAY_BE_FALSE | MAY_BE_STRING),
	F1("fgetcsv",                      MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING),
	F1("get_meta_tags",                MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("stream_get_meta_data",         MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("stream_get_line",              MAY_BE_FALSE | MAY_BE_STRING),
	F1("stream_get_wrappers",          MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("stream_get_transports",        MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("stream_resolve_include_path",  MAY_BE_FALSE | MAY_BE_STRING),
	F1("get_headers",                  MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("socket_get_status",            MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("realpath",                     MAY_BE_FALSE | MAY_BE_STRING),
	F1("pack",                         MAY_BE_STRING),
	F1("unpack",                       MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("get_browser",                  MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_OBJECT | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("crypt",                        MAY_BE_STRING),
	F1("getcwd",                       MAY_BE_FALSE | MAY_BE_STRING),
	F1("readdir",                      MAY_BE_FALSE | MAY_BE_STRING),
	F1("dir",                          MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("scandir",                      MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
#ifdef HAVE_GLOB
	F1("glob",                         MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
#endif
	F1("filetype",                     MAY_BE_FALSE | MAY_BE_STRING),
	F1("stat",                         MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("lstat",                        MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("realpath_cache_get",           MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ARRAY),
#ifdef HAVE_SYSLOG_H
	F0("syslog",                       MAY_BE_TRUE),
	F0("closelog",                     MAY_BE_TRUE),
#endif
	F1("metaphone",                    MAY_BE_STRING),
	F1("ob_get_flush",                 MAY_BE_FALSE | MAY_BE_STRING),
	F1("ob_get_clean",                 MAY_BE_FALSE | MAY_BE_STRING),
	F1("ob_get_status",                MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("ob_list_handlers",             MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F0("array_walk",                   MAY_BE_TRUE),
	F0("array_walk_recursive",         MAY_BE_TRUE),
	F0("arsort",                       MAY_BE_TRUE),
	F0("asort",                        MAY_BE_TRUE),
	F0("krsort",                       MAY_BE_TRUE),
	F0("ksort",                        MAY_BE_TRUE),
	F0("shuffle",                      MAY_BE_TRUE),
	F0("sort",                         MAY_BE_TRUE),
	F0("usort",                        MAY_BE_TRUE),
	F0("uasort",                       MAY_BE_TRUE),
	F0("uksort",                       MAY_BE_TRUE),
	F1("compact",                      MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	FN("array_fill",                   MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	F1("array_fill_keys",              MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	FC("range",                        zend_range_info),
	F1("array_replace",                MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_replace_recursive",      MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	FN("array_keys",                   MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	FN("array_values",                 MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_count_values",           MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG),
	F1("array_column",                 MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_reverse",                MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_flip",                   MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("array_change_key_case",        MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	FN("array_rand",                   MAY_BE_LONG | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("array_intersect",              MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_intersect_key",          MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_intersect_ukey",         MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_uintersect",             MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_intersect_assoc",        MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_uintersect_assoc",       MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_intersect_uassoc",       MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_uintersect_uassoc",      MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_diff_key",               MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_diff_ukey",              MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_udiff",                  MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_diff_assoc",             MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_udiff_assoc",            MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_diff_uassoc",            MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_udiff_uassoc",           MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("str_rot13",                    MAY_BE_STRING),
	F1("stream_get_filters",           MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("stream_bucket_make_writeable", MAY_BE_NULL | MAY_BE_OBJECT),
	F1("stream_bucket_new",            MAY_BE_OBJECT),
	F1("sys_get_temp_dir",             MAY_BE_STRING),

	/* ext/preg */
	F1("preg_grep",				                MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),

	F1("utf8_encode",                           MAY_BE_STRING),
	F1("utf8_decode",                           MAY_BE_STRING),

	/* ext/filter */
	F1("filter_var_array",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF),

	/* ext/spl */
	F1("iterator_to_array",						MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),

};

static HashTable func_info;
ZEND_API int zend_func_info_rid = -1;

uint32_t zend_get_internal_func_info(
		const zend_function *callee_func, const zend_call_info *call_info, const zend_ssa *ssa) {
	if (callee_func->common.scope) {
		/* This is a method, not a function. */
		return 0;
	}

	zend_string *name = callee_func->common.function_name;
	if (!name) {
		/* zend_pass_function has no name. */
		return 0;
	}

	zval *zv = zend_hash_find_known_hash(&func_info, name);
	if (!zv) {
		return 0;
	}

	func_info_t *info = Z_PTR_P(zv);
	if (info->info_func) {
		return call_info ? info->info_func(call_info, ssa) : 0;
	} else {
		return info->info;
	}
}

ZEND_API uint32_t zend_get_func_info(
		const zend_call_info *call_info, const zend_ssa *ssa,
		zend_class_entry **ce, bool *ce_is_instanceof)
{
	uint32_t ret = 0;
	const zend_function *callee_func = call_info->callee_func;
	*ce = NULL;
	*ce_is_instanceof = 0;

	if (callee_func->type == ZEND_INTERNAL_FUNCTION) {
		uint32_t internal_ret = zend_get_internal_func_info(callee_func, call_info, ssa);
#if !ZEND_DEBUG
		if (internal_ret) {
			return internal_ret;
		}
#endif

		ret = zend_get_return_info_from_signature_only(
			callee_func, /* script */ NULL, ce, ce_is_instanceof, /* use_tentative_return_info */ !call_info->is_prototype);

#if ZEND_DEBUG
		if (internal_ret) {
			zend_string *name = callee_func->common.function_name;
			/* Check whether the func_info information is a subset of the information we can
			 * compute from the specified return type, otherwise it contains redundant types. */
			if (internal_ret & ~ret) {
				fprintf(stderr, "Inaccurate func info for %s()\n", ZSTR_VAL(name));
			}
			/* Check whether the func info is completely redundant with arginfo. */
			if (internal_ret == ret) {
				fprintf(stderr, "Useless func info for %s()\n", ZSTR_VAL(name));
			}
			/* If the return type is not mixed, check that the types match exactly if we exclude
			 * RC and array information. */
			uint32_t ret_any = ret & MAY_BE_ANY, internal_ret_any = internal_ret & MAY_BE_ANY;
			if (ret_any != MAY_BE_ANY) {
				uint32_t diff = internal_ret_any ^ ret_any;
				/* Func info may contain "true" types as well as isolated "null" and "false". */
				if (diff && !(diff == MAY_BE_FALSE && (ret & MAY_BE_FALSE))
						&& (internal_ret_any & ~(MAY_BE_NULL|MAY_BE_FALSE))) {
					fprintf(stderr, "Incorrect func info for %s()\n", ZSTR_VAL(name));
				}
			}
			return internal_ret;
		}
#endif
	} else {
		if (!call_info->is_prototype) {
			// FIXME: the order of functions matters!!!
			zend_func_info *info = ZEND_FUNC_INFO((zend_op_array*)callee_func);
			if (info) {
				ret = info->return_info.type;
				*ce = info->return_info.ce;
				*ce_is_instanceof = info->return_info.is_instanceof;
			}
		}
		if (!ret) {
			ret = zend_get_return_info_from_signature_only(
				callee_func, /* TODO: script */ NULL, ce, ce_is_instanceof, /* use_tentative_return_info */ !call_info->is_prototype);
		}
	}
	return ret;
}

void zend_func_info_add(const func_info_t *func_infos, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {
		zend_string *key = zend_string_init_interned(func_infos[i].name, func_infos[i].name_len, 1);

		if (zend_hash_add_ptr(&func_info, key, (void**)&func_infos[i]) == NULL) {
			fprintf(stderr, "ERROR: Duplicate function info for \"%s\"\n", func_infos[i].name);
		}

		zend_string_release_ex(key, 1);
	}
}

int zend_func_info_startup(void)
{
	if (zend_func_info_rid == -1) {
		zend_func_info_rid = zend_get_resource_handle("Zend Optimizer");
		if (zend_func_info_rid < 0) {
			return FAILURE;
		}

		zend_hash_init(&func_info, sizeof(old_func_infos)/sizeof(func_info_t) + sizeof(func_infos)/sizeof(func_info_t), NULL, NULL, 1);

		zend_func_info_add(old_func_infos, sizeof(old_func_infos)/sizeof(func_info_t));
		zend_func_info_add(func_infos, sizeof(func_infos)/sizeof(func_info_t));
	}

	return SUCCESS;
}

int zend_func_info_shutdown(void)
{
	if (zend_func_info_rid != -1) {
		zend_hash_destroy(&func_info);
		zend_func_info_rid = -1;
	}
	return SUCCESS;
}
