/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 2001 The PHP Group                                     |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Tsukada Takuya <tsukada@fminn.nagano.nagano.jp>             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/*
 * PHP4 Multibyte String module "mbstring" (currently only for Japanese)
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_variables.h"
#include "mbstring.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_mail.h"
#include "ext/standard/url.h"
#include "main/php_output.h"
#include "ext/standard/info.h"

#include "php_variables.h"
#include "php_globals.h"
#include "rfc1867.h"
#include "php_content_types.h"
#include "SAPI.h"

#if HAVE_MBSTRING

static enum mbfl_no_encoding php_mbstr_default_identify_list[] = {
	mbfl_no_encoding_ascii,
	mbfl_no_encoding_jis,
	mbfl_no_encoding_utf8,
	mbfl_no_encoding_euc_jp,
	mbfl_no_encoding_sjis
};

static int php_mbstr_default_identify_list_size = sizeof(php_mbstr_default_identify_list)/sizeof(enum mbfl_no_encoding);

static unsigned char third_and_rest_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE_REST };
static unsigned char second_args_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };

#if defined(MBSTR_ENC_TRANS)
SAPI_POST_HANDLER_FUNC(php_mbstr_post_handler);

static sapi_post_entry mbstr_post_entries[] = {
	{ DEFAULT_POST_CONTENT_TYPE,	sizeof(DEFAULT_POST_CONTENT_TYPE)-1,	sapi_read_standard_form_data,	php_mbstr_post_handler },
	{ MULTIPART_CONTENT_TYPE,		sizeof(MULTIPART_CONTENT_TYPE)-1,		sapi_read_standard_form_data,	rfc1867_post_handler },
	{ NULL, 0, NULL }
};
#endif

function_entry mbstring_functions[] = {
	PHP_FE(mb_language,					NULL)
	PHP_FE(mb_internal_encoding,		NULL)
	PHP_FE(mb_http_input,				NULL)
	PHP_FE(mb_http_output,			NULL)
	PHP_FE(mb_detect_order,			NULL)
	PHP_FE(mb_substitute_character,	NULL)
	PHP_FE(mb_parse_str,			second_args_force_ref)
	PHP_FE(mb_output_handler,			NULL)
	PHP_FE(mb_preferred_mime_name,	NULL)
	PHP_FE(mb_strlen,					NULL)
	PHP_FE(mb_strpos,					NULL)
	PHP_FE(mb_strrpos,				NULL)
	PHP_FE(mb_substr,					NULL)
	PHP_FE(mb_strcut,					NULL)
	PHP_FE(mb_strwidth,				NULL)
	PHP_FE(mb_strimwidth,				NULL)
	PHP_FE(mb_convert_encoding,		NULL)
	PHP_FE(mb_detect_encoding,		NULL)
	PHP_FE(mb_convert_kana,			NULL)
	PHP_FE(mb_encode_mimeheader,		NULL)
	PHP_FE(mb_decode_mimeheader,		NULL)
	PHP_FE(mb_convert_variables,		third_and_rest_force_ref)
	PHP_FE(mb_encode_numericentity,		NULL)
	PHP_FE(mb_decode_numericentity,		NULL)
	PHP_FE(mb_send_mail,					NULL)
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
	{ NULL, NULL, NULL }
};

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

ZEND_DECLARE_MODULE_GLOBALS(mbstring)

#ifdef COMPILE_DL_MBSTRING
ZEND_GET_MODULE(mbstring)
#endif


static int
php_mbstring_parse_encoding_list(const char *value, int value_length, int **return_list, int *return_size, int persistent)
{
	int n, l, size, bauto, *src, *list, *entry;
	char *p, *p1, *p2, *endp, *tmpstr;
	enum mbfl_no_encoding no_encoding;

	list = NULL;
	if (value == NULL || value_length <= 0) {
		return 0;
	} else {
		/* copy the value string for work */
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
		size = n + php_mbstr_default_identify_list_size;
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
						l = php_mbstr_default_identify_list_size;
						src = php_mbstr_default_identify_list;
						while (l > 0) {
							*entry++ = *src++;
							l--;
							n++;
						}
					}
				} else if (no_encoding != mbfl_no_encoding_invalid) {
					*entry++ = no_encoding;
					n++;
				}
				p1 = p2 + 1;
			} while (n < size && p2 != NULL);
			*return_list = list;
			*return_size = n;
		}
		efree(tmpstr);
	}

	if (list == NULL) {
		return 0;
	}

	return 1;
}

static int
php_mbstring_parse_encoding_array(zval *array, int **return_list, int *return_size, int persistent)
{
	zval **hash_entry;
	HashTable *target_hash;
	int i, n, l, size, bauto, *list, *entry, *src;
	enum mbfl_no_encoding no_encoding;

	list = NULL;
	if (Z_TYPE_P(array) == IS_ARRAY) {
		target_hash = Z_ARRVAL_P(array);
		zend_hash_internal_pointer_reset(target_hash);
		i = zend_hash_num_elements(target_hash);
		size = i + php_mbstr_default_identify_list_size;
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
						l = php_mbstr_default_identify_list_size;
						src = php_mbstr_default_identify_list;
						while (l > 0) {
							*entry++ = *src++;
							l--;
							n++;
						}
					}
				} else if (no_encoding != mbfl_no_encoding_invalid) {
					*entry++ = no_encoding;
					n++;
				}
				zend_hash_move_forward(target_hash);
				i--;
			}
			*return_list = list;
			*return_size = n;
		}
	}

	if (list == NULL) {
		return 0;
	}

	return 1;
}


