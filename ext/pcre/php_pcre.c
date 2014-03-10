/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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
#include "ext/standard/php_smart_str.h"

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

enum {
	PHP_PCRE_NO_ERROR = 0,
	PHP_PCRE_INTERNAL_ERROR,
	PHP_PCRE_BACKTRACK_LIMIT_ERROR,
	PHP_PCRE_RECURSION_LIMIT_ERROR,
	PHP_PCRE_BAD_UTF8_ERROR,
	PHP_PCRE_BAD_UTF8_OFFSET_ERROR
};


ZEND_DECLARE_MODULE_GLOBALS(pcre)


static void pcre_handle_exec_error(int pcre_code TSRMLS_DC) /* {{{ */
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

		default:
			preg_code = PHP_PCRE_INTERNAL_ERROR;
			break;
	}

	PCRE_G(error_code) = preg_code;
}
/* }}} */

static void php_free_pcre_cache(void *data) /* {{{ */
{
	pcre_cache_entry *pce = (pcre_cache_entry *) data;
	if (!pce) return;
	pefree(pce->re, 1);
	if (pce->extra) pefree(pce->extra, 1);
#if HAVE_SETLOCALE
	if ((void*)pce->tables) pefree((void*)pce->tables, 1);
	pefree(pce->locale, 1);
#endif
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
}
/* }}} */

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("pcre.backtrack_limit", "1000000", PHP_INI_ALL, OnUpdateLong, backtrack_limit, zend_pcre_globals, pcre_globals)
	STD_PHP_INI_ENTRY("pcre.recursion_limit", "100000", PHP_INI_ALL, OnUpdateLong, recursion_limit, zend_pcre_globals, pcre_globals)
PHP_INI_END()


/* {{{ PHP_MINFO_FUNCTION(pcre) */
static PHP_MINFO_FUNCTION(pcre)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "PCRE (Perl Compatible Regular Expressions) Support", "enabled" );
	php_info_print_table_row(2, "PCRE Library Version", pcre_version() );
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

/* {{{ static pcre_clean_cache */
static int pcre_clean_cache(void *data, void *arg TSRMLS_DC)
{
	int *num_clean = (int *)arg;

	if (*num_clean > 0) {
		(*num_clean)--;
		return 1;
	} else {
		return 0;
	}
}
/* }}} */

/* {{{ static make_subpats_table */
static char **make_subpats_table(int num_subpats, pcre_cache_entry *pce TSRMLS_DC)
{
	pcre_extra *extra = pce->extra;
	int name_cnt = 0, name_size, ni = 0;
	int rc;
	char *name_table;
	unsigned short name_idx;
	char **subpat_names = (char **)ecalloc(num_subpats, sizeof(char *));

	rc = pcre_fullinfo(pce->re, extra, PCRE_INFO_NAMECOUNT, &name_cnt);
	if (rc < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Internal pcre_fullinfo() error %d", rc);
		efree(subpat_names);
		return NULL;
	}
	if (name_cnt > 0) {
		int rc1, rc2;

		rc1 = pcre_fullinfo(pce->re, extra, PCRE_INFO_NAMETABLE, &name_table);
		rc2 = pcre_fullinfo(pce->re, extra, PCRE_INFO_NAMEENTRYSIZE, &name_size);
		rc = rc2 ? rc2 : rc1;
		if (rc < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Internal pcre_fullinfo() error %d", rc);
			efree(subpat_names);
			return NULL;
		}

		while (ni++ < name_cnt) {
			name_idx = 0xff * (unsigned char)name_table[0] + (unsigned char)name_table[1];
			subpat_names[name_idx] = name_table + 2;
			if (is_numeric_string(subpat_names[name_idx], strlen(subpat_names[name_idx]), NULL, NULL, 0) > 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Numeric named subpatterns are not allowed");
				efree(subpat_names);
				return NULL;
			}
			name_table += name_size;
		}
	}

	return subpat_names;
}
/* }}} */

/* {{{ pcre_get_compiled_regex_cache
 */
PHPAPI pcre_cache_entry* pcre_get_compiled_regex_cache(char *regex, int regex_len TSRMLS_DC)
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
	int				count = 0;
	unsigned const char *tables = NULL;
#if HAVE_SETLOCALE
	char				*locale;
#endif
	pcre_cache_entry	*pce;
	pcre_cache_entry	 new_entry;
	char                *tmp = NULL;

#if HAVE_SETLOCALE
# if defined(PHP_WIN32) && defined(ZTS)
	_configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
# endif
	locale = setlocale(LC_CTYPE, NULL);
#endif

	/* Try to lookup the cached regex entry, and if successful, just pass
	   back the compiled pattern, otherwise go on and compile it. */
	if (zend_hash_find(&PCRE_G(pcre_cache), regex, regex_len+1, (void **)&pce) == SUCCESS) {
		/*
		 * We use a quick pcre_fullinfo() check to see whether cache is corrupted, and if it
		 * is, we flush it and compile the pattern from scratch.
		 */
		if (pcre_fullinfo(pce->re, NULL, PCRE_INFO_CAPTURECOUNT, &count) == PCRE_ERROR_BADMAGIC) {
			zend_hash_clean(&PCRE_G(pcre_cache));
		} else {
#if HAVE_SETLOCALE
			if (!strcmp(pce->locale, locale)) {
#endif
				return pce;
#if HAVE_SETLOCALE
			}
#endif
		}
	}
	
	p = regex;
	
	/* Parse through the leading whitespace, and display a warning if we
	   get to the end without encountering a delimiter. */
	while (isspace((int)*(unsigned char *)p)) p++;
	if (*p == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, 
						 p < regex + regex_len ? "Null byte in regex" : "Empty regular expression");
		return NULL;
	}
	
	/* Get the delimiter and display a warning if it is alphanumeric
	   or a backslash. */
	delimiter = *p++;
	if (isalnum((int)*(unsigned char *)&delimiter) || delimiter == '\\') {
		php_error_docref(NULL TSRMLS_CC,E_WARNING, "Delimiter must not be alphanumeric or backslash");
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
		if (pp < regex + regex_len) {
			php_error_docref(NULL TSRMLS_CC,E_WARNING, "Null byte in regex");
		} else if (start_delimiter == end_delimiter) {
			php_error_docref(NULL TSRMLS_CC,E_WARNING, "No ending delimiter '%c' found", delimiter);
		} else {
			php_error_docref(NULL TSRMLS_CC,E_WARNING, "No ending matching delimiter '%c' found", delimiter);
		}
		return NULL;
	}
	
	/* Make a copy of the actual pattern. */
	pattern = estrndup(p, pp-p);

	/* Move on to the options */
	pp++;

	/* Parse through the options, setting appropriate flags.  Display
	   a warning if we encounter an unknown modifier. */	
	while (pp < regex + regex_len) {
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
					php_error_docref(NULL TSRMLS_CC,E_WARNING, "Unknown modifier '%c'", pp[-1]);
				} else {
					php_error_docref(NULL TSRMLS_CC,E_WARNING, "Null byte in regex");
				}
				efree(pattern);
				return NULL;
		}
	}

