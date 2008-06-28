dnl
dnl $Id$
dnl

PHP_ARG_WITH(mhash, for mhash support,
[  --with-mhash[=DIR]      Include mhash support])

if test "$PHP_MHASH" != "no"; then
	PHP_NEW_EXTENSION(mhash, mhash.c, $ext_shared)
	PHP_SUBST(MHASH_SHARED_LIBADD)
	PHP_ADD_EXTENSION_DEP(mhash, hash, true)
fi
