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
#include "php_mbregex.h"
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
_php_mb_regex_strbuf_init(struct strbuf *pd)
{
	if (pd) {
		pd->buffer = (unsigned char*)0;
		pd->length = 0;
		pd->pos = 0;
		pd->allocsz = 64;
	}
}

static int
_php_mb_regex_strbuf_ncat(struct strbuf *pd, const unsigned char *psrc, int len)
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
static int _php_mb_regex_name2mbctype(const char *pname)
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

static size_t 
_php_mb_regex_get_option_string(char *str, size_t len, int option)
{
	size_t len_left = len;
	size_t len_req = 0;
	char *p = str;


	if ((option & MBRE_OPTION_IGNORECASE) != 0) {
		if (len_left > 0) {
			--len_left;
			*(p++) = 'i';
		}
		++len_req;	
	}

	if ((option & MBRE_OPTION_EXTENDED) != 0) {
		if (len_left > 0) {
			--len_left;
			*(p++) = 'x';
		}
		++len_req;	
	}

	if ((option & MBRE_OPTION_POSIXLINE) == MBRE_OPTION_POSIXLINE) {
		if (len_left > 0) {
			--len_left;
			*(p++) = 'p';
		}
		++len_req;	
	} else {
		if ((option & MBRE_OPTION_MULTILINE) != 0) {
			if (len_left > 0) {
				--len_left;
				*(p++) = 'm';
			}
			++len_req;	
		}

		if ((option & MBRE_OPTION_SINGLELINE) != 0) {
			if (len_left > 0) {
				--len_left;
				*(p++) = 's';
			}
			++len_req;	
		}
	}	
	if ((option & MBRE_OPTION_LONGEST) != 0) {
		if (len_left > 0) {
			--len_left;
			*(p++) = 'l';
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

static void
_php_mb_regex_init_options(const char *parg, int narg, int *option, int *eval) 
{
	int n;
	char c;
	int optm = 0; 

	if (parg != NULL) {
		n = 0;
		while(n < narg) {
			c = parg[n++];
			switch (c) {
				case 'i':
					optm |= MBRE_OPTION_IGNORECASE;
					break;
				case 'x':
					optm |= MBRE_OPTION_EXTENDED;
					break;
				case 'm':
					optm |= MBRE_OPTION_MULTILINE;
					break;
				case 's':
					optm |= MBRE_OPTION_SINGLELINE;
					break;
				case 'p':
					optm |= MBRE_OPTION_POSIXLINE;
					break;
				case 'l':
					optm |= MBRE_OPTION_LONGEST;
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
			RETVAL_STRING((char *)retval, 1);
		} else {
			RETVAL_FALSE;
		}
	} else if (ZEND_NUM_ARGS() == 1 &&
	           zend_get_parameters_ex(1, &arg1) != FAILURE) {
		convert_to_string_ex(arg1);
		mbctype = _php_mb_regex_name2mbctype(Z_STRVAL_PP(arg1));
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


/* {{{ _php_mb_regex_ereg_exec */
static void
_php_mb_regex_ereg_exec(INTERNAL_FUNCTION_PARAMETERS, int icase)
{
	zval tmp;
	zval *arg_pattern, *array;
	char *string;
	int string_len;
	mb_regex_t re;
	struct mbre_registers regs = {0, 0, 0, 0};
	int i, err, match_len, option, beg, end;
	char *str;

	array = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zs|z", &arg_pattern, &string, &string_len, &array) == FAILURE) {
		RETURN_FALSE;
	}

	option = MBSTRG(regex_default_options);
	if (icase) {
		option |= MBRE_OPTION_IGNORECASE;
	}

	/* compile the regular expression from the supplied regex */
	if (Z_TYPE_P(arg_pattern) != IS_STRING) {
		/* we convert numbers to integers and treat them as a string */
		tmp = *arg_pattern;
		zval_copy_ctor(&tmp);
		if (Z_TYPE_P(&tmp) == IS_DOUBLE) {
			convert_to_long(&tmp);	/* get rid of decimal places */
		}
		convert_to_string(&tmp);
		arg_pattern = &tmp;
		/* don't bother doing an extended regex with just a number */
	}
	err = php_mbregex_compile_pattern(
	     &re,
	     Z_STRVAL_P(arg_pattern),
	     Z_STRLEN_P(arg_pattern),
	     option, MBSTRG(current_mbctype) TSRMLS_CC);
	if (err) {
		RETVAL_FALSE;
		goto out;
	}

	/* actually execute the regular expression */
	err = mbre_search(
	     &re,
	     string,
	     string_len,
	      0, string_len,
	     &regs);
	if (err < 0) {
		mbre_free_registers(&regs);
		RETVAL_FALSE;
		goto out;
	}

	match_len = 1;
	str = string;
	if (array) {
		match_len = regs.end[0] - regs.beg[0];
		zval_dtor(array);	/* start with clean array */
		array_init(array);
		for (i = 0; i < regs.num_regs; i++) {
			beg = regs.beg[i];
			end = regs.end[i];
			if (beg >= 0 && beg < end && end <= string_len) {
				add_index_stringl(array, i, (char *)&str[beg], end - beg, 1);
			} else {
				add_index_bool(array, i, 0);
			}
		}
	}

	mbre_free_registers(&regs);
	if (match_len == 0) {
		match_len = 1;
	}
	RETVAL_LONG(match_len);
out:
	if (arg_pattern == &tmp) {
		zval_dtor(&tmp);
	}
}

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
static void
_php_mb_regex_ereg_replace_exec(INTERNAL_FUNCTION_PARAMETERS, int option)
{
	zval *arg_pattern_zval;

	char *arg_pattern;
	int arg_pattern_len;

	char *replace;
	int replace_len;

	char *string;
	int string_len;

	char *p;
	mb_regex_t re;
	struct mbre_registers regs = {0, 0, 0, 0};
	struct strbuf outdev, evaldev, *pdevice;
	int i, n, err, pos, eval;
	char *description = NULL;
	char pat_buf[2];

	eval = 0;
	{
		char *option_str = NULL;
		int option_str_len = 0;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zss|s",
									&arg_pattern_zval,
									&replace, &replace_len,
									&string, &string_len,
									&option_str, &option_str_len) == FAILURE) {
			RETURN_FALSE;
		}

		if (option_str != NULL) {
			_php_mb_regex_init_options(option_str, option_str_len, &option, &eval);
		} else {
			option |= MBSTRG(regex_default_options);
		}
	}
	if (Z_TYPE_P(arg_pattern_zval) == IS_STRING) {
		arg_pattern = Z_STRVAL_P(arg_pattern_zval);
		arg_pattern_len = Z_STRLEN_P(arg_pattern_zval);
	} else {
		/* FIXME: this code is not multibyte aware! */
		convert_to_long_ex(&arg_pattern_zval);
		pat_buf[0] = (char)Z_LVAL_P(arg_pattern_zval);	
		pat_buf[1] = '\0';

		arg_pattern = pat_buf;
		arg_pattern_len = 1;	
	}
	/* create regex pattern buffer */
	err = php_mbregex_compile_pattern(
	    &re,
	    arg_pattern,
	    arg_pattern_len,
	    option, MBSTRG(current_mbctype) TSRMLS_CC);
	if (err) {
		RETURN_FALSE;
	}

	/* initialize string buffer (auto reallocate buffer) */
	_php_mb_regex_strbuf_init(&outdev);
	_php_mb_regex_strbuf_init(&evaldev);
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
		if (err <= -2) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "mbregex search failure in php_mbereg_replace_exec()");
			break;
		}
		if (err >= 0) {
#if moriyoshi_0
			if ( regs.beg[0] == regs.end[0] ) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty regular expression");
				break;
			}
#endif
			/* copy the part of the string before the match */
			_php_mb_regex_strbuf_ncat(&outdev, (const unsigned char *)&string[pos], regs.beg[0] - pos);
			/* copy replacement and backrefs */
			/* FIXME: this code (\\digit replacement) is not mbyte aware! */ 
			i = 0;
			p = replace;
			while (i < replace_len) {
				n = -1;
				if (p[0] == '\\' && p[1] >= '0' && p[1] <= '9') {
					n = p[1] - '0';
				}
				if (n >= 0 && n < regs.num_regs) {
					if (regs.beg[n] >= 0 && regs.beg[n] < regs.end[n] && regs.end[n] <= string_len) {
						_php_mb_regex_strbuf_ncat(pdevice, (const unsigned char *)&string[regs.beg[n]], regs.end[n] - regs.beg[n]);
					}
					p += 2;
					i += 2;
				} else {
					_php_mb_regex_strbuf_ncat(pdevice, (const unsigned char *)p, 1);
					p++;
					i++;
				}
			}
			if (eval) {
				zval v;
				/* null terminate buffer */
				_php_mb_regex_strbuf_ncat(&evaldev, (const unsigned char *)"\0", 1);
				/* do eval */
				zend_eval_string((char *)evaldev.buffer, &v, description TSRMLS_CC);
				/* result of eval */
				convert_to_string(&v);
				_php_mb_regex_strbuf_ncat(&outdev, Z_STRVAL(v), Z_STRLEN(v));
				/* Clean up */
				evaldev.pos = 0;
				zval_dtor(&v);
			}
			n = regs.end[0];
			if (pos < n) {
				pos = n;
			} else {
				_php_mb_regex_strbuf_ncat(&outdev, (const unsigned char *)&string[pos], 1 ); 
				pos++;
			}
		} else { /* nomatch */
			/* stick that last bit of string on our output */
			_php_mb_regex_strbuf_ncat(&outdev, (const unsigned char *)&string[pos], string_len - pos);
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
	_php_mb_regex_strbuf_ncat(&outdev, (const unsigned char *)"\0", 1);
	if (err <= -2) {
		if (outdev.buffer) {
			efree((void*)outdev.buffer);
		}
		RETVAL_FALSE;
	} else {
		RETVAL_STRINGL((char *)outdev.buffer, n, 0);
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
	_php_mb_regex_ereg_replace_exec(INTERNAL_FUNCTION_PARAM_PASSTHRU, MBRE_OPTION_IGNORECASE);
}
/* }}} */


/* {{{ proto array mb_split(string pattern, string string [, int limit])
   split multibyte string into array by regular expression */
PHP_FUNCTION(mb_split)
{
	char *arg_pattern;
	int arg_pattern_len;
	mb_regex_t re;
	struct mbre_registers regs = {0, 0, 0, 0};
	char *string;
	int string_len;

	int n, err, pos;
	long count = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", &arg_pattern, &arg_pattern_len, &string, &string_len, &count) == FAILURE) {
		RETURN_FALSE;
	} 

	if (count == 0) {
		count = 1;
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	/* create regex pattern buffer */
	err = php_mbregex_compile_pattern(
	     &re,
	     arg_pattern,
	     arg_pattern_len,
	     MBSTRG(regex_default_options), MBSTRG(current_mbctype) TSRMLS_CC);
	if (err) {
		RETURN_FALSE;
	}

	pos = 0;
	err = 0;
	/* churn through str, generating array entries as we go */
	while ((--count != 0) &&
		   (err = mbre_search(&re, string, string_len, pos, string_len - pos, &regs)) >= 0) {
		if ( regs.beg[0] == regs.end[0] ) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty regular expression");
			break;
		}


		/* add it to the array */
		if ( regs.beg[0] < string_len && regs.beg[0] >= pos) {
			add_next_index_stringl(return_value, (char *)&string[pos], regs.beg[0]-pos, 1);
		} else {
			err = -2;
			break;
		}
		/* point at our new starting point */
		n = regs.end[0];
		if (pos < n) {
			pos = n;
		}
		if (count < 0) {
			count = 0;
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
		add_next_index_stringl(return_value, (char *)&string[pos], n, 1);
	} else {
		add_next_index_stringl(return_value, (char *)empty_string, 0, 1);
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

	mb_regex_t re;
	int option, err;

	option = 0;	
	{
		char *option_str = NULL;
		int option_str_len = 0;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s",
		                          &arg_pattern, &arg_pattern_len, &string, &string_len,
		                          &option_str, &option_str_len)==FAILURE) {
			RETURN_FALSE;
		}

		if (option_str != NULL) {
			_php_mb_regex_init_options(option_str, option_str_len, &option, NULL);
		} else {
			option |= MBSTRG(regex_default_options);
		}
	}

	err = php_mbregex_compile_pattern(
	    &re,
	    arg_pattern,
		arg_pattern_len,
	    option, MBSTRG(current_mbctype) TSRMLS_CC);

	if (err) {
		RETURN_FALSE;
	}

	/* match */
	err = mbre_match(&re, string, string_len, 0, NULL);
	if (err >= 0) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */


/* regex search */
static void
_php_mb_regex_ereg_search_exec(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	zval **arg_pattern, **arg_options;
	int n, i, err, pos, len, beg, end, option;
	unsigned char *str;

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
		_php_mb_regex_init_options(Z_STRVAL_PP(arg_options), Z_STRLEN_PP(arg_options), &option, NULL);
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
	if (MBSTRG(search_str) != NULL && Z_TYPE_PP(MBSTRG(search_str)) == IS_STRING){
		str = (unsigned char *)Z_STRVAL_PP(MBSTRG(search_str));
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

	err = mbre_search(MBSTRG(search_re), (const char *)str, len, pos, len - pos, MBSTRG(search_regs));
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
						add_index_stringl(return_value, i, (char *)&str[beg], end - beg, 1);
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
	int err, option;

	option = MBSTRG(regex_default_options);
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
		_php_mb_regex_init_options(Z_STRVAL_PP(arg_options), Z_STRLEN_PP(arg_options), &option, NULL);
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
		str = (unsigned char *)Z_STRVAL_PP(MBSTRG(search_str));
		len = Z_STRLEN_PP(MBSTRG(search_str));
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

/* {{{ php_mb_regex_set_options */
int php_mb_regex_set_options( int options TSRMLS_DC) 
{
	int prev_opt = MBSTRG(regex_default_options);
	MBSTRG(regex_default_options) = options;
	return prev_opt;
}
/* }}} */

/* {{{ php_mb_regex_set_options_by_string */
int php_mb_regex_set_options_by_string( const char *opt_str, int len TSRMLS_DC)
{
	int new_opt = 0;
	_php_mb_regex_init_options( opt_str, len, &new_opt, NULL);
	return php_mb_regex_set_options( new_opt TSRMLS_CC);
}
/* }}} */

/* {{{ proto string mb_regex_set_options([string options])
   Set or get the default options for mbregex functions */
PHP_FUNCTION(mb_regex_set_options)
{
	int opt;
	char *string = NULL;
	int string_len;
	char buf[16];

	if ( zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "|s",
	                            &string, &string_len ) == FAILURE ) {
		RETURN_FALSE;
	}
	if (string != NULL) {
		opt = php_mb_regex_set_options_by_string( (const char*)string,
		                                          string_len TSRMLS_CC );
	} else {
		opt = MBSTRG(regex_default_options);
	}
	_php_mb_regex_get_option_string(buf, sizeof(buf), opt);

	RETVAL_STRING(buf, 1);
}
/* }}} */

#endif	/* HAVE_MBREGEX */
