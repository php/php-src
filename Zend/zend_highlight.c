/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2001 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.92 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        | 
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_92.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#include "zend.h"
#include "zend_language_parser.h"
#include "zend_compile.h"
#include "zend_highlight.h"
#include "zend_ptr_stack.h"
#include "zend_globals.h"

#ifndef ZTS
extern char *zendtext;
extern int zendleng;
#else
#define zendtext ((char *) zend_get_zendtext(CLS_C))
#define zendleng zend_get_zendleng(CLS_C)
#endif

ZEND_API void zend_html_putc(char c)
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


ZEND_API void zend_html_puts(char *s, uint len)
{
	register char *ptr=s, *end=s+len;
	
	while (ptr<end) {
		if (*ptr==' '
			&& len>1
			&& !(((ptr+1)>=end) || (*(ptr+1)==' ')) /* next is not a space */
			&& !((ptr==s) || (*(ptr-1)==' '))) /* last is not a space */ {
			char c = *ptr++;

			ZEND_PUTC(c);
			continue;
		}
		zend_html_putc(*ptr++);
	}
}



ZEND_API void zend_highlight(zend_syntax_highlighter_ini *syntax_highlighter_ini)
{
	zval token;
	int token_type;
	char *last_color = syntax_highlighter_ini->highlight_html;
	char *next_color;
	int in_string=0;
	CLS_FETCH();

	zend_printf("<code>");
	zend_printf("<font color=\"%s\">\n", last_color);
	/* highlight stuff coming back from zendlex() */
	token.type = 0;
	while ((token_type=lex_scan(&token CLS_CC))) {
		switch (token_type) {
			case T_INLINE_HTML:
				next_color = syntax_highlighter_ini->highlight_html;
				break;
			case T_COMMENT:
				next_color = syntax_highlighter_ini->highlight_comment;
				break;
			case T_OPEN_TAG:
			case T_OPEN_TAG_WITH_ECHO:
				next_color = syntax_highlighter_ini->highlight_default;
				break;
			case T_CLOSE_TAG:
				next_color = syntax_highlighter_ini->highlight_default;
				break;
			case T_CONSTANT_ENCAPSED_STRING:
				next_color = syntax_highlighter_ini->highlight_string;
				break;
			case '"':
				next_color = syntax_highlighter_ini->highlight_string;
				in_string = !in_string;
				break;				
			case T_WHITESPACE:
				zend_html_puts(zendtext, zendleng);  /* no color needed */
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
			case T_END_HEREDOC:
				zend_html_puts(token.value.str.val, token.value.str.len);
				break;
			default:
				zend_html_puts(zendtext, zendleng);
				break;
		}

		if (token.type == IS_STRING) {
			switch (token_type) {
				case T_OPEN_TAG:
				case T_OPEN_TAG_WITH_ECHO:
				case T_CLOSE_TAG:
				case T_WHITESPACE:
				case T_COMMENT:
					break;
				default:
					efree(token.value.str.val);
					break;
			}
		} else if (token_type == T_END_HEREDOC) {
			zend_bool has_semicolon=(strchr(token.value.str.val, ';')?1:0);

			efree(token.value.str.val);
			if (has_semicolon) {
				/* the following semicolon was unput(), ignore it */
				lex_scan(&token CLS_CC);
			}
		}
		token.type = 0;
	}
	if (last_color != syntax_highlighter_ini->highlight_html) {
		zend_printf("</font>\n");
	}
	zend_printf("</font>\n");
	zend_printf("</code>");
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
