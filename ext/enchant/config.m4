dnl
dnl $Id$
dnl

PHP_ARG_WITH(enchant,for ENCHANT support,
[  --with-enchant[=DIR]     Include enchant support.
                          GNU Aspell version 1.1.3 or higher required.])

if test "$PHP_ENCHANT" != "no"; then
	PHP_NEW_EXTENSION(enchant, enchant.c, $ext_shared)
	if test "$PHP_ENCHANT" != "yes"; then
	    ENCHANT_SEARCH_DIRS=$PHP_ENCHANT
	else
	    ENCHANT_SEARCH_DIRS="/usr/local /usr"
	fi
	for i in $ENCHANT_SEARCH_DIRS; do
		if test -f $i/include/enchant/enchant.h; then
			ENCHANT_DIR=$i
			ENCHANT_INCDIR=$i/include/enchant
		elif test -f $i/include/enchant.h; then
			ENCHANT_DIR=$i
			ENCHANT_INCDIR=$i/include
		fi
	done

	if test -z "$ENCHANT_DIR"; then
		AC_MSG_ERROR(Cannot find enchant)
	fi

	ENCHANT_LIBDIR=$ENCHANT_DIR/lib

	AC_DEFINE(HAVE_ENCHANT,1,[ ])
	PHP_SUBST(ENCHANT_SHARED_LIBADD)
	PHP_ADD_LIBRARY_WITH_PATH(enchant, $ENCHANT_LIBDIR, ENCHANT_SHARED_LIBADD)
	PHP_ADD_INCLUDE($ENCHANT_INCDIR)
	PHP_CHECK_LIBRARY(enchant, enchant_broker_set_param,
	[
	  AC_DEFINE(HAVE_ENCHANT_BROKER_SET_PARAM,             1, [ ])
	  AC_DEFINE(ENCHANT_VERSION_STRING,             "1.5.x", [ ])
	], [], [ -L$ENCHANT_LIB $ENCHANT_SHARED_LIBADD])

fi
