dnl $Id$

divert(3)

AC_MSG_CHECKING(for Informix support)
AC_ARG_WITH(informix,
[  --with-informix[=DIR]   Include Informix support.  DIR is the Informix base
                          install directory, defaults to ${INFORMIXDIR:-nothing}.],
[
  if test "$withval" != "no"; then
    if test "$INFORMIXDIR" = ""; then
      INFORMIX_WARNING="
WARNING: You asked for Informix support, but don't have \\\$INFORMIXDIR
   environment value set up. Configuring and compiling Informix
   support to PHP3 is impossible and has been turned off. Please
   try again after setting up your environment."
      AC_MSG_RESULT(no)
    else
      if test "$withval" = "yes"; then
        IFX_INCDIR=$INFORMIXDIR/incl/esql
        if test -z "$IFX_LIBDIR"; then
          IFX_LIBDIR="-L$INFORMIXDIR/lib -L$INFORMIXDIR/lib/esql"
        else
          IFX_LIBDIR=$IFX_LIBDIR
        fi
      else
        IFX_INCDIR=$withval/incl/esql
        if test -z "$IFX_LIBDIR"; then
          IFX_LIBDIR="-L$withval/lib -L$withval/lib/esql"
        else
          IFX_LIBDIR=$IFX_LIBDIR
        fi
        if test "$withval" != "$INFORMIXDIR"; then
          INFORMIX_WARNING="
WARNING: You specified Informix base install directory that is different
   than your \\\$INFORMIXDIR environment variable. You'd better know
   exactly what you are doing."
        fi
        fi
      IFX_INCLUDE=-I$IFX_INCDIR
      IFX_LFLAGS=$IFX_LIBDIR
      if test -z "$IFX_LIBS"; then
        IFX_LIBS=`esql -libs | sed -e 's/-lm$//'`
        dnl  -lm twice otherwise?
        IFX_LIBS=`echo $IFX_LIBS | sed -e 's/Libraries to be used://g' -e 's/esql: error -55923: No source or object file\.//g'`
        dnl Seems to get rid of newlines.
        dnl According to Perls DBD-Informix, might contain these strings.
      else
        dnl Allow override to use static and/or threaded libs
        IFX_LIBS="$IFX_LIBS"
      fi
      CFLAGS="$CFLAGS $IFX_INCLUDE"
      LDFLAGS="$LDFLAGS $IFX_LFLAGS"

        if test "`uname -s 2>/dev/null`" = "AIX"; then
        CFLAGS="$CFLAGS -D__H_LOCALEDEF"
      fi
      AC_DEFINE(HAVE_IFX)
      AC_MSG_CHECKING([Informix version])
      IFX_VERSION=[`esql -V | sed -ne '1 s/^[^0-9]*\([0-9]\)\.\([0-9]*\).*/\1\2/p'`]
      if test $IFX_VERSION -ge "900"; then
        AC_DEFINE(HAVE_IFX_IUS)
        IFX_ESQL_FLAGS="-EDHAVE_IFX_IUS"
      else
        IFX_ESQL_FLAGS="-EUHAVE_IFX_IUS"
      fi
      AC_SUBST(IFX_ESQL_FLAGS)
      AC_DEFINE_UNQUOTED(IFX_VERSION, $IFX_VERSION)
      AC_MSG_RESULT(yes)
      PHP_EXTENSION(informix)
      for i in $IFX_LIBS; do
        case "$i" in
        *.o)
            IFX_OBJS="$IFX_OBJS $i"
            IFX_LIBADD="$IFX_LIBADD -Lext/informix -lifx";;
        *)
            IFX_LIBADD="$IFX_LIBADD $i";;
        esac
      done
      IFX_LIBS="$IFX_LFLAGS $IFX_LIBADD"
      INCLUDES="$INCLUDES $IFX_INCLUDE"
    fi
  else
    INFORMIXDIR=
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
AC_SUBST(INFORMIXDIR)
AC_SUBST(IFX_LIBS)
AC_SUBST(IFX_OBJS)
	
divert(5)

dnl Warn if Informix support was requested but environment is not set up correctly.
if test "$INFORMIX_WARNING" != ""; then
  echo "$INFORMIX_WARNING"
fi
