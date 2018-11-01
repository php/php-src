/*
   +----------------------------------------------------------------------+
   | Zend Engine, Func Info                                               |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 The PHP Group                                |
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

/* MSVC defines its own IN macro, undefine it here */
#undef IN

#define F0(name, info) \
	{name, sizeof(name)-1, (FUNC_MAY_WARN | (info)), NULL}
#define F1(name, info) \
	{name, sizeof(name)-1, (FUNC_MAY_WARN | MAY_BE_RC1 | (info)), NULL}
#define FN(name, info) \
	{name, sizeof(name)-1, (FUNC_MAY_WARN | MAY_BE_RC1 | MAY_BE_RCN | (info)), NULL}
#define FR(name, info) \
	{name, sizeof(name)-1, (FUNC_MAY_WARN | MAY_BE_REF | (info)), NULL}
#define FX(name, info) \
	{name, sizeof(name)-1, (FUNC_MAY_WARN | MAY_BE_RC1 | MAY_BE_RCN | MAY_BE_REF | (info)), NULL}
#define I0(name, info) \
	{name, sizeof(name)-1, (info), NULL}
#define I1(name, info) \
	{name, sizeof(name)-1, (MAY_BE_RC1 | (info)), NULL}
#define IN(name, info) \
	{name, sizeof(name)-1, (MAY_BE_RC1 | MAY_BE_RCN | (info)), NULL}
#define FC(name, callback) \
	{name, sizeof(name)-1, 0, callback}

static uint32_t zend_strlen_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	if (call_info->num_args == 1) {
		uint32_t tmp = 0;
		if (call_info->arg_info[0].opline) {
			uint32_t arg_info = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[0].opline);

			if (arg_info & (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_OBJECT)) {
				tmp |= MAY_BE_LONG;
			}
			if (arg_info & (MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
				/* warning, and returns NULL */
				tmp |= FUNC_MAY_WARN | MAY_BE_NULL;
			}
		} else {
			tmp |= MAY_BE_LONG | FUNC_MAY_WARN | MAY_BE_NULL;
		}
		return tmp;
	} else if (call_info->num_args != -1) {
		/* warning, and returns NULL */
		return FUNC_MAY_WARN | MAY_BE_NULL;
	} else {
		return MAY_BE_LONG | FUNC_MAY_WARN | MAY_BE_NULL;
	}
}

static uint32_t zend_dechex_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	if (call_info->num_args == 1) {
		return MAY_BE_RC1 | MAY_BE_STRING;
	} else if (call_info->num_args != -1) {
		/* warning, and returns NULL */
		return FUNC_MAY_WARN | MAY_BE_NULL;
	} else {
		return FUNC_MAY_WARN | MAY_BE_RC1 | MAY_BE_STRING | MAY_BE_NULL;
	}
}

static uint32_t zend_range_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	if (call_info->num_args == 2 || call_info->num_args == 3) {

		uint32_t t1 = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[0].opline);
		uint32_t t2 = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[1].opline);
		uint32_t t3 = 0;
		uint32_t tmp = FUNC_MAY_WARN | MAY_BE_RC1 | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG;

		if (call_info->num_args == 3) {
			t3 = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[2].opline);
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
		/* may warning, and return FALSE */
		return FUNC_MAY_WARN | MAY_BE_RC1 | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING;
	}
}

static uint32_t zend_is_type_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	if (call_info->num_args == 1) {
		return MAY_BE_FALSE | MAY_BE_TRUE;
	} else {
		return MAY_BE_FALSE | MAY_BE_TRUE | FUNC_MAY_WARN;
	}
}

static uint32_t zend_l_ss_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	if (call_info->num_args == 2) {

		uint32_t arg1_info = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[0].opline);
		uint32_t arg2_info = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[1].opline);
		uint32_t tmp = 0;

		if ((arg1_info & (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_OBJECT)) &&
		    (arg2_info & (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_OBJECT))) {
			tmp |= MAY_BE_LONG;
		}
		if ((arg1_info & (MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) ||
		    (arg2_info & (MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE))) {
			/* warning, and returns NULL */
			tmp |= FUNC_MAY_WARN | MAY_BE_NULL;
		}
		return tmp;
	} else {
		/* warning, and returns NULL */
		return FUNC_MAY_WARN | MAY_BE_NULL | MAY_BE_LONG;
	}
}

static uint32_t zend_lb_ssn_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	if (call_info->num_args == 3) {
		uint32_t arg1_info = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[0].opline);
		uint32_t arg2_info = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[1].opline);
		uint32_t arg3_info = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[2].opline);
		uint32_t tmp = 0;

		if ((arg1_info & (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_OBJECT)) &&
		    (arg2_info & (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_OBJECT)) &&
		    (arg3_info & (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_OBJECT))) {
			tmp |= MAY_BE_LONG | MAY_BE_FALSE;
		}
		if ((arg1_info & (MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) ||
		    (arg2_info & (MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) ||
		    (arg3_info & (MAY_BE_STRING|MAY_BE_RESOURCE|MAY_BE_ARRAY|MAY_BE_OBJECT))) {
			/* warning, and returns NULL */
			tmp |= FUNC_MAY_WARN | MAY_BE_NULL;
		}
		return tmp;
	} else {
		/* warning, and returns NULL */
		return FUNC_MAY_WARN | MAY_BE_NULL | MAY_BE_LONG;
	}
}

static uint32_t zend_b_s_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	if (call_info->num_args == 1) {

		uint32_t arg1_info = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[0].opline);
		uint32_t tmp = 0;

		if (arg1_info & (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_OBJECT)) {
			tmp |= MAY_BE_FALSE | MAY_BE_TRUE;
		}
		if (arg1_info & (MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
			/* warning, and returns NULL */
			tmp |= FUNC_MAY_WARN | MAY_BE_NULL;
		}
		return tmp;
	} else {
		/* warning, and returns NULL */
		return FUNC_MAY_WARN | MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE;
	}
}

#define UNKNOWN_INFO (MAY_BE_ANY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF)

