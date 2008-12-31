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
   +----------------------------------------------------------------------+
 */

/* $Id$ */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"

#if HAVE_MBREGEX

#include "ext/standard/php_smart_str.h"
#include "php_mbregex.h"
#include "mbstring.h"

ZEND_EXTERN_MODULE_GLOBALS(mbstring)

/* {{{ static void php_mb_regex_free_cache() */
static void php_mb_regex_free_cache(php_mb_regex_t **pre) 
{
	onig_free(*pre);
}
/* }}} */

/* {{{ _php_mb_regex_globals_ctor */
void _php_mb_regex_globals_ctor(zend_mbstring_globals *pglobals TSRMLS_DC)
{
	MBSTRG(default_mbctype) = ONIG_ENCODING_EUC_JP;
	MBSTRG(current_mbctype) = ONIG_ENCODING_EUC_JP;
	zend_hash_init(&(MBSTRG(ht_rc)), 0, NULL, (void (*)(void *)) php_mb_regex_free_cache, 1);
	MBSTRG(search_str) = (zval*) NULL;
	MBSTRG(search_re) = (php_mb_regex_t*)NULL;
	MBSTRG(search_pos) = 0;
	MBSTRG(search_regs) = (OnigRegion*)NULL;
	MBSTRG(regex_default_options) = ONIG_OPTION_MULTILINE | ONIG_OPTION_SINGLELINE;
	MBSTRG(regex_default_syntax) = ONIG_SYNTAX_RUBY;
}
/* }}} */

