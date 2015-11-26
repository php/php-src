dnl $Id$

PHP_ARG_ENABLE(sysvmsg,whether to enable System V IPC support,
[  --enable-sysvmsg        Enable sysvmsg support])

if test "$PHP_SYSVMSG" != "no"; then
  AC_CHECK_HEADER([sys/msg.h],
    [],
    [AC_MSG_ERROR([Cannot enable System V IPC support, sys/msg.h is missing])
  ])

  AC_DEFINE(HAVE_SYSVMSG, 1, [ ])
  PHP_NEW_EXTENSION(sysvmsg, sysvmsg.c, $ext_shared)
fi
