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
   | Author: Tsukada Takuya <tsukada@fminn.nagano.nagano.jp>              |
   |         Rui Hirokawa <hirokawa@php.net>                              |
   |         Hironori Sato <satoh@jpnnet.com>                             |
   |         Shigeru Kanemoto <sgk@happysize.co.jp>                       |
   +----------------------------------------------------------------------+
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
#include "libmbfl/mbfl/mbfilter_8bit.h"
#include "libmbfl/mbfl/mbfilter_pass.h"
#include "libmbfl/mbfl/mbfilter_wchar.h"
#include "libmbfl/filters/mbfilter_ascii.h"
#include "libmbfl/filters/mbfilter_base64.h"
#include "libmbfl/filters/mbfilter_qprint.h"
#include "libmbfl/filters/mbfilter_ucs4.h"
#include "libmbfl/filters/mbfilter_utf8.h"

#include "php_variables.h"
#include "php_globals.h"
#include "rfc1867.h"
#include "php_content_types.h"
#include "SAPI.h"
#include "php_unicode.h"
#include "TSRM.h"

#include "mb_gpc.h"

#if HAVE_MBREGEX
# include "php_mbregex.h"
# include "php_onig_compat.h"
# include <oniguruma.h>
# undef UChar
#if ONIGURUMA_VERSION_INT < 60800
typedef void OnigMatchParam;
#define onig_new_match_param() (NULL)
#define onig_initialize_match_param(x) (void)(x)
#define onig_set_match_stack_limit_size_of_match_param(x, y)
#define onig_set_retry_limit_in_match_of_match_param(x, y)
#define onig_free_match_param(x)
#define onig_search_with_param(reg, str, end, start, range, region, option, mp) \
onig_search(reg, str, end, start, range, region, option)
#define onig_match_with_param(re, str, end, at, region, option, mp) \
onig_match(re, str, end, at, region, option)
#endif
#else
# include "ext/pcre/php_pcre.h"
#endif

#include "zend_multibyte.h"
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

static inline zend_bool php_mb_is_unsupported_no_encoding(enum mbfl_no_encoding no_enc);

static inline zend_bool php_mb_is_no_encoding_utf8(enum mbfl_no_encoding no_enc);
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_str_split, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, split_length)
	ZEND_ARG_INFO(0, encoding)
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
	ZEND_ARG_INFO(0, is_hex)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_scrub, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_ord, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_mb_chr, 0, 0, 1)
	ZEND_ARG_INFO(0, cp)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

#if HAVE_MBREGEX
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
	ZEND_ARG_INFO(0, option)
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
#endif /* HAVE_MBREGEX */
/* }}} */

