/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2013 The PHP Group                                |
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

/* $Id$ */

#ifndef MYSQLND_RESULT_H
#define MYSQLND_RESULT_H

PHPAPI MYSQLND_RES * mysqlnd_result_init(unsigned int field_count, zend_bool persistent TSRMLS_DC);

enum_func_status mysqlnd_query_read_result_set_header(MYSQLND_CONN_DATA * conn, MYSQLND_STMT * stmt TSRMLS_DC);

#endif /* MYSQLND_RESULT_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
