/*
   +----------------------------------------------------------------------+
   | PHP Version 7														  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,	  |
   | that is bundled with this package in the file LICENSE, and is		  |
   | available through the world-wide-web at the following url:			  |
   | http://www.php.net/license/3_01.txt								  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to		  |
   | license@php.net so we can mail you a copy immediately.				  |
   +----------------------------------------------------------------------+
   | Author: Ed Batutis <ed@batutis.com>								  |
   +----------------------------------------------------------------------+
 */

/* {{{ includes */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include "grapheme.h"
#include "grapheme_util.h"

#include <unicode/utypes.h>
#if U_ICU_VERSION_MAJOR_NUM >= 49
#include <unicode/utf8.h>
#endif
#include <unicode/ucol.h>
#include <unicode/ustring.h>
#include <unicode/ubrk.h>

#include "ext/standard/php_string.h"

/* }}} */

#define GRAPHEME_EXTRACT_TYPE_COUNT		0
#define GRAPHEME_EXTRACT_TYPE_MAXBYTES	1
#define GRAPHEME_EXTRACT_TYPE_MAXCHARS	2
#define GRAPHEME_EXTRACT_TYPE_MIN	GRAPHEME_EXTRACT_TYPE_COUNT
#define GRAPHEME_EXTRACT_TYPE_MAX	GRAPHEME_EXTRACT_TYPE_MAXCHARS


/* {{{ grapheme_register_constants
 * Register API constants
 */
void grapheme_register_constants( INIT_FUNC_ARGS )
{
	REGISTER_LONG_CONSTANT("GRAPHEME_EXTR_COUNT", GRAPHEME_EXTRACT_TYPE_COUNT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GRAPHEME_EXTR_MAXBYTES", GRAPHEME_EXTRACT_TYPE_MAXBYTES, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("GRAPHEME_EXTR_MAXCHARS", GRAPHEME_EXTRACT_TYPE_MAXCHARS, CONST_CS | CONST_PERSISTENT);
}
/* }}} */

/* {{{ proto size_t grapheme_strlen(string str)
   Get number of graphemes in a string */
PHP_FUNCTION(grapheme_strlen)
{
	char* string;
	size_t string_len;
	UChar* ustring = NULL;
	int ustring_len = 0;
	zend_long ret_len;
	UErrorCode status;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &string, &string_len) == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_strlen: unable to parse input param", 0 );
		RETURN_FALSE;
	}

	ret_len = grapheme_ascii_check((unsigned char *)string, string_len);

	if ( ret_len >= 0 )
		RETURN_LONG(string_len);

	/* convert the string to UTF-16. */
	status = U_ZERO_ERROR;
	intl_convert_utf8_to_utf16(&ustring, &ustring_len, string, string_len, &status );

	if ( U_FAILURE( status ) ) {
		/* Set global error code. */
		intl_error_set_code( NULL, status );

		/* Set error messages. */
		intl_error_set_custom_msg( NULL, "Error converting input string to UTF-16", 0 );
		if (ustring) {
			efree( ustring );
		}
		RETURN_NULL();
	}

	ret_len = grapheme_split_string(ustring, ustring_len, NULL, 0 );

	if (ustring) {
		efree( ustring );
	}

	if (ret_len >= 0) {
		RETVAL_LONG(ret_len);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto int grapheme_strpos(string haystack, string needle [, int offset ])
   Find position of first occurrence of a string within another */
PHP_FUNCTION(grapheme_strpos)
{
	char *haystack, *needle;
	size_t haystack_len, needle_len;
	const char *found;
	zend_long loffset = 0;
	int32_t offset = 0;
	size_t noffset = 0;
	zend_long ret_pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|l", &haystack, &haystack_len, &needle, &needle_len, &loffset) == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_strpos: unable to parse input param", 0 );
		RETURN_FALSE;
	}

	if ( OUTSIDE_STRING(loffset, haystack_len) ) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Offset not contained in string", 1 );
		RETURN_FALSE;
	}

	/* we checked that it will fit: */
	offset = (int32_t) loffset;
	noffset = offset >= 0 ? offset : (int32_t)haystack_len + offset;

	/* the offset is 'grapheme count offset' so it still might be invalid - we'll check it later */

	if (needle_len == 0) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Empty delimiter", 1 );
		RETURN_FALSE;
	}

	if (offset >= 0) {
		/* quick check to see if the string might be there
		 * I realize that 'offset' is 'grapheme count offset' but will work in spite of that
		*/
		found = php_memnstr(haystack + noffset, needle, needle_len, haystack + haystack_len);

		/* if it isn't there the we are done */
		if (!found) {
			RETURN_FALSE;
		}

		/* if it is there, and if the haystack is ascii, we are all done */
		if ( grapheme_ascii_check((unsigned char *)haystack, haystack_len) >= 0 ) {
			RETURN_LONG(found - haystack);
		}
	}

	/* do utf16 part of the strpos */
	ret_pos = grapheme_strpos_utf16(haystack, haystack_len, needle, needle_len, offset, NULL, 0 /* fIgnoreCase */, 0 /* last */ );

	if ( ret_pos >= 0 ) {
		RETURN_LONG(ret_pos);
	} else {
		RETURN_FALSE;
	}

}
/* }}} */

