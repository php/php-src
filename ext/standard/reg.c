/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Jim Winstead <jimw@php.net>                                 |
   |          Jaakko Hyvätti <jaakko@hyvatti.iki.fi>                      | 
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdio.h>
#include "php.h"
#include "php3_string.h"
#include "php_regex.h"
#include "reg.h"

unsigned char third_argument_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };

function_entry reg_functions[] = {
	{"ereg",			php3_ereg,			third_argument_force_ref },
	{"ereg_replace",	php3_eregreplace,	NULL },
	{"eregi",			php3_eregi,			third_argument_force_ref },
	{"eregi_replace",	php3_eregireplace,	NULL },
	{"split",			php3_split,			NULL},
	{"join",			php3_implode,		NULL},
	{"sql_regcase",		php3_sql_regcase,	NULL},
	{NULL, NULL, NULL}
};


static int php_minit_regex(INIT_FUNC_ARGS);
static int php_mshutdown_regex(SHUTDOWN_FUNC_ARGS);
static void php_info_regex(ZEND_MODULE_INFO_FUNC_ARGS);

zend_module_entry regexp_module_entry = {
	"Regular Expressions", 
	reg_functions, 
	php_minit_regex, php_mshutdown_regex,
	NULL, NULL, php_info_regex, 
	STANDARD_MODULE_PROPERTIES
};

#ifdef ZTS
int reg_globals_id;
#else
static php_reg_globals reg_globals;
#endif

typedef struct {
	regex_t preg;
	int cflags;
} reg_cache;

static int _php_regcomp(regex_t *preg, const char *pattern, int cflags)
{
	int r = 0;
	int patlen = strlen(pattern);
	reg_cache *rc = NULL;
	REGLS_FETCH();
	
	printf("called for pattern %s\n", pattern);
	if(_php3_hash_find(&REG(ht_rc), (char *) pattern, patlen+1, (void **) &rc) == FAILURE ||
			rc->cflags != cflags) {
		printf("compiling it\n");
		r = regcomp(preg, pattern, cflags);
		printf("regcomp returned %d\n", r);
		if(!r) {
			reg_cache rcp;

			rcp.cflags = cflags;
			memcpy(&rcp.preg, preg, sizeof(*preg));
			_php3_hash_update(&REG(ht_rc), (char *) pattern, patlen+1,
					(void *) &rcp, sizeof(rcp), NULL);
		}
	} else {
		printf("found it at %x\n", rc);
		memcpy(preg, &rc->preg, sizeof(*preg));
	}
	
	return r;
}

static void _free_reg_cache(reg_cache *rc) 
{
	regfree(&rc->preg);
}

#define regfree(a);
#define regcomp(a,b,c) _php_regcomp(a,b,c)
	
static void php_reg_init_globals(php_reg_globals *reg_globals) 
{
	_php3_hash_init(&reg_globals->ht_rc, 0, NULL, (void (*)(void *)) _free_reg_cache, 1);
}

static int php_minit_regex(INIT_FUNC_ARGS)
{
#ifdef ZTS
	reg_globals_id = ts_allocate_id(sizeof(php_reg_globals), php_reg_init_globals, NULL);
#else
	php_reg_init_globals(&reg_globals);
#endif

	return SUCCESS;
}

static int php_mshutdown_regex(SHUTDOWN_FUNC_ARGS)
{
	REGLS_FETCH();

	_php3_hash_destroy(&REG(ht_rc));
	return SUCCESS;
}

static void php_info_regex(ZEND_MODULE_INFO_FUNC_ARGS)
{
#if HSREGEX
	PUTS("Bundled regex library enabled\n");
#else
	PUTS("System regex library enabled\n");
#endif
}


/* This is the maximum number of (..) constructs we'll generate from a
   call to ereg() or eregi() with the optional third argument. */
#define  NS  10

/*
 * _php3_reg_eprint - convert error number to name
 */
static void _php3_reg_eprint(int err, regex_t *re) {
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

		php3_error(E_WARNING, "%s", message);
	}

	STR_FREE(buf);
	STR_FREE(message);
}

