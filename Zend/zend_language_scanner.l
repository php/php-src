%{

/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2006 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#define yyleng SCNG(yy_leng)
#define yytext SCNG(yy_text)
#define yytext_ptr SCNG(yy_text)
#define yyin SCNG(yy_in)
#define yyout SCNG(yy_out)
#define yy_last_accepting_state SCNG(_yy_last_accepting_state)
#define yy_last_accepting_cpos SCNG(_yy_last_accepting_cpos)
#define yy_more_flag SCNG(_yy_more_flag)
#define yy_more_len SCNG(_yy_more_len)

%}

%x ST_IN_SCRIPTING
%x ST_DOUBLE_QUOTES
%x ST_BACKQUOTE
%x ST_HEREDOC
%x ST_LOOKING_FOR_PROPERTY
%x ST_LOOKING_FOR_VARNAME
%x ST_COMMENT
%x ST_DOC_COMMENT
%x ST_ONE_LINE_COMMENT
%option stack

%{

#include <errno.h>
#include "zend.h"
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
#include "zend_unicode.h"

#ifdef HAVE_STDARG_H
# include <stdarg.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#define YY_DECL int lex_scan(zval *zendlval TSRMLS_DC)

#define ECHO { ZEND_WRITE( yytext, yyleng ); }

#ifdef ZTS
#  define MY_INPUT yyinput
#else
#  define MY_INPUT input
#endif


/* Globals Macros */
#define SCNG	LANG_SCNG
#ifdef ZTS
ZEND_API ts_rsrc_id language_scanner_globals_id;
#else
ZEND_API zend_scanner_globals language_scanner_globals;
#endif

#define YY_INPUT(buf, result, max_size) \
	if ( ((result = zend_unicode_yyinput(yyin, buf, max_size TSRMLS_CC)) == 0) \
		  && zend_stream_ferror( yyin TSRMLS_CC) ) \
		YY_FATAL_ERROR( "input in flex scanner failed" );

#define YY_FATAL_ERROR zend_fatal_scanner_error

#define HANDLE_NEWLINES(s, l)													\
do {																			\
	char *p = (s), *boundary = p+(l);											\
																				\
	while (p<boundary) {														\
		if (*p == '\n') {														\
			CG(zend_lineno)++;													\
		} else if ((*p == '\r') && (p+1 < boundary) && (*(p+1) != '\n')) {		\
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


#define ZEND_IS_OCT(c)  ((c)>='0' && (c)<='7')
#define ZEND_IS_HEX(c)  (((c)>='0' && (c)<='9') || ((c)>='a' && (c)<='f') || ((c)>='A' && (c)<='F'))


void zend_fatal_scanner_error(char *message)
{
	zend_error(E_COMPILE_ERROR, "%s", message);
}

BEGIN_EXTERN_C()
void startup_scanner(TSRMLS_D)
{
	CG(heredoc) = NULL;
	CG(heredoc_len) = 0;
	CG(doc_comment) = NULL;
	CG(doc_comment_len) = 0;
	SCNG(yy_start_stack_ptr) = 0;
	SCNG(yy_start_stack_depth) = 0;
	SCNG(current_buffer) = NULL;

	SCNG(input_conv) = NULL;
	SCNG(output_conv) = NULL;
	SCNG(encoding_checked) = 0;
	SCNG(rest_str) = NULL;
	SCNG(rest_len) = 0;
}


void shutdown_scanner(TSRMLS_D)
{
	if (CG(heredoc)) {
		efree(CG(heredoc));
		CG(heredoc_len)=0;
	}
	if (SCNG(yy_start_stack)) {
		yy_flex_free(SCNG(yy_start_stack));
		SCNG(yy_start_stack) = NULL;
	}
	RESET_DOC_COMMENT();

	if (SCNG(input_conv)) {
		ucnv_close(SCNG(input_conv));
		SCNG(input_conv) = NULL;
	}
	if (SCNG(output_conv)) {
		ucnv_close(SCNG(output_conv));
		SCNG(output_conv) = NULL;
	}
	SCNG(encoding_checked) = 0;
	if (SCNG(rest_str)) {
		efree(SCNG(rest_str));
		SCNG(rest_str) = NULL;
	}
	SCNG(rest_len) = 0;
}
END_EXTERN_C()


ZEND_API void zend_save_lexical_state(zend_lex_state *lex_state TSRMLS_DC)
{
	memcpy(&lex_state->buffer_state, &YY_CURRENT_BUFFER, sizeof(YY_BUFFER_STATE));
	lex_state->in = SCNG(yy_in);
	lex_state->state = YYSTATE;
	lex_state->filename = zend_get_compiled_filename(TSRMLS_C);
	lex_state->lineno = CG(zend_lineno);

	lex_state->input_conv = SCNG(input_conv);
	lex_state->output_conv = SCNG(output_conv);
	lex_state->encoding_checked = SCNG(encoding_checked);
	lex_state->rest_str = SCNG(rest_str);
	lex_state->rest_len = SCNG(rest_len);
	SCNG(input_conv) = NULL;
	SCNG(output_conv) = NULL;
	SCNG(encoding_checked) = 0;
	SCNG(rest_str) = NULL;
	SCNG(rest_len) = 0;
}

ZEND_API void zend_restore_lexical_state(zend_lex_state *lex_state TSRMLS_DC)
{
	YY_BUFFER_STATE original_buffer_state = YY_CURRENT_BUFFER;

	if (lex_state->buffer_state) {
		yy_switch_to_buffer(lex_state->buffer_state TSRMLS_CC);
	} else {
		YY_CURRENT_BUFFER = NULL;
	}

	yy_delete_buffer(original_buffer_state TSRMLS_CC);
	SCNG(yy_in) = lex_state->in;
	BEGIN(lex_state->state);
	CG(zend_lineno) = lex_state->lineno;
	zend_restore_compiled_filename(lex_state->filename TSRMLS_CC);
	zend_restore_compiled_script_encoding(lex_state->script_encoding TSRMLS_CC);

	if (SCNG(input_conv)) {
		ucnv_close(SCNG(input_conv));
	}
	SCNG(input_conv) = lex_state->input_conv;
	if (SCNG(output_conv)) {
		ucnv_close(SCNG(output_conv));
	}
	SCNG(output_conv) = lex_state->output_conv;
	SCNG(encoding_checked) = lex_state->encoding_checked;
	if (SCNG(rest_str)) {
		efree(SCNG(rest_str));
	}
	SCNG(rest_str) = lex_state->rest_str;
	SCNG(rest_len) = lex_state->rest_len;
}


BEGIN_EXTERN_C()


ZEND_API void zend_file_handle_dtor(zend_file_handle *fh)
{
	TSRMLS_FETCH();

	switch (fh->type) {
		case ZEND_HANDLE_FP:
			fclose(fh->handle.fp);
			break;
		case ZEND_HANDLE_STREAM:
			if (fh->handle.stream.closer) {
				fh->handle.stream.closer(fh->handle.stream.handle TSRMLS_CC);
			}
			break;
		case ZEND_HANDLE_FILENAME:
			/* We're only supposed to get here when destructing the used_files hash,
			 * which doesn't really contain open files, but references to their names/paths
			 */
			break;
	}
	if (fh->opened_path) {
		efree(fh->opened_path);
		fh->opened_path = NULL;
	}
	if (fh->free_filename && fh->filename) {
		efree(fh->filename);
		fh->filename = NULL;
	}
}


int zend_compare_file_handles(zend_file_handle *fh1, zend_file_handle *fh2)
{
	if (fh1->type != fh2->type) {
		return 0;
	}
	switch (fh1->type) {
		case ZEND_HANDLE_FP:
			return fh1->handle.fp==fh2->handle.fp;
			break;
		case ZEND_HANDLE_STREAM:
			return fh1->handle.stream.handle == fh2->handle.stream.handle;
			break;
	}
	return 0;
}


ZEND_API void zend_destroy_file_handle(zend_file_handle *file_handle TSRMLS_DC)
{
	zend_llist_del_element(&CG(open_files), file_handle, (int (*)(void *, void *)) zend_compare_file_handles);
}

/* Convert one octal digit to a numeric value 0..7, or -1 on failure */
static inline int8_t zend_get_octal_digit(UChar c) {
    if (c >= 0x0030 && c <= 0x0037) {
        return (int8_t)(c - 0x0030);
    }
    return -1;
}

/*
 * Convert one hex digit to a numeric value 0..F, or -1 on failure.
 * Borrowed from ICU.
 */
static inline int8_t zend_get_hex_digit(UChar c) {
    if (c >= 0x0030 && c <= 0x0039) {
        return (int8_t)(c - 0x0030);
    }
    if (c >= 0x0041 && c <= 0x0046) {
        return (int8_t)(c - (0x0041 - 10));
    }
    if (c >= 0x0061 && c <= 0x0066) {
        return (int8_t)(c - (0x0061 - 10));
    }
    return -1;
}

static inline zend_bool zend_digits_to_codepoint(char *s, char *end, UChar32 *c, int8_t digits)
{
	int8_t n = 0;
	int8_t digit = 0;
	UChar32 codepoint = 0;

	while (s < end && n < digits) {
		digit = zend_get_hex_digit((UChar)*s);
		if (digit < 0) {
			break;
		}
		codepoint = (codepoint << 4) | digit;
		++s;
		++n;
	}

	if (n < digits) {
		return 0;
	}

	*c = codepoint;
	return 1;
}

static inline zend_bool zend_udigits_to_codepoint(UChar *s, UChar *end, UChar32 *c, int8_t digits)
{
	int8_t n = 0;
	int8_t digit = 0;
	UChar32 codepoint = 0;

	while (s < end && n < digits) {
		digit = zend_get_hex_digit(*s);
		if (digit < 0) {
			break;
		}
		codepoint = (codepoint << 4) | digit;
		++s;
		++n;
	}

	if (n < digits) {
		return 0;
	}

	*c = codepoint;
	return 1;
}

static inline int zend_uchar_from_name(char *name, UChar32 *c)
{
	UChar32 codepoint = 0;
	UErrorCode status = U_ZERO_ERROR;

	codepoint = u_charFromName(U_UNICODE_CHAR_NAME, name, &status);
	if (U_SUCCESS(status)) {
		*c = codepoint;
		return 1;
	} else {
		return 0;
	}
}

static inline int zend_uchar_from_uname(UChar *name, int name_len, UChar32 *c TSRMLS_DC)
{
	UChar32 codepoint = 0;
	UErrorCode status = U_ZERO_ERROR;
	char *buf;

	buf = zend_unicode_to_ascii(name, name_len TSRMLS_CC);
	if (!buf) return 0;
	codepoint = u_charFromName(U_UNICODE_CHAR_NAME, buf, &status);
	efree(buf);
	if (U_SUCCESS(status)) {
		*c = codepoint;
		return 1;
	} else {
		return 0;
	}
}

static inline int zend_parse_charname_sequence(UChar **s, UChar *end, UChar32 *c TSRMLS_DC)
{
	UChar *start;

	if (**s == '{') {
		start = ++(*s);
		while ((*s)++ != end) {
			if (**s == '}') {
				if (zend_uchar_from_uname(start, *s - start, c TSRMLS_CC)) {
					return 1;
				} else {
					/* safe, since *s points to '}' */
					**s = 0;
					zend_error(E_COMPILE_WARNING, "Invalid Unicode character name: '%r'", start);
					break;
				}
			}
		}
	}

	return 0;
}

static inline int zend_copy_string_value(zval *zendlval, char *str, zend_uint str_len, zend_uchar type TSRMLS_DC)
{
	UErrorCode status = U_ZERO_ERROR;
	int consumed = 0;

	if (type == IS_UNICODE) {
		consumed = zend_convert_scanner_output(&Z_USTRVAL_P(zendlval), &Z_USTRLEN_P(zendlval), str, str_len, &status TSRMLS_CC);

		if (U_FAILURE(status)) {
			zend_error(E_COMPILE_WARNING,"Illegal or truncated character in input: offset %d, state=%d", consumed, YYSTATE);
			efree(Z_USTRVAL_P(zendlval));
			return 0;
		}
		Z_TYPE_P(zendlval) = IS_UNICODE;
	} else {
		Z_STRVAL_P(zendlval) = (char *)estrndup(str, str_len);
		Z_STRLEN_P(zendlval) = str_len;
		Z_TYPE_P(zendlval) = type;
	}

	return 1;
}

static inline int zend_check_and_normalize_identifier(zval *zendlval)
{
	UChar *norm;
	int norm_len;

	if (!zend_is_valid_identifier(Z_USTRVAL_P(zendlval), Z_USTRLEN_P(zendlval))) {
		zend_error(E_COMPILE_WARNING, "Invalid identifier syntax: %r", Z_USTRVAL_P(zendlval));
		efree(Z_USTRVAL_P(zendlval));
		return 0;
	}
	if (!zend_normalize_identifier(&norm, &norm_len, Z_USTRVAL_P(zendlval), Z_USTRLEN_P(zendlval), 0)) {
		zend_error(E_COMPILE_WARNING, "Could not normalize identifier: %r", Z_USTRVAL_P(zendlval));
		efree(Z_USTRVAL_P(zendlval));
		return 0;
	}
	if (norm != Z_USTRVAL_P(zendlval)) {
		efree(Z_USTRVAL_P(zendlval));
		ZVAL_UNICODEL(zendlval, norm, norm_len, 0);
	}
	return 1;
}

static void zend_scanner_output_callback(
    const void *context,
    UConverterToUnicodeArgs *toUArgs,
    const char *codeUnits,
    int32_t length,
    UConverterCallbackReason reason,
    UErrorCode *err
    )
{
    if (*err == U_TRUNCATED_CHAR_FOUND ||
        *err == U_ILLEGAL_CHAR_FOUND ||
		*err == U_INVALID_CHAR_FOUND) {
        *(const char **)context = toUArgs->source - length;
    }

    return;
}

static int is_encoding_flex_compatible(const char *enc TSRMLS_DC)
{
	int key_len = strlen(enc)+1;
	unsigned char ret;
	unsigned char *ret_ptr;

	if (zend_hash_find(&UG(flex_compatible), (char*)enc, key_len, (void**)&ret_ptr) == SUCCESS) {
		return *ret_ptr;
	} else {
		UErrorCode status = U_ZERO_ERROR;
		UConverter *conv = ucnv_open(enc, &status);

		if (U_FAILURE(status)) {
			return 0;
		}

		switch (ucnv_getType(conv)) {
			case UCNV_DBCS:
			case UCNV_UTF16_BigEndian:
			case UCNV_UTF16_LittleEndian:
			case UCNV_UTF32_BigEndian:
			case UCNV_UTF32_LittleEndian:
			case UCNV_EBCDIC_STATEFUL:
			case UCNV_ISO_2022:
			case UCNV_LMBCS_1:
			case UCNV_LMBCS_2:
			case UCNV_LMBCS_3:
			case UCNV_LMBCS_4:
			case UCNV_LMBCS_5:
			case UCNV_LMBCS_6:
			case UCNV_LMBCS_8:
			case UCNV_LMBCS_11:
			case UCNV_LMBCS_16:
			case UCNV_LMBCS_17:
			case UCNV_LMBCS_18:
			case UCNV_LMBCS_19:
			case UCNV_HZ:
			case UCNV_SCSU:
			case UCNV_UTF7:
			case UCNV_BOCU1:
			case UCNV_UTF16:
			case UCNV_UTF32:
			case UCNV_IMAP_MAILBOX:
				ret = 0;
				break;
			case UCNV_LATIN_1:
			case UCNV_UTF8:
			case UCNV_ISCII:
			case UCNV_US_ASCII:
			case UCNV_CESU8:
				ret = 1;
				break;
			default: {
				static const UChar ascii[] = {
					0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
					0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
					0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
					0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
					0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
					0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
					0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
					0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E
				};
				static const char expected[] =
					"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
					"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
					"\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F"
					"\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F"
					"\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4A\x4B\x4C\x4D\x4E\x4F"
					"\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5A\x5B\x5C\x5D\x5E\x5F"
					"\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6A\x6B\x6C\x6D\x6E\x6F"
					"\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7A\x7B\x7C\x7D\x7E";
				char output[sizeof(expected)];

				if (ucnv_fromUChars(conv,
				                    output, sizeof(output),
				                    ascii, sizeof(expected),
				                    &status) != sizeof(expected) ||
				    U_FAILURE(status)) {
					ret = 0;
				} else {
					ret = (memcmp(expected, output, sizeof(expected)) == 0);
				}
			}
		}
		ucnv_close(conv);

		zend_hash_add(&UG(flex_compatible), (char*)enc, key_len, (void**)&ret, sizeof(ret), NULL);

		return ret;
	}
}

ZEND_API int zend_prepare_scanner_converters(const char *onetime_encoding, int run_time TSRMLS_DC)
{
	const char *encoding = NULL;

	if (SCNG(input_conv)) {
		/* Script is already converted to UTF-8 */
		return zend_set_converter_encoding(&SCNG(output_conv), "UTF-8");
	} else {
		encoding = onetime_encoding;
	}

	/* We need to convert the input stream only if script_encoding is not ASCII compatible */
	if (!is_encoding_flex_compatible(encoding TSRMLS_CC)) {
		if (zend_set_converter_encoding(&SCNG(input_conv), encoding) == FAILURE) {
			return FAILURE;
		}
		if (run_time) {
			/* Convert rest of the buffer to unicode.runtime_encoding. */
			YY_BUFFER_STATE b = YY_CURRENT_BUFFER;
			int offset, length, size;
			const char *src, *old_src;
			char rt_buf[8192];
			char *target = rt_buf;
			UErrorCode status = U_ZERO_ERROR;

			old_src = src = yy_c_buf_p;
			offset = yy_c_buf_p - b->yy_ch_buf;
			length = b->yy_n_chars - offset;
			size = b->yy_buf_size - offset;

			ucnv_convertEx(UG(utf8_conv),
			               SCNG(input_conv),
			               &target, rt_buf+size-2,
			               &src, src+length,
			               NULL, NULL, NULL, NULL,
			               TRUE, TRUE,
			               &status);

			if (src - old_src < length) {
				/* Cannot fit into buffer. Schedule for next read. */
				SCNG(rest_len) = length - (src - old_src);
				SCNG(rest_str) = emalloc(SCNG(rest_len));
				memcpy(SCNG(rest_str), src, SCNG(rest_len));
			}
			length = target - rt_buf;
			memcpy(yy_c_buf_p, rt_buf, length);
			SCNG(yy_n_chars) = b->yy_n_chars = length + offset;
			b->yy_ch_buf[b->yy_n_chars] = YY_END_OF_BUFFER_CHAR;
			b->yy_ch_buf[b->yy_n_chars+1] = YY_END_OF_BUFFER_CHAR;
		}
		encoding = "UTF-8";
	}
	return zend_set_converter_encoding(&SCNG(output_conv), encoding);
}

ZEND_API int zend_convert_scanner_output(UChar **target, int *target_len, const char *source, int source_len, UErrorCode *status TSRMLS_DC)
{
	const char *source_consumed = NULL;

	/* set our custom callback with context */
	ucnv_setToUCallBack(SCNG(output_conv), zend_scanner_output_callback, &source_consumed, NULL, NULL, status);

	/* reset the error and perform conversion */
	*status = U_ZERO_ERROR;
	zend_convert_to_unicode(SCNG(output_conv), target, target_len, source, source_len, status);

	/* figure out how many source bytes were consumed */
	if (U_SUCCESS(*status)) {
		return source_len;
	} else if (source_consumed) {
		return source_consumed - source;
	} else {
		return 0;
	}
}

int zend_unicode_yyinput(zend_file_handle *file_handle, char *buf, size_t len TSRMLS_DC)
{
	size_t n;
	int c = '*';
	const char *src = buf;

	/* Look of we have rest from previous call */
	if (SCNG(rest_str)) {
		if (len >= SCNG(rest_len)) {
			memcpy(buf, SCNG(rest_str), SCNG(rest_len));
			efree(SCNG(rest_str));
			SCNG(rest_str) = NULL;
			n = SCNG(rest_len);
			SCNG(rest_len) = 0;
		} else {
			memcpy(buf, SCNG(rest_str), len);
			memcpy(SCNG(rest_str), SCNG(rest_str)+len, SCNG(rest_len)-len);
			n = len;
			SCNG(rest_len) -= len;
		}
	} else {
		if (file_handle->handle.stream.interactive) {
			for (n = 0; n < sizeof(buf) && (c = zend_stream_getc(yyin TSRMLS_CC)) != EOF && c != '\n'; ++n) {
				buf[n] = (char)c;
			}
			if (c == '\n') {
				buf[n++] = (char) c;
			}
		} else {
			n = zend_stream_read(file_handle, buf, len TSRMLS_CC);
	  }
	}

	/* Don't make any conversions if unicode=off */
  if (!UG(unicode)) {
  	return n;
  }

	/* Autodetect encoding */
	if (!SCNG(encoding_checked)) {
		int32_t    signatureLength;
		UErrorCode status = U_ZERO_ERROR;
		const char *encoding;

		encoding = ucnv_detectUnicodeSignature(buf, n, &signatureLength, &status);
		if (encoding && U_SUCCESS(status)) {
			src += signatureLength;
			n -= signatureLength;
			if (is_encoding_flex_compatible(encoding TSRMLS_CC)) {
				if (SCNG(input_conv)) {
					ucnv_close(SCNG(input_conv));
					SCNG(input_conv) = NULL;
				}
				zend_set_converter_encoding(&SCNG(output_conv), encoding);
				if (signatureLength > 0) {
					memcpy(buf, src, n);
				}
			} else {
				zend_set_converter_encoding(&SCNG(input_conv), encoding);
				zend_set_converter_encoding(&SCNG(output_conv), "UTF-8");
			}
		}
		status = U_ZERO_ERROR;
		SCNG(encoding_checked) = 1;
	}

	if (SCNG(input_conv) && n >= 0) {
		UErrorCode status = U_ZERO_ERROR;
		char rt_buf[8192];
		char *target = rt_buf;
		const char *old_src = src;

		ucnv_convertEx(UG(utf8_conv),
		               SCNG(input_conv),
		               &target, rt_buf+len,
		               &src, src+n,
		               NULL, NULL, NULL, NULL,
		               TRUE, TRUE,
		               &status);
		if (src - old_src < n) {
			/* Cannot fit into buffer. Schedule for next read. */
			SCNG(rest_len) = n - (src - old_src);
			SCNG(rest_str) = emalloc(SCNG(rest_len));
			memcpy(SCNG(rest_str), src, SCNG(rest_len));
		}
		n = target - rt_buf;
		memcpy(buf, rt_buf, n);
	}
	return n;
}

ZEND_API int open_file_for_scanning(zend_file_handle *file_handle TSRMLS_DC)
{
	char *file_path=NULL;
	UErrorCode status = U_ZERO_ERROR;

	if (FAILURE == zend_stream_fixup(file_handle TSRMLS_CC)) {
		return FAILURE;
	}

	zend_llist_add_element(&CG(open_files), file_handle);

	/* Reset the scanner for scanning the new file */
	SCNG(yy_in) = file_handle;

	zend_prepare_scanner_converters(ucnv_getName(ZEND_U_CONVERTER(UG(script_encoding_conv)), &status), 0 TSRMLS_CC);
	yy_switch_to_buffer(yy_create_buffer(SCNG(yy_in), YY_BUF_SIZE TSRMLS_CC) TSRMLS_CC);

	BEGIN(INITIAL);

	if (file_handle->opened_path) {
		file_path = file_handle->opened_path;
	} else {
		file_path = file_handle->filename;
	}

	zend_set_compiled_filename(file_path TSRMLS_CC);
	zend_set_compiled_script_encoding((char*)ucnv_getName(SCNG(output_conv), &status) TSRMLS_CC);

	if (CG(start_lineno)) {
		CG(zend_lineno) = CG(start_lineno);
		CG(start_lineno) = 0;
	} else {
		CG(zend_lineno) = 1;
	}

	CG(increment_lineno) = 0;
	return SUCCESS;
}
END_EXTERN_C()


ZEND_API zend_op_array *compile_file(zend_file_handle *file_handle, int type TSRMLS_DC)
{
	zend_lex_state original_lex_state;
	zend_op_array *op_array = (zend_op_array *) emalloc(sizeof(zend_op_array));
	zend_op_array *original_active_op_array = CG(active_op_array);
	zend_op_array *retval=NULL;
	int compiler_result;
	zend_bool compilation_successful=0;
	znode retval_znode;
	zend_bool original_in_compilation = CG(in_compilation);

	retval_znode.op_type = IS_CONST;
	Z_TYPE(retval_znode.u.constant) = IS_LONG;
	Z_LVAL(retval_znode.u.constant) = 1;
	retval_znode.u.constant.is_ref = 0;
	retval_znode.u.constant.refcount = 1;

	zend_save_lexical_state(&original_lex_state TSRMLS_CC);

	retval = op_array; /* success oriented */

	if (open_file_for_scanning(file_handle TSRMLS_CC)==FAILURE) {
		if (type==ZEND_REQUIRE) {
			zend_message_dispatcher(ZMSG_FAILED_REQUIRE_FOPEN, file_handle->filename);
			zend_bailout();
		} else {
			zend_message_dispatcher(ZMSG_FAILED_INCLUDE_FOPEN, file_handle->filename);
		}
		compilation_successful=0;
	} else {
		init_op_array(op_array, ZEND_USER_FUNCTION, INITIAL_OP_ARRAY_SIZE TSRMLS_CC);
		CG(in_compilation) = 1;
		CG(active_op_array) = op_array;
		compiler_result = zendparse(TSRMLS_C);
		zend_do_return(&retval_znode, 0 TSRMLS_CC);
		zend_do_handle_exception(TSRMLS_C);
		CG(in_compilation) = original_in_compilation;
		if (compiler_result==1) { /* parser error */
			zend_bailout();
		}
		compilation_successful=1;
	}

	if (retval) {
		CG(active_op_array) = original_active_op_array;
		if (compilation_successful) {
			pass_two(op_array TSRMLS_CC);
			zend_release_labels(TSRMLS_C);
		} else {
			efree(op_array);
			retval = NULL;
		}
	}
	if (compilation_successful) {
		zend_restore_lexical_state(&original_lex_state TSRMLS_CC);
	}
	return retval;
}


zend_op_array *compile_filename(int type, zval *filename TSRMLS_DC)
{
	zend_file_handle file_handle;
	zval tmp;
	zend_op_array *retval;
	char *opened_path = NULL;

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

	retval = zend_compile_file(&file_handle, type TSRMLS_CC);
	if (retval && file_handle.handle.stream.handle) {
		int dummy = 1;

		if (!file_handle.opened_path) {
			file_handle.opened_path = opened_path = estrndup(Z_STRVAL_P(filename), Z_STRLEN_P(filename));
		}

		zend_hash_add(&EG(included_files), file_handle.opened_path, strlen(file_handle.opened_path)+1, (void *)&dummy, sizeof(int), NULL);

		if (opened_path) {
			efree(opened_path);
		}
	}
	zend_destroy_file_handle(&file_handle TSRMLS_CC);

	if (filename==&tmp) {
		zval_dtor(&tmp);
	}
	return retval;
}

ZEND_API int zend_prepare_string_for_scanning(zval *str, char *filename TSRMLS_DC)
{
	const char *encoding;
	UErrorCode status = U_ZERO_ERROR;

	if (Z_TYPE_P(str) == IS_UNICODE) {
		convert_to_string_with_converter(str, UG(utf8_conv));
		encoding = "UTF-8";
	} else {
		UErrorCode status = U_ZERO_ERROR;
		encoding = ucnv_getName(ZEND_U_CONVERTER(UG(runtime_encoding_conv)), &status);
	}

	/* enforce two trailing NULLs for flex... */
	STR_REALLOC(Z_STRVAL_P(str), Z_STRLEN_P(str)+2);

	Z_STRVAL_P(str)[Z_STRLEN_P(str)+1]=0;

	SCNG(yy_in)=NULL;

	zend_prepare_scanner_converters(encoding, 0 TSRMLS_CC);
	yy_scan_buffer(Z_STRVAL_P(str), Z_STRLEN_P(str)+2 TSRMLS_CC);

	zend_set_compiled_filename(filename TSRMLS_CC);
	zend_set_compiled_script_encoding((char*)ucnv_getName(SCNG(output_conv), &status) TSRMLS_CC);
	CG(zend_lineno) = 1;
	CG(increment_lineno) = 0;
	return SUCCESS;
}


ZEND_API int zend_get_scanned_file_offset(TSRMLS_D)
{
	if (yyin) {
		int offset_in_buffer = (yy_c_buf_p - (YY_CURRENT_BUFFER)->yy_ch_buf);
		int read_bytes = SCNG(yy_n_chars);
		int offset_from_the_end = read_bytes - offset_in_buffer;

		return zend_stream_ftell(yyin TSRMLS_CC) - offset_from_the_end;
	} else {
		return -1;
	}
}


zend_op_array *compile_string(zval *source_string, char *filename TSRMLS_DC)
{
	zend_lex_state original_lex_state;
	zend_op_array *op_array = (zend_op_array *) emalloc(sizeof(zend_op_array));
	zend_op_array *original_active_op_array = CG(active_op_array);
	zend_op_array *retval;
	zval tmp;
	int compiler_result;
	zend_bool original_in_compilation = CG(in_compilation);

	tmp = *source_string;
	zval_copy_ctor(&tmp);
	if (Z_TYPE(tmp) != IS_STRING && Z_TYPE(tmp) != IS_UNICODE) {
		convert_to_text(&tmp);
	}
	source_string = &tmp;

	if (Z_USTRLEN_P(source_string)==0) {
		efree(op_array);
		zval_dtor(&tmp);
		return NULL;
	}

	CG(in_compilation) = 1;

	zend_save_lexical_state(&original_lex_state TSRMLS_CC);
	if (zend_prepare_string_for_scanning(source_string, filename TSRMLS_CC)==FAILURE) {
		efree(op_array);
		retval = NULL;
	} else {
		init_op_array(op_array, ZEND_EVAL_CODE, INITIAL_OP_ARRAY_SIZE TSRMLS_CC);
		CG(active_op_array) = op_array;
		BEGIN(ST_IN_SCRIPTING);
		compiler_result = zendparse(TSRMLS_C);

		if (compiler_result==1) {
			CG(active_op_array) = original_active_op_array;
			CG(unclean_shutdown)=1;
			retval = NULL;
		} else {
			zend_do_return(NULL, 0 TSRMLS_CC);
			zend_do_handle_exception(TSRMLS_C);
			CG(active_op_array) = original_active_op_array;
			pass_two(op_array TSRMLS_CC);
			zend_release_labels(TSRMLS_C);
			retval = op_array;
		}
		zend_restore_lexical_state(&original_lex_state TSRMLS_CC);
	}
	zval_dtor(&tmp);
	CG(in_compilation) = original_in_compilation;
	return retval;
}


BEGIN_EXTERN_C()
int highlight_file(char *filename, zend_syntax_highlighter_ini *syntax_highlighter_ini TSRMLS_DC)
{
	zend_lex_state original_lex_state;
	zend_file_handle file_handle;

	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.filename = filename;
	file_handle.free_filename = 0;
	file_handle.opened_path = NULL;
	zend_save_lexical_state(&original_lex_state TSRMLS_CC);
	if (open_file_for_scanning(&file_handle TSRMLS_CC)==FAILURE) {
		zend_message_dispatcher(ZMSG_FAILED_HIGHLIGHT_FOPEN, filename);
		return FAILURE;
	}
	zend_highlight(syntax_highlighter_ini TSRMLS_CC);
	zend_destroy_file_handle(&file_handle TSRMLS_CC);
	zend_restore_lexical_state(&original_lex_state TSRMLS_CC);
	return SUCCESS;
}

int highlight_string(zval *str, zend_syntax_highlighter_ini *syntax_highlighter_ini, char *str_name TSRMLS_DC)
{
	zend_lex_state original_lex_state;
	zval tmp = *str;

	str = &tmp;
	zval_copy_ctor(str);
	zend_save_lexical_state(&original_lex_state TSRMLS_CC);
	if (zend_prepare_string_for_scanning(str, str_name TSRMLS_CC)==FAILURE) {
		return FAILURE;
	}
	BEGIN(INITIAL);
	zend_highlight(syntax_highlighter_ini TSRMLS_CC);
	zend_restore_lexical_state(&original_lex_state TSRMLS_CC);
	zval_dtor(str);
	return SUCCESS;
}
END_EXTERN_C()

#define zend_copy_value(zendlval, yytext, yyleng) \
	Z_STRVAL_P(zendlval) = (char *)estrndup(yytext, yyleng); \
	Z_STRLEN_P(zendlval) = yyleng;

int zend_scan_unicode_double_string(zval *zendlval TSRMLS_DC)
{
	register UChar *s, *t, c;
	UChar *end;
	UChar32 codepoint = 0;
	int8_t digit;
	int8_t min_digits = 0, max_digits = 0;
	int8_t bits;
	int8_t n;

	HANDLE_NEWLINES(yytext, yyleng);

	if (!zend_copy_string_value(zendlval, yytext+1, yyleng-2, IS_UNICODE TSRMLS_CC)) {
		return 0;
	}

	/* convert escape sequences */
	s = t = Z_USTRVAL_P(zendlval);
	end = s+Z_USTRLEN_P(zendlval);
	while (s<end) {
		if (*s==0x5C /*'\\'*/) {
			s++;
			c = *s;
			if (s>=end) {
				continue;
			}

			min_digits = 0;
			codepoint = 0;
			bits = 4;
			n = 0;

			switch(c) {
				case 0x6E:               /*'n'*/
					*t++ = (UChar) 0x0A; /*'\n'*/
					Z_USTRLEN_P(zendlval)--;
					break;
				case 0x72:               /*'r'*/
					*t++ = (UChar) 0x0D; /*'\r'*/
					Z_USTRLEN_P(zendlval)--;
					break;
				case 0x74:               /*'t'*/
					*t++ = (UChar) 0x09; /*'\t'*/
					Z_USTRLEN_P(zendlval)--;
					break;
				case 0x5C:               /*'\\'*/
				case 0x24:               /*'$'*/
				case 0x22:               /*'"'*/
					*t++ = *s;
					Z_USTRLEN_P(zendlval)--;
					break;
				case 0x43:				 /*'C'*/
					{
						UChar *p = s+1;
						if (p < end && zend_parse_charname_sequence(&p, end, &codepoint TSRMLS_CC)) {
							Z_USTRLEN_P(zendlval) -= p - s + 1;
							s = p;
							if (U_IS_BMP(codepoint)) {
								*t++ = (UChar) codepoint;
							} else {
								*t++ = (UChar) U16_LEAD(codepoint);
								*t++ = (UChar) U16_TRAIL(codepoint);
								Z_USTRLEN_P(zendlval)++;
							}
						} else {
							zend_error(E_COMPILE_WARNING, "Invalid \\C{..} sequence");
							efree(Z_USTRVAL_P(zendlval));
							return 0;
						}
						break;
					}
				case 0x75:               /*'u'*/
					min_digits = 4;
					max_digits = 4;
					Z_USTRLEN_P(zendlval)--;
					break;
				case 0x55:               /*'U'*/
					min_digits = 6;
					max_digits = 6;
					Z_USTRLEN_P(zendlval)--;
					break;
				default:
					digit = zend_get_octal_digit(*s);
					if (digit >= 0) {
						min_digits = 1;
						max_digits = 3;
						bits = 3;
						n = 1; /* already have one digit */
						codepoint = digit;
					} else if (c == 0x78  /*'x'*/
							   && (s+1) < end && (digit = zend_get_hex_digit(*(s+1))) >= 0) {
						min_digits = 1;
						max_digits = 2;
						Z_USTRLEN_P(zendlval)--;
						s++;
						n = 1; /* already have one digit */
						codepoint = digit;
					} else {
						*t++ = 0x5C; /*'\\'*/
						*t++ = *s;
					}
					break;
			}

			/* need to parse a number for one of the escape sequences */
			if (min_digits != 0) {
				while (s++ < end && n < max_digits) {
					digit = (bits == 4) ? zend_get_hex_digit(*s) : zend_get_octal_digit(*s);
					if (digit < 0) {
						break;
					}
					codepoint = (codepoint << bits) | digit;
					n++;
				}

				if (n < min_digits) {
					/* can only happen for \u and \U sequences */
					zend_error(E_COMPILE_WARNING,"\\%c escape sequence requires exactly %d hexadecimal digits", (char) c, min_digits);
					efree(Z_USTRVAL_P(zendlval));
					return 0;
				}

				if (U_IS_BMP(codepoint)) {
					*t++ = (UChar) codepoint;
					Z_USTRLEN_P(zendlval) -= n;
				} else if (codepoint <= 0x10FFFF) {
					*t++ = (UChar) U16_LEAD(codepoint);
					*t++ = (UChar) U16_TRAIL(codepoint);
					Z_USTRLEN_P(zendlval) -= n-1;
				} else {
					zend_error(E_COMPILE_WARNING,"\\U%06x is above the highest valid codepoint 0x10FFFF", codepoint);
					efree(Z_USTRVAL_P(zendlval));
					return 0;
				}
			} else {
				s++;
			}
		} else {
			*t++ = *s++;
		}
	}
	*t = 0;

	return T_CONSTANT_ENCAPSED_STRING;
}

int zend_scan_unicode_single_string(zval *zendlval TSRMLS_DC)
{
	register UChar *s, *t;
	UChar *end;
	UChar32 codepoint = 0;

	HANDLE_NEWLINES(yytext, yyleng);

	if (!zend_copy_string_value(zendlval, yytext+1, yyleng-2, IS_UNICODE TSRMLS_CC)) {
		return 0;
	}

	/* convert escape sequences */
	s = t = Z_USTRVAL_P(zendlval);
	end = s+Z_USTRLEN_P(zendlval);
	while (s<end) {
		if (*s==0x5C /*'\\'*/) {
			s++;
			if (s>=end) {
				continue;
			}
			switch(*s) {
				case 0x5C: /*'\\'*/
				case 0x27: /*'\''*/
					*t++ = *s;
					Z_USTRLEN_P(zendlval)--;
					break;
				case 0x43: /*'C'*/
					{
						UChar *p = s+1;
						if (p < end && zend_parse_charname_sequence(&p, end, &codepoint TSRMLS_CC)) {
							Z_USTRLEN_P(zendlval) -= p - s + 1;
							s = p;
							if (U_IS_BMP(codepoint)) {
								*t++ = (UChar) codepoint;
							} else {
								*t++ = (UChar) U16_LEAD(codepoint);
								*t++ = (UChar) U16_TRAIL(codepoint);
								Z_USTRLEN_P(zendlval)++;
							}
						} else {
							zend_error(E_COMPILE_WARNING, "Invalid \\C{..} sequence");
							efree(Z_USTRVAL_P(zendlval));
							return 0;
						}
						break;
					}
				case 0x75 /*'u'*/:
					{
						codepoint = 0;
						if (zend_udigits_to_codepoint(s+1, end, &codepoint, 4)) {
							*t++ = (UChar) codepoint;
							s += 4;
							Z_USTRLEN_P(zendlval) -= 5;
						} else {
							zend_error(E_COMPILE_WARNING,"\\u escape sequence requires exactly 4 hexadecimal digits");
							efree(Z_USTRVAL_P(zendlval));
							return 0;
						}
						break;
					}
				case 0x55 /*'U'*/:
					{
						codepoint = 0;
						if (zend_udigits_to_codepoint(s+1, end, &codepoint, 6)) {
							if (U_IS_BMP(codepoint)) {
								*t++ = (UChar) codepoint;
								Z_USTRLEN_P(zendlval) -= 7;
							} else if (codepoint <= 0x10FFFF) {
								*t++ = (UChar) U16_LEAD(codepoint);
								*t++ = (UChar) U16_TRAIL(codepoint);
								Z_USTRLEN_P(zendlval) -= 6;
							} else {
								zend_error(E_COMPILE_WARNING,"\\U%06x is above the highest valid codepoint 0x10FFFF", codepoint);
								efree(Z_USTRVAL_P(zendlval));
								return 0;
							}
							s += 6;
						} else {
							zend_error(E_COMPILE_WARNING,"\\U escape sequence requires exactly 6 hexadecimal digits");
							efree(Z_USTRVAL_P(zendlval));
							return 0;
						}
						break;
					}
				default:
					*t++ = 0x5C; /*'\\'*/
					*t++ = *s;
					break;
			}
			s++;
		} else {
			*t++ = *s++;
		}
	}
	*t = 0;

	return T_CONSTANT_ENCAPSED_STRING;
}

int zend_scan_binary_double_string(zval *zendlval TSRMLS_DC)
{
	register char *s, *t;
	char *end;

	Z_STRVAL_P(zendlval) = estrndup(yytext+1, yyleng-2);
	Z_STRLEN_P(zendlval) = yyleng-2;
	Z_TYPE_P(zendlval) = IS_STRING;
	HANDLE_NEWLINES(yytext, yyleng);

	/* convert escape sequences */
	s = t = Z_STRVAL_P(zendlval);
	end = s+Z_STRLEN_P(zendlval);
	while (s<end) {
		if (*s=='\\') {
			s++;
			if (s>=end) {
				continue;
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
				case '\\':
				case '$':
				case '"':
					*t++ = *s;
					Z_STRLEN_P(zendlval)--;
					break;
				default:
					/* check for an octal */
					if (ZEND_IS_OCT(*s)) {
						char octal_buf[4] = { 0, 0, 0, 0 };

						octal_buf[0] = *s;
						Z_STRLEN_P(zendlval)--;
						if ((s+1)<end && ZEND_IS_OCT(*(s+1))) {
							octal_buf[1] = *(++s);
							Z_STRLEN_P(zendlval)--;
							if ((s+1)<end && ZEND_IS_OCT(*(s+1))) {
								octal_buf[2] = *(++s);
								Z_STRLEN_P(zendlval)--;
							}
						}
						*t++ = (char) strtol(octal_buf, NULL, 8);
					} else if (*s=='x' && (s+1)<end && ZEND_IS_HEX(*(s+1))) {
						char hex_buf[3] = { 0, 0, 0};

						Z_STRLEN_P(zendlval)--; /* for the 'x' */

						hex_buf[0] = *(++s);
						Z_STRLEN_P(zendlval)--;
						if ((s+1)<end && ZEND_IS_HEX(*(s+1))) {
							hex_buf[1] = *(++s);
							Z_STRLEN_P(zendlval)--;
						}
						*t++ = (char) strtol(hex_buf, NULL, 16);
					} else {
						*t++ = '\\';
						*t++ = *s;
					}
					break;
			}
			s++;
		} else {
			*t++ = *s++;
		}
	}
	*t = 0;

	return T_CONSTANT_ENCAPSED_STRING;
}

int zend_scan_binary_single_string(zval *zendlval TSRMLS_DC)
{
	register char *s, *t;
	char *end;

	Z_STRVAL_P(zendlval) = estrndup(yytext+1, yyleng-2);
	Z_STRLEN_P(zendlval) = yyleng-2;
	Z_TYPE_P(zendlval) = IS_STRING;
	HANDLE_NEWLINES(yytext, yyleng);

	/* convert escape sequences */
	s = t = Z_STRVAL_P(zendlval);
	end = s+Z_STRLEN_P(zendlval);
	while (s<end) {
		if (*s=='\\') {
			s++;
			if (s>=end) {
				continue;
			}
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
			s++;
		} else {
			*t++ = *s++;
		}
	}
	*t = 0;

	return T_CONSTANT_ENCAPSED_STRING;
}

%}

LNUM	[0-9]+
DNUM	([0-9]*[\.][0-9]+)|([0-9]+[\.][0-9]*)
EXPONENT_DNUM	(({LNUM}|{DNUM})[eE][+-]?{LNUM})
HNUM	"0x"[0-9a-fA-F]+
LABEL	[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*
WHITESPACE [ \n\r\t]+
TABS_AND_SPACES [ \t]*
TOKENS [;:,.\[\]()|^&+-/*=%!~$<>?@]
ENCAPSED_TOKENS [\[\]{}$]
ESCAPED_AND_WHITESPACE [\n\t\r #'.:;,()|^&+-/*=%!~<>?@]+
ANY_CHAR (.|[\n])
NEWLINE ("\r"|"\n"|"\r\n")

%option noyylineno
%option noyywrap
%%

<ST_IN_SCRIPTING>"exit" {
	return T_EXIT;
}

<ST_IN_SCRIPTING>"die" {
	return T_EXIT;
}

<ST_IN_SCRIPTING>"function" {
	return T_FUNCTION;
}

<ST_IN_SCRIPTING>"const" {
	return T_CONST;
}

<ST_IN_SCRIPTING>"return" {
	return T_RETURN;
}

<ST_IN_SCRIPTING>"try" {
	return T_TRY;
}

<ST_IN_SCRIPTING>"catch" {
	return T_CATCH;
}

<ST_IN_SCRIPTING>"throw" {
	return T_THROW;
}

<ST_IN_SCRIPTING>"if" {
	return T_IF;
}

<ST_IN_SCRIPTING>"elseif" {
	return T_ELSEIF;
}

<ST_IN_SCRIPTING>"endif" {
	return T_ENDIF;
}

<ST_IN_SCRIPTING>"else" {
	return T_ELSE;
}

<ST_IN_SCRIPTING>"while" {
	return T_WHILE;
}

<ST_IN_SCRIPTING>"endwhile" {
	return T_ENDWHILE;
}

<ST_IN_SCRIPTING>"do" {
	return T_DO;
}

<ST_IN_SCRIPTING>"for" {
	return T_FOR;
}

<ST_IN_SCRIPTING>"endfor" {
	return T_ENDFOR;
}

<ST_IN_SCRIPTING>"foreach" {
	return T_FOREACH;
}

<ST_IN_SCRIPTING>"endforeach" {
	return T_ENDFOREACH;
}

<ST_IN_SCRIPTING>"declare" {
	return T_DECLARE;
}

<ST_IN_SCRIPTING>"enddeclare" {
	return T_ENDDECLARE;
}

<ST_IN_SCRIPTING>"instanceof" {
	return T_INSTANCEOF;
}

<ST_IN_SCRIPTING>"as" {
	return T_AS;
}

<ST_IN_SCRIPTING>"switch" {
	return T_SWITCH;
}

<ST_IN_SCRIPTING>"endswitch" {
	return T_ENDSWITCH;
}

<ST_IN_SCRIPTING>"case" {
	return T_CASE;
}

<ST_IN_SCRIPTING>"default" {
	return T_DEFAULT;
}

<ST_IN_SCRIPTING>"break" {
	return T_BREAK;
}

<ST_IN_SCRIPTING>"continue" {
	return T_CONTINUE;
}

<ST_IN_SCRIPTING>"goto" {
	return T_GOTO;
}

<ST_IN_SCRIPTING>"echo" {
	return T_ECHO;
}

<ST_IN_SCRIPTING>"print" {
	return T_PRINT;
}

<ST_IN_SCRIPTING>"class" {
	return T_CLASS;
}

<ST_IN_SCRIPTING>"interface" {
	return T_INTERFACE;
}

<ST_IN_SCRIPTING>"extends" {
	return T_EXTENDS;
}

<ST_IN_SCRIPTING>"implements" {
	return T_IMPLEMENTS;
}

<ST_IN_SCRIPTING,ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"->" {
	yy_push_state(ST_LOOKING_FOR_PROPERTY TSRMLS_CC);
	return T_OBJECT_OPERATOR;
}

<ST_LOOKING_FOR_PROPERTY>{LABEL} {
	yy_pop_state(TSRMLS_C);
	if (!zend_copy_string_value(zendlval, yytext, yyleng, UG(unicode)?IS_UNICODE:IS_STRING TSRMLS_CC)) {
		return 0;
	}
	if (UG(unicode) && !zend_check_and_normalize_identifier(zendlval)) {
		return 0;
	}
	return T_STRING;
}

<ST_LOOKING_FOR_PROPERTY>{ANY_CHAR} {
	yyless(0);
	yy_pop_state(TSRMLS_C);
}

<ST_IN_SCRIPTING>"::" {
	return T_PAAMAYIM_NEKUDOTAYIM;
}

<ST_IN_SCRIPTING>"new" {
	return T_NEW;
}

<ST_IN_SCRIPTING>"clone" {
	return T_CLONE;
}

<ST_IN_SCRIPTING>"var" {
	return T_VAR;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("int"|"integer"){TABS_AND_SPACES}")" {
	return T_INT_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("real"|"double"|"float"){TABS_AND_SPACES}")" {
	return T_DOUBLE_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}"string"{TABS_AND_SPACES}")" {
	return T_STRING_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}"unicode"{TABS_AND_SPACES}")" {
	return T_UNICODE_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}"binary"{TABS_AND_SPACES}")" {
	return T_BINARY_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}"array"{TABS_AND_SPACES}")" {
	return T_ARRAY_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}"object"{TABS_AND_SPACES}")" {
	return T_OBJECT_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("bool"|"boolean"){TABS_AND_SPACES}")" {
	return T_BOOL_CAST;
}

<ST_IN_SCRIPTING>"("{TABS_AND_SPACES}("unset"){TABS_AND_SPACES}")" {
	return T_UNSET_CAST;
}

<ST_IN_SCRIPTING>"eval" {
	return T_EVAL;
}

<ST_IN_SCRIPTING>"include" {
	return T_INCLUDE;
}

<ST_IN_SCRIPTING>"include_once" {
	return T_INCLUDE_ONCE;
}

<ST_IN_SCRIPTING>"require" {
	return T_REQUIRE;
}

<ST_IN_SCRIPTING>"require_once" {
	return T_REQUIRE_ONCE;
}

<ST_IN_SCRIPTING>"use" {
	return T_USE;
}

<ST_IN_SCRIPTING>"global" {
	return T_GLOBAL;
}

<ST_IN_SCRIPTING>"isset" {
	return T_ISSET;
}

<ST_IN_SCRIPTING>"empty" {
	return T_EMPTY;
}

<ST_IN_SCRIPTING>"__halt_compiler" {
	return T_HALT_COMPILER;
}

<ST_IN_SCRIPTING>"static" {
	return T_STATIC;
}

<ST_IN_SCRIPTING>"abstract" {
	return T_ABSTRACT;
}

<ST_IN_SCRIPTING>"final" {
	return T_FINAL;
}

<ST_IN_SCRIPTING>"private" {
	return T_PRIVATE;
}

<ST_IN_SCRIPTING>"protected" {
	return T_PROTECTED;
}

<ST_IN_SCRIPTING>"public" {
	return T_PUBLIC;
}

<ST_IN_SCRIPTING>"unset" {
	return T_UNSET;
}

<ST_IN_SCRIPTING>"=>" {
	return T_DOUBLE_ARROW;
}

<ST_IN_SCRIPTING>"list" {
	return T_LIST;
}

<ST_IN_SCRIPTING>"array" {
	return T_ARRAY;
}

<ST_IN_SCRIPTING>"++" {
	return T_INC;
}

<ST_IN_SCRIPTING>"--" {
	return T_DEC;
}

<ST_IN_SCRIPTING>"===" {
	return T_IS_IDENTICAL;
}

<ST_IN_SCRIPTING>"!==" {
	return T_IS_NOT_IDENTICAL;
}

<ST_IN_SCRIPTING>"==" {
	return T_IS_EQUAL;
}

<ST_IN_SCRIPTING>"!="|"<>" {
	return T_IS_NOT_EQUAL;
}

<ST_IN_SCRIPTING>"<=" {
	return T_IS_SMALLER_OR_EQUAL;
}

<ST_IN_SCRIPTING>">=" {
	return T_IS_GREATER_OR_EQUAL;
}

<ST_IN_SCRIPTING>"+=" {
	return T_PLUS_EQUAL;
}

<ST_IN_SCRIPTING>"-=" {
	return T_MINUS_EQUAL;
}

<ST_IN_SCRIPTING>"*=" {
	return T_MUL_EQUAL;
}

<ST_IN_SCRIPTING>"/=" {
	return T_DIV_EQUAL;
}

<ST_IN_SCRIPTING>".=" {
	return T_CONCAT_EQUAL;
}

<ST_IN_SCRIPTING>"%=" {
	return T_MOD_EQUAL;
}

<ST_IN_SCRIPTING>"<<=" {
	return T_SL_EQUAL;
}

<ST_IN_SCRIPTING>">>=" {
	return T_SR_EQUAL;
}

<ST_IN_SCRIPTING>"&=" {
	return T_AND_EQUAL;
}

<ST_IN_SCRIPTING>"|=" {
	return T_OR_EQUAL;
}

<ST_IN_SCRIPTING>"^=" {
	return T_XOR_EQUAL;
}

<ST_IN_SCRIPTING>"||" {
	return T_BOOLEAN_OR;
}

<ST_IN_SCRIPTING>"&&" {
	return T_BOOLEAN_AND;
}

<ST_IN_SCRIPTING>"OR" {
	return T_LOGICAL_OR;
}

<ST_IN_SCRIPTING>"AND" {
	return T_LOGICAL_AND;
}

<ST_IN_SCRIPTING>"XOR" {
	return T_LOGICAL_XOR;
}

<ST_IN_SCRIPTING>"<<" {
	return T_SL;
}

<ST_IN_SCRIPTING>">>" {
	return T_SR;
}

<ST_IN_SCRIPTING>{TOKENS} {
	return yytext[0];
}


<ST_IN_SCRIPTING>"{" {
	yy_push_state(ST_IN_SCRIPTING TSRMLS_CC);
	return '{';
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"${" {
	yy_push_state(ST_LOOKING_FOR_VARNAME TSRMLS_CC);
	return T_DOLLAR_OPEN_CURLY_BRACES;
}


<ST_IN_SCRIPTING>"}" {
	RESET_DOC_COMMENT();
	/* This is a temporary fix which is dependant on flex and it's implementation */
	if (yy_start_stack_ptr) {
		yy_pop_state(TSRMLS_C);
	}
	return '}';
}


<ST_LOOKING_FOR_VARNAME>{LABEL} {
	if (!zend_copy_string_value(zendlval, yytext, yyleng, UG(unicode)?IS_UNICODE:IS_STRING TSRMLS_CC)) {
		return 0;
	}
	if (UG(unicode) && !zend_check_and_normalize_identifier(zendlval)) {
		return 0;
	}
	yy_pop_state(TSRMLS_C);
	yy_push_state(ST_IN_SCRIPTING TSRMLS_CC);
	return T_STRING_VARNAME;
}


<ST_LOOKING_FOR_VARNAME>{ANY_CHAR} {
	yyless(0);
	yy_pop_state(TSRMLS_C);
	yy_push_state(ST_IN_SCRIPTING TSRMLS_CC);
}


<ST_IN_SCRIPTING>{LNUM} {
	errno = 0;
	Z_LVAL_P(zendlval) = strtol(yytext, NULL, 0);
	if (errno == ERANGE) { /* overflow */
		Z_DVAL_P(zendlval) = zend_strtod(yytext, NULL);
		Z_TYPE_P(zendlval) = IS_DOUBLE;
		return T_DNUMBER;
	} else {
		Z_TYPE_P(zendlval) = IS_LONG;
		return T_LNUMBER;
	}
}

<ST_IN_SCRIPTING>{HNUM} {
	errno = 0;
	Z_LVAL_P(zendlval) = strtoul(yytext, NULL, 16);
	if (errno == ERANGE) { /* overflow */
		/* not trying strtod - it returns trash on 0x-es */
		Z_LVAL_P(zendlval) = LONG_MAX; /* maximal long */
		zend_error(E_NOTICE,"Hex number is too big: %s", yytext);
	} else {
		if (Z_LVAL_P(zendlval) < 0) {
			/* maintain consistency with the old way */
			Z_DVAL_P(zendlval) = (unsigned long) Z_LVAL_P(zendlval);
			Z_TYPE_P(zendlval) = IS_DOUBLE;
			return T_DNUMBER;
		}
		Z_TYPE_P(zendlval) = IS_LONG;
	}
	Z_TYPE_P(zendlval) = IS_LONG;
	return T_LNUMBER;
}

<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>{LNUM}|{HNUM} { /* treat numbers (almost) as strings inside encapsulated strings */
	if (!zend_copy_string_value(zendlval, yytext, yyleng, CG(literal_type) TSRMLS_CC)) {
		return 0;
	}
	return T_NUM_STRING;
}

<ST_IN_SCRIPTING>{DNUM}|{EXPONENT_DNUM} {
	Z_DVAL_P(zendlval) = zend_strtod(yytext, NULL);
	Z_TYPE_P(zendlval) = IS_DOUBLE;
	return T_DNUMBER;
}

<ST_IN_SCRIPTING>"__CLASS__" {
	zstr class_name = NULL_ZSTR;

	if (CG(active_class_entry)) {
		class_name = CG(active_class_entry)->name;
	}

	if (!class_name.v) {
		ZVAL_EMPTY_TEXT(zendlval);
	} else {
		ZVAL_TEXT(zendlval, class_name, 1);
	}
	return T_CLASS_C;
}

<ST_IN_SCRIPTING>"__FUNCTION__" {
	zstr func_name = NULL_ZSTR;

	if (CG(active_op_array)) {
		func_name = CG(active_op_array)->function_name;
	}

	if (!func_name.v) {
		ZVAL_EMPTY_TEXT(zendlval);
	} else {
		ZVAL_TEXT(zendlval, func_name, 1);
	}
	return T_FUNC_C;
}

<ST_IN_SCRIPTING>"__METHOD__" {
	zstr class_name = CG(active_class_entry) ? CG(active_class_entry)->name : NULL_ZSTR;
	zstr func_name = CG(active_op_array)? CG(active_op_array)->function_name : NULL_ZSTR;
	size_t len = 0;

	if (UG(unicode)) {
		size_t len1;

		if (class_name.u) {
			len += len1 = u_strlen(class_name.u);
			len += 2;
		}
		if (func_name.u) {
			len += u_strlen(func_name.u);
		} else {
			func_name.u = EMPTY_STR;
		}
		Z_USTRLEN_P(zendlval) = len;
		Z_USTRVAL_P(zendlval) = eumalloc(len+1);
		if (class_name.u) {
			u_strcpy(Z_USTRVAL_P(zendlval), class_name.u);
			Z_USTRVAL_P(zendlval)[len1] = 0x3a;   /* ':' */
			Z_USTRVAL_P(zendlval)[len1+1] = 0x3a; /* ':' */
			Z_USTRVAL_P(zendlval)[len1+2] = 0;
		} else {
			Z_USTRVAL_P(zendlval)[0] = 0;
		}
		u_strcat(Z_USTRVAL_P(zendlval), func_name.u);
		Z_TYPE_P(zendlval) = IS_UNICODE;
	} else {
		if (class_name.s) {
			len += strlen(class_name.s) + 2;
		}
		if (func_name.s) {
			len += strlen(func_name.s);
		}

		Z_STRVAL_P(zendlval) = emalloc(len+1);
		Z_STRLEN_P(zendlval) = sprintf(Z_STRVAL_P(zendlval), "%s%s%s",
			class_name.s ? class_name.s : "",
			class_name.s && func_name.s ? "::" : "",
			func_name.s ? func_name.s : ""
			);
		Z_STRLEN_P(zendlval) = strlen(Z_STRVAL_P(zendlval));
		Z_TYPE_P(zendlval) = IS_STRING;
	}
	return T_METHOD_C;
}

<ST_IN_SCRIPTING>"__LINE__" {
	Z_LVAL_P(zendlval) = CG(zend_lineno);
	Z_TYPE_P(zendlval) = IS_LONG;
	return T_LINE;
}

<ST_IN_SCRIPTING>"__FILE__" {
	char *filename = zend_get_compiled_filename(TSRMLS_C);

	if (!filename) {
		filename = "";
	}
	ZVAL_RT_STRING(zendlval, filename, 1);
	return T_FILE;
}

<INITIAL>(([^<]|"<"[^?%s<]){1,400})|"<s"|"<" {
	Z_STRVAL_P(zendlval) = (char *) estrndup(yytext, yyleng);
	Z_STRLEN_P(zendlval) = yyleng;
	Z_TYPE_P(zendlval) = IS_STRING;
	HANDLE_NEWLINES(yytext, yyleng);
	return T_INLINE_HTML;
}

<INITIAL>"<?"|"<script"{WHITESPACE}+"language"{WHITESPACE}*"="{WHITESPACE}*("php"|"\"php\""|"\'php\'"){WHITESPACE}*">" {
	HANDLE_NEWLINES(yytext, yyleng);
	if (CG(short_tags) || yyleng>2) { /* yyleng>2 means it's not <? but <script> */
		Z_STRVAL_P(zendlval) = yytext; /* no copying - intentional */
		Z_STRLEN_P(zendlval) = yyleng;
		Z_TYPE_P(zendlval) = IS_STRING;
		BEGIN(ST_IN_SCRIPTING);
		return T_OPEN_TAG;
	} else {
		Z_STRVAL_P(zendlval) = (char *) estrndup(yytext, yyleng);
		Z_STRLEN_P(zendlval) = yyleng;
		Z_TYPE_P(zendlval) = IS_STRING;
		return T_INLINE_HTML;
	}
}


<INITIAL>"<%="|"<?=" {
	if ((yytext[1]=='%' && CG(asp_tags)) || (yytext[1]=='?' && CG(short_tags))) {
		Z_STRVAL_P(zendlval) = yytext; /* no copying - intentional */
		Z_STRLEN_P(zendlval) = yyleng;
		Z_TYPE_P(zendlval) = IS_STRING;
		BEGIN(ST_IN_SCRIPTING);
		return T_OPEN_TAG_WITH_ECHO;
	} else {
		Z_STRVAL_P(zendlval) = (char *) estrndup(yytext, yyleng);
		Z_STRLEN_P(zendlval) = yyleng;
		Z_TYPE_P(zendlval) = IS_STRING;
		return T_INLINE_HTML;
	}
}


<INITIAL>"<%" {
	if (CG(asp_tags)) {
		Z_STRVAL_P(zendlval) = yytext; /* no copying - intentional */
		Z_STRLEN_P(zendlval) = yyleng;
		Z_TYPE_P(zendlval) = IS_STRING;
		BEGIN(ST_IN_SCRIPTING);
		return T_OPEN_TAG;
	} else {
		Z_STRVAL_P(zendlval) = (char *) estrndup(yytext, yyleng);
		Z_STRLEN_P(zendlval) = yyleng;
		Z_TYPE_P(zendlval) = IS_STRING;
		return T_INLINE_HTML;
	}
}


<INITIAL>"<?php"([ \t]|{NEWLINE}) {
	Z_STRVAL_P(zendlval) = yytext; /* no copying - intentional */
	Z_STRLEN_P(zendlval) = yyleng;
	Z_TYPE_P(zendlval) = IS_STRING;
	HANDLE_NEWLINE(yytext[yyleng-1]);
	BEGIN(ST_IN_SCRIPTING);
	return T_OPEN_TAG;
}

<ST_IN_SCRIPTING,ST_DOUBLE_QUOTES,ST_HEREDOC,ST_BACKQUOTE>"$"{LABEL} {
	if (!zend_copy_string_value(zendlval, (yytext+1), (yyleng-1), UG(unicode)?IS_UNICODE:IS_STRING TSRMLS_CC)) {
		return 0;
	}
	if (UG(unicode) && !zend_check_and_normalize_identifier(zendlval)) {
		return 0;
	}
	return T_VARIABLE;
}

<ST_IN_SCRIPTING>{LABEL} {
	if (!zend_copy_string_value(zendlval, yytext, yyleng, UG(unicode)?IS_UNICODE:IS_STRING TSRMLS_CC)) {
		return 0;
	}
	if (UG(unicode) && !zend_check_and_normalize_identifier(zendlval)) {
		return 0;
	}
	return T_STRING;
}

<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>{LABEL} {
	if (!zend_copy_string_value(zendlval, yytext, yyleng, CG(literal_type) TSRMLS_CC)) {
		return 0;
	}
	return T_STRING;
}


<ST_IN_SCRIPTING>{WHITESPACE} {
	Z_STRVAL_P(zendlval) = yytext; /* no copying - intentional */
	Z_STRLEN_P(zendlval) = yyleng;
	Z_TYPE_P(zendlval) = IS_STRING;
	HANDLE_NEWLINES(yytext, yyleng);
	return T_WHITESPACE;
}


<ST_IN_SCRIPTING>"#"|"//" {
	BEGIN(ST_ONE_LINE_COMMENT);
	yymore();
}

<ST_ONE_LINE_COMMENT>"?"|"%"|">" {
	yymore();
}

<ST_ONE_LINE_COMMENT>[^\n\r?%>]*{ANY_CHAR} {
	switch (yytext[yyleng-1]) {
		case '?': case '%': case '>':
			yyless(yyleng-1);
			yymore();
			break;
		case '\n':
			CG(zend_lineno)++;
			/* intentional fall through */
		default:
			Z_STRVAL_P(zendlval) = yytext; /* no copying - intentional */
			Z_STRLEN_P(zendlval) = yyleng;
			Z_TYPE_P(zendlval) = IS_STRING;
			BEGIN(ST_IN_SCRIPTING);
			return T_COMMENT;
	}
}

<ST_ONE_LINE_COMMENT>{NEWLINE} {
	Z_STRVAL_P(zendlval) = yytext; /* no copying - intentional */
	Z_STRLEN_P(zendlval) = yyleng;
	Z_TYPE_P(zendlval) = IS_STRING;
	BEGIN(ST_IN_SCRIPTING);
	CG(zend_lineno)++;
	return T_COMMENT;
}

<ST_ONE_LINE_COMMENT>"?>"|"%>" {
    if (CG(asp_tags) || yytext[yyleng-2] != '%') { /* asp comment? */
		Z_STRVAL_P(zendlval) = yytext; /* no copying - intentional */
		Z_STRLEN_P(zendlval) = yyleng-2;
		Z_TYPE_P(zendlval) = IS_STRING;
		yyless(yyleng-2);
		BEGIN(ST_IN_SCRIPTING);
		return T_COMMENT;
	} else {
		yymore();
	}
}

<ST_IN_SCRIPTING>"/**"{WHITESPACE} {
	CG(comment_start_line) = CG(zend_lineno);
	RESET_DOC_COMMENT();
	BEGIN(ST_DOC_COMMENT);
	yymore();
}

<ST_IN_SCRIPTING>"/*" {
	CG(comment_start_line) = CG(zend_lineno);
	BEGIN(ST_COMMENT);
	yymore();
}


<ST_COMMENT,ST_DOC_COMMENT>[^*]+ {
	yymore();
}

<ST_DOC_COMMENT>"*/" {
	CG(doc_comment) = estrndup(yytext, yyleng);
	CG(doc_comment_len) = yyleng;
	HANDLE_NEWLINES(yytext, yyleng);
	BEGIN(ST_IN_SCRIPTING);
	return T_DOC_COMMENT;
}

<ST_COMMENT>"*/" {
	HANDLE_NEWLINES(yytext, yyleng);
	BEGIN(ST_IN_SCRIPTING);
	return T_COMMENT;
}

<ST_COMMENT,ST_DOC_COMMENT>"*" {
	yymore();
}

<ST_IN_SCRIPTING>("?>"|"</script"{WHITESPACE}*">"){NEWLINE}? {
	Z_STRVAL_P(zendlval) = yytext; /* no copying - intentional */
	Z_STRLEN_P(zendlval) = yyleng;
	Z_TYPE_P(zendlval) = IS_STRING;
	BEGIN(INITIAL);
	return T_CLOSE_TAG;  /* implicit ';' at php-end tag */
}


<ST_IN_SCRIPTING>"%>"{NEWLINE}? {
	if (CG(asp_tags)) {
		BEGIN(INITIAL);
		Z_STRLEN_P(zendlval) = yyleng;
		Z_TYPE_P(zendlval) = IS_STRING;
		Z_STRVAL_P(zendlval) = yytext; /* no copying - intentional */
		return T_CLOSE_TAG;  /* implicit ';' at php-end tag */
	} else {
		yyless(1);
		return yytext[0];
	}
}


<ST_IN_SCRIPTING>(["]([^$"\\]|("\\".))*["]) {
	if (UG(unicode)) {
		return zend_scan_unicode_double_string(zendlval TSRMLS_CC);
	} else {
		return zend_scan_binary_double_string(zendlval TSRMLS_CC);
	}
}


<ST_IN_SCRIPTING>(b["]([^$"\\]|("\\".))*["]) {
	yytext++; /* adjust for 'b' */
	yyleng--;
	return zend_scan_binary_double_string(zendlval TSRMLS_CC);
}


<ST_IN_SCRIPTING>([']([^'\\]|("\\".))*[']) {
	if (UG(unicode)) {
		return zend_scan_unicode_single_string(zendlval TSRMLS_CC);
	} else {
		return zend_scan_binary_single_string(zendlval TSRMLS_CC);
	}
}


<ST_IN_SCRIPTING>("b'"([^'\\]|("\\".))*[']) {
	yytext++; /* adjust for 'b' */
	yyleng--;
	return zend_scan_binary_single_string(zendlval TSRMLS_CC);
}


<ST_IN_SCRIPTING>["] {
	BEGIN(ST_DOUBLE_QUOTES);
	return '\"';
}

<ST_IN_SCRIPTING>b["] {
	BEGIN(ST_DOUBLE_QUOTES);
	return T_BINARY_DOUBLE;
}

<ST_IN_SCRIPTING>b"<<<"{TABS_AND_SPACES}{LABEL}{NEWLINE} {
	char *s;
	CG(zend_lineno)++;
	CG(heredoc_len) = yyleng-4-1-(yytext[yyleng-2]=='\r'?1:0);
	s = yytext+4;
	while ((*s == ' ') || (*s == '\t')) {
		s++;
		CG(heredoc_len)--;
	}
	CG(heredoc) = estrndup(s, CG(heredoc_len));
	BEGIN(ST_HEREDOC);
	return T_BINARY_HEREDOC;
}


<ST_IN_SCRIPTING>"<<<"{TABS_AND_SPACES}{LABEL}{NEWLINE} {
	char *s;
	CG(zend_lineno)++;
	CG(heredoc_len) = yyleng-3-1-(yytext[yyleng-2]=='\r'?1:0);
	s = yytext+3;
	while ((*s == ' ') || (*s == '\t')) {
		s++;
		CG(heredoc_len)--;
	}
	CG(heredoc) = estrndup(s, CG(heredoc_len));
	BEGIN(ST_HEREDOC);
	return T_START_HEREDOC;
}


<ST_IN_SCRIPTING>[`] {
	BEGIN(ST_BACKQUOTE);
	return '`';
}


<ST_HEREDOC>^{LABEL}(";")?{NEWLINE} {
	int label_len;

	if (yytext[yyleng-2]=='\r') {
		label_len = yyleng-2;
	} else {
		label_len = yyleng-1;
	}

	if (yytext[label_len-1]==';') {
		label_len--;
	}

	if (label_len==CG(heredoc_len) && !memcmp(yytext, CG(heredoc), label_len)) {
		Z_STRVAL_P(zendlval) = estrndup(yytext, label_len); /* unput destroys yytext */
		Z_STRLEN_P(zendlval) = label_len;
		yyless(yyleng - (yyleng - label_len));
		efree(CG(heredoc));
		CG(heredoc)=NULL;
		CG(heredoc_len)=0;
		BEGIN(ST_IN_SCRIPTING);
		return T_END_HEREDOC;
	} else {
		CG(zend_lineno)++;
		if (!zend_copy_string_value(zendlval, yytext, yyleng, CG(literal_type) TSRMLS_CC)) {
			return 0;
		}
		return T_STRING;
	}
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>{ESCAPED_AND_WHITESPACE} {
	HANDLE_NEWLINES(yytext, yyleng);
	if (!zend_copy_string_value(zendlval, yytext, yyleng, CG(literal_type) TSRMLS_CC)) {
		return 0;
	}
	return T_ENCAPSED_AND_WHITESPACE;
}

<ST_DOUBLE_QUOTES>[`]+ {
	if (!zend_copy_string_value(zendlval, yytext, yyleng, CG(literal_type) TSRMLS_CC)) {
		return 0;
	}
	return T_ENCAPSED_AND_WHITESPACE;
}


<ST_BACKQUOTE>["]+ {
	if (!zend_copy_string_value(zendlval, yytext, yyleng, CG(literal_type) TSRMLS_CC)) {
		return 0;
	}
	return T_ENCAPSED_AND_WHITESPACE;
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"$"[^a-zA-Z_\x7f-\xff{] {
	Z_LVAL_P(zendlval) = (long) yytext[0];
	if (yyleng == 2) {
		yyless(1);
	}
	return T_CHARACTER;
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>{ENCAPSED_TOKENS} {
	Z_LVAL_P(zendlval) = (long) yytext[0];
	return yytext[0];
}

<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"{$" {
	Z_LVAL_P(zendlval) = (long) yytext[0];
	yy_push_state(ST_IN_SCRIPTING TSRMLS_CC);
	yyless(1);
	return T_CURLY_OPEN;
}


<ST_DOUBLE_QUOTES>"\\\"" {
	Z_LVAL_P(zendlval) = (long) '"';
	return T_CHARACTER;
}

<ST_BACKQUOTE>"\\`" {
	Z_LVAL_P(zendlval) = (long) '`';
	return T_CHARACTER;
}

<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"\\"[0-7]{1,3} {
	Z_LVAL_P(zendlval) = strtol(yytext+1, NULL, 8);
	return T_CHARACTER;
}

<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"\\x"[0-9A-Fa-f]{1,2} {
	Z_LVAL_P(zendlval) = strtol (yytext+2, NULL, 16);
	return T_CHARACTER;
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"\\u"[0-9A-Fa-f]{0,6} {
	UChar32 codepoint;
	int req_digits = (yytext[1] == 'U') ? 6 : 4;

	if (CG(literal_type) == IS_UNICODE) {
		if (zend_digits_to_codepoint(yytext+2, yytext+yyleng, &codepoint, req_digits)) {
			if (codepoint <= 0x10FFFF) {
				Z_LVAL_P(zendlval) = (long) codepoint;
				/* give back if we grabbed more than needed for \u case */
				if (yyleng > req_digits + 2) {
					yyless(req_digits + 2);
				}
				return T_CHARACTER;
			} else {
				zend_error(E_COMPILE_WARNING,"\\U%06x is above the highest valid codepoint 0x10FFFF", codepoint);
				return 0;
			}
		} else {
			zend_error(E_COMPILE_WARNING,"\\%c escape sequence requires exactly %d hexadecimal digits", yytext[1], req_digits);
			return 0;
		}
	} else {
		zend_copy_string_value(zendlval, yytext, yyleng, CG(literal_type) TSRMLS_CC);
		return T_STRING;
	}
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"\\C"("{"[A-Z0-9 -]+"}")? {
	UChar32 codepoint;

	if (CG(literal_type) == IS_UNICODE && (yytext[1] == 'C')) {
		/* minimum valid string is \C{.} */
		if (yyleng >= 5) {
			/* safe, since we have } at the end */
			yytext[yyleng-1] = 0;
			if (zend_uchar_from_name(yytext+3, &codepoint)) {
				Z_LVAL_P(zendlval) = (long) codepoint;
				return T_CHARACTER;
			} else {
				zend_error(E_COMPILE_WARNING, "Invalid Unicode character name: '%s'", yytext+3);
				return 0;
			}
		} else {
			zend_error(E_COMPILE_WARNING, "Invalid \\C{..} sequence");
			return 0;
		}
	} else {
		zend_copy_string_value(zendlval, yytext, yyleng, CG(literal_type) TSRMLS_CC);
		return T_STRING;
	}
}

<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"\\{" {
	if (!zend_copy_string_value(zendlval, yytext, yyleng, CG(literal_type) TSRMLS_CC)) {
		return 0;
	}
	return T_STRING;
}

<ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>"\\"{ANY_CHAR} {
	switch (yytext[1]) {
		case 'n':
			Z_LVAL_P(zendlval) = (long) '\n';
			break;
		case 't':
			Z_LVAL_P(zendlval) = (long) '\t';
			break;
		case 'r':
			Z_LVAL_P(zendlval) = (long) '\r';
			break;
		case '\\':
			Z_LVAL_P(zendlval) = (long) '\\';
			break;
		case '$':
			Z_LVAL_P(zendlval) = (long) yytext[1];
			break;
		default:
			if (!zend_copy_string_value(zendlval, yytext, yyleng, CG(literal_type) TSRMLS_CC)) {
				return 0;
			}
			return T_BAD_CHARACTER;
			break;
	}
	return T_CHARACTER;
}


<ST_HEREDOC>["'`]+ {
	if (!zend_copy_string_value(zendlval, yytext, yyleng, CG(literal_type) TSRMLS_CC)) {
		return 0;
	}
	return T_ENCAPSED_AND_WHITESPACE;
}


<ST_DOUBLE_QUOTES>["] {
	BEGIN(ST_IN_SCRIPTING);
	return '\"';
}


<ST_BACKQUOTE>[`] {
	BEGIN(ST_IN_SCRIPTING);
	return '`';
}


<ST_DOUBLE_QUOTES,ST_BACKQUOTE,INITIAL,ST_IN_SCRIPTING,ST_LOOKING_FOR_PROPERTY><<EOF>> {
	return 0;
}

<ST_COMMENT,ST_DOC_COMMENT><<EOF>> {
	zend_error(E_COMPILE_WARNING,"Unterminated comment starting line %d", CG(comment_start_line));
	return 0;
}



<ST_IN_SCRIPTING,INITIAL,ST_DOUBLE_QUOTES,ST_BACKQUOTE,ST_HEREDOC>{ANY_CHAR} {
	zend_error(E_COMPILE_WARNING,"Unexpected character in input:  '%c' (ASCII=%d) state=%d", yytext[0], yytext[0], YYSTATE);
}
