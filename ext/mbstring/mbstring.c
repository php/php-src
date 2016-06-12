/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Tsukada Takuya <tsukada@fminn.nagano.nagano.jp>              |
   |         Rui Hirokawa <hirokawa@php.net>                              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/*
 * PHP 4 Multibyte String module "mbstring"
 *
 * History:
 *   2000.5.19  Release php-4.0RC2_jstring-1.0
 *   2001.4.1   Release php4_jstring-1.0.91
 *   2001.4.30  Release php4_jstring-1.1 (contribute to The PHP Group)
 *   2001.5.1   Renamed from jstring to mbstring (hirokawa@php.net)
 */

/*
 * PHP3 Internationalization support program.
 *
 * Copyright (c) 1999,2000 by the PHP3 internationalization team.
 * All rights reserved.
 *
 * See README_PHP3-i18n-ja for more detail.
 *
 * Authors:
 *    Hironori Sato <satoh@jpnnet.com>
 *    Shigeru Kanemoto <sgk@happysize.co.jp>
 *    Tsukada Takuya <tsukada@fminn.nagano.nagano.jp>
 *    Rui Hirokawa <rui_hirokawa@ybb.ne.jp>
 */

/* {{{ includes */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_variables.h"
#include "mbstring.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_mail.h"
#include "ext/standard/exec.h"
#include "ext/standard/url.h"
#include "main/php_output.h"
#include "ext/standard/info.h"

#include "libmbfl/mbfl/mbfl_allocators.h"
#include "libmbfl/mbfl/mbfilter_pass.h"

#include "php_variables.h"
#include "php_globals.h"
#include "rfc1867.h"
#include "php_content_types.h"
#include "SAPI.h"
#include "php_unicode.h"
#include "TSRM.h"

#include "mb_gpc.h"

#if HAVE_MBREGEX
#include "php_mbregex.h"
#endif

#include "zend_multibyte.h"

#if HAVE_ONIG
#include "php_onig_compat.h"
#include <oniguruma.h>
#undef UChar
#elif HAVE_PCRE || HAVE_BUNDLED_PCRE
#include "ext/pcre/php_pcre.h"
#endif
/* }}} */

#if HAVE_MBSTRING

/* {{{ prototypes */
ZEND_DECLARE_MODULE_GLOBALS(mbstring)

static PHP_GINIT_FUNCTION(mbstring);
static PHP_GSHUTDOWN_FUNCTION(mbstring);

static void php_mb_populate_current_detect_order_list(void);

static int php_mb_encoding_translation(void);

static void php_mb_gpc_get_detect_order(const zend_encoding ***list, size_t *list_size);

static void php_mb_gpc_set_input_encoding(const zend_encoding *encoding);

/* }}} */

/* {{{ php_mb_default_identify_list */
typedef struct _php_mb_nls_ident_list {
	enum mbfl_no_language lang;
	const enum mbfl_no_encoding *list;
	size_t list_size;
} php_mb_nls_ident_list;

static const enum mbfl_no_encoding php_mb_default_identify_list_ja[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_jis,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_euc_jp,
	mbfl_no_encoding_sjis
};

static const enum mbfl_no_encoding php_mb_default_identify_list_cn[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_euc_cn,
	mbfl_no_encoding_cp936
};

static const enum mbfl_no_encoding php_mb_default_identify_list_tw_hk[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_euc_tw,
	mbfl_no_encoding_big5
};

static const enum mbfl_no_encoding php_mb_default_identify_list_kr[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_euc_kr,
	mbfl_no_encoding_uhc
};

static const enum mbfl_no_encoding php_mb_default_identify_list_ru[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_koi8r,
	mbfl_no_encoding_cp1251,
	mbfl_no_encoding_cp866
};

static const enum mbfl_no_encoding php_mb_default_identify_list_hy[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_armscii8
};

static const enum mbfl_no_encoding php_mb_default_identify_list_tr[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_cp1254,
	mbfl_no_encoding_8859_9
};

static const enum mbfl_no_encoding php_mb_default_identify_list_ua[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_koi8u
};

static const enum mbfl_no_encoding php_mb_default_identify_list_neut[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_utf8
};


static const php_mb_nls_ident_list php_mb_default_identify_list[] = {
	{ mbfl_no_language_japanese, php_mb_default_identify_list_ja, sizeof(php_mb_default_identify_list_ja) / sizeof(php_mb_default_identify_list_ja[0]) },
	{ mbfl_no_language_korean, php_mb_default_identify_list_kr, sizeof(php_mb_default_identify_list_kr) / sizeof(php_mb_default_identify_list_kr[0]) },
	{ mbfl_no_language_traditional_chinese, php_mb_default_identify_list_tw_hk, sizeof(php_mb_default_identify_list_tw_hk) / sizeof(php_mb_default_identify_list_tw_hk[0]) },
	{ mbfl_no_language_simplified_chinese, php_mb_default_identify_list_cn, sizeof(php_mb_default_identify_list_cn) / sizeof(php_mb_default_identify_list_cn[0]) },
	{ mbfl_no_language_russian, php_mb_default_identify_list_ru, sizeof(php_mb_default_identify_list_ru) / sizeof(php_mb_default_identify_list_ru[0]) },
	{ mbfl_no_language_armenian, php_mb_default_identify_list_hy, sizeof(php_mb_default_identify_list_hy) / sizeof(php_mb_default_identify_list_hy[0]) },
	{ mbfl_no_language_turkish, php_mb_default_identify_list_tr, sizeof(php_mb_default_identify_list_tr) / sizeof(php_mb_default_identify_list_tr[0]) },
	{ mbfl_no_language_ukrainian, php_mb_default_identify_list_ua, sizeof(php_mb_default_identify_list_ua) / sizeof(php_mb_default_identify_list_ua[0]) },
	{ mbfl_no_language_neutral, php_mb_default_identify_list_neut, sizeof(php_mb_default_identify_list_neut) / sizeof(php_mb_default_identify_list_neut[0]) }
};

/* }}} */

/* {{{ mb_overload_def mb_ovld[] */
static const struct mb_overload_def mb_ovld[] = {
	{MB_OVERLOAD_MAIL, "mail", "mb_send_mail", "mb_orig_mail"},
	{MB_OVERLOAD_STRING, "strlen", "mb_strlen", "mb_orig_strlen"},
	{MB_OVERLOAD_STRING, "strpos", "mb_strpos", "mb_orig_strpos"},
	{MB_OVERLOAD_STRING, "strrpos", "mb_strrpos", "mb_orig_strrpos"},
	{MB_OVERLOAD_STRING, "stripos", "mb_stripos", "mb_orig_stripos"},
	{MB_OVERLOAD_STRING, "strripos", "mb_strripos", "mb_orig_strripos"},
	{MB_OVERLOAD_STRING, "strstr", "mb_strstr", "mb_orig_strstr"},
	{MB_OVERLOAD_STRING, "strrchr", "mb_strrchr", "mb_orig_strrchr"},
	{MB_OVERLOAD_STRING, "stristr", "mb_stristr", "mb_orig_stristr"},
	{MB_OVERLOAD_STRING, "substr", "mb_substr", "mb_orig_substr"},
	{MB_OVERLOAD_STRING, "strtolower", "mb_strtolower", "mb_orig_strtolower"},
	{MB_OVERLOAD_STRING, "strtoupper", "mb_strtoupper", "mb_orig_strtoupper"},
	{MB_OVERLOAD_STRING, "substr_count", "mb_substr_count", "mb_orig_substr_count"},
	{0, NULL, NULL, NULL}
};
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_language, 0, 0, 0)
	ZEND_ARG_INFO(0, language)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_internal_encoding, 0, 0, 0)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_http_input, 0, 0, 0)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_http_output, 0, 0, 0)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_detect_order, 0, 0, 0)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_substitute_character, 0, 0, 0)
	ZEND_ARG_INFO(0, substchar)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_preferred_mime_name, 0, 0, 1)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_parse_str, 0, 0, 1)
	ZEND_ARG_INFO(0, encoded_string)
	ZEND_ARG_INFO(1, result)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_output_handler, 0, 0, 2)
	ZEND_ARG_INFO(0, contents)
	ZEND_ARG_INFO(0, status)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_strlen, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_strpos, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_strrpos, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_stripos, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_strripos, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, offset)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_strstr, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, part)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_strrchr, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, part)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_stristr, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, part)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_strrichr, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, part)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_substr_count, 0, 0, 2)
	ZEND_ARG_INFO(0, haystack)
	ZEND_ARG_INFO(0, needle)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_substr, 0, 0, 2)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_strcut, 0, 0, 2)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, length)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_strwidth, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_strimwidth, 0, 0, 3)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, width)
	ZEND_ARG_INFO(0, trimmarker)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_convert_encoding, 0, 0, 2)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, to)
	ZEND_ARG_INFO(0, from)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_convert_case, 0, 0, 2)
	ZEND_ARG_INFO(0, sourcestring)
	ZEND_ARG_INFO(0, mode)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_strtoupper, 0, 0, 1)
	ZEND_ARG_INFO(0, sourcestring)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_strtolower, 0, 0, 1)
	ZEND_ARG_INFO(0, sourcestring)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_detect_encoding, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, encoding_list)
	ZEND_ARG_INFO(0, strict)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_mb_list_encodings, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_encoding_aliases, 0, 0, 1)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_encode_mimeheader, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, charset)
	ZEND_ARG_INFO(0, transfer)
	ZEND_ARG_INFO(0, linefeed)
	ZEND_ARG_INFO(0, indent)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_decode_mimeheader, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_convert_kana, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, option)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_convert_variables, 0, 0, 3)
	ZEND_ARG_INFO(0, to)
	ZEND_ARG_INFO(0, from)
	ZEND_ARG_VARIADIC_INFO(1, vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_encode_numericentity, 0, 0, 2)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, convmap)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_INFO(0, is_hex)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_decode_numericentity, 0, 0, 2)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, convmap)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_send_mail, 0, 0, 3)
	ZEND_ARG_INFO(0, to)
	ZEND_ARG_INFO(0, subject)
	ZEND_ARG_INFO(0, message)
	ZEND_ARG_INFO(0, additional_headers)
	ZEND_ARG_INFO(0, additional_parameters)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_get_info, 0, 0, 0)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_check_encoding, 0, 0, 0)
	ZEND_ARG_INFO(0, var)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_regex_encoding, 0, 0, 0)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_ereg, 0, 0, 2)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(1, registers)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_eregi, 0, 0, 2)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(1, registers)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_ereg_replace, 0, 0, 3)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, replacement)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_eregi_replace, 0, 0, 3)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, replacement)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_ereg_replace_callback, 0, 0, 3)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_split, 0, 0, 2)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, limit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_ereg_match, 0, 0, 2)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_ereg_search, 0, 0, 0)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_ereg_search_pos, 0, 0, 0)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_ereg_search_regs, 0, 0, 0)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_ereg_search_init, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_mb_ereg_search_getregs, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_mb_ereg_search_getpos, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_ereg_search_setpos, 0, 0, 1)
	ZEND_ARG_INFO(0, position)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_regex_set_options, 0, 0, 0)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ zend_function_entry mbstring_functions[] */
const zend_function_entry mbstring_functions[] = {
	PHP_FE(mb_convert_case,			arginfo_mb_convert_case)
	PHP_FE(mb_strtoupper,			arginfo_mb_strtoupper)
	PHP_FE(mb_strtolower,			arginfo_mb_strtolower)
	PHP_FE(mb_language,				arginfo_mb_language)
	PHP_FE(mb_internal_encoding,	arginfo_mb_internal_encoding)
	PHP_FE(mb_http_input,			arginfo_mb_http_input)
	PHP_FE(mb_http_output,			arginfo_mb_http_output)
	PHP_FE(mb_detect_order,			arginfo_mb_detect_order)
	PHP_FE(mb_substitute_character,	arginfo_mb_substitute_character)
	PHP_FE(mb_parse_str,			arginfo_mb_parse_str)
	PHP_FE(mb_output_handler,		arginfo_mb_output_handler)
	PHP_FE(mb_preferred_mime_name,	arginfo_mb_preferred_mime_name)
	PHP_FE(mb_strlen,				arginfo_mb_strlen)
	PHP_FE(mb_strpos,				arginfo_mb_strpos)
	PHP_FE(mb_strrpos,				arginfo_mb_strrpos)
	PHP_FE(mb_stripos,				arginfo_mb_stripos)
	PHP_FE(mb_strripos,				arginfo_mb_strripos)
	PHP_FE(mb_strstr,				arginfo_mb_strstr)
	PHP_FE(mb_strrchr,				arginfo_mb_strrchr)
	PHP_FE(mb_stristr,				arginfo_mb_stristr)
	PHP_FE(mb_strrichr,				arginfo_mb_strrichr)
	PHP_FE(mb_substr_count,			arginfo_mb_substr_count)
	PHP_FE(mb_substr,				arginfo_mb_substr)
	PHP_FE(mb_strcut,				arginfo_mb_strcut)
	PHP_FE(mb_strwidth,				arginfo_mb_strwidth)
	PHP_FE(mb_strimwidth,			arginfo_mb_strimwidth)
	PHP_FE(mb_convert_encoding,		arginfo_mb_convert_encoding)
	PHP_FE(mb_detect_encoding,		arginfo_mb_detect_encoding)
	PHP_FE(mb_list_encodings,		arginfo_mb_list_encodings)
	PHP_FE(mb_encoding_aliases,		arginfo_mb_encoding_aliases)
	PHP_FE(mb_convert_kana,			arginfo_mb_convert_kana)
	PHP_FE(mb_encode_mimeheader,	arginfo_mb_encode_mimeheader)
	PHP_FE(mb_decode_mimeheader,	arginfo_mb_decode_mimeheader)
	PHP_FE(mb_convert_variables,	arginfo_mb_convert_variables)
	PHP_FE(mb_encode_numericentity,	arginfo_mb_encode_numericentity)
	PHP_FE(mb_decode_numericentity,	arginfo_mb_decode_numericentity)
	PHP_FE(mb_send_mail,			arginfo_mb_send_mail)
	PHP_FE(mb_get_info,				arginfo_mb_get_info)
	PHP_FE(mb_check_encoding,		arginfo_mb_check_encoding)
#if HAVE_MBREGEX
	PHP_MBREGEX_FUNCTION_ENTRIES
#endif
	PHP_FE_END
};
/* }}} */

