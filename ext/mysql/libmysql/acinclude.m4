# Local macros for automake & autoconf

AC_DEFUN(MYSQL_TYPE_ACCEPT,
[ac_save_CXXFLAGS="$CXXFLAGS"
AC_CACHE_CHECK([base type of last arg to accept], mysql_cv_btype_last_arg_accept,
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
if test "$ac_cv_prog_gxx" = "yes"
then
  CXXFLAGS="$CXXFLAGS -Werror"
fi
mysql_cv_btype_last_arg_accept=none
[AC_TRY_COMPILE([#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
],
[int a = accept(1, (struct sockaddr *) 0, (socklen_t *) 0);],
mysql_cv_btype_last_arg_accept=socklen_t)]
if test $mysql_cv_btype_last_arg_accept = none; then
[AC_TRY_COMPILE([#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
],
[int a = accept(1, (struct sockaddr *) 0, (size_t *) 0);],
mysql_cv_btype_last_arg_accept=size_t)]
fi
if test $mysql_cv_btype_last_arg_accept = none; then
mysql_cv_btype_last_arg_accept=int
fi)
AC_LANG_RESTORE
AC_DEFINE_UNQUOTED(SOCKET_SIZE_TYPE, $mysql_cv_btype_last_arg_accept)
CXXFLAGS="$ac_save_CXXFLAGS"
])


#---START: Used in for client configure
AC_DEFUN(MYSQL_CHECK_ULONG,
[AC_MSG_CHECKING(for type ulong)
AC_CACHE_VAL(ac_cv_ulong,
[AC_TRY_RUN([#include <stdio.h>
#include <sys/types.h>
main()
{
  ulong foo;
  foo++;
  exit(0);
}], ac_cv_ulong=yes, ac_cv_ulong=no, ac_cv_ulong=no)])
AC_MSG_RESULT($ac_cv_ulong)
if test "$ac_cv_ulong" = "yes"
then
  AC_DEFINE(HAVE_ULONG)
fi
])

AC_DEFUN(MYSQL_CHECK_UCHAR,
[AC_MSG_CHECKING(for type uchar)
AC_CACHE_VAL(ac_cv_uchar,
[AC_TRY_RUN([#include <stdio.h>
#include <sys/types.h>
main()
{
  uchar foo;
  foo++;
  exit(0);
}], ac_cv_uchar=yes, ac_cv_uchar=no, ac_cv_uchar=no)])
AC_MSG_RESULT($ac_cv_uchar)
if test "$ac_cv_uchar" = "yes"
then
  AC_DEFINE(HAVE_UCHAR)
fi
])

AC_DEFUN(MYSQL_CHECK_UINT,
[AC_MSG_CHECKING(for type uint)
AC_CACHE_VAL(ac_cv_uint,
[AC_TRY_RUN([#include <stdio.h>
#include <sys/types.h>
main()
{
  uint foo;
  foo++;
  exit(0);
}], ac_cv_uint=yes, ac_cv_uint=no, ac_cv_uint=no)])
AC_MSG_RESULT($ac_cv_uint)
if test "$ac_cv_uint" = "yes"
then
  AC_DEFINE(HAVE_UINT)
fi
])

#---END:
