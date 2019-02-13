/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
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
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLND_RESULT_H
#define MYSQLND_RESULT_H

PHPAPI MYSQLND_RES * mysqlnd_result_init(const unsigned int field_count);
PHPAPI MYSQLND_RES_UNBUFFERED * mysqlnd_result_unbuffered_init(MYSQLND_RES * result, const unsigned int field_count, const zend_bool ps);
PHPAPI MYSQLND_RES_BUFFERED_ZVAL * mysqlnd_result_buffered_zval_init(MYSQLND_RES * result, const unsigned int field_count, const zend_bool ps);
PHPAPI MYSQLND_RES_BUFFERED_C * mysqlnd_result_buffered_c_init(MYSQLND_RES * result, const unsigned int field_count, const zend_bool ps);

enum_func_status mysqlnd_query_read_result_set_header(MYSQLND_CONN_DATA * conn, MYSQLND_STMT * stmt);

#endif /* MYSQLND_RESULT_H */
