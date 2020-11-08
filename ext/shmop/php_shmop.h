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
