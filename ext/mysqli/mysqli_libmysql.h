/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
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

#ifndef MYSQLI_LIBMYSQL_H
#define MYSQLI_LIBMYSQL_H

/* These are unused */
#define MYSQLI_CLOSE_EXPLICIT 0
#define MYSQLI_CLOSE_IMPLICIT 1
#define MYSQLI_CLOSE_DISCONNECTED 2
#define MYSQLND_OPT_NUMERIC_AND_DATETIME_AS_UNICODE	200
#define MYSQLND_OPT_INT_AND_YEAR_AS_INT				201

/* r->data should be always NULL, at least in recent libmysql versions, the status changes once data is read*/
#define mysqli_result_is_unbuffered(r)		((r)->handle && (r)->handle->status == MYSQL_STATUS_USE_RESULT && (r)->data == NULL)
#define mysqli_result_is_unbuffered_and_not_everything_is_fetched(r)  mysqli_result_is_unbuffered(r)
#define mysqli_server_status(c)				(c)->server_status
#define mysqli_stmt_get_id(s)				((s)->stmt_id)
#define mysqli_stmt_warning_count(s)		mysql_warning_count((s)->mysql)
#define mysqli_stmt_server_status(s)		(s)->mysql->server_status
#define mysqli_stmt_get_connection(s)		(s)->mysql
#define mysqli_close(c, is_forced)			mysql_close((c))
#define mysqli_stmt_close(c, implicit)		mysql_stmt_close((c))
#define mysqli_free_result(r, is_forced)	mysql_free_result((r))
#define mysqli_change_user_silent(c, u, p, d, p_len)   mysql_change_user((c), (u), (p), (d))


/*
  These functions also reside in ext/mysqlnd/mysqlnd_portability.h but since it is only made
  available if one wants to build mysqli against mysqlnd and they are useful for libmysql as
  well, we check whether they're not defined [build with libmysql is desired] and define them.

  Bit values are sent in reverted order of bytes, compared to normal !!!
*/


#ifndef uint1korr
#define uint1korr(A)	(*(((uint8_t*)(A))))
#endif

#ifndef bit_uint2korr
#define bit_uint2korr(A) ((uint16_t) (((uint16_t) (((unsigned char*) (A))[1])) +\
                                   ((uint16_t) (((unsigned char*) (A))[0]) << 8)))
#endif

#ifndef bit_uint3korr
#define bit_uint3korr(A) ((uint32_t) (((uint32_t) (((unsigned char*) (A))[2])) +\
                                   (((uint32_t) (((unsigned char*) (A))[1])) << 8) +\
                                   (((uint32_t) (((unsigned char*) (A))[0])) << 16)))
#endif

#ifndef bit_uint4korr
#define bit_uint4korr(A) ((uint32_t) (((uint32_t) (((unsigned char*) (A))[3])) +\
                                   (((uint32_t) (((unsigned char*) (A))[2])) << 8) +\
                                   (((uint32_t) (((unsigned char*) (A))[1])) << 16) +\
                                   (((uint32_t) (((unsigned char*) (A))[0])) << 24)))
#endif

#ifndef bit_uint5korr
#define bit_uint5korr(A) ((uint64_t)(((uint32_t) (((unsigned char*) (A))[4])) +\
                                    (((uint32_t) (((unsigned char*) (A))[3])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[2])) << 16) +\
                                   (((uint32_t) (((unsigned char*) (A))[1])) << 24)) +\
                                    (((uint64_t) (((unsigned char*) (A))[0])) << 32))
#endif

#ifndef bit_uint6korr
#define bit_uint6korr(A) ((uint64_t)(((uint32_t) (((unsigned char*) (A))[5])) +\
                                    (((uint32_t) (((unsigned char*) (A))[4])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[3])) << 16) +\
                                    (((uint32_t) (((unsigned char*) (A))[2])) << 24)) +\
                        (((uint64_t) (((uint32_t) (((unsigned char*) (A))[1])) +\
                                    (((uint32_t) (((unsigned char*) (A))[0]) << 8)))) <<\
                                     32))
#endif

#ifndef bit_uint7korr
#define bit_uint7korr(A) ((uint64_t)(((uint32_t) (((unsigned char*) (A))[6])) +\
                                    (((uint32_t) (((unsigned char*) (A))[5])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[4])) << 16) +\
                                   (((uint32_t) (((unsigned char*) (A))[3])) << 24)) +\
                        (((uint64_t) (((uint32_t) (((unsigned char*) (A))[2])) +\
                                    (((uint32_t) (((unsigned char*) (A))[1])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[0])) << 16))) <<\
                                     32))
#endif

#ifndef bit_uint8korr
#define bit_uint8korr(A) ((uint64_t)(((uint32_t) (((unsigned char*) (A))[7])) +\
                                    (((uint32_t) (((unsigned char*) (A))[6])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[5])) << 16) +\
                                    (((uint32_t) (((unsigned char*) (A))[4])) << 24)) +\
                        (((uint64_t) (((uint32_t) (((unsigned char*) (A))[3])) +\
                                    (((uint32_t) (((unsigned char*) (A))[2])) << 8) +\
                                    (((uint32_t) (((unsigned char*) (A))[1])) << 16) +\
                                    (((uint32_t) (((unsigned char*) (A))[0])) << 24))) <<\
                                    32))
#endif

#endif /* MYSQLI_LIBMYSQL_H */

