dnl $Id$

AC_MSG_CHECKING(whether to include cpdflib support)
AC_ARG_WITH(cpdflib,
[  --with-cpdflib[=DIR]    Include cpdflib support (requires cpdflib >= 2).
                          DIR is the cpdfllib install directory,
                          defaults to /usr.],
[
  case $withval in
    no)
      AC_MSG_RESULT(no) ;;
    yes)
      AC_MSG_RESULT(yes)
      PHP_EXTENSION(cpdf)
      AC_CHECK_LIB(cpdf, cpdf_open, [AC_DEFINE(HAVE_CPDFLIB,1,[Whether you have cpdflib])],
        [AC_MSG_ERROR([Cpdf module requires cpdflib >= 2.])])
      PHP_ADD_LIBRARY(cpdf)
      PHP_ADD_LIBRARY(tiff)
      PHP_ADD_LIBRARY(jpeg)
      ;;
    *)
      test -f $withval/include/cpdflib.h && CPDFLIB_INCLUDE=$withval/include
      if test -n "$CPDFLIB_INCLUDE" ; then
        AC_MSG_RESULT(yes)
        PHP_EXTENSION(cpdf)
        old_LIBS=$LIBS
        old_withval=$withval

        AC_MSG_CHECKING([for libjpeg (needed by cpdflib 2.x)])
        AC_ARG_WITH(jpeg-dir,
        [  --with-jpeg-dir[=DIR]   jpeg dir for cpdflib 2.x],[
          AC_MSG_RESULT(yes)
          if test -z $withval; then
            withval=/usr/local
          fi
          LIBS="$LIBS -L$withval/lib"
          AC_CHECK_LIB(jpeg,jpeg_read_header, ,[AC_MSG_RESULT(no)],)
          PHP_ADD_LIBRARY_WITH_PATH(jpeg, $withval/lib)
        ],[
          AC_MSG_RESULT(no)
          AC_MSG_WARN(If configure fails try --with-jpeg-dir=<DIR>)
        ]) 

        AC_MSG_CHECKING([for libtiff (needed by cpdflib 2.x)])
        AC_ARG_WITH(tiff-dir,
        [  --with-tiff-dir[=DIR]   tiff dir for cpdflib 2.x],[
          AC_MSG_RESULT(yes)
          if test -z $withval; then
            withval=/usr/local
          fi
          LIBS="$LIBS -L$withval/lib"
          AC_CHECK_LIB(tiff,TIFFOpen, ,[AC_MSG_RESULT(no)],)
          PHP_ADD_LIBRARY_WITH_PATH(tiff, $withval/lib)
        ],[
          AC_MSG_RESULT(no)
          AC_MSG_WARN(If configure fails try --with-tiff-dir=<DIR>)
        ]) 
        withval=$old_withval

          LIBS="$LIBS -L$withval/lib"
        AC_CHECK_LIB(cpdf, cpdf_open, [AC_DEFINE(HAVE_CPDFLIB,1,[Whether you have cpdflib])],
          [AC_MSG_ERROR([Cpdflib module requires cpdflib >= 2.])])
        LIBS=$old_LIBS
        PHP_ADD_LIBRARY_WITH_PATH(cpdf, $withval/lib)
        PHP_ADD_INCLUDE($CPDFLIB_INCLUDE)
      else
        AC_MSG_RESULT(no)
      fi ;;
  esac
],[
  AC_MSG_RESULT(no)
])
