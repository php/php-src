dnl $Id$

AC_MSG_CHECKING(whether to include pdflib support)
AC_ARG_WITH(pdflib,
[  --with-pdflib[=DIR]     Include pdflib support (tested with 0.6).
                          DIR is the pdflib install directory,
                          defaults to /usr/local.],
[
  case "$withval" in
    no)
      AC_MSG_RESULT(no) ;;
    yes)
      AC_MSG_RESULT(yes)
      PHP_EXTENSION(pdf)
      AC_CHECK_LIB(pdf, PDF_open, [AC_DEFINE(HAVE_PDFLIB) PDFLIB_LIBS="-lpdf"],
        [AC_MSG_ERROR(pdflib extension requires pdflib 0.6.)])
      EXTRA_LIBS="$EXTRA_LIBS $PDFLIB_LIBS"
      ;;
    *)
      test -f $withval/include/pdf.h && PDFLIB_INCLUDE="-I$withval/include"
      if test -n "$PDFLIB_INCLUDE" ; then
        AC_MSG_RESULT(yes)
        PHP_EXTENSION(pdf)
        old_LIBS=$LIBS
        LIBS="$LIBS -L$withval/lib"
        AC_CHECK_LIB(pdf, PDF_open, [AC_DEFINE(HAVE_PDFLIB) PDFLIB_LIBS="-L$withval/lib -lpdf"],
          [AC_MSG_ERROR(pdflib extension requires pdflib 0.6.)])
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
