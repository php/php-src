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
   | Author: Bjørn Borud - Guardian Networks AS <borud@guardian.no>       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php.h"
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include "php3_string.h"

/* Simple soundex algorithm as described by Knuth in TAOCP, vol 3 */
/* {{{ proto string soundex(string str)
   Calculate the soundex key of a string */
PHP_FUNCTION(soundex)
{
	char l, u;
	char *somestring;
	int i, j, n;
	pval *arg;

	/* pad with '0' and terminate with 0 ;-) */
	char soundex[5] =
	{'0', '0', '0', '0', 0};

	static char soundex_table[26] =
	{0,							/* A */
	 '1',						/* B */
	 '2',						/* C */
	 '3',						/* D */
	 0,							/* E */
	 '1',						/* F */
	 '2',						/* G */
	 0,							/* H */
	 0,							/* I */
	 '2',						/* J */
	 '2',						/* K */
	 '4',						/* L */
	 '5',						/* M */
	 '5',						/* N */
	 0,							/* O */
	 '1',						/* P */
	 '2',						/* Q */
	 '6',						/* R */
	 '2',						/* S */
	 '3',						/* T */
	 0,							/* U */
	 '1',						/* V */
	 0,							/* W */
	 '2',						/* X */
	 0,							/* Y */
	 '2'};						/* Z */
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);
	if (arg->value.str.len==0) {
		RETURN_FALSE;
	}

	somestring = arg->value.str.val;

	n = arg->value.str.len;

	/* convert chars to upper case and strip non-letter chars */
	j = 0;
	for (i = 0; i < n; i++) {
		u = toupper(somestring[i]);
		if ((u > 64) && (u < 91)) {
			somestring[j] = u;
			j++;
		}
	}

	/* null-terminate string */
	somestring[j] = 0;

	n = strlen(somestring);

	/* prefix soundex string with first valid char */
	soundex[0] = somestring[0];

	/* remember first char */
	l = soundex_table[((somestring[0]) - 65)];

	j = 1;

	/* build soundex string */
	for (i = 1; i < n && j < 4; i++) {
		u = soundex_table[((somestring[i]) - 65)];

		if (u != l) {
			if (u != 0) {
				soundex[(int) j++] = u;
			}
			l = u;
		}
	}

	return_value->value.str.val = estrndup(soundex, 4);
	return_value->value.str.len = strlen(soundex);
	return_value->type = IS_STRING;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
