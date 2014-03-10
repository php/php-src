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

#ifdef errno
# undef errno
#endif
#define errno			WSAGetLastError()
#define h_errno			WSAGetLastError()
#define set_errno(a)	WSASetLastError(a)
#define close(a)		closesocket(a)

#ifdef ENETUNREACH /* errno.h probably included */
# undef EWOULDBLOCK
# undef EINPROGRESS
# undef EALREADY
# undef ENOTSOCK
# undef EDESTADDRREQ
# undef EMSGSIZE
# undef EPROTOTYPE
# undef ENOPROTOOPT
# undef EPROTONOSUPPORT
# undef ESOCKTNOSUPPORT
# undef EOPNOTSUPP
# undef EPFNOSUPPORT
# undef EAFNOSUPPORT
# undef EADDRINUSE
# undef EADDRNOTAVAIL
# undef ENETDOWN
# undef ENETUNREACH
# undef ENETRESET
# undef ECONNABORTED
# undef ECONNRESET
# undef ENOBUFS
# undef EISCONN
# undef ENOTCONN
# undef ESHUTDOWN
# undef ETOOMANYREFS
# undef ETIMEDOUT
# undef ECONNREFUSED
# undef ELOOP
# undef ENAMETOOLONG
# undef EHOSTDOWN
# undef EHOSTUNREACH
# undef ENOTEMPTY
# undef EPROCLIM
# undef EUSERS
# undef EDQUOT
# undef ESTALE
# undef EREMOTE

# undef EAGAIN
#endif

/* section disabled in WinSock2.h */
#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 WSAENOBUFS
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ETIMEDOUT               WSAETIMEDOUT
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
#define ENAMETOOLONG            WSAENAMETOOLONG
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
#define ENOTEMPTY               WSAENOTEMPTY
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE

/* and an extra one */
#define EAGAIN					WSAEWOULDBLOCK

#endif