/* {{{ zend_function_entry mbstring_functions[] */
static const zend_function_entry mbstring_functions[] = {
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
	PHP_FE(mb_str_split,			arginfo_mb_str_split)
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
	PHP_FE(mb_ord,					arginfo_mb_ord)
	PHP_FE(mb_chr,					arginfo_mb_chr)
	PHP_FE(mb_scrub,				arginfo_mb_scrub)
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
static const sapi_post_entry php_post_entries[] = {
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

/* {{{ allocators */
static void *_php_mb_allocators_malloc(size_t sz)
{
	return emalloc(sz);
}

static void *_php_mb_allocators_realloc(void *ptr, size_t sz)
{
	return erealloc(ptr, sz);
}

static void *_php_mb_allocators_calloc(size_t nelems, size_t szelem)
{
	return ecalloc(nelems, szelem);
}

static void _php_mb_allocators_free(void *ptr)
{
	efree(ptr);
}

static void *_php_mb_allocators_pmalloc(size_t sz)
{
	return pemalloc(sz, 1);
}

static void *_php_mb_allocators_prealloc(void *ptr, size_t sz)
{
	return perealloc(ptr, sz, 1);
}

static void _php_mb_allocators_pfree(void *ptr)
{
	pefree(ptr, 1);
}

static const mbfl_allocators _php_mb_allocators = {
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
static const sapi_post_entry mbstr_post_entries[] = {
	{ DEFAULT_POST_CONTENT_TYPE, sizeof(DEFAULT_POST_CONTENT_TYPE)-1, sapi_read_standard_form_data, php_mb_post_handler },
	{ MULTIPART_CONTENT_TYPE,    sizeof(MULTIPART_CONTENT_TYPE)-1,    NULL,                         rfc1867_post_handler },
	{ NULL, 0, NULL, NULL }
};
/* }}} */

static const mbfl_encoding *php_mb_get_encoding(zend_string *encoding_name) {
	if (encoding_name) {
		const mbfl_encoding *encoding;
		zend_string *last_encoding_name = MBSTRG(last_used_encoding_name);
		if (last_encoding_name && (last_encoding_name == encoding_name
				|| !strcasecmp(ZSTR_VAL(encoding_name), ZSTR_VAL(last_encoding_name)))) {
			return MBSTRG(last_used_encoding);
		}

		encoding = mbfl_name2encoding(ZSTR_VAL(encoding_name));
		if (!encoding) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", ZSTR_VAL(encoding_name));
			return NULL;
		}

		if (last_encoding_name) {
			zend_string_release(last_encoding_name);
		}
		MBSTRG(last_used_encoding_name) = zend_string_copy(encoding_name);
		MBSTRG(last_used_encoding) = encoding;
		return encoding;
	} else {
		return MBSTRG(current_internal_encoding);
	}
}

/* {{{ static int php_mb_parse_encoding_list()
 *  Return FAILURE if input contains any illegal encoding, otherwise SUCCESS.
 *  Even if any illegal encoding is detected the result may contain a list
 *  of parsed encodings.
 */
static int
php_mb_parse_encoding_list(const char *value, size_t value_length, const mbfl_encoding ***return_list, size_t *return_size, int persistent)
{
	int bauto, ret = SUCCESS;
	size_t n, size;
	char *p, *p1, *p2, *endp, *tmpstr;
	const mbfl_encoding **entry, **list;

	list = NULL;
	if (value == NULL || value_length == 0) {
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
					ret = FAILURE;
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
			ret = FAILURE;
		}
		if (return_size) {
			*return_size = n;
		}
		efree(tmpstr);
	}

	return ret;
}
/* }}} */

/* {{{ static int php_mb_parse_encoding_array()
 *  Return FAILURE if input contains any illegal encoding, otherwise SUCCESS.
 *  Even if any illegal encoding is detected the result may contain a list
 *  of parsed encodings.
 */
static int
php_mb_parse_encoding_array(zval *array, const mbfl_encoding ***return_list, size_t *return_size, int persistent)
{
	zval *hash_entry;
	HashTable *target_hash;
	int i, n, bauto, ret = SUCCESS;
	const mbfl_encoding **list, **entry;
	size_t size;

	list = NULL;
	if (Z_TYPE_P(array) == IS_ARRAY) {
		target_hash = Z_ARRVAL_P(array);
		i = zend_hash_num_elements(target_hash);
		size = i + MBSTRG(default_detect_order_list_size);
		list = (const mbfl_encoding **)pecalloc(size, sizeof(mbfl_encoding*), persistent);
		entry = list;
		bauto = 0;
		n = 0;
		ZEND_HASH_FOREACH_VAL(target_hash, hash_entry) {
			zend_string *encoding_str = zval_try_get_string(hash_entry);
			if (UNEXPECTED(!encoding_str)) {
				ret = FAILURE;
				break;
			}

			if (strcasecmp(ZSTR_VAL(encoding_str), "auto") == 0) {
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
				const mbfl_encoding *encoding = mbfl_name2encoding(ZSTR_VAL(encoding_str));
				if (encoding) {
					*entry++ = encoding;
					n++;
				} else {
					ret = FAILURE;
				}
			}
			i--;
			zend_string_release(encoding_str);
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
	return (const zend_encoding *) mbfl_identify_encoding(&string, (const mbfl_encoding **)list, list_size, 0);
}

static size_t php_mb_zend_encoding_converter(unsigned char **to, size_t *to_length, const unsigned char *from, size_t from_length, const zend_encoding *encoding_to, const zend_encoding *encoding_from)
{
	mbfl_string string, result;
	mbfl_buffer_converter *convd;
	int status;
	size_t loc;

	/* new encoding */
	/* initialize string */
	string.encoding = (const mbfl_encoding*)encoding_from;
	string.no_language = MBSTRG(language);
	string.val = (unsigned char*)from;
	string.len = from_length;

	/* initialize converter */
	convd = mbfl_buffer_converter_new((const mbfl_encoding *)encoding_from, (const mbfl_encoding *)encoding_to, string.len);
	if (convd == NULL) {
		return (size_t) -1;
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
	mbfl_string_init(&result);
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

#if HAVE_MBREGEX
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
	OnigMatchParam *mp = onig_new_match_param();
	int err;
	onig_initialize_match_param(mp);
	if (!ZEND_LONG_UINT_OVFL(MBSTRG(regex_stack_limit))) {
		onig_set_match_stack_limit_size_of_match_param(mp, (unsigned int)MBSTRG(regex_stack_limit));
	}
	if (!ZEND_LONG_UINT_OVFL(MBSTRG(regex_retry_limit))) {
		onig_set_retry_limit_in_match_of_match_param(mp, (unsigned int)MBSTRG(regex_retry_limit));
	}
	/* search */
	err = onig_search_with_param((php_mb_regex_t *)opaque, (const OnigUChar *)str,
		(const OnigUChar*)str + str_len, (const OnigUChar *)str,
		(const OnigUChar*)str + str_len, NULL, ONIG_OPTION_NONE, mp);
	onig_free_match_param(mp);
	return err >= 0;
}
/* }}} */

/* {{{ _php_mb_free_regex */
static void _php_mb_free_regex(void *opaque)
{
	onig_free((php_mb_regex_t *)opaque);
}
/* }}} */
#else
/* {{{ _php_mb_compile_regex */
static void *_php_mb_compile_regex(const char *pattern)
{
	pcre2_code *retval;
	PCRE2_SIZE err_offset;
	int errnum;

	if (!(retval = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED,
			PCRE2_CASELESS, &errnum, &err_offset, php_pcre_cctx()))) {
		PCRE2_UCHAR err_str[128];
		pcre2_get_error_message(errnum, err_str, sizeof(err_str));
		php_error_docref(NULL, E_WARNING, "%s (offset=%zu): %s", pattern, err_offset, err_str);
	}
	return retval;
}
/* }}} */

/* {{{ _php_mb_match_regex */
static int _php_mb_match_regex(void *opaque, const char *str, size_t str_len)
{
	int res;

	pcre2_match_data *match_data = php_pcre_create_match_data(0, opaque);
	if (NULL == match_data) {
		pcre2_code_free(opaque);
		php_error_docref(NULL, E_WARNING, "Cannot allocate match data");
		return FAILURE;
	}
	res = pcre2_match(opaque, (PCRE2_SPTR)str, str_len, 0, 0, match_data, php_pcre_mctx()) >= 0;
	php_pcre_free_match_data(match_data);

	return res;
}
/* }}} */

/* {{{ _php_mb_free_regex */
static void _php_mb_free_regex(void *opaque)
{
	pcre2_code_free(opaque);
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

static char *php_mb_rfc1867_substring_conf(const zend_encoding *encoding, char *start, size_t len, char quote)
{
	char *result = emalloc(len + 2);
	char *resp = result;
	size_t i;

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

static int _php_mb_ini_mbstring_http_input_set(const char *new_value, size_t new_value_length) {
	const mbfl_encoding **list;
	size_t size;
	if (FAILURE == php_mb_parse_encoding_list(new_value, new_value_length, &list, &size, 1)) {
		return FAILURE;
	}
	if (MBSTRG(http_input_list)) {
		pefree(MBSTRG(http_input_list), 1);
	}
	MBSTRG(http_input_list) = list;
	MBSTRG(http_input_list_size) = size;
	return SUCCESS;
}

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_http_input) */
static PHP_INI_MH(OnUpdate_mbstring_http_input)
{
	if (stage & (PHP_INI_STAGE_ACTIVATE | PHP_INI_STAGE_RUNTIME)) {
		php_error_docref("ref.mbstring", E_DEPRECATED, "Use of mbstring.http_input is deprecated");
	}

	if (!new_value || !ZSTR_VAL(new_value)) {
		const char *encoding = php_get_input_encoding();
		MBSTRG(http_input_set) = 0;
		_php_mb_ini_mbstring_http_input_set(encoding, strlen(encoding));
		return SUCCESS;
	}

	MBSTRG(http_input_set) = 1;
	return _php_mb_ini_mbstring_http_input_set(ZSTR_VAL(new_value), ZSTR_LEN(new_value));
}
/* }}} */

static int _php_mb_ini_mbstring_http_output_set(const char *new_value) {
	const mbfl_encoding *encoding = mbfl_name2encoding(new_value);
	if (!encoding) {
		return FAILURE;
	}

	MBSTRG(http_output_encoding) = encoding;
	MBSTRG(current_http_output_encoding) = encoding;
	return SUCCESS;
}

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_http_output) */
static PHP_INI_MH(OnUpdate_mbstring_http_output)
{
	if (stage & (PHP_INI_STAGE_ACTIVATE | PHP_INI_STAGE_RUNTIME)) {
		php_error_docref("ref.mbstring", E_DEPRECATED, "Use of mbstring.http_output is deprecated");
	}

	if (new_value == NULL || ZSTR_LEN(new_value) == 0) {
		MBSTRG(http_output_set) = 0;
		_php_mb_ini_mbstring_http_output_set(php_get_output_encoding());
		return SUCCESS;
	}

	MBSTRG(http_output_set) = 1;
	return _php_mb_ini_mbstring_http_output_set(ZSTR_VAL(new_value));
}
/* }}} */

/* {{{ static _php_mb_ini_mbstring_internal_encoding_set */
static int _php_mb_ini_mbstring_internal_encoding_set(const char *new_value, size_t new_value_length)
{
	const mbfl_encoding *encoding;

	if (!new_value || !new_value_length || !(encoding = mbfl_name2encoding(new_value))) {
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

	if (new_value && ZSTR_LEN(new_value)) {
		MBSTRG(internal_encoding_set) = 1;
		return _php_mb_ini_mbstring_internal_encoding_set(ZSTR_VAL(new_value), ZSTR_LEN(new_value));
	} else {
		const char *encoding = php_get_internal_encoding();
		MBSTRG(internal_encoding_set) = 0;
		return _php_mb_ini_mbstring_internal_encoding_set(encoding, strlen(encoding));
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
			zend_string_release_ex(tmp, 0);
			return FAILURE;
		}
	}

	if (MBSTRG(http_output_conv_mimetypes)) {
		_php_mb_free_regex(MBSTRG(http_output_conv_mimetypes));
	}

	MBSTRG(http_output_conv_mimetypes) = re;

	zend_string_release_ex(tmp, 0);
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
		OnUpdateBool,
		strict_detection, zend_mbstring_globals, mbstring_globals)
#if HAVE_MBREGEX
	STD_PHP_INI_ENTRY("mbstring.regex_stack_limit", "100000",PHP_INI_ALL, OnUpdateLong, regex_stack_limit, zend_mbstring_globals, mbstring_globals)
	STD_PHP_INI_ENTRY("mbstring.regex_retry_limit", "1000000",PHP_INI_ALL, OnUpdateLong, regex_retry_limit, zend_mbstring_globals, mbstring_globals)
#endif
PHP_INI_END()
/* }}} */

static void mbstring_internal_encoding_changed_hook() {
	/* One of the internal_encoding / input_encoding / output_encoding ini settings changed. */
	if (!MBSTRG(internal_encoding_set)) {
		const char *encoding = php_get_internal_encoding();
		_php_mb_ini_mbstring_internal_encoding_set(encoding, strlen(encoding));
	}

	if (!MBSTRG(http_output_set)) {
		const char *encoding = php_get_output_encoding();
		_php_mb_ini_mbstring_http_output_set(encoding);
	}

	if (!MBSTRG(http_input_set)) {
		const char *encoding = php_get_input_encoding();
		_php_mb_ini_mbstring_http_input_set(encoding, strlen(encoding));
	}
}

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
	mbstring_globals->last_used_encoding_name = NULL;
	mbstring_globals->last_used_encoding = NULL;
	mbstring_globals->internal_encoding_set = 0;
	mbstring_globals->http_output_set = 0;
	mbstring_globals->http_input_set = 0;
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
	__mbfl_allocators = (mbfl_allocators*)&_php_mb_allocators;

	REGISTER_INI_ENTRIES();

	/* We assume that we're the only user of the hook. */
	ZEND_ASSERT(php_internal_encoding_changed == NULL);
	php_internal_encoding_changed = mbstring_internal_encoding_changed_hook;
	mbstring_internal_encoding_changed_hook();

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
	REGISTER_LONG_CONSTANT("MB_CASE_FOLD", PHP_UNICODE_CASE_FOLD, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MB_CASE_UPPER_SIMPLE", PHP_UNICODE_CASE_UPPER_SIMPLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MB_CASE_LOWER_SIMPLE", PHP_UNICODE_CASE_LOWER_SIMPLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MB_CASE_TITLE_SIMPLE", PHP_UNICODE_CASE_TITLE_SIMPLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MB_CASE_FOLD_SIMPLE", PHP_UNICODE_CASE_FOLD_SIMPLE, CONST_CS | CONST_PERSISTENT);

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

	/* override original function (deprecated). */
	if (MBSTRG(func_overload)){
		zend_function *func, *orig;
		const struct mb_overload_def *p;
		zend_string *str;

		p = &(mb_ovld[0]);
		while (p->type > 0) {
			if ((MBSTRG(func_overload) & p->type) == p->type &&
				!zend_hash_str_exists(CG(function_table), p->save_func, strlen(p->save_func))
			) {
				func = zend_hash_str_find_ptr(CG(function_table), p->ovld_func, strlen(p->ovld_func));

				if ((orig = zend_hash_str_find_ptr(CG(function_table), p->orig_func, strlen(p->orig_func))) == NULL) {
					php_error_docref("ref.mbstring", E_WARNING, "mbstring couldn't find function %s.", p->orig_func);
					return FAILURE;
				} else {
					ZEND_ASSERT(orig->type == ZEND_INTERNAL_FUNCTION);
					str = zend_string_init_interned(p->save_func, strlen(p->save_func), 1);
					zend_hash_add_mem(CG(function_table), str, orig, sizeof(zend_internal_function));
					zend_string_release_ex(str, 1);
					function_add_ref(orig);

					str = zend_string_init_interned(p->orig_func, strlen(p->orig_func), 1);
					zend_hash_update_mem(CG(function_table), str, func, sizeof(zend_internal_function));
					zend_string_release_ex(str, 1);
					function_add_ref(func);
				}
			}
			p++;
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION(mbstring) */
PHP_MSHUTDOWN_FUNCTION(mbstring)
{
	/*  clear overloaded function. */
	if (MBSTRG(func_overload)){
		const struct mb_overload_def *p;
		zend_function *orig;

		p = &(mb_ovld[0]);
		while (p->type > 0) {
			if ((MBSTRG(func_overload) & p->type) == p->type &&
				(orig = zend_hash_str_find_ptr(CG(function_table), p->save_func, strlen(p->save_func)))) {

				zend_hash_str_update_mem(CG(function_table), p->orig_func, strlen(p->orig_func), orig, sizeof(zend_internal_function));
				function_add_ref(orig);
				zend_hash_str_del(CG(function_table), p->save_func, strlen(p->save_func));
			}
			p++;
		}
	}

	UNREGISTER_INI_ENTRIES();

	zend_multibyte_restore_functions();

#if HAVE_MBREGEX
	PHP_MSHUTDOWN(mb_regex) (INIT_FUNC_ARGS_PASSTHRU);
#endif

	php_internal_encoding_changed = NULL;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION(mbstring) */
PHP_RINIT_FUNCTION(mbstring)
{
	MBSTRG(current_internal_encoding) = MBSTRG(internal_encoding);
	MBSTRG(current_http_output_encoding) = MBSTRG(http_output_encoding);
	MBSTRG(current_filter_illegal_mode) = MBSTRG(filter_illegal_mode);
	MBSTRG(current_filter_illegal_substchar) = MBSTRG(filter_illegal_substchar);

	MBSTRG(illegalchars) = 0;

	php_mb_populate_current_detect_order_list();

	/* override original function. */
	if (MBSTRG(func_overload)){
		zend_error(E_DEPRECATED, "The mbstring.func_overload directive is deprecated");

		CG(compiler_options) |= ZEND_COMPILE_NO_BUILTIN_STRLEN;
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

	if (MBSTRG(last_used_encoding_name)) {
		zend_string_release(MBSTRG(last_used_encoding_name));
		MBSTRG(last_used_encoding_name) = NULL;
	}

	MBSTRG(internal_encoding_set) = 0;
	MBSTRG(http_output_set) = 0;
	MBSTRG(http_input_set) = 0;

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
		zend_string_release_ex(ini_name, 0);
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
			MBSTRG(internal_encoding_set) = 1;
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
			MBSTRG(http_output_set) = 1;
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
				if (!try_convert_to_string(arg1)) {
					return;
				}
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

static inline int php_mb_check_code_point(zend_long cp)
{
	if (cp <= 0 || cp >= 0x110000) {
		/* Out of Unicode range */
		return 0;
	}

	if (cp >= 0xd800 && cp <= 0xdfff) {
		/* Surrogate code-point. These are never valid on their own and we only allow a single
		 * substitute character. */
		return 0;
	}

	/* As the we do not know the target encoding of the conversion operation that is going to
	 * use the substitution character, we cannot check whether the codepoint is actually mapped
	 * in the given encoding at this point. Thus we have to accept everything. */
	return 1;
}

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

					if (php_mb_check_code_point(Z_LVAL_P(arg1))) {
						MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
						MBSTRG(current_filter_illegal_substchar) = Z_LVAL_P(arg1);
					} else {
						php_error_docref(NULL, E_WARNING, "Unknown character");
						RETURN_FALSE;
					}
				}
				break;
			default:
				convert_to_long_ex(arg1);
				if (php_mb_check_code_point(Z_LVAL_P(arg1))) {
					MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
					MBSTRG(current_filter_illegal_substchar) = Z_LVAL_P(arg1);
				} else {
					php_error_docref(NULL, E_WARNING, "Unknown character");
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
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|z", &encstr, &encstr_len, &track_vars_array) == FAILURE) {
		return;
	}

	if (track_vars_array != NULL) {
		track_vars_array = zend_try_array_init(track_vars_array);
		if (!track_vars_array) {
			return;
		}
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
		zend_array *symbol_table;
		if (zend_forbid_dynamic_call("mb_parse_str() with a single argument") == FAILURE) {
			efree(encstr);
			return;
		}

		php_error_docref(NULL, E_DEPRECATED, "Calling mb_parse_str() without the result argument is deprecated");

		symbol_table = zend_rebuild_symbol_table();
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
	int last_feed;
	size_t len;
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
 			MBSTRG(outconv) = mbfl_buffer_converter_new(MBSTRG(current_internal_encoding), encoding, 0);
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
	string.encoding = MBSTRG(current_internal_encoding);
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

/* {{{ proto array mb_str_split(string str [, int split_length] [, string encoding])
 Convert a multibyte string to an array. If split_length is specified,
 break the string down into chunks each split_length characters long. */

/* structure to pass split params to the callback */
struct mbfl_split_params {
    zval *return_value; /* php function return value structure pointer */
    mbfl_string *result_string; /* string to store result chunk */
    size_t mb_chunk_length; /* actual chunk length in chars */
    size_t split_length; /* split length in chars */
    mbfl_convert_filter *next_filter; /* widechar to encoding converter */
};

/* callback function to fill split array */
static int mbfl_split_output(int c, void *data)
{
    struct mbfl_split_params *params = (struct mbfl_split_params *)data; /* cast passed data */

    (*params->next_filter->filter_function)(c, params->next_filter); /* decoder filter */

    if(params->split_length == ++params->mb_chunk_length) { /* if current chunk size reached defined chunk size or last char reached */
        mbfl_convert_filter_flush(params->next_filter);/* concatenate separate decoded chars to the solid string */
        mbfl_memory_device *device = (mbfl_memory_device *)params->next_filter->data; /* chars container */
        mbfl_string *chunk = params->result_string;
        mbfl_memory_device_result(device, chunk); /* make chunk */
        add_next_index_stringl(params->return_value, (const char *)chunk->val, chunk->len); /* add chunk to the array */
        efree(chunk->val);
        params->mb_chunk_length = 0; /* reset mb_chunk size */
    }
    return 0;
}

PHP_FUNCTION(mb_str_split)
{
	zend_string *str, *encoding = NULL;
	size_t mb_len, chunks, chunk_len;
	const char *p, *last; /* pointer for the string cursor and last string char */
	mbfl_string string, result_string;
	const mbfl_encoding *mbfl_encoding;
	zend_long split_length = 1;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(split_length)
		Z_PARAM_STR(encoding)
	ZEND_PARSE_PARAMETERS_END();

	if (split_length <= 0) {
		php_error_docref(NULL, E_WARNING, "The length of each segment must be greater than zero");
		RETURN_FALSE;
	}

	/* fill mbfl_string structure */
	string.val = (unsigned char *) ZSTR_VAL(str);
	string.len = ZSTR_LEN(str);
	string.no_language = MBSTRG(language);
	string.encoding = php_mb_get_encoding(encoding);
	if (!string.encoding) {
		RETURN_FALSE;
	}

	p = ZSTR_VAL(str); /* string cursor pointer */
	last = ZSTR_VAL(str) + ZSTR_LEN(str); /* last string char pointer */

	mbfl_encoding = string.encoding;

	/* first scenario: 1,2,4-bytes fixed width encodings (head part) */
	if (mbfl_encoding->flag & MBFL_ENCTYPE_SBCS) { /* 1 byte */
		mb_len = string.len;
		chunk_len = (size_t)split_length; /* chunk length in bytes */
	} else if (mbfl_encoding->flag & (MBFL_ENCTYPE_WCS2BE | MBFL_ENCTYPE_WCS2LE)) { /* 2 bytes */
		mb_len = string.len / 2;
		chunk_len = split_length * 2;
	} else if (mbfl_encoding->flag & (MBFL_ENCTYPE_WCS4BE | MBFL_ENCTYPE_WCS4LE)) { /* 4 bytes */
		mb_len = string.len / 4;
		chunk_len = split_length * 4;
	} else if (mbfl_encoding->mblen_table != NULL) {
		/* second scenario: variable width encodings with length table */
		char unsigned const *mbtab = mbfl_encoding->mblen_table;

		/* assume that we have 1-bytes characters */
		array_init_size(return_value, (string.len + split_length) / split_length); /* round up */

		while (p < last) { /* split cycle work until the cursor has reached the last byte */
			char const *chunk_p = p; /* chunk first byte pointer */
			chunk_len = 0; /* chunk length in bytes */
			zend_long char_count;

			for (char_count = 0; char_count < split_length && p < last; ++char_count) {
				char unsigned const m = mbtab[*(const unsigned char *)p]; /* single character length table */
				chunk_len += m;
				p += m;
			}
			if (p >= last) chunk_len -= p - last; /* check if chunk is in bounds */
			add_next_index_stringl(return_value, chunk_p, chunk_len);
		}
		return;
	} else {
		/* third scenario: other multibyte encodings */
		mbfl_convert_filter *filter, *decoder;

		/* assume that we have 1-bytes characters */
		array_init_size(return_value, (string.len + split_length) / split_length); /* round up */

		/* decoder filter to decode wchar to encoding */
		mbfl_memory_device device;
		mbfl_memory_device_init(&device, split_length + 1, 0);

		decoder = mbfl_convert_filter_new(
				&mbfl_encoding_wchar,
				string.encoding,
				mbfl_memory_device_output,
				NULL,
				&device);
		/* if something wrong with the decoded */
		if (decoder == NULL) {
			RETURN_FALSE;
		}

		/* wchar filter */
		mbfl_string_init(&result_string); /* mbfl_string to store chunk in the callback */
		struct mbfl_split_params params = { /* init callback function params structure */
			.return_value = return_value,
			.result_string = &result_string,
			.mb_chunk_length = 0,
			.split_length = (size_t)split_length,
			.next_filter = decoder,
		};

		filter = mbfl_convert_filter_new(
				string.encoding,
				&mbfl_encoding_wchar,
				mbfl_split_output,
				NULL,
				&params);
		/* if something wrong with the filter */
		if (filter == NULL){
			mbfl_convert_filter_delete(decoder); /* this will free allocated memory for the decoded */
			RETURN_FALSE;
		}

		while (p < last - 1) { /* cycle each byte except last with callback function */
			(*filter->filter_function)(*p++, filter);
		}
		params.mb_chunk_length = split_length - 1; /* force to finish current chunk */
		(*filter->filter_function)(*p++, filter); /*process last char */

		mbfl_convert_filter_delete(decoder);
		mbfl_convert_filter_delete(filter);
		mbfl_memory_device_clear(&device);
		return;
	}

	/* first scenario: 1,2,4-bytes fixed width encodings (tail part) */
	chunks = (mb_len + split_length - 1) / split_length; /* (round up idiom) */
	array_init_size(return_value, chunks);
	if (chunks != 0) {
		zend_long i;

		for (i = 0; i < chunks - 1; p += chunk_len, ++i) {
			add_next_index_stringl(return_value, p, chunk_len);
		}
		add_next_index_stringl(return_value, p, last - p);
	}
}
/* }}} */

/* {{{ proto int mb_strlen(string str [, string encoding])
   Get character numbers of a string */
PHP_FUNCTION(mb_strlen)
{
	size_t n;
	mbfl_string string;
	char *str;
	size_t str_len;
	zend_string *enc_name = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(enc_name)
	ZEND_PARSE_PARAMETERS_END();

	string.val = (unsigned char *) str;
	string.len = str_len;
	string.no_language = MBSTRG(language);
	string.encoding = php_mb_get_encoding(enc_name);
	if (!string.encoding) {
		RETURN_FALSE;
	}

	n = mbfl_strlen(&string);
	if (!mbfl_is_error(n)) {
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
	int reverse = 0;
	zend_long offset = 0;
	mbfl_string haystack, needle;
	zend_string *enc_name = NULL;
	size_t n;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|lS", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &offset, &enc_name) == FAILURE) {
		return;
	}

	haystack.no_language = needle.no_language = MBSTRG(language);
	haystack.encoding = needle.encoding = php_mb_get_encoding(enc_name);
	if (!haystack.encoding) {
		RETURN_FALSE;
	}

	if (offset != 0) {
		size_t slen = mbfl_strlen(&haystack);
		if (offset < 0) {
			offset += slen;
		}
		if (offset < 0 || offset > slen) {
			php_error_docref(NULL, E_WARNING, "Offset not contained in string");
			RETURN_FALSE;
		}
	}

	if (needle.len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty delimiter");
		RETURN_FALSE;
	}

	n = mbfl_strpos(&haystack, &needle, offset, reverse);
	if (!mbfl_is_error(n)) {
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
	mbfl_string haystack, needle;
	zend_string *enc_name = NULL;
	zval *zoffset = NULL;
	zend_long offset = 0, n;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|zS", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &zoffset, &enc_name) == FAILURE) {
		return;
	}

	if (zoffset) {
		if (Z_TYPE_P(zoffset) == IS_STRING) {
			switch (Z_STRVAL_P(zoffset)[0]) {
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
					convert_to_long_ex(zoffset);
					offset = Z_LVAL_P(zoffset);
					break;
				default :
					enc_name = Z_STR_P(zoffset);
					php_error_docref(NULL, E_DEPRECATED,
						"Passing the encoding as third parameter is deprecated. "
						"Use an explicit zero offset");
					break;
			}
		} else {
			convert_to_long_ex(zoffset);
			offset = Z_LVAL_P(zoffset);
		}
	}

	haystack.no_language = needle.no_language = MBSTRG(language);
	haystack.encoding = needle.encoding = php_mb_get_encoding(enc_name);
	if (!haystack.encoding) {
		RETURN_FALSE;
	}

	if (offset != 0) {
		size_t haystack_char_len = mbfl_strlen(&haystack);
		if ((offset > 0 && offset > haystack_char_len) ||
			(offset < 0 && -offset > haystack_char_len)) {
			php_error_docref(NULL, E_WARNING, "Offset is greater than the length of haystack string");
			RETURN_FALSE;
		}
	}

	n = mbfl_strpos(&haystack, &needle, offset, 1);
	if (!mbfl_is_error(n)) {
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
	size_t n = (size_t) -1;
	zend_long offset = 0;
	mbfl_string haystack, needle;
	zend_string *from_encoding = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|lS", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &offset, &from_encoding) == FAILURE) {
		return;
	}

	if (needle.len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty delimiter");
		RETURN_FALSE;
	}

	n = php_mb_stripos(0, (char *)haystack.val, haystack.len, (char *)needle.val, needle.len, offset, from_encoding);

	if (!mbfl_is_error(n)) {
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
	size_t n = (size_t) -1;
	zend_long offset = 0;
	mbfl_string haystack, needle;
	zend_string *from_encoding = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|lS", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &offset, &from_encoding) == FAILURE) {
		return;
	}

	n = php_mb_stripos(1, (char *)haystack.val, haystack.len, (char *)needle.val, needle.len, offset, from_encoding);

	if (!mbfl_is_error(n)) {
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
	size_t n;
	mbfl_string haystack, needle, result, *ret = NULL;
	zend_string *enc_name = NULL;
	zend_bool part = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|bS", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &part, &enc_name) == FAILURE) {
		return;
	}

	haystack.no_language = needle.no_language = MBSTRG(language);
	haystack.encoding = needle.encoding = php_mb_get_encoding(enc_name);
	if (!haystack.encoding) {
		RETURN_FALSE;
	}

	if (needle.len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty delimiter");
		RETURN_FALSE;
	}

	n = mbfl_strpos(&haystack, &needle, 0, 0);
	if (!mbfl_is_error(n)) {
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
			ret = mbfl_substr(&haystack, &result, n, MBFL_SUBSTR_UNTIL_END);
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
	size_t n;
	mbfl_string haystack, needle, result, *ret = NULL;
	zend_string *enc_name = NULL;
	zend_bool part = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|bS", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &part, &enc_name) == FAILURE) {
		return;
	}

	haystack.no_language = needle.no_language = MBSTRG(language);
	haystack.encoding = needle.encoding = php_mb_get_encoding(enc_name);
	if (!haystack.encoding) {
		RETURN_FALSE;
	}

	if (haystack.len == 0) {
		RETURN_FALSE;
	}
	if (needle.len == 0) {
		RETURN_FALSE;
	}

	n = mbfl_strpos(&haystack, &needle, 0, 1);
	if (!mbfl_is_error(n)) {
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
			ret = mbfl_substr(&haystack, &result, n, MBFL_SUBSTR_UNTIL_END);
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
	size_t n;
	mbfl_string haystack, needle, result, *ret = NULL;
	zend_string *from_encoding = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|bS", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &part, &from_encoding) == FAILURE) {
		return;
	}

	haystack.no_language = needle.no_language = MBSTRG(language);
	haystack.encoding = needle.encoding = php_mb_get_encoding(from_encoding);
	if (!haystack.encoding) {
		RETURN_FALSE;
	}

	if (!needle.len) {
		php_error_docref(NULL, E_WARNING, "Empty delimiter");
		RETURN_FALSE;
	}

	n = php_mb_stripos(0, (char *)haystack.val, haystack.len, (char *)needle.val, needle.len, 0, from_encoding);
	if (mbfl_is_error(n)) {
		RETURN_FALSE;
	}

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
		ret = mbfl_substr(&haystack, &result, n, MBFL_SUBSTR_UNTIL_END);
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
	size_t n;
	mbfl_string haystack, needle, result, *ret = NULL;
	zend_string *from_encoding = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|bS", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &part, &from_encoding) == FAILURE) {
		return;
	}

	haystack.no_language = needle.no_language = MBSTRG(language);
	haystack.encoding = needle.encoding = php_mb_get_encoding(from_encoding);
	if (!haystack.encoding) {
		RETURN_FALSE;
	}

	n = php_mb_stripos(1, (char *)haystack.val, haystack.len, (char *)needle.val, needle.len, 0, from_encoding);
	if (mbfl_is_error(n)) {
		RETURN_FALSE;
	}

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
		ret = mbfl_substr(&haystack, &result, n, MBFL_SUBSTR_UNTIL_END);
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
	size_t n;
	mbfl_string haystack, needle;
	zend_string *enc_name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|S", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &enc_name) == FAILURE) {
		return;
	}

	haystack.no_language = needle.no_language = MBSTRG(language);
	haystack.encoding = needle.encoding = php_mb_get_encoding(enc_name);
	if (!haystack.encoding) {
		RETURN_FALSE;
	}

	if (needle.len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty substring");
		RETURN_FALSE;
	}

	n = mbfl_substr_count(&haystack, &needle);
	if (!mbfl_is_error(n)) {
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
	char *str;
	zend_string *encoding = NULL;
	zend_long from, len;
	size_t mblen, real_from, real_len;
	size_t str_len;
	zend_bool len_is_null = 1;
	mbfl_string string, result, *ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl|l!S", &str, &str_len, &from, &len, &len_is_null, &encoding) == FAILURE) {
		return;
	}

	string.no_language = MBSTRG(language);
	string.encoding = php_mb_get_encoding(encoding);
	if (!string.encoding) {
		RETURN_FALSE;
	}

	string.val = (unsigned char *)str;
	string.len = str_len;

	/* measures length */
	mblen = 0;
	if (from < 0 || (!len_is_null && len < 0)) {
		mblen = mbfl_strlen(&string);
	}

	/* if "from" position is negative, count start position from the end
	 * of the string
	 */
	if (from >= 0) {
		real_from = (size_t) from;
	} else if (-from < mblen) {
		real_from = mblen + from;
	} else {
		real_from = 0;
	}

	/* if "length" position is negative, set it to the length
	 * needed to stop that many chars from the end of the string
	 */
	if (len_is_null) {
		real_len = MBFL_SUBSTR_UNTIL_END;
	} else if (len >= 0) {
		real_len = (size_t) len;
	} else if (real_from < mblen && -len < mblen - real_from) {
		real_len = (mblen - real_from) + len;
	} else {
		real_len = 0;
	}

	if (((MBSTRG(func_overload) & MB_OVERLOAD_STRING) == MB_OVERLOAD_STRING)
		&& (real_from > mbfl_strlen(&string))) {
		RETURN_FALSE;
	}

	ret = mbfl_substr(&string, &result, real_from, real_len);
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
	zend_string *encoding = NULL;
	zend_long from, len;
	zend_bool len_is_null = 1;
	mbfl_string string, result, *ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl|l!S", (char **)&string.val, &string.len, &from, &len, &len_is_null, &encoding) == FAILURE) {
		return;
	}

	string.no_language = MBSTRG(language);
	string.encoding = php_mb_get_encoding(encoding);
	if (!string.encoding) {
		RETURN_FALSE;
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

	if (from > string.len) {
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
	size_t n;
	mbfl_string string;
	zend_string *enc_name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|S", (char **)&string.val, &string.len, &enc_name) == FAILURE) {
		return;
	}

	string.no_language = MBSTRG(language);
	string.encoding = php_mb_get_encoding(enc_name);
	if (!string.encoding) {
		RETURN_FALSE;
	}

	n = mbfl_strwidth(&string);
	if (!mbfl_is_error(n)) {
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
	char *str, *trimmarker = NULL;
	zend_string *encoding = NULL;
	zend_long from, width, swidth = 0;
	size_t str_len, trimmarker_len;
	mbfl_string string, result, marker, *ret;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sll|sS", &str, &str_len, &from, &width, &trimmarker, &trimmarker_len, &encoding) == FAILURE) {
		return;
	}

	string.no_language = marker.no_language = MBSTRG(language);
	string.encoding = marker.encoding = php_mb_get_encoding(encoding);
	if (!string.encoding) {
		RETURN_FALSE;
	}

	string.val = (unsigned char *)str;
	string.len = str_len;
	marker.val = NULL;
	marker.len = 0;

	if ((from < 0) || (width < 0)) {
		swidth = mbfl_strwidth(&string);
	}

	if (from < 0) {
		from += swidth;
	}

	if (from < 0 || (size_t)from > str_len) {
		php_error_docref(NULL, E_WARNING, "Start position is out of range");
		RETURN_FALSE;
	}

	if (width < 0) {
		width = swidth + width - from;
	}

	if (width < 0) {
		php_error_docref(NULL, E_WARNING, "Width is out of range");
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


/* See mbfl_no_encoding definition for list of unsupported encodings */
static inline zend_bool php_mb_is_unsupported_no_encoding(enum mbfl_no_encoding no_enc)
{
	return ((no_enc >= mbfl_no_encoding_invalid && no_enc <= mbfl_no_encoding_qprint)
			|| (no_enc >= mbfl_no_encoding_utf7 && no_enc <= mbfl_no_encoding_utf7imap)
			|| (no_enc >= mbfl_no_encoding_jis && no_enc <= mbfl_no_encoding_2022jpms)
			|| (no_enc >= mbfl_no_encoding_cp50220 && no_enc <= mbfl_no_encoding_cp50222));
}


/* See mbfl_no_encoding definition for list of UTF-8 encodings */
static inline zend_bool php_mb_is_no_encoding_utf8(enum mbfl_no_encoding no_enc)
{
	return (no_enc >= mbfl_no_encoding_utf8 && no_enc <= mbfl_no_encoding_utf8_sb);
}

MBSTRING_API char *php_mb_convert_encoding_ex(const char *input, size_t length, const mbfl_encoding *to_encoding, const mbfl_encoding *from_encoding, size_t *output_len)
{
	mbfl_string string, result, *ret;
	mbfl_buffer_converter *convd;
	char *output = NULL;

	if (output_len) {
		*output_len = 0;
	}

	/* initialize string */
	string.encoding = from_encoding;
	string.no_language = MBSTRG(language);
	string.val = (unsigned char *)input;
	string.len = length;

	/* initialize converter */
	convd = mbfl_buffer_converter_new(from_encoding, to_encoding, string.len);
	if (convd == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to create character encoding converter");
		return NULL;
	}

	mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode));
	mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar));

	/* do it */
	mbfl_string_init(&result);
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

/* {{{ MBSTRING_API char *php_mb_convert_encoding() */
MBSTRING_API char *php_mb_convert_encoding(const char *input, size_t length, const char *_to_encoding, const char *_from_encodings, size_t *output_len)
{
	const mbfl_encoding *from_encoding, *to_encoding;

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

	/* pre-conversion encoding */
	from_encoding = MBSTRG(current_internal_encoding);
	if (_from_encodings) {
		const mbfl_encoding **list = NULL;
		size_t size = 0;
		php_mb_parse_encoding_list(_from_encodings, strlen(_from_encodings), &list, &size, 0);
		if (size == 1) {
			from_encoding = *list;
		} else if (size > 1) {
			/* auto detect */
			mbfl_string string;
			mbfl_string_init(&string);
			string.val = (unsigned char *)input;
			string.len = length;
			from_encoding = mbfl_identify_encoding(&string, list, size, MBSTRG(strict_detection));
			if (!from_encoding) {
				php_error_docref(NULL, E_WARNING, "Unable to detect character encoding");
				from_encoding = &mbfl_encoding_pass;
			}
		} else {
			php_error_docref(NULL, E_WARNING, "Illegal character encoding specified");
		}
		if (list != NULL) {
			efree((void *)list);
		}
	}

	return php_mb_convert_encoding_ex(input, length, to_encoding, from_encoding, output_len);
}
/* }}} */

MBSTRING_API HashTable *php_mb_convert_encoding_recursive(HashTable *input, const char *_to_encoding, const char *_from_encodings)
{
	HashTable *output, *chash;
	zend_long idx;
	zend_string *key;
	zval *entry, entry_tmp;
	size_t ckey_len, cval_len;
	char *ckey, *cval;

	if (!input) {
		return NULL;
	}

	if (GC_IS_RECURSIVE(input)) {
		GC_UNPROTECT_RECURSION(input);
		php_error_docref(NULL, E_WARNING, "Cannot convert recursively referenced values");
		return NULL;
	}
	GC_TRY_PROTECT_RECURSION(input);
	output = zend_new_array(zend_hash_num_elements(input));
	ZEND_HASH_FOREACH_KEY_VAL(input, idx, key, entry) {
		/* convert key */
		if (key) {
			ckey = php_mb_convert_encoding(ZSTR_VAL(key), ZSTR_LEN(key), _to_encoding, _from_encodings, &ckey_len);
			key = zend_string_init(ckey, ckey_len, 0);
			efree(ckey);
		}
		/* convert value */
		ZEND_ASSERT(entry);
try_again:
		switch(Z_TYPE_P(entry)) {
			case IS_STRING:
				cval = php_mb_convert_encoding(Z_STRVAL_P(entry), Z_STRLEN_P(entry), _to_encoding, _from_encodings, &cval_len);
				ZVAL_STRINGL(&entry_tmp, cval, cval_len);
				efree(cval);
				break;
			case IS_NULL:
			case IS_TRUE:
			case IS_FALSE:
			case IS_LONG:
			case IS_DOUBLE:
				ZVAL_COPY(&entry_tmp, entry);
				break;
			case IS_ARRAY:
				chash = php_mb_convert_encoding_recursive(Z_ARRVAL_P(entry), _to_encoding, _from_encodings);
				if (chash) {
					ZVAL_ARR(&entry_tmp, chash);
				} else {
					ZVAL_EMPTY_ARRAY(&entry_tmp);
				}
				break;
			case IS_REFERENCE:
				entry = Z_REFVAL_P(entry);
				goto try_again;
			case IS_OBJECT:
			default:
				if (key) {
					zend_string_release(key);
				}
				php_error_docref(NULL, E_WARNING, "Object is not supported");
				continue;
		}
		if (key) {
			zend_hash_add(output, key, &entry_tmp);
			zend_string_release(key);
		} else {
			zend_hash_index_add(output, idx, &entry_tmp);
		}
	} ZEND_HASH_FOREACH_END();
	GC_TRY_UNPROTECT_RECURSION(input);

	return output;
}
/* }}} */


/* {{{ proto string mb_convert_encoding(string str, string to-encoding [, mixed from-encoding])
   Returns converted string in desired encoding */
PHP_FUNCTION(mb_convert_encoding)
{
	zval *input;
	char *arg_new;
	size_t new_len;
	zval *arg_old = NULL;
	size_t size, l, n;
	char *_from_encodings = NULL, *ret, *s_free = NULL;

	zval *hash_entry;
	HashTable *target_hash;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zs|z", &input, &arg_new, &new_len, &arg_old) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(input) != IS_STRING && Z_TYPE_P(input) != IS_ARRAY) {
		if (!try_convert_to_string(input)) {
			return;
		}
	}

	if (arg_old) {
		switch (Z_TYPE_P(arg_old)) {
			case IS_ARRAY:
				target_hash = Z_ARRVAL_P(arg_old);
				_from_encodings = NULL;

				ZEND_HASH_FOREACH_VAL(target_hash, hash_entry) {
					zend_string *encoding_str = zval_try_get_string(hash_entry);
					if (UNEXPECTED(!encoding_str)) {
						if (_from_encodings) {
							efree(_from_encodings);
						}
						return;
					}

					if ( _from_encodings) {
						l = strlen(_from_encodings);
						n = ZSTR_LEN(encoding_str);
						_from_encodings = erealloc(_from_encodings, l+n+2);
						memcpy(_from_encodings + l, ",", 1);
						memcpy(_from_encodings + l + 1, ZSTR_VAL(encoding_str), ZSTR_LEN(encoding_str) + 1);
					} else {
						_from_encodings = estrdup(ZSTR_VAL(encoding_str));
					}
					zend_string_release(encoding_str);
				} ZEND_HASH_FOREACH_END();

				if (_from_encodings != NULL && !strlen(_from_encodings)) {
					efree(_from_encodings);
					_from_encodings = NULL;
				}
				s_free = _from_encodings;
				break;
			default:
				if (!try_convert_to_string(arg_old)) {
					return;
				}

				_from_encodings = Z_STRVAL_P(arg_old);
				break;
			}
	}

	if (Z_TYPE_P(input) == IS_STRING) {
		/* new encoding */
		ret = php_mb_convert_encoding(Z_STRVAL_P(input), Z_STRLEN_P(input), arg_new, _from_encodings, &size);
		if (ret != NULL) {
			// TODO: avoid reallocation ???
			RETVAL_STRINGL(ret, size);		/* the string is already strdup()'ed */
			efree(ret);
		} else {
			RETVAL_FALSE;
		}
		if (s_free) {
			efree(s_free);
		}
	} else {
		HashTable *tmp;
		tmp = php_mb_convert_encoding_recursive(Z_ARRVAL_P(input), arg_new, _from_encodings);
		RETURN_ARR(tmp);
	}

	return;
}
/* }}} */

static char *mbstring_convert_case(
		int case_mode, const char *str, size_t str_len, size_t *ret_len,
		const mbfl_encoding *enc) {
	return php_unicode_convert_case(
		case_mode, str, str_len, ret_len, enc,
		MBSTRG(current_filter_illegal_mode), MBSTRG(current_filter_illegal_substchar));
}

/* {{{ proto string mb_convert_case(string sourcestring, int mode [, string encoding])
   Returns a case-folded version of sourcestring */
PHP_FUNCTION(mb_convert_case)
{
	zend_string *from_encoding = NULL;
	char *str;
	size_t str_len;
	zend_long case_mode = 0;
	char *newstr;
	size_t ret_len;
	const mbfl_encoding *enc;

	RETVAL_FALSE;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl|S!", &str, &str_len,
				&case_mode, &from_encoding) == FAILURE) {
		return;
	}

	enc = php_mb_get_encoding(from_encoding);
	if (!enc) {
		return;
	}

	if (case_mode < 0 || case_mode > PHP_UNICODE_CASE_MODE_MAX) {
		php_error_docref(NULL, E_WARNING, "Invalid case mode");
		return;
	}

	newstr = mbstring_convert_case(case_mode, str, str_len, &ret_len, enc);

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
	zend_string *from_encoding = NULL;
	char *str;
	size_t str_len;
	char *newstr;
	size_t ret_len;
	const mbfl_encoding *enc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|S!", &str, &str_len,
				&from_encoding) == FAILURE) {
		return;
	}

	enc = php_mb_get_encoding(from_encoding);
	if (!enc) {
		RETURN_FALSE;
	}

	newstr = mbstring_convert_case(PHP_UNICODE_CASE_UPPER, str, str_len, &ret_len, enc);

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
	zend_string *from_encoding = NULL;
	char *str;
	size_t str_len;
	char *newstr;
	size_t ret_len;
	const mbfl_encoding *enc;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|S!", &str, &str_len,
				&from_encoding) == FAILURE) {
		return;
	}

	enc = php_mb_get_encoding(from_encoding);
	if (!enc) {
		RETURN_FALSE;
	}

	newstr = mbstring_convert_case(PHP_UNICODE_CASE_LOWER, str, str_len, &ret_len, enc);

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
			if (!try_convert_to_string(encoding_list)) {
				return;
			}
			if (FAILURE == php_mb_parse_encoding_list(Z_STRVAL_P(encoding_list), Z_STRLEN_P(encoding_list), &list, &size, 0)) {
				if (list) {
					efree(list);
					list = NULL;
					size = 0;
				}
			}
			break;
		}
		if (size == 0) {
			php_error_docref(NULL, E_WARNING, "Illegal argument");
		}
	}

	if (ZEND_NUM_ARGS() < 3) {
		strict = MBSTRG(strict_detection);
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
	ret = mbfl_identify_encoding(&string, elist, size, strict);

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

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

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
	const mbfl_encoding *charset, *transenc;
	mbfl_string  string, result, *ret;
	char *charset_name = NULL;
	size_t charset_name_len;
	char *trans_enc_name = NULL;
	size_t trans_enc_name_len;
	char *linefeed = "\r\n";
	size_t linefeed_len;
	zend_long indent = 0;

	string.no_language = MBSTRG(language);
	string.encoding = MBSTRG(current_internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|sssl", (char **)&string.val, &string.len, &charset_name, &charset_name_len, &trans_enc_name, &trans_enc_name_len, &linefeed, &linefeed_len, &indent) == FAILURE) {
		return;
	}

	charset = &mbfl_encoding_pass;
	transenc = &mbfl_encoding_base64;

	if (charset_name != NULL) {
		charset = mbfl_name2encoding(charset_name);
		if (!charset) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", charset_name);
			RETURN_FALSE;
		}
	} else {
		const mbfl_language *lang = mbfl_no2language(MBSTRG(language));
		if (lang != NULL) {
			charset = mbfl_no2encoding(lang->mail_charset);
			transenc = mbfl_no2encoding(lang->mail_header_encoding);
		}
	}

	if (trans_enc_name != NULL) {
		if (*trans_enc_name == 'B' || *trans_enc_name == 'b') {
			transenc = &mbfl_encoding_base64;
		} else if (*trans_enc_name == 'Q' || *trans_enc_name == 'q') {
			transenc = &mbfl_encoding_qprint;
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

	string.no_language = MBSTRG(language);
	string.encoding = MBSTRG(current_internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", (char **)&string.val, &string.len) == FAILURE) {
		return;
	}

	mbfl_string_init(&result);
	ret = mbfl_mime_header_decode(&string, &result, MBSTRG(current_internal_encoding));
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
	int opt;
	mbfl_string string, result, *ret;
	char *optstr = NULL;
	size_t optstr_len;
	zend_string *encname = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|sS", (char **)&string.val, &string.len, &optstr, &optstr_len, &encname) == FAILURE) {
		return;
	}

	/* option */
	if (optstr != NULL) {
		char *p = optstr;
		size_t i = 0, n = optstr_len;
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
	string.no_language = MBSTRG(language);
	string.encoding = php_mb_get_encoding(encname);
	if (!string.encoding) {
		RETURN_FALSE;
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

static int mb_recursive_encoder_detector_feed(mbfl_encoding_detector *identd, zval *var, int *recursion_error) /* {{{ */
{
	mbfl_string string;
	HashTable *ht;
	zval *entry;

	ZVAL_DEREF(var);
	if (Z_TYPE_P(var) == IS_STRING) {
		string.val = (unsigned char *)Z_STRVAL_P(var);
		string.len = Z_STRLEN_P(var);
		if (mbfl_encoding_detector_feed(identd, &string)) {
			return 1; /* complete detecting */
		}
	} else if (Z_TYPE_P(var) == IS_ARRAY || Z_TYPE_P(var) == IS_OBJECT) {
		if (Z_REFCOUNTED_P(var)) {
			if (Z_IS_RECURSIVE_P(var)) {
				*recursion_error = 1;
				return 0;
			}
			Z_PROTECT_RECURSION_P(var);
		}

		ht = HASH_OF(var);
		if (ht != NULL) {
			ZEND_HASH_FOREACH_VAL_IND(ht, entry) {
				if (mb_recursive_encoder_detector_feed(identd, entry, recursion_error)) {
					if (Z_REFCOUNTED_P(var)) {
						Z_UNPROTECT_RECURSION_P(var);
					}
					return 1;
				} else if (*recursion_error) {
					if (Z_REFCOUNTED_P(var)) {
						Z_UNPROTECT_RECURSION_P(var);
					}
					return 0;
				}
			} ZEND_HASH_FOREACH_END();
		}

		if (Z_REFCOUNTED_P(var)) {
			Z_UNPROTECT_RECURSION_P(var);
		}
	}
	return 0;
} /* }}} */

static int mb_recursive_convert_variable(mbfl_buffer_converter *convd, zval *var) /* {{{ */
{
	mbfl_string string, result, *ret;
	HashTable *ht;
	zval *entry, *orig_var;

	orig_var = var;
	ZVAL_DEREF(var);
	if (Z_TYPE_P(var) == IS_STRING) {
		string.val = (unsigned char *)Z_STRVAL_P(var);
		string.len = Z_STRLEN_P(var);
		ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
		if (ret != NULL) {
			zval_ptr_dtor(orig_var);
			// TODO: avoid reallocation ???
			ZVAL_STRINGL(orig_var, (char *)ret->val, ret->len);
			efree(ret->val);
		}
	} else if (Z_TYPE_P(var) == IS_ARRAY || Z_TYPE_P(var) == IS_OBJECT) {
		if (Z_TYPE_P(var) == IS_ARRAY) {
			SEPARATE_ARRAY(var);
		}
		if (Z_REFCOUNTED_P(var)) {
			if (Z_IS_RECURSIVE_P(var)) {
				return 1;
			}
			Z_PROTECT_RECURSION_P(var);
		}

		ht = HASH_OF(var);
		if (ht != NULL) {
			ZEND_HASH_FOREACH_VAL_IND(ht, entry) {
				if (mb_recursive_convert_variable(convd, entry)) {
					if (Z_REFCOUNTED_P(var)) {
						Z_UNPROTECT_RECURSION_P(var);
					}
					return 1;
				}
			} ZEND_HASH_FOREACH_END();
		}

		if (Z_REFCOUNTED_P(var)) {
			Z_UNPROTECT_RECURSION_P(var);
		}
	}
	return 0;
} /* }}} */

/* {{{ proto string mb_convert_variables(string to-encoding, mixed from-encoding, mixed vars [, ...])
   Converts the string resource in variables to desired encoding */
PHP_FUNCTION(mb_convert_variables)
{
	zval *args, *zfrom_enc;
	mbfl_string string, result;
	const mbfl_encoding *from_encoding, *to_encoding;
	mbfl_encoding_detector *identd;
	mbfl_buffer_converter *convd;
	int n, argc;
	size_t to_enc_len;
	size_t elistsz;
	const mbfl_encoding **elist;
	char *to_enc;
	int recursion_error = 0;

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
	string.encoding = from_encoding;
	string.no_language = MBSTRG(language);

	/* pre-conversion encoding */
	elist = NULL;
	elistsz = 0;
	switch (Z_TYPE_P(zfrom_enc)) {
		case IS_ARRAY:
			php_mb_parse_encoding_array(zfrom_enc, &elist, &elistsz, 0);
			break;
		default:
			if (!try_convert_to_string(zfrom_enc)) {
				return;
			}
			php_mb_parse_encoding_list(Z_STRVAL_P(zfrom_enc), Z_STRLEN_P(zfrom_enc), &elist, &elistsz, 0);
			break;
	}

	if (elistsz == 0) {
		from_encoding = &mbfl_encoding_pass;
	} else if (elistsz == 1) {
		from_encoding = *elist;
	} else {
		/* auto detect */
		from_encoding = NULL;
		identd = mbfl_encoding_detector_new(elist, elistsz, MBSTRG(strict_detection));
		if (identd != NULL) {
			n = 0;
			while (n < argc) {
				if (mb_recursive_encoder_detector_feed(identd, &args[n], &recursion_error)) {
					break;
				}
				n++;
			}
			from_encoding = mbfl_encoding_detector_judge(identd);
			mbfl_encoding_detector_delete(identd);
			if (recursion_error) {
				if (elist != NULL) {
					efree((void *)elist);
				}
				php_error_docref(NULL, E_WARNING, "Cannot handle recursive references");
				RETURN_FALSE;
			}
		}

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
		convd = mbfl_buffer_converter_new(from_encoding, to_encoding, 0);
		if (convd == NULL) {
			php_error_docref(NULL, E_WARNING, "Unable to create converter");
			RETURN_FALSE;
		}
		mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode));
		mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar));
	}

	/* convert */
	if (convd != NULL) {
		n = 0;
		while (n < argc) {
			zval *zv = &args[n];

			ZVAL_DEREF(zv);
			recursion_error = mb_recursive_convert_variable(convd, zv);
			if (recursion_error) {
				break;
			}
			n++;
		}

		MBSTRG(illegalchars) += mbfl_buffer_illegalchars(convd);
		mbfl_buffer_converter_delete(convd);

		if (recursion_error) {
			php_error_docref(NULL, E_WARNING, "Cannot handle recursive references");
			RETURN_FALSE;
		}
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sz|sb", &str, &str_len, &zconvmap, &encoding, &encoding_len, &is_hex) == FAILURE) {
		return;
	}

	string.no_language = MBSTRG(language);
	string.encoding = MBSTRG(current_internal_encoding);
	string.val = (unsigned char *)str;
	string.len = str_len;

	/* encoding */
	if (encoding && encoding_len > 0) {
		string.encoding = mbfl_name2encoding(encoding);
		if (!string.encoding) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", encoding);
			RETURN_FALSE;
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
				*mapelm++ = zval_get_long(hash_entry);
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

								zend_string_release_ex(fld_name, 0);
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

			zend_string_release_ex(fld_name, 0);
		}
	}
	return state;
}

