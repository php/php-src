/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
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
  | Authors: Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  |          Georg Richter <georg@mysql.com>                             |
  +----------------------------------------------------------------------+
*/
/* $Id: mysqlnd.h 318051 2011-10-12 16:18:02Z andrey $ */

#ifndef MYSQLND_REVERSE_API_H
#define MYSQLND_REVERSE_API_H
typedef struct st_mysqlnd_reverse_api
{
	zend_module_entry * module;
	MYSQLND *(*conversion_cb)(zval * zv TSRMLS_DC);
} MYSQLND_REVERSE_API;


PHPAPI void mysqlnd_reverse_api_init(TSRMLS_D);
PHPAPI void mysqlnd_reverse_api_end(TSRMLS_D);

PHPAPI HashTable * mysqlnd_reverse_api_get_api_list(TSRMLS_D);

PHPAPI void mysqlnd_reverse_api_register_api(MYSQLND_REVERSE_API * apiext TSRMLS_DC);
PHPAPI MYSQLND * zval_to_mysqlnd(zval * zv, const unsigned int client_api_capabilities, unsigned int * save_client_api_capabilities TSRMLS_DC);

#endif	/* MYSQLND_REVERSE_API_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
