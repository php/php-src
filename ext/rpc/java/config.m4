dnl
dnl $Id$
dnl
AC_DEFUN(JAVA_FIND_JAR, [
 AC_MSG_CHECKING([Java Jar location])
  if test "$PHP_JAVA" = "yes"; then
    if JAVA_JAR=`which jar 2>/dev/null`; then
      JAVA_JAR="$JAVA_JAR cf"
    else
      JAVA_JAR=
    fi

    dnl
    dnl substitue zip for systems which don't have jar
    dnl
    if test -z "$JAVA_JAR"; then
      JAVA_JAR='zip -q0'
    fi
  else
  dnl
  dnl we have a custom path defined so use it
  dnl
    if test -d $PHP_JAVA/bin; then
      JAVA_JAR="$PHP_JAVA/bin/javac cf"
    else
      AC_MSG_ERROR([Unable to locate $PHP_JAVA/bin])
    fi
  fi
  PHP_SUBST(JAVA_JAR)
  AC_MSG_RESULT([$JAVA_JAR])
])

AC_DEFUN(JAVA_FIND_C, [
 AC_MSG_CHECKING([Java C location])
  JAVA_C=`which javac`
  if test -z "$JAVA_C"; then
  dnl
  dnl we will auto assume that the bin directory is underneath
  dnl the user provided directory, and it includes the javac
  dnl directory.  
  dnl
    if test -z "$PHP_JAVA" != "yes"; then
     if test -z $PHP_JAVA/bin; then
      JAVA_C=$PHP_JAVA/bin/javac
     else
      AC_MSG_ERROR([Unable to locate $PHP_JAVA/bin])
     fi
    fi
  fi
  if test -z "$JAVA_C"; then
    AC_MSG_ERROR([Unable to locate the javac binary in your system path
Either adjust your Java installation or provide the Java installation path,
e.g. --with-java=/java expecting /java/bin/ to contain the binaries])
  fi

  PHP_SUBST(JAVA_C)
  AC_MSG_RESULT([$JAVA_C])
])

