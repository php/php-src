/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   | (with helpful hints from Dean Gaudet <dgaudet@arctic.org>            |
   | PHP 4.0 patches by:                                                  |
   | Zeev Suraski <zeev@zend.com>                                         |
   | Stig Bakken <ssb@fast.no>                                            |
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
	// so let zend open it. 
	*/
	
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
	
	zend_try {
		php_request_shutdown(NULL);
	} zend_end_try();
	
	return (OK);
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
