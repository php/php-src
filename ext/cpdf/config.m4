dnl $Id$

AC_DEFUN(CPDF_TEMP_SUBST,[
  old_LIBS=$LIBS
  old_withval=$withval
  $1
  LIBS=$old_LIBS
  withval=$old_withval
])

AC_DEFUN(CPDF_LIB_TEST,[
  CPDF_TEMP_SUBST([
    AC_MSG_CHECKING([for libjpeg (needed by cpdflib 2.x)])
    AC_ARG_WITH(jpeg-dir,
    [  --with-jpeg-dir[=DIR]   jpeg dir for cpdflib 2.x],[
      for i in $withval /usr/local /usr; do
        if test -f $i/lib/libjpeg.$SHLIB_SUFFIX_NAME -o -f $i/lib/libjpeg.a; then
         CPDF_JPEG_DIR=$i
        fi
      done
      if test -z "$CPDF_JPEG_DIR"; then
        AC_MSG_ERROR([libjpeg.(a|so) not found.])
      fi
      LIBS="$LIBS -L$CPDF_JPEG_DIR/lib"
      AC_CHECK_LIB(jpeg,jpeg_read_header, ,[AC_MSG_ERROR(Problem with libjpeg.(a|so). Please check config.log for more information.)],)
      PHP_ADD_LIBRARY_WITH_PATH(jpeg, $CPDF_JPEG_DIR/lib)
    ],[
      AC_CHECK_LIB(jpeg,jpeg_read_header, ,[AC_MSG_ERROR(Problem with libjpeg.(a|so). Please check config.log for more information.)],)
      PHP_ADD_LIBRARY(jpeg)
    ]) 
  ])

  CPDF_TEMP_SUBST([
    AC_MSG_CHECKING([for libtiff (needed by cpdflib 2.x)])
    AC_ARG_WITH(tiff-dir,
    [  --with-tiff-dir[=DIR]   tiff dir for cpdflib 2.x],[
      for i in $withval /usr/local /usr; do
        if test -f $i/lib/libtiff.$SHLIB_SUFFIX_NAME -o -f $i/lib/libtiff.a; then
         CPDF_TIFF_DIR=$i
        fi
      done
      if test -z "$CPDF_TIFF_DIR"; then
        AC_MSG_ERROR([libtiff.(a|so) not found.])
      fi
      LIBS="$LIBS -L$CPDF_TIFF_DIR/lib"
      AC_CHECK_LIB(tiff,TIFFOpen, ,[AC_MSG_ERROR(Problem with libtiff.(a|so). Please check config.log for more information.)],)
      PHP_ADD_LIBRARY_WITH_PATH(tiff, $CPDF_TIFF_DIR/lib)
    ],[
      AC_CHECK_LIB(tiff,TIFFOpen, ,[AC_MSG_ERROR(Problem with libtiff.(a|so). Please check config.log for more information.)],)
      PHP_ADD_LIBRARY(tiff)
    ]) 
  ])
])

AC_MSG_CHECKING(for cpdflib support)
AC_ARG_WITH(cpdflib,
[  --with-cpdflib[=DIR]    Include cpdflib support (requires cpdflib >= 2).],
[
  case $withval in
    no)
      AC_MSG_RESULT(no) ;;
    yes)
      AC_MSG_RESULT(yes)
      PHP_NEW_EXTENSION(cpdf, cpdf.c)
      CPDF_LIB_TEST
      AC_CHECK_LIB(cpdf, cpdf_open, [AC_DEFINE(HAVE_CPDFLIB,1,[Whether you have cpdflib])],
        [AC_MSG_ERROR([Cpdf module requires cpdflib >= 2.])])
      PHP_ADD_LIBRARY(cpdf)
      ;;
    *)
      test -f $withval/include/cpdflib.h && CPDFLIB_INCLUDE=$withval/include
      if test -n "$CPDFLIB_INCLUDE" ; then
        AC_MSG_RESULT(yes)
        PHP_NEW_EXTENSION(cpdf, cpdf.c)
        CPDF_LIB_TEST
        old_LIBS=$LIBS
        LIBS="$LIBS -L$withval/lib"
        AC_CHECK_LIB(cpdf, cpdf_open, [AC_DEFINE(HAVE_CPDFLIB,1,[Whether you have cpdflib])],
          [AC_MSG_ERROR([Cpdflib module requires cpdflib >= 2.])])
        LIBS=$old_LIBS
        PHP_ADD_LIBRARY_WITH_PATH(cpdf, $withval/lib)
        PHP_ADD_INCLUDE($CPDFLIB_INCLUDE)
      else
        AC_MSG_ERROR([required haeder not found])
      fi ;;
  esac
],[
  AC_MSG_RESULT(no)
])
