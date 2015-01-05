/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>

#include "intl_common.h"
#include "intl_convert.h"

/* {{{ intl_convert_utf8_to_utf16
 * Convert given string from UTF-8 to UTF-16 to *target buffer.
 *
 * It *target is NULL then we allocate a large enough buffer,
 * store the converted string into it, and make target point to it.
 *
 * Otherwise, if *target is non-NULL, we assume that it points to a
 * dynamically allocated buffer of *target_len bytes length.
 * In this case the buffer will be used to store the converted string to,
 * and may be resized (made larger) if needed.
 *
 * Note that ICU uses int32_t as string length and PHP uses size_t. While
 * it is not likely in practical situations to have strings longer than
 * INT32_MAX, these are different types and need to be handled carefully.
 *
 * @param target      Where to place the result.
 * @param target_len  Result length.
 * @param source      String to convert.
 * @param source_len  Length of the source string.
 * @param status      Conversion status.
 *
 * @return void       This function does not return anything.
 */
void intl_convert_utf8_to_utf16(
	UChar**     target, int32_t* target_len,
	const char* src,    size_t  src_len,
	UErrorCode* status )
{
	UChar*      dst_buf = NULL;
	int32_t     dst_len = 0;

	/* If *target is NULL determine required destination buffer size (pre-flighting).
	 * Otherwise, attempt to convert source string; if *target buffer is not large enough
	 * it will be resized appropriately.
	 */
	*status = U_ZERO_ERROR;

	if(src_len > INT32_MAX) {
		/* we can not fit this string */
		*status = U_BUFFER_OVERFLOW_ERROR;
		return;
	}

	u_strFromUTF8( *target, *target_len, &dst_len, src, (int32_t)src_len, status );

	if( *status == U_ZERO_ERROR )
	{
		/* String is converted successfully */
		(*target)[dst_len] = 0;
		*target_len = dst_len;
		return;
	}

	/* Bail out if an unexpected error occurred.
	 * (U_BUFFER_OVERFLOW_ERROR means that *target buffer is not large enough).
	 * (U_STRING_NOT_TERMINATED_WARNING usually means that the input string is empty).
	 */
	if( *status != U_BUFFER_OVERFLOW_ERROR && *status != U_STRING_NOT_TERMINATED_WARNING )
		return;

	/* Allocate memory for the destination buffer (it will be zero-terminated). */
	dst_buf = eumalloc( dst_len + 1 );

	/* Convert source string from UTF-8 to UTF-16. */
	*status = U_ZERO_ERROR;
	u_strFromUTF8( dst_buf, dst_len+1, NULL, src, src_len, status );
	if( U_FAILURE( *status ) )
	{
		efree( dst_buf );
		return;
	}

	dst_buf[dst_len] = 0;

	if( *target )
		efree( *target );

	*target     = dst_buf;
	*target_len = dst_len;
}
/* }}} */

/* {{{ intl_convert_utf16_to_utf8
 * Convert given string from UTF-16 to UTF-8.
 *
 * @param target      Where to place the result.
 * @param target_len  Result length.
 * @param source      String to convert.
 * @param source_len  Length of the source string.
 * @param status      Conversion status.
 *
 * @return void       This function does not return anything.
 */
void intl_convert_utf16_to_utf8(
	char**       target, size_t* target_len,
	const UChar* src,    int32_t  src_len,
	UErrorCode*  status )
{
	char*       dst_buf = NULL;
	int32_t     dst_len;

	/* Determine required destination buffer size (pre-flighting). */
	*status = U_ZERO_ERROR;
	u_strToUTF8( NULL, 0, &dst_len, src, src_len, status );

	/* Bail out if an unexpected error occurred.
	 * (U_BUFFER_OVERFLOW_ERROR means that *target buffer is not large enough).
	 * (U_STRING_NOT_TERMINATED_WARNING usually means that the input string is empty).
	 */
	if( *status != U_BUFFER_OVERFLOW_ERROR && *status != U_STRING_NOT_TERMINATED_WARNING )
		return;

	/* Allocate memory for the destination buffer (it will be zero-terminated). */
	dst_buf = emalloc( dst_len+1 );

	/* Convert source string from UTF-8 to UTF-16. */
	*status = U_ZERO_ERROR;
	u_strToUTF8( dst_buf, dst_len, NULL, src, src_len, status );
	if( U_FAILURE( *status ) )
	{
		efree( dst_buf );
		return;
	}

	/* U_STRING_NOT_TERMINATED_WARNING is OK for us => reset 'status'. */
	*status = U_ZERO_ERROR;

	dst_buf[dst_len] = 0;
	*target     = dst_buf;
	*target_len = (size_t)dst_len;
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
