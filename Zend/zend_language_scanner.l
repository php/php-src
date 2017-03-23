/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2017 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   |          Nuno Lopes <nlopess@php.net>                                |
   |          Scott MacVicar <scottmac@php.net>                           |
   | Flex version authors:                                                |
   |          Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#if 0
# define YYDEBUG(s, c) printf("state: %d char: %c\n", s, c)
#else
# define YYDEBUG(s, c)
#endif

#include "zend_language_scanner_defs.h"

#include <errno.h>
#include "zend.h"
#ifdef ZEND_WIN32
# include <Winuser.h>
#endif
#include "zend_alloc.h"
#include <zend_language_parser.h>
#include "zend_compile.h"
#include "zend_language_scanner.h"
#include "zend_highlight.h"
#include "zend_constants.h"
#include "zend_variables.h"
#include "zend_operators.h"
#include "zend_API.h"
#include "zend_strtod.h"
#include "zend_exceptions.h"
#include "zend_virtual_cwd.h"
#include "tsrm_config_common.h"

#define YYCTYPE   unsigned char
#define YYFILL(n) { if ((YYCURSOR + n) >= (YYLIMIT + ZEND_MMAP_AHEAD)) { return 0; } }
#define YYCURSOR  SCNG(yy_cursor)
#define YYLIMIT   SCNG(yy_limit)
#define YYMARKER  SCNG(yy_marker)

#define YYGETCONDITION()  SCNG(yy_state)
#define YYSETCONDITION(s) SCNG(yy_state) = s

#define STATE(name)  yyc##name

/* emulate flex constructs */
#define BEGIN(state) YYSETCONDITION(STATE(state))
#define YYSTATE      YYGETCONDITION()
#define yytext       ((char*)SCNG(yy_text))
#define yyleng       SCNG(yy_leng)
#define yyless(x)    do { YYCURSOR = (unsigned char*)yytext + x; \
                          yyleng   = (unsigned int)x; } while(0)
#define yymore()     goto yymore_restart

/* perform sanity check. If this message is triggered you should
   increase the ZEND_MMAP_AHEAD value in the zend_streams.h file */
/*!max:re2c */
#if ZEND_MMAP_AHEAD < YYMAXFILL
# error ZEND_MMAP_AHEAD should be greater than or equal to YYMAXFILL
#endif

#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

/* Globals Macros */
#define SCNG	LANG_SCNG
#ifdef ZTS
ZEND_API ts_rsrc_id language_scanner_globals_id;
#else
ZEND_API zend_php_scanner_globals language_scanner_globals;
#endif

#define HANDLE_NEWLINES(s, l)													\
do {																			\
	char *p = (s), *boundary = p+(l);											\
																				\
	while (p<boundary) {														\
		if (*p == '\n' || (*p == '\r' && (*(p+1) != '\n'))) {					\
			CG(zend_lineno)++;													\
		}																		\
		p++;																	\
	}																			\
} while (0)

#define HANDLE_NEWLINE(c) \
{ \
	if (c == '\n' || c == '\r') { \
		CG(zend_lineno)++; \
	} \
}

/* To save initial string length after scanning to first variable */
#define SET_DOUBLE_QUOTES_SCANNED_LENGTH(len) SCNG(scanned_string_len) = (len)
#define GET_DOUBLE_QUOTES_SCANNED_LENGTH()    SCNG(scanned_string_len)

#define IS_LABEL_START(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') || (c) == '_' || (c) >= 0x7F)

#define ZEND_IS_OCT(c)  ((c)>='0' && (c)<='7')
#define ZEND_IS_HEX(c)  (((c)>='0' && (c)<='9') || ((c)>='a' && (c)<='f') || ((c)>='A' && (c)<='F'))

BEGIN_EXTERN_C()

static size_t encoding_filter_script_to_internal(unsigned char **to, size_t *to_length, const unsigned char *from, size_t from_length)
{
	const zend_encoding *internal_encoding = zend_multibyte_get_internal_encoding();
	ZEND_ASSERT(internal_encoding);
	return zend_multibyte_encoding_converter(to, to_length, from, from_length, internal_encoding, LANG_SCNG(script_encoding));
}

static size_t encoding_filter_script_to_intermediate(unsigned char **to, size_t *to_length, const unsigned char *from, size_t from_length)
{
	return zend_multibyte_encoding_converter(to, to_length, from, from_length, zend_multibyte_encoding_utf8, LANG_SCNG(script_encoding));
}

static size_t encoding_filter_intermediate_to_script(unsigned char **to, size_t *to_length, const unsigned char *from, size_t from_length)
{
	return zend_multibyte_encoding_converter(to, to_length, from, from_length,
LANG_SCNG(script_encoding), zend_multibyte_encoding_utf8);
}

static size_t encoding_filter_intermediate_to_internal(unsigned char **to, size_t *to_length, const unsigned char *from, size_t from_length)
{
	const zend_encoding *internal_encoding = zend_multibyte_get_internal_encoding();
	ZEND_ASSERT(internal_encoding);
	return zend_multibyte_encoding_converter(to, to_length, from, from_length,
internal_encoding, zend_multibyte_encoding_utf8);
}


static void _yy_push_state(int new_state)
{
	zend_stack_push(&SCNG(state_stack), (void *) &YYGETCONDITION());
	YYSETCONDITION(new_state);
}

#define yy_push_state(state_and_tsrm) _yy_push_state(yyc##state_and_tsrm)

static void yy_pop_state(void)
{
	int *stack_state = zend_stack_top(&SCNG(state_stack));
	YYSETCONDITION(*stack_state);
	zend_stack_del_top(&SCNG(state_stack));
}

static void yy_scan_buffer(char *str, unsigned int len)
{
	YYCURSOR       = (YYCTYPE*)str;
	YYLIMIT        = YYCURSOR + len;
	if (!SCNG(yy_start)) {
		SCNG(yy_start) = YYCURSOR;
	}
}

void startup_scanner(void)
{
	CG(parse_error) = 0;
	CG(doc_comment) = NULL;
	zend_stack_init(&SCNG(state_stack), sizeof(int));
	zend_ptr_stack_init(&SCNG(heredoc_label_stack));
}

static void heredoc_label_dtor(zend_heredoc_label *heredoc_label) {
    efree(heredoc_label->label);
}

void shutdown_scanner(void)
{
	CG(parse_error) = 0;
	RESET_DOC_COMMENT();
	zend_stack_destroy(&SCNG(state_stack));
	zend_ptr_stack_clean(&SCNG(heredoc_label_stack), (void (*)(void *)) &heredoc_label_dtor, 1);
	zend_ptr_stack_destroy(&SCNG(heredoc_label_stack));
	SCNG(on_event) = NULL;
}

ZEND_API void zend_save_lexical_state(zend_lex_state *lex_state)
{
	lex_state->yy_leng   = SCNG(yy_leng);
	lex_state->yy_start  = SCNG(yy_start);
	lex_state->yy_text   = SCNG(yy_text);
	lex_state->yy_cursor = SCNG(yy_cursor);
	lex_state->yy_marker = SCNG(yy_marker);
	lex_state->yy_limit  = SCNG(yy_limit);

	lex_state->state_stack = SCNG(state_stack);
	zend_stack_init(&SCNG(state_stack), sizeof(int));

	lex_state->heredoc_label_stack = SCNG(heredoc_label_stack);
	zend_ptr_stack_init(&SCNG(heredoc_label_stack));

	lex_state->in = SCNG(yy_in);
	lex_state->yy_state = YYSTATE;
	lex_state->filename = zend_get_compiled_filename();
	lex_state->lineno = CG(zend_lineno);

	lex_state->script_org = SCNG(script_org);
	lex_state->script_org_size = SCNG(script_org_size);
	lex_state->script_filtered = SCNG(script_filtered);
	lex_state->script_filtered_size = SCNG(script_filtered_size);
	lex_state->input_filter = SCNG(input_filter);
	lex_state->output_filter = SCNG(output_filter);
	lex_state->script_encoding = SCNG(script_encoding);

	lex_state->on_event = SCNG(on_event);

	lex_state->ast = CG(ast);
	lex_state->ast_arena = CG(ast_arena);
}

ZEND_API void zend_restore_lexical_state(zend_lex_state *lex_state)
{
	SCNG(yy_leng)   = lex_state->yy_leng;
	SCNG(yy_start)  = lex_state->yy_start;
	SCNG(yy_text)   = lex_state->yy_text;
	SCNG(yy_cursor) = lex_state->yy_cursor;
	SCNG(yy_marker) = lex_state->yy_marker;
	SCNG(yy_limit)  = lex_state->yy_limit;

	zend_stack_destroy(&SCNG(state_stack));
	SCNG(state_stack) = lex_state->state_stack;

	zend_ptr_stack_clean(&SCNG(heredoc_label_stack), (void (*)(void *)) &heredoc_label_dtor, 1);
	zend_ptr_stack_destroy(&SCNG(heredoc_label_stack));
	SCNG(heredoc_label_stack) = lex_state->heredoc_label_stack;

	SCNG(yy_in) = lex_state->in;
	YYSETCONDITION(lex_state->yy_state);
	CG(zend_lineno) = lex_state->lineno;
	zend_restore_compiled_filename(lex_state->filename);

	if (SCNG(script_filtered)) {
		efree(SCNG(script_filtered));
		SCNG(script_filtered) = NULL;
	}
	SCNG(script_org) = lex_state->script_org;
	SCNG(script_org_size) = lex_state->script_org_size;
	SCNG(script_filtered) = lex_state->script_filtered;
	SCNG(script_filtered_size) = lex_state->script_filtered_size;
	SCNG(input_filter) = lex_state->input_filter;
	SCNG(output_filter) = lex_state->output_filter;
	SCNG(script_encoding) = lex_state->script_encoding;

	SCNG(on_event) = lex_state->on_event;

	CG(ast) = lex_state->ast;
	CG(ast_arena) = lex_state->ast_arena;

	RESET_DOC_COMMENT();
}

