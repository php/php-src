dnl $Id$

AC_MSG_CHECKING(whether to include DOM support)
AC_ARG_WITH(dom,
[  --with-dom[=DIR]        Include DOM support (requires libxml >= 2.0).
                          DIR is the libxml install directory,
                          defaults to /usr.],
[
  case "$withval" in
    no)
      AC_MSG_RESULT(no) ;;
    yes)
      AC_MSG_RESULT(yes)
      PHP_EXTENSION(domxml)
      old_LIBS=$LIBS
      LIBS="$LIBS -lz"
      AC_CHECK_LIB(xml, xmlFreeURI, [AC_DEFINE(HAVE_DOMXML,1,[ ])],
        [AC_MSG_ERROR(DOM module requires libxml >= 2.0)])
      LIBS=$old_LIBS
      AC_ADD_LIBRARY(z)
      AC_ADD_LIBRARY(xml)
      AC_ADD_INCLUDE(/usr/local/include)
      ;;
    *)
      test -f $withval/include/libxml/tree.h && DOMXML_INCLUDE="$withval/include"
      if test -n "$DOMXML_INCLUDE" ; then
        AC_MSG_RESULT(yes)
        PHP_EXTENSION(domxml)
        old_LIBS=$LIBS
        LIBS="$LIBS -L$withval/lib"

        if test $HAVE_ZLIB; then
          old_withval=$withval
          AC_MSG_CHECKING([for zlib (needed by DOM support)])
          AC_ARG_WITH(zlib-dir,
          [  --with-zlib-dir[=DIR]   zlib dir for libxml or include zlib support],[
            AC_MSG_RESULT( )
            if test -z $withval; then
              withval="/usr/local"
            fi
            LIBS="$LIBS -L$withval/lib -lz"
            AC_CHECK_LIB(z,deflate, ,[AC_MSG_RESULT(no)],)
            AC_ADD_LIBRARY_WITH_PATH(z, $withval/lib)
          ],[
            AC_MSG_RESULT(no)
            AC_MSG_WARN(If configure fails try --with-zlib=<DIR>)
          ])
          withval=$old_withval
        else
          echo "checking for libz needed by pdflib 3.0... already zlib support"
          LIBS="$LIBS -L$withval/lib -lz"
        fi

        AC_CHECK_LIB(xml, xmlFreeURI, [AC_DEFINE(HAVE_DOMXML,1,[ ])],
          [AC_MSG_ERROR(DOM module requires libxml >= 2.0.)])
        LIBS=$old_LIBS
        AC_ADD_LIBRARY_WITH_PATH(xml, $withval/lib)
        AC_ADD_INCLUDE($DOMXML_INCLUDE)
      else
        AC_MSG_RESULT(no)
      fi ;;
  esac
],[
  AC_MSG_RESULT(no)
])
