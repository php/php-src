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
  | Authors: Georg Richter <georg@php.net>                               |
  |          Andrey Hristov <andrey@php.net>                             |
  |          Ulf Wendel <uw@php.net>                                     |
  +----------------------------------------------------------------------+

*/

#ifndef MYSQLI_MYSQLND_H
#define MYSQLI_MYSQLND_H

#include "ext/mysqlnd/mysqlnd_libmysql_compat.h"
#include "ext/mysqlnd/mysqlnd_portability.h"

/* Here comes non-libmysql API to have less ifdefs in mysqli*/
#define MYSQLI_CLOSE_EXPLICIT                  MYSQLND_CLOSE_EXPLICIT
#define MYSQLI_CLOSE_IMPLICIT                  MYSQLND_CLOSE_IMPLICIT
#define MYSQLI_CLOSE_DISCONNECTED              MYSQLND_CLOSE_DISCONNECTED

#define mysqli_result_is_unbuffered(r)	((r)->unbuf)
#define mysqli_result_is_unbuffered_and_not_everything_is_fetched(r)	((r)->unbuf && !(r)->unbuf->eof_reached)
#define mysqli_server_status(c)			mysqlnd_get_server_status((c))
#define mysqli_stmt_get_id(s)			((s)->data->stmt_id)
#define mysqli_stmt_warning_count(s)	mysqlnd_stmt_warning_count((s))
#define mysqli_stmt_server_status(s)	mysqlnd_stmt_server_status((s))
#define mysqli_stmt_get_connection(s)	(s)->data->conn
#define mysqli_close(c, how)			mysqlnd_close((c), (how))
#define mysqli_stmt_close(c, implicit)	mysqlnd_stmt_close((c), (implicit))
#define mysqli_free_result(r, implicit)	mysqlnd_free_result((r), (implicit))
#define mysqli_async_query(c, q, l)		mysqlnd_async_query((c), (q), (l))
#define mysqli_change_user_silent(c, u, p, d, p_len)   mysqlnd_change_user_ex((c), (u), (p), (d), TRUE, (size_t)(p_len))

#define HAVE_STMT_NEXT_RESULT

#endif