/* {{{ proto int grapheme_stripos(string haystack, string needle [, int offset ])
   Find position of first occurrence of a string within another, ignoring case differences */
PHP_FUNCTION(grapheme_stripos)
{
	char *haystack, *needle;
	size_t haystack_len, needle_len;
	const char *found;
	zend_long loffset = 0;
	int32_t offset = 0;
	zend_long ret_pos;
	int is_ascii;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|l", &haystack, &haystack_len, &needle, &needle_len, &loffset) == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_stripos: unable to parse input param", 0 );
		RETURN_FALSE;
	}

	if ( OUTSIDE_STRING(loffset, haystack_len) ) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_stripos: Offset not contained in string", 1 );
		RETURN_FALSE;
	}

	/* we checked that it will fit: */
	offset = (int32_t) loffset;

	/* the offset is 'grapheme count offset' so it still might be invalid - we'll check it later */

	if (needle_len == 0) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_stripos: Empty delimiter", 1 );
		RETURN_FALSE;
	}

	is_ascii = ( grapheme_ascii_check((unsigned char*)haystack, haystack_len) >= 0 );

	if ( is_ascii ) {
		char *haystack_dup, *needle_dup;
		int32_t noffset = offset >= 0 ? offset : (int32_t)haystack_len + offset;
		needle_dup = estrndup(needle, needle_len);
		php_strtolower(needle_dup, needle_len);
		haystack_dup = estrndup(haystack, haystack_len);
		php_strtolower(haystack_dup, haystack_len);

		found = php_memnstr(haystack_dup + noffset, needle_dup, needle_len, haystack_dup + haystack_len);

		efree(haystack_dup);
		efree(needle_dup);

		if (found) {
			RETURN_LONG(found - haystack_dup);
		}

		/* if needle was ascii too, we are all done, otherwise we need to try using Unicode to see what we get */
		if ( grapheme_ascii_check((unsigned char *)needle, needle_len) >= 0 ) {
			RETURN_FALSE;
		}
	}

	/* do utf16 part of the strpos */
	ret_pos = grapheme_strpos_utf16(haystack, haystack_len, needle, needle_len, offset, NULL, 1 /* fIgnoreCase */, 0 /*last */ );

	if ( ret_pos >= 0 ) {
		RETURN_LONG(ret_pos);
	} else {
		RETURN_FALSE;
	}

}
/* }}} */

/* {{{ proto int grapheme_strrpos(string haystack, string needle [, int offset])
   Find position of last occurrence of a string within another */
