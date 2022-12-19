/*
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
   | Author: Tsukada Takuya <tsukada@fminn.nagano.nagano.jp>              |
   |         Rui Hirokawa <hirokawa@php.net>                              |
   |         Hironori Sato <satoh@jpnnet.com>                             |
   |         Shigeru Kanemoto <sgk@happysize.co.jp>                       |
   +----------------------------------------------------------------------+
*/

/* {{{ includes */
#include "libmbfl/config.h"
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
#include "ext/pcre/php_pcre.h"

#include "libmbfl/mbfl/mbfilter_8bit.h"
#include "libmbfl/mbfl/mbfilter_pass.h"
#include "libmbfl/mbfl/mbfilter_wchar.h"
#include "libmbfl/mbfl/eaw_table.h"
#include "libmbfl/filters/mbfilter_base64.h"
#include "libmbfl/filters/mbfilter_qprint.h"
#include "libmbfl/filters/mbfilter_htmlent.h"
#include "libmbfl/filters/mbfilter_uuencode.h"
#include "libmbfl/filters/mbfilter_ucs4.h"
#include "libmbfl/filters/mbfilter_utf8.h"
#include "libmbfl/filters/mbfilter_singlebyte.h"
#include "libmbfl/filters/translit_kana_jisx0201_jisx0208.h"

#include "php_variables.h"
#include "php_globals.h"
#include "rfc1867.h"
#include "php_content_types.h"
#include "SAPI.h"
#include "php_unicode.h"
#include "TSRM.h"

#include "mb_gpc.h"

#ifdef HAVE_MBREGEX
# include "php_mbregex.h"
#endif

#include "zend_multibyte.h"
#include "mbstring_arginfo.h"
/* }}} */

/* {{{ prototypes */
ZEND_DECLARE_MODULE_GLOBALS(mbstring)

static PHP_GINIT_FUNCTION(mbstring);
static PHP_GSHUTDOWN_FUNCTION(mbstring);

static void php_mb_populate_current_detect_order_list(void);

static int php_mb_encoding_translation(void);

static void php_mb_gpc_get_detect_order(const zend_encoding ***list, size_t *list_size);

static void php_mb_gpc_set_input_encoding(const zend_encoding *encoding);

static inline bool php_mb_is_unsupported_no_encoding(enum mbfl_no_encoding no_enc);

static inline bool php_mb_is_no_encoding_utf8(enum mbfl_no_encoding no_enc);

static bool mb_check_str_encoding(zend_string *str, const mbfl_encoding *encoding);

/* See mbfilter_cp5022x.c */
uint32_t mb_convert_kana_codepoint(uint32_t c, uint32_t next, bool *consumed, uint32_t *second, int mode);
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

/* {{{ mbstring_deps[] */
static const zend_module_dep mbstring_deps[] = {
	ZEND_MOD_REQUIRED("pcre")
	ZEND_MOD_END
};
/* }}} */

/* {{{ zend_module_entry mbstring_module_entry */
zend_module_entry mbstring_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	mbstring_deps,
	"mbstring",
	ext_functions,
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

/* {{{ static sapi_post_entry mbstr_post_entries[] */
static const sapi_post_entry mbstr_post_entries[] = {
	{ DEFAULT_POST_CONTENT_TYPE, sizeof(DEFAULT_POST_CONTENT_TYPE)-1, sapi_read_standard_form_data, php_mb_post_handler },
	{ MULTIPART_CONTENT_TYPE,    sizeof(MULTIPART_CONTENT_TYPE)-1,    NULL,                         rfc1867_post_handler },
	{ NULL, 0, NULL, NULL }
};
/* }}} */