#if HAVE_SETLOCALE
	if (strcmp(locale, "C"))
		tables = pcre_maketables();
#endif

	/* Compile pattern and display a warning if compilation failed. */
	re = pcre_compile(pattern,
					  coptions,
					  &error,
					  &erroffset,
					  tables);

	if (re == NULL) {
		php_error_docref(NULL TSRMLS_CC,E_WARNING, "Compilation failed: %s at offset %d", error, erroffset);
		efree(pattern);
		if (tables) {
			pefree((void*)tables, 1);
		}
		return NULL;
	}

	/* If study option was specified, study the pattern and
	   store the result in extra for passing to pcre_exec. */
	if (do_study) {
		extra = pcre_study(re, soptions, &error);
		if (extra) {
			extra->flags |= PCRE_EXTRA_MATCH_LIMIT | PCRE_EXTRA_MATCH_LIMIT_RECURSION;
		}
		if (error != NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error while studying pattern");
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
		zend_hash_apply_with_argument(&PCRE_G(pcre_cache), pcre_clean_cache, &num_clean TSRMLS_CC);
	}

	/* Store the compiled pattern and extra info in the cache. */
	new_entry.re = re;
	new_entry.extra = extra;
	new_entry.preg_options = poptions;
	new_entry.compile_options = coptions;
#if HAVE_SETLOCALE
	new_entry.locale = pestrdup(locale, 1);
	new_entry.tables = tables;
#endif

	/*
	 * Interned strings are not duplicated when stored in HashTable,
	 * but all the interned strings created during HTTP request are removed
	 * at end of request. However PCRE_G(pcre_cache) must be consistent
	 * on the next request as well. So we disable usage of interned strings
	 * as hash keys especually for this table.
	 * See bug #63180 
	 */
	if (IS_INTERNED(regex)) {
		regex = tmp = estrndup(regex, regex_len);
	}

	zend_hash_update(&PCRE_G(pcre_cache), regex, regex_len+1, (void *)&new_entry,
						sizeof(pcre_cache_entry), (void**)&pce);

	if (tmp) {
		efree(tmp);
	}

	return pce;
}
/* }}} */

/* {{{ pcre_get_compiled_regex
 */
PHPAPI pcre* pcre_get_compiled_regex(char *regex, pcre_extra **extra, int *preg_options TSRMLS_DC)
{
	pcre_cache_entry * pce = pcre_get_compiled_regex_cache(regex, strlen(regex) TSRMLS_CC);

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
PHPAPI pcre* pcre_get_compiled_regex_ex(char *regex, pcre_extra **extra, int *preg_options, int *compile_options TSRMLS_DC)
{
	pcre_cache_entry * pce = pcre_get_compiled_regex_cache(regex, strlen(regex) TSRMLS_CC);
	
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
	zval *match_pair;

	ALLOC_ZVAL(match_pair);
	array_init(match_pair);
	INIT_PZVAL(match_pair);

	/* Add (match, offset) to the return value */
	add_next_index_stringl(match_pair, str, len, 1);
	add_next_index_long(match_pair, offset);
	
	if (name) {
		zval_add_ref(&match_pair);
		zend_hash_update(Z_ARRVAL_P(result), name, strlen(name)+1, &match_pair, sizeof(zval *), NULL);
	}
	zend_hash_next_index_insert(Z_ARRVAL_P(result), &match_pair, sizeof(zval *), NULL);
}
/* }}} */

static void php_do_pcre_match(INTERNAL_FUNCTION_PARAMETERS, int global) /* {{{ */
{
	/* parameters */
	char			 *regex;			/* Regular expression */
	char			 *subject;			/* String to match against */
	int				  regex_len;
	int				  subject_len;
	pcre_cache_entry *pce;				/* Compiled regular expression */
	zval			 *subpats = NULL;	/* Array for subpatterns */
	long			  flags = 0;		/* Match control flags */
	long			  start_offset = 0;	/* Where the new search starts */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|zll", &regex, &regex_len,
							  &subject, &subject_len, &subpats, &flags, &start_offset) == FAILURE) {
		RETURN_FALSE;
	}
	
	/* Compile regex or get it from cache. */
	if ((pce = pcre_get_compiled_regex_cache(regex, regex_len TSRMLS_CC)) == NULL) {
		RETURN_FALSE;
	}

	php_pcre_match_impl(pce, subject, subject_len, return_value, subpats, 
		global, ZEND_NUM_ARGS() >= 4, flags, start_offset TSRMLS_CC);
}
/* }}} */

