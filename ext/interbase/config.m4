dnl
dnl $Id$
dnl

PHP_ARG_WITH(interbase,for InterBase support,
[  --with-interbase[=DIR]  Include InterBase support.  DIR is the InterBase base
                          install directory, defaults to /usr/interbase])

if test "$PHP_INTERBASE" != "no"; then
  if test "$PHP_INTERBASE" = "yes"; then
    IBASE_INCDIR=/usr/interbase/include
    IBASE_LIBDIR=/usr/interbase/lib
  else
    IBASE_INCDIR=$PHP_INTERBASE/include
    IBASE_LIBDIR=$PHP_INTERBASE/lib
  fi

  PHP_CHECK_LIBRARY(gds, isc_detach_database,
  [
    PHP_ADD_LIBRARY_WITH_PATH(gds, $IBASE_LIBDIR, INTERBASE_SHARED_LIBADD)
  ], [
    AC_MSG_ERROR([libgds not found! Check config.log for more information.])
  ], [
    -L$IBASE_LIBDIR -lgds
  ])

  PHP_ADD_INCLUDE($IBASE_INCDIR)
  AC_DEFINE(HAVE_IBASE,1,[ ])
  PHP_NEW_EXTENSION(interbase, interbase.c, $ext_shared)
  PHP_SUBST(INTERBASE_SHARED_LIBADD)
fi
