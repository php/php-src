dnl
dnl $Id$
dnl

PHP_ARG_WITH(informix,for Informix support,
[  --with-informix[=DIR]   Include Informix support.  DIR is the Informix base
                          install directory, defaults to ${INFORMIXDIR:-nothing}.])

if test "$PHP_INFORMIX" != "no"; then

  if test "$INFORMIXDIR" = ""; then
    AC_MSG_ERROR([INFORMIXDIR environment variable is not set.])
  fi

  if test "$PHP_INFORMIX" = "yes"; then
    IFX_INCLUDE=-I$INFORMIXDIR/incl/esql
    PHP_ADD_LIBPATH($INFORMIXDIR/lib, INFORMIX_SHARED_LIBADD)
    PHP_ADD_LIBPATH($INFORMIXDIR/lib/esql, INFORMIX_SHARED_LIBADD)
  else
    if test "$PHP_INFORMIX" != "$INFORMIXDIR"; then
      AC_MSG_ERROR([Specified Informix base install directory is different than your INFORMIXDIR environment variable.])
    fi
    IFX_INCLUDE=-I$PHP_INFORMIX/incl/esql
    PHP_ADD_LIBPATH($PHP_INFORMIX/lib, INFORMIX_SHARED_LIBADD)
    PHP_ADD_LIBPATH($PHP_INFORMIX/lib/esql, INFORMIX_SHARED_LIBADD)
  fi

  dnl Check if thread safety flags are needed
  if test "$enable_experimental_zts" = "yes"; then
    IFX_ESQL_FLAGS="-thread"   
    CPPFLAGS="$CPPFLAGS -DIFX_THREAD"
  else
    IFX_ESQL_FLAGS=""
  fi

  IFX_LIBS=`THREADLIB=POSIX $INFORMIXDIR/bin/esql $IFX_ESQL_FLAGS -libs`
  IFX_LIBS=`echo $IFX_LIBS | sed -e 's/Libraries to be used://g' -e 's/esql: error -55923: No source or object file\.//g'`
  dnl Seems to get rid of newlines.
  dnl According to Perls DBD-Informix, might contain these strings.

  case "$host_alias" in
    *aix*)
      CPPFLAGS="$CPPFLAGS -D__H_LOCALEDEF";;
  esac

  AC_MSG_CHECKING([Informix version])
  IFX_VERSION=[`$INFORMIXDIR/bin/esql -V | grep "ESQL Version" | sed -ne '1 s/\(.*\)ESQL Version \([0-9]\)\.\([0-9]*\).*/\2\3/p'`]
  AC_MSG_RESULT($IFX_VERSION)
  AC_DEFINE_UNQUOTED(IFX_VERSION, $IFX_VERSION, [ ])

  if test $IFX_VERSION -ge "900"; then
    AC_DEFINE(HAVE_IFX_IUS,1,[ ])
    IFX_ESQL_FLAGS="$IFX_ESQL_FLAGS -EDHAVE_IFX_IUS"
  else
    IFX_ESQL_FLAGS="$IFX_ESQL_FLAGS -EUHAVE_IFX_IUS"
  fi

  PHP_NEW_EXTENSION(informix, ifx.c, $ext_shared,, $IFX_INCLUDE)
  PHP_ADD_MAKEFILE_FRAGMENT

  for i in $IFX_LIBS; do
    case "$i" in
      *.o)
        IFX_LIBOBJS="$IFX_LIBOBJS $i"
        PHP_ADD_LIBPATH($ext_builddir, INFORMIX_SHARED_LIBADD)
        PHP_ADD_LIBRARY_DEFER(phpifx, 1, INFORMIX_SHARED_LIBADD)
        ;;
      -lm)
        ;;
      -lc)
        ;;
      -l*)
        lib=`echo $i | cut -c 3-`
        PHP_ADD_LIBRARY_DEFER($lib, 1, INFORMIX_SHARED_LIBADD)
        ;;
      *.a)
        case "`uname -s 2>/dev/null`" in
          UnixWare | SCO_SV | UNIX_SV)
            DLIBS="$DLIBS $i"
            ;;
          *)
            ac_dir="`echo $i|sed 's#[^/]*$##;s#\/$##'`"
            ac_lib="`echo $i|sed 's#^/.*/lib##g;s#\.a##g'`"
            DLIBS="$DLIBS -L$ac_dir -l$ac_lib"
            ;;
        esac
        ;;
    esac
  done

  PHP_SUBST(INFORMIX_SHARED_LIBADD)
  PHP_SUBST(INFORMIXDIR)
  PHP_SUBST(IFX_LIBOBJS)
  PHP_SUBST(IFX_ESQL_FLAGS)
  AC_DEFINE(HAVE_IFX,1,[ ])
fi
