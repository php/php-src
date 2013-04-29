/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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

/* {{{ SAPI_POST_READER_FUNC
 */
SAPI_API SAPI_POST_READER_FUNC(php_default_post_reader)
{
	char *data;
	int length;

	/* $HTTP_RAW_POST_DATA registration */
	if (!strcmp(SG(request_info).request_method, "POST")) {
		if (NULL == SG(request_info).post_entry) {
			/* no post handler registered, so we just swallow the data */
			sapi_read_standard_form_data(TSRMLS_C);
		}

		/* For unknown content types we create HTTP_RAW_POST_DATA even if always_populate_raw_post_data off,
		 * this is in-effecient, but we need to keep doing it for BC reasons (for now) */
		if ((PG(always_populate_raw_post_data) || NULL == SG(request_info).post_entry) && SG(request_info).post_data) {
			length = SG(request_info).post_data_length;
			data = estrndup(SG(request_info).post_data, length);
			SET_VAR_STRINGL("HTTP_RAW_POST_DATA", data, length);
		}
	}

	/* for php://input stream:
	 some post handlers modify the content of request_info.post_data
	 so for now we need a copy for the php://input stream
	 in the long run post handlers should be changed to not touch
	 request_info.post_data for memory preservation reasons
	*/
	if (SG(request_info).post_data) {
		SG(request_info).raw_post_data = estrndup(SG(request_info).post_data, SG(request_info).post_data_length);
		SG(request_info).raw_post_data_length = SG(request_info).post_data_length;
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
