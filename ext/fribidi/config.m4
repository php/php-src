dnl
dnl $Id$
dnl

PHP_ARG_WITH(fribidi, for FriBidi support,
[  --with-fribidi[=DIR]    Include FriBidi support (requires FriBidi >= 0.10.4).])

if test "$PHP_FRIBIDI" != "no"; then

  if test "$PHP_FRIBIDI" = "yes"; then
    PHP_FRIBIDI=/usr/local
  fi

  dnl check for fribidi version
  AC_MSG_CHECKING([for FriBidi version])
  FRIBIDI_CONFIG=$PHP_FRIBIDI/bin/fribidi-config
  if test -x "$FRIBIDI_CONFIG"; then
    fribidi_version_full=`$FRIBIDI_CONFIG --version`
  else
    AC_MSG_ERROR([fribidi-config not found.])
  fi

  fribidi_version=`echo ${fribidi_version_full} | awk 'BEGIN { FS = "."; } { printf "%d", ($1 * 1000 + $2) * 1000 + $3;}'`

  if test "$fribidi_version" -ge 10004; then
    AC_MSG_RESULT([$fribidi_version_full])
  else
    AC_MSG_ERROR([FriBidi version 0.10.4 or later required.])
  fi

  dnl Get the paths
  FRIBIDI_LIBS=`$FRIBIDI_CONFIG --libs`
  FRIBIDI_INCS=`$FRIBIDI_CONFIG --cflags`
  
  if  test -n "$FRIBIDI_INCS" && test -n "$FRIBIDI_LIBS"; then
    PHP_EVAL_INCLINE($FRIBIDI_INCS)
    PHP_EVAL_LIBLINE($FRIBIDI_LIBS, FRIBIDI_SHARED_LIBADD)

    PHP_NEW_EXTENSION(fribidi, fribidi.c, $ext_shared)
    PHP_SUBST(FRIBIDI_SHARED_LIBADD)
    AC_DEFINE(HAVE_FRIBIDI, 1, [ ])
  else
    AC_MSG_ERROR([Could not find the required paths. Please check your FriBidi installation.])
  fi
fi