ZEND_API void zend_destroy_file_handle(zend_file_handle *file_handle)
{
	zend_llist_del_element(&CG(open_files), file_handle, (int (*)(void *, void *)) zend_compare_file_handles);
	/* zend_file_handle_dtor() operates on the copy, so we have to NULLify the original here */
	file_handle->opened_path = NULL;
	if (file_handle->free_filename) {
		file_handle->filename = NULL;
	}
}

ZEND_API void zend_lex_tstring(zval *zv)
{
	if (SCNG(on_event)) SCNG(on_event)(ON_FEEDBACK, T_STRING, 0);

	ZVAL_STRINGL(zv, (char*)SCNG(yy_text), SCNG(yy_leng));
}

#define BOM_UTF32_BE	"\x00\x00\xfe\xff"
#define	BOM_UTF32_LE	"\xff\xfe\x00\x00"
#define	BOM_UTF16_BE	"\xfe\xff"
#define	BOM_UTF16_LE	"\xff\xfe"
#define	BOM_UTF8		"\xef\xbb\xbf"

static const zend_encoding *zend_multibyte_detect_utf_encoding(const unsigned char *script, size_t script_size)
{
	const unsigned char *p;
	int wchar_size = 2;
	int le = 0;

	/* utf-16 or utf-32? */
	p = script;
	assert(p >= script);
	while ((size_t)(p-script) < script_size) {
		p = memchr(p, 0, script_size-(p-script)-2);
		if (!p) {
			break;
		}
		if (*(p+1) == '\0' && *(p+2) == '\0') {
			wchar_size = 4;
			break;
		}

		/* searching for UTF-32 specific byte orders, so this will do */
		p += 4;
	}

	/* BE or LE? */
	p = script;
	assert(p >= script);
	while ((size_t)(p-script) < script_size) {
		if (*p == '\0' && *(p+wchar_size-1) != '\0') {
			/* BE */
			le = 0;
			break;
		} else if (*p != '\0' && *(p+wchar_size-1) == '\0') {
			/* LE* */
			le = 1;
			break;
		}
		p += wchar_size;
	}

	if (wchar_size == 2) {
		return le ? zend_multibyte_encoding_utf16le : zend_multibyte_encoding_utf16be;
	} else {
		return le ? zend_multibyte_encoding_utf32le : zend_multibyte_encoding_utf32be;
	}

	return NULL;
}

static const zend_encoding* zend_multibyte_detect_unicode(void)
{
	const zend_encoding *script_encoding = NULL;
	int bom_size;
	unsigned char *pos1, *pos2;

	if (LANG_SCNG(script_org_size) < sizeof(BOM_UTF32_LE)-1) {
		return NULL;
	}

	/* check out BOM */
	if (!memcmp(LANG_SCNG(script_org), BOM_UTF32_BE, sizeof(BOM_UTF32_BE)-1)) {
		script_encoding = zend_multibyte_encoding_utf32be;
		bom_size = sizeof(BOM_UTF32_BE)-1;
	} else if (!memcmp(LANG_SCNG(script_org), BOM_UTF32_LE, sizeof(BOM_UTF32_LE)-1)) {
		script_encoding = zend_multibyte_encoding_utf32le;
		bom_size = sizeof(BOM_UTF32_LE)-1;
	} else if (!memcmp(LANG_SCNG(script_org), BOM_UTF16_BE, sizeof(BOM_UTF16_BE)-1)) {
		script_encoding = zend_multibyte_encoding_utf16be;
		bom_size = sizeof(BOM_UTF16_BE)-1;
	} else if (!memcmp(LANG_SCNG(script_org), BOM_UTF16_LE, sizeof(BOM_UTF16_LE)-1)) {
		script_encoding = zend_multibyte_encoding_utf16le;
		bom_size = sizeof(BOM_UTF16_LE)-1;
	} else if (!memcmp(LANG_SCNG(script_org), BOM_UTF8, sizeof(BOM_UTF8)-1)) {
		script_encoding = zend_multibyte_encoding_utf8;
		bom_size = sizeof(BOM_UTF8)-1;
	}

	if (script_encoding) {
		/* remove BOM */
		LANG_SCNG(script_org) += bom_size;
		LANG_SCNG(script_org_size) -= bom_size;

		return script_encoding;
	}

	/* script contains NULL bytes -> auto-detection */
	if ((pos1 = memchr(LANG_SCNG(script_org), 0, LANG_SCNG(script_org_size)))) {
		/* check if the NULL byte is after the __HALT_COMPILER(); */
		pos2 = LANG_SCNG(script_org);

		while (pos1 - pos2 >= sizeof("__HALT_COMPILER();")-1) {
			pos2 = memchr(pos2, '_', pos1 - pos2);
			if (!pos2) break;
			pos2++;
			if (strncasecmp((char*)pos2, "_HALT_COMPILER", sizeof("_HALT_COMPILER")-1) == 0) {
				pos2 += sizeof("_HALT_COMPILER")-1;
				while (*pos2 == ' '  ||
					   *pos2 == '\t' ||
					   *pos2 == '\r' ||
					   *pos2 == '\n') {
					pos2++;
				}
				if (*pos2 == '(') {
					pos2++;
					while (*pos2 == ' '  ||
						   *pos2 == '\t' ||
						   *pos2 == '\r' ||
						   *pos2 == '\n') {
						pos2++;
					}
					if (*pos2 == ')') {
						pos2++;
						while (*pos2 == ' '  ||
							   *pos2 == '\t' ||
							   *pos2 == '\r' ||
							   *pos2 == '\n') {
							pos2++;
						}
						if (*pos2 == ';') {
							return NULL;
						}
					}
				}
			}
		}
		/* make best effort if BOM is missing */
		return zend_multibyte_detect_utf_encoding(LANG_SCNG(script_org), LANG_SCNG(script_org_size));
	}

	return NULL;
}

static const zend_encoding* zend_multibyte_find_script_encoding(void)
{
	const zend_encoding *script_encoding;

	if (CG(detect_unicode)) {
		/* check out bom(byte order mark) and see if containing wchars */
		script_encoding = zend_multibyte_detect_unicode();
		if (script_encoding != NULL) {
			/* bom or wchar detection is prior to 'script_encoding' option */
			return script_encoding;
		}
	}

	/* if no script_encoding specified, just leave alone */
	if (!CG(script_encoding_list) || !CG(script_encoding_list_size)) {
		return NULL;
	}

	/* if multiple encodings specified, detect automagically */
	if (CG(script_encoding_list_size) > 1) {
		return zend_multibyte_encoding_detector(LANG_SCNG(script_org), LANG_SCNG(script_org_size), CG(script_encoding_list), CG(script_encoding_list_size));
	}

	return CG(script_encoding_list)[0];
}

ZEND_API int zend_multibyte_set_filter(const zend_encoding *onetime_encoding)
{
	const zend_encoding *internal_encoding = zend_multibyte_get_internal_encoding();
	const zend_encoding *script_encoding = onetime_encoding ? onetime_encoding: zend_multibyte_find_script_encoding();

	if (!script_encoding) {
		return FAILURE;
	}

	/* judge input/output filter */
	LANG_SCNG(script_encoding) = script_encoding;
	LANG_SCNG(input_filter) = NULL;
	LANG_SCNG(output_filter) = NULL;

	if (!internal_encoding || LANG_SCNG(script_encoding) == internal_encoding) {
		if (!zend_multibyte_check_lexer_compatibility(LANG_SCNG(script_encoding))) {
			/* and if not, work around w/ script_encoding -> utf-8 -> script_encoding conversion */
			LANG_SCNG(input_filter) = encoding_filter_script_to_intermediate;
			LANG_SCNG(output_filter) = encoding_filter_intermediate_to_script;
		} else {
			LANG_SCNG(input_filter) = NULL;
			LANG_SCNG(output_filter) = NULL;
		}
		return SUCCESS;
	}

	if (zend_multibyte_check_lexer_compatibility(internal_encoding)) {
		LANG_SCNG(input_filter) = encoding_filter_script_to_internal;
		LANG_SCNG(output_filter) = NULL;
	} else if (zend_multibyte_check_lexer_compatibility(LANG_SCNG(script_encoding))) {
		LANG_SCNG(input_filter) = NULL;
		LANG_SCNG(output_filter) = encoding_filter_script_to_internal;
	} else {
		/* both script and internal encodings are incompatible w/ flex */
		LANG_SCNG(input_filter) = encoding_filter_script_to_intermediate;
		LANG_SCNG(output_filter) = encoding_filter_intermediate_to_internal;
	}

	return 0;
}

