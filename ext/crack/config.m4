dnl
dnl $Id$
dnl

PHP_ARG_WITH(crack, for CRACKlib support,
[  --with-crack[=DIR]      Include crack support.])

if test "$PHP_CRACK" != "no"; then

	for i in /usr/local/lib /usr/lib $PHP_CRACK/lib $PHP_CRACK/cracklib; do
		test -f $i/libcrack.$SHLIB_SUFFIX_NAME -o -f $i/libcrack.a && CRACK_LIBDIR=$i
	done

	for i in /usr/local/include /usr/include $PHP_CRACK/include $PHP_CRACK/cracklib; do
		test -f $i/packer.h && CRACK_INCLUDEDIR=$i
	done
  
	if test -z "$CRACK_LIBDIR"; then
		AC_MSG_ERROR(Cannot find the cracklib library file)
	fi

	if test -z "$CRACK_INCLUDEDIR"; then
		AC_MSG_ERROR(Cannot find a cracklib header file)
	fi

	PHP_ADD_INCLUDE($CRACK_INCLUDEDIR)
	PHP_ADD_LIBRARY_WITH_PATH(crack, $CRACK_LIBDIR, CRACK_SHARED_LIBADD)

	PHP_NEW_EXTENSION(crack, crack.c, $ext_shared)
	PHP_SUBST(CRACK_SHARED_LIBADD)
	AC_DEFINE(HAVE_CRACK, 1, [ ])
fi

