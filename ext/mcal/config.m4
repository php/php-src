dnl $Id$

PHP_ARG_WITH(mcal,for MCAL support,
[  --with-mcal[=DIR]       Include MCAL support.])

if test "$PHP_MCAL" != "no"; then
  if test "$PHP_MCAL" = "yes"; then
    MCAL_DIR=/usr/local
  else
    MCAL_DIR=$PHP_MCAL
  fi
    
  AC_ADD_INCLUDE($MCAL_DIR)
  AC_ADD_LIBRARY_WITH_PATH(mcal, $MCAL_DIR, MCAL_SHARED_LIBADD)
  PHP_SUBST(MCAL_SHARED_LIBADD)
  AC_DEFINE(HAVE_MCAL,1,[ ])
  PHP_EXTENSION(mcal,$ext_shared)
fi