ZEND_API int open_file_for_scanning(zend_file_handle *file_handle)
{
	char *buf;
	size_t size, offset = 0;
	zend_string *compiled_filename;

	/* The shebang line was read, get the current position to obtain the buffer start */
	if (CG(start_lineno) == 2 && file_handle->type == ZEND_HANDLE_FP && file_handle->handle.fp) {
		if ((offset = ftell(file_handle->handle.fp)) == -1) {
			offset = 0;
		}
	}

	if (zend_stream_fixup(file_handle, &buf, &size) == FAILURE) {
		return FAILURE;
	}

	zend_llist_add_element(&CG(open_files), file_handle);
	if (file_handle->handle.stream.handle >= (void*)file_handle && file_handle->handle.stream.handle <= (void*)(file_handle+1)) {
		zend_file_handle *fh = (zend_file_handle*)zend_llist_get_last(&CG(open_files));
		size_t diff = (char*)file_handle->handle.stream.handle - (char*)file_handle;
		fh->handle.stream.handle = (void*)(((char*)fh) + diff);
		file_handle->handle.stream.handle = fh->handle.stream.handle;
	}

	/* Reset the scanner for scanning the new file */
	SCNG(yy_in) = file_handle;
	SCNG(yy_start) = NULL;

	if (size != -1) {
		if (CG(multibyte)) {
			SCNG(script_org) = (unsigned char*)buf;
			SCNG(script_org_size) = size;
			SCNG(script_filtered) = NULL;

			zend_multibyte_set_filter(NULL);

			if (SCNG(input_filter)) {
				if ((size_t)-1 == SCNG(input_filter)(&SCNG(script_filtered), &SCNG(script_filtered_size), SCNG(script_org), SCNG(script_org_size))) {
					zend_error_noreturn(E_COMPILE_ERROR, "Could not convert the script from the detected "
							"encoding \"%s\" to a compatible encoding", zend_multibyte_get_encoding_name(LANG_SCNG(script_encoding)));
				}
				buf = (char*)SCNG(script_filtered);
				size = SCNG(script_filtered_size);
			}
		}
		SCNG(yy_start) = (unsigned char *)buf - offset;
		yy_scan_buffer(buf, (unsigned int)size);
	} else {
		zend_error_noreturn(E_COMPILE_ERROR, "zend_stream_mmap() failed");
	}

	BEGIN(INITIAL);

	if (file_handle->opened_path) {
		compiled_filename = zend_string_copy(file_handle->opened_path);
	} else {
		compiled_filename = zend_string_init(file_handle->filename, strlen(file_handle->filename), 0);
	}

	zend_set_compiled_filename(compiled_filename);
	zend_string_release(compiled_filename);

	if (CG(start_lineno)) {
		CG(zend_lineno) = CG(start_lineno);
		CG(start_lineno) = 0;
	} else {
		CG(zend_lineno) = 1;
	}

	RESET_DOC_COMMENT();
	CG(increment_lineno) = 0;
	return SUCCESS;
}
END_EXTERN_C()


ZEND_API zend_op_array *compile_file(zend_file_handle *file_handle, int type)
{
	zend_lex_state original_lex_state;
	zend_op_array *op_array = NULL;
	zend_save_lexical_state(&original_lex_state);

	if (open_file_for_scanning(file_handle)==FAILURE) {
		if (type==ZEND_REQUIRE) {
			zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, file_handle->filename);
			zend_bailout();
		} else {
			zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, file_handle->filename);
		}
	} else {
		zend_bool original_in_compilation = CG(in_compilation);
		CG(in_compilation) = 1;

		CG(ast) = NULL;
		CG(ast_arena) = zend_arena_create(1024 * 32);
		if (!zendparse()) {
			int last_lineno = CG(zend_lineno);
			zval retval_zv;
			zend_file_context original_file_context;
			zend_oparray_context original_oparray_context;
			zend_op_array *original_active_op_array = CG(active_op_array);
			op_array = emalloc(sizeof(zend_op_array));
			init_op_array(op_array, ZEND_USER_FUNCTION, INITIAL_OP_ARRAY_SIZE);
			CG(active_op_array) = op_array;
			ZVAL_LONG(&retval_zv, 1);

			if (zend_ast_process) {
				zend_ast_process(CG(ast));
			}

			zend_file_context_begin(&original_file_context);
			zend_oparray_context_begin(&original_oparray_context);
			zend_compile_top_stmt(CG(ast));
			CG(zend_lineno) = last_lineno;
			zend_emit_final_return(&retval_zv);
			op_array->line_start = 1;
			op_array->line_end = last_lineno;
			pass_two(op_array);
			zend_oparray_context_end(&original_oparray_context);
			zend_file_context_end(&original_file_context);

			CG(active_op_array) = original_active_op_array;
		}

		zend_ast_destroy(CG(ast));
		zend_arena_destroy(CG(ast_arena));
		CG(in_compilation) = original_in_compilation;
	}

	zend_restore_lexical_state(&original_lex_state);
	return op_array;
}


zend_op_array *compile_filename(int type, zval *filename)
{
	zend_file_handle file_handle;
	zval tmp;
	zend_op_array *retval;
	zend_string *opened_path = NULL;

	if (Z_TYPE_P(filename) != IS_STRING) {
		tmp = *filename;
		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		filename = &tmp;
	}
	file_handle.filename = Z_STRVAL_P(filename);
	file_handle.free_filename = 0;
	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.opened_path = NULL;
	file_handle.handle.fp = NULL;

	retval = zend_compile_file(&file_handle, type);
	if (retval && file_handle.handle.stream.handle) {
		if (!file_handle.opened_path) {
			file_handle.opened_path = opened_path = zend_string_copy(Z_STR_P(filename));
		}

		zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path);

		if (opened_path) {
			zend_string_release(opened_path);
		}
	}
	zend_destroy_file_handle(&file_handle);

	if (filename==&tmp) {
		zval_dtor(&tmp);
	}
	return retval;
}

ZEND_API int zend_prepare_string_for_scanning(zval *str, char *filename)
{
	char *buf;
	size_t size, old_len;
	zend_string *new_compiled_filename;

	/* enforce ZEND_MMAP_AHEAD trailing NULLs for flex... */
	old_len = Z_STRLEN_P(str);
	Z_STR_P(str) = zend_string_extend(Z_STR_P(str), old_len + ZEND_MMAP_AHEAD, 0);
	Z_TYPE_INFO_P(str) = IS_STRING_EX;
	memset(Z_STRVAL_P(str) + old_len, 0, ZEND_MMAP_AHEAD + 1);

	SCNG(yy_in) = NULL;
	SCNG(yy_start) = NULL;

	buf = Z_STRVAL_P(str);
	size = old_len;

	if (CG(multibyte)) {
		SCNG(script_org) = (unsigned char*)buf;
		SCNG(script_org_size) = size;
		SCNG(script_filtered) = NULL;

		zend_multibyte_set_filter(zend_multibyte_get_internal_encoding());

		if (SCNG(input_filter)) {
			if ((size_t)-1 == SCNG(input_filter)(&SCNG(script_filtered), &SCNG(script_filtered_size), SCNG(script_org), SCNG(script_org_size))) {
				zend_error_noreturn(E_COMPILE_ERROR, "Could not convert the script from the detected "
						"encoding \"%s\" to a compatible encoding", zend_multibyte_get_encoding_name(LANG_SCNG(script_encoding)));
			}
			buf = (char*)SCNG(script_filtered);
			size = SCNG(script_filtered_size);
		}
	}

	yy_scan_buffer(buf, (unsigned int)size);

	new_compiled_filename = zend_string_init(filename, strlen(filename), 0);
	zend_set_compiled_filename(new_compiled_filename);
	zend_string_release(new_compiled_filename);
	CG(zend_lineno) = 1;
	CG(increment_lineno) = 0;
	RESET_DOC_COMMENT();
	return SUCCESS;
}


ZEND_API size_t zend_get_scanned_file_offset(void)
{
	size_t offset = SCNG(yy_cursor) - SCNG(yy_start);
	if (SCNG(input_filter)) {
		size_t original_offset = offset, length = 0;
		do {
			unsigned char *p = NULL;
			if ((size_t)-1 == SCNG(input_filter)(&p, &length, SCNG(script_org), offset)) {
				return (size_t)-1;
			}
			efree(p);
			if (length > original_offset) {
				offset--;
			} else if (length < original_offset) {
				offset++;
			}
		} while (original_offset != length);
	}
	return offset;
}


zend_op_array *compile_string(zval *source_string, char *filename)
{
	zend_lex_state original_lex_state;
	zend_op_array *op_array = NULL;
	zval tmp;
	zend_bool original_in_compilation = CG(in_compilation);

	if (Z_STRLEN_P(source_string)==0) {
		return NULL;
	}

	ZVAL_DUP(&tmp, source_string);
	convert_to_string(&tmp);
	source_string = &tmp;

	CG(in_compilation) = 1;
	zend_save_lexical_state(&original_lex_state);
	if (zend_prepare_string_for_scanning(source_string, filename) == SUCCESS) {
		CG(ast) = NULL;
		CG(ast_arena) = zend_arena_create(1024 * 32);
		BEGIN(ST_IN_SCRIPTING);

		if (!zendparse()) {
			int last_lineno = CG(zend_lineno);
			zend_file_context original_file_context;
			zend_oparray_context original_oparray_context;
			zend_op_array *original_active_op_array = CG(active_op_array);
			op_array = emalloc(sizeof(zend_op_array));
			init_op_array(op_array, ZEND_EVAL_CODE, INITIAL_OP_ARRAY_SIZE);
			CG(active_op_array) = op_array;

			if (zend_ast_process) {
				zend_ast_process(CG(ast));
			}

			zend_file_context_begin(&original_file_context);
			zend_oparray_context_begin(&original_oparray_context);
			zend_compile_top_stmt(CG(ast));
			CG(zend_lineno) = last_lineno;
			zend_emit_final_return(NULL);
			op_array->line_start = 1;
			op_array->line_end = last_lineno;
			pass_two(op_array);
			zend_oparray_context_end(&original_oparray_context);
			zend_file_context_end(&original_file_context);

			CG(active_op_array) = original_active_op_array;
		}

		zend_ast_destroy(CG(ast));
		zend_arena_destroy(CG(ast_arena));
	}

	zend_restore_lexical_state(&original_lex_state);
	zval_dtor(&tmp);
	CG(in_compilation) = original_in_compilation;
	return op_array;
}


BEGIN_EXTERN_C()
int highlight_file(char *filename, zend_syntax_highlighter_ini *syntax_highlighter_ini)
{
	zend_lex_state original_lex_state;
	zend_file_handle file_handle;

	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.filename = filename;
	file_handle.free_filename = 0;
	file_handle.opened_path = NULL;
	zend_save_lexical_state(&original_lex_state);
	if (open_file_for_scanning(&file_handle)==FAILURE) {
		zend_message_dispatcher(ZMSG_FAILED_HIGHLIGHT_FOPEN, filename);
		zend_restore_lexical_state(&original_lex_state);
		return FAILURE;
	}
	zend_highlight(syntax_highlighter_ini);
	if (SCNG(script_filtered)) {
		efree(SCNG(script_filtered));
		SCNG(script_filtered) = NULL;
	}
	zend_destroy_file_handle(&file_handle);
	zend_restore_lexical_state(&original_lex_state);
	return SUCCESS;
}