PHP_FUNCTION(grapheme_strrpos)
{
	char *haystack, *needle;
	size_t haystack_len, needle_len;
	zend_long loffset = 0;
	int32_t offset = 0;
	zend_long ret_pos;
	int is_ascii;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|l", &haystack, &haystack_len, &needle, &needle_len, &loffset) == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_strrpos: unable to parse input param", 0 );
		RETURN_FALSE;
	}

	if ( OUTSIDE_STRING(loffset, haystack_len) ) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Offset not contained in string", 1 );
		RETURN_FALSE;
	}

	/* we checked that it will fit: */
	offset = (int32_t) loffset;

	/* the offset is 'grapheme count offset' so it still might be invalid - we'll check it later */

	if (needle_len == 0) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Empty delimiter", 1 );
		RETURN_FALSE;
	}

	is_ascii = grapheme_ascii_check((unsigned char *)haystack, haystack_len) >= 0;

	if ( is_ascii ) {

		ret_pos = grapheme_strrpos_ascii(haystack, haystack_len, needle, needle_len, offset);

		if ( ret_pos >= 0 ) {
			RETURN_LONG(ret_pos);
		}

		/* if the needle was ascii too, we are done */

		if (  grapheme_ascii_check((unsigned char *)needle, needle_len) >= 0 ) {
			RETURN_FALSE;
		}

		/* else we need to continue via utf16 */
	}

	ret_pos = grapheme_strpos_utf16(haystack, haystack_len, needle, needle_len, offset, NULL, 0 /* f_ignore_case */, 1/* last */);

	if ( ret_pos >= 0 ) {
		RETURN_LONG(ret_pos);
	} else {
		RETURN_FALSE;
	}


}
/* }}} */

/* {{{ proto int grapheme_strripos(string haystack, string needle [, int offset])
   Find position of last occurrence of a string within another, ignoring case */
PHP_FUNCTION(grapheme_strripos)
{
	char *haystack, *needle;
	size_t haystack_len, needle_len;
	zend_long loffset = 0;
	int32_t offset = 0;
	zend_long ret_pos;
	int is_ascii;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|l", &haystack, &haystack_len, &needle, &needle_len, &loffset) == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_strrpos: unable to parse input param", 0 );
		RETURN_FALSE;
	}

	if ( OUTSIDE_STRING(loffset, haystack_len) ) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Offset not contained in string", 1 );
		RETURN_FALSE;
	}

	/* we checked that it will fit: */
	offset = (int32_t) loffset;

	/* the offset is 'grapheme count offset' so it still might be invalid - we'll check it later */

	if (needle_len == 0) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Empty delimiter", 1 );
		RETURN_FALSE;
	}

	is_ascii = grapheme_ascii_check((unsigned char *)haystack, haystack_len) >= 0;

	if ( is_ascii ) {
		char *needle_dup, *haystack_dup;

		needle_dup = estrndup(needle, needle_len);
		php_strtolower(needle_dup, needle_len);
		haystack_dup = estrndup(haystack, haystack_len);
		php_strtolower(haystack_dup, haystack_len);

		ret_pos = grapheme_strrpos_ascii(haystack_dup, haystack_len, needle_dup, needle_len, offset);

		efree(haystack_dup);
		efree(needle_dup);

		if ( ret_pos >= 0 ) {
			RETURN_LONG(ret_pos);
		}

		/* if the needle was ascii too, we are done */

		if (  grapheme_ascii_check((unsigned char *)needle, needle_len) >= 0 ) {
			RETURN_FALSE;
		}

		/* else we need to continue via utf16 */
	}

	ret_pos = grapheme_strpos_utf16(haystack, haystack_len, needle, needle_len, offset, NULL,  1 /* f_ignore_case */, 1 /*last */);

	if ( ret_pos >= 0 ) {
		RETURN_LONG(ret_pos);
	} else {
		RETURN_FALSE;
	}


}
/* }}} */

