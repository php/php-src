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
   | Authors: Slava Poliakov <hackie@prohost.org>                         |
   |          Ilia Alshanetsky <ilia@prohost.org>                         |
   +----------------------------------------------------------------------+
 */
#ifndef PHP_SHMOP_H
#define PHP_SHMOP_H

#if HAVE_SHMOP

extern zend_module_entry shmop_module_entry;
#define phpext_shmop_ptr &shmop_module_entry

#include "php_version.h"
#define PHP_SHMOP_VERSION PHP_VERSION

PHP_MINIT_FUNCTION(shmop);
PHP_MINFO_FUNCTION(shmop);

PHP_FUNCTION(shmop_open);
PHP_FUNCTION(shmop_read);
PHP_FUNCTION(shmop_close);
PHP_FUNCTION(shmop_size);
PHP_FUNCTION(shmop_write);
PHP_FUNCTION(shmop_delete);

#ifdef PHP_WIN32
typedef int key_t;
#endif

struct php_shmop
{
	int shmid;
	key_t key;
	int shmflg;
	int shmatflg;
	char *addr;
	zend_long size;
};

typedef struct {
	int le_shmop;
} php_shmop_globals;

#ifdef ZTS
#define SHMOPG(v) TSRMG(shmop_globals_id, php_shmop_globals *, v)
#else
#define SHMOPG(v) (shmop_globals.v)
#endif

#else

#define phpext_shmop_ptr NULL

#endif

#endif	/* PHP_SHMOP_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
