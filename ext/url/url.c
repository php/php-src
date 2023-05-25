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
   | Authors: Máté Kocsis <kocsismate@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"

#include "php_url.h"
#include "ada_url.h"

PHP_MINIT_FUNCTION(url)
{
	url_register_symbols();

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(url)
{
	return SUCCESS;
}

zend_module_entry url_module_entry = {
	STANDARD_MODULE_HEADER,
	"url",									/* Extension name */
	NULL,									/* zend_function_entry */
	PHP_MINIT(url),							/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(url),									/* PHP_MSHUTDOWN - Module shutdown */
	NULL,							/* PHP_RINIT - Request initialization */
	NULL,									/* PHP_RSHUTDOWN - Request shutdown */
	NULL,									/* PHP_MINFO - Module info */
	PHP_VERSION,							/* Version */
	STANDARD_MODULE_PROPERTIES
};
