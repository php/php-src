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
  +----------------------------------------------------------------------+
*/

#ifndef MYSQLND_PS_H
#define MYSQLND_PS_H

/* PS stuff */
typedef void (*ps_field_fetch_func)(zval * zv, const MYSQLND_FIELD * const field, const unsigned int pack_len, const zend_uchar ** row);

struct st_mysqlnd_perm_bind {
	ps_field_fetch_func func;
	/* should be signed int */
	int pack_len;
	unsigned int php_type;
};

extern struct st_mysqlnd_perm_bind mysqlnd_ps_fetch_functions[MYSQL_TYPE_LAST + 1];

enum_func_status mysqlnd_fetch_stmt_row_cursor(MYSQLND_RES * result, zval **row_data, const unsigned int flags, bool * fetched_anything);

void _mysqlnd_init_ps_subsystem(void);/* This one is private, mysqlnd_library_init() will call it */
void _mysqlnd_init_ps_fetch_subsystem(void);

void ps_fetch_from_1_to_8_bytes(zval * zv, const MYSQLND_FIELD * const field, const unsigned int pack_len, const zend_uchar ** row, unsigned int byte_count);

#endif /* MYSQLND_PS_H */
