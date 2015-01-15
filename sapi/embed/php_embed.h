/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Edin Kadribasic <edink@php.net>                              |
   +----------------------------------------------------------------------+
*/
/* $Id$ */

#ifndef _PHP_EMBED_H_
#define _PHP_EMBED_H_

#include <main/php.h>
#include <main/SAPI.h>
#include <main/php_main.h>
#include <main/php_variables.h>
#include <main/php_ini.h>
#include <zend_ini.h>

#define PHP_EMBED_START_BLOCK(x,y) { \
    php_embed_init(x, y); \
    zend_first_try {

#define PHP_EMBED_END_BLOCK() \
  } zend_catch { \
    /* int exit_status = EG(exit_status); */ \
  } zend_end_try(); \
  php_embed_shutdown(); \
}

#ifndef PHP_WIN32
    #define EMBED_SAPI_API SAPI_API
#else
    #define EMBED_SAPI_API
#endif

#ifdef ZTS
ZEND_TSRMLS_CACHE_EXTERN;
#endif

BEGIN_EXTERN_C()
EMBED_SAPI_API int php_embed_init(int argc, char **argv);
EMBED_SAPI_API void php_embed_shutdown(void);
extern EMBED_SAPI_API sapi_module_struct php_embed_module;
END_EXTERN_C()


#endif /* _PHP_EMBED_H_ */
