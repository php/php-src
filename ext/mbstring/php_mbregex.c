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
   | Author: Tsukada Takuya <tsukada@fminn.nagano.nagano.jp>              |
   +----------------------------------------------------------------------+
 */

/* $Id$ */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "mbregex.h"
#include "mbstring.h"

#if HAVE_MBREGEX

ZEND_EXTERN_MODULE_GLOBALS(mbstring)

/*
 * string buffer
 */
struct strbuf {
	unsigned char* buffer;
	int length;
	int pos;
	int allocsz;
};

static void
php_mbregex_strbuf_init(struct strbuf *pd)
{
	if (pd) {
		pd->buffer = (char*)0;
		pd->length = 0;
		pd->pos = 0;
		pd->allocsz = 64;
	}
}

static int
php_mbregex_strbuf_ncat(struct strbuf *pd, const unsigned char *psrc, int len)
{
	if (pd == NULL || psrc == NULL) {
		return -1;
	}

	if ((pd->pos + len) >= pd->length) {
		/* reallocate buffer */
		int newlen = pd->length + pd->allocsz + len;
		unsigned char *tmp = (unsigned char*)erealloc((void*)pd->buffer, newlen);
		if (tmp == NULL) {
			return -1;
		}
		pd->length = newlen;
		pd->buffer = tmp;
	}

	while (len > 0) {
		pd->buffer[pd->pos++] = *psrc++;
		len--;
	}

	return len;
}


/*
 * encoding name resolver
 */
int
php_mbregex_name2mbctype(const char *pname)
{
	int mbctype;

	mbctype = -1;
	if (pname != NULL) {
		if (strcasecmp("EUC-JP", pname) == 0
		    || strcasecmp("X-EUC-JP", pname) == 0
		    || strcasecmp("UJIS", pname) == 0
		    || strcasecmp("EUCJP", pname) == 0
		    || strcasecmp("EUC_JP", pname) == 0) {
			mbctype = MBCTYPE_EUC;
		} else if (strcasecmp("UTF-8", pname) == 0
		           || strcasecmp("UTF8", pname) == 0) {
			mbctype = MBCTYPE_UTF8;
		} else if (strcasecmp("SJIS", pname) == 0
		           || strcasecmp("CP932", pname) == 0
		           || strcasecmp("MS932", pname) == 0
		           || strcasecmp("SHIFT_JIS", pname) == 0 ) {
			mbctype = MBCTYPE_SJIS;
		} else if (strcasecmp("ASCII", pname) == 0) {
			mbctype = MBCTYPE_ASCII;
			mbctype = MBCTYPE_EUC;
		}
	}

	return mbctype;
}

static const char*
php_mbregex_mbctype2name(int mbctype)
{
	const char *p = NULL;

	if (mbctype == MBCTYPE_EUC) {
		p = "EUC-JP";
	} else if(mbctype == MBCTYPE_UTF8) {
		p = "UTF-8";
	} else if(mbctype == MBCTYPE_SJIS) {
		p = "SJIS";
	} else if(mbctype == MBCTYPE_ASCII) {
		p = "ascii";
	}

	return p;
}


/*
 * regex cache
 */
static int
php_mbregex_compile_pattern(mb_regex_t *pre, const char *pattern, int patlen, int options, int mbctype TSRMLS_DC)
{
	int res = 0;
	const char *err_str = NULL;
	mb_regex_t *rc = NULL;

	if(zend_hash_find(&MBSTRG(ht_rc), (char *)pattern, patlen+1, (void **) &rc) == FAILURE ||
			rc->options != options || rc->mbctype != mbctype) {
		memset(pre, 0, sizeof(*pre));
		pre->fastmap = (char*)emalloc((1 << MBRE_BYTEWIDTH)*sizeof(char));
		if (pre->fastmap) {
			pre->options = options;
			pre->mbctype = mbctype;
			err_str = mbre_compile_pattern(pattern, patlen, pre);
			if (!err_str) {
				zend_hash_update(&MBSTRG(ht_rc), (char *) pattern, patlen+1, (void *) pre, sizeof(*pre), NULL);
			} else {
				efree(pre->fastmap);
				pre->fastmap = (char*)0;
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "mbregex compile err: %s", err_str);
				res = 1;
			}
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to allocate memory in mbregex_compile_pattern");
			res = 1;
		}
	} else {
		memcpy(pre, rc, sizeof(*pre));
	}

	return res;
}

