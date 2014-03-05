/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  |          Georg Richter <georg@mysql.com>                             |
  +----------------------------------------------------------------------+
*/

/* $Id: mysqlnd.c 317989 2011-10-10 20:49:28Z andrey $ */
#include "php.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_debug.h"
#include "mysqlnd_reverse_api.h"


static HashTable mysqlnd_api_ext_ht;


/* {{{ mysqlnd_reverse_api_init */
PHPAPI void
mysqlnd_reverse_api_init(TSRMLS_D)
{
	zend_hash_init(&mysqlnd_api_ext_ht, 3, NULL, NULL, 1);
}
/* }}} */


/* {{{ mysqlnd_reverse_api_end */
PHPAPI void
mysqlnd_reverse_api_end(TSRMLS_D)
{
	zend_hash_destroy(&mysqlnd_api_ext_ht);
}
/* }}} */


/* {{{ myslqnd_get_api_extensions */
PHPAPI HashTable *
mysqlnd_reverse_api_get_api_list(TSRMLS_D)
{
	return &mysqlnd_api_ext_ht;
}
/* }}} */


/* {{{ mysqlnd_reverse_api_register_api */
PHPAPI void
mysqlnd_reverse_api_register_api(MYSQLND_REVERSE_API * apiext TSRMLS_DC)
{
	zend_hash_add(&mysqlnd_api_ext_ht, apiext->module->name, strlen(apiext->module->name) + 1, &apiext,
				  sizeof(MYSQLND_REVERSE_API), NULL);
}
/* }}} */


/* {{{ zval_to_mysqlnd */
PHPAPI MYSQLND *
zval_to_mysqlnd(zval * zv TSRMLS_DC)
{
	MYSQLND * retval;
	MYSQLND_REVERSE_API ** elem;

	for (zend_hash_internal_pointer_reset(&mysqlnd_api_ext_ht);
		 zend_hash_get_current_data(&mysqlnd_api_ext_ht, (void **)&elem) == SUCCESS;
		 zend_hash_move_forward(&mysqlnd_api_ext_ht))
	{
		if ((*elem)->conversion_cb) {
			retval = (*elem)->conversion_cb(zv TSRMLS_CC);
			if (retval) {
				return retval;
			}
		}
	}

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
