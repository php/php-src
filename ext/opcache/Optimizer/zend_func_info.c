/*
   +----------------------------------------------------------------------+
   | Zend Engine, Func Info                                               |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

/* $Id:$ */

#include "php.h"
#include "zend_compile.h"
#include "zend_extensions.h"
#include "zend_ssa.h"
#include "zend_inference.h"
#include "zend_call_graph.h"
#include "zend_func_info.h"
#include "zend_inference.h"

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
	{name, sizeof(name)-1, (FUNC_MAY_WARN | MAY_BE_RC1 | (info)), NULL}
#define FN(name, info) \
	{name, sizeof(name)-1, (FUNC_MAY_WARN | MAY_BE_RC1 | MAY_BE_RCN | (info)), NULL}
#define FR(name, info) \
	{name, sizeof(name)-1, (FUNC_MAY_WARN | MAY_BE_REF | (info)), NULL}
#define FX(name, info) \
	{name, sizeof(name)-1, (FUNC_MAY_WARN | MAY_BE_RC1 | MAY_BE_RCN | MAY_BE_REF | (info)), NULL}
#define I1(name, info) \
	{name, sizeof(name)-1, (MAY_BE_RC1 | (info)), NULL}
#define FC(name, callback) \
	{name, sizeof(name)-1, 0, callback}

static uint32_t zend_strlen_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	if (call_info->caller_init_opline->extended_value == call_info->num_args &&
	    call_info->num_args == 1) {

		uint32_t tmp = MAY_BE_RC1;
		if (call_info->arg_info[0].opline) {
			uint32_t arg_info = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[0].opline);

			if (arg_info & (MAY_BE_NULL|MAY_BE_FALSE|MAY_BE_TRUE|MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING|MAY_BE_OBJECT)) {
				tmp |= MAY_BE_LONG;
			}
			if (arg_info & (MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)) {
				/* warning, and returns NULL */
				tmp |= FUNC_MAY_WARN | MAY_BE_NULL;
			}
			if ((arg_info & MAY_BE_ANY) == MAY_BE_STRING) {
				/* TODO: strlen() may be overriden by mbstring */
				tmp |= FUNC_MAY_INLINE;
			}
		} else {
			tmp |= MAY_BE_LONG | FUNC_MAY_WARN | MAY_BE_NULL;
		}
		return tmp;
	} else {
		/* warning, and returns NULL */
		return FUNC_MAY_WARN | MAY_BE_RC1 | MAY_BE_NULL;
	}
}

static uint32_t zend_dechex_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	if (call_info->caller_init_opline->extended_value == call_info->num_args &&
	    call_info->num_args == 1) {
		return MAY_BE_RC1 | MAY_BE_STRING;
	} else {
		/* warning, and returns NULL */
		return FUNC_MAY_WARN | MAY_BE_RC1 | MAY_BE_NULL;
	}
}

static uint32_t zend_range_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	if (call_info->caller_init_opline->extended_value == call_info->num_args &&
	    (call_info->num_args == 2 || call_info->num_args == 3)) {

		uint32_t t1 = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[0].opline);
		uint32_t t2 = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[1].opline);
		uint32_t t3 = 0;
		uint32_t tmp = MAY_BE_RC1 | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG;

		if (call_info->num_args == 3) {
			t3 = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[2].opline);
		}
		if ((t1 & MAY_BE_STRING) && (t2 & MAY_BE_STRING)) {
			tmp |= MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING;
			tmp |= FUNC_MAY_WARN | MAY_BE_FALSE;
		}
		if ((t1 & MAY_BE_DOUBLE) || (t2 & MAY_BE_DOUBLE) || (t3 & MAY_BE_DOUBLE)) {
			tmp |= MAY_BE_ARRAY_OF_DOUBLE;
			tmp |= FUNC_MAY_WARN | MAY_BE_FALSE;
		}
		if ((t1 & (MAY_BE_ANY-(MAY_BE_STRING|MAY_BE_DOUBLE))) && (t2 & (MAY_BE_ANY-(MAY_BE_STRING|MAY_BE_DOUBLE)))) {
			if (call_info->num_args == 2 && !(t3 & MAY_BE_DOUBLE)) {
				tmp |= MAY_BE_ARRAY_OF_LONG;
			}
			if (call_info->num_args == 3) {
				tmp |= FUNC_MAY_WARN | MAY_BE_FALSE;
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
	if (call_info->caller_init_opline->extended_value == call_info->num_args &&
	    call_info->num_args == 1) {
		return MAY_BE_RC1 | MAY_BE_FALSE | MAY_BE_TRUE | FUNC_MAY_INLINE;
	} else {
		return MAY_BE_RC1 | MAY_BE_FALSE | MAY_BE_TRUE | FUNC_MAY_WARN;
	}
}

static uint32_t zend_l_ss_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	if (call_info->caller_init_opline->extended_value == call_info->num_args &&
	    call_info->num_args == 2) {

		uint32_t arg1_info = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[0].opline);
		uint32_t arg2_info = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[1].opline);
		uint32_t tmp = MAY_BE_RC1;

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
		return FUNC_MAY_WARN | MAY_BE_RC1 | MAY_BE_NULL | MAY_BE_LONG;
	}
}

static uint32_t zend_lb_ssn_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	if (call_info->caller_init_opline->extended_value == call_info->num_args &&
	    call_info->num_args == 3) {
		uint32_t arg1_info = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[0].opline);
		uint32_t arg2_info = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[1].opline);
		uint32_t arg3_info = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[2].opline);
		uint32_t tmp = MAY_BE_RC1;

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
		return FUNC_MAY_WARN | MAY_BE_RC1 | MAY_BE_NULL | MAY_BE_LONG;
	}
}

static uint32_t zend_b_s_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	if (call_info->caller_init_opline->extended_value == call_info->num_args &&
	    call_info->num_args == 1) {

		uint32_t arg1_info = _ssa_op1_info(call_info->caller_op_array, ssa, call_info->arg_info[0].opline);
		uint32_t tmp = MAY_BE_RC1;

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
		return FUNC_MAY_WARN | MAY_BE_RC1 | MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE;
	}
}

#define UNKNOWN_INFO (MAY_BE_ANY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF)

