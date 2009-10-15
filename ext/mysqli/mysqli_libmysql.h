/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2007 The PHP Group                                     |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Georg Richter <georg@mysql.com>                             |
  |          Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  +----------------------------------------------------------------------+

*/

/* These are unused */
#define MYSQLI_CLOSE_EXPLICIT 0
#define MYSQLI_CLOSE_IMPLICIT 1
#define MYSQLI_CLOSE_DISCONNECTED 2
#define MYSQLND_OPT_NUMERIC_AND_DATETIME_AS_UNICODE	200
#define MYSQLND_OPT_INT_AND_YEAR_AS_INT				201

#define mysqli_result_is_unbuffered(r)		((r)->handle && (r)->handle->status == MYSQL_STATUS_USE_RESULT)
#define mysqli_server_status(c)				(c)->server_status
#define mysqli_stmt_warning_count(s)		mysql_warning_count((s)->mysql)
#define mysqli_stmt_server_status(s)		(s)->mysql->server_status
#define mysqli_stmt_get_connection(s)		(s)->mysql
#define mysqli_close(c, is_forced)			mysql_close((c))
#define mysqli_stmt_close(c, implicit)		mysql_stmt_close((c))
#define mysqli_free_result(r, is_forced)	mysql_free_result((r))
#define mysqli_change_user_silent(c, u, p, d)   mysql_change_user((c), (u), (p), (d))

