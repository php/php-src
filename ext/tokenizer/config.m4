dnl $Id$
dnl config.m4 for extension tokenizer

dnl Otherwise use enable:

PHP_ARG_ENABLE(tokenizer, whether to enable tokenizer support,
[  --enable-tokenizer           Enable tokenizer support])

if test "$PHP_TOKENIZER" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-tokenizer -> check with-path
	dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/tokenizer.h"  # you most likely want to change this
  dnl if test -r $PHP_TOKENIZER/; then # path given as parameter
  dnl   TOKENIZER_DIR=$PHP_TOKENIZER
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for tokenizer files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       TOKENIZER_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$TOKENIZER_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the tokenizer distribution])
  dnl fi

  dnl # --with-tokenizer -> add include path
  dnl PHP_ADD_INCLUDE($TOKENIZER_DIR/include)

  dnl # --with-tokenizer -> chech for lib and symbol presence
  dnl LIBNAME=tokenizer # you may want to change this
  dnl LIBSYMBOL=tokenizer # you most likely want to change this 
  dnl old_LIBS=$LIBS
  dnl LIBS="$LIBS -L$TOKENIZER_DIR/lib -lm -ldl"
  dnl AC_CHECK_LIB($LIBNAME, $LIBSYMBOL, [AC_DEFINE(HAVE_TOKENIZERLIB,1,[ ])],
	dnl			[AC_MSG_ERROR([wrong tokenizer lib version or lib not found])])
  dnl LIBS=$old_LIBS
  dnl
  dnl PHP_SUBST(TOKENIZER_SHARED_LIBADD)
  dnl PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $TOKENIZER_DIR/lib, TOKENIZER_SHARED_LIBADD)

  PHP_NEW_EXTENSION(tokenizer, tokenizer.c, $ext_shared)
fi
