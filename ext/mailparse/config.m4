dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(mailparse, whether to enable mailparse support,
[  --enable-mailparse      Enable mailparse support.])

if test "$PHP_MAILPARSE" != "no"; then
  if test "$ext_shared" != "yes" && test "$enable_mbstring" != "yes"; then
	  AC_MSG_WARN(Activating mbstring)
	  enable_mbstring=yes
  fi
  PHP_NEW_EXTENSION(mailparse, 	mailparse.c rfc2045.c 	rfc2045acchk.c rfc2045acprep.c 	rfc2045appendurl.c rfc2045cdecode.c rfc2045decode.c 	rfc2045find.c 	rfc822.c rfc822_getaddr.c 	rfc822_getaddrs.c, $ext_shared)
fi
