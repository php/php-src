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
#include "config.h"
#endif

#include "php.h"
#include "zend_globals.h"
#include "ext/standard/info.h"

#ifdef HAVE_LEXBOR

#include "php_lexbor.h"

/* Dynamically loadable module stuff */
#ifdef COMPILE_DL_LEXBOR
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(lexbor)
#endif /* COMPILE_DL_LEXBOR */

static PHP_MINFO_FUNCTION(lexbor)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Lexbor support", "active");
	php_info_print_table_row(2, "Lexbor version", LEXBOR_VERSION);
	php_info_print_table_end();
}

zend_module_entry lexbor_module_entry = {
	STANDARD_MODULE_HEADER,
	"lexbor",                   /* extension name */
	NULL,                       /* extension function list */
	NULL,                       /* extension-wide startup function */
	NULL,                       /* extension-wide shutdown function */
	NULL,                       /* per-request startup function */
	NULL,                       /* per-request shutdown function */
	PHP_MINFO(lexbor),          /* information function */
	NULL,
	STANDARD_MODULE_PROPERTIES
};

#endif
