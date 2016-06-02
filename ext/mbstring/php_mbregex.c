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
   +----------------------------------------------------------------------+
 */

/* $Id$ */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"

#if HAVE_MBREGEX

#include "zend_smart_str.h"
#include "ext/standard/info.h"
#include "php_mbregex.h"
#include "mbstring.h"

#include "php_onig_compat.h" /* must come prior to the oniguruma header */
#include <oniguruma.h>
#undef UChar

ZEND_EXTERN_MODULE_GLOBALS(mbstring)

struct _zend_mb_regex_globals {
	OnigEncoding default_mbctype;
	OnigEncoding current_mbctype;
	HashTable ht_rc;
	zval search_str;
	zval *search_str_val;
	unsigned int search_pos;
	php_mb_regex_t *search_re;
	OnigRegion *search_regs;
	OnigOptionType regex_default_options;
	OnigSyntaxType *regex_default_syntax;
};

#define MBREX(g) (MBSTRG(mb_regex_globals)->g)

/* {{{ static void php_mb_regex_free_cache() */
static void php_mb_regex_free_cache(zval *el) {
	onig_free((php_mb_regex_t *)Z_PTR_P(el));
}
/* }}} */

/* {{{ _php_mb_regex_globals_ctor */
static int _php_mb_regex_globals_ctor(zend_mb_regex_globals *pglobals)
{
	pglobals->default_mbctype = ONIG_ENCODING_UTF8;
	pglobals->current_mbctype = ONIG_ENCODING_UTF8;
	zend_hash_init(&(pglobals->ht_rc), 0, NULL, php_mb_regex_free_cache, 1);
	ZVAL_UNDEF(&pglobals->search_str);
	pglobals->search_re = (php_mb_regex_t*)NULL;
	pglobals->search_pos = 0;
	pglobals->search_regs = (OnigRegion*)NULL;
	pglobals->regex_default_options = ONIG_OPTION_MULTILINE | ONIG_OPTION_SINGLELINE;
	pglobals->regex_default_syntax = ONIG_SYNTAX_RUBY;
	return SUCCESS;
}
/* }}} */

/* {{{ _php_mb_regex_globals_dtor */
static void _php_mb_regex_globals_dtor(zend_mb_regex_globals *pglobals)
{
	zend_hash_destroy(&pglobals->ht_rc);
}
/* }}} */

/* {{{ php_mb_regex_globals_alloc */
zend_mb_regex_globals *php_mb_regex_globals_alloc(void)
{
	zend_mb_regex_globals *pglobals = pemalloc(
			sizeof(zend_mb_regex_globals), 1);
	if (!pglobals) {
		return NULL;
	}
	if (SUCCESS != _php_mb_regex_globals_ctor(pglobals)) {
		pefree(pglobals, 1);
		return NULL;
	}
	return pglobals;
}
/* }}} */

