/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Georg Richter <georg@php.net>                                |
  +----------------------------------------------------------------------+
*/

#ifndef __HAVE_MYSQLI_PROFILER_COM_H__
#define __HAVE_MYSQLI_PROFILER_COM_H__

#ifdef PHP_WIN32
int inet_aton(const char *cp, struct in_addr *inp);
#endif

#if WIN32|WINNT
# define SOCK_ERR INVALID_SOCKET
# define SOCK_CONN_ERR SOCKET_ERROR
# define SOCK_RECV_ERR SOCKET_ERROR
#else
# define SOCK_ERR -1
# define SOCK_CONN_ERR -1
# define SOCK_RECV_ERR -1
#endif

#if WIN32|WINNT
#define PR_SREAD(a,b,c) recv(a,b,c,0)
#define PR_SCLOSE(a) closesocket(a)
#define PR_SSENDL(a,b,c) send(a,b,c,0)
#define PR_SSEND(a,b) send(a,b,strlen(b),0)
#else
#define PR_SREAD(a,b,c) read(a,b,c)
#define PR_SCLOSE(a) close(a)
#define PR_SSENDL(a,b,c) write(a,b,c)
#define PR_SSEND(a,b) write(a,b,strlen(b))
#endif

/* function prototypes */
int php_mysqli_create_socket(const char *hostname, int dport);
void php_mysqli_close_socket(int socket);

#endif 

