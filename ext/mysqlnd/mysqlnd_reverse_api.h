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


PHPAPI void mysqlnd_reverse_api_init(void);
PHPAPI void mysqlnd_reverse_api_end(void);

PHPAPI HashTable * mysqlnd_reverse_api_get_api_list(void);

PHPAPI void mysqlnd_reverse_api_register_api(const zend_module_entry * apiext);

#endif	/* MYSQLND_REVERSE_API_H */
