/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Slava Poliakov (slavapl@mailandnews.com)                    |
   |          Ilia Alshanetsky (iliaa@home.com)                           |
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

struct php_shmop
{
	int shmid;
	key_t key;
	int shmflg;
	char *addr;
	int size;
};

typedef struct {
	int le_shmop;
} php_shmop_globals;

#ifdef ZTS
#define SHMOPG(v) (shmop_globals->v)
#define SHMOPLS_FETCH() php_shmop_globals *shmop_globals = ts_resource(gd_shmop_id)
#else
#define SHMOPG(v) (shmop_globals.v)
#define SHMOPLS_FETCH()
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
