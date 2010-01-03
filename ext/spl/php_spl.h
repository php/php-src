/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_SPL_H
#define PHP_SPL_H

#include "php.h"
#include <stdarg.h>

#if 0
#define SPL_DEBUG(x)	x
#else
#define SPL_DEBUG(x)
#endif

extern zend_module_entry spl_module_entry;
#define phpext_spl_ptr &spl_module_entry

#ifdef PHP_WIN32
# ifdef SPL_EXPORTS
#  define SPL_API __declspec(dllexport)
# elif defined(COMPILE_DL_SPL)
#  define SPL_API __declspec(dllimport)
# else
#  define SPL_API /* nothing */
# endif
#else
# define SPL_API
#endif

#if defined(PHP_WIN32) && !defined(COMPILE_DL_SPL)
#undef phpext_spl
#define phpext_spl NULL
#endif

PHP_MINIT_FUNCTION(spl);
PHP_MSHUTDOWN_FUNCTION(spl);
PHP_RINIT_FUNCTION(spl);
PHP_RSHUTDOWN_FUNCTION(spl);
PHP_MINFO_FUNCTION(spl);


ZEND_BEGIN_MODULE_GLOBALS(spl)
	char *       autoload_extensions;
	HashTable *  autoload_functions;
	int          autoload_running;
	int          autoload_extensions_len;
ZEND_END_MODULE_GLOBALS(spl)

#ifdef ZTS
# define SPL_G(v) TSRMG(spl_globals_id, zend_spl_globals *, v)
extern int spl_globals_id;
#else
# define SPL_G(v) (spl_globals.v)
extern zend_spl_globals spl_globals;
#endif

PHP_FUNCTION(spl_classes);
PHP_FUNCTION(class_parents);
PHP_FUNCTION(class_implements);

#endif /* PHP_SPL_H */

/*
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
