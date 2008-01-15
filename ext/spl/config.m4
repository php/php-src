dnl $Id$
dnl config.m4 for extension SPL

PHP_ARG_ENABLE(spl, enable SPL suppport,
[  --disable-spl           Disable Standard PHP Library], yes)

if test "$PHP_SPL" != "no"; then
  AC_MSG_CHECKING(whether zend_object_value is packed)
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS="$INCLUDES -I$abs_srcdir $CPPFLAGS"
  AC_TRY_RUN([
#include "Zend/zend_types.h"
int main(int argc, char **argv) {
	return ((sizeof(zend_object_handle) + sizeof(zend_object_handlers*)) == sizeof(zend_object_value)) ? 0 : 1;
}
  ], [
    ac_result=1
  ],[
    ac_result=0
  ], [
    ac_result=0
  ])
  CPPFLAGS=$old_CPPFLAGS
  if test "$ac_result"; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
  PHP_INSTALL_HEADERS([ext/spl/])
  AC_DEFINE_UNQUOTED(HAVE_PACKED_OBJECT_VALUE, $ac_result, [Whether struct _zend_object_value is packed])
  AC_DEFINE(HAVE_SPL, 1, [Whether you want SPL (Standard PHP Library) support]) 
  PHP_NEW_EXTENSION(spl, php_spl.c spl_functions.c spl_engine.c spl_iterators.c spl_array.c spl_directory.c spl_sxe.c spl_exceptions.c spl_observer.c spl_dllist.c, no)
  PHP_INSTALL_HEADERS([ext/spl], [php_spl.h spl_array.h spl_directory.h spl_engine.h spl_exceptions.h spl_functions.h spl_iterators.h spl_observer.h spl_sxe.h spl_dllist.h])
  PHP_ADD_EXTENSION_DEP(spl, pcre, true)
fi
