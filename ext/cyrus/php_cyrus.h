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
   | Author: Sterling Hughes <sterling@php.net>                           |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_CYRUS_H
#define PHP_CYRUS_H

#include "php.h"

#if HAVE_CYRUS

#include <cyrus/imclient.h>

extern zend_module_entry cyrus_module_entry;
#define phpext_cyrus_ptr &cyrus_module_entry

#ifdef PHP_WIN32
#define PHP_CYRUS_API __declspec(dllexport)
#else
#define PHP_CYRUS_API
#endif

PHP_MINIT_FUNCTION(cyrus);
PHP_MINFO_FUNCTION(cyrus);

PHP_FUNCTION(cyrus_connect);
PHP_FUNCTION(cyrus_authenticate);
PHP_FUNCTION(cyrus_bind);
PHP_FUNCTION(cyrus_unbind);
PHP_FUNCTION(cyrus_query);
PHP_FUNCTION(cyrus_close);

typedef struct {
	struct imclient *client;
	char            *host;
	char            *port;
	int              flags;
	int              id;
}
php_cyrus;

typedef struct {
	zval *function;
	char *trigger;
	long  le;
	int   flags;
}
php_cyrus_callback;

#endif


#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
