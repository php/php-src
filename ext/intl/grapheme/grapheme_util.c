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

/* {{{ grapheme_intl_case_fold: convert string to lowercase */
void
grapheme_intl_case_fold(UChar** ptr_to_free, UChar **str, int32_t *str_len, UErrorCode *pstatus )
{
    UChar *dest;
    int32_t dest_len, size_required;

    /* allocate a destination string that is a bit larger than the src, hoping that is enough */
    dest_len = (*str_len) + ( *str_len / 10 );
    dest = (UChar*) eumalloc(dest_len);

    *pstatus = U_ZERO_ERROR;
    size_required = u_strFoldCase(dest, dest_len, *str, *str_len, U_FOLD_CASE_DEFAULT, pstatus);

    dest_len = size_required;

    if ( U_BUFFER_OVERFLOW_ERROR == *pstatus ) {

        dest = (UChar*) eurealloc(dest, dest_len);

        *pstatus = U_ZERO_ERROR;
        size_required = u_strFoldCase(dest, dest_len, *str, *str_len, U_FOLD_CASE_DEFAULT, pstatus);
    }

    if ( U_FAILURE(*pstatus) ) {
        return;
    }

    if ( NULL != ptr_to_free) {
        efree(*ptr_to_free);
        *ptr_to_free = dest;
    }

    *str = dest;
    *str_len = dest_len;

    return;
}
/* }}} */

/* {{{ grapheme_substr_ascii f='from' - starting point, l='length' */
void
grapheme_substr_ascii(char *str, int str_len, int f, int l, int argc, char **sub_str, int *sub_str_len)
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

/* {{{ grapheme_strrpos_utf16 - strrpos using utf16 */
int
grapheme_strrpos_utf16(unsigned char *haystack, int32_t haystack_len, unsigned char*needle, int32_t needle_len, int32_t offset, int f_ignore_case TSRMLS_DC)
{
    UChar *uhaystack, *puhaystack, *uhaystack_end, *uneedle;
    int32_t uhaystack_len, uneedle_len;
    UErrorCode status;
    unsigned char u_break_iterator_buffer[U_BRK_SAFECLONE_BUFFERSIZE];
    UBreakIterator* bi = NULL;
    int ret_pos, pos;

    /* convert the strings to UTF-16. */
    uhaystack = NULL;
    uhaystack_len = 0;
    status = U_ZERO_ERROR;
    intl_convert_utf8_to_utf16(&uhaystack, &uhaystack_len, (char *) haystack, haystack_len, &status );

    if ( U_FAILURE( status ) ) {
        /* Set global error code. */
        intl_error_set_code( NULL, status TSRMLS_CC );

        /* Set error messages. */
        intl_error_set_custom_msg( NULL, "Error converting input string to UTF-16", 0 TSRMLS_CC );
        efree( uhaystack );
        return -1;
    }

    if ( f_ignore_case ) {
        grapheme_intl_case_fold(&uhaystack, &uhaystack, &uhaystack_len, &status );
    }

    /* get a pointer to the haystack taking into account the offset */
    bi = NULL;
    status = U_ZERO_ERROR;
    bi = grapheme_get_break_iterator(u_break_iterator_buffer, &status TSRMLS_CC );

    puhaystack = grapheme_get_haystack_offset(bi, uhaystack, uhaystack_len, offset);

    if ( NULL == puhaystack ) {
        intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Offset not contained in string", 1 TSRMLS_CC );
        efree( uhaystack );
        ubrk_close (bi);
        return -1;
    }

    uneedle = NULL;
    uneedle_len = 0;
    status = U_ZERO_ERROR;
    intl_convert_utf8_to_utf16(&uneedle, &uneedle_len, (char *) needle, needle_len, &status );

    if ( U_FAILURE( status ) ) {
        /* Set global error code. */
        intl_error_set_code( NULL, status TSRMLS_CC );

        /* Set error messages. */
        intl_error_set_custom_msg( NULL, "Error converting input string to UTF-16", 0 TSRMLS_CC );
        efree( uhaystack );
        efree( uneedle );
        ubrk_close (bi);
        return -1;
    }

    if ( f_ignore_case ) {
        grapheme_intl_case_fold(&uneedle, &uneedle, &uneedle_len, &status );
    }

    ret_pos = -1;   /* -1 represents 'not found' */

    /* back up until there's needle_len characters to compare */

    uhaystack_end = uhaystack + uhaystack_len;
    pos = ubrk_last(bi);
    puhaystack = uhaystack + pos;

    while ( uhaystack_end - puhaystack < uneedle_len ) {

        pos = ubrk_previous(bi);

        if ( UBRK_DONE == pos ) {
            break;
        }

        puhaystack = uhaystack + pos;
    }

    /* is there enough haystack left to hold the needle? */
    if ( ( uhaystack_end - puhaystack ) < uneedle_len ) {
        /* not enough, not found */
        goto exit;
    }

    while ( UBRK_DONE != pos ) {

        if (!u_memcmp(uneedle, puhaystack, uneedle_len)) {  /* needle_len - 1 in zend memnstr? */

            /* does the grapheme in the haystack end at the same place as the last grapheme in the needle? */

            if ( ubrk_isBoundary(bi, pos + uneedle_len) ) {

                /* found it, get grapheme count offset */
                ret_pos = grapheme_count_graphemes(bi, uhaystack, pos);
                break;
            }

            /* set position back */
            ubrk_isBoundary(bi, pos);
        }

        pos = ubrk_previous(bi);
        puhaystack = uhaystack + pos;
    }

exit:
    efree( uhaystack );
    efree( uneedle );
    ubrk_close (bi);

    return ret_pos;
}

/* }}} */