/* {{{ php_pcre_match_impl() */
PHPAPI void php_pcre_match_impl(pcre_cache_entry *pce, char *subject, int subject_len, zval *return_value,
	zval *subpats, int global, int use_flags, long flags, long start_offset TSRMLS_DC)
{
	zval			*result_set,		/* Holds a set of subpatterns after
										   a global match */
				   **match_sets = NULL;	/* An array of sets of matches for each
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
	int				 i, rc;
	int				 subpats_order;		/* Order of subpattern matches */
	int				 offset_capture;    /* Capture match offsets: yes/no */

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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid flags specified");
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
	extra->match_limit = PCRE_G(backtrack_limit);
	extra->match_limit_recursion = PCRE_G(recursion_limit);

	/* Calculate the size of the offsets array, and allocate memory for it. */
	rc = pcre_fullinfo(pce->re, extra, PCRE_INFO_CAPTURECOUNT, &num_subpats);
	if (rc < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Internal pcre_fullinfo() error %d", rc);
		RETURN_FALSE;
	}
	num_subpats++;
	size_offsets = num_subpats * 3;

	/*
	 * Build a mapping from subpattern numbers to their names. We will always
	 * allocate the table, even though there may be no named subpatterns. This
	 * avoids somewhat more complicated logic in the inner loops.
	 */
	subpat_names = make_subpats_table(num_subpats, pce TSRMLS_CC);
	if (!subpat_names) {
		RETURN_FALSE;
	}

	offsets = (int *)safe_emalloc(size_offsets, sizeof(int), 0);

	/* Allocate match sets array and initialize the values. */
	if (global && subpats && subpats_order == PREG_PATTERN_ORDER) {
		match_sets = (zval **)safe_emalloc(num_subpats, sizeof(zval *), 0);
		for (i=0; i<num_subpats; i++) {
			ALLOC_ZVAL(match_sets[i]);
			array_init(match_sets[i]);
			INIT_PZVAL(match_sets[i]);
		}
	}

	matched = 0;
	PCRE_G(error_code) = PHP_PCRE_NO_ERROR;
	
	do {
		/* Execute the regular expression. */
		count = pcre_exec(pce->re, extra, subject, subject_len, start_offset,
						  exoptions|g_notempty, offsets, size_offsets);

		/* the string was already proved to be valid UTF-8 */
		exoptions |= PCRE_NO_UTF8_CHECK;

		/* Check for too many substrings condition. */
		if (count == 0) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Matched, but too many substrings");
			count = size_offsets/3;
		}

		/* If something has matched */
		if (count > 0) {
			matched++;

			/* If subpatterns array has been passed, fill it in with values. */
			if (subpats != NULL) {
				/* Try to get the list of substrings and display a warning if failed. */
				if (pcre_get_substring_list(subject, offsets, count, &stringlist) < 0) {
					efree(subpat_names);
					efree(offsets);
					if (match_sets) efree(match_sets);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Get subpatterns list failed");
					RETURN_FALSE;
				}

				if (global) {	/* global pattern matching */
					if (subpats && subpats_order == PREG_PATTERN_ORDER) {
						/* For each subpattern, insert it into the appropriate array. */
						for (i = 0; i < count; i++) {
							if (offset_capture) {
								add_offset_pair(match_sets[i], (char *)stringlist[i],
												offsets[(i<<1)+1] - offsets[i<<1], offsets[i<<1], NULL);
							} else {
								add_next_index_stringl(match_sets[i], (char *)stringlist[i],
													   offsets[(i<<1)+1] - offsets[i<<1], 1);
							}
						}
						/*
						 * If the number of captured subpatterns on this run is
						 * less than the total possible number, pad the result
						 * arrays with empty strings.
						 */
						if (count < num_subpats) {
							for (; i < num_subpats; i++) {
								add_next_index_string(match_sets[i], "", 1);
							}
						}
					} else {
						/* Allocate the result set array */
						ALLOC_ZVAL(result_set);
						array_init(result_set);
						INIT_PZVAL(result_set);
						
						/* Add all the subpatterns to it */
						for (i = 0; i < count; i++) {
							if (offset_capture) {
								add_offset_pair(result_set, (char *)stringlist[i],
												offsets[(i<<1)+1] - offsets[i<<1], offsets[i<<1], subpat_names[i]);
							} else {
								if (subpat_names[i]) {
									add_assoc_stringl(result_set, subpat_names[i], (char *)stringlist[i],
														   offsets[(i<<1)+1] - offsets[i<<1], 1);
								}
								add_next_index_stringl(result_set, (char *)stringlist[i],
													   offsets[(i<<1)+1] - offsets[i<<1], 1);
							}
						}
						/* And add it to the output array */
						zend_hash_next_index_insert(Z_ARRVAL_P(subpats), &result_set, sizeof(zval *), NULL);
					}
				} else {			/* single pattern matching */
					/* For each subpattern, insert it into the subpatterns array. */
					for (i = 0; i < count; i++) {
						if (offset_capture) {
							add_offset_pair(subpats, (char *)stringlist[i],
											offsets[(i<<1)+1] - offsets[i<<1],
											offsets[i<<1], subpat_names[i]);
						} else {
							if (subpat_names[i]) {
								add_assoc_stringl(subpats, subpat_names[i], (char *)stringlist[i],
												  offsets[(i<<1)+1] - offsets[i<<1], 1);
							}
							add_next_index_stringl(subpats, (char *)stringlist[i],
												   offsets[(i<<1)+1] - offsets[i<<1], 1);
						}
					}
				}

				pcre_free((void *) stringlist);
			}
		} else if (count == PCRE_ERROR_NOMATCH) {
			/* If we previously set PCRE_NOTEMPTY after a null match,
			   this is not necessarily the end. We need to advance
			   the start offset, and continue. Fudge the offset values
			   to achieve this, unless we're already at the end of the string. */
			if (g_notempty != 0 && start_offset < subject_len) {
				offsets[0] = start_offset;
				offsets[1] = start_offset + 1;
			} else
				break;
		} else {
			pcre_handle_exec_error(count TSRMLS_CC);
			break;
		}
		
		/* If we have matched an empty string, mimic what Perl's /g options does.
		   This turns out to be rather cunning. First we set PCRE_NOTEMPTY and try
		   the match again at the same point. If this fails (picked up above) we
		   advance to the next character. */
		g_notempty = (offsets[1] == offsets[0])? PCRE_NOTEMPTY | PCRE_ANCHORED : 0;
		
		/* Advance to the position right after the last full match */
		start_offset = offsets[1];
	} while (global);

	/* Add the match sets to the output array and clean up */
	if (global && subpats && subpats_order == PREG_PATTERN_ORDER) {
		for (i = 0; i < num_subpats; i++) {
			if (subpat_names[i]) {
				zend_hash_update(Z_ARRVAL_P(subpats), subpat_names[i],
								 strlen(subpat_names[i])+1, &match_sets[i], sizeof(zval *), NULL);
				Z_ADDREF_P(match_sets[i]);
			}
			zend_hash_next_index_insert(Z_ARRVAL_P(subpats), &match_sets[i], sizeof(zval *), NULL);
		}
		efree(match_sets);
	}
	
	efree(offsets);
	efree(subpat_names);

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
static int preg_do_repl_func(zval *function, char *subject, int *offsets, char **subpat_names, int count, char **result TSRMLS_DC)
{
	zval		*retval_ptr;		/* Function return value */
	zval	   **args[1];			/* Argument to pass to function */
	zval		*subpats;			/* Captured subpatterns */ 
	int			 result_len;		/* Return value length */
	int			 i;

	MAKE_STD_ZVAL(subpats);
	array_init(subpats);
	for (i = 0; i < count; i++) {
		if (subpat_names[i]) {
			add_assoc_stringl(subpats, subpat_names[i], &subject[offsets[i<<1]] , offsets[(i<<1)+1] - offsets[i<<1], 1);
		}
		add_next_index_stringl(subpats, &subject[offsets[i<<1]], offsets[(i<<1)+1] - offsets[i<<1], 1);
	}
	args[0] = &subpats;

	if (call_user_function_ex(EG(function_table), NULL, function, &retval_ptr, 1, args, 0, NULL TSRMLS_CC) == SUCCESS && retval_ptr) {
		convert_to_string_ex(&retval_ptr);
		*result = estrndup(Z_STRVAL_P(retval_ptr), Z_STRLEN_P(retval_ptr));
		result_len = Z_STRLEN_P(retval_ptr);
		zval_ptr_dtor(&retval_ptr);
	} else {
		if (!EG(exception)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call custom replacement function");
		}
		result_len = offsets[1] - offsets[0];
		*result = estrndup(&subject[offsets[0]], result_len);
	}

	zval_ptr_dtor(&subpats);

	return result_len;
}
/* }}} */