/* {{{ proto string grapheme_substr(string str, int start [, int length])
   Returns part of a string */
PHP_FUNCTION(grapheme_substr)
{
	char *str;
	zend_string *u8_sub_str;
	UChar *ustr;
	size_t str_len;
	int32_t ustr_len;
	zend_long lstart = 0, length = 0;
	int32_t start = 0;
	int iter_val;
	UErrorCode status;
	unsigned char u_break_iterator_buffer[U_BRK_SAFECLONE_BUFFERSIZE];
	UBreakIterator* bi = NULL;
	int sub_str_start_pos, sub_str_end_pos;
	int32_t (*iter_func)(UBreakIterator *);
	zend_bool no_length = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl|l!", &str, &str_len, &lstart, &length, &no_length) == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_substr: unable to parse input param", 0 );
		RETURN_FALSE;
	}

	if ( OUTSIDE_STRING(lstart, str_len)) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_substr: start not contained in string", 1 );
		RETURN_FALSE;
	}

	/* we checked that it will fit: */
	start = (int32_t) lstart;

	if(no_length) {
		length = str_len;
	}

	if(length < INT32_MIN) {
		length = INT32_MIN;
	} else if(length > INT32_MAX) {
		length = INT32_MAX;
	}

	/* the offset is 'grapheme count offset' so it still might be invalid - we'll check it later */

	if ( grapheme_ascii_check((unsigned char *)str, str_len) >= 0 ) {
		int32_t asub_str_len;
		char *sub_str;
		grapheme_substr_ascii(str, str_len, start, (int32_t)length, &sub_str, &asub_str_len);

		if ( NULL == sub_str ) {
			intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_substr: invalid parameters", 1 );
			RETURN_FALSE;
		}

		RETURN_STRINGL(sub_str, asub_str_len);
	}

	ustr = NULL;
	ustr_len = 0;
	status = U_ZERO_ERROR;
	intl_convert_utf8_to_utf16(&ustr, &ustr_len, str, str_len, &status);

	if ( U_FAILURE( status ) ) {
		/* Set global error code. */
		intl_error_set_code( NULL, status );

		/* Set error messages. */
		intl_error_set_custom_msg( NULL, "Error converting input string to UTF-16", 0 );
		if (ustr) {
			efree( ustr );
		}
		RETURN_FALSE;
	}

	bi = grapheme_get_break_iterator((void*)u_break_iterator_buffer, &status );

	if( U_FAILURE(status) ) {
		RETURN_FALSE;
	}

	ubrk_setText(bi, ustr, ustr_len,	&status);

	if ( start < 0 ) {
		iter_func = ubrk_previous;
		ubrk_last(bi);
		iter_val = 1;
	}
	else {
		iter_func = ubrk_next;
		iter_val = -1;
	}

	sub_str_start_pos = 0;

	while ( start ) {
		sub_str_start_pos = iter_func(bi);

		if ( UBRK_DONE == sub_str_start_pos ) {
			break;
		}

		start += iter_val;
	}

	if ( 0 != start || sub_str_start_pos >= ustr_len ) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_substr: start not contained in string", 1 );

		if (ustr) {
			efree(ustr);
		}
		ubrk_close(bi);
		RETURN_FALSE;
	}

	/* OK to convert here since if str_len were big, convert above would fail */
	if (length >= (int32_t)str_len) {

		/* no length supplied or length is too big, return the rest of the string */

		status = U_ZERO_ERROR;
		u8_sub_str = intl_convert_utf16_to_utf8(ustr + sub_str_start_pos, ustr_len - sub_str_start_pos, &status);

		if (ustr) {
			efree( ustr );
		}
		ubrk_close( bi );

		if ( !u8_sub_str ) {
			/* Set global error code. */
			intl_error_set_code( NULL, status );

			/* Set error messages. */
			intl_error_set_custom_msg( NULL, "Error converting output string to UTF-8", 0 );

			RETURN_FALSE;
		}

		/* return the allocated string, not a duplicate */
		RETVAL_NEW_STR(u8_sub_str);
		return;
	}

	if(length == 0) {
		/* empty length - we've validated start, we can return "" now */
		if (ustr) {
			efree(ustr);
		}
		ubrk_close(bi);
		RETURN_EMPTY_STRING();
	}

	/* find the end point of the string to return */

	if ( length < 0 ) {
		iter_func = ubrk_previous;
		ubrk_last(bi);
		iter_val = 1;
	}
	else {
		iter_func = ubrk_next;
		iter_val = -1;
	}

	sub_str_end_pos = 0;

	while ( length ) {
		sub_str_end_pos = iter_func(bi);

		if ( UBRK_DONE == sub_str_end_pos ) {
			break;
		}

		length += iter_val;
	}

	ubrk_close(bi);

	if ( UBRK_DONE == sub_str_end_pos) {
		if(length < 0) {
			intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_substr: length not contained in string", 1 );

			efree(ustr);
			RETURN_FALSE;
		} else {
			sub_str_end_pos = ustr_len;
		}
	}

	if(sub_str_start_pos > sub_str_end_pos) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_substr: length is beyond start", 1 );

		efree(ustr);
		RETURN_FALSE;
	}

	status = U_ZERO_ERROR;
	u8_sub_str = intl_convert_utf16_to_utf8(ustr + sub_str_start_pos, ( sub_str_end_pos - sub_str_start_pos ), &status);

	efree( ustr );

	if ( !u8_sub_str ) {
		/* Set global error code. */
		intl_error_set_code( NULL, status );

		/* Set error messages. */
		intl_error_set_custom_msg( NULL, "Error converting output string to UTF-8", 0 );

		RETURN_FALSE;
	}

	 /* return the allocated string, not a duplicate */
	RETVAL_NEW_STR(u8_sub_str);
}
/* }}} */

