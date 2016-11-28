/*
   +----------------------------------------------------------------------+
   | PHP Version 5														  |
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

/* {{{ proto int grapheme_strlen(string str)
   Get number of graphemes in a string */
PHP_FUNCTION(grapheme_strlen)
{
	unsigned char* string;
	int string_len;
	UChar* ustring = NULL;
	int ustring_len = 0;
	int ret_len;
	UErrorCode status;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", (char **)&string, &string_len) == FAILURE) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_strlen: unable to parse input param", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	ret_len = grapheme_ascii_check(string, string_len);

	if ( ret_len >= 0 )
		RETURN_LONG(ret_len);

	/* convert the string to UTF-16. */
	status = U_ZERO_ERROR;
	intl_convert_utf8_to_utf16(&ustring, &ustring_len, (char*) string, string_len, &status );

	if ( U_FAILURE( status ) ) {
		/* Set global error code. */
		intl_error_set_code( NULL, status TSRMLS_CC );

		/* Set error messages. */
		intl_error_set_custom_msg( NULL, "Error converting input string to UTF-16", 0 TSRMLS_CC );
		if (ustring) {
			efree( ustring );
		}
		RETURN_NULL();
	}

	ret_len = grapheme_split_string(ustring, ustring_len, NULL, 0 TSRMLS_CC );

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
	unsigned char *haystack, *needle;
	int haystack_len, needle_len;
	unsigned char *found;
	long loffset = 0;
	int32_t offset = 0, noffset = 0;
	int ret_pos;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", (char **)&haystack, &haystack_len, (char **)&needle, &needle_len, &loffset) == FAILURE) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_strpos: unable to parse input param", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	if ( OUTSIDE_STRING(loffset, haystack_len) ) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Offset not contained in string", 1 TSRMLS_CC );

		RETURN_FALSE;
	}

	/* we checked that it will fit: */
	offset = (int32_t) loffset;
	noffset = offset >= 0 ? offset : haystack_len + offset;

	/* the offset is 'grapheme count offset' so it still might be invalid - we'll check it later */

	if (needle_len == 0) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Empty delimiter", 1 TSRMLS_CC );

		RETURN_FALSE;
	}


	/* quick check to see if the string might be there
	 * I realize that 'offset' is 'grapheme count offset' but will work in spite of that
	*/
	found = (unsigned char *)php_memnstr((char *)haystack + noffset, (char *)needle, needle_len, (char *)haystack + haystack_len);

	/* if it isn't there the we are done */
	if (!found) {
		RETURN_FALSE;
	}

	/* if it is there, and if the haystack is ascii, we are all done */
	if ( grapheme_ascii_check(haystack, haystack_len) >= 0 ) {

		RETURN_LONG(found - haystack);
	}

	/* do utf16 part of the strpos */
	ret_pos = grapheme_strpos_utf16(haystack, haystack_len, needle, needle_len, offset, NULL, 0 /* fIgnoreCase */, 0 /* last */ TSRMLS_CC );

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
	unsigned char *haystack, *needle, *haystack_dup, *needle_dup;
	int haystack_len, needle_len;
	unsigned char *found;
	long loffset = 0;
	int32_t offset = 0;
	int ret_pos;
	int is_ascii;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", (char **)&haystack, &haystack_len, (char **)&needle, &needle_len, &loffset) == FAILURE) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_stripos: unable to parse input param", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	if ( OUTSIDE_STRING(loffset, haystack_len) ) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_stripos: Offset not contained in string", 1 TSRMLS_CC );

		RETURN_FALSE;
	}

	/* we checked that it will fit: */
	offset = (int32_t) loffset;

	/* the offset is 'grapheme count offset' so it still might be invalid - we'll check it later */

	if (needle_len == 0) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_stripos: Empty delimiter", 1 TSRMLS_CC );

		RETURN_FALSE;
	}


	is_ascii = ( grapheme_ascii_check(haystack, haystack_len) >= 0 );

	if ( is_ascii ) {
		int32_t noffset = offset >= 0 ? offset : haystack_len + offset;
		needle_dup = (unsigned char *)estrndup((char *)needle, needle_len);
		php_strtolower((char *)needle_dup, needle_len);
		haystack_dup = (unsigned char *)estrndup((char *)haystack, haystack_len);
		php_strtolower((char *)haystack_dup, haystack_len);

		found = (unsigned char*) php_memnstr((char *)haystack_dup + noffset, (char *)needle_dup, needle_len, (char *)haystack_dup + haystack_len);

		efree(haystack_dup);
		efree(needle_dup);

		if (found) {
			RETURN_LONG(found - haystack_dup);
		}

		/* if needle was ascii too, we are all done, otherwise we need to try using Unicode to see what we get */
		if ( grapheme_ascii_check(needle, needle_len) >= 0 ) {
			RETURN_FALSE;
		}
	}

	/* do utf16 part of the strpos */
	ret_pos = grapheme_strpos_utf16(haystack, haystack_len, needle, needle_len, offset, NULL, 1 /* fIgnoreCase */, 0 /*last */ TSRMLS_CC );

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
	unsigned char *haystack, *needle;
	int haystack_len, needle_len;
	long loffset = 0;
	int32_t offset = 0;
	int32_t ret_pos;
	int is_ascii;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", (char **)&haystack, &haystack_len, (char **)&needle, &needle_len, &loffset) == FAILURE) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_strrpos: unable to parse input param", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	if ( OUTSIDE_STRING(loffset, haystack_len) ) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Offset not contained in string", 1 TSRMLS_CC );

		RETURN_FALSE;
	}

	/* we checked that it will fit: */
	offset = (int32_t) loffset;

	/* the offset is 'grapheme count offset' so it still might be invalid - we'll check it later */

	if (needle_len == 0) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Empty delimiter", 1 TSRMLS_CC );

		RETURN_FALSE;
	}

	is_ascii = grapheme_ascii_check(haystack, haystack_len) >= 0;

	if ( is_ascii ) {

		ret_pos = grapheme_strrpos_ascii(haystack, haystack_len, needle, needle_len, offset);


		if ( ret_pos >= 0 ) {
			RETURN_LONG(ret_pos);
		}

		/* if the needle was ascii too, we are done */

		if (  grapheme_ascii_check(needle, needle_len) >= 0 ) {
			RETURN_FALSE;
		}

		/* else we need to continue via utf16 */
	}

	ret_pos = grapheme_strpos_utf16(haystack, haystack_len, needle, needle_len, offset, NULL, 0 /* f_ignore_case */, 1/* last */ TSRMLS_CC);

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
	unsigned char *haystack, *needle;
	int haystack_len, needle_len;
	long loffset = 0;
	int32_t offset = 0;
	int32_t ret_pos;
	int is_ascii;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", (char **)&haystack, &haystack_len, (char **)&needle, &needle_len, &loffset) == FAILURE) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_strrpos: unable to parse input param", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	if ( OUTSIDE_STRING(loffset, haystack_len) ) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Offset not contained in string", 1 TSRMLS_CC );

		RETURN_FALSE;
	}

	/* we checked that it will fit: */
	offset = (int32_t) loffset;

	/* the offset is 'grapheme count offset' so it still might be invalid - we'll check it later */

	if (needle_len == 0) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Empty delimiter", 1 TSRMLS_CC );

		RETURN_FALSE;
	}

	is_ascii = grapheme_ascii_check(haystack, haystack_len) >= 0;

	if ( is_ascii ) {
		unsigned char *needle_dup, *haystack_dup;

		needle_dup = (unsigned char *)estrndup((char *)needle, needle_len);
		php_strtolower((char *)needle_dup, needle_len);
		haystack_dup = (unsigned char *)estrndup((char *)haystack, haystack_len);
		php_strtolower((char *)haystack_dup, haystack_len);

		ret_pos = grapheme_strrpos_ascii(haystack_dup, haystack_len, needle_dup, needle_len, offset);

		efree(haystack_dup);
		efree(needle_dup);

		if ( ret_pos >= 0 ) {
			RETURN_LONG(ret_pos);
		}

		/* if the needle was ascii too, we are done */

		if (  grapheme_ascii_check(needle, needle_len) >= 0 ) {
			RETURN_FALSE;
		}

		/* else we need to continue via utf16 */
	}

	ret_pos = grapheme_strpos_utf16(haystack, haystack_len, needle, needle_len, offset, NULL,  1 /* f_ignore_case */, 1 /*last */ TSRMLS_CC);

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
	unsigned char *str, *sub_str;
	UChar *ustr;
	int str_len, sub_str_len, ustr_len;
	long lstart = 0, length = 0;
	int32_t start = 0;
	int iter_val;
	UErrorCode status;
	unsigned char u_break_iterator_buffer[U_BRK_SAFECLONE_BUFFERSIZE];
	UBreakIterator* bi = NULL;
	int sub_str_start_pos, sub_str_end_pos;
	int32_t (*iter_func)(UBreakIterator *);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|l", (char **)&str, &str_len, &lstart, &length) == FAILURE) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_substr: unable to parse input param", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	if ( OUTSIDE_STRING(lstart, str_len) ) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_substr: start not contained in string", 1 TSRMLS_CC );

		RETURN_FALSE;
	}

	/* we checked that it will fit: */
	start = (int32_t) lstart;

	/* the offset is 'grapheme count offset' so it still might be invalid - we'll check it later */

	if ( grapheme_ascii_check(str, str_len) >= 0 ) {
		grapheme_substr_ascii((char *)str, str_len, start, length, ZEND_NUM_ARGS(), (char **) &sub_str, &sub_str_len);

		if ( NULL == sub_str ) {
			intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_substr: invalid parameters", 1 TSRMLS_CC );
			RETURN_FALSE;
		}

		RETURN_STRINGL(((char *)sub_str), sub_str_len, 1);
	}

	ustr = NULL;
	ustr_len = 0;
	status = U_ZERO_ERROR;
	intl_convert_utf8_to_utf16(&ustr, &ustr_len, (char *)str, str_len, &status);

	if ( U_FAILURE( status ) ) {
		/* Set global error code. */
		intl_error_set_code( NULL, status TSRMLS_CC );

		/* Set error messages. */
		intl_error_set_custom_msg( NULL, "Error converting input string to UTF-16", 0 TSRMLS_CC );
		if (ustr) {
			efree( ustr );
		}
		RETURN_FALSE;
	}

	bi = grapheme_get_break_iterator((void*)u_break_iterator_buffer, &status TSRMLS_CC );

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

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_substr: start not contained in string", 1 TSRMLS_CC );

		if (ustr) {
			efree(ustr);
		}
		ubrk_close(bi);
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() <= 2) {

		/* no length supplied, return the rest of the string */

		sub_str = NULL;
		sub_str_len = 0;
		status = U_ZERO_ERROR;
		intl_convert_utf16_to_utf8((char **)&sub_str, &sub_str_len, ustr + sub_str_start_pos, ustr_len - sub_str_start_pos, &status);

		if (ustr) {
			efree( ustr );
		}
		ubrk_close( bi );

		if ( U_FAILURE( status ) ) {
			/* Set global error code. */
			intl_error_set_code( NULL, status TSRMLS_CC );

			/* Set error messages. */
			intl_error_set_custom_msg( NULL, "Error converting output string to UTF-8", 0 TSRMLS_CC );

			if (sub_str) {
				efree( sub_str );
			}

			RETURN_FALSE;
		}

		/* return the allocated string, not a duplicate */
		RETURN_STRINGL(((char *)sub_str), sub_str_len, 0);
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
			intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_substr: length not contained in string", 1 TSRMLS_CC );

			efree(ustr);
			RETURN_FALSE;
		} else {
			sub_str_end_pos = ustr_len;
		}
	}

	if(sub_str_start_pos > sub_str_end_pos) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_substr: length is beyond start", 1 TSRMLS_CC );

		efree(ustr);
		RETURN_FALSE;
	}

	sub_str = NULL;
	status = U_ZERO_ERROR;
	intl_convert_utf16_to_utf8((char **)&sub_str, &sub_str_len, ustr + sub_str_start_pos, ( sub_str_end_pos - sub_str_start_pos ), &status);

	efree( ustr );

	if ( U_FAILURE( status ) ) {
		/* Set global error code. */
		intl_error_set_code( NULL, status TSRMLS_CC );

		/* Set error messages. */
		intl_error_set_custom_msg( NULL, "Error converting output string to UTF-8", 0 TSRMLS_CC );

		if ( NULL != sub_str )
			efree( sub_str );

		RETURN_FALSE;
	}

	 /* return the allocated string, not a duplicate */
	RETURN_STRINGL(((char *)sub_str), sub_str_len, 0);

}
/* }}} */

