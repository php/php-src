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
   | Author: Kristian Koehntopp <kris@koehntopp.de>                       |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_POSIX_H
#define PHP_POSIX_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_POSIX
#ifndef DLEXPORT
#define DLEXPORT
#endif

extern zend_module_entry posix_module_entry;
#define posix_module_ptr &posix_module_entry

#include "php_version.h"
#define PHP_POSIX_VERSION PHP_VERSION

ZEND_BEGIN_MODULE_GLOBALS(posix)
	int last_error;
ZEND_END_MODULE_GLOBALS(posix)

#if defined(ZTS) && defined(COMPILE_DL_POSIX)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

ZEND_EXTERN_MODULE_GLOBALS(posix)
#define POSIX_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(posix, v)

#else

#define posix_module_ptr NULL

#endif

#define phpext_posix_ptr posix_module_ptr

#endif /* PHP_POSIX_H */