/* {{{ zend_module_entry mbstring_module_entry */
zend_module_entry mbstring_module_entry = {
	STANDARD_MODULE_HEADER,
	"mbstring",
	mbstring_functions,
	PHP_MINIT(mbstring),
	PHP_MSHUTDOWN(mbstring),
	PHP_RINIT(mbstring),
	PHP_RSHUTDOWN(mbstring),
	PHP_MINFO(mbstring),
	PHP_MBSTRING_VERSION,
	PHP_MODULE_GLOBALS(mbstring),
	PHP_GINIT(mbstring),
	PHP_GSHUTDOWN(mbstring),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

/* {{{ static sapi_post_entry php_post_entries[] */
static sapi_post_entry php_post_entries[] = {
	{ DEFAULT_POST_CONTENT_TYPE, sizeof(DEFAULT_POST_CONTENT_TYPE)-1, sapi_read_standard_form_data,	php_std_post_handler },
	{ MULTIPART_CONTENT_TYPE,    sizeof(MULTIPART_CONTENT_TYPE)-1,    NULL,                         rfc1867_post_handler },
	{ NULL, 0, NULL, NULL }
};
/* }}} */

#ifdef COMPILE_DL_MBSTRING
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(mbstring)
#endif

static char *get_internal_encoding(void) {
	if (PG(internal_encoding) && PG(internal_encoding)[0]) {
		return PG(internal_encoding);
	} else if (SG(default_charset)) {
		return SG(default_charset);
	}
	return "";
}

static char *get_input_encoding(void) {
	if (PG(input_encoding) && PG(input_encoding)[0]) {
		return PG(input_encoding);
	} else if (SG(default_charset)) {
		return SG(default_charset);
	}
	return "";
}

static char *get_output_encoding(void) {
	if (PG(output_encoding) && PG(output_encoding)[0]) {
		return PG(output_encoding);
	} else if (SG(default_charset)) {
		return SG(default_charset);
	}
	return "";
}


/* {{{ allocators */
static void *_php_mb_allocators_malloc(unsigned int sz)
{
	return emalloc(sz);
}

static void *_php_mb_allocators_realloc(void *ptr, unsigned int sz)
{
	return erealloc(ptr, sz);
}

static void *_php_mb_allocators_calloc(unsigned int nelems, unsigned int szelem)
{
	return ecalloc(nelems, szelem);
}

static void _php_mb_allocators_free(void *ptr)
{
	efree(ptr);
}

static void *_php_mb_allocators_pmalloc(unsigned int sz)
{
	return pemalloc(sz, 1);
}

static void *_php_mb_allocators_prealloc(void *ptr, unsigned int sz)
{
	return perealloc(ptr, sz, 1);
}

static void _php_mb_allocators_pfree(void *ptr)
{
	pefree(ptr, 1);
}

static mbfl_allocators _php_mb_allocators = {
	_php_mb_allocators_malloc,
	_php_mb_allocators_realloc,
	_php_mb_allocators_calloc,
	_php_mb_allocators_free,
	_php_mb_allocators_pmalloc,
	_php_mb_allocators_prealloc,
	_php_mb_allocators_pfree
};
/* }}} */

/* {{{ static sapi_post_entry mbstr_post_entries[] */
static sapi_post_entry mbstr_post_entries[] = {
	{ DEFAULT_POST_CONTENT_TYPE, sizeof(DEFAULT_POST_CONTENT_TYPE)-1, sapi_read_standard_form_data, php_mb_post_handler },
	{ MULTIPART_CONTENT_TYPE,    sizeof(MULTIPART_CONTENT_TYPE)-1,    NULL,                         rfc1867_post_handler },
	{ NULL, 0, NULL, NULL }
};
/* }}} */

/* {{{ static int php_mb_parse_encoding_list()
 *  Return 0 if input contains any illegal encoding, otherwise 1.
 *  Even if any illegal encoding is detected the result may contain a list
 *  of parsed encodings.
 */
static int
php_mb_parse_encoding_list(const char *value, size_t value_length, const mbfl_encoding ***return_list, size_t *return_size, int persistent)
{
	int size, bauto, ret = SUCCESS;
	size_t n;
	char *p, *p1, *p2, *endp, *tmpstr;
	const mbfl_encoding **entry, **list;

	list = NULL;
	if (value == NULL || value_length <= 0) {
		if (return_list) {
			*return_list = NULL;
		}
		if (return_size) {
			*return_size = 0;
		}
		return FAILURE;
	} else {
		/* copy the value string for work */
		if (value[0]=='"' && value[value_length-1]=='"' && value_length>2) {
			tmpstr = (char *)estrndup(value+1, value_length-2);
			value_length -= 2;
		}
		else
			tmpstr = (char *)estrndup(value, value_length);
		if (tmpstr == NULL) {
			return FAILURE;
		}
		/* count the number of listed encoding names */
		endp = tmpstr + value_length;
		n = 1;
		p1 = tmpstr;
		while ((p2 = (char*)php_memnstr(p1, ",", 1, endp)) != NULL) {
			p1 = p2 + 1;
			n++;
		}
		size = n + MBSTRG(default_detect_order_list_size);
		/* make list */
		list = (const mbfl_encoding **)pecalloc(size, sizeof(mbfl_encoding*), persistent);
		if (list != NULL) {
			entry = list;
			n = 0;
			bauto = 0;
			p1 = tmpstr;
			do {
				p2 = p = (char*)php_memnstr(p1, ",", 1, endp);
				if (p == NULL) {
					p = endp;
				}
				*p = '\0';
				/* trim spaces */
				while (p1 < p && (*p1 == ' ' || *p1 == '\t')) {
					p1++;
				}
				p--;
				while (p > p1 && (*p == ' ' || *p == '\t')) {
					*p = '\0';
					p--;
				}
				/* convert to the encoding number and check encoding */
				if (strcasecmp(p1, "auto") == 0) {
					if (!bauto) {
						const enum mbfl_no_encoding *src = MBSTRG(default_detect_order_list);
						const size_t identify_list_size = MBSTRG(default_detect_order_list_size);
						size_t i;
						bauto = 1;
						for (i = 0; i < identify_list_size; i++) {
							*entry++ = mbfl_no2encoding(*src++);
							n++;
						}
					}
				} else {
					const mbfl_encoding *encoding = mbfl_name2encoding(p1);
					if (encoding) {
						*entry++ = encoding;
						n++;
					} else {
						ret = 0;
					}
				}
				p1 = p2 + 1;
			} while (n < size && p2 != NULL);
			if (n > 0) {
				if (return_list) {
					*return_list = list;
				} else {
					pefree(list, persistent);
				}
			} else {
				pefree(list, persistent);
				if (return_list) {
					*return_list = NULL;
				}
				ret = 0;
			}
			if (return_size) {
				*return_size = n;
			}
		} else {
			if (return_list) {
				*return_list = NULL;
			}
			if (return_size) {
				*return_size = 0;
			}
			ret = 0;
		}
		efree(tmpstr);
	}

	return ret;
}
/* }}} */

/* {{{ static int php_mb_parse_encoding_array()
 *  Return 0 if input contains any illegal encoding, otherwise 1.
 *  Even if any illegal encoding is detected the result may contain a list
 *  of parsed encodings.
 */
static int
php_mb_parse_encoding_array(zval *array, const mbfl_encoding ***return_list, size_t *return_size, int persistent)
{
	zval *hash_entry;
	HashTable *target_hash;
	int i, n, size, bauto, ret = SUCCESS;
	const mbfl_encoding **list, **entry;

	list = NULL;
	if (Z_TYPE_P(array) == IS_ARRAY) {
		target_hash = Z_ARRVAL_P(array);
		i = zend_hash_num_elements(target_hash);
		size = i + MBSTRG(default_detect_order_list_size);
		list = (const mbfl_encoding **)pecalloc(size, sizeof(mbfl_encoding*), persistent);
		if (list != NULL) {
			entry = list;
			bauto = 0;
			n = 0;
			ZEND_HASH_FOREACH_VAL(target_hash, hash_entry) {
				convert_to_string_ex(hash_entry);
				if (strcasecmp(Z_STRVAL_P(hash_entry), "auto") == 0) {
					if (!bauto) {
						const enum mbfl_no_encoding *src = MBSTRG(default_detect_order_list);
						const size_t identify_list_size = MBSTRG(default_detect_order_list_size);
						size_t j;

						bauto = 1;
						for (j = 0; j < identify_list_size; j++) {
							*entry++ = mbfl_no2encoding(*src++);
							n++;
						}
					}
				} else {
					const mbfl_encoding *encoding = mbfl_name2encoding(Z_STRVAL_P(hash_entry));
					if (encoding) {
						*entry++ = encoding;
						n++;
					} else {
						ret = FAILURE;
					}
				}
				i--;
			} ZEND_HASH_FOREACH_END();
			if (n > 0) {
				if (return_list) {
					*return_list = list;
				} else {
					pefree(list, persistent);
				}
			} else {
				pefree(list, persistent);
				if (return_list) {
					*return_list = NULL;
				}
				ret = FAILURE;
			}
			if (return_size) {
				*return_size = n;
			}
		} else {
			if (return_list) {
				*return_list = NULL;
			}
			if (return_size) {
				*return_size = 0;
			}
			ret = FAILURE;
		}
	}

	return ret;
}
/* }}} */

/* {{{ zend_multibyte interface */
static const zend_encoding* php_mb_zend_encoding_fetcher(const char *encoding_name)
{
	return (const zend_encoding*)mbfl_name2encoding(encoding_name);
}

static const char *php_mb_zend_encoding_name_getter(const zend_encoding *encoding)
{
	return ((const mbfl_encoding *)encoding)->name;
}

static int php_mb_zend_encoding_lexer_compatibility_checker(const zend_encoding *_encoding)
{
	const mbfl_encoding *encoding = (const mbfl_encoding*)_encoding;
	if (encoding->flag & MBFL_ENCTYPE_SBCS) {
		return 1;
	}
	if ((encoding->flag & (MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE)) == MBFL_ENCTYPE_MBCS) {
		return 1;
	}
	return 0;
}

static const zend_encoding *php_mb_zend_encoding_detector(const unsigned char *arg_string, size_t arg_length, const zend_encoding **list, size_t list_size)
{
	mbfl_string string;

	if (!list) {
		list = (const zend_encoding **)MBSTRG(current_detect_order_list);
		list_size = MBSTRG(current_detect_order_list_size);
	}

	mbfl_string_init(&string);
	string.no_language = MBSTRG(language);
	string.val = (unsigned char *)arg_string;
	string.len = arg_length;
	return (const zend_encoding *) mbfl_identify_encoding2(&string, (const mbfl_encoding **)list, list_size, 0);
}

static size_t php_mb_zend_encoding_converter(unsigned char **to, size_t *to_length, const unsigned char *from, size_t from_length, const zend_encoding *encoding_to, const zend_encoding *encoding_from)
{
	mbfl_string string, result;
	mbfl_buffer_converter *convd;
	int status, loc;

	/* new encoding */
	/* initialize string */
	mbfl_string_init(&string);
	mbfl_string_init(&result);
	string.no_encoding = ((const mbfl_encoding*)encoding_from)->no_encoding;
	string.no_language = MBSTRG(language);
	string.val = (unsigned char*)from;
	string.len = from_length;

	/* initialize converter */
	convd = mbfl_buffer_converter_new2((const mbfl_encoding *)encoding_from, (const mbfl_encoding *)encoding_to, string.len);
	if (convd == NULL) {
		return -1;
	}
	mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode));
	mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar));

	/* do it */
	status = mbfl_buffer_converter_feed2(convd, &string, &loc);
	if (status) {
		mbfl_buffer_converter_delete(convd);
		return (size_t)-1;
	}

	mbfl_buffer_converter_flush(convd);
	if (!mbfl_buffer_converter_result(convd, &result)) {
		mbfl_buffer_converter_delete(convd);
		return (size_t)-1;
	}

	*to = result.val;
	*to_length = result.len;

	mbfl_buffer_converter_delete(convd);

	return loc;
}

static int php_mb_zend_encoding_list_parser(const char *encoding_list, size_t encoding_list_len, const zend_encoding ***return_list, size_t *return_size, int persistent)
{
	return php_mb_parse_encoding_list(encoding_list, encoding_list_len, (const mbfl_encoding ***)return_list, return_size, persistent);
}

static const zend_encoding *php_mb_zend_internal_encoding_getter(void)
{
	return (const zend_encoding *)MBSTRG(internal_encoding);
}

static int php_mb_zend_internal_encoding_setter(const zend_encoding *encoding)
{
	MBSTRG(internal_encoding) = (const mbfl_encoding *)encoding;
	return SUCCESS;
}

static zend_multibyte_functions php_mb_zend_multibyte_functions = {
	"mbstring",
	php_mb_zend_encoding_fetcher,
	php_mb_zend_encoding_name_getter,
	php_mb_zend_encoding_lexer_compatibility_checker,
	php_mb_zend_encoding_detector,
	php_mb_zend_encoding_converter,
	php_mb_zend_encoding_list_parser,
	php_mb_zend_internal_encoding_getter,
	php_mb_zend_internal_encoding_setter
};
/* }}} */

static void *_php_mb_compile_regex(const char *pattern);
static int _php_mb_match_regex(void *opaque, const char *str, size_t str_len);
static void _php_mb_free_regex(void *opaque);

#if HAVE_ONIG
/* {{{ _php_mb_compile_regex */
static void *_php_mb_compile_regex(const char *pattern)
{
	php_mb_regex_t *retval;
	OnigErrorInfo err_info;
	int err_code;

	if ((err_code = onig_new(&retval,
			(const OnigUChar *)pattern,
			(const OnigUChar *)pattern + strlen(pattern),
			ONIG_OPTION_IGNORECASE | ONIG_OPTION_DONT_CAPTURE_GROUP,
			ONIG_ENCODING_ASCII, &OnigSyntaxPerl, &err_info))) {
		OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];
		onig_error_code_to_str(err_str, err_code, err_info);
		php_error_docref(NULL, E_WARNING, "%s: %s", pattern, err_str);
		retval = NULL;
	}
	return retval;
}
/* }}} */

/* {{{ _php_mb_match_regex */
static int _php_mb_match_regex(void *opaque, const char *str, size_t str_len)
{
	return onig_search((php_mb_regex_t *)opaque, (const OnigUChar *)str,
			(const OnigUChar*)str + str_len, (const OnigUChar *)str,
			(const OnigUChar*)str + str_len, NULL, ONIG_OPTION_NONE) >= 0;
}
/* }}} */

/* {{{ _php_mb_free_regex */
static void _php_mb_free_regex(void *opaque)
{
	onig_free((php_mb_regex_t *)opaque);
}
/* }}} */
#elif HAVE_PCRE || HAVE_BUNDLED_PCRE
/* {{{ _php_mb_compile_regex */
static void *_php_mb_compile_regex(const char *pattern)
{
	pcre *retval;
	const char *err_str;
	int err_offset;

	if (!(retval = pcre_compile(pattern,
			PCRE_CASELESS, &err_str, &err_offset, NULL))) {
		php_error_docref(NULL, E_WARNING, "%s (offset=%d): %s", pattern, err_offset, err_str);
	}
	return retval;
}
/* }}} */

/* {{{ _php_mb_match_regex */
static int _php_mb_match_regex(void *opaque, const char *str, size_t str_len)
{
	return pcre_exec((pcre *)opaque, NULL, str, (int)str_len, 0,
			0, NULL, 0) >= 0;
}
/* }}} */

/* {{{ _php_mb_free_regex */
static void _php_mb_free_regex(void *opaque)
{
	pcre_free(opaque);
}
/* }}} */
#endif

/* {{{ php_mb_nls_get_default_detect_order_list */
static int php_mb_nls_get_default_detect_order_list(enum mbfl_no_language lang, enum mbfl_no_encoding **plist, size_t *plist_size)
{
	size_t i;

	*plist = (enum mbfl_no_encoding *) php_mb_default_identify_list_neut;
	*plist_size = sizeof(php_mb_default_identify_list_neut) / sizeof(php_mb_default_identify_list_neut[0]);

	for (i = 0; i < sizeof(php_mb_default_identify_list) / sizeof(php_mb_default_identify_list[0]); i++) {
		if (php_mb_default_identify_list[i].lang == lang) {
			*plist = (enum mbfl_no_encoding *)php_mb_default_identify_list[i].list;
			*plist_size = php_mb_default_identify_list[i].list_size;
			return 1;
		}
	}
	return 0;
}
/* }}} */

static char *php_mb_rfc1867_substring_conf(const zend_encoding *encoding, char *start, int len, char quote)
{
	char *result = emalloc(len + 2);
	char *resp = result;
	int i;

	for (i = 0; i < len && start[i] != quote; ++i) {
		if (start[i] == '\\' && (start[i + 1] == '\\' || (quote && start[i + 1] == quote))) {
			*resp++ = start[++i];
		} else {
			size_t j = php_mb_mbchar_bytes_ex(start+i, (const mbfl_encoding *)encoding);

			while (j-- > 0 && i < len) {
				*resp++ = start[i++];
			}
			--i;
		}
	}

	*resp = '\0';
	return result;
}

static char *php_mb_rfc1867_getword(const zend_encoding *encoding, char **line, char stop) /* {{{ */
{
	char *pos = *line, quote;
	char *res;

	while (*pos && *pos != stop) {
		if ((quote = *pos) == '"' || quote == '\'') {
			++pos;
			while (*pos && *pos != quote) {
				if (*pos == '\\' && pos[1] && pos[1] == quote) {
					pos += 2;
				} else {
					++pos;
				}
			}
			if (*pos) {
				++pos;
			}
		} else {
			pos += php_mb_mbchar_bytes_ex(pos, (const mbfl_encoding *)encoding);

		}
	}
	if (*pos == '\0') {
		res = estrdup(*line);
		*line += strlen(*line);
		return res;
	}

	res = estrndup(*line, pos - *line);

	while (*pos == stop) {
		pos += php_mb_mbchar_bytes_ex(pos, (const mbfl_encoding *)encoding);
	}

	*line = pos;
	return res;
}
/* }}} */

static char *php_mb_rfc1867_getword_conf(const zend_encoding *encoding, char *str) /* {{{ */
{
	while (*str && isspace(*(unsigned char *)str)) {
		++str;
	}

	if (!*str) {
		return estrdup("");
	}

	if (*str == '"' || *str == '\'') {
		char quote = *str;

		str++;
		return php_mb_rfc1867_substring_conf(encoding, str, strlen(str), quote);
	} else {
		char *strend = str;

		while (*strend && !isspace(*(unsigned char *)strend)) {
			++strend;
		}
		return php_mb_rfc1867_substring_conf(encoding, str, strend - str, 0);
	}
}
/* }}} */

static char *php_mb_rfc1867_basename(const zend_encoding *encoding, char *filename) /* {{{ */
{
	char *s, *s2;
	const size_t filename_len = strlen(filename);

	/* The \ check should technically be needed for win32 systems only where
	 * it is a valid path separator. However, IE in all it's wisdom always sends
	 * the full path of the file on the user's filesystem, which means that unless
	 * the user does basename() they get a bogus file name. Until IE's user base drops
	 * to nill or problem is fixed this code must remain enabled for all systems. */
	s = php_mb_safe_strrchr_ex(filename, '\\', filename_len, (const mbfl_encoding *)encoding);
	s2 = php_mb_safe_strrchr_ex(filename, '/', filename_len, (const mbfl_encoding *)encoding);

	if (s && s2) {
		if (s > s2) {
			return ++s;
		} else {
			return ++s2;
		}
	} else if (s) {
		return ++s;
	} else if (s2) {
		return ++s2;
	} else {
		return filename;
	}
}
/* }}} */

/* {{{ php.ini directive handler */
/* {{{ static PHP_INI_MH(OnUpdate_mbstring_language) */
static PHP_INI_MH(OnUpdate_mbstring_language)
{
	enum mbfl_no_language no_language;

	no_language = mbfl_name2no_language(ZSTR_VAL(new_value));
	if (no_language == mbfl_no_language_invalid) {
		MBSTRG(language) = mbfl_no_language_neutral;
		return FAILURE;
	}
	MBSTRG(language) = no_language;
	php_mb_nls_get_default_detect_order_list(no_language, &MBSTRG(default_detect_order_list), &MBSTRG(default_detect_order_list_size));
	return SUCCESS;
}
/* }}} */

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_detect_order) */
static PHP_INI_MH(OnUpdate_mbstring_detect_order)
{
	const mbfl_encoding **list;
	size_t size;

	if (!new_value) {
		if (MBSTRG(detect_order_list)) {
			pefree(MBSTRG(detect_order_list), 1);
		}
		MBSTRG(detect_order_list) = NULL;
		MBSTRG(detect_order_list_size) = 0;
		return SUCCESS;
	}

	if (FAILURE == php_mb_parse_encoding_list(ZSTR_VAL(new_value), ZSTR_LEN(new_value), &list, &size, 1)) {
		return FAILURE;
	}

	if (MBSTRG(detect_order_list)) {
		pefree(MBSTRG(detect_order_list), 1);
	}
	MBSTRG(detect_order_list) = list;
	MBSTRG(detect_order_list_size) = size;
	return SUCCESS;
}
/* }}} */

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_http_input) */
static PHP_INI_MH(OnUpdate_mbstring_http_input)
{
	const mbfl_encoding **list;
	size_t size;

	if (!new_value) {
		if (MBSTRG(http_input_list)) {
			pefree(MBSTRG(http_input_list), 1);
		}
		if (SUCCESS == php_mb_parse_encoding_list(get_input_encoding(), strlen(get_input_encoding())+1, &list, &size, 1)) {
			MBSTRG(http_input_list) = list;
			MBSTRG(http_input_list_size) = size;
			return SUCCESS;
		}
		MBSTRG(http_input_list) = NULL;
		MBSTRG(http_input_list_size) = 0;
		return SUCCESS;
	}

	if (FAILURE == php_mb_parse_encoding_list(ZSTR_VAL(new_value), ZSTR_LEN(new_value), &list, &size, 1)) {
		return FAILURE;
	}

	if (MBSTRG(http_input_list)) {
		pefree(MBSTRG(http_input_list), 1);
	}
	MBSTRG(http_input_list) = list;
	MBSTRG(http_input_list_size) = size;

	if (stage & (PHP_INI_STAGE_ACTIVATE | PHP_INI_STAGE_RUNTIME)) {
		php_error_docref("ref.mbstring", E_DEPRECATED, "Use of mbstring.http_input is deprecated");
	}

	return SUCCESS;
}
/* }}} */

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_http_output) */
static PHP_INI_MH(OnUpdate_mbstring_http_output)
{
	const mbfl_encoding *encoding;

	if (new_value == NULL || ZSTR_LEN(new_value) == 0) {
		encoding = mbfl_name2encoding(get_output_encoding());
		if (!encoding) {
			MBSTRG(http_output_encoding) = &mbfl_encoding_pass;
			MBSTRG(current_http_output_encoding) = &mbfl_encoding_pass;
			return SUCCESS;
		}
	} else {
		encoding = mbfl_name2encoding(ZSTR_VAL(new_value));
		if (!encoding) {
			MBSTRG(http_output_encoding) = &mbfl_encoding_pass;
			MBSTRG(current_http_output_encoding) = &mbfl_encoding_pass;
			return FAILURE;
		}
	}
	MBSTRG(http_output_encoding) = encoding;
	MBSTRG(current_http_output_encoding) = encoding;

	if (stage & (PHP_INI_STAGE_ACTIVATE | PHP_INI_STAGE_RUNTIME)) {
		php_error_docref("ref.mbstring", E_DEPRECATED, "Use of mbstring.http_output is deprecated");
	}

	return SUCCESS;
}
/* }}} */