static void
php_mbregex_init_option(const char *parg, int narg, int *option, int *eval) 
{
	int n;
	char c;

	if (parg) {
		n = 0;
		while(n < narg) {
			c = parg[n++];
			if (option) {
				switch (c) {
				case 'i':
					*option |= MBRE_OPTION_IGNORECASE;
					break;
				case 'x':
					*option |= MBRE_OPTION_EXTENDED;
					break;
				case 'm':
					*option |= MBRE_OPTION_MULTILINE;
					break;
				case 's':
					*option |= MBRE_OPTION_SINGLELINE;
					break;
				case 'p':
					*option |= MBRE_OPTION_POSIXLINE;
					break;
				case 'l':
					*option |= MBRE_OPTION_LONGEST;
					break;
				default:
					break;
				}
			}
			if (eval && (c == 'e')) {
				*eval = 1;
			}
		}
	}
}


/*
 * php funcions
 */

/* {{{ proto string mb_regex_encoding([string encoding])
   Returns the current encoding for regex as a string. */
PHP_FUNCTION(mb_regex_encoding)
{
	zval **arg1;
	int mbctype;

	if (ZEND_NUM_ARGS() == 0) {
		const char *retval = php_mbregex_mbctype2name(MBSTRG(current_mbctype));
		if ( retval != NULL ) {
			RETVAL_STRING((char *)retval);
		} else {
			RETVAL_FALSE;
		}
	} else if (ZEND_NUM_ARGS() == 1 &&
	           zend_get_parameters_ex(1, &arg1) != FAILURE) {
		convert_to_string_ex(arg1);
		mbctype = php_mbregex_name2mbctype(Z_STRVAL_PP(arg1));
		if (mbctype < 0) {
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


/* regex match */
static void
php_mbereg_exec(INTERNAL_FUNCTION_PARAMETERS, int icase)
{
	zval **arg_pattern, **arg_string, **array = NULL;
	mb_regex_t re;
	struct mbre_registers regs = {0, 0, 0, 0};
	int i, err, match_len, string_len, option, beg, end;
	char *str;

	switch(ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &arg_pattern, &arg_string) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;

	case 3:
		if (zend_get_parameters_ex(3, &arg_pattern, &arg_string, &array) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;

	default:
		WRONG_PARAM_COUNT;
		break;
	}

	option = 0;
	if (icase) {
		option |= MBRE_OPTION_IGNORECASE;
	}

	/* compile the regular expression from the supplied regex */
	if (Z_TYPE_PP(arg_pattern) == IS_STRING) {
		option |= MBRE_OPTION_EXTENDED;
	} else {
		/* we convert numbers to integers and treat them as a string */
		if (Z_TYPE_PP(arg_pattern) == IS_DOUBLE) {
			convert_to_long_ex(arg_pattern);	/* get rid of decimal places */
		}
		convert_to_string_ex(arg_pattern);
		/* don't bother doing an extended regex with just a number */
	}
	err = php_mbregex_compile_pattern(
	     &re,
	     Z_STRVAL_PP(arg_pattern),
	     Z_STRLEN_PP(arg_pattern),
	     option, MBSTRG(current_mbctype) TSRMLS_CC);
	if (err) {
		RETURN_FALSE;
	}

	/* actually execute the regular expression */
	convert_to_string_ex(arg_string);
	err = mbre_search(
	     &re,
	     Z_STRVAL_PP(arg_string),
	     Z_STRLEN_PP(arg_string),
	      0, Z_STRLEN_PP(arg_string),
	     &regs);
	if (err < 0) {
		mbre_free_registers(&regs);
		RETURN_FALSE;
	}
	if (regs.beg[0] == regs.end[0]) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty regular expression");
	}
	match_len = 1;
	str = Z_STRVAL_PP(arg_string);
	if (array) {
		match_len = regs.end[0] - regs.beg[0];
		string_len = Z_STRLEN_PP(arg_string);
		zval_dtor(*array);	/* start with clean array */
		array_init(*array);
		for (i = 0; i < regs.num_regs; i++) {
			beg = regs.beg[i];
			end = regs.end[i];
			if (beg >= 0 && beg < end && end <= string_len) {
				add_index_stringl(*array, i, &str[beg], end - beg, 1);
			} else {
				add_index_bool(*array, i, 0);
			}
		}
	}

	mbre_free_registers(&regs);
	if (match_len == 0) {
		match_len = 1;
	}
	RETVAL_LONG(match_len);
}

/* {{{ proto int mb_ereg(string pattern, string string [, array registers])
   Regular expression match for multibyte string */
PHP_FUNCTION(mb_ereg)
{
	php_mbereg_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int mb_eregi(string pattern, string string [, array registers])
   Case-insensitive regular expression match for multibyte string */
PHP_FUNCTION(mb_eregi)
{
	php_mbereg_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */



/* regex replacement */
static void
php_mbereg_replace_exec(INTERNAL_FUNCTION_PARAMETERS, int option)
{
	zval **arg_pattern, **arg_replace, **arg_string, **arg_option;
	char *p, *string, *replace;
	mb_regex_t re;
	struct mbre_registers regs = {0, 0, 0, 0};
	struct strbuf outdev, evaldev, *pdevice;
	int i, n, err, pos, replace_len, string_len, eval;
	char *description = NULL;
	zval retval;

	eval = 0;
	switch(ZEND_NUM_ARGS()) {
	case 3:
		if (zend_get_parameters_ex(3, &arg_pattern, &arg_replace, &arg_string) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;

	case 4:
		if (zend_get_parameters_ex(4, &arg_pattern, &arg_replace, &arg_string, &arg_option) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(arg_option);
		option = 0;
		php_mbregex_init_option(Z_STRVAL_PP(arg_option), Z_STRLEN_PP(arg_option), &option, &eval);
		break;

	default:
		WRONG_PARAM_COUNT;
		break;
	}

	convert_to_string_ex(arg_pattern);
	/* create regex pattern buffer */
	err = php_mbregex_compile_pattern(
	    &re,
	    Z_STRVAL_PP(arg_pattern),
	    Z_STRLEN_PP(arg_pattern),
	    option, MBSTRG(current_mbctype) TSRMLS_CC);
	if (err) {
		RETURN_FALSE;
	}

	convert_to_string_ex(arg_replace);
	replace = Z_STRVAL_PP(arg_replace);
	replace_len = Z_STRLEN_PP(arg_replace);

	convert_to_string_ex(arg_string);
	string = Z_STRVAL_PP(arg_string);
	string_len = Z_STRLEN_PP(arg_string);

	/* initialize string buffer (auto reallocate buffer) */
	php_mbregex_strbuf_init(&outdev);
	php_mbregex_strbuf_init(&evaldev);
	outdev.allocsz = (string_len >> 2) + 8;

	if (eval) {
		pdevice = &evaldev;
		description = zend_make_compiled_string_description("mbregex replace" TSRMLS_CC);
	} else {
		pdevice = &outdev;
		description = NULL;
	}

	/* do the actual work */
	err = 0;
	pos = 0;
	while (err >= 0) {
		err = mbre_search(&re, string, string_len, pos, string_len - pos, &regs);
		if ( regs.beg[0] == regs.end[0] ) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty regular expression");
			break;
		}
		if (err <= -2) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "mbregex search failure in php_mbereg_replace_exec()");
			break;
		}
		if (err >= 0) {
			/* copy the part of the string before the match */
			php_mbregex_strbuf_ncat(&outdev, &string[pos], regs.beg[0] - pos);
			/* copy replacement and backrefs */
			i = 0;
			p = replace;
			while (i < replace_len) {
				n = -1;
				if (p[0] == '\\' && p[1] >= '0' && p[1] <= '9') {
					n = p[1] - '0';
				}
				if (n >= 0 && n < regs.num_regs) {
					if (regs.beg[n] >= 0 && regs.beg[n] < regs.end[n] && regs.end[n] <= string_len) {
						php_mbregex_strbuf_ncat(pdevice, &string[regs.beg[n]], regs.end[n] - regs.beg[n]);
					}
					p += 2;
					i += 2;
				} else {
					php_mbregex_strbuf_ncat(pdevice, p, 1);
					p++;
					i++;
				}
			}
			if (eval) {
				/* null terminate buffer */
				php_mbregex_strbuf_ncat(&evaldev, "\0", 1);
				/* do eval */
				zend_eval_string(evaldev.buffer, &retval, description TSRMLS_CC);
				/* result of eval */
				convert_to_string(&retval);
				php_mbregex_strbuf_ncat(&outdev, retval.value.str.val, retval.value.str.len);
				/* Clean up */
				evaldev.pos = 0;
				zval_dtor(&retval);
			}
			n = regs.end[0];
			if (pos < n) {
				pos = n;
			} else {
				pos++;
			}
		} else { /* nomatch */
			/* stick that last bit of string on our output */
			php_mbregex_strbuf_ncat(&outdev, &string[pos], string_len - pos);
		}
	}

	if (description) {
		efree(description);
	}
	mbre_free_registers(&regs);
	if (evaldev.buffer) {
		efree((void*)evaldev.buffer);
	}
	n = outdev.pos;
	php_mbregex_strbuf_ncat(&outdev, "\0", 1);
	if (err <= -2) {
		if (outdev.buffer) {
			efree((void*)outdev.buffer);
		}
		RETVAL_FALSE;
	} else {
		RETVAL_STRINGL(outdev.buffer, n, 0);
	}
}

/* {{{ proto string mb_ereg_replace(string pattern, string replacement, string string [, string option])
   Replace regular expression for multibyte string */
PHP_FUNCTION(mb_ereg_replace)
{
	php_mbereg_replace_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, MBRE_OPTION_EXTENDED);
}
/* }}} */

/* {{{ proto string mb_eregi_replace(string pattern, string replacement, string string)
   Case insensitive replace regular expression for multibyte string */
PHP_FUNCTION(mb_eregi_replace)
{
	php_mbereg_replace_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, MBRE_OPTION_EXTENDED | MBRE_OPTION_IGNORECASE);
}
/* }}} */


/* {{{ proto array mb_split(string pattern, string string [, int limit])
   split multibyte string into array by regular expression */
PHP_FUNCTION(mb_split)
{
	zval **arg_pat, **arg_str, **arg_count = NULL;
	mb_regex_t re;
	struct mbre_registers regs = {0, 0, 0, 0};
	char *string;
	int n, err, count, string_len, pos;

	count = -1;
	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &arg_pat, &arg_str) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;

	case 3:
		if (zend_get_parameters_ex(3, &arg_pat, &arg_str, &arg_count) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(arg_count);
		count = Z_LVAL_PP(arg_count);
		break;

	default:
		WRONG_PARAM_COUNT;
		break;
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	convert_to_string_ex(arg_pat);
	convert_to_string_ex(arg_str);

	/* create regex pattern buffer */
	err = php_mbregex_compile_pattern(
	     &re,
	     Z_STRVAL_PP(arg_pat),
	     Z_STRLEN_PP(arg_pat),
	     MBRE_OPTION_EXTENDED, MBSTRG(current_mbctype) TSRMLS_CC);
	if (err) {
		RETURN_FALSE;
	}

	string = Z_STRVAL_PP(arg_str);
	string_len = Z_STRLEN_PP(arg_str);
	pos = 0;
	err = 0;
	/* churn through str, generating array entries as we go */
	while ((count != 0) &&
		   (err = mbre_search(&re, string, string_len, pos, string_len - pos, &regs)) >= 0) {
		if ( regs.beg[0] == regs.end[0] ) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty regular expression");
			break;
		}

		n = regs.beg[0];
		/* add it to the array */
		if (n < string_len && n <= pos) {
			n -= pos;
			add_next_index_stringl(return_value, &string[pos], n, 1);
		} else {
			err = -2;
			break;
		}
		/* point at our new starting point */
		n = regs.end[0];
		if (pos < n) {
			pos = n;
		} else {
			pos++;
		}
		/* if we're only looking for a certain number of points,
		   stop looking once we hit it */
		if (count > 0) {
			count--;
		}
	}

	mbre_free_registers(&regs);

	/* see if we encountered an error */
	if (err <= -2) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "mbregex search failure in mbsplit()");
		zval_dtor(return_value);
		RETURN_FALSE;
	}

	/* otherwise we just have one last element to add to the array */
	n = string_len - pos;
	if (n > 0) {
		add_next_index_stringl(return_value, &string[pos], n, 1);
	} else {
		add_next_index_stringl(return_value, empty_string, 0, 1);
	}
}
/* }}} */


