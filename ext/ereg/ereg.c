/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Jim Winstead <jimw@php.net>                                 |
   |          Jaakko Hyvätti <jaakko@hyvatti.iki.fi>                      |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdio.h>
#include <ctype.h>
#include "php.h"
#include "ext/standard/php_string.h"
#include "php_ereg.h"
#include "ext/standard/info.h"

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_ereg, 0, 0, 2)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(1, registers) /* ARRAY_INFO(1, registers, 1) */
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_ereg_replace, 0)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, replacement)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_split, 0, 0, 2)
	ZEND_ARG_INFO(0, pattern)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, limit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_sql_regcase, 0)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ Function table */
const zend_function_entry ereg_functions[] = {
	PHP_DEP_FE(ereg,			arginfo_ereg)
	PHP_DEP_FE(ereg_replace,	arginfo_ereg_replace)
	PHP_DEP_FE(eregi,			arginfo_ereg)
	PHP_DEP_FE(eregi_replace,	arginfo_ereg_replace)
	PHP_DEP_FE(split,			arginfo_split)
	PHP_DEP_FE(spliti,			arginfo_split)
	PHP_DEP_FE(sql_regcase,		arginfo_sql_regcase)
	PHP_FE_END
};
/* }}} */

/* {{{ reg_cache */
typedef struct {
	regex_t preg;
	int cflags;
	unsigned long lastuse;
} reg_cache;
static int reg_magic = 0;
#define EREG_CACHE_SIZE 4096
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(ereg)
static PHP_GINIT_FUNCTION(ereg);
static PHP_GSHUTDOWN_FUNCTION(ereg);

/* {{{ Module entry */
zend_module_entry ereg_module_entry = {
	STANDARD_MODULE_HEADER,
	"ereg",
	ereg_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(ereg),
	NO_VERSION_YET,
	PHP_MODULE_GLOBALS(ereg),
	PHP_GINIT(ereg),
	PHP_GSHUTDOWN(ereg),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

/* {{{ COMPILE_DL_EREG */
#ifdef COMPILE_DL_EREG
ZEND_GET_MODULE(ereg)
#endif
/* }}} */

/* {{{ ereg_lru_cmp */
static int ereg_lru_cmp(const void *a, const void *b TSRMLS_DC)
{
	Bucket *f = *((Bucket **) a);
	Bucket *s = *((Bucket **) b);

	if (((reg_cache *)f->pData)->lastuse <
				((reg_cache *)s->pData)->lastuse) {
		return -1;
	} else if (((reg_cache *)f->pData)->lastuse ==
				((reg_cache *)s->pData)->lastuse) {
		return 0;
	} else {
		return 1;
	}
}
/* }}} */

/* {{{ static ereg_clean_cache */
static int ereg_clean_cache(void *data, void *arg TSRMLS_DC)
{
	int *num_clean = (int *)arg;

	if (*num_clean > 0) {
		(*num_clean)--;
		return ZEND_HASH_APPLY_REMOVE;
	} else {
		return ZEND_HASH_APPLY_STOP;
	}
}
/* }}} */

/* {{{ _php_regcomp
 */
static int _php_regcomp(regex_t *preg, const char *pattern, int cflags TSRMLS_DC)
{
	int r = 0;
	int patlen = strlen(pattern);
	reg_cache *rc = NULL;

	if (zend_hash_num_elements(&EREG(ht_rc)) >= EREG_CACHE_SIZE) {
		/* easier than dealing with overflow as it happens */
		if (EREG(lru_counter) >= (1 << 31) || zend_hash_sort(&EREG(ht_rc), zend_qsort, ereg_lru_cmp, 0 TSRMLS_CC) == FAILURE) {
			zend_hash_clean(&EREG(ht_rc));
			EREG(lru_counter) = 0;
		} else {
			int num_clean = EREG_CACHE_SIZE / 4;
			zend_hash_apply_with_argument(&EREG(ht_rc), ereg_clean_cache, &num_clean TSRMLS_CC);
		}
	}

	if(zend_hash_find(&EREG(ht_rc), (char *) pattern, patlen+1, (void **) &rc) == SUCCESS
	   && rc->cflags == cflags) {
#ifdef HAVE_REGEX_T_RE_MAGIC
		/*
		 * We use a saved magic number to see whether cache is corrupted, and if it
		 * is, we flush it and compile the pattern from scratch.
		 */
		if (rc->preg.re_magic != reg_magic) {
			zend_hash_clean(&EREG(ht_rc));
			EREG(lru_counter) = 0;
		} else {
			memcpy(preg, &rc->preg, sizeof(*preg));
			return r;
		}
	}

	r = regcomp(preg, pattern, cflags);
	if(!r) {
		reg_cache rcp;

		rcp.cflags = cflags;
		rcp.lastuse = ++(EREG(lru_counter));
		memcpy(&rcp.preg, preg, sizeof(*preg));
		/*
		 * Since we don't have access to the actual MAGIC1 definition in the private
		 * header file, we save the magic value immediately after compilation. Hopefully,
		 * it's good.
		 */
		if (!reg_magic) reg_magic = preg->re_magic;
		zend_hash_update(&EREG(ht_rc), (char *) pattern, patlen+1,
						 (void *) &rcp, sizeof(rcp), NULL);
	}
#else
		memcpy(preg, &rc->preg, sizeof(*preg));
	} else {
		r = regcomp(preg, pattern, cflags);
		if(!r) {
			reg_cache rcp;

			rcp.cflags = cflags;
			rcp.lastuse = ++(EREG(lru_counter));
			memcpy(&rcp.preg, preg, sizeof(*preg));
			zend_hash_update(&EREG(ht_rc), (char *) pattern, patlen+1,
							 (void *) &rcp, sizeof(rcp), NULL);
		}
	}
#endif
	return r;
}
/* }}} */

