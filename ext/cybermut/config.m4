dnl
dnl $Id$
dnl

PHP_ARG_WITH(cybermut, for cybermut support,
[  --with-cybermut[=DIR]   Include CyberMut (french Credit Mutuel telepaiement)])

if test "$PHP_CYBERMUT" != "no"; then
  for i in /usr/local /usr $PHP_CYBERMUT; do
    if test -r $i/cm-mac.h; then
      CYBERMUT_INC_DIR=$i
    elif test -r $i/include/cm-mac.h; then
      CYBERMUT_INC_DIR=$i/include
    fi
 
    if test -r $i/libcm-mac.a; then
      CYBERMUT_LIB_DIR=$i
    elif test -r $i/lib/libcm-mac.a; then
      CYBERMUT_LIB_DIR=$i/lib
    fi
  done
    
  if test -z "$CYBERMUT_INC_DIR"; then
    AC_MSG_ERROR(Could not find cm-mac.h Please make sure you have the
                 CyberMut SDK installed. Use
                 ./configure --with-cybermut=<cybermut-dir> if necessary)
  fi
 
  if test -z "$CYBERMUT_LIB_DIR"; then
    AC_MSG_ERROR(Could not find libcm-mac.a Please make sure you have the
                 CyberMut SDK installed. Use
                 ./configure --with-cybermut=<cybermut-dir> if necessary)
  fi                                                                                                                                            
  AC_MSG_RESULT(found in $CYBERMUT_LIB_DIR)
  
  AC_MSG_RESULT(linking libcm-mac.a with libcybermut.a)
  ln -sf $CYBERMUT_LIB_DIR/libcm-mac.a $CYBERMUT_LIB_DIR/libcybermut.a

  PHP_ADD_INCLUDE($CYBERMUT_INC_DIR)
 
  PHP_SUBST(CYBERMUT_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(cybermut, $CYBERMUT_LIB_DIR, CYBERMUT_SHARED_LIBADD)

  AC_DEFINE(HAVE_CYBERMUT, 1, [ ])                                                 


  PHP_EXTENSION(cybermut, $ext_shared)
fi
