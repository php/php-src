dnl $Source$
dnl $Id$

PHP_ARG_WITH(mnogosearch,for mnoGoSearch support,
[  --with-mnogosearch[=DIR]       Include mnoGoSearch support.  DIR is the mnoGoSearch base
                          install directory, defaults to /usr/local/udmsearch.])

  if test "$PHP_MNOGOSEARCH" != "no"; then
  
    if test "$PHP_MNOGOSEARCH" = "yes"; then
      MNOGOSEARCH_BINDIR=/usr/local/udmsearch/bin
      MNOGOSEARCH_INCDIR=/usr/local/udmsearch/include
      MNOGOSEARCH_LIBDIR=/usr/local/udmsearch/lib
    else
      MNOGOSEARCH_BINDIR=$PHP_MNOGOSEARCH/bin
      MNOGOSEARCH_INCDIR=$PHP_MNOGOSEARCH/include
      MNOGOSEARCH_LIBDIR=$PHP_MNOGOSEARCH/lib
    fi
    
    AC_ADD_INCLUDE($MNOGOSEARCH_INCDIR)
    
    if test -x "$MNOGOSEARCH_BINDIR/udm-config"; then
  	PHP_EVAL_LIBLINE(`$MNOGOSEARCH_BINDIR/udm-config --libs`, MNOGOSEARCH_SHARED_LIBADD)
    else
    	AC_ADD_LIBRARY_WITH_PATH(udmsearch, $MNOGOSEARCH_LIBDIR, MNOGOSEARCH_SHARED_LIBADD)
    fi
    
    AC_DEFINE(HAVE_MNOGOSEARCH,1,[ ])
    
    PHP_SUBST(MNOGOSEARCH_SHARED_LIBADD)
    PHP_EXTENSION(mnogosearch,$ext_shared)
    
  fi