/* {{{ grapheme_strpos_utf16 - strrpos using utf16*/
int
grapheme_strpos_utf16(unsigned char *haystack, int32_t haystack_len, unsigned char*needle, int32_t needle_len, int32_t offset, int32_t *puchar_pos, int f_ignore_case TSRMLS_DC)
{
	UChar *uhaystack, *puhaystack, *uneedle;
	int32_t uhaystack_len, uneedle_len;
	int ret_pos;
	unsigned char u_break_iterator_buffer[U_BRK_SAFECLONE_BUFFERSIZE];
	UBreakIterator* bi;
	UErrorCode status;

	*puchar_pos = -1;

	/* convert the strings to UTF-16. */

	uhaystack = NULL;
	uhaystack_len = 0;
	status = U_ZERO_ERROR;
	intl_convert_utf8_to_utf16(&uhaystack, &uhaystack_len, (char *) haystack, haystack_len, &status );

	if ( U_FAILURE( status ) ) {
		/* Set global error code. */
		intl_error_set_code( NULL, status TSRMLS_CC );

		/* Set error messages. */
		intl_error_set_custom_msg( NULL, "Error converting input string to UTF-16", 0 TSRMLS_CC );
		efree( uhaystack );
		return -1;
	}

	/* get a pointer to the haystack taking into account the offset */
	bi = NULL;
	status = U_ZERO_ERROR;
	bi = grapheme_get_break_iterator(u_break_iterator_buffer, &status TSRMLS_CC );
	
	puhaystack = grapheme_get_haystack_offset(bi, uhaystack, uhaystack_len, offset);
	uhaystack_len = (uhaystack_len - ( puhaystack - uhaystack));

	if ( NULL == puhaystack ) {
	
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "grapheme_strpos: Offset not contained in string", 1 TSRMLS_CC );
		
		efree( uhaystack );
		ubrk_close (bi);
					
		return -1;
	}

	if ( f_ignore_case ) {
		grapheme_intl_case_fold(&uhaystack, &puhaystack, &uhaystack_len, &status );
	}

	uneedle = NULL;
	uneedle_len = 0;
	status = U_ZERO_ERROR;
	intl_convert_utf8_to_utf16(&uneedle, &uneedle_len, (char *) needle, needle_len, &status );

	if ( U_FAILURE( status ) ) {
		/* Set global error code. */
		intl_error_set_code( NULL, status TSRMLS_CC );

		/* Set error messages. */
		intl_error_set_custom_msg( NULL, "Error converting input string to UTF-16", 0 TSRMLS_CC );
		efree( uhaystack );
		efree( uneedle );
		ubrk_close (bi);
		
		return -1;
	}

	if ( f_ignore_case ) {
		grapheme_intl_case_fold(&uneedle, &uneedle, &uneedle_len, &status );
	}

	ret_pos = grapheme_memnstr_grapheme(bi, puhaystack, uneedle, uneedle_len, puhaystack + uhaystack_len );
	
	*puchar_pos = ubrk_current(bi);

	efree( uhaystack );
	efree( uneedle );
	ubrk_close (bi);

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
inline int32_t
grapheme_count_graphemes(UBreakIterator *bi, UChar *string, int32_t string_len)
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

/* {{{ grapheme_memnstr_grapheme: find needle in haystack using grapheme boundaries */
inline int32_t
grapheme_memnstr_grapheme(UBreakIterator *bi, UChar *haystack, UChar *needle, int32_t needle_len, UChar *end)
{
	UChar *p = haystack;
	UChar ne = needle[needle_len-1];
	UErrorCode status;
	int32_t grapheme_offset;
	
	end -= needle_len;

	while (p <= end) {

		if ((p = u_memchr(p, *needle, (end-p+1))) && ne == p[needle_len-1]) {

			if (!u_memcmp(needle, p, needle_len - 1)) {  /* needle_len - 1 works because if needle_len is 1, we've already tested the char */

				/* does the grapheme end here? */

				status = U_ZERO_ERROR;
				ubrk_setText (bi, haystack, (end - haystack) + needle_len, &status);

				if ( ubrk_isBoundary (bi, (p - haystack) + needle_len) ) {

					/* found it, get grapheme count offset */
					grapheme_offset = grapheme_count_graphemes(bi, haystack, (p - haystack));

					return grapheme_offset;
				}
			}
		}

		if (p == NULL) {
			return -1;
		}

		p++;
	}

	return -1;
}

/* }}} */

/* {{{ grapheme_memrstr_grapheme: reverse find needle in haystack using grapheme boundaries */
inline void *grapheme_memrchr_grapheme(const void *s, int c, int32_t n)
{
	register unsigned char *e;

	if (n <= 0) {
		return NULL;
	}

	for (e = (unsigned char *)s + n - 1; e >= (unsigned char *)s; e--) {
		if (*e == (unsigned char)c) {
			return (void *)e;
		}
	}

	return NULL;
}
/* }}} */

/* {{{ 	grapheme_get_haystack_offset - bump the haystack pointer based on the grapheme count offset */
UChar *
grapheme_get_haystack_offset(UBreakIterator* bi, UChar *uhaystack, int32_t uhaystack_len, int32_t offset)
{
	UErrorCode		status;
	int32_t pos;
	int32_t (*iter_op)(UBreakIterator* bi);
	int iter_incr;

	if ( NULL != bi ) {
		status = U_ZERO_ERROR;
		ubrk_setText (bi, uhaystack, uhaystack_len, &status);
	}

	if ( 0 == offset ) {
		return uhaystack;
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
		return NULL;
	}
	
	return uhaystack + pos;
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
UBreakIterator* 
grapheme_get_break_iterator(void *stack_buffer, UErrorCode *status TSRMLS_DC )
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

