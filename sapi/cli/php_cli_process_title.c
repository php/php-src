/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
  +----------------------------------------------------------------------+
  | Author: Keyur Govande (kgovande@gmail.com)                           |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "php_cli_process_title.h"
#include "ps_title.h"

/* {{{ Return a boolean to confirm if the process title was successfully changed or not */
PHP_FUNCTION(cli_set_process_title)
{
	char *title = NULL;
	size_t title_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &title, &title_len) == FAILURE) {
		RETURN_THROWS();
	}

	ps_title_status rc = set_ps_title(title, title_len);
	if (rc == PS_TITLE_SUCCESS) {
		RETURN_TRUE;
	}

	php_error_docref(NULL, E_WARNING, "cli_set_process_title had an error: %s", ps_title_errno(rc));
	RETURN_FALSE;
}
/* }}} */

/* {{{ Return a string with the current process title. NULL if error. */
PHP_FUNCTION(cli_get_process_title)
{
	size_t length = 0;
	const char* title = NULL;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	ps_title_status rc = get_ps_title(&length, &title);
	if (rc != PS_TITLE_SUCCESS) {
			php_error_docref(NULL, E_WARNING, "cli_get_process_title had an error: %s", ps_title_errno(rc));
			RETURN_NULL();
	}

	RETURN_STRINGL(title, length);
}
/* }}} */