/* {{{ _php_mb_regex_globals_dtor */
void _php_mb_regex_globals_dtor(zend_mbstring_globals *pglobals TSRMLS_DC) 
{
	zend_hash_destroy(&MBSTRG(ht_rc));
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
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION(mb_regex) */
PHP_RSHUTDOWN_FUNCTION(mb_regex)
{
	MBSTRG(current_mbctype) = MBSTRG(default_mbctype);

	if (MBSTRG(search_str) != NULL) {
		zval_ptr_dtor(&MBSTRG(search_str));
		MBSTRG(search_str) = (zval *)NULL;
	}
	MBSTRG(search_pos) = 0;

	if (MBSTRG(search_regs) != NULL) {
		onig_region_free(MBSTRG(search_regs), 1);
		MBSTRG(search_regs) = (OnigRegion *)NULL;
	}
	zend_hash_clean(&MBSTRG(ht_rc));

	return SUCCESS;
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

php_mb_regex_enc_name_map_t enc_name_map[] ={
	{
		"EUC-JP\0EUCJP\0X-EUC-JP\0UJIS\0EUCJP\0EUCJP-WIN\0",
		ONIG_ENCODING_EUC_JP
	},
	{
		"UTF-8\0UTF8\0",
		ONIG_ENCODING_UTF8
	},
	{
		"UTF-16\0UTF-16BE\0",
		ONIG_ENCODING_UTF16_BE
	},
	{
		"UTF-16LE\0",
		ONIG_ENCODING_UTF16_LE
	},
	{
		"UCS-4\0UTF-32\0UTF-32BE\0",
		ONIG_ENCODING_UTF32_BE
	},
	{
		"UCS-4LE\0UTF-32LE\0",
		ONIG_ENCODING_UTF32_LE
	},
	{
		"SJIS\0CP932\0MS932\0SHIFT_JIS\0SJIS-WIN\0WINDOWS-31J\0",
		ONIG_ENCODING_SJIS
	},
	{
		"BIG5\0BIG-5\0BIGFIVE\0CN-BIG5\0BIG-FIVE\0",
		ONIG_ENCODING_BIG5
	},
	{
		"EUC-CN\0EUCCN\0EUC_CN\0GB-2312\0GB2312\0",
		ONIG_ENCODING_EUC_CN
	},
	{
		"EUC-TW\0EUCTW\0EUC_TW\0",
		ONIG_ENCODING_EUC_TW
	},
	{
		"EUC-KR\0EUCKR\0EUC_KR\0",
		ONIG_ENCODING_EUC_KR
	},
	{
		"KOI8\0KOI-8\0",
		ONIG_ENCODING_KOI8
	},
	{
		"KOI8R\0KOI8-R\0KOI-8R\0",
		ONIG_ENCODING_KOI8_R
	},
	{
		"ISO-8859-1\0ISO8859-1\0ISO_8859_1\0ISO8859_1\0",
		ONIG_ENCODING_ISO_8859_1
	},
	{
		"ISO-8859-2\0ISO8859-2\0ISO_8859_2\0ISO8859_2\0",
		ONIG_ENCODING_ISO_8859_2
	},
	{
		"ISO-8859-3\0ISO8859-3\0ISO_8859_3\0ISO8859_3\0",
		ONIG_ENCODING_ISO_8859_3
	},
	{
		"ISO-8859-4\0ISO8859-4\0ISO_8859_4\0ISO8859_4\0",
		ONIG_ENCODING_ISO_8859_4
	},
	{
		"ISO-8859-5\0ISO8859-5\0ISO_8859_5\0ISO8859_5\0",
		ONIG_ENCODING_ISO_8859_5
	},
	{
		"ISO-8859-6\0ISO8859-6\0ISO_8859_6\0ISO8859_6\0",
		ONIG_ENCODING_ISO_8859_6
	},
	{
		"ISO-8859-7\0ISO8859-7\0ISO_8859_7\0ISO8859_7\0",
		ONIG_ENCODING_ISO_8859_7
	},
	{
		"ISO-8859-8\0ISO8859-8\0ISO_8859_8\0ISO8859_8\0",
		ONIG_ENCODING_ISO_8859_8
	},
	{
		"ISO-8859-9\0ISO8859-9\0ISO_8859_9\0ISO8859_9\0",
		ONIG_ENCODING_ISO_8859_9
	},
	{
		"ISO-8859-10\0ISO8859-10\0ISO_8859_10\0ISO8859_10\0",
		ONIG_ENCODING_ISO_8859_10
	},
	{
		"ISO-8859-11\0ISO8859-11\0ISO_8859_11\0ISO8859_11\0",
		ONIG_ENCODING_ISO_8859_11
	},
	{
		"ISO-8859-13\0ISO8859-13\0ISO_8859_13\0ISO8859_13\0",
		ONIG_ENCODING_ISO_8859_13
	},
	{
		"ISO-8859-14\0ISO8859-14\0ISO_8859_14\0ISO8859_14\0",
		ONIG_ENCODING_ISO_8859_14
	},
	{
		"ISO-8859-15\0ISO8859-15\0ISO_8859_15\0ISO8859_15\0",
		ONIG_ENCODING_ISO_8859_15
	},
	{
		"ISO-8859-16\0ISO8859-16\0ISO_8859_16\0ISO8859_16\0",
		ONIG_ENCODING_ISO_8859_16
	},
	{
		"ASCII\0US-ASCII\0US_ASCII\0ISO646\0",
		ONIG_ENCODING_ASCII
	},
	{ NULL, ONIG_ENCODING_UNDEF }
};
/* }}} */

/* {{{ php_mb_regex_name2mbctype */
OnigEncoding php_mb_regex_name2mbctype(const char *pname)
{
	const char *p;
	php_mb_regex_enc_name_map_t *mapping;

	if (pname == NULL) {
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

/* {{{ php_mbregex_mbctype2name */
const char *php_mb_regex_mbctype2name(OnigEncoding mbctype)
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

/*
 * regex cache
 */
/* {{{ php_mbregex_compile_pattern */
static php_mb_regex_t *php_mbregex_compile_pattern(const char *pattern, int patlen, OnigOptionType options, OnigEncoding enc, OnigSyntaxType *syntax TSRMLS_DC)
{
	int err_code = 0;
	int found = 0;
	php_mb_regex_t *retval = NULL, **rc = NULL;
	OnigErrorInfo err_info;
	OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];

	found = zend_hash_find(&MBSTRG(ht_rc), (char *)pattern, patlen+1, (void **) &rc);
	if (found == FAILURE || (*rc)->options != options || (*rc)->enc != enc || (*rc)->syntax != syntax) {
		if ((err_code = onig_new(&retval, (OnigUChar *)pattern, (OnigUChar *)(pattern + patlen), options, enc, syntax, &err_info)) != ONIG_NORMAL) {
			onig_error_code_to_str(err_str, err_code, err_info);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "mbregex compile err: %s", err_str);
			retval = NULL;
			goto out;
		}
		zend_hash_update(&MBSTRG(ht_rc), (char *) pattern, patlen + 1, (void *) &retval, sizeof(retval), NULL);
	} else if (found == SUCCESS) {
		retval = *rc;
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
 * php funcions
 */

/* {{{ proto string mb_regex_encoding([string encoding])
   Returns the current encoding for regex as a string. */
PHP_FUNCTION(mb_regex_encoding)
{
	zval **arg1;
	OnigEncoding mbctype;

	if (ZEND_NUM_ARGS() == 0) {
		const char *retval = php_mb_regex_mbctype2name(MBSTRG(current_mbctype));
		if ( retval != NULL ) {
			RETVAL_STRING((char *)retval, 1);
		} else {
			RETVAL_FALSE;
		}
	} else if (ZEND_NUM_ARGS() == 1 &&
	           zend_get_parameters_ex(1, &arg1) != FAILURE) {
		convert_to_string_ex(arg1);
		mbctype = php_mb_regex_name2mbctype(Z_STRVAL_PP(arg1));
		if (mbctype == ONIG_ENCODING_UNDEF) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown encoding \"%s\"", Z_STRVAL_PP(arg1));
			RETVAL_FALSE;
		} else {
			MBSTRG(current_mbctype) = mbctype;
			RETVAL_TRUE;
		}
	} else {
		WRONG_PARAM_COUNT;
	}
}
/* }}} */

/* {{{ _php_mb_regex_ereg_exec */
static void _php_mb_regex_ereg_exec(INTERNAL_FUNCTION_PARAMETERS, int icase)
{
	zval **arg_pattern, *array;
	char *string;
	int string_len;
	php_mb_regex_t *re;
	OnigRegion *regs = NULL;
	int i, match_len, beg, end;
	OnigOptionType options;
	char *str;

	array = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zs|z", &arg_pattern, &string, &string_len, &array) == FAILURE) {
		RETURN_FALSE;
	}

	options = MBSTRG(regex_default_options);
	if (icase) {
		options |= ONIG_OPTION_IGNORECASE;
	}

	/* compile the regular expression from the supplied regex */
	if (Z_TYPE_PP(arg_pattern) != IS_STRING) {
		/* we convert numbers to integers and treat them as a string */
		if (Z_TYPE_PP(arg_pattern) == IS_DOUBLE) {
			convert_to_long_ex(arg_pattern);	/* get rid of decimal places */
		}
		convert_to_string_ex(arg_pattern);
		/* don't bother doing an extended regex with just a number */
	}

	if (!Z_STRVAL_PP(arg_pattern) || Z_STRLEN_PP(arg_pattern) == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "empty pattern");
		RETVAL_FALSE;
		goto out;
	}

	re = php_mbregex_compile_pattern(Z_STRVAL_PP(arg_pattern), Z_STRLEN_PP(arg_pattern), options, MBSTRG(current_mbctype), MBSTRG(regex_default_syntax) TSRMLS_CC);
	if (re == NULL) {
		RETVAL_FALSE;
		goto out;
	}

	regs = onig_region_new();

	/* actually execute the regular expression */
	if (onig_search(re, (OnigUChar *)string, (OnigUChar *)(string + string_len), string, (OnigUChar *)(string + string_len), regs, 0) < 0) {
		RETVAL_FALSE;
		goto out;
	}

	match_len = 1;
	str = string;
	if (array != NULL) {
		match_len = regs->end[0] - regs->beg[0];
		zval_dtor(array);
		array_init(array);
		for (i = 0; i < regs->num_regs; i++) {
			beg = regs->beg[i];
			end = regs->end[i];
			if (beg >= 0 && beg < end && end <= string_len) {
				add_index_stringl(array, i, (char *)&str[beg], end - beg, 1);
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
static void _php_mb_regex_ereg_replace_exec(INTERNAL_FUNCTION_PARAMETERS, OnigOptionType options)
{
	zval **arg_pattern_zval;

	char *arg_pattern;
	int arg_pattern_len;

	char *replace;
	int replace_len;

	char *string;
	int string_len;

	char *p;
	php_mb_regex_t *re;
	OnigSyntaxType *syntax;
	OnigRegion *regs = NULL;
	smart_str out_buf = { 0 };
	smart_str eval_buf = { 0 };
	smart_str *pbuf;
	int i, err, eval, n;
	OnigUChar *pos;
	OnigUChar *string_lim;
	char *description = NULL;
	char pat_buf[2];

	const mbfl_encoding *enc;

	{
		const char *current_enc_name;
		current_enc_name = php_mb_regex_mbctype2name(MBSTRG(current_mbctype));
		if (current_enc_name == NULL ||
			(enc = mbfl_name2encoding(current_enc_name)) == NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown error");
			RETURN_FALSE;
		}
	}
	eval = 0;
	{
		char *option_str = NULL;
		int option_str_len = 0;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zss|s",
									&arg_pattern_zval,
									&replace, &replace_len,
									&string, &string_len,
									&option_str, &option_str_len) == FAILURE) {
			RETURN_FALSE;
		}

		if (option_str != NULL) {
			_php_mb_regex_init_options(option_str, option_str_len, &options, &syntax, &eval);
		} else {
			options |= MBSTRG(regex_default_options);
			syntax = MBSTRG(regex_default_syntax);
		}
	}
	if (Z_TYPE_PP(arg_pattern_zval) == IS_STRING) {
		arg_pattern = Z_STRVAL_PP(arg_pattern_zval);
		arg_pattern_len = Z_STRLEN_PP(arg_pattern_zval);
	} else {
		/* FIXME: this code is not multibyte aware! */
		convert_to_long_ex(arg_pattern_zval);
		pat_buf[0] = (char)Z_LVAL_PP(arg_pattern_zval);	
		pat_buf[1] = '\0';

		arg_pattern = pat_buf;
		arg_pattern_len = 1;	
	}
	/* create regex pattern buffer */
	re = php_mbregex_compile_pattern(arg_pattern, arg_pattern_len, options, MBSTRG(current_mbctype), syntax TSRMLS_CC);
	if (re == NULL) {
		RETURN_FALSE;
	}

	if (eval) {
		pbuf = &eval_buf;
		description = zend_make_compiled_string_description("mbregex replace" TSRMLS_CC);
	} else {
		pbuf = &out_buf;
		description = NULL;
	}

	/* do the actual work */
	err = 0;
	pos = string;
	string_lim = (OnigUChar*)(string + string_len);
	regs = onig_region_new();
	while (err >= 0) {
		err = onig_search(re, (OnigUChar *)string, (OnigUChar *)string_lim, pos, (OnigUChar *)string_lim, regs, 0);
		if (err <= -2) {
			OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];
			onig_error_code_to_str(err_str, err);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "mbregex search failure in php_mbereg_replace_exec(): %s", err_str);
			break;
		}
		if (err >= 0) {
#if moriyoshi_0
			if (regs->beg[0] == regs->end[0]) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty regular expression");
				break;
			}
#endif
			/* copy the part of the string before the match */
			smart_str_appendl(&out_buf, pos, (size_t)((OnigUChar *)(string + regs->beg[0]) - pos));
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
			if (eval) {
				zval v;
				/* null terminate buffer */
				smart_str_appendc(&eval_buf, '\0');
				/* do eval */
				if (zend_eval_string(eval_buf.c, &v, description TSRMLS_CC) == FAILURE) {
					efree(description);
					php_error_docref(NULL TSRMLS_CC,E_ERROR, "Failed evaluating code: %s%s", PHP_EOL, eval_buf.c);
					/* zend_error() does not return in this case */
				}

				/* result of eval */
				convert_to_string(&v);
				smart_str_appendl(&out_buf, Z_STRVAL(v), Z_STRLEN(v));
				/* Clean up */
				eval_buf.len = 0;
				zval_dtor(&v);
			}
			n = regs->end[0];
			if ((size_t)(pos - (OnigUChar *)string) < n) {
				pos = string + n;
			} else {
				if (pos < string_lim) {
					smart_str_appendl(&out_buf, pos, 1); 
				}
				pos++;
			}
		} else { /* nomatch */
			/* stick that last bit of string on our output */
			if (string_lim - pos > 0) {
				smart_str_appendl(&out_buf, pos, string_lim - pos);
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
	} else {
		smart_str_appendc(&out_buf, '\0');
		RETVAL_STRINGL((char *)out_buf.c, out_buf.len - 1, 0);
	}
}
/* }}} */

/* {{{ proto string mb_ereg_replace(string pattern, string replacement, string string [, string option])
   Replace regular expression for multibyte string */
PHP_FUNCTION(mb_ereg_replace)
{
	_php_mb_regex_ereg_replace_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string mb_eregi_replace(string pattern, string replacement, string string)
   Case insensitive replace regular expression for multibyte string */
PHP_FUNCTION(mb_eregi_replace)
{
	_php_mb_regex_ereg_replace_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, ONIG_OPTION_IGNORECASE);
}
/* }}} */

/* {{{ proto array mb_split(string pattern, string string [, int limit])
   split multibyte string into array by regular expression */
PHP_FUNCTION(mb_split)
{
	char *arg_pattern;
	int arg_pattern_len;
	php_mb_regex_t *re;
	OnigRegion *regs = NULL;
	char *string;
	OnigUChar *pos;
	int string_len;

	int n, err;
	long count = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", &arg_pattern, &arg_pattern_len, &string, &string_len, &count) == FAILURE) {
		RETURN_FALSE;
	} 

	if (count == 0) {
		count = 1;
	}

	/* create regex pattern buffer */
	if ((re = php_mbregex_compile_pattern(arg_pattern, arg_pattern_len, MBSTRG(regex_default_options), MBSTRG(current_mbctype), MBSTRG(regex_default_syntax) TSRMLS_CC)) == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);

	pos = (OnigUChar *)string;
	err = 0;
	regs = onig_region_new();
	/* churn through str, generating array entries as we go */
	while ((--count != 0) &&
		   (err = onig_search(re, (OnigUChar *)string, (OnigUChar *)(string + string_len), pos, (OnigUChar *)(string + string_len), regs, 0)) >= 0) {
		if (regs->beg[0] == regs->end[0]) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty regular expression");
			break;
		}

		/* add it to the array */
		if (regs->beg[0] < string_len && regs->beg[0] >= (size_t)(pos - (OnigUChar *)string)) {
			add_next_index_stringl(return_value, pos, ((OnigUChar *)(string + regs->beg[0]) - pos), 1);
		} else {
			err = -2;
			break;
		}
		/* point at our new starting point */
		n = regs->end[0];
		if ((pos - (OnigUChar *)string) < n) {
			pos = (OnigUChar *)string + n;
		}
		if (count < 0) {
			count = 0;
		}
		onig_region_free(regs, 0);
	}

	onig_region_free(regs, 1);

	/* see if we encountered an error */
	if (err <= -2) {
		OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];
		onig_error_code_to_str(err_str, err);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "mbregex search failure in mbsplit(): %s", err_str);
		zval_dtor(return_value);
		RETURN_FALSE;
	}

	/* otherwise we just have one last element to add to the array */
	n = ((OnigUChar *)(string + string_len) - pos);
	if (n > 0) {
		add_next_index_stringl(return_value, pos, n, 1);
	} else {
		add_next_index_stringl(return_value, "", 0, 1);
	}
}
/* }}} */

