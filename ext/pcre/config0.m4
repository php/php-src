dnl $Id$
dnl config.m4 for extension pcre

dnl By default we'll compile and link against the bundled PCRE library
dnl if DIR is supplied, we'll use that for linking

AC_MSG_CHECKING(whether to include PCRE support)
AC_ARG_WITH(pcre-regex,
[  --without-pcre-regex    Do not include Perl Compatible Regular Expressions 
                          support. Use --with-pcre-regex=DIR to specify DIR
                          where PCRE's include and library files are located,
                          if not using bundled library.],
[
	case "$withval" in 
		no)
			AC_MSG_RESULT(no)
			;;
		yes)
			EXTRA_LIBS="-L$abs_builddir/ext/pcre/pcrelib -lpcre $EXTRA_LIBS"
			PCRE_SUBDIR="pcrelib"
			AC_DEFINE(HAVE_BUNDLED_PCRE, 1)
			AC_MSG_RESULT(yes)
			PHP_EXTENSION(pcre)
			PHP_OUTPUT(ext/pcre/pcrelib/Makefile)
			;;
		*)
			if test -f $withval/pcre.h ; then
				changequote({,})
				pcre_major=`grep PCRE_MAJOR $withval/pcre.h | sed -e 's/[^0-9]//g'`
				pcre_minor=`grep PCRE_MINOR $withval/pcre.h | sed -e 's/[^0-9]//g'`
				changequote([,])
				pcre_version=$pcre_major$pcre_minor
				if test "$pcre_version" -ge 208; then
					AC_ADD_INCLUDE($withval)
				else
					AC_MSG_ERROR(PCRE extension requires PCRE library version >= 2.08)
				fi
			else
				AC_MSG_ERROR(Could not find pcre.h in $withval)
			fi

			if test -f $withval/libpcre.a ; then
				AC_ADD_LIBRARY_WITH_PATH(pcre, $withval)
			else
				AC_MSG_ERROR(Could not find libpcre.a in $withval)
			fi

			AC_DEFINE(HAVE_PCRE, 1)
			AC_MSG_RESULT(yes)
			PHP_EXTENSION(pcre)
			;;
	esac
],[
  PCRE_LIBADD=pcrelib/libpcre.la
  PCRE_SUBDIRS=pcrelib
  AC_DEFINE(HAVE_BUNDLED_PCRE, 1)
  AC_MSG_RESULT(yes)
  PHP_EXTENSION(pcre)
  PHP_OUTPUT(ext/pcre/pcrelib/Makefile)
]) 
AC_SUBST(PCRE_LIBADD)
AC_SUBST(PCRE_SUBDIRS)

AC_CHECK_FUNC(memmove, [], [AC_DEFINE(USE_BCOPY, 1)])
