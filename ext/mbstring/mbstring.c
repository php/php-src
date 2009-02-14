/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
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
#include "ext/standard/php_smart_str.h"
#include "ext/standard/url.h"
#include "main/php_output.h"
#include "ext/standard/info.h"

#include "libmbfl/mbfl/mbfl_allocators.h"

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
/* }}} */

/* {{{ prototypes */
ZEND_DECLARE_MODULE_GLOBALS(mbstring)
static PHP_GINIT_FUNCTION(mbstring);
static PHP_GSHUTDOWN_FUNCTION(mbstring);
/* }}} */

/* {{{ php_mb_default_identify_list */
typedef struct _php_mb_nls_ident_list {
	enum mbfl_no_language lang;
	const enum mbfl_no_encoding* list;
	int list_size;
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
	mbfl_no_encoding_8859_9
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
	{ mbfl_no_language_neutral, php_mb_default_identify_list_neut, sizeof(php_mb_default_identify_list_neut) / sizeof(php_mb_default_identify_list_neut[0]) }
};

/* }}} */

static
	ZEND_BEGIN_ARG_INFO(third_and_rest_force_ref, 1)
		ZEND_ARG_PASS_INFO(0)
		ZEND_ARG_PASS_INFO(0)
	ZEND_END_ARG_INFO()

