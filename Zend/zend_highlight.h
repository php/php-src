/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend by Perforce and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_HIGHLIGHT_H
#define ZEND_HIGHLIGHT_H

#include "zend_types.h"

#define HL_COMMENT_COLOR     "#FF8000"    /* orange */
#define HL_DEFAULT_COLOR     "#0000BB"    /* blue */
#define HL_HTML_COLOR        "#000000"    /* black */
#define HL_STRING_COLOR      "#DD0000"    /* red */
#define HL_KEYWORD_COLOR     "#007700"    /* green */


typedef struct _zend_syntax_highlighter_ini {
	char *highlight_html;
	char *highlight_comment;
	char *highlight_default;
	char *highlight_string;
	char *highlight_keyword;
} zend_syntax_highlighter_ini;


BEGIN_EXTERN_C()
ZEND_API void zend_highlight(zend_syntax_highlighter_ini *syntax_highlighter_ini);
ZEND_API void zend_strip(void);
ZEND_API zend_result highlight_file(const char *filename, zend_syntax_highlighter_ini *syntax_highlighter_ini);
ZEND_API void highlight_string(zend_string *str, zend_syntax_highlighter_ini *syntax_highlighter_ini, const char *str_name);
ZEND_API void zend_html_putc(char c);
ZEND_API void zend_html_puts(const char *s, size_t len);
END_EXTERN_C()

extern zend_syntax_highlighter_ini syntax_highlighter_ini;

#endif
