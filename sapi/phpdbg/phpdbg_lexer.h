/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_LEXER_H
#define PHPDBG_LEXER_H

#include "phpdbg_cmd.h"

typedef struct {
        unsigned int len;
        unsigned char *text;
        unsigned char *cursor;
        unsigned char *marker;
        unsigned char *ctxmarker;
        int state;
} phpdbg_lexer_data;

#define yyparse phpdbg_parse
#define yylex phpdbg_lex

void phpdbg_init_lexer (phpdbg_param_t *stack, char *input);

int phpdbg_lex (phpdbg_param_t* yylval);

#endif