PHP_FUNCTION(mb_send_mail)
{
	char *to;
	size_t to_len;
	char *message;
	size_t message_len;
	char *subject;
	size_t subject_len;
	zval *headers = NULL;
	zend_string *extra_cmd = NULL;
	zend_string *str_headers = NULL, *tmp_headers;
	size_t n, i;
	char *to_r = NULL;
	char *force_extra_parameters = INI_STR("mail.force_extra_parameters");
	struct {
		int cnt_type:1;
		int cnt_trans_enc:1;
	} suppressed_hdrs = { 0, 0 };

	char *message_buf = NULL, *subject_buf = NULL, *p;
	mbfl_string orig_str, conv_str;
	mbfl_string *pstr;	/* pointer to mbfl string for return value */
	enum mbfl_no_encoding;
	const mbfl_encoding *tran_cs,	/* transfar text charset */
						*head_enc,	/* header transfar encoding */
						*body_enc;	/* body transfar encoding */
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
	tran_cs = &mbfl_encoding_utf8;
	head_enc = &mbfl_encoding_base64;
	body_enc = &mbfl_encoding_base64;
	lang = mbfl_no2language(MBSTRG(language));
	if (lang != NULL) {
		tran_cs = mbfl_no2encoding(lang->mail_charset);
		head_enc = mbfl_no2encoding(lang->mail_header_encoding);
		body_enc = mbfl_no2encoding(lang->mail_body_encoding);
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sss|zS", &to, &to_len, &subject, &subject_len, &message, &message_len, &headers, &extra_cmd) == FAILURE) {
		return;
	}

	/* ASCIIZ check */
	MAIL_ASCIIZ_CHECK_MBSTRING(to, to_len);
	MAIL_ASCIIZ_CHECK_MBSTRING(subject, subject_len);
	MAIL_ASCIIZ_CHECK_MBSTRING(message, message_len);
	if (headers) {
		switch(Z_TYPE_P(headers)) {
			case IS_STRING:
				tmp_headers = zend_string_init(Z_STRVAL_P(headers), Z_STRLEN_P(headers), 0);
				MAIL_ASCIIZ_CHECK_MBSTRING(ZSTR_VAL(tmp_headers), ZSTR_LEN(tmp_headers));
				str_headers = php_trim(tmp_headers, NULL, 0, 2);
				zend_string_release_ex(tmp_headers, 0);
				break;
			case IS_ARRAY:
				str_headers = php_mail_build_headers(headers);
				break;
			default:
				php_error_docref(NULL, E_WARNING, "headers parameter must be string or array");
				RETURN_FALSE;
		}
	}
	if (extra_cmd) {
		MAIL_ASCIIZ_CHECK_MBSTRING(ZSTR_VAL(extra_cmd), ZSTR_LEN(extra_cmd));
	}

	zend_hash_init(&ht_headers, 0, NULL, ZVAL_PTR_DTOR, 0);

	if (str_headers != NULL) {
		_php_mbstr_parse_mail_headers(&ht_headers, ZSTR_VAL(str_headers), ZSTR_LEN(str_headers));
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
						const mbfl_encoding *_tran_cs = tran_cs;

						charset = php_strtok_r(NULL, "= \"", &tmp);
						if (charset != NULL) {
							_tran_cs = mbfl_name2encoding(charset);
						}

						if (!_tran_cs) {
							php_error_docref(NULL, E_WARNING, "Unsupported charset \"%s\" - will be regarded as ascii", charset);
							_tran_cs = &mbfl_encoding_ascii;
						}
						tran_cs = _tran_cs;
					}
				}
			}
		}
		suppressed_hdrs.cnt_type = 1;
	}

	if ((s = zend_hash_str_find(&ht_headers, "CONTENT-TRANSFER-ENCODING", sizeof("CONTENT-TRANSFER-ENCODING") - 1))) {
		const mbfl_encoding *_body_enc;

		ZEND_ASSERT(Z_TYPE_P(s) == IS_STRING);
		_body_enc = mbfl_name2encoding(Z_STRVAL_P(s));
		switch (_body_enc ? _body_enc->no_encoding : mbfl_no_encoding_invalid) {
			case mbfl_no_encoding_base64:
			case mbfl_no_encoding_7bit:
			case mbfl_no_encoding_8bit:
				body_enc = _body_enc;
				break;

			default:
				php_error_docref(NULL, E_WARNING, "Unsupported transfer encoding \"%s\" - will be regarded as 8bit", Z_STRVAL_P(s));
				body_enc =	&mbfl_encoding_8bit;
				break;
		}
		suppressed_hdrs.cnt_trans_enc = 1;
	}

	/* To: */
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

	/* Subject: */
	orig_str.no_language = MBSTRG(language);
	orig_str.val = (unsigned char *)subject;
	orig_str.len = subject_len;
	orig_str.encoding = MBSTRG(current_internal_encoding);
	if (orig_str.encoding->no_encoding == mbfl_no_encoding_invalid
			|| orig_str.encoding->no_encoding == mbfl_no_encoding_pass) {
		orig_str.encoding = mbfl_identify_encoding(&orig_str, MBSTRG(current_detect_order_list), MBSTRG(current_detect_order_list_size), MBSTRG(strict_detection));
	}
	pstr = mbfl_mime_header_encode(&orig_str, &conv_str, tran_cs, head_enc, "\n", sizeof("Subject: [PHP-jp nnnnnnnn]"));
	if (pstr != NULL) {
		subject_buf = subject = (char *)pstr->val;
	}

	/* message body */
	orig_str.no_language = MBSTRG(language);
	orig_str.val = (unsigned char *)message;
	orig_str.len = message_len;
	orig_str.encoding = MBSTRG(current_internal_encoding);

	if (orig_str.encoding->no_encoding == mbfl_no_encoding_invalid
			|| orig_str.encoding->no_encoding == mbfl_no_encoding_pass) {
		orig_str.encoding = mbfl_identify_encoding(&orig_str, MBSTRG(current_detect_order_list), MBSTRG(current_detect_order_list_size), MBSTRG(strict_detection));
	}

	pstr = NULL;
	{
		mbfl_string tmpstr;

		if (mbfl_convert_encoding(&orig_str, &tmpstr, tran_cs) != NULL) {
			tmpstr.encoding = &mbfl_encoding_8bit;
			pstr = mbfl_convert_encoding(&tmpstr, &conv_str, body_enc);
			efree(tmpstr.val);
		}
	}
	if (pstr != NULL) {
		message_buf = message = (char *)pstr->val;
	}

	/* other headers */
