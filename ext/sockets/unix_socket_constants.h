/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Jason Greene <jason@php.net>                                 |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

/* This file is to be included by sockets.c */

#ifdef EPERM
	/* Operation not permitted */
	REGISTER_LONG_CONSTANT("SOCKET_EPERM", EPERM, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOENT
	/* No such file or directory */
	REGISTER_LONG_CONSTANT("SOCKET_ENOENT", ENOENT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EINTR
	/* Interrupted system call */
	REGISTER_LONG_CONSTANT("SOCKET_EINTR", EINTR, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EIO
	/* I/O error */
	REGISTER_LONG_CONSTANT("SOCKET_EIO", EIO, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENXIO
	/* No such device or address */
	REGISTER_LONG_CONSTANT("SOCKET_ENXIO", ENXIO, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef E2BIG
	/* Arg list too long */
	REGISTER_LONG_CONSTANT("SOCKET_E2BIG", E2BIG, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EBADF
	/* Bad file number */
	REGISTER_LONG_CONSTANT("SOCKET_EBADF", EBADF, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EAGAIN
	/* Try again */
	REGISTER_LONG_CONSTANT("SOCKET_EAGAIN", EAGAIN, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOMEM
	/* Out of memory */
	REGISTER_LONG_CONSTANT("SOCKET_ENOMEM", ENOMEM, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EACCES
	/* Permission denied */
	REGISTER_LONG_CONSTANT("SOCKET_EACCES", EACCES, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EFAULT
	/* Bad address */
	REGISTER_LONG_CONSTANT("SOCKET_EFAULT", EFAULT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOTBLK
	/* Block device required */
	REGISTER_LONG_CONSTANT("SOCKET_ENOTBLK", ENOTBLK, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EBUSY
	/* Device or resource busy */
	REGISTER_LONG_CONSTANT("SOCKET_EBUSY", EBUSY, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EEXIST
	/* File exists */
	REGISTER_LONG_CONSTANT("SOCKET_EEXIST", EEXIST, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EXDEV
	/* Cross-device link */
	REGISTER_LONG_CONSTANT("SOCKET_EXDEV", EXDEV, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENODEV
	/* No such device */
	REGISTER_LONG_CONSTANT("SOCKET_ENODEV", ENODEV, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOTDIR
	/* Not a directory */
	REGISTER_LONG_CONSTANT("SOCKET_ENOTDIR", ENOTDIR, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EISDIR
	/* Is a directory */
	REGISTER_LONG_CONSTANT("SOCKET_EISDIR", EISDIR, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EINVAL
	/* Invalid argument */
	REGISTER_LONG_CONSTANT("SOCKET_EINVAL", EINVAL, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENFILE
	/* File table overflow */
	REGISTER_LONG_CONSTANT("SOCKET_ENFILE", ENFILE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EMFILE
	/* Too many open files */
	REGISTER_LONG_CONSTANT("SOCKET_EMFILE", EMFILE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOTTY
	/* Not a typewriter */
	REGISTER_LONG_CONSTANT("SOCKET_ENOTTY", ENOTTY, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOSPC
	/* No space left on device */
	REGISTER_LONG_CONSTANT("SOCKET_ENOSPC", ENOSPC, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ESPIPE
	/* Illegal seek */
	REGISTER_LONG_CONSTANT("SOCKET_ESPIPE", ESPIPE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EROFS
	/* Read-only file system */
	REGISTER_LONG_CONSTANT("SOCKET_EROFS", EROFS, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EMLINK
	/* Too many links */
	REGISTER_LONG_CONSTANT("SOCKET_EMLINK", EMLINK, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EPIPE
	/* Broken pipe */
	REGISTER_LONG_CONSTANT("SOCKET_EPIPE", EPIPE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENAMETOOLONG
	/* File name too long */
	REGISTER_LONG_CONSTANT("SOCKET_ENAMETOOLONG", ENAMETOOLONG, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOLCK
	/* No record locks available */
	REGISTER_LONG_CONSTANT("SOCKET_ENOLCK", ENOLCK, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOSYS
	/* Function not implemented */
	REGISTER_LONG_CONSTANT("SOCKET_ENOSYS", ENOSYS, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOTEMPTY
	/* Directory not empty */
	REGISTER_LONG_CONSTANT("SOCKET_ENOTEMPTY", ENOTEMPTY, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ELOOP
	/* Too many symbolic links encountered */
	REGISTER_LONG_CONSTANT("SOCKET_ELOOP", ELOOP, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EWOULDBLOCK
	/* Operation would block */
	REGISTER_LONG_CONSTANT("SOCKET_EWOULDBLOCK", EWOULDBLOCK, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOMSG
	/* No message of desired type */
	REGISTER_LONG_CONSTANT("SOCKET_ENOMSG", ENOMSG, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EIDRM
	/* Identifier removed */
	REGISTER_LONG_CONSTANT("SOCKET_EIDRM", EIDRM, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ECHRNG
	/* Channel number out of range */
	REGISTER_LONG_CONSTANT("SOCKET_ECHRNG", ECHRNG, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EL2NSYNC
	/* Level 2 not synchronized */
	REGISTER_LONG_CONSTANT("SOCKET_EL2NSYNC", EL2NSYNC, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EL3HLT
	/* Level 3 halted */
	REGISTER_LONG_CONSTANT("SOCKET_EL3HLT", EL3HLT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EL3RST
	/* Level 3 reset */
	REGISTER_LONG_CONSTANT("SOCKET_EL3RST", EL3RST, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ELNRNG
	/* Link number out of range */
	REGISTER_LONG_CONSTANT("SOCKET_ELNRNG", ELNRNG, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EUNATCH
	/* Protocol driver not attached */
	REGISTER_LONG_CONSTANT("SOCKET_EUNATCH", EUNATCH, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOCSI
	/* No CSI structure available */
	REGISTER_LONG_CONSTANT("SOCKET_ENOCSI", ENOCSI, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EL2HLT
	/* Level 2 halted */
	REGISTER_LONG_CONSTANT("SOCKET_EL2HLT", EL2HLT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EBADE
	/* Invalid exchange */
	REGISTER_LONG_CONSTANT("SOCKET_EBADE", EBADE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EBADR
	/* Invalid request descriptor */
	REGISTER_LONG_CONSTANT("SOCKET_EBADR", EBADR, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EXFULL
	/* Exchange full */
	REGISTER_LONG_CONSTANT("SOCKET_EXFULL", EXFULL, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOANO
	/* No anode */
	REGISTER_LONG_CONSTANT("SOCKET_ENOANO", ENOANO, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EBADRQC
	/* Invalid request code */
	REGISTER_LONG_CONSTANT("SOCKET_EBADRQC", EBADRQC, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EBADSLT
	/* Invalid slot */
	REGISTER_LONG_CONSTANT("SOCKET_EBADSLT", EBADSLT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOSTR
	/* Device not a stream */
	REGISTER_LONG_CONSTANT("SOCKET_ENOSTR", ENOSTR, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENODATA
	/* No data available */
	REGISTER_LONG_CONSTANT("SOCKET_ENODATA", ENODATA, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ETIME
	/* Timer expired */
	REGISTER_LONG_CONSTANT("SOCKET_ETIME", ETIME, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOSR
	/* Out of streams resources */
	REGISTER_LONG_CONSTANT("SOCKET_ENOSR", ENOSR, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENONET
	/* Machine is not on the network */
	REGISTER_LONG_CONSTANT("SOCKET_ENONET", ENONET, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EREMOTE
	/* Object is remote */
	REGISTER_LONG_CONSTANT("SOCKET_EREMOTE", EREMOTE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOLINK
	/* Link has been severed */
	REGISTER_LONG_CONSTANT("SOCKET_ENOLINK", ENOLINK, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EADV
	/* Advertise error */
	REGISTER_LONG_CONSTANT("SOCKET_EADV", EADV, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ESRMNT
	/* Srmount error */
	REGISTER_LONG_CONSTANT("SOCKET_ESRMNT", ESRMNT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ECOMM
	/* Communication error on send */
	REGISTER_LONG_CONSTANT("SOCKET_ECOMM", ECOMM, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EPROTO
	/* Protocol error */
	REGISTER_LONG_CONSTANT("SOCKET_EPROTO", EPROTO, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EMULTIHOP
	/* Multihop attempted */
	REGISTER_LONG_CONSTANT("SOCKET_EMULTIHOP", EMULTIHOP, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EBADMSG
	/* Not a data message */
	REGISTER_LONG_CONSTANT("SOCKET_EBADMSG", EBADMSG, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOTUNIQ
	/* Name not unique on network */
	REGISTER_LONG_CONSTANT("SOCKET_ENOTUNIQ", ENOTUNIQ, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EBADFD
	/* File descriptor in bad state */
	REGISTER_LONG_CONSTANT("SOCKET_EBADFD", EBADFD, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EREMCHG
	/* Remote address changed */
	REGISTER_LONG_CONSTANT("SOCKET_EREMCHG", EREMCHG, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ERESTART
	/* Interrupted system call should be restarted */
	REGISTER_LONG_CONSTANT("SOCKET_ERESTART", ERESTART, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ESTRPIPE
	/* Streams pipe error */
	REGISTER_LONG_CONSTANT("SOCKET_ESTRPIPE", ESTRPIPE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EUSERS
	/* Too many users */
	REGISTER_LONG_CONSTANT("SOCKET_EUSERS", EUSERS, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOTSOCK
	/* Socket operation on non-socket */
	REGISTER_LONG_CONSTANT("SOCKET_ENOTSOCK", ENOTSOCK, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EDESTADDRREQ
	/* Destination address required */
	REGISTER_LONG_CONSTANT("SOCKET_EDESTADDRREQ", EDESTADDRREQ, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EMSGSIZE
	/* Message too long */
	REGISTER_LONG_CONSTANT("SOCKET_EMSGSIZE", EMSGSIZE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EPROTOTYPE
	/* Protocol wrong type for socket */
	REGISTER_LONG_CONSTANT("SOCKET_EPROTOTYPE", EPROTOTYPE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOPROTOOPT
	/* Protocol not available */
	REGISTER_LONG_CONSTANT("SOCKET_ENOPROTOOPT", ENOPROTOOPT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EPROTONOSUPPORT
	/* Protocol not supported */
	REGISTER_LONG_CONSTANT("SOCKET_EPROTONOSUPPORT", EPROTONOSUPPORT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ESOCKTNOSUPPORT
	/* Socket type not supported */
	REGISTER_LONG_CONSTANT("SOCKET_ESOCKTNOSUPPORT", ESOCKTNOSUPPORT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EOPNOTSUPP
	/* Operation not supported on transport endpoint */
	REGISTER_LONG_CONSTANT("SOCKET_EOPNOTSUPP", EOPNOTSUPP, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EPFNOSUPPORT
	/* Protocol family not supported */
	REGISTER_LONG_CONSTANT("SOCKET_EPFNOSUPPORT", EPFNOSUPPORT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EAFNOSUPPORT
	/* Address family not supported by protocol */
	REGISTER_LONG_CONSTANT("SOCKET_EAFNOSUPPORT", EAFNOSUPPORT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EADDRINUSE
	/* Address already in use */
	REGISTER_LONG_CONSTANT("SOCKET_EADDRINUSE", EADDRINUSE, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EADDRNOTAVAIL
	/* Cannot assign requested address */
	REGISTER_LONG_CONSTANT("SOCKET_EADDRNOTAVAIL", EADDRNOTAVAIL, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENETDOWN
	/* Network is down */
	REGISTER_LONG_CONSTANT("SOCKET_ENETDOWN", ENETDOWN, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENETUNREACH
	/* Network is unreachable */
	REGISTER_LONG_CONSTANT("SOCKET_ENETUNREACH", ENETUNREACH, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENETRESET
	/* Network dropped connection because of reset */
	REGISTER_LONG_CONSTANT("SOCKET_ENETRESET", ENETRESET, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ECONNABORTED
	/* Software caused connection abort */
	REGISTER_LONG_CONSTANT("SOCKET_ECONNABORTED", ECONNABORTED, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ECONNRESET
	/* Connection reset by peer */
	REGISTER_LONG_CONSTANT("SOCKET_ECONNRESET", ECONNRESET, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOBUFS
	/* No buffer space available */
	REGISTER_LONG_CONSTANT("SOCKET_ENOBUFS", ENOBUFS, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EISCONN
	/* Transport endpoint is already connected */
	REGISTER_LONG_CONSTANT("SOCKET_EISCONN", EISCONN, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOTCONN
	/* Transport endpoint is not connected */
	REGISTER_LONG_CONSTANT("SOCKET_ENOTCONN", ENOTCONN, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ESHUTDOWN
	/* Cannot send after transport endpoint shutdown */
	REGISTER_LONG_CONSTANT("SOCKET_ESHUTDOWN", ESHUTDOWN, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ETOOMANYREFS
	/* Too many references: cannot splice */
	REGISTER_LONG_CONSTANT("SOCKET_ETOOMANYREFS", ETOOMANYREFS, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ETIMEDOUT
	/* Connection timed out */
	REGISTER_LONG_CONSTANT("SOCKET_ETIMEDOUT", ETIMEDOUT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ECONNREFUSED
	/* Connection refused */
	REGISTER_LONG_CONSTANT("SOCKET_ECONNREFUSED", ECONNREFUSED, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EHOSTDOWN
	/* Host is down */
	REGISTER_LONG_CONSTANT("SOCKET_EHOSTDOWN", EHOSTDOWN, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EHOSTUNREACH
	/* No route to host */
	REGISTER_LONG_CONSTANT("SOCKET_EHOSTUNREACH", EHOSTUNREACH, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EALREADY
	/* Operation already in progress */
	REGISTER_LONG_CONSTANT("SOCKET_EALREADY", EALREADY, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EINPROGRESS
	/* Operation now in progress */
	REGISTER_LONG_CONSTANT("SOCKET_EINPROGRESS", EINPROGRESS, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EISNAM
	/* Is a named type file */
	REGISTER_LONG_CONSTANT("SOCKET_EISNAM", EISNAM, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EREMOTEIO
	/* Remote I/O error */
	REGISTER_LONG_CONSTANT("SOCKET_EREMOTEIO", EREMOTEIO, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EDQUOT
	/* Quota exceeded */
	REGISTER_LONG_CONSTANT("SOCKET_EDQUOT", EDQUOT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef ENOMEDIUM
	/* No medium found */
	REGISTER_LONG_CONSTANT("SOCKET_ENOMEDIUM", ENOMEDIUM, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef EMEDIUMTYPE
	/* Wrong medium type */
	REGISTER_LONG_CONSTANT("SOCKET_EMEDIUMTYPE", EMEDIUMTYPE, CONST_CS | CONST_PERSISTENT);
#endif
