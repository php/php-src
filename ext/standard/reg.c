/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Jim Winstead <jimw@php.net>                                 |
   |          Jaakko Hyvätti <jaakko@hyvatti.iki.fi>                      | 
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdio.h>
#include "php.h"
#include "php_string.h"
#include "reg.h"
#include "ext/standard/info.h"

#if 0
zend_module_entry regexp_module_entry = {
	"Regular Expressions", 
	reg_functions, 
	PHP_MINIT(regex), PHP_MSHUTDOWN(regex),
	NULL, NULL, PHP_MINFO(regex), 
	STANDARD_MODULE_PROPERTIES
};
#endif

#ifdef ZTS
int reg_globals_id;
#else
static php_reg_globals reg_globals;
#endif

typedef struct {
	regex_t preg;
	int cflags;
} reg_cache;

/* {{{ _php_regcomp
 */
static int _php_regcomp(regex_t *preg, const char *pattern, int cflags)
{
	int r = 0;
	int patlen = strlen(pattern);
	reg_cache *rc = NULL;
	TSRMLS_FETCH();
	
	if(zend_hash_find(&REG(ht_rc), (char *) pattern, patlen+1, (void **) &rc) == FAILURE ||
			rc->cflags != cflags) {
		r = regcomp(preg, pattern, cflags);
		if(!r) {
			reg_cache rcp;

			rcp.cflags = cflags;
			memcpy(&rcp.preg, preg, sizeof(*preg));
			zend_hash_update(&REG(ht_rc), (char *) pattern, patlen+1,
					(void *) &rcp, sizeof(rcp), NULL);
		}
	} else {
		memcpy(preg, &rc->preg, sizeof(*preg));
	}
	
	return r;
}
/* }}} */

static void _free_reg_cache(reg_cache *rc) 
{
	regfree(&rc->preg);
}

#undef regfree
#define regfree(a);
#undef regcomp
#define regcomp(a, b, c) _php_regcomp(a, b, c)
	
static void php_reg_init_globals(php_reg_globals *reg_globals TSRMLS_DC)
{
	zend_hash_init(&reg_globals->ht_rc, 0, NULL, (void (*)(void *)) _free_reg_cache, 1);
}

