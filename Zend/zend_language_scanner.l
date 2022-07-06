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
   | Authors: Marcus Boerger <helly@php.net>                              |
   |          Nuno Lopes <nlopess@php.net>                                |
   |          Scott MacVicar <scottmac@php.net>                           |
   | Flex version authors:                                                |
   |          Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

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

#include <stdarg.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

/* Globals Macros */
#define SCNG	LANG_SCNG
#ifdef ZTS
ZEND_API ts_rsrc_id language_scanner_globals_id;
ZEND_API size_t language_scanner_globals_offset;
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

#define IS_LABEL_START(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') || (c) == '_' || (c) >= 0x80)
#define IS_LABEL_SUCCESSOR(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') || ((c) >= '0' && (c) <= '9') || (c) == '_' || (c) >= 0x80)

#define ZEND_IS_OCT(c)  ((c)>='0' && (c)<='7')
#define ZEND_IS_HEX(c)  (((c)>='0' && (c)<='9') || ((c)>='a' && (c)<='f') || ((c)>='A' && (c)<='F'))

BEGIN_EXTERN_C()

static void strip_underscores(char *str, size_t *len)
{
	char *src = str, *dest = str;
	while (*src != '\0') {
		if (*src != '_') {
			*dest = *src;
			dest++;
		} else {
			--(*len);
		}
		src++;
	}
	*dest = '\0';
}

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

static void yy_scan_buffer(char *str, size_t len)
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
	CG(extra_fn_flags) = 0;
	zend_stack_init(&SCNG(state_stack), sizeof(int));
	zend_stack_init(&SCNG(nest_location_stack), sizeof(zend_nest_location));
	zend_ptr_stack_init(&SCNG(heredoc_label_stack));
	SCNG(heredoc_scan_ahead) = 0;
}

static void heredoc_label_dtor(zend_heredoc_label *heredoc_label) {
    efree(heredoc_label->label);
}

void shutdown_scanner(void)
{
	CG(parse_error) = 0;
	RESET_DOC_COMMENT();
	zend_stack_destroy(&SCNG(state_stack));
	zend_stack_destroy(&SCNG(nest_location_stack));
	zend_ptr_stack_clean(&SCNG(heredoc_label_stack), (void (*)(void *)) &heredoc_label_dtor, 1);
	zend_ptr_stack_destroy(&SCNG(heredoc_label_stack));
	SCNG(heredoc_scan_ahead) = 0;
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

	lex_state->nest_location_stack = SCNG(nest_location_stack);
	zend_stack_init(&SCNG(nest_location_stack), sizeof(zend_nest_location));

	lex_state->heredoc_label_stack = SCNG(heredoc_label_stack);
	zend_ptr_stack_init(&SCNG(heredoc_label_stack));

	lex_state->in = SCNG(yy_in);
	lex_state->yy_state = YYSTATE;
	lex_state->filename = CG(compiled_filename);
	lex_state->lineno = CG(zend_lineno);
	CG(compiled_filename) = NULL;

	lex_state->script_org = SCNG(script_org);
	lex_state->script_org_size = SCNG(script_org_size);
	lex_state->script_filtered = SCNG(script_filtered);
	lex_state->script_filtered_size = SCNG(script_filtered_size);
	lex_state->input_filter = SCNG(input_filter);
	lex_state->output_filter = SCNG(output_filter);
	lex_state->script_encoding = SCNG(script_encoding);

	lex_state->on_event = SCNG(on_event);
	lex_state->on_event_context = SCNG(on_event_context);

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

	zend_stack_destroy(&SCNG(nest_location_stack));
	SCNG(nest_location_stack) = lex_state->nest_location_stack;

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
	SCNG(on_event_context) = lex_state->on_event_context;

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

ZEND_API zend_result zend_lex_tstring(zval *zv, zend_lexer_ident_ref ident_ref)
{
	char *ident = (char *) SCNG(yy_start) + ident_ref.offset;
	size_t length = ident_ref.len;
	if (length == sizeof("<?=")-1 && memcmp(ident, "<?=", sizeof("<?=")-1) == 0) {
		zend_throw_exception(zend_ce_parse_error, "Cannot use \"<?=\" as an identifier", 0);
		return FAILURE;
	}

	if (SCNG(on_event)) {
		SCNG(on_event)(ON_FEEDBACK, T_STRING, 0, ident, length, SCNG(on_event_context));
	}

	ZVAL_STRINGL(zv, ident, length);
	return SUCCESS;
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

		while ((size_t)(pos1 - pos2) >= sizeof("__HALT_COMPILER();")-1) {
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

ZEND_API zend_result zend_multibyte_set_filter(const zend_encoding *onetime_encoding)
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

	return SUCCESS;
}

ZEND_API zend_result open_file_for_scanning(zend_file_handle *file_handle)
{
	char *buf;
	size_t size;
	zend_string *compiled_filename;

	if (zend_stream_fixup(file_handle, &buf, &size) == FAILURE) {
		/* Still add it to open_files to make destroy_file_handle work */
		zend_llist_add_element(&CG(open_files), file_handle);
		return FAILURE;
	}

	ZEND_ASSERT(!EG(exception) && "stream_fixup() should have failed");
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

	if (size != (size_t)-1) {
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
		SCNG(yy_start) = (unsigned char *)buf;
		yy_scan_buffer(buf, size);
	} else {
		zend_error_noreturn(E_COMPILE_ERROR, "zend_stream_mmap() failed");
	}

	if (CG(skip_shebang)) {
		BEGIN(SHEBANG);
	} else {
		BEGIN(INITIAL);
	}

	if (file_handle->opened_path) {
		compiled_filename = zend_string_copy(file_handle->opened_path);
	} else {
		compiled_filename = zend_string_init(file_handle->filename, strlen(file_handle->filename), 0);
	}

	zend_set_compiled_filename(compiled_filename);
	zend_string_release_ex(compiled_filename, 0);

	RESET_DOC_COMMENT();
	CG(zend_lineno) = 1;
	CG(increment_lineno) = 0;
	return SUCCESS;
}
END_EXTERN_C()

static zend_op_array *zend_compile(int type)
{
	zend_op_array *op_array = NULL;
	zend_bool original_in_compilation = CG(in_compilation);

	CG(in_compilation) = 1;
	CG(ast) = NULL;
	CG(ast_arena) = zend_arena_create(1024 * 32);

	if (!zendparse()) {
		int last_lineno = CG(zend_lineno);
		zend_file_context original_file_context;
		zend_oparray_context original_oparray_context;
		zend_op_array *original_active_op_array = CG(active_op_array);

		op_array = emalloc(sizeof(zend_op_array));
		init_op_array(op_array, type, INITIAL_OP_ARRAY_SIZE);
		CG(active_op_array) = op_array;

		/* Use heap to not waste arena memory */
		op_array->fn_flags |= ZEND_ACC_HEAP_RT_CACHE;

		if (zend_ast_process) {
			zend_ast_process(CG(ast));
		}

		zend_file_context_begin(&original_file_context);
		zend_oparray_context_begin(&original_oparray_context);
		zend_compile_top_stmt(CG(ast));
		CG(zend_lineno) = last_lineno;
		zend_emit_final_return(type == ZEND_USER_FUNCTION);
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

	return op_array;
}

ZEND_API zend_op_array *compile_file(zend_file_handle *file_handle, int type)
{
	zend_lex_state original_lex_state;
	zend_op_array *op_array = NULL;
	zend_save_lexical_state(&original_lex_state);

	if (open_file_for_scanning(file_handle)==FAILURE) {
		if (!EG(exception)) {
			if (type==ZEND_REQUIRE) {
				zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, file_handle->filename);
			} else {
				zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, file_handle->filename);
			}
		}
	} else {
		op_array = zend_compile(ZEND_USER_FUNCTION);
	}

	zend_restore_lexical_state(&original_lex_state);
	return op_array;
}

ZEND_API zend_ast *zend_compile_string_to_ast(
		zend_string *code, zend_arena **ast_arena, const char *filename) {
	zval code_zv;
	zend_bool original_in_compilation;
	zend_lex_state original_lex_state;
	zend_ast *ast;

	ZVAL_STR_COPY(&code_zv, code);

	original_in_compilation = CG(in_compilation);
	CG(in_compilation) = 1;

	zend_save_lexical_state(&original_lex_state);
	zend_prepare_string_for_scanning(&code_zv, filename);
	CG(ast) = NULL;
	CG(ast_arena) = zend_arena_create(1024 * 32);
	LANG_SCNG(yy_state) = yycINITIAL;

	if (zendparse() != 0) {
		zend_ast_destroy(CG(ast));
		zend_arena_destroy(CG(ast_arena));
		CG(ast) = NULL;
	}

	/* restore_lexical_state changes CG(ast) and CG(ast_arena) */
	ast = CG(ast);
	*ast_arena = CG(ast_arena);

	zend_restore_lexical_state(&original_lex_state);
	CG(in_compilation) = original_in_compilation;

	zval_ptr_dtor_str(&code_zv);

	return ast;
}

zend_op_array *compile_filename(int type, zval *filename)
{
	zend_file_handle file_handle;
	zval tmp;
	zend_op_array *retval;
	zend_string *opened_path = NULL;

	if (Z_TYPE_P(filename) != IS_STRING) {
		ZVAL_STR(&tmp, zval_get_string(filename));
		filename = &tmp;
	}
	zend_stream_init_filename(&file_handle, Z_STRVAL_P(filename));

	retval = zend_compile_file(&file_handle, type);
	if (retval && file_handle.handle.stream.handle) {
		if (!file_handle.opened_path) {
			file_handle.opened_path = opened_path = zend_string_copy(Z_STR_P(filename));
		}

		zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path);

		if (opened_path) {
			zend_string_release_ex(opened_path, 0);
		}
	}
	zend_destroy_file_handle(&file_handle);

	if (UNEXPECTED(filename == &tmp)) {
		zval_ptr_dtor(&tmp);
	}
	return retval;
}

