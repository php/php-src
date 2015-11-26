dnl
dnl $Id$
dnl

PHP_ARG_WITH(recode,for recode support,
[  --with-recode[=DIR]       Include recode support])

if test "$PHP_RECODE" != "no"; then
  RECODE_LIST="$PHP_RECODE /usr/local /usr /opt"

  for i in $RECODE_LIST; do
    if test -f $i/include/recode.h; then
      RECODE_DIR=$i
      RECODE_INC=include
      RECODE_LIB=$PHP_LIBDIR
    fi
    if test -f $i/include/recode/recode.h; then
      RECODE_DIR=$i
      RECODE_INC=include/recode
      RECODE_LIB=$PHP_LIBDIR/recode
    fi
    if test -f $i/recode/include/recode.h; then
      RECODE_DIR=$i/recode
      RECODE_INC=include
      RECODE_LIB=$PHP_LIBDIR
    fi
    test -n "$RECODE_DIR" && break
  done

  if test -z "$RECODE_DIR"; then
    AC_MSG_ERROR([Can not find recode.h anywhere under $RECODE_LIST.])
  fi

  PHP_CHECK_LIBRARY(recode, recode_format_table,
  [
    PHP_ADD_LIBRARY_WITH_PATH(recode, $RECODE_DIR/$RECODE_LIB, RECODE_SHARED_LIBADD)
  ], [
    old_LDFLAGS=$LDFLAGS
    old_LIBS=$LIBS
    LDFLAGS="$LDFLAGS -L$RECODE_DIR/$RECODE_LIB"
    LIBS="$LIBS -lrecode"
    AC_TRY_LINK(
    [
char *program_name;
    ],[
recode_format_table();
    ],[
      PHP_ADD_LIBRARY_DEFER_WITH_PATH(recode, $RECODE_DIR/$RECODE_LIB, RECODE_SHARED_LIBADD)
      AC_DEFINE(HAVE_BROKEN_RECODE, 1, [Whether we have librecode 3.5])
    ],[
      AC_MSG_ERROR(I cannot link librecode (-L$RECODE_DIR/$RECODE_LIB -lrecode). Is it installed?)
    ])
    LIBS=$old_LIBS
    LDFLAGS=$old_LDFLAGS
  ], [
    -L$RECODE_DIR/$RECODE_LIB
  ])

  AC_DEFINE(HAVE_LIBRECODE, 1, [Whether we have librecode 3.5 or higher])
  PHP_ADD_INCLUDE($RECODE_DIR/$RECODE_INC)
  PHP_SUBST(RECODE_SHARED_LIBADD)
  AC_CHECK_HEADERS(stdbool.h)
  PHP_NEW_EXTENSION(recode, recode.c, $ext_shared)
fi
