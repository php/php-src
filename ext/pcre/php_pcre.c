/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andrei Zmievski <andrei@php.net>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
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


ZEND_DECLARE_MODULE_GLOBALS(pcre)


static void *php_pcre_malloc(size_t size)
{
	return pemalloc(size, 1);
}


static void php_pcre_free(void *ptr)
{
	pefree(ptr, 1);
}


static void php_free_pcre_cache(void *data)
{
	pcre_cache_entry *pce = (pcre_cache_entry *) data;
	pefree(pce->re, 1);
#if HAVE_SETLOCALE
	if ((void*)pce->tables) pefree((void*)pce->tables, 1);
	pefree(pce->locale, 1);
#endif
}


static void php_pcre_init_globals(zend_pcre_globals *pcre_globals TSRMLS_DC)
{
	zend_hash_init(&pcre_globals->pcre_cache, 0, NULL, php_free_pcre_cache, 1);
}

static void php_pcre_shutdown_globals(zend_pcre_globals *pcre_globals TSRMLS_DC)
{
	zend_hash_destroy(&pcre_globals->pcre_cache);
}


static /* {{{ PHP_MINFO_FUNCTION(pcre) */
PHP_MINFO_FUNCTION(pcre)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "PCRE (Perl Compatible Regular Expressions) Support", "enabled" );
	php_info_print_table_row(2, "PCRE Library Version", pcre_version() );
	php_info_print_table_end();
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION(pcre) */
static PHP_MINIT_FUNCTION(pcre)
{
	ZEND_INIT_MODULE_GLOBALS(pcre, php_pcre_init_globals, php_pcre_shutdown_globals);
	
	REGISTER_LONG_CONSTANT("PREG_PATTERN_ORDER", PREG_PATTERN_ORDER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_SET_ORDER", PREG_SET_ORDER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_OFFSET_CAPTURE", PREG_OFFSET_CAPTURE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_SPLIT_NO_EMPTY", PREG_SPLIT_NO_EMPTY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_SPLIT_DELIM_CAPTURE", PREG_SPLIT_DELIM_CAPTURE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_SPLIT_OFFSET_CAPTURE", PREG_SPLIT_OFFSET_CAPTURE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_GREP_INVERT", PREG_GREP_INVERT, CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION(pcre) */
static PHP_MSHUTDOWN_FUNCTION(pcre)
{
#ifndef ZTS
	php_pcre_shutdown_globals(&pcre_globals TSRMLS_CC);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION(pcre) */
static PHP_RINIT_FUNCTION(pcre)
{
	pcre_malloc = php_pcre_malloc;
	pcre_free = php_pcre_free;
	
	return SUCCESS;
}
/* }}} */

/* {{{ pcre_get_compiled_regex
 */
PHPAPI pcre* pcre_get_compiled_regex(char *regex, pcre_extra **extra, int *preg_options) {
	pcre				*re = NULL;
	int				 	 coptions = 0;
	int				 	 soptions = 0;
	const char	 		*error;
	int			 	 	 erroffset;
	char		 	 	 delimiter;
	char				 start_delimiter;
	char				 end_delimiter;
	char 				*p, *pp;
	char				*pattern;
	int				 	 regex_len;
	int				 	 do_study = 0;
	int					 poptions = 0;
	unsigned const char *tables = NULL;
#if HAVE_SETLOCALE
	char				*locale = setlocale(LC_CTYPE, NULL);
#endif
	pcre_cache_entry	*pce;
	pcre_cache_entry	 new_entry;
	TSRMLS_FETCH();

	/* Try to lookup the cached regex entry, and if successful, just pass
	   back the compiled pattern, otherwise go on and compile it. */
	regex_len = strlen(regex);
	if (zend_hash_find(&PCRE_G(pcre_cache), regex, regex_len+1, (void **)&pce) == SUCCESS) {
#if HAVE_SETLOCALE
		if (!strcmp(pce->locale, locale)) {
#endif
			*extra = pce->extra;
			*preg_options = pce->preg_options;
			return pce->re;
#if HAVE_SETLOCALE
		}
#endif
	}
	
	p = regex;
	
	/* Parse through the leading whitespace, and display a warning if we
	   get to the end without encountering a delimiter. */
	while (isspace((int)*p)) p++;
	if (*p == 0) {
		zend_error(E_WARNING, "Empty regular expression");
		return NULL;
	}
	
	/* Get the delimiter and display a warning if it is alphanumeric
	   or a backslash. */
	delimiter = *p++;
	if (isalnum((int)delimiter) || delimiter == '\\') {
		zend_error(E_WARNING, "Delimiter must not be alphanumeric or backslash");
		return NULL;
	}

	start_delimiter = delimiter;
	if ((pp = strchr("([{< )]}> )]}>", delimiter)))
		delimiter = pp[5];
	end_delimiter = delimiter;

	if (start_delimiter == end_delimiter) {
		/* We need to iterate through the pattern, searching for the ending delimiter,
		   but skipping the backslashed delimiters.  If the ending delimiter is not
		   found, display a warning. */
		pp = p;
		while (*pp != 0) {
			if (*pp == '\\' && pp[1] != 0) pp++;
			else if (*pp == delimiter)
				break;
			pp++;
		}
		if (*pp == 0) {
			zend_error(E_WARNING, "No ending delimiter '%c' found", delimiter);
			return NULL;
		}
	} else {
		/* We iterate through the pattern, searching for the matching ending
		 * delimiter. For each matching starting delimiter, we increment nesting
		 * level, and decrement it for each matching ending delimiter. If we
		 * reach the end of the pattern without matching, display a warning.
		 */
		int brackets = 1; 	/* brackets nesting level */
		pp = p;
		while (*pp != 0) {
			if (*pp == '\\' && pp[1] != 0) pp++;
			else if (*pp == end_delimiter && --brackets <= 0)
				break;
			else if (*pp == start_delimiter)
				brackets++;
			pp++;
		}
		if (*pp == 0) {
			zend_error(E_WARNING, "No ending matching delimiter '%c' found", end_delimiter);
			return NULL;
		}
	}
	
	/* Make a copy of the actual pattern. */
	pattern = estrndup(p, pp-p);

	/* Move on to the options */
	pp++;

	/* Clear out preg options */
	*preg_options = 0;
	
	/* Parse through the options, setting appropriate flags.  Display
	   a warning if we encounter an unknown modifier. */	
	while (*pp != 0) {
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
			case 'u':	coptions |= PCRE_UTF8;			break;

			/* Custom preg options */
			case 'e':	poptions |= PREG_REPLACE_EVAL;	break;
			
			case ' ':
			case '\n':
				break;

			default:
				zend_error(E_WARNING, "Unknown modifier '%c'", pp[-1]);
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
		zend_error(E_WARNING, "Compilation failed: %s at offset %d\n", error, erroffset);
		efree(pattern);
		return NULL;
	}

	/* If study option was specified, study the pattern and
	   store the result in extra for passing to pcre_exec. */
	if (do_study) {
		*extra = pcre_study(re, soptions, &error);
		if (error != NULL) {
			zend_error(E_WARNING, "Error while studying pattern");
		}
	}

	*preg_options = poptions;
	
	efree(pattern);

	/* Store the compiled pattern and extra info in the cache. */
	new_entry.re = re;
	new_entry.extra = *extra;
	new_entry.preg_options = poptions;
#if HAVE_SETLOCALE
	new_entry.locale = pestrdup(locale, 1);
	new_entry.tables = tables;
#endif
	zend_hash_update(&PCRE_G(pcre_cache), regex, regex_len+1, (void *)&new_entry,
						sizeof(pcre_cache_entry), NULL);

	return re;
}
/* }}} */

/* {{{ add_offset_pair */
static inline void add_offset_pair(zval *result, char *str, int len, int offset)
{
	zval *match_pair;

	ALLOC_ZVAL(match_pair);
	array_init(match_pair);
	INIT_PZVAL(match_pair);

	/* Add (match, offset) to the return value */
	add_next_index_stringl(match_pair, str, len, 1);
	add_next_index_long(match_pair, offset);
	
	zend_hash_next_index_insert(Z_ARRVAL_P(result), &match_pair, sizeof(zval *), NULL);
}
/* }}} */

/* {{{ php_pcre_match
 */
static void php_pcre_match(INTERNAL_FUNCTION_PARAMETERS, int global)
{
	zval			**regex,			/* Regular expression */
					**subject,			/* String to match against */
					**subpats = NULL,	/* Array for subpatterns */
					**flags,			/* Match control flags */
					*result_set,		/* Holds a set of subpatterns after
										   a global match */
				   **match_sets = NULL;	/* An array of sets of matches for each
										   subpattern after a global match */
	pcre			*re = NULL;			/* Compiled regular expression */
	pcre_extra		*extra = NULL;		/* Holds results of studying */
	int			 	 exoptions = 0;		/* Execution options */
	int			 	 preg_options = 0;	/* Custom preg options */
	int			 	 count = 0;			/* Count of matched subpatterns */
	int			 	*offsets;			/* Array of subpattern offsets */
	int				 num_subpats;		/* Number of captured subpatterns */
	int			 	 size_offsets;		/* Size of the offsets array */
	int				 start_offset;		/* Where the new search starts */
	int			 	 matched;			/* Has anything matched */
	int				 i;
	int				 subpats_order = 0;	/* Order of subpattern matches */
	int				 offset_capture = 0;/* Capture match offsets: yes/no */
	int				 g_notempty = 0;	/* If the match should not be empty */
	const char	   **stringlist;		/* Used to hold list of subpatterns */
	char			*match;				/* The current match */
	
	
	/* Get function parameters and do error-checking. */
	switch(ZEND_NUM_ARGS()) {
		case 2:
			if (global || zend_get_parameters_ex(2, &regex, &subject) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
			
		case 3:
			if (zend_get_parameters_ex(3, &regex, &subject, &subpats) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			if (global)
				subpats_order = PREG_PATTERN_ORDER;
			break;

		case 4:
			if (zend_get_parameters_ex(4, &regex, &subject, &subpats, &flags) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
	
			convert_to_long_ex(flags);
			offset_capture = Z_LVAL_PP(flags) & PREG_OFFSET_CAPTURE;
			subpats_order = Z_LVAL_PP(flags) & 0xff;
			if ((global && (subpats_order < PREG_PATTERN_ORDER || subpats_order > PREG_SET_ORDER)) ||
				(!global && subpats_order != 0)) {
				zend_error(E_WARNING, "Wrong value for parameter 4 in call to %s()", get_active_function_name(TSRMLS_C));
				return;
			}
			break;
			
		default:
			WRONG_PARAM_COUNT;
	}

	/* Make sure we're dealing with strings. */
	convert_to_string_ex(regex);
	convert_to_string_ex(subject);

	/* Make sure to clean up the passed array and initialize it. */
	if (subpats != NULL) {
		zval_dtor(*subpats);
		array_init(*subpats);
	}

	/* Compile regex or get it from cache. */
	if ((re = pcre_get_compiled_regex(Z_STRVAL_PP(regex), &extra, &preg_options)) == NULL) {
		RETURN_FALSE;
	}

	/* Calculate the size of the offsets array, and allocate memory for it. */
	pcre_fullinfo(re, extra, PCRE_INFO_CAPTURECOUNT, &num_subpats);
	num_subpats++;
	size_offsets = num_subpats * 3;
	offsets = (int *)emalloc(size_offsets * sizeof(int));

	/* Allocate match sets array and initialize the values */
	if (global && subpats_order == PREG_PATTERN_ORDER) {
		match_sets = (zval **)emalloc(num_subpats * sizeof(zval *));
		for (i=0; i<num_subpats; i++) {
			ALLOC_ZVAL(match_sets[i]);
			array_init(match_sets[i]);
			INIT_PZVAL(match_sets[i]);
		}
	}

	/* Start from the beginning of the string */
	start_offset = 0;
	match = NULL;
	matched = 0;
	
	do {
		/* Execute the regular expression. */
		count = pcre_exec(re, extra, Z_STRVAL_PP(subject),
						  Z_STRLEN_PP(subject), start_offset,
						  exoptions|g_notempty, offsets, size_offsets);

		/* Check for too many substrings condition. */	
		if (count == 0) {
			zend_error(E_NOTICE, "Matched, but too many substrings\n");
			count = size_offsets/3;
		}

		/* If something has matched */
		if (count >= 0) {
			matched++;
			match = Z_STRVAL_PP(subject) + offsets[0];

			/* If subpatterns array has been passed, fill it in with values. */
			if (subpats != NULL) {
				/* Try to get the list of substrings and display a warning if failed. */
				if (pcre_get_substring_list(Z_STRVAL_PP(subject),
											offsets, count, &stringlist) < 0) {
					efree(offsets);
					efree(re);
					zend_error(E_WARNING, "Get subpatterns list failed");
					return;
				}

				if (global) {	/* global pattern matching */
					if (subpats_order == PREG_PATTERN_ORDER) {
						/* For each subpattern, insert it into the appropriate array. */
						for (i = 0; i < count; i++) {
							if (offset_capture) {
								add_offset_pair(match_sets[i], (char *)stringlist[i],
												offsets[(i<<1)+1] - offsets[i<<1], offsets[i<<1]);
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
								add_next_index_string(match_sets[i], empty_string, 1);
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
												offsets[(i<<1)+1] - offsets[i<<1], offsets[i<<1]);
							} else {
								add_next_index_stringl(result_set, (char *)stringlist[i],
													   offsets[(i<<1)+1] - offsets[i<<1], 1);
							}
						}
						/* And add it to the output array */
						zend_hash_next_index_insert(Z_ARRVAL_PP(subpats), &result_set,
													sizeof(zval *), NULL);
					}
				} else {			/* single pattern matching */
					/* For each subpattern, insert it into the subpatterns array. */
					for (i = 0; i < count; i++) {
						if (offset_capture) {
							add_offset_pair(*subpats, (char *)stringlist[i],
											offsets[(i<<1)+1] - offsets[i<<1], offsets[i<<1]);
						} else {
							add_next_index_stringl(*subpats, (char *)stringlist[i],
												   offsets[(i<<1)+1] - offsets[i<<1], 1);
						}
					}
				}

				php_pcre_free((void *) stringlist);
			}
		}
		else { /* Failed to match */
			/* If we previously set PCRE_NOTEMPTY after a null match,
			   this is not necessarily the end. We need to advance
			   the start offset, and continue. Fudge the offset values
			   to achieve this, unless we're already at the end of the string. */
			if (g_notempty != 0 && start_offset < Z_STRLEN_PP(subject)) {
				offsets[0] = start_offset;
				offsets[1] = start_offset + 1;
			} else
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
	if (global && subpats_order == PREG_PATTERN_ORDER) {
		for (i=0; i<num_subpats; i++) {
			zend_hash_next_index_insert(Z_ARRVAL_PP(subpats), &match_sets[i], sizeof(zval *), NULL);
		}
		efree(match_sets);
	}
	
	efree(offsets);

	RETVAL_LONG(matched);
}
/* }}} */

/* {{{ proto int preg_match(string pattern, string subject [, array subpatterns [, int flags]])
   Perform a Perl-style regular expression match */
PHP_FUNCTION(preg_match)
{
	php_pcre_match(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int preg_match_all(string pattern, string subject, array subpatterns [, int flags])
   Perform a Perl-style global regular expression match */
PHP_FUNCTION(preg_match_all)
{
	php_pcre_match(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
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
static int preg_do_repl_func(zval *function, char *subject, int *offsets, int count, char **result)
{
	zval		*retval_ptr;		/* Function return value */
	zval	   **args[1];			/* Argument to pass to function */
	zval		*subpats;			/* Captured subpatterns */ 
	int			 result_len;		/* Return value length */
	int			 i;
	TSRMLS_FETCH();

	MAKE_STD_ZVAL(subpats);
	array_init(subpats);
	for (i = 0; i < count; i++)
		add_next_index_stringl(subpats, &subject[offsets[i<<1]], offsets[(i<<1)+1] - offsets[i<<1], 1);
	args[0] = &subpats;

	if (call_user_function_ex(EG(function_table), NULL, function, &retval_ptr, 1, args, 0, NULL TSRMLS_CC) == SUCCESS && retval_ptr) {
		convert_to_string_ex(&retval_ptr);
		*result = estrndup(Z_STRVAL_P(retval_ptr), Z_STRLEN_P(retval_ptr));
		result_len = Z_STRLEN_P(retval_ptr);
		zval_ptr_dtor(&retval_ptr);
	} else {
		php_error(E_WARNING, "Unable to call custom replacement function");
		result_len = offsets[1] - offsets[0];
		*result = estrndup(&subject[offsets[0]], result_len);
	}
	zval_dtor(subpats);
	FREE_ZVAL(subpats);

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
					if (match_len)
						esc_match = php_addslashes(match, match_len, &esc_match_len, 0 TSRMLS_CC);
					else {
						esc_match = match;
						esc_match_len = 0;
					}
				} else {
					esc_match = empty_string;
					esc_match_len = 0;
					match_len = 0;
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
	if (zend_eval_string(code.c, &retval, compiled_string_description TSRMLS_CC) == FAILURE) {
		efree(compiled_string_description);
		zend_error(E_ERROR, "Failed evaluating code:\n%s\n", code);
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
							  int *result_len, int limit TSRMLS_DC)
{
	pcre			*re = NULL;			/* Compiled regular expression */
	pcre_extra		*extra = NULL;		/* Holds results of studying */
	int			 	 exoptions = 0;		/* Execution options */
	int			 	 preg_options = 0;	/* Custom preg options */
	int			 	 count = 0;			/* Count of matched subpatterns */
	int			 	*offsets;			/* Array of subpattern offsets */
	int			 	 size_offsets;		/* Size of the offsets array */
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

	/* Compile regex or get it from cache. */
	if ((re = pcre_get_compiled_regex(regex, &extra, &preg_options)) == NULL) {
		return NULL;
	}

	eval = preg_options & PREG_REPLACE_EVAL;
	if (is_callable_replace) {
		if (eval) {
			php_error(E_WARNING, "/e modifier cannot be used with replacement callback");
			return NULL;
		}
	} else {
		replace = Z_STRVAL_P(replace_val);
		replace_len = Z_STRLEN_P(replace_val);
		replace_end = replace + replace_len;
	}

	/* Calculate the size of the offsets array, and allocate memory for it. */
	size_offsets = (pcre_info(re, NULL, NULL) + 1) * 3;
	offsets = (int *)emalloc(size_offsets * sizeof(int));
	
	alloc_len = 2 * subject_len + 1;
	result = emalloc(alloc_len * sizeof(char));

	/* Initialize */
	match = NULL;
	*result_len = 0;
	start_offset = 0;
	
	while (1) {
		/* Execute the regular expression. */
		count = pcre_exec(re, extra, subject, subject_len, start_offset,
						  exoptions|g_notempty, offsets, size_offsets);
		
		/* Check for too many substrings condition. */
		if (count == 0) {
			zend_error(E_NOTICE, "Matched, but too many substrings\n");
			count = size_offsets/3;
		}

		piece = subject + start_offset;

		if (count > 0 && (limit == -1 || limit > 0)) {
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
				eval_result_len = preg_do_repl_func(replace_val, subject, offsets,
													count, &eval_result);
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

		} else { /* Failed to match */
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
					new_buf = emalloc(alloc_len * sizeof(char));
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

	return result;
}
/* }}} */

/* {{{ php_replace_in_subject
 */
static char *php_replace_in_subject(zval *regex, zval *replace, zval **subject, int *result_len, int limit, zend_bool is_callable_replace TSRMLS_DC)
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
	ZVAL_STRINGL(&empty_replace, empty_string, 0, 0);
	
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
										   limit TSRMLS_CC)) != NULL) {
				efree(subject_value);
				subject_value = result;
				subject_len = *result_len;
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
								  limit TSRMLS_CC);
		return result;
	}
}
/* }}} */

/* {{{ preg_replace_impl
 */
static void preg_replace_impl(INTERNAL_FUNCTION_PARAMETERS, zend_bool is_callable_replace)
{
	zval		   **regex,
				   **replace,
				   **subject,
				   **limit,
				   **subject_entry;
	char			*result;
	int				 result_len;
	int				 limit_val = -1;
	char			*string_key;
	ulong			 num_key;
	char			*callback_name = NULL;
	
	/* Get function parameters and do error-checking. */
	if (ZEND_NUM_ARGS() < 3 || ZEND_NUM_ARGS() > 4 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &regex, &replace, &subject, &limit) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	SEPARATE_ZVAL(replace);
	if (Z_TYPE_PP(replace) != IS_ARRAY)
		convert_to_string_ex(replace);
	if (is_callable_replace) {
		if (!zend_is_callable(*replace, 0, &callback_name)) {
			php_error(E_WARNING, "%s() requires argument 2, '%s', to be a valid callback",
					  get_active_function_name(TSRMLS_C), callback_name);
			efree(callback_name);
			*return_value = **subject;
			zval_copy_ctor(return_value);
			return;
		}
		efree(callback_name);
	}

	SEPARATE_ZVAL(regex);
	SEPARATE_ZVAL(subject);

	if (ZEND_NUM_ARGS() > 3) {
		convert_to_long_ex(limit);
		limit_val = Z_LVAL_PP(limit);
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
			if ((result = php_replace_in_subject(*regex, *replace, subject_entry, &result_len, limit_val, is_callable_replace TSRMLS_CC)) != NULL) {
				/* Add to return array */
				switch(zend_hash_get_current_key(Z_ARRVAL_PP(subject), &string_key, &num_key, 0))
				{
					case HASH_KEY_IS_STRING:
						add_assoc_stringl(return_value, string_key, result, result_len, 0);
						break;

					case HASH_KEY_IS_LONG:
						add_index_stringl(return_value, num_key, result, result_len, 0);
						break;
				}
			}
		
			zend_hash_move_forward(Z_ARRVAL_PP(subject));
		}
	}
	else {	/* if subject is not an array */
		if ((result = php_replace_in_subject(*regex, *replace, subject, &result_len, limit_val, is_callable_replace TSRMLS_CC)) != NULL) {
			RETVAL_STRINGL(result, result_len, 0);
		}
	}	
}
/* }}} */

/* {{{ proto string preg_replace(mixed regex, mixed replace, mixed subject [, int limit])
   Perform Perl-style regular expression replacement. */
PHP_FUNCTION(preg_replace)
{
	preg_replace_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto string preg_replace_callback(mixed regex, mixed callback, mixed subject [, int limit])
   Perform Perl-style regular expression replacement using replacement callback. */
PHP_FUNCTION(preg_replace_callback)
{
	preg_replace_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto array preg_split(string pattern, string subject [, int limit [, int flags]]) 
   Split string into an array using a perl-style regular expression as a delimiter */
PHP_FUNCTION(preg_split)
{
	zval		   **regex,				/* Regular expression to split by */
				   **subject,			/* Subject string to split */
				   **limit,				/* Number of pieces to return */
				   **flags;
	pcre			*re = NULL;			/* Compiled regular expression */
	pcre_extra		*extra = NULL;		/* Holds results of studying */
	int			 	*offsets;			/* Array of subpattern offsets */
	int			 	 size_offsets;		/* Size of the offsets array */
	int				 exoptions = 0;		/* Execution options */
	int			 	 preg_options = 0;	/* Custom preg options */
	int				 argc;				/* Argument count */
	int				 limit_val = -1;	/* Integer value of limit */
	int				 no_empty = 0;		/* If NO_EMPTY flag is set */
	int				 delim_capture = 0; /* If delimiters should be captured */
	int				 offset_capture = 0;/* If offsets should be captured */
	int				 count = 0;			/* Count of matched subpatterns */
	int				 start_offset;		/* Where the new search starts */
	int				 next_offset;		/* End of the last delimiter match + 1 */
	int				 g_notempty = 0;	/* If the match should not be empty */
	char			*match,				/* The current match */
					*last_match;		/* Location of last match */

	/* Get function parameters and do error checking */	
	argc = ZEND_NUM_ARGS();
	if (argc < 2 || argc > 4 || zend_get_parameters_ex(argc, &regex, &subject, &limit, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if (argc > 2) {
		convert_to_long_ex(limit);
		limit_val = Z_LVAL_PP(limit);
		if (limit_val == 0)
			limit_val = -1;

		if (argc > 3) {
			convert_to_long_ex(flags);
			no_empty = Z_LVAL_PP(flags) & PREG_SPLIT_NO_EMPTY;
			delim_capture = Z_LVAL_PP(flags) & PREG_SPLIT_DELIM_CAPTURE;
			offset_capture = Z_LVAL_PP(flags) & PREG_SPLIT_OFFSET_CAPTURE;
		}
	}
	
	/* Make sure we're dealing with strings */
	convert_to_string_ex(regex);
	convert_to_string_ex(subject);
	
	/* Compile regex or get it from cache. */
	if ((re = pcre_get_compiled_regex(Z_STRVAL_PP(regex), &extra, &preg_options)) == NULL) {
		RETURN_FALSE;
	}
	
	/* Initialize return value */
	array_init(return_value);

	/* Calculate the size of the offsets array, and allocate memory for it. */
	size_offsets = (pcre_info(re, NULL, NULL) + 1) * 3;
	offsets = (int *)emalloc(size_offsets * sizeof(int));
	
	/* Start at the beginning of the string */
	start_offset = 0;
	next_offset = 0;
	last_match = Z_STRVAL_PP(subject);
	match = NULL;
	
	/* Get next piece if no limit or limit not yet reached and something matched*/
	while ((limit_val == -1 || limit_val > 1)) {
		count = pcre_exec(re, extra, Z_STRVAL_PP(subject),
						  Z_STRLEN_PP(subject), start_offset,
						  exoptions|g_notempty, offsets, size_offsets);

		/* Check for too many substrings condition. */
		if (count == 0) {
			zend_error(E_NOTICE, "Matched, but too many substrings\n");
			count = size_offsets/3;
		}
				
		/* If something matched */
		if (count > 0) {
			match = Z_STRVAL_PP(subject) + offsets[0];

			if (!no_empty || &Z_STRVAL_PP(subject)[offsets[0]] != last_match) {

				if (offset_capture) {
					/* Add (match, offset) pair to the return value */
					add_offset_pair(return_value, last_match, &Z_STRVAL_PP(subject)[offsets[0]]-last_match, next_offset);
				} else {
                	/* Add the piece to the return value */
					add_next_index_stringl(return_value, last_match,
								   	   &Z_STRVAL_PP(subject)[offsets[0]]-last_match, 1);
				}

				/* One less left to do */
				if (limit_val != -1)
					limit_val--;
			}
			
			last_match = &Z_STRVAL_PP(subject)[offsets[1]];
            next_offset = offsets[1];

			if (delim_capture) {
				int i, match_len;
				for (i = 1; i < count; i++) {
					match_len = offsets[(i<<1)+1] - offsets[i<<1];
					/* If we have matched a delimiter */
					if (!no_empty || match_len > 0) {
						if (offset_capture) {
							add_offset_pair(return_value, &Z_STRVAL_PP(subject)[offsets[i<<1]], match_len, offsets[i<<1]);
						} else {
							add_next_index_stringl(return_value,
												   &Z_STRVAL_PP(subject)[offsets[i<<1]],
												   match_len, 1);
						}
					}
				}
			}
		} else { /* Failed to match */
			/* If we previously set PCRE_NOTEMPTY after a null match,
			   this is not necessarily the end. We need to advance
			   the start offset, and continue. Fudge the offset values
			   to achieve this, unless we're already at the end of the string. */
			if (g_notempty != 0 && start_offset < Z_STRLEN_PP(subject)) {
				offsets[0] = start_offset;
				offsets[1] = start_offset + 1;
			} else
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


	if (!no_empty || start_offset != Z_STRLEN_PP(subject))
	{
		if (offset_capture) {
			/* Add the last (match, offset) pair to the return value */
			add_offset_pair(return_value, &Z_STRVAL_PP(subject)[start_offset], Z_STRLEN_PP(subject) - start_offset, start_offset);
		} else {
			/* Add the last piece to the return value */
			add_next_index_stringl(return_value,
								   &Z_STRVAL_PP(subject)[start_offset],
								   Z_STRLEN_PP(subject) - start_offset, 1);
		}
	}

	
	/* Clean up */
	efree(offsets);
}
/* }}} */

/* {{{ proto string preg_quote(string str, string delim_char)
   Quote regular expression characters plus an optional character */
PHP_FUNCTION(preg_quote)
{
	zval    **in_str_arg;	/* Input string argument */
	zval	**delim;		/* Additional delimiter argument */
	char 	*in_str,		/* Input string */
	        *in_str_end,    /* End of the input string */
			*out_str,		/* Output string with quoted characters */
		 	*p,				/* Iterator for input string */
			*q,				/* Iterator for output string */
			 delim_char=0,	/* Delimiter character to be quoted */
		 	 c;				/* Current character */
	zend_bool quote_delim = 0; /* Whether to quote additional delim char */
	
	/* Get the arguments and check for errors */
	if (ZEND_NUM_ARGS() < 1 || ZEND_NUM_ARGS() > 2 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &in_str_arg, &delim) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	/* Make sure we're working with strings */
	convert_to_string_ex(in_str_arg);
	in_str = Z_STRVAL_PP(in_str_arg);
	in_str_end = Z_STRVAL_PP(in_str_arg) + Z_STRLEN_PP(in_str_arg);

	/* Nothing to do if we got an empty string */
	if (in_str == in_str_end) {
		RETVAL_STRINGL(empty_string, 0, 0);
	}

	if (ZEND_NUM_ARGS() == 2) {
		convert_to_string_ex(delim);
		if (Z_STRLEN_PP(delim) > 0) {
			delim_char = Z_STRVAL_PP(delim)[0];
			quote_delim = 1;
		}
	}
	
	/* Allocate enough memory so that even if each character
	   is quoted, we won't run out of room */
	out_str = emalloc(2 * Z_STRLEN_PP(in_str_arg) + 1);
	
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
				*q++ = '\\';
				*q++ = c;
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

/* {{{ proto array preg_grep(string regex, array input)
   Searches array and returns entries which match regex */
PHP_FUNCTION(preg_grep)
{
	zval		   **regex,				/* Regular expression */
				   **input,				/* Input array */
				   **flags,
			   	   **entry;				/* An entry in the input array */
	pcre			*re = NULL;			/* Compiled regular expression */
	pcre_extra		*extra = NULL;		/* Holds results of studying */
	int			 	 preg_options = 0;	/* Custom preg options */
	int			 	*offsets;			/* Array of subpattern offsets */
	int			 	 size_offsets;		/* Size of the offsets array */
	int			 	 count = 0;			/* Count of matched subpatterns */
	char			*string_key;
	ulong			 num_key;
	zend_bool		 invert = 0;		/* Whether to return non-matching
										   entries */
	
	/* Get arguments and do error checking */
	
	if (ZEND_NUM_ARGS() < 2 || ZEND_NUM_ARGS() > 3 ||
		zend_get_parameters_ex(ZEND_NUM_ARGS(), &regex, &input, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if (Z_TYPE_PP(input) != IS_ARRAY) {
		zend_error(E_WARNING, "Second argument to preg_grep() should be an array");
		return;
	}

	SEPARATE_ZVAL(input);
	
	/* Make sure regex is a string */
	convert_to_string_ex(regex);

	if (ZEND_NUM_ARGS() > 2) {
		convert_to_long_ex(flags);
		invert = (Z_LVAL_PP(flags) & PREG_GREP_INVERT) ? 1 : 0;
	}
	
	/* Compile regex or get it from cache. */
	if ((re = pcre_get_compiled_regex(Z_STRVAL_PP(regex), &extra, &preg_options)) == NULL) {
		RETURN_FALSE;
	}

	/* Calculate the size of the offsets array, and allocate memory for it. */
	size_offsets = (pcre_info(re, NULL, NULL) + 1) * 3;
	offsets = (int *)emalloc(size_offsets * sizeof(int));
	
	/* Initialize return array */
	array_init(return_value);

	/* Go through the input array */
	zend_hash_internal_pointer_reset(Z_ARRVAL_PP(input));
	while(zend_hash_get_current_data(Z_ARRVAL_PP(input), (void **)&entry) == SUCCESS) {

		convert_to_string_ex(entry);

		/* Perform the match */
		count = pcre_exec(re, extra, Z_STRVAL_PP(entry),
						  Z_STRLEN_PP(entry), 0,
						  0, offsets, size_offsets);

		/* Check for too many substrings condition. */
		if (count == 0) {
			zend_error(E_NOTICE, "Matched, but too many substrings");
			count = size_offsets/3;
		}

		/* If the entry fits our requirements */
		if ((count > 0 && !invert) ||
			(count < 0 && invert)) {
			(*entry)->refcount++;

			/* Add to return array */
			switch(zend_hash_get_current_key(Z_ARRVAL_PP(input), &string_key, &num_key, 0))
			{
				case HASH_KEY_IS_STRING:
					zend_hash_update(Z_ARRVAL_P(return_value), string_key,
									 strlen(string_key)+1, entry, sizeof(zval *), NULL);
					break;

				case HASH_KEY_IS_LONG:
					zend_hash_index_update(Z_ARRVAL_P(return_value), num_key, entry,
										   sizeof(zval *), NULL);
					break;
			}
		}
		
		zend_hash_move_forward(Z_ARRVAL_PP(input));
	}
	
	/* Clean up */
	efree(offsets);
}
/* }}} */

/* {{{ module definition structures */

function_entry pcre_functions[] = {
	PHP_FE(preg_match,				third_arg_force_ref)
	PHP_FE(preg_match_all,			third_arg_force_ref)
	PHP_FE(preg_replace,			NULL)
	PHP_FE(preg_replace_callback,	NULL)
	PHP_FE(preg_split,				NULL)
	PHP_FE(preg_quote,				NULL)
	PHP_FE(preg_grep,				NULL)
	{NULL, 		NULL,				NULL}
};

zend_module_entry pcre_module_entry = {
	STANDARD_MODULE_HEADER,
   "pcre",
	pcre_functions,
	PHP_MINIT(pcre),
	PHP_MSHUTDOWN(pcre),
	PHP_RINIT(pcre),
	NULL,
	PHP_MINFO(pcre),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
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
