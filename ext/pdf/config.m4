dnl
dnl $Id$
dnl

PHP_ARG_WITH(pdflib,for PDFlib support,
[  --with-pdflib[=DIR]     Include PDFlib support.])
remember_ext_shared=$ext_shared

if test "$PHP_PDFLIB" != "no"; then

  PHP_NEW_EXTENSION(pdf, pdf.c, $ext_shared)
  PHP_SUBST(PDFLIB_SHARED_LIBADD)

  dnl #
  dnl # Optional libraries for PDFlib 
  dnl #

  dnl # libjpeg
    
  PHP_ARG_WITH(jpeg-dir, for the location of libjpeg, 
  [  --with-jpeg-dir[=DIR]     PDFLIB: define libjpeg install directory.
                                    (OPTIONAL for PDFlib v4)])

  if test "$PHP_JPEG_DIR" != "no"; then
    PHP_CHECK_LIBRARY(jpeg,jpeg_read_header, 
    [
      PHP_ADD_LIBRARY_WITH_PATH(jpeg, $PHP_JPEG_DIR/lib, PDFLIB_SHARED_LIBADD)
    ],[
      AC_MSG_ERROR([libjpeg not found!])
    ],[
      -L$PHP_JPEG_DIR/lib
    ])
  else
    AC_MSG_WARN([If configure fails, try --with-jpeg-dir=<DIR>])
  fi

  dnl # libpng

  PHP_ARG_WITH(png-dir, for the location of libpng, 
  [  --with-png-dir[=DIR]      PDFLIB: define libpng install directory.
                                    (OPTIONAL for PDFlib v4)])
 
  if test "$PHP_PNG_DIR" != "no"; then
    PHP_CHECK_LIBRARY(png,png_create_info_struct, 
    [
      PHP_ADD_LIBRARY_WITH_PATH(png, $PHP_PNG_DIR/lib, PDFLIB_SHARED_LIBADD)
    ],[
      AC_MSG_ERROR([libpng not found!])
    ],[
      -L$PHP_PNG_DIR/lib
    ])
  else
    AC_MSG_WARN([If configure fails, try --with-png-dir=<DIR>])
  fi

  dnl # libtiff

  PHP_ARG_WITH(tiff-dir, for the location of libtiff,
  [  --with-tiff-dir[=DIR]     PDFLIB: define libtiff install directory.
                                    (OPTIONAL for PDFlib v4)])

  if test "$PHP_TIFF_DIR" != "no"; then
    PHP_CHECK_LIBRARY(tiff,TIFFOpen, 
    [
      PHP_ADD_LIBRARY_WITH_PATH(tiff, $PHP_TIFF_DIR/lib, PDFLIB_SHARED_LIBADD)
    ],[
      AC_MSG_ERROR([libtiff not found!])
    ],[
      -L$PHP_TIFF_DIR/lib
    ])
  else
    AC_MSG_WARN([If configure fails, try --with-tiff-dir=<DIR>])
  fi

  dnl # zlib
 
  AC_MSG_CHECKING([for the location of zlib])
  if test "$PHP_ZLIB_DIR" = "no"; then
    AC_MSG_RESULT([no. If configure fails, try --with-zlib-dir=<DIR>])
  else           
    AC_MSG_RESULT([$PHP_ZLIB_DIR])
    PHP_ADD_LIBRARY_WITH_PATH(z, $PHP_ZLIB_DIR/lib, PDFLIB_SHARED_LIBADD)
  fi


  dnl #
  dnl # The main PDFlib configure
  dnl #

  ext_shared=$remember_ext_shared
  case $PHP_PDFLIB in
    yes)
      AC_CHECK_LIB(pdf, PDF_show_boxed, [
        AC_DEFINE(HAVE_PDFLIB,1,[ ])
        PHP_ADD_LIBRARY(pdf,, PDFLIB_SHARED_LIBADD)
      ],[
        AC_MSG_ERROR([
PDFlib extension requires at least pdflib 3.x. You may also need libtiff, libjpeg, libpng and libz.
Use the options --with-tiff-dir=<DIR>, --with-jpeg-dir=<DIR>, --with-png-dir=<DIR> and --with-zlib-dir=<DIR>
See config.log for more information.
])
      ])
    ;;
    *)
      if test -f "$PHP_PDFLIB/include/pdflib.h" ; then

        PHP_CHECK_LIBRARY(pdf, PDF_show_boxed, 
        [
          AC_DEFINE(HAVE_PDFLIB,1,[ ]) 
          PHP_ADD_LIBRARY_WITH_PATH(pdf, $PHP_PDFLIB/lib, PDFLIB_SHARED_LIBADD)
          PHP_ADD_INCLUDE($PHP_PDFLIB/include)
        ],[
          AC_MSG_ERROR([
PDFlib extension requires at least pdflib 3.x. You may also need libtiff, libjpeg, libpng and libz.
Use the options --with-tiff-dir=<DIR>, --with-jpeg-dir=<DIR>, --with-png-dir=<DIR> and --with-zlib-dir=<DIR>
See config.log for more information.
])
        ],[
          -L$PHP_PDFLIB/lib
        ])
      else
        AC_MSG_ERROR([pdflib.h not found! Check the path passed to --with-pdflib=<PATH>. PATH should be the install prefix directory.])
      fi
    ;;
  esac
fi

