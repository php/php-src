/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Nikos Mavroyanopoulos <nmav@hellug.gr> (HMAC, KEYGEN)       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include "php_mhash.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/info.h"

const zend_function_entry mhash_functions[] = {
	{NULL, NULL, NULL}
};

zend_module_entry mhash_module_entry = {
	STANDARD_MODULE_HEADER,
	"mhash",
	mhash_functions,
	PHP_MINIT(mhash), NULL,
	NULL, NULL,
	PHP_MINFO(mhash),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES,
};

#ifdef COMPILE_DL_MHASH
ZEND_GET_MODULE(mhash)
#endif

/* {{{ PHP_MINIT */
PHP_MINIT_FUNCTION(mhash)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO */
PHP_MINFO_FUNCTION(mhash)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "MHASH support", "Enabled");
	php_info_print_table_row(2, "MHASH API Version", "Emulated");
	php_info_print_table_end();
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
