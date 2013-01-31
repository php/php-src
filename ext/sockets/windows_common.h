/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
 */
#ifndef WINDOWS_COMMON_H
#define WINDOWS_COMMON_H

#include <Winsock2.h>
#define NTDDI_XP NTDDI_WINXP /* bug in SDK */
#include <IPHlpApi.h> /* conflicting definition of CMSG_DATA */
#undef NTDDI_XP

#define HAVE_IF_NAMETOINDEX 1

#define IS_INVALID_SOCKET(a)	(a->bsd_socket == INVALID_SOCKET)
#ifdef EPROTONOSUPPORT
# undef EPROTONOSUPPORT
#endif
#ifdef ECONNRESET
# undef ECONNRESET
#endif
#define EPROTONOSUPPORT	WSAEPROTONOSUPPORT
#define ECONNRESET		WSAECONNRESET
#ifdef errno
# undef errno
#endif
#define errno			WSAGetLastError()
#define h_errno			WSAGetLastError()
#define set_errno(a)	WSASetLastError(a)
#define close(a)		closesocket(a)

#endif