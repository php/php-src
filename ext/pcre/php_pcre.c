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
   | Author: Andrei Zmievski <andrei@php.net>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_pcre.h"
#include "ext/standard/info.h"
#include "ext/standard/basic_functions.h"
#include "zend_smart_str.h"

#if HAVE_PCRE || HAVE_BUNDLED_PCRE

#include "ext/standard/php_string.h"

#define PREG_PATTERN_ORDER			1
#define PREG_SET_ORDER				2
#define PREG_OFFSET_CAPTURE			(1<<8)

#define	PREG_SPLIT_NO_EMPTY			(1<<0)
#define PREG_SPLIT_DELIM_CAPTURE	(1<<1)
#define PREG_SPLIT_OFFSET_CAPTURE	(1<<2)

#define PREG_REPLACE_EVAL			(1<<0)

#define PREG_GREP_INVERT			(1<<0)

#define PCRE_CACHE_SIZE 4096

/* not fully functional workaround for libpcre < 8.0, see bug #70232 */
#ifndef PCRE_NOTEMPTY_ATSTART
# define PCRE_NOTEMPTY_ATSTART PCRE_NOTEMPTY
#endif

enum {
	PHP_PCRE_NO_ERROR = 0,
	PHP_PCRE_INTERNAL_ERROR,
	PHP_PCRE_BACKTRACK_LIMIT_ERROR,
	PHP_PCRE_RECURSION_LIMIT_ERROR,
	PHP_PCRE_BAD_UTF8_ERROR,
	PHP_PCRE_BAD_UTF8_OFFSET_ERROR,
	PHP_PCRE_JIT_STACKLIMIT_ERROR
};


PHPAPI ZEND_DECLARE_MODULE_GLOBALS(pcre)

#ifdef PCRE_STUDY_JIT_COMPILE
#define PCRE_JIT_STACK_MIN_SIZE (32 * 1024)
#define PCRE_JIT_STACK_MAX_SIZE (64 * 1024)
ZEND_TLS pcre_jit_stack *jit_stack = NULL;
#endif

static void pcre_handle_exec_error(int pcre_code) /* {{{ */
{
	int preg_code = 0;

	switch (pcre_code) {
		case PCRE_ERROR_MATCHLIMIT:
			preg_code = PHP_PCRE_BACKTRACK_LIMIT_ERROR;
			break;

		case PCRE_ERROR_RECURSIONLIMIT:
			preg_code = PHP_PCRE_RECURSION_LIMIT_ERROR;
			break;

		case PCRE_ERROR_BADUTF8:
			preg_code = PHP_PCRE_BAD_UTF8_ERROR;
			break;

		case PCRE_ERROR_BADUTF8_OFFSET:
			preg_code = PHP_PCRE_BAD_UTF8_OFFSET_ERROR;
			break;
		
#ifdef PCRE_STUDY_JIT_COMPILE
		case PCRE_ERROR_JIT_STACKLIMIT:
			preg_code = PHP_PCRE_JIT_STACKLIMIT_ERROR;
			break;
#endif

		default:
			preg_code = PHP_PCRE_INTERNAL_ERROR;
			break;
	}

	PCRE_G(error_code) = preg_code;
}
/* }}} */

static void php_free_pcre_cache(zval *data) /* {{{ */
{
	pcre_cache_entry *pce = (pcre_cache_entry *) Z_PTR_P(data);
	if (!pce) return;
	pcre_free(pce->re);
	if (pce->extra) {
		pcre_free_study(pce->extra);
	}
#if HAVE_SETLOCALE
	if ((void*)pce->tables) pefree((void*)pce->tables, 1);
	if (pce->locale) {
		zend_string_release(pce->locale);
	}
#endif
	pefree(pce, 1);
}
/* }}} */

static PHP_GINIT_FUNCTION(pcre) /* {{{ */
{
	zend_hash_init(&pcre_globals->pcre_cache, 0, NULL, php_free_pcre_cache, 1);
	pcre_globals->backtrack_limit = 0;
	pcre_globals->recursion_limit = 0;
	pcre_globals->error_code      = PHP_PCRE_NO_ERROR;
}
/* }}} */

static PHP_GSHUTDOWN_FUNCTION(pcre) /* {{{ */
{
	zend_hash_destroy(&pcre_globals->pcre_cache);

#ifdef PCRE_STUDY_JIT_COMPILE
	/* Stack may only be destroyed when no cached patterns
	 	possibly associated with it do exist. */
	if (jit_stack) {
		pcre_jit_stack_free(jit_stack);
		jit_stack = NULL;
	}
#endif

}
/* }}} */

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("pcre.backtrack_limit", "1000000", PHP_INI_ALL, OnUpdateLong, backtrack_limit, zend_pcre_globals, pcre_globals)
	STD_PHP_INI_ENTRY("pcre.recursion_limit", "100000",  PHP_INI_ALL, OnUpdateLong, recursion_limit, zend_pcre_globals, pcre_globals)
#ifdef PCRE_STUDY_JIT_COMPILE
	STD_PHP_INI_ENTRY("pcre.jit",             "1",       PHP_INI_ALL, OnUpdateBool, jit,             zend_pcre_globals, pcre_globals)
#endif
PHP_INI_END()