/* {{{ php_mb_regex_globals_free */
void php_mb_regex_globals_free(zend_mb_regex_globals *pglobals)
{
	if (!pglobals) {
		return;
	}
	_php_mb_regex_globals_dtor(pglobals);
	pefree(pglobals, 1);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(mb_regex) */
PHP_MINIT_FUNCTION(mb_regex)
{
	onig_init();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION(mb_regex) */
PHP_MSHUTDOWN_FUNCTION(mb_regex)
{
	onig_end();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION(mb_regex) */
PHP_RINIT_FUNCTION(mb_regex)
{
	return MBSTRG(mb_regex_globals) ? SUCCESS: FAILURE;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION(mb_regex) */
PHP_RSHUTDOWN_FUNCTION(mb_regex)
{
	MBREX(current_mbctype) = MBREX(default_mbctype);

	if (!Z_ISUNDEF(MBREX(search_str))) {
		zval_ptr_dtor(&MBREX(search_str));
		ZVAL_UNDEF(&MBREX(search_str));
	}
	MBREX(search_pos) = 0;

	if (MBREX(search_regs) != NULL) {
		onig_region_free(MBREX(search_regs), 1);
		MBREX(search_regs) = (OnigRegion *)NULL;
	}
	zend_hash_clean(&MBREX(ht_rc));

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION(mb_regex) */
PHP_MINFO_FUNCTION(mb_regex)
{
	char buf[32];
	php_info_print_table_start();
	php_info_print_table_row(2, "Multibyte (japanese) regex support", "enabled");
	snprintf(buf, sizeof(buf), "%d.%d.%d",
			ONIGURUMA_VERSION_MAJOR,
			ONIGURUMA_VERSION_MINOR,
			ONIGURUMA_VERSION_TEENY);
#ifdef PHP_ONIG_BUNDLED
#ifdef USE_COMBINATION_EXPLOSION_CHECK
	php_info_print_table_row(2, "Multibyte regex (oniguruma) backtrack check", "On");
#else	/* USE_COMBINATION_EXPLOSION_CHECK */
	php_info_print_table_row(2, "Multibyte regex (oniguruma) backtrack check", "Off");
#endif	/* USE_COMBINATION_EXPLOSION_CHECK */
#endif /* PHP_BUNDLED_ONIG */
	php_info_print_table_row(2, "Multibyte regex (oniguruma) version", buf);
	php_info_print_table_end();
}
/* }}} */

/*
 * encoding name resolver
 */

/* {{{ encoding name map */
typedef struct _php_mb_regex_enc_name_map_t {
	const char *names;
	OnigEncoding code;
} php_mb_regex_enc_name_map_t;

php_mb_regex_enc_name_map_t enc_name_map[] = {
#ifdef ONIG_ENCODING_EUC_JP
	{
		"EUC-JP\0EUCJP\0X-EUC-JP\0UJIS\0EUCJP\0EUCJP-WIN\0",
		ONIG_ENCODING_EUC_JP
	},
#endif
#ifdef ONIG_ENCODING_UTF8
	{
		"UTF-8\0UTF8\0",
		ONIG_ENCODING_UTF8
	},
#endif
#ifdef ONIG_ENCODING_UTF16_BE
	{
		"UTF-16\0UTF-16BE\0",
		ONIG_ENCODING_UTF16_BE
	},
#endif
#ifdef ONIG_ENCODING_UTF16_LE
	{
		"UTF-16LE\0",
		ONIG_ENCODING_UTF16_LE
	},
#endif
#ifdef ONIG_ENCODING_UTF32_BE
	{
		"UCS-4\0UTF-32\0UTF-32BE\0",
		ONIG_ENCODING_UTF32_BE
	},
#endif
#ifdef ONIG_ENCODING_UTF32_LE
	{
		"UCS-4LE\0UTF-32LE\0",
		ONIG_ENCODING_UTF32_LE
	},
#endif
#ifdef ONIG_ENCODING_SJIS
	{
		"SJIS\0CP932\0MS932\0SHIFT_JIS\0SJIS-WIN\0WINDOWS-31J\0",
		ONIG_ENCODING_SJIS
	},
#endif
#ifdef ONIG_ENCODING_BIG5
	{
		"BIG5\0BIG-5\0BIGFIVE\0CN-BIG5\0BIG-FIVE\0",
		ONIG_ENCODING_BIG5
	},
#endif
#ifdef ONIG_ENCODING_EUC_CN
	{
		"EUC-CN\0EUCCN\0EUC_CN\0GB-2312\0GB2312\0",
		ONIG_ENCODING_EUC_CN
	},
#endif
#ifdef ONIG_ENCODING_EUC_TW
	{
		"EUC-TW\0EUCTW\0EUC_TW\0",
		ONIG_ENCODING_EUC_TW
	},
#endif
#ifdef ONIG_ENCODING_EUC_KR
	{
		"EUC-KR\0EUCKR\0EUC_KR\0",
		ONIG_ENCODING_EUC_KR
	},
#endif
#if defined(ONIG_ENCODING_KOI8) && !PHP_ONIG_BAD_KOI8_ENTRY
	{
		"KOI8\0KOI-8\0",
		ONIG_ENCODING_KOI8
	},
#endif
#ifdef ONIG_ENCODING_KOI8_R
	{
		"KOI8R\0KOI8-R\0KOI-8R\0",
		ONIG_ENCODING_KOI8_R
	},
#endif
#ifdef ONIG_ENCODING_ISO_8859_1
	{
		"ISO-8859-1\0ISO8859-1\0ISO_8859_1\0ISO8859_1\0",
		ONIG_ENCODING_ISO_8859_1
	},
#endif
#ifdef ONIG_ENCODING_ISO_8859_2
	{
		"ISO-8859-2\0ISO8859-2\0ISO_8859_2\0ISO8859_2\0",
		ONIG_ENCODING_ISO_8859_2
	},
#endif
#ifdef ONIG_ENCODING_ISO_8859_3
	{
		"ISO-8859-3\0ISO8859-3\0ISO_8859_3\0ISO8859_3\0",
		ONIG_ENCODING_ISO_8859_3
	},
#endif
#ifdef ONIG_ENCODING_ISO_8859_4
	{
		"ISO-8859-4\0ISO8859-4\0ISO_8859_4\0ISO8859_4\0",
		ONIG_ENCODING_ISO_8859_4
	},
#endif
#ifdef ONIG_ENCODING_ISO_8859_5
	{
		"ISO-8859-5\0ISO8859-5\0ISO_8859_5\0ISO8859_5\0",
		ONIG_ENCODING_ISO_8859_5
	},
#endif
#ifdef ONIG_ENCODING_ISO_8859_6
	{
		"ISO-8859-6\0ISO8859-6\0ISO_8859_6\0ISO8859_6\0",
		ONIG_ENCODING_ISO_8859_6
	},
#endif
#ifdef ONIG_ENCODING_ISO_8859_7
	{
		"ISO-8859-7\0ISO8859-7\0ISO_8859_7\0ISO8859_7\0",
		ONIG_ENCODING_ISO_8859_7
	},
#endif
#ifdef ONIG_ENCODING_ISO_8859_8
	{
		"ISO-8859-8\0ISO8859-8\0ISO_8859_8\0ISO8859_8\0",
		ONIG_ENCODING_ISO_8859_8
	},
#endif
#ifdef ONIG_ENCODING_ISO_8859_9
	{
		"ISO-8859-9\0ISO8859-9\0ISO_8859_9\0ISO8859_9\0",
		ONIG_ENCODING_ISO_8859_9
	},
#endif
#ifdef ONIG_ENCODING_ISO_8859_10
	{
		"ISO-8859-10\0ISO8859-10\0ISO_8859_10\0ISO8859_10\0",
		ONIG_ENCODING_ISO_8859_10
	},
#endif
#ifdef ONIG_ENCODING_ISO_8859_11
	{
		"ISO-8859-11\0ISO8859-11\0ISO_8859_11\0ISO8859_11\0",
		ONIG_ENCODING_ISO_8859_11
	},
#endif
#ifdef ONIG_ENCODING_ISO_8859_13
	{
		"ISO-8859-13\0ISO8859-13\0ISO_8859_13\0ISO8859_13\0",
		ONIG_ENCODING_ISO_8859_13
	},
#endif
#ifdef ONIG_ENCODING_ISO_8859_14
	{
		"ISO-8859-14\0ISO8859-14\0ISO_8859_14\0ISO8859_14\0",
		ONIG_ENCODING_ISO_8859_14
	},
#endif
#ifdef ONIG_ENCODING_ISO_8859_15
	{
		"ISO-8859-15\0ISO8859-15\0ISO_8859_15\0ISO8859_15\0",
		ONIG_ENCODING_ISO_8859_15
	},
#endif
#ifdef ONIG_ENCODING_ISO_8859_16
	{
		"ISO-8859-16\0ISO8859-16\0ISO_8859_16\0ISO8859_16\0",
		ONIG_ENCODING_ISO_8859_16
	},
#endif
#ifdef ONIG_ENCODING_ASCII
	{
		"ASCII\0US-ASCII\0US_ASCII\0ISO646\0",
		ONIG_ENCODING_ASCII
	},
#endif
	{ NULL, ONIG_ENCODING_UNDEF }
};
/* }}} */

/* {{{ php_mb_regex_name2mbctype */
static OnigEncoding _php_mb_regex_name2mbctype(const char *pname)
{
	const char *p;
	php_mb_regex_enc_name_map_t *mapping;

	if (pname == NULL || !*pname) {
		return ONIG_ENCODING_UNDEF;
	}

	for (mapping = enc_name_map; mapping->names != NULL; mapping++) {
		for (p = mapping->names; *p != '\0'; p += (strlen(p) + 1)) {
			if (strcasecmp(p, pname) == 0) {
				return mapping->code;
			}
		}
	}

	return ONIG_ENCODING_UNDEF;
}
/* }}} */

/* {{{ php_mb_regex_mbctype2name */
static const char *_php_mb_regex_mbctype2name(OnigEncoding mbctype)
{
	php_mb_regex_enc_name_map_t *mapping;

	for (mapping = enc_name_map; mapping->names != NULL; mapping++) {
		if (mapping->code == mbctype) {
			return mapping->names;
		}
	}

	return NULL;
}
/* }}} */

/* {{{ php_mb_regex_set_mbctype */
int php_mb_regex_set_mbctype(const char *encname)
{
	OnigEncoding mbctype = _php_mb_regex_name2mbctype(encname);
	if (mbctype == ONIG_ENCODING_UNDEF) {
		return FAILURE;
	}
	MBREX(current_mbctype) = mbctype;
	return SUCCESS;
}
/* }}} */

/* {{{ php_mb_regex_set_default_mbctype */
int php_mb_regex_set_default_mbctype(const char *encname)
{
	OnigEncoding mbctype = _php_mb_regex_name2mbctype(encname);
	if (mbctype == ONIG_ENCODING_UNDEF) {
		return FAILURE;
	}
	MBREX(default_mbctype) = mbctype;
	return SUCCESS;
}
/* }}} */

/* {{{ php_mb_regex_get_mbctype */
const char *php_mb_regex_get_mbctype(void)
{
	return _php_mb_regex_mbctype2name(MBREX(current_mbctype));
}
/* }}} */

/* {{{ php_mb_regex_get_default_mbctype */
const char *php_mb_regex_get_default_mbctype(void)
{
	return _php_mb_regex_mbctype2name(MBREX(default_mbctype));
}
/* }}} */

/*
 * regex cache
 */
/* {{{ php_mbregex_compile_pattern */
static php_mb_regex_t *php_mbregex_compile_pattern(const char *pattern, int patlen, OnigOptionType options, OnigEncoding enc, OnigSyntaxType *syntax)
{
	int err_code = 0;
	php_mb_regex_t *retval = NULL, *rc = NULL;
	OnigErrorInfo err_info;
	OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];

	rc = zend_hash_str_find_ptr(&MBREX(ht_rc), (char *)pattern, patlen);
	if (!rc || rc->options != options || rc->enc != enc || rc->syntax != syntax) {
		if ((err_code = onig_new(&retval, (OnigUChar *)pattern, (OnigUChar *)(pattern + patlen), options, enc, syntax, &err_info)) != ONIG_NORMAL) {
			onig_error_code_to_str(err_str, err_code, err_info);
			php_error_docref(NULL, E_WARNING, "mbregex compile err: %s", err_str);
			retval = NULL;
			goto out;
		}
		zend_hash_str_update_ptr(&MBREX(ht_rc), (char *)pattern, patlen, retval);
	} else if (rc) {
		retval = rc;
	}
out:
	return retval;
}
/* }}} */

/* {{{ _php_mb_regex_get_option_string */
static size_t _php_mb_regex_get_option_string(char *str, size_t len, OnigOptionType option, OnigSyntaxType *syntax)
{
	size_t len_left = len;
	size_t len_req = 0;
	char *p = str;
	char c;

	if ((option & ONIG_OPTION_IGNORECASE) != 0) {
		if (len_left > 0) {
			--len_left;
			*(p++) = 'i';
		}
		++len_req;
	}

	if ((option & ONIG_OPTION_EXTEND) != 0) {
		if (len_left > 0) {
			--len_left;
			*(p++) = 'x';
		}
		++len_req;
	}

	if ((option & (ONIG_OPTION_MULTILINE | ONIG_OPTION_SINGLELINE)) ==
			(ONIG_OPTION_MULTILINE | ONIG_OPTION_SINGLELINE)) {
		if (len_left > 0) {
			--len_left;
			*(p++) = 'p';
		}
		++len_req;
	} else {
		if ((option & ONIG_OPTION_MULTILINE) != 0) {
			if (len_left > 0) {
				--len_left;
				*(p++) = 'm';
			}
			++len_req;
		}

		if ((option & ONIG_OPTION_SINGLELINE) != 0) {
			if (len_left > 0) {
				--len_left;
				*(p++) = 's';
			}
			++len_req;
		}
	}
	if ((option & ONIG_OPTION_FIND_LONGEST) != 0) {
		if (len_left > 0) {
			--len_left;
			*(p++) = 'l';
		}
		++len_req;
	}
	if ((option & ONIG_OPTION_FIND_NOT_EMPTY) != 0) {
		if (len_left > 0) {
			--len_left;
			*(p++) = 'n';
		}
		++len_req;
	}

	c = 0;

	if (syntax == ONIG_SYNTAX_JAVA) {
		c = 'j';
	} else if (syntax == ONIG_SYNTAX_GNU_REGEX) {
		c = 'u';
	} else if (syntax == ONIG_SYNTAX_GREP) {
		c = 'g';
	} else if (syntax == ONIG_SYNTAX_EMACS) {
		c = 'c';
	} else if (syntax == ONIG_SYNTAX_RUBY) {
		c = 'r';
	} else if (syntax == ONIG_SYNTAX_PERL) {
		c = 'z';
	} else if (syntax == ONIG_SYNTAX_POSIX_BASIC) {
		c = 'b';
	} else if (syntax == ONIG_SYNTAX_POSIX_EXTENDED) {
		c = 'd';
	}

	if (c != 0) {
		if (len_left > 0) {
			--len_left;
			*(p++) = c;
		}
		++len_req;
	}


	if (len_left > 0) {
		--len_left;
		*(p++) = '\0';
	}
	++len_req;
	if (len < len_req) {
		return len_req;
	}

	return 0;
}
/* }}} */

/* {{{ _php_mb_regex_init_options */
static void
_php_mb_regex_init_options(const char *parg, int narg, OnigOptionType *option, OnigSyntaxType **syntax, int *eval)
{
	int n;
	char c;
	int optm = 0;

	*syntax = ONIG_SYNTAX_RUBY;

	if (parg != NULL) {
		n = 0;
		while(n < narg) {
			c = parg[n++];
			switch (c) {
				case 'i':
					optm |= ONIG_OPTION_IGNORECASE;
					break;
				case 'x':
					optm |= ONIG_OPTION_EXTEND;
					break;
				case 'm':
					optm |= ONIG_OPTION_MULTILINE;
					break;
				case 's':
					optm |= ONIG_OPTION_SINGLELINE;
					break;
				case 'p':
					optm |= ONIG_OPTION_MULTILINE | ONIG_OPTION_SINGLELINE;
					break;
				case 'l':
					optm |= ONIG_OPTION_FIND_LONGEST;
					break;
				case 'n':
					optm |= ONIG_OPTION_FIND_NOT_EMPTY;
					break;
				case 'j':
					*syntax = ONIG_SYNTAX_JAVA;
					break;
				case 'u':
					*syntax = ONIG_SYNTAX_GNU_REGEX;
					break;
				case 'g':
					*syntax = ONIG_SYNTAX_GREP;
					break;
				case 'c':
					*syntax = ONIG_SYNTAX_EMACS;
					break;
				case 'r':
					*syntax = ONIG_SYNTAX_RUBY;
					break;
				case 'z':
					*syntax = ONIG_SYNTAX_PERL;
					break;
				case 'b':
					*syntax = ONIG_SYNTAX_POSIX_BASIC;
					break;
				case 'd':
					*syntax = ONIG_SYNTAX_POSIX_EXTENDED;
					break;
				case 'e':
					if (eval != NULL) *eval = 1;
					break;
				default:
					break;
			}
		}
		if (option != NULL) *option|=optm;
	}
}
/* }}} */

/*
 * php functions
 */

/* {{{ proto string mb_regex_encoding([string encoding])
   Returns the current encoding for regex as a string. */
PHP_FUNCTION(mb_regex_encoding)
{
	char *encoding = NULL;
	size_t encoding_len;
	OnigEncoding mbctype;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &encoding, &encoding_len) == FAILURE) {
		return;
	}

	if (!encoding) {
		const char *retval = _php_mb_regex_mbctype2name(MBREX(current_mbctype));

		if (retval == NULL) {
			RETURN_FALSE;
		}

		RETURN_STRING((char *)retval);
	} else {
		mbctype = _php_mb_regex_name2mbctype(encoding);

		if (mbctype == ONIG_ENCODING_UNDEF) {
			php_error_docref(NULL, E_WARNING, "Unknown encoding \"%s\"", encoding);
			RETURN_FALSE;
		}

		MBREX(current_mbctype) = mbctype;
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ _php_mb_regex_ereg_exec */
static void _php_mb_regex_ereg_exec(INTERNAL_FUNCTION_PARAMETERS, int icase)
{
	zval *arg_pattern, *array = NULL;
	char *string;
	size_t string_len;
	php_mb_regex_t *re;
	OnigRegion *regs = NULL;
	int i, match_len, beg, end;
	OnigOptionType options;
	char *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zs|z/", &arg_pattern, &string, &string_len, &array) == FAILURE) {
		RETURN_FALSE;
	}

	options = MBREX(regex_default_options);
	if (icase) {
		options |= ONIG_OPTION_IGNORECASE;
	}

	/* compile the regular expression from the supplied regex */
	if (Z_TYPE_P(arg_pattern) != IS_STRING) {
		/* we convert numbers to integers and treat them as a string */
		if (Z_TYPE_P(arg_pattern) == IS_DOUBLE) {
			convert_to_long_ex(arg_pattern);	/* get rid of decimal places */
		}
		convert_to_string_ex(arg_pattern);
		/* don't bother doing an extended regex with just a number */
	}

	if (Z_STRLEN_P(arg_pattern) == 0) {
		php_error_docref(NULL, E_WARNING, "empty pattern");
		RETVAL_FALSE;
		goto out;
	}

	re = php_mbregex_compile_pattern(Z_STRVAL_P(arg_pattern), Z_STRLEN_P(arg_pattern), options, MBREX(current_mbctype), MBREX(regex_default_syntax));
	if (re == NULL) {
		RETVAL_FALSE;
		goto out;
	}

	regs = onig_region_new();

	/* actually execute the regular expression */
	if (onig_search(re, (OnigUChar *)string, (OnigUChar *)(string + string_len), (OnigUChar *)string, (OnigUChar *)(string + string_len), regs, 0) < 0) {
		RETVAL_FALSE;
		goto out;
	}

	match_len = 1;
	str = string;
	if (array != NULL) {
		zval_dtor(array);
		array_init(array);

		match_len = regs->end[0] - regs->beg[0];
		for (i = 0; i < regs->num_regs; i++) {
			beg = regs->beg[i];
			end = regs->end[i];
			if (beg >= 0 && beg < end && end <= string_len) {
				add_index_stringl(array, i, (char *)&str[beg], end - beg);
			} else {
				add_index_bool(array, i, 0);
			}
		}
	}

	if (match_len == 0) {
		match_len = 1;
	}
	RETVAL_LONG(match_len);
out:
	if (regs != NULL) {
		onig_region_free(regs, 1);
	}
}
/* }}} */

/* {{{ proto int mb_ereg(string pattern, string string [, array registers])
   Regular expression match for multibyte string */
PHP_FUNCTION(mb_ereg)
{
	_php_mb_regex_ereg_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int mb_eregi(string pattern, string string [, array registers])
   Case-insensitive regular expression match for multibyte string */
PHP_FUNCTION(mb_eregi)
{
	_php_mb_regex_ereg_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ _php_mb_regex_ereg_replace_exec */
static void _php_mb_regex_ereg_replace_exec(INTERNAL_FUNCTION_PARAMETERS, OnigOptionType options, int is_callable)
{
	zval *arg_pattern_zval;

	char *arg_pattern;
	size_t arg_pattern_len;

	char *replace;
	size_t replace_len;

	zend_fcall_info arg_replace_fci;
	zend_fcall_info_cache arg_replace_fci_cache;

	char *string;
	size_t string_len;

	char *p;
	php_mb_regex_t *re;
	OnigSyntaxType *syntax;
	OnigRegion *regs = NULL;
	smart_str out_buf = {0};
	smart_str eval_buf = {0};
	smart_str *pbuf;
	int i, err, eval, n;
	OnigUChar *pos;
	OnigUChar *string_lim;
	char *description = NULL;
	char pat_buf[4];

	const mbfl_encoding *enc;

	{
		const char *current_enc_name;
		current_enc_name = _php_mb_regex_mbctype2name(MBREX(current_mbctype));
		if (current_enc_name == NULL ||
			(enc = mbfl_name2encoding(current_enc_name)) == NULL) {
			php_error_docref(NULL, E_WARNING, "Unknown error");
			RETURN_FALSE;
		}
	}
	eval = 0;
	{
		char *option_str = NULL;
		size_t option_str_len = 0;

		if (!is_callable) {
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "zss|s",
						&arg_pattern_zval,
						&replace, &replace_len,
						&string, &string_len,
						&option_str, &option_str_len) == FAILURE) {
				RETURN_FALSE;
			}
		} else {
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "zfs|s",
						&arg_pattern_zval,
						&arg_replace_fci, &arg_replace_fci_cache,
						&string, &string_len,
						&option_str, &option_str_len) == FAILURE) {
				RETURN_FALSE;
			}
		}

		if (option_str != NULL) {
			_php_mb_regex_init_options(option_str, option_str_len, &options, &syntax, &eval);
		} else {
			options |= MBREX(regex_default_options);
			syntax = MBREX(regex_default_syntax);
		}
	}
	if (Z_TYPE_P(arg_pattern_zval) == IS_STRING) {
		arg_pattern = Z_STRVAL_P(arg_pattern_zval);
		arg_pattern_len = Z_STRLEN_P(arg_pattern_zval);
	} else {
		/* FIXME: this code is not multibyte aware! */
		convert_to_long_ex(arg_pattern_zval);
		pat_buf[0] = (char)Z_LVAL_P(arg_pattern_zval);
		pat_buf[1] = '\0';
		pat_buf[2] = '\0';
		pat_buf[3] = '\0';

		arg_pattern = pat_buf;
		arg_pattern_len = 1;
	}
	/* create regex pattern buffer */
	re = php_mbregex_compile_pattern(arg_pattern, arg_pattern_len, options, MBREX(current_mbctype), syntax);
	if (re == NULL) {
		RETURN_FALSE;
	}

	if (eval || is_callable) {
		pbuf = &eval_buf;
		description = zend_make_compiled_string_description("mbregex replace");
	} else {
		pbuf = &out_buf;
		description = NULL;
	}

	if (is_callable) {
		if (eval) {
			php_error_docref(NULL, E_WARNING, "Option 'e' cannot be used with replacement callback");
			RETURN_FALSE;
		}
	}

	/* do the actual work */
	err = 0;
	pos = (OnigUChar *)string;
	string_lim = (OnigUChar*)(string + string_len);
	regs = onig_region_new();
	while (err >= 0) {
		err = onig_search(re, (OnigUChar *)string, (OnigUChar *)string_lim, pos, (OnigUChar *)string_lim, regs, 0);
		if (err <= -2) {
			OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];
			onig_error_code_to_str(err_str, err);
			php_error_docref(NULL, E_WARNING, "mbregex search failure in php_mbereg_replace_exec(): %s", err_str);
			break;
		}
		if (err >= 0) {
#if moriyoshi_0
			if (regs->beg[0] == regs->end[0]) {
				php_error_docref(NULL, E_WARNING, "Empty regular expression");
				break;
			}
#endif
			/* copy the part of the string before the match */
			smart_str_appendl(&out_buf, (char *)pos, (size_t)((OnigUChar *)(string + regs->beg[0]) - pos));

			if (!is_callable) {
				/* copy replacement and backrefs */
				i = 0;
				p = replace;
				while (i < replace_len) {
					int fwd = (int) php_mb_mbchar_bytes_ex(p, enc);
					n = -1;
					if ((replace_len - i) >= 2 && fwd == 1 &&
					p[0] == '\\' && p[1] >= '0' && p[1] <= '9') {
						n = p[1] - '0';
					}
					if (n >= 0 && n < regs->num_regs) {
						if (regs->beg[n] >= 0 && regs->beg[n] < regs->end[n] && regs->end[n] <= string_len) {
							smart_str_appendl(pbuf, string + regs->beg[n], regs->end[n] - regs->beg[n]);
						}
						p += 2;
						i += 2;
					} else {
						smart_str_appendl(pbuf, p, fwd);
						p += fwd;
						i += fwd;
					}
				}
			}

			if (eval) {
				zval v;
				zend_string *eval_str;
				/* null terminate buffer */
				smart_str_0(&eval_buf);

				if (eval_buf.s) {
					eval_str = eval_buf.s;
				} else {
					eval_str = ZSTR_EMPTY_ALLOC();
				}

				/* do eval */
				if (zend_eval_stringl(ZSTR_VAL(eval_str), ZSTR_LEN(eval_str), &v, description) == FAILURE) {
					efree(description);
					php_error_docref(NULL,E_ERROR, "Failed evaluating code: %s%s", PHP_EOL, ZSTR_VAL(eval_str));
					/* zend_error() does not return in this case */
				}

				/* result of eval */
				convert_to_string(&v);
				smart_str_appendl(&out_buf, Z_STRVAL(v), Z_STRLEN(v));
				/* Clean up */
				smart_str_free(&eval_buf);
				zval_dtor(&v);
			} else if (is_callable) {
				zval args[1];
				zval subpats, retval;
				int i;

				array_init(&subpats);
				for (i = 0; i < regs->num_regs; i++) {
					add_next_index_stringl(&subpats, string + regs->beg[i], regs->end[i] - regs->beg[i]);
				}

				ZVAL_COPY_VALUE(&args[0], &subpats);
				/* null terminate buffer */
				smart_str_0(&eval_buf);

				arg_replace_fci.param_count = 1;
				arg_replace_fci.params = args;
				arg_replace_fci.retval = &retval;
				if (zend_call_function(&arg_replace_fci, &arg_replace_fci_cache) == SUCCESS &&
						!Z_ISUNDEF(retval)) {
					convert_to_string_ex(&retval);
					smart_str_appendl(&out_buf, Z_STRVAL(retval), Z_STRLEN(retval));
					smart_str_free(&eval_buf);
					zval_ptr_dtor(&retval);
				} else {
					efree(description);
					if (!EG(exception)) {
						php_error_docref(NULL, E_WARNING, "Unable to call custom replacement function");
					}
				}
				zval_ptr_dtor(&subpats);
			}

			n = regs->end[0];
			if ((pos - (OnigUChar *)string) < n) {
				pos = (OnigUChar *)string + n;
			} else {
				if (pos < string_lim) {
					smart_str_appendl(&out_buf, (char *)pos, 1);
				}
				pos++;
			}
		} else { /* nomatch */
			/* stick that last bit of string on our output */
			if (string_lim - pos > 0) {
				smart_str_appendl(&out_buf, (char *)pos, string_lim - pos);
			}
		}
		onig_region_free(regs, 0);
	}

	if (description) {
		efree(description);
	}
	if (regs != NULL) {
		onig_region_free(regs, 1);
	}
	smart_str_free(&eval_buf);

	if (err <= -2) {
		smart_str_free(&out_buf);
		RETVAL_FALSE;
	} else if (out_buf.s) {
		smart_str_0(&out_buf);
		RETVAL_STR(out_buf.s);
	} else {
		RETVAL_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto string mb_ereg_replace(string pattern, string replacement, string string [, string option])
   Replace regular expression for multibyte string */
PHP_FUNCTION(mb_ereg_replace)
{
	_php_mb_regex_ereg_replace_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
}
/* }}} */

/* {{{ proto string mb_eregi_replace(string pattern, string replacement, string string)
   Case insensitive replace regular expression for multibyte string */
PHP_FUNCTION(mb_eregi_replace)
{
	_php_mb_regex_ereg_replace_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, ONIG_OPTION_IGNORECASE, 0);
}
/* }}} */

/* {{{ proto string mb_ereg_replace_callback(string pattern, string callback, string string [, string option])
    regular expression for multibyte string using replacement callback */
PHP_FUNCTION(mb_ereg_replace_callback)
{
	_php_mb_regex_ereg_replace_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 1);
}
/* }}} */

