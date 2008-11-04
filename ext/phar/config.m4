dnl $Id$
dnl config.m4 for extension phar

PHP_ARG_ENABLE(phar, for phar archive support,
[  --disable-phar          Disable phar support], yes)

if test "$PHP_PHAR" != "no"; then
  PHP_NEW_EXTENSION(phar, util.c tar.c zip.c stream.c func_interceptors.c dirstream.c phar.c phar_object.c phar_path_check.c, $ext_shared)
  AC_MSG_CHECKING([for phar openssl support])
  if test "$PHP_HASH_SHARED" != "yes"; then
    if test "$PHP_HASH" != "no"; then
      AC_DEFINE(PHAR_HASH_OK,1,[ ])
    fi
  else
    AC_MSG_WARN([Phar: sha256/sha512 signature support disabled if ext/hash is built shared])
  fi
  if test "$PHP_OPENSSL_SHARED" = "yes"; then
    AC_MSG_RESULT([no (shared openssl)])
  else
    if test "$PHP_OPENSSL" = "yes"; then
      AC_MSG_RESULT([yes])
      AC_DEFINE(PHAR_HAVE_OPENSSL,1,[ ])
    else
      AC_MSG_RESULT([no])
    fi
  fi
  PHP_ADD_EXTENSION_DEP(phar, hash, true)
  PHP_ADD_EXTENSION_DEP(phar, spl, true)
  PHP_ADD_MAKEFILE_FRAGMENT
fi
