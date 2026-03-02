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
   | Authors: Slava Poliakov <hackie@prohost.org>                         |
   |          Ilia Alshanetsky <ilia@prohost.org>                         |
   +----------------------------------------------------------------------+
 */
#ifndef PHP_SHMOP_H
#define PHP_SHMOP_H

#ifdef HAVE_SHMOP

extern zend_module_entry shmop_module_entry;
#define phpext_shmop_ptr &shmop_module_entry

#include "php_version.h"
#define PHP_SHMOP_VERSION PHP_VERSION

PHP_MINIT_FUNCTION(shmop);
PHP_MINFO_FUNCTION(shmop);

#ifdef PHP_WIN32
# include "win32/ipc.h"
#endif

#else

#define phpext_shmop_ptr NULL

#endif

#endif	/* PHP_SHMOP_H */
