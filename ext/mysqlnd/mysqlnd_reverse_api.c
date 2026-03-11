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
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Johannes Schlüter <johannes@php.net>                        |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_reverse_api.h"


static HashTable mysqlnd_api_ext_ht;


/* {{{ mysqlnd_reverse_api_init */
PHPAPI void
mysqlnd_reverse_api_init(void)
{
	zend_hash_init(&mysqlnd_api_ext_ht, 3, NULL, NULL, 1);
}
/* }}} */


/* {{{ mysqlnd_reverse_api_end */
PHPAPI void
mysqlnd_reverse_api_end(void)
{
	zend_hash_destroy(&mysqlnd_api_ext_ht);
}
/* }}} */


/* {{{ mysqlnd_get_api_extensions */
PHPAPI HashTable *
mysqlnd_reverse_api_get_api_list(void)
{
	return &mysqlnd_api_ext_ht;
}
/* }}} */


/* {{{ mysqlnd_reverse_api_register_api */
PHPAPI void
mysqlnd_reverse_api_register_api(const zend_module_entry * apiext)
{
	zend_hash_str_add_ptr(&mysqlnd_api_ext_ht, apiext->name, strlen(apiext->name), (void*)apiext);
}
/* }}} */
