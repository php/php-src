dnl $Id$
dnl config.m4 for extension recode
dnl don't forget to call PHP_EXTENSION(recode)


AC_MSG_CHECKING(for recode support)
AC_ARG_WITH(recode,
[  --with-recode[=DIR]     Include recode support. DIR is the recode install
                          directory.],
[
	if test "$withval" != "no"; then
		RECODE_LIST="$withval /usr /usr/local /opt"

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

		old_LDFLAGS="$LDFLAGS"
		old_LIBS="$LIBS"
		LDFLAGS="$LDFLAGS -L$RECODE_DIR/$RECODE_LIB"
		LIBS="$LIBS -lrecode"
		AC_TRY_LINK([
char *program_name;
],[
recode_format_table();
],[],[
				AC_MSG_ERROR(I cannot link librecode (-L$RECODE_DIR/$RECODE_LIB -lrecode). Is it installed?)
])
		LIBS="$old_LIBS"
		LDFLAGS="$old_LDFLAGS"

		if test "$RECODE_LIB" = ""; then
			AC_MSG_ERROR(Please reinstall recode - I cannot find librecode.a)
		fi

		AC_ADD_INCLUDE($RECODE_DIR/$RECODE_INC)
		AC_ADD_LIBRARY_DEFER_WITH_PATH(recode, $RECODE_DIR/$RECODE_LIB)

		AC_MSG_RESULT(yes)

		AC_DEFINE(HAVE_LIBRECODE, 1, [Whether we have librecode 3.5 or higher])
		PHP_EXTENSION(recode)
	else
		AC_MSG_RESULT(no)
	fi
],[
	AC_MSG_RESULT(no)
])