/* {{{	strstr_common_handler */
static void strstr_common_handler(INTERNAL_FUNCTION_PARAMETERS, int f_ignore_case)
{
	char *haystack, *needle;
	const char *found;
	size_t haystack_len, needle_len;
	int32_t ret_pos, uchar_pos;
	zend_bool part = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|b", &haystack, &haystack_len, &needle, &needle_len, &part) == FAILURE) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_strstr: unable to parse input param", 0 );

		RETURN_FALSE;
	}

	if (needle_len == 0) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Empty delimiter", 1 );

		RETURN_FALSE;
	}


	if ( !f_ignore_case ) {

		/* ASCII optimization: quick check to see if the string might be there
		 * I realize that 'offset' is 'grapheme count offset' but will work in spite of that
		*/
		found = php_memnstr(haystack, needle, needle_len, haystack + haystack_len);

		/* if it isn't there the we are done */
		if ( !found ) {
			RETURN_FALSE;
		}

		/* if it is there, and if the haystack is ascii, we are all done */
		if ( grapheme_ascii_check((unsigned char *)haystack, haystack_len) >= 0 ) {
			size_t found_offset = found - haystack;

			if (part) {
				RETURN_STRINGL(haystack, found_offset);
			} else {
				RETURN_STRINGL(found, haystack_len - found_offset);
			}
		}

	}

	/* need to work in utf16 */
	ret_pos = grapheme_strpos_utf16(haystack, haystack_len, needle, needle_len, 0, &uchar_pos, f_ignore_case, 0 /*last */ );

	if ( ret_pos < 0 ) {
		RETURN_FALSE;
	}

	/* uchar_pos is the 'nth' Unicode character position of the needle */

	ret_pos = 0;
	U8_FWD_N(haystack, ret_pos, haystack_len, uchar_pos);

	if (part) {
		RETURN_STRINGL(haystack, ret_pos);
	} else {
		RETURN_STRINGL(haystack + ret_pos, haystack_len - ret_pos);
	}

}
/* }}} */