#define PHP_MBSTR_MAIL_MIME_HEADER1 "MIME-Version: 1.0"
#define PHP_MBSTR_MAIL_MIME_HEADER2 "Content-Type: text/plain"
#define PHP_MBSTR_MAIL_MIME_HEADER3 "; charset="
#define PHP_MBSTR_MAIL_MIME_HEADER4 "Content-Transfer-Encoding: "
	if (str_headers != NULL) {
		p = ZSTR_VAL(str_headers);
		n = ZSTR_LEN(str_headers);
		mbfl_memory_device_strncat(&device, p, n);
		if (n > 0 && p[n - 1] != '\n') {
			mbfl_memory_device_strncat(&device, "\n", 1);
		}
		zend_string_release_ex(str_headers, 0);
	}

	if (!zend_hash_str_exists(&ht_headers, "MIME-VERSION", sizeof("MIME-VERSION") - 1)) {
		mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER1, sizeof(PHP_MBSTR_MAIL_MIME_HEADER1) - 1);
		mbfl_memory_device_strncat(&device, "\n", 1);
	}

	if (!suppressed_hdrs.cnt_type) {
		mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER2, sizeof(PHP_MBSTR_MAIL_MIME_HEADER2) - 1);

		p = (char *)mbfl_no2preferred_mime_name(tran_cs->no_encoding);
		if (p != NULL) {
			mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER3, sizeof(PHP_MBSTR_MAIL_MIME_HEADER3) - 1);
			mbfl_memory_device_strcat(&device, p);
		}
		mbfl_memory_device_strncat(&device, "\n", 1);
	}
	if (!suppressed_hdrs.cnt_trans_enc) {
		mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER4, sizeof(PHP_MBSTR_MAIL_MIME_HEADER4) - 1);
		p = (char *)mbfl_no2preferred_mime_name(body_enc->no_encoding);
		if (p == NULL) {
			p = "7bit";
		}
		mbfl_memory_device_strcat(&device, p);
		mbfl_memory_device_strncat(&device, "\n", 1);
	}

	mbfl_memory_device_unput(&device);
	mbfl_memory_device_output('\0', &device);
	str_headers = zend_string_init((char *)device.buffer, strlen((char *)device.buffer), 0);

	if (force_extra_parameters) {
		extra_cmd = php_escape_shell_cmd(force_extra_parameters);
	} else if (extra_cmd) {
		extra_cmd = php_escape_shell_cmd(ZSTR_VAL(extra_cmd));
	}

	if (!err && php_mail(to_r, subject, message, ZSTR_VAL(str_headers), extra_cmd ? ZSTR_VAL(extra_cmd) : NULL)) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}

	if (extra_cmd) {
		zend_string_release_ex(extra_cmd, 0);
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
	if (str_headers) {
		zend_string_release_ex(str_headers, 0);
	}
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


static inline mbfl_buffer_converter *php_mb_init_convd(const mbfl_encoding *encoding)
{
	mbfl_buffer_converter *convd;

	convd = mbfl_buffer_converter_new(encoding, encoding, 0);
	if (convd == NULL) {
		return NULL;
	}
	mbfl_buffer_converter_illegal_mode(convd, MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE);
	mbfl_buffer_converter_illegal_substchar(convd, 0);
	return convd;
}


static inline int php_mb_check_encoding_impl(mbfl_buffer_converter *convd, const char *input, size_t length, const mbfl_encoding *encoding) {
	mbfl_string string, result, *ret = NULL;
	size_t illegalchars = 0;

	/* initialize string */
	mbfl_string_init_set(&string, mbfl_no_language_neutral, encoding);
	mbfl_string_init(&result);

	string.val = (unsigned char *) input;
	string.len = length;

	ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
	illegalchars = mbfl_buffer_illegalchars(convd);

	if (ret != NULL) {
		if (illegalchars == 0 && string.len == result.len && memcmp(string.val, result.val, string.len) == 0) {
			mbfl_string_clear(&result);
			return 1;
		}
		mbfl_string_clear(&result);
	}
	return 0;
}


MBSTRING_API int php_mb_check_encoding(const char *input, size_t length, const char *enc)
{
	const mbfl_encoding *encoding = MBSTRG(current_internal_encoding);
	mbfl_buffer_converter *convd;

	if (input == NULL) {
		return MBSTRG(illegalchars) == 0;
	}

	if (enc != NULL) {
		encoding = mbfl_name2encoding(enc);
		if (!encoding || encoding == &mbfl_encoding_pass) {
			php_error_docref(NULL, E_WARNING, "Invalid encoding \"%s\"", enc);
			return 0;
		}
	}

	convd = php_mb_init_convd(encoding);
	if (convd == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to create converter");
		return 0;
	}

	if (php_mb_check_encoding_impl(convd, input, length, encoding)) {
		mbfl_buffer_converter_delete(convd);
		return 1;
	}
	mbfl_buffer_converter_delete(convd);
	return 0;
}


MBSTRING_API int php_mb_check_encoding_recursive(HashTable *vars, const zend_string *enc)
{
	const mbfl_encoding *encoding = MBSTRG(current_internal_encoding);
	mbfl_buffer_converter *convd;
	zend_long idx;
	zend_string *key;
	zval *entry;
	int valid = 1;

	(void)(idx);

	if (enc != NULL) {
		encoding = mbfl_name2encoding(ZSTR_VAL(enc));
		if (!encoding || encoding == &mbfl_encoding_pass) {
			php_error_docref(NULL, E_WARNING, "Invalid encoding \"%s\"", ZSTR_VAL(enc));
			return 0;
		}
	}

	convd = php_mb_init_convd(encoding);
	if (convd == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to create converter");
		return 0;
	}

	if (GC_IS_RECURSIVE(vars)) {
		mbfl_buffer_converter_delete(convd);
		php_error_docref(NULL, E_WARNING, "Cannot not handle circular references");
		return 0;
	}
	GC_TRY_PROTECT_RECURSION(vars);
	ZEND_HASH_FOREACH_KEY_VAL(vars, idx, key, entry) {
		ZVAL_DEREF(entry);
		if (key) {
			if (!php_mb_check_encoding_impl(convd, ZSTR_VAL(key), ZSTR_LEN(key), encoding)) {
				valid = 0;
				break;
			}
		}
		switch (Z_TYPE_P(entry)) {
			case IS_STRING:
				if (!php_mb_check_encoding_impl(convd, Z_STRVAL_P(entry), Z_STRLEN_P(entry), encoding)) {
					valid = 0;
					break;
				}
				break;
			case IS_ARRAY:
				if (!php_mb_check_encoding_recursive(Z_ARRVAL_P(entry), enc)) {
					valid = 0;
					break;
				}
				break;
			case IS_LONG:
			case IS_DOUBLE:
			case IS_NULL:
			case IS_TRUE:
			case IS_FALSE:
				break;
			default:
				/* Other types are error. */
				valid = 0;
				break;
		}
	} ZEND_HASH_FOREACH_END();
	GC_TRY_UNPROTECT_RECURSION(vars);
	mbfl_buffer_converter_delete(convd);
	return valid;
}


/* {{{ proto bool mb_check_encoding([mixed var[, string encoding]])
   Check if the string is valid for the specified encoding */
PHP_FUNCTION(mb_check_encoding)
{
	zval *input = NULL;
	zend_string *enc = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|zS", &input, &enc) == FAILURE) {
		return;
	}

	/* FIXME: Actually check all inputs, except $_FILES file content. */
	if (input == NULL) {
		if (MBSTRG(illegalchars) == 0) {
			RETURN_TRUE;
		}
		RETURN_FALSE;
	}

	if (Z_TYPE_P(input) == IS_ARRAY) {
		if (!php_mb_check_encoding_recursive(HASH_OF(input), enc)) {
			RETURN_FALSE;
		}
	} else {
		if (!try_convert_to_string(input)) {
			RETURN_FALSE;
		}
		if (!php_mb_check_encoding(Z_STRVAL_P(input), Z_STRLEN_P(input), enc ? ZSTR_VAL(enc): NULL)) {
			RETURN_FALSE;
		}
	}
	RETURN_TRUE;
}
/* }}} */


