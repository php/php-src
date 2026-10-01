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
   | Authors: Chris Vandomelen <chrisv@b0rked.dhs.org>                    |
   |          Sterling Hughes  <sterling@php.net>                         |
   |                                                                      |
   | WinSock: Daniel Beulshausen <daniel@php4win.de>                      |
   +----------------------------------------------------------------------+
 */

/* Code originally from ext/sockets */

#ifndef PHP_WIN32_SOCKETS_H
#define PHP_WIN32_SOCKETS_H

PHPAPI int socketpair_win32(int domain, int type, int protocol, SOCKET sock[2], int overlapped);
PHPAPI int socketpair(int domain, int type, int protocol, SOCKET sock[2]);

#endif
