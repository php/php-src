dnl
dnl $Id$
dnl

PHP_ARG_WITH(pspell,for PSPELL support,
[  --with-pspell[=DIR]     Include PSPELL support.
                          This replaces the old ASPELL extension.])

if test "$PHP_PSPELL" != "no"; then
	PHP_NEW_EXTENSION(pspell, pspell.c, $ext_shared)
	if test "$PHP_PSPELL" != "yes"; then
	    PSPELL_SEARCH_DIRS=$PHP_PSPELL
	else
	    PSPELL_SEARCH_DIRS="/usr/local /usr"
	fi
	for i in $PSPELL_SEARCH_DIRS; do
		if test -f $i/include/pspell/pspell.h; then
			PSPELL_DIR=$i
			PSPELL_INCDIR=$i/include/pspell
		elif test -f $i/include/pspell.h; then
			PSPELL_DIR=$i
			PSPELL_INCDIR=$i/include
		fi
	done

	if test -z "$PSPELL_DIR"; then
		AC_MSG_ERROR(Cannot find pspell)
	fi

	PSPELL_LIBDIR=$PSPELL_DIR/lib

	AC_DEFINE(HAVE_PSPELL,1,[ ])
	PHP_SUBST(PSPELL_SHARED_LIBADD)
	PHP_ADD_LIBRARY_WITH_PATH(pspell, $PSPELL_LIBDIR, PSPELL_SHARED_LIBADD)
	PHP_ADD_INCLUDE($PSPELL_INCDIR)
fi
