dnl config.m4 for PHP4 MCVE Extension

PHP_ARG_WITH(mcve, for MCVE support,
[  --with-mcve[=DIR]       Include MCVE support])

if test "$PHP_MCVE" != "no"; then
  if test -r $PHP_MCVE/include/mcve.h; then
     MCVE_DIR=$PHP_MCVE
  else 
    for i in /usr /usr/local /usr/local/mcve ; do
      if test -r $i/include/mcve.h; then
        MCVE_DIR=$i
      fi
    done
  fi

  if test -z "$MCVE_DIR"; then
    AC_MSG_ERROR(not found. Please check your MCVE installation; mcve.h NOT FOUND)
  fi
  
  PHP_ADD_INCLUDE($MCVE_DIR/include)
  PHP_ADD_LIBRARY_WITH_PATH(mcve, $MCVE_DIR/lib, MCVE_SHARED_LIBADD)
  PHP_NEW_EXTENSION(mcve, mcve.c, $ext_shared)
  PHP_SUBST(MCVE_SHARED_LIBADD)
fi
