/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
  +----------------------------------------------------------------------+
  | Authors: Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  |          Georg Richter <georg@php.net>                               |
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLND_REVERSE_API_H
#define MYSQLND_REVERSE_API_H
typedef struct st_mysqlnd_reverse_api
{
	zend_module_entry * module;
	MYSQLND *(*conversion_cb)(zval * zv);
} MYSQLND_REVERSE_API;


PHPAPI void mysqlnd_reverse_api_init(void);
PHPAPI void mysqlnd_reverse_api_end(void);

PHPAPI HashTable * mysqlnd_reverse_api_get_api_list(void);

PHPAPI void mysqlnd_reverse_api_register_api(const MYSQLND_REVERSE_API * apiext);
PHPAPI MYSQLND * zval_to_mysqlnd(zval * zv, const unsigned int client_api_capabilities, unsigned int * save_client_api_capabilities);

#endif	/* MYSQLND_REVERSE_API_H */