/* {{{ PHP_MINFO_FUNCTION(pcre) */
static PHP_MINFO_FUNCTION(pcre)
{
	int jit_yes = 0;

	php_info_print_table_start();
	php_info_print_table_row(2, "PCRE (Perl Compatible Regular Expressions) Support", "enabled" );
	php_info_print_table_row(2, "PCRE Library Version", pcre_version() );

	if (!pcre_config(PCRE_CONFIG_JIT, &jit_yes)) {
		php_info_print_table_row(2, "PCRE JIT Support", jit_yes ? "enabled" : "disabled");
	} else {
		php_info_print_table_row(2, "PCRE JIT Support", "unknown" );
	}

	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(pcre) */
static PHP_MINIT_FUNCTION(pcre)
{
	REGISTER_INI_ENTRIES();

	REGISTER_LONG_CONSTANT("PREG_PATTERN_ORDER", PREG_PATTERN_ORDER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_SET_ORDER", PREG_SET_ORDER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_OFFSET_CAPTURE", PREG_OFFSET_CAPTURE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_SPLIT_NO_EMPTY", PREG_SPLIT_NO_EMPTY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_SPLIT_DELIM_CAPTURE", PREG_SPLIT_DELIM_CAPTURE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_SPLIT_OFFSET_CAPTURE", PREG_SPLIT_OFFSET_CAPTURE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_GREP_INVERT", PREG_GREP_INVERT, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("PREG_NO_ERROR", PHP_PCRE_NO_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_INTERNAL_ERROR", PHP_PCRE_INTERNAL_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_BACKTRACK_LIMIT_ERROR", PHP_PCRE_BACKTRACK_LIMIT_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_RECURSION_LIMIT_ERROR", PHP_PCRE_RECURSION_LIMIT_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_BAD_UTF8_ERROR", PHP_PCRE_BAD_UTF8_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_BAD_UTF8_OFFSET_ERROR", PHP_PCRE_BAD_UTF8_OFFSET_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_JIT_STACKLIMIT_ERROR", PHP_PCRE_JIT_STACKLIMIT_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("PCRE_VERSION", (char *)pcre_version(), CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION(pcre) */
static PHP_MSHUTDOWN_FUNCTION(pcre)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

#ifdef PCRE_STUDY_JIT_COMPILE
/* {{{ PHP_RINIT_FUNCTION(pcre) */
static PHP_RINIT_FUNCTION(pcre)
{
	if (PCRE_G(jit) && jit_stack == NULL) {
		jit_stack = pcre_jit_stack_alloc(PCRE_JIT_STACK_MIN_SIZE,PCRE_JIT_STACK_MAX_SIZE);
	}

	return SUCCESS;
}
/* }}} */
#endif

/* {{{ static pcre_clean_cache */
static int pcre_clean_cache(zval *data, void *arg)
{
	pcre_cache_entry *pce = (pcre_cache_entry *) Z_PTR_P(data);
	int *num_clean = (int *)arg;

	if (*num_clean > 0 && !pce->refcount) {
		(*num_clean)--;
		return ZEND_HASH_APPLY_REMOVE;
	} else {
		return ZEND_HASH_APPLY_KEEP;
	}
}
/* }}} */

/* {{{ static make_subpats_table */
static char **make_subpats_table(int num_subpats, pcre_cache_entry *pce)
{
	pcre_extra *extra = pce->extra;
	int name_cnt = pce->name_count, name_size, ni = 0;
	int rc;
	char *name_table;
	unsigned short name_idx;
	char **subpat_names;
	int rc1, rc2;

	rc1 = pcre_fullinfo(pce->re, extra, PCRE_INFO_NAMETABLE, &name_table);
	rc2 = pcre_fullinfo(pce->re, extra, PCRE_INFO_NAMEENTRYSIZE, &name_size);
	rc = rc2 ? rc2 : rc1;
	if (rc < 0) {
		php_error_docref(NULL, E_WARNING, "Internal pcre_fullinfo() error %d", rc);
		return NULL;
	}

	subpat_names = (char **)ecalloc(num_subpats, sizeof(char *));
	while (ni++ < name_cnt) {
		name_idx = 0xff * (unsigned char)name_table[0] + (unsigned char)name_table[1];
		subpat_names[name_idx] = name_table + 2;
		if (is_numeric_string(subpat_names[name_idx], strlen(subpat_names[name_idx]), NULL, NULL, 0) > 0) {
			php_error_docref(NULL, E_WARNING, "Numeric named subpatterns are not allowed");
			efree(subpat_names);
			return NULL;
		}
		name_table += name_size;
	}
	return subpat_names;
}
/* }}} */

/* {{{ static calculate_unit_length */
/* Calculates the byte length of the next character. Assumes valid UTF-8 for PCRE_UTF8. */
static zend_always_inline int calculate_unit_length(pcre_cache_entry *pce, char *start)
{
	int unit_len;

	if (pce->compile_options & PCRE_UTF8) {
		char *end = start;

		/* skip continuation bytes */
		while ((*++end & 0xC0) == 0x80);
		unit_len = end - start;
	} else {
		unit_len = 1;
	}
	return unit_len;
}
/* }}} */

/* {{{ pcre_get_compiled_regex_cache
 */
PHPAPI pcre_cache_entry* pcre_get_compiled_regex_cache(zend_string *regex)
{
	pcre				*re = NULL;
	pcre_extra			*extra;
	int					 coptions = 0;
	int					 soptions = 0;
	const char			*error;
	int					 erroffset;
	char				 delimiter;
	char				 start_delimiter;
	char				 end_delimiter;
	char				*p, *pp;
	char				*pattern;
	int					 do_study = 0;
	int					 poptions = 0;
	unsigned const char *tables = NULL;
	pcre_cache_entry	*pce;
	pcre_cache_entry	 new_entry;
	int					 rc;

	/* Try to lookup the cached regex entry, and if successful, just pass
	   back the compiled pattern, otherwise go on and compile it. */
	pce = zend_hash_find_ptr(&PCRE_G(pcre_cache), regex);
	if (pce) {
#if HAVE_SETLOCALE
		if (pce->locale == BG(locale_string) ||
		    (pce->locale && BG(locale_string) &&
		     ZSTR_LEN(pce->locale) == ZSTR_LEN(BG(locale_string)) &&
		     !memcmp(ZSTR_VAL(pce->locale), ZSTR_VAL(BG(locale_string)), ZSTR_LEN(pce->locale))) ||
		    (!pce->locale &&
		     ZSTR_LEN(BG(locale_string)) == 1 &&
		     ZSTR_VAL(BG(locale_string))[0] == 'C') ||
		    (!BG(locale_string) &&
		     ZSTR_LEN(pce->locale) == 1 &&
		     ZSTR_VAL(pce->locale)[0] == 'C')) {
			return pce;
		}
#else
		return pce;
#endif
	}

	p = ZSTR_VAL(regex);

	/* Parse through the leading whitespace, and display a warning if we
	   get to the end without encountering a delimiter. */
	while (isspace((int)*(unsigned char *)p)) p++;
	if (*p == 0) {
		php_error_docref(NULL, E_WARNING,
						 p < ZSTR_VAL(regex) + ZSTR_LEN(regex) ? "Null byte in regex" : "Empty regular expression");
		return NULL;
	}

	/* Get the delimiter and display a warning if it is alphanumeric
	   or a backslash. */
	delimiter = *p++;
	if (isalnum((int)*(unsigned char *)&delimiter) || delimiter == '\\') {
		php_error_docref(NULL,E_WARNING, "Delimiter must not be alphanumeric or backslash");
		return NULL;
	}

	start_delimiter = delimiter;
	if ((pp = strchr("([{< )]}> )]}>", delimiter)))
		delimiter = pp[5];
	end_delimiter = delimiter;

	pp = p;

	if (start_delimiter == end_delimiter) {
		/* We need to iterate through the pattern, searching for the ending delimiter,
		   but skipping the backslashed delimiters.  If the ending delimiter is not
		   found, display a warning. */
		while (*pp != 0) {
			if (*pp == '\\' && pp[1] != 0) pp++;
			else if (*pp == delimiter)
				break;
			pp++;
		}
	} else {
		/* We iterate through the pattern, searching for the matching ending
		 * delimiter. For each matching starting delimiter, we increment nesting
		 * level, and decrement it for each matching ending delimiter. If we
		 * reach the end of the pattern without matching, display a warning.
		 */
		int brackets = 1; 	/* brackets nesting level */
		while (*pp != 0) {
			if (*pp == '\\' && pp[1] != 0) pp++;
			else if (*pp == end_delimiter && --brackets <= 0)
				break;
			else if (*pp == start_delimiter)
				brackets++;
			pp++;
		}
	}

	if (*pp == 0) {
		if (pp < ZSTR_VAL(regex) + ZSTR_LEN(regex)) {
			php_error_docref(NULL,E_WARNING, "Null byte in regex");
		} else if (start_delimiter == end_delimiter) {
			php_error_docref(NULL,E_WARNING, "No ending delimiter '%c' found", delimiter);
		} else {
			php_error_docref(NULL,E_WARNING, "No ending matching delimiter '%c' found", delimiter);
		}
		return NULL;
	}

	/* Make a copy of the actual pattern. */
	pattern = estrndup(p, pp-p);

	/* Move on to the options */
	pp++;

	/* Parse through the options, setting appropriate flags.  Display
	   a warning if we encounter an unknown modifier. */
	while (pp < ZSTR_VAL(regex) + ZSTR_LEN(regex)) {
		switch (*pp++) {
			/* Perl compatible options */
			case 'i':	coptions |= PCRE_CASELESS;		break;
			case 'm':	coptions |= PCRE_MULTILINE;		break;
			case 's':	coptions |= PCRE_DOTALL;		break;
			case 'x':	coptions |= PCRE_EXTENDED;		break;

			/* PCRE specific options */
			case 'A':	coptions |= PCRE_ANCHORED;		break;
			case 'D':	coptions |= PCRE_DOLLAR_ENDONLY;break;
			case 'S':	do_study  = 1;					break;
			case 'U':	coptions |= PCRE_UNGREEDY;		break;
			case 'X':	coptions |= PCRE_EXTRA;			break;
			case 'u':	coptions |= PCRE_UTF8;
	/* In  PCRE,  by  default, \d, \D, \s, \S, \w, and \W recognize only ASCII
       characters, even in UTF-8 mode. However, this can be changed by setting
       the PCRE_UCP option. */
#ifdef PCRE_UCP
						coptions |= PCRE_UCP;
#endif
				break;

			/* Custom preg options */
			case 'e':	poptions |= PREG_REPLACE_EVAL;	break;

			case ' ':
			case '\n':
				break;

			default:
				if (pp[-1]) {
					php_error_docref(NULL,E_WARNING, "Unknown modifier '%c'", pp[-1]);
				} else {
					php_error_docref(NULL,E_WARNING, "Null byte in regex");
				}
				efree(pattern);
				return NULL;
		}
	}

#if HAVE_SETLOCALE
	if (BG(locale_string) &&
	    (ZSTR_LEN(BG(locale_string)) != 1 || ZSTR_VAL(BG(locale_string))[0] != 'C')) {
		tables = pcre_maketables();
	}
#endif

	/* Compile pattern and display a warning if compilation failed. */
	re = pcre_compile(pattern,
					  coptions,
					  &error,
					  &erroffset,
					  tables);

	if (re == NULL) {
		php_error_docref(NULL,E_WARNING, "Compilation failed: %s at offset %d", error, erroffset);
		efree(pattern);
		if (tables) {
			pefree((void*)tables, 1);
		}
		return NULL;
	}

#ifdef PCRE_STUDY_JIT_COMPILE
	if (PCRE_G(jit)) {
		/* Enable PCRE JIT compiler */
		do_study = 1;
		soptions |= PCRE_STUDY_JIT_COMPILE;
	}
#endif

	/* If study option was specified, study the pattern and
	   store the result in extra for passing to pcre_exec. */
	if (do_study) {
		extra = pcre_study(re, soptions, &error);
		if (extra) {
			extra->flags |= PCRE_EXTRA_MATCH_LIMIT | PCRE_EXTRA_MATCH_LIMIT_RECURSION;
			extra->match_limit = (unsigned long)PCRE_G(backtrack_limit);
			extra->match_limit_recursion = (unsigned long)PCRE_G(recursion_limit);
#ifdef PCRE_STUDY_JIT_COMPILE
			if (PCRE_G(jit) && jit_stack) {
				pcre_assign_jit_stack(extra, NULL, jit_stack);
			}
#endif
		}
		if (error != NULL) {
			php_error_docref(NULL, E_WARNING, "Error while studying pattern");
		}
	} else {
		extra = NULL;
	}

	efree(pattern);

	/*
	 * If we reached cache limit, clean out the items from the head of the list;
	 * these are supposedly the oldest ones (but not necessarily the least used
	 * ones).
	 */
	if (zend_hash_num_elements(&PCRE_G(pcre_cache)) == PCRE_CACHE_SIZE) {
		int num_clean = PCRE_CACHE_SIZE / 8;
		zend_hash_apply_with_argument(&PCRE_G(pcre_cache), pcre_clean_cache, &num_clean);
	}

	/* Store the compiled pattern and extra info in the cache. */
	new_entry.re = re;
	new_entry.extra = extra;
	new_entry.preg_options = poptions;
	new_entry.compile_options = coptions;
#if HAVE_SETLOCALE
	new_entry.locale = BG(locale_string) ?
		((GC_FLAGS(BG(locale_string)) & IS_STR_PERSISTENT) ?
			zend_string_copy(BG(locale_string)) :
			zend_string_init(ZSTR_VAL(BG(locale_string)), ZSTR_LEN(BG(locale_string)), 1)) :
		NULL;
	new_entry.tables = tables;
#endif
	new_entry.refcount = 0;

	rc = pcre_fullinfo(re, extra, PCRE_INFO_CAPTURECOUNT, &new_entry.capture_count);
	if (rc < 0) {
		php_error_docref(NULL, E_WARNING, "Internal pcre_fullinfo() error %d", rc);
		return NULL;
	}

	rc = pcre_fullinfo(re, extra, PCRE_INFO_NAMECOUNT, &new_entry.name_count);
	if (rc < 0) {
		php_error_docref(NULL, E_WARNING, "Internal pcre_fullinfo() error %d", rc);
		return NULL;
	}

	/*
	 * Interned strings are not duplicated when stored in HashTable,
	 * but all the interned strings created during HTTP request are removed
	 * at end of request. However PCRE_G(pcre_cache) must be consistent
	 * on the next request as well. So we disable usage of interned strings
	 * as hash keys especually for this table.
	 * See bug #63180
	 */
	if (!ZSTR_IS_INTERNED(regex) || !(GC_FLAGS(regex) & IS_STR_PERMANENT)) {
		zend_string *str = zend_string_init(ZSTR_VAL(regex), ZSTR_LEN(regex), 1);
		GC_REFCOUNT(str) = 0; /* will be incremented by zend_hash_update_mem() */
		ZSTR_H(str) = ZSTR_H(regex);
		regex = str;
	}

	pce = zend_hash_update_mem(&PCRE_G(pcre_cache), regex, &new_entry, sizeof(pcre_cache_entry));

	return pce;
}
/* }}} */

/* {{{ pcre_get_compiled_regex
 */
PHPAPI pcre* pcre_get_compiled_regex(zend_string *regex, pcre_extra **extra, int *preg_options)
{
	pcre_cache_entry * pce = pcre_get_compiled_regex_cache(regex);

	if (extra) {
		*extra = pce ? pce->extra : NULL;
	}
	if (preg_options) {
		*preg_options = pce ? pce->preg_options : 0;
	}

	return pce ? pce->re : NULL;
}
/* }}} */

/* {{{ pcre_get_compiled_regex_ex
 */
PHPAPI pcre* pcre_get_compiled_regex_ex(zend_string *regex, pcre_extra **extra, int *preg_options, int *compile_options)
{
	pcre_cache_entry * pce = pcre_get_compiled_regex_cache(regex);

	if (extra) {
		*extra = pce ? pce->extra : NULL;
	}
	if (preg_options) {
		*preg_options = pce ? pce->preg_options : 0;
	}
	if (compile_options) {
		*compile_options = pce ? pce->compile_options : 0;
	}

	return pce ? pce->re : NULL;
}
/* }}} */

/* {{{ add_offset_pair */
static inline void add_offset_pair(zval *result, char *str, int len, int offset, char *name)
{
	zval match_pair, tmp;

	array_init_size(&match_pair, 2);

	/* Add (match, offset) to the return value */
	ZVAL_STRINGL(&tmp, str, len);
	zend_hash_next_index_insert_new(Z_ARRVAL(match_pair), &tmp);
	ZVAL_LONG(&tmp, offset);
	zend_hash_next_index_insert_new(Z_ARRVAL(match_pair), &tmp);

	if (name) {
		Z_ADDREF(match_pair);
		zend_hash_str_update(Z_ARRVAL_P(result), name, strlen(name), &match_pair);
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(result), &match_pair);
}
/* }}} */

static void php_do_pcre_match(INTERNAL_FUNCTION_PARAMETERS, int global) /* {{{ */
{
	/* parameters */
	zend_string		 *regex;			/* Regular expression */
	zend_string		 *subject;			/* String to match against */
	pcre_cache_entry *pce;				/* Compiled regular expression */
	zval			 *subpats = NULL;	/* Array for subpatterns */
	zend_long		  flags = 0;		/* Match control flags */
	zend_long		  start_offset = 0;	/* Where the new search starts */

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|z/ll", &regex,
							  &subject, &subpats, &flags, &start_offset) == FAILURE) {
		RETURN_FALSE;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_STR(regex)
		Z_PARAM_STR(subject)
		Z_PARAM_OPTIONAL
		Z_PARAM_ZVAL_EX(subpats, 0, 1)
		Z_PARAM_LONG(flags)
		Z_PARAM_LONG(start_offset)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

	if (ZEND_SIZE_T_INT_OVFL(ZSTR_LEN(subject))) {
			php_error_docref(NULL, E_WARNING, "Subject is too long");
			RETURN_FALSE;
	}

	/* Compile regex or get it from cache. */
	if ((pce = pcre_get_compiled_regex_cache(regex)) == NULL) {
		RETURN_FALSE;
	}

	pce->refcount++;
	php_pcre_match_impl(pce, ZSTR_VAL(subject), (int)ZSTR_LEN(subject), return_value, subpats,
		global, ZEND_NUM_ARGS() >= 4, flags, start_offset);
	pce->refcount--;
}
/* }}} */

/* {{{ php_pcre_match_impl() */
PHPAPI void php_pcre_match_impl(pcre_cache_entry *pce, char *subject, int subject_len, zval *return_value,
	zval *subpats, int global, int use_flags, zend_long flags, zend_long start_offset)
{
	zval			 result_set,		/* Holds a set of subpatterns after
										   a global match */
				    *match_sets = NULL;	/* An array of sets of matches for each
										   subpattern after a global match */
	pcre_extra		*extra = pce->extra;/* Holds results of studying */
	pcre_extra		 extra_data;		/* Used locally for exec options */
	int				 exoptions = 0;		/* Execution options */
	int				 count = 0;			/* Count of matched subpatterns */
	int				*offsets;			/* Array of subpattern offsets */
	int				 num_subpats;		/* Number of captured subpatterns */
	int				 size_offsets;		/* Size of the offsets array */
	int				 matched;			/* Has anything matched */
	int				 g_notempty = 0;	/* If the match should not be empty */
	const char	   **stringlist;		/* Holds list of subpatterns */
	char 		   **subpat_names;		/* Array for named subpatterns */
	int				 i;
	int				 subpats_order;		/* Order of subpattern matches */
	int				 offset_capture;    /* Capture match offsets: yes/no */
	unsigned char   *mark = NULL;       /* Target for MARK name */
	zval            marks;      		/* Array of marks for PREG_PATTERN_ORDER */
	ALLOCA_FLAG(use_heap);

	ZVAL_UNDEF(&marks);

	/* Overwrite the passed-in value for subpatterns with an empty array. */
	if (subpats != NULL) {
		zval_dtor(subpats);
		array_init(subpats);
	}

	subpats_order = global ? PREG_PATTERN_ORDER : 0;

	if (use_flags) {
		offset_capture = flags & PREG_OFFSET_CAPTURE;

		/*
		 * subpats_order is pre-set to pattern mode so we change it only if
		 * necessary.
		 */
		if (flags & 0xff) {
			subpats_order = flags & 0xff;
		}
		if ((global && (subpats_order < PREG_PATTERN_ORDER || subpats_order > PREG_SET_ORDER)) ||
			(!global && subpats_order != 0)) {
			php_error_docref(NULL, E_WARNING, "Invalid flags specified");
			return;
		}
	} else {
		offset_capture = 0;
	}

	/* Negative offset counts from the end of the string. */
	if (start_offset < 0) {
		start_offset = subject_len + start_offset;
		if (start_offset < 0) {
			start_offset = 0;
		}
	}

	if (extra == NULL) {
		extra_data.flags = PCRE_EXTRA_MATCH_LIMIT | PCRE_EXTRA_MATCH_LIMIT_RECURSION;
		extra = &extra_data;
	}
	extra->match_limit = (unsigned long)PCRE_G(backtrack_limit);
	extra->match_limit_recursion = (unsigned long)PCRE_G(recursion_limit);
#ifdef PCRE_EXTRA_MARK
	extra->mark = &mark;
	extra->flags |= PCRE_EXTRA_MARK;
#endif

	/* Calculate the size of the offsets array, and allocate memory for it. */
	num_subpats = pce->capture_count + 1;
	size_offsets = num_subpats * 3;

	/*
	 * Build a mapping from subpattern numbers to their names. We will
	 * allocate the table only if there are any named subpatterns.
	 */
	subpat_names = NULL;
	if (pce->name_count > 0) {
		subpat_names = make_subpats_table(num_subpats, pce);
		if (!subpat_names) {
			RETURN_FALSE;
		}
	}

	if (size_offsets <= 32) {
		offsets = (int *)do_alloca(size_offsets * sizeof(int), use_heap);
	} else {
		offsets = (int *)safe_emalloc(size_offsets, sizeof(int), 0);
	}
	memset(offsets, 0, size_offsets*sizeof(int));
	/* Allocate match sets array and initialize the values. */
	if (global && subpats && subpats_order == PREG_PATTERN_ORDER) {
		match_sets = (zval *)safe_emalloc(num_subpats, sizeof(zval), 0);
		for (i=0; i<num_subpats; i++) {
			array_init(&match_sets[i]);
		}
	}

	matched = 0;
	PCRE_G(error_code) = PHP_PCRE_NO_ERROR;

	do {
		/* Execute the regular expression. */
		count = pcre_exec(pce->re, extra, subject, (int)subject_len, (int)start_offset,
						  exoptions|g_notempty, offsets, size_offsets);

		/* the string was already proved to be valid UTF-8 */
		exoptions |= PCRE_NO_UTF8_CHECK;

		/* Check for too many substrings condition. */
		if (count == 0) {
			php_error_docref(NULL, E_NOTICE, "Matched, but too many substrings");
			count = size_offsets/3;
		}

		/* If something has matched */
		if (count > 0) {
			matched++;

			/* If subpatterns array has been passed, fill it in with values. */
			if (subpats != NULL) {
				/* Try to get the list of substrings and display a warning if failed. */
				if ((offsets[1] - offsets[0] < 0) || pcre_get_substring_list(subject, offsets, count, &stringlist) < 0) {
					if (subpat_names) {
						efree(subpat_names);
					}
					if (size_offsets <= 32) {
						free_alloca(offsets, use_heap);
					} else {
						efree(offsets);
					}
					if (match_sets) efree(match_sets);
					php_error_docref(NULL, E_WARNING, "Get subpatterns list failed");
					RETURN_FALSE;
				}

				if (global) {	/* global pattern matching */
					if (subpats && subpats_order == PREG_PATTERN_ORDER) {
						/* For each subpattern, insert it into the appropriate array. */
						if (offset_capture) {
							for (i = 0; i < count; i++) {
								add_offset_pair(&match_sets[i], (char *)stringlist[i],
												offsets[(i<<1)+1] - offsets[i<<1], offsets[i<<1], NULL);
							}
						} else {
							for (i = 0; i < count; i++) {
								add_next_index_stringl(&match_sets[i], (char *)stringlist[i],
													   offsets[(i<<1)+1] - offsets[i<<1]);
							}
						}
						/* Add MARK, if available */
						if (mark) {
							if (Z_TYPE(marks) == IS_UNDEF) {
								array_init(&marks);
							}
							add_index_string(&marks, matched - 1, (char *) mark);
						}
						/*
						 * If the number of captured subpatterns on this run is
						 * less than the total possible number, pad the result
						 * arrays with empty strings.
						 */
						if (count < num_subpats) {
							for (; i < num_subpats; i++) {
								add_next_index_string(&match_sets[i], "");
							}
						}
					} else {
						/* Allocate the result set array */
						array_init_size(&result_set, count + (mark ? 1 : 0));

						/* Add all the subpatterns to it */
						if (subpat_names) {
							if (offset_capture) {
								for (i = 0; i < count; i++) {
									add_offset_pair(&result_set, (char *)stringlist[i],
													offsets[(i<<1)+1] - offsets[i<<1], offsets[i<<1], subpat_names[i]);
								}
							} else {
								for (i = 0; i < count; i++) {
									if (subpat_names[i]) {
										add_assoc_stringl(&result_set, subpat_names[i], (char *)stringlist[i],
															   offsets[(i<<1)+1] - offsets[i<<1]);
									}
									add_next_index_stringl(&result_set, (char *)stringlist[i],
														   offsets[(i<<1)+1] - offsets[i<<1]);
								}
							}
						} else {
							if (offset_capture) {
								for (i = 0; i < count; i++) {
									add_offset_pair(&result_set, (char *)stringlist[i],
													offsets[(i<<1)+1] - offsets[i<<1], offsets[i<<1], NULL);
								}
							} else {
								for (i = 0; i < count; i++) {
									add_next_index_stringl(&result_set, (char *)stringlist[i],
														   offsets[(i<<1)+1] - offsets[i<<1]);
								}
							}
						}
						/* Add MARK, if available */
						if (mark) {
							add_assoc_string_ex(&result_set, "MARK", sizeof("MARK") - 1, (char *)mark);
						}
						/* And add it to the output array */
						zend_hash_next_index_insert(Z_ARRVAL_P(subpats), &result_set);
					}
				} else {			/* single pattern matching */
					/* For each subpattern, insert it into the subpatterns array. */
					if (subpat_names) {
						if (offset_capture) {
							for (i = 0; i < count; i++) {
								add_offset_pair(subpats, (char *)stringlist[i],
												offsets[(i<<1)+1] - offsets[i<<1],
												offsets[i<<1], subpat_names[i]);
							}
						} else {
							for (i = 0; i < count; i++) {
								if (subpat_names[i]) {
									add_assoc_stringl(subpats, subpat_names[i], (char *)stringlist[i],
													  offsets[(i<<1)+1] - offsets[i<<1]);
								}
								add_next_index_stringl(subpats, (char *)stringlist[i],
													   offsets[(i<<1)+1] - offsets[i<<1]);
							}
						}
					} else {
						if (offset_capture) {
							for (i = 0; i < count; i++) {
								add_offset_pair(subpats, (char *)stringlist[i],
												offsets[(i<<1)+1] - offsets[i<<1],
												offsets[i<<1], NULL);
							}
						} else {
							for (i = 0; i < count; i++) {
								add_next_index_stringl(subpats, (char *)stringlist[i],
													   offsets[(i<<1)+1] - offsets[i<<1]);
							}
						}
					}
					/* Add MARK, if available */
					if (mark) {
						add_assoc_string_ex(subpats, "MARK", sizeof("MARK") - 1, (char *)mark);
					}
				}

				pcre_free((void *) stringlist);
			}
		} else if (count == PCRE_ERROR_NOMATCH) {
			/* If we previously set PCRE_NOTEMPTY_ATSTART after a null match,
			   this is not necessarily the end. We need to advance
			   the start offset, and continue. Fudge the offset values
			   to achieve this, unless we're already at the end of the string. */
			if (g_notempty != 0 && start_offset < subject_len) {
				int unit_len = calculate_unit_length(pce, subject + start_offset);
				
				offsets[0] = (int)start_offset;
				offsets[1] = (int)(start_offset + unit_len);
			} else
				break;
		} else {
			pcre_handle_exec_error(count);
			break;
		}

		/* If we have matched an empty string, mimic what Perl's /g options does.
		   This turns out to be rather cunning. First we set PCRE_NOTEMPTY_ATSTART and try
		   the match again at the same point. If this fails (picked up above) we
		   advance to the next character. */
		g_notempty = (offsets[1] == offsets[0])? PCRE_NOTEMPTY_ATSTART | PCRE_ANCHORED : 0;

		/* Advance to the position right after the last full match */
		start_offset = offsets[1];
	} while (global);

	/* Add the match sets to the output array and clean up */
	if (global && subpats && subpats_order == PREG_PATTERN_ORDER) {
		if (subpat_names) {
			for (i = 0; i < num_subpats; i++) {
				if (subpat_names[i]) {
					zend_hash_str_update(Z_ARRVAL_P(subpats), subpat_names[i],
									 strlen(subpat_names[i]), &match_sets[i]);
					Z_ADDREF(match_sets[i]);
				}
				zend_hash_next_index_insert(Z_ARRVAL_P(subpats), &match_sets[i]);
			}
		} else {
			for (i = 0; i < num_subpats; i++) {
				zend_hash_next_index_insert(Z_ARRVAL_P(subpats), &match_sets[i]);
			}
		}
		efree(match_sets);

		if (Z_TYPE(marks) != IS_UNDEF) {
			add_assoc_zval(subpats, "MARK", &marks);
		}
	}

	if (size_offsets <= 32) {
		free_alloca(offsets, use_heap);
	} else {
		efree(offsets);
	}
	if (subpat_names) {
		efree(subpat_names);
	}

	/* Did we encounter an error? */
	if (PCRE_G(error_code) == PHP_PCRE_NO_ERROR) {
		RETVAL_LONG(matched);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto int preg_match(string pattern, string subject [, array &subpatterns [, int flags [, int offset]]])
   Perform a Perl-style regular expression match */
static PHP_FUNCTION(preg_match)
{
	php_do_pcre_match(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int preg_match_all(string pattern, string subject [, array &subpatterns [, int flags [, int offset]]])
   Perform a Perl-style global regular expression match */
static PHP_FUNCTION(preg_match_all)
{
	php_do_pcre_match(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ preg_get_backref
 */
static int preg_get_backref(char **str, int *backref)
{
	register char in_brace = 0;
	register char *walk = *str;

	if (walk[1] == 0)
		return 0;

	if (*walk == '$' && walk[1] == '{') {
		in_brace = 1;
		walk++;
	}
	walk++;

	if (*walk >= '0' && *walk <= '9') {
		*backref = *walk - '0';
		walk++;
	} else
		return 0;

	if (*walk && *walk >= '0' && *walk <= '9') {
		*backref = *backref * 10 + *walk - '0';
		walk++;
	}

	if (in_brace) {
		if (*walk == 0 || *walk != '}')
			return 0;
		else
			walk++;
	}

	*str = walk;
	return 1;
}
/* }}} */

/* {{{ preg_do_repl_func
 */
static zend_string *preg_do_repl_func(zval *function, char *subject, int *offsets, char **subpat_names, int count, unsigned char *mark)
{
	zend_string *result_str;
	zval		 retval;			/* Function return value */
	zval	     args[1];			/* Argument to pass to function */
	int			 i;

	array_init_size(&args[0], count + (mark ? 1 : 0));
	if (subpat_names) {
		for (i = 0; i < count; i++) {
			if (subpat_names[i]) {
				add_assoc_stringl(&args[0], subpat_names[i], &subject[offsets[i<<1]] , offsets[(i<<1)+1] - offsets[i<<1]);
			}
			add_next_index_stringl(&args[0], &subject[offsets[i<<1]], offsets[(i<<1)+1] - offsets[i<<1]);
		}
	} else {
		for (i = 0; i < count; i++) {
			add_next_index_stringl(&args[0], &subject[offsets[i<<1]], offsets[(i<<1)+1] - offsets[i<<1]);
		}
	}
	if (mark) {
		add_assoc_string(&args[0], "MARK", (char *) mark);
	}

	if (call_user_function_ex(EG(function_table), NULL, function, &retval, 1, args, 0, NULL) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
		result_str = zval_get_string(&retval);
		zval_ptr_dtor(&retval);
	} else {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Unable to call custom replacement function");
		}

		result_str = zend_string_init(&subject[offsets[0]], offsets[1] - offsets[0], 0);
	}

	zval_ptr_dtor(&args[0]);

	return result_str;
}
/* }}} */

/* {{{ php_pcre_replace
 */
PHPAPI zend_string *php_pcre_replace(zend_string *regex,
							  zend_string *subject_str,
							  char *subject, int subject_len,
							  zval *replace_val, int is_callable_replace,
							  int limit, int *replace_count)
{
	pcre_cache_entry	*pce;			    /* Compiled regular expression */
	zend_string	 		*result;			/* Function result */

	/* Compile regex or get it from cache. */
	if ((pce = pcre_get_compiled_regex_cache(regex)) == NULL) {
		return NULL;
	}
	pce->refcount++;
	result = php_pcre_replace_impl(pce, subject_str, subject, subject_len, replace_val,
		is_callable_replace, limit, replace_count);
	pce->refcount--;

	return result;
}
/* }}} */

/* {{{ php_pcre_replace_impl() */
PHPAPI zend_string *php_pcre_replace_impl(pcre_cache_entry *pce, zend_string *subject_str, char *subject, int subject_len, zval *replace_val, int is_callable_replace, int limit, int *replace_count)
{
	pcre_extra		*extra = pce->extra;/* Holds results of studying */
	pcre_extra		 extra_data;		/* Used locally for exec options */
	int				 exoptions = 0;		/* Execution options */
	int				 count = 0;			/* Count of matched subpatterns */
	int				*offsets;			/* Array of subpattern offsets */
	char 			**subpat_names;		/* Array for named subpatterns */
	int				 num_subpats;		/* Number of captured subpatterns */
	int				 size_offsets;		/* Size of the offsets array */
	size_t			 new_len;			/* Length of needed storage */
	size_t			 alloc_len;			/* Actual allocated length */
	int				 match_len;			/* Length of the current match */
	int				 backref;			/* Backreference number */
	int				 start_offset;		/* Where the new search starts */
	int				 g_notempty=0;		/* If the match should not be empty */
	int				 replace_len=0;		/* Length of replacement string */
	char			*replace=NULL,		/* Replacement string */
					*walkbuf,			/* Location of current replacement in the result */
					*walk,				/* Used to walk the replacement string */
					*match,				/* The current match */
					*piece,				/* The current piece of subject */
					*replace_end=NULL,	/* End of replacement string */
					 walk_last;			/* Last walked character */
	int				 result_len; 		/* Length of result */
	unsigned char   *mark = NULL;       /* Target for MARK name */
	zend_string		*result;			/* Result of replacement */
	zend_string     *eval_result=NULL;  /* Result of custom function */

	ALLOCA_FLAG(use_heap);

	if (extra == NULL) {
		extra_data.flags = PCRE_EXTRA_MATCH_LIMIT | PCRE_EXTRA_MATCH_LIMIT_RECURSION;
		extra = &extra_data;
	}

	extra->match_limit = (unsigned long)PCRE_G(backtrack_limit);
	extra->match_limit_recursion = (unsigned long)PCRE_G(recursion_limit);

	if (UNEXPECTED(pce->preg_options & PREG_REPLACE_EVAL)) {
		php_error_docref(NULL, E_WARNING, "The /e modifier is no longer supported, use preg_replace_callback instead");
		return NULL;
	}

	if (!is_callable_replace) {
		replace = Z_STRVAL_P(replace_val);
		replace_len = (int)Z_STRLEN_P(replace_val);
		replace_end = replace + replace_len;
	}

	/* Calculate the size of the offsets array, and allocate memory for it. */
	num_subpats = pce->capture_count + 1;
	size_offsets = num_subpats * 3;
	if (size_offsets <= 32) {
		offsets = (int *)do_alloca(size_offsets * sizeof(int), use_heap);
	} else {
		offsets = (int *)safe_emalloc(size_offsets, sizeof(int), 0);
	}

	/*
	 * Build a mapping from subpattern numbers to their names. We will
	 * allocate the table only if there are any named subpatterns.
	 */
	subpat_names = NULL;
	if (UNEXPECTED(pce->name_count > 0)) {
		subpat_names = make_subpats_table(num_subpats, pce);
		if (!subpat_names) {
			return NULL;
		}
	}

	alloc_len = 0;
	result = NULL;

	/* Initialize */
	match = NULL;
	start_offset = 0;
	result_len = 0;
	PCRE_G(error_code) = PHP_PCRE_NO_ERROR;

	while (1) {
#ifdef PCRE_EXTRA_MARK
		extra->mark = &mark;
		extra->flags |= PCRE_EXTRA_MARK;
#endif
		/* Execute the regular expression. */
		count = pcre_exec(pce->re, extra, subject, subject_len, start_offset,
						  exoptions|g_notempty, offsets, size_offsets);

		/* the string was already proved to be valid UTF-8 */
		exoptions |= PCRE_NO_UTF8_CHECK;

		/* Check for too many substrings condition. */
		if (UNEXPECTED(count == 0)) {
			php_error_docref(NULL,E_NOTICE, "Matched, but too many substrings");
			count = size_offsets / 3;
		}

		piece = subject + start_offset;

		/* if (EXPECTED(count > 0 && (limit == -1 || limit > 0))) */
		if (EXPECTED(count > 0 && (offsets[1] - offsets[0] >= 0) && limit)) {
			if (UNEXPECTED(replace_count)) {
				++*replace_count;
			}

			/* Set the match location in subject */
			match = subject + offsets[0];

			new_len = result_len + offsets[0] - start_offset; /* part before the match */
			
			/* if (!is_callable_replace) */
			if (EXPECTED(replace)) {
				/* do regular substitution */
				walk = replace;
				walk_last = 0;

				while (walk < replace_end) {
					if ('\\' == *walk || '$' == *walk) {
						if (walk_last == '\\') {
							walk++;
							walk_last = 0;
							continue;
						}
						if (preg_get_backref(&walk, &backref)) {
							if (backref < count)
								new_len += offsets[(backref<<1)+1] - offsets[backref<<1];
							continue;
						}
					}
					new_len++;
					walk++;
					walk_last = walk[-1];
				}

				if (new_len >= alloc_len) {
					alloc_len = alloc_len + 2 * new_len;
					if (result == NULL) {
						result = zend_string_alloc(alloc_len, 0);
					} else {
						result = zend_string_extend(result, alloc_len, 0);
					}
				}

				/* copy the part of the string before the match */
				memcpy(&ZSTR_VAL(result)[result_len], piece, match-piece);
				result_len += (int)(match-piece);

				/* copy replacement and backrefs */
				walkbuf = ZSTR_VAL(result) + result_len;

				walk = replace;
				walk_last = 0;
				while (walk < replace_end) {
					if ('\\' == *walk || '$' == *walk) {
						if (walk_last == '\\') {
							*(walkbuf-1) = *walk++;
							walk_last = 0;
							continue;
						}
						if (preg_get_backref(&walk, &backref)) {
							if (backref < count) {
								match_len = offsets[(backref<<1)+1] - offsets[backref<<1];
								memcpy(walkbuf, subject + offsets[backref<<1], match_len);
								walkbuf += match_len;
							}
							continue;
						}
					}
					*walkbuf++ = *walk++;
					walk_last = walk[-1];
				}
				*walkbuf = '\0';
				/* increment the result length by how much we've added to the string */
				result_len += (int)(walkbuf - (ZSTR_VAL(result) + result_len));
			} else {
				/* Use custom function to get replacement string and its length. */
				eval_result = preg_do_repl_func(replace_val, subject, offsets, subpat_names, count, mark);
				ZEND_ASSERT(eval_result);
				new_len += (int)ZSTR_LEN(eval_result);
				if (new_len >= alloc_len) {
					alloc_len = alloc_len + 2 * new_len;
					if (result == NULL) {
						result = zend_string_alloc(alloc_len, 0);
					} else {
						result = zend_string_extend(result, alloc_len, 0);
					}
				}
				/* copy the part of the string before the match */
				memcpy(ZSTR_VAL(result) + result_len, piece, match-piece);
				result_len += (int)(match-piece);

				/* copy replacement and backrefs */
				walkbuf = ZSTR_VAL(result) + result_len;

				/* If using custom function, copy result to the buffer and clean up. */
				memcpy(walkbuf, ZSTR_VAL(eval_result), ZSTR_LEN(eval_result));
				result_len += (int)ZSTR_LEN(eval_result);
				zend_string_release(eval_result);
			}

			if (EXPECTED(limit)) {
				limit--;
			}
		} else if (count == PCRE_ERROR_NOMATCH || UNEXPECTED(limit == 0)) {
			/* If we previously set PCRE_NOTEMPTY_ATSTART after a null match,
			   this is not necessarily the end. We need to advance
			   the start offset, and continue. Fudge the offset values
			   to achieve this, unless we're already at the end of the string. */
			if (g_notempty != 0 && start_offset < subject_len) {
				int unit_len = calculate_unit_length(pce, piece);

				offsets[0] = start_offset;
				offsets[1] = start_offset + unit_len;
				memcpy(ZSTR_VAL(result) + result_len, piece, unit_len);
				result_len += unit_len;
			} else {
				if (!result && subject_str) {
					result = zend_string_copy(subject_str);
					break;
				}
				new_len = result_len + subject_len - start_offset;
				if (new_len >= alloc_len) {
					alloc_len = new_len; /* now we know exactly how long it is */
					if (NULL != result) {
						result = zend_string_realloc(result, alloc_len, 0);
					} else {
						result = zend_string_alloc(alloc_len, 0);
					}
				}
				/* stick that last bit of string on our output */
				memcpy(ZSTR_VAL(result) + result_len, piece, subject_len - start_offset);
				result_len += subject_len - start_offset;
				ZSTR_VAL(result)[result_len] = '\0';
				ZSTR_LEN(result) = result_len;
				break;
			}
		} else {
			pcre_handle_exec_error(count);
			if (result) {
				zend_string_free(result);
				result = NULL;
			}
			break;
		}

		/* If we have matched an empty string, mimic what Perl's /g options does.
		   This turns out to be rather cunning. First we set PCRE_NOTEMPTY_ATSTART and try
		   the match again at the same point. If this fails (picked up above) we
		   advance to the next character. */
		g_notempty = (offsets[1] == offsets[0])? PCRE_NOTEMPTY_ATSTART | PCRE_ANCHORED : 0;

		/* Advance to the next piece. */
		start_offset = offsets[1];
	}

	if (size_offsets <= 32) {
		free_alloca(offsets, use_heap);
	} else {
		efree(offsets);
	}
	if (UNEXPECTED(subpat_names)) {
		efree(subpat_names);
	}

	return result;
}
/* }}} */

/* {{{ php_replace_in_subject
 */
static zend_string *php_replace_in_subject(zval *regex, zval *replace, zval *subject, int limit, int is_callable_replace, int *replace_count)
{
	zval		*regex_entry,
				*replace_value,
				 empty_replace;
	zend_string *result;
	uint32_t replace_idx;
	zend_string	*subject_str = zval_get_string(subject);

	/* FIXME: This might need to be changed to ZSTR_EMPTY_ALLOC(). Check if this zval could be dtor()'ed somehow */
	ZVAL_EMPTY_STRING(&empty_replace);

	if (ZEND_SIZE_T_INT_OVFL(ZSTR_LEN(subject_str))) {
			php_error_docref(NULL, E_WARNING, "Subject is too long");
			return NULL;
	}

	/* If regex is an array */
	if (Z_TYPE_P(regex) == IS_ARRAY) {
		replace_value = replace;
		replace_idx = 0;

		/* For each entry in the regex array, get the entry */
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(regex), regex_entry) {
			zval replace_str;
			/* Make sure we're dealing with strings. */
			zend_string *regex_str = zval_get_string(regex_entry);

			ZVAL_UNDEF(&replace_str);
			/* If replace is an array and not a callable construct */
			if (Z_TYPE_P(replace) == IS_ARRAY && !is_callable_replace) {
				/* Get current entry */
				while (replace_idx < Z_ARRVAL_P(replace)->nNumUsed) {
					if (Z_TYPE(Z_ARRVAL_P(replace)->arData[replace_idx].val) != IS_UNDEF) {
						ZVAL_COPY(&replace_str, &Z_ARRVAL_P(replace)->arData[replace_idx].val);
						break;
					}
					replace_idx++;
				}
				if (!Z_ISUNDEF(replace_str)) {
					if (!is_callable_replace) {
						convert_to_string(&replace_str);
					}
					replace_value = &replace_str;
					replace_idx++;
				} else {
					/* We've run out of replacement strings, so use an empty one */
					replace_value = &empty_replace;
				}
			}

			/* Do the actual replacement and put the result back into subject_str
			   for further replacements. */
			if ((result = php_pcre_replace(regex_str,
										   subject_str,
										   ZSTR_VAL(subject_str),
										   (int)ZSTR_LEN(subject_str),
										   replace_value,
										   is_callable_replace,
										   limit,
										   replace_count)) != NULL) {
				zend_string_release(subject_str);
				subject_str = result;
			} else {
				zend_string_release(subject_str);
				zend_string_release(regex_str);
				zval_dtor(&replace_str);
				return NULL;
			}

			zend_string_release(regex_str);
			zval_dtor(&replace_str);
		} ZEND_HASH_FOREACH_END();

		return subject_str;
	} else {
		result = php_pcre_replace(Z_STR_P(regex),
								  subject_str,
								  ZSTR_VAL(subject_str),
								  (int)ZSTR_LEN(subject_str),
								  replace,
								  is_callable_replace,
								  limit,
								  replace_count);
		zend_string_release(subject_str);
		return result;
	}
}
/* }}} */

/* {{{ preg_replace_impl
 */
static int preg_replace_impl(zval *return_value, zval *regex, zval *replace, zval *subject, zend_long limit_val, int is_callable_replace, int is_filter)
{
	zval		*subject_entry;
	zend_string	*result;
	zend_string	*string_key;
	zend_ulong	 num_key;
	int			 replace_count = 0, old_replace_count;

	if (Z_TYPE_P(replace) != IS_ARRAY && (Z_TYPE_P(replace) != IS_OBJECT || !is_callable_replace)) {
		convert_to_string_ex(replace);
	}

	if (Z_TYPE_P(regex) != IS_ARRAY) {
		convert_to_string_ex(regex);
	}

	/* if subject is an array */
	if (Z_TYPE_P(subject) == IS_ARRAY) {
		array_init_size(return_value, zend_hash_num_elements(Z_ARRVAL_P(subject)));

		/* For each subject entry, convert it to string, then perform replacement
		   and add the result to the return_value array. */
		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(subject), num_key, string_key, subject_entry) {
			old_replace_count = replace_count;
			if ((result = php_replace_in_subject(regex, replace, subject_entry, limit_val, is_callable_replace, &replace_count)) != NULL) {
				if (!is_filter || replace_count > old_replace_count) {
					/* Add to return array */
					zval zv;

					ZVAL_STR(&zv, result);
					if (string_key) {
						zend_hash_add_new(Z_ARRVAL_P(return_value), string_key, &zv);
					} else {
						zend_hash_index_add_new(Z_ARRVAL_P(return_value), num_key, &zv);
					}
				} else {
					zend_string_release(result);
				}
			}
		} ZEND_HASH_FOREACH_END();
	} else {	
		/* if subject is not an array */
		old_replace_count = replace_count;
		if ((result = php_replace_in_subject(regex, replace, subject, limit_val, is_callable_replace, &replace_count)) != NULL) {
			if (!is_filter || replace_count > old_replace_count) {
				RETVAL_STR(result);
			} else {
				zend_string_release(result);
			}
		}
	}
	
	return replace_count;
}
/* }}} */

/* {{{ proto mixed preg_replace(mixed regex, mixed replace, mixed subject [, int limit [, int &count]])
   Perform Perl-style regular expression replacement. */
static PHP_FUNCTION(preg_replace)
{
	zval *regex, *replace, *subject, *zcount = NULL;
	zend_long limit = -1;
	int replace_count;

#ifndef FAST_ZPP
	/* Get function parameters and do error-checking. */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zzz|lz/", &regex, &replace, &subject, &limit, &zcount) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(3, 5)
		Z_PARAM_ZVAL(regex)
		Z_PARAM_ZVAL(replace)
		Z_PARAM_ZVAL(subject)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(limit)
		Z_PARAM_ZVAL_EX(zcount, 0, 1)
	ZEND_PARSE_PARAMETERS_END();
#endif

	if (Z_TYPE_P(replace) == IS_ARRAY && Z_TYPE_P(regex) != IS_ARRAY) {
		php_error_docref(NULL, E_WARNING, "Parameter mismatch, pattern is a string while replacement is an array");
		RETURN_FALSE;
	}

	replace_count = preg_replace_impl(return_value, regex, replace, subject, limit, 0, 0);
	if (zcount) {
		zval_dtor(zcount);
		ZVAL_LONG(zcount, replace_count);
	}
}
/* }}} */

/* {{{ proto mixed preg_replace_callback(mixed regex, mixed callback, mixed subject [, int limit [, int &count]])
   Perform Perl-style regular expression replacement using replacement callback. */
static PHP_FUNCTION(preg_replace_callback)
{
	zval *regex, *replace, *subject, *zcount = NULL;
	zend_long limit = -1;
	zend_string	*callback_name;
	int replace_count;

#ifndef FAST_ZPP
	/* Get function parameters and do error-checking. */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zzz|lz/", &regex, &replace, &subject, &limit, &zcount) == FAILURE) {
		return;
	}    
#else
	ZEND_PARSE_PARAMETERS_START(3, 5)
		Z_PARAM_ZVAL(regex)
		Z_PARAM_ZVAL(replace)
		Z_PARAM_ZVAL(subject)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(limit)
		Z_PARAM_ZVAL_EX(zcount, 0, 1)
	ZEND_PARSE_PARAMETERS_END();
#endif

	if (!zend_is_callable(replace, 0, &callback_name)) {
		php_error_docref(NULL, E_WARNING, "Requires argument 2, '%s', to be a valid callback", ZSTR_VAL(callback_name));
		zend_string_release(callback_name);
		ZVAL_COPY(return_value, subject);
		return;
	}
	zend_string_release(callback_name);

	replace_count = preg_replace_impl(return_value, regex, replace, subject, limit, 1, 0);
	if (zcount) {
		zval_dtor(zcount);
		ZVAL_LONG(zcount, replace_count);
	}
}
/* }}} */

/* {{{ proto mixed preg_replace_callback_array(array pattern, mixed subject [, int limit [, int &count]])
   Perform Perl-style regular expression replacement using replacement callback. */
static PHP_FUNCTION(preg_replace_callback_array)
{
	zval regex, zv, *replace, *subject, *pattern, *zcount = NULL;
	zend_long limit = -1;
	zend_string *str_idx;
	zend_string *callback_name;
	int replace_count = 0;

#ifndef FAST_ZPP
	/* Get function parameters and do error-checking. */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "az|lz/", &pattern, &subject, &limit, &zcount) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_ARRAY(pattern)
		Z_PARAM_ZVAL(subject)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(limit)
		Z_PARAM_ZVAL_EX(zcount, 0, 1)
	ZEND_PARSE_PARAMETERS_END();
#endif
	
	ZVAL_UNDEF(&zv);
	ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pattern), str_idx, replace) {
		if (str_idx) {
			ZVAL_STR_COPY(&regex, str_idx);
		} else {
			php_error_docref(NULL, E_WARNING, "Delimiter must not be alphanumeric or backslash");
			zval_ptr_dtor(return_value);
			RETURN_NULL();
		}		

		if (!zend_is_callable(replace, 0, &callback_name)) {
			php_error_docref(NULL, E_WARNING, "'%s' is not a valid callback", ZSTR_VAL(callback_name));
			zend_string_release(callback_name);
			zval_ptr_dtor(&regex);
			zval_ptr_dtor(return_value);
			ZVAL_COPY(return_value, subject);
			return;
		}
		zend_string_release(callback_name);

		if (Z_ISNULL_P(return_value)) {
			replace_count += preg_replace_impl(&zv, &regex, replace, subject, limit, 1, 0);
		} else {
			replace_count += preg_replace_impl(&zv, &regex, replace, return_value, limit, 1, 0);
			zval_ptr_dtor(return_value);
		}

		zval_ptr_dtor(&regex);

		if (Z_ISUNDEF(zv)) {
			RETURN_NULL();	
		}

		ZVAL_COPY_VALUE(return_value, &zv);

		if (UNEXPECTED(EG(exception))) {
			zval_ptr_dtor(return_value);
			RETURN_NULL();	
		}
	} ZEND_HASH_FOREACH_END();

	if (zcount) {
		zval_dtor(zcount);
		ZVAL_LONG(zcount, replace_count);
	}
}
/* }}} */

/* {{{ proto mixed preg_filter(mixed regex, mixed replace, mixed subject [, int limit [, int &count]])
   Perform Perl-style regular expression replacement and only return matches. */
static PHP_FUNCTION(preg_filter)
{
	zval *regex, *replace, *subject, *zcount = NULL;
	zend_long limit = -1;
	int replace_count;

#ifndef FAST_ZPP
	/* Get function parameters and do error-checking. */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zzz|lz/", &regex, &replace, &subject, &limit, &zcount) == FAILURE) {
		return;
	}    
#else
	ZEND_PARSE_PARAMETERS_START(3, 5)
		Z_PARAM_ZVAL(regex)
		Z_PARAM_ZVAL(replace)
		Z_PARAM_ZVAL(subject)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(limit)
		Z_PARAM_ZVAL_EX(zcount, 0, 1)
	ZEND_PARSE_PARAMETERS_END();
#endif

	if (Z_TYPE_P(replace) == IS_ARRAY && Z_TYPE_P(regex) != IS_ARRAY) {
		php_error_docref(NULL, E_WARNING, "Parameter mismatch, pattern is a string while replacement is an array");
		RETURN_FALSE;
	}

	replace_count = preg_replace_impl(return_value, regex, replace, subject, limit, 0, 1);
	if (zcount) {
		zval_dtor(zcount);
		ZVAL_LONG(zcount, replace_count);
	}
}
/* }}} */

/* {{{ proto array preg_split(string pattern, string subject [, int limit [, int flags]])
   Split string into an array using a perl-style regular expression as a delimiter */
static PHP_FUNCTION(preg_split)
{
	zend_string			*regex;			/* Regular expression */
	zend_string			*subject;		/* String to match against */
	zend_long			 limit_val = -1;/* Integer value of limit */
	zend_long			 flags = 0;		/* Match control flags */
	pcre_cache_entry	*pce;			/* Compiled regular expression */

	/* Get function parameters and do error checking */
#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS|ll", &regex,
							  &subject, &limit_val, &flags) == FAILURE) {
		RETURN_FALSE;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 4)
		Z_PARAM_STR(regex)
		Z_PARAM_STR(subject)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(limit_val)
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
#endif

	if (ZEND_SIZE_T_INT_OVFL(ZSTR_LEN(subject))) {
			php_error_docref(NULL, E_WARNING, "Subject is too long");
			RETURN_FALSE;
	}

	/* Compile regex or get it from cache. */
	if ((pce = pcre_get_compiled_regex_cache(regex)) == NULL) {
		RETURN_FALSE;
	}

	pce->refcount++;
	php_pcre_split_impl(pce, ZSTR_VAL(subject), (int)ZSTR_LEN(subject), return_value, (int)limit_val, flags);
	pce->refcount--;
}
/* }}} */