/* {{{ preg_do_eval
 */
static int preg_do_eval(char *eval_str, int eval_str_len, char *subject,
						int *offsets, int count, char **result TSRMLS_DC)
{
	zval		 retval;			/* Return value from evaluation */
	char		*eval_str_end,		/* End of eval string */
				*match,				/* Current match for a backref */
				*esc_match,			/* Quote-escaped match */
				*walk,				/* Used to walk the code string */
				*segment,			/* Start of segment to append while walking */
				 walk_last;			/* Last walked character */
	int			 match_len;			/* Length of the match */
	int			 esc_match_len;		/* Length of the quote-escaped match */
	int			 result_len;		/* Length of the result of the evaluation */
	int			 backref;			/* Current backref */
	char        *compiled_string_description;
	smart_str    code = {0};
	
	eval_str_end = eval_str + eval_str_len;
	walk = segment = eval_str;
	walk_last = 0;
	
	while (walk < eval_str_end) {
		/* If found a backreference.. */
		if ('\\' == *walk || '$' == *walk) {
			smart_str_appendl(&code, segment, walk - segment);
			if (walk_last == '\\') {
				code.c[code.len-1] = *walk++;
				segment = walk;
				walk_last = 0;
				continue;
			}
			segment = walk;
			if (preg_get_backref(&walk, &backref)) {
				if (backref < count) {
					/* Find the corresponding string match and substitute it
					   in instead of the backref */
					match = subject + offsets[backref<<1];
					match_len = offsets[(backref<<1)+1] - offsets[backref<<1];
					if (match_len) {
						esc_match = php_addslashes(match, match_len, &esc_match_len, 0 TSRMLS_CC);
					} else {
						esc_match = match;
						esc_match_len = 0;
					}
				} else {
					esc_match = "";
					esc_match_len = 0;
				}
				smart_str_appendl(&code, esc_match, esc_match_len);

				segment = walk;

				/* Clean up and reassign */
				if (esc_match_len)
					efree(esc_match);
				continue;
			}
		}
		walk++;
		walk_last = walk[-1];
	}
	smart_str_appendl(&code, segment, walk - segment);
	smart_str_0(&code);

	compiled_string_description = zend_make_compiled_string_description("regexp code" TSRMLS_CC);
	/* Run the code */
	if (zend_eval_stringl(code.c, code.len, &retval, compiled_string_description TSRMLS_CC) == FAILURE) {
		efree(compiled_string_description);
		php_error_docref(NULL TSRMLS_CC,E_ERROR, "Failed evaluating code: %s%s", PHP_EOL, code.c);
		/* zend_error() does not return in this case */
	}
	efree(compiled_string_description);
	convert_to_string(&retval);
	
	/* Save the return value and its length */
	*result = estrndup(Z_STRVAL(retval), Z_STRLEN(retval));
	result_len = Z_STRLEN(retval);
	
	/* Clean up */
	zval_dtor(&retval);
	smart_str_free(&code);
	
	return result_len;
}
/* }}} */

/* {{{ php_pcre_replace
 */
PHPAPI char *php_pcre_replace(char *regex,   int regex_len,
							  char *subject, int subject_len,
							  zval *replace_val, int is_callable_replace,
							  int *result_len, int limit, int *replace_count TSRMLS_DC)
{
	pcre_cache_entry	*pce;			    /* Compiled regular expression */

	/* Compile regex or get it from cache. */
	if ((pce = pcre_get_compiled_regex_cache(regex, regex_len TSRMLS_CC)) == NULL) {
		return NULL;
	}

	return php_pcre_replace_impl(pce, subject, subject_len, replace_val, 
		is_callable_replace, result_len, limit, replace_count TSRMLS_CC);
}
/* }}} */

