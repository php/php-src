dnl $Id$

AC_MSG_CHECKING(for Sybase-CT support)
AC_ARG_WITH(sybase-ct,
[  --with-sybase-ct[=DIR]  Include Sybase-CT support.  DIR is the Sybase home
                          directory, defaults to /home/sybase.],
[
  if test "$withval" != "no"; then
    AC_MSG_RESULT(yes)
    if test -z "$SYBASE_LIBS"; then
      PHP_EXTENSION(sybase_ct)
    fi
    AC_DEFINE(HAVE_SYBASE_CT,1,[ ])
    if test "$withval" = "yes"; then
      SYBASE_CT_INCDIR=/home/sybase/include
      SYBASE_CT_LIBDIR=/home/sybase/lib
    else
      SYBASE_CT_INCDIR=$withval/include
      SYBASE_CT_LIBDIR=$withval/lib
    fi
	AC_ADD_INCLUDE($SYBASE_CT_INCDIR)
	AC_ADD_LIBPATH($SYBASE_CT_LIBDIR)
	AC_ADD_LIBRARY(cs)
	AC_ADD_LIBRARY(ct)
	AC_ADD_LIBRARY(comn)
	AC_ADD_LIBRARY(intl)
	SYBASE_CT_LIBS="-lcs -lct -lcomn -lintl"
    old_LDFLAGS=$LDFLAGS
    LDFLAGS="$LDFLAGS -L$SYBASE_CT_LIBDIR"
    AC_CHECK_LIB(tcl, netg_errstr,
              [ AC_ADD_LIBRARY(tcl) ],
              [ AC_ADD_LIBRARY(sybtcl) ],
              [ $SYBASE_CT_LIBS ])
    AC_CHECK_LIB(insck, insck__getVdate,
              [ AC_ADD_LIBRARY(insck) ])
    AC_CHECK_LIB(insck, bsd_tcp,
              [ AC_ADD_LIBRARY(insck) ])
      LDFLAGS=$old_LDFLAGS
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
