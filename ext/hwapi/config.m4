dnl $Id$

PHP_ARG_WITH(hwapi, for hwapi support,
[  --with-hwapi[=DIR]      Include official Hyperwave API support])

if test "$PHP_HWAPI" != "no"; then
  if test -r $PHP_HWAPI/include/sdk/api/object.h; then
    HWAPI_DIR=$PHP_HWAPI
  else
    AC_MSG_CHECKING(for HWAPI in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/hwapi/sdk/api/object.h; then
        HWAPI_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$HWAPI_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please install the hwapi >= 1.0.0 distribution)
  fi

  PHP_ADD_INCLUDE($HWAPI_DIR/include/hwapi)
  PHP_SUBST(HWAPI_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(hwapi, $HWAPI_DIR/lib, HWAPI_SHARED_LIBADD)
  AC_DEFINE(HAVE_HWAPI,1,[ ])

  PHP_REQUIRE_CXX
  PHP_NEW_EXTENSION(hwapi, hwapi.cpp, $ext_shared)
  PHP_ADD_LIBRARY(hwapi)
  PHP_ADD_LIBRARY(pthread)
  PHP_ADD_LIBRARY(stdc++)
  PHP_ADD_INCLUDE($HWAPI_DIR/include)
fi
