dnl $Id$

AC_MSG_CHECKING(for mm support)
AC_ARG_WITH(mm,
[  --with-mm[=DIR]         Include mm support for session storage],[
  PHP_MM=$withval
],[
  PHP_MM=no
])
AC_MSG_RESULT($PHP_MM)

AC_MSG_CHECKING(whether to enable session support)
AC_ARG_ENABLE(session,
[  --disable-session       Disable session support],[
  PHP_SESSION=$enableval
],[
  PHP_SESSION=yes
])
AC_MSG_RESULT($PHP_SESSION)

AC_MSG_CHECKING(whether to enable transparent session id propagation)
AC_ARG_ENABLE(trans-sid,
[  --enable-trans-sid      Enable transparent session id propagation],[
  PHP_TRANS_SID=$enableval
],[
  PHP_TRANS_SID=no
])
AC_MSG_RESULT($PHP_TRANS_SID)

if test "$PHP_MM" != "no"; then
  for i in /usr/local /usr $PHP_MM; do
    if test -f "$i/include/mm.h"; then
      MM_DIR="$i"
    fi
  done

  if test -z "$MM_DIR" ; then
    AC_MSG_ERROR(cannot find mm library)
  fi

  AC_ADD_LIBRARY_WITH_PATH(mm, $MM_DIR/lib)
  AC_ADD_INCLUDE($MM_DIR/include)
  AC_DEFINE(HAVE_LIBMM, 1, [Whether you have libmm])
  PHP_MODULE_PTR(phpext_ps_mm_ptr)
fi

if test "$PHP_TRANS_SID" = "yes"; then
  AC_DEFINE(TRANS_SID, 1, [Whether you want transparent session id propagation])
fi

if test "$PHP_SESSION" = "yes"; then
  PHP_EXTENSION(session)
fi
