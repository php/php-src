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
   "PCRE", pcre_functions, NULL, NULL, php_rinit_pcre, NULL,
		   php_info_pcre, STANDARD_MODULE_PROPERTIES
};

/* }}} */


static void *php_pcre_malloc(size_t size)
{
	return emalloc(size);
}

static void php_pcre_free(void *ptr)
{
	efree(ptr);
}


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
	unsigned char 	*p, *pp;
	char			*pattern;
	int				 do_study = 0;

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
		
	return re;
}
/* }}} */


/* {{{ proto pcre_match(string pattern, string subject [, array subpatterns ])
   Perform a Perl-style regular expression match */
PHP_FUNCTION(pcre_match)
{
	zval			*regex,
					*subject,
					*subpats = NULL;
	pcre			*re = NULL;
	pcre_extra		*extra = NULL;
	int			 	 exoptions = 0;
	const char	 	*error;
	int			 	 count;
	int			 	*offsets;
	int			 	 size_offsets;
	int			 	 matched;
	int				 i;
	zval			*entry;
	const char	   **stringlist;

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

			/* For each subpattern, allocate an array entry, initialize it,
			   and fill in the matched pattern. Then insert it into the
			   subpatterns array. */
			for (i=0; i<count; i++) {
				entry = (zval *)emalloc(sizeof(zval));
				entry->type = IS_STRING;
				entry->value.str.val = estrdup(stringlist[i]);
				entry->value.str.len = offsets[(i<<1)+1] - offsets[i<<1];
				entry->is_ref = 0;
				entry->refcount = 1;

				zend_hash_index_update(subpats->value.ht, i, &entry, sizeof(zval *), NULL);
			}
			
			efree(stringlist);
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
	efree(re);

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


/* {{{ proto pcre_replace()
    */
PHP_FUNCTION(pcre_replace)
{
	zval			*regex,
					*replace,
					*subject;
	pcre			*re = NULL;
	pcre_extra		*extra = NULL;
	int			 	 exoptions = 0;
	const char	 	*error;
	int			 	 count = 0;
	int			 	*offsets;
	int			 	 size_offsets;
	int			 	 matched;
	const char	  ***stringlist;
	int				 new_len;
	int				 alloc_len;
	int				 subject_len;
	int				 subject_offset;
	int				 result_len;
	int				 backref;
	char			*result,
					*new_buf,
					*walkbuf,
					*walk;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &regex, &replace, &subject) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* Make sure we're dealing with strings. */
	convert_to_string(regex);
	convert_to_string(replace);	
	convert_to_string(subject);

	/* Compile regex or get it from cache. */
	if ((re = _pcre_get_compiled_regex(regex->value.str.val, extra)) == NULL)
		return;

	/* Calculate the size of the offsets array, and allocate memory for it. */
	size_offsets = (pcre_info(re, NULL, NULL) + 1) * 3;
	offsets = (int *)emalloc(size_offsets * sizeof(int));
	
	subject_len = subject->value.str.len;

	alloc_len = 2 * subject_len + 1;
	result = emalloc(alloc_len * sizeof(char));
	if (!result) {
		zend_error(E_WARNING, "Unable to allocate memory in pcre_replace");
		efree(re);
		efree(offsets);
		return;
	}

	subject_offset = 0;
	result[0] = '\0';
	
	while (count >= 0) {
		/* Execute the regular expression. */
		count = pcre_exec(re, extra, &subject->value.str.val[subject_offset],
							subject->value.str.len-subject_offset,
						  	(subject_offset ? PCRE_NOTBOL : 0), offsets, size_offsets);

		/* Check for too many substrings condition. */	
		if (count == 0) {
			zend_error(E_NOTICE, "Matched, but too many substrings\n");
			count = size_offsets/3;
		}

		if (count > 0) {
			new_len = strlen(result) + offsets[0]; /* part before the match */
			walk = replace->value.str.val;
			while (*walk)
				if ('\\' == *walk &&
					_pcre_get_backref(walk+1, &backref) &&
					backref < count) {
					new_len += offsets[2*backref+1] - offsets[2*backref];
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
			strncat(result, &subject->value.str.val[subject_offset], offsets[0]);

			/* copy replacement and backrefs */
			walkbuf = &result[result_len + offsets[0]];
			walk = replace->value.str.val;
			while (*walk)
				if ('\\' == *walk &&
					_pcre_get_backref(walk+1, &backref) &&
					backref < count) {
					result_len = offsets[2*backref+1] - offsets[2*backref];
					memcpy (walkbuf,
							&subject->value.str.val[subject_offset + offsets[2*backref]],
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
				result [new_len-1] = subject->value.str.val [subject_offset-1];
				result [new_len] = '\0';
			} else {
				subject_offset += offsets[1];
			}
		} else { /* REG_NOMATCH */
			new_len = strlen(result) + strlen(&subject->value.str.val[subject_offset]);
			if (new_len + 1 > alloc_len) {
				alloc_len = new_len + 1; /* now we know exactly how long it is */
				new_buf = emalloc(alloc_len * sizeof(char));
				strcpy(new_buf, result);
				efree(result);
				result = new_buf;
			}
			/* stick that last bit of string on our output */
			strcat(result, &subject->value.str.val[subject_offset]);
		}
	}
	
	efree(offsets);
	efree(re);
	
	RETVAL_STRING(result, 1);
	efree(result);
}
/* }}} */


#endif /* HAVE_PCRE */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
