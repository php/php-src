dnl $Id$
dnl config.m4 for extension Sablot

PHP_ARG_WITH(sablot, for Sablotron XSL support,
[  --with-sablot[=DIR]     Include Sablotron support])

if test "$PHP_SABLOT" != "no"; then
  if test -r $PHP_SABLOT/include/sablot.h; then
    SABLOT_DIR=$PHP_SABLOT
  else
    AC_MSG_CHECKING(for Sablotron in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/sablot.h; then
        SABLOT_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$SABLOT_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the Sablotron distribution)
  fi

  AC_ADD_INCLUDE($SABLOT_DIR/include)

  PHP_SUBST(SABLOT_SHARED_LIBADD)
  AC_ADD_LIBRARY_WITH_PATH(sablot, $SABLOT_DIR/lib, SABLOT_SHARED_LIBADD)
  AC_ADD_LIBRARY_WITH_PATH(xmlparse, $SABLOT_DIR/lib, SABLOT_SHARED_LIBADD) 
  AC_ADD_LIBRARY_WITH_PATH(xmltok, $SABLOT_DIR/lib, SABLOT_SHARED_LIBADD)
  
  AC_DEFINE(HAVE_SABLOT,1,[ ])

  PHP_EXTENSION(sablot, $ext_shared)
fi
