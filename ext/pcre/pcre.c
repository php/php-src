/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
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
   | Authors: Andrey Zmievski <andrey@ispi.net>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* Get PCRE library from ftp://ftp.cus.cam.ac.uk/pub/software/programs/pcre/ */

#include "php.h"

#if HAVE_LIBPCRE

#include "php_pcre.h"

/* {{{ module definition structures */

unsigned char third_arg_force_ref[] = { 3, BYREF_NONE, BYREF_NONE, BYREF_FORCE };

function_entry pcre_functions[] = {
	PHP_FE(pcre_match,		third_arg_force_ref)
	PHP_FE(pcre_replace,	NULL)
	{NULL, 		NULL, 		NULL}
};

php3_module_entry pcre_module_entry = {
   "PCRE", pcre_functions, php_minit_pcre, php_mshutdown_pcre,
		   php_rinit_pcre, NULL,
		   php_info_pcre, STANDARD_MODULE_PROPERTIES
};

/* }}} */

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


static void _php_free_pcre_cache(void *data)
{
	pcre_cache_entry *pce = (pcre_cache_entry *) data;
	pefree(pce->re, 1);
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


/* {{{ void php_info_pcre(ZEND_MODULE_INFO_FUNC_ARGS) */
void php_info_pcre(ZEND_MODULE_INFO_FUNC_ARGS)
{
	php3_printf("Perl Compatible Regular Expressions");
	php3_printf("<table cellpadding=5>"
				"<tr><td>PCRE library version:</td>"
				"<td>%s</td></tr>"
				"</table>", pcre_version());
}
/* }}} */


/* {{{ int php_minit_pcre(INIT_FUNC_ARGS) */
int php_minit_pcre(INIT_FUNC_ARGS)
{
#ifdef ZTS
	pcre_globals_id = tsrm_allocate_id(
							sizeof(php_pcre_globals),
							_php_pcre_init_globals,
							_php_pcre_shutdown_globals);
#else
	zend_hash_init(&PCRE_G(pcre_cache), 0, NULL, _php_free_pcre_cache, 1);
#endif
	return SUCCESS;
}
/* }}} */


/* {{{ int php_mshutdown_pcre(void) */
int php_mshutdown_pcre(SHUTDOWN_FUNC_ARGS)
{
#ifndef ZTS
	zend_hash_destroy(&PCRE_G(pcre_cache));
#endif
	return SUCCESS;
}
/* }}} */


/* {{{ int php_rinit_pcre(INIT_FUNC_ARGS) */
int php_rinit_pcre(INIT_FUNC_ARGS)
{
	pcre_malloc = php_pcre_malloc;
	pcre_free = php_pcre_free;
	
	return SUCCESS;
}
/* }}} */


/* {{{ static pcre* _pcre_get_compiled_regex(char *regex, pcre_extra *extra) */
static pcre* _pcre_get_compiled_regex(char *regex, pcre_extra *extra) {
	pcre			*re = NULL;
	int				 coptions = 0;
	int				 soptions = 0;
	const char	 	*error;
	int			 	 erroffset;
	char		 	 delimiter;
	char 			*p, *pp;
	char			*pattern;
	int				 regex_len;
	int				 do_study = 0;
	pcre_cache_entry	*pce;
	pcre_cache_entry	new_entry;
	PCRE_LS_FETCH();

	/* Try to lookup the cached regex entry, and if successful, just pass
	   back the compiled pattern, otherwise go on and compile it. */
	regex_len = strlen(regex);
	if (zend_hash_find(&PCRE_G(pcre_cache), regex, regex_len+1, (void **)&pce) == SUCCESS) {
		extra = pce->extra;
		return pce->re;
	}
	
	p = regex;
	
	/* Parse through the leading whitespace, and display a warning if we
	   get to the end without encountering a delimiter. */
	while (isspace(*p)) p++;
	if (*p == 0) {
		zend_error(E_WARNING, "Empty regular expression");
		return NULL;
	}
	
	/* Get the delimiter and display a warning if it is alphanumeric
	   or a backslash. */
	delimiter = *p++;
	if (isalnum(delimiter) || delimiter == '\\') {
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

	/* Parse through the options, setting appropriate flags.  Display
	   a warning if we encounter an unknown option. */	
	while (*pp != 0) {
		switch (*pp++) {
			case 'i':	coptions |= PCRE_CASELESS;		break;
			case 'm':	coptions |= PCRE_MULTILINE;		break;
			case 's':	coptions |= PCRE_DOTALL;		break;
			case 'x':	coptions |= PCRE_EXTENDED;		break;
			
			case 'A':	coptions |= PCRE_ANCHORED;		break;
			case 'D':	coptions |= PCRE_DOLLAR_ENDONLY;break;
			case 'S':	do_study  = 1;					break;
			case 'U':	coptions |= PCRE_UNGREEDY;		break;
			case 'X':	coptions |= PCRE_EXTRA;			break;

			case ' ':
			case '\n':
				break;

			default:
				zend_error(E_WARNING, "Unknown option '%c'", pp[-1]);
				efree(pattern);
				return NULL;
		}
	}

	/* Compile pattern and display a warning if compilation failed. */
	re = pcre_compile(pattern,
					  coptions,
					  &error,
					  &erroffset,
					  NULL);

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

	efree(pattern);

	/* Store the compiled pattern and extra info in the cache. */
	new_entry.re = re;
	new_entry.extra = extra;
	zend_hash_update(&PCRE_G(pcre_cache), regex, regex_len+1, (void *)&new_entry,
						sizeof(pcre_cache_entry), NULL);

	return re;
}
/* }}} */


/* {{{ proto pcre_match(string pattern, string subject [, array subpatterns ])
   Perform a Perl-style regular expression match */
PHP_FUNCTION(pcre_match)
{
	zval			*regex,				/* Regular expression */
					*subject,			/* String to match against */
					*subpats = NULL;	/* Array for subpatterns */
	pcre			*re = NULL;			/* Compiled regular expression */
	pcre_extra		*extra = NULL;		/* Holds results of studying */
	int			 	 exoptions = 0;		/* Execution options */
	int			 	 count;				/* Count of matched subpatterns */
	int			 	*offsets;			/* Array of subpattern offsets */
	int			 	 size_offsets;		/* Size of the offsets array */
	int			 	 matched;			/* Has anything matched */
	int				 i;
	const char	   **stringlist;		/* Used to hold list of subpatterns */

	/* Get function parameters and do error-checking. */
	switch(ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &regex, &subject) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
			
		case 3:
			if (getParameters(ht, 3, &regex, &subject, &subpats) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			if (!ParameterPassedByReference(ht, 3)) {
				zend_error(E_WARNING, "Array to be filled with matches must be passed by reference.");
				RETURN_FALSE;
			}
			break;
			
		default:
			WRONG_PARAM_COUNT;
	}

	/* Make sure we're dealing with strings. */
	convert_to_string(regex);
	convert_to_string(subject);

	/* Compile regex or get it from cache. */
	if ((re = _pcre_get_compiled_regex(regex->value.str.val, extra)) == NULL)
		return;
	
	/* Calculate the size of the offsets array, and allocate memory for it. */
	size_offsets = (pcre_info(re, NULL, NULL) + 1) * 3;
	offsets = (int *)emalloc(size_offsets * sizeof(int));
	
	/* Execute the regular expression. */
	count = pcre_exec(re, extra, subject->value.str.val, subject->value.str.len,
					  exoptions, offsets, size_offsets);

	/* Check for too many substrings condition. */	
	if (count == 0) {
		zend_error(E_NOTICE, "Matched, but too many substrings\n");
		count = size_offsets/3;
	}
	
	/* If something has matched */
	if (count >= 0) {
		matched = 1;

		/* If subpatters array has been passed, fill it in with values. */
		if (subpats != NULL) {
			/* Try to get the list of substrings and display a warning if failed. */
			if (pcre_get_substring_list(subject->value.str.val, offsets, count, &stringlist) < 0) {
				efree(offsets);
				efree(re);
				zend_error(E_WARNING, "Get subpatterns list failed");
				return;
			}

			/* Make sure to clean up the passed array and initialize it. */
			zval_dtor(subpats);
			array_init(subpats);

			/* For each subpattern, insert it into the subpatterns array. */
			for (i=0; i<count; i++) {
				add_next_index_string(subpats, (char *)stringlist[i], 1);
			}
			
			php_pcre_free(stringlist);
		}
	}
	/* If nothing matched */
	else {
		matched = 0;

		/* Make sure to clean up the passed array and initialize it
		   to empty since we don't want to leave previous values in it. */
		if (subpats != NULL) {
			zval_dtor(subpats);
			array_init(subpats);
		}
	}

	efree(offsets);

	RETVAL_LONG(matched);
}
/* }}} */

/* {{{ int _pcre_get_backref(const char *walk, int *backref) */
static int _pcre_get_backref(const char *walk, int *backref)
{
	if (*walk < '0' && *walk > '9')
		return 0;
	else
		*backref = *walk - '0';
	
	if (walk[1] >= '0' && walk[1] <= '9')
		*backref = *backref * 10 + walk[1] - '0';

	return 1;	
}
/* }}} */


/* {{{ char *_php_pcre_replace(char *regex, char *subject, char *replace) */
char *_php_pcre_replace(char *regex, char *subject, char *replace)
{
	pcre			*re = NULL;			/* Compiled regular expression */
	pcre_extra		*extra = NULL;		/* Holds results of studying */
	int			 	 exoptions = 0;		/* Execution options */
	int			 	 count = 0;			/* Count of matched subpatterns */
	int			 	*offsets;			/* Array of subpattern offsets */
	int			 	 size_offsets;		/* Size of the offsets array */
	int				 new_len;			/* Length of needed storage */
	int				 alloc_len;			/* Actual allocated length */
	int				 subject_len;		/* Length of the subject string */
	int				 result_len;		/* Current length of the result */
	int				 subject_offset;	/* Current position in the subject string */
	int				 backref;			/* Backreference number */
	char			*result,			/* Result of replacement */
					*new_buf,			/* Temporary buffer for re-allocation */
					*walkbuf,			/* Location of current replacement in the result */
					*walk;				/* Used to walk the replacement string */

	/* Compile regex or get it from cache. */
	if ((re = _pcre_get_compiled_regex(regex, extra)) == NULL)
		return NULL;

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

	subject_offset = 0;
	result[0] = '\0';
	
	while (count >= 0) {
		/* Execute the regular expression. */
		count = pcre_exec(re, extra, &subject[subject_offset],
							subject_len-subject_offset,
						  	(subject_offset ? exoptions|PCRE_NOTBOL : exoptions),
							offsets, size_offsets);

		/* Check for too many substrings condition. */	
		if (count == 0) {
			zend_error(E_NOTICE, "Matched, but too many substrings\n");
			count = size_offsets/3;
		}

		if (count > 0) {
			new_len = strlen(result) + offsets[0]; /* part before the match */
			walk = replace;
			while (*walk)
				if ('\\' == *walk &&
					_pcre_get_backref(walk+1, &backref) &&
					backref < count) {
					new_len += offsets[(backref<<1)+1] - offsets[backref<<1];
					walk += (backref > 9) ? 3 : 2;
				} else {
					new_len++;
					walk++;
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
			strncat(result, &subject[subject_offset], offsets[0]);

			/* copy replacement and backrefs */
			walkbuf = &result[result_len + offsets[0]];
			walk = replace;
			while (*walk)
				if ('\\' == *walk &&
					_pcre_get_backref(walk+1, &backref) &&
					backref < count) {
					result_len = offsets[(backref<<1)+1] - offsets[backref<<1];
					memcpy (walkbuf,
							&subject[subject_offset + offsets[backref<<1]],
							result_len);
					walkbuf += result_len;
					walk += (backref > 9) ? 3 : 2;
				} else
					*walkbuf++ = *walk++;
			*walkbuf = '\0';

			/* and get ready to keep looking for replacements */
			if (offsets[0] == offsets[1]) {
				if (offsets[0] + subject_offset >= subject_len)
					break;
				new_len = strlen (result) + 1;
				if (new_len + 1 > alloc_len) {
					alloc_len = 1 + alloc_len + 2 * new_len;
					new_buf = emalloc(alloc_len * sizeof(char));
					strcpy(new_buf, result);
					efree(result);
					result = new_buf;
				}
				subject_offset += offsets[1] + 1;
				result [new_len-1] = subject[subject_offset-1];
				result [new_len] = '\0';
			} else {
				subject_offset += offsets[1];
			}
		} else {
			new_len = strlen(result) + strlen(&subject[subject_offset]);
			if (new_len + 1 > alloc_len) {
				alloc_len = new_len + 1; /* now we know exactly how long it is */
				new_buf = emalloc(alloc_len * sizeof(char));
				strcpy(new_buf, result);
				efree(result);
				result = new_buf;
			}
			/* stick that last bit of string on our output */
			strcat(result, &subject[subject_offset]);
		}
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
				*replace_entry;
	char		*replace_value,
				*subject_value,
				*result;

	/* Make sure we're dealing with strings. */	
	convert_to_string(subject);
	
	/* If regex is an array */
	if (regex->type == IS_ARRAY) {
		/* Duplicating subject string for repeated replacement */
		subject_value = estrdup(subject->value.str.val);
		
		zend_hash_internal_pointer_reset(regex->value.ht);

		if (replace->type == IS_ARRAY)
			zend_hash_internal_pointer_reset(replace->value.ht);
		else
			/* Set replacement value to the passed one */
			replace_value = replace->value.str.val;
		
		/* For each entry in the regex array, get the entry */
		while (zend_hash_get_current_data(regex->value.ht, (void **)&regex_entry_ptr) == SUCCESS) {
			regex_entry = *regex_entry_ptr;

			/* Make sure we're dealing with strings. */	
			convert_to_string(regex_entry);
		
			/* If replace is an array */
			if (replace->type == IS_ARRAY) {
				/* Get current entry */
				if (zend_hash_get_current_data(replace->value.ht, (void **)&replace_entry_ptr) == SUCCESS) {
					replace_entry = *replace_entry_ptr;
					
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
			
			zend_hash_move_forward(regex->value.ht);
		}

		return subject_value;
	}
	else {
		/* Make sure we're dealing with strings and do the replacement */
		convert_to_string(replace);
		result = _php_pcre_replace(regex->value.str.val,
									subject->value.str.val,
									replace->value.str.val);
		return result;
	}
}


/* {{{ proto pcre_replace(string|array regex, string|array replace, string|array subject)
    Perform Perl-style regular expression replacement */
PHP_FUNCTION(pcre_replace)
{
	zval			*regex,
					*replace,
					*subject,
					**subject_entry_ptr,
					*subject_entry,
					*return_entry;
	char			*result;
	
	/* Get function parameters and do error-checking. */
	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &regex, &replace, &subject) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* if subject is an array */
	if (subject->type == IS_ARRAY) {
		array_init(return_value);
		zend_hash_internal_pointer_reset(subject->value.ht);
		
		/* For each subject entry, convert it to string, then perform replacement
		   and add the result to the return_value array. */
		while (zend_hash_get_current_data(subject->value.ht, (void **)&subject_entry_ptr) == SUCCESS) {
			subject_entry = *subject_entry_ptr;
			
			if ((result = _php_replace_in_subject(regex, replace, subject_entry)) != NULL)
				add_next_index_string(return_value, result, 0);
		
			zend_hash_move_forward(subject->value.ht);
		}
	}
	else {	/* if subject is not an array */
		result = _php_replace_in_subject(regex, replace, subject);
		RETVAL_STRING(result, 1);
		efree(result);
	}	
}
/* }}} */


#endif /* HAVE_PCRE */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
