/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andrey Zmievski <andrey@ispi.net>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/*
	TODO:
	- Allow user to set PCRE_NOTEMPTY, PCRE_ANCHORED at execution time
	- Have an option for preg_split() to not return empty strings
*/

#include "php.h"
#include "php_globals.h"
#include "php_pcre.h"

#if HAVE_PCRE || HAVE_BUNDLED_PCRE

#include "ext/standard/php3_string.h"

#define PREG_PATTERN_ORDER	0
#define PREG_SET_ORDER		1

#define	PREG_SPLIT_NO_EMPTY	(1<<0)

#define PREG_REPLACE_EVAL	(1<<0)

#ifdef ZTS
int pcre_globals_id;
#else
php_pcre_globals pcre_globals;
#endif


static void *php_pcre_malloc(size_t size)
{
	return pemalloc(size, 1);
}


static void php_pcre_free(void *ptr)
{
	pefree(ptr, 1);
}


static int _php_free_pcre_cache(void *data)
{
	pcre_cache_entry *pce = (pcre_cache_entry *) data;
	pefree(pce->re, 1);
#if HAVE_SETLOCALE
	pefree((void*)pce->tables, 1);
#endif
	return 1;
}


#ifdef ZTS
static void _php_pcre_init_globals(php_pcre_globals *pcre_globals)
{
	zend_hash_init(&PCRE_G(pcre_cache), 0, NULL, _php_free_pcre_cache, 1);
}


static void _php_pcre_shutdown_globals(php_pcre_globals *pcre_globals)
{
	zend_hash_destroy(&PCRE_G(pcre_cache));
}
#endif


static /* {{{ PHP_MINFO_FUNCTION(pcre) */
PHP_MINFO_FUNCTION(pcre)
{
	php_printf("Perl Compatible Regular Expressions");
	php_printf("<table cellpadding=5>"
				"<tr><td>PCRE library version:</td>"
				"<td>%s</td></tr>"
				"</table>", pcre_version());
}
/* }}} */