/* {{{ php_pcre_replace_impl() */
PHPAPI char *php_pcre_replace_impl(pcre_cache_entry *pce, char *subject, int subject_len, zval *replace_val, 
	int is_callable_replace, int *result_len, int limit, int *replace_count TSRMLS_DC)
{
	pcre_extra		*extra = pce->extra;/* Holds results of studying */
	pcre_extra		 extra_data;		/* Used locally for exec options */
	int				 exoptions = 0;		/* Execution options */
	int				 count = 0;			/* Count of matched subpatterns */
	int				*offsets;			/* Array of subpattern offsets */
	char 			**subpat_names;		/* Array for named subpatterns */
	int				 num_subpats;		/* Number of captured subpatterns */
	int				 size_offsets;		/* Size of the offsets array */
	int				 new_len;			/* Length of needed storage */
	int				 alloc_len;			/* Actual allocated length */
	int				 eval_result_len=0;	/* Length of the eval'ed or
										   function-returned string */
	int				 match_len;			/* Length of the current match */
	int				 backref;			/* Backreference number */
	int				 eval;				/* If the replacement string should be eval'ed */
	int				 start_offset;		/* Where the new search starts */
	int				 g_notempty=0;		/* If the match should not be empty */
	int				 replace_len=0;		/* Length of replacement string */
	char			*result,			/* Result of replacement */
					*replace=NULL,		/* Replacement string */
					*new_buf,			/* Temporary buffer for re-allocation */
					*walkbuf,			/* Location of current replacement in the result */
					*walk,				/* Used to walk the replacement string */
					*match,				/* The current match */
					*piece,				/* The current piece of subject */
					*replace_end=NULL,	/* End of replacement string */
					*eval_result,		/* Result of eval or custom function */
					 walk_last;			/* Last walked character */
	int				 rc;

	if (extra == NULL) {
		extra_data.flags = PCRE_EXTRA_MATCH_LIMIT | PCRE_EXTRA_MATCH_LIMIT_RECURSION;
		extra = &extra_data;
	}
	extra->match_limit = PCRE_G(backtrack_limit);
	extra->match_limit_recursion = PCRE_G(recursion_limit);

	eval = pce->preg_options & PREG_REPLACE_EVAL;
	if (is_callable_replace) {
		if (eval) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Modifier /e cannot be used with replacement callback");
			return NULL;
		}
	} else {
		replace = Z_STRVAL_P(replace_val);
		replace_len = Z_STRLEN_P(replace_val);
		replace_end = replace + replace_len;
	}

	if (eval) {
		php_error_docref(NULL TSRMLS_CC, E_DEPRECATED, "The /e modifier is deprecated, use preg_replace_callback instead");
	}

	/* Calculate the size of the offsets array, and allocate memory for it. */
	rc = pcre_fullinfo(pce->re, extra, PCRE_INFO_CAPTURECOUNT, &num_subpats);
	if (rc < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Internal pcre_fullinfo() error %d", rc);
		return NULL;
	}
	num_subpats++;
	size_offsets = num_subpats * 3;

	/*
	 * Build a mapping from subpattern numbers to their names. We will always
	 * allocate the table, even though there may be no named subpatterns. This
	 * avoids somewhat more complicated logic in the inner loops.
	 */
	subpat_names = make_subpats_table(num_subpats, pce TSRMLS_CC);
	if (!subpat_names) {
		return NULL;
	}

	offsets = (int *)safe_emalloc(size_offsets, sizeof(int), 0);
	
	alloc_len = 2 * subject_len + 1;
	result = safe_emalloc(alloc_len, sizeof(char), 0);

	/* Initialize */
	match = NULL;
	*result_len = 0;
	start_offset = 0;
	PCRE_G(error_code) = PHP_PCRE_NO_ERROR;
	
	while (1) {
		/* Execute the regular expression. */
		count = pcre_exec(pce->re, extra, subject, subject_len, start_offset,
						  exoptions|g_notempty, offsets, size_offsets);

		/* the string was already proved to be valid UTF-8 */
		exoptions |= PCRE_NO_UTF8_CHECK;

		/* Check for too many substrings condition. */
		if (count == 0) {
			php_error_docref(NULL TSRMLS_CC,E_NOTICE, "Matched, but too many substrings");
			count = size_offsets/3;
		}

		piece = subject + start_offset;

		if (count > 0 && (limit == -1 || limit > 0)) {
			if (replace_count) {
				++*replace_count;
			}
			/* Set the match location in subject */
			match = subject + offsets[0];

			new_len = *result_len + offsets[0] - start_offset; /* part before the match */
			
			/* If evaluating, do it and add the return string's length */
			if (eval) {
				eval_result_len = preg_do_eval(replace, replace_len, subject,
											   offsets, count, &eval_result TSRMLS_CC);
				new_len += eval_result_len;
			} else if (is_callable_replace) {
				/* Use custom function to get replacement string and its length. */
				eval_result_len = preg_do_repl_func(replace_val, subject, offsets, subpat_names, count, &eval_result TSRMLS_CC);
				new_len += eval_result_len;
			} else { /* do regular substitution */
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
			}

			if (new_len + 1 > alloc_len) {
				alloc_len = 1 + alloc_len + 2 * new_len;
				new_buf = emalloc(alloc_len);
				memcpy(new_buf, result, *result_len);
				efree(result);
				result = new_buf;
			}
			/* copy the part of the string before the match */
			memcpy(&result[*result_len], piece, match-piece);
			*result_len += match-piece;

			/* copy replacement and backrefs */
			walkbuf = result + *result_len;
			
			/* If evaluating or using custom function, copy result to the buffer
			 * and clean up. */
			if (eval || is_callable_replace) {
				memcpy(walkbuf, eval_result, eval_result_len);
				*result_len += eval_result_len;
				STR_FREE(eval_result);
			} else { /* do regular backreference copying */
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
				*result_len += walkbuf - (result + *result_len);
			}

			if (limit != -1)
				limit--;

		} else if (count == PCRE_ERROR_NOMATCH || limit == 0) {
			/* If we previously set PCRE_NOTEMPTY after a null match,
			   this is not necessarily the end. We need to advance
			   the start offset, and continue. Fudge the offset values
			   to achieve this, unless we're already at the end of the string. */
			if (g_notempty != 0 && start_offset < subject_len) {
				offsets[0] = start_offset;
				offsets[1] = start_offset + 1;
				memcpy(&result[*result_len], piece, 1);
				(*result_len)++;
			} else {
				new_len = *result_len + subject_len - start_offset;
				if (new_len + 1 > alloc_len) {
					alloc_len = new_len + 1; /* now we know exactly how long it is */
					new_buf = safe_emalloc(alloc_len, sizeof(char), 0);
					memcpy(new_buf, result, *result_len);
					efree(result);
					result = new_buf;
				}
				/* stick that last bit of string on our output */
				memcpy(&result[*result_len], piece, subject_len - start_offset);
				*result_len += subject_len - start_offset;
				result[*result_len] = '\0';
				break;
			}
		} else {
			pcre_handle_exec_error(count TSRMLS_CC);
			efree(result);
			result = NULL;
			break;
		}
			
		/* If we have matched an empty string, mimic what Perl's /g options does.
		   This turns out to be rather cunning. First we set PCRE_NOTEMPTY and try
		   the match again at the same point. If this fails (picked up above) we
		   advance to the next character. */
		g_notempty = (offsets[1] == offsets[0])? PCRE_NOTEMPTY | PCRE_ANCHORED : 0;
		
		/* Advance to the next piece. */
		start_offset = offsets[1];
	}

	efree(offsets);
	efree(subpat_names);

	return result;
}
/* }}} */

/* {{{ php_replace_in_subject
 */
