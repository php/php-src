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
   | Author: Clayton Collie <clcollie@mindspring.com>                     |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/*
	scanf.c --

	This file contains the base code which implements sscanf and by extension
	fscanf. Original code is from TCL8.3.0 and bears the following copyright:

	This software is copyrighted by the Regents of the University of
	California, Sun Microsystems, Inc., Scriptics Corporation,
	and other parties.  The following terms apply to all files associated
	with the software unless explicitly disclaimed in individual files.

	The authors hereby grant permission to use, copy, modify, distribute,
	and license this software and its documentation for any purpose, provided
	that existing copyright notices are retained in all copies and that this
	notice is included verbatim in any distributions. No written agreement,
	license, or royalty fee is required for any of the authorized uses.
	Modifications to this software may be copyrighted by their authors
	and need not follow the licensing terms described here, provided that
	the new terms are clearly indicated on the first page of each file where
	they apply.

	IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
	FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
	ARISING OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY
	DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.

	THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE
	IS PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE
	NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
	MODIFICATIONS.

	GOVERNMENT USE: If you are acquiring this software on behalf of the
	U.S. government, the Government shall have only "Restricted Rights"
	in the software and related documentation as defined in the Federal
	Acquisition Regulations (FARs) in Clause 52.227.19 (c) (2).  If you
	are acquiring the software on behalf of the Department of Defense, the
	software shall be classified as "Commercial Computer Software" and the
	Government shall have only "Restricted Rights" as defined in Clause
	252.227-7013 (c) (1) of DFARs.  Notwithstanding the foregoing, the
	authors grant the U.S. Government and others acting in its behalf
	permission to use and distribute the software in accordance with the
	terms specified in this license.
*/

#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include "php.h"
#include "php_variables.h"
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif
#include "zend_execute.h"
#include "zend_operators.h"
#include "zend_strtod.h"
#include "php_globals.h"
#include "basic_functions.h"
#include "scanf.h"

/*
 * Flag values used internally by [f|s]canf.
 */
#define SCAN_NOSKIP     0x1       /* Don't skip blanks. */
#define SCAN_SUPPRESS	0x2	  /* Suppress assignment. */
#define SCAN_UNSIGNED	0x4	  /* Read an unsigned value. */
#define SCAN_WIDTH      0x8       /* A width value was supplied. */

#define SCAN_SIGNOK     0x10      /* A +/- character is allowed. */
#define SCAN_NODIGITS   0x20      /* No digits have been scanned. */
#define SCAN_NOZERO     0x40      /* No zero digits have been scanned. */
#define SCAN_XOK        0x80      /* An 'x' is allowed. */
#define SCAN_PTOK       0x100     /* Decimal point is allowed. */
#define SCAN_EXPOK      0x200     /* An exponent is allowed. */

#define UCHAR(x)		(zend_uchar)(x)

/*
 * The following structure contains the information associated with
 * a character set.
 */
typedef struct CharSet {
	int exclude;		/* 1 if this is an exclusion set. */
	int nchars;
	char *chars;
	int nranges;
	struct Range {
		char start;
		char end;
	} *ranges;
} CharSet;

/*
 * Declarations for functions used only in this file.
 */
static char *BuildCharSet(CharSet *cset, char *format);
static int	CharInSet(CharSet *cset, int ch);
static void	ReleaseCharSet(CharSet *cset);
static inline void scan_set_error_return(int numVars, zval *return_value);


/* {{{ BuildCharSet
 *----------------------------------------------------------------------
 *
 * BuildCharSet --
 *
 *	This function examines a character set format specification
 *	and builds a CharSet containing the individual characters and
 *	character ranges specified.
 *
 * Results:
 *	Returns the next format position.
 *
 * Side effects:
 *	Initializes the charset.
 *
 *----------------------------------------------------------------------
 */
