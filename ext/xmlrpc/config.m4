dnl
dnl $Id$
dnl

sinclude(ext/xmlrpc/libxmlrpc/acinclude.m4)
sinclude(ext/xmlrpc/libxmlrpc/xmlrpc.m4)
sinclude(libxmlrpc/acinclude.m4)
sinclude(libxmlrpc/xmlrpc.m4)

PHP_ARG_WITH(xmlrpc, for XMLRPC-EPI support,
[  --with-xmlrpc[=DIR]     Include XMLRPC-EPI support.])

PHP_ARG_WITH(expat-dir, libexpat dir for XMLRPC-EPI,
[  --with-expat-dir=DIR    XMLRPC-EPI: libexpat dir for XMLRPC-EPI.])

if test "$PHP_XMLRPC" != "no"; then

  PHP_EXTENSION(xmlrpc, $ext_shared)
  PHP_SUBST(XMLRPC_SHARED_LIBADD)
  AC_DEFINE(HAVE_XMLRPC,1,[ ])

  testval=no
  for i in $PHP_EXPAT_DIR $XMLRPC_DIR; do
    if test -f $i/lib/libexpat.a -o -f $i/lib/libexpat.$SHLIB_SUFFIX_NAME; then
      AC_DEFINE(HAVE_LIBEXPAT2,1,[ ])
      PHP_ADD_LIBRARY_WITH_PATH(expat, $i/lib, XMLRPC_SHARED_LIBADD)
      PHP_ADD_INCLUDE($i/include)
      testval=yes
    fi
  done


dnl  found_iconv=no
  AC_CHECK_LIB(c, iconv_open, found_iconv=yes)
  if test "$found_iconv" = "no"; then
      for i in /usr /usr/local $ICONV_DIR; do
        if test -f $i/lib/libconv.a -o -f $i/lib/libiconv.$SHLIB_SUFFIX_NAME; then
          PHP_ADD_LIBRARY_WITH_PATH(iconv, $i/lib, XMLRPC_SHARED_LIBADD)
          found_iconv=yes
        fi
      done
  fi
  
  if test "$found_iconv" = "no"; then
    AC_MSG_ERROR(iconv not found, in order to build xmlrpc you need the iconv library)
  fi
  
fi


if test "$PHP_XMLRPC" = "yes"; then
  XMLRPC_CHECKS
  XMLRPC_LIBADD=libxmlrpc/libxmlrpc.la
  XMLRPC_SHARED_LIBADD=libxmlrpc/libxmlrpc.la
  XMLRPC_SUBDIRS=libxmlrpc
  PHP_SUBST(XMLRPC_LIBADD)
  PHP_SUBST(XMLRPC_SUBDIRS)
  LIB_BUILD($ext_builddir/libxmlrpc,$ext_shared,yes)
  PHP_ADD_INCLUDE($ext_srcdir/libxmlrpc)
  XMLRPC_MODULE_TYPE=builtin

elif test "$PHP_XMLRPC" != "no"; then

  if test -r $PHP_XMLRPC/include/xmlrpc.h; then
    XMLRPC_DIR=$PHP_XMLRPC
  else
    AC_MSG_CHECKING(for XMLRPC-EPI in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/xmlrpc.h; then
        XMLRPC_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$XMLRPC_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the XMLRPC-EPI distribution)
  fi

  PHP_ADD_INCLUDE($XMLRPC_DIR/include)
  PHP_ADD_LIBRARY_WITH_PATH(xmlrpc, $XMLRPC_DIR/lib, XMLRPC_SHARED_LIBADD)
  
fi


