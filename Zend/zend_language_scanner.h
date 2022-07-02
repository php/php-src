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

#ifndef ZEND_SCANNER_H
#define ZEND_SCANNER_H

typedef struct _zend_lex_state {
	unsigned int yy_leng;
	unsigned char *yy_start;
	unsigned char *yy_text;
	unsigned char *yy_cursor;
	unsigned char *yy_marker;
	unsigned char *yy_limit;
	int yy_state;
	zend_stack state_stack;
	zend_ptr_stack heredoc_label_stack;
	zend_stack nest_location_stack; /* for syntax error reporting */

	zend_file_handle *in;
	uint32_t lineno;
	zend_string *filename;

	/* original (unfiltered) script */
	unsigned char *script_org;
	size_t script_org_size;

	/* filtered script */
	unsigned char *script_filtered;
	size_t script_filtered_size;

	/* input/output filters */
	zend_encoding_filter input_filter;
	zend_encoding_filter output_filter;
	const zend_encoding *script_encoding;

	/* hooks */
	void (*on_event)(
		zend_php_scanner_event event, int token, int line,
		const char *text, size_t length, void *context);
	void *on_event_context;

	zend_ast *ast;
	zend_arena *ast_arena;
} zend_lex_state;

typedef struct _zend_heredoc_label {
	char *label;
	int length;
	int indentation;
	bool indentation_uses_spaces;
} zend_heredoc_label;

/* Track locations of unclosed {, [, (, etc. for better syntax error reporting */
typedef struct _zend_nest_location {
	char text;
	int  lineno;
} zend_nest_location;

BEGIN_EXTERN_C()
ZEND_API void zend_save_lexical_state(zend_lex_state *lex_state);
ZEND_API void zend_restore_lexical_state(zend_lex_state *lex_state);
ZEND_API void zend_prepare_string_for_scanning(zval *str, zend_string *filename);
ZEND_API void zend_multibyte_yyinput_again(zend_encoding_filter old_input_filter, const zend_encoding *old_encoding);
ZEND_API zend_result zend_multibyte_set_filter(const zend_encoding *onetime_encoding);
ZEND_API zend_result zend_lex_tstring(zval *zv, unsigned char *ident);

END_EXTERN_C()

#endif
