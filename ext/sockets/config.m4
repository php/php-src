dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(sockets, whether to enable sockets support,
[  --enable-sockets        Enable sockets support])

if test "$PHP_SOCKETS" != "no"; then
  dnl Check for struct cmsghdr
  AC_CACHE_CHECK([for struct cmsghdr], ac_cv_cmsghdr,
  [
    AC_TRY_COMPILE([
#include <sys/types.h>
#include <sys/socket.h>], [struct cmsghdr s; s], ac_cv_cmsghdr=yes, ac_cv_cmsghdr=no)
  ])

  if test "$ac_cv_cmsghdr" = yes; then
    AC_DEFINE(HAVE_CMSGHDR,1,[Whether you have struct cmsghdr])
  fi 

  AC_CHECK_FUNCS([hstrerror socketpair if_nametoindex if_indextoname])
  AC_CHECK_HEADERS([netdb.h netinet/tcp.h sys/un.h sys/sockio.h errno.h]) 
  AC_TRY_COMPILE([
#include <sys/types.h>
#include <sys/socket.h>
  ], [static struct msghdr tp; int n = (int) tp.msg_flags; return n],[],
    [AC_DEFINE(MISSING_MSGHDR_MSGFLAGS, 1, [ ])]
  )
  AC_DEFINE([HAVE_SOCKETS], 1, [ ])
  
  dnl Check for fied ss_family in sockaddr_storage (missing in AIX until 5.3)
  AC_CACHE_CHECK([for field ss_family in struct sockaddr_storage], ac_cv_ss_family,
  [
    AC_TRY_COMPILE([
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
  ], [struct sockaddr_storage sa_store; sa_store.ss_family = AF_INET6;],
     ac_cv_ss_family=yes, ac_cv_ss_family=no)
  ])
  
  if test "$ac_cv_ss_family" = yes; then
    AC_DEFINE(HAVE_SA_SS_FAMILY,1,[Whether you have sockaddr_storage.ss_family])
  fi

  PHP_NEW_EXTENSION([sockets], [sockets.c multicast.c], [$ext_shared])
  PHP_INSTALL_HEADERS([ext/sockets/], [php_sockets.h])
fi