static const func_info_t func_infos[] = {
	/* zend */
	I1("zend_version",            MAY_BE_STRING),
	I1("gc_collect_cycles",       MAY_BE_LONG),
	I1("gc_enabled",              MAY_BE_FALSE | MAY_BE_TRUE),
	F1("gc_enable",               MAY_BE_NULL),
	F1("gc_disable",              MAY_BE_NULL),
	F1("func_num_args",           MAY_BE_LONG),
	F1("func_get_arg",            UNKNOWN_INFO),
	F1("func_get_args",           MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF),
	FC("strlen",                  zend_strlen_info),
	FC("strcmp",                  zend_l_ss_info),
	FC("strncmp",                 zend_lb_ssn_info),
	FC("strcasecmp",              zend_l_ss_info),
	FC("strncasecmp",             zend_lb_ssn_info),
	F1("each",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("error_reporting",         MAY_BE_NULL | MAY_BE_LONG),
	F1("define",                  MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_NULL), // TODO: inline
	FC("defined",                 zend_b_s_info), // TODO: inline
	F1("get_class",               MAY_BE_FALSE | MAY_BE_STRING),
	F1("get_called_class",        MAY_BE_FALSE | MAY_BE_STRING),
	F1("get_parrent_class",       MAY_BE_FALSE | MAY_BE_STRING),
	F1("is_subclass_of",          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE), // TODO: inline
	F1("is_a",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE), // TODO: inline
	F1("get_class_vars",          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF),
	F1("get_object_vars",         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF),
	F1("get_class_methods",       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("method_exists",           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("property_exists",         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("class_exists",            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("interface_exists",        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("trait_exists",            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	FC("function_exists",         zend_b_s_info), // TODO: inline
	F1("class_alias",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("get_included_files",      MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("trigger_error",           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("user_error",              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("set_error_handler",       MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_OBJECT | MAY_BE_OBJECT),
	I1("restore_error_handler",   MAY_BE_TRUE),
	F1("get_declared_traits",     MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("get_declared_classes",    MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("get_declared_interfaces", MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("get_defined_functions",   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ARRAY),
	I1("get_defined_vars",        MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY | MAY_BE_ARRAY_OF_REF),
	F1("create_function",         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_STRING),
	F1("get_resource_type",       MAY_BE_NULL | MAY_BE_STRING),
	F1("get_defined_constants",   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_RESOURCE | MAY_BE_ARRAY_OF_ARRAY),
	F1("debug_print_backtrace",   MAY_BE_NULL),
	F1("debug_backtrace",         MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ARRAY),
	F1("get_loaded_extensions",   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	FC("extension_loaded",        zend_b_s_info),
	F1("get_extension_funcs",     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),

	/* ext/statdard */
	F1("constant",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_LONG | MAY_BE_DOUBLE | MAY_BE_STRING | MAY_BE_RESOURCE),
	F1("bin2hex",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("hex2bin",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("sleep",                        MAY_BE_FALSE | MAY_BE_LONG),
	F1("usleep",                       MAY_BE_NULL | MAY_BE_FALSE),
#if HAVE_NANOSLEEP
	F1("time_nanosleep",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("time_sleep_until",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
#if HAVE_STRPTIME
	F1("strptime",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
#endif
	F1("flush",                        MAY_BE_NULL),
	F1("wordwrap",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("htmlspecialchars",             MAY_BE_NULL | MAY_BE_STRING),
	F1("htmlentities",                 MAY_BE_NULL | MAY_BE_STRING),
	F1("html_entity_decode",           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("htmlspecialchars_decode",      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("get_html_translation_table",   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("sha1",                         MAY_BE_NULL | MAY_BE_STRING),
	F1("sha1_file",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("md5",                          MAY_BE_NULL | MAY_BE_STRING),
	F1("md5_file",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("crc32",                        MAY_BE_NULL | MAY_BE_LONG),
	F1("iptcparse",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("iptcembed",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("getimagesize",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("getimagesizefromstring",       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("image_type_to_mime_type",      MAY_BE_NULL | MAY_BE_STRING),
	F1("image_type_to_extension",      MAY_BE_FALSE | MAY_BE_STRING),
	F1("phpinfo",                      MAY_BE_NULL | MAY_BE_TRUE),
	F1("phpversion",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("phpcredits",                   MAY_BE_NULL | MAY_BE_TRUE),
	F1("php_sapi_name",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("php_uname",                    MAY_BE_NULL | MAY_BE_STRING),
	F1("php_ini_scanned_files",        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("php_ini_loaded_file",          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("strnatcmp",                    MAY_BE_NULL | MAY_BE_LONG),
	F1("strnatcasecmp",                MAY_BE_NULL | MAY_BE_LONG),
	F1("substr_count",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("strspn",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("strcspn",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("strtok",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("strtoupper",                   MAY_BE_NULL | MAY_BE_STRING),
	F1("strtolower",                   MAY_BE_NULL | MAY_BE_STRING),
	F1("strpos",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("stripos",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("strrpos",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("strripos",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
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
	F1("str_word_count",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("str_split",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("strpbrk",                      MAY_BE_FALSE | MAY_BE_STRING),
	F1("substr_compare",               MAY_BE_FALSE | MAY_BE_LONG),
#ifdef HAVE_STRCOLL
	F1("strcoll",                      MAY_BE_NULL | MAY_BE_LONG),
#endif
#ifdef HAVE_STRFMON
	F1("money_format",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#endif
	F1("substr",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("substr_replace",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING),
	F1("quotemeta",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("ucfirst",                      MAY_BE_NULL | MAY_BE_STRING),
	F1("lcfirst",                      MAY_BE_NULL | MAY_BE_STRING),
	F1("ucwords",                      MAY_BE_NULL | MAY_BE_STRING),
	F1("strtr",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("addslashes",                   MAY_BE_NULL | MAY_BE_STRING),
	F1("addcslashes",                  MAY_BE_NULL | MAY_BE_STRING),
	F1("rtrim",                        MAY_BE_NULL | MAY_BE_STRING),
	F1("str_replace",                  MAY_BE_NULL | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY | MAY_BE_ARRAY_OF_OBJECT),
	F1("str_ireplace",                 MAY_BE_NULL | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY | MAY_BE_ARRAY_OF_OBJECT),
	F1("str_repeat",                   MAY_BE_NULL | MAY_BE_STRING),
	F1("count_chars",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("chunk_split",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("trim",                         MAY_BE_NULL | MAY_BE_STRING),
	F1("ltrim",                        MAY_BE_NULL | MAY_BE_STRING),
	F1("strip_tags",                   MAY_BE_NULL | MAY_BE_STRING),
	F1("similar_text",                 MAY_BE_NULL | MAY_BE_LONG),
	F1("explode",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("implode",                      MAY_BE_NULL | MAY_BE_STRING),
	F1("join",                         MAY_BE_NULL | MAY_BE_STRING),
	F1("setlocale",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("localeconv",                   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
#if HAVE_NL_LANGINFO
	F1("nl_langinfo",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#endif
	F1("soundex",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("levenshtein",                  MAY_BE_NULL | MAY_BE_LONG),
	F1("chr",                          MAY_BE_NULL | MAY_BE_STRING),
	F1("ord",                          MAY_BE_NULL | MAY_BE_LONG),
	F1("parse_str",                    MAY_BE_NULL),
	F1("str_getcsv",                   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING),
	F1("str_pad",                      MAY_BE_NULL | MAY_BE_STRING),
	F1("chop",                         MAY_BE_NULL | MAY_BE_STRING),
	F1("strchr",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("sprintf",                      MAY_BE_FALSE | MAY_BE_STRING),
	F1("printf",                       MAY_BE_FALSE | MAY_BE_LONG),
	F1("vprintf",                      MAY_BE_FALSE | MAY_BE_LONG),
	F1("vsprintf",                     MAY_BE_FALSE | MAY_BE_STRING),
	F1("fprintf",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("vfprintf",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("sscanf",                       MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	F1("fscanf",                       MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	F1("parse_url",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_LONG),
	F1("urlencode",                    MAY_BE_NULL | MAY_BE_STRING),
	F1("urldecode",                    MAY_BE_NULL | MAY_BE_STRING),
	F1("rawurlencode",                 MAY_BE_NULL | MAY_BE_STRING),
	F1("rawurldecode",                 MAY_BE_NULL | MAY_BE_STRING),
	F1("http_build_query",             MAY_BE_FALSE | MAY_BE_STRING),
#if defined(HAVE_SYMLINK) || defined(PHP_WIN32)
	F1("readlink",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("linkinfo",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("symlink",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("link",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F1("unlink",                       MAY_BE_FALSE | MAY_BE_TRUE),
	F1("exec",                         MAY_BE_FALSE | MAY_BE_STRING),
	F1("system",                       MAY_BE_FALSE | MAY_BE_STRING),
	F1("escapeshellcmd",               MAY_BE_NULL | MAY_BE_STRING),
	F1("escapeshellarg",               MAY_BE_NULL | MAY_BE_STRING),
	F1("passthru",                     MAY_BE_FALSE | MAY_BE_STRING),
	F1("shell_exec",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#ifdef PHP_CAN_SUPPORT_PROC_OPEN
	F1("proc_open",                    MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("proc_close",                   MAY_BE_FALSE | MAY_BE_LONG),
	F1("proc_terminate",               MAY_BE_FALSE | MAY_BE_TRUE),
	F1("proc_get_status",              MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
#endif
#ifdef HAVE_NICE
	F1("proc_nice",                    MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F1("rand",                         MAY_BE_NULL | MAY_BE_LONG),
	F1("srand",                        MAY_BE_NULL),
	F1("getrandmax",                   MAY_BE_NULL | MAY_BE_LONG),
	F1("mt_rand",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mt_srand",                     MAY_BE_NULL),
	F1("mt_getrandmax",                MAY_BE_NULL | MAY_BE_LONG),
#if HAVE_GETSERVBYNAME
	F1("getservbyname",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
#endif
#if HAVE_GETSERVBYPORT
	F1("getservbyport",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#endif
#if HAVE_GETPROTOBYNAME
	F1("getprotobyname",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
#endif
#if HAVE_GETPROTOBYNUMBER
	F1("getprotobynumber",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#endif
	F1("getmyuid",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("getmygid",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("getmypid",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("getmyinode",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("getlastmod",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("base64_decode",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("base64_encode",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("password_hash",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("password_get_info",            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("password_needs_rehash",        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("password_verify",              MAY_BE_FALSE | MAY_BE_TRUE),
	F1("convert_uuencode",             MAY_BE_FALSE | MAY_BE_STRING),
	F1("convert_uudecode",             MAY_BE_FALSE | MAY_BE_STRING),
	F1("abs",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_DOUBLE),
	F1("ceil",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_DOUBLE),
	F1("floor",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_DOUBLE),
	F1("round",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_DOUBLE),
	F1("sin",                          MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("cos",                          MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("tan",                          MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("asin",                         MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("acos",                         MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("atan",                         MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("atanh",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("atan2",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("sinh",                         MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("cosh",                         MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("tanh",                         MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("asinh",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("acosh",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("expm1",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("log1p",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("pi",                           MAY_BE_DOUBLE),
	F1("is_finite",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("is_nan",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("is_infinite",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("pow",                          MAY_BE_NULL | MAY_BE_LONG | MAY_BE_DOUBLE),
	F1("exp",                          MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("log",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_DOUBLE),
	F1("log10",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("sqrt",                         MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("hypot",                        MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("deg2rad",                      MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("rad2deg",                      MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("bindec",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_DOUBLE),
	F1("hexdec",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_DOUBLE),
	F1("octdec",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_DOUBLE),
	F1("decbin",                       MAY_BE_NULL | MAY_BE_STRING),
	F1("decoct",                       MAY_BE_NULL | MAY_BE_STRING),
	FC("dechex",                       zend_dechex_info),
	F1("base_convert",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("number_format",                MAY_BE_NULL | MAY_BE_STRING),
	F1("fmod",                         MAY_BE_NULL | MAY_BE_DOUBLE),
#ifdef HAVE_INET_NTOP
	F1("inet_ntop",                    MAY_BE_FALSE | MAY_BE_STRING),
#endif
#ifdef HAVE_INET_PTON
	F1("inet_pton",                    MAY_BE_FALSE | MAY_BE_STRING),
#endif
	F1("ip2long",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("long2ip",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("getenv",                       MAY_BE_FALSE | MAY_BE_STRING),
#ifdef HAVE_PUTENV
	F1("putenv",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F1("getopt",                       MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
#ifdef HAVE_GETLOADAVG
	F1("sys_getloadavg",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_DOUBLE),
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
	F1("get_current_user",             MAY_BE_NULL | MAY_BE_STRING),
	F1("set_time_limit",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("header_register_callback",     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("get_cfg_var",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("magic_quotes_runtime",         MAY_BE_NULL | MAY_BE_FALSE),
	F1("set_magic_quotes_runtime",     MAY_BE_NULL | MAY_BE_FALSE),
	F1("get_magic_quotes_gpc",         MAY_BE_NULL | MAY_BE_FALSE),
	F1("get_magic_quotes_runtime",     MAY_BE_NULL | MAY_BE_FALSE),
	F1("error_log",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("error_get_last",               MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("call_user_func",               UNKNOWN_INFO),
	F1("call_user_func_array",         UNKNOWN_INFO),
	F1("call_user_method",             UNKNOWN_INFO),
	F1("call_user_method_array",       UNKNOWN_INFO),
	F1("forward_static_call",          UNKNOWN_INFO),
	F1("forward_static_call_array",    UNKNOWN_INFO),
	F1("serialize",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	FN("unserialize",                  UNKNOWN_INFO),
	F1("var_dump",                     MAY_BE_NULL),
	F1("var_export",                   MAY_BE_NULL | MAY_BE_STRING),
	F1("debug_zval_dump",              MAY_BE_NULL),
	F1("print_r",                      MAY_BE_FALSE | MAY_BE_STRING),
	F1("memory_get_usage",             MAY_BE_FALSE | MAY_BE_LONG),
	F1("memory_get_peak_usage",        MAY_BE_FALSE | MAY_BE_LONG),
	F1("register_shutdown_function",   MAY_BE_NULL | MAY_BE_FALSE),
	F1("register_tick_function",       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("unregister_tick_function",     MAY_BE_NULL),
	F1("highlight_file",               MAY_BE_FALSE | MAY_BE_STRING),
	F1("show_source",                  MAY_BE_FALSE | MAY_BE_STRING),
	F1("highlight_string",             MAY_BE_FALSE | MAY_BE_STRING),
	F1("php_strip_whitespace",         MAY_BE_FALSE | MAY_BE_STRING),
	F1("ini_get",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("ini_get_all",                  MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("ini_set",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("ini_alter",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("ini_restore",                  MAY_BE_NULL),
	F1("get_include_path",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("set_include_path",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("restore_include_path",         MAY_BE_NULL),
	F1("setcookie",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("setrawcookie",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("header",                       MAY_BE_NULL),
	F1("header_remove",                MAY_BE_NULL),
	F1("headers_sent",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("headers_list",                 MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("http_response_code",           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("connection_aborted",           MAY_BE_LONG),
	F1("connection_status",            MAY_BE_LONG),
	F1("ignore_user_abort",            MAY_BE_NULL | MAY_BE_LONG),
	F1("parse_ini_file",               MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("parse_ini_string",             MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
#if ZEND_DEBUG
	F1("config_get_hash",              MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
#endif
	F1("is_uploaded_file",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("move_uploaded_file",           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("gethostbyaddr",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("gethostbyname",                MAY_BE_NULL | MAY_BE_STRING),
	F1("gethostbynamel",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
#ifdef HAVE_GETHOSTNAME
	F1("gethostname",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#endif
#if defined(PHP_WIN32) || (HAVE_DNS_SEARCH_FUNC && !(defined(__BEOS__) || defined(NETWARE)))
	F1("dns_check_record",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("checkdnsrr",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
# if defined(PHP_WIN32) || HAVE_FULL_DNS_FUNCS
	F1("dns_get_mx",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("getmxrr",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("dns_get_record",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ARRAY),
# endif
#endif
	F1("intval",                       MAY_BE_NULL | MAY_BE_LONG),
	F1("floatval",                     MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("doubleval",                    MAY_BE_NULL | MAY_BE_DOUBLE),
	F1("strval",                       MAY_BE_NULL | MAY_BE_STRING),
	F1("boolval",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("gettype",                      MAY_BE_NULL | MAY_BE_STRING),
	F1("settype",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	FC("is_null",                      zend_is_type_info),
	F1("is_resource",                  MAY_BE_FALSE | MAY_BE_TRUE), // TODO: inline with support for closed resources
	FC("is_bool",                      zend_is_type_info),
	FC("is_long",                      zend_is_type_info),
	FC("is_float",                     zend_is_type_info),
	FC("is_int",                       zend_is_type_info),
	FC("is_integer",                   zend_is_type_info),
	FC("is_double",                    zend_is_type_info),
	FC("is_real",                      zend_is_type_info),
	F1("is_numeric",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	FC("is_string",                    zend_is_type_info),
	FC("is_array",                     zend_is_type_info),
	F1("is_object",                    MAY_BE_FALSE | MAY_BE_TRUE), // TODO: inline with support for incomplete class
	F1("is_scalar",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("is_callable",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("pclose",                       MAY_BE_FALSE | MAY_BE_LONG),
	F1("popen",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("readfile",                     MAY_BE_FALSE | MAY_BE_LONG),
	F1("rewind",                       MAY_BE_FALSE | MAY_BE_TRUE),
	F1("rmdir",                        MAY_BE_FALSE | MAY_BE_TRUE),
	F1("umask",                        MAY_BE_FALSE | MAY_BE_LONG),
	F1("fclose",                       MAY_BE_FALSE | MAY_BE_TRUE),
	F1("feof",                         MAY_BE_FALSE | MAY_BE_TRUE),
	F1("fgetc",                        MAY_BE_FALSE | MAY_BE_STRING),
	F1("fgets",                        MAY_BE_FALSE | MAY_BE_STRING),
	F1("fgetss",                       MAY_BE_FALSE | MAY_BE_STRING),
	F1("fread",                        MAY_BE_FALSE | MAY_BE_STRING),
	F1("fopen",                        MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("fpassthru",                    MAY_BE_FALSE | MAY_BE_LONG),
	F1("ftruncate",                    MAY_BE_FALSE | MAY_BE_TRUE),
	F1("fstat",                        MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG),
	F1("fseek",                        MAY_BE_FALSE | MAY_BE_LONG),
	F1("ftell",                        MAY_BE_FALSE | MAY_BE_LONG),
	F1("fflush",                       MAY_BE_FALSE | MAY_BE_TRUE),
	F1("fwrite",                       MAY_BE_FALSE | MAY_BE_LONG),
	F1("fputs",                        MAY_BE_FALSE | MAY_BE_LONG),
	F1("mkdir",                        MAY_BE_FALSE | MAY_BE_TRUE),
	F1("rename",                       MAY_BE_FALSE | MAY_BE_TRUE),
	F1("copy",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("tempnam",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("tmpfile",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("file",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("file_get_contents",            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("file_put_contents",            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("stream_select",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("stream_context_create",        MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("stream_context_set_params",    MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("stream_context_get_params",    MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("stream_context_set_option",    MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stream_context_get_options",   MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("stream_context_get_default",   MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("stream_context_set_default",   MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("stream_filter_prepend",        MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("stream_filter_append",         MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("stream_filter_remove",         MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stream_socket_client",         MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("stream_socket_server",         MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("stream_socket_accept",         MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("stream_socket_get_name",       MAY_BE_FALSE | MAY_BE_STRING),
	F1("stream_socket_recvfrom",       MAY_BE_FALSE | MAY_BE_STRING),
	F1("stream_socket_sendto",         MAY_BE_FALSE | MAY_BE_LONG),
	F1("stream_socket_enable_crypto",  MAY_BE_FALSE | MAY_BE_LONG),
#ifdef HAVE_SHUTDOWN
	F1("stream_socket_shutdown",       MAY_BE_FALSE | MAY_BE_TRUE),
#endif
#if HAVE_SOCKETPAIR
	F1("stream_socket_pair",           MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_RESOURCE),
#endif
	F1("stream_copy_to_stream",        MAY_BE_FALSE | MAY_BE_LONG),
	F1("stream_get_contents",          MAY_BE_FALSE | MAY_BE_STRING),
	F1("stream_supports_lock",         MAY_BE_FALSE | MAY_BE_TRUE),
	F1("fgetcsv",                      MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_NULL | MAY_BE_ARRAY_OF_STRING),
	F1("fputcsv",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("flock",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("get_meta_tags",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("stream_set_read_buffer",       MAY_BE_FALSE | MAY_BE_LONG),
	F1("stream_set_write_buffer",      MAY_BE_FALSE | MAY_BE_LONG),
	F1("set_file_buffer",              MAY_BE_FALSE | MAY_BE_LONG),
	F1("stream_set_chunk_size",        MAY_BE_FALSE | MAY_BE_LONG),
	F1("set_socket_blocking",          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stream_set_blocking",          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("socket_set_blocking",          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stream_get_meta_data",         MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("stream_get_line",              MAY_BE_FALSE | MAY_BE_STRING),
	F1("stream_wrapper_register",      MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stream_register_wrapper",      MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stream_wrapper_unregister",    MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stream_wrapper_restore",       MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stream_get_wrappers",          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("stream_get_transports",        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("stream_resolve_include_path",  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("stream_is_local",              MAY_BE_FALSE | MAY_BE_TRUE),
	F1("get_headers",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
#if HAVE_SYS_TIME_H || defined(PHP_WIN32)
	F1("stream_set_timeout",           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("socket_set_timeout",           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F1("socket_get_status",            MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
#if (!defined(__BEOS__) && !defined(NETWARE) && HAVE_REALPATH) || defined(ZTS)
	F1("realpath",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
#endif
#ifdef HAVE_FNMATCH
	F1("fnmatch",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F1("fsockopen",                    MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pfsockopen",                   MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("pack",                         MAY_BE_FALSE | MAY_BE_STRING),
	F1("unpack",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("get_browser",                  MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_OBJECT | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("crypt",                        MAY_BE_NULL | MAY_BE_STRING),
	F1("opendir",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("closedir",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("chdir",                        MAY_BE_FALSE | MAY_BE_TRUE),
#if defined(HAVE_CHROOT) && !defined(ZTS) && ENABLE_CHROOT_FUNC
	F1("chroot",                       MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F1("getcwd",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("rewinddir",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("readdir",                      MAY_BE_FALSE | MAY_BE_STRING),
	F1("dir",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("scandir",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
#ifdef HAVE_GLOB
	F1("glob",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
#endif
	F1("fileatime",                    MAY_BE_NULL | MAY_BE_LONG),
	F1("filectime",                    MAY_BE_NULL | MAY_BE_LONG),
	F1("filegroup",                    MAY_BE_NULL | MAY_BE_LONG),
	F1("fileinode",                    MAY_BE_NULL | MAY_BE_LONG),
	F1("filemtime",                    MAY_BE_NULL | MAY_BE_LONG),
	F1("fileowner",                    MAY_BE_NULL | MAY_BE_LONG),
	F1("fileperms",                    MAY_BE_NULL | MAY_BE_LONG),
	F1("filesize",                     MAY_BE_NULL | MAY_BE_LONG),
	F1("filetype",                     MAY_BE_NULL | MAY_BE_STRING),
	F1("file_exists",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("is_writable",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("is_writeable",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("is_readable",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("is_executable",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("is_file",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("is_dir",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("is_link",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stat",                         MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("lstat",                        MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
#ifndef NETWARE
	F1("chown",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("chgrp",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
#if HAVE_LCHOWN
	F1("lchown",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
#if HAVE_LCHOWN
	F1("lchgrp",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F1("chmod",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#if HAVE_UTIME
	F1("touch",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F1("clearstatcache",               MAY_BE_NULL),
	F1("disk_total_space",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_DOUBLE),
	F1("disk_free_space",              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_DOUBLE),
	F1("diskfreespace",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_DOUBLE),
	F1("realpath_cache_size",          MAY_BE_NULL | MAY_BE_LONG),
	F1("realpath_cache_get",           MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING),
	F1("mail",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("ezmlm_hash",                   MAY_BE_NULL | MAY_BE_LONG),
#ifdef HAVE_SYSLOG_H
	F1("openlog",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("syslog",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("closelog",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
#endif
	F1("lcg_value",                    MAY_BE_DOUBLE),
	F1("metaphone",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("ob_start",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("ob_flush",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("ob_clean",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("ob_end_flush",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("ob_end_clean",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("ob_get_flush",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("ob_get_clean",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("ob_get_length",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("ob_get_level",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("ob_get_status",                MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("ob_get_contents",              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("ob_implicit_flush",            MAY_BE_NULL),
	F1("ob_list_handlers",             MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("ksort",                        MAY_BE_FALSE | MAY_BE_TRUE),
	F1("krsort",                       MAY_BE_FALSE | MAY_BE_TRUE),
	F1("natsort",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("natcasesort",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("asort",                        MAY_BE_FALSE | MAY_BE_TRUE),
	F1("arsort",                       MAY_BE_FALSE | MAY_BE_TRUE),
	F1("sort",                         MAY_BE_FALSE | MAY_BE_TRUE),
	F1("rsort",                        MAY_BE_FALSE | MAY_BE_TRUE),
	F1("usort",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("uasort",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("uksort",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("shuffle",                      MAY_BE_FALSE | MAY_BE_TRUE),
	F1("array_walk",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("array_walk_recursive",         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("count",                        MAY_BE_NULL | MAY_BE_LONG),
	F1("end",                          UNKNOWN_INFO),
	F1("prev",                         UNKNOWN_INFO),
	F1("next",                         UNKNOWN_INFO),
	F1("reset",                        UNKNOWN_INFO),
	F1("current",                      UNKNOWN_INFO),
	F1("key",                          MAY_BE_NULL | MAY_BE_LONG | MAY_BE_STRING),
	F1("min",                          UNKNOWN_INFO),
	F1("max",                          UNKNOWN_INFO),
	F1("in_array",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("array_search",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING),
	F1("extract",                      MAY_BE_NULL | MAY_BE_LONG),
	F1("compact",                      MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_fill",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	F1("array_fill_keys",              MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	FC("range",                        zend_range_info),
	F1("array_multisort",              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("array_push",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("array_pop",                    UNKNOWN_INFO),
	F1("array_shift",                  UNKNOWN_INFO),
	F1("array_unshift",                MAY_BE_NULL | MAY_BE_LONG),
	F1("array_splice",                 MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_slice",                  MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_merge",                  MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_merge_recursive",        MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_replace",                MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_replace_recursive",      MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_keys",                   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("array_values",                 MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_count_values",           MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG),
	F1("array_column",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_reverse",                MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_reduce",                 UNKNOWN_INFO),
	F1("array_pad",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_flip",                   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("array_change_key_case",        MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_rand",                   UNKNOWN_INFO),
	F1("array_unique",                 MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_intersect",              MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_intersect_key",          MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_intersect_ukey",         MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_uintersect",             MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_intersect_assoc",        MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_uintersect_assoc",       MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_intersect_uassoc",       MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_uintersect_uassoc",      MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_diff",                   MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_diff_key",               MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_diff_ukey",              MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_udiff",                  MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_diff_assoc",             MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_udiff_assoc",            MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_diff_uassoc",            MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_udiff_uassoc",           MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_sum",                    MAY_BE_NULL | MAY_BE_LONG | MAY_BE_DOUBLE),
	F1("array_product",                MAY_BE_NULL | MAY_BE_LONG | MAY_BE_DOUBLE),
	F1("array_filter",                 MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_map",                    MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_chunk",                  MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_combine",                MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("array_key_exists",             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("pos",                          UNKNOWN_INFO),
	F1("sizeof",                       MAY_BE_NULL | MAY_BE_LONG),
	F1("key_exists",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("assert",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("assert_options",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_OBJECT | MAY_BE_OBJECT),
	F1("version_compare",              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_LONG),
#if HAVE_FTOK
	F1("ftok",                         MAY_BE_NULL | MAY_BE_LONG),
#endif
	F1("str_rot13",                    MAY_BE_NULL | MAY_BE_STRING),
	F1("stream_get_filters",           MAY_BE_NULL | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("stream_filter_register",       MAY_BE_FALSE | MAY_BE_TRUE),
	F1("stream_bucket_make_writeable", MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("stream_bucket_prepend",        MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("stream_bucket_append",         MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("stream_bucket_new",            MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("output_add_rewrite_var",       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("output_reset_rewrite_vars",    MAY_BE_FALSE),
	F1("sys_get_temp_dir",             MAY_BE_NULL | MAY_BE_STRING),

	/* ext/date */
	F1("strtotime",                             MAY_BE_FALSE | MAY_BE_LONG),
	F1("date",                                  MAY_BE_FALSE | MAY_BE_STRING),
	F1("idate",                                 MAY_BE_FALSE | MAY_BE_LONG),
	F1("gmdate",                                MAY_BE_FALSE | MAY_BE_STRING),
	F1("mktime",                                MAY_BE_FALSE | MAY_BE_LONG),
	F1("gmmktime",                              MAY_BE_FALSE | MAY_BE_LONG),
	F1("checkdate",                             MAY_BE_FALSE | MAY_BE_TRUE),
#ifdef HAVE_STRFTIME
	F1("strftime",                              MAY_BE_FALSE | MAY_BE_STRING),
	F1("gmstrftime",                            MAY_BE_FALSE | MAY_BE_STRING),
#endif
	F1("time",                                  MAY_BE_LONG),
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
	F1("date_modify",                           MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_add",                              MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_sub",                              MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_timezone_get",                     MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_timezone_set",                     MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_offset_get",                       MAY_BE_FALSE | MAY_BE_LONG),
	F1("date_diff",                             MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_time_set",                         MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_date_set",                         MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_isodate_set",                      MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_timestamp_set",                    MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_timestamp_get",                    MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("timezone_open",                         MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("timezone_name_get",                     MAY_BE_FALSE | MAY_BE_STRING),
	F1("timezone_name_from_abbr",               MAY_BE_FALSE | MAY_BE_STRING),
	F1("timezone_offset_get",                   MAY_BE_FALSE | MAY_BE_LONG),
	F1("timezone_transitions_get",              MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("timezone_location_get",                 MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_DOUBLE | MAY_BE_ARRAY_OF_STRING),
	F1("timezone_identifiers_list",             MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("timezone_abbreviations_list",           MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("timezone_version_get",                  MAY_BE_STRING),
	F1("date_interval_create_from_date_string", MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("date_interval_format",                  MAY_BE_FALSE | MAY_BE_STRING),
	F1("date_default_timezone_set",             MAY_BE_FALSE | MAY_BE_TRUE),
	F1("date_default_timezone_get",             MAY_BE_STRING),
	F1("date_sunrise",                          MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_DOUBLE | MAY_BE_STRING),
	F1("date_sunset",                           MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_DOUBLE | MAY_BE_STRING),
	F1("date_sun_info",                         MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_LONG),

	/* ext/preg */
	F1("preg_match",				            MAY_BE_FALSE | MAY_BE_LONG),
	F1("preg_match_all",			            MAY_BE_FALSE | MAY_BE_LONG),
	F1("preg_replace",			                MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING),
	F1("preg_replace_callback",	                MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING),
	F1("preg_filter",				            MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_STRING),
	F1("preg_split",				            MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("preg_quote",				            MAY_BE_NULL | MAY_BE_STRING),
	F1("preg_grep",				                MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_REF | MAY_BE_ARRAY_OF_ANY),
	F1("preg_last_error",			            MAY_BE_NULL | MAY_BE_LONG),

	/* ext/ereg */
	F1("ereg",			                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("ereg_replace",	                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("eregi",			                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("eregi_replace",	                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("split",			                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("spliti",			                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("sql_regcase",		                    MAY_BE_NULL | MAY_BE_STRING),

	/* ext/mysql */
	F1("mysql_connect",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_pconnect",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_close",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_select_db",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_create_db",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_drop_db",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_query",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_RESOURCE),
	F1("mysql_unbuffered_query",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_RESOURCE),
	F1("mysql_db_query",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE | MAY_BE_RESOURCE),
	F1("mysql_list_dbs",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_list_tables",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_list_fields",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_list_processes",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_error",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_errno",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_affected_rows",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_insert_id",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_result",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_num_rows",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_num_fields",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_fetch_row",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_ANY),
	F1("mysql_fetch_array",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("mysql_fetch_assoc",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_ANY),
	F1("mysql_fetch_object",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysql_data_seek",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_fetch_lengths",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("mysql_fetch_field",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_OBJECT),
	F1("mysql_field_seek",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_free_result",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_field_name",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_field_table",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_field_len",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_field_type",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_field_flags",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_escape_string",					MAY_BE_NULL | MAY_BE_STRING),
	F1("mysql_real_escape_string",				MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_stat",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_thread_id",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_client_encoding",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_ping",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_get_client_info",					MAY_BE_NULL | MAY_BE_STRING),
	F1("mysql_get_host_info",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_get_proto_info",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_get_server_info",					MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_info",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_set_charset",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql",									MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_fieldname",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_fieldtable",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_fieldlen",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_fieldtype",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_fieldflags",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_selectdb",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_createdb",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_dropdb",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_freeresult",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mysql_numfields",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_numrows",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mysql_listdbs",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_listtables",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_listfields",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("mysql_db_name",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_dbname",							MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_tablename",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mysql_table_name",						MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),

	/* ext/curl */
	F1("curl_init",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("curl_copy_handle",                      MAY_BE_NULL | MAY_BE_RESOURCE),
	F1("curl_version",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("curl_setopt",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("curl_setopt_array",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("curl_exec",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("curl_getinfo",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_LONG | MAY_BE_DOUBLE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("curl_error",                            MAY_BE_NULL | MAY_BE_STRING),
	F1("curl_errno",                            MAY_BE_NULL | MAY_BE_LONG),
	F1("curl_close",                            MAY_BE_NULL),
	F1("curl_strerror",                         MAY_BE_NULL | MAY_BE_STRING),
	F1("curl_multi_strerror",                   MAY_BE_NULL | MAY_BE_STRING),
	F1("curl_reset",                            MAY_BE_NULL),
	F1("curl_escape",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("curl_unescape",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("curl_pause",                            MAY_BE_NULL | MAY_BE_LONG),
	F1("curl_multi_init",                       MAY_BE_NULL | MAY_BE_RESOURCE),
	F1("curl_multi_add_handle",                 MAY_BE_NULL | MAY_BE_LONG),
	F1("curl_multi_remove_handle",              MAY_BE_NULL | MAY_BE_LONG),
	F1("curl_multi_select",                     MAY_BE_NULL | MAY_BE_LONG),
	F1("curl_multi_exec",                       MAY_BE_NULL | MAY_BE_LONG),
	F1("curl_multi_getcontent",                 MAY_BE_NULL | MAY_BE_STRING),
	F1("curl_multi_info_read",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_RESOURCE),
	F1("curl_multi_close",                      MAY_BE_NULL),
	F1("curl_multi_setopt",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("curl_share_init",                       MAY_BE_NULL | MAY_BE_RESOURCE),
	F1("curl_share_close",                      MAY_BE_NULL),
	F1("curl_share_setopt",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("curl_file_create",                      MAY_BE_OBJECT),

	/* ext/mbstring */
	F1("mb_convert_case",                       MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_strtoupper",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_strtolower",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_language",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_internal_encoding",                  MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_http_input",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_http_output",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_detect_order",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("mb_substitute_character",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING),
	F1("mb_parse_str",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mb_output_handler",                     MAY_BE_NULL | MAY_BE_STRING),
	F1("mb_preferred_mime_name",                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_strlen",                             MAY_BE_FALSE | MAY_BE_LONG),
	F1("mb_strpos",                             MAY_BE_FALSE | MAY_BE_LONG),
	F1("mb_strrpos",                            MAY_BE_FALSE | MAY_BE_LONG),
	F1("mb_stripos",                            MAY_BE_FALSE | MAY_BE_LONG),
	F1("mb_strripos",                           MAY_BE_FALSE | MAY_BE_LONG),
	F1("mb_strstr",                             MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_strrchr",                            MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_stristr",                            MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_strrichr",                           MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_substr_count",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mb_substr",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_strcut",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_strwidth",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mb_strimwidth",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_convert_encoding",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_detect_encoding",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_list_encodings",                     MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("mb_encoding_aliases",                   MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("mb_convert_kana",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_encode_mimeheader",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_decode_mimeheader",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_convert_variables",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_encode_numericentity",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_decode_numericentity",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_send_mail",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mb_get_info",                           MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_LONG | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),
	F1("mb_check_encoding",                     MAY_BE_FALSE | MAY_BE_TRUE),

	F1("mb_regex_encoding",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_regex_set_options",                  MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_ereg",                               MAY_BE_FALSE | MAY_BE_LONG),
	F1("mb_eregi",                              MAY_BE_FALSE | MAY_BE_LONG),
	F1("mb_ereg_replace",                       MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_eregi_replace",                      MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_ereg_replace_callback",              MAY_BE_FALSE | MAY_BE_STRING),
	F1("mb_split",                              MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("mb_ereg_match",                         MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mb_ereg_search",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mb_ereg_search_pos",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("mb_ereg_search_regs",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_STRING),
	F1("mb_ereg_search_init",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mb_ereg_search_getregs",                MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_STRING),
	F1("mb_ereg_search_getpos",                 MAY_BE_LONG),
	F1("mb_ereg_search_setpos",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),

	F1("mbregex_encoding",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mbereg",                                MAY_BE_FALSE | MAY_BE_LONG),
	F1("mberegi",                               MAY_BE_FALSE | MAY_BE_LONG),
	F1("mbereg_replace",                        MAY_BE_FALSE | MAY_BE_STRING),
	F1("mberegi_replace",                       MAY_BE_FALSE | MAY_BE_STRING),
	F1("mbsplit",                               MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("mbereg_match",                          MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mbereg_search",                         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mbereg_search_pos",                     MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_LONG),
	F1("mbereg_search_regs",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_STRING),
	F1("mbereg_search_init",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("mbereg_search_getregs",                 MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_FALSE | MAY_BE_ARRAY_OF_TRUE | MAY_BE_ARRAY_OF_STRING),
	F1("mbereg_search_getpos",                  MAY_BE_LONG),
	F1("mbereg_search_setpos",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),

	/* ext/iconv */
	F1("iconv",                                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("iconv_get_encoding",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING),
	F1("iconv_set_encoding",                    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("iconv_strlen",                          MAY_BE_FALSE | MAY_BE_LONG),
	F1("iconv_substr",                          MAY_BE_FALSE | MAY_BE_STRING),
	F1("iconv_strpos",                          MAY_BE_FALSE | MAY_BE_LONG),
	F1("iconv_strrpos",                         MAY_BE_FALSE | MAY_BE_LONG),
	F1("iconv_mime_encode",                     MAY_BE_FALSE | MAY_BE_STRING),
	F1("iconv_mime_decode",                     MAY_BE_FALSE | MAY_BE_STRING),
	F1("iconv_mime_decode_headers",             MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_STRING | MAY_BE_ARRAY_OF_STRING | MAY_BE_ARRAY_OF_ARRAY),

	/* ext/json */
	F1("json_encode",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("json_decode",                           MAY_BE_ANY | MAY_BE_ARRAY_KEY_ANY | MAY_BE_ARRAY_OF_ANY),
	F1("json_last_error",                       MAY_BE_NULL | MAY_BE_LONG),
	F1("json_last_error_msg",                   MAY_BE_NULL | MAY_BE_STRING),

	/* ext/xml */
	F1("xml_parser_create",                     MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("xml_parser_create_ns",                  MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("xml_set_object",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("xml_set_element_handler",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("xml_set_character_data_handler",        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("xml_set_processing_instruction_handler",MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("xml_set_default_handler",               MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("xml_set_unparsed_entity_decl_handler",  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("xml_set_notation_decl_handler",         MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("xml_set_external_entity_ref_handler",   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("xml_set_start_namespace_decl_handler",  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("xml_set_end_namespace_decl_handler",    MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("xml_parse",                             MAY_BE_NULL | MAY_BE_LONG),
	F1("xml_parse_into_struct",                 MAY_BE_NULL | MAY_BE_LONG),
	F1("xml_get_error_code",                    MAY_BE_NULL | MAY_BE_LONG),
	F1("xml_error_string",                      MAY_BE_NULL | MAY_BE_STRING),
	F1("xml_get_current_line_number",           MAY_BE_NULL | MAY_BE_LONG),
	F1("xml_get_current_column_number",         MAY_BE_NULL | MAY_BE_LONG),
	F1("xml_get_current_byte_index",            MAY_BE_NULL | MAY_BE_LONG),
	F1("xml_parser_free",                       MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("xml_parser_set_option",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("xml_parser_get_option",                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG | MAY_BE_STRING),
	F1("utf8_encode",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("utf8_decode",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),

	/* ext/zlib */
	F1("readgzfile",                            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("gzrewind",                              MAY_BE_FALSE | MAY_BE_TRUE),
	F1("gzclose",                               MAY_BE_FALSE | MAY_BE_TRUE),
	F1("gzeof",                                 MAY_BE_FALSE | MAY_BE_TRUE),
	F1("gzgetc",                                MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzgets",                                MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzgetss",                               MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzread",                                MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzopen",                                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("gzpassthru",                            MAY_BE_FALSE | MAY_BE_LONG),
	F1("gzseek",                                MAY_BE_FALSE | MAY_BE_LONG),
	F1("gztell",                                MAY_BE_FALSE | MAY_BE_LONG),
	F1("gzwrite",                               MAY_BE_FALSE | MAY_BE_LONG),
	F1("gzputs",                                MAY_BE_FALSE | MAY_BE_LONG),
	F1("gzfile",                                MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("gzcompress",                            MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzuncompress",                          MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzdeflate",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzinflate",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzencode",                              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("gzdecode",                              MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("zlib_encode",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("zlib_decode",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("zlib_get_coding_type",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("ob_gzhandler",                          MAY_BE_FALSE | MAY_BE_STRING),

	/* ext/hash */
	F1("hash",                                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("hash_file",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("hash_hmac",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("hash_hmac_file",                        MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("hash_init",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("hash_update",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("hash_update_stream",                    MAY_BE_NULL | MAY_BE_LONG),
	F1("hash_update_file",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_TRUE),
	F1("hash_final",                            MAY_BE_NULL | MAY_BE_STRING),
	F1("hash_copy",                             MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_RESOURCE),
	F1("hash_algos",                            MAY_BE_ARRAY | MAY_BE_ARRAY_KEY_LONG | MAY_BE_ARRAY_OF_STRING),
	F1("hash_pbkdf2",                           MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mhash_keygen_s2k",                      MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mhash_get_block_size",                  MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_LONG),
	F1("mhash_get_hash_name",                   MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
	F1("mhash_count",                           MAY_BE_NULL | MAY_BE_LONG),
	F1("mhash",                                 MAY_BE_NULL | MAY_BE_FALSE | MAY_BE_STRING),
};

static HashTable func_info;
int zend_func_info_rid = -1;

uint32_t zend_get_func_info(const zend_call_info *call_info, const zend_ssa *ssa)
{
	uint32_t ret = 0;

	if (call_info->callee_func->type == ZEND_INTERNAL_FUNCTION) {
		func_info_t *info;

		if ((info = zend_hash_find_ptr(&func_info, Z_STR_P(CRT_CONSTANT_EX(call_info->caller_op_array, call_info->caller_init_opline->op2, ssa->rt_constants)))) != NULL) {
			if (info->info_func) {
				ret = info->info_func(call_info, ssa);
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
	int i;

	if (zend_func_info_rid == -1) {
		zend_func_info_rid = zend_get_resource_handle(&dummy);
		if (zend_func_info_rid < 0) {
			return FAILURE;
		}

		zend_hash_init(&func_info, sizeof(func_infos)/sizeof(func_info_t), NULL, NULL, 1);
		for (i = 0; i < sizeof(func_infos)/sizeof(func_info_t); i++) {
			if (zend_hash_str_add_ptr(&func_info, func_infos[i].name, func_infos[i].name_len, (void**)&func_infos[i]) == NULL) {
				fprintf(stderr, "ERROR: Duplicate function info for \"%s\"\n", func_infos[i].name);
			}
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
