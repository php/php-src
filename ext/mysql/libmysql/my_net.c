/* Copyright Abandoned 1996 TCX DataKonsult AB & Monty Program KB & Detron HB 
This file is public domain and comes with NO WARRANTY of any kind */

/* thread safe version of some common functions */

#include "mysys_priv.h"
#include <m_string.h>

/* for thread safe my_inet_ntoa */
#if !defined(MSDOS) && !defined(__WIN__)
#include <netdb.h>
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

#ifndef THREAD
#define pthread_mutex_lock(A)
#define pthread_mutex_unlock(A)
#endif

void my_inet_ntoa(struct in_addr in, char *buf)
{
  char *ptr;
  pthread_mutex_lock(&THR_LOCK_net);
  ptr=inet_ntoa(in);
  strmov(buf,ptr);
  pthread_mutex_unlock(&THR_LOCK_net);
}
