/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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
 * PHP4 Multibyte String module "mbstring"
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
#include "ext/standard/php_smart_str.h"
#include "ext/standard/url.h"
#include "main/php_output.h"
#include "ext/standard/info.h"

#include "php_variables.h"
#include "php_globals.h"
#include "rfc1867.h"
#include "php_content_types.h"
#include "SAPI.h"
#include "php_unicode.h"
#include "TSRM.h"

#include "mb_gpc.h"

#ifdef ZEND_MULTIBYTE
#include "zend_multibyte.h"
#endif /* ZEND_MULTIBYTE */

#if HAVE_MBSTRING

#if HAVE_MBREGEX
#include "mbregex.h"
#endif
/* }}} */

/* {{{ prototypes */
static void _php_mb_globals_ctor(zend_mbstring_globals *pglobals TSRMLS_DC);
static void _php_mb_globals_dtor(zend_mbstring_globals *pglobals TSRMLS_DC);
/* }}} */

/* {{{ php_mb_default_identify_list[] */
#if defined(HAVE_MBSTR_JA)
static const enum mbfl_no_encoding php_mb_default_identify_list[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_jis,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_euc_jp,
	mbfl_no_encoding_sjis
};
#endif

#if defined(HAVE_MBSTR_CN) & !defined(HAVE_MBSTR_JA)
static const enum mbfl_no_encoding php_mb_default_identify_list[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_euc_cn,
	mbfl_no_encoding_cp936
};
#endif

#if defined(HAVE_MBSTR_TW) & !defined(HAVE_MBSTR_CN) & !defined(HAVE_MBSTR_JA)
static const enum mbfl_no_encoding php_mb_default_identify_list[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_euc_tw,
	mbfl_no_encoding_big5
};
#endif

#if defined(HAVE_MBSTR_KR) & !defined(HAVE_MBSTR_TW) & !defined(HAVE_MBSTR_CN) & !defined(HAVE_MBSTR_JA)
static const enum mbfl_no_encoding php_mb_default_identify_list[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_euc_kr,
	mbfl_no_encoding_uhc
};
#endif

#if defined(HAVE_MBSTR_RU) & !defined(HAVE_MBSTR_KR) & !defined(HAVE_MBSTR_TW) & !defined(HAVE_MBSTR_CN) & !defined(HAVE_MBSTR_JA)
static const enum mbfl_no_encoding php_mb_default_identify_list[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_koi8r,
	mbfl_no_encoding_cp1251,
	mbfl_no_encoding_cp866
};
#endif

#if !defined(HAVE_MBSTR_RU) & !defined(HAVE_MBSTR_KR) & !defined(HAVE_MBSTR_TW) & !defined(HAVE_MBSTR_CN) & !defined(HAVE_MBSTR_JA)
static const enum mbfl_no_encoding php_mb_default_identify_list[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_utf8
};
#endif

static const int php_mb_default_identify_list_size = sizeof(php_mb_default_identify_list)/sizeof(enum mbfl_no_encoding);
/* }}} */

static const unsigned char third_and_rest_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE_REST };
static const unsigned char second_args_force_ref[]    = { 2, BYREF_NONE, BYREF_FORCE };
#if HAVE_MBREGEX
static const unsigned char third_argument_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };
#endif

/* {{{ mb_overload_def mb_ovld[] */
static const struct mb_overload_def mb_ovld[] = {
	{MB_OVERLOAD_MAIL, "mail", "mb_send_mail", "mb_orig_mail"},
	{MB_OVERLOAD_STRING, "strlen", "mb_strlen", "mb_orig_strlen"},
	{MB_OVERLOAD_STRING, "strpos", "mb_strpos", "mb_orig_strpos"},
	{MB_OVERLOAD_STRING, "strrpos", "mb_strrpos", "mb_orig_strrpos"},
	{MB_OVERLOAD_STRING, "substr", "mb_substr", "mb_orig_substr"},
	{MB_OVERLOAD_STRING, "strtolower", "mb_strtolower", "mb_orig_strtolower"},
	{MB_OVERLOAD_STRING, "strtoupper", "mb_strtoupper", "mb_orig_strtoupper"},
	{MB_OVERLOAD_STRING, "substr_count", "mb_substr_count", "mb_orig_substr_count"},
#if HAVE_MBREGEX
	{MB_OVERLOAD_REGEX, "ereg", "mb_ereg", "mb_orig_ereg"},
	{MB_OVERLOAD_REGEX, "eregi", "mb_eregi", "mb_orig_eregi"},
	{MB_OVERLOAD_REGEX, "ereg_replace", "mb_ereg_replace", "mb_orig_ereg_replace"},
	{MB_OVERLOAD_REGEX, "eregi_replace", "mb_eregi_replace", "mb_orig_eregi_replace"},
	{MB_OVERLOAD_REGEX, "split", "mb_split", "mb_orig_split"},
#endif
	{0, NULL, NULL, NULL}
}; 
/* }}} */

#if HAVE_MBREGEX
struct def_mbctype_tbl {
	enum mbfl_no_encoding mbfl_encoding;
	int regex_encoding;
};

const struct def_mbctype_tbl mbctype_tbl[] = {
	{mbfl_no_encoding_ascii,MBCTYPE_ASCII},
	{mbfl_no_encoding_euc_jp,MBCTYPE_EUC},
	{mbfl_no_encoding_sjis,MBCTYPE_SJIS},
	{mbfl_no_encoding_utf8,MBCTYPE_UTF8},
	{mbfl_no_encoding_pass,-1}
};
#endif