ZEND_API void zend_prepare_string_for_scanning(zval *str, const char *filename)
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

	yy_scan_buffer(buf, size);

	new_compiled_filename = zend_string_init(filename, strlen(filename), 0);
	zend_set_compiled_filename(new_compiled_filename);
	zend_string_release_ex(new_compiled_filename, 0);
	CG(zend_lineno) = 1;
	CG(increment_lineno) = 0;
	RESET_DOC_COMMENT();
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

zend_op_array *compile_string(zend_string *source_string, const char *filename)
{
	zend_lex_state original_lex_state;
	zend_op_array *op_array = NULL;
	zval tmp;

	if (ZSTR_LEN(source_string) == 0) {
		return NULL;
	}

	ZVAL_STR_COPY(&tmp, source_string);

	zend_save_lexical_state(&original_lex_state);
	zend_prepare_string_for_scanning(&tmp, filename);
	BEGIN(ST_IN_SCRIPTING);
	op_array = zend_compile(ZEND_EVAL_CODE);

	zend_restore_lexical_state(&original_lex_state);
	zval_ptr_dtor(&tmp);

	return op_array;
}


BEGIN_EXTERN_C()
zend_result highlight_file(const char *filename, zend_syntax_highlighter_ini *syntax_highlighter_ini)
{
	zend_lex_state original_lex_state;
	zend_file_handle file_handle;

	zend_stream_init_filename(&file_handle, filename);
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

void highlight_string(zval *str, zend_syntax_highlighter_ini *syntax_highlighter_ini, const char *str_name)
{
	zend_lex_state original_lex_state;
	zval tmp;

	if (UNEXPECTED(Z_TYPE_P(str) != IS_STRING)) {
		ZVAL_STR(&tmp, zval_get_string_func(str));
		str = &tmp;
	}
	zend_save_lexical_state(&original_lex_state);
	zend_prepare_string_for_scanning(str, str_name);
	BEGIN(INITIAL);
	zend_highlight(syntax_highlighter_ini);
	if (SCNG(script_filtered)) {
		efree(SCNG(script_filtered));
		SCNG(script_filtered) = NULL;
	}
	zend_restore_lexical_state(&original_lex_state);
	if (UNEXPECTED(str == &tmp)) {
		zval_ptr_dtor(&tmp);
	}
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
	} else if (yyleng == 1) { \
		ZVAL_INTERNED_STR(zendlval, ZSTR_CHAR((zend_uchar)*(yytext))); \
	} else { \
		ZVAL_STRINGL(zendlval, yytext, yyleng); \
	}

static zend_result zend_scan_escape_string(zval *zendlval, char *str, int len, char quote_type)
{
	register char *s, *t;
	char *end;

	if (len <= 1) {
		if (len < 1) {
			ZVAL_EMPTY_STRING(zendlval);
		} else {
			zend_uchar c = (zend_uchar)*str;
			if (c == '\n' || c == '\r') {
				CG(zend_lineno)++;
			}
			ZVAL_INTERNED_STR(zendlval, ZSTR_CHAR(c));
		}
		goto skip_escape_conversion;
	}

	ZVAL_STRINGL(zendlval, str, len);

	/* convert escape sequences */
	s = Z_STRVAL_P(zendlval);
	end = s+Z_STRLEN_P(zendlval);
	while (1) {
		if (UNEXPECTED(*s=='\\')) {
			break;
		}
		if (*s == '\n' || (*s == '\r' && (*(s+1) != '\n'))) {
			CG(zend_lineno)++;
		}
		s++;
		if (s == end) {
			goto skip_escape_conversion;
		}
	}

	t = s;
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
					break;
				case 'r':
					*t++ = '\r';
					break;
				case 't':
					*t++ = '\t';
					break;
				case 'f':
					*t++ = '\f';
					break;
				case 'v':
					*t++ = '\v';
					break;
				case 'e':
#ifdef ZEND_WIN32
					*t++ = VK_ESCAPE;
#else
					*t++ = '\e';
#endif
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
					break;
				case 'x':
				case 'X':
					if (ZEND_IS_HEX(*(s+1))) {
						char hex_buf[3] = { 0, 0, 0 };

						hex_buf[0] = *(++s);
						if (ZEND_IS_HEX(*(s+1))) {
							hex_buf[1] = *(++s);
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
							*t++ = codepoint;
						} else if (codepoint <= 0x7FF) {
							*t++ = (codepoint >> 6) + 0xC0;
							*t++ = (codepoint & 0x3F) + 0x80;
						} else if (codepoint <= 0xFFFF) {
							*t++ = (codepoint >> 12) + 0xE0;
							*t++ = ((codepoint >> 6) & 0x3F) + 0x80;
							*t++ = (codepoint & 0x3F) + 0x80;
						} else if (codepoint <= 0x10FFFF) {
							*t++ = (codepoint >> 18) + 0xF0;
							*t++ = ((codepoint >> 12) & 0x3F) + 0x80;
							*t++ = ((codepoint >> 6) & 0x3F) + 0x80;
							*t++ = (codepoint & 0x3F) + 0x80;
						}
					}
					break;
				default:
					/* check for an octal */
					if (ZEND_IS_OCT(*s)) {
						char octal_buf[4] = { 0, 0, 0, 0 };

						octal_buf[0] = *s;
						if (ZEND_IS_OCT(*(s+1))) {
							octal_buf[1] = *(++s);
							if (ZEND_IS_OCT(*(s+1))) {
								octal_buf[2] = *(++s);
							}
						}
						if (octal_buf[2] && (octal_buf[0] > '3') && !SCNG(heredoc_scan_ahead)) {
							/* 3 octit values must not overflow 0xFF (\377) */
							zend_error(E_COMPILE_WARNING, "Octal escape sequence overflow \\%s is greater than \\377", octal_buf);
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
	Z_STRLEN_P(zendlval) = t - Z_STRVAL_P(zendlval);

skip_escape_conversion:
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

#define HEREDOC_USING_SPACES 1
#define HEREDOC_USING_TABS 2

static const char *next_newline(const char *str, const char *end, size_t *newline_len) {
	for (; str < end; str++) {
		if (*str == '\r') {
			*newline_len = str + 1 < end && *(str + 1) == '\n' ? 2 : 1;
			return str;
		} else if (*str == '\n') {
			*newline_len = 1;
			return str;
		}
	}
	*newline_len = 0;
	return NULL;
}

static zend_bool strip_multiline_string_indentation(
	zval *zendlval, int indentation, zend_bool using_spaces,
	zend_bool newline_at_start, zend_bool newline_at_end)
{
	const char *str = Z_STRVAL_P(zendlval), *end = str + Z_STRLEN_P(zendlval);
	char *copy = Z_STRVAL_P(zendlval);

	int newline_count = 0;
	size_t newline_len;
	const char *nl;

	if (!newline_at_start) {
		nl = next_newline(str, end, &newline_len);
		if (!nl) {
			return 1;
		}

		str = nl + newline_len;
		copy = (char *) nl + newline_len;
		newline_count++;
	} else {
		nl = str;
	}

	/* <= intentional */
	while (str <= end && nl) {
		size_t skip;
		nl = next_newline(str, end, &newline_len);
		if (!nl && newline_at_end) {
			nl = end;
		}

		/* Try to skip indentation */
		for (skip = 0; skip < indentation; skip++, str++) {
			if (str == nl) {
				/* Don't require full indentation on whitespace-only lines */
				break;
			}

			if (str == end || (*str != ' ' && *str != '\t')) {
				CG(zend_lineno) += newline_count;
				zend_throw_exception_ex(zend_ce_parse_error, 0,
					"Invalid body indentation level (expecting an indentation level of at least %d)", indentation);
				goto error;
			}

			if ((!using_spaces && *str == ' ') || (using_spaces && *str == '\t')) {
				CG(zend_lineno) += newline_count;
				zend_throw_exception(zend_ce_parse_error,
					"Invalid indentation - tabs and spaces cannot be mixed", 0);
				goto error;
			}
		}

		if (str == end) {
			break;
		}

		size_t len = nl ? (nl - str + newline_len) : (end - str);
		memmove(copy, str, len);
		str += len;
		copy += len;
		newline_count++;
	}

	*copy = '\0';
	Z_STRLEN_P(zendlval) = copy - Z_STRVAL_P(zendlval);
	return 1;

error:
	zval_ptr_dtor_str(zendlval);
	ZVAL_UNDEF(zendlval);

	return 0;
}

static void copy_heredoc_label_stack(void *void_heredoc_label)
{
	zend_heredoc_label *heredoc_label = void_heredoc_label;
	zend_heredoc_label *new_heredoc_label = emalloc(sizeof(zend_heredoc_label));

	*new_heredoc_label = *heredoc_label;
	new_heredoc_label->label = estrndup(heredoc_label->label, heredoc_label->length);

	zend_ptr_stack_push(&SCNG(heredoc_label_stack), (void *) new_heredoc_label);
}

/* Check that { }, [ ], ( ) are nested correctly */
static void report_bad_nesting(char opening, int opening_lineno, char closing)
{
	char   buf[256];
	size_t used = 0;

	used = snprintf(buf, sizeof(buf), "Unclosed '%c'", opening);

	if (opening_lineno != CG(zend_lineno)) {
		used += snprintf(buf + used, sizeof(buf) - used, " on line %d", opening_lineno);
	}

	if (closing) { 	/* 'closing' will be 0 if at end of file */
		used += snprintf(buf + used, sizeof(buf) - used, " does not match '%c'", closing);
	}

	zend_throw_exception(zend_ce_parse_error, buf, 0);
}

static void enter_nesting(char opening)
{
	zend_nest_location nest_loc = {opening, CG(zend_lineno)};
	zend_stack_push(&SCNG(nest_location_stack), &nest_loc);
}

static zend_result exit_nesting(char closing)
{
	if (zend_stack_is_empty(&SCNG(nest_location_stack))) {
		zend_throw_exception_ex(zend_ce_parse_error, 0, "Unmatched '%c'", closing);
		return FAILURE;
	}

	zend_nest_location *nest_loc = zend_stack_top(&SCNG(nest_location_stack));
	char opening = nest_loc->text;

	if ((opening == '{' && closing != '}') ||
	    (opening == '[' && closing != ']') ||
	    (opening == '(' && closing != ')')) {
		report_bad_nesting(opening, nest_loc->lineno, closing);
		return FAILURE;
	}

	zend_stack_del_top(&SCNG(nest_location_stack));
	return SUCCESS;
}

static zend_result check_nesting_at_end()
{
	if (!zend_stack_is_empty(&SCNG(nest_location_stack))) {
		zend_nest_location *nest_loc = zend_stack_top(&SCNG(nest_location_stack));
		report_bad_nesting(nest_loc->text, nest_loc->lineno, 0);
		return FAILURE;
	}

	return SUCCESS;
}

#define PARSER_MODE() \
	EXPECTED(elem != NULL)

#define RETURN_TOKEN(_token) do { \
		token = _token; \
		goto emit_token; \
	} while (0)

#define RETURN_TOKEN_WITH_VAL(_token) do { \
		token = _token; \
		goto emit_token_with_val; \
	} while (0)

#define RETURN_TOKEN_WITH_STR(_token, _offset) do { \
		token = _token; \
		offset = _offset; \
		goto emit_token_with_str; \
	} while (0)

#define RETURN_TOKEN_WITH_IDENT(_token) do { \
		token = _token; \
		goto emit_token_with_ident; \
	} while (0)