/* php.ini directive handler */
static PHP_INI_MH(OnUpdate_mbstring_detect_order)
{
	int *list, size;

	if (php_mbstring_parse_encoding_list(new_value, new_value_length, &list, &size, 1)) {
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

static PHP_INI_MH(OnUpdate_mbstring_http_input)
{
	int *list, size;

	if (php_mbstring_parse_encoding_list(new_value, new_value_length, &list, &size, 1)) {
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

static PHP_INI_MH(OnUpdate_mbstring_internal_encoding)
{
	enum mbfl_no_encoding no_encoding;

	no_encoding = mbfl_name2no_encoding(new_value);
	if (no_encoding != mbfl_no_encoding_invalid) {
		MBSTRG(internal_encoding) = no_encoding;
		MBSTRG(current_internal_encoding) = no_encoding;
	} else {
		if (new_value != NULL && new_value_length > 0) {
			return FAILURE;
		}
	}

	return SUCCESS;
}

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

/* php.ini directive registration */
PHP_INI_BEGIN()
	PHP_INI_ENTRY("mbstring.detect_order", NULL, PHP_INI_ALL, OnUpdate_mbstring_detect_order)
	PHP_INI_ENTRY("mbstring.http_input", NULL, PHP_INI_ALL, OnUpdate_mbstring_http_input)
	PHP_INI_ENTRY("mbstring.http_output", NULL, PHP_INI_ALL, OnUpdate_mbstring_http_output)
	PHP_INI_ENTRY("mbstring.internal_encoding", NULL, PHP_INI_ALL, OnUpdate_mbstring_internal_encoding)
	PHP_INI_ENTRY("mbstring.substitute_character", NULL, PHP_INI_ALL, OnUpdate_mbstring_substitute_character)
PHP_INI_END()


/* module global initialize handler */
static void
php_mbstring_init_globals(zend_mbstring_globals *pglobals)
{
	pglobals->language = mbfl_no_language_japanese;
	pglobals->current_language = mbfl_no_language_japanese;
	pglobals->internal_encoding = mbfl_no_encoding_euc_jp;
	pglobals->current_internal_encoding = mbfl_no_encoding_euc_jp;
	pglobals->http_output_encoding = mbfl_no_encoding_invalid;
	pglobals->current_http_output_encoding = mbfl_no_encoding_invalid;
	pglobals->http_input_identify = mbfl_no_encoding_invalid;
	pglobals->http_input_identify_get = mbfl_no_encoding_invalid;
	pglobals->http_input_identify_post = mbfl_no_encoding_invalid;
	pglobals->http_input_identify_cookie = mbfl_no_encoding_invalid;
	pglobals->http_input_identify_string = mbfl_no_encoding_invalid;
	pglobals->http_input_list = NULL;
	pglobals->http_input_list_size = 0;
	pglobals->detect_order_list = NULL;
	pglobals->detect_order_list_size = 0;
	pglobals->current_detect_order_list = NULL;
	pglobals->current_detect_order_list_size = 0;
	pglobals->filter_illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
	pglobals->filter_illegal_substchar = 0x3f;	/* '?' */
	pglobals->current_filter_illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
	pglobals->current_filter_illegal_substchar = 0x3f;	/* '?' */
	pglobals->outconv = NULL;
}

PHP_MINIT_FUNCTION(mbstring)
{
	ZEND_INIT_MODULE_GLOBALS(mbstring, php_mbstring_init_globals, NULL);
	REGISTER_INI_ENTRIES();

#if defined(MBSTR_ENC_TRANS)
	sapi_unregister_post_entry(mbstr_post_entries);
	sapi_register_post_entries(mbstr_post_entries);
#endif

	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(mbstring)
{
	UNREGISTER_INI_ENTRIES();
	
	if (MBSTRG(http_input_list)) {
		free(MBSTRG(http_input_list));
	}
	if (MBSTRG(detect_order_list)) {
		free(MBSTRG(detect_order_list));
	}

	return SUCCESS;
}


PHP_RINIT_FUNCTION(mbstring)
{
	int n, *list, *entry;

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
		list = php_mbstr_default_identify_list;
		n = php_mbstr_default_identify_list_size;
	}
	entry = (int *)emalloc(n*sizeof(int));
	if (entry != NULL) {
		MBSTRG(current_detect_order_list) = entry;
		MBSTRG(current_detect_order_list_size) = n;
		while (n > 0) {
			*entry++ = *list++;
			n--;
		}
	}

	return SUCCESS;
}


PHP_RSHUTDOWN_FUNCTION(mbstring)
{
	if (MBSTRG(current_detect_order_list) != NULL) {
		efree(MBSTRG(current_detect_order_list));
		MBSTRG(current_detect_order_list) = NULL;
		MBSTRG(current_detect_order_list_size) = 0;
	}
	if (MBSTRG(outconv) != NULL) {
		mbfl_buffer_converter_delete(MBSTRG(outconv));
		MBSTRG(outconv) = NULL;
	}

	/* clear http input identification. */
	MBSTRG(http_input_identify) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_post) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_get) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_cookie) = mbfl_no_encoding_invalid;
	MBSTRG(http_input_identify_string) = mbfl_no_encoding_invalid;

	return SUCCESS;
}


PHP_MINFO_FUNCTION(mbstring)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Multibyte (Japanese) Support", "enabled");
#if defined(MBSTR_ENC_TRANS)
	php_info_print_table_row(2, "http input encoding translation", "enabled");	
#endif
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}



/* {{{ proto string mb_language([string language])
   Sets the current language or Returns the current language as a string. */
