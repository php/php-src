dnl
dnl $Id$
dnl

PHP_ARG_WITH(recode,for recode support,
[  --with-recode[=DIR]     Include recode support.])

	if test "$PHP_RECODE" != "no"; then
		RECODE_LIST="$PHP_RECODE /usr /usr/local /opt"

		for i in $RECODE_LIST; do
			if test -f $i/include/recode.h; then
				RECODE_DIR=$i
				RECODE_INC=include
				RECODE_LIB=lib
			fi
			if test -f $i/include/recode/recode.h; then
				RECODE_DIR=$i
				RECODE_INC=include/recode
				RECODE_LIB=lib/recode
			fi
			if test -f $i/recode/include/recode.h; then
				RECODE_DIR=$i/recode
				RECODE_INC=include
				RECODE_LIB=lib
			fi
		done
		if test "$RECODE_DIR" = ""; then
			AC_MSG_ERROR(I cannot find recode.h anywhere below $RECODE_LIST. Is it installed?)
		fi

		old_LDFLAGS=$LDFLAGS
		old_LIBS=$LIBS
		LDFLAGS="$LDFLAGS -L$RECODE_DIR/$RECODE_LIB"
		LIBS="$LIBS -lrecode"
		AC_TRY_LINK([
char *program_name;
],[
recode_format_table();
],[],[
				AC_MSG_ERROR(I cannot link librecode (-L$RECODE_DIR/$RECODE_LIB -lrecode). Is it installed?)
])
		LIBS=$old_LIBS
		LDFLAGS=$old_LDFLAGS

		if test "$RECODE_LIB" = ""; then
			AC_MSG_ERROR(Please reinstall recode - I cannot find librecode.a)
		fi

		PHP_ADD_INCLUDE($RECODE_DIR/$RECODE_INC)
		if test "$ext_shared" = "yes"; then
			PHP_ADD_LIBRARY_WITH_PATH(recode, $RECODE_DIR/$RECODE_LIB, RECODE_SHARED_LIBADD)
			PHP_SUBST(RECODE_SHARED_LIBADD)
		else
			PHP_ADD_LIBRARY_DEFER_WITH_PATH(recode, $RECODE_DIR/$RECODE_LIB)
		fi

		AC_DEFINE(HAVE_LIBRECODE, 1, [Whether we have librecode 3.5 or higher])

		AC_CHECK_HEADERS(stdbool.h)

		PHP_NEW_EXTENSION(recode, recode.c, $ext_shared)
	fi
