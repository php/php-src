dnl $Id$

PHP_ARG_WITH(sybase,for Sybase support,
[  --with-sybase[=DIR]     Include Sybase-DB support.  DIR is the Sybase home
                          directory, defaults to /home/sybase.])


if test "$PHP_SYBASE" != "no"; then
  if test "$PHP_SYBASE" = "yes"; then
    SYBASE_INCDIR=/home/sybase/include
    SYBASE_LIBDIR=/home/sybase/lib
  else
    SYBASE_INCDIR=$PHP_SYBASE/include
    SYBASE_LIBDIR=$PHP_SYBASE/lib
  fi
  PHP_ADD_INCLUDE($SYBASE_INCDIR)
  PHP_ADD_LIBRARY_WITH_PATH(sybdb, $SYBASE_LIBDIR, SYBASE_SHARED_LIBADD)
  PHP_EXTENSION(sybase, $ext_shared)
  AC_CHECK_LIB(dnet_stub, dnet_addr,
     [ PHP_ADD_LIBRARY_WITH_PATH(dnet_stub,,SYBASE_SHARED_LIBADD)
        AC_DEFINE(HAVE_LIBDNET_STUB,1,[ ])
     ])
  AC_DEFINE(HAVE_SYBASE,1,[ ])
fi
