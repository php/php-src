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

  AC_CHECK_FUNCS([hstrerror])
  AC_CHECK_HEADERS([netdb.h netinet/tcp.h sys/un.h errno.h])
  AC_TRY_COMPILE([
#include <sys/types.h>
#include <sys/socket.h>
  ], [static struct msghdr tp; int n = (int) tp.msg_flags; return n],[],
    [AC_DEFINE(MISSING_MSGHDR_MSGFLAGS, 1, [ ])]
  )
  AC_DEFINE([HAVE_SOCKETS], 1, [ ])

  PHP_NEW_EXTENSION([sockets], [sockets.c], [$ext_shared])
fi
