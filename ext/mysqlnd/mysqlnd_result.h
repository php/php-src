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

#ifndef MYSQLND_RESULT_H
#define MYSQLND_RESULT_H

PHPAPI MYSQLND_RES * mysqlnd_result_init(unsigned int field_count, zend_bool persistent);
PHPAPI MYSQLND_RES_UNBUFFERED * mysqlnd_result_unbuffered_init(unsigned int field_count, zend_bool ps, zend_bool persistent);
PHPAPI MYSQLND_RES_BUFFERED_ZVAL * mysqlnd_result_buffered_zval_init(unsigned int field_count, zend_bool ps, zend_bool persistent);
PHPAPI MYSQLND_RES_BUFFERED_C * mysqlnd_result_buffered_c_init(unsigned int field_count, zend_bool ps, zend_bool persistent);

enum_func_status mysqlnd_query_read_result_set_header(MYSQLND_CONN_DATA * conn, MYSQLND_STMT * stmt);

#endif /* MYSQLND_RESULT_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