/* {{{ mb_overload_def mb_ovld[] */
static const struct mb_overload_def mb_ovld[] = {
	{MB_OVERLOAD_MAIL, "mail", "mb_send_mail", "mb_orig_mail"},
	{MB_OVERLOAD_STRING, "strlen", "mb_strlen", "mb_orig_strlen"},
	{MB_OVERLOAD_STRING, "strpos", "mb_strpos", "mb_orig_strpos"},
	{MB_OVERLOAD_STRING, "strrpos", "mb_strrpos", "mb_orig_strrpos"},
	{MB_OVERLOAD_STRING, "stripos", "mb_stripos", "mb_orig_stripos"},
	{MB_OVERLOAD_STRING, "strripos", "mb_strripos", "mb_orig_stripos"},
	{MB_OVERLOAD_STRING, "strstr", "mb_strstr", "mb_orig_strstr"},
	{MB_OVERLOAD_STRING, "strrchr", "mb_strrchr", "mb_orig_strrchr"},
	{MB_OVERLOAD_STRING, "stristr", "mb_stristr", "mb_orig_stristr"},
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

/* {{{ zend_function_entry mbstring_functions[] */
zend_function_entry mbstring_functions[] = {
	PHP_FE(mb_convert_case,			NULL)
	PHP_FE(mb_strtoupper,			NULL)
	PHP_FE(mb_strtolower,			NULL)
	PHP_FE(mb_language,				NULL)
	PHP_FE(mb_internal_encoding,	NULL)
	PHP_FE(mb_http_input,			NULL)
	PHP_FE(mb_http_output,			NULL)
	PHP_FE(mb_detect_order,			NULL)
	PHP_FE(mb_substitute_character,	NULL)
	PHP_FE(mb_parse_str,			second_arg_force_ref)
	PHP_FE(mb_output_handler,		NULL)
	PHP_FE(mb_preferred_mime_name,	NULL)
	PHP_FE(mb_strlen,				NULL)
	PHP_FE(mb_strpos,				NULL)
	PHP_FE(mb_strrpos,				NULL)
	PHP_FE(mb_stripos,				NULL)
	PHP_FE(mb_strripos,				NULL)
	PHP_FE(mb_strstr,				NULL)
	PHP_FE(mb_strrchr,				NULL)
	PHP_FE(mb_stristr,				NULL)
	PHP_FE(mb_strrichr,				NULL)
	PHP_FE(mb_substr_count,			NULL)
	PHP_FE(mb_substr,				NULL)
	PHP_FE(mb_strcut,				NULL)
	PHP_FE(mb_strwidth,				NULL)
	PHP_FE(mb_strimwidth,			NULL)
	PHP_FE(mb_convert_encoding,		NULL)
	PHP_FE(mb_detect_encoding,		NULL)
	PHP_FE(mb_list_encodings,		NULL)
	PHP_FE(mb_convert_kana,			NULL)
	PHP_FE(mb_encode_mimeheader,	NULL)
	PHP_FE(mb_decode_mimeheader,	NULL)
	PHP_FE(mb_convert_variables,	third_and_rest_force_ref)
	PHP_FE(mb_encode_numericentity,	NULL)
	PHP_FE(mb_decode_numericentity,	NULL)
	PHP_FE(mb_send_mail,			NULL)
	PHP_FE(mb_get_info,				NULL)
	PHP_FE(mb_check_encoding,		NULL)
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
ZEND_GET_MODULE(mbstring)
# ifdef PHP_WIN32
# include "zend_arg_defs.c"
# endif
#endif

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
php_mb_parse_encoding_list(const char *value, int value_length, enum mbfl_no_encoding **return_list, int *return_size, int persistent TSRMLS_DC)
{
	int n, l, size, bauto, ret = 1;
	char *p, *p1, *p2, *endp, *tmpstr;
	enum mbfl_no_encoding no_encoding;
	enum mbfl_no_encoding *src, *entry, *list;

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
		enum mbfl_no_encoding *identify_list;
		int identify_list_size;

		identify_list = MBSTRG(default_detect_order_list);
		identify_list_size = MBSTRG(default_detect_order_list_size);

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
		size = n + identify_list_size;
		/* make list */
		list = (enum mbfl_no_encoding *)pecalloc(size, sizeof(int), persistent);
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
				if (strcasecmp(p1, "auto") == 0) {
					if (!bauto) {
						bauto = 1;
						l = identify_list_size;
						src = identify_list;
						while (l > 0) {
							*entry++ = *src++;
							l--;
							n++;
						}
					}
				} else {
					no_encoding = mbfl_name2no_encoding(p1);
					if (no_encoding != mbfl_no_encoding_invalid) {
						*entry++ = no_encoding;
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

/* {{{ MBSTRING_API php_mb_check_encoding_list */
MBSTRING_API int php_mb_check_encoding_list(const char *encoding_list TSRMLS_DC) {
	return php_mb_parse_encoding_list(encoding_list, strlen(encoding_list), NULL, NULL, 0 TSRMLS_CC);	
}
/* }}} */

/* {{{ static int php_mb_parse_encoding_array()
 *  Return 0 if input contains any illegal encoding, otherwise 1.
 *  Even if any illegal encoding is detected the result may contain a list 
 *  of parsed encodings.
 */
static int
php_mb_parse_encoding_array(zval *array, enum mbfl_no_encoding **return_list, int *return_size, int persistent TSRMLS_DC)
{
	zval **hash_entry;
	HashTable *target_hash;
	int i, n, l, size, bauto,ret = 1;
	enum mbfl_no_encoding no_encoding;
	enum mbfl_no_encoding *src, *list, *entry;

	list = NULL;
	if (Z_TYPE_P(array) == IS_ARRAY) {
		enum mbfl_no_encoding *identify_list;
		int identify_list_size;

		identify_list = MBSTRG(default_detect_order_list);
		identify_list_size = MBSTRG(default_detect_order_list_size);

		target_hash = Z_ARRVAL_P(array);
		zend_hash_internal_pointer_reset(target_hash);
		i = zend_hash_num_elements(target_hash);
		size = i + identify_list_size;
		list = (enum mbfl_no_encoding *)pecalloc(size, sizeof(int), persistent);
		if (list != NULL) {
			entry = list;
			bauto = 0;
			n = 0;
			while (i > 0) {
				if (zend_hash_get_current_data(target_hash, (void **) &hash_entry) == FAILURE) {
					break;
				}
				convert_to_string_ex(hash_entry);
				if (strcasecmp(Z_STRVAL_PP(hash_entry), "auto") == 0) {
					if (!bauto) {
						bauto = 1;
						l = identify_list_size; 
						src = identify_list;
						while (l > 0) {
							*entry++ = *src++;
							l--;
							n++;
						}
					}
				} else {
					no_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(hash_entry));
					if (no_encoding != mbfl_no_encoding_invalid) {
						*entry++ = no_encoding;
						n++;
					} else {
						ret = 0;
					}
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

/* {{{ php_mb_nls_get_default_detect_order_list */
static int php_mb_nls_get_default_detect_order_list(enum mbfl_no_language lang, enum mbfl_no_encoding **plist, int* plist_size)
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

/* {{{ php.ini directive handler */
static PHP_INI_MH(OnUpdate_mbstring_language)
{
	enum mbfl_no_language no_language;

	no_language = mbfl_name2no_language(new_value);
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
	enum mbfl_no_encoding *list;
	int size;

	if (php_mb_parse_encoding_list(new_value, new_value_length, &list, &size, 1 TSRMLS_CC)) {
		if (MBSTRG(detect_order_list)) {
			free(MBSTRG(detect_order_list));
		}
		MBSTRG(detect_order_list) = list;
		MBSTRG(detect_order_list_size) = size;
	} else {
		if (MBSTRG(detect_order_list)) {
			free(MBSTRG(detect_order_list));
			MBSTRG(detect_order_list) = NULL;
		}
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_http_input) */
static PHP_INI_MH(OnUpdate_mbstring_http_input)
{
	enum mbfl_no_encoding *list;
	int size;

	if (php_mb_parse_encoding_list(new_value, new_value_length, &list, &size, 1 TSRMLS_CC)) {
		if (MBSTRG(http_input_list)) {
			free(MBSTRG(http_input_list));
		}
		MBSTRG(http_input_list) = list;
		MBSTRG(http_input_list_size) = size;
	} else {
		if (MBSTRG(http_input_list)) {
			free(MBSTRG(http_input_list));
			MBSTRG(http_input_list) = NULL;
		}
		MBSTRG(http_input_list_size) = 0;
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
		MBSTRG(http_output_encoding) = mbfl_no_encoding_pass;
		MBSTRG(current_http_output_encoding) = mbfl_no_encoding_pass;
		if (new_value != NULL && new_value_length > 0) {
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

/* {{{ static _php_mb_ini_mbstring_internal_encoding_set */
static int _php_mb_ini_mbstring_internal_encoding_set(const char *new_value, uint new_value_length TSRMLS_DC)
{
	enum mbfl_no_encoding no_encoding;
	const char *enc_name = NULL;
	uint enc_name_len = 0;

	no_encoding = new_value ? mbfl_name2no_encoding(new_value):
				mbfl_no_encoding_invalid;
	if (no_encoding != mbfl_no_encoding_invalid) {
		enc_name = new_value;
		enc_name_len = new_value_length;
	} else {
		switch (MBSTRG(language)) {
			case mbfl_no_language_uni:
				enc_name = "UTF-8";
				enc_name_len = sizeof("UTF-8") - 1;
				break;
			case mbfl_no_language_japanese:
				enc_name = "EUC-JP";
				enc_name_len = sizeof("EUC-JP") - 1;
				break;
			case mbfl_no_language_korean:
				enc_name = "EUC-KR";
				enc_name_len = sizeof("EUC-KR") - 1;
				break;
			case mbfl_no_language_simplified_chinese:
				enc_name = "EUC-CN";
				enc_name_len = sizeof("EUC-CN") - 1;
				break;
			case mbfl_no_language_traditional_chinese:
				enc_name = "EUC-TW";
				enc_name_len = sizeof("EUC-TW") - 1;
				break;
			case mbfl_no_language_russian:
				enc_name = "KOI8-R";
				enc_name_len = sizeof("KOI8-R") - 1;
				break;
			case mbfl_no_language_german:
				enc_name = "ISO-8859-15";
				enc_name_len = sizeof("ISO-8859-15") - 1;
				break;
			case mbfl_no_language_armenian:
				enc_name = "ArmSCII-8";
				enc_name_len = sizeof("ArmSCII-8") - 1;
				break;
			case mbfl_no_language_turkish:
				enc_name = "ISO-8859-9";
				enc_name_len = sizeof("ISO-8859-9") - 1;
				break;
			default:
				enc_name = "ISO-8859-1";
				enc_name_len = sizeof("ISO-8859-1") - 1;
				break;
		}
		no_encoding = mbfl_name2no_encoding(enc_name);
	}
	MBSTRG(internal_encoding) = no_encoding;
	MBSTRG(current_internal_encoding) = no_encoding;
#if HAVE_MBREGEX
	{
		OnigEncoding mbctype;
		mbctype = php_mb_regex_name2mbctype(enc_name);
		if (mbctype == ONIG_ENCODING_UNDEF) {
			mbctype = ONIG_ENCODING_EUC_JP;
		}
		MBSTRG(current_mbctype) = MBSTRG(default_mbctype) = mbctype;
	}
#endif
#ifdef ZEND_MULTIBYTE
	zend_multibyte_set_internal_encoding((char *)enc_name, enc_name_len TSRMLS_CC);
#endif /* ZEND_MULTIBYTE */

	return SUCCESS;
}
/* }}} */

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_internal_encoding) */
static PHP_INI_MH(OnUpdate_mbstring_internal_encoding)
{
	if (stage == PHP_INI_STAGE_STARTUP || stage == PHP_INI_STAGE_SHUTDOWN
			|| stage == PHP_INI_STAGE_RUNTIME) {
		return _php_mb_ini_mbstring_internal_encoding_set(new_value, new_value_length TSRMLS_CC);
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

#ifdef ZEND_MULTIBYTE
/* {{{ static PHP_INI_MH(OnUpdate_mbstring_script_encoding) */
static PHP_INI_MH(OnUpdate_mbstring_script_encoding)
{
	int *list, size;

	if (php_mb_parse_encoding_list(new_value, new_value_length, &list, &size, 1 TSRMLS_CC)) {
		if (MBSTRG(script_encoding_list) != NULL) {
			free(MBSTRG(script_encoding_list));
		}
		MBSTRG(script_encoding_list) = list;
		MBSTRG(script_encoding_list_size) = size;
	} else {
		if (MBSTRG(script_encoding_list) != NULL) {
			free(MBSTRG(script_encoding_list));
		}
		MBSTRG(script_encoding_list) = NULL;
		MBSTRG(script_encoding_list_size) = 0;
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */
#endif /* ZEND_MULTIBYTE */

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_substitute_character) */
static PHP_INI_MH(OnUpdate_mbstring_substitute_character)
{
	int c;
	char *endptr = NULL;

	if (new_value != NULL) {
		if (strcasecmp("none", new_value) == 0) {
			MBSTRG(filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
			MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
		} else if (strcasecmp("long", new_value) == 0) {
			MBSTRG(filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG;
			MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG;
		} else if (strcasecmp("entity", new_value) == 0) {
			MBSTRG(filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY;
			MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY;
		} else {
			MBSTRG(filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
			MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
			if (new_value_length >0) {
				c = strtol(new_value, &endptr, 0);
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

	OnUpdateBool(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);

	if (MBSTRG(encoding_translation)) {
		sapi_unregister_post_entry(php_post_entries TSRMLS_CC);
		sapi_register_post_entries(mbstr_post_entries TSRMLS_CC);
		sapi_register_treat_data(mbstr_treat_data);
	} else {
		sapi_unregister_post_entry(mbstr_post_entries TSRMLS_CC);
		sapi_register_post_entries(php_post_entries TSRMLS_CC);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ php.ini directive registration */
PHP_INI_BEGIN()
	 PHP_INI_ENTRY("mbstring.language", "neutral", PHP_INI_ALL, OnUpdate_mbstring_language)
	 PHP_INI_ENTRY("mbstring.detect_order", NULL, PHP_INI_ALL, OnUpdate_mbstring_detect_order)
	 PHP_INI_ENTRY("mbstring.http_input", "pass", PHP_INI_ALL, OnUpdate_mbstring_http_input)
	 PHP_INI_ENTRY("mbstring.http_output", "pass", PHP_INI_ALL, OnUpdate_mbstring_http_output)
	 PHP_INI_ENTRY("mbstring.internal_encoding", NULL, PHP_INI_ALL, OnUpdate_mbstring_internal_encoding)
#ifdef ZEND_MULTIBYTE
	 PHP_INI_ENTRY("mbstring.script_encoding", NULL, PHP_INI_ALL, OnUpdate_mbstring_script_encoding)
#endif /* ZEND_MULTIBYTE */
	 PHP_INI_ENTRY("mbstring.substitute_character", NULL, PHP_INI_ALL, OnUpdate_mbstring_substitute_character)
	 STD_PHP_INI_ENTRY("mbstring.func_overload", "0",
	 PHP_INI_SYSTEM, OnUpdateLong, func_overload, zend_mbstring_globals, mbstring_globals)
	 	 								  
	 STD_PHP_INI_BOOLEAN("mbstring.encoding_translation", "0",
	 PHP_INI_SYSTEM | PHP_INI_PERDIR, OnUpdate_mbstring_encoding_translation, 
	 encoding_translation, zend_mbstring_globals, mbstring_globals)					 

	 STD_PHP_INI_BOOLEAN("mbstring.strict_detection", "0",
	 PHP_INI_ALL, OnUpdateLong, strict_detection, zend_mbstring_globals, mbstring_globals)
PHP_INI_END()
/* }}} */

/* {{{ module global initialize handler */
static PHP_GINIT_FUNCTION(mbstring)
{
	mbstring_globals->language = mbfl_no_language_uni;
	mbstring_globals->internal_encoding = mbfl_no_encoding_invalid;
	mbstring_globals->current_internal_encoding = mbstring_globals->internal_encoding;
#ifdef ZEND_MULTIBYTE
	mbstring_globals->script_encoding_list = NULL;
	mbstring_globals->script_encoding_list_size = 0;
#endif /* ZEND_MULTIBYTE */
	mbstring_globals->http_output_encoding = mbfl_no_encoding_pass;
	mbstring_globals->current_http_output_encoding = mbfl_no_encoding_pass;
	mbstring_globals->http_input_identify = mbfl_no_encoding_invalid;
	mbstring_globals->http_input_identify_get = mbfl_no_encoding_invalid;
	mbstring_globals->http_input_identify_post = mbfl_no_encoding_invalid;
	mbstring_globals->http_input_identify_cookie = mbfl_no_encoding_invalid;
	mbstring_globals->http_input_identify_string = mbfl_no_encoding_invalid;
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
#if HAVE_MBREGEX
	_php_mb_regex_globals_ctor(mbstring_globals TSRMLS_CC);
#endif
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION */
static PHP_GSHUTDOWN_FUNCTION(mbstring)
{
	if (mbstring_globals->http_input_list) {
		free(mbstring_globals->http_input_list);
	}
#ifdef ZEND_MULTIBYTE
	if (mbstring_globals->script_encoding_list) {
		free(mbstring_globals->script_encoding_list);
	}
#endif /* ZEND_MULTIBYTE */
	if (mbstring_globals->detect_order_list) {
		free(mbstring_globals->detect_order_list);
	}

#if HAVE_MBREGEX
	_php_mb_regex_globals_dtor(mbstring_globals TSRMLS_CC);
#endif
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(mbstring) */
PHP_MINIT_FUNCTION(mbstring)
{
	__mbfl_allocators = &_php_mb_allocators;

	REGISTER_INI_ENTRIES();

	if (MBSTRG(encoding_translation)) {
		sapi_register_post_entries(mbstr_post_entries TSRMLS_CC);
		sapi_register_treat_data(mbstr_treat_data);
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
	
#if HAVE_MBREGEX
	PHP_MSHUTDOWN(mb_regex) (INIT_FUNC_ARGS_PASSTHRU);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION(mbstring) */
PHP_RINIT_FUNCTION(mbstring)
{
	int n;
	enum mbfl_no_encoding *list=NULL, *entry;
	zend_function *func, *orig;
	const struct mb_overload_def *p;

	{
		char *value = zend_ini_string("mbstring.internal_encoding", sizeof("mbstring.internal_encoding"), 0);
		_php_mb_ini_mbstring_internal_encoding_set(value, value ? strlen(value): 0 TSRMLS_CC);
	}

	MBSTRG(current_internal_encoding) = MBSTRG(internal_encoding);
	MBSTRG(current_http_output_encoding) = MBSTRG(http_output_encoding);
	MBSTRG(current_filter_illegal_mode) = MBSTRG(filter_illegal_mode);
	MBSTRG(current_filter_illegal_substchar) = MBSTRG(filter_illegal_substchar);

	MBSTRG(illegalchars) = 0;

	n = 0;
	if (MBSTRG(detect_order_list)) {
		list = MBSTRG(detect_order_list);
		n = MBSTRG(detect_order_list_size);
	}
	if (n <= 0) {
		list = MBSTRG(default_detect_order_list);
		n = MBSTRG(default_detect_order_list_size);
	}
	entry = (enum mbfl_no_encoding *)safe_emalloc(n, sizeof(int), 0);
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
#ifdef ZEND_MULTIBYTE
	php_mb_set_zend_encoding(TSRMLS_C);
#endif /* ZEND_MULTIBYTE */

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
	MBSTRG(http_input_identify) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_post) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_get) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_cookie) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_string) = mbfl_no_encoding_invalid;

 	/*  clear overloaded function. */
	if (MBSTRG(func_overload)){
		p = &(mb_ovld[0]);
		while (p->type > 0) {
			if ((MBSTRG(func_overload) & p->type) == p->type && 
				zend_hash_find(EG(function_table), p->save_func,
							   strlen(p->save_func)+1, (void **)&orig) == SUCCESS) {
				
				zend_hash_update(EG(function_table), p->orig_func, strlen(p->orig_func)+1, orig, sizeof(zend_function), NULL);
				zend_hash_del(EG(function_table), p->save_func, strlen(p->save_func)+1);
			}
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
	php_info_print_table_row(2, "Multibyte string engine", "libmbfl");
	if (MBSTRG(encoding_translation)) {
		php_info_print_table_row(2, "HTTP input encoding translation", "enabled");	
	}
#if defined(HAVE_MBREGEX)
	{
		char buf[32];
		php_info_print_table_row(2, "Multibyte (japanese) regex support", "enabled");
		snprintf(buf, sizeof(buf), "%d.%d.%d",
			ONIGURUMA_VERSION_MAJOR,ONIGURUMA_VERSION_MINOR,ONIGURUMA_VERSION_TEENY);
		php_info_print_table_row(2, "Multibyte regex (oniguruma) version", buf);
#ifdef USE_COMBINATION_EXPLOSION_CHECK
		php_info_print_table_row(2, "Multibyte regex (oniguruma) backtrack check", "On");
#else	/* USE_COMBINATION_EXPLOSION_CHECK */
		php_info_print_table_row(2, "Multibyte regex (oniguruma) backtrack check", "Off");
#endif	/* USE_COMBINATION_EXPLOSION_CHECK */
	}
#endif
	php_info_print_table_end();

	php_info_print_table_start();
	php_info_print_table_header(1, "mbstring extension makes use of \"streamable kanji code filter and converter\", which is distributed under the GNU Lesser General Public License version 2.1.");
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
		return;
	}
	if (name == NULL) {
		RETVAL_STRING((char *)mbfl_no_language2name(MBSTRG(language)), 1);
	} else {
		if (FAILURE == zend_alter_ini_entry(
				"mbstring.language", sizeof("mbstring.language"),
				name, name_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown language \"%s\"", name);
			RETVAL_FALSE;
		} else {
			RETVAL_TRUE;
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
			/* TODO: make independent from mbstring.encoding_translation? */
			if (MBSTRG(encoding_translation)) {
				zend_multibyte_set_internal_encoding(name, name_len TSRMLS_CC);
			}
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
	int retname, n;
	char *name, *list, *temp;
	enum mbfl_no_encoding *entry;
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
	int n, size;
	enum mbfl_no_encoding *list, *entry;
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
			if (!php_mb_parse_encoding_array(*arg1, &list, &size, 0 TSRMLS_CC)) {
				if (list) {
					efree(list);
				}
				RETURN_FALSE;
			}
			break;
		default:
			convert_to_string_ex(arg1);
			if (!php_mb_parse_encoding_list(Z_STRVAL_PP(arg1), Z_STRLEN_PP(arg1), &list, &size, 0 TSRMLS_CC)) {
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
		} else if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY) {
			RETVAL_STRING("entity", 1);
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
			} else if (strcasecmp("entity", Z_STRVAL_PP(arg1)) == 0) {
				MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY;
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
	zval *track_vars_array = NULL;
	char *encstr = NULL;
	int encstr_len;
	php_mb_encoding_handler_info_t info;
	enum mbfl_no_encoding detected;

	track_vars_array = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z", &encstr, &encstr_len, &track_vars_array) == FAILURE) {
		return;
	}

	/* Clear out the array */
	if (track_vars_array != NULL) {
		zval_dtor(track_vars_array);
		array_init(track_vars_array);
	}

	encstr = estrndup(encstr, encstr_len);

	info.data_type              = PARSE_STRING;
	info.separator              = PG(arg_separator).input; 
	info.force_register_globals = (track_vars_array == NULL);
	info.report_errors          = 1;
	info.to_encoding            = MBSTRG(current_internal_encoding);
	info.to_language            = MBSTRG(language);
	info.from_encodings         = MBSTRG(http_input_list);
	info.num_from_encodings     = MBSTRG(http_input_list_size); 
	info.from_language          = MBSTRG(language);

	detected = _php_mb_encoding_handler_ex(&info, track_vars_array, encstr TSRMLS_CC);

	MBSTRG(http_input_identify) = detected;

	RETVAL_BOOL(detected != mbfl_no_encoding_invalid);

	if (encstr != NULL) efree(encstr);
}
/* }}} */

/* {{{ proto string mb_output_handler(string contents, int status)
   Returns string in output buffer converted to the http_output encoding */
PHP_FUNCTION(mb_output_handler)
{
	char *arg_string;
	int arg_string_len;
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
			MBSTRG(illegalchars) += mbfl_buffer_illegalchars(MBSTRG(outconv));
 			mbfl_buffer_converter_delete(MBSTRG(outconv));
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
 			MBSTRG(outconv) = mbfl_buffer_converter_new(MBSTRG(current_internal_encoding), encoding, 0);
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
 	mbfl_buffer_converter_illegal_mode(MBSTRG(outconv), MBSTRG(current_filter_illegal_mode));
 	mbfl_buffer_converter_illegal_substchar(MBSTRG(outconv), MBSTRG(current_filter_illegal_substchar));
 
 	/* feed the string */
 	mbfl_string_init(&string);
 	string.no_language = MBSTRG(language);
 	string.no_encoding = MBSTRG(current_internal_encoding);
 	string.val = (unsigned char *)arg_string;
 	string.len = arg_string_len;
 	mbfl_buffer_converter_feed(MBSTRG(outconv), &string);
 	if (last_feed) {
 		mbfl_buffer_converter_flush(MBSTRG(outconv));
	} 
 	/* get the converter output, and return it */
 	mbfl_buffer_converter_result(MBSTRG(outconv), &result);
 	RETVAL_STRINGL((char *)result.val, result.len, 0);		/* the string is already strdup()'ed */
 
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
	int enc_name_len;

	mbfl_string_init(&string);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", (char **)&string.val, &string.len, &enc_name, &enc_name_len) == FAILURE) {
		RETURN_FALSE;
	}

	string.no_language = MBSTRG(language);
	if (enc_name == NULL) {
		string.no_encoding = MBSTRG(current_internal_encoding);
	} else {
		string.no_encoding = mbfl_name2no_encoding(enc_name);
		if (string.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", enc_name);
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
	long offset;
	mbfl_string haystack, needle;
	char *enc_name = NULL;
	int enc_name_len;
	
	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding);
	needle.no_language = MBSTRG(language);
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

	if (offset < 0 || offset > mbfl_strlen(&haystack)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset not contained in string");
		RETURN_FALSE;
	}
	if (needle.len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty delimiter");
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Needle has not positive length");
			break;
		case 4:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding or conversion error");
			break;
		case 8:
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Argument is empty");
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown error in mb_strpos");
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
	int enc_name_len;
	zval **zoffset;
	long offset = 0, str_flg;
	char *enc_name2 = NULL;
	int enc_name_len2;

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding);
	needle.no_language = MBSTRG(language);
	needle.no_encoding = MBSTRG(current_internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|Zs", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &zoffset, &enc_name, &enc_name_len) == FAILURE) {
		RETURN_FALSE;
	}

	if(ZEND_NUM_ARGS() >= 3) {
		if (Z_TYPE_PP(zoffset) == IS_STRING) {
			enc_name2     = Z_STRVAL_PP(zoffset);
			enc_name_len2 = Z_STRLEN_PP(zoffset);
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

			if(str_flg) {
					convert_to_long_ex(zoffset);
					offset   = Z_LVAL_PP(zoffset);
			} else {
				enc_name     = enc_name2;
				enc_name_len = enc_name_len2;
			}
		} else {
			convert_to_long_ex(zoffset);
			offset = Z_LVAL_PP(zoffset);
		}
	}

	if (enc_name != NULL) {
		haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(enc_name);
		if (haystack.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", enc_name);
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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset is greater than the length of haystack string");
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
	int n;
	long offset;
	char *old_haystack, *old_needle;
	char *from_encoding = (char*)mbfl_no2preferred_mime_name(MBSTRG(current_internal_encoding));
	int old_haystack_len, old_needle_len, from_encoding_len;
	n = -1;
	offset = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ls", &old_haystack, &old_haystack_len, &old_needle, &old_needle_len, &offset, &from_encoding, &from_encoding_len ) == FAILURE) {
		RETURN_FALSE;
	}

	n = php_mb_stripos(0, old_haystack, old_haystack_len, old_needle, old_needle_len, offset, from_encoding TSRMLS_CC);

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
	int n;
	long offset;
	char *old_haystack, *old_needle;
	char *from_encoding = (char*)mbfl_no2preferred_mime_name(MBSTRG(current_internal_encoding));
	int old_haystack_len, old_needle_len, from_encoding_len;
	n = -1;
	offset = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ls", &old_haystack, &old_haystack_len, &old_needle, &old_needle_len, &offset, &from_encoding, &from_encoding_len ) == FAILURE) {
		RETURN_FALSE;
	}

	n = php_mb_stripos(1, old_haystack, old_haystack_len, old_needle, old_needle_len, offset, from_encoding TSRMLS_CC);

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
	int enc_name_len;
	zend_bool part = 0;

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding);
	needle.no_language = MBSTRG(language);
	needle.no_encoding = MBSTRG(current_internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|bs", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &part, &enc_name, &enc_name_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (enc_name != NULL) {
		haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(enc_name);
		if (haystack.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", enc_name);
			RETURN_FALSE;
		}
	}

	if (needle.len <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Empty delimiter.");
		RETURN_FALSE;
	}
	n = mbfl_strpos(&haystack, &needle, 0, 0);
	if (n >= 0) {
		mblen = mbfl_strlen(&haystack);
		if (part) {
			ret = mbfl_substr(&haystack, &result, 0, n);
			if (ret != NULL) {
				RETVAL_STRINGL((char *)ret->val, ret->len, 0);
			} else {
				RETVAL_FALSE;
			}
		} else {
			len = (mblen - n);
			ret = mbfl_substr(&haystack, &result, n, len);
			if (ret != NULL) {
				RETVAL_STRINGL((char *)ret->val, ret->len, 0);
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
	int enc_name_len;
	zend_bool part = 0;

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding);
	needle.no_language = MBSTRG(language);
	needle.no_encoding = MBSTRG(current_internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|bs", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &part, &enc_name, &enc_name_len) == FAILURE) {
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
				RETVAL_STRINGL((char *)ret->val, ret->len, 0);
			} else {
				RETVAL_FALSE;
			}
		} else {
			len = (mblen - n);
			ret = mbfl_substr(&haystack, &result, n, len);
			if (ret != NULL) {
				RETVAL_STRINGL((char *)ret->val, ret->len, 0);
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
	int n, from_encoding_len, len, mblen;
	mbfl_string haystack, needle, result, *ret = NULL;
	char *from_encoding = (char*)mbfl_no2preferred_mime_name(MBSTRG(current_internal_encoding));
	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding);
	needle.no_language = MBSTRG(language);
	needle.no_encoding = MBSTRG(current_internal_encoding);


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|bs", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &part, &from_encoding, &from_encoding_len) == FAILURE) {
		RETURN_FALSE;
	}

	if(!needle.len){
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Empty delimiter.");
		RETURN_FALSE;
	}

	haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(from_encoding);
	if (haystack.no_encoding == mbfl_no_encoding_invalid) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", from_encoding);
		RETURN_FALSE;
	}

	n = php_mb_stripos(0, haystack.val, haystack.len, needle.val, needle.len, 0, from_encoding TSRMLS_CC);

	if (n <0) {
		RETURN_FALSE;
	}

	mblen = mbfl_strlen(&haystack);

	if (part) {
		ret = mbfl_substr(&haystack, &result, 0, n);
		if (ret != NULL) {
			RETVAL_STRINGL((char *)ret->val, ret->len, 0);
		} else {
			RETVAL_FALSE;
		}
	} else {
		len = (mblen - n);
		ret = mbfl_substr(&haystack, &result, n, len);
		if (ret != NULL) {
			RETVAL_STRINGL((char *)ret->val, ret->len, 0);
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
	int n, from_encoding_len, len, mblen;
	mbfl_string haystack, needle, result, *ret = NULL;
	char *from_encoding = (char*)mbfl_no2preferred_mime_name(MBSTRG(current_internal_encoding));
	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding);
	needle.no_language = MBSTRG(language);
	needle.no_encoding = MBSTRG(current_internal_encoding);


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|bs", (char **)&haystack.val, &haystack.len, (char **)&needle.val, &needle.len, &part, &from_encoding, &from_encoding_len) == FAILURE) {
		RETURN_FALSE;
	}

	haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(from_encoding);
	if (haystack.no_encoding == mbfl_no_encoding_invalid) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", from_encoding);
		RETURN_FALSE;
	}

	n = php_mb_stripos(1, haystack.val, haystack.len, needle.val, needle.len, 0, from_encoding TSRMLS_CC);

	if (n <0) {
		RETURN_FALSE;
	}

	mblen = mbfl_strlen(&haystack);

	if (part) {
		ret = mbfl_substr(&haystack, &result, 0, n);
		if (ret != NULL) {
			RETVAL_STRINGL((char *)ret->val, ret->len, 0);
		} else {
			RETVAL_FALSE;
		}
	} else {
		len = (mblen - n);
		ret = mbfl_substr(&haystack, &result, n, len);
		if (ret != NULL) {
			RETVAL_STRINGL((char *)ret->val, ret->len, 0);
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
	int enc_name_len;

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding);
	needle.no_language = MBSTRG(language);
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Empty substring.");
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
	zval **arg1, **arg2, **arg3, **arg4;
	int argc, from, len, mblen;
	mbfl_string string, result, *ret;

	mbfl_string_init(&string);
	string.no_language = MBSTRG(language);
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
	zval **arg1, **arg2, **arg3, **arg4;
	int argc, from, len;
	mbfl_string string, result, *ret;

	mbfl_string_init(&string);
	string.no_language = MBSTRG(language);
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

	if (from > Z_STRLEN_PP(arg1)) {
		RETURN_FALSE;
	}
	if (((unsigned) from + (unsigned) len) > Z_STRLEN_PP(arg1)) {
		len = Z_STRLEN_PP(arg1) - from;
	}

	ret = mbfl_strcut(&string, &result, from, len);
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

	string.no_language = MBSTRG(language);
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
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	int from, width;
	mbfl_string string, result, marker, *ret;

	mbfl_string_init(&string);
	mbfl_string_init(&marker);
	string.no_language = MBSTRG(language);
	string.no_encoding = MBSTRG(current_internal_encoding);
	marker.no_language = MBSTRG(language);
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Start position is out of range");
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

	ret = mbfl_strimwidth(&string, &marker, &result, from, width);
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
	if (!input) {
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
	string.no_language = MBSTRG(language);
	string.val = (unsigned char *)input;
	string.len = length;

	/* pre-conversion encoding */
	if (_from_encodings) {
		list = NULL;
		size = 0;
	    php_mb_parse_encoding_list(_from_encodings, strlen(_from_encodings), &list, &size, 0 TSRMLS_CC);
		if (size == 1) {
			from_encoding = *list;
			string.no_encoding = from_encoding;
		} else if (size > 1) {
			/* auto detect */
			from_encoding = mbfl_identify_encoding_no(&string, list, size, MBSTRG(strict_detection));
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
	convd = mbfl_buffer_converter_new(from_encoding, to_encoding, string.len);
	if (convd == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create character encoding converter");
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
	zval **arg_str, **arg_new, **arg_old;
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
			if (_from_encodings != NULL && !strlen(_from_encodings)) {
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
	int str_len, from_encoding_len;
	long case_mode = 0;
	char *newstr;
	size_t ret_len;

	RETVAL_FALSE;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|s!", &str, &str_len,
				&case_mode, &from_encoding, &from_encoding_len) == FAILURE)
		RETURN_FALSE;

	newstr = php_unicode_convert_case(case_mode, str, (size_t) str_len, &ret_len, from_encoding TSRMLS_CC);

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
	int str_len, from_encoding_len;
	char *newstr;
	size_t ret_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!", &str, &str_len,
				&from_encoding, &from_encoding_len) == FAILURE) {
		return;
	}
	newstr = php_unicode_convert_case(PHP_UNICODE_CASE_UPPER, str, (size_t) str_len, &ret_len, from_encoding TSRMLS_CC);

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
	int str_len, from_encoding_len;
	char *newstr;
	size_t ret_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!", &str, &str_len,
				&from_encoding, &from_encoding_len) == FAILURE) {
		return;
	}
	newstr = php_unicode_convert_case(PHP_UNICODE_CASE_LOWER, str, (size_t) str_len, &ret_len, from_encoding TSRMLS_CC);

	if (newstr) {
		RETURN_STRINGL(newstr, ret_len, 0);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string mb_detect_encoding(string str [, mixed encoding_list [, bool strict]])
   Encodings of the given string is returned (as a string) */
PHP_FUNCTION(mb_detect_encoding)
{
	zval **arg_str, **arg_list, **arg_strict;
	mbfl_string string;
	const char *ret;
	enum mbfl_no_encoding *elist;
	int size, *list, strict = 0;

	if (ZEND_NUM_ARGS() == 1) {
		if (zend_get_parameters_ex(1, &arg_str) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (ZEND_NUM_ARGS() == 2) {
		if (zend_get_parameters_ex(2, &arg_str, &arg_list) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (ZEND_NUM_ARGS() == 3) {
		if (zend_get_parameters_ex(3, &arg_str, &arg_list, &arg_strict) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else {
		WRONG_PARAM_COUNT;
	}

	/* make encoding list */
	list = NULL;
	size = 0;
	if (ZEND_NUM_ARGS() >= 2 &&  Z_STRVAL_PP(arg_list)) {
		switch (Z_TYPE_PP(arg_list)) {
		case IS_ARRAY:
			if (!php_mb_parse_encoding_array(*arg_list, &list, &size, 0 TSRMLS_CC)) {
				if (list) {
					efree(list);
					list = NULL;
					size = 0;
				}
			}
			break;
		default:
			convert_to_string_ex(arg_list);
			if (!php_mb_parse_encoding_list(Z_STRVAL_PP(arg_list), Z_STRLEN_PP(arg_list), &list, &size, 0 TSRMLS_CC)) {
				if (list) {
					efree(list);
					list = NULL;
					size = 0;
				}
			}
			break;
		}
		if (size <= 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Illegal argument");
		}
	}

	if (ZEND_NUM_ARGS() == 3) {
		convert_to_long_ex(arg_strict);
		strict = Z_LVAL_PP(arg_strict);
	}
	else {
		strict = MBSTRG(strict_detection);
	}

	if (size > 0 && list != NULL) {
		elist = list;
	} else {
		elist = MBSTRG(current_detect_order_list);
		size = MBSTRG(current_detect_order_list_size);
	}

	convert_to_string_ex(arg_str);
	mbfl_string_init(&string);
	string.no_language = MBSTRG(language);
	string.val = (unsigned char *)Z_STRVAL_PP(arg_str);
	string.len = Z_STRLEN_PP(arg_str);
	ret = mbfl_identify_encoding_name(&string, elist, size, strict);
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
		add_next_index_string(return_value, (char *) encoding->name, 1);
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
	int charset_name_len;
	char *trans_enc_name = NULL;
	int trans_enc_name_len;
	char *linefeed = "\r\n";
	int linefeed_len;
	long indent = 0;

	mbfl_string_init(&string);
	string.no_language = MBSTRG(language);
	string.no_encoding = MBSTRG(current_internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sssl", (char **)&string.val, &string.len, &charset_name, &charset_name_len, &trans_enc_name, &trans_enc_name_len, &linefeed, &linefeed_len, &indent) == FAILURE) {
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
	string.no_language = MBSTRG(language);
	string.no_encoding = MBSTRG(current_internal_encoding);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", (char **)&string.val, &string.len) == FAILURE) {
		return;
	}

	mbfl_string_init(&result);
	ret = mbfl_mime_header_decode(&string, &result, MBSTRG(current_internal_encoding));
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
	string.no_language = MBSTRG(language);
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

	ret = mbfl_ja_jp_hantozen(&string, &result, opt);
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
	zval ***args, ***stack, **var, **hash_entry;
	HashTable *target_hash;
	mbfl_string string, result, *ret;
	enum mbfl_no_encoding from_encoding, to_encoding;
	mbfl_encoding_detector *identd;
	mbfl_buffer_converter *convd;
	int n, argc, stack_level, stack_max, elistsz;
	enum mbfl_no_encoding *elist;
	char *name;
	void *ptmp;

	argc = ZEND_NUM_ARGS();
	if (argc < 3) {
		WRONG_PARAM_COUNT;
	}
	args = (zval ***)ecalloc(argc, sizeof(zval **));
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
	string.no_language = MBSTRG(language);

	/* pre-conversion encoding */
	elist = NULL;
	elistsz = 0;
	switch (Z_TYPE_PP(args[1])) {
	case IS_ARRAY:
		php_mb_parse_encoding_array(*args[1], &elist, &elistsz, 0 TSRMLS_CC);
		break;
	default:
		convert_to_string_ex(args[1]);
		php_mb_parse_encoding_list(Z_STRVAL_PP(args[1]), Z_STRLEN_PP(args[1]), &elist, &elistsz, 0 TSRMLS_CC);
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
		stack = (zval ***)safe_emalloc(stack_max, sizeof(zval **), 0);
		stack_level = 0;
		identd = mbfl_encoding_detector_new(elist, elistsz, MBSTRG(strict_detection));
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
									ptmp = erealloc(stack, sizeof(zval **)*stack_max);
									stack = (zval ***)ptmp;
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
								if (mbfl_encoding_detector_feed(identd, &string)) {
									goto detect_end;		/* complete detecting */
								}
							}
						}
					}
				} else if (Z_TYPE_PP(var) == IS_STRING) {
					string.val = (unsigned char *)Z_STRVAL_PP(var);
					string.len = Z_STRLEN_PP(var);
					if (mbfl_encoding_detector_feed(identd, &string)) {
						goto detect_end;		/* complete detecting */
					}
				}
			}
detect_end:
			from_encoding = mbfl_encoding_detector_judge(identd);
			mbfl_encoding_detector_delete(identd);
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
		convd = mbfl_buffer_converter_new(from_encoding, to_encoding, 0);
		if (convd == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create converter");
			RETURN_FALSE;
		}
		mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode));
		mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar));
	}

	/* convert */
	if (convd != NULL) {
		stack_max = PHP_MBSTR_STACK_BLOCK_SIZE;
		stack = (zval ***)safe_emalloc(stack_max, sizeof(zval **), 0);
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
								ptmp = erealloc(stack, sizeof(zval **)*stack_max);
								stack = (zval ***)ptmp;
							}
							stack[stack_level] = var;
							stack_level++;
							var = hash_entry;
							SEPARATE_ZVAL(hash_entry);
							target_hash = HASH_OF(*var);
							if (target_hash != NULL) {
								zend_hash_internal_pointer_reset(target_hash);
								continue;
							}
						} else if (Z_TYPE_PP(hash_entry) == IS_STRING) {
							string.val = (unsigned char *)Z_STRVAL_PP(hash_entry);
							string.len = Z_STRLEN_PP(hash_entry);
							ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
							if (ret != NULL) {
								if ((*hash_entry)->refcount > 1) {
									ZVAL_DELREF(*hash_entry);
									MAKE_STD_ZVAL(*hash_entry);
								} else {
									zval_dtor(*hash_entry);
								}
								ZVAL_STRINGL(*hash_entry, ret->val, ret->len, 0);
							}
						}
					}
				}
			} else if (Z_TYPE_PP(var) == IS_STRING) {
				string.val = (unsigned char *)Z_STRVAL_PP(var);
				string.len = Z_STRLEN_PP(var);
				ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
				if (ret != NULL) {
					zval_dtor(*var);
					ZVAL_STRINGL(*var, ret->val, ret->len, 0);
				}
			}
		}
		efree(stack);

		MBSTRG(illegalchars) += mbfl_buffer_illegalchars(convd);
		mbfl_buffer_converter_delete(convd);
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
	zval **arg1, **arg2, **arg3, **hash_entry;
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
	string.no_language = MBSTRG(language);
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
			convmap = (int *)safe_emalloc(i, sizeof(int), 0);
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

	ret = mbfl_html_numeric_entity(&string, &result, convmap, mapsize, type);
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
	int i;
	char *to_r = NULL;
	char *force_extra_parameters = INI_STR("mail.force_extra_parameters");
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
	extern void mbfl_memory_device_unput(mbfl_memory_device *device);
	char *pp, *ee;
    
	if (PG(safe_mode) && (ZEND_NUM_ARGS() == 5)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "SAFE MODE Restriction in effect.  The fifth parameter is disabled in SAFE MODE.");
		RETURN_FALSE;
	}
    
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss|ss", &to, &to_len, &subject, &subject_len, &message, &message_len, &headers, &headers_len, &extra_cmd, &extra_cmd_len) == FAILURE) {
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
		MAIL_ASCIIZ_CHECK_MBSTRING(extra_cmd, extra_cmd_len);
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
						
						charset = php_strtok_r(NULL, "= \"", &tmp);
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing To: field");
		err = 1;
	}

	/* Subject: */
	if (subject != NULL && subject_len >= 0) {
		orig_str.no_language = MBSTRG(language);
		orig_str.val = (unsigned char *)subject;
		orig_str.len = subject_len;
		orig_str.no_encoding = MBSTRG(current_internal_encoding);
		if (orig_str.no_encoding == mbfl_no_encoding_invalid
		    || orig_str.no_encoding == mbfl_no_encoding_pass) {
			orig_str.no_encoding = mbfl_identify_encoding_no(&orig_str, MBSTRG(current_detect_order_list), MBSTRG(current_detect_order_list_size), MBSTRG(strict_detection));
		}
		pstr = mbfl_mime_header_encode(&orig_str, &conv_str, tran_cs, head_enc, "\n", sizeof("Subject: [PHP-jp nnnnnnnn]"));
		if (pstr != NULL) {
			subject_buf = subject = (char *)pstr->val;
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Missing Subject: field");
		err = 1;
	}

	/* message body */
	if (message != NULL) {
		orig_str.no_language = MBSTRG(language);
		orig_str.val = message;
		orig_str.len = message_len;
		orig_str.no_encoding = MBSTRG(current_internal_encoding);

		if (orig_str.no_encoding == mbfl_no_encoding_invalid
		    || orig_str.no_encoding == mbfl_no_encoding_pass) {
			orig_str.no_encoding = mbfl_identify_encoding_no(&orig_str, MBSTRG(current_detect_order_list), MBSTRG(current_detect_order_list_size), MBSTRG(strict_detection));
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
		mbfl_memory_device_strncat(&device, p, n);
		if (n > 0 && p[n - 1] != '\n') {
			mbfl_memory_device_strncat(&device, "\n", 1);
		}
	}

	mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER1, sizeof(PHP_MBSTR_MAIL_MIME_HEADER1) - 1);
	mbfl_memory_device_strncat(&device, "\n", 1);

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
		extra_cmd = php_escape_shell_cmd(extra_cmd);
	} 

	if (!err && php_mail(to_r, subject, message, headers, extra_cmd TSRMLS_CC)) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}

	if (extra_cmd) {
		efree(extra_cmd);
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
#undef APPEND_ONE_CHAR
#undef SEPARATE_SMART_STR
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
	int typ_len, n;
	char *name;
	const struct mb_overload_def *over_func;
	zval *row1, *row2;
	const mbfl_language *lang = mbfl_no2language(MBSTRG(language));
	enum mbfl_no_encoding *entry;
#ifdef ZEND_MULTIBYTE
	zval *row3;
#endif /* ZEND_MULTIBYTE */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &typ, &typ_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (!typ || !strcasecmp("all", typ)) {
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
		add_assoc_long(return_value, "func_overload", MBSTRG(func_overload));
		if (MBSTRG(func_overload)){
			over_func = &(mb_ovld[0]);
			MAKE_STD_ZVAL(row1);
			array_init(row1);
			while (over_func->type > 0) {
				if ((MBSTRG(func_overload) & over_func->type) == over_func->type ) {
					add_assoc_string(row1, over_func->orig_func, over_func->ovld_func, 1);
				}
				over_func++;
			}
			add_assoc_zval(return_value, "func_overload_list", row1);
		} else {
			add_assoc_string(return_value, "func_overload_list", "no overload", 1);
 		}
		if (lang != NULL) {
			if ((name = (char *)mbfl_no_encoding2name(lang->mail_charset)) != NULL) {
				add_assoc_string(return_value, "mail_charset", name, 1);
			}
			if ((name = (char *)mbfl_no_encoding2name(lang->mail_header_encoding)) != NULL) {
				add_assoc_string(return_value, "mail_header_encoding", name, 1);
			}
			if ((name = (char *)mbfl_no_encoding2name(lang->mail_body_encoding)) != NULL) {
				add_assoc_string(return_value, "mail_body_encoding", name, 1);
			}
		}
		add_assoc_long(return_value, "illegal_chars", MBSTRG(illegalchars));
		if (MBSTRG(encoding_translation)) {
			add_assoc_string(return_value, "encoding_translation", "On", 1);
		} else {
			add_assoc_string(return_value, "encoding_translation", "Off", 1);
		}
		if ((name = (char *)mbfl_no_language2name(MBSTRG(language))) != NULL) {
			add_assoc_string(return_value, "language", name, 1);
		}		
		n = MBSTRG(current_detect_order_list_size);
		entry = MBSTRG(current_detect_order_list);
		if(n > 0) {
			MAKE_STD_ZVAL(row2);
			array_init(row2);
			while (n > 0) {
				if ((name = (char *)mbfl_no_encoding2name(*entry)) != NULL) {
					add_next_index_string(row2, name, 1);
				}
				entry++;
				n--;
			}
			add_assoc_zval(return_value, "detect_order", row2);
		}
		if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			add_assoc_string(return_value, "substitute_character", "none", 1);
		} else if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG) {
			add_assoc_string(return_value, "substitute_character", "long", 1);
		} else if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY) {
			add_assoc_string(return_value, "substitute_character", "entity", 1);
		} else {
			add_assoc_long(return_value, "substitute_character", MBSTRG(current_filter_illegal_substchar));
		}
		if (MBSTRG(strict_detection)) {
			add_assoc_string(return_value, "strict_detection", "On", 1);
		} else {
			add_assoc_string(return_value, "strict_detection", "Off", 1);
		}
#ifdef ZEND_MULTIBYTE
		entry = MBSTRG(script_encoding_list);
		n = MBSTRG(script_encoding_list_size);
		if(n > 0) {
			MAKE_STD_ZVAL(row3);
			array_init(row3);
			while (n > 0) {
				if ((name = (char *)mbfl_no_encoding2name(*entry)) != NULL) {
					add_next_index_string(row3, name, 1);
				}
				entry++;
				n--;
			}
			add_assoc_zval(return_value, "script_encoding", row3);
		}
#endif /* ZEND_MULTIBYTE */
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
 		RETVAL_LONG(MBSTRG(func_overload));
	} else if (!strcasecmp("func_overload_list", typ)) {
		if (MBSTRG(func_overload)){
				over_func = &(mb_ovld[0]);
				array_init(return_value);
				while (over_func->type > 0) {
					if ((MBSTRG(func_overload) & over_func->type) == over_func->type ) {
						add_assoc_string(return_value, over_func->orig_func, over_func->ovld_func, 1);
					}
					over_func++;
				}
		} else {
			RETVAL_STRING("no overload", 1);
		}
	} else if (!strcasecmp("mail_charset", typ)) {
		if (lang != NULL && (name = (char *)mbfl_no_encoding2name(lang->mail_charset)) != NULL) {
			RETVAL_STRING(name, 1);
		}
	} else if (!strcasecmp("mail_header_encoding", typ)) {
		if (lang != NULL && (name = (char *)mbfl_no_encoding2name(lang->mail_header_encoding)) != NULL) {
			RETVAL_STRING(name, 1);
		}
	} else if (!strcasecmp("mail_body_encoding", typ)) {
		if (lang != NULL && (name = (char *)mbfl_no_encoding2name(lang->mail_body_encoding)) != NULL) {
			RETVAL_STRING(name, 1);
		}
	} else if (!strcasecmp("illegal_chars", typ)) {
		RETVAL_LONG(MBSTRG(illegalchars));
	} else if (!strcasecmp("encoding_translation", typ)) {
		if (MBSTRG(encoding_translation)) {
			RETVAL_STRING("On", 1);
		} else {
			RETVAL_STRING("Off", 1);
		}
	} else if (!strcasecmp("language", typ)) {
		if ((name = (char *)mbfl_no_language2name(MBSTRG(language))) != NULL) {
			RETVAL_STRING(name, 1);
		}		
	} else if (!strcasecmp("detect_order", typ)) {
		n = MBSTRG(current_detect_order_list_size);
		entry = MBSTRG(current_detect_order_list);
		if(n > 0) {
			array_init(return_value);
			while (n > 0) {
				name = (char *)mbfl_no_encoding2name(*entry);
				if (name) {
					add_next_index_string(return_value, name, 1);
				}
				entry++;
				n--;
			}
		}
	} else if (!strcasecmp("substitute_character", typ)) {
		if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			RETVAL_STRING("none", 1);
		} else if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG) {
			RETVAL_STRING("long", 1);
		} else if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY) {
			RETVAL_STRING("entity", 1);
		} else {
			RETVAL_LONG(MBSTRG(current_filter_illegal_substchar));
		}
	} else if (!strcasecmp("strict_detection", typ)) {
		if (MBSTRG(strict_detection)) {
			RETVAL_STRING("On", 1);
		} else {
			RETVAL_STRING("Off", 1);
		}
	} else {
#ifdef ZEND_MULTIBYTE
	if (!strcasecmp("script_encoding", typ)) {
		entry = MBSTRG(script_encoding_list);
		n = MBSTRG(script_encoding_list_size);
		if(n > 0) {
			array_init(return_value);
			while (n > 0) {
				name = (char *)mbfl_no_encoding2name(*entry);
				if (name) {
					add_next_index_string(return_value, name, 1);
				}
				entry++;
				n--;
			}
		}
		return;
	}
#endif /* ZEND_MULTIBYTE */
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool mb_check_encoding([string var[, string encoding]])
   Check if the string is valid for the specified encoding */
PHP_FUNCTION(mb_check_encoding)
{
	char *var = NULL;
	int var_len;
	char *enc = NULL;
	int enc_len;
	mbfl_buffer_converter *convd;
	enum mbfl_no_encoding no_encoding = MBSTRG(current_internal_encoding);
	mbfl_string string, result, *ret = NULL;
	long illegalchars = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ss", &var, &var_len, &enc, &enc_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (var == NULL) {
		RETURN_BOOL(MBSTRG(illegalchars) == 0);
	}

	if (enc != NULL) {
		no_encoding = mbfl_name2no_encoding(enc);
		if (no_encoding == mbfl_no_encoding_invalid || no_encoding == mbfl_no_encoding_pass) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid encoding \"%s\"", enc);
			RETURN_FALSE;
		}
	}
	
	convd = mbfl_buffer_converter_new(no_encoding, no_encoding, 0);
	if (convd == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to create converter");
		RETURN_FALSE;
	}	
	mbfl_buffer_converter_illegal_mode(convd, MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE);
	mbfl_buffer_converter_illegal_substchar(convd, 0);	
	
	/* initialize string */
	mbfl_string_init_set(&string, mbfl_no_language_neutral, no_encoding);
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

/* {{{ MBSTRING_API size_t php_mb_gpc_mbchar_bytes() */
MBSTRING_API size_t php_mb_gpc_mbchar_bytes(const char *s TSRMLS_DC)
{

	if (MBSTRG(http_input_identify) != mbfl_no_encoding_invalid){
		return php_mb_mbchar_bytes_ex(s,
    		mbfl_no2encoding(MBSTRG(http_input_identify)));
	} else {
		return php_mb_mbchar_bytes_ex(s,
	    	mbfl_no2encoding(MBSTRG(internal_encoding)));
	}
}
/* }}} */

/*	{{{ MBSTRING_API int php_mb_gpc_encoding_converter() */
MBSTRING_API int php_mb_gpc_encoding_converter(char **str, int *len, int num, const char *encoding_to, const char *encoding_from 
		TSRMLS_DC)
{
	int i;
	mbfl_string string, result, *ret = NULL;
	enum mbfl_no_encoding from_encoding, to_encoding;
	mbfl_buffer_converter *convd;

	if (encoding_to) {
		/* new encoding */
		to_encoding = mbfl_name2no_encoding(encoding_to);
		if (to_encoding == mbfl_no_encoding_invalid) {
			return -1;
		}
	} else {
		to_encoding = MBSTRG(current_internal_encoding);
	}	
	if (encoding_from) {
		/* old encoding */
		from_encoding = mbfl_name2no_encoding(encoding_from);
		if (from_encoding == mbfl_no_encoding_invalid) {
			return -1;
		}
	} else {
		from_encoding = MBSTRG(http_input_identify);
	}

	if (from_encoding == mbfl_no_encoding_pass) {
		return 0;
	}

	/* initialize string */
	mbfl_string_init(&string);
	mbfl_string_init(&result);
	string.no_encoding = from_encoding;
	string.no_language = MBSTRG(language);

	for (i=0; i<num; i++){
		string.val = (char*)str[i];
		string.len = len[i];

		/* initialize converter */
		convd = mbfl_buffer_converter_new(from_encoding, to_encoding, string.len);
		if (convd == NULL) {
			return -1;
		}
		mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode));
		mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar));
		
		/* do it */
		ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
		if (ret != NULL) {
			efree(str[i]);
			str[i] = ret->val;
			len[i] = ret->len;
		}
		
		MBSTRG(illegalchars) += mbfl_buffer_illegalchars(convd);
		mbfl_buffer_converter_delete(convd);
	}

	return ret ? 0 : -1;
}
/* }}} */

/* {{{ MBSTRING_API int php_mb_gpc_encoding_detector()
 */
MBSTRING_API int php_mb_gpc_encoding_detector(char **arg_string, int *arg_length, int num, char *arg_list TSRMLS_DC)
{
	mbfl_string string;
	enum mbfl_no_encoding *elist;
	enum mbfl_no_encoding encoding = mbfl_no_encoding_invalid;
	mbfl_encoding_detector *identd = NULL; 

	int size;
	enum mbfl_no_encoding *list;

	if (MBSTRG(http_input_list_size) == 1 && 
		MBSTRG(http_input_list)[0] == mbfl_no_encoding_pass) {
		MBSTRG(http_input_identify) = mbfl_no_encoding_pass;
		return SUCCESS;
	}

	if (MBSTRG(http_input_list_size) == 1 && 
		MBSTRG(http_input_list)[0] != mbfl_no_encoding_auto &&
		mbfl_no_encoding2name(MBSTRG(http_input_list)[0]) != NULL) {
		MBSTRG(http_input_identify) = MBSTRG(http_input_list)[0];
		return SUCCESS;
	}

	if (arg_list && strlen(arg_list)>0) {
		/* make encoding list */
		list = NULL;
		size = 0;
		php_mb_parse_encoding_list(arg_list, strlen(arg_list), &list, &size, 0 TSRMLS_CC);
		
		if (size > 0 && list != NULL) {
			elist = list;
		} else {
			elist = MBSTRG(current_detect_order_list);
			size = MBSTRG(current_detect_order_list_size);
			if (size <= 0){
				elist = MBSTRG(default_detect_order_list);
				size = MBSTRG(default_detect_order_list_size);
			}
		}
	} else {
		elist = MBSTRG(current_detect_order_list);
		size = MBSTRG(current_detect_order_list_size);
		if (size <= 0){
			elist = MBSTRG(default_detect_order_list);
			size = MBSTRG(default_detect_order_list_size);
		}
	}

	mbfl_string_init(&string);
	string.no_language = MBSTRG(language);

	identd = mbfl_encoding_detector_new(elist, size, MBSTRG(strict_detection));

	if (identd) {
		int n = 0;
		while(n < num){
			string.val = (unsigned char *)arg_string[n];
			string.len = arg_length[n];
			if (mbfl_encoding_detector_feed(identd, &string)) {
				break;
			}
			n++;
		}
		encoding = mbfl_encoding_detector_judge(identd);
		mbfl_encoding_detector_delete(identd);
	}

	if (encoding != mbfl_no_encoding_invalid) {
		MBSTRG(http_input_identify) = encoding;
		return SUCCESS;
	} else {
		return FAILURE;
	}
}
/* }}} */

/* {{{ MBSTRING_API int php_mb_stripos()
 */

MBSTRING_API int php_mb_stripos(int mode, char *old_haystack, int old_haystack_len, char *old_needle, int old_needle_len, long offset, char *from_encoding TSRMLS_DC)
{
	int n;
	mbfl_string haystack, needle;
	n = -1;

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(language);
	haystack.no_encoding = MBSTRG(current_internal_encoding);
	needle.no_language = MBSTRG(language);
	needle.no_encoding = MBSTRG(current_internal_encoding);

	do {
		haystack.val = php_unicode_convert_case(PHP_UNICODE_CASE_UPPER, old_haystack, (size_t) old_haystack_len, &haystack.len, from_encoding TSRMLS_CC);

		if (!haystack.val) {
			break;
		}

		if (haystack.len <= 0) {
			break;
		}

		needle.val = php_unicode_convert_case(PHP_UNICODE_CASE_UPPER, old_needle, (size_t) old_needle_len, &needle.len, from_encoding TSRMLS_CC);

		if (!needle.val) {
			break;
		}

		if (needle.len <= 0) {
			break;
		}

		haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(from_encoding);
		if (haystack.no_encoding == mbfl_no_encoding_invalid) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", from_encoding);
			break;
		}

		{
			int haystack_char_len = mbfl_strlen(&haystack);

			if (mode) {
				if ((offset > 0 && offset > haystack_char_len) ||
					(offset < 0 && -offset > haystack_char_len)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset is greater than the length of haystack string");
					break;
				}
			} else {
				if (offset < 0 || offset > haystack_char_len) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset not contained in string");
					break;
				}
			}
		}

		n = mbfl_strpos(&haystack, &needle, offset, mode);
	} while(0);

	if(haystack.val){
		efree(haystack.val);
	}

	if(needle.val){
		efree(needle.val);
	}

	return n;
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
	zend_encoding_oddlen encoding_oddlen;

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
	encoding_converter = php_mb_encoding_converter;
	encoding_oddlen = php_mb_oddlen;

	/* TODO: make independent from mbstring.encoding_translation? */
	if (MBSTRG(encoding_translation)) {
		/* notify internal encoding to Zend Engine */
		name = (char*)mbfl_no_encoding2name(MBSTRG(current_internal_encoding));
		zend_multibyte_set_internal_encoding(name, strlen(name) TSRMLS_CC);
	}

	zend_multibyte_set_functions(encoding_detector, encoding_converter, encoding_oddlen TSRMLS_CC);

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
	php_mb_parse_encoding_list(arg_list, strlen(arg_list), &list, &size, 0 TSRMLS_CC);
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
	string.no_language = MBSTRG(language);
	string.val = (char*)arg_string;
	string.len = arg_length;
	ret = mbfl_identify_encoding_name(&string, elist, size, 0);
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
	string.no_language = MBSTRG(language);
	string.val = (char*)from;
	string.len = from_length;

	/* initialize converter */
	convd = mbfl_buffer_converter_new(from_encoding, to_encoding, string.len);
	if (convd == NULL) {
		return -1;
	}
	mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode));
	mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar));

	/* do it */
	ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
	if (ret != NULL) {
		*to = ret->val;
		*to_length = ret->len;
	}

	MBSTRG(illegalchars) += mbfl_buffer_illegalchars(convd);
	mbfl_buffer_converter_delete(convd);

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
	mb_string.no_language = MBSTRG(language);
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
