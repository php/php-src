dnl $Id$
dnl config.m4 for extension fam

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(fam, for fam support,
[  --with-fam              Include fam support])

if test "$PHP_FAM" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-fam -> check with-path
SEARCH_PATH="/usr/local /usr"     # you might want to change this
SEARCH_FOR="/include/fam.h"  # you most likely want to change this
if test -r $PHP_FAM/$SEARCH_FOR; then # path given as parameter
  FAM_DIR=$PHP_FAM
else # search default path list
  AC_MSG_CHECKING([for fam files in default path])
  for i in $SEARCH_PATH ; do
    if test -r $i/$SEARCH_FOR; then
      FAM_DIR=$i
      AC_MSG_RESULT(found in $i)
    fi
  done
fi

if test -z "$FAM_DIR"; then
  AC_MSG_RESULT([not found])
  AC_MSG_ERROR([Please reinstall the fam distribution])
fi

# --with-fam -> add include path
PHP_ADD_INCLUDE($FAM_DIR/include)

# --with-fam -> chech for lib and symbol presence
LIBNAME=fam # you may want to change this
LIBSYMBOL=FAMOpen # you most likely want to change this 

PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
[
  PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $FAM_DIR/lib, FAM_SHARED_LIBADD)
  AC_DEFINE(HAVE_FAMLIB,1,[ ])
],[
  AC_MSG_ERROR([wrong fam lib version or lib not found])
],[
  -L$FAM_DIR/lib -lm -ldl
])

PHP_SUBST(FAM_SHARED_LIBADD)

  PHP_NEW_EXTENSION(fam, fam.c, $ext_shared)
fi
