dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for CLI build)
AC_ARG_WITH(cli,
[  --with-cli              Build PHP as CLI application],[
  if test "$withval" = "yes"; then
        CLIPATH=/usr/local
  else
        CLIPATH=$withval
  fi
  PHP_SAPI=cli
  PHP_PROGRAM=php
  INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_PROGRAM \$(bindir)/$PHP_PROGRAM"
  RESULT=yes
  PHP_SUBST(EXT_PROGRAM_LDADD)
],[
  RESULT=no
])
AC_MSG_RESULT($RESULT)