/* {{{ static _php_mb_ini_mbstring_internal_encoding_set */
int _php_mb_ini_mbstring_internal_encoding_set(const char *new_value, uint new_value_length)
{
	const mbfl_encoding *encoding;

	if (!new_value || new_value_length == 0 || !(encoding = mbfl_name2encoding(new_value))) {
		/* falls back to UTF-8 if an unknown encoding name is given */
		encoding = mbfl_no2encoding(mbfl_no_encoding_utf8);
	}
	MBSTRG(internal_encoding) = encoding;
	MBSTRG(current_internal_encoding) = encoding;
#if HAVE_MBREGEX
	{
		const char *enc_name = new_value;
		if (FAILURE == php_mb_regex_set_default_mbctype(enc_name)) {
			/* falls back to UTF-8 if an unknown encoding name is given */
			enc_name = "UTF-8";
			php_mb_regex_set_default_mbctype(enc_name);
		}
		php_mb_regex_set_mbctype(new_value);
	}
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_internal_encoding) */
static PHP_INI_MH(OnUpdate_mbstring_internal_encoding)
{
	if (stage & (PHP_INI_STAGE_ACTIVATE | PHP_INI_STAGE_RUNTIME)) {
		php_error_docref("ref.mbstring", E_DEPRECATED, "Use of mbstring.internal_encoding is deprecated");
	}

	if (OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage) == FAILURE) {
		return FAILURE;
	}

	if (stage & (PHP_INI_STAGE_STARTUP | PHP_INI_STAGE_SHUTDOWN | PHP_INI_STAGE_RUNTIME)) {
		if (new_value && ZSTR_LEN(new_value)) {
			return _php_mb_ini_mbstring_internal_encoding_set(ZSTR_VAL(new_value), ZSTR_LEN(new_value));
		} else {
			return _php_mb_ini_mbstring_internal_encoding_set(get_internal_encoding(), strlen(get_internal_encoding())+1);
		}
	} else {
		/* the corresponding mbstring globals needs to be set according to the
		 * ini value in the later stage because it never falls back to the
		 * default value if 1. no value for mbstring.internal_encoding is given,
		 * 2. mbstring.language directive is processed in per-dir or runtime
		 * context and 3. call to the handler for mbstring.language is done
		 * after mbstring.internal_encoding is handled. */
		return SUCCESS;
	}
}
/* }}} */

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_substitute_character) */
static PHP_INI_MH(OnUpdate_mbstring_substitute_character)
{
	int c;
	char *endptr = NULL;

	if (new_value != NULL) {
		if (strcasecmp("none", ZSTR_VAL(new_value)) == 0) {
			MBSTRG(filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
			MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
		} else if (strcasecmp("long", ZSTR_VAL(new_value)) == 0) {
			MBSTRG(filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG;
			MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG;
		} else if (strcasecmp("entity", ZSTR_VAL(new_value)) == 0) {
			MBSTRG(filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY;
			MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY;
		} else {
			MBSTRG(filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
			MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
			if (ZSTR_LEN(new_value) > 0) {
				c = strtol(ZSTR_VAL(new_value), &endptr, 0);
				if (*endptr == '\0') {
					MBSTRG(filter_illegal_substchar) = c;
					MBSTRG(current_filter_illegal_substchar) = c;
				}
			}
		}
	} else {
		MBSTRG(filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
		MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
		MBSTRG(filter_illegal_substchar) = 0x3f;	/* '?' */
		MBSTRG(current_filter_illegal_substchar) = 0x3f;	/* '?' */
	}

	return SUCCESS;
}
/* }}} */

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_encoding_translation) */
static PHP_INI_MH(OnUpdate_mbstring_encoding_translation)
{
	if (new_value == NULL) {
		return FAILURE;
	}

	OnUpdateBool(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);

	if (MBSTRG(encoding_translation)) {
		sapi_unregister_post_entry(php_post_entries);
		sapi_register_post_entries(mbstr_post_entries);
	} else {
		sapi_unregister_post_entry(mbstr_post_entries);
		sapi_register_post_entries(php_post_entries);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_http_output_conv_mimetypes */
static PHP_INI_MH(OnUpdate_mbstring_http_output_conv_mimetypes)
{
	zend_string *tmp;
	void *re = NULL;

	if (!new_value) {
		new_value = entry->orig_value;
	}
	tmp = php_trim(new_value, NULL, 0, 3);

	if (ZSTR_LEN(tmp) > 0) {
		if (!(re = _php_mb_compile_regex(ZSTR_VAL(tmp)))) {
			zend_string_release(tmp);
			return FAILURE;
		}
	}

	if (MBSTRG(http_output_conv_mimetypes)) {
		_php_mb_free_regex(MBSTRG(http_output_conv_mimetypes));
	}

	MBSTRG(http_output_conv_mimetypes) = re;

	zend_string_release(tmp);
	return SUCCESS;
}
/* }}} */
/* }}} */

/* {{{ php.ini directive registration */
PHP_INI_BEGIN()
	PHP_INI_ENTRY("mbstring.language", "neutral", PHP_INI_ALL, OnUpdate_mbstring_language)
	PHP_INI_ENTRY("mbstring.detect_order", NULL, PHP_INI_ALL, OnUpdate_mbstring_detect_order)
	PHP_INI_ENTRY("mbstring.http_input", NULL, PHP_INI_ALL, OnUpdate_mbstring_http_input)
	PHP_INI_ENTRY("mbstring.http_output", NULL, PHP_INI_ALL, OnUpdate_mbstring_http_output)
	STD_PHP_INI_ENTRY("mbstring.internal_encoding", NULL, PHP_INI_ALL, OnUpdate_mbstring_internal_encoding, internal_encoding_name, zend_mbstring_globals, mbstring_globals)
	PHP_INI_ENTRY("mbstring.substitute_character", NULL, PHP_INI_ALL, OnUpdate_mbstring_substitute_character)
	STD_PHP_INI_ENTRY("mbstring.func_overload", "0",
	PHP_INI_SYSTEM, OnUpdateLong, func_overload, zend_mbstring_globals, mbstring_globals)

	STD_PHP_INI_BOOLEAN("mbstring.encoding_translation", "0",
		PHP_INI_SYSTEM | PHP_INI_PERDIR,
		OnUpdate_mbstring_encoding_translation,
		encoding_translation, zend_mbstring_globals, mbstring_globals)
	PHP_INI_ENTRY("mbstring.http_output_conv_mimetypes",
		"^(text/|application/xhtml\\+xml)",
		PHP_INI_ALL,
		OnUpdate_mbstring_http_output_conv_mimetypes)

	STD_PHP_INI_BOOLEAN("mbstring.strict_detection", "0",
		PHP_INI_ALL,
		OnUpdateLong,
		strict_detection, zend_mbstring_globals, mbstring_globals)
PHP_INI_END()
/* }}} */

/* {{{ module global initialize handler */
static PHP_GINIT_FUNCTION(mbstring)
{
#if defined(COMPILE_DL_MBSTRING) && defined(ZTS)
ZEND_TSRMLS_CACHE_UPDATE();
#endif

	mbstring_globals->language = mbfl_no_language_uni;
	mbstring_globals->internal_encoding = NULL;
	mbstring_globals->current_internal_encoding = mbstring_globals->internal_encoding;
	mbstring_globals->http_output_encoding = &mbfl_encoding_pass;
	mbstring_globals->current_http_output_encoding = &mbfl_encoding_pass;
	mbstring_globals->http_input_identify = NULL;
	mbstring_globals->http_input_identify_get = NULL;
	mbstring_globals->http_input_identify_post = NULL;
	mbstring_globals->http_input_identify_cookie = NULL;
	mbstring_globals->http_input_identify_string = NULL;
	mbstring_globals->http_input_list = NULL;
	mbstring_globals->http_input_list_size = 0;
	mbstring_globals->detect_order_list = NULL;
	mbstring_globals->detect_order_list_size = 0;
	mbstring_globals->current_detect_order_list = NULL;
	mbstring_globals->current_detect_order_list_size = 0;
	mbstring_globals->default_detect_order_list = (enum mbfl_no_encoding *) php_mb_default_identify_list_neut;
	mbstring_globals->default_detect_order_list_size = sizeof(php_mb_default_identify_list_neut) / sizeof(php_mb_default_identify_list_neut[0]);
	mbstring_globals->filter_illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
	mbstring_globals->filter_illegal_substchar = 0x3f;	/* '?' */
	mbstring_globals->current_filter_illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
	mbstring_globals->current_filter_illegal_substchar = 0x3f;	/* '?' */
	mbstring_globals->illegalchars = 0;
	mbstring_globals->func_overload = 0;
	mbstring_globals->encoding_translation = 0;
	mbstring_globals->strict_detection = 0;
	mbstring_globals->outconv = NULL;
	mbstring_globals->http_output_conv_mimetypes = NULL;
#if HAVE_MBREGEX
	mbstring_globals->mb_regex_globals = php_mb_regex_globals_alloc();
#endif
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION */
static PHP_GSHUTDOWN_FUNCTION(mbstring)
{
	if (mbstring_globals->http_input_list) {
		free(mbstring_globals->http_input_list);
	}
	if (mbstring_globals->detect_order_list) {
		free(mbstring_globals->detect_order_list);
	}
	if (mbstring_globals->http_output_conv_mimetypes) {
		_php_mb_free_regex(mbstring_globals->http_output_conv_mimetypes);
	}
#if HAVE_MBREGEX
	php_mb_regex_globals_free(mbstring_globals->mb_regex_globals);
#endif
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(mbstring) */
PHP_MINIT_FUNCTION(mbstring)
{
#if defined(COMPILE_DL_MBSTRING) && defined(ZTS)
ZEND_TSRMLS_CACHE_UPDATE();
#endif
	__mbfl_allocators = &_php_mb_allocators;

	REGISTER_INI_ENTRIES();

	/* This is a global handler. Should not be set in a per-request handler. */
	sapi_register_treat_data(mbstr_treat_data);

	/* Post handlers are stored in the thread-local context. */
	if (MBSTRG(encoding_translation)) {
		sapi_register_post_entries(mbstr_post_entries);
	}

	REGISTER_LONG_CONSTANT("MB_OVERLOAD_MAIL", MB_OVERLOAD_MAIL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MB_OVERLOAD_STRING", MB_OVERLOAD_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MB_OVERLOAD_REGEX", MB_OVERLOAD_REGEX, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("MB_CASE_UPPER", PHP_UNICODE_CASE_UPPER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MB_CASE_LOWER", PHP_UNICODE_CASE_LOWER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MB_CASE_TITLE", PHP_UNICODE_CASE_TITLE, CONST_CS | CONST_PERSISTENT);

#if HAVE_MBREGEX
	PHP_MINIT(mb_regex) (INIT_FUNC_ARGS_PASSTHRU);
#endif

	if (FAILURE == zend_multibyte_set_functions(&php_mb_zend_multibyte_functions)) {
		return FAILURE;
	}

	php_rfc1867_set_multibyte_callbacks(
		php_mb_encoding_translation,
		php_mb_gpc_get_detect_order,
		php_mb_gpc_set_input_encoding,
		php_mb_rfc1867_getword,
		php_mb_rfc1867_getword_conf,
		php_mb_rfc1867_basename);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION(mbstring) */
PHP_MSHUTDOWN_FUNCTION(mbstring)
{
	UNREGISTER_INI_ENTRIES();

#if HAVE_MBREGEX
	PHP_MSHUTDOWN(mb_regex) (INIT_FUNC_ARGS_PASSTHRU);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION(mbstring) */
PHP_RINIT_FUNCTION(mbstring)
{
	zend_function *func, *orig;
	const struct mb_overload_def *p;

	MBSTRG(current_internal_encoding) = MBSTRG(internal_encoding);
	MBSTRG(current_http_output_encoding) = MBSTRG(http_output_encoding);
	MBSTRG(current_filter_illegal_mode) = MBSTRG(filter_illegal_mode);
	MBSTRG(current_filter_illegal_substchar) = MBSTRG(filter_illegal_substchar);

	MBSTRG(illegalchars) = 0;

	php_mb_populate_current_detect_order_list();

 	/* override original function. */
	if (MBSTRG(func_overload)){
		p = &(mb_ovld[0]);

		CG(compiler_options) |= ZEND_COMPILE_NO_BUILTIN_STRLEN;
		while (p->type > 0) {
			if ((MBSTRG(func_overload) & p->type) == p->type &&
				!zend_hash_str_exists(EG(function_table), p->save_func, strlen(p->save_func))
			) {
				func = zend_hash_str_find_ptr(EG(function_table), p->ovld_func, strlen(p->ovld_func));

				if ((orig = zend_hash_str_find_ptr(EG(function_table), p->orig_func, strlen(p->orig_func))) == NULL) {
					php_error_docref("ref.mbstring", E_WARNING, "mbstring couldn't find function %s.", p->orig_func);
					return FAILURE;
				} else {
					ZEND_ASSERT(orig->type == ZEND_INTERNAL_FUNCTION);
					zend_hash_str_add_mem(EG(function_table), p->save_func, strlen(p->save_func), orig, sizeof(zend_internal_function));
					function_add_ref(orig);

					if (zend_hash_str_update_mem(EG(function_table), p->orig_func, strlen(p->orig_func), func, sizeof(zend_internal_function)) == NULL) {
						php_error_docref("ref.mbstring", E_WARNING, "mbstring couldn't replace function %s.", p->orig_func);
						return FAILURE;
					}

					function_add_ref(func);
				}
			}
			p++;
		}
	}
#if HAVE_MBREGEX
	PHP_RINIT(mb_regex) (INIT_FUNC_ARGS_PASSTHRU);
#endif
	zend_multibyte_set_internal_encoding((const zend_encoding *)MBSTRG(internal_encoding));

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION(mbstring) */
PHP_RSHUTDOWN_FUNCTION(mbstring)
{
	const struct mb_overload_def *p;
	zend_function *orig;

	if (MBSTRG(current_detect_order_list) != NULL) {
		efree(MBSTRG(current_detect_order_list));
		MBSTRG(current_detect_order_list) = NULL;
		MBSTRG(current_detect_order_list_size) = 0;
	}
	if (MBSTRG(outconv) != NULL) {
		MBSTRG(illegalchars) += mbfl_buffer_illegalchars(MBSTRG(outconv));
		mbfl_buffer_converter_delete(MBSTRG(outconv));
		MBSTRG(outconv) = NULL;
	}

	/* clear http input identification. */
	MBSTRG(http_input_identify) = NULL;
	MBSTRG(http_input_identify_post) = NULL;
	MBSTRG(http_input_identify_get) = NULL;
	MBSTRG(http_input_identify_cookie) = NULL;
	MBSTRG(http_input_identify_string) = NULL;

 	/*  clear overloaded function. */
	if (MBSTRG(func_overload)){
		p = &(mb_ovld[0]);
		while (p->type > 0) {
			if ((MBSTRG(func_overload) & p->type) == p->type &&
				(orig = zend_hash_str_find_ptr(EG(function_table), p->save_func, strlen(p->save_func)))) {

				zend_hash_str_update_mem(EG(function_table), p->orig_func, strlen(p->orig_func), orig, sizeof(zend_internal_function));
				function_add_ref(orig);
				zend_hash_str_del(EG(function_table), p->save_func, strlen(p->save_func));
			}
			p++;
		}
		CG(compiler_options) &= ~ZEND_COMPILE_NO_BUILTIN_STRLEN;
	}

#if HAVE_MBREGEX
	PHP_RSHUTDOWN(mb_regex) (INIT_FUNC_ARGS_PASSTHRU);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION(mbstring) */
PHP_MINFO_FUNCTION(mbstring)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Multibyte Support", "enabled");
	php_info_print_table_row(2, "Multibyte string engine", "libmbfl");
	php_info_print_table_row(2, "HTTP input encoding translation", MBSTRG(encoding_translation) ? "enabled": "disabled");
	{
		char tmp[256];
		snprintf(tmp, sizeof(tmp), "%d.%d.%d", MBFL_VERSION_MAJOR, MBFL_VERSION_MINOR, MBFL_VERSION_TEENY);
		php_info_print_table_row(2, "libmbfl version", tmp);
	}
#if HAVE_ONIG
	{
		char tmp[256];
		snprintf(tmp, sizeof(tmp), "%d.%d.%d", ONIGURUMA_VERSION_MAJOR, ONIGURUMA_VERSION_MINOR, ONIGURUMA_VERSION_TEENY);
		php_info_print_table_row(2, "oniguruma version", tmp);
	}
#endif
	php_info_print_table_end();

	php_info_print_table_start();
	php_info_print_table_header(1, "mbstring extension makes use of \"streamable kanji code filter and converter\", which is distributed under the GNU Lesser General Public License version 2.1.");
	php_info_print_table_end();

#if HAVE_MBREGEX
	PHP_MINFO(mb_regex)(ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU);
#endif

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ proto string mb_language([string language])
   Sets the current language or Returns the current language as a string */
PHP_FUNCTION(mb_language)
{
	zend_string *name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
		return;
	}
	if (name == NULL) {
		RETVAL_STRING((char *)mbfl_no_language2name(MBSTRG(language)));
	} else {
		zend_string *ini_name = zend_string_init("mbstring.language", sizeof("mbstring.language") - 1, 0);
		if (FAILURE == zend_alter_ini_entry(ini_name, name, PHP_INI_USER, PHP_INI_STAGE_RUNTIME)) {
			php_error_docref(NULL, E_WARNING, "Unknown language \"%s\"", ZSTR_VAL(name));
			RETVAL_FALSE;
		} else {
			RETVAL_TRUE;
		}
		zend_string_release(ini_name);
	}
}
/* }}} */

/* {{{ proto string mb_internal_encoding([string encoding])
   Sets the current internal encoding or Returns the current internal encoding as a string */
PHP_FUNCTION(mb_internal_encoding)
{
	const char *name = NULL;
	size_t name_len;
	const mbfl_encoding *encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &name, &name_len) == FAILURE) {
		return;
	}
	if (name == NULL) {
		name = MBSTRG(current_internal_encoding) ? MBSTRG(current_internal_encoding)->name: NULL;
		if (name != NULL) {
			RETURN_STRING(name);
		} else {
			RETURN_FALSE;
		}
	} else {
		encoding = mbfl_name2encoding(name);
		if (!encoding) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", name);
			RETURN_FALSE;
		} else {
			MBSTRG(current_internal_encoding) = encoding;
			RETURN_TRUE;
		}
	}
}
/* }}} */

/* {{{ proto mixed mb_http_input([string type])
   Returns the input encoding */
PHP_FUNCTION(mb_http_input)
{
	char *typ = NULL;
	size_t typ_len;
	int retname;
	char *list, *temp;
	const mbfl_encoding *result = NULL;

	retname = 1;
 	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &typ, &typ_len) == FAILURE) {
		return;
 	}
 	if (typ == NULL) {
 		result = MBSTRG(http_input_identify);
 	} else {
 		switch (*typ) {
		case 'G':
		case 'g':
			result = MBSTRG(http_input_identify_get);
			break;
		case 'P':
		case 'p':
			result = MBSTRG(http_input_identify_post);
			break;
		case 'C':
		case 'c':
			result = MBSTRG(http_input_identify_cookie);
			break;
		case 'S':
		case 's':
			result = MBSTRG(http_input_identify_string);
			break;
		case 'I':
		case 'i':
			{
				const mbfl_encoding **entry = MBSTRG(http_input_list);
				const size_t n = MBSTRG(http_input_list_size);
				size_t i;
				array_init(return_value);
				for (i = 0; i < n; i++) {
					add_next_index_string(return_value, (*entry)->name);
					entry++;
				}
				retname = 0;
			}
			break;
		case 'L':
		case 'l':
			{
				const mbfl_encoding **entry = MBSTRG(http_input_list);
				const size_t n = MBSTRG(http_input_list_size);
				size_t i;
				list = NULL;
				for (i = 0; i < n; i++) {
					if (list) {
						temp = list;
						spprintf(&list, 0, "%s,%s", temp, (*entry)->name);
						efree(temp);
						if (!list) {
							break;
						}
					} else {
						list = estrdup((*entry)->name);
					}
					entry++;
				}
			}
			if (!list) {
				RETURN_FALSE;
			}
			RETVAL_STRING(list);
			efree(list);
			retname = 0;
			break;
		default:
			result = MBSTRG(http_input_identify);
			break;
		}
	}

	if (retname) {
		if (result) {
			RETVAL_STRING(result->name);
		} else {
			RETVAL_FALSE;
		}
	}
}
/* }}} */

/* {{{ proto string mb_http_output([string encoding])
   Sets the current output_encoding or returns the current output_encoding as a string */
PHP_FUNCTION(mb_http_output)
{
	const char *name = NULL;
	size_t name_len;
	const mbfl_encoding *encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &name, &name_len) == FAILURE) {
		return;
	}

	if (name == NULL) {
		name = MBSTRG(current_http_output_encoding) ? MBSTRG(current_http_output_encoding)->name: NULL;
		if (name != NULL) {
			RETURN_STRING(name);
		} else {
			RETURN_FALSE;
		}
	} else {
		encoding = mbfl_name2encoding(name);
		if (!encoding) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", name);
			RETURN_FALSE;
		} else {
			MBSTRG(current_http_output_encoding) = encoding;
			RETURN_TRUE;
		}
	}
}
/* }}} */

/* {{{ proto bool|array mb_detect_order([mixed encoding-list])
   Sets the current detect_order or Return the current detect_order as a array */
PHP_FUNCTION(mb_detect_order)
{
	zval *arg1 = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &arg1) == FAILURE) {
		return;
	}

	if (!arg1) {
		size_t i;
		size_t n = MBSTRG(current_detect_order_list_size);
		const mbfl_encoding **entry = MBSTRG(current_detect_order_list);
		array_init(return_value);
		for (i = 0; i < n; i++) {
			add_next_index_string(return_value, (*entry)->name);
			entry++;
		}
	} else {
		const mbfl_encoding **list = NULL;
		size_t size = 0;
		switch (Z_TYPE_P(arg1)) {
			case IS_ARRAY:
				if (FAILURE == php_mb_parse_encoding_array(arg1, &list, &size, 0)) {
					if (list) {
						efree(list);
					}
					RETURN_FALSE;
				}
				break;
			default:
				convert_to_string_ex(arg1);
				if (FAILURE == php_mb_parse_encoding_list(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1), &list, &size, 0)) {
					if (list) {
						efree(list);
					}
					RETURN_FALSE;
				}
				break;
		}

		if (list == NULL) {
			RETURN_FALSE;
		}

		if (MBSTRG(current_detect_order_list)) {
			efree(MBSTRG(current_detect_order_list));
		}
		MBSTRG(current_detect_order_list) = list;
		MBSTRG(current_detect_order_list_size) = size;
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto mixed mb_substitute_character([mixed substchar])
   Sets the current substitute_character or returns the current substitute_character */
PHP_FUNCTION(mb_substitute_character)
{
	zval *arg1 = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &arg1) == FAILURE) {
		return;
	}

	if (!arg1) {
		if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			RETURN_STRING("none");
		} else if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG) {
			RETURN_STRING("long");
		} else if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY) {
			RETURN_STRING("entity");
		} else {
			RETURN_LONG(MBSTRG(current_filter_illegal_substchar));
		}
	} else {
		RETVAL_TRUE;

		switch (Z_TYPE_P(arg1)) {
			case IS_STRING:
				if (strncasecmp("none", Z_STRVAL_P(arg1), Z_STRLEN_P(arg1)) == 0) {
					MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
				} else if (strncasecmp("long", Z_STRVAL_P(arg1), Z_STRLEN_P(arg1)) == 0) {
					MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG;
				} else if (strncasecmp("entity", Z_STRVAL_P(arg1), Z_STRLEN_P(arg1)) == 0) {
					MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY;
				} else {
					convert_to_long_ex(arg1);

					if (Z_LVAL_P(arg1) < 0xffff && Z_LVAL_P(arg1) > 0x0) {
						MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
						MBSTRG(current_filter_illegal_substchar) = Z_LVAL_P(arg1);
					} else {
						php_error_docref(NULL, E_WARNING, "Unknown character.");
						RETURN_FALSE;
					}
				}
				break;
			default:
				convert_to_long_ex(arg1);
				if (Z_LVAL_P(arg1) < 0xffff && Z_LVAL_P(arg1) > 0x0) {
					MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
					MBSTRG(current_filter_illegal_substchar) = Z_LVAL_P(arg1);
				} else {
					php_error_docref(NULL, E_WARNING, "Unknown character.");
					RETURN_FALSE;
				}
				break;
		}
	}
}
/* }}} */

