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
 */

#ifndef PHP_DIO_H
#define PHP_DIO_H

extern zend_module_entry dio_module_entry;
#define phpext_dio_ptr &dio_module_entry

#ifdef PHP_WIN32
#define PHP_DIO_API __declspec(dllexport)
#else
#define PHP_DIO_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(dio);
PHP_MSHUTDOWN_FUNCTION(dio);
PHP_RINIT_FUNCTION(dio);
PHP_RSHUTDOWN_FUNCTION(dio);
PHP_MINFO_FUNCTION(dio);

PHP_FUNCTION(dio_open);
PHP_FUNCTION(dio_truncate);
PHP_FUNCTION(dio_stat);
PHP_FUNCTION(dio_seek);
PHP_FUNCTION(dio_read);
PHP_FUNCTION(dio_write);
PHP_FUNCTION(dio_fcntl);
PHP_FUNCTION(dio_close);
PHP_FUNCTION(dio_tcsetattr);

typedef struct {
	int fd;
} php_fd_t;

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
