# $Id$
# config.m4 for extension java

AC_MSG_CHECKING(for Java support)
AC_ARG_WITH(java,
[  --with-java[=DIR]       Include Java support. DIR is the base install
                          directory for the JDK.  This extension can only
                          be built as a shared dl.],
[
  if test "$withval" != "no"; then
    JAVA_SHARED=libphp_java.la

    pltform=`uname -s 2>/dev/null`
    java_libext=libjava.so
    case $pltform in
      AIX) java_libext=libjava.a ;;
      HP-UX) java_libext=libjava.sl ;;
    esac  
    # substitute zip for systems which don't have jar in the PATH
    if JAVA_JAR=`which jar 2>/dev/null`; then
      JAVA_JAR="$JAVA_JAR cf"
    else
      JAVA_JAR='zip -q0'
    fi

    if test "$withval" = "yes"; then
      withval=`cd \`dirname \\\`which javac\\\`\`/..;pwd`
    fi

    if test -d $withval/lib/kaffe; then
      PHP_ADD_LIBPATH($withval/lib)

      JAVA_CFLAGS=-DKAFFE
      JAVA_INCLUDE=-I$withval/include/kaffe
      JAVA_CLASSPATH=$withval/share/kaffe/Klasses.jar
      JAVA_LIB=kaffevm
      JAVA_LIBPATH=$withval/lib/kaffe
      java_libext=kaffevm

      test -f $withval/lib/$JAVA_LIB && JAVA_LIBPATH=$withval/lib
      test -f $withval/lib/kaffe/$JAVA_LIB && JAVA_LIBPATH=$withval/lib/kaffe

      # accomodate old versions of kaffe which don't support jar
      if kaffe -version 2>&1 | grep 1.0b > /dev/null; then
        JAVA_JAR='zip -q0'
      fi

    elif test -f $withval/lib/$java_libext; then
      JAVA_LIB=java
      JAVA_LIBPATH=$withval/lib
      JAVA_INCLUDE=-I$withval/include
      test -f $withval/lib/classes.zip && JAVA_CFLAGS=-DJNI_11
      test -f $withval/lib/jvm.jar     && JAVA_CFLAGS=-DJNI_12
      test -f $withval/lib/classes.zip && JAVA_CLASSPATH=$withval/lib/classes.zip
      test -f $withval/lib/jvm.jar     && JAVA_CLASSPATH=$withval/lib/jvm.jar
      for i in $JAVA_INCLUDE/*; do
        test -f $i/jni_md.h && JAVA_INCLUDE="$JAVA_INCLUDE $i"
      done

    else

      for i in `find $withval/include -type d`; do
        test -f $i/jni.h && JAVA_INCLUDE=-I$i
        test -f $i/jni_md.h && JAVA_INCLUDE="$JAVA_INCLUDE -I$i"
      done

      for i in `find $withval/. -type d`; do
        test -f $i/classes.zip && JAVA_CFLAGS=-DJNI_11
        test -f $i/rt.jar      && JAVA_CFLAGS=-DJNI_12
        test -f $i/classes.zip && JAVA_CLASSPATH=$i/classes.zip
        test -f $i/rt.jar      && JAVA_CLASSPATH=$i/rt.jar

        if test -f $i/$java_libext; then 
          JAVA_LIB=java
          JAVA_LIBPATH=$i
          test -d $i/hotspot && PHP_ADD_LIBPATH($i/hotspot)
          test -d $i/classic && PHP_ADD_LIBPATH($i/classic)
          test -d $i/server && PHP_ADD_LIBPATH($i/server)
          test -d $i/native_threads && PHP_ADD_LIBPATH($i/native_threads)
        fi
      done

      if test -z "$JAVA_INCLUDE"; then
        AC_MSG_RESULT(no)
        AC_MSG_ERROR(unable to find Java VM libraries)
      fi

      JAVA_CFLAGS="$JAVA_CFLAGS -D_REENTRANT"
    fi

    AC_DEFINE(HAVE_JAVA,1,[ ])
    if test -z "$JAVA_LIBPATH"; then
      AC_MSG_ERROR(unable to find Java VM libraries)
    fi
    PHP_ADD_LIBPATH($JAVA_LIBPATH)
    JAVA_CFLAGS="$JAVA_CFLAGS '-DJAVALIB=\"$JAVA_LIBPATH/$java_libext\"'"

    if test "$PHP_SAPI" != "servlet"; then
      PHP_EXTENSION(java, shared)

      if test "$PHP_SAPI" = "cgi"; then
        PHP_ADD_LIBRARY($JAVA_LIB)
      fi

      INSTALL_IT="$INSTALL_IT; \$(srcdir)/build/shtool mkdir -p -f -m 0755 \$(INSTALL_ROOT)\$(libdir)"
      INSTALL_IT="$INSTALL_IT; \$(INSTALL) -m 0755 \$(srcdir)/ext/java/php_java.jar \$(INSTALL_ROOT)\$(libdir)"
    fi

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