int highlight_string(zval *str, zend_syntax_highlighter_ini *syntax_highlighter_ini, char *str_name)
{
	zend_lex_state original_lex_state;
	zval tmp = *str;

	str = &tmp;
	zval_copy_ctor(str);
	zend_save_lexical_state(&original_lex_state);
	if (zend_prepare_string_for_scanning(str, str_name)==FAILURE) {
		zend_restore_lexical_state(&original_lex_state);
		return FAILURE;
	}
	BEGIN(INITIAL);
	zend_highlight(syntax_highlighter_ini);
	if (SCNG(script_filtered)) {
		efree(SCNG(script_filtered));
		SCNG(script_filtered) = NULL;
	}
	zend_restore_lexical_state(&original_lex_state);
	zval_dtor(str);
	return SUCCESS;
}

ZEND_API void zend_multibyte_yyinput_again(zend_encoding_filter old_input_filter, const zend_encoding *old_encoding)
{
	size_t length;
	unsigned char *new_yy_start;

	/* convert and set */
	if (!SCNG(input_filter)) {
		if (SCNG(script_filtered)) {
			efree(SCNG(script_filtered));
			SCNG(script_filtered) = NULL;
		}
		SCNG(script_filtered_size) = 0;
		length = SCNG(script_org_size);
		new_yy_start = SCNG(script_org);
	} else {
		if ((size_t)-1 == SCNG(input_filter)(&new_yy_start, &length, SCNG(script_org), SCNG(script_org_size))) {
			zend_error_noreturn(E_COMPILE_ERROR, "Could not convert the script from the detected "
					"encoding \"%s\" to a compatible encoding", zend_multibyte_get_encoding_name(LANG_SCNG(script_encoding)));
		}
		if (SCNG(script_filtered)) {
			efree(SCNG(script_filtered));
		}
		SCNG(script_filtered) = new_yy_start;
		SCNG(script_filtered_size) = length;
	}

	SCNG(yy_cursor) = new_yy_start + (SCNG(yy_cursor) - SCNG(yy_start));
	SCNG(yy_marker) = new_yy_start + (SCNG(yy_marker) - SCNG(yy_start));
	SCNG(yy_text) = new_yy_start + (SCNG(yy_text) - SCNG(yy_start));
	SCNG(yy_limit) = new_yy_start + length;

	SCNG(yy_start) = new_yy_start;
}


// TODO: avoid reallocation ???
# define zend_copy_value(zendlval, yytext, yyleng) \
	if (SCNG(output_filter)) { \
		size_t sz = 0; \
		char *s = NULL; \
		SCNG(output_filter)((unsigned char **)&s, &sz, (unsigned char *)yytext, (size_t)yyleng); \
		ZVAL_STRINGL(zendlval, s, sz); \
		efree(s); \
	} else { \
		ZVAL_STRINGL(zendlval, yytext, yyleng); \
	}

static int zend_scan_escape_string(zval *zendlval, char *str, int len, char quote_type)
{
	register char *s, *t;
	char *end;

	ZVAL_STRINGL(zendlval, str, len);

	/* convert escape sequences */
	s = t = Z_STRVAL_P(zendlval);
	end = s+Z_STRLEN_P(zendlval);
	while (s<end) {
		if (*s=='\\') {
			s++;
			if (s >= end) {
				*t++ = '\\';
				break;
			}

			switch(*s) {
				case 'n':
					*t++ = '\n';
					Z_STRLEN_P(zendlval)--;
					break;
				case 'r':
					*t++ = '\r';
					Z_STRLEN_P(zendlval)--;
					break;
				case 't':
					*t++ = '\t';
					Z_STRLEN_P(zendlval)--;
					break;
				case 'f':
					*t++ = '\f';
					Z_STRLEN_P(zendlval)--;
					break;
				case 'v':
					*t++ = '\v';
					Z_STRLEN_P(zendlval)--;
					break;
				case 'e':
#ifdef ZEND_WIN32
					*t++ = VK_ESCAPE;
#else
					*t++ = '\e';
#endif
					Z_STRLEN_P(zendlval)--;
					break;
				case '"':
				case '`':
					if (*s != quote_type) {
						*t++ = '\\';
						*t++ = *s;
						break;
					}
				case '\\':
				case '$':
					*t++ = *s;
					Z_STRLEN_P(zendlval)--;
					break;
				case 'x':
				case 'X':
					if (ZEND_IS_HEX(*(s+1))) {
						char hex_buf[3] = { 0, 0, 0 };

						Z_STRLEN_P(zendlval)--; /* for the 'x' */

						hex_buf[0] = *(++s);
						Z_STRLEN_P(zendlval)--;
						if (ZEND_IS_HEX(*(s+1))) {
							hex_buf[1] = *(++s);
							Z_STRLEN_P(zendlval)--;
						}
						*t++ = (char) ZEND_STRTOL(hex_buf, NULL, 16);
					} else {
						*t++ = '\\';
						*t++ = *s;
					}
					break;
				/* UTF-8 codepoint escape, format: /\\u\{\x+\}/ */
				case 'u':
					{
						/* cache where we started so we can parse after validating */
						char *start = s + 1;
						size_t len = 0;
						zend_bool valid = 1;
						unsigned long codepoint;
						size_t byte_len = 0;

						if (*start != '{') {
							/* we silently let this pass to avoid breaking code
							 * with JSON in string literals (e.g. "\"\u202e\""
							 */
							*t++ = '\\';
							*t++ = 'u';
							break;
						} else {
							/* on the other hand, invalid \u{blah} errors */
							s++;
							len++;
							s++;
							while (*s != '}') {
								if (!ZEND_IS_HEX(*s)) {
									valid = 0;
									break;
								} else {
									len++;
								}
								s++;
							}
							if (*s == '}') {
								valid = 1;
								len++;
							}
						}

						/* \u{} is invalid */
						if (len <= 2) {
							valid = 0;
						}

						if (!valid) {
							zend_throw_exception(zend_ce_parse_error,
								"Invalid UTF-8 codepoint escape sequence", 0);
							zval_ptr_dtor(zendlval);
							ZVAL_UNDEF(zendlval);
							return FAILURE;
						}

						errno = 0;
						codepoint = strtoul(start + 1, NULL, 16);

						/* per RFC 3629, UTF-8 can only represent 21 bits */
						if (codepoint > 0x10FFFF || errno) {
							zend_throw_exception(zend_ce_parse_error,
								"Invalid UTF-8 codepoint escape sequence: Codepoint too large", 0);
							zval_ptr_dtor(zendlval);
							ZVAL_UNDEF(zendlval);
							return FAILURE;
						}

						/* based on https://en.wikipedia.org/wiki/UTF-8#Sample_code */
						if (codepoint < 0x80) {
							byte_len = 1;
							*t++ = codepoint;
						} else if (codepoint <= 0x7FF) {
							byte_len = 2;
							*t++ = (codepoint >> 6) + 0xC0;
							*t++ = (codepoint & 0x3F) + 0x80;
						} else if (codepoint <= 0xFFFF) {
							byte_len = 3;
							*t++ = (codepoint >> 12) + 0xE0;
							*t++ = ((codepoint >> 6) & 0x3F) + 0x80;
							*t++ = (codepoint & 0x3F) + 0x80;
						} else if (codepoint <= 0x10FFFF) {
							byte_len = 4;
							*t++ = (codepoint >> 18) + 0xF0;
							*t++ = ((codepoint >> 12) & 0x3F) + 0x80;
							*t++ = ((codepoint >> 6) & 0x3F) + 0x80;
							*t++ = (codepoint & 0x3F) + 0x80;
						}

						Z_STRLEN_P(zendlval) -= 2; /* \u */
						Z_STRLEN_P(zendlval) -= (len - byte_len);
					}
					break;
				default:
					/* check for an octal */
					if (ZEND_IS_OCT(*s)) {
						char octal_buf[4] = { 0, 0, 0, 0 };

						octal_buf[0] = *s;
						Z_STRLEN_P(zendlval)--;
						if (ZEND_IS_OCT(*(s+1))) {
							octal_buf[1] = *(++s);
							Z_STRLEN_P(zendlval)--;
							if (ZEND_IS_OCT(*(s+1))) {
								octal_buf[2] = *(++s);
								Z_STRLEN_P(zendlval)--;
							}
						}
						*t++ = (char) ZEND_STRTOL(octal_buf, NULL, 8);
					} else {
						*t++ = '\\';
						*t++ = *s;
					}
					break;
			}
		} else {
			*t++ = *s;
		}

		if (*s == '\n' || (*s == '\r' && (*(s+1) != '\n'))) {
			CG(zend_lineno)++;
		}
		s++;
	}
	*t = 0;
	if (SCNG(output_filter)) {
		size_t sz = 0;
		unsigned char *str;
		// TODO: avoid realocation ???
		s = Z_STRVAL_P(zendlval);
		SCNG(output_filter)(&str, &sz, (unsigned char *)s, (size_t)Z_STRLEN_P(zendlval));
		zval_ptr_dtor(zendlval);
		ZVAL_STRINGL(zendlval, (char *) str, sz);
		efree(str);
	}
	return SUCCESS;
}

static zend_always_inline int emit_token(int token, int token_line)
{
	if(SCNG(on_event)) SCNG(on_event)(ON_TOKEN, token, token_line);

	return token;
}

#define RETURN_TOKEN(token) return emit_token(token, start_line);

