dnl $Id$

AC_MSG_CHECKING(whether to include Pdflib 2.0 support)
AC_ARG_WITH(pdflib,
[  --with-pdflib[=DIR]     Include pdflib 2.0 support.
                          DIR is the pdflib install directory,
                          defaults to /usr/local.],
[
echo $withval
  case "$withval" in
    no)
      AC_MSG_RESULT(no) ;;
    yes)
      AC_MSG_RESULT(yes)
      PHP_EXTENSION(pdf)
      AC_CHECK_LIB(pdf, PDF_close, [AC_DEFINE(HAVE_PDFLIB) PDFLIB_LIBS="-lpdf"],
        [AC_MSG_ERROR(pdflib extension requires pdflib 2.0.)])
      EXTRA_LIBS="$EXTRA_LIBS $PDFLIB_LIBS"
      ;;
    *)
      test -f $withval/include/pdflib.h && PDFLIB_INCLUDE="-I$withval/include"
      if test -n "$PDFLIB_INCLUDE" ; then
        AC_MSG_RESULT(yes)
        PHP_EXTENSION(pdf)
        old_LIBS=$LIBS

        if test -z $ZLIB_LIBS; then
          old_withval=$withval
          AC_MSG_CHECKING([for zlib (needed by pdflib 2.0)])
          AC_ARG_WITH(zlib-dir,
          [  --with-zlib-dir[=DIR]   zlib dir for pdflib 2.0 or include zlib support],[
            AC_MSG_RESULT( )
            if test -z $withval; then
              withval="/usr/local"
            fi
            LIBS="$LIBS -L$withval/lib -lz"
            AC_CHECK_LIB(z,deflate, [PDFLIB_LIBS="-L$withval/lib -lz"],[AC_MSG_RESULT(no)],)
          ],[
            AC_MSG_RESULT(no)
            AC_MSG_WARN(If configure fails try --with-zlib=<DIR>)
          ])
        else
          echo "checking for libz needed by pdflib 2.0... already zlib support"
          PDFLIB_LIBS="$ZLIB_LIBS"
          LIBS="$LIBS $ZLIB_LIBS"
        fi
        withval=$old_withval

        LIBS="$LIBS -L$withval/lib"
        AC_CHECK_LIB(pdf, PDF_close, [AC_DEFINE(HAVE_PDFLIB) PDFLIB_LIBS="-L$withval/lib -lpdf"],
          [AC_MSG_ERROR(pdflib extension requires pdflib 2.0.)])
        LIBS=$old_LIBS
        EXTRA_LIBS="$EXTRA_LIBS $PDFLIB_LIBS"
        INCLUDES="$INCLUDES $PDFLIB_INCLUDE"
      else
        AC_MSG_RESULT(no)
      fi ;;
  esac
],[
  AC_MSG_RESULT(no)
])
