#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "php_intl.h"
#include "collator.h"
#include "collator_class.h"
#include "collator_string.h"
#include "intl_convert.h"
#include "grapheme/grapheme_util.h"

#include <unicode/usearch.h>

#undef COLLATOR_CHECK_STATUS
#define COLLATOR_CHECK_STATUS(ro, msg)                                \
	do {                                                              \
		intl_error_set_code(NULL, COLLATOR_ERROR_CODE(ro));           \
		if (U_FAILURE(COLLATOR_ERROR_CODE(ro))) {                     \
			intl_errors_set_custom_msg(COLLATOR_ERROR_P(ro), msg, 0); \
			goto end;                                                 \
		}                                                             \
	} while (0);

#define UTF8_TO_UTF16(ro, to, to_len, from, from_len)                                        \
	do {                                                                                     \
		to = NULL;                                                                           \
		to_len = 0;                                                                          \
		intl_convert_utf8_to_utf16(&to, &to_len, from, from_len, COLLATOR_ERROR_CODE_P(ro)); \
		COLLATOR_CHECK_STATUS(ro, "string conversion of " #from " to UTF-16 failed");        \
	} while (0);

#define UTF16_TO_UTF8(ro, to, from, from_len)                                        \
	do {                                                                             \
		to = intl_convert_utf16_to_utf8(from, from_len, COLLATOR_ERROR_CODE_P(ro));  \
		COLLATOR_CHECK_STATUS(ro, "string conversion of " #from " to UTF-8 failed"); \
	} while (0);

static void starts_or_ends_with(INTERNAL_FUNCTION_PARAMETERS, int first)
{
	UChar *uneedle = NULL;
	int32_t uneedle_len = 0;
	UChar *uhaystack = NULL;
	int32_t uhaystack_len = 0;
	UStringSearch *uss = NULL;
	UBreakIterator *ubrk = NULL;
	zend_string *haystack, *needle;

	COLLATOR_METHOD_INIT_VARS
	if ( FAILURE == zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "OSS", &object, Collator_ce_ptr, &haystack, &needle )) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "collator_replace: unable to parse input params", 0 );

		RETURN_FALSE;
	}
	COLLATOR_METHOD_FETCH_OBJECT;
	if (!co || !co->ucoll) {
		intl_error_set_code( NULL, COLLATOR_ERROR_CODE( co ) );
		intl_errors_set_custom_msg( COLLATOR_ERROR_P( co ), "Object not initialized", 0 );
		php_error_docref(NULL, E_RECOVERABLE_ERROR, "Object not initialized");

		RETURN_FALSE;
	}

	UTF8_TO_UTF16(co, uhaystack, uhaystack_len, ZSTR_VAL(haystack), ZSTR_LEN(haystack));
	UTF8_TO_UTF16(co, uneedle, uneedle_len, ZSTR_VAL(needle), ZSTR_LEN(needle));
	uss = usearch_openFromCollator(uneedle, uneedle_len, uhaystack, uhaystack_len, co->ucoll, NULL, COLLATOR_ERROR_CODE_P( co ));
	COLLATOR_CHECK_STATUS(co, "failed creating UStringSearch");

	if (first) {
		RETVAL_BOOL(0 == usearch_first(uss, COLLATOR_ERROR_CODE_P( co )));
		COLLATOR_CHECK_STATUS(co, "failed while searching");
	} else {
		int32_t lastMatch;

		lastMatch = usearch_last(uss, COLLATOR_ERROR_CODE_P( co ));
		COLLATOR_CHECK_STATUS(co, "failed while searching");
		RETVAL_BOOL(uhaystack_len == lastMatch + usearch_getMatchedLength(uss));
	}
	if (FALSE) {
end:
		RETVAL_FALSE;
	}
	if (NULL != ubrk) {
		ubrk_close(ubrk);
	}
	if (NULL != uneedle) {
		efree(uneedle);
	}
	if (NULL != uhaystack) {
		efree(uhaystack);
	}
	if (NULL != uss) {
		usearch_close(uss);
	}
}

/* {{{ proto bool Collator::startsWith( Collator $coll, string $string, string $prefix )
 * Does string begins with prefix? }}} */
/* {{{ proto bool collator_startswith( Collator $coll, string $string, string $prefix )
 * Does string begins with prefix?
 */
