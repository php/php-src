dnl
dnl $Id$
dnl

PHP_ARG_WITH(pdflib,whether to include PDFlib support,
[  --with-pdflib[=DIR]     Include pdflib 3.x support. DIR is the pdflib
                          base install directory, defaults to /usr/local
                          Set DIR to "shared" to build as dl, or "shared,DIR"
                          to build as dl and still specify DIR.])

if test "$PHP_PDFLIB" != "no"; then

  PHP_SUBST(PDFLIB_SHARED_LIBADD)
  PHP_EXTENSION(pdf, $ext_shared)

  case "$PHP_PDFLIB" in
    yes)
      AC_CHECK_LIB(pdf, PDF_show_boxed, [
        AC_DEFINE(HAVE_PDFLIB,1,[ ])
      ],[
        AC_MSG_ERROR(pdflib extension requires at least pdflib 3.x. You may also need libtiff and libjpeg. If so, use the options --with-tiff-dir=<DIR> and --with-jpeg-dir=<DIR>)
      ],[
        -ltiff -ljpeg -lpng -lz
      ])

      PHP_ADD_LIBRARY(pdf,, PDFLIB_SHARED_LIBADD)
      PHP_ADD_LIBRARY(tiff,, PDFLIB_SHARED_LIBADD)
      PHP_ADD_LIBRARY(png,, PDFLIB_SHARED_LIBADD)
      PHP_ADD_LIBRARY(jpeg,, PDFLIB_SHARED_LIBADD)
      PHP_ADD_LIBRARY(z,, PDFLIB_SHARED_LIBADD)
    ;;
    *)
      test -f $PHP_PDFLIB/include/pdflib.h && PDFLIB_INCLUDE="$PHP_PDFLIB/include"
      if test -n "$PDFLIB_INCLUDE" ; then

        if test "$PHP_ZLIB" = "no"; then
          AC_MSG_CHECKING([for zlib (needed by pdflib)])

          AC_CHECK_LIB(z,deflate, 
          [
            PHP_ADD_LIBRARY_WITH_PATH(z, $PHP_ZLIB_DIR/lib, PDFLIB_SHARED_LIBADD)
          ],[
            AC_MSG_ERROR(libz.(a|so) not found! Try with --with-zlib-dir=<DIR>)
          ],[
            -L$PHP_ZLIB_DIR/lib
          ])
        else
          echo "checking for libz needed by pdflib... already zlib support"
        fi

 
        PHP_ARG_WITH(jpeg-dir, for the location of libjpeg, 
        [  --with-jpeg-dir[=DIR]     PDFLIB: define libjpeg install directory])

        if test "$PHP_JPEG_DIR" != "no" -a "$PHP_JPEG_DIR"; then
          AC_CHECK_LIB(jpeg,jpeg_read_header, 
          [
            PHP_ADD_LIBRARY_WITH_PATH(jpeg, $PHP_JPEG_DIR/lib, PDFLIB_SHARED_LIBADD)
          ],[
            AC_MSG_ERROR(libjpeg not found!)
          ],[
            -L$PHP_JPEG_DIR/lib
          ])
        else
          AC_CHECK_LIB(jpeg,jpeg_read_header,[ ],[AC_MSG_RESULT(no, try --with-jpeg-dir=<DIR>)])
          PHP_ADD_LIBRARY(jpeg,, PDFLIB_SHARED_LIBADD)
        fi

 
        PHP_ARG_WITH(png-dir, for the location of libpng, 
        [  --with-png-dir[=DIR]      PDFLIB: define libpng install directory])
        
        if test "$PHP_PNG_DIR" != "no" -a "$PHP_PNG_DIR"; then
          AC_CHECK_LIB(png,png_create_info_struct, 
          [
            PHP_ADD_LIBRARY_WITH_PATH(png, $PHP_PNG_DIR/lib, PDFLIB_SHARED_LIBADD)
          ],[
            AC_MSG_ERROR(libpng not found!)
          ],[
            -L$PHP_PNG_DIR/lib
          ])
        else
          AC_CHECK_LIB(png,png_create_info_struct,[ ],[AC_MSG_RESULT(no, try --with-png-dir=<DIR>)])
          PHP_ADD_LIBRARY(png,, PDFLIB_SHARED_LIBADD)
        fi
        

        PHP_ARG_WITH(tiff-dir, for the location of libtiff,
        [  --with-tiff-dir[=DIR]     PDFLIB: define libtiff install directory])

        if test "$PHP_TIFF_DIR" != "no" -a "$PHP_TIFF_DIR"; then
          AC_CHECK_LIB(tiff,TIFFOpen, 
          [
            PHP_ADD_LIBRARY_WITH_PATH(tiff, $PHP_TIFF_DIR/lib, PDFLIB_SHARED_LIBADD)
          ],[
            AC_MSG_ERROR(libtiff not found!)
          ],[
            -L$PHP_TIFF_DIR/lib
          ])
        else
          AC_CHECK_LIB(tiff,TIFFOpen, ,[AC_MSG_RESULT(no, Try --with-tiff-dir=<DIR>)],)
          PHP_ADD_LIBRARY(tiff,, PDFLIB_SHARED_LIBADD)
        fi


        AC_CHECK_LIB(pdf, PDF_show_boxed, 
        [
          AC_DEFINE(HAVE_PDFLIB,1,[ ]) 
          PHP_ADD_LIBRARY_WITH_PATH(pdf, $PHP_PDFLIB/lib, PDFLIB_SHARED_LIBADD)
          PHP_ADD_INCLUDE($PDFLIB_INCLUDE)
        ],[
          AC_MSG_ERROR(pdflib extension requires pdflib 3.x.)
        ],[
          -L$PHP_PDFLIB/lib
        ])
      fi 
    ;;
  esac
fi