static const func_info_t func_infos[] = {
	/* zend */
	I1("zend_version",            MAY_BE_STRING),
	I0("gc_collect_cycles",       MAY_BE_LONG),
	I0("gc_enabled",              MAY_BE_FALSE | MAY_BE_TRUE),
	F0("gc_enable",               MAY_BE_NULL),
	F0("gc_disable",              MAY_BE_NULL),
	F0("func_num_args",           MAY_BE_LONG),
	FN("func_get_arg",            UNKNOWN_INFO),
	F1("func_get_args",           MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	FC("strlen",                  zend_strlen_info),
	FC("strcmp",                  zend_l_ss_info),
	FC("strncmp",                 zend_lb_ssn_info),
	FC("strcasecmp",              zend_l_ss_info),
	FC("strncasecmp",             zend_lb_ssn_info),
	F1("each",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_KEY_ANY),
	F0("error_reporting",         MAY_BE_NULL | MAY_BE_LONG),
	F0("define",                  MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_NULL), // TODO: inline
	FC("defined",                 zend_b_s_info), // TODO: inline
	FN("get_class",               MAY_BE_FALSE | MAY_BE_STRING),
	FN("get_called_class",        MAY_BE_FALSE | MAY_BE_STRING),
	FN("get_parrent_class",       MAY_BE_FALSE | MAY_BE_STRING),
	F0("is_subclass_of",          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE), // TODO: inline
	F0("is_a",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE), // TODO: inline
	F1("get_class_vars",          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF),
	FN("get_object_vars",         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF),
	F1("get_class_methods",       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F0("method_exists",           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("property_exists",         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("class_exists",            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("interface_exists",        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("trait_exists",            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	FC("function_exists",         zend_b_s_info), // TODO: inline
	F0("class_alias",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	I1("get_included_files",      MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F0("trigger_error",           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("user_error",              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	FN("set_error_handler",       MAY_BE_NULL | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_OBJECT | MAY_BE_OBJECT),
	I0("restore_error_handler",   MAY_BE_TRUE),
	I0("restore_exception_handler", MAY_BE_TRUE),
	I1("get_declared_traits",     MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	I1("get_declared_classes",    MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	I1("get_declared_interfaces", MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("get_defined_functions",   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ARRAY),
	I1("get_defined_vars",        MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF),
	FN("create_function",         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("get_resource_type",       MAY_BE_NULL | MAY_BE_STRING),
	F1("get_defined_constants",   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_RESOURCE | MAY_BE_ARRAY_OF_ARRAY),
	F0("debug_print_backtrace",   MAY_BE_NULL),
	F1("debug_backtrace",         MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ARRAY),
	F1("get_loaded_extensions",   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	FC("extension_loaded",        zend_b_s_info),
	F1("get_extension_funcs",     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),

	/* ext/standard */
	FN("constant",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_LONG | MAY_BE_DOUBLE | MAY_BE_STRING | MAY_BE_RESOURCE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("bin2hex",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("hex2bin",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("sleep",                        MAY_BE_FALSE | MAY_BE_LONG),
	F0("usleep",                       MAY_BE_NULL | MAY_BE_FALSE),
#if HAVE_NANOSLEEP
	F0("time_nanosleep",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("time_sleep_until",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
#if HAVE_STRPTIME
	F1("strptime",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
#endif
	F0("flush",                        MAY_BE_NULL),
	F1("wordwrap",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("htmlspecialchars",             MAY_BE_NULL | MAY_BE_STRING),
	F1("htmlentities",                 MAY_BE_NULL | MAY_BE_STRING),
	FN("html_entity_decode",           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	FN("htmlspecialchars_decode",      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("get_html_translation_table",   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("sha1",                         MAY_BE_NULL | MAY_BE_STRING),
	F1("sha1_file",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("md5",                          MAY_BE_NULL | MAY_BE_STRING),
	F1("md5_file",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("crc32",                        MAY_BE_NULL | MAY_BE_LONG),
	F1("iptcparse",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("iptcembed",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("getimagesize",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("getimagesizefromstring",       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("image_type_to_mime_type",      MAY_BE_NULL | MAY_BE_STRING),
	F1("image_type_to_extension",      MAY_BE_FALSE | MAY_BE_STRING),
	F0("phpinfo",                      MAY_BE_NULL | MAY_BE_TRUE),
	F1("phpversion",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("phpcredits",                   MAY_BE_NULL | MAY_BE_TRUE),
	I1("php_sapi_name",                MAY_BE_FALSE | MAY_BE_STRING),
	F1("php_uname",                    MAY_BE_NULL | MAY_BE_STRING),
	I1("php_ini_scanned_files",        MAY_BE_FALSE | MAY_BE_STRING),
	I1("php_ini_loaded_file",          MAY_BE_FALSE | MAY_BE_STRING),
	F0("strnatcmp",                    MAY_BE_NULL | MAY_BE_LONG),
	F0("strnatcasecmp",                MAY_BE_NULL | MAY_BE_LONG),
	F0("substr_count",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("strspn",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("strcspn",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("strtok",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	FN("strtoupper",                   MAY_BE_NULL | MAY_BE_STRING),
	FN("strtolower",                   MAY_BE_NULL | MAY_BE_STRING),
	F0("strpos",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("stripos",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("strrpos",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("strripos",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("strrev",                       MAY_BE_NULL | MAY_BE_STRING),
	F1("hebrev",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("hebrevc",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("nl2br",                        MAY_BE_NULL | MAY_BE_STRING),
	F1("basename",                     MAY_BE_NULL | MAY_BE_STRING),
	F1("dirname",                      MAY_BE_NULL | MAY_BE_STRING),
	F1("pathinfo",                     MAY_BE_NULL | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("stripslashes",                 MAY_BE_NULL | MAY_BE_STRING),
	F1("stripcslashes",                MAY_BE_NULL | MAY_BE_STRING),
	F1("strstr",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("stristr",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("strrchr",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("str_shuffle",                  MAY_BE_NULL | MAY_BE_STRING),
	F1("str_word_count",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("str_split",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("strpbrk",                      MAY_BE_FALSE | MAY_BE_STRING),
	F0("substr_compare",               MAY_BE_FALSE | MAY_BE_LONG),
#ifdef HAVE_STRCOLL
	F0("strcoll",                      MAY_BE_NULL | MAY_BE_LONG),
#endif
#ifdef HAVE_STRFMON
	F1("money_format",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#endif
	FN("substr",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	FN("substr_replace",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING),
	F1("quotemeta",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	FN("ucfirst",                      MAY_BE_NULL | MAY_BE_STRING),
	FN("lcfirst",                      MAY_BE_NULL | MAY_BE_STRING),
	F1("ucwords",                      MAY_BE_NULL | MAY_BE_STRING),
	FN("strtr",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	FN("addslashes",                   MAY_BE_NULL | MAY_BE_STRING),
	F1("addcslashes",                  MAY_BE_NULL | MAY_BE_STRING),
	FN("rtrim",                        MAY_BE_NULL | MAY_BE_STRING),
	FN("str_replace",                  MAY_BE_NULL | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY | MAY_BE_ARRAY_OF_OBJECT),
	FN("str_ireplace",                 MAY_BE_NULL | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY | MAY_BE_ARRAY_OF_OBJECT),
	F1("str_repeat",                   MAY_BE_NULL | MAY_BE_STRING),
	F1("count_chars",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("chunk_split",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	FN("trim",                         MAY_BE_NULL | MAY_BE_STRING),
	FN("ltrim",                        MAY_BE_NULL | MAY_BE_STRING),
	F1("strip_tags",                   MAY_BE_NULL | MAY_BE_STRING),
	F0("similar_text",                 MAY_BE_NULL | MAY_BE_LONG),
	F1("explode",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	FN("implode",                      MAY_BE_NULL | MAY_BE_STRING),
	FN("join",                         MAY_BE_NULL | MAY_BE_STRING),
	FN("setlocale",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("localeconv",                   MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
#if HAVE_NL_LANGINFO
	F1("nl_langinfo",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#endif
	F1("soundex",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("levenshtein",                  MAY_BE_NULL | MAY_BE_LONG),
	F1("chr",                          MAY_BE_NULL | MAY_BE_STRING),
	F0("ord",                          MAY_BE_NULL | MAY_BE_LONG),
	F0("parse_str",                    MAY_BE_NULL),
	F1("str_getcsv",                   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING),
	F1("str_pad",                      MAY_BE_NULL | MAY_BE_STRING),
	F1("chop",                         MAY_BE_NULL | MAY_BE_STRING),
	F1("strchr",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("sprintf",                      MAY_BE_FALSE | MAY_BE_STRING),
	F0("printf",                       MAY_BE_FALSE | MAY_BE_LONG),
	F0("vprintf",                      MAY_BE_FALSE | MAY_BE_LONG),
	F1("vsprintf",                     MAY_BE_FALSE | MAY_BE_STRING),
	F0("fprintf",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("vfprintf",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("sscanf",                       MAY_BE_NULL | MAY_BE_LONG | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	F1("fscanf",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	F1("parse_url",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_LONG),
	F1("urlencode",                    MAY_BE_NULL | MAY_BE_STRING),
	F1("urldecode",                    MAY_BE_NULL | MAY_BE_STRING),
	F1("rawurlencode",                 MAY_BE_NULL | MAY_BE_STRING),
	F1("rawurldecode",                 MAY_BE_NULL | MAY_BE_STRING),
	F1("http_build_query",             MAY_BE_FALSE | MAY_BE_STRING),
#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
	F1("readlink",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("linkinfo",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("symlink",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("link",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F0("unlink",                       MAY_BE_FALSE | MAY_BE_TRUE),
	F1("exec",                         MAY_BE_FALSE | MAY_BE_STRING),
	F1("system",                       MAY_BE_FALSE | MAY_BE_STRING),
	F1("escapeshellcmd",               MAY_BE_NULL | MAY_BE_STRING),
	F1("escapeshellarg",               MAY_BE_NULL | MAY_BE_STRING),
	F1("passthru",                     MAY_BE_FALSE | MAY_BE_STRING),
	F1("shell_exec",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	F1("proc_open",                    MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("proc_close",                   MAY_BE_FALSE | MAY_BE_LONG),
	F0("proc_terminate",               MAY_BE_FALSE | MAY_BE_TRUE),
	F1("proc_get_status",              MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
#endif
#ifdef HAVE_NICE
	F0("proc_nice",                    MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F0("rand",                         MAY_BE_NULL | MAY_BE_LONG),
	F0("srand",                        MAY_BE_NULL),
	F0("getrandmax",                   MAY_BE_NULL | MAY_BE_LONG),
	F0("mt_rand",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("mt_srand",                     MAY_BE_NULL),
	I0("mt_getrandmax",                MAY_BE_LONG),
#if HAVE_GETSERVBYNAME
	F0("getservbyname",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
#endif
#if HAVE_GETSERVBYPORT
	F1("getservbyport",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#endif
#if HAVE_GETPROTOBYNAME
	F0("getprotobyname",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
#endif
#if HAVE_GETPROTOBYNUMBER
	F1("getprotobynumber",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#endif
	F0("getmyuid",                     MAY_BE_FALSE | MAY_BE_LONG),
	F0("getmygid",                     MAY_BE_FALSE | MAY_BE_LONG),
	F0("getmypid",                     MAY_BE_FALSE | MAY_BE_LONG),
	F0("getmyinode",                   MAY_BE_FALSE | MAY_BE_LONG),
	F0("getlastmod",                   MAY_BE_FALSE | MAY_BE_LONG),
	F1("base64_decode",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("base64_encode",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("password_hash",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("password_get_info",            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F0("password_needs_rehash",        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("password_verify",              MAY_BE_FALSE | MAY_BE_TRUE),
	F1("convert_uuencode",             MAY_BE_FALSE | MAY_BE_STRING),
	F1("convert_uudecode",             MAY_BE_FALSE | MAY_BE_STRING),
	F0("abs",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_DOUBLE),
	F0("ceil",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_DOUBLE),
	F0("floor",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_DOUBLE),
	F0("round",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_DOUBLE),
	F0("sin",                          MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("cos",                          MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("tan",                          MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("asin",                         MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("acos",                         MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("atan",                         MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("atanh",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("atan2",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("sinh",                         MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("cosh",                         MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("tanh",                         MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("asinh",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("acosh",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("expm1",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("log1p",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("pi",                           MAY_BE_DOUBLE),
	F0("is_finite",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("is_nan",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("is_infinite",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("pow",                          MAY_BE_NULL | MAY_BE_LONG | MAY_BE_DOUBLE),
	F0("exp",                          MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("log",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_DOUBLE),
	F0("log10",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("sqrt",                         MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("hypot",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("deg2rad",                      MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("rad2deg",                      MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("bindec",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_DOUBLE),
	F0("hexdec",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_DOUBLE),
	F0("octdec",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_DOUBLE),
	F1("decbin",                       MAY_BE_NULL | MAY_BE_STRING),
	F1("decoct",                       MAY_BE_NULL | MAY_BE_STRING),
	FC("dechex",                       zend_dechex_info),
	F1("base_convert",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("number_format",                MAY_BE_NULL | MAY_BE_STRING),
	F0("fmod",                         MAY_BE_NULL | MAY_BE_DOUBLE),
#ifdef HAVE_INET_NTOP
	F1("inet_ntop",                    MAY_BE_FALSE | MAY_BE_STRING),
#endif
#ifdef HAVE_INET_PTON
	F1("inet_pton",                    MAY_BE_FALSE | MAY_BE_STRING),
#endif
	F0("ip2long",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("long2ip",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("getenv",                       MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
#ifdef HAVE_PUTENV
	F0("putenv",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F1("getopt",                       MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
#ifdef HAVE_GETLOADAVG
	F1("sys_getloadavg",               MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_DOUBLE),
#endif
#ifdef HAVE_GETTIMEOFDAY
	F1("microtime",                    MAY_BE_NULL | MAY_BE_DOUBLE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG | MAY_BE_STRING),
	F1("gettimeofday",                 MAY_BE_NULL | MAY_BE_DOUBLE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_STRING),
#endif
#ifdef HAVE_GETRUSAGE
	F1("getrusage",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG),
#endif
#ifdef HAVE_GETTIMEOFDAY
	F1("uniqid",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#endif
	F1("quoted_printable_decode",      MAY_BE_NULL | MAY_BE_STRING),
	F1("quoted_printable_encode",      MAY_BE_NULL | MAY_BE_STRING),
	F1("convert_cyr_string",           MAY_BE_NULL | MAY_BE_STRING),
	I1("get_current_user",             MAY_BE_STRING),
	F0("set_time_limit",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("header_register_callback",     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("get_cfg_var",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	I0("magic_quotes_runtime",         MAY_BE_FALSE),
	I0("set_magic_quotes_runtime",     MAY_BE_FALSE),
	I0("get_magic_quotes_gpc",         MAY_BE_FALSE),
	I0("get_magic_quotes_runtime",     MAY_BE_FALSE),
	F0("error_log",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	I1("error_get_last",               MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	FN("call_user_func",               UNKNOWN_INFO),
	FN("call_user_func_array",         UNKNOWN_INFO),
	FN("call_user_method",             UNKNOWN_INFO),
	FN("call_user_method_array",       UNKNOWN_INFO),
	FN("forward_static_call",          UNKNOWN_INFO),
	FN("forward_static_call_array",    UNKNOWN_INFO),
	F1("serialize",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	FN("unserialize",                  UNKNOWN_INFO),
	F0("var_dump",                     MAY_BE_NULL),
	F1("var_export",                   MAY_BE_NULL | MAY_BE_STRING),
	F0("debug_zval_dump",              MAY_BE_NULL),
	F1("print_r",                      MAY_BE_FALSE | MAY_BE_STRING),
	F0("memory_get_usage",             MAY_BE_FALSE | MAY_BE_LONG),
	F0("memory_get_peak_usage",        MAY_BE_FALSE | MAY_BE_LONG),
	F0("register_shutdown_function",   MAY_BE_NULL | MAY_BE_FALSE),
	F0("register_tick_function",       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("unregister_tick_function",     MAY_BE_NULL),
	F1("highlight_file",               MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("show_source",                  MAY_BE_FALSE | MAY_BE_STRING),
	F1("highlight_string",             MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("php_strip_whitespace",         MAY_BE_FALSE | MAY_BE_STRING),
	FN("ini_get",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("ini_get_all",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	FN("ini_set",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("ini_alter",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("ini_restore",                  MAY_BE_NULL),
	I1("get_include_path",             MAY_BE_FALSE | MAY_BE_STRING),
	F1("set_include_path",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("restore_include_path",         MAY_BE_NULL),
	F0("setcookie",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("setrawcookie",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("header",                       MAY_BE_NULL),
	F0("header_remove",                MAY_BE_NULL),
	F0("headers_sent",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("headers_list",                 MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F0("http_response_code",           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("connection_aborted",           MAY_BE_LONG),
	F0("connection_status",            MAY_BE_LONG),
	F0("ignore_user_abort",            MAY_BE_NULL | MAY_BE_LONG),
	F1("parse_ini_file",               MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("parse_ini_string",             MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
#if ZEND_DEBUG
	F1("config_get_hash",              MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
#endif
	F0("is_uploaded_file",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("move_uploaded_file",           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("gethostbyaddr",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("gethostbyname",                MAY_BE_NULL | MAY_BE_STRING),
	F1("gethostbynamel",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
#ifdef HAVE_GETHOSTNAME
	F1("gethostname",                  MAY_BE_FALSE | MAY_BE_STRING),
#endif
#if defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC
	F0("dns_check_record",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("checkdnsrr",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
# if defined(PHP_WIN32) || HAVE_FULL_DNS_FUNCS
	F0("dns_get_mx",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("getmxrr",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("dns_get_record",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ARRAY),
# endif
#endif
	F0("intval",                       MAY_BE_NULL | MAY_BE_LONG),
	F0("floatval",                     MAY_BE_NULL | MAY_BE_DOUBLE),
	F0("doubleval",                    MAY_BE_NULL | MAY_BE_DOUBLE),
	FN("strval",                       MAY_BE_NULL | MAY_BE_STRING),
	F0("boolval",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	FN("gettype",                      MAY_BE_NULL | MAY_BE_STRING),
	F0("settype",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	FC("is_null",                      zend_is_type_info),
	F0("is_resource",                  MAY_BE_FALSE | MAY_BE_TRUE), // TODO: inline with support for closed resources
	FC("is_bool",                      zend_is_type_info),
	FC("is_long",                      zend_is_type_info),
	FC("is_float",                     zend_is_type_info),
	FC("is_int",                       zend_is_type_info),
	FC("is_integer",                   zend_is_type_info),
	FC("is_double",                    zend_is_type_info),
	FC("is_real",                      zend_is_type_info),
	F0("is_numeric",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	FC("is_string",                    zend_is_type_info),
	FC("is_array",                     zend_is_type_info),
	F0("is_object",                    MAY_BE_FALSE | MAY_BE_TRUE), // TODO: inline with support for incomplete class
	F0("is_scalar",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("is_callable",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("is_countable",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("pclose",                       MAY_BE_FALSE | MAY_BE_LONG),
	F1("popen",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("readfile",                     MAY_BE_FALSE | MAY_BE_LONG),
	F0("rewind",                       MAY_BE_FALSE | MAY_BE_TRUE),
	F0("rmdir",                        MAY_BE_FALSE | MAY_BE_TRUE),
	F0("umask",                        MAY_BE_FALSE | MAY_BE_LONG),
	F0("fclose",                       MAY_BE_FALSE | MAY_BE_TRUE),
	F0("feof",                         MAY_BE_FALSE | MAY_BE_TRUE),
	F1("fgetc",                        MAY_BE_FALSE | MAY_BE_STRING),
	F1("fgets",                        MAY_BE_FALSE | MAY_BE_STRING),
	F1("fgetss",                       MAY_BE_FALSE | MAY_BE_STRING),
	F1("fread",                        MAY_BE_FALSE | MAY_BE_STRING),
	F1("fopen",                        MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("fpassthru",                    MAY_BE_FALSE | MAY_BE_LONG),
	F0("ftruncate",                    MAY_BE_FALSE | MAY_BE_TRUE),
	F1("fstat",                        MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG),
	F0("fseek",                        MAY_BE_FALSE | MAY_BE_LONG),
	F0("ftell",                        MAY_BE_FALSE | MAY_BE_LONG),
	F0("fflush",                       MAY_BE_FALSE | MAY_BE_TRUE),
	F0("fwrite",                       MAY_BE_FALSE | MAY_BE_LONG),
	F0("fputs",                        MAY_BE_FALSE | MAY_BE_LONG),
	F0("mkdir",                        MAY_BE_FALSE | MAY_BE_TRUE),
	F0("rename",                       MAY_BE_FALSE | MAY_BE_TRUE),
	F0("copy",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("tempnam",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("tmpfile",                      MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("file",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("file_get_contents",            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("file_put_contents",            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("stream_select",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("stream_context_create",        MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("stream_context_set_params",    MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stream_context_get_params",    MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F0("stream_context_set_option",    MAY_BE_FALSE | MAY_BE_TRUE),
	FN("stream_context_get_options",   MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	FN("stream_context_get_default",   MAY_BE_FALSE | MAY_BE_RESOURCE),
	FN("stream_context_set_default",   MAY_BE_FALSE | MAY_BE_RESOURCE),
	FN("stream_filter_prepend",        MAY_BE_FALSE | MAY_BE_RESOURCE),
	FN("stream_filter_append",         MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("stream_filter_remove",         MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stream_socket_client",         MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("stream_socket_server",         MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("stream_socket_accept",         MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("stream_socket_get_name",       MAY_BE_FALSE | MAY_BE_STRING),
	F1("stream_socket_recvfrom",       MAY_BE_FALSE | MAY_BE_STRING),
	F0("stream_socket_sendto",         MAY_BE_FALSE | MAY_BE_LONG),
	F0("stream_socket_enable_crypto",  MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_LONG),
#ifdef HAVE_SHUTDOWN
	F0("stream_socket_shutdown",       MAY_BE_FALSE | MAY_BE_TRUE),
#endif
#if HAVE_SOCKETPAIR
	F1("stream_socket_pair",           MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_RESOURCE),
#endif
	F0("stream_copy_to_stream",        MAY_BE_FALSE | MAY_BE_LONG),
	F1("stream_get_contents",          MAY_BE_FALSE | MAY_BE_STRING),
	F0("stream_supports_lock",         MAY_BE_FALSE | MAY_BE_TRUE),
	F1("fgetcsv",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING),
	F0("fputcsv",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("flock",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("get_meta_tags",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F0("stream_set_read_buffer",       MAY_BE_FALSE | MAY_BE_LONG),
	F0("stream_set_write_buffer",      MAY_BE_FALSE | MAY_BE_LONG),
	F0("set_file_buffer",              MAY_BE_FALSE | MAY_BE_LONG),
	F0("stream_set_chunk_size",        MAY_BE_FALSE | MAY_BE_LONG),
	F0("set_socket_blocking",          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("stream_set_blocking",          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("socket_set_blocking",          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stream_get_meta_data",         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("stream_get_line",              MAY_BE_FALSE | MAY_BE_STRING),
	F0("stream_wrapper_register",      MAY_BE_FALSE | MAY_BE_TRUE),
	F0("stream_register_wrapper",      MAY_BE_FALSE | MAY_BE_TRUE),
	F0("stream_wrapper_unregister",    MAY_BE_FALSE | MAY_BE_TRUE),
	F0("stream_wrapper_restore",       MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stream_get_wrappers",          MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("stream_get_transports",        MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("stream_resolve_include_path",  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("stream_is_local",              MAY_BE_FALSE | MAY_BE_TRUE),
	F1("get_headers",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
#if HAVE_SYS_TIME_H || defined(PHP_WIN32)
	F0("stream_set_timeout",           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("socket_set_timeout",           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F1("socket_get_status",            MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
#if HAVE_REALPATH || defined(ZTS)
	F1("realpath",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#endif
#ifdef HAVE_FNMATCH
	F0("fnmatch",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F1("fsockopen",                    MAY_BE_FALSE | MAY_BE_RESOURCE),
	FN("pfsockopen",                   MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pack",                         MAY_BE_FALSE | MAY_BE_STRING),
	F1("unpack",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("get_browser",                  MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_OBJECT | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("crypt",                        MAY_BE_NULL | MAY_BE_STRING),
	FN("opendir",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("closedir",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("chdir",                        MAY_BE_FALSE | MAY_BE_TRUE),
#if defined(HAVE_CHROOT) && !defined(ZTS) && ENABLE_CHROOT_FUNC
	F0("chroot",                       MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F1("getcwd",                       MAY_BE_FALSE | MAY_BE_STRING),
	F0("rewinddir",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("readdir",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("dir",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("scandir",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
#ifdef HAVE_GLOB
	F1("glob",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
#endif
	F0("fileatime",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("filectime",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("filegroup",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("fileinode",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("filemtime",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("fileowner",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("fileperms",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("filesize",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("filetype",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("file_exists",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("is_writable",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("is_writeable",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("is_readable",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("is_executable",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("is_file",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("is_dir",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("is_link",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stat",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("lstat",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F0("chown",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("chgrp",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#if HAVE_LCHOWN
	F0("lchown",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
#if HAVE_LCHOWN
	F0("lchgrp",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F0("chmod",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#if HAVE_UTIME
	F0("touch",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F0("clearstatcache",               MAY_BE_NULL),
	F0("disk_total_space",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_DOUBLE),
	F0("disk_free_space",              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_DOUBLE),
	F0("diskfreespace",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_DOUBLE),
	I0("realpath_cache_size",          MAY_BE_LONG),
	I1("realpath_cache_get",           MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F0("mail",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("ezmlm_hash",                   MAY_BE_NULL | MAY_BE_LONG),
#ifdef HAVE_SYSLOG_H
	F0("openlog",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("syslog",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("closelog",                     MAY_BE_TRUE),
#endif
	F0("lcg_value",                    MAY_BE_DOUBLE),
	F1("metaphone",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("ob_start",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("ob_flush",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("ob_clean",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("ob_end_flush",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("ob_end_clean",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("ob_get_flush",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("ob_get_clean",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("ob_get_length",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("ob_get_level",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("ob_get_status",                MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	FN("ob_get_contents",              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("ob_implicit_flush",            MAY_BE_NULL),
	F1("ob_list_handlers",             MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F0("ksort",                        MAY_BE_FALSE | MAY_BE_TRUE),
	F0("krsort",                       MAY_BE_FALSE | MAY_BE_TRUE),
	F0("natsort",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("natcasesort",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("asort",                        MAY_BE_FALSE | MAY_BE_TRUE),
	F0("arsort",                       MAY_BE_FALSE | MAY_BE_TRUE),
	F0("sort",                         MAY_BE_FALSE | MAY_BE_TRUE),
	F0("rsort",                        MAY_BE_FALSE | MAY_BE_TRUE),
	F0("usort",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("uasort",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("uksort",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("shuffle",                      MAY_BE_FALSE | MAY_BE_TRUE),
	F0("array_walk",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("array_walk_recursive",         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("count",                        MAY_BE_NULL | MAY_BE_LONG),
	FN("end",                          UNKNOWN_INFO),
	FN("prev",                         UNKNOWN_INFO),
	FN("next",                         UNKNOWN_INFO),
	FN("reset",                        UNKNOWN_INFO),
	FN("current",                      UNKNOWN_INFO),
	FN("key",                          MAY_BE_NULL | MAY_BE_LONG | MAY_BE_STRING),
	FN("min",                          UNKNOWN_INFO),
	FN("max",                          UNKNOWN_INFO),
	F0("in_array",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	FN("array_search",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING),
	F0("extract",                      MAY_BE_NULL | MAY_BE_LONG),
	F1("compact",                      MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_fill",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	F1("array_fill_keys",              MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	FC("range",                        zend_range_info),
	F0("array_multisort",              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("array_push",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	FN("array_pop",                    UNKNOWN_INFO),
	FN("array_shift",                  UNKNOWN_INFO),
	F0("array_unshift",                MAY_BE_NULL | MAY_BE_LONG),
	F1("array_splice",                 MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_slice",                  MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_merge",                  MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_merge_recursive",        MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_replace",                MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_replace_recursive",      MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	FN("array_keys",                   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	FN("array_values",                 MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_count_values",           MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG),
	F1("array_column",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_reverse",                MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_reduce",                 UNKNOWN_INFO),
	FN("array_pad",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_flip",                   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("array_change_key_case",        MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_rand",                   UNKNOWN_INFO),
	FN("array_unique",                 MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_intersect",              MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_intersect_key",          MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_intersect_ukey",         MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_uintersect",             MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_intersect_assoc",        MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_uintersect_assoc",       MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_intersect_uassoc",       MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_uintersect_uassoc",      MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	FN("array_diff",                   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_diff_key",               MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_diff_ukey",              MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_udiff",                  MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_diff_assoc",             MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_udiff_assoc",            MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_diff_uassoc",            MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_udiff_uassoc",           MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F0("array_sum",                    MAY_BE_NULL | MAY_BE_LONG | MAY_BE_DOUBLE),
	F0("array_product",                MAY_BE_NULL | MAY_BE_LONG | MAY_BE_DOUBLE),
	F1("array_filter",                 MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	FN("array_map",                    MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_chunk",                  MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_combine",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F0("array_key_exists",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("pos",                          UNKNOWN_INFO),
	F0("sizeof",                       MAY_BE_NULL | MAY_BE_LONG),
	F0("key_exists",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("assert",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("assert_options",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_OBJECT | MAY_BE_OBJECT),
	F0("version_compare",              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_LONG),
#if HAVE_FTOK
	F0("ftok",                         MAY_BE_NULL | MAY_BE_LONG),
#endif
	F1("str_rot13",                    MAY_BE_NULL | MAY_BE_STRING),
	I1("stream_get_filters",           MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F0("stream_filter_register",       MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stream_bucket_make_writeable", MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("stream_bucket_prepend",        MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("stream_bucket_append",         MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("stream_bucket_new",            MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("output_add_rewrite_var",       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("output_reset_rewrite_vars",    MAY_BE_FALSE),
	I1("sys_get_temp_dir",             MAY_BE_STRING),

	/* ext/date */
	F0("strtotime",                             MAY_BE_FALSE | MAY_BE_LONG),
	F1("date",                                  MAY_BE_FALSE | MAY_BE_STRING),
	F0("idate",                                 MAY_BE_FALSE | MAY_BE_LONG),
	F1("gmdate",                                MAY_BE_FALSE | MAY_BE_STRING),
	F0("mktime",                                MAY_BE_FALSE | MAY_BE_LONG),
	F0("gmmktime",                              MAY_BE_FALSE | MAY_BE_LONG),
	F0("checkdate",                             MAY_BE_FALSE | MAY_BE_TRUE),
#ifdef HAVE_STRFTIME
	F1("strftime",                              MAY_BE_FALSE | MAY_BE_STRING),
	F1("gmstrftime",                            MAY_BE_FALSE | MAY_BE_STRING),
#endif
	F0("time",                                  MAY_BE_LONG),
	F1("localtime",                             MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG),
	F1("getdate",                               MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("date_create",                           MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_create_immutable",                 MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_create_from_format",               MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_create_immutable_from_format",     MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_parse",                            MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("date_parse_from_format",                MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("date_get_last_errors",                  MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_ARRAY),
	F1("date_format",                           MAY_BE_FALSE | MAY_BE_STRING),
	FN("date_modify",                           MAY_BE_FALSE | MAY_BE_OBJECT),
	FN("date_add",                              MAY_BE_FALSE | MAY_BE_OBJECT),
	FN("date_sub",                              MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_timezone_get",                     MAY_BE_FALSE | MAY_BE_OBJECT),
	FN("date_timezone_set",                     MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("date_offset_get",                       MAY_BE_FALSE | MAY_BE_LONG),
	F1("date_diff",                             MAY_BE_FALSE | MAY_BE_OBJECT),
	FN("date_time_set",                         MAY_BE_FALSE | MAY_BE_OBJECT),
	FN("date_date_set",                         MAY_BE_FALSE | MAY_BE_OBJECT),
	FN("date_isodate_set",                      MAY_BE_FALSE | MAY_BE_OBJECT),
	FN("date_timestamp_set",                    MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("date_timestamp_get",                    MAY_BE_FALSE | MAY_BE_LONG),
	F1("timezone_open",                         MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("timezone_name_get",                     MAY_BE_FALSE | MAY_BE_STRING),
	F1("timezone_name_from_abbr",               MAY_BE_FALSE | MAY_BE_STRING),
	F0("timezone_offset_get",                   MAY_BE_FALSE | MAY_BE_LONG),
	F1("timezone_transitions_get",              MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("timezone_location_get",                 MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING),
	F1("timezone_identifiers_list",             MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("timezone_abbreviations_list",           MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("timezone_version_get",                  MAY_BE_STRING),
	F1("date_interval_create_from_date_string", MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_interval_format",                  MAY_BE_FALSE | MAY_BE_STRING),
	F0("date_default_timezone_set",             MAY_BE_FALSE | MAY_BE_TRUE),
	F1("date_default_timezone_get",             MAY_BE_STRING),
	F1("date_sunrise",                          MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_DOUBLE | MAY_BE_STRING),
	F1("date_sunset",                           MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_DOUBLE | MAY_BE_STRING),
	F1("date_sun_info",                         MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG),

	/* ext/preg */
	F0("preg_match",				            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("preg_match_all",			            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	FN("preg_replace",			                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING),
	FN("preg_replace_callback",	                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING),
	F1("preg_filter",				            MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING),
	F1("preg_split",				            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	FN("preg_quote",				            MAY_BE_NULL | MAY_BE_STRING),
	F1("preg_grep",				                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F0("preg_last_error",			            MAY_BE_NULL | MAY_BE_LONG),

	/* ext/ereg */
	F0("ereg",			                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("ereg_replace",	                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("eregi",			                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("eregi_replace",	                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("split",			                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("spliti",			                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("sql_regcase",		                    MAY_BE_NULL | MAY_BE_STRING),

	/* ext/mysql */
	F1("mysql_connect",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_pconnect",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("mysql_close",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysql_select_db",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysql_create_db",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysql_drop_db",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_query",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_RESOURCE),
	F1("mysql_unbuffered_query",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_RESOURCE),
	F1("mysql_db_query",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_RESOURCE),
	F1("mysql_list_dbs",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_list_tables",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_list_fields",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_list_processes",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_error",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("mysql_errno",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("mysql_affected_rows",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("mysql_insert_id",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_result",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("mysql_num_rows",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("mysql_num_fields",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_fetch_row",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	F1("mysql_fetch_array",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("mysql_fetch_assoc",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("mysql_fetch_object",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("mysql_data_seek",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_fetch_lengths",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("mysql_fetch_field",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("mysql_field_seek",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysql_free_result",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_field_name",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_field_table",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("mysql_field_len",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_field_type",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_field_flags",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_escape_string",					MAY_BE_NULL | MAY_BE_STRING),
	F1("mysql_real_escape_string",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_stat",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("mysql_thread_id",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_client_encoding",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("mysql_ping",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_get_client_info",					MAY_BE_NULL | MAY_BE_STRING),
	F1("mysql_get_host_info",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("mysql_get_proto_info",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_get_server_info",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_info",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("mysql_set_charset",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql",									MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_fieldname",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_fieldtable",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("mysql_fieldlen",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_fieldtype",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_fieldflags",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("mysql_selectdb",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysql_createdb",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysql_dropdb",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysql_freeresult",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysql_numfields",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("mysql_numrows",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_listdbs",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_listtables",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_listfields",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_db_name",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_dbname",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_tablename",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_table_name",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),

	/* ext/mysqli */
	F1("mysqli_connect",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("mysqli_close",							MAY_BE_NULL | MAY_BE_TRUE),
	I1("mysqli_connect_error",					MAY_BE_NULL | MAY_BE_STRING),
	I0("mysqli_connect_errno",					MAY_BE_LONG),
	F1("mysqli_get_client_stats",				MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("mysqli_error_list",						MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ARRAY),
	F1("mysqli_get_links_stats",				MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG),
	F1("mysqli_query",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_OBJECT),
	F0("mysqli_multi_query",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_set_charset",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysqli_get_charset", 					MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F0("mysqli_begin_transaction",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_savepoint",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_release_savepoint",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysqli_fetch_array",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("mysqli_fetch_assoc",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("mysqli_fetch_all",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("mysqli_fetch_object",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("mysqli_free_result",					MAY_BE_NULL),
	F0("mysqli_affected_rows",					MAY_BE_NULL | MAY_BE_LONG),
	F0("mysqli_autocommit",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_stmt_bind_param",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_stmt_bind_result",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_change_user",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysqli_character_set_name",				MAY_BE_NULL | MAY_BE_STRING),
	F0("mysqli_commit",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_data_seek",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_debug",							MAY_BE_NULL | MAY_BE_TRUE),
	F0("mysqli_dump_debug_info",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_errno",							MAY_BE_NULL | MAY_BE_LONG),
	F1("mysqli_error",							MAY_BE_NULL | MAY_BE_STRING),
	F0("mysqli_stmt_execute",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_poll",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysqli_reap_async_query",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_OBJECT),
	F1("mysqli_stmt_get_result",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysqli_get_warnings",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysqli_stmt_error_list",				MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ARRAY),
	F1("mysqli_stmt_get_warnings",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("mysqli_stmt_fetch",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysqli_fetch_field",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysqli_fetch_fields",					MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_OBJECT),
	F1("mysqli_fetch_field_direct",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysqli_fetch_lengths",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("mysqli_fetch_row",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	F0("mysqli_field_count",					MAY_BE_NULL | MAY_BE_LONG),
	F0("mysqli_field_seek",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_field_tell",						MAY_BE_NULL | MAY_BE_LONG),
	I1("mysqli_get_client_info",				MAY_BE_STRING),
	I0("mysqli_get_client_version",				MAY_BE_LONG),
	F1("mysqli_get_host_info",					MAY_BE_NULL | MAY_BE_STRING),
	F0("mysqli_get_proto_info",					MAY_BE_NULL | MAY_BE_LONG),
	F1("mysqli_get_server_info",				MAY_BE_NULL | MAY_BE_STRING),
	F0("mysqli_get_server_version",				MAY_BE_NULL | MAY_BE_LONG),
	F1("mysqli_info",							MAY_BE_NULL | MAY_BE_STRING),
	F1("mysqli_init",							MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("mysqli_insert_id",						MAY_BE_NULL | MAY_BE_LONG),
	F0("mysqli_kill",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_more_results",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_next_result",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_stmt_more_results",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_stmt_next_result",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_num_fields",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("mysqli_num_rows",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("mysqli_options",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_ping",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysqli_prepare",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("mysqli_real_connect",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_real_query",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysqli_real_escape_string",				MAY_BE_NULL | MAY_BE_STRING),
	F0("mysqli_rollback",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_stmt_send_long_data",			MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_stmt_affected_rows",				MAY_BE_NULL | MAY_BE_LONG),
	F0("mysqli_stmt_close",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_stmt_data_seek",					MAY_BE_NULL | MAY_BE_FALSE),
	F0("mysqli_stmt_field_count",				MAY_BE_NULL | MAY_BE_LONG),
	F0("mysqli_stmt_free_result",				MAY_BE_NULL),
	F0("mysqli_stmt_insert_id",					MAY_BE_NULL | MAY_BE_LONG),
	F0("mysqli_stmt_param_count",				MAY_BE_NULL | MAY_BE_LONG),
	F0("mysqli_stmt_reset",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_stmt_num_rows",					MAY_BE_NULL | MAY_BE_LONG),
	F0("mysqli_select_db",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysqli_sqlstate",						MAY_BE_NULL | MAY_BE_STRING),
	F0("mysqli_ssl_set",						MAY_BE_NULL | MAY_BE_TRUE),
	F1("mysqli_stat",							MAY_BE_NULL | MAY_BE_STRING),
	F0("mysqli_refresh",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_stmt_attr_set",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mysqli_stmt_attr_get",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("mysqli_stmt_errno",						MAY_BE_NULL | MAY_BE_LONG),
	F1("mysqli_stmt_error",						MAY_BE_NULL | MAY_BE_STRING),
	F1("mysqli_stmt_init",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("mysqli_stmt_prepare",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysqli_stmt_result_metadata",			MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("mysqli_stmt_store_result",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysqli_stmt_sqlstate",					MAY_BE_NULL | MAY_BE_STRING),
	F1("mysqli_store_result",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("mysqli_thread_id",						MAY_BE_NULL | MAY_BE_LONG),
	I0("mysqli_thread_safe",					MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysqli_use_result",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("mysqli_warning_count",					MAY_BE_NULL | MAY_BE_LONG),

	/* ext/curl */
	F1("curl_init",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("curl_copy_handle",                      MAY_BE_NULL | MAY_BE_RESOURCE),
	F1("curl_version",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F0("curl_setopt",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("curl_setopt_array",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	FN("curl_exec",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("curl_getinfo",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_LONG | MAY_BE_DOUBLE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("curl_error",                            MAY_BE_NULL | MAY_BE_STRING),
	F0("curl_errno",                            MAY_BE_NULL | MAY_BE_LONG),
	F0("curl_close",                            MAY_BE_NULL),
	F1("curl_strerror",                         MAY_BE_NULL | MAY_BE_STRING),
	F1("curl_multi_strerror",                   MAY_BE_NULL | MAY_BE_STRING),
	F0("curl_reset",                            MAY_BE_NULL),
	F1("curl_escape",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("curl_unescape",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("curl_pause",                            MAY_BE_NULL | MAY_BE_LONG),
	F1("curl_multi_init",                       MAY_BE_RESOURCE),
	F0("curl_multi_add_handle",                 MAY_BE_NULL | MAY_BE_LONG),
	F0("curl_multi_remove_handle",              MAY_BE_NULL | MAY_BE_LONG),
	F0("curl_multi_select",                     MAY_BE_NULL | MAY_BE_LONG),
	F0("curl_multi_exec",                       MAY_BE_NULL | MAY_BE_LONG),
	FN("curl_multi_getcontent",                 MAY_BE_NULL | MAY_BE_STRING),
	F1("curl_multi_info_read",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_RESOURCE),
	F0("curl_multi_close",                      MAY_BE_NULL | MAY_BE_FALSE),
	F0("curl_multi_setopt",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	I1("curl_share_init",                       MAY_BE_RESOURCE),
	F0("curl_share_close",                      MAY_BE_NULL),
	F0("curl_share_setopt",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("curl_file_create",                      MAY_BE_OBJECT),

	/* ext/mbstring */
	F1("mb_convert_case",                       MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_strtoupper",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_strtolower",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_language",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("mb_internal_encoding",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("mb_http_input",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_http_output",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("mb_detect_order",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("mb_substitute_character",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_LONG | MAY_BE_STRING),
	F0("mb_parse_str",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mb_output_handler",                     MAY_BE_NULL | MAY_BE_STRING),
	F1("mb_preferred_mime_name",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("mb_strlen",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("mb_strpos",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("mb_strrpos",                            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("mb_stripos",                            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("mb_strripos",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mb_strstr",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_strrchr",                            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_stristr",                            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_strrichr",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("mb_substr_count",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mb_substr",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_strcut",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("mb_strwidth",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mb_strimwidth",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_convert_encoding",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("mb_detect_encoding",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	I1("mb_list_encodings",                     MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("mb_encoding_aliases",                   MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("mb_convert_kana",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_encode_mimeheader",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_decode_mimeheader",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_convert_variables",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_encode_numericentity",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_decode_numericentity",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("mb_send_mail",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mb_get_info",                           MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F0("mb_check_encoding",                     MAY_BE_FALSE | MAY_BE_TRUE),

	F1("mb_regex_encoding",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("mb_regex_set_options",                  MAY_BE_FALSE | MAY_BE_STRING),
	F0("mb_ereg",                               MAY_BE_FALSE | MAY_BE_LONG),
	F0("mb_eregi",                              MAY_BE_FALSE | MAY_BE_LONG),
	F1("mb_ereg_replace",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_eregi_replace",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_ereg_replace_callback",              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_split",                              MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F0("mb_ereg_match",                         MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mb_ereg_search",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mb_ereg_search_pos",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("mb_ereg_search_regs",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_STRING),
	F0("mb_ereg_search_init",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mb_ereg_search_getregs",                MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_STRING),
	F0("mb_ereg_search_getpos",                 MAY_BE_LONG),
	F0("mb_ereg_search_setpos",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),

	F0("mbregex_encoding",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mbereg",                                MAY_BE_FALSE | MAY_BE_LONG),
	F0("mberegi",                               MAY_BE_FALSE | MAY_BE_LONG),
	F1("mbereg_replace",                        MAY_BE_FALSE | MAY_BE_STRING),
	F1("mberegi_replace",                       MAY_BE_FALSE | MAY_BE_STRING),
	F1("mbsplit",                               MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F0("mbereg_match",                          MAY_BE_FALSE | MAY_BE_TRUE),
	F0("mbereg_search",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mbereg_search_pos",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("mbereg_search_regs",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_STRING),
	F0("mbereg_search_init",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mbereg_search_getregs",                 MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_STRING),
	F0("mbereg_search_getpos",                  MAY_BE_LONG),
	F0("mbereg_search_setpos",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),

	/* ext/iconv */
	F1("iconv",                                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("iconv_get_encoding",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F0("iconv_set_encoding",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("iconv_strlen",                          MAY_BE_FALSE | MAY_BE_LONG),
	F1("iconv_substr",                          MAY_BE_FALSE | MAY_BE_STRING),
	F0("iconv_strpos",                          MAY_BE_FALSE | MAY_BE_LONG),
	F0("iconv_strrpos",                         MAY_BE_FALSE | MAY_BE_LONG),
	F1("iconv_mime_encode",                     MAY_BE_FALSE | MAY_BE_STRING),
	F1("iconv_mime_decode",                     MAY_BE_FALSE | MAY_BE_STRING),
	F1("iconv_mime_decode_headers",             MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),

	/* ext/json */
	F1("json_encode",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("json_decode",                           MAY_BE_ANY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	I0("json_last_error",                       MAY_BE_LONG),
	I1("json_last_error_msg",                   MAY_BE_STRING),

	/* ext/xml */
	FN("xml_parser_create",                     MAY_BE_FALSE | MAY_BE_RESOURCE),
	FN("xml_parser_create_ns",                  MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("xml_set_object",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("xml_set_element_handler",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("xml_set_character_data_handler",        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("xml_set_processing_instruction_handler",MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("xml_set_default_handler",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("xml_set_unparsed_entity_decl_handler",  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("xml_set_notation_decl_handler",         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("xml_set_external_entity_ref_handler",   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("xml_set_start_namespace_decl_handler",  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("xml_set_end_namespace_decl_handler",    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("xml_parse",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("xml_parse_into_struct",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("xml_get_error_code",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("xml_error_string",                      MAY_BE_NULL | MAY_BE_STRING),
	F0("xml_get_current_line_number",           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("xml_get_current_column_number",         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("xml_get_current_byte_index",            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("xml_parser_free",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("xml_parser_set_option",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("xml_parser_get_option",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING),
	F1("utf8_encode",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("utf8_decode",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),

	/* ext/zlib */
	F0("readgzfile",                            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("gzrewind",                              MAY_BE_FALSE | MAY_BE_TRUE),
	F0("gzclose",                               MAY_BE_FALSE | MAY_BE_TRUE),
	F0("gzeof",                                 MAY_BE_FALSE | MAY_BE_TRUE),
	F1("gzgetc",                                MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzgets",                                MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzgetss",                               MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzread",                                MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzopen",                                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("gzpassthru",                            MAY_BE_FALSE | MAY_BE_LONG),
	F0("gzseek",                                MAY_BE_FALSE | MAY_BE_LONG),
	F0("gztell",                                MAY_BE_FALSE | MAY_BE_LONG),
	F0("gzwrite",                               MAY_BE_FALSE | MAY_BE_LONG),
	F0("gzputs",                                MAY_BE_FALSE | MAY_BE_LONG),
	F1("gzfile",                                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("gzcompress",                            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzuncompress",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzdeflate",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzinflate",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzencode",                              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzdecode",                              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("zlib_encode",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("zlib_decode",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	I1("zlib_get_coding_type",                  MAY_BE_FALSE | MAY_BE_STRING),
	F1("ob_gzhandler",                          MAY_BE_FALSE | MAY_BE_STRING),

	/* ext/hash */
	F1("hash",                                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("hash_file",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("hash_hmac",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("hash_hmac_file",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("hash_init",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F0("hash_update",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("hash_update_stream",                    MAY_BE_NULL | MAY_BE_LONG),
	F0("hash_update_file",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("hash_final",                            MAY_BE_NULL | MAY_BE_STRING),
	F1("hash_copy",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("hash_algos",                            MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("hash_pbkdf2",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mhash_keygen_s2k",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("mhash_get_block_size",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mhash_get_hash_name",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	I0("mhash_count",                           MAY_BE_LONG),
	F1("mhash",                                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),

	/* ext/sodium */
	F0("sodium_memzero",						MAY_BE_NULL),
	F0("sodium_increment",						MAY_BE_NULL),
	F0("sodium_add",							MAY_BE_NULL),
	F0("sodium_memcmp",							MAY_BE_NULL | MAY_BE_LONG),
	F1("sodium_crypto_shorthash",				MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_secretbox",				MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_secretbox_open",			MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_crypto_generichash",				MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_generichash_init",		MAY_BE_NULL | MAY_BE_STRING),
	F0("sodium_crypto_generichash_update",		MAY_BE_NULL | MAY_BE_TRUE),
	F1("sodium_crypto_generichash_final",		MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_box_keypair",				MAY_BE_STRING),
	F1("sodium_crypto_box_seed_keypair",		MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_box_secretkey",			MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_box_publickey",			MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_box",						MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_box_open",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_crypto_box_seal",				MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_box_seal_open",			MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_crypto_sign_keypair",			MAY_BE_STRING),
	F1("sodium_crypto_sign_seed_keypair",		MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_sign_secretkey",			MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_sign_publickey",			MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_sign",					MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_sign_open",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_crypto_sign_detached",			MAY_BE_NULL | MAY_BE_STRING),
	F0("sodium_crypto_sign_verify_detached",	MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("sodium_crypto_stream",					MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_stream_xor",				MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_pwhash",					MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_pwhash_str",				MAY_BE_NULL | MAY_BE_STRING),
	F0("sodium_crypto_pwhash_str_verify",		MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("sodium_crypto_aead_aes256gcm_encrypt",	MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_aead_aes256gcm_decrypt",	MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_bin2hex",						MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_hex2bin",						MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_scalarmult",				MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_kx_seed_keypair",			MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_kx_keypair",				MAY_BE_STRING),
	F1("sodium_crypto_kx_secretkey",			MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_kx_publickey",			MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_kx_client_session_keys",	MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("sodium_crypto_kx_server_session_keys",	MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("sodium_crypto_auth",					MAY_BE_NULL | MAY_BE_STRING),
	F0("sodium_crypto_auth_verify",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("sodium_compare",						MAY_BE_NULL | MAY_BE_LONG),
	F1("sodium_crypto_aead_aes256gcm_keygen",	MAY_BE_STRING),
	F1("sodium_crypto_auth_keygen",				MAY_BE_STRING),
	F1("sodium_crypto_generichash_keygen",		MAY_BE_STRING),
	F1("sodium_crypto_kdf_keygen",				MAY_BE_STRING),
	F1("sodium_crypto_secretbox_keygen",		MAY_BE_STRING),
	F1("sodium_crypto_shorthash_keygen",		MAY_BE_STRING),
	F1("sodium_crypto_stream_keygen",			MAY_BE_STRING),
	F1("sodium_crypto_kdf_derive_from_key",		MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_pad",							MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_unpad",							MAY_BE_NULL | MAY_BE_STRING),

	F1("sodium_crypto_box_keypair_from_secretkey_and_publickey",	MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_box_publickey_from_secretkey",				MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_sign_keypair_from_secretkey_and_publickey",	MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_sign_publickey_from_secretkey",				MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_pwhash_scryptsalsa208sha256",					MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_pwhash_scryptsalsa208sha256_str",				MAY_BE_NULL | MAY_BE_STRING),
	F0("sodium_crypto_pwhash_scryptsalsa208sha256_str_verify",		MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("sodium_crypto_aead_aes256gcm_is_available",					MAY_BE_FALSE | MAY_BE_TRUE),
	F1("sodium_crypto_sign_ed25519_sk_to_curve25519",				MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_sign_ed25519_pk_to_curve25519",				MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_aead_chacha20poly1305_encrypt",				MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_aead_chacha20poly1305_decrypt",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_crypto_aead_chacha20poly1305_ietf_encrypt",			MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_aead_chacha20poly1305_ietf_decrypt",			MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_crypto_aead_xchacha20poly1305_ietf_encrypt",			MAY_BE_NULL | MAY_BE_STRING),
	F1("sodium_crypto_aead_xchacha20poly1305_ietf_decrypt",			MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("sodium_crypto_aead_chacha20poly1305_keygen",				MAY_BE_STRING),
	F1("sodium_crypto_aead_chacha20poly1305_ietf_keygen",			MAY_BE_STRING),
	F1("sodium_crypto_aead_xchacha20poly1305_ietf_keygen",			MAY_BE_STRING),

	/* ext/session */
	F0("session_set_cookie_params",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	I1("session_get_cookie_params",				MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("session_name",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("session_module_name",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("session_set_save_handler",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("session_save_path",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	FN("session_id",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("session_regenerate_id",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("session_create_id",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("session_cache_limiter",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("session_cache_expire",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	I1("session_encode",						MAY_BE_FALSE | MAY_BE_STRING),
	F0("session_decode",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("session_start",							MAY_BE_FALSE | MAY_BE_TRUE),
	I0("session_destroy",						MAY_BE_FALSE | MAY_BE_TRUE),
	I0("session_unset",							MAY_BE_FALSE | MAY_BE_TRUE),
	F0("session_gc",							MAY_BE_FALSE | MAY_BE_LONG),
	F0("session_write_close",					MAY_BE_FALSE | MAY_BE_TRUE),
	F0("session_abort",							MAY_BE_FALSE | MAY_BE_TRUE),
	F0("session_reset",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("session_status",						MAY_BE_NULL | MAY_BE_LONG),
	I0("session_register_shutdown",				MAY_BE_NULL),

	/* ext/pgsql */
	F1("pg_connect",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	FN("pg_pconnect",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("pg_connect_poll",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("pg_close",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("pg_dbname",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_last_error",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_options",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_port",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_tty",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_host",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_version",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("pg_parameter_status",					MAY_BE_FALSE | MAY_BE_STRING),
	F0("pg_ping",								MAY_BE_FALSE | MAY_BE_TRUE),
	F1("pg_query",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pg_query_params",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pg_prepare",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pg_execute",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("pg_num_rows",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("pg_num_fields",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("pg_affected_rows",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	FN("pg_last_notice",						UNKNOWN_INFO),
	F1("pg_field_table",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING),
	F1("pg_field_name",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("pg_field_size",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("pg_field_type",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_field_type_oid",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING),
	F0("pg_field_num",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("pg_fetch_result",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_fetch_row",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING),
	F1("pg_fetch_assoc",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING),
	F1("pg_fetch_array",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING),
	F1("pg_fetch_object",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("pg_fetch_all",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ARRAY),
	F1("pg_fetch_all_columns",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING),
	F0("pg_result_seek",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("pg_field_prtlen",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("pg_field_is_null",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("pg_free_result",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("pg_last_oid",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING),
	F0("pg_trace",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("pg_untrace",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("pg_lo_create",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING),
	F0("pg_lo_unlink",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("pg_lo_open",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pg_lo_read",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("pg_lo_write",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("pg_lo_read_all",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("pg_lo_import",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING),
	F0("pg_lo_export",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("pg_lo_seek",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("pg_lo_tell",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("pg_lo_truncate",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("pg_set_error_verbosity",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("pg_set_client_encoding",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("pg_end_copy",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("pg_put_line",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("pg_copy_to",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F0("pg_copy_from",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("pg_escape_string",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_escape_bytea",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_unescape_bytea",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_escape_literal",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_escape_identifier",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_result_error",						MAY_BE_FALSE | MAY_BE_STRING),
	F1("pg_result_error_field",					MAY_BE_FALSE | MAY_BE_STRING),
	F0("pg_connection_status",					MAY_BE_FALSE | MAY_BE_LONG),
	F0("pg_transaction_status",					MAY_BE_FALSE | MAY_BE_LONG),
	F0("pg_connection_reset",					MAY_BE_FALSE | MAY_BE_TRUE),
	F0("pg_cancel_query",						MAY_BE_FALSE | MAY_BE_TRUE),
	F0("pg_connection_busy",					MAY_BE_FALSE | MAY_BE_TRUE),
	F0("pg_send_query",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_LONG),
	F0("pg_send_query_params",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_LONG),
	F0("pg_send_prepare",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_LONG),
	F0("pg_send_execute",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_LONG),
	F1("pg_get_result",							MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pg_result_status",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING),
	F1("pg_get_notify",							MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F0("pg_get_pid",							MAY_BE_FALSE | MAY_BE_LONG),
	F1("pg_socket",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("pg_consume_input",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("pg_flush",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_LONG),
	F1("pg_meta_data",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("pg_convert",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("pg_insert",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_RESOURCE | MAY_BE_STRING),
	F1("pg_update",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("pg_delete",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("pg_select",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),

	/* ext/bcmath */
	F1("bcadd",									MAY_BE_NULL | MAY_BE_STRING),
	F1("bcsub",									MAY_BE_NULL | MAY_BE_STRING),
	F1("bcmul",									MAY_BE_NULL | MAY_BE_STRING),
	F1("bcdiv",									MAY_BE_NULL | MAY_BE_STRING),
	F1("bcmod",									MAY_BE_NULL | MAY_BE_STRING),
	F1("bcpowmod",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("bcpow",									MAY_BE_NULL | MAY_BE_STRING),
	F1("bcsqrt",								MAY_BE_NULL | MAY_BE_STRING),
	F0("bccomp",								MAY_BE_NULL | MAY_BE_LONG),
	F0("bcscale",								MAY_BE_NULL | MAY_BE_LONG),

	/* ext/exif */
	F1("exif_tagname",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("exif_read_data",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("exif_thumbnail",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F0("exif_imagetype",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),

	/* ext/filter */
	F0("filter_has_var",						MAY_BE_FALSE | MAY_BE_TRUE),
	FN("filter_input",							UNKNOWN_INFO),
	FN("filter_var",							UNKNOWN_INFO),
	F1("filter_input_array",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("filter_var_array",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	I1("filter_list",							MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F0("filter_id",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),

	/* ext/gettext */
	F1("textdomain",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("gettext",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("_",										MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("dgettext",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("dcgettext",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("bindtextdomain",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#if HAVE_NGETTEXT
	F1("ngettext",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#endif
#if HAVE_DNGETTEXT
	F1("dcngettext",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#endif
#if HAVE_BIND_TEXTDOMAIN_CODESET
	F1("bind_textdomain_codeset",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#endif

	/* ext/ctype */
	F0("ctype_alnum",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("ctype_alpha",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("ctype_cntrl",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("ctype_digit",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("ctype_lower",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("ctype_graph",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("ctype_print",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("ctype_punct",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("ctype_space",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("ctype_upper",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("ctype_xdigit",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),

	/* ext/fileinfo */
	F1("finfo_open",							MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("finfo_close",							MAY_BE_FALSE | MAY_BE_TRUE),
	F0("finfo_set_flags",						MAY_BE_FALSE | MAY_BE_TRUE),
	F1("finfo_file",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("finfo_buffer",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mime_content_type",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),

	/* ext/gd */
	F1("gd_info",								MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE),
	F0("imageloadfont",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("imagesetstyle",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("imagecreatetruecolor",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("imageistruecolor",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagetruecolortopalette",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagepalettetotruecolor",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagecolormatch",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagesetthickness",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagefilledellipse",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagefilledarc",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagealphablending",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagesavealpha",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagelayereffect",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagecolorallocatealpha",				MAY_BE_FALSE | MAY_BE_LONG),
	F0("imagecolorresolvealpha",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("imagecolorclosestalpha",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("imagecolorexactalpha",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("imagecopyresampled",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#ifdef PHP_WIN32
	F1("imagegrabwindow",						MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("imagegrabscreen",						MAY_BE_FALSE | MAY_BE_RESOURCE),
#endif
	F1("imagerotate",							MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("imagesettile",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagesetbrush",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("imagecreate",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	I0("imagetypes",							MAY_BE_LONG),
	F1("imagecreatefromstring",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("imagecreatefromgif",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
#ifdef HAVE_GD_JPG
	F1("imagecreatefromjpeg",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("imagejpeg",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("jpeg2wbmp",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
#ifdef HAVE_GD_PNG
	F1("imagecreatefrompng",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("imagepng",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("png2wbmp",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
#ifdef HAVE_GD_WEBP
	F1("imagecreatefromwebp",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("imagewebp",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F1("imagecreatefromxbm",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
#if defined(HAVE_GD_XPM)
	F1("imagecreatefromxpm",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
#endif
	F1("imagecreatefromwbmp",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("imagecreatefromgd",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("imagecreatefromgd2",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("imagecreatefromgd2part",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
#if defined(HAVE_GD_BMP)
	F1("imagecreatefrombmp",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F0("imagebmp",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F0("imagexbm",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagegif",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagewbmp",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagegd",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagegd2",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagedestroy",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("magecolorallocate",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("imagepalettecopy",						MAY_BE_NULL | MAY_BE_FALSE),
	F0("imagecolorat",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("imagecolorclosest",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("imagecolorclosesthwb",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("imagecolordeallocate",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagecolorresolve",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("imagecolorexact",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("imagecolorset",							MAY_BE_NULL | MAY_BE_FALSE),
	F1("imagecolorsforindex",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY |  MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG),
	F0("imagegammacorrect",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagesetpixel",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imageline",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagedashedline",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagerectangle",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagefilledrectangle",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagearc",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imageellipse",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagefilltoborder",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagefill",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagecolorstotal",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("imagecolortransparent",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("imageinterlace",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("imagepolygon",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imageopenpolygon",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagefilledpolygon",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagefontwidth",						MAY_BE_NULL | MAY_BE_LONG),
	F0("imagefontheight",						MAY_BE_NULL | MAY_BE_LONG),
	F0("imagechar",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagecharup",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagestring",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagestringup",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagecopy",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagecopymerge",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagecopymergegray",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagecopyresized",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagesx",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("imagesy",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F0("imagesetclip",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("imagegetclip",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("imageftbbox",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("imagefttext",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("imagettfbbox",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("imagettftext",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F0("image2wbmp",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imagefilter",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imageconvolution",						MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imageflip",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F0("imageantialias",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("imagecrop",								MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("imagecropauto",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("imagescale",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("imageaffine",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("imageaffinematrixget",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_DOUBLE),
	F1("imageaffinematrixconcat",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_DOUBLE),
	F0("imagesetinterpolation",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("imageresolution",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),

};

static HashTable func_info;
int zend_func_info_rid = -1;

uint32_t zend_get_func_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	uint32_t ret = 0;

	if (call_info->callee_func->type == ZEND_INTERNAL_FUNCTION) {
		zval *zv;
		func_info_t *info;

		zv = zend_hash_find_ex(&func_info, Z_STR_P(CRT_CONSTANT_EX(call_info->caller_op_array, call_info->caller_init_opline, call_info->caller_init_opline->op2, ssa->rt_constants)), 1);
		if (zv) {
			info = Z_PTR_P(zv);
			if (UNEXPECTED(zend_optimizer_is_disabled_func(info->name, info->name_len))) {
				ret = MAY_BE_NULL;
			} else if (info->info_func) {
				ret = info->info_func(call_info, ssa);
			} else if (/*call_info->callee_func->common.arg_info && */
					call_info->callee_func->common.num_args == 0 &&
					call_info->callee_func->common.required_num_args == 0) {
				if (call_info->num_args == 0) {
					ret = info->info;
				} else {
					ret = FUNC_MAY_WARN | MAY_BE_NULL;
				}
			} else {
				ret = info->info;
			}
#if 0
		} else {
			fprintf(stderr, "Unknown internal function '%s'\n", func->common.function_name);
#endif
		}
	} else {
		// FIXME: the order of functions matters!!!
		zend_func_info *info = ZEND_FUNC_INFO((zend_op_array*)call_info->callee_func);
		if (info) {
			ret = info->return_info.type;
		}
	}
	if (!ret) {
		ret = MAY_BE_ANY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF;
		if (call_info->callee_func->type == ZEND_INTERNAL_FUNCTION) {
			ret |= FUNC_MAY_WARN;
		}
		if (call_info->callee_func->common.fn_flags & ZEND_ACC_GENERATOR) {
			ret = MAY_BE_RC1 | MAY_BE_RCN | MAY_BE_OBJECT;
		} else  if (call_info->callee_func->common.fn_flags & ZEND_ACC_RETURN_REFERENCE) {
			ret |= MAY_BE_REF;
		} else {
			ret |= MAY_BE_RC1 | MAY_BE_RCN;
		}
	}
	return ret;
}

int zend_func_info_startup(void)
{
	zend_extension dummy;
	size_t i;

	if (zend_func_info_rid == -1) {
		zend_func_info_rid = zend_get_resource_handle(&dummy);
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