/* {{{ proto array mb_split(string pattern, string string [, int limit])
   split multibyte string into array by regular expression */
PHP_FUNCTION(mb_split)
{
	char *arg_pattern;
	size_t arg_pattern_len;
	php_mb_regex_t *re;
	OnigRegion *regs = NULL;
	char *string;
	OnigUChar *pos, *chunk_pos;
	size_t string_len;

	int n, err;
	zend_long count = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|l", &arg_pattern, &arg_pattern_len, &string, &string_len, &count) == FAILURE) {
		RETURN_FALSE;
	}

	if (count > 0) {
		count--;
	}

	/* create regex pattern buffer */
	if ((re = php_mbregex_compile_pattern(arg_pattern, arg_pattern_len, MBREX(regex_default_options), MBREX(current_mbctype), MBREX(regex_default_syntax))) == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);

	chunk_pos = pos = (OnigUChar *)string;
	err = 0;
	regs = onig_region_new();
	/* churn through str, generating array entries as we go */
	while (count != 0 && (pos - (OnigUChar *)string) < (ptrdiff_t)string_len) {
		int beg, end;
		err = onig_search(re, (OnigUChar *)string, (OnigUChar *)(string + string_len), pos, (OnigUChar *)(string + string_len), regs, 0);
		if (err < 0) {
			break;
		}
		beg = regs->beg[0], end = regs->end[0];
		/* add it to the array */
		if ((pos - (OnigUChar *)string) < end) {
			if (beg < string_len && beg >= (chunk_pos - (OnigUChar *)string)) {
				add_next_index_stringl(return_value, (char *)chunk_pos, ((OnigUChar *)(string + beg) - chunk_pos));
				--count;
			} else {
				err = -2;
				break;
			}
			/* point at our new starting point */
			chunk_pos = pos = (OnigUChar *)string + end;
		} else {
			pos++;
		}
		onig_region_free(regs, 0);
	}

	onig_region_free(regs, 1);

	/* see if we encountered an error */
	if (err <= -2) {
		OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];
		onig_error_code_to_str(err_str, err);
		php_error_docref(NULL, E_WARNING, "mbregex search failure in mbsplit(): %s", err_str);
		zval_dtor(return_value);
		RETURN_FALSE;
	}

	/* otherwise we just have one last element to add to the array */
	n = ((OnigUChar *)(string + string_len) - chunk_pos);
	if (n > 0) {
		add_next_index_stringl(return_value, (char *)chunk_pos, n);
	} else {
		add_next_index_stringl(return_value, "", 0);
	}
}
/* }}} */