/* {{{ PHP_MINIT_FUNCTION(pcre) */
static PHP_MINIT_FUNCTION(pcre)
{
	ELS_FETCH();

#ifdef ZTS
	pcre_globals_id = ts_allocate_id(
							sizeof(php_pcre_globals),
							_php_pcre_init_globals,
							_php_pcre_shutdown_globals);
#else
	zend_hash_init(&PCRE_G(pcre_cache), 0, NULL, _php_free_pcre_cache, 1);
#endif
	
	REGISTER_LONG_CONSTANT("PREG_PATTERN_ORDER", PREG_PATTERN_ORDER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_SET_ORDER", PREG_SET_ORDER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PREG_SPLIT_NO_EMPTY", PREG_SPLIT_NO_EMPTY, CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}
/* }}} */


/* {{{ PHP_MSHUTDOWN_FUNCTION(pcre) */
static PHP_MSHUTDOWN_FUNCTION(pcre)
{
#ifndef ZTS
	zend_hash_destroy(&PCRE_G(pcre_cache));
#else
	ts_free_id(pcre_globals_id);
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


/* {{{ static pcre* _pcre_get_compiled_regex(char *regex, pcre_extra *extra) */
static pcre* _pcre_get_compiled_regex(char *regex, pcre_extra *extra, int *preg_options) {
	pcre				*re = NULL;
	int				 	 coptions = 0;
	int				 	 soptions = 0;
	const char	 		*error;
	int			 	 	 erroffset;
	char		 	 	 delimiter;
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
	PCRE_LS_FETCH();

	/* Try to lookup the cached regex entry, and if successful, just pass
	   back the compiled pattern, otherwise go on and compile it. */
	regex_len = strlen(regex);
	if (zend_hash_find(&PCRE_G(pcre_cache), regex, regex_len+1, (void **)&pce) == SUCCESS) {
#if HAVE_SETLOCALE
		if (!strcmp(pce->locale, locale)) {
#endif
			extra = pce->extra;
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
	
	/* We need to iterate through the pattern, searching for the ending delimiter,
	   but skipping the backslashed delimiters.  If the ending delimiter is not
	   found, display a warning. */
	pp = p;
	while (*pp != 0) {
		if (*pp == delimiter && pp[-1] != '\\')
			break;
		pp++;
	}
	if (*pp == 0) {
		zend_error(E_WARNING, "No ending delimiter found");
		return NULL;
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

			/* Custom preg options */
			case 'e':	poptions |= PREG_REPLACE_EVAL; break;
			
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
		extra = pcre_study(re, soptions, &error);
		if (error != NULL) {
			zend_error(E_WARNING, "Error while studying pattern");
		}
	}

	*preg_options = poptions;
	
	efree(pattern);

	/* Store the compiled pattern and extra info in the cache. */
	new_entry.re = re;
	new_entry.extra = extra;
	new_entry.preg_options = poptions;
#if HAVE_SETLOCALE
	new_entry.locale = locale;
	new_entry.tables = tables;
#endif
	zend_hash_update(&PCRE_G(pcre_cache), regex, regex_len+1, (void *)&new_entry,
						sizeof(pcre_cache_entry), NULL);

	return re;
}
/* }}} */


/* {{{ void _pcre_match(INTERNAL_FUNCTION_PARAMETERS, int global) */
static void _pcre_match(INTERNAL_FUNCTION_PARAMETERS, int global)
{
	zval			**regex,			/* Regular expression */
					**subject,			/* String to match against */
					**subpats = NULL,	/* Array for subpatterns */
					**subpats_order,	/* Order of the results in the subpatterns
										   array for global match */
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
	int				 subpats_order_val = 0;	/* Integer value of subpats_order */
	int				 g_notempty = 0;	/* If the match should not be empty */
	const char	   **stringlist;		/* Used to hold list of subpatterns */
	char			*match;				/* The current match */
	
	
	/* Get function parameters and do error-checking. */
	switch(ARG_COUNT(ht)) {
		case 2:
			if (getParametersEx(2, &regex, &subject) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
			
		case 3:
			if (getParametersEx(3, &regex, &subject, &subpats) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			if (global)
				subpats_order_val = PREG_PATTERN_ORDER;
			if (!ParameterPassedByReference(ht, 3)) {
				zend_error(E_WARNING, "Array to be filled with matches must be passed by reference.");
				RETURN_FALSE;
			}
			break;

		case 4:
			if (getParametersEx(4, &regex, &subject, &subpats, &subpats_order) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			if (!ParameterPassedByReference(ht, 3)) {
				zend_error(E_WARNING, "Array to be filled with matches must be passed by reference.");
				RETURN_FALSE;
			}
	
			/* Make sure subpats_order is a number */
			convert_to_long_ex(subpats_order);
			subpats_order_val = (*subpats_order)->value.lval;
			if (subpats_order_val < PREG_PATTERN_ORDER ||
				subpats_order_val > PREG_SET_ORDER) {
				zend_error(E_WARNING, "Wrong value for parameter 4 in call to preg_match_all()");
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
	if ((re = _pcre_get_compiled_regex((*regex)->value.str.val, extra, &preg_options)) == NULL) {
		RETURN_FALSE;
	}

	/* Calculate the size of the offsets array, and allocate memory for it. */
	num_subpats = pcre_info(re, NULL, NULL) + 1;
	size_offsets = num_subpats * 3;
	offsets = (int *)emalloc(size_offsets * sizeof(int));

	/* Allocate match sets array and initialize the values */
	if (global && subpats_order_val == PREG_PATTERN_ORDER) {
		match_sets = (zval **)emalloc(num_subpats * sizeof(zval *));
		for (i=0; i<num_subpats; i++) {
			match_sets[i] = (zval *)emalloc(sizeof(zval));
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
		count = pcre_exec(re, extra, (*subject)->value.str.val,
						  (*subject)->value.str.len, start_offset,
						  exoptions|g_notempty, offsets, size_offsets);

		/* Check for too many substrings condition. */	
		if (count == 0) {
			zend_error(E_NOTICE, "Matched, but too many substrings\n");
			count = size_offsets/3;
		}

		/* If something has matched */
		if (count >= 0) {
			matched++;
			match = (*subject)->value.str.val + offsets[0];

			/* If subpatters array has been passed, fill it in with values. */
			if (subpats != NULL) {
				/* Try to get the list of substrings and display a warning if failed. */
				if (pcre_get_substring_list((*subject)->value.str.val,
											offsets, count, &stringlist) < 0) {
					efree(offsets);
					efree(re);
					zend_error(E_WARNING, "Get subpatterns list failed");
					return;
				}

				if (global) {	/* global pattern matching */
					if (subpats_order_val == PREG_PATTERN_ORDER) {
						/* For each subpattern, insert it into the appropriate array */
						for (i=0; i<count; i++) {
							add_next_index_string(match_sets[i], (char *)stringlist[i], 1);
						}
					}
					else {
						/* Allocate the result set array */
						result_set = emalloc(sizeof(zval));
						array_init(result_set);
						INIT_PZVAL(result_set);
						
						/* Add all the subpatterns to it */
						for (i=0; i<count; i++) {
							add_next_index_string(result_set, (char *)stringlist[i], 1);
						}
						/* And add it to the output array */
						zend_hash_next_index_insert((*subpats)->value.ht, &result_set,
								sizeof(zval *), NULL);
					}
				}
				else {			/* single pattern matching */
					/* For each subpattern, insert it into the subpatterns array. */
					for (i=0; i<count; i++) {
						add_next_index_string((*subpats), (char *)stringlist[i], 1);
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
			if (g_notempty != 0 && start_offset < (*subject)->value.str.len) {
				offsets[0] = start_offset;
				offsets[1] = start_offset + 1;
			} else
				break;
		}
		
		/* If we have matched an empty string, mimic what Perl's /g options does.
		   This turns out to be rather cunning. First we set PCRE_NOTEMPTY and try
		   the match again at the same point. If this fails (picked up above) we
		   advance to the next character. */
		g_notempty = (offsets[1] == offsets[0])? PCRE_NOTEMPTY : 0;
		
		/* Advance to the position right after the last full match */
		start_offset = offsets[1];
	} while (global);

	/* Add the match sets to the output array and clean up */
	if (global && subpats_order_val == PREG_PATTERN_ORDER) {
		for (i=0; i<num_subpats; i++) {
			zend_hash_next_index_insert((*subpats)->value.ht, &match_sets[i], sizeof(zval *), NULL);
		}
		efree(match_sets);
	}
	
	efree(offsets);

	RETVAL_LONG(matched);
}
/* }}} */


/* {{{ proto int preg_match(string pattern, string subject [, array subpatterns ])
   Perform a Perl-style regular expression match */
PHP_FUNCTION(preg_match)
{
	_pcre_match(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */


/* {{{ proto int preg_match_all(string pattern, string subject, array subpatterns [, int order ])
   Perform a Perl-style global regular expression match */
PHP_FUNCTION(preg_match_all)
{
	_pcre_match(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


/* {{{ int _preg_get_backref(const char *walk, int *backref) */
static int _preg_get_backref(const char *walk, int *backref)
{
	if (*walk && *walk >= '0' && *walk <= '9')
		*backref = *walk - '0';
	else
		return 0;
	
	if (walk[1] && walk[1] >= '0' && walk[1] <= '9')
		*backref = *backref * 10 + walk[1] - '0';
	
	return 1;	
}
/* }}} */


/* {{{ int _preg_do_eval(char *eval_str, char *subject, int *offsets,
				  int count, char **result) */
static int _preg_do_eval(char *eval_str, char *subject, int *offsets,
				  int count, char **result)
{
	zval		 retval;			/* Return value from evaluation */
	char		 backref_buf[4],	/* Buffer for string version of backref */
				*code,				/* PHP code string */
				*new_code,			/* Code as result of substitution */
				*match,				/* Current match for a backref */
				*walk;				/* Used to walk the code string */
	int			 code_len;			/* Length of the code string */
	int			 new_code_len;		/* Length of the substituted code string */
	int			 match_len;			/* Length of the match */
	int			 result_len;		/* Length of the result of the evaluation */
	int			 backref;			/* Current backref */
	CLS_FETCH();
	ELS_FETCH();
	
	/* Save string to be evaluated, since we will be modifying it */
	code = estrdup(eval_str);
	walk = code;
	new_code_len = code_len = strlen(code);
	
	while (*walk) {
		/* If found a backreference.. */
		if ('\\' == *walk &&
			_preg_get_backref(walk+1, &backref) &&
			backref < count) {
			/* Find the corresponding string match and substitute it
			   in instead of the backref */
			match = subject + offsets[backref<<1];
			match_len = offsets[(backref<<1)+1] - offsets[backref<<1];
			sprintf(backref_buf, "\\%d", backref);
			new_code = _php3_str_to_str(code, code_len,
										backref_buf, (backref > 9) ? 3 : 2,
										match, match_len, &new_code_len);
			
			/* Adjust the walk pointer */
			walk = new_code + (walk - code) + match_len;
			
			/* Clean up and reassign */
			efree(code);
			code = new_code;
			code_len = new_code_len;
		} else {
			walk++;
		}
	}

	/* Run the code */
	zend_eval_string(code, &retval CLS_CC ELS_CC);
	convert_to_string(&retval);
	
	/* Save the return value and its length */
	*result = estrdup(retval.value.str.val);
	result_len = retval.value.str.len;
	
	/* Clean up */
	zval_dtor(&retval);
	efree(code);
	
	return result_len;
}
/* }}} */


/* {{{ char *_php_pcre_replace(char *regex, char *subject, char *replace) */
char *_php_pcre_replace(char *regex, char *subject, char *replace)
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
	int				 subject_len;		/* Length of the subject string */
	int				 eval_result_len=0;	/* Length of the eval'ed string */
	int				 result_len;		/* Current length of the result */
	int				 match_len;			/* Length of the current match */
	int				 backref;			/* Backreference number */
	int				 eval;				/* If the replacement string should be eval'ed */
	int				 start_offset;		/* Where the new search starts */
	int				 g_notempty = 0;	/* If the match should not be empty */
	char			*result,			/* Result of replacement */
					*new_buf,			/* Temporary buffer for re-allocation */
					*walkbuf,			/* Location of current replacement in the result */
					*walk,				/* Used to walk the replacement string */
					*match,				/* The current match */
					*piece,				/* The current piece of subject */
					*eval_result;		/* Result of eval */

	/* Compile regex or get it from cache. */
	if ((re = _pcre_get_compiled_regex(regex, extra, &preg_options)) == NULL) {
		return NULL;
	}
	
	/* Calculate the size of the offsets array, and allocate memory for it. */
	size_offsets = (pcre_info(re, NULL, NULL) + 1) * 3;
	offsets = (int *)emalloc(size_offsets * sizeof(int));
	
	subject_len = strlen(subject);

	alloc_len = 2 * subject_len + 1;
	result = emalloc(alloc_len * sizeof(char));
	if (!result) {
		zend_error(E_WARNING, "Unable to allocate memory in pcre_replace");
		efree(re);
		efree(offsets);
		return NULL;
	}

	/* Initialize */
	match = NULL;
	result[0] = '\0';
	start_offset = 0;
	eval = preg_options & PREG_REPLACE_EVAL;
	
	while (1) {
		/* Execute the regular expression. */
		count = pcre_exec(re, extra, subject, subject_len, start_offset,
						  exoptions|g_notempty, offsets, size_offsets);
		
		/* Check for too many substrings condition. */
		if (count == 0) {
			zend_error(E_NOTICE, "Matched, but too many substrings\n");
			count = size_offsets/3;
		}

		piece = &subject[start_offset];

		if (count > 0) {
			/* Set the match location in subject */
			match = subject + offsets[0];

			new_len = strlen(result) + offsets[0] - start_offset; /* part before the match */
			
			/* If evaluating, do it and add the return string's length */
			if (eval) {
				eval_result_len = _preg_do_eval(replace, piece, offsets,
												count, &eval_result);
				new_len += eval_result_len;
			} else { /* do regular substitution */
				walk = replace;
				while (*walk)
					if ('\\' == *walk &&
						_preg_get_backref(walk+1, &backref) &&
						backref < count) {
						new_len += offsets[(backref<<1)+1] - offsets[backref<<1];
						walk += (backref > 9) ? 3 : 2;
					} else {
						new_len++;
						walk++;
					}
			}

			if (new_len + 1 > alloc_len) {
				alloc_len = 1 + alloc_len + 2 * new_len;
				new_buf = emalloc(alloc_len);
				strcpy(new_buf, result);
				efree(result);
				result = new_buf;
			}
			result_len = strlen(result);
			/* copy the part of the string before the match */
			strncat(result, piece, match-piece);

			/* copy replacement and backrefs */
			walkbuf = &result[result_len + offsets[0] - start_offset];
			
			/* If evaluating, copy result to the buffer and clean up */
			if (eval) {
				memcpy(walkbuf, eval_result, eval_result_len);
				walkbuf += eval_result_len;
				efree(eval_result);
			} else { /* do regular backreference copying */
				walk = replace;
				while (*walk)
					if ('\\' == *walk &&
						_preg_get_backref(walk+1, &backref) &&
						backref < count) {
						match_len = offsets[(backref<<1)+1] - offsets[backref<<1];
						memcpy (walkbuf,
								piece + offsets[backref<<1],
								match_len);
						walkbuf += match_len;
						walk += (backref > 9) ? 3 : 2;
					} else
						*walkbuf++ = *walk++;
			}
			*walkbuf = '\0';
		} else { /* Failed to match */
			/* If we previously set PCRE_NOTEMPTY after a null match,
			   this is not necessarily the end. We need to advance
			   the start offset, and continue. Fudge the offset values
			   to achieve this, unless we're already at the end of the string. */
			if (g_notempty != 0 && start_offset < subject_len) {
				offsets[0] = start_offset;
				offsets[1] = start_offset + 1;
				strncat(result, piece, 1);
			}
			else {
				new_len = strlen(result) + subject_len - start_offset;
				if (new_len + 1 > alloc_len) {
					alloc_len = new_len + 1; /* now we know exactly how long it is */
					new_buf = emalloc(alloc_len * sizeof(char));
					strcpy(new_buf, result);
					efree(result);
					result = new_buf;
				}
				/* stick that last bit of string on our output */
				strcat(result, piece);
				break;
			}
		}
			
		/* If we have matched an empty string, mimic what Perl's /g options does.
		   This turns out to be rather cunning. First we set PCRE_NOTEMPTY and try
		   the match again at the same point. If this fails (picked up above) we
		   advance to the next character. */
		g_notempty = (offsets[1] == offsets[0])? PCRE_NOTEMPTY : 0;
		
		/* Advance to the next piece */
		start_offset = offsets[1];
	}
	
	efree(offsets);

	return result;
}
/* }}} */


static char *_php_replace_in_subject(zval *regex, zval *replace, zval *subject)
{
	zval		**regex_entry_ptr,
				*regex_entry,
				**replace_entry_ptr,
				*replace_entry = NULL;
	char		*replace_value = NULL,
				*subject_value,
				*result;

	/* Make sure we're dealing with strings. */	
	convert_to_string(subject);
	
	/* If regex is an array */
	if (regex->type == IS_ARRAY) {
		/* Duplicating subject string for repeated replacement */
		subject_value = estrdup(subject->value.str.val);
		
		zend_hash_internal_pointer_reset(regex->value.ht);

		if (replace->type == IS_ARRAY) {
			zend_hash_internal_pointer_reset(replace->value.ht);
			MAKE_STD_ZVAL(replace_entry);
		}
		else
			/* Set replacement value to the passed one */
			replace_value = replace->value.str.val;

		MAKE_STD_ZVAL(regex_entry);
				
		/* For each entry in the regex array, get the entry */
		while (zend_hash_get_current_data(regex->value.ht, (void **)&regex_entry_ptr) == SUCCESS) {
			*regex_entry = **regex_entry_ptr;
			zval_copy_ctor(regex_entry);

			/* Make sure we're dealing with strings. */	
			convert_to_string(regex_entry);
		
			/* If replace is an array */
			if (replace->type == IS_ARRAY) {
				/* Get current entry */
				if (zend_hash_get_current_data(replace->value.ht, (void **)&replace_entry_ptr) == SUCCESS) {
					*replace_entry = **replace_entry_ptr;
					zval_copy_ctor(replace_entry);
					
					/* Make sure we're dealing with strings. */	
					convert_to_string(replace_entry);
					
					/* Set replacement value to the one we got from array */
					replace_value = replace_entry->value.str.val;

					zend_hash_move_forward(replace->value.ht);
				}
				else
					/* We've run out of replacement strings, so use an empty one */
					replace_value = empty_string;
			}
			
			/* Do the actual replacement and put the result back into subject_value
			   for further replacements. */
			if ((result = _php_pcre_replace(regex_entry->value.str.val,
											subject_value,
											replace_value)) != NULL) {
				efree(subject_value);
				subject_value = result;
			}
			
			if (replace->type == IS_ARRAY)
				zval_dtor(replace_entry);
			zval_dtor(regex_entry);
			zend_hash_move_forward(regex->value.ht);
		}

		if (replace->type == IS_ARRAY)
			efree(replace_entry);
		efree(regex_entry);
		return subject_value;
	}
	else {
		/* Make sure we're dealing with strings and do the replacement */
		convert_to_string(regex);
		convert_to_string(replace);
		result = _php_pcre_replace(regex->value.str.val,
									subject->value.str.val,
									replace->value.str.val);
		return result;
	}
}


/* {{{ proto string preg_replace(string|array regex, string|array replace, string|array subject)
    Perform Perl-style regular expression replacement */
PHP_FUNCTION(preg_replace)
{
	zval		   **regex,
				   **replace,
				   **subject,
				   **subject_entry_ptr,
					*subject_entry;
	char			*result;
	char			*string_key;
	ulong			 num_key;
	
	/* Get function parameters and do error-checking. */
	if (ARG_COUNT(ht) != 3 || getParametersEx(3, &regex, &replace, &subject) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* if subject is an array */
	if ((*subject)->type == IS_ARRAY) {
		array_init(return_value);
		zend_hash_internal_pointer_reset((*subject)->value.ht);

		MAKE_STD_ZVAL(subject_entry);
				
		/* For each subject entry, convert it to string, then perform replacement
		   and add the result to the return_value array. */
		while (zend_hash_get_current_data((*subject)->value.ht, (void **)&subject_entry_ptr) == SUCCESS) {
			*subject_entry = **subject_entry_ptr;
			zval_copy_ctor(subject_entry);
			
			if ((result = _php_replace_in_subject(*regex, *replace, subject_entry)) != NULL)
			{
				/* Add to return array */
				switch(zend_hash_get_current_key((*subject)->value.ht, &string_key, &num_key))
				{
					case HASH_KEY_IS_STRING:
						add_assoc_string(return_value, string_key, result, 0);
						efree(string_key);
						break;

					case HASH_KEY_IS_LONG:
						add_index_string(return_value, num_key, result, 0);
						break;
				}
			}
		
			zval_dtor(subject_entry);
			
			zend_hash_move_forward((*subject)->value.ht);
		}
		
		efree(subject_entry);
	}
	else {	/* if subject is not an array */
		if ((result = _php_replace_in_subject(*regex, *replace, *subject)) != NULL) {
			RETVAL_STRING(result, 1);
			efree(result);
		}
	}	
}
/* }}} */


/* {{{ proto array preg_split(string pattern, string subject [, int limit [, int flags]]) 
    split string into an array using a perl-style regular expression as a delimiter */
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
	int				 limit_val;			/* Integer value of limit */
	int				 no_empty = 0;		/* If NO_EMPTY flag is set */
	int				 count = 0;			/* Count of matched subpatterns */
	int				 start_offset;		/* Where the new search starts */
	int				 g_notempty = 0;	/* If the match should not be empty */
	char			*match,				/* The current match */
					*last_match;		/* Location of last match */

	/* Get function parameters and do error checking */	
	argc = ARG_COUNT(ht);
	if (argc < 1 || argc > 4 || getParametersEx(argc, &regex, &subject, &limit, &flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if (argc == 3) {
		convert_to_long_ex(limit);
		limit_val = (*limit)->value.lval;
	}
	else
		limit_val = -1;
	
	if (argc == 4) {
		convert_to_long_ex(flags);
		no_empty = (*flags)->value.lval & PREG_SPLIT_NO_EMPTY;
	}
	
	/* Make sure we're dealing with strings */
	convert_to_string_ex(regex);
	convert_to_string_ex(subject);
	
	/* Compile regex or get it from cache. */
	if ((re = _pcre_get_compiled_regex((*regex)->value.str.val, extra, &preg_options)) == NULL) {
		RETURN_FALSE;
	}
	
	/* Initialize return value */
	array_init(return_value);

	/* Calculate the size of the offsets array, and allocate memory for it. */
	size_offsets = (pcre_info(re, NULL, NULL) + 1) * 3;
	offsets = (int *)emalloc(size_offsets * sizeof(int));
	
	/* Start at the beginning of the string */
	start_offset = 0;
	last_match = (*subject)->value.str.val;
	match = NULL;
	
	/* Get next piece if no limit or limit not yet reached and something matched*/
	while ((limit_val == -1 || limit_val > 1)) {
		count = pcre_exec(re, extra, (*subject)->value.str.val,
						  (*subject)->value.str.len, start_offset,
						  exoptions|g_notempty, offsets, size_offsets);

		/* Check for too many substrings condition. */
		if (count == 0) {
			zend_error(E_NOTICE, "Matched, but too many substrings\n");
			count = size_offsets/3;
		}
				
		/* If something matched */
		if (count > 0) {
			match = (*subject)->value.str.val + offsets[0];

			if (!no_empty || &(*subject)->value.str.val[offsets[0]] != last_match)
				/* Add the piece to the return value */
				add_next_index_stringl(return_value, last_match,
									   &(*subject)->value.str.val[offsets[0]]-last_match, 1);
			
			last_match = &(*subject)->value.str.val[offsets[1]];
			
			/* One less left to do */
			if (limit_val != -1)
				limit_val--;
		} else { /* Failed to match */
			/* If we previously set PCRE_NOTEMPTY after a null match,
			   this is not necessarily the end. We need to advance
			   the start offset, and continue. Fudge the offset values
			   to achieve this, unless we're already at the end of the string. */
			if (g_notempty != 0 && start_offset < (*subject)->value.str.len) {
				offsets[0] = start_offset;
				offsets[1] = start_offset + 1;
			} else
				break;
		}

		/* If we have matched an empty string, mimic what Perl's /g options does.
		   This turns out to be rather cunning. First we set PCRE_NOTEMPTY and try
		   the match again at the same point. If this fails (picked up above) we
		   advance to the next character. */
		g_notempty = (offsets[1] == offsets[0])? PCRE_NOTEMPTY : 0;
		
		/* Advance to the position right after the last full match */
		start_offset = offsets[1];
	}
	
	if (!no_empty || start_offset != (*subject)->value.str.len)
		/* Add the last piece to the return value */
		add_next_index_string(return_value,
							  &(*subject)->value.str.val[start_offset], 1);
	
	/* Clean up */
	efree(offsets);
}
/* }}} */


/* {{{ proto string preg_quote(string str)
   Quote regular expression characters */
PHP_FUNCTION(preg_quote)
{
	zval    **in_str_arg;	/* Input string argument */
	char 	*in_str,		/* Input string */
			*out_str,		/* Output string with quoted characters */
		 	*p,				/* Iterator for input string */
			*q,				/* Iterator for output string */
		 	 c;				/* Current character */
	
	/* Get the arguments and check for errors */
	if (ARG_COUNT(ht) != 1 || getParametersEx(1, &in_str_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	/* Make sure we're working with strings */
	convert_to_string_ex(in_str_arg);
	in_str = (*in_str_arg)->value.str.val;

	/* Nothing to do if we got an empty string */
	if (!*in_str) {
		RETVAL_STRING(empty_string, 0);
	}
	
	/* Allocate enough memory so that even if each character
	   is quoted, we won't run out of room */
	out_str = emalloc(2 * (*in_str_arg)->value.str.len + 1);
	
	/* Go through the string and quote necessary characters */
	for(p = in_str, q = out_str; (c = *p); p++) {
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
				/* break is missing _intentionally_ */
			default:
				*q++ = c;
		}
	}
	*q = '\0';
	
	/* Reallocate string and return it */
	RETVAL_STRING(erealloc(out_str, q - out_str + 1), 0);
}
/* }}} */


/* {{{ proto array preg_grep(string regex, array input)
   Searches array and returns entries which match regex */
PHP_FUNCTION(preg_grep)
{
	zval		   **regex,				/* Regular expression */
				   **input,				/* Input array */
					*entry,				/* A copy of the entry in the input array */
			   	   **entry_ptr;			/* An entry in the input array */
	pcre			*re = NULL;			/* Compiled regular expression */
	pcre_extra		*extra = NULL;		/* Holds results of studying */
	int			 	 preg_options = 0;	/* Custom preg options */
	int			 	*offsets;			/* Array of subpattern offsets */
	int			 	 size_offsets;		/* Size of the offsets array */
	int			 	 count = 0;			/* Count of matched subpatterns */
	char			*string_key;
	ulong			 num_key;
	
	/* Get arguments and do error checking */
	
	if (ARG_COUNT(ht) != 2 || getParametersEx(ARG_COUNT(ht), &regex, &input) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if ((*input)->type != IS_ARRAY) {
		zend_error(E_WARNING, "Secong argument to preg_grep() should be an array");
		return;
	}
	
	/* Make sure regex is a string */
	convert_to_string_ex(regex);
	
	/* Compile regex or get it from cache. */
	if ((re = _pcre_get_compiled_regex((*regex)->value.str.val, extra, &preg_options)) == NULL) {
		RETURN_FALSE;
	}

	/* Calculate the size of the offsets array, and allocate memory for it. */
	size_offsets = (pcre_info(re, NULL, NULL) + 1) * 3;
	offsets = (int *)emalloc(size_offsets * sizeof(int));
	
	/* Initialize return array */
	array_init(return_value);

	/* Allocate entry storage */
	entry = (zval *)emalloc(sizeof(zval));
		
	/* Go through the input array */
	zend_hash_internal_pointer_reset((*input)->value.ht);
	while(zend_hash_get_current_data((*input)->value.ht, (void **)&entry_ptr) == SUCCESS) {

		/* Copy entry and convert to string */
		*entry = **entry_ptr;
		zval_copy_ctor(entry);
		convert_to_string(entry);
		
		/* Perform the match */
		count = pcre_exec(re, extra, entry->value.str.val,
						  entry->value.str.len, 0,
						  0, offsets, size_offsets);

		/* Check for too many substrings condition. */
		if (count == 0) {
			zend_error(E_NOTICE, "Matched, but too many substrings\n");
			count = size_offsets/3;
		}

		/* If something matched */
		if (count > 0) {
			(*entry_ptr)->refcount++;

			/* Add to return array */
			switch(zend_hash_get_current_key((*input)->value.ht, &string_key, &num_key))
			{
				case HASH_KEY_IS_STRING:
					zend_hash_update(return_value->value.ht, string_key,
									 strlen(string_key)+1, entry_ptr, sizeof(zval *), NULL);
					efree(string_key);
					break;

				case HASH_KEY_IS_LONG:
					zend_hash_next_index_insert(return_value->value.ht, entry_ptr,
												sizeof(zval *), NULL);
					break;
			}
		}
		
		zval_dtor(entry);
		zend_hash_move_forward((*input)->value.ht);
	}
	
	/* Clean up */
	efree(entry);
	efree(offsets);
}
/* }}} */


/* {{{ module definition structures */

unsigned char third_arg_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };

function_entry pcre_functions[] = {
	PHP_FE(preg_match,		third_arg_force_ref)
	PHP_FE(preg_match_all,	third_arg_force_ref)
	PHP_FE(preg_replace,	NULL)
	PHP_FE(preg_split,		NULL)
	PHP_FE(preg_quote,		NULL)
	PHP_FE(preg_grep,		NULL)
	{NULL, 		NULL, 		NULL}
};

zend_module_entry pcre_module_entry = {
   "PCRE", pcre_functions,
		   PHP_MINIT(pcre),
		   PHP_MSHUTDOWN(pcre),
		   PHP_RINIT(pcre),
		   NULL,
		   PHP_MINFO(pcre),
		   STANDARD_MODULE_PROPERTIES
};

/* }}} */


#endif /* HAVE_PCRE || HAVE_BUNDLED_PCRE */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
