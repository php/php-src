/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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