/* {{{ proto bool mb_ereg_match(string pattern, string string [,string option])
   Regular expression match for multibyte string */
PHP_FUNCTION(mb_ereg_match)
{
	char *arg_pattern;
	size_t arg_pattern_len;

	char *string;
	size_t string_len;

	php_mb_regex_t *re;
	OnigSyntaxType *syntax;
	OnigOptionType option = 0;
	int err;

	{
		char *option_str = NULL;
		size_t option_str_len = 0;

		if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|s",
		                          &arg_pattern, &arg_pattern_len, &string, &string_len,
		                          &option_str, &option_str_len)==FAILURE) {
			RETURN_FALSE;
		}

		if (option_str != NULL) {
			_php_mb_regex_init_options(option_str, option_str_len, &option, &syntax, NULL);
		} else {
			option |= MBREX(regex_default_options);
			syntax = MBREX(regex_default_syntax);
		}
	}

	if ((re = php_mbregex_compile_pattern(arg_pattern, arg_pattern_len, option, MBREX(current_mbctype), syntax)) == NULL) {
		RETURN_FALSE;
	}

	/* match */
	err = onig_match(re, (OnigUChar *)string, (OnigUChar *)(string + string_len), (OnigUChar *)string, NULL, 0);
	if (err >= 0) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* regex search */