/* {{{ proto string mb_preferred_mime_name(string encoding)
   Return the preferred MIME name (charset) as a string */
PHP_FUNCTION(mb_preferred_mime_name)
{
	enum mbfl_no_encoding no_encoding;
	char *name = NULL;
	size_t name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		return;
	} else {
		no_encoding = mbfl_name2no_encoding(name);
		if (no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", name);
			RETVAL_FALSE;
		} else {
			const char *preferred_name = mbfl_no2preferred_mime_name(no_encoding);
			if (preferred_name == NULL || *preferred_name == '\0') {
				php_error_docref(NULL, E_WARNING, "No MIME preferred name corresponding to \"%s\"", name);
				RETVAL_FALSE;
			} else {
				RETVAL_STRING((char *)preferred_name);
			}
		}
	}
}
/* }}} */

#define IS_SJIS1(c) ((((c)>=0x81 && (c)<=0x9f) || ((c)>=0xe0 && (c)<=0xf5)) ? 1 : 0)
#define IS_SJIS2(c) ((((c)>=0x40 && (c)<=0x7e) || ((c)>=0x80 && (c)<=0xfc)) ? 1 : 0)

/* {{{ proto bool mb_parse_str(string encoded_string [, array result])
   Parses GET/POST/COOKIE data and sets global variables */
PHP_FUNCTION(mb_parse_str)
{
	zval *track_vars_array = NULL;
	char *encstr = NULL;
	size_t encstr_len;
	php_mb_encoding_handler_info_t info;
	const mbfl_encoding *detected;

	track_vars_array = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|z/", &encstr, &encstr_len, &track_vars_array) == FAILURE) {
		return;
	}

	if (track_vars_array != NULL) {
		/* Clear out the array */
		zval_dtor(track_vars_array);
		array_init(track_vars_array);
	}

	encstr = estrndup(encstr, encstr_len);

	info.data_type              = PARSE_STRING;
	info.separator              = PG(arg_separator).input;
	info.report_errors          = 1;
	info.to_encoding            = MBSTRG(current_internal_encoding);
	info.to_language            = MBSTRG(language);
	info.from_encodings         = MBSTRG(http_input_list);
	info.num_from_encodings     = MBSTRG(http_input_list_size);
	info.from_language          = MBSTRG(language);

	if (track_vars_array != NULL) {
		detected = _php_mb_encoding_handler_ex(&info, track_vars_array, encstr);
	} else {
		zval tmp;
		zend_array *symbol_table = zend_rebuild_symbol_table();

		ZVAL_ARR(&tmp, symbol_table);
		detected = _php_mb_encoding_handler_ex(&info, &tmp, encstr);
	}

	MBSTRG(http_input_identify) = detected;

	RETVAL_BOOL(detected);

	if (encstr != NULL) efree(encstr);
}
/* }}} */

/* {{{ proto string mb_output_handler(string contents, int status)
   Returns string in output buffer converted to the http_output encoding */
PHP_FUNCTION(mb_output_handler)
{
	char *arg_string;
	size_t arg_string_len;
	zend_long arg_status;
	mbfl_string string, result;
	const char *charset;
	char *p;
	const mbfl_encoding *encoding;
	int last_feed, len;
	unsigned char send_text_mimetype = 0;
	char *s, *mimetype = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl", &arg_string, &arg_string_len, &arg_status) == FAILURE) {
		return;
	}

	encoding = MBSTRG(current_http_output_encoding);

 	/* start phase only */
 	if ((arg_status & PHP_OUTPUT_HANDLER_START) != 0) {
 		/* delete the converter just in case. */
 		if (MBSTRG(outconv)) {
			MBSTRG(illegalchars) += mbfl_buffer_illegalchars(MBSTRG(outconv));
 			mbfl_buffer_converter_delete(MBSTRG(outconv));
 			MBSTRG(outconv) = NULL;
  		}
		if (encoding == &mbfl_encoding_pass) {
			RETURN_STRINGL(arg_string, arg_string_len);
		}

		/* analyze mime type */
		if (SG(sapi_headers).mimetype &&
			_php_mb_match_regex(
				MBSTRG(http_output_conv_mimetypes),
				SG(sapi_headers).mimetype,
				strlen(SG(sapi_headers).mimetype))) {
			if ((s = strchr(SG(sapi_headers).mimetype,';')) == NULL){
				mimetype = estrdup(SG(sapi_headers).mimetype);
			} else {
				mimetype = estrndup(SG(sapi_headers).mimetype,s-SG(sapi_headers).mimetype);
			}
			send_text_mimetype = 1;
		} else if (SG(sapi_headers).send_default_content_type) {
			mimetype = SG(default_mimetype) ? SG(default_mimetype) : SAPI_DEFAULT_MIMETYPE;
		}

 		/* if content-type is not yet set, set it and activate the converter */
 		if (SG(sapi_headers).send_default_content_type || send_text_mimetype) {
			charset = encoding->mime_name;
			if (charset) {
				len = spprintf( &p, 0, "Content-Type: %s; charset=%s",  mimetype, charset );
				if (sapi_add_header(p, len, 0) != FAILURE) {
					SG(sapi_headers).send_default_content_type = 0;
				}
			}
 			/* activate the converter */
 			MBSTRG(outconv) = mbfl_buffer_converter_new2(MBSTRG(current_internal_encoding), encoding, 0);
			if (send_text_mimetype){
				efree(mimetype);
			}
 		}
  	}

 	/* just return if the converter is not activated. */
 	if (MBSTRG(outconv) == NULL) {
		RETURN_STRINGL(arg_string, arg_string_len);
	}

 	/* flag */
 	last_feed = ((arg_status & PHP_OUTPUT_HANDLER_END) != 0);
 	/* mode */
 	mbfl_buffer_converter_illegal_mode(MBSTRG(outconv), MBSTRG(current_filter_illegal_mode));
 	mbfl_buffer_converter_illegal_substchar(MBSTRG(outconv), MBSTRG(current_filter_illegal_substchar));

 	/* feed the string */
 	mbfl_string_init(&string);
	/* these are not needed. convd has encoding info.
	string.no_language = MBSTRG(language);
	string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
	*/
 	string.val = (unsigned char *)arg_string;
 	string.len = arg_string_len;
 	mbfl_buffer_converter_feed(MBSTRG(outconv), &string);
 	if (last_feed) {
 		mbfl_buffer_converter_flush(MBSTRG(outconv));
	}
 	/* get the converter output, and return it */
 	mbfl_buffer_converter_result(MBSTRG(outconv), &result);
	// TODO: avoid reallocation ???
 	RETVAL_STRINGL((char *)result.val, result.len);		/* the string is already strdup()'ed */
	efree(result.val);

 	/* delete the converter if it is the last feed. */
 	if (last_feed) {
		MBSTRG(illegalchars) += mbfl_buffer_illegalchars(MBSTRG(outconv));
		mbfl_buffer_converter_delete(MBSTRG(outconv));
		MBSTRG(outconv) = NULL;
	}
}
/* }}} */

/* {{{ proto int mb_strlen(string str [, string encoding])
   Get character numbers of a string */