static const mbfl_encoding *php_mb_get_encoding(zend_string *encoding_name, uint32_t arg_num) {
	if (encoding_name) {
		const mbfl_encoding *encoding;
		zend_string *last_encoding_name = MBSTRG(last_used_encoding_name);
		if (last_encoding_name && (last_encoding_name == encoding_name
				|| zend_string_equals_ci(encoding_name, last_encoding_name))) {
			return MBSTRG(last_used_encoding);
		}

		encoding = mbfl_name2encoding(ZSTR_VAL(encoding_name));
		if (!encoding) {
			zend_argument_value_error(arg_num, "must be a valid encoding, \"%s\" given", ZSTR_VAL(encoding_name));
			return NULL;
		} else if (encoding->no_encoding <= mbfl_no_encoding_qprint) {
			if (encoding == &mbfl_encoding_base64) {
				php_error_docref(NULL, E_DEPRECATED, "Handling Base64 via mbstring is deprecated; use base64_encode/base64_decode instead");
			} else if (encoding == &mbfl_encoding_qprint) {
				php_error_docref(NULL, E_DEPRECATED, "Handling QPrint via mbstring is deprecated; use quoted_printable_encode/quoted_printable_decode instead");
			} else if (encoding == &mbfl_encoding_html_ent) {
				php_error_docref(NULL, E_DEPRECATED, "Handling HTML entities via mbstring is deprecated; use htmlspecialchars, htmlentities, or mb_encode_numericentity/mb_decode_numericentity instead");
			} else if (encoding == &mbfl_encoding_uuencode) {
				php_error_docref(NULL, E_DEPRECATED, "Handling Uuencode via mbstring is deprecated; use convert_uuencode/convert_uudecode instead");
			}
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

static const mbfl_encoding *php_mb_get_encoding_or_pass(const char *encoding_name) {
	if (strcmp(encoding_name, "pass") == 0) {
		return &mbfl_encoding_pass;
	}

	return mbfl_name2encoding(encoding_name);
}

static size_t count_commas(const char *p, const char *end) {
	size_t count = 0;
	while ((p = memchr(p, ',', end - p))) {
		count++;
		p++;
	}
	return count;
}

/* {{{ static zend_result php_mb_parse_encoding_list()
 *  Return FAILURE if input contains any illegal encoding, otherwise SUCCESS.
 * 	Emits a ValueError in function context and a warning in INI context, in INI context arg_num must be 0.
 */
static zend_result php_mb_parse_encoding_list(const char *value, size_t value_length,
	const mbfl_encoding ***return_list, size_t *return_size, bool persistent, uint32_t arg_num,
	bool allow_pass_encoding)
{
	if (value == NULL || value_length == 0) {
		*return_list = NULL;
		*return_size = 0;
		return SUCCESS;
	} else {
		bool included_auto;
		size_t n, size;
		char *p1, *endp, *tmpstr;
		const mbfl_encoding **entry, **list;

		/* copy the value string for work */
		if (value[0]=='"' && value[value_length-1]=='"' && value_length>2) {
			tmpstr = (char *)estrndup(value+1, value_length-2);
			value_length -= 2;
		} else {
			tmpstr = (char *)estrndup(value, value_length);
		}

		endp = tmpstr + value_length;
		size = 1 + count_commas(tmpstr, endp) + MBSTRG(default_detect_order_list_size);
		list = (const mbfl_encoding **)pecalloc(size, sizeof(mbfl_encoding*), persistent);
		entry = list;
		n = 0;
		included_auto = 0;
		p1 = tmpstr;
		while (1) {
			char *comma = memchr(p1, ',', endp - p1);
			char *p = comma ? comma : endp;
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
				if (!included_auto) {
					const enum mbfl_no_encoding *src = MBSTRG(default_detect_order_list);
					const size_t identify_list_size = MBSTRG(default_detect_order_list_size);
					size_t i;
					included_auto = 1;
					for (i = 0; i < identify_list_size; i++) {
						*entry++ = mbfl_no2encoding(*src++);
						n++;
					}
				}
			} else {
				const mbfl_encoding *encoding =
					allow_pass_encoding ? php_mb_get_encoding_or_pass(p1) : mbfl_name2encoding(p1);
				if (!encoding) {
					/* Called from an INI setting modification */
					if (arg_num == 0) {
						php_error_docref("ref.mbstring", E_WARNING, "INI setting contains invalid encoding \"%s\"", p1);
					} else {
						zend_argument_value_error(arg_num, "contains invalid encoding \"%s\"", p1);
					}
					efree(tmpstr);
					pefree(ZEND_VOIDP(list), persistent);
					return FAILURE;
				}

				*entry++ = encoding;
				n++;
			}
			if (n >= size || comma == NULL) {
				break;
			}
			p1 = comma + 1;
		}
		*return_list = list;
		*return_size = n;
		efree(tmpstr);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ static int php_mb_parse_encoding_array()
 *  Return FAILURE if input contains any illegal encoding, otherwise SUCCESS.
 * 	Emits a ValueError in function context and a warning in INI context, in INI context arg_num must be 0.
 */
static int php_mb_parse_encoding_array(HashTable *target_hash, const mbfl_encoding ***return_list,
	size_t *return_size, uint32_t arg_num)
{
	/* Allocate enough space to include the default detect order if "auto" is used. */
	size_t size = zend_hash_num_elements(target_hash) + MBSTRG(default_detect_order_list_size);
	const mbfl_encoding **list = ecalloc(size, sizeof(mbfl_encoding*));
	const mbfl_encoding **entry = list;
	bool included_auto = 0;
	size_t n = 0;
	zval *hash_entry;
	ZEND_HASH_FOREACH_VAL(target_hash, hash_entry) {
		zend_string *encoding_str = zval_try_get_string(hash_entry);
		if (UNEXPECTED(!encoding_str)) {
			efree(ZEND_VOIDP(list));
			return FAILURE;
		}

		if (zend_string_equals_literal_ci(encoding_str, "auto")) {
			if (!included_auto) {
				const enum mbfl_no_encoding *src = MBSTRG(default_detect_order_list);
				const size_t identify_list_size = MBSTRG(default_detect_order_list_size);
				size_t j;

				included_auto = 1;
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
				zend_argument_value_error(arg_num, "contains invalid encoding \"%s\"", ZSTR_VAL(encoding_str));
				zend_string_release(encoding_str);
				efree(ZEND_VOIDP(list));
				return FAILURE;
			}
		}
		zend_string_release(encoding_str);
	} ZEND_HASH_FOREACH_END();
	*return_list = list;
	*return_size = n;
	return SUCCESS;
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

static bool php_mb_zend_encoding_lexer_compatibility_checker(const zend_encoding *_encoding)
{
	const mbfl_encoding *encoding = (const mbfl_encoding*)_encoding;
	return !(encoding->flag & MBFL_ENCTYPE_GL_UNSAFE);
}

static const zend_encoding *php_mb_zend_encoding_detector(const unsigned char *arg_string, size_t arg_length, const zend_encoding **list, size_t list_size)
{
	mbfl_string string;

	if (!list) {
		list = (const zend_encoding **)MBSTRG(current_detect_order_list);
		list_size = MBSTRG(current_detect_order_list_size);
	}

	mbfl_string_init(&string);
	string.val = (unsigned char *)arg_string;
	string.len = arg_length;
	return (const zend_encoding *) mbfl_identify_encoding(&string, (const mbfl_encoding **)list, list_size, 0);
}

static size_t php_mb_zend_encoding_converter(unsigned char **to, size_t *to_length, const unsigned char *from, size_t from_length, const zend_encoding *encoding_to, const zend_encoding *encoding_from)
{
	mbfl_string string, result;
	mbfl_buffer_converter *convd;

	/* new encoding */
	/* initialize string */
	string.encoding = (const mbfl_encoding*)encoding_from;
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
	size_t loc = mbfl_buffer_converter_feed(convd, &string);

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

static zend_result php_mb_zend_encoding_list_parser(const char *encoding_list, size_t encoding_list_len, const zend_encoding ***return_list, size_t *return_size, bool persistent)
{
	return php_mb_parse_encoding_list(
		encoding_list, encoding_list_len,
		(const mbfl_encoding ***)return_list, return_size,
		persistent, /* arg_num */ 0, /* allow_pass_encoding */ 1);
}

static const zend_encoding *php_mb_zend_internal_encoding_getter(void)
{
	return (const zend_encoding *)MBSTRG(internal_encoding);
}

static zend_result php_mb_zend_internal_encoding_setter(const zend_encoding *encoding)
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
			size_t j = php_mb_mbchar_bytes(start+i, (const mbfl_encoding *)encoding);

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
			pos += php_mb_mbchar_bytes(pos, (const mbfl_encoding *)encoding);

		}
	}
	if (*pos == '\0') {
		res = estrdup(*line);
		*line += strlen(*line);
		return res;
	}

	res = estrndup(*line, pos - *line);

	while (*pos == stop) {
		pos += php_mb_mbchar_bytes(pos, (const mbfl_encoding *)encoding);
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
	s = php_mb_safe_strrchr(filename, '\\', filename_len, (const mbfl_encoding *)encoding);
	s2 = php_mb_safe_strrchr(filename, '/', filename_len, (const mbfl_encoding *)encoding);

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
			pefree(ZEND_VOIDP(MBSTRG(detect_order_list)), 1);
		}
		MBSTRG(detect_order_list) = NULL;
		MBSTRG(detect_order_list_size) = 0;
		return SUCCESS;
	}

	if (FAILURE == php_mb_parse_encoding_list(ZSTR_VAL(new_value), ZSTR_LEN(new_value), &list, &size, /* persistent */ 1, /* arg_num */ 0, /* allow_pass_encoding */ 0) || size == 0) {
		return FAILURE;
	}

	if (MBSTRG(detect_order_list)) {
		pefree(ZEND_VOIDP(MBSTRG(detect_order_list)), 1);
	}
	MBSTRG(detect_order_list) = list;
	MBSTRG(detect_order_list_size) = size;
	return SUCCESS;
}
/* }}} */

static int _php_mb_ini_mbstring_http_input_set(const char *new_value, size_t new_value_length) {
	const mbfl_encoding **list;
	size_t size;
	if (FAILURE == php_mb_parse_encoding_list(new_value, new_value_length, &list, &size, /* persistent */ 1, /* arg_num */ 0, /* allow_pass_encoding */ 1) || size == 0) {
		return FAILURE;
	}
	if (MBSTRG(http_input_list)) {
		pefree(ZEND_VOIDP(MBSTRG(http_input_list)), 1);
	}
	MBSTRG(http_input_list) = list;
	MBSTRG(http_input_list_size) = size;
	return SUCCESS;
}

/* {{{ static PHP_INI_MH(OnUpdate_mbstring_http_input) */
static PHP_INI_MH(OnUpdate_mbstring_http_input)
{
	if (new_value) {
		php_error_docref("ref.mbstring", E_DEPRECATED, "Use of mbstring.http_input is deprecated");
	}

	if (!new_value) {
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
	const mbfl_encoding *encoding = php_mb_get_encoding_or_pass(new_value);
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
	if (new_value) {
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
		if (new_value) {
			php_error_docref("ref.mbstring", E_WARNING, "Unknown encoding \"%s\" in ini setting", new_value);
		}
		encoding = &mbfl_encoding_utf8;
	}
	MBSTRG(internal_encoding) = encoding;
	MBSTRG(current_internal_encoding) = encoding;
#ifdef HAVE_MBREGEX
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
	if (new_value) {
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
		if (zend_string_equals_literal_ci(new_value, "none")) {
			MBSTRG(filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
			MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
		} else if (zend_string_equals_literal_ci(new_value, "long")) {
			MBSTRG(filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG;
			MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG;
		} else if (zend_string_equals_literal_ci(new_value, "entity")) {
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
		MBSTRG(filter_illegal_substchar) = '?';
		MBSTRG(current_filter_illegal_substchar) = '?';
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
#ifdef HAVE_MBREGEX
	STD_PHP_INI_ENTRY("mbstring.regex_stack_limit", "100000",PHP_INI_ALL, OnUpdateLong, regex_stack_limit, zend_mbstring_globals, mbstring_globals)
	STD_PHP_INI_ENTRY("mbstring.regex_retry_limit", "1000000",PHP_INI_ALL, OnUpdateLong, regex_retry_limit, zend_mbstring_globals, mbstring_globals)
#endif
PHP_INI_END()
/* }}} */

static void mbstring_internal_encoding_changed_hook(void) {
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
	mbstring_globals->filter_illegal_substchar = '?';
	mbstring_globals->current_filter_illegal_mode = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
	mbstring_globals->current_filter_illegal_substchar = '?';
	mbstring_globals->illegalchars = 0;
	mbstring_globals->encoding_translation = 0;
	mbstring_globals->strict_detection = 0;
	mbstring_globals->outconv = NULL;
	mbstring_globals->http_output_conv_mimetypes = NULL;
#ifdef HAVE_MBREGEX
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
		free(ZEND_VOIDP(mbstring_globals->http_input_list));
	}
	if (mbstring_globals->detect_order_list) {
		free(ZEND_VOIDP(mbstring_globals->detect_order_list));
	}
	if (mbstring_globals->http_output_conv_mimetypes) {
		_php_mb_free_regex(mbstring_globals->http_output_conv_mimetypes);
	}
#ifdef HAVE_MBREGEX
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

#ifdef HAVE_MBREGEX
	PHP_MINIT(mb_regex) (INIT_FUNC_ARGS_PASSTHRU);
#endif

	register_mbstring_symbols(module_number);

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

	zend_multibyte_restore_functions();

#ifdef HAVE_MBREGEX
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

#ifdef HAVE_MBREGEX
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
		efree(ZEND_VOIDP(MBSTRG(current_detect_order_list)));
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

#ifdef HAVE_MBREGEX
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

#ifdef HAVE_MBREGEX
	PHP_MINFO(mb_regex)(ZEND_MODULE_INFO_FUNC_ARGS_PASSTHRU);
#endif

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ Sets the current language or Returns the current language as a string */
PHP_FUNCTION(mb_language)
{
	zend_string *name = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(name)
	ZEND_PARSE_PARAMETERS_END();

	if (name == NULL) {
		RETVAL_STRING((char *)mbfl_no_language2name(MBSTRG(language)));
	} else {
		zend_string *ini_name = zend_string_init("mbstring.language", sizeof("mbstring.language") - 1, 0);
		if (FAILURE == zend_alter_ini_entry(ini_name, name, PHP_INI_USER, PHP_INI_STAGE_RUNTIME)) {
			zend_argument_value_error(1, "must be a valid language, \"%s\" given", ZSTR_VAL(name));
			zend_string_release_ex(ini_name, 0);
			RETURN_THROWS();
		}
		// TODO Make return void
		RETVAL_TRUE;
		zend_string_release_ex(ini_name, 0);
	}
}
/* }}} */

/* {{{ Sets the current internal encoding or Returns the current internal encoding as a string */
PHP_FUNCTION(mb_internal_encoding)
{
	char *name = NULL;
	size_t name_len;
	const mbfl_encoding *encoding;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(name, name_len)
	ZEND_PARSE_PARAMETERS_END();

	if (name == NULL) {
		ZEND_ASSERT(MBSTRG(current_internal_encoding));
		RETURN_STRING(MBSTRG(current_internal_encoding)->name);
	} else {
		encoding = mbfl_name2encoding(name);
		if (!encoding) {
			zend_argument_value_error(1, "must be a valid encoding, \"%s\" given", name);
			RETURN_THROWS();
		} else {
			MBSTRG(current_internal_encoding) = encoding;
			MBSTRG(internal_encoding_set) = 1;
			/* TODO Return old encoding */
			RETURN_TRUE;
		}
	}
}
/* }}} */

/* {{{ Returns the input encoding */
PHP_FUNCTION(mb_http_input)
{
	char *type = NULL;
	size_t type_len = 0, n;
	const mbfl_encoding **entry;
	const mbfl_encoding *encoding;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(type, type_len)
	ZEND_PARSE_PARAMETERS_END();

	if (type == NULL) {
		encoding = MBSTRG(http_input_identify);
	} else {
		switch (*type) {
		case 'G':
		case 'g':
			encoding = MBSTRG(http_input_identify_get);
			break;
		case 'P':
		case 'p':
			encoding = MBSTRG(http_input_identify_post);
			break;
		case 'C':
		case 'c':
			encoding = MBSTRG(http_input_identify_cookie);
			break;
		case 'S':
		case 's':
			encoding = MBSTRG(http_input_identify_string);
			break;
		case 'I':
		case 'i':
			entry = MBSTRG(http_input_list);
			n = MBSTRG(http_input_list_size);
			array_init(return_value);
			for (size_t i = 0; i < n; i++, entry++) {
				add_next_index_string(return_value, (*entry)->name);
			}
			return;
		case 'L':
		case 'l':
			entry = MBSTRG(http_input_list);
			n = MBSTRG(http_input_list_size);
			if (n == 0) {
				RETURN_FALSE;
			}
			// TODO Use smart_str instead.
			mbfl_string result;
			mbfl_memory_device device;
			mbfl_memory_device_init(&device, n * 12, 0);
			for (size_t i = 0; i < n; i++, entry++) {
				mbfl_memory_device_strcat(&device, (*entry)->name);
				mbfl_memory_device_output(',', &device);
			}
			mbfl_memory_device_unput(&device); /* Remove trailing comma */
			mbfl_memory_device_result(&device, &result);
			RETVAL_STRINGL((const char*)result.val, result.len);
			mbfl_string_clear(&result);
			return;
		default:
			zend_argument_value_error(1,
				"must be one of \"G\", \"P\", \"C\", \"S\", \"I\", or \"L\"");
			RETURN_THROWS();
		}
	}

	if (encoding) {
		RETURN_STRING(encoding->name);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Sets the current output_encoding or returns the current output_encoding as a string */
PHP_FUNCTION(mb_http_output)
{
	char *name = NULL;
	size_t name_len;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING_OR_NULL(name, name_len)
	ZEND_PARSE_PARAMETERS_END();

	if (name == NULL) {
		ZEND_ASSERT(MBSTRG(current_http_output_encoding));
		RETURN_STRING(MBSTRG(current_http_output_encoding)->name);
	} else {
		const mbfl_encoding *encoding = php_mb_get_encoding_or_pass(name);
		if (!encoding) {
			zend_argument_value_error(1, "must be a valid encoding, \"%s\" given", name);
			RETURN_THROWS();
		} else {
			MBSTRG(http_output_set) = 1;
			MBSTRG(current_http_output_encoding) = encoding;
			/* TODO Return previous encoding? */
			RETURN_TRUE;
		}
	}
}
/* }}} */

/* {{{ Sets the current detect_order or Return the current detect_order as a array */
PHP_FUNCTION(mb_detect_order)
{
	zend_string *order_str = NULL;
	HashTable *order_ht = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_STR_OR_NULL(order_ht, order_str)
	ZEND_PARSE_PARAMETERS_END();

	if (!order_str && !order_ht) {
		size_t n = MBSTRG(current_detect_order_list_size);
		const mbfl_encoding **entry = MBSTRG(current_detect_order_list);
		array_init(return_value);
		for (size_t i = 0; i < n; i++) {
			add_next_index_string(return_value, (*entry)->name);
			entry++;
		}
	} else {
		const mbfl_encoding **list;
		size_t size;
		if (order_ht) {
			if (FAILURE == php_mb_parse_encoding_array(order_ht, &list, &size, 1)) {
				RETURN_THROWS();
			}
		} else {
			if (FAILURE == php_mb_parse_encoding_list(ZSTR_VAL(order_str), ZSTR_LEN(order_str), &list, &size, /* persistent */ 0, /* arg_num */ 1, /* allow_pass_encoding */ 0)) {
				RETURN_THROWS();
			}
		}

		if (size == 0) {
			efree(ZEND_VOIDP(list));
			zend_argument_value_error(1, "must specify at least one encoding");
			RETURN_THROWS();
		}

		if (MBSTRG(current_detect_order_list)) {
			efree(ZEND_VOIDP(MBSTRG(current_detect_order_list)));
		}
		MBSTRG(current_detect_order_list) = list;
		MBSTRG(current_detect_order_list_size) = size;
		RETURN_TRUE;
	}
}
/* }}} */

static inline int php_mb_check_code_point(zend_long cp)
{
	if (cp < 0 || cp >= 0x110000) {
		/* Out of Unicode range */
		return 0;
	}

	if (cp >= 0xd800 && cp <= 0xdfff) {
		/* Surrogate code-point. These are never valid on their own and we only allow a single
		 * substitute character. */
		return 0;
	}

	/* As we do not know the target encoding of the conversion operation that is going to
	 * use the substitution character, we cannot check whether the codepoint is actually mapped
	 * in the given encoding at this point. Thus we have to accept everything. */
	return 1;
}

/* {{{ Sets the current substitute_character or returns the current substitute_character */
PHP_FUNCTION(mb_substitute_character)
{
	zend_string *substitute_character = NULL;
	zend_long substitute_codepoint;
	bool substitute_is_null = 1;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_LONG_OR_NULL(substitute_character, substitute_codepoint, substitute_is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (substitute_is_null) {
		if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			RETURN_STRING("none");
		}
		if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG) {
			RETURN_STRING("long");
		}
		if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY) {
			RETURN_STRING("entity");
		}
		RETURN_LONG(MBSTRG(current_filter_illegal_substchar));
	}

	if (substitute_character != NULL) {
		if (zend_string_equals_literal_ci(substitute_character, "none")) {
			MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
			RETURN_TRUE;
		}
		if (zend_string_equals_literal_ci(substitute_character, "long")) {
			MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG;
			RETURN_TRUE;
		}
		if (zend_string_equals_literal_ci(substitute_character, "entity")) {
			MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY;
			RETURN_TRUE;
		}
		/* Invalid string value */
		zend_argument_value_error(1, "must be \"none\", \"long\", \"entity\" or a valid codepoint");
		RETURN_THROWS();
	}
	/* Integer codepoint passed */
	if (!php_mb_check_code_point(substitute_codepoint)) {
		zend_argument_value_error(1, "is not a valid codepoint");
		RETURN_THROWS();
	}

	MBSTRG(current_filter_illegal_mode) = MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
	MBSTRG(current_filter_illegal_substchar) = substitute_codepoint;
	RETURN_TRUE;
}
/* }}} */

/* {{{ Return the preferred MIME name (charset) as a string */
PHP_FUNCTION(mb_preferred_mime_name)
{
	enum mbfl_no_encoding no_encoding;
	char *name = NULL;
	size_t name_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(name, name_len)
	ZEND_PARSE_PARAMETERS_END();

	no_encoding = mbfl_name2no_encoding(name);
	if (no_encoding == mbfl_no_encoding_invalid) {
		zend_argument_value_error(1, "must be a valid encoding, \"%s\" given", name);
		RETURN_THROWS();
	}

	const char *preferred_name = mbfl_no2preferred_mime_name(no_encoding);
	if (preferred_name == NULL || *preferred_name == '\0') {
		php_error_docref(NULL, E_WARNING, "No MIME preferred name corresponding to \"%s\"", name);
		RETVAL_FALSE;
	} else {
		RETVAL_STRING((char *)preferred_name);
	}
}
/* }}} */

/* {{{ Parses GET/POST/COOKIE data and sets global variables */
PHP_FUNCTION(mb_parse_str)
{
	zval *track_vars_array = NULL;
	char *encstr;
	size_t encstr_len;
	php_mb_encoding_handler_info_t info;
	const mbfl_encoding *detected;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(encstr, encstr_len)
		Z_PARAM_ZVAL(track_vars_array)
	ZEND_PARSE_PARAMETERS_END();

	track_vars_array = zend_try_array_init(track_vars_array);
	if (!track_vars_array) {
		RETURN_THROWS();
	}

	encstr = estrndup(encstr, encstr_len);

	info.data_type              = PARSE_STRING;
	info.separator              = PG(arg_separator).input;
	info.report_errors          = true;
	info.to_encoding            = MBSTRG(current_internal_encoding);
	info.from_encodings         = MBSTRG(http_input_list);
	info.num_from_encodings     = MBSTRG(http_input_list_size);

	detected = _php_mb_encoding_handler_ex(&info, track_vars_array, encstr);

	MBSTRG(http_input_identify) = detected;

	RETVAL_BOOL(detected);

	if (encstr != NULL) efree(encstr);
}
/* }}} */

/* {{{ Returns string in output buffer converted to the http_output encoding */
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

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(arg_string, arg_string_len)
		Z_PARAM_LONG(arg_status)
	ZEND_PARSE_PARAMETERS_END();

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
			if ((s = strchr(SG(sapi_headers).mimetype,';')) == NULL) {
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

/* {{{ Convert a multibyte string to an array. If split_length is specified,
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

	if (params->split_length == ++params->mb_chunk_length) { /* if current chunk size reached defined chunk size or last char reached */
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
		Z_PARAM_STR_OR_NULL(encoding)
	ZEND_PARSE_PARAMETERS_END();

	if (split_length <= 0) {
		zend_argument_value_error(2, "must be greater than 0");
		RETURN_THROWS();
	}

	/* fill mbfl_string structure */
	string.val = (unsigned char *) ZSTR_VAL(str);
	string.len = ZSTR_LEN(str);
	string.encoding = php_mb_get_encoding(encoding, 3);
	if (!string.encoding) {
		RETURN_THROWS();
	}

	if (ZSTR_LEN(str) == 0) {
		RETURN_EMPTY_ARRAY();
	}

	p = ZSTR_VAL(str); /* string cursor pointer */
	last = ZSTR_VAL(str) + ZSTR_LEN(str); /* last string char pointer */

	mbfl_encoding = string.encoding;

	/* first scenario: 1,2,4-bytes fixed width encodings (head part) */
	if (mbfl_encoding->flag & MBFL_ENCTYPE_SBCS) { /* 1 byte */
		mb_len = string.len;
		chunk_len = (size_t)split_length; /* chunk length in bytes */
	} else if (mbfl_encoding->flag & MBFL_ENCTYPE_WCS2) { /* 2 bytes */
		mb_len = string.len / 2;
		chunk_len = split_length * 2;
	} else if (mbfl_encoding->flag & MBFL_ENCTYPE_WCS4) { /* 4 bytes */
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
		/* assert that nothing is wrong with the decoder */
		ZEND_ASSERT(decoder != NULL);

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
		/* assert that nothing is wrong with the filter */
		ZEND_ASSERT(filter != NULL);

		while (p < last - 1) { /* cycle each byte except last with callback function */
			(*filter->filter_function)(*p++, filter);
		}
		params.mb_chunk_length = split_length - 1; /* force to finish current chunk */
		(*filter->filter_function)(*p++, filter); /* process last char */

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

static size_t mb_get_strlen(zend_string *string, const mbfl_encoding *encoding)
{
	size_t len = 0;

	if (encoding->flag & MBFL_ENCTYPE_SBCS) {
		return ZSTR_LEN(string);
	} else if (encoding->flag & MBFL_ENCTYPE_WCS2) {
		return ZSTR_LEN(string) / 2;
	} else if (encoding->flag & MBFL_ENCTYPE_WCS4) {
		return ZSTR_LEN(string) / 4;
	} else if (encoding->mblen_table) {
		const unsigned char *mbtab = encoding->mblen_table;
		unsigned char *p = (unsigned char*)ZSTR_VAL(string), *e = p + ZSTR_LEN(string);
		while (p < e) {
			p += mbtab[*p];
			len++;
		}
	} else {
		uint32_t wchar_buf[128];
		unsigned char *in = (unsigned char*)ZSTR_VAL(string);
		size_t in_len = ZSTR_LEN(string);
		unsigned int state = 0;

		while (in_len) {
			len += encoding->to_wchar(&in, &in_len, wchar_buf, 128, &state);
		}
	}

	return len;
}

/* {{{ Get character numbers of a string */
PHP_FUNCTION(mb_strlen)
{
	zend_string *string, *enc_name = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(string)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(enc_name)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *enc = php_mb_get_encoding(enc_name, 2);
	if (!enc) {
		RETURN_THROWS();
	}

	RETVAL_LONG(mb_get_strlen(string, enc));
}
/* }}} */

/* See mbfl_no_encoding definition for list of UTF-8 encodings */
static inline bool php_mb_is_no_encoding_utf8(enum mbfl_no_encoding no_enc)
{
	return (no_enc >= mbfl_no_encoding_utf8 && no_enc <= mbfl_no_encoding_utf8_sb);
}

static unsigned char* offset_to_pointer_utf8(unsigned char *str, unsigned char *end, ssize_t offset) {
	if (offset < 0) {
		unsigned char *pos = end;
		while (offset < 0) {
			if (pos <= str) {
				return NULL;
			}

			unsigned char c = *--pos;
			if (c < 0x80 || (c & 0xC0) != 0x80) {
				offset++;
			}
		}
		return pos;
	} else {
		const unsigned char *u8_tbl = mbfl_encoding_utf8.mblen_table;
		unsigned char *pos = str;
		while (offset-- > 0) {
			if (pos >= end) {
				return NULL;
			}
			pos += u8_tbl[*pos];
		}
		return pos;
	}
}

static size_t pointer_to_offset_utf8(unsigned char *start, unsigned char *pos) {
	size_t result = 0;
	while (pos > start) {
		unsigned char c = *--pos;
		if (c < 0x80 || (c & 0xC0) != 0x80) {
			result++;
		}
	}
	return result;
}

static size_t mb_find_strpos(zend_string *haystack, zend_string *needle, const mbfl_encoding *enc, ssize_t offset, bool reverse)
{
	size_t result;
	zend_string *haystack_u8 = NULL, *needle_u8 = NULL;
	unsigned char *offset_pointer;

	if (!php_mb_is_no_encoding_utf8(enc->no_encoding)) {
		unsigned int num_errors = 0;
		haystack_u8 = mb_fast_convert((unsigned char*)ZSTR_VAL(haystack), ZSTR_LEN(haystack), enc, &mbfl_encoding_utf8, 0, MBFL_OUTPUTFILTER_ILLEGAL_MODE_BADUTF8, &num_errors);
		needle_u8 = mb_fast_convert((unsigned char*)ZSTR_VAL(needle), ZSTR_LEN(needle), enc, &mbfl_encoding_utf8, 0, MBFL_OUTPUTFILTER_ILLEGAL_MODE_BADUTF8, &num_errors);
	} else {
		haystack_u8 = haystack;
		needle_u8 = needle;
	}

	offset_pointer = offset_to_pointer_utf8((unsigned char*)ZSTR_VAL(haystack_u8), (unsigned char*)ZSTR_VAL(haystack_u8) + ZSTR_LEN(haystack_u8), offset);
	if (!offset_pointer) {
		result = MBFL_ERROR_OFFSET;
		goto out;
	}

	result = MBFL_ERROR_NOT_FOUND;
	if (ZSTR_LEN(haystack_u8) < ZSTR_LEN(needle_u8)) {
		goto out;
	}

	const char *found_pos;
	if (!reverse) {
		found_pos = zend_memnstr((const char*)offset_pointer, ZSTR_VAL(needle_u8), ZSTR_LEN(needle_u8), ZSTR_VAL(haystack_u8) + ZSTR_LEN(haystack_u8));
	} else if (offset >= 0) {
		found_pos = zend_memnrstr((const char*)offset_pointer, ZSTR_VAL(needle_u8), ZSTR_LEN(needle_u8), ZSTR_VAL(haystack_u8) + ZSTR_LEN(haystack_u8));
	} else {
		size_t needle_len = pointer_to_offset_utf8((unsigned char*)ZSTR_VAL(needle), (unsigned char*)ZSTR_VAL(needle) + ZSTR_LEN(needle));
		offset_pointer = offset_to_pointer_utf8(offset_pointer, (unsigned char*)ZSTR_VAL(haystack_u8) + ZSTR_LEN(haystack_u8), needle_len);
		if (!offset_pointer) {
			offset_pointer = (unsigned char*)ZSTR_VAL(haystack_u8) + ZSTR_LEN(haystack_u8);
		}

		found_pos = zend_memnrstr(ZSTR_VAL(haystack_u8), ZSTR_VAL(needle_u8), ZSTR_LEN(needle_u8), (const char*)offset_pointer);
	}

	if (found_pos) {
		result = pointer_to_offset_utf8((unsigned char*)ZSTR_VAL(haystack_u8), (unsigned char*)found_pos);
	}

out:
	if (haystack_u8 != haystack) {
		zend_string_free(haystack_u8);
	}
	if (needle_u8 != needle) {
		zend_string_free(needle_u8);
	}
	return result;
}

static void handle_strpos_error(size_t error) {
	switch (error) {
	case MBFL_ERROR_NOT_FOUND:
		break;
	case MBFL_ERROR_ENCODING:
		php_error_docref(NULL, E_WARNING, "Conversion error");
		break;
	case MBFL_ERROR_OFFSET:
		zend_argument_value_error(3, "must be contained in argument #1 ($haystack)");
		break;
	default:
		zend_value_error("mb_strpos(): Unknown error");
		break;
	}
}

PHP_FUNCTION(mb_strpos)
{
	zend_long offset = 0;
	zend_string *needle, *haystack;
	zend_string *enc_name = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STR(haystack)
		Z_PARAM_STR(needle)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(offset)
		Z_PARAM_STR_OR_NULL(enc_name)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *enc = php_mb_get_encoding(enc_name, 4);
	if (!enc) {
		RETURN_THROWS();
	}

	size_t n = mb_find_strpos(haystack, needle, enc, offset, false);
	if (!mbfl_is_error(n)) {
		RETVAL_LONG(n);
	} else {
		handle_strpos_error(n);
		RETVAL_FALSE;
	}
}

/* {{{ Find position of last occurrence of a string within another */
PHP_FUNCTION(mb_strrpos)
{
	zend_long offset = 0;
	zend_string *needle, *haystack;
	zend_string *enc_name = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STR(haystack)
		Z_PARAM_STR(needle)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(offset)
		Z_PARAM_STR_OR_NULL(enc_name)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *enc = php_mb_get_encoding(enc_name, 4);
	if (!enc) {
		RETURN_THROWS();
	}

	size_t n = mb_find_strpos(haystack, needle, enc, offset, true);
	if (!mbfl_is_error(n)) {
		RETVAL_LONG(n);
	} else {
		handle_strpos_error(n);
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ Finds position of first occurrence of a string within another, case insensitive */
PHP_FUNCTION(mb_stripos)
{
	zend_long offset = 0;
	zend_string *haystack, *needle;
	zend_string *from_encoding = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STR(haystack)
		Z_PARAM_STR(needle)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(offset)
		Z_PARAM_STR_OR_NULL(from_encoding)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *enc = php_mb_get_encoding(from_encoding, 4);
	if (!enc) {
		RETURN_THROWS();
	}

	size_t n = php_mb_stripos(false, haystack, needle, offset, enc);

	if (!mbfl_is_error(n)) {
		RETVAL_LONG(n);
	} else {
		handle_strpos_error(n);
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ Finds position of last occurrence of a string within another, case insensitive */
PHP_FUNCTION(mb_strripos)
{
	zend_long offset = 0;
	zend_string *haystack, *needle;
	zend_string *from_encoding = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STR(haystack)
		Z_PARAM_STR(needle)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(offset)
		Z_PARAM_STR_OR_NULL(from_encoding)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *enc = php_mb_get_encoding(from_encoding, 4);
	if (!enc) {
		RETURN_THROWS();
	}

	size_t n = php_mb_stripos(true, haystack, needle, offset, enc);

	if (!mbfl_is_error(n)) {
		RETVAL_LONG(n);
	} else {
		handle_strpos_error(n);
		RETVAL_FALSE;
	}
}
/* }}} */

static zend_string* mb_get_substr_slow(unsigned char *in, size_t in_len, size_t from, size_t len, const mbfl_encoding *enc)
{
	uint32_t wchar_buf[128];
	unsigned int state = 0;

	mb_convert_buf buf;
	mb_convert_buf_init(&buf, MIN(len, in_len - from), MBSTRG(current_filter_illegal_substchar), MBSTRG(current_filter_illegal_mode));

	while (in_len && len) {
		size_t out_len = enc->to_wchar(&in, &in_len, wchar_buf, 128, &state);
		ZEND_ASSERT(out_len <= 128);

		if (from >= out_len) {
			from -= out_len;
		} else {
			enc->from_wchar(wchar_buf + from, MIN(out_len - from, len), &buf, !in_len || out_len >= len);
			from = 0;
			len -= MIN(out_len, len);
		}
	}

	return mb_convert_buf_result(&buf);
}

static zend_string* mb_get_substr(zend_string *input, size_t from, size_t len, const mbfl_encoding *enc)
{
	unsigned char *in = (unsigned char*)ZSTR_VAL(input);
	size_t in_len = ZSTR_LEN(input);

	if (from >= in_len) {
		/* No supported text encoding decodes to more than one codepoint per byte
		 * So if the number of codepoints to skip >= number of input bytes,
		 * then definitely the output should be empty */
		return zend_empty_string;
	}

	/* Does each codepoint have a fixed byte width? */
	unsigned int flag = enc->flag & (MBFL_ENCTYPE_SBCS | MBFL_ENCTYPE_WCS2 | MBFL_ENCTYPE_WCS4);
	if (flag) {
		/* The value of the flag is 2 if each codepoint takes 2 bytes, or 4 if 4 bytes */
		from *= flag;
		len *= flag;
		if (from >= in_len) {
			return zend_empty_string;
		}
		in += from;
		in_len -= from;
		if (len > in_len) {
			len = in_len;
		}
		return zend_string_init_fast((const char*)in, len);
	} else if (enc->mblen_table != NULL) {
		const unsigned char *mbtab = enc->mblen_table;
		unsigned char *limit = in + in_len;
		while (from && in < limit) {
			in += mbtab[*in];
			from--;
		}
		if (in >= limit) {
			return zend_empty_string;
		} else if (len == MBFL_SUBSTR_UNTIL_END) {
			return zend_string_init_fast((const char*)in, limit - in);
		}
		unsigned char *end = in;
		while (len && end < limit) {
			end += mbtab[*end];
			len--;
		}
		if (end > limit) {
			end = limit;
		}
		return zend_string_init_fast((const char*)in, end - in);
	}

	return mb_get_substr_slow(in, in_len, from, len, enc);
}

#define MB_STRSTR 1
#define MB_STRRCHR 2
#define MB_STRISTR 3
#define MB_STRRICHR 4

static void php_mb_strstr_variants(INTERNAL_FUNCTION_PARAMETERS, unsigned int variant)
{
	bool reverse_mode = false, part = false;
	size_t n;
	zend_string *haystack, *needle;
	zend_string *encoding_name = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STR(haystack)
		Z_PARAM_STR(needle)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(part)
		Z_PARAM_STR_OR_NULL(encoding_name)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *enc = php_mb_get_encoding(encoding_name, 4);
	if (!enc) {
		RETURN_THROWS();
	}

	if (variant == MB_STRRCHR || variant == MB_STRRICHR) {
		reverse_mode = true;
	}

	if (variant == MB_STRISTR || variant == MB_STRRICHR) {
		n = php_mb_stripos(reverse_mode, haystack, needle, 0, enc);
	} else {
		n = mb_find_strpos(haystack, needle, enc, 0, reverse_mode);
	}

	if (!mbfl_is_error(n)) {
		if (part) {
			RETVAL_STR(mb_get_substr(haystack, 0, n, enc));
		} else {
			RETVAL_STR(mb_get_substr(haystack, n, MBFL_SUBSTR_UNTIL_END, enc));
		}
	} else {
		// FIXME use handle_strpos_error(n)
		RETVAL_FALSE;
	}
}

/* {{{ Finds first occurrence of a string within another */
PHP_FUNCTION(mb_strstr)
{
	php_mb_strstr_variants(INTERNAL_FUNCTION_PARAM_PASSTHRU, MB_STRSTR);
}
/* }}} */

/* {{{ Finds the last occurrence of a character in a string within another */
PHP_FUNCTION(mb_strrchr)
{
	php_mb_strstr_variants(INTERNAL_FUNCTION_PARAM_PASSTHRU, MB_STRRCHR);
}
/* }}} */

/* {{{ Finds first occurrence of a string within another, case insensitive */
PHP_FUNCTION(mb_stristr)
{
	php_mb_strstr_variants(INTERNAL_FUNCTION_PARAM_PASSTHRU, MB_STRISTR);
}
/* }}} */

/* {{{ Finds the last occurrence of a character in a string within another, case insensitive */
PHP_FUNCTION(mb_strrichr)
{
	php_mb_strstr_variants(INTERNAL_FUNCTION_PARAM_PASSTHRU, MB_STRRICHR);
}
/* }}} */

#undef MB_STRSTR
#undef MB_STRRCHR
#undef MB_STRISTR
#undef MB_STRRICHR

PHP_FUNCTION(mb_substr_count)
{
	zend_string *haystack, *needle, *enc_name = NULL, *haystack_u8 = NULL, *needle_u8 = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(haystack)
		Z_PARAM_STR(needle)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(enc_name)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_LEN(needle) == 0) {
		zend_argument_value_error(2, "must not be empty");
		RETURN_THROWS();
	}

	const mbfl_encoding *enc = php_mb_get_encoding(enc_name, 3);
	if (!enc) {
		RETURN_THROWS();
	}

	if (php_mb_is_no_encoding_utf8(enc->no_encoding)) {
		/* No need to do any conversion if haystack/needle are already known-valid UTF-8
		 * (If they are not valid, then not passing them through conversion filters could affect output) */
		if (GC_FLAGS(haystack) & IS_STR_VALID_UTF8) {
			haystack_u8 = haystack;
		} else {
			unsigned int num_errors = 0;
			haystack_u8 = mb_fast_convert((unsigned char*)ZSTR_VAL(haystack), ZSTR_LEN(haystack), enc, &mbfl_encoding_utf8, 0, MBFL_OUTPUTFILTER_ILLEGAL_MODE_BADUTF8, &num_errors);
			if (!num_errors && !ZSTR_IS_INTERNED(haystack)) {
				GC_ADD_FLAGS(haystack, IS_STR_VALID_UTF8);
			}
		}

		if (GC_FLAGS(needle) & IS_STR_VALID_UTF8) {
			needle_u8 = needle;
		} else {
			unsigned int num_errors = 0;
			needle_u8 = mb_fast_convert((unsigned char*)ZSTR_VAL(needle), ZSTR_LEN(needle), enc, &mbfl_encoding_utf8, 0, MBFL_OUTPUTFILTER_ILLEGAL_MODE_BADUTF8, &num_errors);
			if (!num_errors && !ZSTR_IS_INTERNED(needle)) {
				GC_ADD_FLAGS(needle, IS_STR_VALID_UTF8);
			}
		}
	} else {
		unsigned int num_errors = 0;
		haystack_u8 = mb_fast_convert((unsigned char*)ZSTR_VAL(haystack), ZSTR_LEN(haystack), enc, &mbfl_encoding_utf8, 0, MBFL_OUTPUTFILTER_ILLEGAL_MODE_BADUTF8, &num_errors);
		needle_u8 = mb_fast_convert((unsigned char*)ZSTR_VAL(needle), ZSTR_LEN(needle), enc, &mbfl_encoding_utf8, 0, MBFL_OUTPUTFILTER_ILLEGAL_MODE_BADUTF8, &num_errors);
		/* A string with >0 bytes may convert to 0 codepoints; for example, the contents
		 * may be only escape sequences */
		if (ZSTR_LEN(needle_u8) == 0) {
			zend_string_free(haystack_u8);
			zend_string_free(needle_u8);
			zend_argument_value_error(2, "must not be empty");
			RETURN_THROWS();
		}
	}

	size_t result = 0;

	if (ZSTR_LEN(haystack_u8) < ZSTR_LEN(needle_u8)) {
		goto out;
	}

	const char *p = ZSTR_VAL(haystack_u8), *e = p + ZSTR_LEN(haystack_u8);
	while (true) {
		p = zend_memnstr(p, ZSTR_VAL(needle_u8), ZSTR_LEN(needle_u8), e);
		if (!p) {
			break;
		}
		p += ZSTR_LEN(needle_u8);
		result++;
	}

out:
	if (haystack_u8 != haystack) {
		zend_string_free(haystack_u8);
	}
	if (needle_u8 != needle) {
		zend_string_free(needle_u8);
	}

	RETVAL_LONG(result);
}

/* {{{ Returns part of a string */
PHP_FUNCTION(mb_substr)
{
	zend_string *str, *encoding = NULL;
	zend_long from, len;
	size_t real_from, real_len;
	bool len_is_null = true;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STR(str)
		Z_PARAM_LONG(from)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(len, len_is_null)
		Z_PARAM_STR_OR_NULL(encoding)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *enc = php_mb_get_encoding(encoding, 4);
	if (!enc) {
		RETURN_THROWS();
	}

	size_t mblen = 0;
	if (from < 0 || (!len_is_null && len < 0)) {
		mblen = mb_get_strlen(str, enc);
	}

	/* if "from" position is negative, count start position from the end
	 * of the string */
	if (from >= 0) {
		real_from = (size_t) from;
	} else if (-from < mblen) {
		real_from = mblen + from;
	} else {
		real_from = 0;
	}

	/* if "length" position is negative, set it to the length
	 * needed to stop that many chars from the end of the string */
	if (len_is_null) {
		real_len = MBFL_SUBSTR_UNTIL_END;
	} else if (len >= 0) {
		real_len = (size_t) len;
	} else if (real_from < mblen && -len < mblen - real_from) {
		real_len = (mblen - real_from) + len;
	} else {
		real_len = 0;
	}

	RETVAL_STR(mb_get_substr(str, real_from, real_len, enc));
}
/* }}} */

/* {{{ Returns part of a string */
PHP_FUNCTION(mb_strcut)
{
	zend_string *encoding = NULL;
	char *string_val;
	zend_long from, len;
	bool len_is_null = 1;
	mbfl_string string, result, *ret;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STRING(string_val, string.len)
		Z_PARAM_LONG(from)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(len, len_is_null)
		Z_PARAM_STR_OR_NULL(encoding)
	ZEND_PARSE_PARAMETERS_END();

	string.val = (unsigned char*)string_val;
	string.encoding = php_mb_get_encoding(encoding, 4);
	if (!string.encoding) {
		RETURN_THROWS();
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
		RETURN_EMPTY_STRING();
	}

	ret = mbfl_strcut(&string, &result, from, len);
	ZEND_ASSERT(ret != NULL);

	// TODO: avoid reallocation ???
	RETVAL_STRINGL((char *)ret->val, ret->len); /* the string is already strdup()'ed */
	efree(ret->val);
}
/* }}} */

/* Some East Asian characters, when printed at a terminal (or the like), require double
 * the usual amount of horizontal space. We call these "fullwidth" characters. */
static size_t character_width(uint32_t c)
{
	if (c < FIRST_DOUBLEWIDTH_CODEPOINT) {
		return 1;
	}

	/* Do a binary search to see if we fall in any of the fullwidth ranges */
	int lo = 0, hi = sizeof(mbfl_eaw_table) / sizeof(mbfl_eaw_table[0]);
	while (lo < hi) {
		int probe = (lo + hi) / 2;
		if (c < mbfl_eaw_table[probe].begin) {
			hi = probe;
		} else if (c > mbfl_eaw_table[probe].end) {
			lo = probe + 1;
		} else {
			return 2;
		}
	}

	return 1;
}

static size_t mb_get_strwidth(zend_string *string, const mbfl_encoding *enc)
{
	size_t width = 0;
	uint32_t wchar_buf[128];
	unsigned char *in = (unsigned char*)ZSTR_VAL(string);
	size_t in_len = ZSTR_LEN(string);
	unsigned int state = 0;

	while (in_len) {
		size_t out_len = enc->to_wchar(&in, &in_len, wchar_buf, 128, &state);
		ZEND_ASSERT(out_len <= 128);

		while (out_len) {
			/* NOTE: 'bad input' marker will be counted as 1 unit of width
			 * If text conversion is performed with an ordinary ASCII character as
			 * the 'replacement character', this will give us the correct display width. */
			width += character_width(wchar_buf[--out_len]);
		}
	}

	return width;
}

/* Gets terminal width of a string */
PHP_FUNCTION(mb_strwidth)
{
	zend_string *string, *enc_name = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(string)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(enc_name)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *enc = php_mb_get_encoding(enc_name, 2);
	if (!enc) {
		RETURN_THROWS();
	}

	RETVAL_LONG(mb_get_strwidth(string, enc));
}

static zend_string* mb_trim_string(zend_string *input, zend_string *marker, const mbfl_encoding *enc, unsigned int from, int width)
{
	uint32_t wchar_buf[128];
	unsigned char *in = (unsigned char*)ZSTR_VAL(input);
	size_t in_len = ZSTR_LEN(input);
	unsigned int state = 0;
	int remaining_width = width;
	unsigned int to_skip = from;
	size_t out_len = 0;
	bool first_call = true, input_err = false;
	mb_convert_buf buf;

	while (in_len) {
		out_len = enc->to_wchar(&in, &in_len, wchar_buf, 128, &state);
		ZEND_ASSERT(out_len <= 128);

		if (out_len <= to_skip) {
			to_skip -= out_len;
		} else {
			for (int i = to_skip; i < out_len; i++) {
				uint32_t w = wchar_buf[i];
				input_err |= (w == MBFL_BAD_INPUT);
				remaining_width -= character_width(w);
				if (remaining_width < 0) {
					/* We need to truncate string and append trim marker */
					width -= mb_get_strwidth(marker, enc);
					/* 'width' is now the amount we want to take from 'input' */
					if (width <= 0) {
						return zend_string_copy(marker);
					}
					mb_convert_buf_init(&buf, width, MBSTRG(current_filter_illegal_substchar), MBSTRG(current_filter_illegal_mode));

					if (first_call) {
						/* We can use the buffer of wchars which we have right now;
						 * no need to convert again */
						goto dont_restart_conversion;
					} else {
						goto restart_conversion;
					}
				}
			}
			to_skip = 0;
		}
		first_call = false;
	}

	/* The input string fits in the requested width; we don't need to append the trim marker
	 * However, if the string contains erroneous byte sequences, those should be converted
	 * to error markers */
	if (!input_err) {
		if (from == 0) {
			/* This just increments the string's refcount; it doesn't really 'copy' it */
			return zend_string_copy(input);
		} else {
			return mb_get_substr(input, from, MBFL_SUBSTR_UNTIL_END, enc);
		}
	} else {
		/* We can't use `mb_get_substr`, because it uses the fastest method possible of
		 * picking out a substring, which may not include converting erroneous byte
		 * sequences to error markers */
		return mb_get_substr_slow((unsigned char*)ZSTR_VAL(input), ZSTR_LEN(input), from, MBFL_SUBSTR_UNTIL_END, enc);
	}

	/* The input string is too wide; we need to build a new string which
	 * includes some portion of the input string, with the trim marker
	 * concatenated onto it */
restart_conversion:
	in = (unsigned char*)ZSTR_VAL(input);
	in_len = ZSTR_LEN(input);
	state = 0;

	while (true) {
		out_len = enc->to_wchar(&in, &in_len, wchar_buf, 128, &state);
		ZEND_ASSERT(out_len <= 128);

dont_restart_conversion:
		if (out_len <= from) {
			from -= out_len;
		} else {
			for (int i = from; i < out_len; i++) {
				width -= character_width(wchar_buf[i]);
				if (width < 0) {
					enc->from_wchar(wchar_buf + from, i - from, &buf, true);
					goto append_trim_marker;
				}
			}
			ZEND_ASSERT(in_len > 0);
			enc->from_wchar(wchar_buf + from, out_len - from, &buf, false);
			from = 0;
		}
	}

append_trim_marker:
	if (ZSTR_LEN(marker) > 0) {
		MB_CONVERT_BUF_ENSURE((&buf), buf.out, buf.limit, ZSTR_LEN(marker));
		memcpy(buf.out, ZSTR_VAL(marker), ZSTR_LEN(marker));
		buf.out += ZSTR_LEN(marker);
	}

	return mb_convert_buf_result(&buf);
}

/* Trim the string to terminal width; optional, add a 'trim marker' if it was truncated */
PHP_FUNCTION(mb_strimwidth)
{
	zend_string *str, *trimmarker = zend_empty_string, *encoding = NULL;
	zend_long from, width;

	ZEND_PARSE_PARAMETERS_START(3, 5)
		Z_PARAM_STR(str)
		Z_PARAM_LONG(from)
		Z_PARAM_LONG(width)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(trimmarker)
		Z_PARAM_STR_OR_NULL(encoding)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *enc = php_mb_get_encoding(encoding, 5);
	if (!enc) {
		RETURN_THROWS();
	}

	if (from != 0) {
		size_t str_len = mb_get_strlen(str, enc);
		if (from < 0) {
			from += str_len;
		}
		if (from < 0 || from > str_len) {
			zend_argument_value_error(2, "is out of range");
			RETURN_THROWS();
		}
	}

	if (width < 0) {
		width += mb_get_strwidth(str, enc);

		if (from > 0) {
			zend_string *trimmed = mb_get_substr(str, 0, from, enc);
			width -= mb_get_strwidth(trimmed, enc);
			zend_string_free(trimmed);
		}

		if (width < 0) {
			zend_argument_value_error(3, "is out of range");
			RETURN_THROWS();
		}
	}

	RETVAL_STR(mb_trim_string(str, trimmarker, enc, from, width));
}


/* See mbfl_no_encoding definition for list of unsupported encodings */
static inline bool php_mb_is_unsupported_no_encoding(enum mbfl_no_encoding no_enc)
{
	return ((no_enc >= mbfl_no_encoding_invalid && no_enc <= mbfl_no_encoding_qprint)
			|| (no_enc >= mbfl_no_encoding_utf7 && no_enc <= mbfl_no_encoding_utf7imap)
			|| (no_enc >= mbfl_no_encoding_jis && no_enc <= mbfl_no_encoding_2022jpms)
			|| (no_enc >= mbfl_no_encoding_cp50220 && no_enc <= mbfl_no_encoding_cp50222));
}

MBSTRING_API zend_string* php_mb_convert_encoding_ex(const char *input, size_t length, const mbfl_encoding *to_encoding, const mbfl_encoding *from_encoding)
{
	unsigned int num_errors = 0;
	zend_string *result = mb_fast_convert((unsigned char*)input, length, from_encoding, to_encoding, MBSTRG(current_filter_illegal_substchar), MBSTRG(current_filter_illegal_mode), &num_errors);
	MBSTRG(illegalchars) += num_errors;
	return result;
}

MBSTRING_API zend_string* php_mb_convert_encoding(const char *input, size_t length, const mbfl_encoding *to_encoding, const mbfl_encoding **from_encodings, size_t num_from_encodings)
{
	const mbfl_encoding *from_encoding;

	/* pre-conversion encoding */
	ZEND_ASSERT(num_from_encodings >= 1);
	if (num_from_encodings == 1) {
		from_encoding = *from_encodings;
	} else {
		/* auto detect */
		mbfl_string string;
		mbfl_string_init(&string);
		string.val = (unsigned char *)input;
		string.len = length;
		from_encoding = mbfl_identify_encoding(
			&string, from_encodings, num_from_encodings, MBSTRG(strict_detection));
		if (!from_encoding) {
			php_error_docref(NULL, E_WARNING, "Unable to detect character encoding");
			return NULL;
		}
	}

	return php_mb_convert_encoding_ex(input, length, to_encoding, from_encoding);
}

MBSTRING_API HashTable *php_mb_convert_encoding_recursive(HashTable *input, const mbfl_encoding *to_encoding, const mbfl_encoding **from_encodings, size_t num_from_encodings)
{
	HashTable *output, *chash;
	zend_long idx;
	zend_string *key;
	zval *entry, entry_tmp;

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
			key = php_mb_convert_encoding(ZSTR_VAL(key), ZSTR_LEN(key), to_encoding, from_encodings, num_from_encodings);
		}
		/* convert value */
		ZEND_ASSERT(entry);
try_again:
		switch(Z_TYPE_P(entry)) {
			case IS_STRING:
				ZVAL_STR(&entry_tmp, php_mb_convert_encoding(Z_STRVAL_P(entry), Z_STRLEN_P(entry), to_encoding, from_encodings, num_from_encodings));
				break;
			case IS_NULL:
			case IS_TRUE:
			case IS_FALSE:
			case IS_LONG:
			case IS_DOUBLE:
				ZVAL_COPY(&entry_tmp, entry);
				break;
			case IS_ARRAY:
				chash = php_mb_convert_encoding_recursive(
					Z_ARRVAL_P(entry), to_encoding, from_encodings, num_from_encodings);
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

static void remove_non_encodings_from_elist(const mbfl_encoding **elist, size_t *size)
{
	/* mbstring supports some 'text encodings' which aren't really text encodings
	 * at all, but really 'byte encodings', like Base64, QPrint, and so on.
	 * These should never be returned by `mb_detect_encoding`. */
	int shift = 0;
	for (int i = 0; i < *size; i++) {
		const mbfl_encoding *encoding = elist[i];
		if (encoding->no_encoding <= mbfl_no_encoding_charset_min) {
			shift++; /* Remove this encoding from the list */
		} else if (shift) {
			elist[i - shift] = encoding;
		}
	}
	*size -= shift;
}

/* {{{ Returns converted string in desired encoding */
PHP_FUNCTION(mb_convert_encoding)
{
	zend_string *to_encoding_name;
	zend_string *input_str, *from_encodings_str = NULL;
	HashTable *input_ht, *from_encodings_ht = NULL;
	const mbfl_encoding **from_encodings;
	size_t num_from_encodings;
	bool free_from_encodings;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_ARRAY_HT_OR_STR(input_ht, input_str)
		Z_PARAM_STR(to_encoding_name)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_STR_OR_NULL(from_encodings_ht, from_encodings_str)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *to_encoding = php_mb_get_encoding(to_encoding_name, 2);
	if (!to_encoding) {
		RETURN_THROWS();
	}

	if (from_encodings_ht) {
		if (php_mb_parse_encoding_array(from_encodings_ht, &from_encodings, &num_from_encodings, 3) == FAILURE) {
			RETURN_THROWS();
		}
		free_from_encodings = 1;
	} else if (from_encodings_str) {
		if (php_mb_parse_encoding_list(ZSTR_VAL(from_encodings_str), ZSTR_LEN(from_encodings_str),
				&from_encodings, &num_from_encodings,
				/* persistent */ 0, /* arg_num */ 3, /* allow_pass_encoding */ 0) == FAILURE) {
			RETURN_THROWS();
		}
		free_from_encodings = 1;
	} else {
		from_encodings = &MBSTRG(current_internal_encoding);
		num_from_encodings = 1;
		free_from_encodings = 0;
	}

	if (num_from_encodings > 1) {
		remove_non_encodings_from_elist(from_encodings, &num_from_encodings);
	}

	if (!num_from_encodings) {
		efree(ZEND_VOIDP(from_encodings));
		zend_argument_value_error(3, "must specify at least one encoding");
		RETURN_THROWS();
	}

	if (input_str) {
		zend_string *ret = php_mb_convert_encoding(ZSTR_VAL(input_str), ZSTR_LEN(input_str), to_encoding, from_encodings, num_from_encodings);
		if (ret != NULL) {
			RETVAL_STR(ret);
		} else {
			RETVAL_FALSE;
		}
	} else {
		HashTable *tmp;
		tmp = php_mb_convert_encoding_recursive(
			input_ht, to_encoding, from_encodings, num_from_encodings);
		RETVAL_ARR(tmp);
	}

	if (free_from_encodings) {
		efree(ZEND_VOIDP(from_encodings));
	}
}
/* }}} */

static zend_string *mbstring_convert_case(php_case_mode case_mode, const char *str, size_t str_len, const mbfl_encoding *enc)
{
	return php_unicode_convert_case(case_mode, str, str_len, enc, enc, MBSTRG(current_filter_illegal_mode), MBSTRG(current_filter_illegal_substchar));
}

PHP_FUNCTION(mb_convert_case)
{
	zend_string *str, *from_encoding = NULL;
	zend_long case_mode = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(str)
		Z_PARAM_LONG(case_mode)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(from_encoding)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *enc = php_mb_get_encoding(from_encoding, 3);
	if (!enc) {
		RETURN_THROWS();
	}

	if (case_mode < 0 || case_mode >= PHP_UNICODE_CASE_MODE_MAX) {
		zend_argument_value_error(2, "must be one of the MB_CASE_* constants");
		RETURN_THROWS();
	}

	RETURN_STR(mbstring_convert_case(case_mode, ZSTR_VAL(str), ZSTR_LEN(str), enc));
}

PHP_FUNCTION(mb_strtoupper)
{
	zend_string *str, *from_encoding = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(from_encoding)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *enc = php_mb_get_encoding(from_encoding, 2);
	if (!enc) {
		RETURN_THROWS();
	}

	RETURN_STR(mbstring_convert_case(PHP_UNICODE_CASE_UPPER, ZSTR_VAL(str), ZSTR_LEN(str), enc));
}

PHP_FUNCTION(mb_strtolower)
{
	zend_string *str, *from_encoding = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(from_encoding)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *enc = php_mb_get_encoding(from_encoding, 2);
	if (!enc) {
		RETURN_THROWS();
	}

	RETURN_STR(mbstring_convert_case(PHP_UNICODE_CASE_LOWER, ZSTR_VAL(str), ZSTR_LEN(str), enc));
}

static const mbfl_encoding **duplicate_elist(const mbfl_encoding **elist, size_t size)
{
	const mbfl_encoding **new_elist = safe_emalloc(size, sizeof(mbfl_encoding*), 0);
	memcpy(ZEND_VOIDP(new_elist), elist, size * sizeof(mbfl_encoding*));
	return new_elist;
}

/* {{{ Encodings of the given string is returned (as a string) */
PHP_FUNCTION(mb_detect_encoding)
{
	zend_string *str, *encoding_str = NULL;
	HashTable *encoding_ht = NULL;
	bool strict = false;

	mbfl_string string;
	const mbfl_encoding *ret;
	const mbfl_encoding **elist;
	size_t size;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_STR_OR_NULL(encoding_ht, encoding_str)
		Z_PARAM_BOOL(strict)
	ZEND_PARSE_PARAMETERS_END();

	/* make encoding list */
	if (encoding_ht) {
		if (FAILURE == php_mb_parse_encoding_array(encoding_ht, &elist, &size, 2)) {
			RETURN_THROWS();
		}
	} else if (encoding_str) {
		if (FAILURE == php_mb_parse_encoding_list(ZSTR_VAL(encoding_str), ZSTR_LEN(encoding_str), &elist, &size, /* persistent */ 0, /* arg_num */ 2, /* allow_pass_encoding */ 0)) {
			RETURN_THROWS();
		}
	} else {
		elist = duplicate_elist(MBSTRG(current_detect_order_list), MBSTRG(current_detect_order_list_size));
		size = MBSTRG(current_detect_order_list_size);
	}

	if (size == 0) {
		efree(ZEND_VOIDP(elist));
		zend_argument_value_error(2, "must specify at least one encoding");
		RETURN_THROWS();
	}

	remove_non_encodings_from_elist(elist, &size);
	if (size == 0) {
		efree(ZEND_VOIDP(elist));
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() < 3) {
		strict = MBSTRG(strict_detection);
	}

	if (strict && size == 1) {
		/* If there is only a single candidate encoding, mb_check_encoding is faster */
		ret = (mb_check_str_encoding(str, *elist)) ? *elist : NULL;
	} else {
		mbfl_string_init(&string);
		string.val = (unsigned char*)ZSTR_VAL(str);
		string.len = ZSTR_LEN(str);
		ret = mbfl_identify_encoding(&string, elist, size, strict);
	}

	efree(ZEND_VOIDP(elist));

	if (ret == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRING((char *)ret->name);
}
/* }}} */

/* {{{ Returns an array of all supported entity encodings */
PHP_FUNCTION(mb_list_encodings)
{
	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);
	for (const mbfl_encoding **encodings = mbfl_get_supported_encodings(); *encodings; encodings++) {
		add_next_index_string(return_value, (*encodings)->name);
	}
}
/* }}} */

/* {{{ Returns an array of the aliases of a given encoding name */
PHP_FUNCTION(mb_encoding_aliases)
{
	const mbfl_encoding *encoding;
	zend_string *encoding_name = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(encoding_name)
	ZEND_PARSE_PARAMETERS_END();

	encoding = php_mb_get_encoding(encoding_name, 1);
	if (!encoding) {
		RETURN_THROWS();
	}

	array_init(return_value);
	if (encoding->aliases != NULL) {
		for (const char **alias = encoding->aliases; *alias; ++alias) {
			add_next_index_string(return_value, (char *)*alias);
		}
	}
}
/* }}} */

/* {{{ Converts the string to MIME "encoded-word" in the format of =?charset?(B|Q)?encoded_string?= */
PHP_FUNCTION(mb_encode_mimeheader)
{
	const mbfl_encoding *charset, *transenc;
	mbfl_string  string, result, *ret;
	zend_string *charset_name = NULL;
	char *trans_enc_name = NULL, *string_val;
	size_t trans_enc_name_len;
	char *linefeed = "\r\n";
	size_t linefeed_len;
	zend_long indent = 0;

	string.encoding = MBSTRG(current_internal_encoding);

	ZEND_PARSE_PARAMETERS_START(1, 5)
		Z_PARAM_STRING(string_val, string.len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(charset_name)
		Z_PARAM_STRING(trans_enc_name, trans_enc_name_len)
		Z_PARAM_STRING(linefeed, linefeed_len)
		Z_PARAM_LONG(indent)
	ZEND_PARSE_PARAMETERS_END();

	string.val = (unsigned char*)string_val;
	charset = &mbfl_encoding_pass;
	transenc = &mbfl_encoding_base64;

	if (charset_name != NULL) {
		charset = php_mb_get_encoding(charset_name, 2);
		if (!charset) {
			RETURN_THROWS();
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
	ZEND_ASSERT(ret != NULL);
	// TODO: avoid reallocation ???
	RETVAL_STRINGL((char *)ret->val, ret->len);	/* the string is already strdup()'ed */
	efree(ret->val);
}
/* }}} */

/* {{{ Decodes the MIME "encoded-word" in the string */
PHP_FUNCTION(mb_decode_mimeheader)
{
	char *string_val;
	mbfl_string string, result, *ret;

	string.encoding = MBSTRG(current_internal_encoding);

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(string_val, string.len)
	ZEND_PARSE_PARAMETERS_END();

	string.val = (unsigned char*)string_val;
	mbfl_string_init(&result);
	ret = mbfl_mime_header_decode(&string, &result, MBSTRG(current_internal_encoding));
	ZEND_ASSERT(ret != NULL);
	// TODO: avoid reallocation ???
	RETVAL_STRINGL((char *)ret->val, ret->len);	/* the string is already strdup()'ed */
	efree(ret->val);
}
/* }}} */

static zend_string* jp_kana_convert(zend_string *input, const mbfl_encoding *encoding, unsigned int mode)
{
	/* Each wchar may potentially expand to 2 when we perform kana conversion...
	 * if we are converting zenkaku kana to hankaku kana
	 * Make the buffer for converted kana big enough that we never need to
	 * perform bounds checks */
	uint32_t wchar_buf[64], converted_buf[64 * 2];
	unsigned int buf_offset = 0;
	unsigned int state = 0;
	unsigned char *in = (unsigned char*)ZSTR_VAL(input);
	size_t in_len = ZSTR_LEN(input);

	mb_convert_buf buf;
	mb_convert_buf_init(&buf, in_len, MBSTRG(current_filter_illegal_substchar), MBSTRG(current_filter_illegal_mode));

	while (in_len) {
		uint32_t *converted = converted_buf;
		/* If one codepoint has been left in wchar_buf[0] to be reprocessed from the
		 * previous iteration, don't overwrite it */
		size_t out_len = encoding->to_wchar(&in, &in_len, wchar_buf + buf_offset, 64 - buf_offset, &state);
		out_len += buf_offset;
		ZEND_ASSERT(out_len <= 64);

		if (!out_len) {
			continue;
		}

		for (int i = 0; i < out_len-1; i++) {
			uint32_t second = 0;
			bool consumed = false;
			*converted++ = mb_convert_kana_codepoint(wchar_buf[i], wchar_buf[i+1], &consumed, &second, mode);
			if (second) {
				*converted++ = second;
			}
			if (consumed) {
				i++;
				if (i == out_len-1) {
					/* We consumed two codepoints at the very end of the wchar buffer
					 * So there is nothing remaining to reprocess on the next iteration */
					buf_offset = 0;
					goto emit_converted_kana;
				}
			}
		}

		if (!in_len) {
			/* This is the last iteration, so we need to process the final codepoint now */
			uint32_t second = 0;
			*converted++ = mb_convert_kana_codepoint(wchar_buf[out_len-1], 0, NULL, &second, mode);
			if (second) {
				*converted++ = second;
			}
		} else {
			/* Reprocess the last codepoint on the next iteration */
			wchar_buf[0] = wchar_buf[out_len-1];
			buf_offset = 1;
		}

emit_converted_kana:
		encoding->from_wchar(converted_buf, converted - converted_buf, &buf, !in_len);
	}

	return mb_convert_buf_result(&buf);
}

char mb_convert_kana_flags[17] = {
	'A', 'R', 'N', 'S', 'K', 'H', 'M', 'C',
	'a', 'r', 'n', 's', 'k', 'h', 'm', 'c',
	'V'
};

/* Conversion between full-width characters and half-width characters (Japanese) */
PHP_FUNCTION(mb_convert_kana)
{
	unsigned int opt;
	char *optstr = NULL;
	size_t optstr_len;
	zend_string *encname = NULL, *str;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(optstr, optstr_len)
		Z_PARAM_STR_OR_NULL(encname)
	ZEND_PARSE_PARAMETERS_END();

	if (optstr != NULL) {
		char *p = optstr, *e = p + optstr_len;
		opt = 0;
next_option:
		while (p < e) {
			/* Walk through option string and convert to bit vector
			 * See translit_kana_jisx0201_jisx0208.h for the values used */
			char c = *p++;
			if (c == 'A') {
				opt |= MBFL_HAN2ZEN_ALL | MBFL_HAN2ZEN_ALPHA | MBFL_HAN2ZEN_NUMERIC;
			} else if (c == 'a') {
				opt |= MBFL_ZEN2HAN_ALL | MBFL_ZEN2HAN_ALPHA | MBFL_ZEN2HAN_NUMERIC;
			} else {
				for (int i = 0; i < sizeof(mb_convert_kana_flags) / sizeof(char); i++) {
					if (c == mb_convert_kana_flags[i]) {
						opt |= (1 << i);
						goto next_option;
					}
				}

				zend_argument_value_error(2, "contains invalid flag: '%c'", c);
				RETURN_THROWS();
			}
		}

		/* Check for illegal combinations of options */
		if (((opt & 0xFF00) >> 8) & opt) {
			/* It doesn't make sense to convert the same type of characters from halfwidth to
			 * fullwidth and then back to halfwidth again. Neither does it make sense to convert
			 * FW hiragana to FW katakana and then back again. */
			int badflag = ((opt & 0xFF00) >> 8) & opt, i;
			for (i = 0; (badflag & 1) == 0; badflag >>= 1, i++);
			char flag1 = mb_convert_kana_flags[i], flag2 = mb_convert_kana_flags[i+8];
			if ((flag1 == 'R' || flag1 == 'N') && (opt & MBFL_HAN2ZEN_ALL))
				flag1 = 'A';
			if ((flag2 == 'r' || flag2 == 'n') && (opt & MBFL_ZEN2HAN_ALL))
				flag2 = 'a';
			zend_argument_value_error(2, "must not combine '%c' and '%c' flags", flag1, flag2);
			RETURN_THROWS();
		}

		if ((opt & MBFL_HAN2ZEN_HIRAGANA) && (opt & MBFL_HAN2ZEN_KATAKANA)) {
			/* We can either convert all HW kana to FW hiragana, or to FW katakana, but not both */
			zend_argument_value_error(2, "must not combine 'H' and 'K' flags");
			RETURN_THROWS();
		}

		/* We can either convert all FW kana to HW hiragana, or all FW kana to HW katakana,
		 * or all FW hiragana to FW katakana, or all FW katakana to FW hiragana, but not
		 * more than one of these */
		if (opt & MBFL_ZEN2HAN_HIRAGANA) {
			if (opt & MBFL_ZEN2HAN_KATAKANA) {
				zend_argument_value_error(2, "must not combine 'h' and 'k' flags");
				RETURN_THROWS();
			} else if (opt & MBFL_ZENKAKU_HIRA2KATA) {
				zend_argument_value_error(2, "must not combine 'h' and 'C' flags");
				RETURN_THROWS();
			} else if (opt & MBFL_ZENKAKU_KATA2HIRA) {
				zend_argument_value_error(2, "must not combine 'h' and 'c' flags");
				RETURN_THROWS();
			}
		} else if (opt & MBFL_ZEN2HAN_KATAKANA) {
			if (opt & MBFL_ZENKAKU_HIRA2KATA) {
				zend_argument_value_error(2, "must not combine 'k' and 'C' flags");
				RETURN_THROWS();
			} else if (opt & MBFL_ZENKAKU_KATA2HIRA) {
				zend_argument_value_error(2, "must not combine 'k' and 'c' flags");
				RETURN_THROWS();
			}
		}
	} else {
		opt = MBFL_HAN2ZEN_KATAKANA | MBFL_HAN2ZEN_GLUE;
	}

	const mbfl_encoding *enc = php_mb_get_encoding(encname, 3);
	if (!enc) {
		RETURN_THROWS();
	}

	RETVAL_STR(jp_kana_convert(str, enc, opt));
}

static int mb_recursive_encoder_detector_feed(mbfl_encoding_detector *identd, zval *var, bool *recursion_error) /* {{{ */
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
				*recursion_error = true;
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

static bool mb_recursive_convert_variable(zval *var, const mbfl_encoding* from_encoding, const mbfl_encoding* to_encoding)
{
	HashTable *ht;
	zval *entry, *orig_var;

	orig_var = var;
	ZVAL_DEREF(var);

	if (Z_TYPE_P(var) == IS_STRING) {
		zend_string *ret = php_mb_convert_encoding_ex(Z_STRVAL_P(var), Z_STRLEN_P(var), to_encoding, from_encoding);
		zval_ptr_dtor(orig_var);
		ZVAL_STR(orig_var, ret);
	} else if (Z_TYPE_P(var) == IS_ARRAY || Z_TYPE_P(var) == IS_OBJECT) {
		if (Z_TYPE_P(var) == IS_ARRAY) {
			SEPARATE_ARRAY(var);
		}
		if (Z_REFCOUNTED_P(var)) {
			if (Z_IS_RECURSIVE_P(var)) {
				return true;
			}
			Z_PROTECT_RECURSION_P(var);
		}

		ht = HASH_OF(var);
		if (ht != NULL) {
			ZEND_HASH_FOREACH_VAL_IND(ht, entry) {
				if (mb_recursive_convert_variable(entry, from_encoding, to_encoding)) {
					if (Z_REFCOUNTED_P(var)) {
						Z_UNPROTECT_RECURSION_P(var);
					}
					return true;
				}
			} ZEND_HASH_FOREACH_END();
		}

		if (Z_REFCOUNTED_P(var)) {
			Z_UNPROTECT_RECURSION_P(var);
		}
	}

	return false;
}

/* {{{ Converts the string resource in variables to desired encoding */
PHP_FUNCTION(mb_convert_variables)
{
	zval *args;
	zend_string *to_enc_str;
	zend_string *from_enc_str;
	HashTable *from_enc_ht;
	const mbfl_encoding *from_encoding, *to_encoding;
	mbfl_encoding_detector *identd;
	int n, argc;
	size_t elistsz;
	const mbfl_encoding **elist;
	bool recursion_error = false;

	ZEND_PARSE_PARAMETERS_START(3, -1)
		Z_PARAM_STR(to_enc_str)
		Z_PARAM_ARRAY_HT_OR_STR(from_enc_ht, from_enc_str)
		Z_PARAM_VARIADIC('+', args, argc)
	ZEND_PARSE_PARAMETERS_END();

	/* new encoding */
	to_encoding = php_mb_get_encoding(to_enc_str, 1);
	if (!to_encoding) {
		RETURN_THROWS();
	}

	from_encoding = MBSTRG(current_internal_encoding);

	/* pre-conversion encoding */
	if (from_enc_ht) {
		if (php_mb_parse_encoding_array(from_enc_ht, &elist, &elistsz, 2) == FAILURE) {
			RETURN_THROWS();
		}
	} else {
		if (php_mb_parse_encoding_list(ZSTR_VAL(from_enc_str), ZSTR_LEN(from_enc_str), &elist, &elistsz, /* persistent */ 0, /* arg_num */ 2, /* allow_pass_encoding */ 0) == FAILURE) {
			RETURN_THROWS();
		}
	}

	if (elistsz == 0) {
		efree(ZEND_VOIDP(elist));
		zend_argument_value_error(2, "must specify at least one encoding");
		RETURN_THROWS();
	}

	if (elistsz == 1) {
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
				efree(ZEND_VOIDP(elist));
				php_error_docref(NULL, E_WARNING, "Cannot handle recursive references");
				RETURN_FALSE;
			}
		}

		if (!from_encoding) {
			php_error_docref(NULL, E_WARNING, "Unable to detect encoding");
			efree(ZEND_VOIDP(elist));
			RETURN_FALSE;
		}
	}

	efree(ZEND_VOIDP(elist));

	/* convert */
	n = 0;
	while (n < argc) {
		zval *zv = &args[n];
		ZVAL_DEREF(zv);
		recursion_error = mb_recursive_convert_variable(zv, from_encoding, to_encoding);
		if (recursion_error) {
			break;
		}
		n++;
	}

	if (recursion_error) {
		php_error_docref(NULL, E_WARNING, "Cannot handle recursive references");
		RETURN_FALSE;
	}

	RETURN_STRING(from_encoding->name);
}
/* }}} */

/* HTML numeric entities */

/* Convert PHP array to data structure required by mbfl_html_numeric_entity */
static uint32_t *make_conversion_map(HashTable *target_hash, int *convmap_size)
{
	zval *hash_entry;

	int n_elems = zend_hash_num_elements(target_hash);
	if (n_elems % 4 != 0) {
		zend_argument_value_error(2, "must have a multiple of 4 elements");
		return NULL;
	}

	uint32_t *convmap = (uint32_t*)safe_emalloc(n_elems, sizeof(uint32_t), 0);
	uint32_t *mapelm = convmap;

	ZEND_HASH_FOREACH_VAL(target_hash, hash_entry) {
		*mapelm++ = zval_get_long(hash_entry);
	} ZEND_HASH_FOREACH_END();

	*convmap_size = n_elems / 4;
	return convmap;
}

static bool html_numeric_entity_convert(uint32_t w, uint32_t *convmap, int mapsize, uint32_t *retval)
{
	uint32_t *convmap_end = convmap + (mapsize * 4);

	for (uint32_t *mapelm = convmap; mapelm < convmap_end; mapelm += 4) {
		uint32_t lo_code = mapelm[0];
		uint32_t hi_code = mapelm[1];
		uint32_t offset  = mapelm[2];
		uint32_t mask    = mapelm[3];

		if (w >= lo_code && w <= hi_code) {
			/* This wchar falls inside one of the ranges which should be
			 * converted to HTML entities */
			*retval = (w + offset) & mask;
			return true;
		}
	}

	/* None of the ranges matched */
	return false;
}

static zend_string* html_numeric_entity_encode(zend_string *input, const mbfl_encoding *encoding, uint32_t *convmap, int mapsize, bool hex)
{
	/* Each wchar which we get from decoding the input string may become up to
	 * 13 wchars when we convert it to an HTML entity */
	uint32_t wchar_buf[32], converted_buf[32 * 13];
	unsigned char entity[16]; /* For converting wchars to hex/decimal string */

	unsigned int state = 0;
	unsigned char *in = (unsigned char*)ZSTR_VAL(input);
	size_t in_len = ZSTR_LEN(input);

	mb_convert_buf buf;
	mb_convert_buf_init(&buf, in_len, MBSTRG(current_filter_illegal_substchar), MBSTRG(current_filter_illegal_mode));

	while (in_len) {
		/* Convert input string to wchars, up to 32 at a time */
		size_t out_len = encoding->to_wchar(&in, &in_len, wchar_buf, 32, &state);
		ZEND_ASSERT(out_len <= 32);
		uint32_t *converted = converted_buf;

		/* Run through wchars and see if any of them fall into the ranges
		 * which we want to convert to HTML entities */
		for (int i = 0; i < out_len; i++) {
			uint32_t w = wchar_buf[i];

			if (html_numeric_entity_convert(w, convmap, mapsize, &w)) {
				*converted++ = '&';
				*converted++ = '#';
				if (hex) {
					*converted++ = 'x';
				}

				/* Convert wchar to decimal/hex string */
				if (w == 0) {
					*converted++ = '0';
				} else {
					unsigned char *p = entity + sizeof(entity);
					if (hex) {
						while (w > 0) {
							*(--p) = "0123456789ABCDEF"[w & 0xF];
							w >>= 4;
						}
					} else {
						while (w > 0) {
							*(--p) = "0123456789"[w % 10];
							w /= 10;
						}
					}
					while (p < entity + sizeof(entity)) {
						*converted++ = *p++;
					}
				}

				*converted++ = ';';
			} else {
				*converted++ = w;
			}
		}

		ZEND_ASSERT(converted <= converted_buf + sizeof(converted_buf)/sizeof(*converted_buf));
		encoding->from_wchar(converted_buf, converted - converted_buf, &buf, !in_len);
	}

	return mb_convert_buf_result(&buf);
}

/* {{{ Converts specified characters to HTML numeric entities */
PHP_FUNCTION(mb_encode_numericentity)
{
	zend_string *encoding = NULL, *str;
	int mapsize;
	HashTable *target_hash;
	bool is_hex = false;

	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STR(str)
		Z_PARAM_ARRAY_HT(target_hash)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(encoding)
		Z_PARAM_BOOL(is_hex)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *enc = php_mb_get_encoding(encoding, 3);
	if (!enc) {
		RETURN_THROWS();
	}

	uint32_t *convmap = make_conversion_map(target_hash, &mapsize);
	if (convmap == NULL) {
		RETURN_THROWS();
	}

	RETVAL_STR(html_numeric_entity_encode(str, enc, convmap, mapsize, is_hex));
	efree(convmap);
}
/* }}} */

static bool html_numeric_entity_deconvert(uint32_t number, uint32_t *convmap, int mapsize, uint32_t *retval)
{
	uint32_t *convmap_end = convmap + (mapsize * 4);

	for (uint32_t *mapelm = convmap; mapelm < convmap_end; mapelm += 4) {
		uint32_t lo_code = mapelm[0];
		uint32_t hi_code = mapelm[1];
		uint32_t offset  = mapelm[2];
		uint32_t codepoint = number - offset;
		if (codepoint >= lo_code && codepoint <= hi_code) {
			*retval = codepoint;
			return true;
		}
	}

	return false;
}

#define DEC_ENTITY_MINLEN 3  /* For "&#" and 1 decimal digit */
#define HEX_ENTITY_MINLEN 4  /* For "&#x" and 1 hexadecimal digit */
#define DEC_ENTITY_MAXLEN 12 /* For "&#" and 10 decimal digits */
#define HEX_ENTITY_MAXLEN 11 /* For "&#x" and 8 hexadecimal digits */

static zend_string* html_numeric_entity_decode(zend_string *input, const mbfl_encoding *encoding, uint32_t *convmap, int mapsize)
{
	uint32_t wchar_buf[128], converted_buf[128];

	unsigned int state = 0;
	unsigned char *in = (unsigned char*)ZSTR_VAL(input);
	size_t in_len = ZSTR_LEN(input);

	mb_convert_buf buf;
	mb_convert_buf_init(&buf, in_len, MBSTRG(current_filter_illegal_substchar), MBSTRG(current_filter_illegal_mode));

	/* Decode input string from bytes to wchars one 128-wchar buffer at a time, then deconvert HTML entities,
	 * copying the deconverted wchars to a second buffer, then convert back to original encoding from the
	 * 2nd 'converted' buffer.
	 *
	 * Tricky part: an HTML entity might be truncated at the end of the wchar buffer; the remaining
	 * part could come in the next buffer of wchars. To deal with this problem, when we find what looks
	 * like an HTML entity, we scan to see if it terminates before the end of the wchar buffer or not.
	 * If not, we copy it to the beginning of the wchar buffer, and tell the input conversion routine
	 * to store the next batch of wchars after it.
	 *
	 * Optimization: Scan for &, and if we don't find it anywhere, don't even bother copying the
	 * wchars from the 1st buffer to the 2nd one.
	 *
	 * 'converted_buf' is big enough that the deconverted wchars will *always* fit in it, so we don't
	 * have to do bounds checks when writing wchars into it.
	 */

	unsigned int wchar_buf_offset = 0;

	while (in_len) {
		/* Leave space for sentinel at the end of the buffer */
		size_t out_len = encoding->to_wchar(&in, &in_len, wchar_buf + wchar_buf_offset, 127 - wchar_buf_offset, &state);
		out_len += wchar_buf_offset;
		ZEND_ASSERT(out_len <= 127);
		wchar_buf[out_len] = '&'; /* Sentinel, to avoid bounds checks */

		uint32_t *p, *converted;

		/* Scan for & first; however, if `wchar_buf_offset` > 0, then definitely & will
		 * be there (in `wchar_buf[0]`), so don't bother in that case */
		if (wchar_buf_offset == 0) {
			p = wchar_buf;
			while (*p != '&')
				p++;
			if (p == wchar_buf + out_len) {
				/* No HTML entities in this buffer */
				encoding->from_wchar(wchar_buf, out_len, &buf, !in_len);
				continue;
			}

			/* Copy over the prefix with no & which we already scanned */
			memcpy(converted_buf, wchar_buf, (p - wchar_buf) * 4);
			converted = converted_buf + (p - wchar_buf);
		} else {
			p = wchar_buf;
			converted = converted_buf;
		}

found_ampersand:
		ZEND_ASSERT(*p == '&');
		uint32_t *p2 = p;

		/* These tests can't overrun end of buffer, because we have a '&' sentinel there */
		if (*++p2 == '#') {
			if (*++p2 == 'x') {
				/* Possible hex entity */
				uint32_t w = *++p2;
				while ((w >= '0' && w <= '9') || (w >= 'A' && w <= 'F') || (w >= 'a' && w <= 'f'))
					w = *++p2;
				if ((p2 == wchar_buf + out_len) && in_len && (p2 - p) <= HEX_ENTITY_MAXLEN) {
					/* We hit the end of the buffer while reading digits, and
					 * more wchars are still coming in the next buffer
					 * Reprocess this identity on next iteration */
					memmove(wchar_buf, p, (p2 - p) * 4);
					wchar_buf_offset = p2 - p;
					goto process_converted_wchars;
				} else if ((p2 - p) < HEX_ENTITY_MINLEN || (p2 - p) > HEX_ENTITY_MAXLEN) {
					/* Invalid entity (too long or "&#x" only) */
					memcpy(converted, p, (p2 - p) * 4);
					converted += p2 - p;
				} else {
					/* Valid hexadecimal entity */
					uint32_t value = 0, *p3 = p + 3;
					while (p3 < p2) {
						w = *p3++;
						if (w <= '9') {
							value = (value * 16) + (w - '0');
						} else if (w >= 'a') {
							value = (value * 16) + 10 + (w - 'a');
						} else {
							value = (value * 16) + 10 + (w - 'A');
						}
					}
					if (html_numeric_entity_deconvert(value, convmap, mapsize, converted)) {
						converted++;
						if (*p2 == ';')
							p2++;
					} else {
						memcpy(converted, p, (p2 - p) * 4);
						converted += p2 - p;
					}
				}
			} else {
				/* Possible decimal entity */
				uint32_t w = *p2;
				while (w >= '0' && w <= '9')
					w = *++p2;
				if ((p2 == wchar_buf + out_len) && in_len && (p2 - p) <= DEC_ENTITY_MAXLEN) {
					/* The number of digits was legal (no more than 10 decimal digits)
					 * Reprocess this identity on next iteration of main loop */
					memmove(wchar_buf, p, (p2 - p) * 4);
					wchar_buf_offset = p2 - p;
					goto process_converted_wchars;
				} else if ((p2 - p) < DEC_ENTITY_MINLEN || (p2 - p) > DEC_ENTITY_MAXLEN) {
					/* Invalid entity (too long or "&#" only) */
					memcpy(converted, p, (p2 - p) * 4);
					converted += p2 - p;
				} else {
					/* Valid decimal entity */
					uint32_t value = 0, *p3 = p + 2;
					while (p3 < p2) {
						/* If unsigned integer overflow would occur in the below
						 * multiplication by 10, this entity is no good
						 * 0x19999999 is 1/10th of 0xFFFFFFFF */
						if (value > 0x19999999) {
							memcpy(converted, p, (p2 - p) * 4);
							converted += p2 - p;
							goto decimal_entity_too_big;
						}
						value = (value * 10) + (*p3++ - '0');
					}
					if (html_numeric_entity_deconvert(value, convmap, mapsize, converted)) {
						converted++;
						if (*p2 == ';')
							p2++;
					} else {
						memcpy(converted, p, (p2 - p) * 4);
						converted += p2 - p;
					}
				}
			}
		} else if ((p2 == wchar_buf + out_len) && in_len) {
			/* Corner case: & at end of buffer */
			wchar_buf[0] = '&';
			wchar_buf_offset = 1;
			goto process_converted_wchars;
		} else {
			*converted++ = '&';
		}
decimal_entity_too_big:

		/* Starting to scan a new section of the wchar buffer
		 * 'p2' is pointing at the next wchar which needs to be processed */
		p = p2;
		while (*p2 != '&')
			p2++;

		if (p2 > p) {
			memcpy(converted, p, (p2 - p) * 4);
			converted += p2 - p;
			p = p2;
		}

		if (p < wchar_buf + out_len)
			goto found_ampersand;

		/* We do not have any wchars remaining at the end of this buffer which
		 * we need to reprocess on the next call */
		wchar_buf_offset = 0;
process_converted_wchars:
		ZEND_ASSERT(converted <= converted_buf + 128);
		encoding->from_wchar(converted_buf, converted - converted_buf, &buf, !in_len);
	}

	return mb_convert_buf_result(&buf);
}

/* {{{ Converts HTML numeric entities to character code */
PHP_FUNCTION(mb_decode_numericentity)
{
	zend_string *encoding = NULL, *str;
	int mapsize;
	HashTable *target_hash;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(str)
		Z_PARAM_ARRAY_HT(target_hash)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(encoding)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *enc = php_mb_get_encoding(encoding, 3);
	if (!enc) {
		RETURN_THROWS();
	}

	uint32_t *convmap = make_conversion_map(target_hash, &mapsize);
	if (convmap == NULL) {
		RETURN_THROWS();
	}

	RETVAL_STR(html_numeric_entity_decode(str, enc, convmap, mapsize));
	efree(convmap);
}
/* }}} */

/* {{{ Sends an email message with MIME scheme */
#define CRLF "\r\n"

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
						ZEND_FALLTHROUGH;

					case 3:
						if (crlf_state == -1) {
							if(token && token_pos > 0) {
								fld_val = zend_string_init(token, token_pos, 0);
							}

							if (fld_name != NULL && fld_val != NULL) {
								zval val;
								zend_str_tolower(ZSTR_VAL(fld_name), ZSTR_LEN(fld_name));
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
			zend_str_tolower(ZSTR_VAL(fld_name), ZSTR_LEN(fld_name));
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
	zend_string *extra_cmd = NULL;
	HashTable *headers_ht = NULL;
	zend_string *str_headers = NULL;
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
	const mbfl_encoding *tran_cs,	/* transfer text charset */
						*head_enc,	/* header transfer encoding */
						*body_enc;	/* body transfer encoding */
	mbfl_memory_device device;	/* automatic allocateable buffer for additional header */
	const mbfl_language *lang;
	int err = 0;
	HashTable ht_headers;
	zval *s;
	extern void mbfl_memory_device_unput(mbfl_memory_device *device);

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

	ZEND_PARSE_PARAMETERS_START(3, 5)
		Z_PARAM_PATH(to, to_len)
		Z_PARAM_PATH(subject, subject_len)
		Z_PARAM_PATH(message, message_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_STR(headers_ht, str_headers)
		Z_PARAM_PATH_STR_OR_NULL(extra_cmd)
	ZEND_PARSE_PARAMETERS_END();

	if (str_headers) {
		if (strlen(ZSTR_VAL(str_headers)) != ZSTR_LEN(str_headers)) {
			zend_argument_value_error(4, "must not contain any null bytes");
			RETURN_THROWS();
		}
		str_headers = php_trim(str_headers, NULL, 0, 2);
	} else if (headers_ht) {
		str_headers = php_mail_build_headers(headers_ht);
		if (EG(exception)) {
			RETURN_THROWS();
		}
	}

	zend_hash_init(&ht_headers, 0, NULL, ZVAL_PTR_DTOR, 0);

	if (str_headers != NULL) {
		_php_mbstr_parse_mail_headers(&ht_headers, ZSTR_VAL(str_headers), ZSTR_LEN(str_headers));
	}

	if ((s = zend_hash_str_find(&ht_headers, "content-type", sizeof("content-type") - 1))) {
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

	if ((s = zend_hash_str_find(&ht_headers, "content-transfer-encoding", sizeof("content-transfer-encoding") - 1))) {
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
				 * To prevent these separators from being replaced with a space, we skip over them. */
				if (to_r[i] == '\r' && to_r[i + 1] == '\n' && (to_r[i + 2] == ' ' || to_r[i + 2] == '\t')) {
					i += 2;
					while (to_r[i + 1] == ' ' || to_r[i + 1] == '\t') {
						i++;
					}
					continue;
				}

				to_r[i] = ' ';
			}
		}
	} else {
		to_r = to;
	}

	/* Subject: */
	orig_str.val = (unsigned char *)subject;
	orig_str.len = subject_len;
	orig_str.encoding = MBSTRG(current_internal_encoding);
	if (orig_str.encoding->no_encoding == mbfl_no_encoding_invalid
			|| orig_str.encoding->no_encoding == mbfl_no_encoding_pass) {
		orig_str.encoding = mbfl_identify_encoding(&orig_str, MBSTRG(current_detect_order_list), MBSTRG(current_detect_order_list_size), MBSTRG(strict_detection));
	}
	pstr = mbfl_mime_header_encode(&orig_str, &conv_str, tran_cs, head_enc, CRLF, sizeof("Subject: [PHP-jp nnnnnnnn]" CRLF) - 1);
	if (pstr != NULL) {
		subject_buf = subject = (char *)pstr->val;
	}

	/* message body */
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
			mbfl_memory_device_strncat(&device, CRLF, sizeof(CRLF)-1);
		}
		zend_string_release_ex(str_headers, 0);
	}

	if (!zend_hash_str_exists(&ht_headers, "mime-version", sizeof("mime-version") - 1)) {
		mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER1, sizeof(PHP_MBSTR_MAIL_MIME_HEADER1) - 1);
		mbfl_memory_device_strncat(&device, CRLF, sizeof(CRLF)-1);
	}

	if (!suppressed_hdrs.cnt_type) {
		mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER2, sizeof(PHP_MBSTR_MAIL_MIME_HEADER2) - 1);

		p = (char *)mbfl_encoding_preferred_mime_name(tran_cs);
		if (p != NULL) {
			mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER3, sizeof(PHP_MBSTR_MAIL_MIME_HEADER3) - 1);
			mbfl_memory_device_strcat(&device, p);
		}
		mbfl_memory_device_strncat(&device, CRLF, sizeof(CRLF)-1);
	}
	if (!suppressed_hdrs.cnt_trans_enc) {
		mbfl_memory_device_strncat(&device, PHP_MBSTR_MAIL_MIME_HEADER4, sizeof(PHP_MBSTR_MAIL_MIME_HEADER4) - 1);
		p = (char *)mbfl_encoding_preferred_mime_name(body_enc);
		if (p == NULL) {
			p = "7bit";
		}
		mbfl_memory_device_strcat(&device, p);
		mbfl_memory_device_strncat(&device, CRLF, sizeof(CRLF)-1);
	}

	mbfl_memory_device_unput(&device);
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

#undef CRLF
#undef MAIL_ASCIIZ_CHECK_MBSTRING
#undef PHP_MBSTR_MAIL_MIME_HEADER1
#undef PHP_MBSTR_MAIL_MIME_HEADER2
#undef PHP_MBSTR_MAIL_MIME_HEADER3
#undef PHP_MBSTR_MAIL_MIME_HEADER4
/* }}} */

/* {{{ Returns the current settings of mbstring */
PHP_FUNCTION(mb_get_info)
{
	zend_string *type = NULL;
	size_t n;
	char *name;
	zval row;
	const mbfl_language *lang = mbfl_no2language(MBSTRG(language));
	const mbfl_encoding **entry;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(type)
	ZEND_PARSE_PARAMETERS_END();

	if (!type || zend_string_equals_literal_ci(type, "all")) {
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
			array_init(&row);
			for (i = 0; i < n; i++) {
				add_next_index_string(&row, (*entry)->name);
				entry++;
			}
			add_assoc_zval(return_value, "detect_order", &row);
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
	} else if (zend_string_equals_literal_ci(type, "internal_encoding")) {
		if (MBSTRG(current_internal_encoding)) {
			RETVAL_STRING((char *)MBSTRG(current_internal_encoding)->name);
		}
	} else if (zend_string_equals_literal_ci(type, "http_input")) {
		if (MBSTRG(http_input_identify)) {
			RETVAL_STRING((char *)MBSTRG(http_input_identify)->name);
		}
	} else if (zend_string_equals_literal_ci(type, "http_output")) {
		if (MBSTRG(current_http_output_encoding)) {
			RETVAL_STRING((char *)MBSTRG(current_http_output_encoding)->name);
		}
	} else if (zend_string_equals_literal_ci(type, "http_output_conv_mimetypes")) {
		if ((name = (char *)zend_ini_string("mbstring.http_output_conv_mimetypes", sizeof("mbstring.http_output_conv_mimetypes") - 1, 0)) != NULL) {
			RETVAL_STRING(name);
		}
	} else if (zend_string_equals_literal_ci(type, "mail_charset")) {
		if (lang != NULL && (name = (char *)mbfl_no_encoding2name(lang->mail_charset)) != NULL) {
			RETVAL_STRING(name);
		}
	} else if (zend_string_equals_literal_ci(type, "mail_header_encoding")) {
		if (lang != NULL && (name = (char *)mbfl_no_encoding2name(lang->mail_header_encoding)) != NULL) {
			RETVAL_STRING(name);
		}
	} else if (zend_string_equals_literal_ci(type, "mail_body_encoding")) {
		if (lang != NULL && (name = (char *)mbfl_no_encoding2name(lang->mail_body_encoding)) != NULL) {
			RETVAL_STRING(name);
		}
	} else if (zend_string_equals_literal_ci(type, "illegal_chars")) {
		RETVAL_LONG(MBSTRG(illegalchars));
	} else if (zend_string_equals_literal_ci(type, "encoding_translation")) {
		if (MBSTRG(encoding_translation)) {
			RETVAL_STRING("On");
		} else {
			RETVAL_STRING("Off");
		}
	} else if (zend_string_equals_literal_ci(type, "language")) {
		if ((name = (char *)mbfl_no_language2name(MBSTRG(language))) != NULL) {
			RETVAL_STRING(name);
		}
	} else if (zend_string_equals_literal_ci(type, "detect_order")) {
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
	} else if (zend_string_equals_literal_ci(type, "substitute_character")) {
		if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
			RETVAL_STRING("none");
		} else if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG) {
			RETVAL_STRING("long");
		} else if (MBSTRG(current_filter_illegal_mode) == MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY) {
			RETVAL_STRING("entity");
		} else {
			RETVAL_LONG(MBSTRG(current_filter_illegal_substchar));
		}
	} else if (zend_string_equals_literal_ci(type, "strict_detection")) {
		if (MBSTRG(strict_detection)) {
			RETVAL_STRING("On");
		} else {
			RETVAL_STRING("Off");
		}
	} else {
		// TODO Convert to ValueError
		RETURN_FALSE;
	}
}
/* }}} */

MBSTRING_API bool php_mb_check_encoding(const char *input, size_t length, const mbfl_encoding *encoding)
{
	uint32_t wchar_buf[128];
	unsigned char *in = (unsigned char*)input;
	unsigned int state = 0;

	/* If the input string is not encoded in the given encoding, there is a significant chance
	 * that this will be seen in the first bytes. Therefore, rather than converting an entire
	 * buffer of 128 codepoints, convert and check just a few codepoints first */
	size_t out_len = encoding->to_wchar(&in, &length, wchar_buf, 8, &state);
	ZEND_ASSERT(out_len <= 8);
	for (int i = 0; i < out_len; i++) {
		if (wchar_buf[i] == MBFL_BAD_INPUT) {
			return false;
		}
	}

	while (length) {
		out_len = encoding->to_wchar(&in, &length, wchar_buf, 128, &state);
		ZEND_ASSERT(out_len <= 128);
		for (int i = 0; i < out_len; i++) {
			if (wchar_buf[i] == MBFL_BAD_INPUT) {
				return false;
			}
		}
	}

	return true;
}

static bool mb_check_str_encoding(zend_string *str, const mbfl_encoding *encoding)
{
	if (encoding == &mbfl_encoding_utf8) {
		if (GC_FLAGS(str) & IS_STR_VALID_UTF8) {
			return true;
		}
		bool result = php_mb_check_encoding(ZSTR_VAL(str), ZSTR_LEN(str), encoding);
		if (result && !ZSTR_IS_INTERNED(str)) {
			GC_ADD_FLAGS(str, IS_STR_VALID_UTF8);
		}
		return result;
	} else {
		return php_mb_check_encoding(ZSTR_VAL(str), ZSTR_LEN(str), encoding);
	}
}

static int php_mb_check_encoding_recursive(HashTable *vars, const mbfl_encoding *encoding)
{
	zend_long idx;
	zend_string *key;
	zval *entry;
	int valid = 1;

	(void)(idx); /* Suppress spurious compiler warning that `idx` is not used */

	if (GC_IS_RECURSIVE(vars)) {
		php_error_docref(NULL, E_WARNING, "Cannot not handle circular references");
		return 0;
	}
	GC_TRY_PROTECT_RECURSION(vars);
	ZEND_HASH_FOREACH_KEY_VAL(vars, idx, key, entry) {
		ZVAL_DEREF(entry);
		if (key) {
			if (!mb_check_str_encoding(key, encoding)) {
				valid = 0;
				break;
			}
		}
		switch (Z_TYPE_P(entry)) {
			case IS_STRING:
				if (!mb_check_str_encoding(Z_STR_P(entry), encoding)) {
					valid = 0;
					break;
				}
				break;
			case IS_ARRAY:
				if (!php_mb_check_encoding_recursive(Z_ARRVAL_P(entry), encoding)) {
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
	return valid;
}

/* {{{ Check if the string is valid for the specified encoding */
PHP_FUNCTION(mb_check_encoding)
{
	zend_string *input_str = NULL, *enc = NULL;
	HashTable *input_ht = NULL;
	const mbfl_encoding *encoding;

	ZEND_PARSE_PARAMETERS_START(0, 2)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_STR_OR_NULL(input_ht, input_str)
		Z_PARAM_STR_OR_NULL(enc)
	ZEND_PARSE_PARAMETERS_END();

	encoding = php_mb_get_encoding(enc, 2);
	if (!encoding) {
		RETURN_THROWS();
	}

	if (input_ht) {
		RETURN_BOOL(php_mb_check_encoding_recursive(input_ht, encoding));
	} else if (input_str) {
		RETURN_BOOL(mb_check_str_encoding(input_str, encoding));
	} else {
		php_error_docref(NULL, E_DEPRECATED,
			"Calling mb_check_encoding() without argument is deprecated");

		/* FIXME: Actually check all inputs, except $_FILES file content. */
		RETURN_BOOL(MBSTRG(illegalchars) == 0);
	}
}
/* }}} */


static inline zend_long php_mb_ord(const char *str, size_t str_len, zend_string *enc_name,
	const uint32_t enc_name_arg_num)
{
	const mbfl_encoding *enc;
	enum mbfl_no_encoding no_enc;

	ZEND_ASSERT(str_len > 0);

	enc = php_mb_get_encoding(enc_name, enc_name_arg_num);
	if (!enc) {
		return -2;
	}

	no_enc = enc->no_encoding;
	if (php_mb_is_unsupported_no_encoding(no_enc)) {
		zend_value_error("mb_ord() does not support the \"%s\" encoding", enc->name);
		return -2;
	}

	/* Some legacy text encodings have a minimum required wchar buffer size;
	 * the ones which need the most are SJIS-Mac, UTF-7, and UTF7-IMAP */
	uint32_t wchar_buf[MBSTRING_MIN_WCHAR_BUFSIZE];
	unsigned int state = 0;
	size_t out_len = enc->to_wchar((unsigned char**)&str, &str_len, wchar_buf, MBSTRING_MIN_WCHAR_BUFSIZE, &state);
	ZEND_ASSERT(out_len <= MBSTRING_MIN_WCHAR_BUFSIZE);

	if (!out_len || wchar_buf[0] == MBFL_BAD_INPUT) {
		return -1;
	}
	return wchar_buf[0];
}


/* {{{ */
PHP_FUNCTION(mb_ord)
{
	char *str;
	size_t str_len;
	zend_string *enc = NULL;
	zend_long cp;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(enc)
	ZEND_PARSE_PARAMETERS_END();

	if (str_len == 0) {
		zend_argument_value_error(1, "must not be empty");
		RETURN_THROWS();
	}

	cp = php_mb_ord(str, str_len, enc, 2);

	if (0 > cp) {
		if (cp == -2) {
			RETURN_THROWS();
		}
		RETURN_FALSE;
	}

	RETURN_LONG(cp);
}
/* }}} */


static inline zend_string *php_mb_chr(zend_long cp, zend_string *enc_name, uint32_t enc_name_arg_num)
{
	const mbfl_encoding *enc;
	enum mbfl_no_encoding no_enc;
	zend_string *ret;
	char buf[4];

	enc = php_mb_get_encoding(enc_name, enc_name_arg_num);
	if (!enc) {
		return NULL;
	}

	no_enc = enc->no_encoding;
	if (php_mb_is_unsupported_no_encoding(no_enc)) {
		zend_value_error("mb_chr() does not support the \"%s\" encoding", enc->name);
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

	buf[0] = (cp >> 24) & 0xff;
	buf[1] = (cp >> 16) & 0xff;
	buf[2] = (cp >>  8) & 0xff;
	buf[3] = cp & 0xff;

	long orig_illegalchars = MBSTRG(illegalchars);
	MBSTRG(illegalchars) = 0;
	ret = php_mb_convert_encoding_ex(buf, 4, enc, &mbfl_encoding_ucs4be);

	if (MBSTRG(illegalchars) != 0) {
		zend_string_release(ret);
		ret = NULL;
	}

	MBSTRG(illegalchars) = orig_illegalchars;
	return ret;
}


/* {{{ */
PHP_FUNCTION(mb_chr)
{
	zend_long cp;
	zend_string *enc = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(cp)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(enc)
	ZEND_PARSE_PARAMETERS_END();

	zend_string* ret = php_mb_chr(cp, enc, 2);
	if (ret == NULL) {
		RETURN_FALSE;
	}

	RETURN_STR(ret);
}
/* }}} */

/* {{{ */
PHP_FUNCTION(mb_scrub)
{
	char* str;
	size_t str_len;
	zend_string *enc_name = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(enc_name)
	ZEND_PARSE_PARAMETERS_END();

	const mbfl_encoding *enc = php_mb_get_encoding(enc_name, 2);
	if (!enc) {
		RETURN_THROWS();
	}

	RETURN_STR(php_mb_convert_encoding_ex(str, str_len, enc, enc));
}
/* }}} */


/* {{{ php_mb_populate_current_detect_order_list */
static void php_mb_populate_current_detect_order_list(void)
{
	const mbfl_encoding **entry = 0;
	size_t nentries;

	if (MBSTRG(detect_order_list) && MBSTRG(detect_order_list_size)) {
		nentries = MBSTRG(detect_order_list_size);
		entry = (const mbfl_encoding **)safe_emalloc(nentries, sizeof(mbfl_encoding*), 0);
		memcpy(ZEND_VOIDP(entry), MBSTRG(detect_order_list), sizeof(mbfl_encoding*) * nentries);
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

MBSTRING_API size_t php_mb_mbchar_bytes(const char *s, const mbfl_encoding *enc)
{
	if (enc) {
		if (enc->mblen_table) {
			if (s) {
				return enc->mblen_table[*(unsigned char *)s];
			}
		} else if (enc->flag & MBFL_ENCTYPE_WCS2) {
			return 2;
		} else if (enc->flag & MBFL_ENCTYPE_WCS4) {
			return 4;
		}
	}
	return 1;
}

MBSTRING_API char *php_mb_safe_strrchr(const char *s, unsigned int c, size_t nbytes, const mbfl_encoding *enc)
{
	const char *p = s;
	char *last=NULL;

	if (nbytes == (size_t)-1) {
		size_t nb = 0;

		while (*p != '\0') {
			if (nb == 0) {
				if ((unsigned char)*p == (unsigned char)c) {
					last = (char *)p;
				}
				nb = php_mb_mbchar_bytes(p, enc);
				if (nb == 0) {
					return NULL; /* something is going wrong! */
				}
			}
			--nb;
			++p;
		}
	} else {
		size_t bcnt = nbytes;
		size_t nbytes_char;
		while (bcnt > 0) {
			if ((unsigned char)*p == (unsigned char)c) {
				last = (char *)p;
			}
			nbytes_char = php_mb_mbchar_bytes(p, enc);
			if (bcnt < nbytes_char) {
				return NULL;
			}
			p += nbytes_char;
			bcnt -= nbytes_char;
		}
	}
	return last;
}

MBSTRING_API size_t php_mb_stripos(bool mode, zend_string *haystack, zend_string *needle, zend_long offset, const mbfl_encoding *enc)
{
	/* We're using simple case-folding here, because we'd have to deal with remapping of
	 * offsets otherwise. */
	zend_string *haystack_conv = php_unicode_convert_case(PHP_UNICODE_CASE_FOLD_SIMPLE, ZSTR_VAL(haystack), ZSTR_LEN(haystack), enc, &mbfl_encoding_utf8, MBFL_OUTPUTFILTER_ILLEGAL_MODE_BADUTF8, 0);
	zend_string *needle_conv = php_unicode_convert_case(PHP_UNICODE_CASE_FOLD_SIMPLE, ZSTR_VAL(needle), ZSTR_LEN(needle), enc, &mbfl_encoding_utf8, MBFL_OUTPUTFILTER_ILLEGAL_MODE_BADUTF8, 0);

	size_t n = mb_find_strpos(haystack_conv, needle_conv, &mbfl_encoding_utf8, offset, mode);

	zend_string_free(haystack_conv);
	zend_string_free(needle_conv);

	return n;
}

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
