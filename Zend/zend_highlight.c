/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include <zend_language_parser.h>
#include "zend_compile.h"
#include "zend_highlight.h"
#include "zend_ptr_stack.h"
#include "zend_globals.h"
#include "zend_exceptions.h"

ZEND_API void zend_html_putc(char c)
{
	switch (c) {
		case '\n':
			ZEND_PUTS("<br />");
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


ZEND_API void zend_html_puts(const char *s, size_t len)
{
	const unsigned char *ptr = (const unsigned char*)s, *end = ptr + len;
	unsigned char *filtered = NULL;
	size_t filtered_len;

	if (LANG_SCNG(output_filter)) {
		LANG_SCNG(output_filter)(&filtered, &filtered_len, ptr, len);
		ptr = filtered;
		end = filtered + filtered_len;
	}

	while (ptr<end) {
		if (*ptr==' ') {
			do {
				zend_html_putc(*ptr);
			} while ((++ptr < end) && (*ptr==' '));
		} else {
			zend_html_putc(*ptr++);
		}
	}

	if (LANG_SCNG(output_filter)) {
		efree(filtered);
	}
}


ZEND_API void zend_highlight(zend_syntax_highlighter_ini *syntax_highlighter_ini)
{
	zval token;
	int token_type;
	char *last_color = syntax_highlighter_ini->highlight_html;
	char *next_color;

	zend_printf("<code>");
	zend_printf("<span style=\"color: %s\">\n", last_color);
	/* highlight stuff coming back from zendlex() */
	while ((token_type=lex_scan(&token, NULL))) {
		switch (token_type) {
			case T_INLINE_HTML:
				next_color = syntax_highlighter_ini->highlight_html;
				break;
			case T_COMMENT:
			case T_DOC_COMMENT:
				next_color = syntax_highlighter_ini->highlight_comment;
				break;
			case T_OPEN_TAG:
			case T_OPEN_TAG_WITH_ECHO:
			case T_CLOSE_TAG:
			case T_LINE:
			case T_FILE:
			case T_DIR:
			case T_TRAIT_C:
			case T_METHOD_C:
			case T_FUNC_C:
			case T_NS_C:
			case T_CLASS_C:
				next_color = syntax_highlighter_ini->highlight_default;
				break;
			case '"':
			case T_ENCAPSED_AND_WHITESPACE:
			case T_CONSTANT_ENCAPSED_STRING:
				next_color = syntax_highlighter_ini->highlight_string;
				break;
			case T_WHITESPACE:
				zend_html_puts((char*)LANG_SCNG(yy_text), LANG_SCNG(yy_leng));  /* no color needed */
				ZVAL_UNDEF(&token);
				continue;
				break;
			default:
				if (Z_TYPE(token) == IS_UNDEF) {
					next_color = syntax_highlighter_ini->highlight_keyword;
				} else {
					next_color = syntax_highlighter_ini->highlight_default;
				}
				break;
		}

		if (last_color != next_color) {
			if (last_color != syntax_highlighter_ini->highlight_html) {
				zend_printf("</span>");
			}
			last_color = next_color;
			if (last_color != syntax_highlighter_ini->highlight_html) {
				zend_printf("<span style=\"color: %s\">", last_color);
			}
		}

		zend_html_puts((char*)LANG_SCNG(yy_text), LANG_SCNG(yy_leng));

		if (Z_TYPE(token) == IS_STRING) {
			switch (token_type) {
				case T_OPEN_TAG:
				case T_OPEN_TAG_WITH_ECHO:
				case T_CLOSE_TAG:
				case T_WHITESPACE:
				case T_COMMENT:
				case T_DOC_COMMENT:
					break;
				default:
					zval_ptr_dtor_str(&token);
					break;
			}
		}
		ZVAL_UNDEF(&token);
	}

	if (last_color != syntax_highlighter_ini->highlight_html) {
		zend_printf("</span>\n");
	}
	zend_printf("</span>\n");
	zend_printf("</code>");

	/* Discard parse errors thrown during tokenization */
	zend_clear_exception();
}

ZEND_API void zend_strip(void)
{
	zval token;
	int token_type;
	int prev_space = 0;

	while ((token_type=lex_scan(&token, NULL))) {
		switch (token_type) {
			case T_WHITESPACE:
				if (!prev_space) {
					zend_write(" ", sizeof(" ") - 1);
					prev_space = 1;
				}
						/* lack of break; is intentional */
			case T_COMMENT:
			case T_DOC_COMMENT:
				ZVAL_UNDEF(&token);
				continue;

			case T_END_HEREDOC:
				zend_write((char*)LANG_SCNG(yy_text), LANG_SCNG(yy_leng));
				/* read the following character, either newline or ; */
				if (lex_scan(&token, NULL) != T_WHITESPACE) {
					zend_write((char*)LANG_SCNG(yy_text), LANG_SCNG(yy_leng));
				}
				zend_write("\n", sizeof("\n") - 1);
				prev_space = 1;
				ZVAL_UNDEF(&token);
				continue;

			default:
				zend_write((char*)LANG_SCNG(yy_text), LANG_SCNG(yy_leng));
				break;
		}

		if (Z_TYPE(token) == IS_STRING) {
			switch (token_type) {
				case T_OPEN_TAG:
				case T_OPEN_TAG_WITH_ECHO:
				case T_CLOSE_TAG:
				case T_WHITESPACE:
				case T_COMMENT:
				case T_DOC_COMMENT:
					break;

				default:
					zval_ptr_dtor_str(&token);
					break;
			}
		}
		prev_space = 0;
		ZVAL_UNDEF(&token);
	}

	/* Discard parse errors thrown during tokenization */
	zend_clear_exception();
}