PHP_FUNCTION(mb_strlen)
{
	int n;
	mbfl_string string;
	char *enc_name = NULL;
	size_t enc_name_len, string_len;

	mbfl_string_init(&string);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s", (char **)&string.val, &string_len, &enc_name, &enc_name_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_UINT_OVFL(string_len)) {
			php_error_docref(NULL, E_WARNING, "String overflows the max allowed length of %u", UINT_MAX);
			return;
	}

	string.len = (uint32_t)string_len;

	string.no_language = MBSTRG(language);
	if (enc_name == NULL) {
		string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
	} else {
		string.no_encoding = mbfl_name2no_encoding(enc_name);
		if (string.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", enc_name);
			RETURN_FALSE;
		}
	}

	n = mbfl_strlen(&string);
	if (n >= 0) {
		RETVAL_LONG(n);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto int mb_strpos(string haystack, string needle [, int offset [, string encoding]])
   Find position of first occurrence of a string within another */
PHP_FUNCTION(mb_strpos)
{
	int n, reverse = 0;
	zend_long offset = 0;
	mbfl_string haystack, needle;
	char *enc_name = NULL;
	size_t enc_name_len, haystack_len, needle_len;

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
	needle.no_language = MBSTRG(language);
	needle.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|ls", (char **)&haystack.val, &haystack_len, (char **)&needle.val, &needle_len, &offset, &enc_name, &enc_name_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_UINT_OVFL(haystack_len)) {
			php_error_docref(NULL, E_WARNING, "Haystack length overflows the max allowed length of %u", UINT_MAX);
			return;
	} else if (ZEND_SIZE_T_UINT_OVFL(needle_len)) {
			php_error_docref(NULL, E_WARNING, "Needle length overflows the max allowed length of %u", UINT_MAX);
			return;
	}

	haystack.len = (uint32_t)haystack_len;
	needle.len = (uint32_t)needle_len;

	if (enc_name != NULL) {
		haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(enc_name);
		if (haystack.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", enc_name);
			RETURN_FALSE;
		}
	}

	if (offset < 0 || offset > mbfl_strlen(&haystack)) {
		php_error_docref(NULL, E_WARNING, "Offset not contained in string");
		RETURN_FALSE;
	}
	if (needle.len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty delimiter");
		RETURN_FALSE;
	}

	n = mbfl_strpos(&haystack, &needle, offset, reverse);
	if (n >= 0) {
		RETVAL_LONG(n);
	} else {
		switch (-n) {
		case 1:
			break;
		case 2:
			php_error_docref(NULL, E_WARNING, "Needle has not positive length");
			break;
		case 4:
			php_error_docref(NULL, E_WARNING, "Unknown encoding or conversion error");
			break;
		case 8:
			php_error_docref(NULL, E_NOTICE, "Argument is empty");
			break;
		default:
			php_error_docref(NULL, E_WARNING, "Unknown error in mb_strpos");
			break;
		}
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto int mb_strrpos(string haystack, string needle [, int offset [, string encoding]])
   Find position of last occurrence of a string within another */
PHP_FUNCTION(mb_strrpos)
{
	int n;
	mbfl_string haystack, needle;
	char *enc_name = NULL;
	size_t enc_name_len, haystack_len, needle_len;
	zval *zoffset = NULL;
	long offset = 0, str_flg;
	char *enc_name2 = NULL;
	int enc_name_len2;

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
	needle.no_language = MBSTRG(language);
	needle.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|zs", (char **)&haystack.val, &haystack_len, (char **)&needle.val, &needle_len, &zoffset, &enc_name, &enc_name_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_UINT_OVFL(haystack_len)) {
			php_error_docref(NULL, E_WARNING, "Haystack length overflows the max allowed length of %u", UINT_MAX);
			return;
	} else if (ZEND_SIZE_T_UINT_OVFL(needle_len)) {
			php_error_docref(NULL, E_WARNING, "Needle length overflows the max allowed length of %u", UINT_MAX);
			return;
	}

	haystack.len = (uint32_t)haystack_len;
	needle.len = (uint32_t)needle_len;

	if (zoffset) {
		if (Z_TYPE_P(zoffset) == IS_STRING) {
			enc_name2     = Z_STRVAL_P(zoffset);
			enc_name_len2 = Z_STRLEN_P(zoffset);
			str_flg       = 1;

			if (enc_name2 != NULL) {
				switch (*enc_name2) {
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
					case ' ':
					case '-':
					case '.':
						break;
					default :
						str_flg = 0;
						break;
				}
			}

			if (str_flg) {
				convert_to_long_ex(zoffset);
				offset   = Z_LVAL_P(zoffset);
			} else {
				enc_name     = enc_name2;
				enc_name_len = enc_name_len2;
			}
		} else {
			convert_to_long_ex(zoffset);
			offset = Z_LVAL_P(zoffset);
		}
	}

	if (enc_name != NULL) {
		haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(enc_name);
		if (haystack.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", enc_name);
			RETURN_FALSE;
		}
	}

	if (haystack.len <= 0) {
		RETURN_FALSE;
	}
	if (needle.len <= 0) {
		RETURN_FALSE;
	}

	{
		int haystack_char_len = mbfl_strlen(&haystack);
		if ((offset > 0 && offset > haystack_char_len) ||
			(offset < 0 && -offset > haystack_char_len)) {
			php_error_docref(NULL, E_WARNING, "Offset is greater than the length of haystack string");
			RETURN_FALSE;
		}
	}

	n = mbfl_strpos(&haystack, &needle, offset, 1);
	if (n >= 0) {
		RETVAL_LONG(n);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto int mb_stripos(string haystack, string needle [, int offset [, string encoding]])
   Finds position of first occurrence of a string within another, case insensitive */
PHP_FUNCTION(mb_stripos)
{
	int n = -1;
	zend_long offset = 0;
	mbfl_string haystack, needle;
	const char *from_encoding = MBSTRG(current_internal_encoding)->mime_name;
	size_t from_encoding_len, haystack_len, needle_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|ls", (char **)&haystack.val, &haystack_len, (char **)&needle.val, &needle_len, &offset, &from_encoding, &from_encoding_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_UINT_OVFL(haystack_len)) {
			php_error_docref(NULL, E_WARNING, "Haystack length overflows the max allowed length of %u", UINT_MAX);
			return;
	} else if (ZEND_SIZE_T_UINT_OVFL(needle_len)) {
			php_error_docref(NULL, E_WARNING, "Needle length overflows the max allowed length of %u", UINT_MAX);
			return;
	}

	haystack.len = (uint32_t)haystack_len;
	needle.len = (uint32_t)needle_len;

	if (needle.len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty delimiter");
		RETURN_FALSE;
	}
	n = php_mb_stripos(0, (char *)haystack.val, haystack.len, (char *)needle.val, needle.len, offset, from_encoding);

	if (n >= 0) {
		RETVAL_LONG(n);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto int mb_strripos(string haystack, string needle [, int offset [, string encoding]])
   Finds position of last occurrence of a string within another, case insensitive */
PHP_FUNCTION(mb_strripos)
{
	int n = -1;
	zend_long offset = 0;
	mbfl_string haystack, needle;
	const char *from_encoding = MBSTRG(current_internal_encoding)->mime_name;
	size_t from_encoding_len, haystack_len, needle_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|ls", (char **)&haystack.val, &haystack_len, (char **)&needle.val, &needle_len, &offset, &from_encoding, &from_encoding_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_UINT_OVFL(haystack_len)) {
			php_error_docref(NULL, E_WARNING, "Haystack length overflows the max allowed length of %u", UINT_MAX);
			return;
	} else if (ZEND_SIZE_T_UINT_OVFL(needle_len)) {
			php_error_docref(NULL, E_WARNING, "Needle length overflows the max allowed length of %u", UINT_MAX);
			return;
	}

	haystack.len = (uint32_t)haystack_len;
	needle.len = (uint32_t)needle_len;

	n = php_mb_stripos(1, (char *)haystack.val, haystack.len, (char *)needle.val, needle.len, offset, from_encoding);

	if (n >= 0) {
		RETVAL_LONG(n);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string mb_strstr(string haystack, string needle[, bool part[, string encoding]])
   Finds first occurrence of a string within another */
PHP_FUNCTION(mb_strstr)
{
	int n, len, mblen;
	mbfl_string haystack, needle, result, *ret = NULL;
	char *enc_name = NULL;
	size_t enc_name_len, haystack_len, needle_len;
	zend_bool part = 0;

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
	needle.no_language = MBSTRG(language);
	needle.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|bs", (char **)&haystack.val, &haystack_len, (char **)&needle.val, &needle_len, &part, &enc_name, &enc_name_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_UINT_OVFL(haystack_len)) {
			php_error_docref(NULL, E_WARNING, "Haystack length overflows the max allowed length of %u", UINT_MAX);
			return;
	} else if (ZEND_SIZE_T_UINT_OVFL(needle_len)) {
			php_error_docref(NULL, E_WARNING, "Needle length overflows the max allowed length of %u", UINT_MAX);
			return;
	}

	haystack.len = (uint32_t)haystack_len;
	needle.len = (uint32_t)needle_len;

	if (enc_name != NULL) {
		haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(enc_name);
		if (haystack.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", enc_name);
			RETURN_FALSE;
		}
	}

	if (needle.len <= 0) {
		php_error_docref(NULL, E_WARNING, "Empty delimiter");
		RETURN_FALSE;
	}
	n = mbfl_strpos(&haystack, &needle, 0, 0);
	if (n >= 0) {
		mblen = mbfl_strlen(&haystack);
		if (part) {
			ret = mbfl_substr(&haystack, &result, 0, n);
			if (ret != NULL) {
				// TODO: avoid reallocation ???
				RETVAL_STRINGL((char *)ret->val, ret->len);
				efree(ret->val);
			} else {
				RETVAL_FALSE;
			}
		} else {
			len = (mblen - n);
			ret = mbfl_substr(&haystack, &result, n, len);
			if (ret != NULL) {
				// TODO: avoid reallocation ???
				RETVAL_STRINGL((char *)ret->val, ret->len);
				efree(ret->val);
			} else {
				RETVAL_FALSE;
			}
		}
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string mb_strrchr(string haystack, string needle[, bool part[, string encoding]])
   Finds the last occurrence of a character in a string within another */
PHP_FUNCTION(mb_strrchr)
{
	int n, len, mblen;
	mbfl_string haystack, needle, result, *ret = NULL;
	char *enc_name = NULL;
	size_t enc_name_len, haystack_len, needle_len;
	zend_bool part = 0;

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
	needle.no_language = MBSTRG(language);
	needle.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|bs", (char **)&haystack.val, &haystack_len, (char **)&needle.val, &needle_len, &part, &enc_name, &enc_name_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_UINT_OVFL(haystack_len)) {
			php_error_docref(NULL, E_WARNING, "Haystack length overflows the max allowed length of %u", UINT_MAX);
			return;
	} else if (ZEND_SIZE_T_UINT_OVFL(needle_len)) {
			php_error_docref(NULL, E_WARNING, "Needle length overflows the max allowed length of %u", UINT_MAX);
			return;
	}

	haystack.len = (uint32_t)haystack_len;
	needle.len = (uint32_t)needle_len;

	if (enc_name != NULL) {
		haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(enc_name);
		if (haystack.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", enc_name);
			RETURN_FALSE;
		}
	}

	if (haystack.len <= 0) {
		RETURN_FALSE;
	}
	if (needle.len <= 0) {
		RETURN_FALSE;
	}
	n = mbfl_strpos(&haystack, &needle, 0, 1);
	if (n >= 0) {
		mblen = mbfl_strlen(&haystack);
		if (part) {
			ret = mbfl_substr(&haystack, &result, 0, n);
			if (ret != NULL) {
				// TODO: avoid reallocation ???
				RETVAL_STRINGL((char *)ret->val, ret->len);
				efree(ret->val);
			} else {
				RETVAL_FALSE;
			}
		} else {
			len = (mblen - n);
			ret = mbfl_substr(&haystack, &result, n, len);
			if (ret != NULL) {
				// TODO: avoid reallocation ???
				RETVAL_STRINGL((char *)ret->val, ret->len);
				efree(ret->val);
			} else {
				RETVAL_FALSE;
			}
		}
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string mb_stristr(string haystack, string needle[, bool part[, string encoding]])
   Finds first occurrence of a string within another, case insensitive */
PHP_FUNCTION(mb_stristr)
{
	zend_bool part = 0;
	size_t from_encoding_len, len, mblen, haystack_len, needle_len;
	int n;
	mbfl_string haystack, needle, result, *ret = NULL;
	const char *from_encoding = MBSTRG(current_internal_encoding)->mime_name;
	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
	needle.no_language = MBSTRG(language);
	needle.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;


	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|bs", (char **)&haystack.val, &haystack_len, (char **)&needle.val, &needle_len, &part, &from_encoding, &from_encoding_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_UINT_OVFL(haystack_len)) {
			php_error_docref(NULL, E_WARNING, "Haystack length overflows the max allowed length of %u", UINT_MAX);
			return;
	} else if (ZEND_SIZE_T_UINT_OVFL(needle_len)) {
			php_error_docref(NULL, E_WARNING, "Needle length overflows the max allowed length of %u", UINT_MAX);
			return;
	}

	haystack.len = (uint32_t)haystack_len;
	needle.len = (uint32_t)needle_len;

	if (!needle.len) {
		php_error_docref(NULL, E_WARNING, "Empty delimiter");
		RETURN_FALSE;
	}

	haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(from_encoding);
	if (haystack.no_encoding == mbfl_no_encoding_invalid) {
		php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", from_encoding);
		RETURN_FALSE;
	}

	n = php_mb_stripos(0, (char *)haystack.val, haystack.len, (char *)needle.val, needle.len, 0, from_encoding);

	if (n <0) {
		RETURN_FALSE;
	}

	mblen = mbfl_strlen(&haystack);

	if (part) {
		ret = mbfl_substr(&haystack, &result, 0, n);
		if (ret != NULL) {
			// TODO: avoid reallocation ???
			RETVAL_STRINGL((char *)ret->val, ret->len);
			efree(ret->val);
		} else {
			RETVAL_FALSE;
		}
	} else {
		len = (mblen - n);
		ret = mbfl_substr(&haystack, &result, n, len);
		if (ret != NULL) {
			// TODO: avoid reallocaton ???
			RETVAL_STRINGL((char *)ret->val, ret->len);
			efree(ret->val);
		} else {
			RETVAL_FALSE;
		}
	}
}
/* }}} */

/* {{{ proto string mb_strrichr(string haystack, string needle[, bool part[, string encoding]])
   Finds the last occurrence of a character in a string within another, case insensitive */
PHP_FUNCTION(mb_strrichr)
{
	zend_bool part = 0;
	int n, len, mblen;
	size_t from_encoding_len, haystack_len, needle_len;
	mbfl_string haystack, needle, result, *ret = NULL;
	const char *from_encoding = MBSTRG(current_internal_encoding)->name;
	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
	needle.no_language = MBSTRG(language);
	needle.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;


	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|bs", (char **)&haystack.val, &haystack_len, (char **)&needle.val, &needle_len, &part, &from_encoding, &from_encoding_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_UINT_OVFL(haystack_len)) {
			php_error_docref(NULL, E_WARNING, "Haystack length overflows the max allowed length of %u", UINT_MAX);
			return;
	} else if (ZEND_SIZE_T_UINT_OVFL(needle_len)) {
			php_error_docref(NULL, E_WARNING, "Needle length overflows the max allowed length of %u", UINT_MAX);
			return;
	}

	haystack.len = (uint32_t)haystack_len;
	needle.len = (uint32_t)needle_len;

	haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(from_encoding);
	if (haystack.no_encoding == mbfl_no_encoding_invalid) {
		php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", from_encoding);
		RETURN_FALSE;
	}

	n = php_mb_stripos(1, (char *)haystack.val, haystack.len, (char *)needle.val, needle.len, 0, from_encoding);

	if (n <0) {
		RETURN_FALSE;
	}

	mblen = mbfl_strlen(&haystack);

	if (part) {
		ret = mbfl_substr(&haystack, &result, 0, n);
		if (ret != NULL) {
			// TODO: avoid reallocation ???
			RETVAL_STRINGL((char *)ret->val, ret->len);
			efree(ret->val);
		} else {
			RETVAL_FALSE;
		}
	} else {
		len = (mblen - n);
		ret = mbfl_substr(&haystack, &result, n, len);
		if (ret != NULL) {
			// TODO: avoid reallocation ???
			RETVAL_STRINGL((char *)ret->val, ret->len);
			efree(ret->val);
		} else {
			RETVAL_FALSE;
		}
	}
}
/* }}} */

/* {{{ proto int mb_substr_count(string haystack, string needle [, string encoding])
   Count the number of substring occurrences */
PHP_FUNCTION(mb_substr_count)
{
	int n;
	mbfl_string haystack, needle;
	char *enc_name = NULL;
	size_t enc_name_len, haystack_len, needle_len;

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
	needle.no_language = MBSTRG(language);
	needle.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|s", (char **)&haystack.val, &haystack_len, (char **)&needle.val, &needle_len, &enc_name, &enc_name_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_UINT_OVFL(haystack_len)) {
			php_error_docref(NULL, E_WARNING, "Haystack length overflows the max allowed length of %u", UINT_MAX);
			return;
	} else if (ZEND_SIZE_T_UINT_OVFL(needle_len)) {
			php_error_docref(NULL, E_WARNING, "Needle length overflows the max allowed length of %u", UINT_MAX);
			return;
	}

	haystack.len = (uint32_t)haystack_len;
	needle.len = (uint32_t)needle_len;

	if (enc_name != NULL) {
		haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(enc_name);
		if (haystack.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", enc_name);
			RETURN_FALSE;
		}
	}

	if (needle.len <= 0) {
		php_error_docref(NULL, E_WARNING, "Empty substring");
		RETURN_FALSE;
	}

	n = mbfl_substr_count(&haystack, &needle);
	if (n >= 0) {
		RETVAL_LONG(n);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string mb_substr(string str, int start [, int length [, string encoding]])
   Returns part of a string */
PHP_FUNCTION(mb_substr)
{
	char *str, *encoding = NULL;
	zend_long from, len;
	int mblen;
	size_t str_len, encoding_len;
	zend_bool len_is_null = 1;
	mbfl_string string, result, *ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl|l!s", &str, &str_len, &from, &len, &len_is_null, &encoding, &encoding_len) == FAILURE) {
		return;
	}

	mbfl_string_init(&string);
	string.no_language = MBSTRG(language);
	string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;

	if (encoding) {
		string.no_encoding = mbfl_name2no_encoding(encoding);
		if (string.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", encoding);
			RETURN_FALSE;
		}
	}

	string.val = (unsigned char *)str;
	string.len = str_len;

	if (len_is_null) {
		len = str_len;
	}

	/* measures length */
	mblen = 0;
	if (from < 0 || len < 0) {
		mblen = mbfl_strlen(&string);
	}

	/* if "from" position is negative, count start position from the end
	 * of the string
	 */
	if (from < 0) {
		from = mblen + from;
		if (from < 0) {
			from = 0;
		}
	}

	/* if "length" position is negative, set it to the length
	 * needed to stop that many chars from the end of the string
	 */
	if (len < 0) {
		len = (mblen - from) + len;
		if (len < 0) {
			len = 0;
		}
	}

	if (((MBSTRG(func_overload) & MB_OVERLOAD_STRING) == MB_OVERLOAD_STRING)
		&& (from >= mbfl_strlen(&string))) {
		RETURN_FALSE;
	}

	ret = mbfl_substr(&string, &result, from, len);
	if (NULL == ret) {
		RETURN_FALSE;
	}

	// TODO: avoid reallocation ???
	RETVAL_STRINGL((char *)ret->val, ret->len); /* the string is already strdup()'ed */
	efree(ret->val);
}
/* }}} */

/* {{{ proto string mb_strcut(string str, int start [, int length [, string encoding]])
   Returns part of a string */
PHP_FUNCTION(mb_strcut)
{
	char *encoding = NULL;
	zend_long from, len;
	size_t encoding_len, string_len;
	zend_bool len_is_null = 1;
	mbfl_string string, result, *ret;

	mbfl_string_init(&string);
	string.no_language = MBSTRG(language);
	string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl|l!s", (char **)&string.val, &string_len, &from, &len, &len_is_null, &encoding, &encoding_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_UINT_OVFL(string_len)) {
			php_error_docref(NULL, E_WARNING, "String length overflows the max allowed length of %u", UINT_MAX);
			return;
	}

	string.len = (uint32_t)string_len;

	if (encoding) {
		string.no_encoding = mbfl_name2no_encoding(encoding);
		if (string.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", encoding);
			RETURN_FALSE;
		}
	}

	if (len_is_null) {
		len = string.len;
	}

	/* if "from" position is negative, count start position from the end
	 * of the string
	 */
	if (from < 0) {
		from = string.len + from;
		if (from < 0) {
			from = 0;
		}
	}

	/* if "length" position is negative, set it to the length
	 * needed to stop that many chars from the end of the string
	 */
	if (len < 0) {
		len = (string.len - from) + len;
		if (len < 0) {
			len = 0;
		}
	}

	if ((unsigned int)from > string.len) {
		RETURN_FALSE;
	}

	ret = mbfl_strcut(&string, &result, from, len);
	if (ret == NULL) {
		RETURN_FALSE;
	}

	// TODO: avoid reallocation ???
	RETVAL_STRINGL((char *)ret->val, ret->len); /* the string is already strdup()'ed */
	efree(ret->val);
}
/* }}} */

/* {{{ proto int mb_strwidth(string str [, string encoding])
   Gets terminal width of a string */
PHP_FUNCTION(mb_strwidth)
{
	int n;
	mbfl_string string;
	char *enc_name = NULL;
	size_t enc_name_len, string_len;

	mbfl_string_init(&string);

	string.no_language = MBSTRG(language);
	string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s", (char **)&string.val, &string_len, &enc_name, &enc_name_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_UINT_OVFL(string_len)) {
			php_error_docref(NULL, E_WARNING, "String length overflows the max allowed length of %u", UINT_MAX);
			return;
	}

	string.len = (uint32_t)string_len;

	if (enc_name != NULL) {
		string.no_encoding = mbfl_name2no_encoding(enc_name);
		if (string.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", enc_name);
			RETURN_FALSE;
		}
	}

	n = mbfl_strwidth(&string);
	if (n >= 0) {
		RETVAL_LONG(n);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string mb_strimwidth(string str, int start, int width [, string trimmarker [, string encoding]])
   Trim the string in terminal width */
PHP_FUNCTION(mb_strimwidth)
{
	char *str, *trimmarker = NULL, *encoding = NULL;
	zend_long from, width;
	size_t str_len, trimmarker_len, encoding_len;
	mbfl_string string, result, marker, *ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sll|ss", &str, &str_len, &from, &width, &trimmarker, &trimmarker_len, &encoding, &encoding_len) == FAILURE) {
		return;
	}

	mbfl_string_init(&string);
	mbfl_string_init(&marker);
	string.no_language = MBSTRG(language);
	string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
	marker.no_language = MBSTRG(language);
	marker.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
	marker.val = NULL;
	marker.len = 0;

	if (encoding) {
		string.no_encoding = marker.no_encoding = mbfl_name2no_encoding(encoding);
		if (string.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", encoding);
			RETURN_FALSE;
		}
	}

	string.val = (unsigned char *)str;
	string.len = str_len;

	if (from < 0 || (size_t)from > str_len) {
		php_error_docref(NULL, E_WARNING, "Start position is out of range");
		RETURN_FALSE;
	}

	if (width < 0) {
		php_error_docref(NULL, E_WARNING, "Width is negative value");
		RETURN_FALSE;
	}

	if (trimmarker) {
		marker.val = (unsigned char *)trimmarker;
		marker.len = trimmarker_len;
	}

	ret = mbfl_strimwidth(&string, &marker, &result, from, width);

	if (ret == NULL) {
		RETURN_FALSE;
	}
	// TODO: avoid reallocation ???
	RETVAL_STRINGL((char *)ret->val, ret->len); /* the string is already strdup()'ed */
	efree(ret->val);
}
/* }}} */

/* {{{ MBSTRING_API char *php_mb_convert_encoding() */
MBSTRING_API char * php_mb_convert_encoding(const char *input, size_t length, const char *_to_encoding, const char *_from_encodings, size_t *output_len)
{
	mbfl_string string, result, *ret;
	const mbfl_encoding *from_encoding, *to_encoding;
	mbfl_buffer_converter *convd;
	size_t size;
	const mbfl_encoding **list;
	char *output=NULL;

	if (output_len) {
		*output_len = 0;
	}
	if (!input) {
		return NULL;
	}
	/* new encoding */
	if (_to_encoding && strlen(_to_encoding)) {
		to_encoding = mbfl_name2encoding(_to_encoding);
		if (!to_encoding) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", _to_encoding);
			return NULL;
		}
	} else {
		to_encoding = MBSTRG(current_internal_encoding);
	}

	/* initialize string */
	mbfl_string_init(&string);
	mbfl_string_init(&result);
	from_encoding = MBSTRG(current_internal_encoding);
	string.no_encoding = from_encoding->no_encoding;
	string.no_language = MBSTRG(language);
	string.val = (unsigned char *)input;
	string.len = length;

	/* pre-conversion encoding */
	if (_from_encodings) {
		list = NULL;
		size = 0;
		php_mb_parse_encoding_list(_from_encodings, strlen(_from_encodings), &list, &size, 0);
		if (size == 1) {
			from_encoding = *list;
			string.no_encoding = from_encoding->no_encoding;
		} else if (size > 1) {
			/* auto detect */
			from_encoding = mbfl_identify_encoding2(&string, list, size, MBSTRG(strict_detection));
			if (from_encoding) {
				string.no_encoding = from_encoding->no_encoding;
			} else {
				php_error_docref(NULL, E_WARNING, "Unable to detect character encoding");
				from_encoding = &mbfl_encoding_pass;
				to_encoding = from_encoding;
				string.no_encoding = from_encoding->no_encoding;
			}
		} else {
			php_error_docref(NULL, E_WARNING, "Illegal character encoding specified");
		}
		if (list != NULL) {
			efree((void *)list);
		}
	}

	/* initialize converter */
	convd = mbfl_buffer_converter_new2(from_encoding, to_encoding, string.len);
	if (convd == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to create character encoding converter");
		return NULL;
	}
	mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode));
	mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar));

	/* do it */
	ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
	if (ret) {
		if (output_len) {
			*output_len = ret->len;
		}
		output = (char *)ret->val;
	}

	MBSTRG(illegalchars) += mbfl_buffer_illegalchars(convd);
	mbfl_buffer_converter_delete(convd);
	return output;
}
/* }}} */

