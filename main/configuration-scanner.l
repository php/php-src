%{

/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of the GNU General Public License as published by |
   | the Free Software Foundation; either version 2 of the License, or    |
   | (at your option) any later version.                                  |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of the GNU General Public License    |
   | along with this program; if not, write to the Free Software          |
   | Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            |
   +----------------------------------------------------------------------+
   | Authors:  Zeev Suraski <zeev@zend.com>                    |
   +----------------------------------------------------------------------+
*/


#include "php.h"
#include "configuration-parser.h"

#undef YYSTYPE
#define YYSTYPE pval

#define YY_DECL cfglex(pval *cfglval)


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

<INITIAL>[ ]*("true"|"on"|"yes")[ ]* {
	cfglval->value.str.val = php3_strndup("1",1);
	cfglval->value.str.len = 1;
	cfglval->type = IS_STRING;
	return CFG_TRUE;
}


<INITIAL>[ ]*("false"|"off"|"no")[ ]* {
	cfglval->value.str.val = php3_strndup("",0);
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

	cfglval->value.str.val = php3_strndup(yytext,yyleng);
	cfglval->value.str.len = yyleng;
	cfglval->type = IS_STRING;
	return SECTION;
}


<INITIAL>["][^\n\r"]*["] {
	/* ENCAPSULATED TC_STRING */
	register int i;

	/* eat trailing " */
	yytext[yyleng-1]=0;
	
	/* eat leading " */
	yytext++;

	cfglval->value.str.val = php3_strndup(yytext,yyleng);
	cfglval->value.str.len = yyleng;
	cfglval->type = IS_STRING;
	return TC_ENCAPSULATED_STRING;
}


<INITIAL>[^=\n\r\t;"]+ {
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
		cfglval->value.str.val = php3_strndup(yytext,yyleng);
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
	php3_error(E_NOTICE,"Unexpected character on line %d:  '%s' (ASCII %d)\n",yylineno,yytext,yytext[0]);
#endif
}
