dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(yp,whether to include YP support,
[  --enable-yp             Include YP support.])

if test "$PHP_YP" != "no"; then

  AC_CHECK_LIB(nsl, yp_match, [
	YP_LIBS=nsl
	YP_CHECK_IN_LIB=nsl
	],
	AC_CHECK_LIB(c, yp_match, [
		YP_LIBS=
		YP_CHECK_IN_LIB=c
	],[
		AC_MSG_ERROR(Unable to find required yp/nis library)
	])
  )


  AC_DEFINE(HAVE_YP,1,[ ])
  PHP_NEW_EXTENSION(yp, yp.c, $ext_shared)

  PHP_SUBST(YP_SHARED_LIBADD)

  if test -n "$YP_LIBS"; then
    PHP_ADD_LIBRARY_WITH_PATH($YP_LIBS, $YP_LIBDIR, YP_SHARED_LIBADD)
  fi



  case $host_alias in
  *solaris*)
    AC_DEFINE(SOLARIS_YP,1,[ ]) ;;
  esac
fi