/* {{{ proto string mb_convert_encoding(string str, string to-encoding [, mixed from-encoding])
   Returns converted string in desired encoding */
PHP_FUNCTION(mb_convert_encoding)
{
	char *arg_str, *arg_new;
	size_t str_len, new_len;
	zval *arg_old = NULL;
	size_t size, l, n;
	char *_from_encodings = NULL, *ret, *s_free = NULL;

	zval *hash_entry;
	HashTable *target_hash;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|z", &arg_str, &str_len, &arg_new, &new_len, &arg_old) == FAILURE) {
		return;
	}

	if (arg_old) {
		switch (Z_TYPE_P(arg_old)) {
			case IS_ARRAY:
				target_hash = Z_ARRVAL_P(arg_old);
				_from_encodings = NULL;

				ZEND_HASH_FOREACH_VAL(target_hash, hash_entry) {

					convert_to_string_ex(hash_entry);

					if ( _from_encodings) {
						l = strlen(_from_encodings);
						n = strlen(Z_STRVAL_P(hash_entry));
						_from_encodings = erealloc(_from_encodings, l+n+2);
						memcpy(_from_encodings + l, ",", 1);
						memcpy(_from_encodings + l + 1, Z_STRVAL_P(hash_entry), Z_STRLEN_P(hash_entry) + 1);
					} else {
						_from_encodings = estrdup(Z_STRVAL_P(hash_entry));
					}
				} ZEND_HASH_FOREACH_END();

				if (_from_encodings != NULL && !strlen(_from_encodings)) {
					efree(_from_encodings);
					_from_encodings = NULL;
				}
				s_free = _from_encodings;
				break;
			default:
				convert_to_string(arg_old);
				_from_encodings = Z_STRVAL_P(arg_old);
				break;
			}
	}

	/* new encoding */
	ret = php_mb_convert_encoding(arg_str, str_len, arg_new, _from_encodings, &size);
	if (ret != NULL) {
		// TODO: avoid reallocation ???
		RETVAL_STRINGL(ret, size);		/* the string is already strdup()'ed */
		efree(ret);
	} else {
		RETVAL_FALSE;
	}

	if ( s_free) {
		efree(s_free);
	}
}
/* }}} */

/* {{{ proto string mb_convert_case(string sourcestring, int mode [, string encoding])
   Returns a case-folded version of sourcestring */
PHP_FUNCTION(mb_convert_case)
{
	const char *from_encoding = MBSTRG(current_internal_encoding)->mime_name;
	char *str;
	size_t str_len, from_encoding_len;
	zend_long case_mode = 0;
	char *newstr;
	size_t ret_len;

	RETVAL_FALSE;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl|s!", &str, &str_len,
				&case_mode, &from_encoding, &from_encoding_len) == FAILURE) {
		return;
	}

	newstr = php_unicode_convert_case(case_mode, str, (size_t) str_len, &ret_len, from_encoding);

	if (newstr) {
		// TODO: avoid reallocation ???
		RETVAL_STRINGL(newstr, ret_len);
		efree(newstr);
	}
}
/* }}} */

/* {{{ proto string mb_strtoupper(string sourcestring [, string encoding])
 *  Returns a uppercased version of sourcestring
 */
PHP_FUNCTION(mb_strtoupper)
{
	const char *from_encoding = MBSTRG(current_internal_encoding)->mime_name;
	char *str;
	size_t str_len, from_encoding_len;
	char *newstr;
	size_t ret_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!", &str, &str_len,
				&from_encoding, &from_encoding_len) == FAILURE) {
		return;
	}
	newstr = php_unicode_convert_case(PHP_UNICODE_CASE_UPPER, str, (size_t) str_len, &ret_len, from_encoding);

	if (newstr) {
		// TODO: avoid reallocation ???
		RETVAL_STRINGL(newstr, ret_len);
		efree(newstr);
		return;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string mb_strtolower(string sourcestring [, string encoding])
 *  Returns a lowercased version of sourcestring
 */
PHP_FUNCTION(mb_strtolower)
{
	const char *from_encoding = MBSTRG(current_internal_encoding)->mime_name;
	char *str;
	size_t str_len, from_encoding_len;
	char *newstr;
	size_t ret_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!", &str, &str_len,
				&from_encoding, &from_encoding_len) == FAILURE) {
		return;
	}
	newstr = php_unicode_convert_case(PHP_UNICODE_CASE_LOWER, str, (size_t) str_len, &ret_len, from_encoding);

	if (newstr) {
		// TODO: avoid reallocation ???
		RETVAL_STRINGL(newstr, ret_len);
		efree(newstr);
		return;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string mb_detect_encoding(string str [, mixed encoding_list [, bool strict]])
   Encodings of the given string is returned (as a string) */
PHP_FUNCTION(mb_detect_encoding)
{
	char *str;
	size_t str_len;
	zend_bool strict=0;
	zval *encoding_list = NULL;

	mbfl_string string;
	const mbfl_encoding *ret;
	const mbfl_encoding **elist, **list;
	size_t size;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|z!b", &str, &str_len, &encoding_list, &strict) == FAILURE) {
		return;
	}

	/* make encoding list */
	list = NULL;
	size = 0;
	if (encoding_list) {
		switch (Z_TYPE_P(encoding_list)) {
		case IS_ARRAY:
			if (FAILURE == php_mb_parse_encoding_array(encoding_list, &list, &size, 0)) {
				if (list) {
					efree(list);
					list = NULL;
					size = 0;
				}
			}
			break;
		default:
			convert_to_string(encoding_list);
			if (FAILURE == php_mb_parse_encoding_list(Z_STRVAL_P(encoding_list), Z_STRLEN_P(encoding_list), &list, &size, 0)) {
				if (list) {
					efree(list);
					list = NULL;
					size = 0;
				}
			}
			break;
		}
		if (size <= 0) {
			php_error_docref(NULL, E_WARNING, "Illegal argument");
		}
	}

	if (ZEND_NUM_ARGS() < 3) {
		strict = (zend_bool)MBSTRG(strict_detection);
	}

	if (size > 0 && list != NULL) {
		elist = list;
	} else {
		elist = MBSTRG(current_detect_order_list);
		size = MBSTRG(current_detect_order_list_size);
	}

	mbfl_string_init(&string);
	string.no_language = MBSTRG(language);
	string.val = (unsigned char *)str;
	string.len = str_len;
	ret = mbfl_identify_encoding2(&string, elist, size, strict);

	if (list != NULL) {
		efree((void *)list);
	}

	if (ret == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRING((char *)ret->name);
}
/* }}} */

/* {{{ proto mixed mb_list_encodings()
   Returns an array of all supported entity encodings */
PHP_FUNCTION(mb_list_encodings)
{
	const mbfl_encoding **encodings;
	const mbfl_encoding *encoding;
	int i;

	array_init(return_value);
	i = 0;
	encodings = mbfl_get_supported_encodings();
	while ((encoding = encodings[i++]) != NULL) {
		add_next_index_string(return_value, (char *) encoding->name);
	}
}
/* }}} */

/* {{{ proto array mb_encoding_aliases(string encoding)
   Returns an array of the aliases of a given encoding name */
PHP_FUNCTION(mb_encoding_aliases)
{
	const mbfl_encoding *encoding;
	char *name = NULL;
	size_t name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		return;
	}

	encoding = mbfl_name2encoding(name);
	if (!encoding) {
		php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", name);
		RETURN_FALSE;
	}

	array_init(return_value);
	if (encoding->aliases != NULL) {
		const char **alias;
		for (alias = *encoding->aliases; *alias; ++alias) {
			add_next_index_string(return_value, (char *)*alias);
		}
	}
}
/* }}} */

/* {{{ proto string mb_encode_mimeheader(string str [, string charset [, string transfer-encoding [, string linefeed [, int indent]]]])
   Converts the string to MIME "encoded-word" in the format of =?charset?(B|Q)?encoded_string?= */
PHP_FUNCTION(mb_encode_mimeheader)
{
	enum mbfl_no_encoding charset, transenc;
	mbfl_string  string, result, *ret;
	char *charset_name = NULL;
	size_t charset_name_len;
	char *trans_enc_name = NULL;
	size_t trans_enc_name_len;
	char *linefeed = "\r\n";
	size_t linefeed_len, string_len;
	zend_long indent = 0;

	mbfl_string_init(&string);
	string.no_language = MBSTRG(language);
	string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|sssl", (char **)&string.val, &string_len, &charset_name, &charset_name_len, &trans_enc_name, &trans_enc_name_len, &linefeed, &linefeed_len, &indent) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_UINT_OVFL(string_len)) {
			php_error_docref(NULL, E_WARNING, "String length overflows the max allowed length of %u", UINT_MAX);
			return;
	}

	string.len = (uint32_t)string_len;

	charset = mbfl_no_encoding_pass;
	transenc = mbfl_no_encoding_base64;

	if (charset_name != NULL) {
		charset = mbfl_name2no_encoding(charset_name);
		if (charset == mbfl_no_encoding_invalid) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", charset_name);
			RETURN_FALSE;
		}
	} else {
		const mbfl_language *lang = mbfl_no2language(MBSTRG(language));
		if (lang != NULL) {
			charset = lang->mail_charset;
			transenc = lang->mail_header_encoding;
		}
	}

	if (trans_enc_name != NULL) {
		if (*trans_enc_name == 'B' || *trans_enc_name == 'b') {
			transenc = mbfl_no_encoding_base64;
		} else if (*trans_enc_name == 'Q' || *trans_enc_name == 'q') {
			transenc = mbfl_no_encoding_qprint;
		}
	}

	mbfl_string_init(&result);
	ret = mbfl_mime_header_encode(&string, &result, charset, transenc, linefeed, indent);
	if (ret != NULL) {
		// TODO: avoid reallocation ???
		RETVAL_STRINGL((char *)ret->val, ret->len);	/* the string is already strdup()'ed */
		efree(ret->val);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string mb_decode_mimeheader(string string)
   Decodes the MIME "encoded-word" in the string */
PHP_FUNCTION(mb_decode_mimeheader)
{
	mbfl_string string, result, *ret;
	size_t string_len;

	mbfl_string_init(&string);
	string.no_language = MBSTRG(language);
	string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", (char **)&string.val, &string_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_UINT_OVFL(string_len)) {
			php_error_docref(NULL, E_WARNING, "String length overflows the max allowed length of %u", UINT_MAX);
			return;
	}

	string.len = (uint32_t)string_len;

	mbfl_string_init(&result);
	ret = mbfl_mime_header_decode(&string, &result, MBSTRG(current_internal_encoding)->no_encoding);
	if (ret != NULL) {
		// TODO: avoid reallocation ???
		RETVAL_STRINGL((char *)ret->val, ret->len);	/* the string is already strdup()'ed */
		efree(ret->val);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string mb_convert_kana(string str [, string option] [, string encoding])
   Conversion between full-width character and half-width character (Japanese) */
PHP_FUNCTION(mb_convert_kana)
{
	int opt, i;
	mbfl_string string, result, *ret;
	char *optstr = NULL;
	size_t optstr_len;
	char *encname = NULL;
	size_t encname_len, string_len;

	mbfl_string_init(&string);
	string.no_language = MBSTRG(language);
	string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|ss", (char **)&string.val, &string_len, &optstr, &optstr_len, &encname, &encname_len) == FAILURE) {
		return;
	}

	if (ZEND_SIZE_T_UINT_OVFL(string_len)) {
			php_error_docref(NULL, E_WARNING, "String length overflows the max allowed length of %u", UINT_MAX);
			return;
	}

	string.len = (uint32_t)string_len;

	/* option */
	if (optstr != NULL) {
		char *p = optstr;
		int n = optstr_len;
		i = 0;
		opt = 0;
		while (i < n) {
			i++;
			switch (*p++) {
			case 'A':
				opt |= 0x1;
				break;
			case 'a':
				opt |= 0x10;
				break;
			case 'R':
				opt |= 0x2;
				break;
			case 'r':
				opt |= 0x20;
				break;
			case 'N':
				opt |= 0x4;
				break;
			case 'n':
				opt |= 0x40;
				break;
			case 'S':
				opt |= 0x8;
				break;
			case 's':
				opt |= 0x80;
				break;
			case 'K':
				opt |= 0x100;
				break;
			case 'k':
				opt |= 0x1000;
				break;
			case 'H':
				opt |= 0x200;
				break;
			case 'h':
				opt |= 0x2000;
				break;
			case 'V':
				opt |= 0x800;
				break;
			case 'C':
				opt |= 0x10000;
				break;
			case 'c':
				opt |= 0x20000;
				break;
			case 'M':
				opt |= 0x100000;
				break;
			case 'm':
				opt |= 0x200000;
				break;
			}
		}
	} else {
		opt = 0x900;
	}

	/* encoding */
	if (encname != NULL) {
		string.no_encoding = mbfl_name2no_encoding(encname);
		if (string.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", encname);
			RETURN_FALSE;
		}
	}

	ret = mbfl_ja_jp_hantozen(&string, &result, opt);
	if (ret != NULL) {
		// TODO: avoid reallocation ???
		RETVAL_STRINGL((char *)ret->val, ret->len);		/* the string is already strdup()'ed */
		efree(ret->val);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

#define PHP_MBSTR_STACK_BLOCK_SIZE 32

/* {{{ proto string mb_convert_variables(string to-encoding, mixed from-encoding, mixed vars [, ...])
   Converts the string resource in variables to desired encoding */
PHP_FUNCTION(mb_convert_variables)
{
	zval *args, *stack, *var, *hash_entry, *hash_entry_ptr, *zfrom_enc;
	HashTable *target_hash;
	mbfl_string string, result, *ret;
	const mbfl_encoding *from_encoding, *to_encoding;
	mbfl_encoding_detector *identd;
	mbfl_buffer_converter *convd;
	int n, argc, stack_level, stack_max;
	size_t to_enc_len;
	size_t elistsz;
	const mbfl_encoding **elist;
	char *to_enc;
	void *ptmp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz+", &to_enc, &to_enc_len, &zfrom_enc, &args, &argc) == FAILURE) {
		return;
	}

	/* new encoding */
	to_encoding = mbfl_name2encoding(to_enc);
	if (!to_encoding) {
		php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", to_enc);
		RETURN_FALSE;
	}

	/* initialize string */
	mbfl_string_init(&string);
	mbfl_string_init(&result);
	from_encoding = MBSTRG(current_internal_encoding);
	string.no_encoding = from_encoding->no_encoding;
	string.no_language = MBSTRG(language);

	/* pre-conversion encoding */
	elist = NULL;
	elistsz = 0;
	switch (Z_TYPE_P(zfrom_enc)) {
		case IS_ARRAY:
			php_mb_parse_encoding_array(zfrom_enc, &elist, &elistsz, 0);
			break;
		default:
			convert_to_string_ex(zfrom_enc);
			php_mb_parse_encoding_list(Z_STRVAL_P(zfrom_enc), Z_STRLEN_P(zfrom_enc), &elist, &elistsz, 0);
			break;
	}

	if (elistsz <= 0) {
		from_encoding = &mbfl_encoding_pass;
	} else if (elistsz == 1) {
		from_encoding = *elist;
	} else {
		/* auto detect */
		from_encoding = NULL;
		stack_max = PHP_MBSTR_STACK_BLOCK_SIZE;
		stack = (zval *)safe_emalloc(stack_max, sizeof(zval), 0);
		stack_level = 0;
		identd = mbfl_encoding_detector_new2(elist, elistsz, MBSTRG(strict_detection));
		if (identd != NULL) {
			n = 0;
			while (n < argc || stack_level > 0) {
				if (stack_level <= 0) {
					var = &args[n++];
					ZVAL_DEREF(var);
					SEPARATE_ZVAL_NOREF(var);
					if (Z_TYPE_P(var) == IS_ARRAY || Z_TYPE_P(var) == IS_OBJECT) {
						target_hash = HASH_OF(var);
						if (target_hash != NULL) {
							zend_hash_internal_pointer_reset(target_hash);
						}
					}
				} else {
					stack_level--;
					var = &stack[stack_level];
				}
				if (Z_TYPE_P(var) == IS_ARRAY || Z_TYPE_P(var) == IS_OBJECT) {
					target_hash = HASH_OF(var);
					if (target_hash != NULL) {
						while ((hash_entry = zend_hash_get_current_data(target_hash)) != NULL) {
							zend_hash_move_forward(target_hash);
							if (Z_TYPE_P(hash_entry) == IS_INDIRECT) {
								hash_entry = Z_INDIRECT_P(hash_entry);
							}
							ZVAL_DEREF(hash_entry);
							if (Z_TYPE_P(hash_entry) == IS_ARRAY || Z_TYPE_P(hash_entry) == IS_OBJECT) {
								if (stack_level >= stack_max) {
									stack_max += PHP_MBSTR_STACK_BLOCK_SIZE;
									ptmp = erealloc(stack, sizeof(zval) * stack_max);
									stack = (zval *)ptmp;
								}
								ZVAL_COPY_VALUE(&stack[stack_level], var);
								stack_level++;
								var = hash_entry;
								target_hash = HASH_OF(var);
								if (target_hash != NULL) {
									zend_hash_internal_pointer_reset(target_hash);
									continue;
								}
							} else if (Z_TYPE_P(hash_entry) == IS_STRING) {
								string.val = (unsigned char *)Z_STRVAL_P(hash_entry);
								string.len = Z_STRLEN_P(hash_entry);
								if (mbfl_encoding_detector_feed(identd, &string)) {
									goto detect_end;		/* complete detecting */
								}
							}
						}
					}
				} else if (Z_TYPE_P(var) == IS_STRING) {
					string.val = (unsigned char *)Z_STRVAL_P(var);
					string.len = Z_STRLEN_P(var);
					if (mbfl_encoding_detector_feed(identd, &string)) {
						goto detect_end;		/* complete detecting */
					}
				}
			}
detect_end:
			from_encoding = mbfl_encoding_detector_judge2(identd);
			mbfl_encoding_detector_delete(identd);
		}
		efree(stack);

		if (!from_encoding) {
			php_error_docref(NULL, E_WARNING, "Unable to detect encoding");
			from_encoding = &mbfl_encoding_pass;
		}
	}
	if (elist != NULL) {
		efree((void *)elist);
	}
	/* create converter */
	convd = NULL;
	if (from_encoding != &mbfl_encoding_pass) {
		convd = mbfl_buffer_converter_new2(from_encoding, to_encoding, 0);
		if (convd == NULL) {
			php_error_docref(NULL, E_WARNING, "Unable to create converter");
			RETURN_FALSE;
		}
		mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode));
		mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar));
	}

	/* convert */
	if (convd != NULL) {
		stack_max = PHP_MBSTR_STACK_BLOCK_SIZE;
		stack = (zval*)safe_emalloc(stack_max, sizeof(zval), 0);
		stack_level = 0;
		n = 0;
		while (n < argc || stack_level > 0) {
			if (stack_level <= 0) {
				var = &args[n++];
				ZVAL_DEREF(var);
				SEPARATE_ZVAL_NOREF(var);
				if (Z_TYPE_P(var) == IS_ARRAY || Z_TYPE_P(var) == IS_OBJECT) {
					target_hash = HASH_OF(var);
					if (target_hash != NULL) {
						zend_hash_internal_pointer_reset(target_hash);
					}
				}
			} else {
				stack_level--;
				var = &stack[stack_level];
			}
			if (Z_TYPE_P(var) == IS_ARRAY || Z_TYPE_P(var) == IS_OBJECT) {
				target_hash = HASH_OF(var);
				if (target_hash != NULL) {
					while ((hash_entry_ptr = zend_hash_get_current_data(target_hash)) != NULL) {
						zend_hash_move_forward(target_hash);
						if (Z_TYPE_P(hash_entry_ptr) == IS_INDIRECT) {
							hash_entry_ptr = Z_INDIRECT_P(hash_entry_ptr);
						}
						hash_entry = hash_entry_ptr;
						ZVAL_DEREF(hash_entry);
						if (Z_TYPE_P(hash_entry) == IS_ARRAY || Z_TYPE_P(hash_entry) == IS_OBJECT) {
							if (stack_level >= stack_max) {
								stack_max += PHP_MBSTR_STACK_BLOCK_SIZE;
								ptmp = erealloc(stack, sizeof(zval) * stack_max);
								stack = (zval *)ptmp;
							}
							ZVAL_COPY_VALUE(&stack[stack_level], var);
							stack_level++;
							var = hash_entry;
							SEPARATE_ZVAL(hash_entry);
							target_hash = HASH_OF(var);
							if (target_hash != NULL) {
								zend_hash_internal_pointer_reset(target_hash);
								continue;
							}
						} else if (Z_TYPE_P(hash_entry) == IS_STRING) {
							string.val = (unsigned char *)Z_STRVAL_P(hash_entry);
							string.len = Z_STRLEN_P(hash_entry);
							ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
							if (ret != NULL) {
								zval_ptr_dtor(hash_entry_ptr);
								// TODO: avoid reallocation ???
								ZVAL_STRINGL(hash_entry_ptr, (char *)ret->val, ret->len);
								efree(ret->val);
							}
						}
					}
				}
			} else if (Z_TYPE_P(var) == IS_STRING) {
				string.val = (unsigned char *)Z_STRVAL_P(var);
				string.len = Z_STRLEN_P(var);
				ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
				if (ret != NULL) {
					zval_ptr_dtor(var);
					// TODO: avoid reallocation ???
					ZVAL_STRINGL(var, (char *)ret->val, ret->len);
					efree(ret->val);
				}
			}
		}
		efree(stack);

		MBSTRG(illegalchars) += mbfl_buffer_illegalchars(convd);
		mbfl_buffer_converter_delete(convd);
	}

	if (from_encoding) {
		RETURN_STRING(from_encoding->name);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ HTML numeric entity */
/* {{{ static void php_mb_numericentity_exec() */
static void
php_mb_numericentity_exec(INTERNAL_FUNCTION_PARAMETERS, int type)
{
	char *str, *encoding = NULL;
	size_t str_len, encoding_len;
	zval *zconvmap, *hash_entry;
	HashTable *target_hash;
	int i, *convmap, *mapelm, mapsize=0;
	zend_bool is_hex = 0;
	mbfl_string string, result, *ret;
	enum mbfl_no_encoding no_encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz|sb", &str, &str_len, &zconvmap, &encoding, &encoding_len, &is_hex) == FAILURE) {
		return;
	}

	mbfl_string_init(&string);
	string.no_language = MBSTRG(language);
	string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
	string.val = (unsigned char *)str;
	string.len = str_len;

	/* encoding */
	if (encoding && encoding_len > 0) {
		no_encoding = mbfl_name2no_encoding(encoding);
		if (no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", encoding);
			RETURN_FALSE;
		} else {
			string.no_encoding = no_encoding;
		}
	}

	if (type == 0 && is_hex) {
		type = 2; /* output in hex format */
	}

	/* conversion map */
	convmap = NULL;
	if (Z_TYPE_P(zconvmap) == IS_ARRAY) {
		target_hash = Z_ARRVAL_P(zconvmap);
		i = zend_hash_num_elements(target_hash);
		if (i > 0) {
			convmap = (int *)safe_emalloc(i, sizeof(int), 0);
			mapelm = convmap;
			mapsize = 0;
			ZEND_HASH_FOREACH_VAL(target_hash, hash_entry) {
				convert_to_long_ex(hash_entry);
				*mapelm++ = Z_LVAL_P(hash_entry);
				mapsize++;
			} ZEND_HASH_FOREACH_END();
		}
	}
	if (convmap == NULL) {
		RETURN_FALSE;
	}
	mapsize /= 4;

	ret = mbfl_html_numeric_entity(&string, &result, convmap, mapsize, type);
	if (ret != NULL) {
		// TODO: avoid reallocation ???
		RETVAL_STRINGL((char *)ret->val, ret->len);
		efree(ret->val);
	} else {
		RETVAL_FALSE;
	}
	efree((void *)convmap);
}
/* }}} */

