dnl $Id$

AC_MSG_CHECKING(whether to include Pdflib 3.x support)

PHP_ARG_WITH(pdflib,whether to include pdflib support,
[  --with-pdflib[=DIR]     Include pdflib 3.x support. DIR is the pdflib
			  base install directory, defaults to /usr/local
			  Set DIR to "shared" to build as dl, or "shared,DIR"
                          to build as dl and still specify DIR.])

  case "$PHP_PDFLIB" in
    no)
      AC_MSG_RESULT(no) ;;
    yes)
      AC_MSG_RESULT(yes)
      PHP_EXTENSION(pdf, $ext_shared)
      old_LDFLAGS=$LDFLAGS
		  old_LIBS=$LIBS
		  LIBS="$LIBS -ltiff -ljpeg -lpng -lz"
      AC_CHECK_LIB(pdf, PDF_show_boxed, [AC_DEFINE(HAVE_PDFLIB,1,[ ])],
        [AC_MSG_ERROR(pdflib extension requires at least pdflib 3.x. You may also need libtiff and libjpeg. If so, use the options --with-tiff-dir=<DIR> and --with-jpeg-dir=<DIR>)])
      LIBS=$old_LIBS
      LDFLAGS=$old_LDFLAGS
      PHP_SUBST(PDFLIB_SHARED_LIBADD)
      AC_ADD_LIBRARY(pdf, PDFLIB_SHARED_LIBADD)
      AC_ADD_LIBRARY(tiff)
      AC_ADD_LIBRARY(png)
      AC_ADD_LIBRARY(jpeg)
      AC_ADD_LIBRARY(z)
      ;;
    *)
      test -f $withval/include/pdflib.h && PDFLIB_INCLUDE="$withval/include"
      if test -n "$PDFLIB_INCLUDE" ; then
        AC_MSG_RESULT(yes)
        PHP_EXTENSION(pdf, $ext_shared)
        old_withval=$withval

        if test $HAVE_ZLIB; then
          AC_MSG_CHECKING([for zlib (needed by pdflib 3.x)])
          AC_ARG_WITH(zlib-dir,
          [  --with-zlib-dir[=DIR]   zlib dir for pdflib 3.x or include zlib support],[
            if test -z $withval; then
              withval="/usr/local"
            fi
            old_LIBS=$LIBS
            LIBS="$LIBS -L$withval/lib"
            AC_CHECK_LIB(z,deflate, [PDFLIB_LIBS="-L$withval/lib -lz"],[AC_MSG_RESULT(no)],)
            LIBS=$old_LIBS
            AC_ADD_LIBRARY_WITH_PATH(z, $withval/lib)
          ],[
            AC_CHECK_LIB(z,deflate, ,[AC_MSG_RESULT(no, try --with-zlib=<DIR>)],)
            AC_ADD_LIBRARY(z)
            LIBS="$LIBS -lz"
          ])
        else
          echo "checking for libz needed by pdflib 3.x... already zlib support"
          PDFLIB_LIBS="$ZLIB_LIBS"
          LIBS="$LIBS -lz"
        fi

        AC_ARG_WITH(jpeg-dir,
        [  --with-jpeg-dir[=DIR]   jpeg dir for pdflib 3.x],[
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
          AC_CHECK_LIB(jpeg,jpeg_read_header, ,[AC_MSG_RESULT(no, try --with-jpeg-dir=<DIR>)],)
          AC_ADD_LIBRARY(jpeg)
          LIBS="$LIBS -ljpeg"
        ]) 

        AC_ARG_WITH(png-dir,
        [  --with-png-dir[=DIR]    png dir for pdflib 3.x],[
          if test -z $withval; then
            withval="/usr/local"
          fi
          old_LIBS=$LIBS
          LIBS="$LIBS -L$withval/lib"
          AC_CHECK_LIB(png,png_create_info_struct, [PDFLIB_LIBS="$PDFLIB_LIBS -L$withval/lib -lpng"],[AC_MSG_RESULT(no)],)
          LIBS=$old_LIBS
          AC_ADD_LIBRARY_WITH_PATH(png, $withval/lib)
          LIBS="$LIBS -L$withval/lib -lpng"
        ],[
          AC_CHECK_LIB(png,png_create_info_struct, ,[AC_MSG_RESULT(no, try --with-png-dir=<DIR>)],)
          AC_ADD_LIBRARY(png)
          LIBS="$LIBS -lpng"
        ]) 

        AC_ARG_WITH(tiff-dir,
        [  --with-tiff-dir[=DIR]   tiff dir for pdflib 3.x],[
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
          AC_CHECK_LIB(tiff,TIFFOpen, ,[AC_MSG_RESULT(no, Try --with-tiff-dir=<DIR>)],)
          AC_ADD_LIBRARY(tiff)
          LIBS="$LIBS -ltiff"
        ]) 
        withval=$old_withval

        old_LIBS=$LIBS
        LIBS="$LIBS -L$withval/lib"
        AC_CHECK_LIB(pdf, PDF_show_boxed, [AC_DEFINE(HAVE_PDFLIB,1,[ ]) PDFLIB_LIBS="$PDFLIB_LIBS -L$withval/lib -lpdf"],
          [AC_MSG_ERROR(pdflib extension requires pdflib 3.x.)])
        LIBS=$old_LIBS
	PHP_SUBST(PDFLIB_SHARED_LIBADD)
        AC_ADD_LIBRARY_WITH_PATH(pdf, $withval/lib, PDFLIB_SHARED_LIBADD)
        AC_ADD_INCLUDE($PDFLIB_INCLUDE)
      else
        AC_MSG_RESULT(no)
      fi ;;
  esac