static void _free_ereg_cache(reg_cache *rc)
{
	regfree(&rc->preg);
}

#undef regfree
#define regfree(a);
#undef regcomp
#define regcomp(a, b, c) _php_regcomp(a, b, c TSRMLS_CC)

/* {{{ PHP_GINIT_FUNCTION
 */
static PHP_GINIT_FUNCTION(ereg)
{
	zend_hash_init(&ereg_globals->ht_rc, 0, NULL, (void (*)(void *)) _free_ereg_cache, 1);
	ereg_globals->lru_counter = 0;
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION
 */
static PHP_GSHUTDOWN_FUNCTION(ereg)
{
	zend_hash_destroy(&ereg_globals->ht_rc);
}
/* }}} */

PHP_MINFO_FUNCTION(ereg)
{
	php_info_print_table_start();
#if HSREGEX
	php_info_print_table_row(2, "Regex Library", "Bundled library enabled");
#else
	php_info_print_table_row(2, "Regex Library", "System library enabled");
#endif
	php_info_print_table_end();
}


/* {{{ php_ereg_eprint
 * php_ereg_eprint - convert error number to name
 */
static void php_ereg_eprint(int err, regex_t *re TSRMLS_DC) {
	char *buf = NULL, *message = NULL;
	size_t len;
	size_t buf_len;

#ifdef REG_ITOA
	/* get the length of the message */
	buf_len = regerror(REG_ITOA | err, re, NULL, 0);
	if (buf_len) {
		buf = (char *)safe_emalloc(buf_len, sizeof(char), 0);
		if (!buf) return; /* fail silently */
		/* finally, get the error message */
		regerror(REG_ITOA | err, re, buf, buf_len);
	}
#else
	buf_len = 0;
#endif
	len = regerror(err, re, NULL, 0);
	if (len) {
		message = (char *)safe_emalloc((buf_len + len + 2), sizeof(char), 0);
		if (!message) {
			return; /* fail silently */
		}
		if (buf_len) {
			snprintf(message, buf_len, "%s: ", buf);
			buf_len += 1; /* so pointer math below works */
		}
		/* drop the message into place */
		regerror(err, re, message + buf_len, len);

		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", message);
	}

	STR_FREE(buf);
	STR_FREE(message);
}
/* }}} */

/* {{{ php_ereg
 */
static void php_ereg(INTERNAL_FUNCTION_PARAMETERS, int icase)
{
	zval **regex,			/* Regular expression */
		**array = NULL;		/* Optional register array */
	char *findin;		/* String to apply expression to */
	int findin_len;
	regex_t re;
	regmatch_t *subs;
	int err, match_len, string_len;
	uint i;
	int copts = 0;
	off_t start, end;
	char *buf = NULL;
	char *string = NULL;
	int   argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "Zs|Z", &regex, &findin, &findin_len, &array) == FAILURE) {
		return;
	}

	if (icase) {
		copts |= REG_ICASE;
	}

	if (argc == 2) {
		copts |= REG_NOSUB;
	}

	/* compile the regular expression from the supplied regex */
	if (Z_TYPE_PP(regex) == IS_STRING) {
		err = regcomp(&re, Z_STRVAL_PP(regex), REG_EXTENDED | copts);
	} else {
		/* we convert numbers to integers and treat them as a string */
		if (Z_TYPE_PP(regex) == IS_DOUBLE) {
			convert_to_long_ex(regex);	/* get rid of decimal places */
		}
		convert_to_string_ex(regex);
		/* don't bother doing an extended regex with just a number */
		err = regcomp(&re, Z_STRVAL_PP(regex), copts);
	}

	if (err) {
		php_ereg_eprint(err, &re TSRMLS_CC);
		RETURN_FALSE;
	}

	/* make a copy of the string we're looking in */
	string = estrndup(findin, findin_len);

	/* allocate storage for (sub-)expression-matches */
	subs = (regmatch_t *)ecalloc(sizeof(regmatch_t),re.re_nsub+1);

	/* actually execute the regular expression */
	err = regexec(&re, string, re.re_nsub+1, subs, 0);
	if (err && err != REG_NOMATCH) {
		php_ereg_eprint(err, &re TSRMLS_CC);
		regfree(&re);
		efree(subs);
		RETURN_FALSE;
	}
	match_len = 1;

	if (array && err != REG_NOMATCH) {
		match_len = (int) (subs[0].rm_eo - subs[0].rm_so);
		string_len = findin_len + 1;

		buf = emalloc(string_len);

		zval_dtor(*array);	/* start with clean array */
		array_init(*array);

		for (i = 0; i <= re.re_nsub; i++) {
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

	efree(subs);
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

/* {{{ php_ereg_replace
 * this is the meat and potatoes of regex replacement! */
PHP_EREG_API char *php_ereg_replace(const char *pattern, const char *replace, const char *string, int icase, int extended TSRMLS_DC)
{
	regex_t re;
	regmatch_t *subs;

	char *buf,	/* buf is where we build the replaced string */
		 *nbuf,	/* nbuf is used when we grow the buffer */
		 *walkbuf; /* used to walk buf when replacing backrefs */
	const char *walk; /* used to walk replacement string for backrefs */
	size_t buf_len, new_l;
	int pos, tmp, string_len;
	int err, copts = 0;

	string_len = strlen(string);

	if (icase) {
		copts = REG_ICASE;
	}
	if (extended) {
		copts |= REG_EXTENDED;
	}

	err = regcomp(&re, pattern, copts);
	if (err) {
		php_ereg_eprint(err, &re TSRMLS_CC);
		return ((char *) -1);
	}


	/* allocate storage for (sub-)expression-matches */
	subs = (regmatch_t *)ecalloc(sizeof(regmatch_t),re.re_nsub+1);

	/* start with a buffer that is twice the size of the stringo
	   we're doing replacements in */
	buf = safe_emalloc(string_len, 2, 1);
	buf_len = 2 * string_len + 1;

	err = pos = 0;
	buf[0] = '\0';
	while (!err) {
		err = regexec(&re, &string[pos], re.re_nsub+1, subs, (pos ? REG_NOTBOL : 0));

		if (err && err != REG_NOMATCH) {
			php_ereg_eprint(err, &re TSRMLS_CC);
			efree(subs);
			efree(buf);
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
			while (*walk) {
				if ('\\' == *walk && isdigit((unsigned char)walk[1]) && ((unsigned char)walk[1]) - '0' <= (int)re.re_nsub) {
					if (subs[walk[1] - '0'].rm_so > -1 && subs[walk[1] - '0'].rm_eo > -1) {
						new_l += subs[walk[1] - '0'].rm_eo - subs[walk[1] - '0'].rm_so;
					}
					walk += 2;
				} else {
					new_l++;
					walk++;
				}
			}
			if (new_l + 1 > buf_len) {
				nbuf = safe_emalloc(new_l + 1, 2, buf_len);
				buf_len = 1 + buf_len + 2 * new_l;
				strncpy(nbuf, buf, buf_len - 1);
				nbuf[buf_len - 1] = '\0';
				efree(buf);
				buf = nbuf;
			}
			tmp = strlen(buf);
			/* copy the part of the string before the match */
			strncat(buf, &string[pos], subs[0].rm_so);

			/* copy replacement and backrefs */
			walkbuf = &buf[tmp + subs[0].rm_so];
			walk = replace;
			while (*walk) {
				if ('\\' == *walk && isdigit((unsigned char)walk[1]) && (unsigned char)walk[1] - '0' <= (int)re.re_nsub) {
					if (subs[walk[1] - '0'].rm_so > -1 && subs[walk[1] - '0'].rm_eo > -1
						/* this next case shouldn't happen. it does. */
						&& subs[walk[1] - '0'].rm_so <= subs[walk[1] - '0'].rm_eo) {

						tmp = subs[walk[1] - '0'].rm_eo - subs[walk[1] - '0'].rm_so;
						memcpy (walkbuf, &string[pos + subs[walk[1] - '0'].rm_so], tmp);
						walkbuf += tmp;
					}
					walk += 2;
				} else {
					*walkbuf++ = *walk++;
				}
			}
			*walkbuf = '\0';

			/* and get ready to keep looking for replacements */
			if (subs[0].rm_so == subs[0].rm_eo) {
				if (subs[0].rm_so + pos >= string_len) {
					break;
				}
				new_l = strlen (buf) + 1;
				if (new_l + 1 > buf_len) {
					nbuf = safe_emalloc(new_l + 1, 2, buf_len);
					buf_len = 1 + buf_len + 2 * new_l;
					strncpy(nbuf, buf, buf_len-1);
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
				nbuf = safe_emalloc(new_l, 1, 1);
				strncpy(nbuf, buf, buf_len-1);
				efree(buf);
				buf = nbuf;
			}
			/* stick that last bit of string on our output */
			strlcat(buf, &string[pos], buf_len);
		}
	}

	/* don't want to leak memory .. */
	efree(subs);
	regfree(&re);

	/* whew. */
	return (buf);
}
/* }}} */

/* {{{ php_do_ereg_replace
 */
static void php_do_ereg_replace(INTERNAL_FUNCTION_PARAMETERS, int icase)
{
	zval **arg_pattern,
		**arg_replace;
	char *pattern, *arg_string;
	char *string;
	char *replace;
	char *ret;
	int arg_string_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZs", &arg_pattern, &arg_replace, &arg_string, &arg_string_len) == FAILURE) {
		return;
	}

	if (Z_TYPE_PP(arg_pattern) == IS_STRING) {
		if (Z_STRVAL_PP(arg_pattern) && Z_STRLEN_PP(arg_pattern)) {
			pattern = estrndup(Z_STRVAL_PP(arg_pattern), Z_STRLEN_PP(arg_pattern));
		} else {
			pattern = STR_EMPTY_ALLOC();
		}
	} else {
		convert_to_long_ex(arg_pattern);
		pattern = emalloc(2);
		pattern[0] = (char) Z_LVAL_PP(arg_pattern);
		pattern[1] = '\0';
	}

	if (Z_TYPE_PP(arg_replace) == IS_STRING) {
		if (Z_STRVAL_PP(arg_replace) && Z_STRLEN_PP(arg_replace)) {
			replace = estrndup(Z_STRVAL_PP(arg_replace), Z_STRLEN_PP(arg_replace));
		} else {
			replace = STR_EMPTY_ALLOC();
		}
	} else {
		convert_to_long_ex(arg_replace);
		replace = emalloc(2);
		replace[0] = (char) Z_LVAL_PP(arg_replace);
		replace[1] = '\0';
	}

	if (arg_string && arg_string_len) {
		string = estrndup(arg_string, arg_string_len);
	} else {
		string = STR_EMPTY_ALLOC();
	}

	/* do the actual work */
	ret = php_ereg_replace(pattern, replace, string, icase, 1 TSRMLS_CC);
	if (ret == (char *) -1) {
		RETVAL_FALSE;
	} else {
		RETVAL_STRINGL_CHECK(ret, strlen(ret), 1);
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
	php_do_ereg_replace(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string eregi_replace(string pattern, string replacement, string string)
   Case insensitive replace regular expression */
PHP_FUNCTION(eregi_replace)
{
	php_do_ereg_replace(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ php_split
 */
static void php_split(INTERNAL_FUNCTION_PARAMETERS, int icase)
{
	long count = -1;
	regex_t re;
	regmatch_t subs[1];
	char *spliton, *str, *strp, *endp;
	int spliton_len, str_len;
	int err, size, copts = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", &spliton, &spliton_len, &str, &str_len, &count) == FAILURE) {
		return;
	}

	if (icase) {
		copts = REG_ICASE;
	}

	strp = str;
	endp = strp + str_len;

	err = regcomp(&re, spliton, REG_EXTENDED | copts);
	if (err) {
		php_ereg_eprint(err, &re TSRMLS_CC);
		RETURN_FALSE;
	}

	array_init(return_value);

	/* churn through str, generating array entries as we go */
	while ((count == -1 || count > 1) && !(err = regexec(&re, strp, 1, subs, 0))) {
		if (subs[0].rm_so == 0 && subs[0].rm_eo) {
			/* match is at start of string, return empty string */
			add_next_index_stringl(return_value, "", 0, 1);
			/* skip ahead the length of the regex match */
			strp += subs[0].rm_eo;
		} else if (subs[0].rm_so == 0 && subs[0].rm_eo == 0) {
			/* No more matches */
			regfree(&re);

			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Regular Expression");

			zend_hash_destroy(Z_ARRVAL_P(return_value));
			efree(Z_ARRVAL_P(return_value));
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
		php_ereg_eprint(err, &re TSRMLS_CC);
		regfree(&re);
		zend_hash_destroy(Z_ARRVAL_P(return_value));
		efree(Z_ARRVAL_P(return_value));
		RETURN_FALSE;
	}

	/* otherwise we just have one last element to add to the array */
	size = endp - strp;

	add_next_index_stringl(return_value, strp, size, 1);

	regfree(&re);
}
/* }}} */

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
PHP_EREG_API PHP_FUNCTION(sql_regcase)
{
	char *string, *tmp;
	int string_len;
	unsigned char c;
	register int i, j;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &string, &string_len) == FAILURE) {
		return;
	}

	tmp = safe_emalloc(string_len, 4, 1);

	for (i = j = 0; i < string_len; i++) {
		c = (unsigned char) string[i];
		if ( j >= INT_MAX - 1 || (isalpha(c) && j >= INT_MAX - 4)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "String too long, max length is %d", INT_MAX);
			efree(tmp);
			RETURN_FALSE;
		}
		if (isalpha(c)) {
			tmp[j++] = '[';
			tmp[j++] = toupper(c);
			tmp[j++] = tolower(c);
			tmp[j++] = ']';
		} else {
			tmp[j++] = c;
		}
	}
	tmp[j] = 0;

	RETVAL_STRINGL(tmp, j, 1);
	efree(tmp);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