static char *php_replace_in_subject(zval *regex, zval *replace, zval **subject, int *result_len, int limit, int is_callable_replace, int *replace_count TSRMLS_DC)
{
	zval		**regex_entry,
				**replace_entry = NULL,
				 *replace_value,
				  empty_replace;
	char		*subject_value,
				*result;
	int			 subject_len;

	/* Make sure we're dealing with strings. */	
	convert_to_string_ex(subject);
	/* FIXME: This might need to be changed to STR_EMPTY_ALLOC(). Check if this zval could be dtor()'ed somehow */
	ZVAL_STRINGL(&empty_replace, "", 0, 0);
	
	/* If regex is an array */
	if (Z_TYPE_P(regex) == IS_ARRAY) {
		/* Duplicate subject string for repeated replacement */
		subject_value = estrndup(Z_STRVAL_PP(subject), Z_STRLEN_PP(subject));
		subject_len = Z_STRLEN_PP(subject);
		*result_len = subject_len;
		
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(regex));

		replace_value = replace;
		if (Z_TYPE_P(replace) == IS_ARRAY && !is_callable_replace)
			zend_hash_internal_pointer_reset(Z_ARRVAL_P(replace));

		/* For each entry in the regex array, get the entry */
		while (zend_hash_get_current_data(Z_ARRVAL_P(regex), (void **)&regex_entry) == SUCCESS) {
			/* Make sure we're dealing with strings. */	
			convert_to_string_ex(regex_entry);
		
			/* If replace is an array and not a callable construct */
			if (Z_TYPE_P(replace) == IS_ARRAY && !is_callable_replace) {
				/* Get current entry */
				if (zend_hash_get_current_data(Z_ARRVAL_P(replace), (void **)&replace_entry) == SUCCESS) {
					if (!is_callable_replace) {
						convert_to_string_ex(replace_entry);
					}
					replace_value = *replace_entry;
					zend_hash_move_forward(Z_ARRVAL_P(replace));
				} else {
					/* We've run out of replacement strings, so use an empty one */
					replace_value = &empty_replace;
				}
			}
			
			/* Do the actual replacement and put the result back into subject_value
			   for further replacements. */
			if ((result = php_pcre_replace(Z_STRVAL_PP(regex_entry),
										   Z_STRLEN_PP(regex_entry),
										   subject_value,
										   subject_len,
										   replace_value,
										   is_callable_replace,
										   result_len,
										   limit,
										   replace_count TSRMLS_CC)) != NULL) {
				efree(subject_value);
				subject_value = result;
				subject_len = *result_len;
			} else {
				efree(subject_value);
				return NULL;
			}

			zend_hash_move_forward(Z_ARRVAL_P(regex));
		}

		return subject_value;
	} else {
		result = php_pcre_replace(Z_STRVAL_P(regex),
								  Z_STRLEN_P(regex),
								  Z_STRVAL_PP(subject),
								  Z_STRLEN_PP(subject),
								  replace,
								  is_callable_replace,
								  result_len,
								  limit,
								  replace_count TSRMLS_CC);
		return result;
	}
}
/* }}} */

/* {{{ preg_replace_impl
 */
static void preg_replace_impl(INTERNAL_FUNCTION_PARAMETERS, int is_callable_replace, int is_filter)
{
	zval		   **regex,
				   **replace,
				   **subject,
				   **subject_entry,
				   **zcount = NULL;
	char			*result;
	int				 result_len;
	int				 limit_val = -1;
	long			limit = -1;
	char			*string_key;
	uint			 string_key_len;
	ulong			 num_key;
	char			*callback_name;
	int				 replace_count=0, old_replace_count;
	
	/* Get function parameters and do error-checking. */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ZZZ|lZ", &regex, &replace, &subject, &limit, &zcount) == FAILURE) {
		return;
	}
	
	if (!is_callable_replace && Z_TYPE_PP(replace) == IS_ARRAY && Z_TYPE_PP(regex) != IS_ARRAY) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Parameter mismatch, pattern is a string while replacement is an array");
		RETURN_FALSE;
	}

	SEPARATE_ZVAL(replace);
	if (Z_TYPE_PP(replace) != IS_ARRAY && (Z_TYPE_PP(replace) != IS_OBJECT || !is_callable_replace)) {
		convert_to_string_ex(replace);
	}
	if (is_callable_replace) {
		if (!zend_is_callable(*replace, 0, &callback_name TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Requires argument 2, '%s', to be a valid callback", callback_name);
			efree(callback_name);
			MAKE_COPY_ZVAL(subject, return_value);
			return;
		}
		efree(callback_name);
	}

	SEPARATE_ZVAL(regex);
	SEPARATE_ZVAL(subject);

	if (ZEND_NUM_ARGS() > 3) {
		limit_val = limit;
	}
		
	if (Z_TYPE_PP(regex) != IS_ARRAY)
		convert_to_string_ex(regex);
	
	/* if subject is an array */
	if (Z_TYPE_PP(subject) == IS_ARRAY) {
		array_init(return_value);
		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(subject));

		/* For each subject entry, convert it to string, then perform replacement
		   and add the result to the return_value array. */
		while (zend_hash_get_current_data(Z_ARRVAL_PP(subject), (void **)&subject_entry) == SUCCESS) {
			SEPARATE_ZVAL(subject_entry);
			old_replace_count = replace_count;
			if ((result = php_replace_in_subject(*regex, *replace, subject_entry, &result_len, limit_val, is_callable_replace, &replace_count TSRMLS_CC)) != NULL) {
				if (!is_filter || replace_count > old_replace_count) {
					/* Add to return array */
					switch(zend_hash_get_current_key_ex(Z_ARRVAL_PP(subject), &string_key, &string_key_len, &num_key, 0, NULL))
					{
					case HASH_KEY_IS_STRING:
						add_assoc_stringl_ex(return_value, string_key, string_key_len, result, result_len, 0);
						break;

					case HASH_KEY_IS_LONG:
						add_index_stringl(return_value, num_key, result, result_len, 0);
						break;
					}
				} else {
					efree(result);
				}
			}
		
			zend_hash_move_forward(Z_ARRVAL_PP(subject));
		}
	} else {	/* if subject is not an array */
		old_replace_count = replace_count;
		if ((result = php_replace_in_subject(*regex, *replace, subject, &result_len, limit_val, is_callable_replace, &replace_count TSRMLS_CC)) != NULL) {
			if (!is_filter || replace_count > old_replace_count) {
				RETVAL_STRINGL(result, result_len, 0);
			} else {
				efree(result);
			}
		}
	}
	if (ZEND_NUM_ARGS() > 4) {
		zval_dtor(*zcount);
		ZVAL_LONG(*zcount, replace_count);
	}
    	
}
/* }}} */

/* {{{ proto mixed preg_replace(mixed regex, mixed replace, mixed subject [, int limit [, int &count]])
   Perform Perl-style regular expression replacement. */
static PHP_FUNCTION(preg_replace)
{
	preg_replace_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 0);
}
/* }}} */

