dnl
dnl $Id$
dnl

dnl Just for fun (not actually need)
AC_MSG_CHECKING(for CLI build)
AC_MSG_RESULT(yes)

INSTALL_IT="$INSTALL_IT; \$(INSTALL) -m 0755 sapi/cli/php \$(INSTALL_ROOT)\$(bindir)/"

