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

    # substitute zip for systems which don't have jar in the PATH
    if JAVA_JAR=`which jar 2>/dev/null`; then
      JAVA_JAR="$JAVA_JAR cf"
    else
      JAVA_JAR='zip -q0'
    fi

    if test "$withval" = "yes"; then
      if test -d /usr/local/lib/kaffe; then
	JAVA_CFLAGS="-DKAFFE"
	JAVA_INCLUDE=-I/usr/local/include/kaffe
	JAVA_CLASSPATH=/usr/local/share/kaffe/Klasses.jar
      elif test -d /usr/lib/kaffe; then
	JAVA_CFLAGS="-DKAFFE"
	JAVA_INCLUDE=-I/usr/include/kaffe
	JAVA_CLASSPATH=/usr/share/kaffe/Klasses.jar
      else
	AC_MSG_RESULT(no)
	AC_MSG_ERROR(unable to find Java VM libraries)
      fi

      # accomodate old versions of kaffe which don't support jar
      if kaffe -version 2>&1 | grep 1.0b > /dev/null; then
        JAVA_JAR='zip -q0'
      fi

    else
      if test -f $withval/lib/libjava.so; then
	JAVA_INCLUDE="-I$withval/include"
	test -f $withval/lib/classes.zip && JAVA_CFLAGS="-DJNI_11"
	test -f $withval/lib/jvm.jar	 && JAVA_CFLAGS="-DJNI_12"
	test -f $withval/lib/classes.zip && JAVA_CLASSPATH="$withval/lib/classes.zip"
	test -f $withval/lib/jvm.jar	 && JAVA_CLASSPATH="$withval/lib/jvm.jar"
	for i in $JAVA_INCLUDE/*; do
	  test -f $i/jni_md.h	 && JAVA_INCLUDE="$JAVA_INCLUDE $i"
	done
      else
	for i in `find $withval -type d`; do
	  test -f $i/jni.h	 && JAVA_INCLUDE="-I$i"
	  test -f $i/jni_md.h	 && JAVA_INCLUDE="$JAVA_INCLUDE -I$i"
	  test -f $i/classes.zip && JAVA_CFLAGS="-DJNI_11"
	  test -f $i/jvm.jar	 && JAVA_CFLAGS="-DJNI_12"
	  test -f $i/classes.zip && JAVA_CLASSPATH="$i/classes.zip"
	  test -f $i/jvm.jar	 && JAVA_CLASSPATH="$i/jvm.jar"
	done
	if test -z "$JAVA_INCLUDE"; then
	  AC_MSG_RESULT(no)
	  AC_MSG_ERROR(unable to find Java VM libraries)
	fi
      fi
    fi

    AC_DEFINE(HAVE_JAVA,1,[ ])
    PHP_EXTENSION(java, shared)
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])

PHP_SUBST(JAVA_CFLAGS)
PHP_SUBST(JAVA_CLASSPATH)
PHP_SUBST(JAVA_INCLUDE)
PHP_SUBST(JAVA_SHARED)
PHP_SUBST(JAVA_JAR)

