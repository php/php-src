dnl
dnl $Id$
dnl

PHP_ARG_WITH(fribidi, for FriBidi support,
[  --with-fribidi[=DIR]    Include fribidi support (requires FriBidi >=0.1.12).])

if test "$PHP_FRIBIDI" != "no"; then

dnl if module was requested with default path of fribidi installation then 
dnl $PHP_FRIBIDI will be "yes"

  if test "$PHP_FRIBIDI" = "yes"; then
    PHP_FRIBIDI=/usr/local
  fi

  dnl check for fribidi header files

  AC_MSG_CHECKING([for header files in "$PHP_FRIBIDI/include/fribidi"])
  if test -f $PHP_FRIBIDI/include/fribidi/fribidi.h && test -f $PHP_FRIBIDI/include/fribidi/fribidi_types.h && test -f $PHP_FRIBIDI/include/fribidi/fribidi_char_sets.h; then
    FRIBIDI_INCDIR=$PHP_FRIBIDI/include/fribidi/
    AC_MSG_RESULT([found all])
  else
    AC_MSG_RESULT([missing])
  fi

  dnl check for fribidi shared library
  
  AC_MSG_CHECKING([for libfribidi.so file in "$PHP_FRIBIDI/lib/"])
  if test -f "$PHP_FRIBIDI/lib/libfribidi.so" ; then
    FRIBIDI_LIBDIR=$PHP_FRIBIDI/lib/
    AC_MSG_RESULT([found])
  else
    AC_MSG_RESULT([missing])
  fi

  dnl check for glib header files

  AC_MSG_CHECKING([for glib header files])
  if test -f `which glib-config` ; then
    AC_MSG_RESULT([found])
    GLIB_INCDIR=`glib-config --cflags`
  else
    AC_MSG_RESULT([missing])
  fi

  
  dnl check for fribidi version
  AC_MSG_CHECKING([for FriBidi version])
  FRIBIDI_CONFIG=$PHP_FRIBIDI/bin/fribidi-config
  if test -x $FRIBIDI_CONFIG; then
    fribidi_version_full=`$FRIBIDI_CONFIG --version`
  fi
  
  fribidi_version=`echo ${fribidi_version_full} | awk 'BEGIN { FS = "."; } { printf "%d", ($1 * 1000 + $2) * 1000 + $3;}'`

  if test "$fribidi_version" -ge 9000; then
    AC_MSG_RESULT([$fribidi_version_full])
  else
    AC_MSG_ERROR(FriBidi version 0.9.0 or later is required to compile php with FriBidi support)
  fi


  AC_MSG_CHECKING([sanity to build  extension])
  if  test -n "$FRIBIDI_INCDIR" && test -n "$FRIBIDI_LIBDIR" && test -n "$GLIB_INCDIR"; then

    AC_MSG_RESULT([yes])

    PHP_ADD_INCLUDE("$FRIBIDI_INCDIR")
    PHP_EVAL_INCLINE("$GLIB_INCDIR")
    PHP_ADD_LIBRARY_WITH_PATH(fribidi,"$FRIBIDI_LIBDIR", FRIBIDI_SHARED_LIBADD)

    AC_DEFINE(HAVE_FRIBIDI, 1, [ ])
    PHP_NEW_EXTENSION(fribidi, fribidi.c, $ext_shared)
    PHP_SUBST(FRIBIDI_SHARED_LIBADD)
  else
    AC_MSG_RESULT([no])
  fi
fi