#define RETURN_OR_SKIP_TOKEN(_token) do { \
		token = _token; \
		if (PARSER_MODE()) { \
			goto skip_token; \
		} \
		goto emit_token; \
	} while (0)

#define RETURN_EXIT_NESTING_TOKEN(_token) do { \
		if (exit_nesting(_token) && PARSER_MODE()) { \
			RETURN_TOKEN(T_ERROR); \
		} else { \
			RETURN_TOKEN(_token); \
		} \
	} while(0)

#define RETURN_END_TOKEN do { \
		if (check_nesting_at_end() && PARSER_MODE()) { \
			RETURN_TOKEN(T_ERROR); \
		} else { \
			RETURN_TOKEN(END); \
		} \
	} while (0)

int ZEND_FASTCALL lex_scan(zval *zendlval, zend_parser_stack_elem *elem)
{
int token;
int offset;
int start_line = CG(zend_lineno);

	ZVAL_UNDEF(zendlval);
restart:
	SCNG(yy_text) = YYCURSOR;

/*!re2c
re2c:yyfill:check = 0;
LNUM	[0-9]+(_[0-9]+)*
DNUM	({LNUM}?"."{LNUM})|({LNUM}"."{LNUM}?)
EXPONENT_DNUM	(({LNUM}|{DNUM})[eE][+-]?{LNUM})
HNUM	"0x"[0-9a-fA-F]+(_[0-9a-fA-F]+)*
BNUM	"0b"[01]+(_[01]+)*
LABEL	[a-zA-Z_\x80-\xff][a-zA-Z0-9_\x80-\xff]*
WHITESPACE [ \n\r\t]+
TABS_AND_SPACES [ \t]*
TOKENS [;:,.|^&+-/*=%!~$<>?@]
ANY_CHAR [^]
NEWLINE ("\r"|"\n"|"\r\n")

/* compute yyleng before each rule */
<!*> := yyleng = YYCURSOR - SCNG(yy_text);

<ST_IN_SCRIPTING>"exit" {
	RETURN_TOKEN_WITH_IDENT(T_EXIT);
}

<ST_IN_SCRIPTING>"die" {
	RETURN_TOKEN_WITH_IDENT(T_EXIT);
}

<ST_IN_SCRIPTING>"fn" {
	RETURN_TOKEN_WITH_IDENT(T_FN);
}

<ST_IN_SCRIPTING>"function" {
	RETURN_TOKEN_WITH_IDENT(T_FUNCTION);
}

<ST_IN_SCRIPTING>"const" {
	RETURN_TOKEN_WITH_IDENT(T_CONST);
}

<ST_IN_SCRIPTING>"return" {
	RETURN_TOKEN_WITH_IDENT(T_RETURN);
}

<ST_IN_SCRIPTING>"#[" {
	enter_nesting('[');
	RETURN_TOKEN(T_ATTRIBUTE);
}

<ST_IN_SCRIPTING>"yield"{WHITESPACE}"from"[^a-zA-Z0-9_\x80-\xff] {
	yyless(yyleng - 1);
	HANDLE_NEWLINES(yytext, yyleng);
	RETURN_TOKEN_WITH_IDENT(T_YIELD_FROM);
}

<ST_IN_SCRIPTING>"yield" {
	RETURN_TOKEN_WITH_IDENT(T_YIELD);
}

<ST_IN_SCRIPTING>"try" {
	RETURN_TOKEN_WITH_IDENT(T_TRY);
}

<ST_IN_SCRIPTING>"catch" {
	RETURN_TOKEN_WITH_IDENT(T_CATCH);
}

<ST_IN_SCRIPTING>"finally" {
	RETURN_TOKEN_WITH_IDENT(T_FINALLY);
}

<ST_IN_SCRIPTING>"throw" {
	RETURN_TOKEN_WITH_IDENT(T_THROW);
}

<ST_IN_SCRIPTING>"if" {
	RETURN_TOKEN_WITH_IDENT(T_IF);
}

<ST_IN_SCRIPTING>"elseif" {
	RETURN_TOKEN_WITH_IDENT(T_ELSEIF);
}

<ST_IN_SCRIPTING>"endif" {
	RETURN_TOKEN_WITH_IDENT(T_ENDIF);
}

<ST_IN_SCRIPTING>"else" {
	RETURN_TOKEN_WITH_IDENT(T_ELSE);
}

<ST_IN_SCRIPTING>"while" {
	RETURN_TOKEN_WITH_IDENT(T_WHILE);
}

<ST_IN_SCRIPTING>"endwhile" {
	RETURN_TOKEN_WITH_IDENT(T_ENDWHILE);
}

<ST_IN_SCRIPTING>"do" {
	RETURN_TOKEN_WITH_IDENT(T_DO);
}

<ST_IN_SCRIPTING>"for" {
	RETURN_TOKEN_WITH_IDENT(T_FOR);
}

<ST_IN_SCRIPTING>"endfor" {
	RETURN_TOKEN_WITH_IDENT(T_ENDFOR);
}

<ST_IN_SCRIPTING>"foreach" {
	RETURN_TOKEN_WITH_IDENT(T_FOREACH);
}

<ST_IN_SCRIPTING>"endforeach" {
	RETURN_TOKEN_WITH_IDENT(T_ENDFOREACH);
}

<ST_IN_SCRIPTING>"declare" {
	RETURN_TOKEN_WITH_IDENT(T_DECLARE);
}

<ST_IN_SCRIPTING>"enddeclare" {
	RETURN_TOKEN_WITH_IDENT(T_ENDDECLARE);
}

<ST_IN_SCRIPTING>"instanceof" {
	RETURN_TOKEN_WITH_IDENT(T_INSTANCEOF);
}

<ST_IN_SCRIPTING>"as" {
	RETURN_TOKEN_WITH_IDENT(T_AS);
}

<ST_IN_SCRIPTING>"switch" {
	RETURN_TOKEN_WITH_IDENT(T_SWITCH);
}

<ST_IN_SCRIPTING>"match" {
	RETURN_TOKEN_WITH_IDENT(T_MATCH);
}

<ST_IN_SCRIPTING>"endswitch" {
	RETURN_TOKEN_WITH_IDENT(T_ENDSWITCH);
}

<ST_IN_SCRIPTING>"case" {
	RETURN_TOKEN_WITH_IDENT(T_CASE);
}

<ST_IN_SCRIPTING>"default" {
	RETURN_TOKEN_WITH_IDENT(T_DEFAULT);
}

<ST_IN_SCRIPTING>"break" {
	RETURN_TOKEN_WITH_IDENT(T_BREAK);
}

<ST_IN_SCRIPTING>"continue" {
	RETURN_TOKEN_WITH_IDENT(T_CONTINUE);
}

<ST_IN_SCRIPTING>"goto" {
	RETURN_TOKEN_WITH_IDENT(T_GOTO);
}

<ST_IN_SCRIPTING>"echo" {
	RETURN_TOKEN_WITH_IDENT(T_ECHO);
}

<ST_IN_SCRIPTING>"print" {
	RETURN_TOKEN_WITH_IDENT(T_PRINT);
}

<ST_IN_SCRIPTING>"class" {
	RETURN_TOKEN_WITH_IDENT(T_CLASS);
}

<ST_IN_SCRIPTING>"interface" {
	RETURN_TOKEN_WITH_IDENT(T_INTERFACE);
}

<ST_IN_SCRIPTING>"trait" {
	RETURN_TOKEN_WITH_IDENT(T_TRAIT);
}

<ST_IN_SCRIPTING>"extends" {
	RETURN_TOKEN_WITH_IDENT(T_EXTENDS);
}

<ST_IN_SCRIPTING>"implements" {
	RETURN_TOKEN_WITH_IDENT(T_IMPLEMENTS);
}

<ST_IN_SCRIPTING>"->" {
	yy_push_state(ST_LOOKING_FOR_PROPERTY);
	RETURN_TOKEN(T_OBJECT_OPERATOR);
}

<ST_IN_SCRIPTING>"?->" {
	yy_push_state(ST_LOOKING_FOR_PROPERTY);
	RETURN_TOKEN(T_NULLSAFE_OBJECT_OPERATOR);
}

<ST_IN_SCRIPTING,ST_LOOKING_FOR_PROPERTY>{WHITESPACE}+ {
	goto return_whitespace;
}

<ST_LOOKING_FOR_PROPERTY>"->" {
	RETURN_TOKEN(T_OBJECT_OPERATOR);
}

<ST_LOOKING_FOR_PROPERTY>"?->" {
	RETURN_TOKEN(T_NULLSAFE_OBJECT_OPERATOR);
}

<ST_LOOKING_FOR_PROPERTY>{LABEL} {
	yy_pop_state();
	RETURN_TOKEN_WITH_STR(T_STRING, 0);
}

<ST_LOOKING_FOR_PROPERTY>{ANY_CHAR} {
	yyless(0);
	yy_pop_state();
	goto restart;
}

<ST_IN_SCRIPTING>"::" {
	RETURN_TOKEN(T_PAAMAYIM_NEKUDOTAYIM);
}

<ST_IN_SCRIPTING>"..." {
	RETURN_TOKEN(T_ELLIPSIS);
}

<ST_IN_SCRIPTING>"??" {
	RETURN_TOKEN(T_COALESCE);
}

<ST_IN_SCRIPTING>"new" {
	RETURN_TOKEN_WITH_IDENT(T_NEW);
}

<ST_IN_SCRIPTING>"clone" {
	RETURN_TOKEN_WITH_IDENT(T_CLONE);
}

<ST_IN_SCRIPTING>"var" {
	RETURN_TOKEN_WITH_IDENT(T_VAR);
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("int"|"integer"){TABS_AND_SPACES}")" {
	RETURN_TOKEN(T_INT_CAST);
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("double"|"float"){TABS_AND_SPACES}")" {
	RETURN_TOKEN(T_DOUBLE_CAST);
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}"real"{TABS_AND_SPACES}")" {
	if (PARSER_MODE()) {
		zend_throw_exception(zend_ce_parse_error, "The (real) cast has been removed, use (float) instead", 0);
		RETURN_TOKEN(T_ERROR);
	}
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
	RETURN_TOKEN_WITH_IDENT(T_EVAL);
}

<ST_IN_SCRIPTING>"include" {
	RETURN_TOKEN_WITH_IDENT(T_INCLUDE);
}

<ST_IN_SCRIPTING>"include_once" {
	RETURN_TOKEN_WITH_IDENT(T_INCLUDE_ONCE);
}

<ST_IN_SCRIPTING>"require" {
	RETURN_TOKEN_WITH_IDENT(T_REQUIRE);
}

<ST_IN_SCRIPTING>"require_once" {
	RETURN_TOKEN_WITH_IDENT(T_REQUIRE_ONCE);
}

<ST_IN_SCRIPTING>"namespace" {
	RETURN_TOKEN_WITH_IDENT(T_NAMESPACE);
}

<ST_IN_SCRIPTING>"use" {
	RETURN_TOKEN_WITH_IDENT(T_USE);
}

<ST_IN_SCRIPTING>"insteadof" {
    RETURN_TOKEN_WITH_IDENT(T_INSTEADOF);
}

<ST_IN_SCRIPTING>"global" {
	RETURN_TOKEN_WITH_IDENT(T_GLOBAL);
}

<ST_IN_SCRIPTING>"isset" {
	RETURN_TOKEN_WITH_IDENT(T_ISSET);
}

<ST_IN_SCRIPTING>"empty" {
	RETURN_TOKEN_WITH_IDENT(T_EMPTY);
}

<ST_IN_SCRIPTING>"__halt_compiler" {
	RETURN_TOKEN_WITH_IDENT(T_HALT_COMPILER);
}

<ST_IN_SCRIPTING>"static" {
	RETURN_TOKEN_WITH_IDENT(T_STATIC);
}

<ST_IN_SCRIPTING>"abstract" {
	RETURN_TOKEN_WITH_IDENT(T_ABSTRACT);
}

<ST_IN_SCRIPTING>"final" {
	RETURN_TOKEN_WITH_IDENT(T_FINAL);
}

<ST_IN_SCRIPTING>"private" {
	RETURN_TOKEN_WITH_IDENT(T_PRIVATE);
}

<ST_IN_SCRIPTING>"protected" {
	RETURN_TOKEN_WITH_IDENT(T_PROTECTED);
}

<ST_IN_SCRIPTING>"public" {
	RETURN_TOKEN_WITH_IDENT(T_PUBLIC);
}

<ST_IN_SCRIPTING>"unset" {
	RETURN_TOKEN_WITH_IDENT(T_UNSET);
}

<ST_IN_SCRIPTING>"=>" {
	RETURN_TOKEN(T_DOUBLE_ARROW);
}

<ST_IN_SCRIPTING>"list" {
	RETURN_TOKEN_WITH_IDENT(T_LIST);
}

<ST_IN_SCRIPTING>"array" {
	RETURN_TOKEN_WITH_IDENT(T_ARRAY);
}

<ST_IN_SCRIPTING>"callable" {
	RETURN_TOKEN_WITH_IDENT(T_CALLABLE);
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

<ST_IN_SCRIPTING>"??=" {
	RETURN_TOKEN(T_COALESCE_EQUAL);
}

<ST_IN_SCRIPTING>"||" {
	RETURN_TOKEN(T_BOOLEAN_OR);
}

<ST_IN_SCRIPTING>"&&" {
	RETURN_TOKEN(T_BOOLEAN_AND);
}

<ST_IN_SCRIPTING>"OR" {
	RETURN_TOKEN_WITH_IDENT(T_LOGICAL_OR);
}

<ST_IN_SCRIPTING>"AND" {
	RETURN_TOKEN_WITH_IDENT(T_LOGICAL_AND);
}

<ST_IN_SCRIPTING>"XOR" {
	RETURN_TOKEN_WITH_IDENT(T_LOGICAL_XOR);
}

<ST_IN_SCRIPTING>"<<" {
	RETURN_TOKEN(T_SL);
}

<ST_IN_SCRIPTING>">>" {
	RETURN_TOKEN(T_SR);
}

<ST_IN_SCRIPTING>"]"|")" {
	/* Check that ] and ) match up properly with a preceding [ or ( */
	RETURN_EXIT_NESTING_TOKEN(yytext[0]);
}

<ST_IN_SCRIPTING>"["|"(" {
	enter_nesting(yytext[0]);
	RETURN_TOKEN(yytext[0]);
}

<ST_IN_SCRIPTING>{TOKENS} {
	RETURN_TOKEN(yytext[0]);
}


<ST_IN_SCRIPTING>"{" {
	yy_push_state(ST_IN_SCRIPTING);
	enter_nesting('{');
	RETURN_TOKEN('{');
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"${" {
	yy_push_state(ST_LOOKING_FOR_VARNAME);
	enter_nesting('{');
	RETURN_TOKEN(T_DOLLAR_OPEN_CURLY_BRACES);
}

<ST_IN_SCRIPTING>"}" {
	RESET_DOC_COMMENT();
	if (!zend_stack_is_empty(&SCNG(state_stack))) {
		yy_pop_state();
	}
	RETURN_EXIT_NESTING_TOKEN('}');
}


<ST_LOOKING_FOR_VARNAME>{LABEL}[[}] {
	yyless(yyleng - 1);
	yy_pop_state();
	yy_push_state(ST_IN_SCRIPTING);
	RETURN_TOKEN_WITH_STR(T_STRING_VARNAME, 0);
}


<ST_LOOKING_FOR_VARNAME>{ANY_CHAR} {
	yyless(0);
	yy_pop_state();
	yy_push_state(ST_IN_SCRIPTING);
	goto restart;
}

<ST_IN_SCRIPTING>{BNUM} {
	/* The +/- 2 skips "0b" */
	size_t len = yyleng - 2;
	char *end, *bin = yytext + 2;
	zend_bool contains_underscores;

	/* Skip any leading 0s */
	while (len > 0 && (*bin == '0' || *bin == '_')) {
		++bin;
		--len;
	}

	contains_underscores = (memchr(bin, '_', len) != NULL);

	if (contains_underscores) {
		bin = estrndup(bin, len);
		strip_underscores(bin, &len);
	}

	if (len < SIZEOF_ZEND_LONG * 8) {
		if (len == 0) {
			ZVAL_LONG(zendlval, 0);
		} else {
			errno = 0;
			ZVAL_LONG(zendlval, ZEND_STRTOL(bin, &end, 2));
			ZEND_ASSERT(!errno && end == bin + len);
		}
		if (contains_underscores) {
			efree(bin);
		}
		RETURN_TOKEN_WITH_VAL(T_LNUMBER);
	} else {
		ZVAL_DOUBLE(zendlval, zend_bin_strtod(bin, (const char **)&end));
		/* errno isn't checked since we allow HUGE_VAL/INF overflow */
		ZEND_ASSERT(end == bin + len);
		if (contains_underscores) {
			efree(bin);
		}
		RETURN_TOKEN_WITH_VAL(T_DNUMBER);
	}
}

<ST_IN_SCRIPTING>{LNUM} {
	size_t len = yyleng;
	char *end, *lnum = yytext;
	zend_bool is_octal = lnum[0] == '0';
	zend_bool contains_underscores = (memchr(lnum, '_', len) != NULL);

	if (contains_underscores) {
		lnum = estrndup(lnum, len);
		strip_underscores(lnum, &len);
	}

	/* Digits 8 and 9 are illegal in octal literals. */
	if (is_octal) {
		size_t i;
		for (i = 0; i < len; i++) {
			if (lnum[i] == '8' || lnum[i] == '9') {
				zend_throw_exception(zend_ce_parse_error, "Invalid numeric literal", 0);
				if (PARSER_MODE()) {
					if (contains_underscores) {
						efree(lnum);
					}
					ZVAL_UNDEF(zendlval);
					RETURN_TOKEN(T_ERROR);
				}

				/* Continue in order to determine if this is T_LNUMBER or T_DNUMBER. */
				len = i;
				break;
			}
		}
	}


	if (len < MAX_LENGTH_OF_LONG - 1) { /* Won't overflow */
		errno = 0;
		/* base must be passed explicitly for correct parse error on Windows */
		ZVAL_LONG(zendlval, ZEND_STRTOL(lnum, &end, is_octal ? 8 : 10));
		ZEND_ASSERT(end == lnum + len);
	} else {
		errno = 0;
		ZVAL_LONG(zendlval, ZEND_STRTOL(lnum, &end, 0));
		if (errno == ERANGE) { /* Overflow */
			errno = 0;
			if (is_octal) { /* octal overflow */
				ZVAL_DOUBLE(zendlval, zend_oct_strtod(lnum, (const char **)&end));
			} else {
				ZVAL_DOUBLE(zendlval, zend_strtod(lnum, (const char **)&end));
			}
			ZEND_ASSERT(end == lnum + len);
			if (contains_underscores) {
				efree(lnum);
			}
			RETURN_TOKEN_WITH_VAL(T_DNUMBER);
		}
		ZEND_ASSERT(end == lnum + len);
	}
	ZEND_ASSERT(!errno);
	if (contains_underscores) {
		efree(lnum);
	}
	RETURN_TOKEN_WITH_VAL(T_LNUMBER);
}

<ST_IN_SCRIPTING>{HNUM} {
	/* The +/- 2 skips "0x" */
	size_t len = yyleng - 2;
	char *end, *hex = yytext + 2;
	zend_bool contains_underscores;

	/* Skip any leading 0s */
	while (len > 0 && (*hex == '0' || *hex == '_')) {
		++hex;
		--len;
	}

	contains_underscores = (memchr(hex, '_', len) != NULL);

	if (contains_underscores) {
		hex = estrndup(hex, len);
		strip_underscores(hex, &len);
	}

	if (len < SIZEOF_ZEND_LONG * 2 || (len == SIZEOF_ZEND_LONG * 2 && *hex <= '7')) {
		if (len == 0) {
			ZVAL_LONG(zendlval, 0);
		} else {
			errno = 0;
			ZVAL_LONG(zendlval, ZEND_STRTOL(hex, &end, 16));
			ZEND_ASSERT(!errno && end == hex + len);
		}
		if (contains_underscores) {
			efree(hex);
		}
		RETURN_TOKEN_WITH_VAL(T_LNUMBER);
	} else {
		ZVAL_DOUBLE(zendlval, zend_hex_strtod(hex, (const char **)&end));
		/* errno isn't checked since we allow HUGE_VAL/INF overflow */
		ZEND_ASSERT(end == hex + len);
		if (contains_underscores) {
			efree(hex);
		}
		RETURN_TOKEN_WITH_VAL(T_DNUMBER);
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
	RETURN_TOKEN_WITH_VAL(T_NUM_STRING);
}

<ST_VAR_OFFSET>{LNUM}|{HNUM}|{BNUM} { /* Offset must be treated as a string */
	if (yyleng == 1) {
		ZVAL_INTERNED_STR(zendlval, ZSTR_CHAR((zend_uchar)*(yytext)));
	} else {
		ZVAL_STRINGL(zendlval, yytext, yyleng);
	}
	RETURN_TOKEN_WITH_VAL(T_NUM_STRING);
}

<ST_IN_SCRIPTING>{DNUM}|{EXPONENT_DNUM} {
	const char *end;
	size_t len = yyleng;
	char *dnum = yytext;
	zend_bool contains_underscores = (memchr(dnum, '_', len) != NULL);

	if (contains_underscores) {
		dnum = estrndup(dnum, len);
		strip_underscores(dnum, &len);
	}

	ZVAL_DOUBLE(zendlval, zend_strtod(dnum, &end));
	/* errno isn't checked since we allow HUGE_VAL/INF overflow */
	ZEND_ASSERT(end == dnum + len);
	if (contains_underscores) {
		efree(dnum);
	}
	RETURN_TOKEN_WITH_VAL(T_DNUMBER);
}

<ST_IN_SCRIPTING>"__CLASS__" {
	RETURN_TOKEN_WITH_IDENT(T_CLASS_C);
}

<ST_IN_SCRIPTING>"__TRAIT__" {
	RETURN_TOKEN_WITH_IDENT(T_TRAIT_C);
}

<ST_IN_SCRIPTING>"__FUNCTION__" {
	RETURN_TOKEN_WITH_IDENT(T_FUNC_C);
}

<ST_IN_SCRIPTING>"__METHOD__" {
	RETURN_TOKEN_WITH_IDENT(T_METHOD_C);
}

<ST_IN_SCRIPTING>"__LINE__" {
	RETURN_TOKEN_WITH_IDENT(T_LINE);
}

<ST_IN_SCRIPTING>"__FILE__" {
	RETURN_TOKEN_WITH_IDENT(T_FILE);
}

<ST_IN_SCRIPTING>"__DIR__" {
	RETURN_TOKEN_WITH_IDENT(T_DIR);
}

<ST_IN_SCRIPTING>"__NAMESPACE__" {
	RETURN_TOKEN_WITH_IDENT(T_NS_C);
}

<SHEBANG>"#!" .* {NEWLINE} {
	CG(zend_lineno)++;
	BEGIN(INITIAL);
	goto restart;
}

<SHEBANG>{ANY_CHAR} {
	yyless(0);
	BEGIN(INITIAL);
	goto restart;
}

<INITIAL>"<?=" {
	BEGIN(ST_IN_SCRIPTING);
	if (PARSER_MODE()) {
		/* We'll reject this as an identifier in zend_lex_tstring. */
		RETURN_TOKEN_WITH_IDENT(T_ECHO);
	}
	RETURN_TOKEN(T_OPEN_TAG_WITH_ECHO);
}


<INITIAL>"<?php"([ \t]|{NEWLINE}) {
	HANDLE_NEWLINE(yytext[yyleng-1]);
	BEGIN(ST_IN_SCRIPTING);
	RETURN_OR_SKIP_TOKEN(T_OPEN_TAG);
}

<INITIAL>"<?php" {
	/* Allow <?php followed by end of file. */
	if (YYCURSOR == YYLIMIT) {
		BEGIN(ST_IN_SCRIPTING);
		RETURN_OR_SKIP_TOKEN(T_OPEN_TAG);
	}
	/* Degenerate case: <?phpX is interpreted as <? phpX with short tags. */
	if (CG(short_tags)) {
		yyless(2);
		BEGIN(ST_IN_SCRIPTING);
		RETURN_OR_SKIP_TOKEN(T_OPEN_TAG);
	}
	goto inline_char_handler;
}

<INITIAL>"<?" {
	if (CG(short_tags)) {
		BEGIN(ST_IN_SCRIPTING);
		RETURN_OR_SKIP_TOKEN(T_OPEN_TAG);
	} else {
		goto inline_char_handler;
	}
}

<INITIAL>{ANY_CHAR} {
	if (YYCURSOR > YYLIMIT) {
		RETURN_END_TOKEN;
	}

inline_char_handler:

	while (1) {
		YYCTYPE *ptr = memchr(YYCURSOR, '<', YYLIMIT - YYCURSOR);

		YYCURSOR = ptr ? ptr + 1 : YYLIMIT;

		if (YYCURSOR >= YYLIMIT) {
			break;
		}

		if (*YYCURSOR == '?') {
			if (CG(short_tags) /* <? */
				|| (*(YYCURSOR + 1) == '=') /* <?= */
				|| (!strncasecmp((char*)YYCURSOR + 1, "php", 3) && /* <?php[ \t\r\n] */
					(YYCURSOR + 4 == YYLIMIT ||
					YYCURSOR[4] == ' ' || YYCURSOR[4] == '\t' ||
					YYCURSOR[4] == '\n' || YYCURSOR[4] == '\r'))
			) {
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
	} else if (yyleng == 1) {
		ZVAL_INTERNED_STR(zendlval, ZSTR_CHAR((zend_uchar)*yytext));
	} else {
		ZVAL_STRINGL(zendlval, yytext, yyleng);
	}
	HANDLE_NEWLINES(yytext, yyleng);
	RETURN_TOKEN_WITH_VAL(T_INLINE_HTML);
}


/* Make sure a label character follows "->" or "?->", otherwise there is no property
 * and "->"/"?->" will be taken literally
 */
<ST_DOUBLE_QUOTES,ST_HEREDOC,ST_BACKQUOTE>"$"{LABEL}"->"[a-zA-Z_\x80-\xff] {
	yyless(yyleng - 3);
	yy_push_state(ST_LOOKING_FOR_PROPERTY);
	RETURN_TOKEN_WITH_STR(T_VARIABLE, 1);
}

<ST_DOUBLE_QUOTES,ST_HEREDOC,ST_BACKQUOTE>"$"{LABEL}"?->"[a-zA-Z_\x80-\xff] {
	yyless(yyleng - 4);
	yy_push_state(ST_LOOKING_FOR_PROPERTY);
	RETURN_TOKEN_WITH_STR(T_VARIABLE, 1);
}

/* A [ always designates a variable offset, regardless of what follows
 */
<ST_DOUBLE_QUOTES,ST_HEREDOC,ST_BACKQUOTE>"$"{LABEL}"[" {
	yyless(yyleng - 1);
	yy_push_state(ST_VAR_OFFSET);
	RETURN_TOKEN_WITH_STR(T_VARIABLE, 1);
}

<ST_IN_SCRIPTING,ST_DOUBLE_QUOTES,ST_HEREDOC,ST_BACKQUOTE,ST_VAR_OFFSET>"$"{LABEL} {
	RETURN_TOKEN_WITH_STR(T_VARIABLE, 1);
}

<ST_VAR_OFFSET>"]" {
	yy_pop_state();
	RETURN_TOKEN(']');
}

<ST_VAR_OFFSET>{TOKENS}|[[(){}"`] {
	/* Only '[' or '-' can be valid, but returning other tokens will allow a more explicit parse error */
	RETURN_TOKEN(yytext[0]);
}

<ST_VAR_OFFSET>[ \n\r\t\\'#] {
	/* Invalid rule to return a more explicit parse error with proper line number */
	yyless(0);
	yy_pop_state();
	ZVAL_NULL(zendlval);
	RETURN_TOKEN_WITH_VAL(T_ENCAPSED_AND_WHITESPACE);
}

<ST_IN_SCRIPTING>"namespace"("\\"{LABEL})+ {
	RETURN_TOKEN_WITH_STR(T_NAME_RELATIVE, sizeof("namespace\\") - 1);
}

<ST_IN_SCRIPTING>{LABEL}("\\"{LABEL})+ {
	RETURN_TOKEN_WITH_STR(T_NAME_QUALIFIED, 0);
}

<ST_IN_SCRIPTING>"\\"{LABEL}("\\"{LABEL})* {
	RETURN_TOKEN_WITH_STR(T_NAME_FULLY_QUALIFIED, 1);
}

<ST_IN_SCRIPTING>"\\" {
	RETURN_TOKEN(T_NS_SEPARATOR);
}

<ST_IN_SCRIPTING,ST_VAR_OFFSET>{LABEL} {
	RETURN_TOKEN_WITH_STR(T_STRING, 0);
}


<ST_IN_SCRIPTING>"#"|"//" {
	while (YYCURSOR < YYLIMIT) {
		switch (*YYCURSOR++) {
			case '\r':
			case '\n':
				YYCURSOR--;
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
	RETURN_OR_SKIP_TOKEN(T_COMMENT);
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
		zend_throw_exception_ex(zend_ce_parse_error, 0, "Unterminated comment starting line %d", CG(zend_lineno));
		if (PARSER_MODE()) {
			RETURN_TOKEN(T_ERROR);
		}
	}

	yyleng = YYCURSOR - SCNG(yy_text);
	HANDLE_NEWLINES(yytext, yyleng);

	if (doc_com) {
		CG(doc_comment) = zend_string_init(yytext, yyleng, 0);
		RETURN_OR_SKIP_TOKEN(T_DOC_COMMENT);
	}

	RETURN_OR_SKIP_TOKEN(T_COMMENT);
}

<ST_IN_SCRIPTING>"?>"{NEWLINE}? {
	BEGIN(INITIAL);
	if (yytext[yyleng-1] != '>') {
		CG(increment_lineno) = 1;
	}
	if (PARSER_MODE()) {
		RETURN_TOKEN(';');  /* implicit ';' at php-end tag */
	}
	RETURN_TOKEN(T_CLOSE_TAG);
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
			RETURN_TOKEN_WITH_VAL(T_ENCAPSED_AND_WHITESPACE);
		}
	}

	if (yyleng-bprefix-2 <= 1) {
		if (yyleng-bprefix-2 < 1) {
			ZVAL_EMPTY_STRING(zendlval);
		} else {
			zend_uchar c = (zend_uchar)*(yytext+bprefix+1);
			if (c == '\n' || c == '\r') {
				CG(zend_lineno)++;
			}
			ZVAL_INTERNED_STR(zendlval, ZSTR_CHAR(c));
		}
		goto skip_escape_conversion;
	}
	ZVAL_STRINGL(zendlval, yytext+bprefix+1, yyleng-bprefix-2);

	/* convert escape sequences */
	s = Z_STRVAL_P(zendlval);
	end = s+Z_STRLEN_P(zendlval);
	while (1) {
		if (UNEXPECTED(*s=='\\')) {
			break;
		}
		if (*s == '\n' || (*s == '\r' && (*(s+1) != '\n'))) {
			CG(zend_lineno)++;
		}
		s++;
		if (s == end) {
			goto skip_escape_conversion;
		}
	}

	t = s;
	while (s<end) {
		if (*s=='\\') {
			s++;
			if (*s == '\\' || *s == '\'') {
				*t++ = *s;
			} else {
				*t++ = '\\';
				*t++ = *s;
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
	Z_STRLEN_P(zendlval) = t - Z_STRVAL_P(zendlval);

skip_escape_conversion:
	if (SCNG(output_filter)) {
		size_t sz = 0;
		char *str = NULL;
		zend_string *new_str;
		s = Z_STRVAL_P(zendlval);
		// TODO: avoid reallocation ???
		SCNG(output_filter)((unsigned char **)&str, &sz, (unsigned char *)s, (size_t)Z_STRLEN_P(zendlval));
		new_str = zend_string_init(str, sz, 0);
		if (str != s) {
			efree(str);
		}
		zend_string_release_ex(Z_STR_P(zendlval), 0);
		ZVAL_STR(zendlval, new_str);
	}
	RETURN_TOKEN_WITH_VAL(T_CONSTANT_ENCAPSED_STRING);
}


<ST_IN_SCRIPTING>b?["] {
	int bprefix = (yytext[0] != '"') ? 1 : 0;

	while (YYCURSOR < YYLIMIT) {
		switch (*YYCURSOR++) {
			case '"':
				yyleng = YYCURSOR - SCNG(yy_text);
				if (EXPECTED(zend_scan_escape_string(zendlval, yytext+bprefix+1, yyleng-bprefix-2, '"') == SUCCESS)
				 || !PARSER_MODE()) {
					RETURN_TOKEN_WITH_VAL(T_CONSTANT_ENCAPSED_STRING);
				} else {
					RETURN_TOKEN(T_ERROR);
				}
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
	unsigned char *saved_cursor;
	int bprefix = (yytext[0] != '<') ? 1 : 0, spacing = 0, indentation = 0;
	zend_heredoc_label *heredoc_label = emalloc(sizeof(zend_heredoc_label));
	zend_bool is_heredoc = 1;

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
		is_heredoc = 0;

		BEGIN(ST_NOWDOC);
	} else {
		if (*s == '"') {
			s++;
			heredoc_label->length -= 2;
		}

		BEGIN(ST_HEREDOC);
	}

	heredoc_label->label = estrndup(s, heredoc_label->length);
	heredoc_label->indentation_uses_spaces = 0;
	heredoc_label->indentation = 0;
	saved_cursor = YYCURSOR;

	zend_ptr_stack_push(&SCNG(heredoc_label_stack), (void *) heredoc_label);

	while (YYCURSOR < YYLIMIT && (*YYCURSOR == ' ' || *YYCURSOR == '\t')) {
		if (*YYCURSOR == '\t') {
			spacing |= HEREDOC_USING_TABS;
		} else {
			spacing |= HEREDOC_USING_SPACES;
		}
		++YYCURSOR;
		++indentation;
	}

	if (YYCURSOR == YYLIMIT) {
		YYCURSOR = saved_cursor;
		RETURN_TOKEN(T_START_HEREDOC);
	}

	/* Check for ending label on the next line */
	if (heredoc_label->length < YYLIMIT - YYCURSOR && !memcmp(YYCURSOR, s, heredoc_label->length)) {
		if (!IS_LABEL_SUCCESSOR(YYCURSOR[heredoc_label->length])) {
			if (spacing == (HEREDOC_USING_SPACES | HEREDOC_USING_TABS)) {
				zend_throw_exception(zend_ce_parse_error, "Invalid indentation - tabs and spaces cannot be mixed", 0);
				if (PARSER_MODE()) {
					RETURN_TOKEN(T_ERROR);
				}
			}

			YYCURSOR = saved_cursor;
			heredoc_label->indentation = indentation;

			BEGIN(ST_END_HEREDOC);
			RETURN_TOKEN(T_START_HEREDOC);
		}
	}

	YYCURSOR = saved_cursor;

	if (is_heredoc && !SCNG(heredoc_scan_ahead)) {
		zend_lex_state current_state;
		zend_string *saved_doc_comment = CG(doc_comment);
		int heredoc_nesting_level = 1;
		int first_token = 0;
		int error = 0;

		zend_save_lexical_state(&current_state);

		SCNG(heredoc_scan_ahead) = 1;
		SCNG(heredoc_indentation) = 0;
		SCNG(heredoc_indentation_uses_spaces) = 0;
		LANG_SCNG(on_event) = NULL;
		CG(doc_comment) = NULL;

		zend_ptr_stack_reverse_apply(&current_state.heredoc_label_stack, copy_heredoc_label_stack);

		zend_exception_save();
		while (heredoc_nesting_level) {
			zval zv;
			int retval;

			ZVAL_UNDEF(&zv);
			retval = lex_scan(&zv, NULL);
			zval_ptr_dtor_nogc(&zv);

			if (EG(exception)) {
				zend_clear_exception();
				break;
			}

			if (!first_token) {
				first_token = retval;
			}

			switch (retval) {
				case T_START_HEREDOC:
					++heredoc_nesting_level;
					break;
				case T_END_HEREDOC:
					--heredoc_nesting_level;
					break;
				case END:
					heredoc_nesting_level = 0;
			}
		}
		zend_exception_restore();

		if (
		    (first_token == T_VARIABLE
		     || first_token == T_DOLLAR_OPEN_CURLY_BRACES
		     || first_token == T_CURLY_OPEN
		    ) && SCNG(heredoc_indentation)) {
			zend_throw_exception_ex(zend_ce_parse_error, 0, "Invalid body indentation level (expecting an indentation level of at least %d)", SCNG(heredoc_indentation));
			error = 1;
		}

		heredoc_label->indentation = SCNG(heredoc_indentation);
		heredoc_label->indentation_uses_spaces = SCNG(heredoc_indentation_uses_spaces);

		zend_restore_lexical_state(&current_state);
		SCNG(heredoc_scan_ahead) = 0;
		CG(increment_lineno) = 0;
		CG(doc_comment) = saved_doc_comment;

		if (PARSER_MODE() && error) {
			RETURN_TOKEN(T_ERROR);
		}
	}

	RETURN_TOKEN(T_START_HEREDOC);
}


<ST_IN_SCRIPTING>[`] {
	BEGIN(ST_BACKQUOTE);
	RETURN_TOKEN('`');
}


<ST_END_HEREDOC>{ANY_CHAR} {
	zend_heredoc_label *heredoc_label = zend_ptr_stack_pop(&SCNG(heredoc_label_stack));

	yyleng = heredoc_label->indentation + heredoc_label->length;
	YYCURSOR += yyleng - 1;

	heredoc_label_dtor(heredoc_label);
	efree(heredoc_label);

	BEGIN(ST_IN_SCRIPTING);
	RETURN_TOKEN(T_END_HEREDOC);
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"{$" {
	yy_push_state(ST_IN_SCRIPTING);
	yyless(1);
	enter_nesting('{');
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
		RETURN_END_TOKEN;
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

	if (EXPECTED(zend_scan_escape_string(zendlval, yytext, yyleng, '"') == SUCCESS)
	 || !PARSER_MODE()) {
		RETURN_TOKEN_WITH_VAL(T_ENCAPSED_AND_WHITESPACE);
	} else {
		RETURN_TOKEN(T_ERROR);
	}
}


<ST_BACKQUOTE>{ANY_CHAR} {
	if (YYCURSOR > YYLIMIT) {
		RETURN_END_TOKEN;
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

	if (EXPECTED(zend_scan_escape_string(zendlval, yytext, yyleng, '`') == SUCCESS)
	 || !PARSER_MODE()) {
		RETURN_TOKEN_WITH_VAL(T_ENCAPSED_AND_WHITESPACE);
	} else {
		RETURN_TOKEN(T_ERROR);
	}
}


<ST_HEREDOC>{ANY_CHAR} {
	zend_heredoc_label *heredoc_label = zend_ptr_stack_top(&SCNG(heredoc_label_stack));
	int newline = 0, indentation = 0, spacing = 0;

	if (YYCURSOR > YYLIMIT) {
		RETURN_END_TOKEN;
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
				indentation = spacing = 0;

				while (YYCURSOR < YYLIMIT && (*YYCURSOR == ' ' || *YYCURSOR == '\t')) {
					if (*YYCURSOR == '\t') {
						spacing |= HEREDOC_USING_TABS;
					} else {
						spacing |= HEREDOC_USING_SPACES;
					}
					++YYCURSOR;
					++indentation;
				}

				if (YYCURSOR == YYLIMIT) {
					yyleng = YYCURSOR - SCNG(yy_text);
					HANDLE_NEWLINES(yytext, yyleng);
					ZVAL_NULL(zendlval);
					RETURN_TOKEN_WITH_VAL(T_ENCAPSED_AND_WHITESPACE);
				}

				/* Check for ending label on the next line */
				if (IS_LABEL_START(*YYCURSOR) && heredoc_label->length < YYLIMIT - YYCURSOR && !memcmp(YYCURSOR, heredoc_label->label, heredoc_label->length)) {
					if (IS_LABEL_SUCCESSOR(YYCURSOR[heredoc_label->length])) {
						continue;
					}

					if (spacing == (HEREDOC_USING_SPACES | HEREDOC_USING_TABS)) {
						zend_throw_exception(zend_ce_parse_error, "Invalid indentation - tabs and spaces cannot be mixed", 0);
						if (PARSER_MODE()) {
							RETURN_TOKEN(T_ERROR);
						}
					}

					/* newline before label will be subtracted from returned text, but
					 * yyleng/yytext will include it, for zend_highlight/strip, tokenizer, etc. */
					if (YYCURSOR[-indentation - 2] == '\r' && YYCURSOR[-indentation - 1] == '\n') {
						newline = 2; /* Windows newline */
					} else {
						newline = 1;
					}

					CG(increment_lineno) = 1; /* For newline before label */

					if (SCNG(heredoc_scan_ahead)) {
						SCNG(heredoc_indentation) = indentation;
						SCNG(heredoc_indentation_uses_spaces) = (spacing == HEREDOC_USING_SPACES);
					} else {
						YYCURSOR -= indentation;
					}

					BEGIN(ST_END_HEREDOC);

					goto heredoc_scan_done;
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
	ZVAL_STRINGL(zendlval, yytext, yyleng - newline);

	if (!SCNG(heredoc_scan_ahead) && !EG(exception) && PARSER_MODE()) {
		zend_bool newline_at_start = *(yytext - 1) == '\n' || *(yytext - 1) == '\r';
		zend_string *copy = Z_STR_P(zendlval);

		if (!strip_multiline_string_indentation(
				zendlval, heredoc_label->indentation, heredoc_label->indentation_uses_spaces,
				newline_at_start, newline != 0)) {
			RETURN_TOKEN(T_ERROR);
		}

		if (UNEXPECTED(zend_scan_escape_string(zendlval, ZSTR_VAL(copy), ZSTR_LEN(copy), 0) != SUCCESS)) {
			zend_string_efree(copy);
			RETURN_TOKEN(T_ERROR);
		}

		zend_string_efree(copy);
	} else {
		HANDLE_NEWLINES(yytext, yyleng - newline);
	}

	RETURN_TOKEN_WITH_VAL(T_ENCAPSED_AND_WHITESPACE);
}


<ST_NOWDOC>{ANY_CHAR} {
	zend_heredoc_label *heredoc_label = zend_ptr_stack_top(&SCNG(heredoc_label_stack));
	int newline = 0, indentation = 0, spacing = -1;

	if (YYCURSOR > YYLIMIT) {
		RETURN_END_TOKEN;
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
				indentation = spacing = 0;

				while (YYCURSOR < YYLIMIT && (*YYCURSOR == ' ' || *YYCURSOR == '\t')) {
					if (*YYCURSOR == '\t') {
						spacing |= HEREDOC_USING_TABS;
					} else {
						spacing |= HEREDOC_USING_SPACES;
					}
					++YYCURSOR;
					++indentation;
				}

				if (YYCURSOR == YYLIMIT) {
					yyleng = YYCURSOR - SCNG(yy_text);
					HANDLE_NEWLINES(yytext, yyleng);
					ZVAL_NULL(zendlval);
					RETURN_TOKEN_WITH_VAL(T_ENCAPSED_AND_WHITESPACE);
				}

				/* Check for ending label on the next line */
				if (IS_LABEL_START(*YYCURSOR) && heredoc_label->length < YYLIMIT - YYCURSOR && !memcmp(YYCURSOR, heredoc_label->label, heredoc_label->length)) {
					if (IS_LABEL_SUCCESSOR(YYCURSOR[heredoc_label->length])) {
						continue;
					}

					if (spacing == (HEREDOC_USING_SPACES | HEREDOC_USING_TABS)) {
						zend_throw_exception(zend_ce_parse_error, "Invalid indentation - tabs and spaces cannot be mixed", 0);
						if (PARSER_MODE()) {
							RETURN_TOKEN(T_ERROR);
						}
					}

					/* newline before label will be subtracted from returned text, but
					 * yyleng/yytext will include it, for zend_highlight/strip, tokenizer, etc. */
					if (YYCURSOR[-indentation - 2] == '\r' && YYCURSOR[-indentation - 1] == '\n') {
						newline = 2; /* Windows newline */
					} else {
						newline = 1;
					}

					CG(increment_lineno) = 1; /* For newline before label */

					YYCURSOR -= indentation;
					heredoc_label->indentation = indentation;

					BEGIN(ST_END_HEREDOC);

					goto nowdoc_scan_done;
				}
				/* fall through */
			default:
				continue;
		}
	}

nowdoc_scan_done:
	yyleng = YYCURSOR - SCNG(yy_text);
	ZVAL_STRINGL(zendlval, yytext, yyleng - newline);

	if (!EG(exception) && spacing != -1 && PARSER_MODE()) {
		zend_bool newline_at_start = *(yytext - 1) == '\n' || *(yytext - 1) == '\r';
		if (!strip_multiline_string_indentation(
				zendlval, indentation, spacing == HEREDOC_USING_SPACES,
				newline_at_start, newline != 0)) {
			RETURN_TOKEN(T_ERROR);
		}
	}

	HANDLE_NEWLINES(yytext, yyleng - newline);
	RETURN_TOKEN_WITH_VAL(T_ENCAPSED_AND_WHITESPACE);
}


<ST_IN_SCRIPTING,ST_VAR_OFFSET>{ANY_CHAR} {
	if (YYCURSOR > YYLIMIT) {
		RETURN_END_TOKEN;
	}

	RETURN_TOKEN(T_BAD_CHARACTER);
}

*/

emit_token_with_str:
	zend_copy_value(zendlval, (yytext + offset), (yyleng - offset));

emit_token_with_val:
	if (PARSER_MODE()) {
		ZEND_ASSERT(Z_TYPE_P(zendlval) != IS_UNDEF);
		elem->ast = zend_ast_create_zval_with_lineno(zendlval, start_line);
	}

emit_token:
	if (SCNG(on_event)) {
		SCNG(on_event)(ON_TOKEN, token, start_line, yytext, yyleng, SCNG(on_event_context));
	}
	return token;

emit_token_with_ident:
	if (PARSER_MODE()) {
		elem->ident.offset = SCNG(yy_text) - SCNG(yy_start);
		elem->ident.len = SCNG(yy_leng);
	}
	if (SCNG(on_event)) {
		SCNG(on_event)(ON_TOKEN, token, start_line, yytext, yyleng, SCNG(on_event_context));
	}
	return token;

return_whitespace:
	HANDLE_NEWLINES(yytext, yyleng);
	if (SCNG(on_event)) {
		SCNG(on_event)(ON_TOKEN, T_WHITESPACE, start_line, yytext, yyleng, SCNG(on_event_context));
	}
	if (PARSER_MODE()) {
		start_line = CG(zend_lineno);
		goto restart;
	} else {
		return T_WHITESPACE;
	}

skip_token:
	if (SCNG(on_event)) {
		SCNG(on_event)(ON_TOKEN, token, start_line, yytext, yyleng, SCNG(on_event_context));
	}
	start_line = CG(zend_lineno);
	goto restart;
}
