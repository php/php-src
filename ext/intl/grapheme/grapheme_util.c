/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Ed Batutis <ed@batutis.com>                                  |
   +----------------------------------------------------------------------+
 */

/* {{{ includes */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include "grapheme.h"
#include "grapheme_util.h"
#include "intl_common.h"

#include <unicode/utypes.h>
#include <unicode/ucol.h>
#include <unicode/ustring.h>
#include <unicode/ubrk.h>
#include <unicode/usearch.h>

#include "ext/standard/php_string.h"

ZEND_EXTERN_MODULE_GLOBALS( intl )

/* }}} */

/* {{{ grapheme_close_global_iterator - clean up */
void
grapheme_close_global_iterator( TSRMLS_D )
{
	UBreakIterator *global_break_iterator = INTL_G( grapheme_iterator );

	if ( NULL != global_break_iterator ) {
		ubrk_close(global_break_iterator);
	}
}
/* }}} */

/* {{{ grapheme_substr_ascii f='from' - starting point, l='length' */
void grapheme_substr_ascii(char *str, int str_len, int f, int l, int argc, char **sub_str, int *sub_str_len)
{
    *sub_str = NULL;

    if (argc > 2) {
        if ((l < 0 && -l > str_len)) {
            return;
        } else if (l > str_len) {
            l = str_len;
        }
    } else {
        l = str_len;
    }

    if (f > str_len || (f < 0 && -f > str_len)) {
        return;
    }

    if (l < 0 && (l + str_len - f) < 0) {
        return;
    }

    /* if "from" position is negative, count start position from the end
     * of the string
     */
    if (f < 0) {
        f = str_len + f;
        if (f < 0) {
            f = 0;
        }
    }


    /* if "length" position is negative, set it to the length
     * needed to stop that many chars from the end of the string
     */
    if (l < 0) {
        l = (str_len - f) + l;
        if (l < 0) {
            l = 0;
        }
    }

    if (f >= str_len) {
        return;
    }

    if ((f + l) > str_len) {
        l = str_len - f;
    }

    *sub_str = str + f;
    *sub_str_len = l;

    return;
}
/* }}} */

#define STRPOS_CHECK_STATUS(status, error) 							\
	if ( U_FAILURE( (status) ) ) { 									\
		intl_error_set_code( NULL, (status) TSRMLS_CC ); 			\
		intl_error_set_custom_msg( NULL, (error), 0 TSRMLS_CC ); 	\
		if (uhaystack) { 											\
			efree( uhaystack ); 									\
		} 															\
		if (uneedle) { 												\
			efree( uneedle ); 										\
		} 															\
		if(bi) { 													\
			ubrk_close (bi); 										\
		} 															\
		if(src) {													\
			usearch_close(src);										\
		}															\
		return -1; 													\
	}


/* {{{ grapheme_strpos_utf16 - strrpos using utf16*/
int grapheme_strpos_utf16(unsigned char *haystack, int32_t haystack_len, unsigned char*needle, int32_t needle_len, int32_t offset, int32_t *puchar_pos, int f_ignore_case, int last TSRMLS_DC)
{
	UChar *uhaystack = NULL, *uneedle = NULL;
	int32_t uhaystack_len = 0, uneedle_len = 0, char_pos, ret_pos, offset_pos = 0;
	unsigned char u_break_iterator_buffer[U_BRK_SAFECLONE_BUFFERSIZE];
	UBreakIterator* bi = NULL;
	UErrorCode status;
	UStringSearch* src = NULL;
	UCollator *coll;

	if(puchar_pos) {
		*puchar_pos = -1;
	}
	/* convert the strings to UTF-16. */

	status = U_ZERO_ERROR;
	intl_convert_utf8_to_utf16(&uhaystack, &uhaystack_len, (char *) haystack, haystack_len, &status );
	STRPOS_CHECK_STATUS(status, "Error converting input string to UTF-16");

	status = U_ZERO_ERROR;
	intl_convert_utf8_to_utf16(&uneedle, &uneedle_len, (char *) needle, needle_len, &status );
	STRPOS_CHECK_STATUS(status, "Error converting input string to UTF-16");

	/* get a pointer to the haystack taking into account the offset */
	status = U_ZERO_ERROR;
	bi = grapheme_get_break_iterator(u_break_iterator_buffer, &status TSRMLS_CC );
	STRPOS_CHECK_STATUS(status, "Failed to get iterator");
	status = U_ZERO_ERROR;
	ubrk_setText(bi, uhaystack, uhaystack_len, &status);
	STRPOS_CHECK_STATUS(status, "Failed to set up iterator");

	status = U_ZERO_ERROR;
	src = usearch_open(uneedle, uneedle_len, uhaystack, uhaystack_len, "", bi, &status);
	STRPOS_CHECK_STATUS(status, "Error creating search object");

	if(f_ignore_case) {
		coll = usearch_getCollator(src);
		status = U_ZERO_ERROR;
		ucol_setAttribute(coll, UCOL_STRENGTH, UCOL_SECONDARY, &status);
		STRPOS_CHECK_STATUS(status, "Error setting collation strength");
		usearch_reset(src);
	}

	if(offset != 0) {
		offset_pos = grapheme_get_haystack_offset(bi, offset);
		if(offset_pos == -1) {
			status = U_ILLEGAL_ARGUMENT_ERROR;
			STRPOS_CHECK_STATUS(status, "Invalid search offset");	
		}
		status = U_ZERO_ERROR;
		usearch_setOffset(src, offset_pos, &status);	
		STRPOS_CHECK_STATUS(status, "Invalid search offset");
	}


	if(last) {
		char_pos = usearch_last(src, &status);
		if(char_pos < offset_pos) {
			/* last one is beyound our start offset */
			char_pos = USEARCH_DONE;
		}
	} else {
		char_pos = usearch_next(src, &status);
	}
	STRPOS_CHECK_STATUS(status, "Error looking up string");
	if(char_pos != USEARCH_DONE && ubrk_isBoundary(bi, char_pos)) {
		ret_pos = grapheme_count_graphemes(bi, uhaystack,char_pos);
		if(puchar_pos) {
			*puchar_pos = char_pos;
		}
	} else {
		ret_pos = -1;
	}

	if (uhaystack) {
		efree( uhaystack );
	}
	if (uneedle) {
		efree( uneedle );
	}
	ubrk_close (bi);
	usearch_close (src);

	return ret_pos;
}

