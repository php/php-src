PHP_ARG_WITH([pspell],
  [for spell checker support],
  [AS_HELP_STRING([[--with-pspell[=DIR]]],
    [Include Aspell support. GNU Aspell version 0.50.0 or higher required])])

if test "$PHP_PSPELL" != "no"; then
	dnl Add -Wno-strict-prototypes as depends on user libs
	PHP_NEW_EXTENSION(pspell, pspell.c, $ext_shared, , "-Wno-strict-prototypes")
	if test "$PHP_PSPELL" != "yes"; then
	    PSPELL_SEARCH_DIRS=$PHP_PSPELL
	else
	    PSPELL_SEARCH_DIRS="/usr/local /usr"
	fi
	for i in $PSPELL_SEARCH_DIRS; do
		if test -f $i/include/aspell.h; then
			PSPELL_DIR=$i
			PSPELL_INCDIR=$i/include
		fi
	done

	if test -z "$PSPELL_DIR"; then
		AC_MSG_ERROR(Cannot find aspell library)
	fi

	PSPELL_LIBDIR=$PSPELL_DIR/$PHP_LIBDIR

	PHP_ADD_LIBRARY_WITH_PATH(pspell, $PSPELL_LIBDIR, PSPELL_SHARED_LIBADD)

	dnl Add -laspell to LIBS if it exists
	PHP_CHECK_LIBRARY(aspell,new_aspell_config,
	[
		PHP_ADD_LIBRARY_WITH_PATH(aspell, $PSPELL_LIBDIR, PSPELL_SHARED_LIBADD)
	], [], [
		-L$PSPELL_LIBDIR
	])

	PHP_ADD_INCLUDE($PSPELL_INCDIR)
	PHP_SUBST(PSPELL_SHARED_LIBADD)
	AC_DEFINE(HAVE_PSPELL,1,[ ])
fi
