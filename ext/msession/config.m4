dnl
dnl $Id$
dnl

PHP_ARG_WITH(msession, for msession support,
[  --with-msession[=DIR]   Include msession support])
 
if test "$PHP_MSESSION" != "no"; then
	if test -r $PHP_MSESSION/lib/libphoenix.a; then
		PHOENIX_DIR=$PHP_MSESSION
	else
		AC_MSG_CHECKING(for msession in default path)
		for i in /usr/local/phoenix /usr/local /usr; do
			if test -r $i/lib/libphoenix.a; then
				PHOENIX_DIR=$i
				AC_MSG_RESULT(found in $i)
			fi
		done
	fi

	if test -z "$PHOENIX_DIR"; then
		AC_MSG_RESULT(not found)
		AC_MSG_ERROR(Please reinstall the Phoenix / msession distribution)
	fi


  	AC_DEFINE(HAVE_MSESSION, 1, [ ])
  	PHP_ADD_LIBRARY_WITH_PATH(phoenix, "$PHOENIX_DIR/lib", PHOENIX_LIB)
	PHOENIX_INCLUDE="-I$PHOENIX_DIR"
	PHP_SUBST(PHOENIX_LIB)

  	PHP_REQUIRE_CXX
  	PHP_NEW_EXTENSION(msession, msession.c, $ext_shared)
  	PHP_ADD_LIBRARY(stdc++)
fi

