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
      old_LDFLAGS=$LDFLAGS
		  old_LIBS=$LIBS
		  LIBS="$LIBS -ltiff -ljpeg -lz"
      AC_CHECK_LIB(pdf, PDF_close, [AC_DEFINE(HAVE_PDFLIB,,[ ])],
        [AC_MSG_ERROR(pdflib extension requires pdflib 2.x. You may as well need libtiff and libjpeg. In such a case use the options --with-tiff-dir=<DIR> and --with-jpeg-dir=<DIR>)])
      LIBS=$old_LIBS
      LDFLAGS=$old_LDFLAGS
      AC_ADD_LIBRARY(pdf)
      AC_ADD_LIBRARY(tiff)
      AC_ADD_LIBRARY(jpeg)
      AC_ADD_LIBRARY(z)
      ;;
    *)
      test -f $withval/include/pdflib.h && PDFLIB_INCLUDE="$withval/include"
      if test -n "$PDFLIB_INCLUDE" ; then
        AC_MSG_RESULT(yes)
        PHP_EXTENSION(pdf)
        old_withval=$withval

        if test $HAVE_ZLIB; then
          AC_MSG_CHECKING([for zlib (needed by pdflib 2.x)])
          AC_ARG_WITH(zlib-dir,
          [  --with-zlib-dir[=DIR]   zlib dir for pdflib 2.x or include zlib support],[
            AC_MSG_RESULT( )
            if test -z $withval; then
              withval="/usr/local"
            fi
            old_LIBS=$LIBS
            LIBS="$LIBS -L$withval/lib"
            AC_CHECK_LIB(z,deflate, [PDFLIB_LIBS="-L$withval/lib -lz"],[AC_MSG_RESULT(no)],)
            LIBS=$old_LIBS
            AC_ADD_LIBRARY_WITH_PATH(z, $withval/lib)
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
          old_LIBS=$LIBS
          LIBS="$LIBS -L$withval/lib"
          AC_CHECK_LIB(jpeg,jpeg_read_header, [PDFLIB_LIBS="$PDFLIB_LIBS -L$withval/lib -ljpeg"],[AC_MSG_RESULT(no)],)
          LIBS=$old_LIBS
          AC_ADD_LIBRARY_WITH_PATH(jpeg, $withval/lib)
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
          old_LIBS=$LIBS
          LIBS="$LIBS -L$withval/lib"
          AC_CHECK_LIB(tiff,TIFFOpen, [PDFLIB_LIBS="$PDFLIB_LIBS -L$withval/lib -ltiff"],[AC_MSG_RESULT(no)],)
          LIBS=$old_LIBS
          AC_ADD_LIBRARY_WITH_PATH(tiff, $withval/lib)
          LIBS="$LIBS -L$withval/lib -ltiff"
        ],[
          AC_MSG_RESULT(no)
          AC_MSG_WARN(If configure fails try --with-tiff-dir=<DIR>)
        ]) 
        withval=$old_withval

        old_LIBS=$LIBS
        LIBS="$LIBS -L$withval/lib"
        AC_CHECK_LIB(pdf, PDF_close, [AC_DEFINE(HAVE_PDFLIB,,[ ]) PDFLIB_LIBS="$PDFLIB_LIBS -L$withval/lib -lpdf"],
          [AC_MSG_ERROR(pdflib extension requires pdflib 2.x.)])
        LIBS=$old_LIBS
        AC_ADD_LIBRARY_WITH_PATH(pdf, $withval/lib)
        AC_ADD_INCLUDE($PDFLIB_INCLUDE)
      else
        AC_MSG_RESULT(no)
      fi ;;
  esac
],[
  AC_MSG_RESULT(no)
])