/* {{{ proto mixed preg_replace_callback(mixed regex, mixed callback, mixed subject [, int limit [, int &count]])
   Perform Perl-style regular expression replacement using replacement callback. */
static PHP_FUNCTION(preg_replace_callback)
{
	preg_replace_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1, 0);
}
/* }}} */

/* {{{ proto mixed preg_filter(mixed regex, mixed replace, mixed subject [, int limit [, int &count]])
   Perform Perl-style regular expression replacement and only return matches. */
static PHP_FUNCTION(preg_filter)
{
	preg_replace_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 1);
}
/* }}} */

/* {{{ proto array preg_split(string pattern, string subject [, int limit [, int flags]]) 
   Split string into an array using a perl-style regular expression as a delimiter */
static PHP_FUNCTION(preg_split)
{
	char				*regex;			/* Regular expression */
	char				*subject;		/* String to match against */
	int					 regex_len;
	int					 subject_len;
	long				 limit_val = -1;/* Integer value of limit */
	long				 flags = 0;		/* Match control flags */
	pcre_cache_entry	*pce;			/* Compiled regular expression */

	/* Get function parameters and do error checking */	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ll", &regex, &regex_len,
							  &subject, &subject_len, &limit_val, &flags) == FAILURE) {
		RETURN_FALSE;
	}
	
	/* Compile regex or get it from cache. */
	if ((pce = pcre_get_compiled_regex_cache(regex, regex_len TSRMLS_CC)) == NULL) {
		RETURN_FALSE;
	}

	php_pcre_split_impl(pce, subject, subject_len, return_value, limit_val, flags TSRMLS_CC);
}
/* }}} */

/* {{{ php_pcre_split
 */
PHPAPI void php_pcre_split_impl(pcre_cache_entry *pce, char *subject, int subject_len, zval *return_value,
	long limit_val, long flags TSRMLS_DC)
{
	pcre_extra		*extra = NULL;		/* Holds results of studying */
	pcre			*re_bump = NULL;	/* Regex instance for empty matches */
	pcre_extra		*extra_bump = NULL;	/* Almost dummy */
	pcre_extra		 extra_data;		/* Used locally for exec options */
	int				*offsets;			/* Array of subpattern offsets */
	int				 size_offsets;		/* Size of the offsets array */
	int				 exoptions = 0;		/* Execution options */
	int				 count = 0;			/* Count of matched subpatterns */
	int				 start_offset;		/* Where the new search starts */
	int				 next_offset;		/* End of the last delimiter match + 1 */
	int				 g_notempty = 0;	/* If the match should not be empty */
	char			*last_match;		/* Location of last match */
	int				 rc;
	int				 no_empty;			/* If NO_EMPTY flag is set */
	int				 delim_capture; 	/* If delimiters should be captured */
	int				 offset_capture;	/* If offsets should be captured */

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
	extra->match_limit = PCRE_G(backtrack_limit);
	extra->match_limit_recursion = PCRE_G(recursion_limit);
	
	/* Initialize return value */
	array_init(return_value);

	/* Calculate the size of the offsets array, and allocate memory for it. */
	rc = pcre_fullinfo(pce->re, extra, PCRE_INFO_CAPTURECOUNT, &size_offsets);
	if (rc < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Internal pcre_fullinfo() error %d", rc);
		RETURN_FALSE;
	}
	size_offsets = (size_offsets + 1) * 3;
	offsets = (int *)safe_emalloc(size_offsets, sizeof(int), 0);
	
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
			php_error_docref(NULL TSRMLS_CC,E_NOTICE, "Matched, but too many substrings");
			count = size_offsets/3;
		}
				
		/* If something matched */
		if (count > 0) {
			if (!no_empty || &subject[offsets[0]] != last_match) {

				if (offset_capture) {
					/* Add (match, offset) pair to the return value */
					add_offset_pair(return_value, last_match, &subject[offsets[0]]-last_match, next_offset, NULL);
				} else {
					/* Add the piece to the return value */
					add_next_index_stringl(return_value, last_match,
								   	   &subject[offsets[0]]-last_match, 1);
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
							add_next_index_stringl(return_value,
												   &subject[offsets[i<<1]],
												   match_len, 1);
						}
					}
				}
			}
		} else if (count == PCRE_ERROR_NOMATCH) {
			/* If we previously set PCRE_NOTEMPTY after a null match,
			   this is not necessarily the end. We need to advance
			   the start offset, and continue. Fudge the offset values
			   to achieve this, unless we're already at the end of the string. */
			if (g_notempty != 0 && start_offset < subject_len) {
				if (pce->compile_options & PCRE_UTF8) {
					if (re_bump == NULL) {
						int dummy;

						if ((re_bump = pcre_get_compiled_regex("/./us", &extra_bump, &dummy TSRMLS_CC)) == NULL) {
							RETURN_FALSE;
						}
					}
					count = pcre_exec(re_bump, extra_bump, subject,
							  subject_len, start_offset,
							  exoptions, offsets, size_offsets);
					if (count < 1) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown error");
						RETURN_FALSE;
					}
				} else {
					offsets[0] = start_offset;
					offsets[1] = start_offset + 1;
				}
			} else
				break;
		} else {
			pcre_handle_exec_error(count TSRMLS_CC);
			break;
		}

		/* If we have matched an empty string, mimic what Perl's /g options does.
		   This turns out to be rather cunning. First we set PCRE_NOTEMPTY and try
		   the match again at the same point. If this fails (picked up above) we
		   advance to the next character. */
		g_notempty = (offsets[1] == offsets[0])? PCRE_NOTEMPTY | PCRE_ANCHORED : 0;
		
		/* Advance to the position right after the last full match */
		start_offset = offsets[1];
	}


	start_offset = last_match - subject; /* the offset might have been incremented, but without further successful matches */

	if (!no_empty || start_offset < subject_len)
	{
		if (offset_capture) {
			/* Add the last (match, offset) pair to the return value */
			add_offset_pair(return_value, &subject[start_offset], subject_len - start_offset, start_offset, NULL);
		} else {
			/* Add the last piece to the return value */
			add_next_index_stringl(return_value, last_match, subject + subject_len - last_match, 1);
		}
	}

	
	/* Clean up */
	efree(offsets);
}
/* }}} */

/* {{{ proto string preg_quote(string str [, string delim_char])
   Quote regular expression characters plus an optional character */
