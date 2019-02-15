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
   | Authors: Anatol Belski <ab@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifndef PHPDBG_IO_H
#define PHPDBG_IO_H

#include "phpdbg.h"

/* Older versions of glibc <= 2.3.0 and <= OS X 10.5 do not have this constant defined */
#ifndef AI_NUMERICSERV
#define AI_NUMERICSERV 0
#endif

PHPDBG_API int phpdbg_consume_stdin_line(char *buf);

PHPDBG_API int phpdbg_consume_bytes(int sock, char *ptr, int len, int tmo);
PHPDBG_API int phpdbg_send_bytes(int sock, const char *ptr, int len);
PHPDBG_API int phpdbg_mixed_read(int sock, char *ptr, int len, int tmo);
PHPDBG_API int phpdbg_mixed_write(int sock, const char *ptr, int len);

PHPDBG_API int phpdbg_create_listenable_socket(const char *addr, unsigned short port, struct addrinfo *res);
PHPDBG_API int phpdbg_open_socket(const char *interface, unsigned short port);
PHPDBG_API void phpdbg_close_socket(int sock);

#endif /* PHPDBG_IO_H */