static void _php3_ereg(INTERNAL_FUNCTION_PARAMETERS, int icase)
{
	pval *regex,			/* Regular expression */
		*findin,		/* String to apply expression to */
		*array = NULL;		/* Optional register array */
	regex_t re;
	regmatch_t subs[NS];
	int err, i, match_len, string_len;
	int copts = 0;
	off_t start, end;
	char *buf = NULL;
	char *string = NULL;
	
	if (icase)
		copts |= REG_ICASE;

	switch(ARG_COUNT(ht)) {
	case 2:
		if (getParameters(ht, 2, &regex, &findin) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		/* don't bother doing substring matching if we're not going
		   to make use of the information */
		copts |= REG_NOSUB;
		break;
	case 3:
		if (getParameters(ht, 3, &regex, &findin, &array) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		if (!ParameterPassedByReference(ht, 3)) {
			php3_error(E_WARNING, "Array to be filled with values must be passed by reference.");
			RETURN_FALSE;
		}
		break;
	default:
		WRONG_PARAM_COUNT;
	}


	/* compile the regular expression from the supplied regex */
	if (regex->type == IS_STRING) {
		err = regcomp(&re, regex->value.str.val, REG_EXTENDED | copts);
	} else {
		/* we convert numbers to integers and treat them as a string */
		if (regex->type == IS_DOUBLE)
			convert_to_long(regex);	/* get rid of decimal places */
		convert_to_string(regex);
		/* don't bother doing an extended regex with just a number */
		err = regcomp(&re, regex->value.str.val, copts);
	}

	if (err) {
		_php3_reg_eprint(err, &re);
		RETURN_FALSE;
	}

	/* make a copy of the string we're looking in */
	convert_to_string(findin);
	string = estrndup(findin->value.str.val, findin->value.str.len);

	/* actually execute the regular expression */
	err = regexec(&re, string, (size_t) NS, subs, 0);
	if (err && err != REG_NOMATCH) {
		_php3_reg_eprint(err, &re);
		regfree(&re);
		RETURN_FALSE;
	}
	match_len = 1;

	if (array && err != REG_NOMATCH) {
		match_len = (int) (subs[0].rm_eo - subs[0].rm_so);
		string_len = strlen(string) + 1;

		buf = emalloc(string_len);
		if (!buf) {
			php3_error(E_WARNING, "Unable to allocate memory in _php3_ereg");
			RETURN_FALSE;
		}

		pval_destructor(array);	/* start with clean array */
		array_init(array);

		for (i = 0; i < NS; i++) {
			start = subs[i].rm_so;
			end = subs[i].rm_eo;
			if (start != -1 && end > 0 && start < string_len && end < string_len && start < end) {
				add_index_stringl(array, i, string+start, end-start, 1);
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

/* {{{ proto int ereg(string pattern, string string [, array registers])
   Regular expression match */
PHP_FUNCTION(ereg)
{
	_php3_ereg(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int eregi(string pattern, string string [, array registers])
   Case-insensitive regular expression match */
PHP_FUNCTION(eregi)
{
	_php3_ereg(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* this is the meat and potatoes of regex replacement! */
char *_php3_regreplace(const char *pattern, const char *replace, const char *string, int icase, int extended)
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
	if (!string_len)
		return (char *)string;

	if (icase)
		copts = REG_ICASE;
	if (extended)
		copts |= REG_EXTENDED;
	err = regcomp(&re, pattern, copts);
	if (err) {
		_php3_reg_eprint(err, &re);
		return ((char *) -1);
	}

	/* start with a buffer that is twice the size of the stringo
	   we're doing replacements in */
	buf_len = 2 * string_len + 1;
	buf = emalloc(buf_len * sizeof(char));
	if (!buf) {
		php3_error(E_WARNING, "Unable to allocate memory in _php3_regreplace");
		regfree(&re);
		return ((char *) -1);
	}

	err = pos = 0;
	buf[0] = '\0';

	while (!err) {
		err = regexec(&re, &string[pos], (size_t) NS, subs, (pos ? REG_NOTBOL : 0));

		if (err && err != REG_NOMATCH) {
			_php3_reg_eprint(err, &re);
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

static void _php3_eregreplace(INTERNAL_FUNCTION_PARAMETERS, int icase)
{
	pval *arg_pattern,
		*arg_replace,
		*arg_string;
	char *pattern;
	char *string;
	char *replace;
	char *ret;
	
	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg_pattern, &arg_replace, &arg_string) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (arg_pattern->type == IS_STRING) {
		if (arg_pattern->value.str.val && arg_pattern->value.str.len)
			pattern = estrndup(arg_pattern->value.str.val,arg_pattern->value.str.len);
		else
			pattern = empty_string;
	} else {
		convert_to_long(arg_pattern);
		pattern = emalloc(2);
		pattern[0] = (char) arg_pattern->value.lval;
		pattern[1] = '\0';
	}

	if (arg_replace->type == IS_STRING) {
		if (arg_replace->value.str.val && arg_replace->value.str.len)
			replace = estrndup(arg_replace->value.str.val, arg_replace->value.str.len);
		else
			replace = empty_string;
	} else {
		convert_to_long(arg_replace);
		replace = emalloc(2);
		replace[0] = (char) arg_replace->value.lval;
		replace[1] = '\0';
	}

	convert_to_string(arg_string);
	if (arg_string->value.str.val && arg_string->value.str.len)
		string = estrndup(arg_string->value.str.val, arg_string->value.str.len);
	else
		string = empty_string;

	/* do the actual work */
	ret = _php3_regreplace(pattern, replace, string, icase, 1);
	if (ret == (char *) -1) {
		RETVAL_FALSE;
	} else {
		RETVAL_STRING(ret,1);
		STR_FREE(ret);
	}
	STR_FREE(string);
	STR_FREE(replace);
	STR_FREE(pattern);
}

/* {{{ proto string ereg_replace(string pattern, string string [, array registers])
   Replace regular expression */
PHP_FUNCTION(eregreplace)
{
	_php3_eregreplace(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string eregi_replace(string pattern, string string [, array registers])
   Case insensitive replace regular expression */
PHP_FUNCTION(eregireplace)
{
	_php3_eregreplace(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* ("root", "passwd", "uid", "gid", "other:stuff:like:/bin/sh")
   = split(":", $passwd_file, 5); */
/* {{{ proto array split(string pattern, string string [, int limit])
   split string into array by regular expression */
PHP_FUNCTION(split)
{
	pval *spliton, *str, *arg_count = NULL;
	regex_t re;
	regmatch_t subs[1];
	char *strp, *endp;
	int err, size, count;
	
	switch (ARG_COUNT(ht)) {
	case 2:
		if (getParameters(ht, 2, &spliton, &str) == FAILURE)
			WRONG_PARAM_COUNT;
		count = -1;
		break;
	case 3:
		if (getParameters(ht, 3, &spliton, &str, &arg_count) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_long(arg_count);
		count = arg_count->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;                                         
	}                                                                  

	convert_to_string(spliton);                                        
	convert_to_string(str);                                            

	strp = str->value.str.val;
	endp = str->value.str.val + strlen(str->value.str.val);

	err = regcomp(&re, spliton->value.str.val, REG_EXTENDED);
	if (err) {
		php3_error(E_WARNING, "unexpected regex error (%d)", err);
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
			php3_error(E_WARNING, "bad regular expression for split()");
			_php3_hash_destroy(return_value->value.ht);
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
		if (count != -1) count--;
	}

	/* see if we encountered an error */
	if (err && err != REG_NOMATCH) {
		php3_error(E_WARNING, "unexpected regex error (%d)", err);
		regfree(&re);
		_php3_hash_destroy(return_value->value.ht);
		efree(return_value->value.ht);
		RETURN_FALSE;
	}

	/* otherwise we just have one last element to add to the array */
	size = endp - strp;
	
	add_next_index_stringl(return_value, strp, size, 1);

	regfree(&re);

	return;
}
/* }}} */

/* {{{ proto string sql_regcase(string string)
   Make regular expression for case insensitive match */
PHPAPI PHP_FUNCTION(sql_regcase)
{
	pval *string;
	char *tmp;
	unsigned char c;
	register int i, j;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &string)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(string);
	
	tmp = (char *) emalloc(string->value.str.len*4+1);
	
	for (i=j=0; i<string->value.str.len; i++) {
		c = (unsigned char) string->value.str.val[i];
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
 */
