dnl
dnl $Id$
dnl

PHP_ARG_WITH(interbase,for InterBase support,
[  --with-interbase[=DIR]  Include InterBase support.  DIR is the InterBase base
                          install directory [/usr/interbase]])

if test "$PHP_INTERBASE" != "no"; then
  if test "$PHP_INTERBASE" = "yes"; then
    IBASE_INCDIR=/usr/interbase/include
    IBASE_LIBDIR=/usr/interbase/lib
  else
    IBASE_INCDIR=$PHP_INTERBASE/include
    IBASE_LIBDIR=$PHP_INTERBASE/$PHP_LIBDIR
  fi

  PHP_CHECK_LIBRARY(fbclient, isc_detach_database,
  [
    IBASE_LIBNAME=fbclient
  ], [
    PHP_CHECK_LIBRARY(gds, isc_detach_database,
    [
      IBASE_LIBNAME=gds
    ], [
      PHP_CHECK_LIBRARY(ib_util, isc_detach_database,
      [
        IBASE_LIBNAME=ib_util
      ], [
        AC_MSG_ERROR([libgds, libib_util or libfbclient not found! Check config.log for more information.])
      ], [
        -L$IBASE_LIBDIR
      ])
    ], [
      -L$IBASE_LIBDIR
    ])
  ], [
    -L$IBASE_LIBDIR
  ])
  
  PHP_ADD_LIBRARY_WITH_PATH($IBASE_LIBNAME, $IBASE_LIBDIR, INTERBASE_SHARED_LIBADD)
  PHP_ADD_INCLUDE($IBASE_INCDIR)
  AC_DEFINE(HAVE_IBASE,1,[ ])
  PHP_NEW_EXTENSION(interbase, interbase.c ibase_query.c ibase_service.c ibase_events.c ibase_blobs.c, $ext_shared)
  PHP_SUBST(INTERBASE_SHARED_LIBADD)
fi
