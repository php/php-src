dnl $Id$

PHP_ARG_WITH(sybase-ct, for Sybase-CT support,
[  --with-sybase-ct[=DIR]  Include Sybase-CT support.  DIR is the Sybase home
                          directory. Defaults to /home/sybase.])

if test "$PHP_SYBASE_CT" != "no"; then
  AC_DEFINE(HAVE_SYBASE_CT,1,[ ])
  PHP_EXTENSION(sybase_ct,$ext_shared)
  PHP_SUBST(SYBASE_CT_SHARED_LIBADD)
  
  if test "$PHP_SYBASE_CT" = "yes"; then
    SYBASE_CT_INCDIR=/home/sybase/include
    SYBASE_CT_LIBDIR=/home/sybase/lib
  else
    SYBASE_CT_INCDIR=$PHP_SYBASE_CT/include
    SYBASE_CT_LIBDIR=$PHP_SYBASE_CT/lib
  fi

  PHP_ADD_INCLUDE($SYBASE_CT_INCDIR)
  PHP_ADD_LIBPATH($SYBASE_CT_LIBDIR, SYBASE_CT_SHARED_LIBADD)
  PHP_ADD_LIBRARY(cs,, SYBASE_CT_SHARED_LIBADD)
  PHP_ADD_LIBRARY(ct,, SYBASE_CT_SHARED_LIBADD)
  PHP_ADD_LIBRARY(comn,, SYBASE_CT_SHARED_LIBADD)
  PHP_ADD_LIBRARY(intl,, SYBASE_CT_SHARED_LIBADD)

  SYBASE_CT_LIBS="-L$SYBASE_CT_LIBDIR -lcs -lct -lcomn -lintl"

  AC_CHECK_LIB(tcl, netg_errstr, [
    PHP_ADD_LIBRARY(tcl,,SYBASE_CT_SHARED_LIBADD)
  ],[ 
    PHP_ADD_LIBRARY(sybtcl,,SYBASE_CT_SHARED_LIBADD)
  ],[ 
    $SYBASE_CT_LIBS 
  ])

  PHP_CHECK_LIBRARY(insck, insck__getVdate, [PHP_ADD_LIBRARY(insck,, SYBASE_CT_SHARED_LIBADD)],[],[-L$SYBASE_CT_LIBDIR])
  PHP_CHECK_LIBRARY(insck, bsd_tcp,         [PHP_ADD_LIBRARY(insck,, SYBASE_CT_SHARED_LIBADD)],[],[-L$SYBASE_CT_LIBDIR])
fi