PHP_FUNCTION(mb_language)
{
	pval **arg1;
	char *name;
	enum mbfl_no_language no_language;

	if (ZEND_NUM_ARGS() == 0) {
		name = (char *)mbfl_no_language2name(MBSTRG(current_language));
		if (name != NULL) {
			RETURN_STRING(name, 1);
		} else {
			RETURN_FALSE;
		}
	} else if (ZEND_NUM_ARGS() == 1 && zend_get_parameters_ex(1, &arg1) != FAILURE) {
		convert_to_string_ex(arg1);
		no_language = mbfl_name2no_language(Z_STRVAL_PP(arg1));
		if (no_language == mbfl_no_language_invalid) {
			php_error(E_WARNING, "unknown language \"%s\"", Z_STRVAL_PP(arg1));
			RETURN_FALSE;
		} else {
			MBSTRG(current_language) = no_language;
			RETURN_TRUE;
		}
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */



/* {{{ proto string mb_internal_encoding([string encoding])
   Sets the current internal encoding or Returns the current internal encoding as a string. */
PHP_FUNCTION(mb_internal_encoding)
{
	pval **arg1;
	char *name;
	enum mbfl_no_encoding no_encoding;

	if (ZEND_NUM_ARGS() == 0) {
		name = (char *)mbfl_no_encoding2name(MBSTRG(current_internal_encoding));
		if (name != NULL) {
			RETURN_STRING(name, 1);
		} else {
			RETURN_FALSE;
		}
	} else if (ZEND_NUM_ARGS() == 1 && zend_get_parameters_ex(1, &arg1) != FAILURE) {
		convert_to_string_ex(arg1);
		no_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(arg1));
		if (no_encoding == mbfl_no_encoding_invalid) {
			php_error(E_WARNING, "unknown encoding \"%s\"", Z_STRVAL_PP(arg1));
			RETURN_FALSE;
		} else {
			MBSTRG(current_internal_encoding) = no_encoding;
			RETURN_TRUE;
		}
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */


/* {{{ proto string mb_http_input([string type])
   Returns the input encoding. */
PHP_FUNCTION(mb_http_input)
{
	pval **arg1;
	int result, retname, n, *entry;
	char *name;

	retname = 1;
	if (ZEND_NUM_ARGS() == 0) {
		result = MBSTRG(http_input_identify);
	} else if (ARG_COUNT(ht) == 1 && zend_get_parameters_ex(1, &arg1) != FAILURE) {
		convert_to_string_ex(arg1);
		switch (*(Z_STRVAL_PP(arg1))) {
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
			if (array_init(return_value) == FAILURE) {
				RETURN_FALSE;
			}
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
		default:
			result = MBSTRG(http_input_identify);
			break;
		}
	} else {
		WRONG_PARAM_COUNT;
	}

	if (retname) {
		name = (char *)mbfl_no_encoding2name(result);
		if (name != NULL) {
			RETVAL_STRING(name, 1);
		} else {
			RETVAL_FALSE;
		}
	}
}
/* }}} */


/* {{{ proto string mb_http_output([string encoding])
   Sets the current output_encoding or Returns the current output_encoding as a string. */
PHP_FUNCTION(mb_http_output)
{
	pval **arg1;
	char *name;
	enum mbfl_no_encoding no_encoding;

	if (ZEND_NUM_ARGS() == 0) {
		name = (char *)mbfl_no_encoding2name(MBSTRG(current_http_output_encoding));
		if (name != NULL) {
			RETURN_STRING(name, 1);
		} else {
			RETURN_FALSE;
		}
	} else if (ZEND_NUM_ARGS() == 1 && zend_get_parameters_ex(1, &arg1) != FAILURE) {
		convert_to_string_ex(arg1);
		no_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(arg1));
		if (no_encoding == mbfl_no_encoding_invalid) {
			php_error(E_WARNING, "unknown encoding \"%s\"", Z_STRVAL_PP(arg1));
			RETURN_FALSE;
		} else {
			MBSTRG(current_http_output_encoding) = no_encoding;
			RETURN_TRUE;
		}
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */


/* {{{ proto array mb_detect_order([mixed encoding-list])
   Sets the current detect_order or Return the current detect_order as a array. */
PHP_FUNCTION(mb_detect_order)
{
	pval **arg1;
	int n, size, *list, *entry;
	char *name;

	if (ZEND_NUM_ARGS() == 0) {
		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}
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
			php_mbstring_parse_encoding_array(*arg1, &list, &size, 0);
			break;
		default:
			convert_to_string_ex(arg1);
			php_mbstring_parse_encoding_list(Z_STRVAL_PP(arg1), Z_STRLEN_PP(arg1), &list, &size, 0);
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
   Sets the current substitute_character or Returns the current substitute_character. */
PHP_FUNCTION(mb_substitute_character)
{
	pval **arg1;

	if (ZEND_NUM_ARGS() == 0) {
		if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			RETVAL_STRING("none", 1);
		} else if(MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG) {
			RETVAL_STRING("long", 1);
		} else {
			RETVAL_LONG(MBSTRG(current_filter_illegal_substchar));
		}
	} else if (ZEND_NUM_ARGS() == 1 && zend_get_parameters_ex(1, &arg1) != FAILURE) {
		switch (Z_TYPE_PP(arg1)) {
		case IS_STRING:
			if (strcasecmp("none", Z_STRVAL_PP(arg1)) == 0) {
				MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
			} else if (strcasecmp("long", Z_STRVAL_PP(arg1)) == 0) {
				MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG;
			} else {
				convert_to_long_ex(arg1);
				MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
				MBSTRG(current_filter_illegal_substchar) = Z_LVAL_PP(arg1);
			}
			break;
		default:
			convert_to_long_ex(arg1);
			MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
			MBSTRG(current_filter_illegal_substchar) = Z_LVAL_PP(arg1);
			break;
		}
		RETVAL_TRUE;
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */


/* {{{ proto string mb_preferred_mime_name(string encoding)
   Return the preferred MIME name (charset) as a string. */
PHP_FUNCTION(mb_preferred_mime_name)
{
	pval **arg1;
	enum mbfl_no_encoding no_encoding;
	const char *name;

	if (ZEND_NUM_ARGS() == 1 && zend_get_parameters_ex(1, &arg1) != FAILURE) {
		convert_to_string_ex(arg1);
		no_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(arg1));
		if (no_encoding == mbfl_no_encoding_invalid) {
			php_error(E_WARNING, "unknown encoding \"%s\"", Z_STRVAL_PP(arg1));
			RETVAL_FALSE;
		} else {
			name = mbfl_no2preferred_mime_name(no_encoding);
			if (name == NULL || *name == '\0') {
				php_error(E_WARNING, "no name for \"%s\"", Z_STRVAL_PP(arg1));
				RETVAL_FALSE;
			} else {
				RETVAL_STRING((char *)name, 1);
			}
		}
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

#if defined(MBSTR_ENC_TRANS)
static void
php_mbstr_encoding_handler(zval *arg, char *res, char *separator TSRMLS_DC)
{
	char *var, *val;
	char *strtok_buf = NULL, **val_list;
	zval *array_ptr = (zval *) arg;
	int n, num, *len_list, *elist, elistsz;
	enum mbfl_no_encoding from_encoding, to_encoding;
	mbfl_string string, result, *ret;
	mbfl_encoding_detector *identd;
	mbfl_buffer_converter *convd;

	mbfl_string_init(&string);
	mbfl_string_init(&result);
	string.no_language = MBSTRG(current_language);
	string.no_encoding = MBSTRG(current_internal_encoding);
	
	/* count the variables contained in the query */
	num = 0;
	var = res;
	n = strlen(res);
	while(n > 0) {
		if (*var == '=') {
			num++;
		}
		var++;
		n--;
	}
	num *= 2;
	val_list = (char **)ecalloc(num, sizeof(char *));
	len_list = (int *)ecalloc(num, sizeof(int));

	/* split and decode the query */
	n = 0;
	strtok_buf = NULL;
	var = php_strtok_r(res, separator, &strtok_buf);
	
	while (var && n < num) {
		val = strchr(var, '=');
		if (val) { /* have a value */
			*val++ = '\0';
			val_list[n] = var;
			len_list[n] = php_url_decode(var, strlen(var));
			n++;
			val_list[n] = val;
			len_list[n] = php_url_decode(val, strlen(val));
		} else {
			val_list[n] = var;
			len_list[n] = php_url_decode(var, strlen(var));
			n++;
			val_list[n] = NULL;
			len_list[n] = 0;
		}
		n++;
		var = php_strtok_r(NULL, separator, &strtok_buf);
	}
	num = n;

	/* initialize converter */
	to_encoding = MBSTRG(current_internal_encoding);
	elist = MBSTRG(http_input_list);
	elistsz = MBSTRG(http_input_list_size);
	if (elistsz <= 0) {
		from_encoding = mbfl_no_encoding_pass;
	} else if (elistsz == 1) {
		from_encoding = *elist;
	} else {
		/* auto detect */
		from_encoding = mbfl_no_encoding_invalid;
		identd = mbfl_encoding_detector_new(elist, elistsz);
		if (identd) {
			n = 0;
			while (n < num) {
				string.val = val_list[n];
				string.len = len_list[n];
				if (mbfl_encoding_detector_feed(identd, &string)) {
					break;
				}
				n++;
			}
			from_encoding = mbfl_encoding_detector_judge(identd);
			mbfl_encoding_detector_delete(identd);
		}
		if (from_encoding == mbfl_no_encoding_invalid) {
			from_encoding = mbfl_no_encoding_pass;
		}
	}
	convd = NULL;
	if (from_encoding != mbfl_no_encoding_pass) {
		convd = mbfl_buffer_converter_new(from_encoding, to_encoding, 0);
		if (convd != NULL) {
			mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode));
			mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar));
		} else {
			php_error(E_WARNING, "Unable to create converter in php_mb_encoding_handler()");
		}
	}

	/* convert encoding */
	string.no_encoding = from_encoding;
	n = 0;
	while (n < num) {
		string.val = val_list[n+1];
		string.len = len_list[n+1];
		ret = NULL;
		if (convd) {
			ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
		}
		if (ret != NULL) {
			php_register_variable_safe(val_list[n], ret->val, ret->len, array_ptr TSRMLS_CC);
			efree(ret->val);
		} else {
			php_register_variable_safe(val_list[n], val_list[n+1], len_list[n+1], array_ptr TSRMLS_CC);
		}
		n+=2;
	}
	MBSTRG(http_input_identify) = from_encoding;

	if (convd != NULL) {
		mbfl_buffer_converter_delete(convd);
	}
	if (val_list != NULL) {
		efree((void *)val_list);
	}
	if (len_list != NULL) {
		efree((void *)len_list);
	}

}

#if defined(MBSTR_ENC_TRANS)
SAPI_POST_HANDLER_FUNC(php_mbstr_post_handler)
{
	MBSTRG(http_input_identify_post) = mbfl_no_encoding_invalid;

	php_mbstr_encoding_handler(arg, SG(request_info).post_data, "&" TSRMLS_CC);

	if (MBSTRG(http_input_identify) != mbfl_no_encoding_invalid) {
		MBSTRG(http_input_identify_post) = MBSTRG(http_input_identify);
	}
}
#endif

/* http input processing */
void mbstr_treat_data(int arg, char *str, zval* destArray TSRMLS_DC)
{
	char *res = NULL, *separator=NULL;
	const char *c_var;
	pval *array_ptr;
	int free_buffer=0;

	switch (arg) {
		case PARSE_POST:
		case PARSE_GET:
		case PARSE_COOKIE:
			ALLOC_ZVAL(array_ptr);
			array_init(array_ptr);
			INIT_PZVAL(array_ptr);
			switch (arg) {
				case PARSE_POST:
					PG(http_globals)[TRACK_VARS_POST] = array_ptr;
					break;
				case PARSE_GET:
					PG(http_globals)[TRACK_VARS_GET] = array_ptr;
					break;
				case PARSE_COOKIE:
					PG(http_globals)[TRACK_VARS_COOKIE] = array_ptr;
					break;
			}
			break;
		default:
			array_ptr=destArray;
			break;
	}

	if (arg==PARSE_POST) { 
		sapi_handle_post(array_ptr TSRMLS_CC);
		return;
	}

	if (arg == PARSE_GET) {		/* GET data */
		c_var = SG(request_info).query_string;
		if (c_var && *c_var) {
			res = (char *) estrdup(c_var);
			free_buffer = 1;
		} else {
			free_buffer = 0;
		}
	} else if (arg == PARSE_COOKIE) {		/* Cookie data */
		c_var = SG(request_info).cookie_data;
		if (c_var && *c_var) {
			res = (char *) estrdup(c_var);
			free_buffer = 1;
		} else {
			free_buffer = 0;
		}
	} else if (arg == PARSE_STRING) {		/* String data */
		res = str;
		free_buffer = 1;
	}

	if (!res) {
		return;
	}

	switch (arg) {
	case PARSE_POST:
	case PARSE_GET:
	case PARSE_STRING:
		separator = (char *) estrdup(PG(arg_separator).input);
		break;
	case PARSE_COOKIE:
		separator = ";\0";
		break;
	}
	
	switch(arg) {
	case PARSE_POST:
		MBSTRG(http_input_identify_post) = mbfl_no_encoding_invalid;
		break;
	case PARSE_GET:
		MBSTRG(http_input_identify_get) = mbfl_no_encoding_invalid;
		break;
	case PARSE_COOKIE:
		MBSTRG(http_input_identify_cookie) = mbfl_no_encoding_invalid;
		break;
	case PARSE_STRING:
		MBSTRG(http_input_identify_string) = mbfl_no_encoding_invalid;
		break;
	}

	php_mbstr_encoding_handler(array_ptr, res, separator TSRMLS_CC);

	if (MBSTRG(http_input_identify) != mbfl_no_encoding_invalid) {
		switch(arg){
		case PARSE_POST:
			MBSTRG(http_input_identify_post) = MBSTRG(http_input_identify);
			break;
		case PARSE_GET:
			MBSTRG(http_input_identify_get) = MBSTRG(http_input_identify);
			break;
		case PARSE_COOKIE:
			MBSTRG(http_input_identify_cookie) = MBSTRG(http_input_identify);
			break;
		case PARSE_STRING:
			MBSTRG(http_input_identify_string) = MBSTRG(http_input_identify);
			break;
		}
	}

	if(arg != PARSE_COOKIE) {
		efree(separator);
	}

	if (free_buffer) {
		efree(res);
	}
}
#endif

/* {{{ proto bool mb_parse_str(string encoded_string, [array result])
   Parses GET/POST/COOKIE data and sets global variables */
PHP_FUNCTION(mb_parse_str)
{
	pval **arg_str, **arg_array, *track_vars_array;
	char *var, *val, *encstr, *strtok_buf, **str_list, *separator;
	int n, num, val_len, *len_list, *elist, elistsz, old_rg, argc;
	enum mbfl_no_encoding from_encoding, to_encoding;
	mbfl_string string, resvar, resval;
	mbfl_encoding_detector *identd;
	mbfl_buffer_converter *convd;

	track_vars_array = NULL;
	argc = ZEND_NUM_ARGS();
	if (argc == 1) {
		if (zend_get_parameters_ex(1, &arg_str) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (argc == 2) {
		if (zend_get_parameters_ex(2, &arg_str, &arg_array) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		/* Clear out the array */
		zval_dtor(*arg_array);
		array_init(*arg_array);
		track_vars_array = *arg_array;
	} else {
		WRONG_PARAM_COUNT;
	}
	separator = (char *)estrdup(PG(arg_separator).input);
	if (separator == NULL) {
		RETURN_FALSE;
	}
	convert_to_string_ex(arg_str);
	encstr = estrndup(Z_STRVAL_PP(arg_str), Z_STRLEN_PP(arg_str));
	if (encstr == NULL) {
		efree((void *)separator);
		RETURN_FALSE;
	}
	mbfl_string_init_set(&string, MBSTRG(current_language), MBSTRG(current_internal_encoding));
	mbfl_string_init_set(&resvar, MBSTRG(current_language), MBSTRG(current_internal_encoding));
	mbfl_string_init_set(&resval, MBSTRG(current_language), MBSTRG(current_internal_encoding));

	/* count the variables contained in the query */
	num = 1;
	var = encstr;
	n = Z_STRLEN_PP(arg_str);
	while (n > 0) {
		if (*var == *separator) {
			num++;
		}
		var++;
		n--;
	}
	num *= 2;
	str_list = (char **)ecalloc(num, sizeof(char *));
	if (str_list == NULL) {
		efree((void *)separator);
		efree((void *)encstr);
		RETURN_FALSE;
	}
	len_list = (int *)ecalloc(num, sizeof(int));
	if (len_list == NULL) {
		efree((void *)separator);
		efree((void *)encstr);
		efree((void *)str_list);
		RETURN_FALSE;
	}

	/* split and decode the query */
	n = 0;
	strtok_buf = NULL;
	var = php_strtok_r(encstr, separator, &strtok_buf);
	while (var && n < num) {
		val = strchr(var, '=');
		if (val) { /* have a value */
			*val++ = '\0';
			str_list[n] = var;
			len_list[n] = php_url_decode(var, strlen(var));
			n++;
			str_list[n] = val;
			len_list[n] = php_url_decode(val, strlen(val));
		} else {
			str_list[n] = var;
			len_list[n] = php_url_decode(var, strlen(var));
			n++;
			str_list[n] = NULL;
			len_list[n] = 0;
		}
		n++;
		var = php_strtok_r(NULL, separator, &strtok_buf);
	}
	num = n;

	/* initialize converter */
	to_encoding = MBSTRG(current_internal_encoding);
	elist = MBSTRG(http_input_list);
	elistsz = MBSTRG(http_input_list_size);
	if (elistsz <= 0) {
		from_encoding = mbfl_no_encoding_pass;
	} else if (elistsz == 1) {
		from_encoding = *elist;
	} else {
		/* auto detect */
		from_encoding = mbfl_no_encoding_invalid;
		identd = mbfl_encoding_detector_new(elist, elistsz);
		if (identd != NULL) {
			n = 0;
			while (n < num) {
				string.val = str_list[n];
				string.len = len_list[n];
				if (mbfl_encoding_detector_feed(identd, &string)) {
					break;
				}
				n++;
			}
			from_encoding = mbfl_encoding_detector_judge(identd);
			mbfl_encoding_detector_delete(identd);
		}
		if (from_encoding == mbfl_no_encoding_invalid) {
			php_error(E_WARNING, "Unable to detect encoding in mb_parse_str()");
			from_encoding = mbfl_no_encoding_pass;
		}
	}
	convd = NULL;
	if (from_encoding != mbfl_no_encoding_pass) {
		convd = mbfl_buffer_converter_new(from_encoding, to_encoding, 0);
		if (convd != NULL) {
			mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode));
			mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar));
		} else {
			php_error(E_WARNING, "Unable to create converter in mb_parse_str()");
		}
	}

	/* convert encoding */
	string.no_encoding = from_encoding;
	old_rg = PG(register_globals);
	if(argc == 1) {
		PG(register_globals) = 1;
	} else {
		PG(register_globals) = 0;
	}
	n = 0;
	while (n < num) {
		/* convert variable name */
		string.val = str_list[n];
		string.len = len_list[n];
		if (convd != NULL && mbfl_buffer_converter_feed_result(convd, &string, &resvar) != NULL) {
			var = resvar.val;
		} else {
			var = str_list[n];
		}
		n++;
		/* convert value */
		string.val = str_list[n];
		string.len = len_list[n];
		if (convd != NULL && mbfl_buffer_converter_feed_result(convd, &string, &resval) != NULL) {
			val = resval.val;
			val_len = resval.len;
		} else {
			val = str_list[n];
			val_len = len_list[n];
		}
		n++;
		/* add variable to symbol table */
		php_register_variable_safe(var, val, val_len, track_vars_array TSRMLS_CC);
		mbfl_string_clear(&resvar);
		mbfl_string_clear(&resval);
	}
	PG(register_globals) = old_rg;

	if (convd != NULL) {
		mbfl_buffer_converter_delete(convd);
	}
	efree((void *)str_list);
	efree((void *)len_list);
	efree((void *)encstr);
	efree((void *)separator);
	MBSTRG(http_input_identify) = from_encoding;
	MBSTRG(http_input_identify_string) = from_encoding;
	RETURN_TRUE;
}
/* }}} */



/* {{{ proto string mb_output_handler(string contents, int status)
   Returns string in output buffer converted to the http_output encoding */
PHP_FUNCTION(mb_output_handler)
{
	pval **arg_string, **arg_status;
	mbfl_string string, result, *ret;
	const char *mimetype, *charset;
	mbfl_memory_device device;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg_string, &arg_status) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg_string);
	convert_to_long_ex(arg_status);

	if ( SG(sapi_headers).send_default_content_type && ! SG(headers_sent) &&
	     MBSTRG(current_http_output_encoding) != mbfl_no_encoding_pass &&
	     (Z_LVAL_PP(arg_status) & PHP_OUTPUT_HANDLER_START) != 0) {
		mimetype = SG(default_mimetype) ? SG(default_mimetype) : SAPI_DEFAULT_MIMETYPE;
		charset = mbfl_no2preferred_mime_name(MBSTRG(current_http_output_encoding));
		if ( charset != NULL && (*mimetype == '\0' || strncasecmp(mimetype, "text/", 5) == 0) ) {
			mbfl_memory_device_init(&device, 0, 0);
			mbfl_memory_device_strcat(&device, "Content-Type: ");
			if (*mimetype == '\0') {
				mbfl_memory_device_strcat(&device, "text/html");
			} else {
				mbfl_memory_device_strcat(&device, mimetype);
			}
			mbfl_memory_device_strcat(&device, ";charset=");
			mbfl_memory_device_strcat(&device, charset);
			ret = mbfl_memory_device_result(&device, &result);
			if (ret != NULL) {
				if (sapi_add_header(ret->val, ret->len, 0) != FAILURE) {
					SG(sapi_headers).send_default_content_type = 0;
				}
			}
		}
	}

	ret = NULL;
	if (MBSTRG(current_http_output_encoding) != mbfl_no_encoding_pass && MBSTRG(outconv) == NULL) {
		MBSTRG(outconv) = mbfl_buffer_converter_new(MBSTRG(current_internal_encoding), MBSTRG(current_http_output_encoding), 0);
	}
	if (MBSTRG(current_http_output_encoding) != mbfl_no_encoding_pass && MBSTRG(outconv) != NULL) {
		mbfl_buffer_converter_illegal_mode(MBSTRG(outconv), MBSTRG(current_filter_illegal_mode));
		mbfl_buffer_converter_illegal_substchar(MBSTRG(outconv), MBSTRG(current_filter_illegal_substchar));
		mbfl_string_init(&string);
		string.no_language = MBSTRG(current_language);
		string.no_encoding = MBSTRG(current_internal_encoding);
		string.val = Z_STRVAL_PP(arg_string);
		string.len = Z_STRLEN_PP(arg_string);
		if ((Z_LVAL_PP(arg_status) & PHP_OUTPUT_HANDLER_END) != 0) {
			mbfl_buffer_converter_feed(MBSTRG(outconv), &string);
			mbfl_buffer_converter_flush(MBSTRG(outconv));
			ret = mbfl_buffer_converter_result(MBSTRG(outconv), &result);
		} else {
			mbfl_buffer_converter_feed(MBSTRG(outconv), &string);
			ret = mbfl_buffer_converter_result(MBSTRG(outconv), &result);
		}
	}

	if (ret != NULL) {
		RETVAL_STRINGL(ret->val, ret->len, 0);		/* the string is already strdup()'ed */
	} else {
		zval_dtor(return_value);
		*return_value = **arg_string;
		zval_copy_ctor(return_value);
	}
	if ((Z_LVAL_PP(arg_status) & PHP_OUTPUT_HANDLER_END) != 0) {
		mbfl_buffer_converter_delete(MBSTRG(outconv));
		MBSTRG(outconv) = NULL;
	}
}
/* }}} */



