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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Jaakko Hyvätti <jaakko.hyvatti@iki.fi>                      |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "reg.h"
#include "html.h"

/* This must be fixed to handle the input string according to LC_CTYPE.
   Defaults to ISO-8859-1 for now. */
	
static char EntTable[][7] =
{
	"nbsp","iexcl","cent","pound","curren","yen","brvbar",
	"sect","uml","copy","ordf","laquo","not","shy","reg",
	"macr","deg","plusmn","sup2","sup3","acute","micro",
	"para","middot","cedil","sup1","ordm","raquo","frac14",
	"frac12","frac34","iquest","Agrave","Aacute","Acirc",
	"Atilde","Auml","Aring","AElig","Ccedil","Egrave",
	"Eacute","Ecirc","Euml","Igrave","Iacute","Icirc",
	"Iuml","ETH","Ntilde","Ograve","Oacute","Ocirc","Otilde",
	"Ouml","times","Oslash","Ugrave","Uacute","Ucirc","Uuml",
	"Yacute","THORN","szlig","agrave","aacute","acirc",
	"atilde","auml","aring","aelig","ccedil","egrave",
	"eacute","ecirc","euml","igrave","iacute","icirc",
	"iuml","eth","ntilde","ograve","oacute","ocirc","otilde",
	"ouml","divide","oslash","ugrave","uacute","ucirc",
	"uuml","yacute","thorn","yuml"
};

static void _php3_htmlentities(INTERNAL_FUNCTION_PARAMETERS, int all)
{
    pval **arg;
    int i, len, maxlen;
    unsigned char *old;
	char *new;

    if (ARG_COUNT(ht) != 1 || getParametersEx(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
    }

    convert_to_string_ex(arg);

	maxlen = 2 * (*arg)->value.str.len;
	if (maxlen < 128)
		maxlen = 128;
	new = emalloc (maxlen);
	len = 0;

	old = (unsigned char *)(*arg)->value.str.val;
	i = (*arg)->value.str.len;
	while (i--) {
		if (len + 9 > maxlen)
			new = erealloc (new, maxlen += 128);
		if (38 == *old) {
			memcpy (new + len, "&amp;", 5);
			len += 5;
		} else if (34 == *old) {
			memcpy (new + len, "&quot;", 6);
			len += 6;
		} else if (60 == *old) {
			memcpy (new + len, "&lt;", 4);
			len += 4;
		} else if (62 == *old) {
			memcpy (new + len, "&gt;", 4);
			len += 4;
		} else if (all && 160 <= *old) {
			new [len++] = '&';
			strcpy (new + len, EntTable [*old - 160]);
			len += strlen (EntTable [*old - 160]);
			new [len++] = ';';
		} else {
			new [len++] = *old;
		}
		old++;
	}
    new [len] = '\0';

    RETVAL_STRINGL(new,len,1);
    efree(new);
}

/* {{{ proto string htmlspecialchars(string string)
   Convert special characters to HTML entities */
PHP_FUNCTION(htmlspecialchars)
{
	_php3_htmlentities(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}
/* }}} */

/* {{{ proto string htmlentities(string string)
   Convert all applicable characters to HTML entities */
PHP_FUNCTION(htmlentities)
{
	_php3_htmlentities(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
