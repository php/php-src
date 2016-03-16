/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  |          Georg Richter <georg@php.net>                               |
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


/* {{{ myslqnd_get_api_extensions */
PHPAPI HashTable *
mysqlnd_reverse_api_get_api_list(void)
{
	return &mysqlnd_api_ext_ht;
}
/* }}} */


/* {{{ mysqlnd_reverse_api_register_api */
PHPAPI void
mysqlnd_reverse_api_register_api(MYSQLND_REVERSE_API * apiext)
{
	zend_hash_str_add_ptr(&mysqlnd_api_ext_ht, apiext->module->name, strlen(apiext->module->name), apiext);
}
/* }}} */


/* {{{ zval_to_mysqlnd */
PHPAPI MYSQLND *
zval_to_mysqlnd(zval * zv, const unsigned int client_api_capabilities, unsigned int * save_client_api_capabilities)
{
	MYSQLND * retval;
#ifdef OLD_CODE
	MYSQLND_REVERSE_API * elem;
	ZEND_HASH_FOREACH_PTR(&mysqlnd_api_ext_ht, elem) {
		if (elem->conversion_cb) {
			retval = elem->conversion_cb(zv);
			if (retval) {
				if (retval->data) {
					*save_client_api_capabilities = retval->data->m->negotiate_client_api_capabilities(retval->data, client_api_capabilities);
				}
				return retval;
			}
		}
	} ZEND_HASH_FOREACH_END();
#else
	MYSQLND_REVERSE_API * api;
	ZEND_HASH_FOREACH_PTR(&mysqlnd_api_ext_ht, api) {
		if (api && api->conversion_cb) {
			retval = api->conversion_cb(zv);
			if (retval) {
				if (retval->data) {
					*save_client_api_capabilities = retval->data->m->negotiate_client_api_capabilities(retval->data, client_api_capabilities);
				}
				return retval;
			}
		}
	} ZEND_HASH_FOREACH_END();
#endif
	return NULL;
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
