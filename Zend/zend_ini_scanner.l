%{
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2000 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.92 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_92.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_ini_parser.h"

#undef YYSTYPE
#define YYSTYPE zval

#define YY_DECL cfglex(zval *cfglval)


void init_cfg_scanner()
{
	cfglineno=1;
}


%}

%option noyywrap
%option yylineno

%%

<INITIAL>"extension" {
#if	0
	printf("found extension\n");
#endif
	return EXTENSION;
}


<INITIAL>"zend_extension" {
	return T_ZEND_EXTENSION;
}


<INITIAL>"zend_extension_ts" {
	return T_ZEND_EXTENSION_TS;
}


<INITIAL>"zend_extension_debug" {
	return T_ZEND_EXTENSION_DEBUG;
}


<INITIAL>"zend_extension_debug_ts" {
	return T_ZEND_EXTENSION_DEBUG_TS;
}


<INITIAL>[ ]*("true"|"on"|"yes")[ ]* {
	cfglval->value.str.val = zend_strndup("1",1);
	cfglval->value.str.len = 1;
	cfglval->type = IS_STRING;
	return CFG_TRUE;
}


<INITIAL>[ ]*("false"|"off"|"no"|"none")[ ]* {
	cfglval->value.str.val = zend_strndup("",0);
	cfglval->value.str.len = 0;
	cfglval->type = IS_STRING;
	return CFG_FALSE;
}

<INITIAL>[[][^[]+[\]]([\n]?|"\r\n"?) {
	/* SECTION */

	/* eat trailng ] */
	while (yyleng>0 && (yytext[yyleng-1]=='\n' || yytext[yyleng-1]=='\r' || yytext[yyleng-1]==']')) {
		yyleng--;
		yytext[yyleng]=0;
	}

	/* eat leading [ */
	yytext++;
	yyleng--;

	cfglval->value.str.val = zend_strndup(yytext,yyleng);
	cfglval->value.str.len = yyleng;
	cfglval->type = IS_STRING;
	return SECTION;
}


<INITIAL>["][^\n\r"]*["] {
	/* ENCAPSULATED TC_STRING */

	/* eat trailing " */
	yytext[yyleng-1]=0;
	
	/* eat leading " */
	yytext++;

	cfglval->value.str.val = zend_strndup(yytext, yyleng - 2);
	cfglval->value.str.len = yyleng - 2;
	cfglval->type = IS_STRING;
	return TC_ENCAPSULATED_STRING;
}

<INITIAL>[&|~()!] {
	return yytext[0];
}


<INITIAL>[^=\n\r\t;|&~()!"]+ {
	/* STRING */
	register int i;

	/* eat trailing whitespace */
	for (i=yyleng-1; i>=0; i--) {
		if (yytext[i]==' ' || yytext[i]=='\t') {
			yytext[i]=0;
			yyleng--;
		} else {
			break;
		}
	}
	/* eat leading whitespace */
	while (yytext[0]) {
		if (yytext[0]==' ' || yytext[0]=='\t') {
			yytext++;
			yyleng--;
		} else {
			break;
		}
	}
	if (yyleng!=0) {
		cfglval->value.str.val = zend_strndup(yytext,yyleng);
		cfglval->value.str.len = yyleng;
		cfglval->type = IS_STRING;
		return TC_STRING;
	} else {
		/* whitespace */
	}
}



<INITIAL>[=\n] {
	return yytext[0];
}

<INITIAL>"\r\n" {
	return '\n';
}

<INITIAL>[;][^\r\n]*[\r\n]? {
	/* comment */
	return '\n';
}

<INITIAL>[ \t] {
	/* eat whitespace */
}

<INITIAL>. {
#if DEBUG
	php_error(E_NOTICE,"Unexpected character on line %d:  '%s' (ASCII %d)\n",yylineno,yytext,yytext[0]);
#endif
}