/* {{{ proto string grapheme_strstr(string haystack, string needle[, bool part])
   Finds first occurrence of a string within another */
PHP_FUNCTION(grapheme_strstr)
{
	strstr_common_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0 /* f_ignore_case */);
}
/* }}} */

/* {{{ proto string grapheme_stristr(string haystack, string needle[, bool part])
   Finds first occurrence of a string within another */
PHP_FUNCTION(grapheme_stristr)
{
	strstr_common_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1 /* f_ignore_case */);
}
/* }}} */

/* {{{ grapheme_extract_charcount_iter - grapheme iterator for grapheme_extract MAXCHARS */
static inline int32_t
grapheme_extract_charcount_iter(UBreakIterator *bi, int32_t csize, unsigned char *pstr, int32_t str_len)
{
	int pos = 0;
	int ret_pos = 0;
	int break_pos, prev_break_pos;
	int count = 0;

	while ( 1 ) {
		pos = ubrk_next(bi);

		if ( UBRK_DONE == pos ) {
			break;
		}

		for ( break_pos = ret_pos; break_pos < pos; ) {
			count++;
			prev_break_pos = break_pos;
			U8_FWD_1(pstr, break_pos, str_len);

			if ( prev_break_pos == break_pos ) {
				/* something wrong - malformed utf8? */
				csize = 0;
				break;
			}
		}

		/* if we are beyond our limit, then the loop is done */
		if ( count > csize ) {
			break;
		}

		ret_pos = break_pos;
	}

	return ret_pos;
}
/* }}} */

/* {{{ grapheme_extract_bytecount_iter - grapheme iterator for grapheme_extract MAXBYTES */
static inline int32_t
grapheme_extract_bytecount_iter(UBreakIterator *bi, int32_t bsize, unsigned char *pstr, int32_t str_len)
{
	int pos = 0;
	int ret_pos = 0;

	while ( 1 ) {
		pos = ubrk_next(bi);

		if ( UBRK_DONE == pos ) {
			break;
		}

		if ( pos > bsize ) {
			break;
		}

		ret_pos = pos;
	}

	return ret_pos;
}
/* }}} */

/* {{{ grapheme_extract_count_iter - grapheme iterator for grapheme_extract COUNT */
static inline int32_t
grapheme_extract_count_iter(UBreakIterator *bi, int32_t size, unsigned char *pstr, int32_t str_len)
{
	int next_pos = 0;
	int ret_pos = 0;

	while ( size ) {
		next_pos = ubrk_next(bi);

		if ( UBRK_DONE == next_pos ) {
			break;
		}
		ret_pos = next_pos;
		size--;
	}

	return ret_pos;
}
/* }}} */

/* {{{ grapheme extract iter function pointer array */
typedef int32_t (*grapheme_extract_iter)(UBreakIterator * /*bi*/, int32_t /*size*/, unsigned char * /*pstr*/, int32_t /*str_len*/);

static grapheme_extract_iter grapheme_extract_iters[] = {
	&grapheme_extract_count_iter,
	&grapheme_extract_bytecount_iter,
	&grapheme_extract_charcount_iter,
};
/* }}} */

/* {{{ proto string grapheme_extract(string str, int size[, int extract_type[, int start[, int next]]])
	Function to extract a sequence of default grapheme clusters */
