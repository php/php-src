dnl $Id$
dnl config.m4 for extension sockets
dnl don't forget to call PHP_EXTENSION(sockets)

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(sockets, for sockets support,
dnl Make sure that the comment is aligned:
dnl [  --with-sockets          Include sockets support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(sockets, whether to enable sockets support,
dnl Make sure that the comment is aligned:
[  --enable-sockets        Enable sockets support])

if test "$PHP_SOCKETS" != "no"; then
  dnl If you will not be testing anything external, like existence of
  dnl headers, libraries or functions in them, just uncomment the 
  dnl following line and you are ready to go.
  
  dnl ---------------------------------
  dnl             Headers
  dnl ---------------------------------

  AC_HEADER_STDC
  AC_CHECK_HEADERS(unistd.h)
  AC_CHECK_HEADERS(sys/types.h sys/socket.h netdb.h netinet/in.h netinet/tcp.h sys/un.h arpa/inet.h)
  AC_CHECK_HEADERS(sys/time.h errno.h fcntl.h)

  AC_DEFINE(HAVE_SOCKETS, 1, [ ])
  dnl Write more examples of tests here...
  PHP_EXTENSION(sockets, $ext_shared)
fi
