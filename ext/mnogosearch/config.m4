dnl
dnl $Source$
dnl $Id$
dnl

PHP_ARG_WITH(mnogosearch,for mnoGoSearch support,
[  --with-mnogosearch[=DIR]
                          Include mnoGoSearch support.  DIR is the mnoGoSearch 
                          base install directory, defaults to 
                          /usr/local/mnogosearch.])

  if test "$PHP_MNOGOSEARCH" != "no"; then
  
    if test "$PHP_MNOGOSEARCH" = "yes"; then
      MNOGOSEARCH_BINDIR=/usr/local/mnogosearch/bin
      MNOGOSEARCH_INCDIR=/usr/local/mnogosearch/include
      MNOGOSEARCH_LIBDIR=/usr/local/mnogosearch/lib
    else
      MNOGOSEARCH_BINDIR=$PHP_MNOGOSEARCH/bin
      MNOGOSEARCH_INCDIR=$PHP_MNOGOSEARCH/include
      MNOGOSEARCH_LIBDIR=$PHP_MNOGOSEARCH/lib
    fi
    
    AC_MSG_CHECKING(for mnoGoSearch version)    
    
    if test -x "$MNOGOSEARCH_BINDIR/udm-config"; then
    	MNOGOSEARCH_VERSION=`$MNOGOSEARCH_BINDIR/udm-config --version`
    	MNOGOSEARCH_VERSION_ID=`$MNOGOSEARCH_BINDIR/udm-config --version-id`

	if test $? -ne 0; then
		AC_MSG_RESULT(<= 3.1.9)    
		AC_MSG_ERROR(mnoGoSearch 3.1.10 at least required)
	fi
	
	if test "$MNOGOSEARCH_VERSION_ID" -lt 30110; then
		AC_MSG_RESULT(<= 3.1.9)
		AC_MSG_ERROR(mnoGoSearch 3.1.10 at least required)
	fi
	
	AC_MSG_RESULT($MNOGOSEARCH_VERSION)
	
  	PHP_EVAL_LIBLINE(`$MNOGOSEARCH_BINDIR/udm-config --libs`, MNOGOSEARCH_SHARED_LIBADD)
    else
	AC_MSG_ERROR(udm-config script not found in $MNOGOSEARCH_BINDIR)
    fi
    
    PHP_ADD_INCLUDE($MNOGOSEARCH_INCDIR)
    
    AC_DEFINE(HAVE_MNOGOSEARCH,1,[ ])
    
    PHP_SUBST(MNOGOSEARCH_SHARED_LIBADD)
    PHP_NEW_EXTENSION(mnogosearch, php_mnogo.c, $ext_shared)
    
  fi