/* {{{ _php_mb_regex_ereg_search_exec */
static void
_php_mb_regex_ereg_search_exec(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	char *arg_pattern = NULL, *arg_options = NULL;
	size_t arg_pattern_len, arg_options_len;
	int n, i, err, pos, len, beg, end;
	OnigOptionType option;
	OnigUChar *str;
	OnigSyntaxType *syntax;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|ss", &arg_pattern, &arg_pattern_len, &arg_options, &arg_options_len) == FAILURE) {
		return;
	}

	option = MBREX(regex_default_options);

	if (arg_options) {
		option = 0;
		_php_mb_regex_init_options(arg_options, arg_options_len, &option, &syntax, NULL);
	}

	if (arg_pattern) {
		/* create regex pattern buffer */
		if ((MBREX(search_re) = php_mbregex_compile_pattern(arg_pattern, arg_pattern_len, option, MBREX(current_mbctype), MBREX(regex_default_syntax))) == NULL) {
			RETURN_FALSE;
		}
	}

	pos = MBREX(search_pos);
	str = NULL;
	len = 0;
	if (!Z_ISUNDEF(MBREX(search_str)) && Z_TYPE(MBREX(search_str)) == IS_STRING){
		str = (OnigUChar *)Z_STRVAL(MBREX(search_str));
		len = Z_STRLEN(MBREX(search_str));
	}

	if (MBREX(search_re) == NULL) {
		php_error_docref(NULL, E_WARNING, "No regex given");
		RETURN_FALSE;
	}

	if (str == NULL) {
		php_error_docref(NULL, E_WARNING, "No string given");
		RETURN_FALSE;
	}

	if (MBREX(search_regs)) {
		onig_region_free(MBREX(search_regs), 1);
	}
	MBREX(search_regs) = onig_region_new();

	err = onig_search(MBREX(search_re), str, str + len, str + pos, str  + len, MBREX(search_regs), 0);
	if (err == ONIG_MISMATCH) {
		MBREX(search_pos) = len;
		RETVAL_FALSE;
	} else if (err <= -2) {
		OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];
		onig_error_code_to_str(err_str, err);
		php_error_docref(NULL, E_WARNING, "mbregex search failure in mbregex_search(): %s", err_str);
		RETVAL_FALSE;
	} else {
		if (MBREX(search_regs)->beg[0] == MBREX(search_regs)->end[0]) {
			php_error_docref(NULL, E_WARNING, "Empty regular expression");
		}
		switch (mode) {
		case 1:
			array_init(return_value);
			beg = MBREX(search_regs)->beg[0];
			end = MBREX(search_regs)->end[0];
			add_next_index_long(return_value, beg);
			add_next_index_long(return_value, end - beg);
			break;
		case 2:
			array_init(return_value);
			n = MBREX(search_regs)->num_regs;
			for (i = 0; i < n; i++) {
				beg = MBREX(search_regs)->beg[i];
				end = MBREX(search_regs)->end[i];
				if (beg >= 0 && beg <= end && end <= len) {
					add_index_stringl(return_value, i, (char *)&str[beg], end - beg);
				} else {
					add_index_bool(return_value, i, 0);
				}
			}
			break;
		default:
			RETVAL_TRUE;
			break;
		}
		end = MBREX(search_regs)->end[0];
		if (pos < end) {
			MBREX(search_pos) = end;
		} else {
			MBREX(search_pos) = pos + 1;
		}
	}

	if (err < 0) {
		onig_region_free(MBREX(search_regs), 1);
		MBREX(search_regs) = (OnigRegion *)NULL;
	}
}
/* }}} */

