/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998, 1999 Andi Gutmans, Zeev Suraski                  |
   +----------------------------------------------------------------------+
   | This source file is subject to the Zend license, that is bundled     |
   | with this package in the file LICENSE.  If you did not receive a     |
   | copy of the Zend license, please mail us at zend@zend.com so we can  |
   | send you a copy immediately.                                         |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_compile.h"
#include "zend_highlight.h"
#include "zend_ptr_stack.h"
#include "zend_globals.h"

extern char *zendtext;
extern int zendleng;


static void html_putc(char c)
{
	switch (c) {
		case '\n':
			ZEND_PUTS("<br>");
			break;
		case '<':
			ZEND_PUTS("&lt;");
			break;
		case '>':
			ZEND_PUTS("&gt;");
			break;
		case '&':
			ZEND_PUTS("&amp;");
			break;
		case ' ':
			ZEND_PUTS("&nbsp;");
			break;
		case '\t':
			ZEND_PUTS("&nbsp;&nbsp;&nbsp;&nbsp;");
			break;
		default:
			ZEND_PUTC(c);
			break;
	}
}


static void html_puts(char *s, uint len)
{
	register char *ptr=s, *end=s+len;
	
	while (ptr<end) {
		html_putc(*ptr++);
	}
}




void zend_highlight(zend_syntax_highlighter_ini *syntax_highlighter_ini)
{
	zval token;
	int token_type;
	char *last_color = syntax_highlighter_ini->highlight_html;
	char *next_color;
	int in_string=0;
	CLS_FETCH();

	zend_printf("<font color=\"%s\">\n", last_color);
	/* highlight stuff coming back from zendlex() */
	token.type = 0;
	while ((token_type=lex_scan(&token CLS_CC))) {
		switch (token_type) {
			case INLINE_HTML:
				next_color = syntax_highlighter_ini->highlight_html;
				break;
			case ZEND_COMMENT:
				next_color = syntax_highlighter_ini->highlight_comment;
				break;
			case PHP_OPEN_TAG:
				next_color = syntax_highlighter_ini->highlight_default;
				break;
			case PHP_CLOSE_TAG:
				next_color = syntax_highlighter_ini->highlight_default;
				break;
			case CONSTANT_ENCAPSED_STRING:
				next_color = syntax_highlighter_ini->highlight_string;
				break;
			case '"':
				next_color = syntax_highlighter_ini->highlight_string;
				in_string = !in_string;
				break;				
			case T_WHITESPACE:
				html_puts(zendtext, zendleng);  /* no color needed */
				token.type = 0;
				continue;
				break;
			default:
				if (token.type==0) {
					next_color = syntax_highlighter_ini->highlight_keyword;
				} else {
					if (in_string) {
						next_color = syntax_highlighter_ini->highlight_string;
					} else {
						next_color = syntax_highlighter_ini->highlight_default;
					}
				}
				break;
		}

		if (last_color != next_color) {
			if (last_color != syntax_highlighter_ini->highlight_html) {
				zend_printf("</font>");
			}
			last_color = next_color;
			if (last_color != syntax_highlighter_ini->highlight_html) {
				zend_printf("<font color=\"%s\">", last_color);
			}
		}
		switch (token_type) {
			case DOLLAR_OPEN_CURLY_BRACES:
				html_puts("{", 1);
				break;
			default:
				html_puts(zendtext, zendleng);
				break;
		}

		if (token.type == IS_STRING) {
			switch (token_type) {
			case PHP_OPEN_TAG:
			case PHP_CLOSE_TAG:
			case T_WHITESPACE:
				break;
			default:
				efree(token.value.str.val);
				break;
			}
		}
		token.type = 0;
	}
	if (last_color != syntax_highlighter_ini->highlight_html) {
		zend_printf("</font>\n");
	}
	zend_printf("</font>\n");
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
