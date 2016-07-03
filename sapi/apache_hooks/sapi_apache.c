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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   | (with helpful hints from Dean Gaudet <dgaudet@arctic.org>            |
   | PHP 4.0 patches by:                                                  |
   | Zeev Suraski <zeev@zend.com>                                         |
   | Stig Bakken <ssb@php.net>                                            |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include "php_apache_http.h"

/* {{{ apache_php_module_main
 */
int apache_php_module_main(request_rec *r, int display_source_mode TSRMLS_DC)
{
	zend_file_handle file_handle;

	if (php_request_startup(TSRMLS_C) == FAILURE) {
		return FAILURE;
	}
	/* sending a file handle to another dll is not working
	   so let zend open it. */
	
	if (display_source_mode) {
		zend_syntax_highlighter_ini syntax_highlighter_ini;

		php_get_highlight_struct(&syntax_highlighter_ini);
		if (highlight_file(SG(request_info).path_translated, &syntax_highlighter_ini TSRMLS_CC)){
			return OK;
		} else {
			return NOT_FOUND;
		}
	} else {
		file_handle.type = ZEND_HANDLE_FILENAME;
		file_handle.handle.fd = 0;
		file_handle.filename = SG(request_info).path_translated;
		file_handle.opened_path = NULL;
		file_handle.free_filename = 0;
		(void) php_execute_script(&file_handle TSRMLS_CC);
	}
	AP(in_request) = 0;
	
	return (OK);
}
/* }}} */

/* {{{ apache_php_module_hook
 */
int apache_php_module_hook(request_rec *r, php_handler *handler, zval **ret TSRMLS_DC)
{
	zend_file_handle file_handle;
	zval *req;
    char *tmp;

#if PHP_SIGCHILD
	signal(SIGCHLD, sigchld_handler);
#endif
    if(AP(current_hook) == AP_RESPONSE) {
        if (php_request_startup_for_hook(TSRMLS_C) == FAILURE)
            return FAILURE;
    }
    else {
        if (php_request_startup_for_hook(TSRMLS_C) == FAILURE)
            return FAILURE;
    }

    req = php_apache_request_new(r);
    php_register_variable_ex("request", req, PG(http_globals)[TRACK_VARS_SERVER] TSRMLS_CC);

    switch(handler->type) {
        case AP_HANDLER_TYPE_FILE:
            php_register_variable("PHP_SELF_HOOK", handler->name, PG(http_globals)[TRACK_VARS_SERVER] TSRMLS_CC);
	        memset(&file_handle, 0, sizeof(file_handle));
	        file_handle.type = ZEND_HANDLE_FILENAME;
	        file_handle.filename = handler->name;
	        (void) php_execute_simple_script(&file_handle, ret TSRMLS_CC);
            break;
        case AP_HANDLER_TYPE_METHOD:
            if( (tmp = strstr(handler->name, "::")) != NULL &&  *(tmp+2) != '\0' ) {
                zval *class;
                zval *method;
                *tmp = '\0';
                ALLOC_ZVAL(class);
                ZVAL_STRING(class, handler->name, 1);
                ALLOC_ZVAL(method);
                ZVAL_STRING(method, tmp +2, 1);
                *tmp = ':';
                call_user_function_ex(EG(function_table), &class, method, ret, 0, NULL, 0, NULL TSRMLS_CC);
                zval_dtor(class);
                zval_dtor(method);
            }
            else {
                php_error(E_ERROR, "Unable to call %s - not a Class::Method\n", handler->name);
                /* not a class::method */
            }
            break;
        default:
            /* not a valid type */
            assert(0);
            break;
    }
	zval_dtor(req);
	AP(in_request) = 0;

	return OK;
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
