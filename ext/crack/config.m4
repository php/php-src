dnl $Id$

PHP_ARG_WITH(crack, whether to include crack support,
[  --with-crack[=DIR]      Include crack support.])

if test "$PHP_CRACK" != "no"; then
	for i in /usr/local/lib /usr/lib $PHP_CRACK $PHP_CRACK/cracklib; do
		if test -f $i/libcrack.a; then
			CRACK_LIBDIR=$i
		fi
	done
	for i in /usr/local/include /usr/include $PHP_CRACK $PHP_CRACK/cracklib; do
		if test -f $i/packer.h; then
			CRACK_INCLUDEDIR=$i
		fi
	done
  
	if test -z "$CRACK_LIBDIR"; then
		AC_MSG_ERROR(Cannot find the cracklib library file)
	fi
	if test -z "$CRACK_INCLUDEDIR"; then
		AC_MSG_ERROR(Cannot find a cracklib header file)
	fi

    AC_ADD_INCLUDE($CRACK_INCLUDEDIR)
	AC_ADD_LIBRARY_WITH_PATH(crack, $CRACK_LIBDIR)

	PHP_EXTENSION(crack, $ext_shared)

	AC_DEFINE(HAVE_CRACK, 1, [ ])
fi
