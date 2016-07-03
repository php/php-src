/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author:                                                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "php.h"
#include "SAPI.h"
#include "rfc1867.h"

#include "php_content_types.h"

/* {{{ php_post_entries[]
 */
static sapi_post_entry php_post_entries[] = {
	{ DEFAULT_POST_CONTENT_TYPE, sizeof(DEFAULT_POST_CONTENT_TYPE)-1, sapi_read_standard_form_data,	php_std_post_handler },
	{ MULTIPART_CONTENT_TYPE,    sizeof(MULTIPART_CONTENT_TYPE)-1,    NULL,                         rfc1867_post_handler },
	{ NULL, 0, NULL, NULL }
};
/* }}} */

static zend_bool populate_raw_post_data(TSRMLS_D)
{
	if (!SG(request_info).request_body) {
		return (zend_bool) 0;
	}

	if (!PG(always_populate_raw_post_data)) {
		return (zend_bool) !SG(request_info).post_entry;
	}

	return (zend_bool) (PG(always_populate_raw_post_data) > 0);
}

/* {{{ SAPI_POST_READER_FUNC
 */
SAPI_API SAPI_POST_READER_FUNC(php_default_post_reader)
{
	if (!strcmp(SG(request_info).request_method, "POST")) {
		if (NULL == SG(request_info).post_entry) {
			/* no post handler registered, so we just swallow the data */
			sapi_read_standard_form_data(TSRMLS_C);
		}

		if (populate_raw_post_data(TSRMLS_C)) {
			size_t length;
			char *data = NULL;

			php_stream_rewind(SG(request_info).request_body);
			length = php_stream_copy_to_mem(SG(request_info).request_body, &data, PHP_STREAM_COPY_ALL, 0);
			php_stream_rewind(SG(request_info).request_body);

			if (length > INT_MAX) {
				sapi_module.sapi_error(E_WARNING,
					"HTTP_RAW_POST_DATA truncated from %lu to %d bytes",
					(unsigned long) length, INT_MAX);
				length = INT_MAX;
			}
			if (!data) {
				data = STR_EMPTY_ALLOC();
			}
			SET_VAR_STRINGL("HTTP_RAW_POST_DATA", data, length);

			sapi_module.sapi_error(E_DEPRECATED,
				"Automatically populating $HTTP_RAW_POST_DATA is deprecated and "
				"will be removed in a future version. To avoid this warning set "
				"'always_populate_raw_post_data' to '-1' in php.ini and use the "
				"php://input stream instead.");
		}
	}
}
/* }}} */

/* {{{ php_startup_sapi_content_types
 */
int php_startup_sapi_content_types(TSRMLS_D)
{
	sapi_register_default_post_reader(php_default_post_reader TSRMLS_CC);
	sapi_register_treat_data(php_default_treat_data TSRMLS_CC);
	sapi_register_input_filter(php_default_input_filter, NULL TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

/* {{{ php_setup_sapi_content_types
 */
int php_setup_sapi_content_types(TSRMLS_D)
{
	sapi_register_post_entries(php_post_entries TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
