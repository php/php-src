dnl $Id$

AC_MSG_CHECKING(whether to enable System V semaphore support)
AC_ARG_ENABLE(sysvsem,
[  --enable-sysvsem        Enable System V semaphore support.],
[
  if test "$enableval" = "yes"; then
    AC_MSG_RESULT(yes)
    PHP_EXTENSION(sysvsem)
    AC_DEFINE(HAVE_SYSVSEM, 1)
    AC_CACHE_CHECK(for union semun,php_cv_semun,
      AC_TRY_COMPILE([
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
      ],
      [union semun x;],
      [
        php_cv_semun=yes
      ],[
        php_cv_semun=no
      ])
    )
  if test $php_cv_semun = "yes"; then
    AC_DEFINE(HAVE_SEMUN, 1)
  else
    AC_DEFINE(HAVE_SEMUN, 0)
  fi
  else
    AC_MSG_RESULT(no)
    AC_DEFINE(HAVE_SYSVSEM, 0)
  fi
],[
  AC_MSG_RESULT(no)
  AC_DEFINE(HAVE_SYSVSEM, 0)
])

