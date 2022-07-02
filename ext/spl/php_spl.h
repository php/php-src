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
   | Authors: Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_SPL_H
#define PHP_SPL_H

#include "php.h"
#include <stdarg.h>

#define PHP_SPL_VERSION PHP_VERSION

extern zend_module_entry spl_module_entry;
#define phpext_spl_ptr &spl_module_entry

#ifdef PHP_WIN32
#	ifdef SPL_EXPORTS
#		define SPL_API __declspec(dllexport)
#	elif defined(COMPILE_DL_SPL)
#		define SPL_API __declspec(dllimport)
#	else
#		define SPL_API /* nothing */
#	endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define SPL_API __attribute__ ((visibility("default")))
#else
#	define SPL_API
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

PHPAPI zend_string *php_spl_object_hash(zend_object *obj);

#endif /* PHP_SPL_H */
