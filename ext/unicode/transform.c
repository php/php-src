/*
   +----------------------------------------------------------------------+
   | PHP Version 6                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andrei Zmievski <andrei@php.net>                            |
   +----------------------------------------------------------------------+
 */

/* $Id$ */ 

#include "php_unicode.h"

PHP_FUNCTION(transliterate)
{
	UChar	   *str, *from, *to, *variant = NULL;
	int			str_len, from_len, to_len, variant_len = 0;
	UChar		id[256];
	int			id_len;
	UChar	   *result = NULL;
	int			result_len = 0;
	UTransliterator *trans = NULL;
	UErrorCode	status = U_ZERO_ERROR;
	int32_t		capacity, start, limit;

	/*
	{

		char *str;
		int32_t str_len;
		UEnumeration *ids;

		ids = utrans_openIDs(&status);
		printf("%d\n", uenum_count(ids, &status));
		str = (char*)uenum_next(ids, &str_len, &status);
		while (str) {
			printf("id: %s\n", str);
			str = (char*)uenum_next(ids, &str_len, &status);
		}
		uenum_close(ids);
	}

	return;
	*/

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "uuu|u", &str,
							  &str_len, &from, &from_len, &to, &to_len,
							  &variant, &variant_len) == FAILURE) {
		return;
	}

	if (variant) {
		id_len = u_snprintf(id, sizeof(id)-1, "%S-%S/%S", from, to, variant);
	} else {
		id_len = u_snprintf(id, sizeof(id)-1, "%S-%S", from, to);
	}

	if (id_len < 0) {
		php_error_docref("", E_WARNING, "Transliterator ID too long");
		return;
	}

	id[id_len] = 0;

	trans = utrans_openU(id, id_len, UTRANS_FORWARD, NULL, 0, NULL, &status);
	if (U_FAILURE(status)) {
		php_error_docref("", E_WARNING, "Failed to create transliterator");
		return;
	}

	result = eustrndup(str, str_len);
	result_len = limit = str_len;
	capacity = str_len + 1;

	while (1) {
		utrans_transUChars(trans, result, &result_len, capacity, 0, &limit, &status);
		if (status == U_BUFFER_OVERFLOW_ERROR) {
			result = eurealloc(result, result_len + 1);
			memcpy(result, str, UBYTES(str_len));
			capacity = result_len + 1;
			result_len = limit = str_len;
			status = U_ZERO_ERROR;
			utrans_transUChars(trans, result, &result_len, capacity, 0, &limit, &status);
		} else {
			if (status == U_STRING_NOT_TERMINATED_WARNING) {
				result = eurealloc(result, result_len + 1);
			}
			break;
		}
	}

	result[result_len] = 0;
	utrans_close(trans);

	RETURN_UNICODEL(result, result_len, 0);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