PHP_FUNCTION(collator_startswith)
{
	starts_or_ends_with(INTERNAL_FUNCTION_PARAM_PASSTHRU, TRUE);
}
/* }}} */

/* {{{ proto bool Collator::startsWith( Collator $coll, string $string, string $suffix )
 * Does string ends with suffix? }}} */
/* {{{ proto bool collator_startswith( Collator $coll, string $string, string $suffix )
 * Does string ends with suffix?
 */
PHP_FUNCTION(collator_endswith)
{
	starts_or_ends_with(INTERNAL_FUNCTION_PARAM_PASSTHRU, FALSE);
}
/* }}} */

static void collator_index(INTERNAL_FUNCTION_PARAMETERS, int search_first, int want_only_pos)
{
	int ret;
	int32_t cuoffset; /* Unit: UTF-16 CU */
	long startoffset = 0; /* Unit: grapheme */
	UChar *uneedle = NULL;
	int32_t uneedle_len = 0;
	UChar *uhaystack = NULL;
	int32_t uhaystack_len = 0;
	UStringSearch *uss = NULL;
	int32_t match_start = -1; /* Unit: UTF-16 CU */
	zend_bool before = FALSE;
	UBreakIterator *ubrk = NULL;
	zend_string *haystack, *needle;
	unsigned char u_break_iterator_buffer[U_BRK_SAFECLONE_BUFFERSIZE];

	COLLATOR_METHOD_INIT_VARS
	if (want_only_pos) {
		ret = zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OSS|l", &object, Collator_ce_ptr, &haystack, &needle, &startoffset);
	} else {
		ret = zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "OSS|lb", &object, Collator_ce_ptr, &haystack, &needle, &startoffset, &before);
	}
	if (FAILURE == ret) {
		intl_error_set(NULL, U_ILLEGAL_ARGUMENT_ERROR, "unable to parse input params", 0);

		RETURN_FALSE;
	}
	COLLATOR_METHOD_FETCH_OBJECT;
	if (!co || !co->ucoll) {
		intl_error_set_code( NULL, COLLATOR_ERROR_CODE( co ) );
		intl_errors_set_custom_msg( COLLATOR_ERROR_P( co ), "Object not initialized", 0 );
		php_error_docref(NULL, E_RECOVERABLE_ERROR, "Object not initialized");

		RETURN_FALSE;
	}

	UTF8_TO_UTF16(co, uhaystack, uhaystack_len, ZSTR_VAL(haystack), ZSTR_LEN(haystack));
	UTF8_TO_UTF16(co, uneedle, uneedle_len, ZSTR_VAL(needle), ZSTR_LEN(needle));
	ubrk = grapheme_get_break_iterator((void*) u_break_iterator_buffer, COLLATOR_ERROR_CODE_P( co ));
	COLLATOR_CHECK_STATUS(co, "failed cloning UBreakIterator");
	ubrk_setText(ubrk, uhaystack, uhaystack_len, COLLATOR_ERROR_CODE_P( co ));
	COLLATOR_CHECK_STATUS(co, "failed binding text to UBreakIterator");
	if (0 != startoffset) {
		if (UBRK_DONE == (cuoffset = utf16_grapheme_to_cu(ubrk, startoffset))) {
			intl_error_set(NULL, U_INDEX_OUTOFBOUNDS_ERROR, "offset is out of bounds", 0);
			goto end;
		}
	} else {
		if (search_first) {
			cuoffset = 0;
		} else {
			cuoffset = uhaystack_len;
		}
	}
	uss = usearch_openFromCollator(uneedle, uneedle_len, uhaystack, uhaystack_len, co->ucoll, ubrk, COLLATOR_ERROR_CODE_P( co ));
	COLLATOR_CHECK_STATUS(co, "failed creating UStringSearch");

	if (search_first) {
		match_start = usearch_following(uss, cuoffset, COLLATOR_ERROR_CODE_P( co ));
	} else {
		usearch_setAttribute(uss, USEARCH_OVERLAP, USEARCH_ON, COLLATOR_ERROR_CODE_P( co ));
		COLLATOR_CHECK_STATUS(co, "failed switching overlap attribute to on");
		match_start = usearch_preceding(uss, cuoffset, COLLATOR_ERROR_CODE_P( co ));
	}
	COLLATOR_CHECK_STATUS(co, "failed while searching");
	if (USEARCH_DONE != match_start) {
		if (want_only_pos) {
			RETVAL_LONG((long) utf16_cu_to_grapheme(ubrk, match_start));
		} else {
			zend_string *result;

			if (before) {
				UTF16_TO_UTF8(co, result, uhaystack, match_start);
			} else {
				UTF16_TO_UTF8(co, result, uhaystack + match_start, uhaystack_len - match_start);
			}
			RETVAL_STR(result);
		}
	} else {
		if (want_only_pos) {
			RETVAL_LONG((long) -1);
		} else {
			RETVAL_FALSE;
		}
	}

	if (FALSE) {
end:
		RETVAL_FALSE;
	}
	if (NULL != ubrk) {
		ubrk_close(ubrk);
	}
	if (NULL != uneedle) {
		efree(uneedle);
	}
	if (NULL != uhaystack) {
		efree(uhaystack);
	}
	if (NULL != uss) {
		usearch_close(uss);
	}
}