static inline zend_long php_mb_ord(const char *str, size_t str_len, zend_string *enc_name)
{
	const mbfl_encoding *enc;
	enum mbfl_no_encoding no_enc;

	enc = php_mb_get_encoding(enc_name);
	if (!enc) {
		return -1;
	}

	no_enc = enc->no_encoding;
	if (php_mb_is_unsupported_no_encoding(no_enc)) {
		php_error_docref(NULL, E_WARNING, "Unsupported encoding \"%s\"", enc->name);
		return -1;
	}

	if (str_len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty string");
		return -1;
	}

	{
		mbfl_wchar_device dev;
		mbfl_convert_filter *filter;
		zend_long cp;

		mbfl_wchar_device_init(&dev);
		filter = mbfl_convert_filter_new(
			enc, &mbfl_encoding_wchar,
			mbfl_wchar_device_output, 0, &dev);
		if (!filter) {
			php_error_docref(NULL, E_WARNING, "Creation of filter failed");
			return -1;
		}

		mbfl_convert_filter_feed_string(filter, (const unsigned char *) str, str_len);
		mbfl_convert_filter_flush(filter);

		if (dev.pos < 1 || filter->num_illegalchar || dev.buffer[0] >= MBFL_WCSGROUP_UCS4MAX) {
			mbfl_convert_filter_delete(filter);
			mbfl_wchar_device_clear(&dev);
			return -1;
		}

		cp = dev.buffer[0];
		mbfl_convert_filter_delete(filter);
		mbfl_wchar_device_clear(&dev);
		return cp;
	}
}