/* {{{ php_pcre_split
 */
PHPAPI void php_pcre_split_impl(pcre_cache_entry *pce, char *subject, int subject_len, zval *return_value,
	zend_long limit_val, zend_long flags)
{
	pcre_extra		*extra = pce->extra;/* Holds results of studying */
	pcre_extra		 extra_data;		/* Used locally for exec options */
	int				*offsets;			/* Array of subpattern offsets */
	int				 size_offsets;		/* Size of the offsets array */
	int				 exoptions = 0;		/* Execution options */
	int				 count = 0;			/* Count of matched subpatterns */
	int				 start_offset;		/* Where the new search starts */
	int				 next_offset;		/* End of the last delimiter match + 1 */
	int				 g_notempty = 0;	/* If the match should not be empty */
	char			*last_match;		/* Location of last match */
	int				 no_empty;			/* If NO_EMPTY flag is set */
	int				 delim_capture; 	/* If delimiters should be captured */
	int				 offset_capture;	/* If offsets should be captured */
	zval			 tmp;
	ALLOCA_FLAG(use_heap);

	no_empty = flags & PREG_SPLIT_NO_EMPTY;
	delim_capture = flags & PREG_SPLIT_DELIM_CAPTURE;
	offset_capture = flags & PREG_SPLIT_OFFSET_CAPTURE;

	if (limit_val == 0) {
		limit_val = -1;
	}

	if (extra == NULL) {
		extra_data.flags = PCRE_EXTRA_MATCH_LIMIT | PCRE_EXTRA_MATCH_LIMIT_RECURSION;
		extra = &extra_data;
	}
	extra->match_limit = (unsigned long)PCRE_G(backtrack_limit);
	extra->match_limit_recursion = (unsigned long)PCRE_G(recursion_limit);
#ifdef PCRE_EXTRA_MARK
	extra->flags &= ~PCRE_EXTRA_MARK;
#endif

	/* Initialize return value */
	array_init(return_value);

	/* Calculate the size of the offsets array, and allocate memory for it. */
	size_offsets = (pce->capture_count + 1) * 3;
	if (size_offsets <= 32) {
		offsets = (int *)do_alloca(size_offsets * sizeof(int), use_heap);
	} else {
		offsets = (int *)safe_emalloc(size_offsets, sizeof(int), 0);
	}

	/* Start at the beginning of the string */
	start_offset = 0;
	next_offset = 0;
	last_match = subject;
	PCRE_G(error_code) = PHP_PCRE_NO_ERROR;

	/* Get next piece if no limit or limit not yet reached and something matched*/
	while ((limit_val == -1 || limit_val > 1)) {
		count = pcre_exec(pce->re, extra, subject,
						  subject_len, start_offset,
						  exoptions|g_notempty, offsets, size_offsets);

		/* the string was already proved to be valid UTF-8 */
		exoptions |= PCRE_NO_UTF8_CHECK;

		/* Check for too many substrings condition. */
		if (count == 0) {
			php_error_docref(NULL,E_NOTICE, "Matched, but too many substrings");
			count = size_offsets/3;
		}

		/* If something matched */
		if (count > 0 && (offsets[1] - offsets[0] >= 0)) {
			if (!no_empty || &subject[offsets[0]] != last_match) {

				if (offset_capture) {
					/* Add (match, offset) pair to the return value */
					add_offset_pair(return_value, last_match, (int)(&subject[offsets[0]]-last_match), next_offset, NULL);
				} else {
					/* Add the piece to the return value */
					ZVAL_STRINGL(&tmp, last_match, &subject[offsets[0]]-last_match);
					zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &tmp);
				}

				/* One less left to do */
				if (limit_val != -1)
					limit_val--;
			}

			last_match = &subject[offsets[1]];
			next_offset = offsets[1];

			if (delim_capture) {
				int i, match_len;
				for (i = 1; i < count; i++) {
					match_len = offsets[(i<<1)+1] - offsets[i<<1];
					/* If we have matched a delimiter */
					if (!no_empty || match_len > 0) {
						if (offset_capture) {
							add_offset_pair(return_value, &subject[offsets[i<<1]], match_len, offsets[i<<1], NULL);
						} else {
							ZVAL_STRINGL(&tmp, &subject[offsets[i<<1]], match_len);
							zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &tmp);
						}
					}
				}
			}
		} else if (count == PCRE_ERROR_NOMATCH) {
			/* If we previously set PCRE_NOTEMPTY_ATSTART after a null match,
			   this is not necessarily the end. We need to advance
			   the start offset, and continue. Fudge the offset values
			   to achieve this, unless we're already at the end of the string. */
			if (g_notempty != 0 && start_offset < subject_len) {
				offsets[0] = start_offset;
				offsets[1] = start_offset + calculate_unit_length(pce, subject + start_offset);
			} else {
				break;
			}
		} else {
			pcre_handle_exec_error(count);
			break;
		}

		/* If we have matched an empty string, mimic what Perl's /g options does.
		   This turns out to be rather cunning. First we set PCRE_NOTEMPTY_ATSTART and try
		   the match again at the same point. If this fails (picked up above) we
		   advance to the next character. */
		g_notempty = (offsets[1] == offsets[0])? PCRE_NOTEMPTY_ATSTART | PCRE_ANCHORED : 0;

		/* Advance to the position right after the last full match */
		start_offset = offsets[1];
	}


	start_offset = (int)(last_match - subject); /* the offset might have been incremented, but without further successful matches */

	if (!no_empty || start_offset < subject_len)
	{
		if (offset_capture) {
			/* Add the last (match, offset) pair to the return value */
			add_offset_pair(return_value, &subject[start_offset], subject_len - start_offset, start_offset, NULL);
		} else {
			/* Add the last piece to the return value */
			ZVAL_STRINGL(&tmp, last_match, subject + subject_len - last_match);
			zend_hash_next_index_insert_new(Z_ARRVAL_P(return_value), &tmp);
		}
	}


	/* Clean up */
	if (size_offsets <= 32) {
		free_alloca(offsets, use_heap);
	} else {
		efree(offsets);
	}
}
/* }}} */

