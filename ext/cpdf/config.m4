dnl $Id$

AC_DEFUN(CPDF_TEMP_SUBST,[
  old_LIBS=$LIBS
  $1
  LIBS=$old_LIBS
])

AC_DEFUN(CPDF_JPEG_TEST,[
  CPDF_TEMP_SUBST([
    AC_ARG_WITH(jpeg-dir,
    [  --with-jpeg-dir[=DIR]     CPDF: Set the path to libjpeg install prefix.],[
      for i in $withval /usr/local /usr; do
        if test -f "$i/lib/libjpeg.$SHLIB_SUFFIX_NAME" -o -f "$i/lib/libjpeg.a"; then
          CPDF_JPEG_DIR=$i
          break;
        fi
      done
      if test -z "$CPDF_JPEG_DIR"; then
        AC_MSG_CHECKING([for libjpeg (needed by cpdflib 2.x)])
        AC_MSG_ERROR([libjpeg.(a|so) not found.])
      fi
      LIBS="$LIBS -L$CPDF_JPEG_DIR/lib"
    ],) 
    AC_CHECK_LIB(jpeg,jpeg_read_header, ,[AC_MSG_ERROR(Problem with libjpeg.(a|so). Please check config.log for more information.)],)
    if test -z "$CPDF_JPEG_DIR"; then
      PHP_ADD_LIBRARY(jpeg)
    else
      PHP_ADD_LIBRARY_WITH_PATH(jpeg, $CPDF_JPEG_DIR/lib)
    fi
  ])
])

AC_DEFUN(CPDF_TIFF_TEST,[
  CPDF_TEMP_SUBST([
    AC_ARG_WITH(tiff-dir,
    [  --with-tiff-dir[=DIR]     CPDF: Set the path to libtiff install prefix.],[
      for i in $withval /usr/local /usr; do
        if test -f "$i/lib/libtiff.$SHLIB_SUFFIX_NAME" -o -f "$i/lib/libtiff.a"; then
          CPDF_TIFF_DIR=$i
          break;
        fi
      done
      if test -z "$CPDF_TIFF_DIR"; then
        AC_MSG_CHECKING([for libtiff (needed by cpdflib 2.x)])
        AC_MSG_ERROR([libtiff.(a|so) not found.])
      fi
      LIBS="$LIBS -L$CPDF_TIFF_DIR/lib"
    ],) 
    AC_CHECK_LIB(tiff,TIFFOpen, ,[AC_MSG_ERROR(Problem with libtiff.(a|so). Please check config.log for more information.)],)
    if test -z "$CPDF_TIFF_DIR"; then
      PHP_ADD_LIBRARY(tiff)
    else
      PHP_ADD_LIBRARY_WITH_PATH(tiff, $CPDF_TIFF_DIR/lib)
    fi
  ])
])

AC_MSG_CHECKING(for cpdflib support)
AC_ARG_WITH(cpdflib,
[  --with-cpdflib[=DIR]    Include cpdflib support (requires cpdflib >= 2).],
[
  if test "$withval" = "no"; then
    AC_MSG_RESULT(no)
  else
    AC_MSG_RESULT(yes)
    PHP_NEW_EXTENSION(cpdf, cpdf.c)
    CPDF_JPEG_TEST
    CPDF_TIFF_TEST
    for i in $withval /usr /usr/local; do
      if test -f "$i/include/cpdflib.h"; then
        CPDFLIB_INCLUDE=$i/include
        AC_MSG_CHECKING(for cpdflib.h)
        AC_MSG_RESULT([in $i/include])
        CPDF_TEMP_SUBST([
          LIBS="$LIBS -L$i/lib"
          AC_CHECK_LIB(cpdf, cpdf_open, [AC_DEFINE(HAVE_CPDFLIB,1,[Whether you have cpdflib])],
            [AC_MSG_ERROR([Cpdflib module requires cpdflib >= 2.])])
        ])
        if test "$i" != "/usr" -a "$i" != "/usr/local"; then
          PHP_ADD_LIBRARY_WITH_PATH(cpdf, $i/lib)
          PHP_ADD_INCLUDE($CPDFLIB_INCLUDE)
        else
          PHP_ADD_LIBRARY(cpdf)
        fi
        break
      fi
    done  
    if test -z "$CPDFLIB_INCLUDE"; then
      AC_MSG_CHECKING(for cpdflib.h)
      AC_MSG_ERROR([not found])
    fi
  fi
],[
  AC_MSG_RESULT(no)
])