/* {{{ proto string Collator::rindex( Collator $coll, string $haystack, string $needle [, int $start ] )
 * Returns the index of the rightmost occurrence of the given substring }}} */
/* {{{ proto string collator_rindex( Collator $coll, string $haystack, string $needle [, int $start ] )
 * Returns the index of the rightmost occurrence of the given substring
 */
PHP_FUNCTION(collator_rindex)
{
	collator_index(INTERNAL_FUNCTION_PARAM_PASSTHRU, FALSE, TRUE);
}
/* }}} */

/* {{{ proto string Collator::lindex( Collator $coll, string $haystack, string $needle [, int $start ] )
 * Returns the index of the leftmost occurrence of the given substring }}} */
/* {{{ proto string collator_lindex( Collator $coll, string $haystack, string $needle [, int $start ] )
 * Returns the index of the leftmost occurrence of the given substring
 */
PHP_FUNCTION(collator_lindex)
{
	collator_index(INTERNAL_FUNCTION_PARAM_PASSTHRU, TRUE, TRUE);
}
/* }}} */

/* {{{ proto string Collator::rfind( Collator $coll, string $haystack, string $needle [, int $start [, bool $before ] ] )
 * Returns part of haystack string starting from and including the rightmost occurrence of needle to the end of haystack }}} */
/* {{{ proto string collator_rfind( Collator $coll, string $haystack, string $needle [, int $start [, bool $before ] ] )
 * Returns part of haystack string starting from and including the rightmost occurrence of needle to the end of haystack
 */
PHP_FUNCTION(collator_rfind)
{
	collator_index(INTERNAL_FUNCTION_PARAM_PASSTHRU, FALSE, FALSE);
}
/* }}} */

/* {{{ proto string Collator::lfind( Collator $coll, string $haystack, string $needle [, int $start [, bool $before ] ] )
 * Returns part of haystack string starting from and including the leftmost occurrence of needle to the end of haystack }}} */
/* {{{ proto string collator_lfind( Collator $coll, string $haystack, string $needle [, int $start [, bool $before ] ] )
 * Returns part of haystack string starting from and including the leftmost occurrence of needle to the end of haystack
 */
PHP_FUNCTION(collator_lfind)
{
	collator_index(INTERNAL_FUNCTION_PARAM_PASSTHRU, TRUE, FALSE);
}
/* }}} */

typedef enum {
	REPLACE_FORWARD,
	REPLACE_REVERSE
} ReplacementDirection;

static void utf16_replace_len(
    UChar **ucopy, int32_t *ucopy_len,
    UChar *ureplacement, int32_t ureplacement_len,
    UChar *ustring /* UNUSED */, int32_t ustring_len, /* Used by ICU, don't alter it ! So, we work on a copy. */
    int32_t start_match_offset, int32_t match_cu_length,
    ReplacementDirection direction
) {
	int32_t diff_len;
	int32_t real_offset;

	if (REPLACE_REVERSE == direction) {
		real_offset = start_match_offset;
	} else {
		real_offset = *ucopy_len - (ustring_len - start_match_offset);
	}
	diff_len = ureplacement_len - match_cu_length;
	if (diff_len > 0) {
		*ucopy = safe_erealloc(*ucopy, *ucopy_len + diff_len, sizeof(**ucopy), 1 * sizeof(**ucopy)); // reference may no longer be valid from this point
	}
	if (ureplacement_len != match_cu_length) {
		u_memmove(*ucopy + real_offset + match_cu_length + diff_len, *ucopy + real_offset + match_cu_length, *ucopy_len - real_offset - match_cu_length);
	}
	u_memcpy(*ucopy + real_offset, ureplacement, ureplacement_len);
	*ucopy_len += diff_len;
}

