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
