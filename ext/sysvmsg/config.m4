PHP_ARG_ENABLE([sysvmsg],
  [whether to enable System V IPC support],
  [AS_HELP_STRING([--enable-sysvmsg],
    [Enable sysvmsg support])])

if test "$PHP_SYSVMSG" != "no"; then
  AC_CHECK_HEADER([sys/msg.h],,
    [AC_MSG_FAILURE(m4_text_wrap([
      Cannot enable System V IPC support. Required <sys/msg.h> header file not
      found.
    ]))])

  AC_DEFINE([HAVE_SYSVMSG], [1],
    [Define to 1 if the PHP extension 'sysvmsg' is available.])
  PHP_NEW_EXTENSION([sysvmsg], [sysvmsg.c], [$ext_shared])
fi
