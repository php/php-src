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
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLND_RESULT_H
#define MYSQLND_RESULT_H

PHPAPI MYSQLND_RES * mysqlnd_result_init(const unsigned int field_count);
PHPAPI MYSQLND_RES_UNBUFFERED * mysqlnd_result_unbuffered_init(MYSQLND_RES * result, const unsigned int field_count, MYSQLND_STMT_DATA *stmt);
PHPAPI MYSQLND_RES_BUFFERED * mysqlnd_result_buffered_init(MYSQLND_RES * result, const unsigned int field_count, MYSQLND_STMT_DATA *stmt);

enum_func_status mysqlnd_query_read_result_set_header(MYSQLND_CONN_DATA * conn, MYSQLND_STMT * stmt);

#endif /* MYSQLND_RESULT_H */
