dnl
dnl $Id$
dnl

PHP_ARG_WITH(mhash, for mhash support,
[  --with-mhash[=DIR]      Include mhash support])

if test "$PHP_MHASH" != "no"; then
  for i in $PHP_MHASH /usr/local /usr /opt/mhash; do
    test -f $i/include/mhash.h && MHASH_DIR=$i && break
  done

  if test -z "$MHASH_DIR"; then
    AC_MSG_ERROR(Please reinstall libmhash - I cannot find mhash.h)
  fi
  
  PHP_MHASH_MISSING_PROTOS=
  AC_MSG_CHECKING(for missing protos)
  AC_EGREP_HEADER(mhash_get_hash_name_static, [$MHASH_DIR/include/mhash.h], [
    AC_DEFINE([HAVE_MHASH_GET_HASH_NAME_STATIC_PROTO], 1, [ ])
  ], [
    PHP_MHASH_MISSING_PROTOS="mhash_get_hash_name_static"
  ])
  AC_EGREP_HEADER(mhash_get_keygen_name_static, [$MHASH_DIR/include/mhash.h], [
    AC_DEFINE([HAVE_MHASH_GET_KEYGEN_NAME_STATIC_PROTO], 1, [ ])
  ], [
    PHP_MHASH_MISSING_PROTOS="mhash_get_keygen_name_static $PHP_MHASH_MISSING_PROTOS"
  ])
  AC_MSG_RESULT([$PHP_MHASH_MISSING_PROTOS])

  PHP_ADD_INCLUDE($MHASH_DIR/include)
  PHP_ADD_LIBRARY_WITH_PATH(mhash, $MHASH_DIR/$PHP_LIBDIR, MHASH_SHARED_LIBADD)

  PHP_NEW_EXTENSION(mhash, mhash.c, $ext_shared)
  PHP_SUBST(MHASH_SHARED_LIBADD)
  AC_DEFINE(HAVE_LIBMHASH,1,[ ])
fi