/* {{{ proto int mb_strlen(string str, [string encoding])
   Get character numbers of a string */
PHP_FUNCTION(mb_strlen)
{
	pval **arg1, **arg2;
	int n;
	mbfl_string string;

	n = ZEND_NUM_ARGS();
	if ((n == 1 && zend_get_parameters_ex(1, &arg1) == FAILURE) ||
	   (n == 2 && zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) ||
	    n < 1 || n > 2) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	mbfl_string_init(&string);
	string.no_language = MBSTRG(current_language);
	string.no_encoding = MBSTRG(current_internal_encoding);
	string.val = Z_STRVAL_PP(arg1);
	string.len = Z_STRLEN_PP(arg1);

	if (n == 2) {
		convert_to_string_ex(arg2);
		string.no_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(arg2));
		if(string.no_encoding == mbfl_no_encoding_invalid) {
			php_error(E_WARNING, "unknown encoding \"%s\"", Z_STRVAL_PP(arg2));
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
	pval **arg1, **arg2, **arg3, **arg4;
	int offset, n;
	mbfl_string haystack, needle;

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(current_language);
	haystack.no_encoding = MBSTRG(current_internal_encoding);
	needle.no_language = MBSTRG(current_language);
	needle.no_encoding = MBSTRG(current_internal_encoding);
	offset = 0;
	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 3:
		if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(arg3);
		offset = Z_LVAL_PP(arg3);
		break;
	case 4:
		if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(arg3);
		offset = Z_LVAL_PP(arg3);
		convert_to_string_ex(arg4);
		haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(arg4));
		if(haystack.no_encoding == mbfl_no_encoding_invalid) {
			php_error(E_WARNING, "unknown encoding \"%s\"", Z_STRVAL_PP(arg4));
			RETURN_FALSE;
		}
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	convert_to_string_ex(arg2);
	if (offset < 0) {
		php_error(E_WARNING,"offset is minus value");
		offset = 0;
	}
	if (offset > Z_STRLEN_PP(arg1)) {
		php_error(E_WARNING,"offset not contained in string");
		RETURN_FALSE;
	}
	if (Z_STRLEN_PP(arg2) == 0) {
		php_error(E_WARNING,"Empty needle");
		RETURN_FALSE;
	}
	haystack.val = Z_STRVAL_PP(arg1);
	haystack.len = Z_STRLEN_PP(arg1);
	needle.val = Z_STRVAL_PP(arg2);
	needle.len = Z_STRLEN_PP(arg2);

	n = mbfl_strpos(&haystack, &needle, offset, 0);
	if (n >= 0) {
		RETVAL_LONG(n);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */



/* {{{ proto int mb_strrpos(string haystack, string needle [, string encoding])
   Find the last occurrence of a character in a string within another */
PHP_FUNCTION(mb_strrpos)
{
	pval **arg1, **arg2, **arg3;
	int n;
	mbfl_string haystack, needle;

	mbfl_string_init(&haystack);
	mbfl_string_init(&needle);
	haystack.no_language = MBSTRG(current_language);
	haystack.no_encoding = MBSTRG(current_internal_encoding);
	needle.no_language = MBSTRG(current_language);
	needle.no_encoding = MBSTRG(current_internal_encoding);
	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 3:
		if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(arg3);
		haystack.no_encoding = needle.no_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(arg3));
		if(haystack.no_encoding == mbfl_no_encoding_invalid) {
			php_error(E_WARNING, "unknown encoding \"%s\"", Z_STRVAL_PP(arg3));
			RETURN_FALSE;
		}
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	convert_to_string_ex(arg2);
	if (Z_STRLEN_PP(arg1) <= 0) {
		php_error(E_WARNING,"Empty haystack");
		RETURN_FALSE;
	}
	if (Z_STRLEN_PP(arg2) <= 0) {
		php_error(E_WARNING,"Empty needle");
		RETURN_FALSE;
	}
	haystack.val = Z_STRVAL_PP(arg1);
	haystack.len = Z_STRLEN_PP(arg1);
	needle.val = Z_STRVAL_PP(arg2);
	needle.len = Z_STRLEN_PP(arg2);
	n = mbfl_strpos(&haystack, &needle, 0, 1);
	if (n >= 0) {
		RETVAL_LONG(n);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */



/* {{{ proto string mb_substr(string str, int start [, int length [, string encoding]])
   Return part of a string */
PHP_FUNCTION(mb_substr)
{
	pval **arg1, **arg2, **arg3, **arg4;
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
			php_error(E_WARNING, "unknown encoding \"%s\"", Z_STRVAL_PP(arg4));
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

	ret = mbfl_substr(&string, &result, from, len);
	if (ret != NULL) {
		RETVAL_STRINGL(ret->val, ret->len, 0);		/* the string is already strdup()'ed */
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */



/* {{{ proto string mb_strcut(string str, int start [, int length [, string encoding]])
   Return part of a string */
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
			php_error(E_WARNING, "unknown encoding \"%s\"", Z_STRVAL_PP(arg4));
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

	ret = mbfl_strcut(&string, &result, from, len);
	if (ret != NULL) {
		RETVAL_STRINGL(ret->val, ret->len, 0);		/* the string is already strdup()'ed */
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */



/* {{{ proto int mb_strwidth(string str, [string encoding])
   Get terminal width of a string */
PHP_FUNCTION(mb_strwidth)
{
	pval **arg1, **arg2;
	int n;
	mbfl_string string;

	n = ZEND_NUM_ARGS();
	if ((n == 1 && zend_get_parameters_ex(1, &arg1) == FAILURE) ||
	   (n == 2 && zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) ||
	    n < 1 || n > 2) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	mbfl_string_init(&string);
	string.no_language = MBSTRG(current_language);
	string.no_encoding = MBSTRG(current_internal_encoding);
	string.val = Z_STRVAL_PP(arg1);
	string.len = Z_STRLEN_PP(arg1);

	if (n == 2){
		convert_to_string_ex(arg2);
		string.no_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(arg2));
		if(string.no_encoding == mbfl_no_encoding_invalid) {
			php_error(E_WARNING, "unknown encoding \"%s\"", Z_STRVAL_PP(arg2));
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
			php_error(E_WARNING, "unknown encoding \"%s\"", Z_STRVAL_PP(arg5));
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
	convert_to_long_ex(arg3);
	width = Z_LVAL_PP(arg3);

	if (ZEND_NUM_ARGS() >= 4) {
		convert_to_string_ex(arg4);
		marker.val = Z_STRVAL_PP(arg4);
		marker.len = Z_STRLEN_PP(arg4);
	}

	ret = mbfl_strimwidth(&string, &marker, &result, from, width);
	if (ret != NULL) {
		RETVAL_STRINGL(ret->val, ret->len, 0);		/* the string is already strdup()'ed */
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */



/* {{{ proto string mb_convert_encoding(string str, string to-encoding [, mixed from-encoding])
   Returns converted string in desired encoding */
PHP_FUNCTION(mb_convert_encoding)
{
	pval **arg_str, **arg_new, **arg_old;
	mbfl_string string, result, *ret;
	enum mbfl_no_encoding from_encoding, to_encoding;
	mbfl_buffer_converter *convd;
	int size, *list;

	if (ZEND_NUM_ARGS() == 2) {
		if (zend_get_parameters_ex(2, &arg_str, &arg_new) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else if (ZEND_NUM_ARGS() == 3) {
		if (zend_get_parameters_ex(3, &arg_str, &arg_new, &arg_old) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else {
		WRONG_PARAM_COUNT;
	}

	/* new encoding */
	convert_to_string_ex(arg_new);
	to_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(arg_new));
	if (to_encoding == mbfl_no_encoding_invalid) {
		php_error(E_WARNING, "unknown encoding \"%s\"", Z_STRVAL_PP(arg_new));
		RETURN_FALSE;
	}

	/* initialize string */
	convert_to_string_ex(arg_str);
	mbfl_string_init(&string);
	mbfl_string_init(&result);
	from_encoding = MBSTRG(current_internal_encoding);
	string.no_encoding = from_encoding;
	string.no_language = MBSTRG(current_language);
	string.val = Z_STRVAL_PP(arg_str);
	string.len = Z_STRLEN_PP(arg_str);

	/* pre-conversion encoding */
	if (ZEND_NUM_ARGS() >= 3) {
		list = NULL;
		size = 0;
		switch (Z_TYPE_PP(arg_old)) {
		case IS_ARRAY:
			php_mbstring_parse_encoding_array(*arg_old, &list, &size, 0);
			break;
		default:
			convert_to_string_ex(arg_old);
			php_mbstring_parse_encoding_list(Z_STRVAL_PP(arg_old), Z_STRLEN_PP(arg_old), &list, &size, 0);
			break;
		}
		if (size == 1) {
			from_encoding = *list;
		} else if (size > 1) {
			/* auto detect */
			from_encoding = mbfl_identify_encoding_no(&string, list, size);
			if (from_encoding != mbfl_no_encoding_invalid) {
				string.no_encoding = from_encoding;
			} else {
				php_error(E_WARNING, "Unable to detect encoding in mb_convert_encoding()");
				from_encoding = mbfl_no_encoding_pass;
				to_encoding = from_encoding;
				string.no_encoding = from_encoding;
			}
		} else {
			php_error(E_WARNING, "illegal from-encoding in mb_convert_encoding()");
		}
		if (list != NULL) {
			efree((void *)list);
		}
	}

	/* initialize converter */
	convd = mbfl_buffer_converter_new(from_encoding, to_encoding, string.len);
	if (convd == NULL) {
		php_error(E_WARNING, "Unable to create converter in mb_convert_encoding()");
		RETURN_FALSE;
	}
	mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode));
	mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar));

	/* do it */
	ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
	if (ret != NULL) {
		RETVAL_STRINGL(ret->val, ret->len, 0);		/* the string is already strdup()'ed */
	} else {
		RETVAL_FALSE;
	}

	mbfl_buffer_converter_delete(convd);
}
/* }}} */



/* {{{ proto string mb_detect_encoding(string str [, mixed encoding_list])
    Encoding of the given string is returned (as a string) */
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
			php_mbstring_parse_encoding_array(*arg_list, &list, &size, 0);
			break;
		default:
			convert_to_string_ex(arg_list);
			php_mbstring_parse_encoding_list(Z_STRVAL_PP(arg_list), Z_STRLEN_PP(arg_list), &list, &size, 0);
			break;
		}
		if (size <= 0) {
			php_error(E_WARNING, "illegal argument");
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
	string.val = Z_STRVAL_PP(arg_str);
	string.len = Z_STRLEN_PP(arg_str);
	ret = mbfl_identify_encoding_name(&string, elist, size);
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
    Convert the string to MIME "encoded-word" in the format of =?charset?(B|Q)?encoded_string?= */
PHP_FUNCTION(mb_encode_mimeheader)
{
	pval **argv[4];
	enum mbfl_no_encoding charset, transenc;
	mbfl_string  string, result, *ret;
	char *p, *linefeed;

	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 4 || zend_get_parameters_array_ex(ZEND_NUM_ARGS(), argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	charset = mbfl_no_encoding_pass;
	transenc = mbfl_no_encoding_base64;
	if (ZEND_NUM_ARGS() >= 2) {
		convert_to_string_ex(argv[1]);
		charset = mbfl_name2no_encoding(Z_STRVAL_PP(argv[1]));
		if (charset == mbfl_no_encoding_invalid) {
			php_error(E_WARNING, "unknown encoding \"%s\"", Z_STRVAL_PP(argv[1]));
			RETURN_FALSE;
		}
	} else {
		switch (MBSTRG(current_language)) {
		case mbfl_no_language_japanese:
			charset = mbfl_no_encoding_2022jp;
			break;
		case mbfl_no_language_english:
			charset = mbfl_no_encoding_8859_1;
			transenc = mbfl_no_encoding_qprint;
			break;
		default:
			charset = mbfl_no_encoding_utf8;
			break;
		}
	}

	if (ZEND_NUM_ARGS() >= 3) {
		convert_to_string_ex(argv[2]);
		p = Z_STRVAL_PP(argv[2]);
		if (*p == 'B' || *p == 'b') {
			transenc = mbfl_no_encoding_base64;
		} else if (*p == 'Q' || *p == 'q') {
			transenc = mbfl_no_encoding_qprint;
		}
	}

	linefeed = "\r\n";
	if (ZEND_NUM_ARGS() >= 4) {
		convert_to_string_ex(argv[3]);
		linefeed = Z_STRVAL_PP(argv[3]);
	}

	convert_to_string_ex(argv[0]);
	mbfl_string_init(&string);
	mbfl_string_init(&result);
	string.no_language = MBSTRG(current_language);
	string.no_encoding = MBSTRG(current_internal_encoding);
	string.val = Z_STRVAL_PP(argv[0]);
	string.len = Z_STRLEN_PP(argv[0]);
	ret = mbfl_mime_header_encode(&string, &result, charset, transenc, linefeed, 0);
	if (ret != NULL) {
		RETVAL_STRINGL(ret->val, ret->len, 0)	/* the string is already strdup()'ed */
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */



/* {{{ proto string mb_decode_mimeheader(string string)
    Decodes the MIME "encoded-word" in the string */
PHP_FUNCTION(mb_decode_mimeheader)
{
	pval **arg_str;
	mbfl_string string, result, *ret;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg_str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg_str);
	mbfl_string_init(&string);
	mbfl_string_init(&result);
	string.no_language = MBSTRG(current_language);
	string.no_encoding = MBSTRG(current_internal_encoding);
	string.val = Z_STRVAL_PP(arg_str);
	string.len = Z_STRLEN_PP(arg_str);
	ret = mbfl_mime_header_decode(&string, &result, MBSTRG(current_internal_encoding));
	if (ret != NULL) {
		RETVAL_STRINGL(ret->val, ret->len, 0)	/* the string is already strdup()'ed */
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */



/* {{{ proto string mb_convert_kana(string str [,string option] [, string encoding])
   Conversion between full-width character and half-width character (Japanese) */
PHP_FUNCTION(mb_convert_kana)
{
	pval **arg1, **arg2, **arg3;
	int argc, opt, i, n;
	char *p;
	mbfl_string string, result, *ret;
	enum mbfl_no_encoding no_encoding;

	mbfl_string_init(&string);
	string.no_language = MBSTRG(current_language);
	string.no_encoding = MBSTRG(current_internal_encoding);

	argc = ZEND_NUM_ARGS();
	if ((argc == 1 && zend_get_parameters_ex(1, &arg1) == FAILURE) ||
	   (argc == 2 && zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) ||
	   (argc == 3 && zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) ||
		argc < 1 || argc > 3) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	string.val = Z_STRVAL_PP(arg1);
	string.len = Z_STRLEN_PP(arg1);

	/* option */
	if (argc >= 2){
		convert_to_string_ex(arg2);
		p = Z_STRVAL_PP(arg2);
		n = Z_STRLEN_PP(arg2);
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
	if (argc == 3) {
		convert_to_string_ex(arg3);
		no_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(arg3));
		if (no_encoding == mbfl_no_encoding_invalid) {
			php_error(E_WARNING, "unknown encoding \"%s\"", Z_STRVAL_PP(arg3));
			RETURN_FALSE;
		} else {
			string.no_encoding = no_encoding;
		}
	}

	ret = mbfl_ja_jp_hantozen(&string, &result, opt);
	if (ret != NULL) {
		RETVAL_STRINGL(ret->val, ret->len, 0);		/* the string is already strdup()'ed */
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
	if (args == NULL) {
		RETURN_FALSE;
	}
	if (zend_get_parameters_array_ex(argc, args) == FAILURE) {
		efree((void *)args);
		WRONG_PARAM_COUNT;
	}

	/* new encoding */
	convert_to_string_ex(args[0]);
	to_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(args[0]));
	if (to_encoding == mbfl_no_encoding_invalid) {
		php_error(E_WARNING, "unknown encoding \"%s\"", Z_STRVAL_PP(args[0]));
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
		php_mbstring_parse_encoding_array(*args[1], &elist, &elistsz, 0);
		break;
	default:
		convert_to_string_ex(args[1]);
		php_mbstring_parse_encoding_list(Z_STRVAL_PP(args[1]), Z_STRLEN_PP(args[1]), &elist, &elistsz, 0);
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
		if (stack != NULL) {
			stack_level = 0;
			identd = mbfl_encoding_detector_new(elist, elistsz);
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
										if (ptmp == NULL) {
											php_error(E_WARNING, "stack err at %s:(%d)", __FILE__, __LINE__);
											continue;
										}
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
									string.val = Z_STRVAL_PP(hash_entry);
									string.len = Z_STRLEN_PP(hash_entry);
									if (mbfl_encoding_detector_feed(identd, &string)) {
										goto detect_end;		/* complete detecting */
									}
								}
							}
						}
					} else if (Z_TYPE_PP(var) == IS_STRING) {
						string.val = Z_STRVAL_PP(args[n]);
						string.len = Z_STRLEN_PP(args[n]);
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
		}
		if (from_encoding == mbfl_no_encoding_invalid) {
			php_error(E_WARNING, "Unable to detect encoding in mb_convert_variables()");
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
			php_error(E_WARNING, "Unable to create converter in mb_convert_variables()");
			RETURN_FALSE;
		}
		mbfl_buffer_converter_illegal_mode(convd, MBSTRG(current_filter_illegal_mode));
		mbfl_buffer_converter_illegal_substchar(convd, MBSTRG(current_filter_illegal_substchar));
	}

	/* convert */
	if (convd != NULL) {
		stack_max = PHP_MBSTR_STACK_BLOCK_SIZE;
		stack = (pval ***)emalloc(stack_max*sizeof(pval **));
		if (stack != NULL) {
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
									if (ptmp == NULL) {
										php_error(E_WARNING, "stack err at %s:(%d)", __FILE__, __LINE__);
										continue;
									}
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
								string.val = Z_STRVAL_PP(hash_entry);
								string.len = Z_STRLEN_PP(hash_entry);
								ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
								if (ret != NULL) {
									STR_FREE(Z_STRVAL_PP(hash_entry));
									Z_STRVAL_PP(hash_entry) = ret->val;
									Z_STRLEN_PP(hash_entry) = ret->len;
								}
							}
						}
					}
				} else if (Z_TYPE_PP(var) == IS_STRING) {
					string.val = Z_STRVAL_PP(var);
					string.len = Z_STRLEN_PP(var);
					ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
					if (ret != NULL) {
						STR_FREE(Z_STRVAL_PP(var));
						Z_STRVAL_PP(var) = ret->val;
						Z_STRLEN_PP(var) = ret->len;
					}
				}
			}
			efree(stack);
		}
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


/* HTML numeric entity */
static void
php_mbstr_numericentity_exec(INTERNAL_FUNCTION_PARAMETERS, int type)
{
	pval **arg1, **arg2, **arg3, **hash_entry;
	HashTable *target_hash;
	int argc, i, *convmap, *mapelm, mapsize;
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
	string.val = Z_STRVAL_PP(arg1);
	string.len = Z_STRLEN_PP(arg1);

	/* encoding */
	if (argc == 3) {
		convert_to_string_ex(arg3);
		no_encoding = mbfl_name2no_encoding(Z_STRVAL_PP(arg3));
		if (no_encoding == mbfl_no_encoding_invalid) {
			php_error(E_WARNING, "unknown encoding \"%s\"", Z_STRVAL_PP(arg3));
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
			if (convmap != NULL) {
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
	}
	if (convmap == NULL) {
		RETURN_FALSE;
	}
	mapsize /= 4;

	ret = mbfl_html_numeric_entity(&string, &result, convmap, mapsize, type);
	if (ret != NULL) {
		RETVAL_STRINGL(ret->val, ret->len, 0);
	} else {
		RETVAL_FALSE;
	}
	efree((void *)convmap);
}

/* {{{ proto string mb_encode_numericentity(string string, array convmap [, string encoding])
   Convert specified characters to HTML numeric entities */
PHP_FUNCTION(mb_encode_numericentity)
{
	php_mbstr_numericentity_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */


/* {{{ proto string mb_decode_numericentity(string string, array convmap [, string encoding])
   Converts HTML numeric entities to character code */
PHP_FUNCTION(mb_decode_numericentity)
{
	php_mbstr_numericentity_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */



#if HAVE_SENDMAIL
/* {{{ proto int mb_send_mail(string to, string subject, string message [, string additional_headers [, string additional_parameters]])
   Sends an email message with MIME scheme */
PHP_FUNCTION(mb_send_mail)
{
	int argc, n;
	pval **argv[5];
	char *to=NULL, *message=NULL, *headers=NULL, *subject=NULL, *extra_cmd=NULL;
	char *message_buf=NULL, *subject_buf=NULL, *p;
	mbfl_string orig_str, conv_str;
	mbfl_string *pstr;	/* pointer to mbfl string for return value */
	enum mbfl_no_encoding
	    tran_cs,	/* transfar text charset */
	    head_enc,	/* header transfar encoding */
	    body_enc;	/* body transfar encoding */
	mbfl_memory_device device;	/* automatic allocateable buffer for additional header */
	int err = 0;

	/* initialize */
	mbfl_memory_device_init(&device, 0, 0);
	mbfl_string_init(&orig_str);
	mbfl_string_init(&conv_str);

	/* character-set, transfer-encoding */
	tran_cs = mbfl_no_encoding_utf8;
	head_enc = mbfl_no_encoding_base64;
	body_enc = mbfl_no_encoding_base64;
	switch (MBSTRG(current_language)) {
	case mbfl_no_language_japanese:
		tran_cs = mbfl_no_encoding_2022jp;
		body_enc = mbfl_no_encoding_7bit;
		break;
	case mbfl_no_language_english:
		tran_cs = mbfl_no_encoding_8859_1;
		head_enc = mbfl_no_encoding_qprint;
		body_enc = mbfl_no_encoding_qprint;
		break;
	}

	argc = ZEND_NUM_ARGS();
	if (argc < 3 || argc > 5 || zend_get_parameters_array_ex(argc, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* To: */
	convert_to_string_ex(argv[0]);
	if (Z_STRVAL_PP(argv[0])) {
		to = Z_STRVAL_PP(argv[0]);
	} else {
		php_error(E_WARNING, "No to field in mb_send_mail()");
		err = 1;
	}

	/* Subject: */
	convert_to_string_ex(argv[1]);
	if (Z_STRVAL_PP(argv[1])) {
		orig_str.no_language = MBSTRG(current_language);
		orig_str.val = Z_STRVAL_PP(argv[1]);
		orig_str.len = Z_STRLEN_PP(argv[1]);
		orig_str.no_encoding = mbfl_identify_encoding_no(&orig_str, MBSTRG(current_detect_order_list), MBSTRG(current_detect_order_list_size));
		if(orig_str.no_encoding == mbfl_no_encoding_invalid) {
			orig_str.no_encoding = MBSTRG(current_internal_encoding);
		}
		pstr = mbfl_mime_header_encode(&orig_str, &conv_str, tran_cs, head_enc, "\n", sizeof("Subject: [PHP-jp nnnnnnnn]"));
		if (pstr != NULL) {
			subject_buf = subject = pstr->val;
		} else {
			subject = Z_STRVAL_PP(argv[1]);
		}
	} else {
		php_error(E_WARNING, "No subject field in mb_send_mail()");
		err = 1;
	}

	/* message body */
	convert_to_string_ex(argv[2]);
	if (Z_STRVAL_PP(argv[2])) {
		orig_str.no_language = MBSTRG(current_language);
		orig_str.val = Z_STRVAL_PP(argv[2]);
		orig_str.len = Z_STRLEN_PP(argv[2]);
		orig_str.no_encoding = mbfl_identify_encoding_no(&orig_str, MBSTRG(current_detect_order_list), MBSTRG(current_detect_order_list_size));
		if(orig_str.no_encoding == mbfl_no_encoding_invalid) {
			orig_str.no_encoding = MBSTRG(current_internal_encoding);
		}
		pstr = mbfl_convert_encoding(&orig_str, &conv_str, tran_cs);
		if (pstr != NULL) {
			message_buf = message = pstr->val;
		} else {
			message = Z_STRVAL_PP(argv[2]);
		}
	} else {
		/* this is not really an error, so it is allowed. */
		php_error(E_WARNING, "No message string in mb_send_mail()");
		message = NULL;
	}

	/* other headers */
#define PHP_MBSTR_MAIL_MIME_HEADER1 "Mime-Version: 1.0\nContent-Type: text/plain"
#define PHP_MBSTR_MAIL_MIME_HEADER2 "; charset="
#define PHP_MBSTR_MAIL_MIME_HEADER3 "\nContent-Transfer-Encoding: "
	if (argc >= 4) {
		convert_to_string_ex(argv[3]);
		p = Z_STRVAL_PP(argv[3]);
		n = Z_STRLEN_PP(argv[3]);
		mbfl_memory_device_strncat(&device, p, n);
		if (p[n - 1] != '\n') {
			mbfl_memory_device_strncat(&device, "\n", 1);
		}
	}
	mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER1, sizeof(PHP_MBSTR_MAIL_MIME_HEADER1) - 1);
	p = (char *)mbfl_no2preferred_mime_name(tran_cs);
	if (p != NULL) {
		mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER2, sizeof(PHP_MBSTR_MAIL_MIME_HEADER2) - 1);
		mbfl_memory_device_strcat(&device, p);
	}
	mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER3, sizeof(PHP_MBSTR_MAIL_MIME_HEADER3) - 1);
	p = (char *)mbfl_no2preferred_mime_name(body_enc);
	if (p == NULL) {
		p = "7bit";
	}
	mbfl_memory_device_strcat(&device, p);
	mbfl_memory_device_output('\0', &device);
	headers = device.buffer;

	if (argc == 5) {	/* extra options that get passed to the mailer */
		convert_to_string_ex(argv[4]);
		extra_cmd = Z_STRVAL_PP(argv[4]);
	}

	if (!err && php_mail(to, subject, message, headers, extra_cmd)){
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
	mbfl_memory_device_clear(&device);
}
/* }}} */

#else	/* HAVE_SENDMAIL */

PHP_FUNCTION(mb_send_mail)
{
	RETURN_FALSE;
}

#endif	/* HAVE_SENDMAIL */

#endif	/* HAVE_MBSTRING */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
