dnl $Id$
dnl config.m4 for extension ncurses

PHP_ARG_WITH(ncurses, for ncurses support,
[  --with-ncurses             Include ncurses support])

if test "$PHP_NCURSES" != "no"; then
   # --with-ncurses -> check with-path
   SEARCH_PATH="/usr/local /usr"     
   SEARCH_FOR="/include/curses.h"
   if test -r $PHP_NCURSES/; then # path given as parameter
     NCURSES_DIR=$PHP_NCURSES
   else # search default path list
     AC_MSG_CHECKING(for ncurses files in default path)
     for i in $SEARCH_PATH ; do
       if test -r $i/$SEARCH_FOR; then
         NCURSES_DIR=$i
         AC_MSG_RESULT(found in $i)
       fi
     done
   fi
  
   if test -z "$NCURSES_DIR"; then
     AC_MSG_RESULT(not found)
     AC_MSG_ERROR(Please reinstall the ncurses distribution)
   fi

   # --with-ncurses -> add include path
   PHP_ADD_INCLUDE($NCURSES_DIR/include)

   # --with-ncurses -> chech for lib and symbol presence
   LIBNAME=ncurses 
   LIBSYMBOL=initscr 
   old_LIBS=$LIBS
   LIBS="$LIBS -L$NCURSES_DIR/lib -lm"
   AC_CHECK_LIB($LIBNAME, $LIBSYMBOL, [AC_DEFINE(HAVE_NCURSESLIB,1,[ ])],
        [AC_MSG_ERROR(wrong ncurses lib version or lib not found)])
   LIBS=$old_LIBS
  
   PHP_SUBST(NCURSES_SHARED_LIBADD)
   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $NCURSES_DIR/lib, SAPRFC_SHARED_LIBADD)

   AC_CHECK_LIB(ncurses, color_set,   [AC_DEFINE(HAVE_NCURSES_COLOR_SET,  1, [ ])])
   AC_CHECK_LIB(ncurses, slk_color,   [AC_DEFINE(HAVE_NCURSES_SLK_COLOR,  1, [ ])])
   AC_CHECK_LIB(ncurses, asume_default_colors,   [AC_DEFINE(HAVE_NCURSES_ASSUME_DEFAULT_COLORS,  1, [ ])])
   AC_CHECK_LIB(ncurses, use_extended_names,   [AC_DEFINE(HAVE_NCURSES_USE_EXTENDED_NAMES,  1, [ ])])


  PHP_EXTENSION(ncurses, $ext_shared)
fi