/* {{{ proto string preg_quote(string str [, string delim_char])
   Quote regular expression characters plus an optional character */
static PHP_FUNCTION(preg_quote)
{
	size_t		 in_str_len;
	char	*in_str;		/* Input string argument */
	char	*in_str_end;    /* End of the input string */
	size_t		 delim_len = 0;
	char	*delim = NULL;	/* Additional delimiter argument */
	zend_string	*out_str;	/* Output string with quoted characters */
	char 	*p,				/* Iterator for input string */
			*q,				/* Iterator for output string */
			 delim_char=0,	/* Delimiter character to be quoted */
			 c;				/* Current character */
	zend_bool quote_delim = 0; /* Whether to quote additional delim char */

	/* Get the arguments and check for errors */
#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s", &in_str, &in_str_len,
							  &delim, &delim_len) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(in_str, in_str_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(delim, delim_len)
	ZEND_PARSE_PARAMETERS_END();
#endif

	in_str_end = in_str + in_str_len;

	/* Nothing to do if we got an empty string */
	if (in_str == in_str_end) {
		RETURN_EMPTY_STRING();
	}

	if (delim && *delim) {
		delim_char = delim[0];
		quote_delim = 1;
	}

	/* Allocate enough memory so that even if each character
	   is quoted, we won't run out of room */
	out_str = zend_string_safe_alloc(4, in_str_len, 0, 0);

	/* Go through the string and quote necessary characters */
	for (p = in_str, q = ZSTR_VAL(out_str); p != in_str_end; p++) {
		c = *p;
		switch(c) {
			case '.':
			case '\\':
			case '+':
			case '*':
			case '?':
			case '[':
			case '^':
			case ']':
			case '$':
			case '(':
			case ')':
			case '{':
			case '}':
			case '=':
			case '!':
			case '>':
			case '<':
			case '|':
			case ':':
			case '-':
				*q++ = '\\';
				*q++ = c;
				break;

			case '\0':
				*q++ = '\\';
				*q++ = '0';
				*q++ = '0';
				*q++ = '0';
				break;

			default:
				if (quote_delim && c == delim_char)
					*q++ = '\\';
				*q++ = c;
				break;
		}
	}
	*q = '\0';

	/* Reallocate string and return it */
	out_str = zend_string_truncate(out_str, q - ZSTR_VAL(out_str), 0);
	RETURN_NEW_STR(out_str);
}
/* }}} */