/* {{{ proto bool mb_ereg_match(string pattern, string string [,string option])
   Regular expression match for multibyte string */
PHP_FUNCTION(mb_ereg_match)
{
	zval **arg_pattern, **arg_str, **arg_option;
	mb_regex_t re;
	int option, err;

	option = MBRE_OPTION_EXTENDED;
	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &arg_pattern, &arg_str) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 3:
		if (zend_get_parameters_ex(3, &arg_pattern, &arg_str, &arg_option) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(arg_option);
		option = 0;
		php_mbregex_init_option(Z_STRVAL_PP(arg_option), Z_STRLEN_PP(arg_option), &option, NULL);
		break;
	default:
		WRONG_PARAM_COUNT;
		break;
	}

	/* create regex pattern buffer */
	convert_to_string_ex(arg_pattern);
	err = php_mbregex_compile_pattern(
	    &re,
	    Z_STRVAL_PP(arg_pattern),
	    Z_STRLEN_PP(arg_pattern),
	    option, MBSTRG(current_mbctype) TSRMLS_CC);
	if (err) {
		RETURN_FALSE;
	}

	/* match */
	convert_to_string_ex(arg_str);
	err = mbre_match(&re, Z_STRVAL_PP(arg_str), Z_STRLEN_PP(arg_str), 0, NULL);
	if (err >= 0) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */


/* regex search */
static void
php_mbereg_search_exec(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	zval **arg_pattern, **arg_option;
	int n, i, err, pos, len, beg, end, option;
	unsigned char *str;

	option = MBRE_OPTION_EXTENDED;
	switch (ZEND_NUM_ARGS()) {
	case 0:
		break;
	case 1:
		if (zend_get_parameters_ex(1, &arg_pattern) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (zend_get_parameters_ex(2, &arg_pattern, &arg_option) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(arg_option);
		option = 0;
		php_mbregex_init_option(Z_STRVAL_PP(arg_option), Z_STRLEN_PP(arg_option), &option, NULL);
		break;
	default:
		WRONG_PARAM_COUNT;
		break;
	}
	if (ZEND_NUM_ARGS() > 0) {
		/* create regex pattern buffer */
		convert_to_string_ex(arg_pattern);
		if (!MBSTRG(search_re)) {
			MBSTRG(search_re) = (mb_regex_t*)ecalloc(1, sizeof(mb_regex_t));
		}
		err = php_mbregex_compile_pattern(
		    MBSTRG(search_re),
		    Z_STRVAL_PP(arg_pattern),
		    Z_STRLEN_PP(arg_pattern),
		    option, MBSTRG(current_mbctype) TSRMLS_CC);
		if (err) {
			efree(MBSTRG(search_re));
			MBSTRG(search_re) = (mb_regex_t*)0;
			RETURN_FALSE;
		}
	}

	pos = MBSTRG(search_pos);
	str = NULL;
	len = 0;
	if (Z_TYPE_PP(MBSTRG(search_str)) == IS_STRING){
		str = Z_STRVAL_PP(MBSTRG(search_str));
		len = Z_STRLEN_PP(MBSTRG(search_str));
	}

	if (!MBSTRG(search_re)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No regex given");
		RETURN_FALSE;
	}
	if (!str) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No string given");
		RETURN_FALSE;
	}
	if (MBSTRG(search_regs)) {
		mbre_free_registers(MBSTRG(search_regs));
		memset(MBSTRG(search_regs), 0, sizeof(struct mbre_registers));
	} else {
		MBSTRG(search_regs) = (struct mbre_registers*)ecalloc(1, sizeof(struct mbre_registers));
	}

	err = mbre_search(MBSTRG(search_re), str, len, pos, len - pos, MBSTRG(search_regs));
	if (err <= -2) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "mbregex search failure in mbregex_search()");
		RETVAL_FALSE;
	} else if (err < 0) {
		MBSTRG(search_pos) = len;
		RETVAL_FALSE;
	} else {
		if (MBSTRG(search_regs)->beg[0] == MBSTRG(search_regs)->end[0]) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty regular expression");
		}
		switch (mode) {
		case 1:
			if (array_init(return_value) != FAILURE) {
				beg = MBSTRG(search_regs)->beg[0];
				end = MBSTRG(search_regs)->end[0];
				add_next_index_long(return_value, beg);
				add_next_index_long(return_value, end - beg);
			} else {
				RETVAL_FALSE;
			}
			break;
		case 2:
			if (array_init(return_value) != FAILURE) {
				n = MBSTRG(search_regs)->num_regs;
				for (i = 0; i < n; i++) {
					beg = MBSTRG(search_regs)->beg[i];
					end = MBSTRG(search_regs)->end[i];
					if (beg >= 0 && beg <= end && end <= len) {
						add_index_stringl(return_value, i, &str[beg], end - beg, 1);
					} else {
						add_index_bool(return_value, i, 0);
					}
				}
			} else {
				RETVAL_FALSE;
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
		mbre_free_registers(MBSTRG(search_regs));
		efree(MBSTRG(search_regs));
		MBSTRG(search_regs) = (struct mbre_registers*)0;
	}
}
/* }}} */


