dnl
dnl $Id$
dnl

if test "$PHP_MHASH" != "no"; then
	PHP_NEW_EXTENSION(mhash, mhash.c, $ext_shared)
	PHP_SUBST(MHASH_SHARED_LIBADD)
	PHP_ADD_EXTENSION_DEP(mhash, hash, true)
fi
