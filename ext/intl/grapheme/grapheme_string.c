/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Ed Batutis <ed@batutis.com>								  |
   +----------------------------------------------------------------------+
 */

/* {{{ includes */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <php.h>
#include "grapheme.h"
#include "grapheme_util.h"

#include <unicode/utypes.h>
#include <unicode/utf8.h>
#include <unicode/ucol.h>
#include <unicode/ustring.h>
#include <unicode/ubrk.h>

/* }}} */

/* {{{ Get number of graphemes in a string */
PHP_FUNCTION(grapheme_strlen)
{
	char* string;
	size_t string_len;
	UChar* ustring = NULL;
	int ustring_len = 0;
	zend_long ret_len;
	UErrorCode status;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(string, string_len)
	ZEND_PARSE_PARAMETERS_END();

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

/* {{{ Find position of first occurrence of a string within another */
PHP_FUNCTION(grapheme_strpos)
{
	char *haystack, *needle;
	size_t haystack_len, needle_len;
	const char *found;
	zend_long loffset = 0;
	int32_t offset = 0;
	size_t noffset = 0;
	zend_long ret_pos;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(haystack, haystack_len)
		Z_PARAM_STRING(needle, needle_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(loffset)
	ZEND_PARSE_PARAMETERS_END();

	if ( OUTSIDE_STRING(loffset, haystack_len) ) {
		zend_argument_value_error(3, "must be contained in argument #1 ($haystack)");
		RETURN_THROWS();
	}

	/* we checked that it will fit: */
	offset = (int32_t) loffset;
	noffset = offset >= 0 ? offset : (int32_t)haystack_len + offset;

	/* the offset is 'grapheme count offset' so it still might be invalid - we'll check it later */

	if (offset >= 0 && grapheme_ascii_check((unsigned char *)haystack, haystack_len) >= 0) {
		/* quick check to see if the string might be there
		 * I realize that 'offset' is 'grapheme count offset' but will work in spite of that
		*/
		found = php_memnstr(haystack + noffset, needle, needle_len, haystack + haystack_len);

		/* if it isn't there the we are done */
		if (found) {
			RETURN_LONG(found - haystack);
		}
		RETURN_FALSE;
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

/* {{{ Find position of first occurrence of a string within another, ignoring case differences */
PHP_FUNCTION(grapheme_stripos)
{
	char *haystack, *needle;
	size_t haystack_len, needle_len;
	const char *found;
	zend_long loffset = 0;
	int32_t offset = 0;
	zend_long ret_pos;
	int is_ascii;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(haystack, haystack_len)
		Z_PARAM_STRING(needle, needle_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(loffset)
	ZEND_PARSE_PARAMETERS_END();

	if ( OUTSIDE_STRING(loffset, haystack_len) ) {
		zend_argument_value_error(3, "must be contained in argument #1 ($haystack)");
		RETURN_THROWS();
	}

	/* we checked that it will fit: */
	offset = (int32_t) loffset;

	/* the offset is 'grapheme count offset' so it still might be invalid - we'll check it later */

	is_ascii = ( grapheme_ascii_check((unsigned char*)haystack, haystack_len) >= 0 );

	if ( is_ascii ) {
		char *haystack_dup, *needle_dup;
		int32_t noffset = offset >= 0 ? offset : (int32_t)haystack_len + offset;
		needle_dup = estrndup(needle, needle_len);
		zend_str_tolower(needle_dup, needle_len);
		haystack_dup = estrndup(haystack, haystack_len);
		zend_str_tolower(haystack_dup, haystack_len);

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

/* {{{ Find position of last occurrence of a string within another */
PHP_FUNCTION(grapheme_strrpos)
{
	char *haystack, *needle;
	size_t haystack_len, needle_len;
	zend_long loffset = 0;
	int32_t offset = 0;
	zend_long ret_pos;
	int is_ascii;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(haystack, haystack_len)
		Z_PARAM_STRING(needle, needle_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(loffset)
	ZEND_PARSE_PARAMETERS_END();

	if ( OUTSIDE_STRING(loffset, haystack_len) ) {
		zend_argument_value_error(3, "must be contained in argument #1 ($haystack)");
		RETURN_THROWS();
	}

	/* we checked that it will fit: */
	offset = (int32_t) loffset;

	/* the offset is 'grapheme count offset' so it still might be invalid - we'll check it later */

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

/* {{{ Find position of last occurrence of a string within another, ignoring case */
PHP_FUNCTION(grapheme_strripos)
{
	char *haystack, *needle;
	size_t haystack_len, needle_len;
	zend_long loffset = 0;
	int32_t offset = 0;
	zend_long ret_pos;
	int is_ascii;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(haystack, haystack_len)
		Z_PARAM_STRING(needle, needle_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(loffset)
	ZEND_PARSE_PARAMETERS_END();

	if ( OUTSIDE_STRING(loffset, haystack_len) ) {
		zend_argument_value_error(3, "must be contained in argument #1 ($haystack)");
		RETURN_THROWS();
	}

	/* we checked that it will fit: */
	offset = (int32_t) loffset;

	/* the offset is 'grapheme count offset' so it still might be invalid - we'll check it later */

	is_ascii = grapheme_ascii_check((unsigned char *)haystack, haystack_len) >= 0;

	if ( is_ascii ) {
		char *needle_dup, *haystack_dup;

		needle_dup = estrndup(needle, needle_len);
		zend_str_tolower(needle_dup, needle_len);
		haystack_dup = estrndup(haystack, haystack_len);
		zend_str_tolower(haystack_dup, haystack_len);

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

/* {{{ Returns part of a string */
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
	bool no_length = true;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_LONG(lstart)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(length, no_length)
	ZEND_PARSE_PARAMETERS_END();

	if (lstart < INT32_MIN || lstart > INT32_MAX) {
		zend_argument_value_error(2, "is too large");
		RETURN_THROWS();
	}

	start = (int32_t) lstart;

	if (no_length) {
		length = str_len;
	}

	if (length < INT32_MIN || length > INT32_MAX) {
		zend_argument_value_error(3, "is too large");
		RETURN_THROWS();
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

	if (0 != start) {
		if (start > 0) {
			if (ustr) {
				efree(ustr);
			}
			ubrk_close(bi);
			RETURN_EMPTY_STRING();
		}

		sub_str_start_pos = 0;
		ubrk_first(bi);
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
		if (length < 0) {
			efree(ustr);
			RETURN_EMPTY_STRING();
		} else {
			sub_str_end_pos = ustr_len;
		}
	}

	if (sub_str_start_pos > sub_str_end_pos) {
		efree(ustr);
		RETURN_EMPTY_STRING();
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
	bool part = false;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_STRING(haystack, haystack_len)
		Z_PARAM_STRING(needle, needle_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(part)
	ZEND_PARSE_PARAMETERS_END();

	if ( !f_ignore_case ) {

		/* ASCII optimization: quick check to see if the string might be there */
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

/* {{{ Finds first occurrence of a string within another */
PHP_FUNCTION(grapheme_strstr)
{
	strstr_common_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0 /* f_ignore_case */);
}
/* }}} */

/* {{{ Finds first occurrence of a string within another */
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

static const grapheme_extract_iter grapheme_extract_iters[] = {
	&grapheme_extract_count_iter,
	&grapheme_extract_bytecount_iter,
	&grapheme_extract_charcount_iter,
};
/* }}} */

/* {{{ Function to extract a sequence of default grapheme clusters */
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

	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_STRING(str, str_len)
		Z_PARAM_LONG(size)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(extract_type)
		Z_PARAM_LONG(lstart)
		Z_PARAM_ZVAL(next)
	ZEND_PARSE_PARAMETERS_END();

	if (lstart < 0) {
		lstart += str_len;
	}

	if ( NULL != next ) {
		ZEND_ASSERT(Z_ISREF_P(next));
		ZEND_TRY_ASSIGN_REF_LONG(next, lstart);
		if (UNEXPECTED(EG(exception))) {
			RETURN_THROWS();
		}
	}

	if ( extract_type < GRAPHEME_EXTRACT_TYPE_MIN || extract_type > GRAPHEME_EXTRACT_TYPE_MAX ) {
		zend_argument_value_error(3, "must be one of GRAPHEME_EXTR_COUNT, GRAPHEME_EXTR_MAXBYTES, or GRAPHEME_EXTR_MAXCHARS");
		RETURN_THROWS();
	}

	if ( lstart > INT32_MAX || lstart < 0 || (size_t)lstart >= str_len ) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_extract: start not contained in string", 0 );
		RETURN_FALSE;
	}

	if (size < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	if (size > INT32_MAX) {
		zend_argument_value_error(2, "is too large");
		RETURN_THROWS();
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
			start++;
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
			ZEND_TRY_ASSIGN_REF_LONG(next, start + nsize);
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
		ZEND_TRY_ASSIGN_REF_LONG(next, start + ret_pos);
	}

	RETURN_STRINGL(((char *)pstr), ret_pos);
}

PHP_FUNCTION(grapheme_str_split)
{
	char *pstr, *end;
	zend_string *str;
	zend_long split_len = 1;

	unsigned char u_break_iterator_buffer[U_BRK_SAFECLONE_BUFFERSIZE];
	UErrorCode ustatus = U_ZERO_ERROR;
	int32_t pos, current, i, end_len = 0;
	UBreakIterator* bi;
	UText *ut = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(split_len)
	ZEND_PARSE_PARAMETERS_END();

	if (split_len <= 0 || split_len > UINT_MAX / 4) {
		zend_argument_value_error(2, "must be greater than 0 and less than or equal to %d", UINT_MAX / 4);
		RETURN_THROWS();
	}

	if (ZSTR_LEN(str) == 0) {
		RETURN_EMPTY_ARRAY();
	}

	pstr = ZSTR_VAL(str);
	ut = utext_openUTF8(ut, pstr, ZSTR_LEN(str), &ustatus);

	if ( U_FAILURE( ustatus ) ) {
		/* Set global error code. */
		intl_error_set_code( NULL, ustatus );

		/* Set error messages. */
		intl_error_set_custom_msg( NULL, "Error opening UTF-8 text", 0 );

		RETURN_FALSE;
	}

	bi = NULL;
	ustatus = U_ZERO_ERROR;
	bi = grapheme_get_break_iterator((void*)u_break_iterator_buffer, &ustatus );

	if( U_FAILURE(ustatus) ) {
		RETURN_FALSE;
	}

	ubrk_setUText(bi, ut, &ustatus);

	pos = 0;
	array_init(return_value);

	for (end = pstr, i = 0, current = 0; pos != UBRK_DONE;) {
		end_len = pos - current;
		pos = ubrk_next(bi);

		if (i == split_len - 1) {
			if ( pos != UBRK_DONE ) {
				add_next_index_stringl(return_value, pstr, pos - current);
				end = pstr + pos - current;
				i = 0;
			}
			pstr += pos - current;
			current = pos;
		} else {
			i += 1;
		}
	}

	if (i != 0 && end_len != 0) {
		add_next_index_stringl(return_value, end, end_len);
	}

	utext_close(ut);
	ubrk_close(bi);
}

PHP_FUNCTION(grapheme_levenshtein)
{
	zend_string *string1, *string2;
	zend_long cost_ins = 1;
	zend_long cost_rep = 1;
	zend_long cost_del = 1;

	ZEND_PARSE_PARAMETERS_START(2, 5)
		Z_PARAM_STR(string1)
		Z_PARAM_STR(string2)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(cost_ins)
		Z_PARAM_LONG(cost_rep)
		Z_PARAM_LONG(cost_del)
	ZEND_PARSE_PARAMETERS_END();

	if (cost_ins <= 0 || cost_ins > UINT_MAX / 4) {
		zend_argument_value_error(3, "must be greater than 0 and less than or equal to %d", UINT_MAX / 4);
		RETURN_THROWS();
	}

	if (cost_rep <= 0 || cost_rep > UINT_MAX / 4) {
		zend_argument_value_error(4, "must be greater than 0 and less than or equal to %d", UINT_MAX / 4);
		RETURN_THROWS();
	}

	if (cost_del <= 0 || cost_del > UINT_MAX / 4) {
		zend_argument_value_error(5, "must be greater than 0 and less than or equal to %d", UINT_MAX / 4);
		RETURN_THROWS();
	}

	zend_long c0, c1, c2;
	zend_long retval;
	size_t i2;
	char *pstr1, *pstr2;

	UChar *ustring1 = NULL;
	UChar *ustring2 = NULL;

	int32_t ustring1_len = 0;
	int32_t ustring2_len = 0;

	UErrorCode ustatus = U_ZERO_ERROR;

	/* When all costs are equal, levenshtein fulfills the requirements of a metric, which means
	 * that the distance is symmetric. If string1 is shorter than string2 we can save memory (and CPU time)
	 * by having shorter rows (p1 & p2). */
	if (ZSTR_LEN(string1) < ZSTR_LEN(string2) && cost_ins == cost_rep && cost_rep == cost_del) {
		zend_string *tmp = string1;
		string1 = string2;
		string2 = tmp;
	}

	pstr1 = ZSTR_VAL(string1);
	pstr2 = ZSTR_VAL(string2);

	intl_convert_utf8_to_utf16(&ustring1, &ustring1_len, pstr1, ZSTR_LEN(string1), &ustatus);

	if (U_FAILURE(ustatus)) {
		intl_error_set_code(NULL, ustatus);

		intl_error_set_custom_msg(NULL, "Error converting input string to UTF-16", 0);
		RETVAL_FALSE;
		goto out_ustring1;
	}

	intl_convert_utf8_to_utf16(&ustring2, &ustring2_len, pstr2, ZSTR_LEN(string2), &ustatus);

	if (U_FAILURE(ustatus)) {
		intl_error_set_code(NULL, ustatus);

		intl_error_set_custom_msg(NULL, "Error converting input string to UTF-16", 0);
		RETVAL_FALSE;
		goto out_ustring2;
	}

	UBreakIterator *bi1, *bi2;

	int32_t strlen_1, strlen_2;
	strlen_1 = grapheme_split_string(ustring1, ustring1_len, NULL, 0);
	strlen_2 = grapheme_split_string(ustring2, ustring2_len, NULL, 0);
	if (UNEXPECTED(strlen_1 < 0 || strlen_2 < 0)) {
		RETVAL_FALSE;
		goto out_ustring2;
	}

	if (strlen_1 == 0) {
		RETVAL_LONG(strlen_2 * cost_ins);
		goto out_ustring2;
	}
	if (strlen_2 == 0) {
		RETVAL_LONG(strlen_1 * cost_del);
		goto out_ustring2;
	}

	unsigned char u_break_iterator_buffer1[U_BRK_SAFECLONE_BUFFERSIZE];
	unsigned char u_break_iterator_buffer2[U_BRK_SAFECLONE_BUFFERSIZE];
	bi1 = grapheme_get_break_iterator(u_break_iterator_buffer1, &ustatus);
	if (U_FAILURE(ustatus)) {
		intl_error_set_code(NULL, ustatus);
		intl_error_set_custom_msg(NULL, "Error on grapheme_get_break_iterator for argument #1 ($string1)", 0);
		RETVAL_FALSE;
		goto out_bi1;
	}

	bi2 = grapheme_get_break_iterator(u_break_iterator_buffer2, &ustatus);
	if (U_FAILURE(ustatus)) {
		intl_error_set_code(NULL, ustatus);
		intl_error_set_custom_msg(NULL, "Error on grapheme_get_break_iterator for argument #2 ($string2)", 0);
		RETVAL_FALSE;
		goto out_bi2;
	}

	ubrk_setText(bi1, ustring1, ustring1_len, &ustatus);
	if (U_FAILURE(ustatus)) {
		intl_error_set_code(NULL, ustatus);

		intl_error_set_custom_msg(NULL, "Error on ubrk_setText for argument #1 ($string1)", 0);
		RETVAL_FALSE;
		goto out_bi2;
	}

	ubrk_setText(bi2, ustring2, ustring2_len, &ustatus);
	if (U_FAILURE(ustatus)) {
		intl_error_set_code(NULL, ustatus);

		intl_error_set_custom_msg(NULL, "Error on ubrk_setText for argument #2 ($string2)", 0);
		RETVAL_FALSE;
		goto out_bi2;
	}
	UCollator *collator = ucol_open("", &ustatus);
	if (U_FAILURE(ustatus)) {
		intl_error_set_code(NULL, ustatus);

		intl_error_set_custom_msg(NULL, "Error on ucol_open", 0);
		RETVAL_FALSE;
		goto out_collator;
	}

	zend_long *p1, *p2, *tmp;
	p1 = safe_emalloc((size_t) strlen_2 + 1, sizeof(zend_long), 0);
	p2 = safe_emalloc((size_t) strlen_2 + 1, sizeof(zend_long), 0);

	for (i2 = 0; i2 <= strlen_2; i2++) {
		p1[i2] = i2 * cost_ins;
	}

	int32_t current1 = 0;
	int32_t current2 = 0;
	int32_t pos1 = 0;
	int32_t pos2 = 0;

	while (true) {
		current1 = ubrk_current(bi1);
		pos1 = ubrk_next(bi1);
		if (pos1 == UBRK_DONE) {
			break;
		}
		p2[0] = p1[0] + cost_del;
		for (i2 = 0, pos2 = 0; pos2 != UBRK_DONE; i2++) {
			current2 = ubrk_current(bi2);
			pos2 = ubrk_next(bi2);
			if (pos2 == UBRK_DONE) {
				break;
			}
			if (ucol_strcoll(collator, ustring1 + current1, pos1 - current1, ustring2 + current2, pos2 - current2) == UCOL_EQUAL) {
				c0 = p1[i2];
			} else {
				c0 = p1[i2] + cost_rep;
			}
			c1 = p1[i2 + 1] + cost_del;
			if (c1 < c0) {
				c0 = c1;
			}
			c2 = p2[i2] + cost_ins;
			if (c2 < c0) {
				c0 = c2;
			}
			p2[i2 + 1] = c0;
		}
		ubrk_first(bi2);
		tmp = p1;
		p1 = p2;
		p2 = tmp;
	}

	retval = p1[strlen_2];
	RETVAL_LONG(retval);

	efree(p2);
	efree(p1);

out_collator:
	ucol_close(collator);
out_bi2:
	ubrk_close(bi2);
out_bi1:
	ubrk_close(bi1);
out_ustring2:
	efree(ustring2);
out_ustring1:
	efree(ustring1);
}

/* }}} */
