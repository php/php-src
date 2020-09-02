/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_cli_process_title.h"
#include "ps_title.h"

/* {{{ Return a boolean to confirm if the process title was successfully changed or not */
PHP_FUNCTION(cli_set_process_title)
{
    char *title = NULL;
    size_t title_len;
    int rc;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &title, &title_len) == FAILURE) {
        RETURN_THROWS();
    }

    rc = set_ps_title(title);
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
        int length = 0;
        const char* title = NULL;
        int rc;

        if (zend_parse_parameters_none() == FAILURE) {
            RETURN_THROWS();
        }

        rc = get_ps_title(&length, &title);
        if (rc != PS_TITLE_SUCCESS) {
                php_error_docref(NULL, E_WARNING, "cli_get_process_title had an error: %s", ps_title_errno(rc));
                RETURN_NULL();
        }

        RETURN_STRINGL(title, length);
}
/* }}} */