static char * BuildCharSet(CharSet *cset, char *format)
{
	char *ch, start;
	int  nranges;
	char *end;

	memset(cset, 0, sizeof(CharSet));

	ch = format;
	if (*ch == '^') {
		cset->exclude = 1;
		ch = ++format;
	}
	end = format + 1;	/* verify this - cc */

	/*
	 * Find the close bracket so we can overallocate the set.
	 */
	if (*ch == ']') {
		ch = end++;
	}
	nranges = 0;
	while (*ch != ']') {
		if (*ch == '-') {
			nranges++;
		}
		ch = end++;
	}

	cset->chars = (char *) safe_emalloc(sizeof(char), (end - format - 1), 0);
	if (nranges > 0) {
		cset->ranges = (struct Range *) safe_emalloc(sizeof(struct Range), nranges, 0);
	} else {
		cset->ranges = NULL;
	}

	/*
	 * Now build the character set.
	 */
	cset->nchars = cset->nranges = 0;
	ch    = format++;
	start = *ch;
	if (*ch == ']' || *ch == '-') {
		cset->chars[cset->nchars++] = *ch;
		ch = format++;
	}
	while (*ch != ']') {
		if (*format == '-') {
			/*
			 * This may be the first character of a range, so don't add
			 * it yet.
			 */
			start = *ch;
		} else if (*ch == '-') {
			/*
			 * Check to see if this is the last character in the set, in which
			 * case it is not a range and we should add the previous character
			 * as well as the dash.
			 */
			if (*format == ']') {
				cset->chars[cset->nchars++] = start;
				cset->chars[cset->nchars++] = *ch;
			} else {
				ch = format++;

				/*
				 * Check to see if the range is in reverse order.
				 */
				if (start < *ch) {
					cset->ranges[cset->nranges].start = start;
					cset->ranges[cset->nranges].end = *ch;
				} else {
					cset->ranges[cset->nranges].start = *ch;
					cset->ranges[cset->nranges].end = start;
				}
				cset->nranges++;
			}
		} else {
			cset->chars[cset->nchars++] = *ch;
		}
		ch = format++;
	}
	return format;
}
/* }}} */

/* {{{ CharInSet
 *----------------------------------------------------------------------
 *
 * CharInSet --
 *
 *	Check to see if a character matches the given set.
 *
 * Results:
 *	Returns non-zero if the character matches the given set.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
static int CharInSet(CharSet *cset, int c)
{
	char ch = (char) c;
	int i, match = 0;

	for (i = 0; i < cset->nchars; i++) {
		if (cset->chars[i] == ch) {
			match = 1;
			break;
		}
	}
	if (!match) {
		for (i = 0; i < cset->nranges; i++) {
			if ((cset->ranges[i].start <= ch)
				&& (ch <= cset->ranges[i].end)) {
				match = 1;
				break;
			}
		}
	}
	return (cset->exclude ? !match : match);
}
/* }}} */

/* {{{ ReleaseCharSet
 *----------------------------------------------------------------------
 *
 * ReleaseCharSet --
 *
 *	Free the storage associated with a character set.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
static void ReleaseCharSet(CharSet *cset)
{
	efree((char *)cset->chars);
	if (cset->ranges) {
		efree((char *)cset->ranges);
	}
}
/* }}} */

