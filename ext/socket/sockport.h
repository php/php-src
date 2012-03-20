/************************************************

  sockport.h -

  $Author$
  created at: Fri Apr 30 23:19:34 JST 1999

************************************************/

#ifndef SOCKPORT_H
#define SOCKPORT_H

#ifdef SA_LEN
#  define SS_LEN(ss) (ss)->ss_len
#else
# ifdef HAVE_SA_LEN
#  define SA_LEN(sa) (sa)->sa_len
#  define SS_LEN(ss) (ss)->ss_len
# else
#  ifdef AF_INET6
#   define SA_LEN(sa) \
	(((sa)->sa_family == AF_INET6) ? sizeof(struct sockaddr_in6) \
				       : sizeof(struct sockaddr))
#   define SS_LEN(ss) \
	(((ss)->ss_family == AF_INET6) ? sizeof(struct sockaddr_in6) \
				       : sizeof(struct sockaddr))
#  else
    /* by tradition, sizeof(struct sockaddr) covers most of the sockaddrs */
#   define SA_LEN(sa)	(sizeof(struct sockaddr))
#   define SS_LEN(ss)	(sizeof(struct sockaddr))
#  endif
# endif
#endif

#ifdef HAVE_SA_LEN
# define SET_SA_LEN(sa, len) (void)((sa)->sa_len = (len))
# define SET_SS_LEN(ss, len) (void)((ss)->ss_len = (len))
#else
# define SET_SA_LEN(sa, len) (void)(len)
# define SET_SS_LEN(ss, len) (void)(len)
#endif

#ifdef HAVE_SIN_LEN
# define SIN_LEN(si) (si)->sin_len
# define SET_SIN_LEN(si,len) (si)->sin_len = (len)
#else
# define SIN_LEN(si) sizeof(struct sockaddr_in)
# define SET_SIN_LEN(si,len)
#endif

#ifndef IN_MULTICAST
# define IN_CLASSD(i)	(((long)(i) & 0xf0000000) == 0xe0000000)
# define IN_MULTICAST(i)	IN_CLASSD(i)
#endif

#ifndef IN_EXPERIMENTAL
# define IN_EXPERIMENTAL(i) ((((long)(i)) & 0xe0000000) == 0xe0000000)
#endif

#ifndef IN_CLASSA_NSHIFT
# define IN_CLASSA_NSHIFT 24
#endif

#ifndef IN_LOOPBACKNET
# define IN_LOOPBACKNET 127
#endif

#ifndef AF_UNSPEC
# define AF_UNSPEC 0
#endif

#ifndef PF_UNSPEC
# define PF_UNSPEC AF_UNSPEC
#endif

#ifndef PF_INET
# define PF_INET AF_INET
#endif

#if defined(HOST_NOT_FOUND) && !defined(h_errno) && !defined(__CYGWIN__)
extern int h_errno;
#endif

#endif
