dnl
dnl $Id$
dnl

PHP_ARG_WITH(sybase-ct, for Sybase-CT support,
[  --with-sybase-ct[=DIR]  Include Sybase-CT support.  DIR is the Sybase home
                          directory. Defaults to /home/sybase.])

if test "$PHP_SYBASE" != "no"; then
  AC_MSG_ERROR([You can not use both --with-sybase and --with-sybase-ct in same build!])
fi

if test "$PHP_SYBASE_CT" != "no"; then
  AC_DEFINE(HAVE_SYBASE_CT,1,[ ])
  PHP_NEW_EXTENSION(sybase_ct, php_sybase_ct.c, $ext_shared)
  PHP_SUBST(SYBASE_CT_SHARED_LIBADD)
  
  if test "$PHP_SYBASE_CT" = "yes"; then
    SYBASE_CT_INCDIR=/home/sybase/include
    SYBASE_CT_LIBDIR=/home/sybase/lib
  else
    SYBASE_CT_INCDIR=$PHP_SYBASE_CT/include
    SYBASE_CT_LIBDIR=$PHP_SYBASE_CT/lib
  fi

  if test -f $SYBASE_CT_INCDIR/ctpublic.h; then
    PHP_ADD_INCLUDE($SYBASE_CT_INCDIR)
  else
    AC_MSG_ERROR([ctpublic.h missing!])
  fi
  
  PHP_ADD_LIBPATH($SYBASE_CT_LIBDIR, SYBASE_CT_SHARED_LIBADD)
  if test -f $SYBASE_CT_INCDIR/tds.h; then
    PHP_ADD_LIBRARY(ct,, SYBASE_CT_SHARED_LIBADD)
    SYBASE_CT_LIBS="-L$SYBASE_CT_LIBDIR -lct"
  else
    PHP_ADD_LIBRARY(cs,, SYBASE_CT_SHARED_LIBADD)
    PHP_ADD_LIBRARY(ct,, SYBASE_CT_SHARED_LIBADD)
    PHP_ADD_LIBRARY(comn,, SYBASE_CT_SHARED_LIBADD)
    PHP_ADD_LIBRARY(intl,, SYBASE_CT_SHARED_LIBADD)
  
    SYBASE_CT_LIBS="-L$SYBASE_CT_LIBDIR -lcs -lct -lcomn -lintl"
  
    PHP_CHECK_LIBRARY(tcl, netg_errstr, [
      PHP_ADD_LIBRARY(tcl,,SYBASE_CT_SHARED_LIBADD)
    ],[ 
      PHP_ADD_LIBRARY(sybtcl,,SYBASE_CT_SHARED_LIBADD)
    ],[ 
      $SYBASE_CT_LIBS 
    ])
  
    PHP_CHECK_LIBRARY(insck, insck__getVdate, [PHP_ADD_LIBRARY(insck,, SYBASE_CT_SHARED_LIBADD)],[],[-L$SYBASE_CT_LIBDIR])
    PHP_CHECK_LIBRARY(insck, bsd_tcp,         [PHP_ADD_LIBRARY(insck,, SYBASE_CT_SHARED_LIBADD)],[],[-L$SYBASE_CT_LIBDIR])
  fi
fi
