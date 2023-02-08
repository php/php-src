/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Tyson Andre <tandre@php.net>                                |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_COLLECTIONS_H
#define PHP_COLLECTIONS_H

#include "php.h"
#include <stdarg.h>

#define PHP_COLLECTIONS_VERSION PHP_VERSION

extern zend_module_entry collections_module_entry;
#define phpext_collections_ptr &collections_module_entry

PHP_MINIT_FUNCTION(collections);
PHP_MINFO_FUNCTION(collections);

#endif /* PHP_COLLECTIONS_H */