AC_DEFUN(JAVA_CHECK_LIB, [
 AC_MSG_CHECKING([Checking for libjava])
 if test -d $PHP_JAVA/lib/kaffe; then
   PHP_ADD_LIBPATH($PHP_JAVA/lib)
   JAVA_CFLAGS=-DKAFFE
   JAVA_INCLUDE=-I$PHP_JAVA/include/kaffe
   JAVA_CLASSPATH=$PHP_JAVA/share/kaffe/Klasses.jar
   JAVA_LIB=kaffevm
   JAVA_LIBPATH=$PHP_JAVA/lib/kaffe
   java_libext=kaffevm

   test -f $PHP_JAVA/lib/$JAVA_LIB       && JAVA_LIBPATH=$PHP_JAVA/lib
   test -f $PHP_JAVA/lib/kaffe/$JAVA_LIB && JAVA_LIBPATH=$PHP_JAVA/lib/kaffe

   dnl
   dnl accomodate old versions of kaffe which don't support jar
   dnl
   if kaffe -version 2>&1 | grep 1.0b > /dev/null; then
     JAVA_JAR='zip -q0'
   fi
 elif test -f $PHP_JAVA/lib/$java_libext; then
   JAVA_LIB=java
   JAVA_LIBPATH=$PHP_JAVA/lib
   JAVA_INCLUDE=-I$PHP_JAVA/include

   test -f $PHP_JAVA/lib/classes.zip && JAVA_CFLAGS=-DJNI_11
   test -f $PHP_JAVA/lib/jvm.jar     && JAVA_CFLAGS=-DJNI_12
   test -f $PHP_JAVA/lib/classes.zip && JAVA_CLASSPATH=$PHP_JAVA/lib/classes.zip
   test -f $PHP_JAVA/lib/jvm.jar     && JAVA_CLASSPATH=$PHP_JAVA/lib/jvm.jar

   for i in $PHP_JAVA/include/*; do
     test -f $i/jni_md.h && JAVA_INCLUDE="$JAVA_INCLUDE $i"
   done
dnl
dnl sample JDK v 1.4 path
dnl /usr/java/j2sdk1.4.0_01/jre/lib/i386/libjava.so
dnl
 else
   dnl
   dnl We have to find everything
   dnl
   for i in `find $PHP_JAVA/include -type d`; do
     test -f $i/jni.h      && JAVA_INCLUDE=-I$i
     test -f $i/jni_md.h   && JAVA_INCLUDE="$JAVA_INCLUDE -I$i"
   done

   for i in `find $PHP_JAVA/. -type d`; do
     test -f $i/classes.zip && JAVA_CFLAGS=-DJNI_11
     test -f $i/rt.jar      && JAVA_CFLAGS=-DJNI_12
     test -f $i/classes.zip && JAVA_CLASSPATH=$i/classes.zip
     test -f $i/rt.jar      && JAVA_CLASSPATH=$i/rt.jar

     if test -f $i/$java_libext; then
       JAVA_LIB=java
       JAVA_LIBPATH=$i

       test -d $i/hotspot && PHP_ADD_LIBPATH($i/hotspot)
       test -d $i/classic && PHP_ADD_LIBPATH($i/classic)
       test -d $i/server  && PHP_ADD_LIBPATH($i/server)
       test -d $i/native_threads && PHP_ADD_LIBPATH($i/native_threads)
     fi
   done

   if test -z "$JAVA_INCLUDE"; then
     AC_MSG_RESULT(no)
     AC_MSG_ERROR(unable to find Java VM libraries)
   fi

   JAVA_CFLAGS="$JAVA_CFLAGS -D_REENTRANT"
 fi
])


PHP_ARG_WITH(java, for Java support,
[  --with-java[=DIR]       Include Java support. DIR is the JDK base install directory. 
                          This extension is always built as shared.])

if test "$PHP_JAVA" != "no"; then
  platform=`uname -s 2>/dev/null`
  java_libext=libjava.so
  case $platform in
    AIX) java_libext=libjava.a ;;
    HP-UX) java_libext=libjava.sl ;;
    Darwin) java_libext=libjava.jnilib ;;
  esac  
  JAVA_FIND_JAR()
  JAVA_FIND_C()

  if test "$platform" = "Darwin"; then
    AC_CHECK_HEADERS([JavaVM/JavaVM.h])
    AC_CHECK_HEADERS([JavaVM/jni.h])
    JAVA_CLASSPATH=/System/Library/Frameworks/JavaVM.framework/Classes/classes.jar
    PHP_CHECK_FRAMEWORK("JavaVM", JNI_CreateJavaVM,[AC_DEFINE(HAVE_JAVA,1,[ ])])
    PHP_ADD_FRAMEWORK("JavaVM")
  else
    JAVA_CHECK_LIB()
    AC_DEFINE(HAVE_JAVA,1,[ ])

    if test -z "$JAVA_LIBPATH"; then
      AC_MSG_ERROR(unable to find Java VM libraries)
    fi

    PHP_ADD_LIBPATH($JAVA_LIBPATH)
    JAVA_CFLAGS="$JAVA_CFLAGS '-DJAVALIB=\"$JAVA_LIBPATH/$java_libext\"'"
  fi

  if test "$PHP_SAPI" != "servlet"; then
    PHP_NEW_EXTENSION(java, java.c, shared,, $JAVA_CFLAGS $JAVA_INCLUDE)

    if test "$PHP_SAPI" = "cgi"; then
      if test "$platform" != "Darwin"; then
        PHP_ADD_LIBRARY($JAVA_LIB)
      fi
    fi

    INSTALL_IT="$INSTALL_IT; \$(srcdir)/build/shtool mkdir -p -f -m 0755 \$(INSTALL_ROOT)\$(libdir)"
    INSTALL_IT="$INSTALL_IT; \$(INSTALL) -m 0755 \$(srcdir)/ext/java/php_java.jar \$(INSTALL_ROOT)\$(libdir)"
  fi

  PHP_SUBST(JAVA_CLASSPATH)

  PHP_ADD_MAKEFILE_FRAGMENT
fi

