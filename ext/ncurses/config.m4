dnl
dnl $Id$
dnl

PHP_ARG_WITH(ncurses, for ncurses support,
[  --with-ncurses[=DIR]    Include ncurses support (CLI/CGI only).])

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

   PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL, [
     AC_DEFINE(HAVE_NCURSESLIB,1,[ ])
	 
	 PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $NCURSES_DIR/lib, NCURSES_SHARED_LIBADD)

     PHP_CHECK_LIBRARY(panel, new_panel, [
   	   AC_DEFINE(HAVE_NCURSES_PANEL,1,[ ])
	   PHP_ADD_LIBRARY_WITH_PATH(panel, $NCURSES_DIR/lib, NCURSES_SHARED_LIBADD)
     ], [
     ], [ -L$NCURSES_DIR/lib -l$LIBNAME -lm
     ])
	

   ], [
     AC_MSG_ERROR(Wrong ncurses lib version or lib not found)
   ], [
     -L$NCURSES_DIR/lib -lm
   ])
 
   AC_CHECK_LIB($LIBNAME, color_set,   [AC_DEFINE(HAVE_NCURSES_COLOR_SET,  1, [ ])])
   AC_CHECK_LIB($LIBNAME, slk_color,   [AC_DEFINE(HAVE_NCURSES_SLK_COLOR,  1, [ ])])
   AC_CHECK_LIB($LIBNAME, asume_default_colors,   [AC_DEFINE(HAVE_NCURSES_ASSUME_DEFAULT_COLORS,  1, [ ])])
   AC_CHECK_LIB($LIBNAME, use_extended_names,   [AC_DEFINE(HAVE_NCURSES_USE_EXTENDED_NAMES,  1, [ ])])

   PHP_NEW_EXTENSION(ncurses, ncurses.c ncurses_fe.c ncurses_functions.c, $ext_shared, cli)
   PHP_SUBST(NCURSES_SHARED_LIBADD)

fi
