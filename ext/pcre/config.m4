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
			AC_DEFINE(HAVE_BUNDLED_PCRE, 1, [ ])
			AC_MSG_RESULT(yes)
			PHP_EXTENSION(pcre)
			PHP_FAST_OUTPUT(ext/pcre/pcrelib/Makefile)
			;;
		*)
			test -f $withval/pcre.h && PCRE_INCDIR="$withval"
			test -f $withval/include/pcre.h && PCRE_INCDIR="$withval/include"

			if test -n "$PCRE_INCDIR" ; then
				changequote({,})
				pcre_major=`grep PCRE_MAJOR $PCRE_INCDIR/pcre.h | sed -e 's/[^0-9]//g'`
				pcre_minor=`grep PCRE_MINOR $PCRE_INCDIR/pcre.h | sed -e 's/[^0-9]//g'`
				changequote([,])
				pcre_minor_length=`echo "$pcre_minor" | wc -c | sed -e 's/[^0-9]//g'`
				if test "$pcre_minor_length" -eq 2 ; then
					pcre_minor="$pcre_minor"0
				fi
				pcre_version=$pcre_major$pcre_minor
				if test "$pcre_version" -ge 208; then
					AC_ADD_INCLUDE($PCRE_INCDIR)
				else
					AC_MSG_ERROR(PCRE extension requires PCRE library version >= 2.08)
				fi
			else
				AC_MSG_ERROR(Could not find pcre.h in $withval)
			fi

			test -f $withval/libpcre.a && PCRE_LIBDIR="$withval"
			test -f $withval/lib/libpcre.a && PCRE_LIBDIR="$withval/lib"
			if test -n "$PCRE_LIBDIR" ; then
				AC_ADD_LIBRARY_WITH_PATH(pcre, $PCRE_LIBDIR)
			else
				AC_MSG_ERROR(Could not find libpcre.a in $withval)
			fi

			AC_DEFINE(HAVE_PCRE, 1, [ ])
			AC_MSG_RESULT(yes)
			PHP_EXTENSION(pcre)
			;;
	esac
],[
  PCRE_LIBADD=pcrelib/libpcre.la
  PCRE_SUBDIRS=pcrelib
  AC_DEFINE(HAVE_BUNDLED_PCRE, 1, [ ])
  AC_MSG_RESULT(yes)
  PHP_EXTENSION(pcre)
  PHP_FAST_OUTPUT(ext/pcre/pcrelib/Makefile)
]) 
PHP_SUBST(PCRE_LIBADD)
PHP_SUBST(PCRE_SUBDIRS)

AC_CHECK_FUNC(memmove, [], [AC_DEFINE(USE_BCOPY, 1, [ ])])