/* {{{	strstr_common_handler */
static void strstr_common_handler(INTERNAL_FUNCTION_PARAMETERS, int f_ignore_case)
{
	unsigned char *haystack, *needle, *found;
	int haystack_len, needle_len;
	int ret_pos, uchar_pos;
	zend_bool part = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|b", (char **)&haystack, &haystack_len, (char **)&needle, &needle_len, &part) == FAILURE) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_strstr: unable to parse input param", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	if (needle_len == 0) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Empty delimiter", 1 TSRMLS_CC );

		RETURN_FALSE;
	}


	if ( !f_ignore_case ) {

		/* ASCII optimization: quick check to see if the string might be there
		 * I realize that 'offset' is 'grapheme count offset' but will work in spite of that
		*/
		found = (unsigned char *)php_memnstr((char *)haystack, (char *)needle, needle_len, (char *)haystack + haystack_len);

		/* if it isn't there the we are done */
		if ( !found ) {
			RETURN_FALSE;
		}

		/* if it is there, and if the haystack is ascii, we are all done */
		if ( grapheme_ascii_check(haystack, haystack_len) >= 0 ) {
			size_t found_offset = found - haystack;

			if (part) {
				RETURN_STRINGL(((char *)haystack) , found_offset, 1);
			} else {
				RETURN_STRINGL(((char *)found), haystack_len - found_offset, 1);
			}
		}

	}

	/* need to work in utf16 */
	ret_pos = grapheme_strpos_utf16(haystack, haystack_len, needle, needle_len, 0, &uchar_pos, f_ignore_case, 0 /*last */ TSRMLS_CC );

	if ( ret_pos < 0 ) {
		RETURN_FALSE;
	}

	/* uchar_pos is the 'nth' Unicode character position of the needle */

	ret_pos = 0;
	U8_FWD_N(haystack, ret_pos, haystack_len, uchar_pos);

	if (part) {
		RETURN_STRINGL(((char *)haystack), ret_pos, 1);
	}
	else {
		RETURN_STRINGL(((char *)haystack) + ret_pos, haystack_len - ret_pos, 1);
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
	int str_len;
	long size; /* maximum number of grapheme clusters, bytes, or characters (based on extract_type) to return */
	long lstart = 0; /* starting position in str in bytes */
	int32_t start = 0;
	long extract_type = GRAPHEME_EXTRACT_TYPE_COUNT;
	UErrorCode status;
	unsigned char u_break_iterator_buffer[U_BRK_SAFECLONE_BUFFERSIZE];
	UBreakIterator* bi = NULL;
	int ret_pos;
	zval *next = NULL; /* return offset of next part of the string */

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|llz", (char **)&str, &str_len, &size, &extract_type, &lstart, &next) == FAILURE) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_extract: unable to parse input param", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	if ( NULL != next ) {
		if ( !PZVAL_IS_REF(next) ) {
			intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
				 "grapheme_extract: 'next' was not passed by reference", 0 TSRMLS_CC );

			RETURN_FALSE;
		}
		else {
			/* initialize next */
			zval_dtor(next);
            ZVAL_LONG(next, lstart);
		}
	}

	if ( extract_type < GRAPHEME_EXTRACT_TYPE_MIN || extract_type > GRAPHEME_EXTRACT_TYPE_MAX ) {

		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "grapheme_extract: unknown extract type param", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	if ( lstart > INT32_MAX || lstart < 0 || lstart >= str_len ) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_extract: start not contained in string", 0 TSRMLS_CC );
		RETURN_FALSE;
	}

	if ( size > INT32_MAX || size < 0) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_extract: size is invalid", 0 TSRMLS_CC );
		RETURN_FALSE;
	}
	if (size == 0) {
		RETURN_EMPTY_STRING();
	}

	/* we checked that it will fit: */
	start = (int32_t) lstart;

	pstr = str + start;

	/* just in case pstr points in the middle of a character, move forward to the start of the next char */
	if ( !UTF8_IS_SINGLE(*pstr) && !U8_IS_LEAD(*pstr) ) {
		unsigned char *str_end = str + str_len;

		while ( !UTF8_IS_SINGLE(*pstr) && !U8_IS_LEAD(*pstr) ) {
			pstr++;
			if ( pstr >= str_end ) {
				intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
								"grapheme_extract: invalid input string", 0 TSRMLS_CC );

				RETURN_FALSE;
			}
		}
	}

	str_len -= (pstr - str);

	/* if the string is all ASCII up to size+1 - or str_len whichever is first - then we are done.
		(size + 1 because the size-th character might be the beginning of a grapheme cluster)
	 */

	if ( -1 != grapheme_ascii_check(pstr, size + 1 < str_len ? size + 1 : str_len ) ) {
        long nsize = ( size < str_len ? size : str_len );
		if ( NULL != next ) {
			ZVAL_LONG(next, start+nsize);
		}
		RETURN_STRINGL(((char *)pstr), nsize, 1);
	}

	status = U_ZERO_ERROR;
	utext_openUTF8(&ut, pstr, str_len, &status);

	if ( U_FAILURE( status ) ) {
		/* Set global error code. */
		intl_error_set_code( NULL, status TSRMLS_CC );

		/* Set error messages. */
		intl_error_set_custom_msg( NULL, "Error opening UTF-8 text", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	bi = NULL;
	status = U_ZERO_ERROR;
	bi = grapheme_get_break_iterator(u_break_iterator_buffer, &status TSRMLS_CC );

	ubrk_setUText(bi, &ut, &status);
	/* if the caller put us in the middle of a grapheme, we can't detect it in all cases since we
		can't back up. So, we will not do anything. */

	/* now we need to find the end of the chunk the user wants us to return */

	ret_pos = (*grapheme_extract_iters[extract_type])(bi, size, pstr, str_len);

	utext_close(&ut);
	ubrk_close(bi);

	if ( NULL != next ) {
		ZVAL_LONG(next, start+ret_pos);
	}

	RETURN_STRINGL(((char *)pstr), ret_pos, 1);
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