/* {{{ function_entry mbstring_functions[] */
function_entry mbstring_functions[] = {
	PHP_FE(mb_convert_case,				NULL)
	PHP_FE(mb_strtoupper,				NULL)
	PHP_FE(mb_strtolower,				NULL)
	PHP_FE(mb_language,					NULL)
	PHP_FE(mb_internal_encoding,		NULL)
	PHP_FE(mb_http_input,				NULL)
	PHP_FE(mb_http_output,			NULL)
	PHP_FE(mb_detect_order,			NULL)
	PHP_FE(mb_substitute_character,	NULL)
	PHP_FE(mb_parse_str,			(unsigned char *)second_args_force_ref)
	PHP_FE(mb_output_handler,			NULL)
	PHP_FE(mb_preferred_mime_name,	NULL)
	PHP_FE(mb_strlen,					NULL)
	PHP_FE(mb_strpos,					NULL)
	PHP_FE(mb_strrpos,				NULL)
	PHP_FE(mb_substr_count,			NULL)
	PHP_FE(mb_substr,					NULL)
	PHP_FE(mb_strcut,					NULL)
	PHP_FE(mb_strwidth,				NULL)
	PHP_FE(mb_strimwidth,				NULL)
	PHP_FE(mb_convert_encoding,		NULL)
	PHP_FE(mb_detect_encoding,		NULL)
	PHP_FE(mb_convert_kana,			NULL)
	PHP_FE(mb_encode_mimeheader,		NULL)
	PHP_FE(mb_decode_mimeheader,		NULL)
	PHP_FE(mb_convert_variables,		(unsigned char *)third_and_rest_force_ref)
	PHP_FE(mb_encode_numericentity,		NULL)
	PHP_FE(mb_decode_numericentity,		NULL)
	PHP_FE(mb_send_mail,					NULL)
	PHP_FE(mb_get_info,					NULL)
	PHP_FALIAS(mbstrlen,	mb_strlen,	NULL)
	PHP_FALIAS(mbstrpos,	mb_strpos,	NULL)
	PHP_FALIAS(mbstrrpos,	mb_strrpos,	NULL)
	PHP_FALIAS(mbsubstr,	mb_substr,	NULL)
	PHP_FALIAS(mbstrcut,	mb_strcut,	NULL)
	PHP_FALIAS(i18n_internal_encoding,	mb_internal_encoding,	NULL)
	PHP_FALIAS(i18n_http_input,			mb_http_input,		NULL)
	PHP_FALIAS(i18n_http_output,		mb_http_output,		NULL)
	PHP_FALIAS(i18n_convert,			mb_convert_encoding,	NULL)
	PHP_FALIAS(i18n_discover_encoding,	mb_detect_encoding,	NULL)
	PHP_FALIAS(i18n_mime_header_encode,	mb_encode_mimeheader,	NULL)
	PHP_FALIAS(i18n_mime_header_decode,	mb_decode_mimeheader,	NULL)
	PHP_FALIAS(i18n_ja_jp_hantozen,		mb_convert_kana,		NULL)
#if HAVE_MBREGEX
	PHP_MBREGEX_FUNCTION_ENTRIES
#endif
	{ NULL, NULL, NULL }
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
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(mbstring)

#ifdef COMPILE_DL_MBSTRING
ZEND_GET_MODULE(mbstring)
#endif

/* {{{ static int php_mb_parse_encoding_list()
 *  Return 0 if input contains any illegal encoding, otherwise 1.
 *  Even if any illegal encoding is detected the result may contain a list 
 *  of parsed encodings.
 */
static int
php_mb_parse_encoding_list(const char *value, int value_length, int **return_list, int *return_size, int persistent)
{
	int n, l, size, bauto, *src, *list, *entry, ret = 1;
	char *p, *p1, *p2, *endp, *tmpstr;
	enum mbfl_no_encoding no_encoding;

	list = NULL;
	if (value == NULL || value_length <= 0) {
		if (return_list) {
			*return_list = NULL;
		}
		if (return_size) {
			*return_size = 0;
		}
		return 0;
	} else {
		/* copy the value string for work */
		if (value[0]=='"' && value[value_length-1]=='"' && value_length>2) {
			tmpstr = (char *)estrndup(value+1, value_length-2);
			value_length -= 2;
		}
		else
			tmpstr = (char *)estrndup(value, value_length);
		if (tmpstr == NULL) {
			return 0;
		}
		/* count the number of listed encoding names */
		endp = tmpstr + value_length;
		n = 1;
		p1 = tmpstr;
		while ((p2 = php_memnstr(p1, ",", 1, endp)) != NULL) {
			p1 = p2 + 1;
			n++;
		}
		size = n + php_mb_default_identify_list_size;
		/* make list */
		list = (int *)pecalloc(size, sizeof(int), persistent);
		if (list != NULL) {
			entry = list;
			n = 0;
			bauto = 0;
			p1 = tmpstr;
			do {
				p2 = p = php_memnstr(p1, ",", 1, endp);
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
				no_encoding = mbfl_name2no_encoding(p1);
				if (no_encoding == mbfl_no_encoding_auto) {
					if (!bauto) {
						bauto = 1;
						l = php_mb_default_identify_list_size;
						src = (int*)php_mb_default_identify_list;
						while (l > 0) {
							*entry++ = *src++;
							l--;
							n++;
						}
					}
				} else if (no_encoding != mbfl_no_encoding_invalid) {
					*entry++ = no_encoding;
					n++;
				} else {
					ret = 0;
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

/* {{{ MBSTRING_API php_mb_check_encoding_list */
MBSTRING_API int php_mb_check_encoding_list(const char *encoding_list TSRMLS_DC) {
	return php_mb_parse_encoding_list(encoding_list, strlen(encoding_list), NULL, NULL, 0);	
}
/* }}} */

/* {{{ static int php_mb_parse_encoding_array()
 *  Return 0 if input contains any illegal encoding, otherwise 1.
 *  Even if any illegal encoding is detected the result may contain a list 
 *  of parsed encodings.
 */
static int
php_mb_parse_encoding_array(zval *array, int **return_list, int *return_size, int persistent)
{
	zval **hash_entry;
	HashTable *target_hash;
	int i, n, l, size, bauto, *list, *entry, *src, ret = 1;
	enum mbfl_no_encoding no_encoding;

	list = NULL;
	if (Z_TYPE_P(array) == IS_ARRAY) {
		target_hash = Z_ARRVAL_P(array);
		zend_hash_internal_pointer_reset(target_hash);
		i = zend_hash_num_elements(target_hash);
		size = i + php_mb_default_identify_list_size;
		list = (int *)pecalloc(size, sizeof(int), persistent);
		if (list != NULL) {
			entry = list;
			bauto = 0;
			n = 0;
			while (i > 0) {
				if (zend_hash_get_current_data(target_hash, (void **) &hash_entry) == FAILURE) {
					break;
				}
				convert_to_string_ex(hash_entry);
				no_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(hash_entry));
				if (no_encoding == mbfl_no_encoding_auto) {
					if (!bauto) {
						bauto = 1;
						l = php_mb_default_identify_list_size;
						src = (int*)php_mb_default_identify_list;
						while (l > 0) {
							*entry++ = *src++;
							l--;
							n++;
						}
					}
				} else if (no_encoding != mbfl_no_encoding_invalid) {
					*entry++ = no_encoding;
					n++;
				} else {
					ret = 0;;
				}
				zend_hash_move_forward(target_hash);
				i--;
			}
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
	}

	return ret;
}
/* }}} */

/* {{{ php.ini directive handler */
static PHP_INI_MH(OnUpdate_mbstring_language)
{
	enum mbfl_no_language no_language;
	char *default_enc = NULL;

	no_language = mbfl_name2no_language(new_value);
	if (no_language != mbfl_no_language_invalid) {
		MBSTRG(language) = no_language;
		switch (no_language) {
		case mbfl_no_language_uni:
			default_enc = "UTF-8";
			break;
		case mbfl_no_language_japanese:
			default_enc = "EUC-JP";
			break;
		case mbfl_no_language_korean:
			default_enc = "EUC-KR";
			break;
		case mbfl_no_language_simplified_chinese:
			default_enc = "EUC-CN";
			break;
		case mbfl_no_language_traditional_chinese:
			default_enc = "EUC-TW";
			break;
		case mbfl_no_language_russian:
			default_enc = "KOI8-R";
			break;
		case mbfl_no_language_english:
		default:
			default_enc = "ISO-8859-1";
			break;
		}
		MBSTRG(current_language) = MBSTRG(language);
		if (default_enc) {
			zend_alter_ini_entry("mbstring.internal_encoding",
			                     sizeof("mbstring.internal_encoding"),
			                     default_enc, strlen(default_enc),
			                     PHP_INI_PERDIR, stage); 
		}
	} else {
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_detect_order) */
static PHP_INI_MH(OnUpdate_mbstring_detect_order)
{
	int *list, size;

	if (php_mb_parse_encoding_list(new_value, new_value_length, &list, &size, 1)) {
		if (MBSTRG(detect_order_list) != NULL) {
			free(MBSTRG(detect_order_list));
		}
		MBSTRG(detect_order_list) = list;
		MBSTRG(detect_order_list_size) = size;
	} else {
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_http_input) */
static PHP_INI_MH(OnUpdate_mbstring_http_input)
{
	int *list, size;

	if (php_mb_parse_encoding_list(new_value, new_value_length, &list, &size, 1)) {
		if (MBSTRG(http_input_list) != NULL) {
			free(MBSTRG(http_input_list));
		}
		MBSTRG(http_input_list) = list;
		MBSTRG(http_input_list_size) = size;
	} else {
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_http_output) */
static PHP_INI_MH(OnUpdate_mbstring_http_output)
{
	enum mbfl_no_encoding no_encoding;

	no_encoding = mbfl_name2no_encoding(new_value);
	if (no_encoding != mbfl_no_encoding_invalid) {
		MBSTRG(http_output_encoding) = no_encoding;
		MBSTRG(current_http_output_encoding) = no_encoding;
	} else {
		if (new_value != NULL && new_value_length > 0) {
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_internal_encoding) */
static PHP_INI_MH(OnUpdate_mbstring_internal_encoding)
{
	enum mbfl_no_encoding no_encoding;
#if HAVE_MBREGEX
	const struct def_mbctype_tbl *p = NULL;
#endif

	no_encoding = mbfl_name2no_encoding(new_value);
	if (no_encoding != mbfl_no_encoding_invalid) {
		MBSTRG(internal_encoding) = no_encoding;
		MBSTRG(current_internal_encoding) = no_encoding;
#if HAVE_MBREGEX
		p=&(mbctype_tbl[0]);
		while (p->regex_encoding >= 0){
			if (p->mbfl_encoding == MBSTRG(internal_encoding)){
				MBSTRG(default_mbctype) = p->regex_encoding;
				MBSTRG(current_mbctype) = p->regex_encoding;
				break;
			}
			p++;
		}
#endif
	} else {
		if (new_value != NULL && new_value_length > 0) {
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

#ifdef ZEND_MULTIBYTE
/* {{{ static PHP_INI_MH(OnUpdate_mbstring_script_encoding) */
static PHP_INI_MH(OnUpdate_mbstring_script_encoding)
{
	int *list, size;

	if (php_mb_parse_encoding_list(new_value, new_value_length, &list, &size, 1)) {
		if (MBSTRG(script_encoding_list) != NULL) {
			free(MBSTRG(script_encoding_list));
		}
		MBSTRG(script_encoding_list) = list;
		MBSTRG(script_encoding_list_size) = size;
	} else {
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */
#endif /* ZEND_MULTIBYTE */

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_substitute_character) */
static PHP_INI_MH(OnUpdate_mbstring_substitute_character)
{
	if (new_value != NULL) {
		if (strcasecmp("none", new_value) == 0) {
			MBSTRG(filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
		} else if (strcasecmp("long", new_value) == 0) {
			MBSTRG(filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG;
		} else {
			MBSTRG(filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
			MBSTRG(filter_illegal_substchar) = zend_atoi(new_value, new_value_length);
		}
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

	if (!strncasecmp(new_value, "off", sizeof("off"))) {
		new_value = "0";
		new_value_length = sizeof("0");
	} else if (!strncasecmp(new_value, "on", sizeof("on"))) {
		new_value = "1";
		new_value_length = sizeof("1");
	}

	OnUpdateInt(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);

	if (MBSTRG(encoding_translation)){
		_php_mb_enable_encoding_translation(1);
	} else {
		_php_mb_enable_encoding_translation(0);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ php.ini directive registration */
PHP_INI_BEGIN()
	 PHP_INI_ENTRY("mbstring.language", "neutral", PHP_INI_SYSTEM | PHP_INI_PERDIR, OnUpdate_mbstring_language)
	 PHP_INI_ENTRY("mbstring.detect_order", NULL, PHP_INI_ALL, OnUpdate_mbstring_detect_order)
	 PHP_INI_ENTRY("mbstring.http_input", "pass", PHP_INI_ALL, OnUpdate_mbstring_http_input)
	 PHP_INI_ENTRY("mbstring.http_output", "pass", PHP_INI_ALL, OnUpdate_mbstring_http_output)
	 PHP_INI_ENTRY("mbstring.internal_encoding", "none", PHP_INI_ALL, OnUpdate_mbstring_internal_encoding)
#ifdef ZEND_MULTIBYTE
	 PHP_INI_ENTRY("mbstring.script_encoding", NULL, PHP_INI_ALL, OnUpdate_mbstring_script_encoding)
#endif /* ZEND_MULTIBYTE */
	 PHP_INI_ENTRY("mbstring.substitute_character", NULL, PHP_INI_ALL, OnUpdate_mbstring_substitute_character)
	 STD_PHP_INI_ENTRY("mbstring.func_overload", "0", PHP_INI_SYSTEM |
	 PHP_INI_PERDIR, OnUpdateInt, func_overload, zend_mbstring_globals, mbstring_globals)
	 	 								  
	 STD_PHP_INI_BOOLEAN("mbstring.encoding_translation", "0",
	 PHP_INI_SYSTEM | PHP_INI_PERDIR, OnUpdate_mbstring_encoding_translation, 
	 encoding_translation, zend_mbstring_globals, mbstring_globals)					 
PHP_INI_END()
/* }}} */

/* {{{ module global initialize handler */
static void _php_mb_globals_ctor(zend_mbstring_globals *pglobals TSRMLS_DC)
{
	MBSTRG(language) = mbfl_no_language_uni;
	MBSTRG(current_language) = MBSTRG(language);
	MBSTRG(internal_encoding) = mbfl_no_encoding_pass;
	MBSTRG(current_internal_encoding) = MBSTRG(internal_encoding);
#ifdef ZEND_MULTIBYTE
	MBSTRG(script_encoding_list) = NULL;
	MBSTRG(script_encoding_list_size) = 0;
#endif /* ZEND_MULTIBYTE */
	MBSTRG(http_output_encoding) = mbfl_no_encoding_pass;
	MBSTRG(current_http_output_encoding) = mbfl_no_encoding_pass;
	MBSTRG(http_input_identify) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_get) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_post) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_cookie) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_string) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_list) = NULL;
	MBSTRG(http_input_list_size) = 0;
	MBSTRG(detect_order_list) = NULL;
	MBSTRG(detect_order_list_size) = 0;
	MBSTRG(current_detect_order_list) = NULL;
	MBSTRG(current_detect_order_list_size) = 0;
	MBSTRG(filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
	MBSTRG(filter_illegal_substchar) = 0x3f;	/* '?' */
	MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
	MBSTRG(current_filter_illegal_substchar) = 0x3f;	/* '?' */
	MBSTRG(func_overload) = 0;
	MBSTRG(encoding_translation) = 0;
	pglobals->outconv = NULL;
#if HAVE_MBREGEX
	_php_mb_regex_globals_ctor(pglobals TSRMLS_CC);
#endif
}
/* }}} */

/* {{{ static void _php_mb_globals_dtor() */
static void _php_mb_globals_dtor(zend_mbstring_globals *pglobals TSRMLS_DC)
{
#if HAVE_MBREGEX
	_php_mb_regex_globals_dtor(pglobals TSRMLS_CC);
#endif
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(mbstring) */
PHP_MINIT_FUNCTION(mbstring)
{
#ifdef ZTS
	ts_allocate_id(&mbstring_globals_id, sizeof(zend_mbstring_globals),
		(ts_allocate_ctor) _php_mb_globals_ctor,
		(ts_allocate_dtor) _php_mb_globals_dtor);
#else
	_php_mb_globals_ctor(&mbstring_globals TSRMLS_CC);
#endif

	REGISTER_INI_ENTRIES();

	if (MBSTRG(encoding_translation)) {
		_php_mb_enable_encoding_translation(1);
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
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION(mbstring) */
PHP_MSHUTDOWN_FUNCTION(mbstring)
{
	UNREGISTER_INI_ENTRIES();
	
	if (MBSTRG(http_input_list)) {
		free(MBSTRG(http_input_list));
	}
#ifdef ZEND_MULTIBYTE
	if (MBSTRG(script_encoding_list)) {
		free(MBSTRG(script_encoding_list));
	}
#endif /* ZEND_MULTIBYTE */
	if (MBSTRG(detect_order_list)) {
		free(MBSTRG(detect_order_list));
	}

	if (MBSTRG(encoding_translation)) {
		_php_mb_enable_encoding_translation(0);
	}

#if HAVE_MBREGEX
	PHP_MSHUTDOWN(mb_regex) (INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef ZTS
	ts_free_id(mbstring_globals_id);
#else
	_php_mb_globals_dtor(&mbstring_globals TSRMLS_CC);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION(mbstring) */
PHP_RINIT_FUNCTION(mbstring)
{
	int n, *list=NULL, *entry;
	zend_function *func, *orig;
	const struct mb_overload_def *p;

	MBSTRG(current_language) = MBSTRG(language);
	MBSTRG(current_internal_encoding) = MBSTRG(internal_encoding);
	MBSTRG(current_http_output_encoding) = MBSTRG(http_output_encoding);
	MBSTRG(current_filter_illegal_mode) = MBSTRG(filter_illegal_mode);
	MBSTRG(current_filter_illegal_substchar) = MBSTRG(filter_illegal_substchar);

	n = 0;
	if (MBSTRG(detect_order_list)) {
		list = MBSTRG(detect_order_list);
		n = MBSTRG(detect_order_list_size);
	}
	if (n <= 0) {
		list = (int*)php_mb_default_identify_list;
		n = php_mb_default_identify_list_size;
	}
	entry = (int *)emalloc(n*sizeof(int));
	MBSTRG(current_detect_order_list) = entry;
	MBSTRG(current_detect_order_list_size) = n;
	while (n > 0) {
		*entry++ = *list++;
		n--;
	}

 	/* override original function. */
	if (MBSTRG(func_overload)){
		p = &(mb_ovld[0]);
		
		while (p->type > 0) {
			if ((MBSTRG(func_overload) & p->type) == p->type && 
				zend_hash_find(EG(function_table), p->save_func,
					strlen(p->save_func)+1, (void **)&orig) != SUCCESS) {

				zend_hash_find(EG(function_table), p->ovld_func, strlen(p->ovld_func)+1 , (void **)&func);
				
				if (zend_hash_find(EG(function_table), p->orig_func, strlen(p->orig_func)+1, (void **)&orig) != SUCCESS) {
					php_error_docref("ref.mbstring" TSRMLS_CC, E_WARNING, "mbstring couldn't find function %s.", p->orig_func);
					return FAILURE;
				} else {
					zend_hash_add(EG(function_table), p->save_func, strlen(p->save_func)+1, orig, sizeof(zend_function), NULL);

					if (zend_hash_update(EG(function_table), p->orig_func, strlen(p->orig_func)+1, func, sizeof(zend_function), 
						NULL) == FAILURE) {
						php_error_docref("ref.mbstring" TSRMLS_CC, E_WARNING, "mbstring couldn't replace function %s.", p->orig_func);
						return FAILURE;
					}
				}
			}
			p++;
		}
	}
#if HAVE_MBREGEX
	PHP_RINIT(mb_regex) (INIT_FUNC_ARGS_PASSTHRU);
#endif

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
		mbfl_buffer_converter_delete(MBSTRG(outconv) TSRMLS_CC);
		MBSTRG(outconv) = NULL;
	}

	/* clear http input identification. */
	MBSTRG(http_input_identify) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_post) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_get) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_cookie) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_string) = mbfl_no_encoding_invalid;

 	/*  clear overloaded function. */
	if (MBSTRG(func_overload)){
		p = &(mb_ovld[0]);
		while (p->type > 0 && zend_hash_find(EG(function_table), p->save_func, strlen(p->save_func)+1 , (void **)&orig) == SUCCESS) {
			zend_hash_update(EG(function_table), p->orig_func, strlen(p->orig_func)+1, orig, sizeof(zend_function), NULL);
			zend_hash_del(EG(function_table), p->save_func, strlen(p->save_func)+1);
			p++;
		}
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
#if defined(HAVE_MBSTR_JA)
	php_info_print_table_row(2, "Japanese support", "enabled");	
#endif
#if defined(HAVE_MBSTR_CN)
	php_info_print_table_row(2, "Simplified chinese support", "enabled");	
#endif
#if defined(HAVE_MBSTR_TW)
	php_info_print_table_row(2, "Traditional chinese support", "enabled");	
#endif
#if defined(HAVE_MBSTR_KR)
	php_info_print_table_row(2, "Korean support", "enabled");	
#endif
	if (MBSTRG(encoding_translation)) {
		php_info_print_table_row(2, "HTTP input encoding translation", "enabled");	
	}
#if defined(HAVE_MBREGEX)
	php_info_print_table_row(2, "Multibyte (japanese) regex support", "enabled");	
#endif
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ proto string mb_language([string language])
   Sets the current language or Returns the current language as a string */
PHP_FUNCTION(mb_language)
{
	char *name = NULL;
	int name_len = 0;
	enum mbfl_no_language no_language;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
		return;
	}
	if (name == NULL) {
		RETURN_STRING((char *)mbfl_no_language2name(MBSTRG(current_language)), 1);
	} else {
		no_language = mbfl_name2no_language(name);
		if (no_language == mbfl_no_language_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown language \"%s\"", name);
			RETURN_FALSE;
		} else {
			MBSTRG(current_language) = no_language;
			RETURN_TRUE;
		}
	}
}
/* }}} */

/* {{{ proto string mb_internal_encoding([string encoding])
   Sets the current internal encoding or Returns the current internal encoding as a string */
PHP_FUNCTION(mb_internal_encoding)
{
	char *name = NULL;
	int name_len;
	enum mbfl_no_encoding no_encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
		RETURN_FALSE;
	}
	if (name == NULL) {
		name = (char *)mbfl_no_encoding2name(MBSTRG(current_internal_encoding));
		if (name != NULL) {
			RETURN_STRING(name, 1);
		} else {
			RETURN_FALSE;
		}
	} else {
		no_encoding = mbfl_name2no_encoding(name);
		if (no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", name);
			RETURN_FALSE;
		} else {
			MBSTRG(current_internal_encoding) = no_encoding;
#ifdef ZEND_MULTIBYTE
			zend_multibyte_set_internal_encoding(Z_STRVAL_PP(arg1), Z_STRLEN_PP(arg1) TSRMLS_CC);
#endif /* ZEND_MULTIBYTE */
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
	int typ_len;
	int retname, n, *entry;
	char *name, *list, *temp;
	enum mbfl_no_encoding result = mbfl_no_encoding_invalid;

	retname = 1;
 	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &typ, &typ_len) == FAILURE) {
 		RETURN_FALSE;
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
			array_init(return_value);
			entry = MBSTRG(http_input_list);
			n = MBSTRG(http_input_list_size);
			while (n > 0) {
				name = (char *)mbfl_no_encoding2name(*entry);
				if (name) {
					add_next_index_string(return_value, name, 1);
				}
				entry++;
				n--;
			}
			retname = 0;
			break;
		case 'L':
		case 'l':
			entry = MBSTRG(http_input_list);
			n = MBSTRG(http_input_list_size);
			list = NULL;
			while (n > 0) {
				name = (char *)mbfl_no_encoding2name(*entry);
				if (name) {
					if (list) {
						temp = list;
						spprintf(&list, 0, "%s,%s", temp, name);
						efree(temp);
						if (!list) { 
							break;
						}
					} else {
						list = estrdup(name);
					}
				}
				entry++;
				n--;
			}
			if (!list) {
				RETURN_FALSE;
			}
			RETVAL_STRING(list, 0);
			retname = 0;
			break;
		default:
			result = MBSTRG(http_input_identify);
			break;
		}
	}

	if (retname) {
		if (result != mbfl_no_encoding_invalid &&
			(name = (char *)mbfl_no_encoding2name(result)) != NULL) {
			RETVAL_STRING(name, 1);
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
	char *name = NULL;
	int name_len;
	enum mbfl_no_encoding no_encoding;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", (char **)&name, &name_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (name == NULL) {
		name = (char *)mbfl_no_encoding2name(MBSTRG(current_http_output_encoding));
		if (name != NULL) {
			RETURN_STRING(name, 1);
		} else {
			RETURN_FALSE;
		}
	} else {
		no_encoding = mbfl_name2no_encoding(name);
		if (no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", name);
			RETURN_FALSE;
		} else {
			MBSTRG(current_http_output_encoding) = no_encoding;
			RETURN_TRUE;
		}
	}
}
/* }}} */

/* {{{ proto bool|array mb_detect_order([mixed encoding-list])
   Sets the current detect_order or Return the current detect_order as a array */
PHP_FUNCTION(mb_detect_order)
{
	zval **arg1;
	int n, size, *list, *entry;
	char *name;

	if (ZEND_NUM_ARGS() == 0) {
		array_init(return_value);
		entry = MBSTRG(current_detect_order_list);
		n = MBSTRG(current_detect_order_list_size);
		while (n > 0) {
			name = (char *)mbfl_no_encoding2name(*entry);
			if (name) {
				add_next_index_string(return_value, name, 1);
			}
			entry++;
			n--;
		}
	} else if (ZEND_NUM_ARGS() == 1 && zend_get_parameters_ex(1, &arg1) != FAILURE) {
		list = NULL;
		size = 0;
		switch (Z_TYPE_PP(arg1)) {
		case IS_ARRAY:
			if (!php_mb_parse_encoding_array(*arg1, &list, &size, 0)) {
				if (list) {
					efree(list);
				}
				RETURN_FALSE;
			}
			break;
		default:
			convert_to_string_ex(arg1);
			if (!php_mb_parse_encoding_list(Z_STRVAL_PP(arg1), Z_STRLEN_PP(arg1), &list, &size, 0)) {
				if (list) {
					efree(list);
				}
				RETURN_FALSE;
			}
			break;
		}
		if (list == NULL) {
			RETVAL_FALSE;
		} else {
			if (MBSTRG(current_detect_order_list)) {
				efree(MBSTRG(current_detect_order_list));
			}
			MBSTRG(current_detect_order_list) = list;
			MBSTRG(current_detect_order_list_size) = size;
			RETVAL_TRUE;
		}
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto mixed mb_substitute_character([mixed substchar])
   Sets the current substitute_character or returns the current substitute_character */
PHP_FUNCTION(mb_substitute_character)
{
	zval **arg1;

	if (ZEND_NUM_ARGS() == 0) {
		if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			RETVAL_STRING("none", 1);
		} else if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG) {
			RETVAL_STRING("long", 1);
		} else {
			RETVAL_LONG(MBSTRG(current_filter_illegal_substchar));
		}
	} else if (ZEND_NUM_ARGS() == 1 && zend_get_parameters_ex(1, &arg1) != FAILURE) {
		RETVAL_TRUE;
		switch (Z_TYPE_PP(arg1)) {
		case IS_STRING:
			if (strcasecmp("none", Z_STRVAL_PP(arg1)) == 0) {
				MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
			} else if (strcasecmp("long", Z_STRVAL_PP(arg1)) == 0) {
				MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG;
			} else {
				convert_to_long_ex(arg1);
				if (Z_LVAL_PP(arg1)< 0xffff && Z_LVAL_PP(arg1)> 0x0) {
					MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
					MBSTRG(current_filter_illegal_substchar) = Z_LVAL_PP(arg1);
				} else {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown character.");					  
					RETVAL_FALSE;					
				}
			}
			break;
		default:
			convert_to_long_ex(arg1);
			if (Z_LVAL_PP(arg1)< 0xffff && Z_LVAL_PP(arg1)> 0x0) {
				MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
				MBSTRG(current_filter_illegal_substchar) = Z_LVAL_PP(arg1);
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown character.");
				RETVAL_FALSE;
			}
			break;
		}
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ proto string mb_preferred_mime_name(string encoding)
   Return the preferred MIME name (charset) as a string */
PHP_FUNCTION(mb_preferred_mime_name)
{
	enum mbfl_no_encoding no_encoding;
	char *name = NULL;
	int name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	} else {
		no_encoding = mbfl_name2no_encoding(name);
		if (no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", name);
			RETVAL_FALSE;
		} else {
			const char *preferred_name = mbfl_no2preferred_mime_name(no_encoding);
			if (preferred_name == NULL || *preferred_name == '\0') {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "No MIME preferred name corresponding to \"%s\"", name);
				RETVAL_FALSE;
			} else {
				RETVAL_STRING((char *)preferred_name, 1);
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
	zval *track_vars_array;
	char *encstr = NULL, *separator = NULL;
	int encstr_len;

	track_vars_array = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z", &encstr, &encstr_len, &track_vars_array) == FAILURE) {
		return;
	}

	separator = (char *)estrdup(PG(arg_separator).input);

	/* Clear out the array */
	if (track_vars_array != NULL) {
		zval_dtor(track_vars_array);
		array_init(track_vars_array);
	}

	encstr = estrndup(encstr, encstr_len);

	RETVAL_BOOL(_php_mb_encoding_handler_ex(PARSE_STRING, track_vars_array, encstr, separator, (track_vars_array == NULL), 1 TSRMLS_CC));

	if (encstr != NULL) efree(encstr);
	if (separator != NULL) efree(separator);
}
/* }}} */

/* {{{ proto string mb_output_handler(string contents, int status)
   Returns string in output buffer converted to the http_output encoding */
PHP_FUNCTION(mb_output_handler)
{
	char *arg_string;
	size_t arg_string_len;
	long arg_status;
	mbfl_string string, result;
	const char *charset;
	char *p;
	enum mbfl_no_encoding encoding;
	int last_feed, len;
	unsigned char send_text_mimetype = 0;
	char *s, *mimetype = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &arg_string, &arg_string_len, &arg_status) == FAILURE) {
		return;
	}

	encoding = MBSTRG(current_http_output_encoding);

 	/* start phase only */
 	if ((arg_status & PHP_OUTPUT_HANDLER_START) != 0) {
 		/* delete the converter just in case. */
 		if (MBSTRG(outconv)) {
 			mbfl_buffer_converter_delete(MBSTRG(outconv) TSRMLS_CC);
 			MBSTRG(outconv) = NULL;
  		}
		if (encoding == mbfl_no_encoding_pass) {
			RETURN_STRINGL(arg_string, arg_string_len, 1);
		}

		/* analyze mime type */
		if (SG(sapi_headers).mimetype && 
			strncmp(SG(sapi_headers).mimetype, "text/", 5) == 0) {
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
			charset = mbfl_no2preferred_mime_name(encoding);
			if (charset) {
				len = spprintf( &p, 0, "Content-Type: %s; charset=%s",  mimetype, charset ); 
				if (sapi_add_header(p, len, 0) != FAILURE) {
					SG(sapi_headers).send_default_content_type = 0;
				}
			}
 			/* activate the converter */
 			MBSTRG(outconv) = mbfl_buffer_converter_new(MBSTRG(current_internal_encoding), encoding, 0 TSRMLS_CC);
			if (send_text_mimetype){
				efree(mimetype);
			}
 		}
  	}

 	/* just return if the converter is not activated. */
 	if (MBSTRG(outconv) == NULL) {
		RETURN_STRINGL(arg_string, arg_string_len, 1);
	}

 	/* flag */
 	last_feed = ((arg_status & PHP_OUTPUT_HANDLER_END) != 0);
 	/* mode */
 	mbfl_buffer_converter_illegal_mode(MBSTRG(outconv), MBSTRG(current_filter_illegal_mode) TSRMLS_CC);
 	mbfl_buffer_converter_illegal_substchar(MBSTRG(outconv), MBSTRG(current_filter_illegal_substchar) TSRMLS_CC);
 
 	/* feed the string */
 	mbfl_string_init(&string);
 	string.no_language = MBSTRG(current_language);
 	string.no_encoding = MBSTRG(current_internal_encoding);
 	string.val = (unsigned char *)arg_string;
 	string.len = arg_string_len;
 	mbfl_buffer_converter_feed(MBSTRG(outconv), &string TSRMLS_CC);
 	if (last_feed) {
 		mbfl_buffer_converter_flush(MBSTRG(outconv) TSRMLS_CC);
	} 
 	/* get the converter output, and return it */
 	mbfl_buffer_converter_result(MBSTRG(outconv), &result TSRMLS_CC);
 	RETVAL_STRINGL((char *)result.val, result.len, 0);		/* the string is already strdup()'ed */
 
 	/* delete the converter if it is the last feed. */
 	if (last_feed) {
		mbfl_buffer_converter_delete(MBSTRG(outconv) TSRMLS_CC);
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
	int enc_name_len;

	mbfl_string_init(&string);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", (char **)&string.val, &string.len, &enc_name, &enc_name_len) == FAILURE) {
		RETURN_FALSE;
	}

	string.no_language = MBSTRG(current_language);
	if (enc_name == NULL) {
		string.no_encoding = MBSTRG(current_internal_encoding);
	} else {
		string.no_encoding = mbfl_name2no_encoding(enc_name);
		if (string.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", enc_name);
			RETURN_FALSE;
		}
	}

	n = mbfl_strlen(&string TSRMLS_CC);
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
	long offset;
	mbfl_string haystack, needle;
	char *enc_name = NULL;
	int enc_name_len;
	
	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(current_language);
	haystack.no_encoding = MBSTRG(current_internal_encoding);
	needle.no_language = MBSTRG(current_language);
	needle.no_encoding = MBSTRG(current_internal_encoding);
	offset = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ls", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &offset, &enc_name, &enc_name_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (enc_name != NULL) {
		haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(enc_name);
		if (haystack.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", enc_name);
			RETURN_FALSE;
		}
	}

	if (offset < 0 || (unsigned long)offset > haystack.len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset is out of range");
		RETURN_FALSE;
	}
	if (needle.len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty needle");
		RETURN_FALSE;
	}

	n = mbfl_strpos(&haystack, &needle, offset, reverse TSRMLS_CC);
	if (n >= 0) {
		RETVAL_LONG(n);
	} else {
		switch (-n) {
		case 1:
			break;
		case 2:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Needle has not positive length.");
			break;
		case 4:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding or conversion error.");
			break;
		case 8:
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Argument is empty.");
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown error in mb_strpos.");
			break;			
		}
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto int mb_strrpos(string haystack, string needle [, string encoding])
   Find the last occurrence of a character in a string within another */
PHP_FUNCTION(mb_strrpos)
{
	int n;
	mbfl_string haystack, needle;
	char *enc_name = NULL;
	int enc_name_len;

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(current_language);
	haystack.no_encoding = MBSTRG(current_internal_encoding);
	needle.no_language = MBSTRG(current_language);
	needle.no_encoding = MBSTRG(current_internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &enc_name, &enc_name_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (enc_name != NULL) {
		haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(enc_name);
		if (haystack.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", enc_name);
			RETURN_FALSE;
		}
	}

	if (haystack.len <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty haystack");
		RETURN_FALSE;
	}
	if (needle.len <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Empty needle");
		RETURN_FALSE;
	}
	n = mbfl_strpos(&haystack, &needle, 0, 1 TSRMLS_CC);
	if (n >= 0) {
		RETVAL_LONG(n);
	} else {
		RETVAL_FALSE;
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
	int enc_name_len;

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(current_language);
	haystack.no_encoding = MBSTRG(current_internal_encoding);
	needle.no_language = MBSTRG(current_language);
	needle.no_encoding = MBSTRG(current_internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &enc_name, &enc_name_len) == FAILURE) {
		return;
	}

	if (enc_name != NULL) {
		haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(enc_name);
		if (haystack.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", enc_name);
			RETURN_FALSE;
		}
	}

	if (needle.len <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Empty needle");
		RETURN_FALSE;
	}

	n = mbfl_substr_count(&haystack, &needle TSRMLS_CC);
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
	zval **arg1, **arg2, **arg3, **arg4;
	int argc, from, len, mblen;
	mbfl_string string, result, *ret;

	mbfl_string_init(&string);
	string.no_language = MBSTRG(current_language);
	string.no_encoding = MBSTRG(current_internal_encoding);

	argc = ZEND_NUM_ARGS();
	switch (argc) {
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 3:
		if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 4:
		if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(arg4);
		string.no_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(arg4));
		if (string.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", Z_STRVAL_PP(arg4));
			RETURN_FALSE;
		}
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	string.val = (unsigned char *)Z_STRVAL_PP(arg1);
	string.len = Z_STRLEN_PP(arg1);

	convert_to_long_ex(arg2);
	from = Z_LVAL_PP(arg2);
	if (argc >= 3) {
		convert_to_long_ex(arg3);
		len = Z_LVAL_PP(arg3);
	} else {
		len = Z_STRLEN_PP(arg1);
	}

	/* measures length */
	mblen = 0;
	if (from < 0 || len < 0) {
		mblen = mbfl_strlen(&string TSRMLS_CC);
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

	ret = mbfl_substr(&string, &result, from, len TSRMLS_CC);
	if (ret != NULL) {
		RETVAL_STRINGL((char *)ret->val, ret->len, 0);		/* the string is already strdup()'ed */
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string mb_strcut(string str, int start [, int length [, string encoding]])
   Returns part of a string */
PHP_FUNCTION(mb_strcut)
{
	pval **arg1, **arg2, **arg3, **arg4;
	int argc, from, len;
	mbfl_string string, result, *ret;

	mbfl_string_init(&string);
	string.no_language = MBSTRG(current_language);
	string.no_encoding = MBSTRG(current_internal_encoding);

	argc = ZEND_NUM_ARGS();
	switch (argc) {
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 3:
		if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 4:
		if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(arg4);
		string.no_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(arg4));
		if (string.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", Z_STRVAL_PP(arg4));
			RETURN_FALSE;
		}
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	string.val = Z_STRVAL_PP(arg1);
	string.len = Z_STRLEN_PP(arg1);

	convert_to_long_ex(arg2);
	from = Z_LVAL_PP(arg2);
	if (argc >= 3) {
		convert_to_long_ex(arg3);
		len = Z_LVAL_PP(arg3);
	} else {
		len = Z_STRLEN_PP(arg1);
	}

	/* if "from" position is negative, count start position from the end
	 * of the string
	 */
	if (from < 0) {
		from = Z_STRLEN_PP(arg1) + from;
		if (from < 0) {
			from = 0;
		}
	}

	/* if "length" position is negative, set it to the length
	 * needed to stop that many chars from the end of the string
	 */
	if (len < 0) {
		len = (Z_STRLEN_PP(arg1) - from) + len;
		if (len < 0) {
			len = 0;
		}
	}

	ret = mbfl_strcut(&string, &result, from, len TSRMLS_CC);
	if (ret != NULL) {
		RETVAL_STRINGL(ret->val, ret->len, 0);		/* the string is already strdup()'ed */
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto int mb_strwidth(string str [, string encoding])
   Gets terminal width of a string */
PHP_FUNCTION(mb_strwidth)
{
	int n;
	mbfl_string string;
	char *enc_name = NULL;
	int enc_name_len;

	mbfl_string_init(&string);

	string.no_language = MBSTRG(current_language);
	string.no_encoding = MBSTRG(current_internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", (char **)&string.val, &string.len, &enc_name, &enc_name_len) == FAILURE) {
		return;
	}

	if (enc_name != NULL) {
		string.no_encoding = mbfl_name2no_encoding(enc_name);
		if (string.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", enc_name);
			RETURN_FALSE;
		}
	}

	n = mbfl_strwidth(&string TSRMLS_CC);
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
	pval **arg1, **arg2, **arg3, **arg4, **arg5;
	int from, width;
	mbfl_string string, result, marker, *ret;

	mbfl_string_init(&string);
	mbfl_string_init(&marker);
	string.no_language = MBSTRG(current_language);
	string.no_encoding = MBSTRG(current_internal_encoding);
	marker.no_language = MBSTRG(current_language);
	marker.no_encoding = MBSTRG(current_internal_encoding);
	marker.val = NULL;
	marker.len = 0;

	switch (ZEND_NUM_ARGS()) {
	case 3:
		if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 4:
		if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 5:
		if (zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(arg5);
		string.no_encoding = marker.no_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(arg5));
		if (string.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", Z_STRVAL_PP(arg5));
			RETURN_FALSE;
		}
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	string.val = (unsigned char *)Z_STRVAL_PP(arg1);
	string.len = Z_STRLEN_PP(arg1);

	convert_to_long_ex(arg2);
	from = Z_LVAL_PP(arg2);
	if (from < 0 || from > Z_STRLEN_PP(arg1)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Start position is out of reange");
		RETURN_FALSE;
	}

	convert_to_long_ex(arg3);
	width = Z_LVAL_PP(arg3);

	if (width < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Width is negative value");
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() >= 4) {
		convert_to_string_ex(arg4);
		marker.val = (unsigned char *)Z_STRVAL_PP(arg4);
		marker.len = Z_STRLEN_PP(arg4);
	}

	ret = mbfl_strimwidth(&string, &marker, &result, from, width TSRMLS_CC);
	if (ret != NULL) {
		RETVAL_STRINGL((char *)ret->val, ret->len, 0);		/* the string is already strdup()'ed */
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ MBSTRING_API char *php_mb_convert_encoding() */
MBSTRING_API char * php_mb_convert_encoding(char *input, size_t length, char *_to_encoding, char *_from_encodings, size_t *output_len TSRMLS_DC)
{
	mbfl_string string, result, *ret;
	enum mbfl_no_encoding from_encoding, to_encoding;
	mbfl_buffer_converter *convd;
	int size, *list;
	char *output=NULL;

	if (output_len) {
		*output_len = 0;
	}
	if ( !input || !length) {
		return NULL;
	}
	/* new encoding */
	if (_to_encoding && strlen(_to_encoding)) {
		to_encoding = mbfl_name2no_encoding(_to_encoding);
		if (to_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", _to_encoding);
			return NULL;
		}
	} else {
		to_encoding = MBSTRG(current_internal_encoding);
	}

	/* initialize string */
	mbfl_string_init(&string);
	mbfl_string_init(&result);
	from_encoding = MBSTRG(current_internal_encoding);
	string.no_encoding = from_encoding;
	string.no_language = MBSTRG(current_language);
	string.val = (unsigned char *)input;
	string.len = length;

	/* pre-conversion encoding */
	if (_from_encodings) {
		list = NULL;
		size = 0;
	    php_mb_parse_encoding_list(_from_encodings, strlen(_from_encodings), &list, &size, 0);
		if (size == 1) {
			from_encoding = *list;
			string.no_encoding = from_encoding;
		} else if (size > 1) {
			/* auto detect */
			from_encoding = mbfl_identify_encoding_no(&string, list, size TSRMLS_CC);
			if (from_encoding != mbfl_no_encoding_invalid) {
				string.no_encoding = from_encoding;
			} else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to detect character encoding");
				from_encoding = mbfl_no_encoding_pass;
				to_encoding = from_encoding;
				string.no_encoding = from_encoding;
			}
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Illegal character encoding specified");
		}
		if (list != NULL) {
			efree((void *)list);
		}
	}

	/* initialize converter */
	convd = mbfl_buffer_converter_new(from_encoding, to_encoding, string.len TSRMLS_CC);
	if (convd == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create character encoding converter");
		return NULL;
	}
	mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode) TSRMLS_CC);
	mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar) TSRMLS_CC);

	/* do it */
	ret = mbfl_buffer_converter_feed_result(convd, &string, &result TSRMLS_CC);
	if (ret) {
		if (output_len) {
			*output_len = ret->len;
		}
		output = (char *)ret->val;
	}

	mbfl_buffer_converter_delete(convd TSRMLS_CC);
	return output;
}
/* }}} */

/* {{{ proto string mb_convert_encoding(string str, string to-encoding [, mixed from-encoding])
   Returns converted string in desired encoding */
PHP_FUNCTION(mb_convert_encoding)
{
	pval **arg_str, **arg_new, **arg_old;
	int i;
	size_t size, l, n;
	char *_from_encodings, *ret, *s_free = NULL;

	zval **hash_entry;
	HashTable *target_hash;

	_from_encodings = NULL;
	if (ZEND_NUM_ARGS() == 2) {
		if (zend_get_parameters_ex(2, &arg_str, &arg_new) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (ZEND_NUM_ARGS() == 3) {
		if (zend_get_parameters_ex(3, &arg_str, &arg_new, &arg_old) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		switch (Z_TYPE_PP(arg_old)) {
		case IS_ARRAY:

			target_hash = Z_ARRVAL_PP(arg_old);
			zend_hash_internal_pointer_reset(target_hash);
			i = zend_hash_num_elements(target_hash);
			_from_encodings = NULL;
			while (i > 0) {
				if (zend_hash_get_current_data(target_hash, (void **) &hash_entry) == FAILURE) {
					break;
				}
				convert_to_string_ex(hash_entry);					
				if ( _from_encodings) {
					l = strlen(_from_encodings);
					n = strlen(Z_STRVAL_PP(hash_entry));
					_from_encodings = erealloc(_from_encodings, l+n+2);
					strcpy(_from_encodings+l,",");
					strcpy(_from_encodings+l+1,Z_STRVAL_PP(hash_entry));
				} else {
					_from_encodings = estrdup(Z_STRVAL_PP(hash_entry));
				}
				zend_hash_move_forward(target_hash);
				i--;
			}
			if ( !strlen(_from_encodings)) {
				efree(_from_encodings);
				_from_encodings = NULL;
			}
			s_free = _from_encodings;
			break;
		default:
			convert_to_string_ex(arg_old);
			_from_encodings = Z_STRVAL_PP(arg_old);
			break;
		}
	} else {
		WRONG_PARAM_COUNT;
	}

	/* new encoding */
	convert_to_string_ex(arg_str);
	convert_to_string_ex(arg_new);
	ret = php_mb_convert_encoding( Z_STRVAL_PP(arg_str), Z_STRLEN_PP(arg_str), Z_STRVAL_PP(arg_new), _from_encodings, &size TSRMLS_CC);
	if (ret != NULL) {
		RETVAL_STRINGL(ret, size, 0);		/* the string is already strdup()'ed */
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
	char *str, *from_encoding = (char*)mbfl_no2preferred_mime_name(MBSTRG(current_internal_encoding));
	long str_len, from_encoding_len;
	long case_mode = 0;
	char *newstr;
	size_t ret_len;

	RETVAL_FALSE;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|s!", &str, &str_len,
				&case_mode, &from_encoding, &from_encoding_len) == FAILURE)
		RETURN_FALSE;

	newstr = php_unicode_convert_case(case_mode, str, str_len, &ret_len, from_encoding TSRMLS_CC);

	if (newstr) {
		RETVAL_STRINGL(newstr, ret_len, 0);
	}	
}
/* }}} */

/* {{{ proto string mb_strtoupper(string sourcestring [, string encoding])
 *  Returns a uppercased version of sourcestring
 */
PHP_FUNCTION(mb_strtoupper)
{
	char *str, *from_encoding = (char*)mbfl_no2preferred_mime_name(MBSTRG(current_internal_encoding));
	long str_len, from_encoding_len;
	char *newstr;
	size_t ret_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!", &str, &str_len,
				&from_encoding, &from_encoding_len) == FAILURE) {
		return;
	}
	newstr = php_unicode_convert_case(PHP_UNICODE_CASE_UPPER, str, str_len, &ret_len, from_encoding TSRMLS_CC);

	if (newstr) {
		RETURN_STRINGL(newstr, ret_len, 0);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string mb_strtolower(string sourcestring [, string encoding])
 *  Returns a lowercased version of sourcestring
 */
PHP_FUNCTION(mb_strtolower)
{
	char *str, *from_encoding = (char*)mbfl_no2preferred_mime_name(MBSTRG(current_internal_encoding));
	long str_len, from_encoding_len;
	char *newstr;
	size_t ret_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!", &str, &str_len,
				&from_encoding, &from_encoding_len) == FAILURE) {
		return;
	}
	newstr = php_unicode_convert_case(PHP_UNICODE_CASE_LOWER, str, str_len, &ret_len, from_encoding TSRMLS_CC);

	if (newstr) {
		RETURN_STRINGL(newstr, ret_len, 0);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string mb_detect_encoding(string str [, mixed encoding_list])
   Encodings of the given string is returned (as a string) */
PHP_FUNCTION(mb_detect_encoding)
{
	pval **arg_str, **arg_list;
	mbfl_string string;
	const char *ret;
	enum mbfl_no_encoding *elist;
	int size, *list;

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &arg_str) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (ZEND_NUM_ARGS() == 2) {
		if (zend_get_parameters_ex(2, &arg_str, &arg_list) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else {
		WRONG_PARAM_COUNT;
	}

	/* make encoding list */
	list = NULL;
	size = 0;
	if (ZEND_NUM_ARGS() >= 2) {
		switch (Z_TYPE_PP(arg_list)) {
		case IS_ARRAY:
			if (!php_mb_parse_encoding_array(*arg_list, &list, &size, 0)) {
				if (list) {
					efree(list);
					size = 0;
				}
			}
			break;
		default:
			convert_to_string_ex(arg_list);
			if (!php_mb_parse_encoding_list(Z_STRVAL_PP(arg_list), Z_STRLEN_PP(arg_list), &list, &size, 0)) {
				if (list) {
					efree(list);
					size = 0;
				}
			}
			break;
		}
		if (size <= 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Illegal argument");
		}
	}

	if (size > 0 && list != NULL) {
		elist = list;
	} else {
		elist = MBSTRG(current_detect_order_list);
		size = MBSTRG(current_detect_order_list_size);
	}

	convert_to_string_ex(arg_str);
	mbfl_string_init(&string);
	string.no_language = MBSTRG(current_language);
	string.val = (unsigned char *)Z_STRVAL_PP(arg_str);
	string.len = Z_STRLEN_PP(arg_str);
	ret = mbfl_identify_encoding_name(&string, elist, size TSRMLS_CC);
	if (list != NULL) {
		efree((void *)list);
	}
	if (ret != NULL) {
		RETVAL_STRING((char *)ret, 1);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string mb_encode_mimeheader(string str [, string charset [, string transfer-encoding [, string linefeed]]])
   Converts the string to MIME "encoded-word" in the format of =?charset?(B|Q)?encoded_string?= */
PHP_FUNCTION(mb_encode_mimeheader)
{
	enum mbfl_no_encoding charset, transenc;
	mbfl_string  string, result, *ret;
	char *charset_name = NULL;
	int charset_name_len;
	char *trans_enc_name = NULL;
	int trans_enc_name_len;
	char *linefeed = "\r\n";
	int linefeed_len;

	mbfl_string_init(&string);
	string.no_language = MBSTRG(current_language);
	string.no_encoding = MBSTRG(current_internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sss", (char **)&string.val, &string.len, &charset_name, &charset_name_len, &trans_enc_name, &trans_enc_name_len, &linefeed, &linefeed_len) == FAILURE) {
		return;
	}

	charset = mbfl_no_encoding_pass;
	transenc = mbfl_no_encoding_base64;

	if (charset_name != NULL) {
		charset = mbfl_name2no_encoding(charset_name);
		if (charset == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", charset_name);
			RETURN_FALSE;
		}
	} else {
		const mbfl_language *lang = mbfl_no2language(MBSTRG(current_language));
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
	ret = mbfl_mime_header_encode(&string, &result, charset, transenc, linefeed, 0 TSRMLS_CC);
	if (ret != NULL) {
		RETVAL_STRINGL((char *)ret->val, ret->len, 0)	/* the string is already strdup()'ed */
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

	mbfl_string_init(&string);
	string.no_language = MBSTRG(current_language);
	string.no_encoding = MBSTRG(current_internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", (char **)&string.val, &string.len) == FAILURE) {
		return;
	}

	mbfl_string_init(&result);
	ret = mbfl_mime_header_decode(&string, &result, MBSTRG(current_internal_encoding) TSRMLS_CC);
	if (ret != NULL) {
		RETVAL_STRINGL((char *)ret->val, ret->len, 0)	/* the string is already strdup()'ed */
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
	int optstr_len;
	char *encname = NULL;
	int encname_len;

	mbfl_string_init(&string);
	string.no_language = MBSTRG(current_language);
	string.no_encoding = MBSTRG(current_internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ss", (char **)&string.val, &string.len, &optstr, &optstr_len, &encname, &encname_len) == FAILURE) {
		return;
	}

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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", encname);
			RETURN_FALSE;
		}
	}

	ret = mbfl_ja_jp_hantozen(&string, &result, opt TSRMLS_CC);
	if (ret != NULL) {
		RETVAL_STRINGL((char *)ret->val, ret->len, 0);		/* the string is already strdup()'ed */
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

#define PHP_MBSTR_STACK_BLOCK_SIZE 32

/* {{{ proto string mb_convert_variables(string to-encoding, mixed from-encoding [, mixed ...])
   Converts the string resource in variables to desired encoding */
PHP_FUNCTION(mb_convert_variables)
{
	pval ***args, ***stack, **var, **hash_entry;
	HashTable *target_hash;
	mbfl_string string, result, *ret;
	enum mbfl_no_encoding from_encoding, to_encoding;
	mbfl_encoding_detector *identd;
	mbfl_buffer_converter *convd;
	int n, argc, stack_level, stack_max, *elist, elistsz;
	char *name;
	void *ptmp;

	argc = ZEND_NUM_ARGS();
	if (argc < 3) {
		WRONG_PARAM_COUNT;
	}
	args = (pval ***)ecalloc(argc, sizeof(pval **));
	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree((void *)args);
		WRONG_PARAM_COUNT;
	}

	/* new encoding */
	convert_to_string_ex(args[0]);
	to_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(args[0]));
	if (to_encoding == mbfl_no_encoding_invalid) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", Z_STRVAL_PP(args[0]));
		efree((void *)args);
		RETURN_FALSE;
	}

	/* initialize string */
	mbfl_string_init(&string);
	mbfl_string_init(&result);
	from_encoding = MBSTRG(current_internal_encoding);
	string.no_encoding = from_encoding;
	string.no_language = MBSTRG(current_language);

	/* pre-conversion encoding */
	elist = NULL;
	elistsz = 0;
	switch (Z_TYPE_PP(args[1])) {
	case IS_ARRAY:
		php_mb_parse_encoding_array(*args[1], &elist, &elistsz, 0);
		break;
	default:
		convert_to_string_ex(args[1]);
		php_mb_parse_encoding_list(Z_STRVAL_PP(args[1]), Z_STRLEN_PP(args[1]), &elist, &elistsz, 0);
		break;
	}
	if (elistsz <= 0) {
		from_encoding = mbfl_no_encoding_pass;
	} else if (elistsz == 1) {
		from_encoding = *elist;
	} else {
		/* auto detect */
		from_encoding = mbfl_no_encoding_invalid;
		stack_max = PHP_MBSTR_STACK_BLOCK_SIZE;
		stack = (pval ***)emalloc(stack_max*sizeof(pval **));
		stack_level = 0;
		identd = mbfl_encoding_detector_new(elist, elistsz TSRMLS_CC);
		if (identd != NULL) {
			n = 2;
			while (n < argc || stack_level > 0) {
				if (stack_level <= 0) {
					var = args[n++];
					if (Z_TYPE_PP(var) == IS_ARRAY || Z_TYPE_PP(var) == IS_OBJECT) {
						target_hash = HASH_OF(*var);
						if (target_hash != NULL) {
							zend_hash_internal_pointer_reset(target_hash);
						}
					}
				} else {
					stack_level--;
					var = stack[stack_level];
				}
				if (Z_TYPE_PP(var) == IS_ARRAY || Z_TYPE_PP(var) == IS_OBJECT) {
					target_hash = HASH_OF(*var);
					if (target_hash != NULL) {
						while (zend_hash_get_current_data(target_hash, (void **) &hash_entry) != FAILURE) {
							zend_hash_move_forward(target_hash);
							if (Z_TYPE_PP(hash_entry) == IS_ARRAY || Z_TYPE_PP(hash_entry) == IS_OBJECT) {
								if (stack_level >= stack_max) {
									stack_max += PHP_MBSTR_STACK_BLOCK_SIZE;
									ptmp = erealloc(stack, sizeof(pval **)*stack_max);
									stack = (pval ***)ptmp;
								}
								stack[stack_level] = var;
								stack_level++;
								var = hash_entry;
								target_hash = HASH_OF(*var);
								if (target_hash != NULL) {
									zend_hash_internal_pointer_reset(target_hash);
									continue;
								}
							} else if (Z_TYPE_PP(hash_entry) == IS_STRING) {
								string.val = (unsigned char *)Z_STRVAL_PP(hash_entry);
								string.len = Z_STRLEN_PP(hash_entry);
								if (mbfl_encoding_detector_feed(identd, &string TSRMLS_CC)) {
									goto detect_end;		/* complete detecting */
								}
							}
						}
					}
				} else if (Z_TYPE_PP(var) == IS_STRING) {
					string.val = (unsigned char *)Z_STRVAL_PP(var);
					string.len = Z_STRLEN_PP(var);
					if (mbfl_encoding_detector_feed(identd, &string TSRMLS_CC)) {
						goto detect_end;		/* complete detecting */
					}
				}
			}
detect_end:
			from_encoding = mbfl_encoding_detector_judge(identd TSRMLS_CC);
			mbfl_encoding_detector_delete(identd TSRMLS_CC);
		}
		efree(stack);

		if (from_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to detect encoding");
			from_encoding = mbfl_no_encoding_pass;
		}
	}
	if (elist != NULL) {
		efree((void *)elist);
	}
	/* create converter */
	convd = NULL;
	if (from_encoding != mbfl_no_encoding_pass) {
		convd = mbfl_buffer_converter_new(from_encoding, to_encoding, 0 TSRMLS_CC);
		if (convd == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create converter");
			RETURN_FALSE;
		}
		mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode) TSRMLS_CC);
		mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar) TSRMLS_CC);
	}

	/* convert */
	if (convd != NULL) {
		stack_max = PHP_MBSTR_STACK_BLOCK_SIZE;
		stack = (pval ***)emalloc(stack_max*sizeof(pval **));
		stack_level = 0;
		n = 2;
		while (n < argc || stack_level > 0) {
			if (stack_level <= 0) {
				var = args[n++];
				if (Z_TYPE_PP(var) == IS_ARRAY || Z_TYPE_PP(var) == IS_OBJECT) {
					target_hash = HASH_OF(*var);
					if (target_hash != NULL) {
						zend_hash_internal_pointer_reset(target_hash);
					}
				}
			} else {
				stack_level--;
				var = stack[stack_level];
			}
			if (Z_TYPE_PP(var) == IS_ARRAY || Z_TYPE_PP(var) == IS_OBJECT) {
				target_hash = HASH_OF(*var);
				if (target_hash != NULL) {
					while (zend_hash_get_current_data(target_hash, (void **) &hash_entry) != FAILURE) {
						zend_hash_move_forward(target_hash);
						if (Z_TYPE_PP(hash_entry) == IS_ARRAY || Z_TYPE_PP(hash_entry) == IS_OBJECT) {
							if (stack_level >= stack_max) {
								stack_max += PHP_MBSTR_STACK_BLOCK_SIZE;
								ptmp = erealloc(stack, sizeof(pval **)*stack_max);
								stack = (pval ***)ptmp;
							}
							stack[stack_level] = var;
							stack_level++;
							var = hash_entry;
							target_hash = HASH_OF(*var);
							if (target_hash != NULL) {
								zend_hash_internal_pointer_reset(target_hash);
								continue;
							}
						} else if (Z_TYPE_PP(hash_entry) == IS_STRING) {
							string.val = (unsigned char *)Z_STRVAL_PP(hash_entry);
							string.len = Z_STRLEN_PP(hash_entry);
							ret = mbfl_buffer_converter_feed_result(convd, &string, &result TSRMLS_CC);
							if (ret != NULL) {
								STR_FREE(Z_STRVAL_PP(hash_entry));
								Z_STRVAL_PP(hash_entry) = (char *)ret->val;
								Z_STRLEN_PP(hash_entry) = ret->len;
							}
						}
					}
				}
			} else if (Z_TYPE_PP(var) == IS_STRING) {
				string.val = (unsigned char *)Z_STRVAL_PP(var);
				string.len = Z_STRLEN_PP(var);
				ret = mbfl_buffer_converter_feed_result(convd, &string, &result TSRMLS_CC);
				if (ret != NULL) {
					STR_FREE(Z_STRVAL_PP(var));
					Z_STRVAL_PP(var) = (char *)ret->val;
					Z_STRLEN_PP(var) = ret->len;
				}
			}
		}
		efree(stack);

		mbfl_buffer_converter_delete(convd TSRMLS_CC);
	}

	efree((void *)args);

	name = (char *)mbfl_no_encoding2name(from_encoding);
	if (name != NULL) {
		RETURN_STRING(name, 1);
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
	pval **arg1, **arg2, **arg3, **hash_entry;
	HashTable *target_hash;
	int argc, i, *convmap, *mapelm, mapsize=0;
	mbfl_string string, result, *ret;
	enum mbfl_no_encoding no_encoding;

	argc = ZEND_NUM_ARGS();
	if ((argc == 2 && zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) ||
	   (argc == 3 && zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) ||
		argc < 2 || argc > 3) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	mbfl_string_init(&string);
	string.no_language = MBSTRG(current_language);
	string.no_encoding = MBSTRG(current_internal_encoding);
	string.val = (unsigned char *)Z_STRVAL_PP(arg1);
	string.len = Z_STRLEN_PP(arg1);

	/* encoding */
	if (argc == 3) {
		convert_to_string_ex(arg3);
		no_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(arg3));
		if (no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", Z_STRVAL_PP(arg3));
			RETURN_FALSE;
		} else {
			string.no_encoding = no_encoding;
		}
	}

	/* conversion map */
	convmap = NULL;
	if (Z_TYPE_PP(arg2) == IS_ARRAY){
		target_hash = Z_ARRVAL_PP(arg2);
		zend_hash_internal_pointer_reset(target_hash);
		i = zend_hash_num_elements(target_hash);
		if (i > 0) {
			convmap = (int *)emalloc(i*sizeof(int));
			mapelm = convmap;
			mapsize = 0;
			while (i > 0) {
				if (zend_hash_get_current_data(target_hash, (void **) &hash_entry) == FAILURE) {
					break;
				}
				convert_to_long_ex(hash_entry);
				*mapelm++ = Z_LVAL_PP(hash_entry);
				mapsize++;
				i--;
				zend_hash_move_forward(target_hash);
			}
		}
	}
	if (convmap == NULL) {
		RETURN_FALSE;
	}
	mapsize /= 4;

	ret = mbfl_html_numeric_entity(&string, &result, convmap, mapsize, type TSRMLS_CC);
	if (ret != NULL) {
		RETVAL_STRINGL((char *)ret->val, ret->len, 0);
	} else {
		RETVAL_FALSE;
	}
	efree((void *)convmap);
}
/* }}} */

/* {{{ proto string mb_encode_numericentity(string string, array convmap [, string encoding])
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
#if HAVE_SENDMAIL

#define APPEND_ONE_CHAR(ch) do { \
	if (token.a > 0) { \
		smart_str_appendc(&token, ch); \
	} else {\
		token.len++; \
	} \
} while (0)

#define SEPARATE_SMART_STR(str) do {\
	if ((str)->a == 0) { \
		char *tmp_ptr; \
		(str)->a = 1; \
		while ((str)->a < (str)->len) { \
			(str)->a <<= 1; \
		} \
		tmp_ptr = emalloc((str)->a + 1); \
		memcpy(tmp_ptr, (str)->c, (str)->len); \
		(str)->c = tmp_ptr; \
	} \
} while (0)

static void my_smart_str_dtor(smart_str *s)
{
	if (s->a > 0) {
		smart_str_free(s);
	}
}

static int _php_mbstr_parse_mail_headers(HashTable *ht, const char *str, size_t str_len)
{
	const char *ps;
	size_t icnt;
	int state = 0;
	int crlf_state = -1;

	smart_str token = { 0, 0, 0 };
	smart_str fld_name = { 0, 0, 0 }, fld_val = { 0, 0, 0 };

	ps = str;
	icnt = str_len;

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
					APPEND_ONE_CHAR('\r');
				}

				if (state == 0 || state == 1) {
					fld_name = token;

					state = 2;
				} else {
					APPEND_ONE_CHAR(*ps);
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
					APPEND_ONE_CHAR('\r');
				} else {
					crlf_state = 1;
				}
				break;

			case ' ': case '\t':
				if (crlf_state == -1) {
					if (state == 3) {
						/* continuing from the previous line */
						SEPARATE_SMART_STR(&token);
						state = 4;
					} else {
						/* simply skipping this new line */
						state = 5;
					}
				} else {
					if (crlf_state == 1) {
						APPEND_ONE_CHAR('\r');
					}
					if (state == 1 || state == 3) {
						APPEND_ONE_CHAR(*ps);
					}
				}
				crlf_state = 0;
				break;

			default:
				switch (state) {
					case 0:
						token.c = (char *)ps;
						token.len = 0;
						token.a = 0;
						state = 1;
						break;
					
					case 2:
						if (crlf_state != -1) {
							token.c = (char *)ps;
							token.len = 0;
							token.a = 0;

							state = 3;
							break;
						}
						/* break is missing intentionally */

					case 3:
						if (crlf_state == -1) {
							fld_val = token;

							if (fld_name.c != NULL && fld_val.c != NULL) {
								char *dummy;

								/* FIXME: some locale free implementation is
								 * really required here,,, */
								SEPARATE_SMART_STR(&fld_name);
								php_strtoupper(fld_name.c, fld_name.len);

								zend_hash_update(ht, (char *)fld_name.c, fld_name.len, &fld_val, sizeof(smart_str), (void **)&dummy);

								my_smart_str_dtor(&fld_name);
							}

							memset(&fld_name, 0, sizeof(smart_str));
							memset(&fld_val, 0, sizeof(smart_str));

							token.c = (char *)ps;
							token.len = 0;
							token.a = 0;

							state = 1;
						}
						break;

					case 4:
						APPEND_ONE_CHAR(' ');
						state = 3;
						break;
				}

				if (crlf_state == 1) {
					APPEND_ONE_CHAR('\r');
				}

				APPEND_ONE_CHAR(*ps);

				crlf_state = 0;
				break;
		}
		ps++, icnt--;
	}
out:
	if (state == 2) {
		token.c = "";
		token.len = 0;
		token.a = 0;

		state = 3;
	}
	if (state == 3) {
		fld_val = token;

		if (fld_name.c != NULL && fld_val.c != NULL) {
			void *dummy;

			/* FIXME: some locale free implementation is
			 * really required here,,, */
			SEPARATE_SMART_STR(&fld_name);
			php_strtoupper(fld_name.c, fld_name.len);

			zend_hash_update(ht, (char *)fld_name.c, fld_name.len, &fld_val, sizeof(smart_str), (void **)&dummy);

			my_smart_str_dtor(&fld_name);
		}
	}
	return state;
}

PHP_FUNCTION(mb_send_mail)
{
	int n;
	char *to=NULL;
	int to_len;
	char *message=NULL;
	int message_len;
	char *headers=NULL;
	int headers_len;
	char *subject=NULL;
	int subject_len;
	char *extra_cmd=NULL;
	int extra_cmd_len;
	struct {
		int cnt_type:1;
		int cnt_trans_enc:1;
	} suppressed_hdrs = { 0, 0 };

	char *message_buf=NULL, *subject_buf=NULL, *p;
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
	smart_str *s;
	extern void mbfl_memory_device_unput(mbfl_memory_device *device TSRMLS_DC);
	
	/* initialize */
	mbfl_memory_device_init(&device, 0, 0 TSRMLS_CC);
	mbfl_string_init(&orig_str);
	mbfl_string_init(&conv_str);

	/* character-set, transfer-encoding */
	tran_cs = mbfl_no_encoding_utf8;
	head_enc = mbfl_no_encoding_base64;
	body_enc = mbfl_no_encoding_base64;
	lang = mbfl_no2language(MBSTRG(current_language));
	if (lang != NULL) {
		tran_cs = lang->mail_charset;
		head_enc = lang->mail_header_encoding;
		body_enc = lang->mail_body_encoding;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|ss", &to, &to_len, &subject, &subject_len, &message, &message_len, &headers, &headers_len, &extra_cmd, &extra_cmd_len) == FAILURE) {
		return;
	}

	zend_hash_init(&ht_headers, 0, NULL, (dtor_func_t) my_smart_str_dtor, 0);

	if (headers != NULL) {
		_php_mbstr_parse_mail_headers(&ht_headers, headers, headers_len);
	}

	if (zend_hash_find(&ht_headers, "CONTENT-TYPE", sizeof("CONTENT-TYPE") - 1, (void **)&s) == SUCCESS) {
		char *tmp;
		char *param_name;
		char *charset = NULL;

		SEPARATE_SMART_STR(s);
		smart_str_0(s);

		p = strchr(s->c, ';');

		if (p != NULL) {
			/* skipping the padded spaces */
			do {
				++p;
			} while (*p == ' ' || *p == '\t');

			if (*p != '\0') {
				if ((param_name = php_strtok_r(p, "= ", &tmp)) != NULL) {
					if (strcasecmp(param_name, "charset") == 0) {
						enum mbfl_no_encoding _tran_cs = tran_cs;
						
						charset = php_strtok_r(NULL, "= ", &tmp);
						if (charset != NULL) {
							_tran_cs = mbfl_name2no_encoding(charset);
						}

						if (_tran_cs == mbfl_no_encoding_invalid) {
							php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unsupported charset \"%s\" - will be regarded as ascii", charset); 
							_tran_cs = mbfl_no_encoding_ascii;
						}
						tran_cs = _tran_cs;
					}
				}
			}
		}
		suppressed_hdrs.cnt_type = 1;
	}

	if (zend_hash_find(&ht_headers, "CONTENT-TRANSFER-ENCODING", sizeof("CONTENT-TRANSFER-ENCODING") - 1, (void **)&s) == SUCCESS) {
		enum mbfl_no_encoding _body_enc;
		SEPARATE_SMART_STR(s);
		smart_str_0(s);

		_body_enc = mbfl_name2no_encoding(s->c);
		switch (_body_enc) {
			case mbfl_no_encoding_base64:
			case mbfl_no_encoding_7bit:
			case mbfl_no_encoding_8bit:
				body_enc = _body_enc;
				break;

			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unsupported transfer encoding \"%s\" - will be regarded as 8bit", s->c); 
				body_enc =	mbfl_no_encoding_8bit;
				break;
		}
		suppressed_hdrs.cnt_trans_enc = 1;
	}

	/* To: */
	if (to == NULL || to_len <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing To: field");
		err = 1;
	}

	/* Subject: */
	if (subject != NULL && subject_len >= 0) {
		orig_str.no_language = MBSTRG(current_language);
		orig_str.val = (unsigned char *)subject;
		orig_str.len = subject_len;
		orig_str.no_encoding = MBSTRG(current_internal_encoding);
		if (orig_str.no_encoding == mbfl_no_encoding_invalid
		    || orig_str.no_encoding == mbfl_no_encoding_pass) {
			orig_str.no_encoding = mbfl_identify_encoding_no(&orig_str, MBSTRG(current_detect_order_list), MBSTRG(current_detect_order_list_size) TSRMLS_CC);
		}
		pstr = mbfl_mime_header_encode(&orig_str, &conv_str, tran_cs, head_enc, "\n", sizeof("Subject: [PHP-jp nnnnnnnn]") TSRMLS_CC);
		if (pstr != NULL) {
			subject_buf = subject = (char *)pstr->val;
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing Subject: field");
		err = 1;
	}

	/* message body */
	if (message != NULL) {
		orig_str.no_language = MBSTRG(current_language);
		orig_str.val = message;
		orig_str.len = message_len;
		orig_str.no_encoding = MBSTRG(current_internal_encoding);

		if (orig_str.no_encoding == mbfl_no_encoding_invalid
		    || orig_str.no_encoding == mbfl_no_encoding_pass) {
			orig_str.no_encoding = mbfl_identify_encoding_no(&orig_str, MBSTRG(current_detect_order_list), MBSTRG(current_detect_order_list_size) TSRMLS_CC);
		}

		pstr = NULL;
		{
			mbfl_string tmpstr;

			if (mbfl_convert_encoding(&orig_str, &tmpstr, tran_cs TSRMLS_CC) != NULL) {
				tmpstr.no_encoding=mbfl_no_encoding_8bit;
				pstr = mbfl_convert_encoding(&tmpstr, &conv_str, body_enc TSRMLS_CC);
				efree(tmpstr.val);
			}
		}
		if (pstr != NULL) {
			message_buf = message = (char *)pstr->val;
		}
	} else {
		/* this is not really an error, so it is allowed. */
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty message body");
		message = NULL;
	}

	/* other headers */
#define PHP_MBSTR_MAIL_MIME_HEADER1 "Mime-Version: 1.0"
#define PHP_MBSTR_MAIL_MIME_HEADER2 "Content-Type: text/plain"
#define PHP_MBSTR_MAIL_MIME_HEADER3 "; charset="
#define PHP_MBSTR_MAIL_MIME_HEADER4 "Content-Transfer-Encoding: "
	if (headers != NULL) {
		p = headers;
		n = headers_len;
		mbfl_memory_device_strncat(&device, p, n TSRMLS_CC);
		if (n > 0 && p[n - 1] != '\n') {
			mbfl_memory_device_strncat(&device, "\n", 1 TSRMLS_CC);
		}
	}

	mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER1, sizeof(PHP_MBSTR_MAIL_MIME_HEADER1) - 1 TSRMLS_CC);
	mbfl_memory_device_strncat(&device, "\n", 1 TSRMLS_CC);

	if (!suppressed_hdrs.cnt_type) {
		mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER2, sizeof(PHP_MBSTR_MAIL_MIME_HEADER2) - 1 TSRMLS_CC);

		p = (char *)mbfl_no2preferred_mime_name(tran_cs);
		if (p != NULL) {
			mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER3, sizeof(PHP_MBSTR_MAIL_MIME_HEADER3) - 1 TSRMLS_CC);
			mbfl_memory_device_strcat(&device, p TSRMLS_CC);
		}
		mbfl_memory_device_strncat(&device, "\n", 1 TSRMLS_CC);
	}
	if (!suppressed_hdrs.cnt_trans_enc) {
		mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER4, sizeof(PHP_MBSTR_MAIL_MIME_HEADER4) - 1 TSRMLS_CC);
		p = (char *)mbfl_no2preferred_mime_name(body_enc);
		if (p == NULL) {
			p = "7bit";
		}
		mbfl_memory_device_strcat(&device, p TSRMLS_CC);
		mbfl_memory_device_strncat(&device, "\n", 1 TSRMLS_CC);
	}

	mbfl_memory_device_unput(&device TSRMLS_CC);
	mbfl_memory_device_output('\0', &device TSRMLS_CC);
	headers = (char *)device.buffer;

	if (!err && php_mail(to, subject, message, headers, extra_cmd TSRMLS_CC)) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}

	if (subject_buf) {
		efree((void *)subject_buf);
	}
	if (message_buf) {
		efree((void *)message_buf);
	}
	mbfl_memory_device_clear(&device TSRMLS_CC);
	zend_hash_destroy(&ht_headers);
}

#undef APPEND_ONE_CHAR
#undef SEPARATE_SMART_STR
#undef PHP_MBSTR_MAIL_MIME_HEADER1
#undef PHP_MBSTR_MAIL_MIME_HEADER2
#undef PHP_MBSTR_MAIL_MIME_HEADER3
#undef PHP_MBSTR_MAIL_MIME_HEADER4

#else	/* HAVE_SENDMAIL */

PHP_FUNCTION(mb_send_mail)
{
	RETURN_FALSE;
}

#endif	/* HAVE_SENDMAIL */

/* }}} */

/* {{{ proto string mb_get_info([string type])
   Returns the current settings of mbstring */
PHP_FUNCTION(mb_get_info)
{
	char *typ = NULL;
	int typ_len;
	char *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &typ, &typ_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (!strcasecmp("all", typ)) {
		array_init(return_value);
		if ((name = (char *)mbfl_no_encoding2name(MBSTRG(current_internal_encoding))) != NULL) {
			add_assoc_string(return_value, "internal_encoding", name, 1);
		}
		if ((name = (char *)mbfl_no_encoding2name(MBSTRG(http_input_identify))) != NULL) {
			add_assoc_string(return_value, "http_input", name, 1);
		}
		if ((name = (char *)mbfl_no_encoding2name(MBSTRG(current_http_output_encoding))) != NULL) {
			add_assoc_string(return_value, "http_output", name, 1);
		}
		if ((name = (char *)mbfl_no_encoding2name(MBSTRG(func_overload))) != NULL) {
			add_assoc_string(return_value, "func_overload", name, 1);
		}
	} else if (!strcasecmp("internal_encoding", typ)) {
		if ((name = (char *)mbfl_no_encoding2name(MBSTRG(current_internal_encoding))) != NULL) {
			RETVAL_STRING(name, 1);
		}		
	} else if (!strcasecmp("http_input", typ)) {
		if ((name = (char *)mbfl_no_encoding2name(MBSTRG(http_input_identify))) != NULL) {
			RETVAL_STRING(name, 1);
		}		
	} else if (!strcasecmp("http_output", typ)) {
		if ((name = (char *)mbfl_no_encoding2name(MBSTRG(current_http_output_encoding))) != NULL) {
			RETVAL_STRING(name, 1);
		}		
	} else if (!strcasecmp("func_overload", typ)) {
		if ((name = (char *)mbfl_no_encoding2name(MBSTRG(func_overload))) != NULL) {
			RETVAL_STRING(name, 1);
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ MBSTRING_API int php_mb_encoding_translation() */
MBSTRING_API int php_mb_encoding_translation(TSRMLS_D) 
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
MBSTRING_API size_t php_mb_mbchar_bytes(const char *s TSRMLS_DC)
{
	return php_mb_mbchar_bytes_ex(s,
		mbfl_no2encoding(MBSTRG(internal_encoding)));
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
MBSTRING_API char *php_mb_safe_strrchr(const char *s, unsigned int c, size_t nbytes TSRMLS_DC)
{
	return php_mb_safe_strrchr_ex(s, c, nbytes,
		mbfl_no2encoding(MBSTRG(internal_encoding)));
}
/* }}} */

/* {{{ MBSTRING_API char *php_mb_strrchr() */
MBSTRING_API char *php_mb_strrchr(const char *s, char c TSRMLS_DC)
{
	return php_mb_safe_strrchr(s, c, -1 TSRMLS_CC);
}
/* }}} */

#ifdef ZEND_MULTIBYTE
/* {{{ MBSTRING_API int php_mb_set_zend_encoding() */
MBSTRING_API int php_mb_set_zend_encoding(TSRMLS_D)
{
	/* 'd better use mbfl_memory_device? */
	char *name, *list = NULL;
	int n, *entry, list_size = 0;
	zend_encoding_detector encoding_detector;
	zend_encoding_converter encoding_converter;
	zend_multibyte_oddlen multibyte_oddlen;

	/* notify script encoding to Zend Engine */
	entry = MBSTRG(script_encoding_list);
	n = MBSTRG(script_encoding_list_size);
	while (n > 0) {
		name = (char *)mbfl_no_encoding2name(*entry);
		if (name) {
			list_size += strlen(name) + 1;
			if (!list) {
				list = (char*)emalloc(list_size);
				*list = (char)NULL;
			} else {
				list = (char*)erealloc(list, list_size);
				strcat(list, ",");
			}
			strcat(list, name);
		}
		entry++;
		n--;
	}
	zend_multibyte_set_script_encoding(list, (list ? strlen(list) : 0) TSRMLS_CC);
	if (list) {
		efree(list);
	}
	encoding_detector = php_mb_encoding_detector;
	encoding_converter = NULL;
	multibyte_oddlen = php_mb_oddlen;

	if (MBSTRG(encoding_translation)) {
		/* notify internal encoding to Zend Engine */
		name = (char*)mbfl_no_encoding2name(MBSTRG(current_internal_encoding));
		zend_multibyte_set_internal_encoding(name, strlen(name) TSRMLS_CC);

		encoding_converter = php_mb_encoding_converter;
	}

	zend_multibyte_set_functions(encoding_detector, encoding_converter,
			multibyte_oddlen TSRMLS_CC);

	return 0;
}
/* }}} */

/* {{{ char *php_mb_encoding_detector()
 * Interface for Zend Engine
 */
char* php_mb_encoding_detector(const char *arg_string, int arg_length, char *arg_list TSRMLS_DC)
{
	mbfl_string string;
	const char *ret;
	enum mbfl_no_encoding *elist;
	int size, *list;

	/* make encoding list */
	list = NULL;
	size = 0;
	php_mb_parse_encoding_list(arg_list, strlen(arg_list), &list, &size, 0);
	if (size <= 0) {
		return NULL;
	}
	if (size > 0 && list != NULL) {
		elist = list;
	} else {
		elist = MBSTRG(current_detect_order_list);
		size = MBSTRG(current_detect_order_list_size);
	}

	mbfl_string_init(&string);
	string.no_language = MBSTRG(current_language);
	string.val = (char*)arg_string;
	string.len = arg_length;
	ret = mbfl_identify_encoding_name(&string, elist, size TSRMLS_CC);
	if (list != NULL) {
		efree((void *)list);
	}
	if (ret != NULL) {
		return estrdup(ret);
	} else {
		return NULL;
	}
}
/* }}} */

/*	{{{ int php_mb_encoding_converter() */
int php_mb_encoding_converter(char **to, int *to_length, const char *from,
		int from_length, const char *encoding_to, const char *encoding_from 
		TSRMLS_DC)
{
	mbfl_string string, result, *ret;
	enum mbfl_no_encoding from_encoding, to_encoding;
	mbfl_buffer_converter *convd;

	/* new encoding */
	to_encoding = mbfl_name2no_encoding(encoding_to);
	if (to_encoding == mbfl_no_encoding_invalid) {
		return -1;
	}	
	/* old encoding */
	from_encoding = mbfl_name2no_encoding(encoding_from);
	if (from_encoding == mbfl_no_encoding_invalid) {
		return -1;
	}
	/* initialize string */
	mbfl_string_init(&string);
	mbfl_string_init(&result);
	string.no_encoding = from_encoding;
	string.no_language = MBSTRG(current_language);
	string.val = (char*)from;
	string.len = from_length;

	/* initialize converter */
	convd = mbfl_buffer_converter_new(from_encoding, to_encoding, string.len TSRMLS_CC);
	if (convd == NULL) {
		return -1;
	}
	mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode) TSRMLS_CC);
	mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar) TSRMLS_CC);

	/* do it */
	ret = mbfl_buffer_converter_feed_result(convd, &string, &result TSRMLS_CC);
	if (ret != NULL) {
		*to = ret->val;
		*to_length = ret->len;
	}
	mbfl_buffer_converter_delete(convd TSRMLS_CC);

	return ret ? 0 : -1;
}
/* }}} */

/* {{{ int php_mb_oddlen()
 *	returns number of odd (e.g. appears only first byte of multibyte
 *	character) chars
 */
int php_mb_oddlen(const char *string, int length, const char *encoding TSRMLS_DC)
{
	mbfl_string mb_string;

	mbfl_string_init(&mb_string);
	mb_string.no_language = MBSTRG(current_language);
	mb_string.no_encoding = mbfl_name2no_encoding(encoding);
	mb_string.val = (char*)string;
	mb_string.len = length;

	if (mb_string.no_encoding == mbfl_no_encoding_invalid) {
		return 0;
	}
	return mbfl_oddlen(&mb_string);
}
/* }}} */
#endif /* ZEND_MULTIBYTE */

#endif	/* HAVE_MBSTRING */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