/* {{{ proto string mb_encode_numericentity(string string, array convmap [, string encoding [, bool is_hex]])
   Converts specified characters to HTML numeric entities */
PHP_FUNCTION(mb_encode_numericentity)
{
	php_mb_numericentity_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string mb_decode_numericentity(string string, array convmap [, string encoding])
   Converts HTML numeric entities to character code */
PHP_FUNCTION(mb_decode_numericentity)
{
	php_mb_numericentity_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */
/* }}} */

/* {{{ proto int mb_send_mail(string to, string subject, string message [, string additional_headers [, string additional_parameters]])
 *  Sends an email message with MIME scheme
 */

#define SKIP_LONG_HEADER_SEP_MBSTRING(str, pos)										\
	if (str[pos] == '\r' && str[pos + 1] == '\n' && (str[pos + 2] == ' ' || str[pos + 2] == '\t')) {	\
		pos += 2;											\
		while (str[pos + 1] == ' ' || str[pos + 1] == '\t') {							\
			pos++;											\
		}												\
		continue;											\
	}

#define MAIL_ASCIIZ_CHECK_MBSTRING(str, len)			\
	pp = str;					\
	ee = pp + len;					\
	while ((pp = memchr(pp, '\0', (ee - pp)))) {	\
		*pp = ' ';				\
	}						\

static int _php_mbstr_parse_mail_headers(HashTable *ht, const char *str, size_t str_len)
{
	const char *ps;
	size_t icnt;
	int state = 0;
	int crlf_state = -1;
	char *token = NULL;
	size_t token_pos = 0;
	zend_string *fld_name, *fld_val;

	ps = str;
	icnt = str_len;
	fld_name = fld_val = NULL;

	/*
	 *             C o n t e n t - T y p e :   t e x t / h t m l \r\n
	 *             ^ ^^^^^^^^^^^^^^^^^^^^^ ^^^ ^^^^^^^^^^^^^^^^^ ^^^^
	 *      state  0            1           2          3
	 *
	 *             C o n t e n t - T y p e :   t e x t / h t m l \r\n
	 *             ^ ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ^^^^
	 * crlf_state -1                       0                     1 -1
	 *
	 */

	while (icnt > 0) {
		switch (*ps) {
			case ':':
				if (crlf_state == 1) {
					token_pos++;
				}

				if (state == 0 || state == 1) {
					if(token && token_pos > 0) {
						fld_name = zend_string_init(token, token_pos, 0);
					}
					state = 2;
				} else {
					token_pos++;
				}

				crlf_state = 0;
				break;

			case '\n':
				if (crlf_state == -1) {
					goto out;
				}
				crlf_state = -1;
				break;

			case '\r':
				if (crlf_state == 1) {
					token_pos++;
				} else {
					crlf_state = 1;
				}
				break;

			case ' ': case '\t':
				if (crlf_state == -1) {
					if (state == 3) {
						/* continuing from the previous line */
						state = 4;
					} else {
						/* simply skipping this new line */
						state = 5;
					}
				} else {
					if (crlf_state == 1) {
						token_pos++;
					}
					if (state == 1 || state == 3) {
						token_pos++;
					}
				}
				crlf_state = 0;
				break;

			default:
				switch (state) {
					case 0:
						token = (char*)ps;
						token_pos = 0;
						state = 1;
						break;

					case 2:
						if (crlf_state != -1) {
							token = (char*)ps;
							token_pos = 0;

							state = 3;
							break;
						}
						/* break is missing intentionally */

					case 3:
						if (crlf_state == -1) {
							if(token && token_pos > 0) {
								fld_val = zend_string_init(token, token_pos, 0);
							}

							if (fld_name != NULL && fld_val != NULL) {
								zval val;
								/* FIXME: some locale free implementation is
								 * really required here,,, */
								php_strtoupper(ZSTR_VAL(fld_name), ZSTR_LEN(fld_name));
								ZVAL_STR(&val, fld_val);

								zend_hash_update(ht, fld_name, &val);

								zend_string_release(fld_name);
							}

							fld_name = fld_val = NULL;
							token = (char*)ps;
							token_pos = 0;

							state = 1;
						}
						break;

					case 4:
						token_pos++;
						state = 3;
						break;
				}

				if (crlf_state == 1) {
					token_pos++;
				}

				token_pos++;

				crlf_state = 0;
				break;
		}
		ps++, icnt--;
	}
out:
	if (state == 2) {
		token = "";
		token_pos = 0;

		state = 3;
	}
	if (state == 3) {
		if(token && token_pos > 0) {
			fld_val = zend_string_init(token, token_pos, 0);
		}
		if (fld_name != NULL && fld_val != NULL) {
			zval val;
			/* FIXME: some locale free implementation is
			 * really required here,,, */
			php_strtoupper(ZSTR_VAL(fld_name), ZSTR_LEN(fld_name));
			ZVAL_STR(&val, fld_val);

			zend_hash_update(ht, fld_name, &val);

			zend_string_release(fld_name);
		}
	}
	return state;
}

PHP_FUNCTION(mb_send_mail)
{
	int n;
	char *to = NULL;
	size_t to_len;
	char *message = NULL;
	size_t message_len;
	char *headers = NULL;
	size_t headers_len;
	char *subject = NULL;
	zend_string *extra_cmd = NULL;
	size_t subject_len;
	int i;
	char *to_r = NULL;
	char *force_extra_parameters = INI_STR("mail.force_extra_parameters");
	struct {
		int cnt_type:1;
		int cnt_trans_enc:1;
	} suppressed_hdrs = { 0, 0 };

	char *message_buf = NULL, *subject_buf = NULL, *p;
	mbfl_string orig_str, conv_str;
	mbfl_string *pstr;	/* pointer to mbfl string for return value */
	enum mbfl_no_encoding
		tran_cs,	/* transfar text charset */
		head_enc,	/* header transfar encoding */
		body_enc;	/* body transfar encoding */
	mbfl_memory_device device;	/* automatic allocateable buffer for additional header */
	const mbfl_language *lang;
	int err = 0;
	HashTable ht_headers;
	zval *s;
	extern void mbfl_memory_device_unput(mbfl_memory_device *device);
	char *pp, *ee;

	/* initialize */
	mbfl_memory_device_init(&device, 0, 0);
	mbfl_string_init(&orig_str);
	mbfl_string_init(&conv_str);

	/* character-set, transfer-encoding */
	tran_cs = mbfl_no_encoding_utf8;
	head_enc = mbfl_no_encoding_base64;
	body_enc = mbfl_no_encoding_base64;
	lang = mbfl_no2language(MBSTRG(language));
	if (lang != NULL) {
		tran_cs = lang->mail_charset;
		head_enc = lang->mail_header_encoding;
		body_enc = lang->mail_body_encoding;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss|sS", &to, &to_len, &subject, &subject_len, &message, &message_len, &headers, &headers_len, &extra_cmd) == FAILURE) {
		return;
	}

	/* ASCIIZ check */
	MAIL_ASCIIZ_CHECK_MBSTRING(to, to_len);
	MAIL_ASCIIZ_CHECK_MBSTRING(subject, subject_len);
	MAIL_ASCIIZ_CHECK_MBSTRING(message, message_len);
	if (headers) {
		MAIL_ASCIIZ_CHECK_MBSTRING(headers, headers_len);
	}
	if (extra_cmd) {
		MAIL_ASCIIZ_CHECK_MBSTRING(ZSTR_VAL(extra_cmd), ZSTR_LEN(extra_cmd));
	}

	zend_hash_init(&ht_headers, 0, NULL, ZVAL_PTR_DTOR, 0);

	if (headers != NULL) {
		_php_mbstr_parse_mail_headers(&ht_headers, headers, headers_len);
	}

	if ((s = zend_hash_str_find(&ht_headers, "CONTENT-TYPE", sizeof("CONTENT-TYPE") - 1))) {
		char *tmp;
		char *param_name;
		char *charset = NULL;

		ZEND_ASSERT(Z_TYPE_P(s) == IS_STRING);
		p = strchr(Z_STRVAL_P(s), ';');

		if (p != NULL) {
			/* skipping the padded spaces */
			do {
				++p;
			} while (*p == ' ' || *p == '\t');

			if (*p != '\0') {
				if ((param_name = php_strtok_r(p, "= ", &tmp)) != NULL) {
					if (strcasecmp(param_name, "charset") == 0) {
						enum mbfl_no_encoding _tran_cs = tran_cs;

						charset = php_strtok_r(NULL, "= \"", &tmp);
						if (charset != NULL) {
							_tran_cs = mbfl_name2no_encoding(charset);
						}

						if (_tran_cs == mbfl_no_encoding_invalid) {
							php_error_docref(NULL, E_WARNING, "Unsupported charset \"%s\" - will be regarded as ascii", charset);
							_tran_cs = mbfl_no_encoding_ascii;
						}
						tran_cs = _tran_cs;
					}
				}
			}
		}
		suppressed_hdrs.cnt_type = 1;
	}

	if ((s = zend_hash_str_find(&ht_headers, "CONTENT-TRANSFER-ENCODING", sizeof("CONTENT-TRANSFER-ENCODING") - 1))) {
		enum mbfl_no_encoding _body_enc;

		ZEND_ASSERT(Z_TYPE_P(s) == IS_STRING);
		_body_enc = mbfl_name2no_encoding(Z_STRVAL_P(s));
		switch (_body_enc) {
			case mbfl_no_encoding_base64:
			case mbfl_no_encoding_7bit:
			case mbfl_no_encoding_8bit:
				body_enc = _body_enc;
				break;

			default:
				php_error_docref(NULL, E_WARNING, "Unsupported transfer encoding \"%s\" - will be regarded as 8bit", Z_STRVAL_P(s));
				body_enc =	mbfl_no_encoding_8bit;
				break;
		}
		suppressed_hdrs.cnt_trans_enc = 1;
	}

	/* To: */
	if (to != NULL) {
		if (to_len > 0) {
			to_r = estrndup(to, to_len);
			for (; to_len; to_len--) {
				if (!isspace((unsigned char) to_r[to_len - 1])) {
					break;
				}
				to_r[to_len - 1] = '\0';
			}
			for (i = 0; to_r[i]; i++) {
			if (iscntrl((unsigned char) to_r[i])) {
				/* According to RFC 822, section 3.1.1 long headers may be separated into
				 * parts using CRLF followed at least one linear-white-space character ('\t' or ' ').
				 * To prevent these separators from being replaced with a space, we use the
				 * SKIP_LONG_HEADER_SEP_MBSTRING to skip over them.
				 */
				SKIP_LONG_HEADER_SEP_MBSTRING(to_r, i);
				to_r[i] = ' ';
			}
			}
		} else {
			to_r = to;
		}
	} else {
		php_error_docref(NULL, E_WARNING, "Missing To: field");
		err = 1;
	}

	/* Subject: */
	if (subject != NULL) {
		orig_str.no_language = MBSTRG(language);
		orig_str.val = (unsigned char *)subject;
		orig_str.len = subject_len;
		orig_str.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
		if (orig_str.no_encoding == mbfl_no_encoding_invalid || orig_str.no_encoding == mbfl_no_encoding_pass) {
			const mbfl_encoding *encoding = mbfl_identify_encoding2(&orig_str, MBSTRG(current_detect_order_list), MBSTRG(current_detect_order_list_size), MBSTRG(strict_detection));
			orig_str.no_encoding = encoding ? encoding->no_encoding: mbfl_no_encoding_invalid;
		}
		pstr = mbfl_mime_header_encode(&orig_str, &conv_str, tran_cs, head_enc, "\n", sizeof("Subject: [PHP-jp nnnnnnnn]"));
		if (pstr != NULL) {
			subject_buf = subject = (char *)pstr->val;
		}
	} else {
		php_error_docref(NULL, E_WARNING, "Missing Subject: field");
		err = 1;
	}

	/* message body */
	if (message != NULL) {
		orig_str.no_language = MBSTRG(language);
		orig_str.val = (unsigned char *)message;
		orig_str.len = (unsigned int)message_len;
		orig_str.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;

		if (orig_str.no_encoding == mbfl_no_encoding_invalid || orig_str.no_encoding == mbfl_no_encoding_pass) {
			const mbfl_encoding *encoding = mbfl_identify_encoding2(&orig_str, MBSTRG(current_detect_order_list), MBSTRG(current_detect_order_list_size), MBSTRG(strict_detection));
			orig_str.no_encoding = encoding ? encoding->no_encoding: mbfl_no_encoding_invalid;
		}

		pstr = NULL;
		{
			mbfl_string tmpstr;

			if (mbfl_convert_encoding(&orig_str, &tmpstr, tran_cs) != NULL) {
				tmpstr.no_encoding=mbfl_no_encoding_8bit;
				pstr = mbfl_convert_encoding(&tmpstr, &conv_str, body_enc);
				efree(tmpstr.val);
			}
		}
		if (pstr != NULL) {
			message_buf = message = (char *)pstr->val;
		}
	} else {
		/* this is not really an error, so it is allowed. */
		php_error_docref(NULL, E_WARNING, "Empty message body");
		message = NULL;
	}

	/* other headers */
#define PHP_MBSTR_MAIL_MIME_HEADER1 "MIME-Version: 1.0"
#define PHP_MBSTR_MAIL_MIME_HEADER2 "Content-Type: text/plain"
#define PHP_MBSTR_MAIL_MIME_HEADER3 "; charset="
#define PHP_MBSTR_MAIL_MIME_HEADER4 "Content-Transfer-Encoding: "
	if (headers != NULL) {
		p = headers;
		n = headers_len;
		mbfl_memory_device_strncat(&device, p, n);
		if (n > 0 && p[n - 1] != '\n') {
			mbfl_memory_device_strncat(&device, "\n", 1);
		}
	}

	if (!zend_hash_str_exists(&ht_headers, "MIME-VERSION", sizeof("MIME-VERSION") - 1)) {
		mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER1, sizeof(PHP_MBSTR_MAIL_MIME_HEADER1) - 1);
		mbfl_memory_device_strncat(&device, "\n", 1);
	}

	if (!suppressed_hdrs.cnt_type) {
		mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER2, sizeof(PHP_MBSTR_MAIL_MIME_HEADER2) - 1);

		p = (char *)mbfl_no2preferred_mime_name(tran_cs);
		if (p != NULL) {
			mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER3, sizeof(PHP_MBSTR_MAIL_MIME_HEADER3) - 1);
			mbfl_memory_device_strcat(&device, p);
		}
		mbfl_memory_device_strncat(&device, "\n", 1);
	}
	if (!suppressed_hdrs.cnt_trans_enc) {
		mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER4, sizeof(PHP_MBSTR_MAIL_MIME_HEADER4) - 1);
		p = (char *)mbfl_no2preferred_mime_name(body_enc);
		if (p == NULL) {
			p = "7bit";
		}
		mbfl_memory_device_strcat(&device, p);
		mbfl_memory_device_strncat(&device, "\n", 1);
	}

	mbfl_memory_device_unput(&device);
	mbfl_memory_device_output('\0', &device);
	headers = (char *)device.buffer;

	if (force_extra_parameters) {
		extra_cmd = php_escape_shell_cmd(force_extra_parameters);
	} else if (extra_cmd) {
		extra_cmd = php_escape_shell_cmd(ZSTR_VAL(extra_cmd));
	}

	if (!err && php_mail(to_r, subject, message, headers, extra_cmd ? ZSTR_VAL(extra_cmd) : NULL)) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}

	if (extra_cmd) {
		zend_string_release(extra_cmd);
	}

	if (to_r != to) {
		efree(to_r);
	}
	if (subject_buf) {
		efree((void *)subject_buf);
	}
	if (message_buf) {
		efree((void *)message_buf);
	}
	mbfl_memory_device_clear(&device);
	zend_hash_destroy(&ht_headers);
}