/* {{{ proto bool mb_ereg_match(string pattern, string string [,string option])
   Regular expression match for multibyte string */
PHP_FUNCTION(mb_ereg_match)
{
	char *arg_pattern;
	int arg_pattern_len;

	char *string;
	int string_len;

	php_mb_regex_t *re;
	OnigSyntaxType *syntax;
	int option = 0, err;

	{
		char *option_str = NULL;
		int option_str_len = 0;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s",
		                          &arg_pattern, &arg_pattern_len, &string, &string_len,
		                          &option_str, &option_str_len)==FAILURE) {
			RETURN_FALSE;
		}

		if (option_str != NULL) {
			_php_mb_regex_init_options(option_str, option_str_len, &option, &syntax, NULL);
		} else {
			option |= MBSTRG(regex_default_options);
			syntax = MBSTRG(regex_default_syntax);
		}
	}

	if ((re = php_mbregex_compile_pattern(arg_pattern, arg_pattern_len, option, MBSTRG(current_mbctype), syntax TSRMLS_CC)) == NULL) {
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
	zval **arg_pattern, **arg_options;
	int n, i, err, pos, len, beg, end, option;
	OnigUChar *str;
	OnigSyntaxType *syntax;

	option = MBSTRG(regex_default_options);
	switch (ZEND_NUM_ARGS()) {
	case 0:
		break;
	case 1:
		if (zend_get_parameters_ex(1, &arg_pattern) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (zend_get_parameters_ex(2, &arg_pattern, &arg_options) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(arg_options);
		option = 0;
		_php_mb_regex_init_options(Z_STRVAL_PP(arg_options), Z_STRLEN_PP(arg_options), &option, &syntax, NULL);
		break;
	default:
		WRONG_PARAM_COUNT;
		break;
	}
	if (ZEND_NUM_ARGS() > 0) {
		/* create regex pattern buffer */
		convert_to_string_ex(arg_pattern);

		if ((MBSTRG(search_re) = php_mbregex_compile_pattern(Z_STRVAL_PP(arg_pattern), Z_STRLEN_PP(arg_pattern), option, MBSTRG(current_mbctype), MBSTRG(regex_default_syntax) TSRMLS_CC)) == NULL) {
			RETURN_FALSE;
		}
	}

	pos = MBSTRG(search_pos);
	str = NULL;
	len = 0;
	if (MBSTRG(search_str) != NULL && Z_TYPE_P(MBSTRG(search_str)) == IS_STRING){
		str = (OnigUChar *)Z_STRVAL_P(MBSTRG(search_str));
		len = Z_STRLEN_P(MBSTRG(search_str));
	}

	if (MBSTRG(search_re) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No regex given");
		RETURN_FALSE;
	}

	if (str == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No string given");
		RETURN_FALSE;
	}

	if (MBSTRG(search_regs)) {
		onig_region_free(MBSTRG(search_regs), 1);
	}
	MBSTRG(search_regs) = onig_region_new();

	err = onig_search(MBSTRG(search_re), str, str + len, str + pos, str  + len, MBSTRG(search_regs), 0);
	if (err == ONIG_MISMATCH) {
		MBSTRG(search_pos) = len;
		RETVAL_FALSE;
	} else if (err <= -2) {
		OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];
		onig_error_code_to_str(err_str, err);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "mbregex search failure in mbregex_search(): %s", err_str);
		RETVAL_FALSE;
	} else {
		if (MBSTRG(search_regs)->beg[0] == MBSTRG(search_regs)->end[0]) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty regular expression");
		}
		switch (mode) {
		case 1:
			array_init(return_value);
			beg = MBSTRG(search_regs)->beg[0];
			end = MBSTRG(search_regs)->end[0];
			add_next_index_long(return_value, beg);
			add_next_index_long(return_value, end - beg);
			break;
		case 2:
			array_init(return_value);
			n = MBSTRG(search_regs)->num_regs;
			for (i = 0; i < n; i++) {
				beg = MBSTRG(search_regs)->beg[i];
				end = MBSTRG(search_regs)->end[i];
				if (beg >= 0 && beg <= end && end <= len) {
					add_index_stringl(return_value, i, (char *)&str[beg], end - beg, 1);
				} else {
					add_index_bool(return_value, i, 0);
				}
			}
			break;
		default:
			RETVAL_TRUE;
			break;
		}
		end = MBSTRG(search_regs)->end[0];
		if (pos < end) {
			MBSTRG(search_pos) = end;
		} else {
			MBSTRG(search_pos) = pos + 1;
		}
	}

	if (err < 0) {
		onig_region_free(MBSTRG(search_regs), 1);
		MBSTRG(search_regs) = (OnigRegion *)NULL;
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
	zval **arg_str, **arg_pattern, **arg_options;
	OnigSyntaxType *syntax = NULL;
	int option;

	option = MBSTRG(regex_default_options);
	syntax = MBSTRG(regex_default_syntax);
	switch (ZEND_NUM_ARGS()) {
	case 1:
		if (zend_get_parameters_ex(1, &arg_str) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (zend_get_parameters_ex(2, &arg_str, &arg_pattern) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 3:
		if (zend_get_parameters_ex(3, &arg_str, &arg_pattern, &arg_options) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(arg_options);
		option = 0;
		_php_mb_regex_init_options(Z_STRVAL_PP(arg_options), Z_STRLEN_PP(arg_options), &option, &syntax, NULL);
		break;
	default:
		WRONG_PARAM_COUNT;
		break;
	}
	convert_to_string_ex(arg_str);
	if (ZEND_NUM_ARGS() > 1) {
		/* create regex pattern buffer */
		convert_to_string_ex(arg_pattern);

		if ((MBSTRG(search_re) = php_mbregex_compile_pattern(Z_STRVAL_PP(arg_pattern), Z_STRLEN_PP(arg_pattern), option, MBSTRG(current_mbctype), syntax TSRMLS_CC)) == NULL) {
			RETURN_FALSE;
		}
	}

	if (MBSTRG(search_str) != NULL) {
		zval_ptr_dtor(&MBSTRG(search_str));
		MBSTRG(search_str) = (zval *)NULL;
	}

	MBSTRG(search_str) = *arg_str;
	ZVAL_ADDREF(MBSTRG(search_str));
	SEPARATE_ZVAL_IF_NOT_REF(&MBSTRG(search_str));

	MBSTRG(search_pos) = 0;

	if (MBSTRG(search_regs) != NULL) {
		onig_region_free(MBSTRG(search_regs), 1);
		MBSTRG(search_regs) = (OnigRegion *) NULL;
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

	if (MBSTRG(search_regs) != NULL && Z_TYPE_P(MBSTRG(search_str)) == IS_STRING && Z_STRVAL_P(MBSTRG(search_str)) != NULL) {
		array_init(return_value);

		str = (OnigUChar *)Z_STRVAL_P(MBSTRG(search_str));
		len = Z_STRLEN_P(MBSTRG(search_str));
		n = MBSTRG(search_regs)->num_regs;
		for (i = 0; i < n; i++) {
			beg = MBSTRG(search_regs)->beg[i];
			end = MBSTRG(search_regs)->end[i];
			if (beg >= 0 && beg <= end && end <= len) {
				add_index_stringl(return_value, i, (char *)&str[beg], end - beg, 1);
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
	RETVAL_LONG(MBSTRG(search_pos));
}
/* }}} */

/* {{{ proto bool mb_ereg_search_setpos(int position)
   Set search start position */
PHP_FUNCTION(mb_ereg_search_setpos)
{
	zval **arg_pos;
	int n;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg_pos) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(arg_pos);
	n = Z_LVAL_PP(arg_pos);
	if (n < 0 || (MBSTRG(search_str) != NULL && Z_TYPE_P(MBSTRG(search_str)) == IS_STRING && n >= Z_STRLEN_P(MBSTRG(search_str)))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Position is out of range");
		MBSTRG(search_pos) = 0;
		RETVAL_FALSE;
	} else {
		MBSTRG(search_pos) = n;
		RETVAL_TRUE;
	}
}
/* }}} */

/* {{{ php_mb_regex_set_options */
void php_mb_regex_set_options(OnigOptionType options, OnigSyntaxType *syntax, OnigOptionType *prev_options, OnigSyntaxType **prev_syntax TSRMLS_DC) 
{
	if (prev_options != NULL) {
		*prev_options = MBSTRG(regex_default_options);
	}
	if (prev_syntax != NULL) {
		*prev_syntax = MBSTRG(regex_default_syntax);
	}
	MBSTRG(regex_default_options) = options;
	MBSTRG(regex_default_syntax) = syntax;
}
/* }}} */

/* {{{ proto string mb_regex_set_options([string options])
   Set or get the default options for mbregex functions */
PHP_FUNCTION(mb_regex_set_options)
{
	OnigOptionType opt;
	OnigSyntaxType *syntax;
	char *string = NULL;
	int string_len;
	char buf[16];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s",
	                          &string, &string_len) == FAILURE) {
		RETURN_FALSE;
	}
	if (string != NULL) {
		opt = 0;
		syntax = NULL;
		_php_mb_regex_init_options(string, string_len, &opt, &syntax, NULL);
		php_mb_regex_set_options(opt, syntax, NULL, NULL TSRMLS_CC);
	} else {
		opt = MBSTRG(regex_default_options);
		syntax = MBSTRG(regex_default_syntax);
	}
	_php_mb_regex_get_option_string(buf, sizeof(buf), opt, syntax);

	RETVAL_STRING(buf, 1);
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
