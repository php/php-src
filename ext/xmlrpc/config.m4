
dnl $Id$

sinclude(ext/xmlrpc/libxmlrpc/acinclude.m4)
sinclude(ext/xmlrpc/libxmlrpc/xmlrpc.m4)
sinclude(libxmlrpc/acinclude.m4)
sinclude(libxmlrpc/xmlrpc.m4)

AC_DEFUN(XMLRPC_LIB_CHK,[
  str="$XMLRPC_DIR/$1/libxmlrpc.*"
  for j in `echo $str`; do
    if test -r $j; then
      XMLRPC_LIB_DIR=$XMLRPC_DIR/$1
      break 2
    fi
  done
])
	
PHP_ARG_WITH(xmlrpc, for XMLRPC-EPI support,
[  --with-xmlrpc[=DIR]      Include XMLRPC-EPI support. DIR is the XMLRPC-EPI base
                          directory. If unspecified, the bundled XMLRPC-EPI library
                          will be used.], yes)
                          
PHP_ARG_WITH(expat-dir, libexpat dir for XMLRPC-EPI
[  --with-expat-dir=DIR    XMLRPC-EPI: libexpat dir for XMLRPC-EPI])


if test "$PHP_XMLRPC" != "no"; then
  AC_DEFINE(HAVE_XMLRPC, 1, [Whether you have XMLRPC-EPI])
  PHP_EXTENSION(xmlrpc,$ext_shared)

dnl check for iconv
  found_iconv=no
  AC_CHECK_LIB(c, iconv_open, found_iconv=yes)
  if test "$found_iconv" = "no"; then
      for i in /usr /usr/local $ICONV_DIR; do
        if test -f $i/lib/libconv.a -o -f $i/lib/libiconv.s?; then
          PHP_ADD_LIBRARY_WITH_PATH(iconv, $i/lib, XMLRPC_SHARED_LIBADD)
          found_iconv=yes
        fi
      done
  fi
  
  if test "$found_iconv" = "no"; then
    AC_MSG_ERROR(iconv not found, in order to build XMLRPC-EPI you need the iconv library)
  fi
  
dnl check for expat
  testval=no
  for i in $PHP_EXPAT_DIR; do
    if test -f $i/lib/libexpat.a -o -f $i/lib/libexpat.s?; then
      AC_DEFINE(HAVE_LIBEXPAT2,1,[ ])
      PHP_ADD_LIBRARY_WITH_PATH(expat, $i/lib, XMLRPC_SHARED_LIBADD)
      PHP_ADD_INCLUDE($i/include)
      testval=yes
    fi
  done

  if test "$testval" = "no"; then
    PHP_ADD_LIBRARY(xmlparse)
    PHP_ADD_LIBRARY(xmltok)
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
  for i in $PHP_XMLRPC; do
    if test -r $i/include/xmlrpc/xmlrpc.h; then
      XMLRPC_DIR=$i
      XMLRPC_INC_DIR=$i/include/xmlrpc
    elif test -r $i/include/xmlrpc.h; then
      XMLRPC_DIR=$i
      XMLRPC_INC_DIR=$i/include
    fi
  done

  if test -z "$XMLRPC_DIR"; then
    AC_MSG_ERROR(Cannot find header files under $PHP_XMLRPC)
  fi

  XMLRPC_MODULE_TYPE=external

  for i in lib lib/xmlrpc; do
    XMLRPC_LIB_CHK($i)
  done

  if test -z "$XMLRPC_LIB_DIR"; then
    AC_MSG_ERROR(Cannot find xmlrpc library under $XMLRPC_DIR)
  fi

  if test "$PHP_ZLIB_DIR" != "no"; then
    PHP_ADD_LIBRARY(z,, XMLRPC_SHARED_LIBADD)
    XMLRPC_LIBS="-L$PHP_ZLIB_DIR -z"
  fi

  PHP_ADD_LIBRARY_WITH_PATH(xmlrpc, $XMLRPC_LIB_DIR, XMLRPC_SHARED_LIBADD)
  XMLRPC_LIBS="-L$XMLRPC_LIB_DIR -lxmlrpc $XMLRPC_LIBS"

  PHP_ADD_INCLUDE($XMLRPC_INC_DIR)
  XMLRPC_INCLUDE="-I$XMLRPC_INC_DIR"

else
  XMLRPC_MODULE_TYPE=none
fi


PHP_SUBST(XMLRPC_SHARED_LIBADD)
PHP_SUBST_OLD(XMLRPC_MODULE_TYPE)
PHP_SUBST_OLD(XMLRPC_LIBS)
PHP_SUBST_OLD(XMLRPC_INCLUDE)