/* {{{ proto bool mb_ereg_search([string pattern[, string option]])
   Regular expression search for multibyte string */
PHP_FUNCTION(mb_ereg_search)
{
	_php_mb_regex_ereg_search_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto array mb_ereg_search_pos([string pattern[, string option]])
   Regular expression search for multibyte string */
PHP_FUNCTION(mb_ereg_search_pos)
{
	_php_mb_regex_ereg_search_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto array mb_ereg_search_regs([string pattern[, string option]])
   Regular expression search for multibyte string */
PHP_FUNCTION(mb_ereg_search_regs)
{
	_php_mb_regex_ereg_search_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 2);
}
/* }}} */

/* {{{ proto bool mb_ereg_search_init(string string [, string pattern[, string option]])
   Initialize string and regular expression for search. */
PHP_FUNCTION(mb_ereg_search_init)
{
	size_t argc = ZEND_NUM_ARGS();
	zval *arg_str;
	char *arg_pattern = NULL, *arg_options = NULL;
	size_t arg_pattern_len = 0, arg_options_len = 0;
	OnigSyntaxType *syntax = NULL;
	OnigOptionType option;

	if (zend_parse_parameters(argc, "z|ss", &arg_str, &arg_pattern, &arg_pattern_len, &arg_options, &arg_options_len) == FAILURE) {
		return;
	}

	if (argc > 1 && arg_pattern_len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty pattern");
		RETURN_FALSE;
	}

	option = MBREX(regex_default_options);
	syntax = MBREX(regex_default_syntax);

	if (argc == 3) {
		option = 0;
		_php_mb_regex_init_options(arg_options, arg_options_len, &option, &syntax, NULL);
	}

	if (argc > 1) {
		/* create regex pattern buffer */
		if ((MBREX(search_re) = php_mbregex_compile_pattern(arg_pattern, arg_pattern_len, option, MBREX(current_mbctype), syntax)) == NULL) {
			RETURN_FALSE;
		}
	}

	if (!Z_ISNULL(MBREX(search_str))) {
		zval_ptr_dtor(&MBREX(search_str));
	}

	ZVAL_DUP(&MBREX(search_str), arg_str);

	MBREX(search_pos) = 0;

	if (MBREX(search_regs) != NULL) {
		onig_region_free(MBREX(search_regs), 1);
		MBREX(search_regs) = NULL;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array mb_ereg_search_getregs(void)
   Get matched substring of the last time */
PHP_FUNCTION(mb_ereg_search_getregs)
{
	int n, i, len, beg, end;
	OnigUChar *str;

	if (MBREX(search_regs) != NULL && Z_TYPE(MBREX(search_str)) == IS_STRING) {
		array_init(return_value);

		str = (OnigUChar *)Z_STRVAL(MBREX(search_str));
		len = Z_STRLEN(MBREX(search_str));
		n = MBREX(search_regs)->num_regs;
		for (i = 0; i < n; i++) {
			beg = MBREX(search_regs)->beg[i];
			end = MBREX(search_regs)->end[i];
			if (beg >= 0 && beg <= end && end <= len) {
				add_index_stringl(return_value, i, (char *)&str[beg], end - beg);
			} else {
				add_index_bool(return_value, i, 0);
			}
		}
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto int mb_ereg_search_getpos(void)
   Get search start position */
PHP_FUNCTION(mb_ereg_search_getpos)
{
	RETVAL_LONG(MBREX(search_pos));
}
/* }}} */

/* {{{ proto bool mb_ereg_search_setpos(int position)
   Set search start position */
PHP_FUNCTION(mb_ereg_search_setpos)
{
	zend_long position;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &position) == FAILURE) {
		return;
	}

	/* Accept negative position if length of search string can be determined */
	if ((position < 0) && (!Z_ISUNDEF(MBREX(search_str))) && (Z_TYPE(MBREX(search_str)) == IS_STRING)) {
		position += Z_STRLEN(MBREX(search_str));
	}
		
	if (position < 0 || (!Z_ISUNDEF(MBREX(search_str)) && Z_TYPE(MBREX(search_str)) == IS_STRING && (size_t)position >= Z_STRLEN(MBREX(search_str)))) {
		php_error_docref(NULL, E_WARNING, "Position is out of range");
		MBREX(search_pos) = 0;
		RETURN_FALSE;
	}

	MBREX(search_pos) = position;
	RETURN_TRUE;
}
/* }}} */

/* {{{ php_mb_regex_set_options */
static void _php_mb_regex_set_options(OnigOptionType options, OnigSyntaxType *syntax, OnigOptionType *prev_options, OnigSyntaxType **prev_syntax)
{
	if (prev_options != NULL) {
		*prev_options = MBREX(regex_default_options);
	}
	if (prev_syntax != NULL) {
		*prev_syntax = MBREX(regex_default_syntax);
	}
	MBREX(regex_default_options) = options;
	MBREX(regex_default_syntax) = syntax;
}
/* }}} */

/* {{{ proto string mb_regex_set_options([string options])
   Set or get the default options for mbregex functions */
PHP_FUNCTION(mb_regex_set_options)
{
	OnigOptionType opt;
	OnigSyntaxType *syntax;
	char *string = NULL;
	size_t string_len;
	char buf[16];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s",
	                          &string, &string_len) == FAILURE) {
		RETURN_FALSE;
	}
	if (string != NULL) {
		opt = 0;
		syntax = NULL;
		_php_mb_regex_init_options(string, string_len, &opt, &syntax, NULL);
		_php_mb_regex_set_options(opt, syntax, NULL, NULL);
	} else {
		opt = MBREX(regex_default_options);
		syntax = MBREX(regex_default_syntax);
	}
	_php_mb_regex_get_option_string(buf, sizeof(buf), opt, syntax);

	RETVAL_STRING(buf);
}
/* }}} */

#endif	/* HAVE_MBREGEX */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
