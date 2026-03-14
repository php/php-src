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
   | Author: Tom May <tom@go2net.com>                                     |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_SYSVSEM_H
#define PHP_SYSVSEM_H

#ifdef HAVE_SYSVSEM

extern zend_module_entry sysvsem_module_entry;
#define sysvsem_module_ptr &sysvsem_module_entry

#include "php_version.h"
#define PHP_SYSVSEM_VERSION PHP_VERSION

PHP_MINIT_FUNCTION(sysvsem);
PHP_MINFO_FUNCTION(sysvsem);

typedef struct {
	int key;					/* For error reporting. */
	int semid;					/* Returned by semget(). */
	int count;					/* Acquire count for auto-release. */
	int auto_release;			/* flag that says to auto-release. */
	zend_object std;
} sysvsem_sem;

#else

#define sysvsem_module_ptr NULL

#endif

#define phpext_sysvsem_ptr sysvsem_module_ptr

#endif /* PHP_SYSVSEM_H */
