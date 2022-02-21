/*
   +----------------------------------------------------------------------+
   | Zend Engine, Func Info                                               |
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
   | Authors: Dmitry Stogov <dmitry@php.net>                              |
   |          Xinchen Hui <laruence@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "php.h"
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
#define FR(name, info) \
	{name, sizeof(name)-1, (MAY_BE_REF | (info)), NULL}
#define FX(name, info) \
	{name, sizeof(name)-1, (MAY_BE_RC1 | MAY_BE_RCN | MAY_BE_REF | (info)), NULL}
#define FC(name, callback) \
	{name, sizeof(name)-1, 0, callback}

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
		if ((t1 & (MAY_BE_ANY-MAY_BE_DOUBLE)) && (t2 & (MAY_BE_ANY-MAY_BE_DOUBLE))) {
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

#define UNKNOWN_INFO (MAY_BE_ANY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF)

static const func_info_t func_infos[] = {
	/* zend */
	F1("zend_version",            MAY_BE_STRING),
	FN("func_get_arg",            UNKNOWN_INFO),
	FN("func_get_args",           MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	F1("get_class_vars",          MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF),
	F1("get_class_methods",       MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("get_included_files",      MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	FN("set_error_handler",       MAY_BE_NULL | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_OBJECT | MAY_BE_OBJECT),
	F0("restore_error_handler",   MAY_BE_TRUE),
	F0("restore_exception_handler", MAY_BE_TRUE),
	F1("get_declared_traits",     MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("get_declared_classes",    MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("get_declared_interfaces", MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("get_defined_functions",   MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("get_defined_vars",        MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF),
	F1("get_resource_type",       MAY_BE_STRING),
	F1("get_defined_constants",   MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_RESOURCE | MAY_BE_ARRAY_OF_ARRAY),
	F1("debug_backtrace",         MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ARRAY),
	F1("get_loaded_extensions",   MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("get_extension_funcs",     MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),

	/* ext/standard */
	FN("constant",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_LONG | MAY_BE_DOUBLE | MAY_BE_STRING | MAY_BE_RESOURCE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("bin2hex",                      MAY_BE_STRING),
	F1("hex2bin",                      MAY_BE_FALSE | MAY_BE_STRING),
#if HAVE_NANOSLEEP
	F1("time_nanosleep",               MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG),
#endif
#if HAVE_STRPTIME
	F1("strptime",                     MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
#endif
	F1("wordwrap",                     MAY_BE_STRING),
	F1("htmlspecialchars",             MAY_BE_STRING),
	F1("htmlentities",                 MAY_BE_STRING),
	F1("get_html_translation_table",   MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("sha1",                         MAY_BE_STRING),
	F1("sha1_file",                    MAY_BE_FALSE | MAY_BE_STRING),
	F1("md5",                          MAY_BE_STRING),
	F1("md5_file",                     MAY_BE_FALSE | MAY_BE_STRING),
	F1("iptcparse",                    MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("iptcembed",                    MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("getimagesize",                 MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("getimagesizefromstring",       MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("image_type_to_mime_type",      MAY_BE_STRING),
	F1("image_type_to_extension",      MAY_BE_FALSE | MAY_BE_STRING),
	F0("phpinfo",                      MAY_BE_TRUE),
	F1("phpversion",                   MAY_BE_FALSE | MAY_BE_STRING),
	F0("phpcredits",                   MAY_BE_TRUE),
	F1("php_sapi_name",                MAY_BE_FALSE | MAY_BE_STRING),
	F1("php_uname",                    MAY_BE_STRING),
	F1("php_ini_scanned_files",        MAY_BE_FALSE | MAY_BE_STRING),
	F1("php_ini_loaded_file",          MAY_BE_FALSE | MAY_BE_STRING),
	F1("strtok",                       MAY_BE_FALSE | MAY_BE_STRING),
	F1("strrev",                       MAY_BE_STRING),
	F1("hebrev",                       MAY_BE_STRING),
	F1("basename",                     MAY_BE_STRING),
	F1("dirname",                      MAY_BE_STRING),
	F1("pathinfo",                     MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("stripslashes",                 MAY_BE_STRING),
	F1("stripcslashes",                MAY_BE_STRING),
	F1("strstr",                       MAY_BE_FALSE | MAY_BE_STRING),
	F1("stristr",                      MAY_BE_FALSE | MAY_BE_STRING),
	F1("strrchr",                      MAY_BE_FALSE | MAY_BE_STRING),
	F1("str_shuffle",                  MAY_BE_STRING),
	F1("str_word_count",               MAY_BE_LONG | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("str_split",                    MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("strpbrk",                      MAY_BE_FALSE | MAY_BE_STRING),
	FN("substr_replace",               MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING),
	F1("quotemeta",                    MAY_BE_STRING),
	F1("ucwords",                      MAY_BE_STRING),
	FN("str_replace",                  MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING),
	FN("str_ireplace",                 MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING),
	F1("str_repeat",                   MAY_BE_STRING),
	F1("count_chars",                  MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("chunk_split",                  MAY_BE_STRING),
	F1("strip_tags",                   MAY_BE_STRING),
	F1("explode",                      MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("localeconv",                   MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
#if HAVE_NL_LANGINFO
	F1("nl_langinfo",                  MAY_BE_FALSE | MAY_BE_STRING),
#endif
	F1("soundex",                      MAY_BE_STRING),
	F1("chr",                          MAY_BE_STRING),
	F1("str_getcsv",                   MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING),
	F1("strchr",                       MAY_BE_FALSE | MAY_BE_STRING),
	F1("sprintf",                      MAY_BE_STRING),
	F1("vsprintf",                     MAY_BE_STRING),
	F1("sscanf",                       MAY_BE_NULL | MAY_BE_LONG | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	F1("fscanf",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	F1("parse_url",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_LONG),
	F1("urlencode",                    MAY_BE_STRING),
	F1("urldecode",                    MAY_BE_STRING),
	F1("rawurlencode",                 MAY_BE_STRING),
	F1("rawurldecode",                 MAY_BE_STRING),
	F1("http_build_query",             MAY_BE_STRING),
#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
	F1("readlink",                     MAY_BE_FALSE | MAY_BE_STRING),
#endif
	F1("exec",                         MAY_BE_FALSE | MAY_BE_STRING),
	F1("system",                       MAY_BE_FALSE | MAY_BE_STRING),
	F1("escapeshellcmd",               MAY_BE_STRING),
	F1("escapeshellarg",               MAY_BE_STRING),
	F0("passthru",                     MAY_BE_NULL | MAY_BE_FALSE),
	F1("shell_exec",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	F1("proc_open",                    MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("proc_get_status",              MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
#endif
	F1("random_bytes",                 MAY_BE_STRING),
#if HAVE_GETSERVBYPORT
	F1("getservbyport",                MAY_BE_FALSE | MAY_BE_STRING),
#endif
#if HAVE_GETPROTOBYNUMBER
	F1("getprotobynumber",             MAY_BE_FALSE | MAY_BE_STRING),
#endif
	F1("base64_decode",                MAY_BE_FALSE | MAY_BE_STRING),
	F1("base64_encode",                MAY_BE_STRING),
	F1("password_hash",                MAY_BE_STRING),
	F1("password_get_info",            MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("convert_uuencode",             MAY_BE_STRING),
	F1("convert_uudecode",             MAY_BE_FALSE | MAY_BE_STRING),
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
	FN("call_user_func",               UNKNOWN_INFO),
	FN("call_user_func_array",         UNKNOWN_INFO),
	FN("call_user_method",             UNKNOWN_INFO),
	FN("call_user_method_array",       UNKNOWN_INFO),
	FN("forward_static_call",          UNKNOWN_INFO),
	FN("forward_static_call_array",    UNKNOWN_INFO),
	F1("serialize",                    MAY_BE_STRING),
	FN("unserialize",                  UNKNOWN_INFO),
	F1("var_export",                   MAY_BE_NULL | MAY_BE_STRING),
	F1("print_r",                      MAY_BE_TRUE | MAY_BE_STRING),
	F0("register_shutdown_function",   MAY_BE_NULL | MAY_BE_FALSE),
	F1("highlight_file",               MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("show_source",                  MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("highlight_string",             MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("php_strip_whitespace",         MAY_BE_STRING),
	F1("ini_get_all",                  MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
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
#if defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC
# if defined(PHP_WIN32) || HAVE_FULL_DNS_FUNCS
	F1("dns_get_record",               MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ARRAY),
# endif
#endif
	F1("popen",                        MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("fgetc",                        MAY_BE_FALSE | MAY_BE_STRING),
	F1("fgets",                        MAY_BE_FALSE | MAY_BE_STRING),
	F1("fread",                        MAY_BE_FALSE | MAY_BE_STRING),
	F1("fopen",                        MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("fstat",                        MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG),
	F1("tempnam",                      MAY_BE_FALSE | MAY_BE_STRING),
	F1("tmpfile",                      MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("file",                         MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("file_get_contents",            MAY_BE_FALSE | MAY_BE_STRING),
	F1("stream_context_create",        MAY_BE_RESOURCE),
	F1("stream_context_get_params",    MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	FN("stream_context_get_options",   MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	FN("stream_context_get_default",   MAY_BE_RESOURCE),
	FN("stream_context_set_default",   MAY_BE_RESOURCE),
	FN("stream_filter_prepend",        MAY_BE_FALSE | MAY_BE_RESOURCE),
	FN("stream_filter_append",         MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("stream_socket_client",         MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("stream_socket_server",         MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("stream_socket_accept",         MAY_BE_FALSE | MAY_BE_RESOURCE),
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
	F1("fsockopen",                    MAY_BE_FALSE | MAY_BE_RESOURCE),
	FN("pfsockopen",                   MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pack",                         MAY_BE_STRING),
	F1("unpack",                       MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("get_browser",                  MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_OBJECT | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("crypt",                        MAY_BE_STRING),
	FN("opendir",                      MAY_BE_FALSE | MAY_BE_RESOURCE),
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
	FN("end",                          UNKNOWN_INFO),
	FN("prev",                         UNKNOWN_INFO),
	FN("next",                         UNKNOWN_INFO),
	FN("reset",                        UNKNOWN_INFO),
	FN("current",                      UNKNOWN_INFO),
	FN("min",                          UNKNOWN_INFO),
	FN("max",                          UNKNOWN_INFO),
	F1("compact",                      MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	FN("array_fill",                   MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	F1("array_fill_keys",              MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	FC("range",                        zend_range_info),
	FN("array_pop",                    UNKNOWN_INFO),
	FN("array_shift",                  UNKNOWN_INFO),
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

	/* ext/date */
	F1("date",                                  MAY_BE_STRING),
	F1("gmdate",                                MAY_BE_STRING),
	F1("strftime",                              MAY_BE_FALSE | MAY_BE_STRING),
	F1("gmstrftime",                            MAY_BE_FALSE | MAY_BE_STRING),
	F1("localtime",                             MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG),
	F1("getdate",                               MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("date_create",                           MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_create_immutable",                 MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_create_from_format",               MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_create_immutable_from_format",     MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_parse",                            MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("date_parse_from_format",                MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("date_get_last_errors",                  MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_ARRAY),
	F1("date_format",                           MAY_BE_STRING),
	F1("date_timezone_get",                     MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_diff",                             MAY_BE_OBJECT),
	F1("timezone_open",                         MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("timezone_name_get",                     MAY_BE_STRING),
	F1("timezone_name_from_abbr",               MAY_BE_FALSE | MAY_BE_STRING),
	F1("timezone_transitions_get",              MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("timezone_location_get",                 MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING),
	F1("timezone_identifiers_list",             MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("timezone_abbreviations_list",           MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("timezone_version_get",                  MAY_BE_STRING),
	F1("date_interval_create_from_date_string", MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_interval_format",                  MAY_BE_STRING),
	F1("date_default_timezone_get",             MAY_BE_STRING),
	F1("date_sunrise",                          MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_DOUBLE | MAY_BE_STRING),
	F1("date_sunset",                           MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_DOUBLE | MAY_BE_STRING),
	F1("date_sun_info",                         MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG),

	/* ext/preg */
	FN("preg_replace",			                MAY_BE_NULL | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING),
	FN("preg_replace_callback",	                MAY_BE_NULL | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING),
	F1("preg_filter",				            MAY_BE_NULL | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING),
	F1("preg_split",				            MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("preg_grep",				                MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),

	/* ext/mysqli */
	F1("mysqli_connect",						MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("mysqli_close",							MAY_BE_TRUE),
	F1("mysqli_connect_error",					MAY_BE_NULL | MAY_BE_STRING),
	F1("mysqli_get_client_stats",				MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("mysqli_error_list",						MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ARRAY),
	F1("mysqli_get_links_stats",				MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG),
	F1("mysqli_query",							MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_OBJECT),
	F1("mysqli_get_charset", 					MAY_BE_NULL | MAY_BE_OBJECT),
	F1("mysqli_fetch_array",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("mysqli_fetch_assoc",					MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("mysqli_fetch_all",						MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("mysqli_fetch_object",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysqli_affected_rows",					MAY_BE_LONG | MAY_BE_STRING),
	F1("mysqli_character_set_name",				MAY_BE_STRING),
	F0("mysqli_debug",							MAY_BE_TRUE),
	F1("mysqli_error",							MAY_BE_STRING),
	F1("mysqli_reap_async_query",				MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_OBJECT),
	F1("mysqli_stmt_get_result",				MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysqli_get_warnings",					MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysqli_stmt_error_list",				MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ARRAY),
	F1("mysqli_stmt_get_warnings",				MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysqli_fetch_field",					MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysqli_fetch_fields",					MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_OBJECT),
	F1("mysqli_fetch_field_direct",				MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysqli_fetch_lengths",					MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("mysqli_fetch_row",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	F1("mysqli_get_client_info",				MAY_BE_STRING),
	F1("mysqli_get_host_info",					MAY_BE_STRING),
	F1("mysqli_get_server_info",				MAY_BE_STRING),
	F1("mysqli_info",							MAY_BE_NULL | MAY_BE_STRING),
	F1("mysqli_init",							MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysqli_insert_id",						MAY_BE_LONG | MAY_BE_STRING),
	F1("mysqli_num_rows",						MAY_BE_LONG | MAY_BE_STRING),
	F1("mysqli_prepare",						MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysqli_real_escape_string",				MAY_BE_STRING),
	F1("mysqli_stmt_affected_rows",				MAY_BE_LONG | MAY_BE_STRING),
	F1("mysqli_stmt_insert_id",					MAY_BE_LONG | MAY_BE_STRING),
	F1("mysqli_stmt_num_rows",					MAY_BE_LONG | MAY_BE_STRING),
	F1("mysqli_sqlstate",						MAY_BE_STRING),
	F0("mysqli_ssl_set",						MAY_BE_TRUE),
	F1("mysqli_stat",							MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysqli_stmt_error",						MAY_BE_STRING),
	F1("mysqli_stmt_init",						MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysqli_stmt_result_metadata",			MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysqli_stmt_sqlstate",					MAY_BE_STRING),
	F1("mysqli_store_result",					MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysqli_use_result",						MAY_BE_FALSE | MAY_BE_OBJECT),

	/* ext/curl */
	F1("curl_init",                             MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("curl_copy_handle",                      MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("curl_version",                          MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("curl_getinfo",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_LONG | MAY_BE_DOUBLE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("curl_error",                            MAY_BE_STRING),
	F1("curl_strerror",                         MAY_BE_NULL | MAY_BE_STRING),
	F1("curl_multi_strerror",                   MAY_BE_NULL | MAY_BE_STRING),
	F1("curl_escape",                           MAY_BE_FALSE | MAY_BE_STRING),
	F1("curl_unescape",                         MAY_BE_FALSE | MAY_BE_STRING),
	F1("curl_multi_init",                       MAY_BE_OBJECT),
	F1("curl_multi_info_read",                  MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_OBJECT),
	F1("curl_share_init",                       MAY_BE_OBJECT),
	F1("curl_file_create",                      MAY_BE_OBJECT),

	/* ext/mbstring */
	F1("mb_convert_case",                       MAY_BE_STRING),
	F1("mb_strtoupper",                         MAY_BE_STRING),
	F1("mb_strtolower",                         MAY_BE_STRING),
	F1("mb_language",                           MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("mb_internal_encoding",                  MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("mb_http_input",                         MAY_BE_FALSE | MAY_BE_STRING| MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("mb_http_output",                        MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("mb_detect_order",                       MAY_BE_TRUE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("mb_substitute_character",               MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_LONG | MAY_BE_STRING),
	F1("mb_output_handler",                     MAY_BE_STRING),
	F1("mb_preferred_mime_name",                MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_strstr",                             MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_strrchr",                            MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_stristr",                            MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_strrichr",                           MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_substr",                             MAY_BE_STRING),
	F1("mb_strcut",                             MAY_BE_STRING),
	F1("mb_strimwidth",                         MAY_BE_STRING),
	F1("mb_convert_encoding",                   MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("mb_detect_encoding",                    MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_list_encodings",                     MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("mb_encoding_aliases",                   MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("mb_convert_kana",                       MAY_BE_STRING),
	F1("mb_encode_mimeheader",                  MAY_BE_STRING),
	F1("mb_decode_mimeheader",                  MAY_BE_STRING),
	F1("mb_convert_variables",                  MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_encode_numericentity",               MAY_BE_STRING),
	F1("mb_decode_numericentity",               MAY_BE_STRING),
	F1("mb_get_info",                           MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),

	F1("mb_regex_encoding",                     MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("mb_regex_set_options",                  MAY_BE_STRING),
	F1("mb_ereg_replace",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_eregi_replace",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_ereg_replace_callback",              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_split",                              MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("mb_ereg_search_pos",                    MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("mb_ereg_search_regs",                   MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_STRING),
	F1("mb_ereg_search_getregs",                MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_STRING),

	/* ext/iconv */
	F1("iconv",                                 MAY_BE_FALSE | MAY_BE_STRING),
	F1("iconv_get_encoding",                    MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("iconv_substr",                          MAY_BE_FALSE | MAY_BE_STRING),
	F1("iconv_mime_encode",                     MAY_BE_FALSE | MAY_BE_STRING),
	F1("iconv_mime_decode",                     MAY_BE_FALSE | MAY_BE_STRING),
	F1("iconv_mime_decode_headers",             MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),

	/* ext/json */
	F1("json_encode",                           MAY_BE_FALSE | MAY_BE_STRING),
	FN("json_decode",                           MAY_BE_ANY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("json_last_error_msg",                   MAY_BE_STRING),

	/* ext/xml */
	F1("xml_error_string",                      MAY_BE_NULL | MAY_BE_STRING),
	F1("xml_parser_get_option",                 MAY_BE_LONG | MAY_BE_STRING),
	F1("utf8_encode",                           MAY_BE_STRING),
	F1("utf8_decode",                           MAY_BE_STRING),

	/* ext/zlib */
	F1("gzgetc",                                MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzgets",                                MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzread",                                MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzopen",                                MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("gzfile",                                MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("gzcompress",                            MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzuncompress",                          MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzdeflate",                             MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzinflate",                             MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzencode",                              MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzdecode",                              MAY_BE_FALSE | MAY_BE_STRING),
	F1("zlib_encode",                           MAY_BE_FALSE | MAY_BE_STRING),
	F1("zlib_decode",                           MAY_BE_FALSE | MAY_BE_STRING),
	F1("zlib_get_coding_type",                  MAY_BE_FALSE | MAY_BE_STRING),
	F1("ob_gzhandler",                          MAY_BE_FALSE | MAY_BE_STRING),

	/* ext/hash */
	F1("hash",                                  MAY_BE_STRING),
	F1("hash_file",                             MAY_BE_FALSE | MAY_BE_STRING),
	F1("hash_hmac",                             MAY_BE_STRING),
	F1("hash_hmac_algos",                       MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("hash_hmac_file",                        MAY_BE_FALSE | MAY_BE_STRING),
	F1("hash_hkdf",                             MAY_BE_STRING),
	F1("hash_init",                             MAY_BE_OBJECT),
	F1("hash_final",                            MAY_BE_STRING),
	F1("hash_copy",                             MAY_BE_OBJECT),
	F1("hash_algos",                            MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("hash_pbkdf2",                           MAY_BE_STRING),
	F1("mhash_keygen_s2k",                      MAY_BE_FALSE | MAY_BE_STRING),
	F1("mhash_get_hash_name",                   MAY_BE_FALSE | MAY_BE_STRING),
	F1("mhash",                                 MAY_BE_FALSE | MAY_BE_FALSE | MAY_BE_STRING),

	/* ext/sodium */
	F1("sodium_crypto_shorthash",				MAY_BE_STRING),
	F1("sodium_crypto_secretbox",				MAY_BE_STRING),
	F1("sodium_crypto_secretbox_open",			MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_crypto_generichash",				MAY_BE_STRING),
	F1("sodium_crypto_generichash_init",		MAY_BE_STRING),
	F0("sodium_crypto_generichash_update",		MAY_BE_TRUE),
	F1("sodium_crypto_generichash_final",		MAY_BE_STRING),
	F1("sodium_crypto_box_keypair",				MAY_BE_STRING),
	F1("sodium_crypto_box_seed_keypair",		MAY_BE_STRING),
	F1("sodium_crypto_box_secretkey",			MAY_BE_STRING),
	F1("sodium_crypto_box_publickey",			MAY_BE_STRING),
	F1("sodium_crypto_box",						MAY_BE_STRING),
	F1("sodium_crypto_box_open",				MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_crypto_box_seal",				MAY_BE_STRING),
	F1("sodium_crypto_box_seal_open",			MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_crypto_sign_keypair",			MAY_BE_STRING),
	F1("sodium_crypto_sign_seed_keypair",		MAY_BE_STRING),
	F1("sodium_crypto_sign_secretkey",			MAY_BE_STRING),
	F1("sodium_crypto_sign_publickey",			MAY_BE_STRING),
	F1("sodium_crypto_sign",					MAY_BE_STRING),
	F1("sodium_crypto_sign_open",				MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_crypto_sign_detached",			MAY_BE_STRING),
	F1("sodium_crypto_stream",					MAY_BE_STRING),
	F1("sodium_crypto_stream_xor",				MAY_BE_STRING),
	F1("sodium_crypto_pwhash",					MAY_BE_STRING),
	F1("sodium_crypto_pwhash_str",				MAY_BE_STRING),
	F1("sodium_crypto_aead_aes256gcm_encrypt",	MAY_BE_STRING),
	F1("sodium_crypto_aead_aes256gcm_decrypt",	MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_bin2hex",						MAY_BE_STRING),
	F1("sodium_hex2bin",						MAY_BE_STRING),
	F1("sodium_crypto_scalarmult",				MAY_BE_STRING),
	F1("sodium_crypto_kx_seed_keypair",			MAY_BE_STRING),
	F1("sodium_crypto_kx_keypair",				MAY_BE_STRING),
	F1("sodium_crypto_kx_secretkey",			MAY_BE_STRING),
	F1("sodium_crypto_kx_publickey",			MAY_BE_STRING),
	F1("sodium_crypto_kx_client_session_keys",	MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("sodium_crypto_kx_server_session_keys",	MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("sodium_crypto_auth",					MAY_BE_STRING),
	F1("sodium_crypto_aead_aes256gcm_keygen",	MAY_BE_STRING),
	F1("sodium_crypto_auth_keygen",				MAY_BE_STRING),
	F1("sodium_crypto_generichash_keygen",		MAY_BE_STRING),
	F1("sodium_crypto_kdf_keygen",				MAY_BE_STRING),
	F1("sodium_crypto_secretbox_keygen",		MAY_BE_STRING),
	F1("sodium_crypto_shorthash_keygen",		MAY_BE_STRING),
	F1("sodium_crypto_stream_keygen",			MAY_BE_STRING),
	F1("sodium_crypto_kdf_derive_from_key",		MAY_BE_STRING),
	F1("sodium_pad",							MAY_BE_STRING),
	F1("sodium_unpad",							MAY_BE_STRING),

	F1("sodium_crypto_box_keypair_from_secretkey_and_publickey",	MAY_BE_STRING),
	F1("sodium_crypto_box_publickey_from_secretkey",				MAY_BE_STRING),
	F1("sodium_crypto_sign_keypair_from_secretkey_and_publickey",	MAY_BE_STRING),
	F1("sodium_crypto_sign_publickey_from_secretkey",				MAY_BE_STRING),
	F1("sodium_crypto_pwhash_scryptsalsa208sha256",					MAY_BE_STRING),
	F1("sodium_crypto_pwhash_scryptsalsa208sha256_str",				MAY_BE_STRING),
	F1("sodium_crypto_sign_ed25519_sk_to_curve25519",				MAY_BE_STRING),
	F1("sodium_crypto_sign_ed25519_pk_to_curve25519",				MAY_BE_STRING),
	F1("sodium_crypto_aead_chacha20poly1305_encrypt",				MAY_BE_STRING),
	F1("sodium_crypto_aead_chacha20poly1305_decrypt",				MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_crypto_aead_chacha20poly1305_ietf_encrypt",			MAY_BE_STRING),
	F1("sodium_crypto_aead_chacha20poly1305_ietf_decrypt",			MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_crypto_aead_xchacha20poly1305_ietf_encrypt",			MAY_BE_STRING),
	F1("sodium_crypto_aead_xchacha20poly1305_ietf_decrypt",			MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_crypto_aead_chacha20poly1305_keygen",				MAY_BE_STRING),
	F1("sodium_crypto_aead_chacha20poly1305_ietf_keygen",			MAY_BE_STRING),
	F1("sodium_crypto_aead_xchacha20poly1305_ietf_keygen",			MAY_BE_STRING),

	/* ext/session */
	F1("session_get_cookie_params",				MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("session_name",							MAY_BE_FALSE | MAY_BE_STRING),
	F1("session_module_name",					MAY_BE_FALSE | MAY_BE_STRING),
	F1("session_save_path",						MAY_BE_FALSE | MAY_BE_STRING),
	F1("session_create_id",						MAY_BE_FALSE | MAY_BE_STRING),
	F1("session_cache_limiter",					MAY_BE_FALSE | MAY_BE_STRING),
	F1("session_encode",						MAY_BE_FALSE | MAY_BE_STRING),

	/* ext/pgsql */
	FN("pg_connect",							MAY_BE_FALSE | MAY_BE_RESOURCE),
	FN("pg_pconnect",							MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pg_dbname",								MAY_BE_STRING),
	F1("pg_last_error",							MAY_BE_STRING),
	F1("pg_options",							MAY_BE_STRING),
	F1("pg_port",								MAY_BE_STRING),
	F1("pg_tty",								MAY_BE_STRING),
	F1("pg_host",								MAY_BE_STRING),
	F1("pg_version",							MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_NULL),
	F1("pg_parameter_status",					MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_query",								MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pg_query_params",						MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pg_prepare",							MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pg_execute",							MAY_BE_FALSE | MAY_BE_RESOURCE),
	FN("pg_last_notice",						MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY ),
	F1("pg_field_table",						MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING),
	F1("pg_field_name",							MAY_BE_STRING),
	F1("pg_field_type",							MAY_BE_STRING),
	F1("pg_field_type_oid",						MAY_BE_LONG | MAY_BE_STRING),
	F1("pg_fetch_result",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_fetch_row",							MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING),
	F1("pg_fetch_assoc",						MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING),
	F1("pg_fetch_array",						MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING),
	F1("pg_fetch_object",						MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("pg_fetch_all",							MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ARRAY),
	F1("pg_fetch_all_columns",					MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING),
	F1("pg_last_oid",							MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING),
	F1("pg_lo_create",							MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING),
	F1("pg_lo_open",							MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pg_lo_read",							MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_lo_import",							MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING),
	F1("pg_copy_to",							MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("pg_escape_string",						MAY_BE_STRING),
	F1("pg_escape_bytea",						MAY_BE_STRING),
	F1("pg_unescape_bytea",						MAY_BE_STRING),
	F1("pg_escape_literal",						MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_escape_identifier",					MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_result_error",						MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_result_error_field",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_get_result",							MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pg_result_status",						MAY_BE_LONG | MAY_BE_STRING),
	F1("pg_get_notify",							MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("pg_socket",								MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pg_meta_data",							MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("pg_convert",							MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("pg_insert",								MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_RESOURCE | MAY_BE_STRING),
	F1("pg_update",								MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("pg_delete",								MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("pg_select",								MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ARRAY),

	/* ext/bcmath */
	F1("bcadd",									MAY_BE_STRING),
	F1("bcsub",									MAY_BE_STRING),
	F1("bcmul",									MAY_BE_STRING),
	F1("bcdiv",									MAY_BE_STRING),
	F1("bcmod",									MAY_BE_STRING),
	F1("bcpowmod",								MAY_BE_STRING),
	F1("bcpow",									MAY_BE_STRING),
	F1("bcsqrt",								MAY_BE_STRING),

	/* ext/exif */
	F1("exif_tagname",							MAY_BE_FALSE | MAY_BE_STRING),
	F1("exif_read_data",						MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("exif_thumbnail",						MAY_BE_FALSE | MAY_BE_STRING),

	/* ext/filter */
	FN("filter_input",							UNKNOWN_INFO),
	FN("filter_var",							UNKNOWN_INFO),
	F1("filter_input_array",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("filter_var_array",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF),
	F1("filter_list",							MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),

	/* ext/gettext */
	F1("textdomain",							MAY_BE_STRING),
	F1("gettext",								MAY_BE_STRING),
	F1("_",										MAY_BE_STRING),
	F1("dgettext",								MAY_BE_STRING),
	F1("dcgettext",								MAY_BE_STRING),
	F1("bindtextdomain",						MAY_BE_FALSE | MAY_BE_STRING),
#if HAVE_NGETTEXT
	F1("ngettext",								MAY_BE_STRING),
#endif
#if HAVE_DNGETTEXT
	F1("dcngettext",							MAY_BE_STRING),
#endif
#if HAVE_BIND_TEXTDOMAIN_CODESET
	F1("bind_textdomain_codeset",				MAY_BE_FALSE | MAY_BE_STRING),
#endif

	/* ext/fileinfo */
	F1("finfo_open",							MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("finfo_file",							MAY_BE_FALSE | MAY_BE_STRING),
	F1("finfo_buffer",							MAY_BE_FALSE | MAY_BE_STRING),
	F1("mime_content_type",						MAY_BE_FALSE | MAY_BE_STRING),

	/* ext/gd */
	F1("gd_info",								MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE),
	F1("imagecreatetruecolor",					MAY_BE_FALSE | MAY_BE_OBJECT),
#ifdef PHP_WIN32
	F1("imagegrabwindow",						MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("imagegrabscreen",						MAY_BE_FALSE | MAY_BE_OBJECT),
#endif
	F1("imagerotate",							MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("imagecreate",							MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("imagecreatefromstring",					MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("imagecreatefromgif",					MAY_BE_FALSE | MAY_BE_OBJECT),
#ifdef HAVE_GD_JPG
	F1("imagecreatefromjpeg",					MAY_BE_FALSE | MAY_BE_OBJECT),
#endif
#ifdef HAVE_GD_PNG
	F1("imagecreatefrompng",					MAY_BE_FALSE | MAY_BE_OBJECT),
#endif
#ifdef HAVE_GD_WEBP
	F1("imagecreatefromwebp",					MAY_BE_FALSE | MAY_BE_OBJECT),
#endif
	F1("imagecreatefromxbm",					MAY_BE_FALSE | MAY_BE_OBJECT),
#if defined(HAVE_GD_XPM)
	F1("imagecreatefromxpm",					MAY_BE_FALSE | MAY_BE_OBJECT),
#endif
	F1("imagecreatefromwbmp",					MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("imagecreatefromgd",						MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("imagecreatefromgd2",					MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("imagecreatefromgd2part",				MAY_BE_FALSE | MAY_BE_OBJECT),
#if defined(HAVE_GD_BMP)
	F1("imagecreatefrombmp",					MAY_BE_FALSE | MAY_BE_OBJECT),
#endif
	F0("imagecolorset",							MAY_BE_NULL | MAY_BE_FALSE),
	F1("imagecolorsforindex",					MAY_BE_ARRAY |  MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG),
	F1("imagegetclip",							MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("imageftbbox",							MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("imagefttext",							MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("imagettfbbox",							MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("imagettftext",							MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("imagecrop",								MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("imagecropauto",							MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("imagescale",							MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("imageaffine",							MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("imageaffinematrixget",					MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_DOUBLE),
	F1("imageaffinematrixconcat",				MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_DOUBLE),
	F1("imageresolution",						MAY_BE_TRUE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),

	/* ext/spl */
	F1("class_implements",						MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("class_parents",							MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("class_uses",							MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("iterator_to_array",						MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("spl_classes",							MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("spl_object_hash",						MAY_BE_STRING),

};

static HashTable func_info;
int zend_func_info_rid = -1;

static uint32_t get_internal_func_info(
		const zend_call_info *call_info, const zend_ssa *ssa, zend_string *lcname) {
	if (call_info->callee_func->common.scope) {
		/* This is a method, not a function. */
		return 0;
	}

	zval *zv = zend_hash_find_ex(&func_info, lcname, 1);
	if (!zv) {
		return 0;
	}

	func_info_t *info = Z_PTR_P(zv);
	if (info->info_func) {
		return info->info_func(call_info, ssa);
	} else {
		return info->info;
	}
}

uint32_t zend_get_func_info(
		const zend_call_info *call_info, const zend_ssa *ssa,
		zend_class_entry **ce, zend_bool *ce_is_instanceof)
{
	uint32_t ret = 0;
	const zend_function *callee_func = call_info->callee_func;
	*ce = NULL;
	*ce_is_instanceof = 0;

	if (callee_func->type == ZEND_INTERNAL_FUNCTION) {
		zend_string *lcname = Z_STR_P(CRT_CONSTANT_EX(call_info->caller_op_array, call_info->caller_init_opline, call_info->caller_init_opline->op2));

		uint32_t internal_ret = get_internal_func_info(call_info, ssa, lcname);
#if !ZEND_DEBUG
		if (internal_ret) {
			return internal_ret;
		}
#endif

		if (callee_func->common.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
			ret = zend_fetch_arg_info_type(NULL, callee_func->common.arg_info - 1, ce);
			*ce_is_instanceof = 1;
		} else {
#if 0
			fprintf(stderr, "Unknown internal function '%s'\n", func->common.function_name);
#endif
			ret = MAY_BE_ANY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF
				| MAY_BE_RC1 | MAY_BE_RCN;
		}
		if (callee_func->common.fn_flags & ZEND_ACC_RETURN_REFERENCE) {
			ret |= MAY_BE_REF;
		}

#if ZEND_DEBUG
		if (internal_ret) {
			/* Check whether the func_info information is a subset of the information we can
			 * compute from the specified return type, otherwise it contains redundant types. */
			if (internal_ret & ~ret) {
				fprintf(stderr, "Inaccurate func info for %s()\n", ZSTR_VAL(lcname));
			}
			/* Check whether the func info is completely redundant with arginfo.
			 * Ignore UNKNOWN_INFO for now. */
			if (internal_ret == ret && (internal_ret & MAY_BE_ANY) != MAY_BE_ANY) {
				fprintf(stderr, "Useless func info for %s()\n", ZSTR_VAL(lcname));
			}
			/* If the return type is not mixed, check that the types match exactly if we exclude
			 * RC and array information. */
			uint32_t ret_any = ret & MAY_BE_ANY, internal_ret_any = internal_ret & MAY_BE_ANY;
			if (ret_any != MAY_BE_ANY) {
				uint32_t diff = internal_ret_any ^ ret_any;
				/* Func info may contain "true" types as well as isolated "null" and "false". */
				if (diff && !(diff == MAY_BE_FALSE && (ret & MAY_BE_FALSE))
						&& (internal_ret_any & ~(MAY_BE_NULL|MAY_BE_FALSE))) {
					fprintf(stderr, "Incorrect func info for %s()\n", ZSTR_VAL(lcname));
				}
			}
			return internal_ret;
		}
#endif
	} else {
		// FIXME: the order of functions matters!!!
		zend_func_info *info = ZEND_FUNC_INFO((zend_op_array*)callee_func);
		if (info) {
			ret = info->return_info.type;
			*ce = info->return_info.ce;
			*ce_is_instanceof = info->return_info.is_instanceof;
		}
		if (!ret) {
			ret = MAY_BE_ANY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF
				| MAY_BE_RC1 | MAY_BE_RCN;
			/* For generators RETURN_REFERENCE refers to the yielded values. */
			if ((callee_func->common.fn_flags & ZEND_ACC_RETURN_REFERENCE)
					&& !(callee_func->common.fn_flags & ZEND_ACC_GENERATOR)) {
				ret |= MAY_BE_REF;
			}
		}
	}
	return ret;
}

int zend_func_info_startup(void)
{
	size_t i;

	if (zend_func_info_rid == -1) {
		zend_func_info_rid = zend_get_resource_handle("Zend Optimizer");
		if (zend_func_info_rid < 0) {
			return FAILURE;
		}

		zend_hash_init(&func_info, sizeof(func_infos)/sizeof(func_info_t), NULL, NULL, 1);
		for (i = 0; i < sizeof(func_infos)/sizeof(func_info_t); i++) {
			zend_string *key = zend_string_init_interned(func_infos[i].name, func_infos[i].name_len, 1);

			if (zend_hash_add_ptr(&func_info, key, (void**)&func_infos[i]) == NULL) {
				fprintf(stderr, "ERROR: Duplicate function info for \"%s\"\n", func_infos[i].name);
			}
			zend_string_release_ex(key, 1);
		}
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
