PHP_ARG_WITH(interbase,for Firebird support,
[  --with-interbase[=DIR]    Include Firebird support.  DIR is the Firebird base
                          install directory [/opt/firebird]])

if test "$PHP_INTERBASE" != "no"; then

  AC_PATH_PROG(FB_CONFIG, fb_config, no)

  if test -x "$FB_CONFIG" && test "$PHP_INTERBASE" = "yes"; then
    AC_MSG_CHECKING(for libfbconfig)
    FB_CFLAGS=`$FB_CONFIG --cflags`
    FB_LIBDIR=`$FB_CONFIG --libs`
    FB_VERSION=`$FB_CONFIG --version`
    AC_MSG_RESULT(version $FB_VERSION)
    PHP_EVAL_LIBLINE($FB_LIBDIR, INTERBASE_SHARED_LIBADD)
    PHP_EVAL_INCLINE($FB_CFLAGS)

  else
    if test "$PHP_INTERBASE" = "yes"; then
      IBASE_INCDIR=/opt/firebird/include
      IBASE_LIBDIR=/opt/firebird/lib
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
          AC_MSG_ERROR([libfbclient, libgds or libib_util not found! Check config.log for more information.])
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
  fi

  AC_DEFINE(HAVE_IBASE,1,[ ])
  PHP_NEW_EXTENSION(interbase, interbase.c ibase_query.c ibase_service.c ibase_events.c ibase_blobs.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_SUBST(INTERBASE_SHARED_LIBADD)
fi