PHP_MINIT_FUNCTION(regex)
{
#ifdef ZTS
	ts_allocate_id(&reg_globals_id, sizeof(php_reg_globals), (ts_allocate_ctor) php_reg_init_globals, NULL);
#else
	php_reg_init_globals(&reg_globals TSRMLS_CC);
#endif

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(regex)
{
	zend_hash_destroy(&REG(ht_rc));
	return SUCCESS;
}

PHP_MINFO_FUNCTION(regex)
{
#if HSREGEX
	php_info_print_table_row(2, "Regex Library", "Bundled library enabled");
#else
	php_info_print_table_row(2, "Regex Library", "System library enabled");
#endif
}


/* This is the maximum number of (..) constructs we'll generate from a
   call to ereg() or eregi() with the optional third argument. */
#define  NS  10

/* {{{ php_reg_eprint
 * php_reg_eprint - convert error number to name
 */
static void php_reg_eprint(int err, regex_t *re) {
	char *buf = NULL, *message = NULL;
	size_t len;
	size_t buf_len;

#ifdef REG_ITOA
	/* get the length of the message */
	buf_len = regerror(REG_ITOA | err, re, NULL, 0);
	if (buf_len) {
		buf = (char *)emalloc(buf_len * sizeof(char));
		if (!buf) return; /* fail silently */
		/* finally, get the error message */
		regerror(REG_ITOA | err, re, buf, buf_len);
	}
#else
	buf_len = 0;
#endif
	len = regerror(err, re, NULL, 0);
	if (len) {
		message = (char *)emalloc((buf_len + len + 2) * sizeof(char));
		if (!message) {
			return; /* fail silently */
		}
		if (buf_len) {
			snprintf(message, buf_len, "%s: ", buf);
			buf_len += 1; /* so pointer math below works */
		}
		/* drop the message into place */
		regerror(err, re, message + buf_len, len);

		php_error(E_WARNING, "%s", message);
	}

	STR_FREE(buf);
	STR_FREE(message);
}
/* }}} */

/* {{{ php_ereg
 */
static void php_ereg(INTERNAL_FUNCTION_PARAMETERS, int icase)
{
	pval **regex,			/* Regular expression */
		**findin,		/* String to apply expression to */
		**array = NULL;		/* Optional register array */
	regex_t re;
	regmatch_t subs[NS];
	int err, i, match_len, string_len;
	int copts = 0;
	off_t start, end;
	char *buf = NULL;
	char *string = NULL;
	
	if (icase)
		copts |= REG_ICASE;

	switch(ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &regex, &findin) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		/* don't bother doing substring matching if we're not going
		   to make use of the information */
		copts |= REG_NOSUB;
		break;
	case 3:
		if (zend_get_parameters_ex(3, &regex, &findin, &array) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	default:
		WRONG_PARAM_COUNT;
	}


	/* compile the regular expression from the supplied regex */
	if ((*regex)->type == IS_STRING) {
		err = regcomp(&re, (*regex)->value.str.val, REG_EXTENDED | copts);
	} else {
		/* we convert numbers to integers and treat them as a string */
		if ((*regex)->type == IS_DOUBLE)
			convert_to_long_ex(regex);	/* get rid of decimal places */
		convert_to_string_ex(regex);
		/* don't bother doing an extended regex with just a number */
		err = regcomp(&re, (*regex)->value.str.val, copts);
	}

	if (err) {
		php_reg_eprint(err, &re);
		RETURN_FALSE;
	}

	/* make a copy of the string we're looking in */
	convert_to_string_ex(findin);
	string = estrndup((*findin)->value.str.val, (*findin)->value.str.len);

	/* actually execute the regular expression */
	err = regexec(&re, string, (size_t) NS, subs, 0);
	if (err && err != REG_NOMATCH) {
		php_reg_eprint(err, &re);
		regfree(&re);
		RETURN_FALSE;
	}
	match_len = 1;

	if (array && err != REG_NOMATCH) {
		match_len = (int) (subs[0].rm_eo - subs[0].rm_so);
		string_len = strlen(string) + 1;

		buf = emalloc(string_len);
		if (!buf) {
			php_error(E_WARNING, "Unable to allocate memory in php_ereg");
			RETURN_FALSE;
		}

		pval_destructor(*array);	/* start with clean array */
		array_init(*array);

		for (i = 0; i < NS; i++) {
			start = subs[i].rm_so;
			end = subs[i].rm_eo;
			if (start != -1 && end > 0 && start < string_len && end < string_len && start < end) {
				add_index_stringl(*array, i, string+start, end-start, 1);
			} else {
				add_index_bool(*array, i, 0);
			}
		}
		efree(buf);
	}

	efree(string);
	if (err == REG_NOMATCH) {
		RETVAL_FALSE;
	} else {
		if (match_len == 0)
			match_len = 1;
		RETVAL_LONG(match_len);
	}
	regfree(&re);
}
/* }}} */

/* {{{ proto int ereg(string pattern, string string [, array registers])
   Regular expression match */
PHP_FUNCTION(ereg)
{
	php_ereg(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int eregi(string pattern, string string [, array registers])
   Case-insensitive regular expression match */
PHP_FUNCTION(eregi)
{
	php_ereg(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ php_reg_replace
 * this is the meat and potatoes of regex replacement! */
char *php_reg_replace(const char *pattern, const char *replace, const char *string, int icase, int extended)
{
	regex_t re;
	regmatch_t subs[NS];

	char *buf,	/* buf is where we build the replaced string */
	     *nbuf,	/* nbuf is used when we grow the buffer */
		 *walkbuf; /* used to walk buf when replacing backrefs */
	const char *walk; /* used to walk replacement string for backrefs */
	int buf_len;
	int pos, tmp, string_len, new_l;
	int err, copts = 0;

	string_len = strlen(string);

	if (icase)
		copts = REG_ICASE;
	if (extended)
		copts |= REG_EXTENDED;
	err = regcomp(&re, pattern, copts);
	if (err) {
		php_reg_eprint(err, &re);
		return ((char *) -1);
	}

	/* start with a buffer that is twice the size of the stringo
	   we're doing replacements in */
	buf_len = 2 * string_len + 1;
	buf = emalloc(buf_len * sizeof(char));
	if (!buf) {
		php_error(E_WARNING, "Unable to allocate memory in php_reg_replace");
		regfree(&re);
		return ((char *) -1);
	}

	err = pos = 0;
	buf[0] = '\0';

	while (!err) {
		err = regexec(&re, &string[pos], (size_t) NS, subs, (pos ? REG_NOTBOL : 0));

		if (err && err != REG_NOMATCH) {
			php_reg_eprint(err, &re);
			regfree(&re);
			return ((char *) -1);
		}
		if (!err) {
			/* backref replacement is done in two passes:
			   1) find out how long the string will be, and allocate buf
			   2) copy the part before match, replacement and backrefs to buf

			   Jaakko Hyvätti <Jaakko.Hyvatti@iki.fi>
			   */

			new_l = strlen(buf) + subs[0].rm_so; /* part before the match */
			walk = replace;
			while (*walk)
				if ('\\' == *walk
					&& '0' <= walk[1] && '9' >= walk[1]
					&& subs[walk[1] - '0'].rm_so > -1
					&& subs[walk[1] - '0'].rm_eo > -1) {
					new_l += subs[walk[1] - '0'].rm_eo
						- subs[walk[1] - '0'].rm_so;
					walk += 2;
				} else {
					new_l++;
					walk++;
				}

			if (new_l + 1 > buf_len) {
				buf_len = 1 + buf_len + 2 * new_l;
				nbuf = emalloc(buf_len);
				strcpy(nbuf, buf);
				efree(buf);
				buf = nbuf;
			}
			tmp = strlen(buf);
			/* copy the part of the string before the match */
			strncat(buf, &string[pos], subs[0].rm_so);

			/* copy replacement and backrefs */
			walkbuf = &buf[tmp + subs[0].rm_so];
			walk = replace;
			while (*walk)
				if ('\\' == *walk
					&& '0' <= walk[1] && '9' >= walk[1]
					&& subs[walk[1] - '0'].rm_so > -1
					&& subs[walk[1] - '0'].rm_eo > -1) {
					tmp = subs[walk[1] - '0'].rm_eo
						- subs[walk[1] - '0'].rm_so;
					memcpy (walkbuf,
							&string[pos + subs[walk[1] - '0'].rm_so],
							tmp);
					walkbuf += tmp;
					walk += 2;
				} else
					*walkbuf++ = *walk++;
			*walkbuf = '\0';

			/* and get ready to keep looking for replacements */
			if (subs[0].rm_so == subs[0].rm_eo) {
				if (subs[0].rm_so + pos >= string_len)
					break;
				new_l = strlen (buf) + 1;
				if (new_l + 1 > buf_len) {
					buf_len = 1 + buf_len + 2 * new_l;
					nbuf = emalloc(buf_len * sizeof(char));
					strcpy(nbuf, buf);
					efree(buf);
					buf = nbuf;
				}
				pos += subs[0].rm_eo + 1;
				buf [new_l-1] = string [pos-1];
				buf [new_l] = '\0';
			} else {
				pos += subs[0].rm_eo;
			}
		} else { /* REG_NOMATCH */
			new_l = strlen(buf) + strlen(&string[pos]);
			if (new_l + 1 > buf_len) {
				buf_len = new_l + 1; /* now we know exactly how long it is */
				nbuf = emalloc(buf_len * sizeof(char));
				strcpy(nbuf, buf);
				efree(buf);
				buf = nbuf;
			}
			/* stick that last bit of string on our output */
			strcat(buf, &string[pos]);
		}
	}

	/* don't want to leak memory .. */
	regfree(&re);

	/* whew. */
	return (buf);
}
/* }}} */

/* {{{ php_ereg_replace
 */
static void php_ereg_replace(INTERNAL_FUNCTION_PARAMETERS, int icase)
{
	pval **arg_pattern,
		**arg_replace,
		**arg_string;
	char *pattern;
	char *string;
	char *replace;
	char *ret;
	
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg_pattern, &arg_replace, &arg_string) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if ((*arg_pattern)->type == IS_STRING) {
		if ((*arg_pattern)->value.str.val && (*arg_pattern)->value.str.len)
			pattern = estrndup((*arg_pattern)->value.str.val, (*arg_pattern)->value.str.len);
		else
			pattern = empty_string;
	} else {
		convert_to_long_ex(arg_pattern);
		pattern = emalloc(2);
		pattern[0] = (char) (*arg_pattern)->value.lval;
		pattern[1] = '\0';
	}

	if ((*arg_replace)->type == IS_STRING) {
		if ((*arg_replace)->value.str.val && (*arg_replace)->value.str.len)
			replace = estrndup((*arg_replace)->value.str.val, (*arg_replace)->value.str.len);
		else
			replace = empty_string;
	} else {
		convert_to_long_ex(arg_replace);
		replace = emalloc(2);
		replace[0] = (char) (*arg_replace)->value.lval;
		replace[1] = '\0';
	}

	convert_to_string_ex(arg_string);
	if ((*arg_string)->value.str.val && (*arg_string)->value.str.len)
		string = estrndup((*arg_string)->value.str.val, (*arg_string)->value.str.len);
	else
		string = empty_string;

	/* do the actual work */
	ret = php_reg_replace(pattern, replace, string, icase, 1);
	if (ret == (char *) -1) {
		RETVAL_FALSE;
	} else {
		RETVAL_STRING(ret, 1);
		STR_FREE(ret);
	}
	STR_FREE(string);
	STR_FREE(replace);
	STR_FREE(pattern);
}
/* }}} */

/* {{{ proto string ereg_replace(string pattern, string replacement, string string)
   Replace regular expression */
PHP_FUNCTION(ereg_replace)
{
	php_ereg_replace(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string eregi_replace(string pattern, string replacement, string string)
   Case insensitive replace regular expression */
PHP_FUNCTION(eregi_replace)
{
	php_ereg_replace(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ php_split
 */
static void php_split(INTERNAL_FUNCTION_PARAMETERS, int icase)
{
	pval **spliton, **str, **arg_count = NULL;
	regex_t re;
	regmatch_t subs[1];
	char *strp, *endp;
	int err, size, count, copts = 0;

	if (icase)
		copts = REG_ICASE;
	
	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &spliton, &str) == FAILURE)
			WRONG_PARAM_COUNT;
		count = -1;
		break;
	case 3:
		if (zend_get_parameters_ex(3, &spliton, &str, &arg_count) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_long_ex(arg_count);
		count = (*arg_count)->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;                                         
	}                                                                  

	convert_to_string_ex(spliton);                                        
	convert_to_string_ex(str);                                            

	strp = (*str)->value.str.val;
	endp = (*str)->value.str.val + strlen((*str)->value.str.val);

	err = regcomp(&re, (*spliton)->value.str.val, REG_EXTENDED | copts);
	if (err) {
		php_error(E_WARNING, "unexpected regex error (%d)", err);
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		regfree(&re);
		RETURN_FALSE;
	}

	/* churn through str, generating array entries as we go */
	while ((count == -1 || count > 1) && !(err = regexec(&re, strp, 1, subs, 0))) {
		if (subs[0].rm_so == 0 && subs[0].rm_eo) {
			/* match is at start of string, return empty string */
			add_next_index_stringl(return_value, empty_string, 0, 1);
			/* skip ahead the length of the regex match */
			strp+=subs[0].rm_eo;
		} else if (subs[0].rm_so==0 && subs[0].rm_eo==0) {
			/* No more matches */
			regfree(&re);
			php_error(E_WARNING, "bad regular expression for split()");
			zend_hash_destroy(return_value->value.ht);
			efree(return_value->value.ht);
			RETURN_FALSE;
		} else {
			/* On a real match */

			/* make a copy of the substring */
			size = subs[0].rm_so;
		
			/* add it to the array */
			add_next_index_stringl(return_value, strp, size, 1);

			/* point at our new starting point */
			strp = strp + subs[0].rm_eo;
		}

		/* if we're only looking for a certain number of points,
		   stop looking once we hit it */
		if (count != -1) {
			count--;
		}
	}

	/* see if we encountered an error */
	if (err && err != REG_NOMATCH) {
		php_error(E_WARNING, "unexpected regex error (%d)", err);
		regfree(&re);
		zend_hash_destroy(return_value->value.ht);
		efree(return_value->value.ht);
		RETURN_FALSE;
	}

	/* otherwise we just have one last element to add to the array */
	size = endp - strp;
	
	add_next_index_stringl(return_value, strp, size, 1);

	regfree(&re);
}
/* }}} */

/* ("root", "passwd", "uid", "gid", "other:stuff:like:/bin/sh")
   = split(":", $passwd_file, 5); */
/* {{{ proto array split(string pattern, string string [, int limit])
   Split string into array by regular expression */

PHP_FUNCTION(split)
{
	php_split(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

/* }}} */

/* {{{ proto array spliti(string pattern, string string [, int limit])
   Split string into array by regular expression case-insensitive */

PHP_FUNCTION(spliti)
{
	php_split(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

/* }}} */

/* {{{ proto string sql_regcase(string string)
   Make regular expression for case insensitive match */
PHPAPI PHP_FUNCTION(sql_regcase)
{
	pval **string;
	char *tmp;
	unsigned char c;
	register int i, j;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &string)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(string);
	
	tmp = (char *) emalloc((*string)->value.str.len*4+1);
	
	for (i=j=0; i<(*string)->value.str.len; i++) {
		c = (unsigned char) (*string)->value.str.val[i];
		if(isalpha(c)) {
			tmp[j++] = '[';
			tmp[j++] = toupper(c);
			tmp[j++] = tolower(c);
			tmp[j++] = ']';
		} else {
			tmp[j++] = c;
		}
	}
	tmp[j]=0;
	
	tmp = erealloc(tmp, j + 1);
	
	RETVAL_STRINGL(tmp, j, 0);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 tw=78 fdm=marker
 * vim<600: sw=4 ts=4 tw=78
 */
