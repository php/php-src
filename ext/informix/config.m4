dnl $Id$

PHPIFXLIB=ext/informix/libphp_ifx.a
	
PHP_ARG_WITH(informix,for Informix support,
[  --with-informix[=DIR]   Include Informix support.  DIR is the Informix base
                          install directory, defaults to ${INFORMIXDIR:-nothing}.])

  if test "$PHP_INFORMIX" != "no"; then
    if test "$INFORMIXDIR" = ""; then
      INFORMIX_WARNING="
WARNING: You asked for Informix support, but don't have \\\$INFORMIXDIR
   environment value set up. Configuring and compiling Informix
   support to PHP is impossible and has been turned off. Please
   try again after setting up your environment."
    else
      if test "$PHP_INFORMIX" = "yes"; then
        IFX_INCDIR=$INFORMIXDIR/incl/esql
        if test -z "$IFX_LIBDIR"; then
          PHP_ADD_LIBPATH($INFORMIXDIR/lib, INFORMIX_SHARED_LIBADD)
          PHP_ADD_LIBPATH($INFORMIXDIR/lib/esql, INFORMIX_SHARED_LIBADD)
        else
          IFX_LIBDIR="$IFX_LIBDIR"
        fi
      else
        IFX_INCDIR=$PHP_INFORMIX/incl/esql
        if test -z "$IFX_LIBDIR"; then
          PHP_ADD_LIBPATH($PHP_INFORMIX/lib, INFORMIX_SHARED_LIBADD)
          PHP_ADD_LIBPATH($PHP_INFORMIX/lib/esql, INFORMIX_SHARED_LIBADD)
        else
          IFX_LIBDIR="$IFX_LIBDIR"
        fi
        if test "$PHP_INFORMIX" != "$INFORMIXDIR"; then
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
        :
      fi
      CPPFLAGS="$CPPFLAGS $IFX_INCLUDE"
      LDFLAGS="$LDFLAGS $IFX_LFLAGS"

      case "$host_alias" in
	  *aix*)
        CPPFLAGS="$CPPFLAGS -D__H_LOCALEDEF";;
      esac

      AC_DEFINE(HAVE_IFX,1,[ ])
      AC_MSG_CHECKING([Informix version])
      IFX_VERSION=[`esql -V | sed -ne '1 s/^[^0-9]*\([0-9]\)\.\([0-9]*\).*/\1\2/p'`]
      if test $IFX_VERSION -ge "900"; then
        AC_DEFINE(HAVE_IFX_IUS,1,[ ])
        IFX_ESQL_FLAGS="-EDHAVE_IFX_IUS"
      else
        IFX_ESQL_FLAGS="-EUHAVE_IFX_IUS"
      fi
      PHP_SUBST(IFX_ESQL_FLAGS)
	  PHP_SUBST(INFORMIX_SHARED_LIBADD)
      AC_DEFINE_UNQUOTED(IFX_VERSION, $IFX_VERSION, [ ])
      PHP_EXTENSION(informix, $ext_shared)
      for i in $IFX_LIBS; do
        case "$i" in
        *.o)
            PHP_ADD_LIBPATH($abs_builddir/ext/informix, INFORMIX_SHARED_LIBADD)
            PHP_ADD_LIBRARY(php_ifx, 1, INFORMIX_SHARED_LIBADD)
            $srcdir/build/shtool mkdir -p ext/informix
            cd ext/informix
            ar r libphp_ifx.a $i
            ranlib libphp_ifx.a
            cd ../..;;
        -l*)
            lib=`echo $i|sed 's/^-l//'`
            PHP_ADD_LIBRARY($lib, 1, INFORMIX_SHARED_LIBADD);;
        *)
            IFX_LIBADD="$IFX_LIBADD $i";;
        esac
      done
      IFX_LIBS="$IFX_LFLAGS $IFX_LIBADD"
      INCLUDES="$INCLUDES $IFX_INCLUDE"
    fi
  fi
PHP_SUBST(INFORMIXDIR)
PHP_SUBST(IFX_LIBS)
	
divert(7)dnl

dnl Warn if Informix support was requested but environment is not set up correctly.
if test "$INFORMIX_WARNING" != ""; then
  echo "$INFORMIX_WARNING"
fi
