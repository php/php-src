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
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   |          Mate Kocsis <kocsismate@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#include "zend_globals.h"
#include "ext/standard/info.h"
#include "lexbor/core/types.h"
#include "lexbor/core/lexbor.h"

#ifdef HAVE_LEXBOR

#include "php_lexbor.h"

static void *php_lexbor_malloc(size_t size)
{
	return emalloc(size);
}

static void *php_lexbor_realloc(void *dst, size_t size)
{
	return erealloc(dst, size);
}

static void *php_lexbor_calloc(size_t num, size_t size)
{
	return ecalloc(num, size);
}

static void php_lexbor_free(void *ptr)
{
	efree(ptr);
}

static PHP_MINFO_FUNCTION(lexbor)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Lexbor support", "active");
	php_info_print_table_row(2, "Lexbor version", LEXBOR_VERSION);
	php_info_print_table_end();
}

static PHP_MINIT_FUNCTION(lexbor)
{
	lexbor_memory_setup(php_lexbor_malloc, php_lexbor_realloc, php_lexbor_calloc, php_lexbor_free);
	return SUCCESS;
}

zend_module_entry lexbor_module_entry = {
	STANDARD_MODULE_HEADER,
	"lexbor",                   /* extension name */
	NULL,                       /* extension function list */
	PHP_MINIT(lexbor),          /* extension-wide startup function */
	NULL,                       /* extension-wide shutdown function */
	NULL,                       /* per-request startup function */
	NULL,                       /* per-request shutdown function */
	PHP_MINFO(lexbor),          /* information function */
	PHP_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#endif
