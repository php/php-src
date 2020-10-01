/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Bjørn Borud - Guardian Networks AS <borud@guardian.no>       |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include "php_string.h"

/* Simple soundex algorithm as described by Knuth in TAOCP, vol 3 */
/* {{{ Calculate the soundex key of a string */
PHP_FUNCTION(soundex)
{
	char	*str;
	size_t	i, _small, str_len, code, last;
	char	soundex[4 + 1];

	static const char soundex_table[26] =
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

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(str, str_len)
	ZEND_PARSE_PARAMETERS_END();

	/* build soundex string */
	last = -1;
	for (i = 0, _small = 0; i < str_len && _small < 4; i++) {
		/* convert chars to upper case and strip non-letter chars */
		/* BUG: should also map here accented letters used in non */
		/* English words or names (also found in English text!): */
		/* esstsett, thorn, n-tilde, c-cedilla, s-caron, ... */
		code = toupper((int)(unsigned char)str[i]);
		if (code >= 'A' && code <= 'Z') {
			if (_small == 0) {
				/* remember first valid char */
				soundex[_small++] = (char)code;
				last = soundex_table[code - 'A'];
			}
			else {
				/* ignore sequences of consonants with same soundex */
				/* code in trail, and vowels unless they separate */
				/* consonant letters */
				code = soundex_table[code - 'A'];
				if (code != last) {
					if (code != 0) {
						soundex[_small++] = (char)code;
					}
					last = code;
				}
			}
		}
	}
	/* pad with '0' and terminate with 0 ;-) */
	while (_small < 4) {
		soundex[_small++] = '0';
	}
	soundex[_small] = '\0';

	RETURN_STRINGL(soundex, _small);
}
/* }}} */
