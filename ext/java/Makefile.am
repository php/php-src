# $Id$

SRC=java.c

CFLAGS=@CFLAGS@ @JAVA_CFLAGS@
INCLUDES=@INCLUDES@ @JAVA_INCLUDE@ -I@top_srcdir@ -I@top_srcdir@/Zend
EXTRA_LTLIBRARIES=libphp_java.la
phplib_LTLIBRARIES=@JAVA_SHARED@
libphp_java_la_SOURCES=$(SRC)
libphp_java_la_LIBADD=@JAVA_LFLAGS@
libphp_java_la_LDFLAGS=-avoid-version -module -rpath $(phplibdir)
EXTRA_LIBS=

phplib_DATA=php_java.jar
php_java.jar : reflect.java
	@test -e net || mkdir net
	@test -e net/php || mkdir net/php
	@cp reflect.java net/php
	@echo library=php4>net/php/reflect.properties
	javac net/php/reflect.java
	@test ! -f reflect.class || mv reflect.class net/php # bug in KJC javac
	zip -q0 php_java.jar net/php/*.class net/php/*.properties
	@rm net/php/reflect.*
	@rmdir net/php
	@rmdir net
