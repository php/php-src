dnl $Id$
dnl config.m4 for extension dbplus


PHP_ARG_WITH(dbplus, for dbplus support,
[  --with-dbplus           Include dbplus support])

if test "$PHP_DBPLUS" != "no"; then
  # --with-dbplus -> check with-path
  SEARCH_PATH="/usr/dbplus /usr/local/dbplus /opt/dbplus"    
  SEARCH_FOR="/include/dbconfig.h"  
  if test -r $PHP_DBPLUS/; then # path given as parameter
    DBPLUS_DIR=$PHP_DBPLUS
  else # search default path list
    AC_MSG_CHECKING(for dbplus files in default path)
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        DBPLUS_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$DBPLUS_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the dbplus distribution)
  fi

  # --with-dbplus -> add include path
  AC_ADD_INCLUDE($DBPLUS_DIR/include)

  # --with-dbplus -> chech for lib and symbol presence 
  LIBNAME=Ddb
  LIBSYMBOL=Db_Init
  old_LIBS=$LIBS
  LIBS="-L$DBPLUS_DIR/lib -lm -ldl -lDmacc -lDracc"
  AC_CHECK_LIB($LIBNAME, $LIBSYMBOL, [AC_DEFINE(HAVE_DBPLUSLIB,1,[ ])],
    [AC_MSG_ERROR(wrong dbplus lib version or lib not found)])
  LIBS=$old_LIBS
  
  PHP_SUBST(DBPLUS_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $DBPLUS_DIR/lib, DBPLUS_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(Dmacc, $DBPLUS_DIR/lib, DBPLUS_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(Dracc, $DBPLUS_DIR/lib, DBPLUS_SHARED_LIBADD)

  PHP_EXTENSION(dbplus, $ext_shared)
fi
