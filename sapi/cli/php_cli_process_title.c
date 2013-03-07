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
  | Author: Keyur Govande (kgovande@gmail.com)                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_cli_process_title.h"
#include "ps_title.h"

/* {{{ proto boolean cli_set_process_title(string arg)
   Return a boolean to confirm if the process title was successfully changed or not */
PHP_FUNCTION(cli_set_process_title)
{
    char *title = NULL;
    int title_len;
    int rc;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &title, &title_len) == FAILURE) {
        return;
    }

    rc = set_ps_title(title);
    if (rc == PS_TITLE_SUCCESS) {
        RETURN_TRUE;
    }

    php_error_docref(NULL TSRMLS_CC, E_WARNING, "cli_set_process_title had an error: %s", ps_title_errno(rc));
    RETURN_FALSE;
}
/* }}} */

/* {{{ proto string cli_get_process_title()
   Return a string with the current process title. NULL if error. */
PHP_FUNCTION(cli_get_process_title)
{
        int length = 0;
        const char* title = NULL;
        int rc;

        if (zend_parse_parameters_none() == FAILURE) {
            return;
        }

        rc = get_ps_title(&length, &title);
        if (rc != PS_TITLE_SUCCESS) {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "cli_get_process_title had an error: %s", ps_title_errno(rc));
                RETURN_NULL();
        }

        RETURN_STRINGL(title, length, 1);
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