/* {{{ proto array preg_grep(string regex, array input [, int flags])
   Searches array and returns entries which match regex */
static PHP_FUNCTION(preg_grep)
{
	zend_string			*regex;			/* Regular expression */
	zval				*input;			/* Input array */
	zend_long			 flags = 0;		/* Match control flags */
	pcre_cache_entry	*pce;			/* Compiled regular expression */

	/* Get arguments and do error checking */
#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sa|l", &regex,
							  &input, &flags) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STR(regex)
		Z_PARAM_ARRAY(input)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();
#endif

	/* Compile regex or get it from cache. */
	if ((pce = pcre_get_compiled_regex_cache(regex)) == NULL) {
		RETURN_FALSE;
	}

	pce->refcount++;
	php_pcre_grep_impl(pce, input, return_value, flags);
	pce->refcount--;
}
/* }}} */

PHPAPI void  php_pcre_grep_impl(pcre_cache_entry *pce, zval *input, zval *return_value, zend_long flags) /* {{{ */
{
	zval		    *entry;				/* An entry in the input array */
	pcre_extra		*extra = pce->extra;/* Holds results of studying */
	pcre_extra		 extra_data;		/* Used locally for exec options */
	int				*offsets;			/* Array of subpattern offsets */
	int				 size_offsets;		/* Size of the offsets array */
	int				 count = 0;			/* Count of matched subpatterns */
	zend_string		*string_key;
	zend_ulong		 num_key;
	zend_bool		 invert;			/* Whether to return non-matching
										   entries */
	ALLOCA_FLAG(use_heap);

	invert = flags & PREG_GREP_INVERT ? 1 : 0;

	if (extra == NULL) {
		extra_data.flags = PCRE_EXTRA_MATCH_LIMIT | PCRE_EXTRA_MATCH_LIMIT_RECURSION;
		extra = &extra_data;
	}
	extra->match_limit = (unsigned long)PCRE_G(backtrack_limit);
	extra->match_limit_recursion = (unsigned long)PCRE_G(recursion_limit);
#ifdef PCRE_EXTRA_MARK
	extra->flags &= ~PCRE_EXTRA_MARK;
#endif

	/* Calculate the size of the offsets array, and allocate memory for it. */
	size_offsets = (pce->capture_count + 1) * 3;
	if (size_offsets <= 32) {
		offsets = (int *)do_alloca(size_offsets * sizeof(int), use_heap);
	} else {
		offsets = (int *)safe_emalloc(size_offsets, sizeof(int), 0);
	}

	/* Initialize return array */
	array_init(return_value);

	PCRE_G(error_code) = PHP_PCRE_NO_ERROR;

	/* Go through the input array */
	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(input), num_key, string_key, entry) {
		zend_string *subject_str = zval_get_string(entry);

		/* Perform the match */
		count = pcre_exec(pce->re, extra, ZSTR_VAL(subject_str),
						  (int)ZSTR_LEN(subject_str), 0,
						  0, offsets, size_offsets);

		/* Check for too many substrings condition. */
		if (count == 0) {
			php_error_docref(NULL, E_NOTICE, "Matched, but too many substrings");
			count = size_offsets/3;
		} else if (count < 0 && count != PCRE_ERROR_NOMATCH) {
			pcre_handle_exec_error(count);
			zend_string_release(subject_str);
			break;
		}

		/* If the entry fits our requirements */
		if ((count > 0 && !invert) || (count == PCRE_ERROR_NOMATCH && invert)) {
			if (Z_REFCOUNTED_P(entry)) {
			   	Z_ADDREF_P(entry);
			}

			/* Add to return array */
			if (string_key) {
				zend_hash_update(Z_ARRVAL_P(return_value), string_key, entry);
			} else {
				zend_hash_index_update(Z_ARRVAL_P(return_value), num_key, entry);
			}
		}

		zend_string_release(subject_str);
	} ZEND_HASH_FOREACH_END();

	/* Clean up */
	if (size_offsets <= 32) {
		free_alloca(offsets, use_heap);
	} else {
		efree(offsets);
	}
}
/* }}} */

