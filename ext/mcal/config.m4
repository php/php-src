dnl
dnl $Id$
dnl

PHP_ARG_WITH(mcal,for MCAL support,
[  --with-mcal[=DIR]       Include MCAL support.])

if test "$PHP_MCAL" != "no"; then
  MCAL_DEFAULT_SEARCH=/usr/local
  if test "$PHP_MCAL" = "yes"; then
    MCAL_DIR=$MCAL_DEFAULT_SEARCH
  else
    PHP_EXPAND_PATH($PHP_MCAL, MCAL_DIR)
  fi

  for i in $MCAL_DIR $MCAL_DIR/mcal $MCAL_DIR/mcal/include $MCAL_DIR/include/mcal $MCAL_DIR/include; do
    if test -r "$i/mcal.h"; then
      MCAL_INCLUDE=$i
    fi
  done

  for i in $MCAL_DIR $MCAL_DIR/mcal $MCAL_DIR/mcal/lib $MCAL_DIR/lib/mcal $MCAL_DIR/lib; do
    if test -r "$i/libmcal.a"; then
      MCAL_LIBRARY=$i
    fi
  done

  if test ! -f "$MCAL_INCLUDE/mcal.h"; then
    AC_MSG_ERROR(Unable to locate your libmcal header files - mcal.h should be in the directory you specify or in the include/ subdirectory below it - default search location is $MCAL_DEFAULT_SEARCH)
  fi

  if test ! -f "$MCAL_INCLUDE/cal_misc.h"; then
    AC_MSG_ERROR(Unable to locate your libmcal header files - cal_misc.h should be in the directory you specify or in the include/ subdirectory below it - default search location is $MCAL_DEFAULT_SEARCH)
  fi

  if test ! -f "$MCAL_INCLUDE/icalroutines.h"; then
    AC_MSG_ERROR(Unable to locate your libmcal header files - icalroutines.h should be in the directory you specify or in the include/ subdirectory below it - default search location is $MCAL_DEFAULT_SEARCH)
  fi

  if test ! -f "$MCAL_LIBRARY/libmcal.a"; then
    AC_MSG_ERROR(Unable to locate your libmcal library files - libmcal.a should be in the directory you specify or in the lib/ subdirectory below it - default search location is $MCAL_DEFAULT_SEARCH)
  fi

  PHP_ADD_INCLUDE($MCAL_INCLUDE)
  PHP_ADD_LIBPATH($MCAL_LIBRARY,MCAL_SHARED_LIBADD)
  PHP_ADD_LIBRARY_DEFER(mcal,[],MCAL_SHARED_LIBADD)
  PHP_SUBST(MCAL_SHARED_LIBADD)
  AC_DEFINE(HAVE_MCAL,1,[ ])
  PHP_EXTENSION(mcal,$ext_shared)
fi

