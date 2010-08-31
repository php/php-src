dnl
dnl $Id$
dnl

PHP_ARG_WITH(sybase-ct, for Sybase-CT support,
[  --with-sybase-ct[=DIR]  Include Sybase-CT support.  DIR is the Sybase home
                          directory [/home/sybase]])

if test "$PHP_SYBASE_CT" != "no"; then

  if test "$PHP_SYBASE" && test "$PHP_SYBASE" != "no" && test "$ext_shared" = "no"; then
    AC_MSG_ERROR([You can not use both --with-sybase and --with-sybase-ct in same build!])
  fi

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
  if test -f $SYBASE_CT_INCDIR/tds.h || test -f $SYBASE_CT_INCDIR/tds_sysdep_public.h; then
    PHP_ADD_LIBRARY(ct,, SYBASE_CT_SHARED_LIBADD)
    SYBASE_CT_LIBS="-L$SYBASE_CT_LIBDIR -lct"
  elif test -f $SYBASE_CT_INCDIR/libsybct64.so; then
    PHP_ADD_LIBRARY(sybcs64,, SYBASE_CT_SHARED_LIBADD)
    PHP_ADD_LIBRARY(sybct64,, SYBASE_CT_SHARED_LIBADD)
    PHP_ADD_LIBRARY(sybcomn64,, SYBASE_CT_SHARED_LIBADD)
    PHP_ADD_LIBRARY(sybintl64,, SYBASE_CT_SHARED_LIBADD)

    ac_solid_uname_s=`uname -s 2>/dev/null`
    case $ac_solid_uname_s in
      *OSF*)                            ;; # Tru64/DEC OSF does NOT use the SYB_LP64 define
      *) CFLAGS="${CFLAGS} -DSYB_LP64"  ;; # 
    esac
    SYBASE_CT_LIBS="-L$SYBASE_CT_LIBDIR -lsybcs64 -lsybct64 -lsybcomn64 -lsybintl64"
  
    PHP_CHECK_LIBRARY(sybtcl64, netg_errstr, [
      PHP_ADD_LIBRARY(sybtcl64,,SYBASE_CT_SHARED_LIBADD)
    ],[ 
      PHP_ADD_LIBRARY(sybtcl64,,SYBASE_CT_SHARED_LIBADD)
    ],[ 
      $SYBASE_CT_LIBS 
    ])
  
    PHP_CHECK_LIBRARY(insck64, insck__getVdate, [PHP_ADD_LIBRARY(insck64,, SYBASE_CT_SHARED_LIBADD)],[],[-L$SYBASE_CT_LIBDIR])
    PHP_CHECK_LIBRARY(insck64, bsd_tcp,         [PHP_ADD_LIBRARY(insck64,, SYBASE_CT_SHARED_LIBADD)],[],[-L$SYBASE_CT_LIBDIR])
  elif test -f $SYBASE_CT_INCDIR/libsybct.so; then
    PHP_ADD_LIBRARY(sybcs,, SYBASE_CT_SHARED_LIBADD)
    PHP_ADD_LIBRARY(sybct,, SYBASE_CT_SHARED_LIBADD)
    PHP_ADD_LIBRARY(sybcomn,, SYBASE_CT_SHARED_LIBADD)
    PHP_ADD_LIBRARY(sybintl,, SYBASE_CT_SHARED_LIBADD)
  
    SYBASE_CT_LIBS="-L$SYBASE_CT_LIBDIR -lsybcs -lsybct -lsybcomn -lsybintl"
  
    PHP_CHECK_LIBRARY(sybtcl, netg_errstr, [
      PHP_ADD_LIBRARY(sybtcl,,SYBASE_CT_SHARED_LIBADD)
    ],[ 
      PHP_ADD_LIBRARY(sybtcl,,SYBASE_CT_SHARED_LIBADD)
    ],[ 
      $SYBASE_CT_LIBS 
    ])
  
    PHP_CHECK_LIBRARY(insck, insck__getVdate, [PHP_ADD_LIBRARY(insck,, SYBASE_CT_SHARED_LIBADD)],[],[-L$SYBASE_CT_LIBDIR])
    PHP_CHECK_LIBRARY(insck, bsd_tcp,         [PHP_ADD_LIBRARY(insck,, SYBASE_CT_SHARED_LIBADD)],[],[-L$SYBASE_CT_LIBDIR])
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