/* {{{ proto int preg_last_error()
   Returns the error code of the last regexp execution. */
static PHP_FUNCTION(preg_last_error)
{
#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "") == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(0, 0)
	ZEND_PARSE_PARAMETERS_END();
#endif

	RETURN_LONG(PCRE_G(error_code));
}
/* }}} */

/* {{{ module definition structures */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_preg_match, 0, 0, 2)
    ZEND_ARG_INFO(0, pattern)
    ZEND_ARG_INFO(0, subject)
    ZEND_ARG_INFO(1, subpatterns) /* array */
    ZEND_ARG_INFO(0, flags)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_preg_match_all, 0, 0, 2)
    ZEND_ARG_INFO(0, pattern)
    ZEND_ARG_INFO(0, subject)
    ZEND_ARG_INFO(1, subpatterns) /* array */
    ZEND_ARG_INFO(0, flags)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_preg_replace, 0, 0, 3)
    ZEND_ARG_INFO(0, regex)
    ZEND_ARG_INFO(0, replace)
    ZEND_ARG_INFO(0, subject)
    ZEND_ARG_INFO(0, limit)
    ZEND_ARG_INFO(1, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_preg_replace_callback, 0, 0, 3)
    ZEND_ARG_INFO(0, regex)
    ZEND_ARG_INFO(0, callback)
    ZEND_ARG_INFO(0, subject)
    ZEND_ARG_INFO(0, limit)
    ZEND_ARG_INFO(1, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_preg_replace_callback_array, 0, 0, 2)
    ZEND_ARG_INFO(0, pattern)
    ZEND_ARG_INFO(0, subject)
    ZEND_ARG_INFO(0, limit)
    ZEND_ARG_INFO(1, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_preg_split, 0, 0, 2)
    ZEND_ARG_INFO(0, pattern)
    ZEND_ARG_INFO(0, subject)
    ZEND_ARG_INFO(0, limit)
    ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_preg_quote, 0, 0, 1)
    ZEND_ARG_INFO(0, str)
    ZEND_ARG_INFO(0, delim_char)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_preg_grep, 0, 0, 2)
    ZEND_ARG_INFO(0, regex)
    ZEND_ARG_INFO(0, input) /* array */
    ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_preg_last_error, 0)
