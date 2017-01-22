/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andrei Zmievski <andrei@php.net>                             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifndef PHP_WDDX_H
#define PHP_WDDX_H

#if HAVE_WDDX

extern zend_module_entry wddx_module_entry;
#define wddx_module_ptr &wddx_module_entry

#include "php_version.h"
#define PHP_WDDX_VERSION PHP_VERSION

PHP_FUNCTION(wddx_serialize_value);
PHP_FUNCTION(wddx_serialize_vars);
PHP_FUNCTION(wddx_packet_start);
PHP_FUNCTION(wddx_packet_end);
PHP_FUNCTION(wddx_add_vars);
PHP_FUNCTION(wddx_deserialize);

#else

#define wddx_module_ptr NULL

#endif /* HAVE_WDDX */

#define phpext_wddx_ptr wddx_module_ptr

#endif /* !PHP_WDDX_H */
