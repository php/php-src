/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Jaakko Hyvätti <jaakko.hyvatti@iki.fi>                      |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

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

#define ENT_COMPAT			1
#define ENT_QUOTES			2
#define ENT_NOQUOTES		4

PHPAPI char *php_escape_html_entities(unsigned char *old, int oldlen, int *newlen, int all, int quote_style)
{
	int i, maxlen, len;
	char *new;

	maxlen = 2 * oldlen;
	if (maxlen < 128)
		maxlen = 128;
	new = emalloc (maxlen);
	len = 0;

	i = oldlen;
	while (i--) {
		if (len + 9 > maxlen)
			new = erealloc (new, maxlen += 128);
		if (38 == *old) {
			memcpy (new + len, "&amp;", 5);
			len += 5;
		} else if (34 == *old && !(quote_style&ENT_NOQUOTES)) {
			memcpy (new + len, "&quot;", 6);
			len += 6;
		} else if (39 == *old && (quote_style&ENT_QUOTES)) {
			memcpy (new + len, "&#039;", 6);
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
	*newlen = len;

	return new;
}

static void php_html_entities(INTERNAL_FUNCTION_PARAMETERS, int all)
{
    zval **arg, **quotes;
    int len, quote_style = ENT_COMPAT;
	int ac = ZEND_NUM_ARGS();
	char *new;

	if (ac < 1 || ac > 2 || zend_get_parameters_ex(ac, &arg, &quotes) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    convert_to_string_ex(arg);
	if(ac==2) {
		convert_to_long_ex(quotes);
		quote_style = (*quotes)->value.lval;
	}

	new = php_escape_html_entities((*arg)->value.str.val, (*arg)->value.str.len, &len, all, quote_style);
    RETVAL_STRINGL(new,len,0);
}

#define HTML_SPECIALCHARS 	0
#define HTML_ENTITIES	 	1

void register_html_constants(INIT_FUNC_ARGS)
{
	REGISTER_LONG_CONSTANT("HTML_SPECIALCHARS", HTML_SPECIALCHARS, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("HTML_ENTITIES", HTML_ENTITIES, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("ENT_COMPAT", ENT_COMPAT, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("ENT_QUOTES", ENT_QUOTES, CONST_PERSISTENT|CONST_CS);
	REGISTER_LONG_CONSTANT("ENT_NOQUOTES", ENT_NOQUOTES, CONST_PERSISTENT|CONST_CS);
}

/* {{{ proto string htmlspecialchars(string string [, int quote_style])
   Convert special characters to HTML entities */
PHP_FUNCTION(htmlspecialchars)
{
	php_html_entities(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}
/* }}} */

/* {{{ proto string htmlentities(string string [, int quote_style])
   Convert all applicable characters to HTML entities */
PHP_FUNCTION(htmlentities)
{
	php_html_entities(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */

/* {{{ proto array get_html_translation_table([int table [, int quote_style]])
   Returns the internal translation table used by htmlspecialchars and htmlentities */
PHP_FUNCTION(get_html_translation_table)
{
	zval **whichone, **quotes;
	int which = 0, quote_style = ENT_COMPAT;
	int ac = ZEND_NUM_ARGS();
	int inx;
	char ind[ 2 ];

	if (ac < 0 || ac > 2 || zend_get_parameters_ex(ac, &whichone, &quotes) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

	if (ac > 0) {
		convert_to_long_ex(whichone);
		which = (*whichone)->value.lval;
	} 
	if (ac == 2) {
		convert_to_long_ex(quotes);
		quote_style = (*quotes)->value.lval;
	}

	array_init(return_value);

	ind[1] = 0;

	switch (which) {
		case HTML_ENTITIES:
			for (inx = 160; inx <= 255; inx++) {
				char buffer[16];
				ind[0] = inx;
				sprintf(buffer,"&%s;",EntTable[inx-160]);
				add_assoc_string(return_value,ind,buffer,1);
			}
			/* break thru */

		case HTML_SPECIALCHARS:
			ind[0]=38; add_assoc_string(return_value,ind,"&amp;",1);
			if(quote_style&ENT_QUOTES) {
				ind[0]=39; add_assoc_string(return_value,ind,"&#039;",1); }
			if(!(quote_style&ENT_NOQUOTES)) {
				ind[0]=34; add_assoc_string(return_value,ind,"&quot;",1); }
			ind[0]=60; add_assoc_string(return_value,ind,"&lt;",1);
			ind[0]=62; add_assoc_string(return_value,ind,"&gt;",1);
			break;
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