/* {{{ proto int|false mb_ord([string str[, string encoding]]) */
PHP_FUNCTION(mb_ord)
{
	char *str;
	size_t str_len;
	zend_string *enc = NULL;
	zend_long cp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(enc)
	ZEND_PARSE_PARAMETERS_END();

	cp = php_mb_ord(str, str_len, enc);

	if (0 > cp) {
		RETURN_FALSE;
	}

	RETURN_LONG(cp);
}
/* }}} */


static inline zend_string *php_mb_chr(zend_long cp, zend_string *enc_name)
{
	const mbfl_encoding *enc;
	enum mbfl_no_encoding no_enc;
	zend_string *ret;
	char* buf;
	size_t buf_len;

	enc = php_mb_get_encoding(enc_name);
	if (!enc) {
		return NULL;
	}

	no_enc = enc->no_encoding;
	if (php_mb_is_unsupported_no_encoding(no_enc)) {
		php_error_docref(NULL, E_WARNING, "Unsupported encoding \"%s\"", enc->name);
		return NULL;
	}

	if (cp < 0 || cp > 0x10ffff) {
		return NULL;
	}

	if (php_mb_is_no_encoding_utf8(no_enc)) {
		if (cp > 0xd7ff && 0xe000 > cp) {
			return NULL;
		}

		if (cp < 0x80) {
			ret = ZSTR_CHAR(cp);
		} else if (cp < 0x800) {
			ret = zend_string_alloc(2, 0);
			ZSTR_VAL(ret)[0] = 0xc0 | (cp >> 6);
			ZSTR_VAL(ret)[1] = 0x80 | (cp & 0x3f);
			ZSTR_VAL(ret)[2] = 0;
		} else if (cp < 0x10000) {
			ret = zend_string_alloc(3, 0);
			ZSTR_VAL(ret)[0] = 0xe0 | (cp >> 12);
			ZSTR_VAL(ret)[1] = 0x80 | ((cp >> 6) & 0x3f);
			ZSTR_VAL(ret)[2] = 0x80 | (cp & 0x3f);
			ZSTR_VAL(ret)[3] = 0;
		} else {
			ret = zend_string_alloc(4, 0);
			ZSTR_VAL(ret)[0] = 0xf0 | (cp >> 18);
			ZSTR_VAL(ret)[1] = 0x80 | ((cp >> 12) & 0x3f);
			ZSTR_VAL(ret)[2] = 0x80 | ((cp >> 6) & 0x3f);
			ZSTR_VAL(ret)[3] = 0x80 | (cp & 0x3f);
			ZSTR_VAL(ret)[4] = 0;
		}

		return ret;
	}

	buf_len = 4;
	buf = (char *) emalloc(buf_len + 1);
	buf[0] = (cp >> 24) & 0xff;
	buf[1] = (cp >> 16) & 0xff;
	buf[2] = (cp >>  8) & 0xff;
	buf[3] = cp & 0xff;
	buf[4] = 0;

	{
		char *ret_str;
		size_t ret_len;
		long orig_illegalchars = MBSTRG(illegalchars);
		MBSTRG(illegalchars) = 0;
		ret_str = php_mb_convert_encoding_ex(buf, buf_len, enc, &mbfl_encoding_ucs4be, &ret_len);
		if (MBSTRG(illegalchars) != 0) {
			efree(buf);
			efree(ret_str);
			MBSTRG(illegalchars) = orig_illegalchars;
			return NULL;
		}

		ret = zend_string_init(ret_str, ret_len, 0);
		efree(ret_str);
		MBSTRG(illegalchars) = orig_illegalchars;
	}

	efree(buf);
	return ret;
}