/* {{{ ValidateFormat
 *----------------------------------------------------------------------
 *
 * ValidateFormat --
 *
 *	Parse the format string and verify that it is properly formed
 *	and that there are exactly enough variables on the command line.
 *
 * Results:
 *    FAILURE or SUCCESS.
 *
 * Side effects:
 *     May set php_error based on abnormal conditions.
 *
 * Parameters :
 *     format     The format string.
 *     numVars    The number of variables passed to the scan command.
 *     totalSubs  The number of variables that will be required.
 *
 *----------------------------------------------------------------------
*/
PHPAPI int ValidateFormat(char *format, int numVars, int *totalSubs)
{
#define STATIC_LIST_SIZE 16
	int gotXpg, gotSequential, value, i, flags;
	char *end, *ch = NULL;
	int staticAssign[STATIC_LIST_SIZE];
	int *nassign = staticAssign;
	int objIndex, xpgSize, nspace = STATIC_LIST_SIZE;

	/*
	 * Initialize an array that records the number of times a variable
	 * is assigned to by the format string.  We use this to detect if
	 * a variable is multiply assigned or left unassigned.
	 */
	if (numVars > nspace) {
		nassign = (int*)safe_emalloc(sizeof(int), numVars, 0);
		nspace = numVars;
	}
	for (i = 0; i < nspace; i++) {
		nassign[i] = 0;
	}

	xpgSize = objIndex = gotXpg = gotSequential = 0;

	while (*format != '\0') {
		ch = format++;
		flags = 0;

		if (*ch != '%') {
			continue;
		}
		ch = format++;
		if (*ch == '%') {
			continue;
		}
		if (*ch == '*') {
			flags |= SCAN_SUPPRESS;
			ch = format++;
			goto xpgCheckDone;
		}

		if ( isdigit( (int)*ch ) ) {
			/*
			 * Check for an XPG3-style %n$ specification.  Note: there
			 * must not be a mixture of XPG3 specs and non-XPG3 specs
			 * in the same format string.
			 */
			value = ZEND_STRTOUL(format-1, &end, 10);
			if (*end != '$') {
				goto notXpg;
			}
			format = end+1;
			ch     = format++;
			gotXpg = 1;
			if (gotSequential) {
				goto mixedXPG;
			}
			objIndex = value - 1;
			if ((objIndex < 0) || (numVars && (objIndex >= numVars))) {
				goto badIndex;
			} else if (numVars == 0) {
				/*
				 * In the case where no vars are specified, the user can
				 * specify %9999$ legally, so we have to consider special
				 * rules for growing the assign array.  'value' is
				 * guaranteed to be > 0.
				 */

				/* set a lower artificial limit on this
				 * in the interest of security and resource friendliness
				 * 255 arguments should be more than enough. - cc
				 */
				if (value > SCAN_MAX_ARGS) {
					goto badIndex;
				}

				xpgSize = (xpgSize > value) ? xpgSize : value;
			}
			goto xpgCheckDone;
		}

notXpg:
		gotSequential = 1;
		if (gotXpg) {
mixedXPG:
			php_error_docref(NULL, E_WARNING, "%s", "cannot mix \"%\" and \"%n$\" conversion specifiers");
			goto error;
		}

xpgCheckDone:
		/*
		 * Parse any width specifier.
		 */
		if (isdigit(UCHAR(*ch))) {
			value = ZEND_STRTOUL(format-1, &format, 10);
			flags |= SCAN_WIDTH;
			ch = format++;
		}

		/*
		 * Ignore size specifier.
		 */
		if ((*ch == 'l') || (*ch == 'L') || (*ch == 'h')) {
			ch = format++;
		}

		if (!(flags & SCAN_SUPPRESS) && numVars && (objIndex >= numVars)) {
			goto badIndex;
		}

		/*
		 * Handle the various field types.
		 */
		switch (*ch) {
			case 'n':
			case 'd':
			case 'D':
			case 'i':
			case 'o':
			case 'x':
			case 'X':
			case 'u':
			case 'f':
			case 'e':
			case 'E':
			case 'g':
			case 's':
				break;

			case 'c':
				/* we differ here with the TCL implementation in allowing for */
				/* a character width specification, to be more consistent with */
				/* ANSI. since Zend auto allocates space for vars, this is no */
				/* problem - cc                                               */
				/*
				if (flags & SCAN_WIDTH) {
					php_error_docref(NULL, E_WARNING, "Field width may not be specified in %c conversion");
					goto error;
				}
				*/
				break;

			case '[':
				if (*format == '\0') {
					goto badSet;
				}
				ch = format++;
				if (*ch == '^') {
					if (*format == '\0') {
						goto badSet;
					}
					ch = format++;
				}
				if (*ch == ']') {
					if (*format == '\0') {
						goto badSet;
					}
					ch = format++;
				}
				while (*ch != ']') {
					if (*format == '\0') {
						goto badSet;
					}
					ch = format++;
				}
				break;
badSet:
				php_error_docref(NULL, E_WARNING, "Unmatched [ in format string");
				goto error;

			default: {
				php_error_docref(NULL, E_WARNING, "Bad scan conversion character \"%c\"", *ch);
				goto error;
			}
		}

		if (!(flags & SCAN_SUPPRESS)) {
			if (objIndex >= nspace) {
				/*
				 * Expand the nassign buffer.  If we are using XPG specifiers,
				 * make sure that we grow to a large enough size.  xpgSize is
				 * guaranteed to be at least one larger than objIndex.
				 */
				value = nspace;
				if (xpgSize) {
					nspace = xpgSize;
				} else {
					nspace += STATIC_LIST_SIZE;
				}
				if (nassign == staticAssign) {
					nassign = (void *)safe_emalloc(nspace, sizeof(int), 0);
					for (i = 0; i < STATIC_LIST_SIZE; ++i) {
						nassign[i] = staticAssign[i];
					}
				} else {
					nassign = (void *)erealloc((void *)nassign, nspace * sizeof(int));
				}
				for (i = value; i < nspace; i++) {
					nassign[i] = 0;
				}
			}
			nassign[objIndex]++;
			objIndex++;
		}
	} /* while (*format != '\0') */

	/*
	 * Verify that all of the variable were assigned exactly once.
	 */
	if (numVars == 0) {
		if (xpgSize) {
			numVars = xpgSize;
		} else {
			numVars = objIndex;
		}
	}
	if (totalSubs) {
		*totalSubs = numVars;
	}
	for (i = 0; i < numVars; i++) {
		if (nassign[i] > 1) {
			php_error_docref(NULL, E_WARNING, "%s", "Variable is assigned by multiple \"%n$\" conversion specifiers");
			goto error;
		} else if (!xpgSize && (nassign[i] == 0)) {
			/*
			 * If the space is empty, and xpgSize is 0 (means XPG wasn't
			 * used, and/or numVars != 0), then too many vars were given
			 */
			php_error_docref(NULL, E_WARNING, "Variable is not assigned by any conversion specifiers");
			goto error;
		}
	}

	if (nassign != staticAssign) {
		efree((char *)nassign);
	}
	return SCAN_SUCCESS;

badIndex:
	if (gotXpg) {
		php_error_docref(NULL, E_WARNING, "%s", "\"%n$\" argument index out of range");
	} else {
		php_error_docref(NULL, E_WARNING, "Different numbers of variable names and field specifiers");
	}

error:
	if (nassign != staticAssign) {
		efree((char *)nassign);
	}
	return SCAN_ERROR_INVALID_FORMAT;
#undef STATIC_LIST_SIZE
}
/* }}} */