int lex_scan(zval *zendlval)
{

int start_line = CG(zend_lineno);

restart:
	SCNG(yy_text) = YYCURSOR;

/*!re2c
re2c:yyfill:check = 0;
LNUM	[0-9]+
DNUM	([0-9]*"."[0-9]+)|([0-9]+"."[0-9]*)
EXPONENT_DNUM	(({LNUM}|{DNUM})[eE][+-]?{LNUM})
HNUM	"0x"[0-9a-fA-F]+
BNUM	"0b"[01]+
LABEL	[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*
WHITESPACE [ \n\r\t]+
TABS_AND_SPACES [ \t]*
TOKENS [;:,.\[\]()|^&+-/*=%!~$<>?@]
ANY_CHAR [^]
NEWLINE ("\r"|"\n"|"\r\n")

/* compute yyleng before each rule */
<!*> := yyleng = YYCURSOR - SCNG(yy_text);

<ST_IN_SCRIPTING>"exit" {
	RETURN_TOKEN(T_EXIT);
}

<ST_IN_SCRIPTING>"die" {
	RETURN_TOKEN(T_EXIT);
}

<ST_IN_SCRIPTING>"function" {
	RETURN_TOKEN(T_FUNCTION);
}

<ST_IN_SCRIPTING>"const" {
	RETURN_TOKEN(T_CONST);
}

<ST_IN_SCRIPTING>"return" {
	RETURN_TOKEN(T_RETURN);
}

<ST_IN_SCRIPTING>"yield"{WHITESPACE}"from"[^a-zA-Z0-9_\x80-\xff] {
	yyless(yyleng - 1);
	HANDLE_NEWLINES(yytext, yyleng);
	RETURN_TOKEN(T_YIELD_FROM);
}

<ST_IN_SCRIPTING>"yield" {
	RETURN_TOKEN(T_YIELD);
}

<ST_IN_SCRIPTING>"try" {
	RETURN_TOKEN(T_TRY);
}

<ST_IN_SCRIPTING>"catch" {
	RETURN_TOKEN(T_CATCH);
}

<ST_IN_SCRIPTING>"finally" {
	RETURN_TOKEN(T_FINALLY);
}

<ST_IN_SCRIPTING>"throw" {
	RETURN_TOKEN(T_THROW);
}

<ST_IN_SCRIPTING>"if" {
	RETURN_TOKEN(T_IF);
}

<ST_IN_SCRIPTING>"elseif" {
	RETURN_TOKEN(T_ELSEIF);
}

<ST_IN_SCRIPTING>"endif" {
	RETURN_TOKEN(T_ENDIF);
}

<ST_IN_SCRIPTING>"else" {
	RETURN_TOKEN(T_ELSE);
}

<ST_IN_SCRIPTING>"while" {
	RETURN_TOKEN(T_WHILE);
}

<ST_IN_SCRIPTING>"endwhile" {
	RETURN_TOKEN(T_ENDWHILE);
}

<ST_IN_SCRIPTING>"do" {
	RETURN_TOKEN(T_DO);
}

<ST_IN_SCRIPTING>"for" {
	RETURN_TOKEN(T_FOR);
}

<ST_IN_SCRIPTING>"endfor" {
	RETURN_TOKEN(T_ENDFOR);
}

<ST_IN_SCRIPTING>"foreach" {
	RETURN_TOKEN(T_FOREACH);
}

<ST_IN_SCRIPTING>"endforeach" {
	RETURN_TOKEN(T_ENDFOREACH);
}

<ST_IN_SCRIPTING>"declare" {
	RETURN_TOKEN(T_DECLARE);
}

<ST_IN_SCRIPTING>"enddeclare" {
	RETURN_TOKEN(T_ENDDECLARE);
}

<ST_IN_SCRIPTING>"instanceof" {
	RETURN_TOKEN(T_INSTANCEOF);
}

<ST_IN_SCRIPTING>"as" {
	RETURN_TOKEN(T_AS);
}

<ST_IN_SCRIPTING>"switch" {
	RETURN_TOKEN(T_SWITCH);
}

<ST_IN_SCRIPTING>"endswitch" {
	RETURN_TOKEN(T_ENDSWITCH);
}

<ST_IN_SCRIPTING>"case" {
	RETURN_TOKEN(T_CASE);
}

<ST_IN_SCRIPTING>"default" {
	RETURN_TOKEN(T_DEFAULT);
}

<ST_IN_SCRIPTING>"break" {
	RETURN_TOKEN(T_BREAK);
}

<ST_IN_SCRIPTING>"continue" {
	RETURN_TOKEN(T_CONTINUE);
}

<ST_IN_SCRIPTING>"goto" {
	RETURN_TOKEN(T_GOTO);
}

<ST_IN_SCRIPTING>"echo" {
	RETURN_TOKEN(T_ECHO);
}

<ST_IN_SCRIPTING>"print" {
	RETURN_TOKEN(T_PRINT);
}

<ST_IN_SCRIPTING>"class" {
	RETURN_TOKEN(T_CLASS);
}

<ST_IN_SCRIPTING>"interface" {
	RETURN_TOKEN(T_INTERFACE);
}

<ST_IN_SCRIPTING>"trait" {
	RETURN_TOKEN(T_TRAIT);
}

<ST_IN_SCRIPTING>"extends" {
	RETURN_TOKEN(T_EXTENDS);
}

<ST_IN_SCRIPTING>"implements" {
	RETURN_TOKEN(T_IMPLEMENTS);
}

<ST_IN_SCRIPTING>"->" {
	yy_push_state(ST_LOOKING_FOR_PROPERTY);
	RETURN_TOKEN(T_OBJECT_OPERATOR);
}

<ST_IN_SCRIPTING,ST_LOOKING_FOR_PROPERTY>{WHITESPACE}+ {
	HANDLE_NEWLINES(yytext, yyleng);
	RETURN_TOKEN(T_WHITESPACE);
}

<ST_LOOKING_FOR_PROPERTY>"->" {
	RETURN_TOKEN(T_OBJECT_OPERATOR);
}

<ST_LOOKING_FOR_PROPERTY>{LABEL} {
	yy_pop_state();
	zend_copy_value(zendlval, yytext, yyleng);
	RETURN_TOKEN(T_STRING);
}

<ST_LOOKING_FOR_PROPERTY>{ANY_CHAR} {
	yyless(0);
	yy_pop_state();
	goto restart;
}

<ST_IN_SCRIPTING>"::" {
	RETURN_TOKEN(T_PAAMAYIM_NEKUDOTAYIM);
}

<ST_IN_SCRIPTING>"\\" {
	RETURN_TOKEN(T_NS_SEPARATOR);
}

<ST_IN_SCRIPTING>"..." {
	RETURN_TOKEN(T_ELLIPSIS);
}

<ST_IN_SCRIPTING>"??" {
	RETURN_TOKEN(T_COALESCE);
}

<ST_IN_SCRIPTING>"new" {
	RETURN_TOKEN(T_NEW);
}

<ST_IN_SCRIPTING>"clone" {
	RETURN_TOKEN(T_CLONE);
}

<ST_IN_SCRIPTING>"var" {
	RETURN_TOKEN(T_VAR);
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("int"|"integer"){TABS_AND_SPACES}")" {
	RETURN_TOKEN(T_INT_CAST);
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("real"|"double"|"float"){TABS_AND_SPACES}")" {
	RETURN_TOKEN(T_DOUBLE_CAST);
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("string"|"binary"){TABS_AND_SPACES}")" {
	RETURN_TOKEN(T_STRING_CAST);
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}"array"{TABS_AND_SPACES}")" {
	RETURN_TOKEN(T_ARRAY_CAST);
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}"object"{TABS_AND_SPACES}")" {
	RETURN_TOKEN(T_OBJECT_CAST);
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("bool"|"boolean"){TABS_AND_SPACES}")" {
	RETURN_TOKEN(T_BOOL_CAST);
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("unset"){TABS_AND_SPACES}")" {
	RETURN_TOKEN(T_UNSET_CAST);
}

<ST_IN_SCRIPTING>"eval" {
	RETURN_TOKEN(T_EVAL);
}

<ST_IN_SCRIPTING>"include" {
	RETURN_TOKEN(T_INCLUDE);
}

<ST_IN_SCRIPTING>"include_once" {
	RETURN_TOKEN(T_INCLUDE_ONCE);
}

<ST_IN_SCRIPTING>"require" {
	RETURN_TOKEN(T_REQUIRE);
}

<ST_IN_SCRIPTING>"require_once" {
	RETURN_TOKEN(T_REQUIRE_ONCE);
}

<ST_IN_SCRIPTING>"namespace" {
	RETURN_TOKEN(T_NAMESPACE);
}

<ST_IN_SCRIPTING>"use" {
	RETURN_TOKEN(T_USE);
}

<ST_IN_SCRIPTING>"insteadof" {
    RETURN_TOKEN(T_INSTEADOF);
}

<ST_IN_SCRIPTING>"global" {
	RETURN_TOKEN(T_GLOBAL);
}

<ST_IN_SCRIPTING>"isset" {
	RETURN_TOKEN(T_ISSET);
}

<ST_IN_SCRIPTING>"empty" {
	RETURN_TOKEN(T_EMPTY);
}

<ST_IN_SCRIPTING>"__halt_compiler" {
	RETURN_TOKEN(T_HALT_COMPILER);
}

<ST_IN_SCRIPTING>"static" {
	RETURN_TOKEN(T_STATIC);
}

<ST_IN_SCRIPTING>"abstract" {
	RETURN_TOKEN(T_ABSTRACT);
}

<ST_IN_SCRIPTING>"final" {
	RETURN_TOKEN(T_FINAL);
}

<ST_IN_SCRIPTING>"private" {
	RETURN_TOKEN(T_PRIVATE);
}

<ST_IN_SCRIPTING>"protected" {
	RETURN_TOKEN(T_PROTECTED);
}

<ST_IN_SCRIPTING>"public" {
	RETURN_TOKEN(T_PUBLIC);
}

<ST_IN_SCRIPTING>"unset" {
	RETURN_TOKEN(T_UNSET);
}

<ST_IN_SCRIPTING>"=>" {
	RETURN_TOKEN(T_DOUBLE_ARROW);
}

<ST_IN_SCRIPTING>"list" {
	RETURN_TOKEN(T_LIST);
}

<ST_IN_SCRIPTING>"array" {
	RETURN_TOKEN(T_ARRAY);
}

<ST_IN_SCRIPTING>"callable" {
	RETURN_TOKEN(T_CALLABLE);
}

<ST_IN_SCRIPTING>"++" {
	RETURN_TOKEN(T_INC);
}

<ST_IN_SCRIPTING>"--" {
	RETURN_TOKEN(T_DEC);
}

<ST_IN_SCRIPTING>"===" {
	RETURN_TOKEN(T_IS_IDENTICAL);
}

<ST_IN_SCRIPTING>"!==" {
	RETURN_TOKEN(T_IS_NOT_IDENTICAL);
}

<ST_IN_SCRIPTING>"==" {
	RETURN_TOKEN(T_IS_EQUAL);
}

<ST_IN_SCRIPTING>"!="|"<>" {
	RETURN_TOKEN(T_IS_NOT_EQUAL);
}

<ST_IN_SCRIPTING>"<=>" {
	RETURN_TOKEN(T_SPACESHIP);
}

<ST_IN_SCRIPTING>"<=" {
	RETURN_TOKEN(T_IS_SMALLER_OR_EQUAL);
}

<ST_IN_SCRIPTING>">=" {
	RETURN_TOKEN(T_IS_GREATER_OR_EQUAL);
}

<ST_IN_SCRIPTING>"+=" {
	RETURN_TOKEN(T_PLUS_EQUAL);
}

<ST_IN_SCRIPTING>"-=" {
	RETURN_TOKEN(T_MINUS_EQUAL);
}

<ST_IN_SCRIPTING>"*=" {
	RETURN_TOKEN(T_MUL_EQUAL);
}

<ST_IN_SCRIPTING>"*\*" {
	RETURN_TOKEN(T_POW);
}

<ST_IN_SCRIPTING>"*\*=" {
	RETURN_TOKEN(T_POW_EQUAL);
}

<ST_IN_SCRIPTING>"/=" {
	RETURN_TOKEN(T_DIV_EQUAL);
}

<ST_IN_SCRIPTING>".=" {
	RETURN_TOKEN(T_CONCAT_EQUAL);
}

<ST_IN_SCRIPTING>"%=" {
	RETURN_TOKEN(T_MOD_EQUAL);
}

<ST_IN_SCRIPTING>"<<=" {
	RETURN_TOKEN(T_SL_EQUAL);
}

<ST_IN_SCRIPTING>">>=" {
	RETURN_TOKEN(T_SR_EQUAL);
}

<ST_IN_SCRIPTING>"&=" {
	RETURN_TOKEN(T_AND_EQUAL);
}

<ST_IN_SCRIPTING>"|=" {
	RETURN_TOKEN(T_OR_EQUAL);
}

<ST_IN_SCRIPTING>"^=" {
	RETURN_TOKEN(T_XOR_EQUAL);
}

<ST_IN_SCRIPTING>"||" {
	RETURN_TOKEN(T_BOOLEAN_OR);
}

<ST_IN_SCRIPTING>"&&" {
	RETURN_TOKEN(T_BOOLEAN_AND);
}

<ST_IN_SCRIPTING>"OR" {
	RETURN_TOKEN(T_LOGICAL_OR);
}

<ST_IN_SCRIPTING>"AND" {
	RETURN_TOKEN(T_LOGICAL_AND);
}

<ST_IN_SCRIPTING>"XOR" {
	RETURN_TOKEN(T_LOGICAL_XOR);
}

<ST_IN_SCRIPTING>"<<" {
	RETURN_TOKEN(T_SL);
}

<ST_IN_SCRIPTING>">>" {
	RETURN_TOKEN(T_SR);
}

<ST_IN_SCRIPTING>{TOKENS} {
	RETURN_TOKEN(yytext[0]);
}


<ST_IN_SCRIPTING>"{" {
	yy_push_state(ST_IN_SCRIPTING);
	RETURN_TOKEN('{');
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"${" {
	yy_push_state(ST_LOOKING_FOR_VARNAME);
	RETURN_TOKEN(T_DOLLAR_OPEN_CURLY_BRACES);
}


<ST_IN_SCRIPTING>"}" {
	RESET_DOC_COMMENT();
	if (!zend_stack_is_empty(&SCNG(state_stack))) {
		yy_pop_state();
	}
	RETURN_TOKEN('}');
}


<ST_LOOKING_FOR_VARNAME>{LABEL}[[}] {
	yyless(yyleng - 1);
	zend_copy_value(zendlval, yytext, yyleng);
	yy_pop_state();
	yy_push_state(ST_IN_SCRIPTING);
	RETURN_TOKEN(T_STRING_VARNAME);
}


<ST_LOOKING_FOR_VARNAME>{ANY_CHAR} {
	yyless(0);
	yy_pop_state();
	yy_push_state(ST_IN_SCRIPTING);
	goto restart;
}

<ST_IN_SCRIPTING>{BNUM} {
	char *bin = yytext + 2; /* Skip "0b" */
	int len = yyleng - 2;
	char *end;

	/* Skip any leading 0s */
	while (*bin == '0') {
		++bin;
		--len;
	}

	if (len < SIZEOF_ZEND_LONG * 8) {
		if (len == 0) {
			ZVAL_LONG(zendlval, 0);
		} else {
			errno = 0;
			ZVAL_LONG(zendlval, ZEND_STRTOL(bin, &end, 2));
			ZEND_ASSERT(!errno && end == yytext + yyleng);
		}
		RETURN_TOKEN(T_LNUMBER);
	} else {
		ZVAL_DOUBLE(zendlval, zend_bin_strtod(bin, (const char **)&end));
		/* errno isn't checked since we allow HUGE_VAL/INF overflow */
		ZEND_ASSERT(end == yytext + yyleng);
		RETURN_TOKEN(T_DNUMBER);
	}
}

<ST_IN_SCRIPTING>{LNUM} {
	char *end;
	if (yyleng < MAX_LENGTH_OF_LONG - 1) { /* Won't overflow */
		errno = 0;
		ZVAL_LONG(zendlval, ZEND_STRTOL(yytext, &end, 0));
		/* This isn't an assert, we need to ensure 019 isn't valid octal
		 * Because the lexing itself doesn't do that for us
		 */
		if (end != yytext + yyleng) {
			zend_throw_exception(zend_ce_parse_error, "Invalid numeric literal", 0);
			ZVAL_UNDEF(zendlval);
			RETURN_TOKEN(T_LNUMBER);
		}
	} else {
		errno = 0;
		ZVAL_LONG(zendlval, ZEND_STRTOL(yytext, &end, 0));
		if (errno == ERANGE) { /* Overflow */
			errno = 0;
			if (yytext[0] == '0') { /* octal overflow */
				errno = 0;
				ZVAL_DOUBLE(zendlval, zend_oct_strtod(yytext, (const char **)&end));
			} else {
				ZVAL_DOUBLE(zendlval, zend_strtod(yytext, (const char **)&end));
			}
			/* Also not an assert for the same reason */
			if (end != yytext + yyleng) {
				zend_throw_exception(zend_ce_parse_error,
					"Invalid numeric literal", 0);
				ZVAL_UNDEF(zendlval);
				RETURN_TOKEN(T_DNUMBER);
			}
			ZEND_ASSERT(!errno);
			RETURN_TOKEN(T_DNUMBER);
		}
		/* Also not an assert for the same reason */
		if (end != yytext + yyleng) {
			zend_throw_exception(zend_ce_parse_error, "Invalid numeric literal", 0);
			ZVAL_UNDEF(zendlval);
			RETURN_TOKEN(T_DNUMBER);
		}
	}
	ZEND_ASSERT(!errno);
	RETURN_TOKEN(T_LNUMBER);
}

<ST_IN_SCRIPTING>{HNUM} {
	char *hex = yytext + 2; /* Skip "0x" */
	int len = yyleng - 2;
	char *end;

	/* Skip any leading 0s */
	while (*hex == '0') {
		hex++;
		len--;
	}

	if (len < SIZEOF_ZEND_LONG * 2 || (len == SIZEOF_ZEND_LONG * 2 && *hex <= '7')) {
		if (len == 0) {
			ZVAL_LONG(zendlval, 0);
		} else {
			errno = 0;
			ZVAL_LONG(zendlval, ZEND_STRTOL(hex, &end, 16));
			ZEND_ASSERT(!errno && end == hex + len);
		}
		RETURN_TOKEN(T_LNUMBER);
	} else {
		ZVAL_DOUBLE(zendlval, zend_hex_strtod(hex, (const char **)&end));
		/* errno isn't checked since we allow HUGE_VAL/INF overflow */
		ZEND_ASSERT(end == hex + len);
		RETURN_TOKEN(T_DNUMBER);
	}
}

<ST_VAR_OFFSET>[0]|([1-9][0-9]*) { /* Offset could be treated as a long */
	if (yyleng < MAX_LENGTH_OF_LONG - 1 || (yyleng == MAX_LENGTH_OF_LONG - 1 && strcmp(yytext, long_min_digits) < 0)) {
		char *end;
		errno = 0;
		ZVAL_LONG(zendlval, ZEND_STRTOL(yytext, &end, 10));
		if (errno == ERANGE) {
			goto string;
		}
		ZEND_ASSERT(end == yytext + yyleng);
	} else {
string:
		ZVAL_STRINGL(zendlval, yytext, yyleng);
	}
	RETURN_TOKEN(T_NUM_STRING);
}

<ST_VAR_OFFSET>{LNUM}|{HNUM}|{BNUM} { /* Offset must be treated as a string */
	ZVAL_STRINGL(zendlval, yytext, yyleng);
	RETURN_TOKEN(T_NUM_STRING);
}

<ST_IN_SCRIPTING>{DNUM}|{EXPONENT_DNUM} {
	const char *end;

	ZVAL_DOUBLE(zendlval, zend_strtod(yytext, &end));
	/* errno isn't checked since we allow HUGE_VAL/INF overflow */
	ZEND_ASSERT(end == yytext + yyleng);
	RETURN_TOKEN(T_DNUMBER);
}

<ST_IN_SCRIPTING>"__CLASS__" {
	RETURN_TOKEN(T_CLASS_C);
}

<ST_IN_SCRIPTING>"__TRAIT__" {
	RETURN_TOKEN(T_TRAIT_C);
}

<ST_IN_SCRIPTING>"__FUNCTION__" {
	RETURN_TOKEN(T_FUNC_C);
}

<ST_IN_SCRIPTING>"__METHOD__" {
	RETURN_TOKEN(T_METHOD_C);
}

<ST_IN_SCRIPTING>"__LINE__" {
	RETURN_TOKEN(T_LINE);
}

<ST_IN_SCRIPTING>"__FILE__" {
	RETURN_TOKEN(T_FILE);
}

<ST_IN_SCRIPTING>"__DIR__" {
	RETURN_TOKEN(T_DIR);
}

<ST_IN_SCRIPTING>"__NAMESPACE__" {
	RETURN_TOKEN(T_NS_C);
}


<INITIAL>"<?=" {
	BEGIN(ST_IN_SCRIPTING);
	RETURN_TOKEN(T_OPEN_TAG_WITH_ECHO);
}


<INITIAL>"<?php"([ \t]|{NEWLINE}) {
	HANDLE_NEWLINE(yytext[yyleng-1]);
	BEGIN(ST_IN_SCRIPTING);
	RETURN_TOKEN(T_OPEN_TAG);
}


<INITIAL>"<?" {
	if (CG(short_tags)) {
		BEGIN(ST_IN_SCRIPTING);
		RETURN_TOKEN(T_OPEN_TAG);
	} else {
		goto inline_char_handler;
	}
}

<INITIAL>{ANY_CHAR} {
	if (YYCURSOR > YYLIMIT) {
		RETURN_TOKEN(END);
	}

inline_char_handler:

	while (1) {
		YYCTYPE *ptr = memchr(YYCURSOR, '<', YYLIMIT - YYCURSOR);

		YYCURSOR = ptr ? ptr + 1 : YYLIMIT;

		if (YYCURSOR >= YYLIMIT) {
			break;
		}

		if (*YYCURSOR == '?') {
			if (CG(short_tags) || !strncasecmp((char*)YYCURSOR + 1, "php", 3) || (*(YYCURSOR + 1) == '=')) { /* Assume [ \t\n\r] follows "php" */

				YYCURSOR--;
				break;
			}
		}
	}

	yyleng = YYCURSOR - SCNG(yy_text);

	if (SCNG(output_filter)) {
		size_t readsize;
		char *s = NULL;
		size_t sz = 0;
		// TODO: avoid reallocation ???
		readsize = SCNG(output_filter)((unsigned char **)&s, &sz, (unsigned char *)yytext, (size_t)yyleng);
		ZVAL_STRINGL(zendlval, s, sz);
		efree(s);
		if (readsize < yyleng) {
			yyless(readsize);
		}
	} else {
	  ZVAL_STRINGL(zendlval, yytext, yyleng);
	}
	HANDLE_NEWLINES(yytext, yyleng);
	RETURN_TOKEN(T_INLINE_HTML);
}


/* Make sure a label character follows "->", otherwise there is no property
 * and "->" will be taken literally
 */
<ST_DOUBLE_QUOTES,ST_HEREDOC,ST_BACKQUOTE>"$"{LABEL}"->"[a-zA-Z_\x7f-\xff] {
	yyless(yyleng - 3);
	yy_push_state(ST_LOOKING_FOR_PROPERTY);
	zend_copy_value(zendlval, (yytext+1), (yyleng-1));
	RETURN_TOKEN(T_VARIABLE);
}

/* A [ always designates a variable offset, regardless of what follows
 */
<ST_DOUBLE_QUOTES,ST_HEREDOC,ST_BACKQUOTE>"$"{LABEL}"[" {
	yyless(yyleng - 1);
	yy_push_state(ST_VAR_OFFSET);
	zend_copy_value(zendlval, (yytext+1), (yyleng-1));
	RETURN_TOKEN(T_VARIABLE);
}

<ST_IN_SCRIPTING,ST_DOUBLE_QUOTES,ST_HEREDOC,ST_BACKQUOTE,ST_VAR_OFFSET>"$"{LABEL} {
	zend_copy_value(zendlval, (yytext+1), (yyleng-1));
	RETURN_TOKEN(T_VARIABLE);
}

<ST_VAR_OFFSET>"]" {
	yy_pop_state();
	RETURN_TOKEN(']');
}

<ST_VAR_OFFSET>{TOKENS}|[{}"`] {
	/* Only '[' can be valid, but returning other tokens will allow a more explicit parse error */
	RETURN_TOKEN(yytext[0]);
}

<ST_VAR_OFFSET>[ \n\r\t\\'#] {
	/* Invalid rule to return a more explicit parse error with proper line number */
	yyless(0);
	yy_pop_state();
	ZVAL_NULL(zendlval);
	RETURN_TOKEN(T_ENCAPSED_AND_WHITESPACE);
}

<ST_IN_SCRIPTING,ST_VAR_OFFSET>{LABEL} {
	zend_copy_value(zendlval, yytext, yyleng);
	RETURN_TOKEN(T_STRING);
}


<ST_IN_SCRIPTING>"#"|"//" {
	while (YYCURSOR < YYLIMIT) {
		switch (*YYCURSOR++) {
			case '\r':
				if (*YYCURSOR == '\n') {
					YYCURSOR++;
				}
				/* fall through */
			case '\n':
				CG(zend_lineno)++;
				break;
			case '?':
				if (*YYCURSOR == '>') {
					YYCURSOR--;
					break;
				}
				/* fall through */
			default:
				continue;
		}

		break;
	}

	yyleng = YYCURSOR - SCNG(yy_text);

	RETURN_TOKEN(T_COMMENT);
}

<ST_IN_SCRIPTING>"/*"|"/**"{WHITESPACE} {
	int doc_com;

	if (yyleng > 2) {
		doc_com = 1;
		RESET_DOC_COMMENT();
	} else {
		doc_com = 0;
	}

	while (YYCURSOR < YYLIMIT) {
		if (*YYCURSOR++ == '*' && *YYCURSOR == '/') {
			break;
		}
	}

	if (YYCURSOR < YYLIMIT) {
		YYCURSOR++;
	} else {
		zend_error(E_COMPILE_WARNING, "Unterminated comment starting line %d", CG(zend_lineno));
	}

	yyleng = YYCURSOR - SCNG(yy_text);
	HANDLE_NEWLINES(yytext, yyleng);

	if (doc_com) {
		CG(doc_comment) = zend_string_init(yytext, yyleng, 0);
		RETURN_TOKEN(T_DOC_COMMENT);
	}

	RETURN_TOKEN(T_COMMENT);
}

<ST_IN_SCRIPTING>"?>"{NEWLINE}? {
	BEGIN(INITIAL);
	RETURN_TOKEN(T_CLOSE_TAG);  /* implicit ';' at php-end tag */
}


<ST_IN_SCRIPTING>b?['] {
	register char *s, *t;
	char *end;
	int bprefix = (yytext[0] != '\'') ? 1 : 0;

	while (1) {
		if (YYCURSOR < YYLIMIT) {
			if (*YYCURSOR == '\'') {
				YYCURSOR++;
				yyleng = YYCURSOR - SCNG(yy_text);

				break;
			} else if (*YYCURSOR++ == '\\' && YYCURSOR < YYLIMIT) {
				YYCURSOR++;
			}
		} else {
			yyleng = YYLIMIT - SCNG(yy_text);

			/* Unclosed single quotes; treat similar to double quotes, but without a separate token
			 * for ' (unrecognized by parser), instead of old flex fallback to "Unexpected character..."
			 * rule, which continued in ST_IN_SCRIPTING state after the quote */
			ZVAL_NULL(zendlval);
			RETURN_TOKEN(T_ENCAPSED_AND_WHITESPACE);
		}
	}

	ZVAL_STRINGL(zendlval, yytext+bprefix+1, yyleng-bprefix-2);

	/* convert escape sequences */
	s = t = Z_STRVAL_P(zendlval);
	end = s+Z_STRLEN_P(zendlval);
	while (s<end) {
		if (*s=='\\') {
			s++;

			switch(*s) {
				case '\\':
				case '\'':
					*t++ = *s;
					Z_STRLEN_P(zendlval)--;
					break;
				default:
					*t++ = '\\';
					*t++ = *s;
					break;
			}
		} else {
			*t++ = *s;
		}

		if (*s == '\n' || (*s == '\r' && (*(s+1) != '\n'))) {
			CG(zend_lineno)++;
		}
		s++;
	}
	*t = 0;

	if (SCNG(output_filter)) {
		size_t sz = 0;
		char *str = NULL;
		s = Z_STRVAL_P(zendlval);
		// TODO: avoid reallocation ???
		SCNG(output_filter)((unsigned char **)&str, &sz, (unsigned char *)s, (size_t)Z_STRLEN_P(zendlval));
		ZVAL_STRINGL(zendlval, str, sz);
	}
	RETURN_TOKEN(T_CONSTANT_ENCAPSED_STRING);
}


<ST_IN_SCRIPTING>b?["] {
	int bprefix = (yytext[0] != '"') ? 1 : 0;

	while (YYCURSOR < YYLIMIT) {
		switch (*YYCURSOR++) {
			case '"':
				yyleng = YYCURSOR - SCNG(yy_text);
				zend_scan_escape_string(zendlval, yytext+bprefix+1, yyleng-bprefix-2, '"');
				RETURN_TOKEN(T_CONSTANT_ENCAPSED_STRING);
			case '$':
				if (IS_LABEL_START(*YYCURSOR) || *YYCURSOR == '{') {
					break;
				}
				continue;
			case '{':
				if (*YYCURSOR == '$') {
					break;
				}
				continue;
			case '\\':
				if (YYCURSOR < YYLIMIT) {
					YYCURSOR++;
				}
				/* fall through */
			default:
				continue;
		}

		YYCURSOR--;
		break;
	}

	/* Remember how much was scanned to save rescanning */
	SET_DOUBLE_QUOTES_SCANNED_LENGTH(YYCURSOR - SCNG(yy_text) - yyleng);

	YYCURSOR = SCNG(yy_text) + yyleng;

	BEGIN(ST_DOUBLE_QUOTES);
	RETURN_TOKEN('"');
}


<ST_IN_SCRIPTING>b?"<<<"{TABS_AND_SPACES}({LABEL}|([']{LABEL}['])|(["]{LABEL}["])){NEWLINE} {
	char *s;
	int bprefix = (yytext[0] != '<') ? 1 : 0;
	zend_heredoc_label *heredoc_label = emalloc(sizeof(zend_heredoc_label));

	CG(zend_lineno)++;
	heredoc_label->length = yyleng-bprefix-3-1-(yytext[yyleng-2]=='\r'?1:0);
	s = yytext+bprefix+3;
	while ((*s == ' ') || (*s == '\t')) {
		s++;
		heredoc_label->length--;
	}

	if (*s == '\'') {
		s++;
		heredoc_label->length -= 2;

		BEGIN(ST_NOWDOC);
	} else {
		if (*s == '"') {
			s++;
			heredoc_label->length -= 2;
		}

		BEGIN(ST_HEREDOC);
	}

	heredoc_label->label = estrndup(s, heredoc_label->length);

	/* Check for ending label on the next line */
	if (heredoc_label->length < YYLIMIT - YYCURSOR && !memcmp(YYCURSOR, s, heredoc_label->length)) {
		YYCTYPE *end = YYCURSOR + heredoc_label->length;

		if (*end == ';') {
			end++;
		}

		if (*end == '\n' || *end == '\r') {
			BEGIN(ST_END_HEREDOC);
		}
	}

	zend_ptr_stack_push(&SCNG(heredoc_label_stack), (void *) heredoc_label);

	RETURN_TOKEN(T_START_HEREDOC);
}


<ST_IN_SCRIPTING>[`] {
	BEGIN(ST_BACKQUOTE);
	RETURN_TOKEN('`');
}


<ST_END_HEREDOC>{ANY_CHAR} {
	zend_heredoc_label *heredoc_label = zend_ptr_stack_pop(&SCNG(heredoc_label_stack));

	YYCURSOR += heredoc_label->length - 1;
	yyleng = heredoc_label->length;

	heredoc_label_dtor(heredoc_label);
	efree(heredoc_label);

	BEGIN(ST_IN_SCRIPTING);
	RETURN_TOKEN(T_END_HEREDOC);
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"{$" {
	Z_LVAL_P(zendlval) = (zend_long) '{';
	yy_push_state(ST_IN_SCRIPTING);
	yyless(1);
	RETURN_TOKEN(T_CURLY_OPEN);
}


<ST_DOUBLE_QUOTES>["] {
	BEGIN(ST_IN_SCRIPTING);
	RETURN_TOKEN('"');
}

<ST_BACKQUOTE>[`] {
	BEGIN(ST_IN_SCRIPTING);
	RETURN_TOKEN('`');
}


<ST_DOUBLE_QUOTES>{ANY_CHAR} {
	if (GET_DOUBLE_QUOTES_SCANNED_LENGTH()) {
		YYCURSOR += GET_DOUBLE_QUOTES_SCANNED_LENGTH() - 1;
		SET_DOUBLE_QUOTES_SCANNED_LENGTH(0);

		goto double_quotes_scan_done;
	}

	if (YYCURSOR > YYLIMIT) {
		RETURN_TOKEN(END);
	}
	if (yytext[0] == '\\' && YYCURSOR < YYLIMIT) {
		YYCURSOR++;
	}

	while (YYCURSOR < YYLIMIT) {
		switch (*YYCURSOR++) {
			case '"':
				break;
			case '$':
				if (IS_LABEL_START(*YYCURSOR) || *YYCURSOR == '{') {
					break;
				}
				continue;
			case '{':
				if (*YYCURSOR == '$') {
					break;
				}
				continue;
			case '\\':
				if (YYCURSOR < YYLIMIT) {
					YYCURSOR++;
				}
				/* fall through */
			default:
				continue;
		}

		YYCURSOR--;
		break;
	}

double_quotes_scan_done:
	yyleng = YYCURSOR - SCNG(yy_text);

	zend_scan_escape_string(zendlval, yytext, yyleng, '"');
	RETURN_TOKEN(T_ENCAPSED_AND_WHITESPACE);
}


<ST_BACKQUOTE>{ANY_CHAR} {
	if (YYCURSOR > YYLIMIT) {
		RETURN_TOKEN(END);
	}
	if (yytext[0] == '\\' && YYCURSOR < YYLIMIT) {
		YYCURSOR++;
	}

	while (YYCURSOR < YYLIMIT) {
		switch (*YYCURSOR++) {
			case '`':
				break;
			case '$':
				if (IS_LABEL_START(*YYCURSOR) || *YYCURSOR == '{') {
					break;
				}
				continue;
			case '{':
				if (*YYCURSOR == '$') {
					break;
				}
				continue;
			case '\\':
				if (YYCURSOR < YYLIMIT) {
					YYCURSOR++;
				}
				/* fall through */
			default:
				continue;
		}

		YYCURSOR--;
		break;
	}

	yyleng = YYCURSOR - SCNG(yy_text);

	zend_scan_escape_string(zendlval, yytext, yyleng, '`');
	RETURN_TOKEN(T_ENCAPSED_AND_WHITESPACE);
}


<ST_HEREDOC>{ANY_CHAR} {
	int newline = 0;

	zend_heredoc_label *heredoc_label = zend_ptr_stack_top(&SCNG(heredoc_label_stack));

	if (YYCURSOR > YYLIMIT) {
		RETURN_TOKEN(END);
	}

	YYCURSOR--;

	while (YYCURSOR < YYLIMIT) {
		switch (*YYCURSOR++) {
			case '\r':
				if (*YYCURSOR == '\n') {
					YYCURSOR++;
				}
				/* fall through */
			case '\n':
				/* Check for ending label on the next line */
				if (IS_LABEL_START(*YYCURSOR) && heredoc_label->length < YYLIMIT - YYCURSOR && !memcmp(YYCURSOR, heredoc_label->label, heredoc_label->length)) {
					YYCTYPE *end = YYCURSOR + heredoc_label->length;

					if (*end == ';') {
						end++;
					}

					if (*end == '\n' || *end == '\r') {
						/* newline before label will be subtracted from returned text, but
						 * yyleng/yytext will include it, for zend_highlight/strip, tokenizer, etc. */
						if (YYCURSOR[-2] == '\r' && YYCURSOR[-1] == '\n') {
							newline = 2; /* Windows newline */
						} else {
							newline = 1;
						}

						CG(increment_lineno) = 1; /* For newline before label */
						BEGIN(ST_END_HEREDOC);

						goto heredoc_scan_done;
					}
				}
				continue;
			case '$':
				if (IS_LABEL_START(*YYCURSOR) || *YYCURSOR == '{') {
					break;
				}
				continue;
			case '{':
				if (*YYCURSOR == '$') {
					break;
				}
				continue;
			case '\\':
				if (YYCURSOR < YYLIMIT && *YYCURSOR != '\n' && *YYCURSOR != '\r') {
					YYCURSOR++;
				}
				/* fall through */
			default:
				continue;
		}

		YYCURSOR--;
		break;
	}

heredoc_scan_done:
	yyleng = YYCURSOR - SCNG(yy_text);

	zend_scan_escape_string(zendlval, yytext, yyleng - newline, 0);
	RETURN_TOKEN(T_ENCAPSED_AND_WHITESPACE);
}


<ST_NOWDOC>{ANY_CHAR} {
	int newline = 0;

	zend_heredoc_label *heredoc_label = zend_ptr_stack_top(&SCNG(heredoc_label_stack));

	if (YYCURSOR > YYLIMIT) {
		RETURN_TOKEN(END);
	}

	YYCURSOR--;

	while (YYCURSOR < YYLIMIT) {
		switch (*YYCURSOR++) {
			case '\r':
				if (*YYCURSOR == '\n') {
					YYCURSOR++;
				}
				/* fall through */
			case '\n':
				/* Check for ending label on the next line */
				if (IS_LABEL_START(*YYCURSOR) && heredoc_label->length < YYLIMIT - YYCURSOR && !memcmp(YYCURSOR, heredoc_label->label, heredoc_label->length)) {
					YYCTYPE *end = YYCURSOR + heredoc_label->length;

					if (*end == ';') {
						end++;
					}

					if (*end == '\n' || *end == '\r') {
						/* newline before label will be subtracted from returned text, but
						 * yyleng/yytext will include it, for zend_highlight/strip, tokenizer, etc. */
						if (YYCURSOR[-2] == '\r' && YYCURSOR[-1] == '\n') {
							newline = 2; /* Windows newline */
						} else {
							newline = 1;
						}

						CG(increment_lineno) = 1; /* For newline before label */
						BEGIN(ST_END_HEREDOC);

						goto nowdoc_scan_done;
					}
				}
				/* fall through */
			default:
				continue;
		}
	}

nowdoc_scan_done:
	yyleng = YYCURSOR - SCNG(yy_text);

	zend_copy_value(zendlval, yytext, yyleng - newline);
	HANDLE_NEWLINES(yytext, yyleng - newline);
	RETURN_TOKEN(T_ENCAPSED_AND_WHITESPACE);
}


<ST_IN_SCRIPTING,ST_VAR_OFFSET>{ANY_CHAR} {
	if (YYCURSOR > YYLIMIT) {
		RETURN_TOKEN(END);
	}

	zend_error(E_COMPILE_WARNING,"Unexpected character in input:  '%c' (ASCII=%d) state=%d", yytext[0], yytext[0], YYSTATE);
	goto restart;
}

*/
}