/* }}} */

/* {{{ grapheme_ascii_check: ASCII check */
int grapheme_ascii_check(const unsigned char *day, int32_t len)
{
	int ret_len = len;
	while ( len-- ) {
	if ( *day++ > 0x7f )
		return -1;
	}

	return ret_len;
}

/* }}} */

/* {{{ grapheme_split_string: find and optionally return grapheme boundaries */
int grapheme_split_string(const UChar *text, int32_t text_length, int boundary_array[], int boundary_array_len TSRMLS_DC )
{
	unsigned char u_break_iterator_buffer[U_BRK_SAFECLONE_BUFFERSIZE];
	UErrorCode		status = U_ZERO_ERROR;
	int ret_len, pos;
	UBreakIterator* bi;

	bi = grapheme_get_break_iterator((void*)u_break_iterator_buffer, &status TSRMLS_CC );

	if( U_FAILURE(status) ) {
		return -1;
	}
	
	ubrk_setText(bi, text, text_length,	&status);

	pos = 0;
	
	for ( ret_len = 0; pos != UBRK_DONE; ) {
	
		pos = ubrk_next(bi);
		
		if ( pos != UBRK_DONE ) {
		
			if ( NULL != boundary_array && ret_len < boundary_array_len ) {
				boundary_array[ret_len] = pos;
			}

			ret_len++;
		}
	}
	 		
	ubrk_close(bi);
	
	return ret_len;
}
/* }}} */

/* {{{ grapheme_count_graphemes */
int32_t grapheme_count_graphemes(UBreakIterator *bi, UChar *string, int32_t string_len)
{
	int ret_len = 0;
	int pos = 0;
	UErrorCode		status = U_ZERO_ERROR;
	
	ubrk_setText(bi, string, string_len, &status);

	do {
	
		pos = ubrk_next(bi);
		
		if ( UBRK_DONE != pos ) {
			ret_len++;
		}
		
	} while ( UBRK_DONE != pos );
	
	return ret_len;
}
/* }}} */


/* {{{ 	grapheme_get_haystack_offset - bump the haystack pointer based on the grapheme count offset */
int grapheme_get_haystack_offset(UBreakIterator* bi, int32_t offset)
{
	int32_t pos;
	int32_t (*iter_op)(UBreakIterator* bi);
	int iter_incr;

	if ( 0 == offset ) {
		return 0;
	}
	
	if ( offset < 0 ) {
		iter_op = ubrk_previous;
		ubrk_last(bi); /* one past the end */
		iter_incr = 1;
	}
	else {
		iter_op = ubrk_next;
		iter_incr = -1;
	}
	
	pos = 0;
	
	while ( pos != UBRK_DONE && offset != 0 ) {
	
		pos = iter_op(bi);
		
		if ( UBRK_DONE != pos ) {
			offset += iter_incr;
		}
	}

	if ( offset != 0 ) {
		return -1;
	}
	
	return pos;
}
/* }}} */

/* {{{ grapheme_strrpos_ascii: borrowed from the php ext/standard/string.c */
 int32_t
grapheme_strrpos_ascii(unsigned char *haystack, int32_t haystack_len, unsigned char *needle, int32_t needle_len, int32_t offset)
{
	unsigned char *p, *e;

	if (offset >= 0) {
		p = haystack + offset;
		e = haystack + haystack_len - needle_len;
	} else {
		p = haystack;
		if (needle_len > -offset) {
			e = haystack + haystack_len - needle_len;
		} else {
			e = haystack + haystack_len + offset;
		}
	}

	if (needle_len == 1) {
		/* Single character search can shortcut memcmps */
		while (e >= p) {
			if (*e == *needle) {
				return (e - p + (offset > 0 ? offset : 0));
			}
			e--;
		}
		return -1;
	}

	while (e >= p) {
		if (memcmp(e, needle, needle_len) == 0) {
			return (e - p + (offset > 0 ? offset : 0));
		}
		e--;
	}

	return -1;
}

/* }}} */

/* {{{ grapheme_get_break_iterator: get a clone of the global character break iterator */
UBreakIterator* grapheme_get_break_iterator(void *stack_buffer, UErrorCode *status TSRMLS_DC )
{
	int32_t buffer_size;

	UBreakIterator *global_break_iterator = INTL_G( grapheme_iterator );

	if ( NULL == global_break_iterator ) {

		global_break_iterator = ubrk_open(UBRK_CHARACTER, 
											NULL,	/* icu default locale - locale has no effect on this iterator */
											NULL,	/* text not set in global iterator */
											0,		/* text length = 0 */
											status);

		INTL_G(grapheme_iterator) = global_break_iterator;
	}

	buffer_size = U_BRK_SAFECLONE_BUFFERSIZE;

	return ubrk_safeClone(global_break_iterator, stack_buffer, &buffer_size, status);
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

