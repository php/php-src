AC_MSG_CHECKING(whether to include GD support)
AC_ARG_WITH(gd,
[  --without-gd            Disable GD support.
  --with-gd[=DIR]         Include GD support (DIR is GD's install dir).],
[
  case "$withval" in
    no)
      AC_MSG_RESULT(no) ;;
    yes)
      GD_LIBS="-lgd"
      AC_DEFINE(HAVE_LIBGD)
      AC_MSG_RESULT(yes)
      AC_CHECK_LIB(gd, gdImageString16, [ AC_DEFINE(HAVE_LIBGD13) ])
      ac_cv_lib_gd_gdImageLine=yes
      ;;
    *)
dnl A whole whack of possible places where this might be
      test -f $withval/include/gd1.3/gd.h && GD_INCLUDE="-I$withval/include/gd1.3"
      test -f $withval/include/gd/gd.h && GD_INCLUDE="-I$withval/include/gd"
      test -f $withval/include/gd.h && GD_INCLUDE="-I$withval/include"
      test -f $withval/gd1.3/gd.h && GD_INCLUDE="-I$withval/gd1.3"
      test -f $withval/gd/gd.h && GD_INCLUDE="-I$withval/gd"
      test -f $withval/gd.h && GD_INCLUDE="-I$withval"

      test -f $withval/lib/libgd.a && GD_LIB="$withval/lib"
      test -f $withval/lib/gd/libgd.a && GD_LIB="$withval/lib/gd"
      test -f $withval/lib/gd1.3/libgd.a && GD_LIB="$withval/lib/gd1.3"
      test -f $withval/libgd.a && GD_LIB="$withval"
      test -f $withval/gd/libgd.a && GD_LIB="$withval/gd"
      test -f $withval/gd1.3/libgd.a && GD_LIB="$withval/gd1.3"

      if test -n "$GD_INCLUDE" && test -n "$GD_LIB" ; then
        GD_LIBS="-L$GD_LIB -lgd"
        AC_DEFINE(HAVE_LIBGD)
        AC_MSG_RESULT(yes)
        AC_CHECK_LIB(gd, gdImageString16, [ AC_DEFINE(HAVE_LIBGD13) ])
	ac_cv_lib_gd_gdImageLine=yes
      else
        AC_MSG_ERROR(Unable to find libgd.a anywhere under $withval)
      fi ;;
  esac
],[
  AC_CHECK_LIB(gd, gdImageLine)
  AC_CHECK_LIB(gd, gdImageString16, [ AC_DEFINE(HAVE_LIBGD13) ])
])
if test "$ac_cv_lib_gd_gdImageLine" = "yes"; then
  if test -f /usr/pkg/include/gd/gd.h -a -z "$GD_INCLUDE" ; then
    GD_INCLUDE="-I/usr/pkg/include/gd"
  fi
  AC_CHECK_LIB(ttf, TT_Open_Face)
  AC_CHECK_HEADERS(freetype.h)
  PHP_EXTENSION(gd)
  EXTRA_LIBS="$EXTRA_LIBS $GD_LIBS"
  INCLUDES="$INCLUDES $GD_INCLUDE"
fi
