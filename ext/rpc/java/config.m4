# $Id$
# config.m4 for extension java

AC_MSG_CHECKING(for Java support)
AC_ARG_WITH(java,
[  --with-java[=DIR]	  Include Java support. DIR is the base install
			  directory for the JDK.  This extension can only
			  be built as a shared dl.],
[
  if test "$withval" != "no"; then
    JAVA_SHARED="libphp_java.la"

    if test "$withval" = "yes"; then
      if test -d /usr/local/lib/kaffe; then
	JAVA_CFLAGS="-DKAFFE"
	JAVA_INCLUDE=-I/usr/local/include/kaffe
      elif test -d /usr/lib/kaffe; then
	JAVA_CFLAGS="-DKAFFE"
	JAVA_INCLUDE=-I/usr/include/kaffe
      else
	AC_MSG_RESULT(no)
	AC_MSG_ERROR(unable to find Java VM libraries)
      fi
    else
      if test -f $withval/lib/libjava.so; then
	JAVA_INCLUDE="-I$withval/include"
	test -f $withval/lib/classes.zip && JAVA_CFLAGS="-DJNI_11"
	test -f $withval/lib/jvm.jar	 && JAVA_CFLAGS="-DJNI_12"
	for i in $JAVA_INCLUDE/*; do
	  test -f $i/jni_md.h	 && JAVA_INCLUDE="$JAVA_INCLUDE $i"
	done
      else
	for i in `find $withval -type d`; do
	  test -f $i/jni.h	 && JAVA_INCLUDE="-I$i"
	  test -f $i/jni_md.h	 && JAVA_INCLUDE="$JAVA_INCLUDE -I$i"
	  test -f $i/classes.zip && JAVA_CFLAGS="-DJNI_11"
	  test -f $i/jvm.jar	 && JAVA_CFLAGS="-DJNI_12"
	done
	if test -z "$JAVA_INCLUDE"; then
	  AC_MSG_RESULT(no)
	  AC_MSG_ERROR(unable to find Java VM libraries)
	fi
      fi
    fi

    AC_DEFINE(HAVE_JAVA)
    PHP_EXTENSION(java, "shared")
    PHP_BUILD_SHARED
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])

AC_SUBST(JAVA_CFLAGS)
AC_SUBST(JAVA_INCLUDE)
AC_SUBST(JAVA_SHARED)