/* {{{ proto string Collator::replace( Collator $coll, string $subject, string $search, string $replacement [, int &$count ] )
 * Replace all matches of search, according to collation, by replacement in subject. }}} */
/* {{{ proto string collator_replace(  Collator $coll, string $subject, string $search, string $replacement [, int &$count ] )
 * Replace all matches of search, according to collation, by replacement in subject.
 */
PHP_FUNCTION(collator_replace)
{
	int32_t l;
	zval *zcount = NULL;
	long count = 0;
	UChar *usearch = NULL;
	int32_t usearch_len = 0;
	UChar *usubject = NULL;
	int32_t usubject_len = 0;
	UStringSearch *uss = NULL;
	UChar *ureplace = NULL;
	int32_t ureplace_len = 0;
	UChar *uresult = NULL;
	int32_t uresult_len = 0;
	zend_string *search, *replace, *subject, *result = NULL;

	COLLATOR_METHOD_INIT_VARS
	if ( FAILURE == zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "OSSS|z/", &object, Collator_ce_ptr, &subject, &search, &replace, &zcount )) {
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR, "collator_replace: unable to parse input params", 0 );

		RETURN_FALSE;
	}
	COLLATOR_METHOD_FETCH_OBJECT;
	if (!co || !co->ucoll) {
		intl_error_set_code( NULL, COLLATOR_ERROR_CODE( co ) );
		intl_errors_set_custom_msg( COLLATOR_ERROR_P( co ), "Object not initialized", 0 );
		php_error_docref(NULL, E_RECOVERABLE_ERROR, "Object not initialized");

		RETURN_FALSE;
	}

	UTF8_TO_UTF16(co, usearch, usearch_len, ZSTR_VAL(search), ZSTR_LEN(search));
	UTF8_TO_UTF16(co, usubject, usubject_len, ZSTR_VAL(subject), ZSTR_LEN(subject));
	uss = usearch_openFromCollator(usearch, usearch_len, usubject, usubject_len, co->ucoll, NULL, COLLATOR_ERROR_CODE_P( co ));
	COLLATOR_CHECK_STATUS(co, "failed creating UStringSearch");
	UTF8_TO_UTF16(co, ureplace, ureplace_len, ZSTR_VAL(replace), ZSTR_LEN(replace));
	uresult_len = usubject_len;
	uresult = safe_emalloc(usubject_len, sizeof(*usubject), 1 * sizeof(*usubject));
	u_memcpy(uresult, usubject, usubject_len);
	uresult[uresult_len] = 0;
	for (l = usearch_first(uss, COLLATOR_ERROR_CODE_P( co )); U_SUCCESS(COLLATOR_ERROR_CODE( co )) && USEARCH_DONE != l; l = usearch_next(uss, COLLATOR_ERROR_CODE_P( co )), count++) {
		utf16_replace_len(&uresult, &uresult_len, ureplace, ureplace_len, usubject, usubject_len, l, usearch_getMatchedLength(uss), REPLACE_FORWARD);
	}
	COLLATOR_CHECK_STATUS(co, "failed while searching");
	UTF16_TO_UTF8(co, result, uresult, uresult_len);
	RETVAL_STR(result);

	if (FALSE) {
end:
		if (NULL != result) {
			zend_string_release(result);
		}
		RETVAL_FALSE;
	}
	if (NULL != uss) {
		usearch_close(uss);
	}
	if (NULL != uresult) {
		efree(uresult);
	}
	if (NULL != ureplace) {
		efree(ureplace);
	}
	if (NULL != usearch) {
		efree(usearch);
	}
	if (NULL != usubject) {
		efree(usubject);
	}
	if (ZEND_NUM_ARGS() > 3) {
		zval_dtor(zcount);
		ZVAL_LONG(zcount, count);
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
