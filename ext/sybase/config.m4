dnl $Id$

AC_MSG_CHECKING(for Sybase support)
AC_ARG_WITH(sybase,
[  --with-sybase[=DIR]     Include Sybase-DB support.  DIR is the Sybase home
                          directory, defaults to /home/sybase.],
[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      SYBASE_INCDIR=/home/sybase/include
      SYBASE_LIBDIR=/home/sybase/lib
    else
      SYBASE_INCDIR=$withval/include
      SYBASE_LIBDIR=$withval/lib
    fi
	AC_ADD_INCLUDE($SYBASE_INCDIR)
	AC_ADD_LIBPATH($SYBASE_LIBDIR)
	AC_ADD_LIBRARY(sybdb)
    AC_MSG_RESULT(yes)
    PHP_EXTENSION(sybase)
    AC_CHECK_LIB(dnet_stub, dnet_addr,
     [ AC_ADD_LIBRARY(dnet_stub)
        AC_DEFINE(HAVE_LIBDNET_STUB)
     ])
    AC_DEFINE(HAVE_SYBASE)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
