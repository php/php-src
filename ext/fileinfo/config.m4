dnl $Id$
dnl config.m4 for extension fileinfo

PHP_ARG_WITH(fileinfo, for fileinfo support,
[  --with-fileinfo=DIR   Include fileinfo support])

if test "$PHP_FILEINFO" != "no"; then
  SEARCH_PATH="/usr/local/include /usr/include /usr/share/file"
  SEARCH_FOR="/magic.h"
  if test -r $PHP_FILEINFO/$SEARCH_FOR; then
    FILEINFO_DIR=$PHP_FILEINFO
  else
    AC_MSG_CHECKING([for magic files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        FILEINFO_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  
  if test -z "$FILEINFO_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the libmagic distribution])
  fi

  PHP_ADD_INCLUDE($FILEINFO_DIR/include)

  LIBNAME=magic
  LIBSYMBOL=magic_open

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $FILEINFO_DIR/lib, FILEINFO_SHARED_LIBADD)
    AC_DEFINE(HAVE_FILEINFOLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong magic lib version or lib not found])
  ],[
    -L$FILEINFO_DIR/lib -lm -lz -ldl
  ])

  MAGIC_MIME_LOCATIONS="/usr/local/share/file/magic /usr/share/file/magic /usr/share/misc/file/magic /etc/magic /usr/share/misc"
  for i in $MAGIC_MIME_LOCATIONS; do
    if test -f $i; then
       PHP_DEFAULT_MAGIC_FILE=$i
       break
    fi
  done
  AC_DEFINE_UNQUOTED(PHP_DEFAULT_MAGIC_FILE,"$PHP_DEFAULT_MAGIC_FILE",[magic file path])

  PHP_SUBST(FILEINFO_SHARED_LIBADD)

  PHP_NEW_EXTENSION(fileinfo, fileinfo.c, $ext_shared)
fi
