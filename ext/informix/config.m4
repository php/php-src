dnl $Id$

PHP_ARG_WITH(informix,for Informix support,
[  --with-informix[=DIR]   Include Informix support.  DIR is the Informix base
                          install directory, defaults to ${INFORMIXDIR:-nothing}.])

if test "$PHP_INFORMIX" != "no"; then

  PHP_EXTENSION(informix, $ext_shared)
  PHP_SUBST(INFORMIX_SHARED_LIBADD)

  if test "$INFORMIXDIR" = ""; then
    AC_MSG_ERROR([INFORMIXDIR environment variable is not set.])
  fi

  if test "$PHP_INFORMIX" = "yes"; then
    PHP_ADD_INCLUDE($INFORMIXDIR/incl/esql)
    PHP_ADD_LIBPATH($INFORMIXDIR/lib, INFORMIX_SHARED_LIBADD)
    PHP_ADD_LIBPATH($INFORMIXDIR/lib/esql, INFORMIX_SHARED_LIBADD)
  else
    if test "$PHP_INFORMIX" != "$INFORMIXDIR"; then
      AC_MSG_ERROR([Specified Informix base install directory is different than your INFORMIXDIR environment variable.])
    fi
    PHP_ADD_INCLUDE($PHP_INFORMIX/incl/esql)
    PHP_ADD_LIBPATH($PHP_INFORMIX/lib, INFORMIX_SHARED_LIBADD)
    PHP_ADD_LIBPATH($PHP_INFORMIX/lib/esql, INFORMIX_SHARED_LIBADD)
  fi

  IFX_LIBS=`$INFORMIXDIR/bin/esql -libs -shared | sed -e 's/-lm$//'`
  dnl  -lm twice otherwise?
  IFX_LIBS=`echo $IFX_LIBS | sed -e 's/Libraries to be used://g' -e 's/esql: error -55923: No source or object file\.//g'`
  dnl Seems to get rid of newlines.
  dnl According to Perls DBD-Informix, might contain these strings.

  case "$host_alias" in
    *aix*)
      CPPFLAGS="$CPPFLAGS -D__H_LOCALEDEF";;
  esac

  AC_MSG_CHECKING([Informix version])
  IFX_VERSION=[`$INFORMIXDIR/bin/esql -V | sed -ne '1 s/^[^0-9]*\([0-9]\)\.\([0-9]*\).*/\1\2/p'`]
  AC_MSG_RESULT($IFX_VERSION)
  AC_DEFINE_UNQUOTED(IFX_VERSION, $IFX_VERSION, [ ])

  if test $IFX_VERSION -ge "900"; then
    AC_DEFINE(HAVE_IFX_IUS,1,[ ])
    IFX_ESQL_FLAGS="-EDHAVE_IFX_IUS"
  else
    IFX_ESQL_FLAGS="-EUHAVE_IFX_IUS"
  fi
  PHP_SUBST(IFX_ESQL_FLAGS)

  for i in $IFX_LIBS; do
    case "$i" in
      *.o)
        IFX_LIBOBJS="$IFX_LIBOBJS $i"
        PHP_ADD_LIBPATH($ext_builddir, INFORMIX_SHARED_LIBADD)
        PHP_ADD_LIBRARY_DEFER(phpifx, 1, INFORMIX_SHARED_LIBADD)
        ;;
      -l*)
        lib=`echo $i|sed -e "s/^-l//"`
        PHP_ADD_LIBRARY_DEFER($lib, 1, INFORMIX_SHARED_LIBADD)
        ;;
      *.a)
        lib=`echo $i|sed -e "s#^/.*/lib##g;s#\.a##g"`
        PHP_ADD_LIBRARY_DEFER($lib, 1, INFORMIX_SHARED_LIBADD)
        ;;
    esac
  done
  
  AC_DEFINE(HAVE_IFX,1,[ ])
  PHP_SUBST(INFORMIXDIR)
  PHP_SUBST(IFX_LIBOBJS)
fi
