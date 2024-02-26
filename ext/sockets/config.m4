PHP_ARG_ENABLE([sockets],
  [whether to enable sockets support],
  [AS_HELP_STRING([--enable-sockets],
    [Enable sockets support])])

if test "$PHP_SOCKETS" != "no"; then
  AC_CHECK_FUNCS([hstrerror if_nametoindex if_indextoname sockatmark])
  AC_CHECK_HEADERS([sys/sockio.h linux/filter.h])
  AC_DEFINE([HAVE_SOCKETS], 1, [ ])

  dnl Check for field ss_family in sockaddr_storage (missing in AIX until 5.3)
  AC_CHECK_MEMBERS([struct sockaddr_storage.ss_family],,,[#include <sys/socket.h>])

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

  dnl Check for struct ucred. Checking the header is not enough (DragonFlyBSD).
  AC_CHECK_TYPES([struct ucred],,,
    [#ifndef _GNU_SOURCE
    # define _GNU_SOURCE
    #endif
    #include <sys/socket.h>])

  AC_CHECK_TYPES([struct cmsgcred],,,[#include <sys/socket.h>])

  PHP_SOCKETS_CFLAGS=-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1
  case $host_alias in
  *darwin*) PHP_SOCKETS_CFLAGS="$PHP_SOCKETS_CFLAGS -D__APPLE_USE_RFC_3542"
  esac
  PHP_NEW_EXTENSION(
    [sockets],
    [sockets.c multicast.c conversions.c sockaddr_conv.c sendrecvmsg.c],
    [$ext_shared],,
    $PHP_SOCKETS_CFLAGS)
  PHP_INSTALL_HEADERS([ext/sockets], [php_sockets.h])
fi