static PHP_FUNCTION(preg_quote)
{
	int		 in_str_len;
	char	*in_str;		/* Input string argument */
	char	*in_str_end;    /* End of the input string */
	int		 delim_len = 0;
	char	*delim = NULL;	/* Additional delimiter argument */
	char	*out_str,		/* Output string with quoted characters */
		 	*p,				/* Iterator for input string */
			*q,				/* Iterator for output string */
			 delim_char=0,	/* Delimiter character to be quoted */
			 c;				/* Current character */
	zend_bool quote_delim = 0; /* Whether to quote additional delim char */
	
	/* Get the arguments and check for errors */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &in_str, &in_str_len,
							  &delim, &delim_len) == FAILURE) {
		return;
	}
	
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
	out_str = safe_emalloc(4, in_str_len, 1);
	
	/* Go through the string and quote necessary characters */
	for(p = in_str, q = out_str; p != in_str_end; p++) {
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
	RETVAL_STRINGL(erealloc(out_str, q - out_str + 1), q - out_str, 0);
}
/* }}} */

/* {{{ proto array preg_grep(string regex, array input [, int flags])
   Searches array and returns entries which match regex */
static PHP_FUNCTION(preg_grep)
{
	char				*regex;			/* Regular expression */
	int				 	 regex_len;
	zval				*input;			/* Input array */
	long				 flags = 0;		/* Match control flags */
	pcre_cache_entry	*pce;			/* Compiled regular expression */

	/* Get arguments and do error checking */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa|l", &regex, &regex_len,
							  &input, &flags) == FAILURE) {
		return;
	}
	
	/* Compile regex or get it from cache. */
	if ((pce = pcre_get_compiled_regex_cache(regex, regex_len TSRMLS_CC)) == NULL) {
		RETURN_FALSE;
	}
	
	php_pcre_grep_impl(pce, input, return_value, flags TSRMLS_CC);
}
/* }}} */

PHPAPI void  php_pcre_grep_impl(pcre_cache_entry *pce, zval *input, zval *return_value, long flags TSRMLS_DC) /* {{{ */
{
	zval		   **entry;				/* An entry in the input array */
	pcre_extra		*extra = pce->extra;/* Holds results of studying */
	pcre_extra		 extra_data;		/* Used locally for exec options */
	int				*offsets;			/* Array of subpattern offsets */
	int				 size_offsets;		/* Size of the offsets array */
	int				 count = 0;			/* Count of matched subpatterns */
	char			*string_key;
	uint			 string_key_len;
	ulong			 num_key;
	zend_bool		 invert;			/* Whether to return non-matching
										   entries */
	int				 rc;
	
	invert = flags & PREG_GREP_INVERT ? 1 : 0;
	
	if (extra == NULL) {
		extra_data.flags = PCRE_EXTRA_MATCH_LIMIT | PCRE_EXTRA_MATCH_LIMIT_RECURSION;
		extra = &extra_data;
	}
	extra->match_limit = PCRE_G(backtrack_limit);
	extra->match_limit_recursion = PCRE_G(recursion_limit);

	/* Calculate the size of the offsets array, and allocate memory for it. */
	rc = pcre_fullinfo(pce->re, extra, PCRE_INFO_CAPTURECOUNT, &size_offsets);
	if (rc < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Internal pcre_fullinfo() error %d", rc);
		RETURN_FALSE;
	}
	size_offsets = (size_offsets + 1) * 3;
	offsets = (int *)safe_emalloc(size_offsets, sizeof(int), 0);
	
	/* Initialize return array */
	array_init(return_value);

	PCRE_G(error_code) = PHP_PCRE_NO_ERROR;

	/* Go through the input array */
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(input));
	while (zend_hash_get_current_data(Z_ARRVAL_P(input), (void **)&entry) == SUCCESS) {
		zval subject = **entry;

		if (Z_TYPE_PP(entry) != IS_STRING) {
			zval_copy_ctor(&subject);
			convert_to_string(&subject);
		}

		/* Perform the match */
		count = pcre_exec(pce->re, extra, Z_STRVAL(subject),
						  Z_STRLEN(subject), 0,
						  0, offsets, size_offsets);

		/* Check for too many substrings condition. */
		if (count == 0) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Matched, but too many substrings");
			count = size_offsets/3;
		} else if (count < 0 && count != PCRE_ERROR_NOMATCH) {
			pcre_handle_exec_error(count TSRMLS_CC);
			break;
		}

		/* If the entry fits our requirements */
		if ((count > 0 && !invert) || (count == PCRE_ERROR_NOMATCH && invert)) {

			Z_ADDREF_PP(entry);

			/* Add to return array */
			switch (zend_hash_get_current_key_ex(Z_ARRVAL_P(input), &string_key, &string_key_len, &num_key, 0, NULL))
			{
				case HASH_KEY_IS_STRING:
					zend_hash_update(Z_ARRVAL_P(return_value), string_key,
									 string_key_len, entry, sizeof(zval *), NULL);
					break;

				case HASH_KEY_IS_LONG:
					zend_hash_index_update(Z_ARRVAL_P(return_value), num_key, entry,
										   sizeof(zval *), NULL);
					break;
			}
		}

		if (Z_TYPE_PP(entry) != IS_STRING) {
			zval_dtor(&subject);
		}

		zend_hash_move_forward(Z_ARRVAL_P(input));
	}
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(input));
	/* Clean up */
	efree(offsets);
}
/* }}} */

/* {{{ proto int preg_last_error()
   Returns the error code of the last regexp execution. */
static PHP_FUNCTION(preg_last_error)
{
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

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
	PHP_FE(preg_match,				arginfo_preg_match)
	PHP_FE(preg_match_all,			arginfo_preg_match_all)
	PHP_FE(preg_replace,			arginfo_preg_replace)
	PHP_FE(preg_replace_callback,	arginfo_preg_replace_callback)
	PHP_FE(preg_filter,				arginfo_preg_replace)
	PHP_FE(preg_split,				arginfo_preg_split)
	PHP_FE(preg_quote,				arginfo_preg_quote)
	PHP_FE(preg_grep,				arginfo_preg_grep)
	PHP_FE(preg_last_error,			arginfo_preg_last_error)
	PHP_FE_END
};

zend_module_entry pcre_module_entry = {
	STANDARD_MODULE_HEADER,
   "pcre",
	pcre_functions,
	PHP_MINIT(pcre),
	PHP_MSHUTDOWN(pcre),
	NULL,
	NULL,
	PHP_MINFO(pcre),
	NO_VERSION_YET,
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
