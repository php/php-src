/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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

#ifdef PHP_WIN32
#define PHP_SHMOP_API __declspec(dllexport)
#else
#define PHP_SHMOP_API
#endif

PHP_MINIT_FUNCTION(shmop);
PHP_MSHUTDOWN_FUNCTION(shmop);
PHP_RINIT_FUNCTION(shmop);
PHP_RSHUTDOWN_FUNCTION(shmop);
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
	int size;
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
