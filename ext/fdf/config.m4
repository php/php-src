dnl $Id$

AC_MSG_CHECKING(whether to include fdftk support)
AC_ARG_WITH(fdftk,
[  --with-fdftk[=DIR]      Include fdftk support.
                          DIR is the fdftk install directory,
                          defaults to /usr/local.],
[
  case "$withval" in
    no)
      AC_MSG_RESULT(no) ;;
    yes)
      AC_MSG_RESULT(yes)
      PHP_EXTENSION(fdf)
      AC_CHECK_LIB(FdfTk, FDFOpen, [
        AC_DEFINE(HAVE_FDFLIB)
        EXTRA_LIBS="$EXTRA_LIBS -lFdfTk"
      ],[AC_MSG_ERROR(fdftk module requires fdftk 2.0)])
      ;;
    *)
      test -f $withval/include/FdfTk.h && FDFLIB_INCLUDE="-I$withval/include"
      if test -n "$FDFLIB_INCLUDE" ; then
        AC_MSG_RESULT(yes)
        PHP_EXTENSION(fdf)
        old_LIBS=$LIBS
        LIBS="$LIBS -L$withval/lib"
        AC_CHECK_LIB(FdfTk, FDFOpen, [
          AC_DEFINE(HAVE_FDFLIB)
          EXTRA_LIBS="$EXTRA_LIBS -L$withval/lib -lFdfTk"
        ],[AC_MSG_ERROR(fdftk module requires ftftk lib 2.0.)])
        LIBS=$old_LIBS
        INCLUDES="$INCLUDES $FDFLIB_INCLUDE"
      else
        AC_MSG_RESULT(no)
      fi ;;
  esac
],[
  AC_MSG_RESULT(no)
])
