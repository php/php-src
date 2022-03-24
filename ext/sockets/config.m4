PHP_ARG_ENABLE([sockets],
  [whether to enable sockets support],
  [AS_HELP_STRING([--enable-sockets],
    [Enable sockets support])])

if test "$PHP_SOCKETS" != "no"; then
  AC_CHECK_FUNCS([hstrerror if_nametoindex if_indextoname])
  AC_CHECK_HEADERS([netinet/tcp.h sys/un.h sys/sockio.h])
  AC_DEFINE([HAVE_SOCKETS], 1, [ ])

  dnl Check for fied ss_family in sockaddr_storage (missing in AIX until 5.3)
  AC_CACHE_CHECK([for field ss_family in struct sockaddr_storage], ac_cv_ss_family,
  [
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
  ]], [[struct sockaddr_storage sa_store; sa_store.ss_family = AF_INET6;]])],
    [ac_cv_ss_family=yes], [ac_cv_ss_family=no])
  ])

  if test "$ac_cv_ss_family" = yes; then
    AC_DEFINE(HAVE_SA_SS_FAMILY,1,[Whether you have sockaddr_storage.ss_family])
  fi

  dnl Check for AI_V4MAPPED flag
  AC_CACHE_CHECK([if getaddrinfo supports AI_V4MAPPED],[ac_cv_gai_ai_v4mapped],
  [
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <netdb.h>
  ]], [[int flag = AI_V4MAPPED;]])],
    [ac_cv_gai_ai_v4mapped=yes], [ac_cv_gai_ai_v4mapped=no])
  ])

  if test "$ac_cv_gai_ai_v4mapped" = yes; then
    AC_DEFINE(HAVE_AI_V4MAPPED,1,[Whether you have AI_V4MAPPED])
  fi

  dnl Check for AI_ALL flag
  AC_CACHE_CHECK([if getaddrinfo supports AI_ALL],[ac_cv_gai_ai_all],
  [
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <netdb.h>
  ]], [[int flag = AI_ALL;]])],
    [ac_cv_gai_ai_all=yes], [ac_cv_gai_ai_all=no])
  ])

  if test "$ac_cv_gai_ai_all" = yes; then
    AC_DEFINE(HAVE_AI_ALL,1,[Whether you have AI_ALL])
  fi

  dnl Check for AI_IDN flag
  AC_CACHE_CHECK([if getaddrinfo supports AI_IDN],[ac_cv_gai_ai_idn],
  [
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <netdb.h>
  ]], [[int flag = AI_IDN;]])],
      [ac_cv_gai_ai_idn=yes], [ac_cv_gai_ai_idn=no])
  ])

  if test "$ac_cv_gai_ai_idn" = yes; then
    AC_DEFINE(HAVE_AI_IDN,1,[Whether you have AI_IDN])
  fi

  dnl Check for struct ucred
  dnl checking the header is not enough	(eg DragonFlyBSD)
  AC_CACHE_CHECK([if ancillary credentials uses ucred],[ac_cv_ucred],
  [
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sys/socket.h>
  ]], [[struct ucred u = {.gid = 0};]])],
      [ac_cv_ucred=yes], [ac_cv_ucred=no])
  ])

  if test "$ac_cv_ucred" = yes; then
    AC_DEFINE(ANC_CREDS_UCRED,1,[Uses ucred struct])
  fi

  dnl Check for struct cmsgcred
  AC_CACHE_CHECK([if ancillary credentials uses cmsgcred],[ac_cv_cmsgcred],
  [
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/socket.h>
  ]], [[struct cmsgcred c = {0};]])],
      [ac_cv_cmsgcred=yes], [ac_cv_cmsgcred=no])
  ])

  if test "$ac_cv_cmsgcred" = yes; then
    AC_DEFINE(ANC_CREDS_CMSGCRED,1,[Uses cmsgcred struct])
  fi


  PHP_SOCKETS_CFLAGS=-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1
  case $host_alias in
  *darwin*) PHP_SOCKETS_CFLAGS="$PHP_SOCKETS_CFLAGS -D__APPLE_USE_RFC_3542"
  esac
  PHP_NEW_EXTENSION(
    [sockets],
    [sockets.c multicast.c conversions.c sockaddr_conv.c sendrecvmsg.c],
    [$ext_shared],,
    $PHP_SOCKETS_CFLAGS)
  PHP_INSTALL_HEADERS([ext/sockets/], [php_sockets.h])
fi