/* {{{ php_sscanf_internal
 * This is the internal function which does processing on behalf of
 * both sscanf() and fscanf()
 *
 * parameters :
 * 		string		literal string to be processed
 * 		format		format string
 *		argCount	total number of elements in the args array
 *		args		arguments passed in from user function (f|s)scanf
 * 		varStart	offset (in args) of 1st variable passed in to (f|s)scanf
 *		return_value set with the results of the scan
 */

PHPAPI int php_sscanf_internal( char *string, char *format,
				int argCount, zval *args,
				int varStart, zval *return_value)
{
	int  numVars, nconversions, totalVars = -1;
	int  i, result;
	zend_long value;
	int  objIndex;
	char *end, *baseString;
	zval *current;
	char op   = 0;
	int  base = 0;
	int  underflow = 0;
	size_t width;
	zend_long (*fn)() = NULL;
	char *ch, sch;
	int  flags;
	char buf[64];	/* Temporary buffer to hold scanned number
					 * strings before they are passed to strtoul() */

	/* do some sanity checking */
	if ((varStart > argCount) || (varStart < 0)){
		varStart = SCAN_MAX_ARGS + 1;
	}
	numVars = argCount - varStart;
	if (numVars < 0) {
		numVars = 0;
	}

#if 0
	zend_printf("<br>in sscanf_internal : <br> string is \"%s\", format = \"%s\"<br> NumVars = %d. VarStart = %d<br>-------------------------<br>",
					string, format, numVars, varStart);
#endif
	/*
	 * Check for errors in the format string.
	 */
	if (ValidateFormat(format, numVars, &totalVars) != SCAN_SUCCESS) {
		scan_set_error_return( numVars, return_value );
		return SCAN_ERROR_INVALID_FORMAT;
	}

	objIndex = numVars ? varStart : 0;

	/*
	 * If any variables are passed, make sure they are all passed by reference
	 */
	if (numVars) {
		for (i = varStart;i < argCount;i++){
			if ( ! Z_ISREF(args[ i ] ) ) {
				php_error_docref(NULL, E_WARNING, "Parameter %d must be passed by reference", i);
				scan_set_error_return(numVars, return_value);
				return SCAN_ERROR_VAR_PASSED_BYVAL;
			}
		}
	}

	/*
	 * Allocate space for the result objects. Only happens when no variables
	 * are specified
	 */
	if (!numVars) {
		zval tmp;

		/* allocate an array for return */
		array_init(return_value);

		for (i = 0; i < totalVars; i++) {
			ZVAL_NULL(&tmp);
			if (add_next_index_zval(return_value, &tmp) == FAILURE) {
				scan_set_error_return(0, return_value);
				return FAILURE;
			}
		}
		varStart = 0; /* Array index starts from 0 */
	}

	baseString = string;

	/*
	 * Iterate over the format string filling in the result objects until
	 * we reach the end of input, the end of the format string, or there
	 * is a mismatch.
	 */
	nconversions = 0;
	/* note ! - we need to limit the loop for objIndex to keep it in bounds */

	while (*format != '\0') {
		ch    = format++;
		flags = 0;

		/*
		 * If we see whitespace in the format, skip whitespace in the string.
		 */
		if ( isspace( (int)*ch ) ) {
			sch = *string;
			while ( isspace( (int)sch ) ) {
				if (*string == '\0') {
					goto done;
				}
				string++;
				sch = *string;
			}
			continue;
		}

		if (*ch != '%') {
literal:
			if (*string == '\0') {
				underflow = 1;
				goto done;
			}
			sch = *string;
			string++;
			if (*ch != sch) {
				goto done;
			}
			continue;
		}

		ch = format++;
		if (*ch == '%') {
			goto literal;
		}

		/*
		 * Check for assignment suppression ('*') or an XPG3-style
		 * assignment ('%n$').
		 */
		if (*ch == '*') {
			flags |= SCAN_SUPPRESS;
			ch = format++;
		} else if ( isdigit(UCHAR(*ch))) {
			value = ZEND_STRTOUL(format-1, &end, 10);
			if (*end == '$') {
				format = end+1;
				ch = format++;
				objIndex = varStart + value - 1;
			}
		}

		/*
		 * Parse any width specifier.
		 */
		if ( isdigit(UCHAR(*ch))) {
			width = ZEND_STRTOUL(format-1, &format, 10);
			ch = format++;
		} else {
			width = 0;
		}

		/*
		 * Ignore size specifier.
		 */
		if ((*ch == 'l') || (*ch == 'L') || (*ch == 'h')) {
			ch = format++;
		}

		/*
		 * Handle the various field types.
		 */
		switch (*ch) {
			case 'n':
				if (!(flags & SCAN_SUPPRESS)) {
					if (numVars && objIndex >= argCount) {
						break;
					} else if (numVars) {
						current = Z_REFVAL(args[objIndex++]);
						zval_ptr_dtor(current);
						ZVAL_LONG(current, (zend_long)(string - baseString) );
					} else {
						add_index_long(return_value, objIndex++, string - baseString);
					}
				}
				nconversions++;
				continue;

			case 'd':
			case 'D':
				op = 'i';
				base = 10;
				fn = (zend_long (*)())ZEND_STRTOL_PTR;
				break;
			case 'i':
				op = 'i';
				base = 0;
				fn = (zend_long (*)())ZEND_STRTOL_PTR;
				break;
			case 'o':
				op = 'i';
				base = 8;
				fn = (zend_long (*)())ZEND_STRTOL_PTR;
				break;
			case 'x':
			case 'X':
				op = 'i';
				base = 16;
				fn = (zend_long (*)())ZEND_STRTOL_PTR;
				break;
			case 'u':
				op = 'i';
				base = 10;
				flags |= SCAN_UNSIGNED;
				fn = (zend_long (*)())ZEND_STRTOUL_PTR;
				break;

			case 'f':
			case 'e':
			case 'E':
			case 'g':
				op = 'f';
				break;

			case 's':
				op = 's';
				break;

			case 'c':
				op = 's';
				flags |= SCAN_NOSKIP;
				/*-cc-*/
				if (0 == width) {
					width = 1;
				}
				/*-cc-*/
				break;
			case '[':
				op = '[';
				flags |= SCAN_NOSKIP;
				break;
		}   /* switch */

		/*
		 * At this point, we will need additional characters from the
		 * string to proceed.
		 */
		if (*string == '\0') {
			underflow = 1;
			goto done;
		}

		/*
		 * Skip any leading whitespace at the beginning of a field unless
		 * the format suppresses this behavior.
		 */
		if (!(flags & SCAN_NOSKIP)) {
			while (*string != '\0') {
				sch = *string;
				if (! isspace((int)sch) ) {
					break;
				}
				string++;
			}
			if (*string == '\0') {
				underflow = 1;
				goto done;
			}
		}

		/*
		 * Perform the requested scanning operation.
		 */
		switch (op) {
			case 'c':
			case 's':
				/*
				 * Scan a string up to width characters or whitespace.
				 */
				if (width == 0) {
					width = (size_t) ~0;
				}
				end = string;
				while (*end != '\0') {
					sch = *end;
					if ( isspace( (int)sch ) ) {
						break;
					}
					end++;
					if (--width == 0) {
					   break;
					}
				}
				if (!(flags & SCAN_SUPPRESS)) {
					if (numVars && objIndex >= argCount) {
						break;
					} else if (numVars) {
						current = Z_REFVAL(args[objIndex++]);
						zval_ptr_dtor(current);
						ZVAL_STRINGL(current, string, end-string);
					} else {
						add_index_stringl(return_value, objIndex++, string, end-string);
					}
				}
				string = end;
				break;

			case '[': {
				CharSet cset;

				if (width == 0) {
					width = (size_t) ~0;
				}
				end = string;

				format = BuildCharSet(&cset, format);
				while (*end != '\0') {
					sch = *end;
					if (!CharInSet(&cset, (int)sch)) {
						break;
					}
					end++;
					if (--width == 0) {
						break;
					}
				}
				ReleaseCharSet(&cset);

				if (string == end) {
					/*
					 * Nothing matched the range, stop processing
					 */
					goto done;
				}
				if (!(flags & SCAN_SUPPRESS)) {
					if (numVars && objIndex >= argCount) {
						break;
					} else if (numVars) {
						current = Z_REFVAL(args[objIndex++]);
						zval_ptr_dtor(current);
						ZVAL_STRINGL(current, string, end-string);
					} else {
						add_index_stringl(return_value, objIndex++, string, end-string);
					}
				}
				string = end;
				break;
			}
/*
			case 'c':
			   / Scan a single character./

				sch = *string;
				string++;
				if (!(flags & SCAN_SUPPRESS)) {
					if (numVars) {
						char __buf[2];
						__buf[0] = sch;
						__buf[1] = '\0';;
						current = args[objIndex++];
						zval_dtor(*current);
						ZVAL_STRINGL( *current, __buf, 1);
					} else {
						add_index_stringl(return_value, objIndex++, &sch, 1);
					}
				}
				break;
*/
			case 'i':
				/*
				 * Scan an unsigned or signed integer.
				 */
				/*-cc-*/
				buf[0] = '\0';
				/*-cc-*/
				if ((width == 0) || (width > sizeof(buf) - 1)) {
					width = sizeof(buf) - 1;
				}

				flags |= SCAN_SIGNOK | SCAN_NODIGITS | SCAN_NOZERO;
				for (end = buf; width > 0; width--) {
					switch (*string) {
						/*
						 * The 0 digit has special meaning at the beginning of
						 * a number.  If we are unsure of the base, it
						 * indicates that we are in base 8 or base 16 (if it is
						 * followed by an 'x').
						 */
						case '0':
							/*-cc-*/
							if (base == 16) {
								flags |= SCAN_XOK;
							}
							/*-cc-*/
							if (base == 0) {
								base = 8;
								flags |= SCAN_XOK;
							}
							if (flags & SCAN_NOZERO) {
								flags &= ~(SCAN_SIGNOK | SCAN_NODIGITS | SCAN_NOZERO);
							} else {
								flags &= ~(SCAN_SIGNOK | SCAN_XOK | SCAN_NODIGITS);
							}
							goto addToInt;

						case '1': case '2': case '3': case '4':
						case '5': case '6': case '7':
							if (base == 0) {
								base = 10;
							}
							flags &= ~(SCAN_SIGNOK | SCAN_XOK | SCAN_NODIGITS);
							goto addToInt;

						case '8': case '9':
							if (base == 0) {
								base = 10;
							}
							if (base <= 8) {
							   break;
							}
							flags &= ~(SCAN_SIGNOK | SCAN_XOK | SCAN_NODIGITS);
							goto addToInt;

						case 'A': case 'B': case 'C':
						case 'D': case 'E': case 'F':
						case 'a': case 'b': case 'c':
						case 'd': case 'e': case 'f':
							if (base <= 10) {
								break;
							}
							flags &= ~(SCAN_SIGNOK | SCAN_XOK | SCAN_NODIGITS);
							goto addToInt;

						case '+': case '-':
							if (flags & SCAN_SIGNOK) {
								flags &= ~SCAN_SIGNOK;
								goto addToInt;
							}
							break;

						case 'x': case 'X':
							if ((flags & SCAN_XOK) && (end == buf+1)) {
								base = 16;
								flags &= ~SCAN_XOK;
								goto addToInt;
							}
							break;
					}

					/*
					 * We got an illegal character so we are done accumulating.
					 */
					break;

addToInt:
					/*
					 * Add the character to the temporary buffer.
					 */
					*end++ = *string++;
					if (*string == '\0') {
						break;
					}
				}

				/*
				 * Check to see if we need to back up because we only got a
				 * sign or a trailing x after a 0.
				 */
				if (flags & SCAN_NODIGITS) {
					if (*string == '\0') {
						underflow = 1;
					}
					goto done;
				} else if (end[-1] == 'x' || end[-1] == 'X') {
					end--;
					string--;
				}

				/*
				 * Scan the value from the temporary buffer.  If we are
				 * returning a large unsigned value, we have to convert it back
				 * to a string since PHP only supports signed values.
				 */
				if (!(flags & SCAN_SUPPRESS)) {
					*end = '\0';
					value = (zend_long) (*fn)(buf, NULL, base);
					if ((flags & SCAN_UNSIGNED) && (value < 0)) {
						snprintf(buf, sizeof(buf), ZEND_ULONG_FMT, value); /* INTL: ISO digit */
						if (numVars && objIndex >= argCount) {
							break;
						} else if (numVars) {
						  /* change passed value type to string */
							current = Z_REFVAL(args[objIndex++]);
							zval_ptr_dtor(current);
							ZVAL_STRING(current, buf);
						} else {
							add_index_string(return_value, objIndex++, buf);
						}
					} else {
						if (numVars && objIndex >= argCount) {
							break;
						} else if (numVars) {
							current = Z_REFVAL(args[objIndex++]);
							zval_ptr_dtor(current);
							ZVAL_LONG(current, value);
						} else {
							add_index_long(return_value, objIndex++, value);
						}
					}
				}
				break;

			case 'f':
				/*
				 * Scan a floating point number
				 */
				buf[0] = '\0';     /* call me pedantic */
				if ((width == 0) || (width > sizeof(buf) - 1)) {
					width = sizeof(buf) - 1;
				}
				flags |= SCAN_SIGNOK | SCAN_NODIGITS | SCAN_PTOK | SCAN_EXPOK;
				for (end = buf; width > 0; width--) {
					switch (*string) {
						case '0': case '1': case '2': case '3':
						case '4': case '5': case '6': case '7':
						case '8': case '9':
							flags &= ~(SCAN_SIGNOK | SCAN_NODIGITS);
							goto addToFloat;
						case '+':
						case '-':
							if (flags & SCAN_SIGNOK) {
								flags &= ~SCAN_SIGNOK;
								goto addToFloat;
							}
							break;
						case '.':
							if (flags & SCAN_PTOK) {
								flags &= ~(SCAN_SIGNOK | SCAN_PTOK);
								goto addToFloat;
							}
							break;
						case 'e':
						case 'E':
							/*
							 * An exponent is not allowed until there has
							 * been at least one digit.
							 */
							if ((flags & (SCAN_NODIGITS | SCAN_EXPOK)) == SCAN_EXPOK) {
								flags = (flags & ~(SCAN_EXPOK|SCAN_PTOK))
									| SCAN_SIGNOK | SCAN_NODIGITS;
								goto addToFloat;
							}
							break;
					}

					/*
					 * We got an illegal character so we are done accumulating.
					 */
					break;

addToFloat:
					/*
					 * Add the character to the temporary buffer.
					 */
					*end++ = *string++;
					if (*string == '\0') {
						break;
					}
				}

				/*
				 * Check to see if we need to back up because we saw a
				 * trailing 'e' or sign.
				 */
				if (flags & SCAN_NODIGITS) {
					if (flags & SCAN_EXPOK) {
						/*
						 * There were no digits at all so scanning has
						 * failed and we are done.
						 */
						if (*string == '\0') {
							underflow = 1;
						}
						goto done;
					}

					/*
					 * We got a bad exponent ('e' and maybe a sign).
					 */
					end--;
					string--;
					if (*end != 'e' && *end != 'E') {
						end--;
						string--;
					}
				}

				/*
				 * Scan the value from the temporary buffer.
				 */
				if (!(flags & SCAN_SUPPRESS)) {
					double dvalue;
					*end = '\0';
					dvalue = zend_strtod(buf, NULL);
					if (numVars && objIndex >= argCount) {
						break;
					} else if (numVars) {
						current = Z_REFVAL(args[objIndex++]);
						zval_ptr_dtor(current);
						ZVAL_DOUBLE(current, dvalue);
					} else {
						add_index_double(return_value, objIndex++, dvalue );
					}
				}
				break;
		} /* switch (op) */
		nconversions++;
	} /*  while (*format != '\0') */

done:
	result = SCAN_SUCCESS;

	if (underflow && (0==nconversions)) {
		scan_set_error_return( numVars, return_value );
		result = SCAN_ERROR_EOF;
	} else if (numVars) {
		convert_to_long(return_value );
		Z_LVAL_P(return_value) = nconversions;
	} else if (nconversions < totalVars) {
		/* TODO: not all elements converted. we need to prune the list - cc */
	}
	return result;
}
/* }}} */

/* the compiler choked when i tried to make this a macro    */
static inline void scan_set_error_return(int numVars, zval *return_value) /* {{{ */
{
	if (numVars) {
		ZVAL_LONG(return_value, SCAN_ERROR_EOF);  /* EOF marker */
	} else {
		/* convert_to_null calls destructor */
		convert_to_null(return_value);
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