ZEND_END_ARG_INFO()
/* }}} */

static const zend_function_entry pcre_functions[] = {
	PHP_FE(preg_match,					arginfo_preg_match)
	PHP_FE(preg_match_all,				arginfo_preg_match_all)
	PHP_FE(preg_replace,				arginfo_preg_replace)
	PHP_FE(preg_replace_callback,		arginfo_preg_replace_callback)
	PHP_FE(preg_replace_callback_array,	arginfo_preg_replace_callback_array)
	PHP_FE(preg_filter,					arginfo_preg_replace)
	PHP_FE(preg_split,					arginfo_preg_split)
	PHP_FE(preg_quote,					arginfo_preg_quote)
	PHP_FE(preg_grep,					arginfo_preg_grep)
	PHP_FE(preg_last_error,				arginfo_preg_last_error)
	PHP_FE_END
};

zend_module_entry pcre_module_entry = {
	STANDARD_MODULE_HEADER,
   "pcre",
	pcre_functions,
	PHP_MINIT(pcre),
	PHP_MSHUTDOWN(pcre),
#ifdef PCRE_STUDY_JIT_COMPILE
	PHP_RINIT(pcre),
#else
	NULL
#endif
	NULL,
	PHP_MINFO(pcre),
	PHP_PCRE_VERSION,
	PHP_MODULE_GLOBALS(pcre),
	PHP_GINIT(pcre),
	PHP_GSHUTDOWN(pcre),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_PCRE
ZEND_GET_MODULE(pcre)
#endif

/* }}} */

#endif /* HAVE_PCRE || HAVE_BUNDLED_PCRE */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