/* {{{ proto string|false mb_chr([int cp[, string encoding]]) */
PHP_FUNCTION(mb_chr)
{
	zend_long cp;
	zend_string *enc = NULL;
	zend_string* ret;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(cp)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(enc)
	ZEND_PARSE_PARAMETERS_END();

	ret = php_mb_chr(cp, enc);
	if (ret == NULL) {
		RETURN_FALSE;
	}

	RETURN_STR(ret);
}
/* }}} */


static inline char* php_mb_scrub(const char* str, size_t str_len, const mbfl_encoding *enc, size_t *ret_len)
{
	return php_mb_convert_encoding_ex(str, str_len, enc, enc, ret_len);
}


/* {{{ proto string|false mb_scrub([string str[, string encoding]]) */
PHP_FUNCTION(mb_scrub)
{
	const mbfl_encoding *enc;
	char* str;
	size_t str_len;
	zend_string *enc_name = NULL;
	char *ret;
	size_t ret_len;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(enc_name)
	ZEND_PARSE_PARAMETERS_END();

	enc = php_mb_get_encoding(enc_name);
	if (!enc) {
		RETURN_FALSE;
	}

	ret = php_mb_scrub(str, str_len, enc, &ret_len);

	if (ret == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(ret, ret_len);
	efree(ret);
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
MBSTRING_API size_t php_mb_stripos(int mode, const char *old_haystack, size_t old_haystack_len, const char *old_needle, size_t old_needle_len, zend_long offset, zend_string *from_encoding)
{
	size_t n = (size_t) -1;
	mbfl_string haystack, needle;
	const mbfl_encoding *enc;

	enc = php_mb_get_encoding(from_encoding);
	if (!enc) {
		return (size_t) -1;
	}

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.encoding = enc;
	needle.no_language = MBSTRG(language);
	needle.encoding = enc;

	do {
		/* We're using simple case-folding here, because we'd have to deal with remapping of
		 * offsets otherwise. */

		size_t len = 0;
		haystack.val = (unsigned char *)mbstring_convert_case(PHP_UNICODE_CASE_FOLD_SIMPLE, (char *)old_haystack, old_haystack_len, &len, enc);
		haystack.len = len;

		if (!haystack.val) {
			break;
		}

		if (haystack.len == 0) {
			break;
		}

		needle.val = (unsigned char *)mbstring_convert_case(PHP_UNICODE_CASE_FOLD_SIMPLE, (char *)old_needle, old_needle_len, &len, enc);
		needle.len = len;

		if (!needle.val) {
			break;
		}

		if (needle.len == 0) {
			break;
		}

 		if (offset != 0) {
 			size_t haystack_char_len = mbfl_strlen(&haystack);

 			if (mode) {
				if ((offset > 0 && (size_t)offset > haystack_char_len) ||
					(offset < 0 && (size_t)(-offset) > haystack_char_len)) {
 					php_error_docref(NULL, E_WARNING, "Offset is greater than the length of haystack string");
 					break;
 				}
 			} else {
				if (offset < 0) {
					offset += (zend_long)haystack_char_len;
				}
				if (offset < 0 || (size_t)offset > haystack_char_len) {
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
