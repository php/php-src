/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/* thread safe version of some common functions */

/* for thread safe my_inet_ntoa */
#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#if !defined(MSDOS) && !defined(__WIN__) && !defined(__BEOS__)
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#endif /* !defined(MSDOS) && !defined(__WIN__) */

void my_inet_ntoa(struct in_addr in, char *buf);

#ifdef	__cplusplus
}
#endif