#undef SKIP_LONG_HEADER_SEP_MBSTRING
#undef MAIL_ASCIIZ_CHECK_MBSTRING
#undef PHP_MBSTR_MAIL_MIME_HEADER1
#undef PHP_MBSTR_MAIL_MIME_HEADER2
#undef PHP_MBSTR_MAIL_MIME_HEADER3
#undef PHP_MBSTR_MAIL_MIME_HEADER4
/* }}} */

/* {{{ proto mixed mb_get_info([string type])
   Returns the current settings of mbstring */
PHP_FUNCTION(mb_get_info)
{
	char *typ = NULL;
	size_t typ_len;
	size_t n;
	char *name;
	const struct mb_overload_def *over_func;
	zval row1, row2;
	const mbfl_language *lang = mbfl_no2language(MBSTRG(language));
	const mbfl_encoding **entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &typ, &typ_len) == FAILURE) {
		return;
	}

	if (!typ || !strcasecmp("all", typ)) {
		array_init(return_value);
		if (MBSTRG(current_internal_encoding)) {
			add_assoc_string(return_value, "internal_encoding", (char *)MBSTRG(current_internal_encoding)->name);
		}
		if (MBSTRG(http_input_identify)) {
			add_assoc_string(return_value, "http_input", (char *)MBSTRG(http_input_identify)->name);
		}
		if (MBSTRG(current_http_output_encoding)) {
			add_assoc_string(return_value, "http_output", (char *)MBSTRG(current_http_output_encoding)->name);
		}
		if ((name = (char *)zend_ini_string("mbstring.http_output_conv_mimetypes", sizeof("mbstring.http_output_conv_mimetypes") - 1, 0)) != NULL) {
			add_assoc_string(return_value, "http_output_conv_mimetypes", name);
		}
		add_assoc_long(return_value, "func_overload", MBSTRG(func_overload));
		if (MBSTRG(func_overload)){
			over_func = &(mb_ovld[0]);
			array_init(&row1);
			while (over_func->type > 0) {
				if ((MBSTRG(func_overload) & over_func->type) == over_func->type ) {
					add_assoc_string(&row1, over_func->orig_func, over_func->ovld_func);
				}
				over_func++;
			}
			add_assoc_zval(return_value, "func_overload_list", &row1);
		} else {
			add_assoc_string(return_value, "func_overload_list", "no overload");
 		}
		if (lang != NULL) {
			if ((name = (char *)mbfl_no_encoding2name(lang->mail_charset)) != NULL) {
				add_assoc_string(return_value, "mail_charset", name);
			}
			if ((name = (char *)mbfl_no_encoding2name(lang->mail_header_encoding)) != NULL) {
				add_assoc_string(return_value, "mail_header_encoding", name);
			}
			if ((name = (char *)mbfl_no_encoding2name(lang->mail_body_encoding)) != NULL) {
				add_assoc_string(return_value, "mail_body_encoding", name);
			}
		}
		add_assoc_long(return_value, "illegal_chars", MBSTRG(illegalchars));
		if (MBSTRG(encoding_translation)) {
			add_assoc_string(return_value, "encoding_translation", "On");
		} else {
			add_assoc_string(return_value, "encoding_translation", "Off");
		}
		if ((name = (char *)mbfl_no_language2name(MBSTRG(language))) != NULL) {
			add_assoc_string(return_value, "language", name);
		}
		n = MBSTRG(current_detect_order_list_size);
		entry = MBSTRG(current_detect_order_list);
		if (n > 0) {
			size_t i;
			array_init(&row2);
			for (i = 0; i < n; i++) {
				add_next_index_string(&row2, (*entry)->name);
				entry++;
			}
			add_assoc_zval(return_value, "detect_order", &row2);
		}
		if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			add_assoc_string(return_value, "substitute_character", "none");
		} else if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG) {
			add_assoc_string(return_value, "substitute_character", "long");
		} else if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY) {
			add_assoc_string(return_value, "substitute_character", "entity");
		} else {
			add_assoc_long(return_value, "substitute_character", MBSTRG(current_filter_illegal_substchar));
		}
		if (MBSTRG(strict_detection)) {
			add_assoc_string(return_value, "strict_detection", "On");
		} else {
			add_assoc_string(return_value, "strict_detection", "Off");
		}
	} else if (!strcasecmp("internal_encoding", typ)) {
		if (MBSTRG(current_internal_encoding)) {
			RETVAL_STRING((char *)MBSTRG(current_internal_encoding)->name);
		}
	} else if (!strcasecmp("http_input", typ)) {
		if (MBSTRG(http_input_identify)) {
			RETVAL_STRING((char *)MBSTRG(http_input_identify)->name);
		}
	} else if (!strcasecmp("http_output", typ)) {
		if (MBSTRG(current_http_output_encoding)) {
			RETVAL_STRING((char *)MBSTRG(current_http_output_encoding)->name);
		}
	} else if (!strcasecmp("http_output_conv_mimetypes", typ)) {
		if ((name = (char *)zend_ini_string("mbstring.http_output_conv_mimetypes", sizeof("mbstring.http_output_conv_mimetypes") - 1, 0)) != NULL) {
			RETVAL_STRING(name);
		}
	} else if (!strcasecmp("func_overload", typ)) {
 		RETVAL_LONG(MBSTRG(func_overload));
	} else if (!strcasecmp("func_overload_list", typ)) {
		if (MBSTRG(func_overload)){
				over_func = &(mb_ovld[0]);
				array_init(return_value);
				while (over_func->type > 0) {
					if ((MBSTRG(func_overload) & over_func->type) == over_func->type ) {
						add_assoc_string(return_value, over_func->orig_func, over_func->ovld_func);
					}
					over_func++;
				}
		} else {
			RETVAL_STRING("no overload");
		}
	} else if (!strcasecmp("mail_charset", typ)) {
		if (lang != NULL && (name = (char *)mbfl_no_encoding2name(lang->mail_charset)) != NULL) {
			RETVAL_STRING(name);
		}
	} else if (!strcasecmp("mail_header_encoding", typ)) {
		if (lang != NULL && (name = (char *)mbfl_no_encoding2name(lang->mail_header_encoding)) != NULL) {
			RETVAL_STRING(name);
		}
	} else if (!strcasecmp("mail_body_encoding", typ)) {
		if (lang != NULL && (name = (char *)mbfl_no_encoding2name(lang->mail_body_encoding)) != NULL) {
			RETVAL_STRING(name);
		}
	} else if (!strcasecmp("illegal_chars", typ)) {
		RETVAL_LONG(MBSTRG(illegalchars));
	} else if (!strcasecmp("encoding_translation", typ)) {
		if (MBSTRG(encoding_translation)) {
			RETVAL_STRING("On");
		} else {
			RETVAL_STRING("Off");
		}
	} else if (!strcasecmp("language", typ)) {
		if ((name = (char *)mbfl_no_language2name(MBSTRG(language))) != NULL) {
			RETVAL_STRING(name);
		}
	} else if (!strcasecmp("detect_order", typ)) {
		n = MBSTRG(current_detect_order_list_size);
		entry = MBSTRG(current_detect_order_list);
		if (n > 0) {
			size_t i;
			array_init(return_value);
			for (i = 0; i < n; i++) {
				add_next_index_string(return_value, (*entry)->name);
				entry++;
			}
		}
	} else if (!strcasecmp("substitute_character", typ)) {
		if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			RETVAL_STRING("none");
		} else if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG) {
			RETVAL_STRING("long");
		} else if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY) {
			RETVAL_STRING("entity");
		} else {
			RETVAL_LONG(MBSTRG(current_filter_illegal_substchar));
		}
	} else if (!strcasecmp("strict_detection", typ)) {
		if (MBSTRG(strict_detection)) {
			RETVAL_STRING("On");
		} else {
			RETVAL_STRING("Off");
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool mb_check_encoding([string var[, string encoding]])
   Check if the string is valid for the specified encoding */
PHP_FUNCTION(mb_check_encoding)
{
	char *var = NULL;
	size_t var_len;
	char *enc = NULL;
	size_t enc_len;
	mbfl_buffer_converter *convd;
	const mbfl_encoding *encoding = MBSTRG(current_internal_encoding);
	mbfl_string string, result, *ret = NULL;
	long illegalchars = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|ss", &var, &var_len, &enc, &enc_len) == FAILURE) {
		return;
	}

	if (var == NULL) {
		RETURN_BOOL(MBSTRG(illegalchars) == 0);
	}

	if (enc != NULL) {
		encoding = mbfl_name2encoding(enc);
		if (!encoding || encoding == &mbfl_encoding_pass) {
			php_error_docref(NULL, E_WARNING, "Invalid encoding \"%s\"", enc);
			RETURN_FALSE;
		}
	}

	convd = mbfl_buffer_converter_new2(encoding, encoding, 0);
	if (convd == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to create converter");
		RETURN_FALSE;
	}
	mbfl_buffer_converter_illegal_mode(convd, MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE);
	mbfl_buffer_converter_illegal_substchar(convd, 0);

	/* initialize string */
	mbfl_string_init_set(&string, mbfl_no_language_neutral, encoding->no_encoding);
	mbfl_string_init(&result);

	string.val = (unsigned char *)var;
	string.len = var_len;
	ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
	illegalchars = mbfl_buffer_illegalchars(convd);
	mbfl_buffer_converter_delete(convd);

	RETVAL_FALSE;
	if (ret != NULL) {
		if (illegalchars == 0 && string.len == result.len && memcmp(string.val, result.val, string.len) == 0) {
			RETVAL_TRUE;
		}
		mbfl_string_clear(&result);
	}
}
/* }}} */

/* {{{ php_mb_populate_current_detect_order_list */
static void php_mb_populate_current_detect_order_list(void)
{
	const mbfl_encoding **entry = 0;
	size_t nentries;

	if (MBSTRG(current_detect_order_list)) {
		return;
	}

	if (MBSTRG(detect_order_list) && MBSTRG(detect_order_list_size)) {
		nentries = MBSTRG(detect_order_list_size);
		entry = (const mbfl_encoding **)safe_emalloc(nentries, sizeof(mbfl_encoding*), 0);
		memcpy(entry, MBSTRG(detect_order_list), sizeof(mbfl_encoding*) * nentries);
	} else {
		const enum mbfl_no_encoding *src = MBSTRG(default_detect_order_list);
		size_t i;
		nentries = MBSTRG(default_detect_order_list_size);
		entry = (const mbfl_encoding **)safe_emalloc(nentries, sizeof(mbfl_encoding*), 0);
		for (i = 0; i < nentries; i++) {
			entry[i] = mbfl_no2encoding(src[i]);
		}
	}
	MBSTRG(current_detect_order_list) = entry;
	MBSTRG(current_detect_order_list_size) = nentries;
}
/* }}} */

/* {{{ static int php_mb_encoding_translation() */
static int php_mb_encoding_translation(void)
{
	return MBSTRG(encoding_translation);
}
/* }}} */

/* {{{ MBSTRING_API size_t php_mb_mbchar_bytes_ex() */
MBSTRING_API size_t php_mb_mbchar_bytes_ex(const char *s, const mbfl_encoding *enc)
{
	if (enc != NULL) {
		if (enc->flag & MBFL_ENCTYPE_MBCS) {
			if (enc->mblen_table != NULL) {
				if (s != NULL) return enc->mblen_table[*(unsigned char *)s];
			}
		} else if (enc->flag & (MBFL_ENCTYPE_WCS2BE | MBFL_ENCTYPE_WCS2LE)) {
			return 2;
		} else if (enc->flag & (MBFL_ENCTYPE_WCS4BE | MBFL_ENCTYPE_WCS4LE)) {
			return 4;
		}
	}
	return 1;
}
/* }}} */

/* {{{ MBSTRING_API size_t php_mb_mbchar_bytes() */
MBSTRING_API size_t php_mb_mbchar_bytes(const char *s)
{
	return php_mb_mbchar_bytes_ex(s, MBSTRG(internal_encoding));
}
/* }}} */

/* {{{ MBSTRING_API char *php_mb_safe_strrchr_ex() */
MBSTRING_API char *php_mb_safe_strrchr_ex(const char *s, unsigned int c, size_t nbytes, const mbfl_encoding *enc)
{
	register const char *p = s;
	char *last=NULL;

	if (nbytes == (size_t)-1) {
		size_t nb = 0;

		while (*p != '\0') {
			if (nb == 0) {
				if ((unsigned char)*p == (unsigned char)c) {
					last = (char *)p;
				}
				nb = php_mb_mbchar_bytes_ex(p, enc);
				if (nb == 0) {
					return NULL; /* something is going wrong! */
				}
			}
			--nb;
			++p;
		}
	} else {
		register size_t bcnt = nbytes;
		register size_t nbytes_char;
		while (bcnt > 0) {
			if ((unsigned char)*p == (unsigned char)c) {
				last = (char *)p;
			}
			nbytes_char = php_mb_mbchar_bytes_ex(p, enc);
			if (bcnt < nbytes_char) {
				return NULL;
			}
			p += nbytes_char;
			bcnt -= nbytes_char;
		}
	}
	return last;
}
/* }}} */

/* {{{ MBSTRING_API char *php_mb_safe_strrchr() */
MBSTRING_API char *php_mb_safe_strrchr(const char *s, unsigned int c, size_t nbytes)
{
	return php_mb_safe_strrchr_ex(s, c, nbytes, MBSTRG(internal_encoding));
}
/* }}} */

/* {{{ MBSTRING_API int php_mb_stripos()
 */
MBSTRING_API int php_mb_stripos(int mode, const char *old_haystack, unsigned int old_haystack_len, const char *old_needle, unsigned int old_needle_len, long offset, const char *from_encoding)
{
	int n;
	mbfl_string haystack, needle;
	n = -1;

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
	needle.no_language = MBSTRG(language);
	needle.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;

	do {
		size_t len = 0;
		haystack.val = (unsigned char *)php_unicode_convert_case(PHP_UNICODE_CASE_UPPER, (char *)old_haystack, old_haystack_len, &len, from_encoding);
		haystack.len = len;

		if (!haystack.val) {
			break;
		}

		if (haystack.len <= 0) {
			break;
		}

		needle.val = (unsigned char *)php_unicode_convert_case(PHP_UNICODE_CASE_UPPER, (char *)old_needle, old_needle_len, &len, from_encoding);
		needle.len = len;

		if (!needle.val) {
			break;
		}

		if (needle.len <= 0) {
			break;
		}

		haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(from_encoding);
		if (haystack.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", from_encoding);
			break;
		}

 		{
 			int haystack_char_len = mbfl_strlen(&haystack);

 			if (mode) {
 				if ((offset > 0 && offset > haystack_char_len) ||
 					(offset < 0 && -offset > haystack_char_len)) {
 					php_error_docref(NULL, E_WARNING, "Offset is greater than the length of haystack string");
 					break;
 				}
 			} else {
 				if (offset < 0 || offset > haystack_char_len) {
 					php_error_docref(NULL, E_WARNING, "Offset not contained in string");
 					break;
 				}
 			}
		}

		n = mbfl_strpos(&haystack, &needle, offset, mode);
	} while(0);

	if (haystack.val) {
		efree(haystack.val);
	}

	if (needle.val) {
		efree(needle.val);
	}

	return n;
}
/* }}} */

static void php_mb_gpc_get_detect_order(const zend_encoding ***list, size_t *list_size) /* {{{ */
{
	*list = (const zend_encoding **)MBSTRG(http_input_list);
	*list_size = MBSTRG(http_input_list_size);
}
/* }}} */

static void php_mb_gpc_set_input_encoding(const zend_encoding *encoding) /* {{{ */
{
	MBSTRG(http_input_identify) = (const mbfl_encoding*)encoding;
}
/* }}} */

#endif	/* HAVE_MBSTRING */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