/* {{{ proto bool mb_ereg_search([string pattern[, string option]])
   Regular expression search for multibyte string */
PHP_FUNCTION(mb_ereg_search)
{
	php_mbereg_search_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */


/* {{{ proto array mb_ereg_search_pos([string pattern[, string option]])
   Regular expression search for multibyte string */
PHP_FUNCTION(mb_ereg_search_pos)
{
	php_mbereg_search_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


/* {{{ proto array mb_ereg_search_regs([string pattern[, string option]])
   Regular expression search for multibyte string */
PHP_FUNCTION(mb_ereg_search_regs)
{
	php_mbereg_search_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, 2);
}
/* }}} */


/* {{{ proto bool mb_ereg_search_init(string string [, string pattern[, string option]])
   Initialize string and regular expression for search. */
PHP_FUNCTION(mb_ereg_search_init)
{
	zval **arg_str, **arg_pattern, **arg_option;
	int err, option;

	option = MBRE_OPTION_EXTENDED;
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
		if (zend_get_parameters_ex(3, &arg_str, &arg_pattern, &arg_option) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_string_ex(arg_option);
		option = 0;
		php_mbregex_init_option(Z_STRVAL_PP(arg_option), Z_STRLEN_PP(arg_option), &option, NULL);
		break;
	default:
		WRONG_PARAM_COUNT;
		break;
	}
	if (ZEND_NUM_ARGS() > 1) {
		/* create regex pattern buffer */
		convert_to_string_ex(arg_pattern);
		if (!MBSTRG(search_re)) {
			MBSTRG(search_re) = (mb_regex_t*)ecalloc(1, sizeof(mb_regex_t));
		}
		err = php_mbregex_compile_pattern(
		    MBSTRG(search_re),
		    Z_STRVAL_PP(arg_pattern),
		    Z_STRLEN_PP(arg_pattern),
		    option, MBSTRG(current_mbctype) TSRMLS_CC);
		if (err) {
			efree(MBSTRG(search_re));
			MBSTRG(search_re) = (mb_regex_t*)0;
			RETURN_FALSE;
		}
	}

	if (MBSTRG(search_str)) {
		if (ZVAL_REFCOUNT(*MBSTRG(search_str)) > 1) {
			ZVAL_DELREF(*MBSTRG(search_str));
		} else {
			zval_dtor(*MBSTRG(search_str));
			FREE_ZVAL(*MBSTRG(search_str));
		}
		MBSTRG(search_str) = (zval **)0;
		MBSTRG(search_str_val) = (zval *)0;
	}

	if (PZVAL_IS_REF(*arg_str)) {
		ZVAL_ADDREF(*arg_str);
		MBSTRG(search_str_val) = *arg_str;
		MBSTRG(search_str) = &MBSTRG(search_str_val);
	} else {
		MAKE_STD_ZVAL(MBSTRG(search_str_val));
		*MBSTRG(search_str_val) = **arg_str;
		zval_copy_ctor(MBSTRG(search_str_val));
		MBSTRG(search_str_val)->refcount = 1;
		MBSTRG(search_str_val)->is_ref = 0;
		MBSTRG(search_str) = &MBSTRG(search_str_val);
		convert_to_string_ex(MBSTRG(search_str));
	}

	MBSTRG(search_pos) = 0;

	if (MBSTRG(search_regs)) {
		mbre_free_registers(MBSTRG(search_regs));
		efree(MBSTRG(search_regs));
		MBSTRG(search_regs) = (struct mbre_registers*)0;
	}

	RETURN_TRUE;
}
/* }}} */


/* {{{ proto array mb_ereg_search_getregs(void)
   Get matched substring of the last time */
PHP_FUNCTION(mb_ereg_search_getregs)
{
	int n, i, len, beg, end;
	unsigned char *str;

	if (MBSTRG(search_regs) && Z_TYPE_PP(MBSTRG(search_str)) == IS_STRING &&
	    Z_STRVAL_PP(MBSTRG(search_str)) && array_init(return_value) != FAILURE) {
		str = Z_STRVAL_PP(MBSTRG(search_str));
		len = Z_STRLEN_PP(MBSTRG(search_str));
		n = MBSTRG(search_regs)->num_regs;
		for (i = 0; i < n; i++) {
			beg = MBSTRG(search_regs)->beg[i];
			end = MBSTRG(search_regs)->end[i];
			if (beg >= 0 && beg <= end && end <= len) {
				add_index_stringl(return_value, i, &str[beg], end - beg, 1);
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
	if (n < 0
	    || ( MBSTRG(search_str) != NULL && *MBSTRG(search_str) != NULL &&
	         Z_TYPE_PP(MBSTRG(search_str)) == IS_STRING && 
	         n >= Z_STRLEN_PP(MBSTRG(search_str)) ) ) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Position is out of range");
		MBSTRG(search_pos) = 0;
		RETVAL_FALSE;
	} else {
		MBSTRG(search_pos) = n;
		RETVAL_TRUE;
	}
}
/* }}} */

#endif	/* HAVE_MBREGEX */
