dnl $Id$

AC_MSG_CHECKING(whether to include Pdflib 2.x support)
AC_ARG_WITH(pdflib,
[  --with-pdflib[=DIR]     Include pdflib 2.x support.
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
      AC_CHECK_LIB(pdf, PDF_close, [AC_DEFINE(HAVE_PDFLIB) PDFLIB_LIBS="-L/usr/local -lpdf -lz"],
        [AC_MSG_ERROR(pdflib extension requires pdflib 2.x. You may as well need libtiff and libjpeg. In such a case use the options --with-tiff-dir=<DIR> and --with-jpeg-dir=<DIR>)])
      EXTRA_LIBS="$EXTRA_LIBS $PDFLIB_LIBS"
      ;;
    *)
      test -f $withval/include/pdflib.h && PDFLIB_INCLUDE="-I$withval/include"
      if test -n "$PDFLIB_INCLUDE" ; then
        AC_MSG_RESULT(yes)
        PHP_EXTENSION(pdf)
        old_LIBS=$LIBS
        old_withval=$withval

        if test $HAVE_ZLIB; then
          AC_MSG_CHECKING([for zlib (needed by pdflib 2.x)])
          AC_ARG_WITH(zlib-dir,
          [  --with-zlib-dir[=DIR]   zlib dir for pdflib 2.x or include zlib support],[
            AC_MSG_RESULT( )
            if test -z $withval; then
              withval="/usr/local"
            fi
            AC_CHECK_LIB(z,deflate, [PDFLIB_LIBS="-L$withval/lib -lz"],[AC_MSG_RESULT(no)],)
            LIBS="$LIBS -L$withval/lib -lz"
          ],[
            AC_MSG_RESULT(no)
            AC_MSG_WARN(If configure fails try --with-zlib=<DIR>)
          ])
        else
          echo "checking for libz needed by pdflib 2.x... already zlib support"
          PDFLIB_LIBS="$ZLIB_LIBS"
          LIBS="$LIBS -lz"
        fi

        AC_MSG_CHECKING([for libjpeg (needed by pdflib 2.x)])
        AC_ARG_WITH(jpeg-dir,
        [  --with-jpeg-dir[=DIR]   jpeg dir for pdflib 2.x],[
          AC_MSG_RESULT(yes)
          if test -z $withval; then
            withval="/usr/local"
          fi
          LIBS="$LIBS -L$withval/lib -ljpeg"
          AC_CHECK_LIB(jpeg,jpeg_read_header, [PDFLIB_LIBS="$PDFLIB_LIBS -L$withval/lib -ljpeg"],[AC_MSG_RESULT(no)],)
          LIBS="$LIBS -L$withval/lib -ljpeg"
        ],[
          AC_MSG_RESULT(no)
          AC_MSG_WARN(If configure fails try --with-jpeg-dir=<DIR>)
        ]) 

        AC_MSG_CHECKING([for libtiff (needed by pdflib 2.x)])
        AC_ARG_WITH(tiff-dir,
        [  --with-tiff-dir[=DIR]   tiff dir for pdflib 2.x],[
          AC_MSG_RESULT(yes)
          if test -z $withval; then
            withval="/usr/local"
          fi
          LIBS="$LIBS -L$withval/lib -ltiff -ljpeg"
          AC_CHECK_LIB(tiff,TIFFOpen, [PDFLIB_LIBS="$PDFLIB_LIBS -L$withval/lib -ltiff"],[AC_MSG_RESULT(no)],)
        ],[
          AC_MSG_RESULT(no)
          AC_MSG_WARN(If configure fails try --with-tiff-dir=<DIR>)
        ]) 
        withval=$old_withval

        LIBS="$LIBS -L$withval/lib"
        AC_CHECK_LIB(pdf, PDF_close, [AC_DEFINE(HAVE_PDFLIB) PDFLIB_LIBS="$PDFLIB_LIBS -L$withval/lib -lpdf"],
          [AC_MSG_ERROR(pdflib extension requires pdflib 2.x.)])
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