PHP_FUNCTION(grapheme_extract)
{
	char *str, *pstr;
	UText ut = UTEXT_INITIALIZER;
	size_t str_len;
	zend_long size; /* maximum number of grapheme clusters, bytes, or characters (based on extract_type) to return */
	zend_long lstart = 0; /* starting position in str in bytes */
	int32_t start = 0;
	zend_long extract_type = GRAPHEME_EXTRACT_TYPE_COUNT;
	UErrorCode status;
	unsigned char u_break_iterator_buffer[U_BRK_SAFECLONE_BUFFERSIZE];
	UBreakIterator* bi = NULL;
	int ret_pos;
	zval *next = NULL; /* return offset of next part of the string */

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl|llz", &str, &str_len, &size, &extract_type, &lstart, &next) == FAILURE) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_extract: unable to parse input param", 0 );
		RETURN_FALSE;
	}

	if (lstart < 0) {
		lstart += str_len;
	}

	if ( NULL != next ) {
		if ( !Z_ISREF_P(next) ) {
			intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
				 "grapheme_extract: 'next' was not passed by reference", 0 );
			RETURN_FALSE;
		} else {
			ZVAL_DEREF(next);
			/* initialize next */
			zval_ptr_dtor(next);
            ZVAL_LONG(next, lstart);
		}
	}

	if ( extract_type < GRAPHEME_EXTRACT_TYPE_MIN || extract_type > GRAPHEME_EXTRACT_TYPE_MAX ) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_extract: unknown extract type param", 0 );
		RETURN_FALSE;
	}

	if ( lstart > INT32_MAX || lstart < 0 || (size_t)lstart >= str_len ) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_extract: start not contained in string", 0 );
		RETURN_FALSE;
	}

	if ( size > INT32_MAX || size < 0) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_extract: size is invalid", 0 );
		RETURN_FALSE;
	}
	if (size == 0) {
		RETURN_EMPTY_STRING();
	}

	/* we checked that it will fit: */
	start = (int32_t) lstart;

	pstr = str + start;

	/* just in case pstr points in the middle of a character, move forward to the start of the next char */
	if ( !U8_IS_SINGLE(*pstr) && !U8_IS_LEAD(*pstr) ) {
		char *str_end = str + str_len;

		while ( !U8_IS_SINGLE(*pstr) && !U8_IS_LEAD(*pstr) ) {
			pstr++;
			if ( pstr >= str_end ) {
				intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
								"grapheme_extract: invalid input string", 0 );

				RETURN_FALSE;
			}
		}
	}

	str_len -= (pstr - str);

	/* if the string is all ASCII up to size+1 - or str_len whichever is first - then we are done.
		(size + 1 because the size-th character might be the beginning of a grapheme cluster)
	 */

	if ( -1 != grapheme_ascii_check((unsigned char *)pstr, MIN(size + 1, str_len)) ) {
        size_t nsize = MIN(size, str_len);
		if ( NULL != next ) {
			ZVAL_LONG(next, start+nsize);
		}
		RETURN_STRINGL(pstr, nsize);
	}

	status = U_ZERO_ERROR;
	utext_openUTF8(&ut, pstr, str_len, &status);

	if ( U_FAILURE( status ) ) {
		/* Set global error code. */
		intl_error_set_code( NULL, status );

		/* Set error messages. */
		intl_error_set_custom_msg( NULL, "Error opening UTF-8 text", 0 );

		RETURN_FALSE;
	}

	bi = NULL;
	status = U_ZERO_ERROR;
	bi = grapheme_get_break_iterator(u_break_iterator_buffer, &status );

	ubrk_setUText(bi, &ut, &status);
	/* if the caller put us in the middle of a grapheme, we can't detect it in all cases since we
		can't back up. So, we will not do anything. */

	/* now we need to find the end of the chunk the user wants us to return */
	/* it's ok to convert str_len to in32_t since if it were too big intl_convert_utf8_to_utf16 above would fail */
	ret_pos = (*grapheme_extract_iters[extract_type])(bi, size, (unsigned char *)pstr, (int32_t)str_len);

	utext_close(&ut);
	ubrk_close(bi);

	if ( NULL != next ) {
		ZVAL_LONG(next, start+ret_pos);
	}

	RETURN_STRINGL(((char *)pstr), ret_pos);
}

/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
