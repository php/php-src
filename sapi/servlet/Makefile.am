# $Id$

SRC=servlet.c java.c

CFLAGS=@CFLAGS@ @JAVA_CFLAGS@
INCLUDES=@INCLUDES@ @JAVA_INCLUDE@ -I@top_srcdir@ -I@top_srcdir@/Zend
noinst_LTLIBRARIES=libphpsapi_servlet.la
libphpsapi_servlet_la_SOURCES=$(SRC)

java.c : ../../ext/java/java.c
	@cp ../../ext/java/java.c .

phplib_DATA=phpsrvlt.jar
phpsrvlt.jar : servlet.java ../../ext/java/reflect.java
	@test -e net || mkdir net
	@test -e net/php || mkdir net/php
	@cp servlet.java net/php
	@cp ../../ext/java/reflect.java net/php
	javac net/php/reflect.java
	@echo library=php4>net/php/reflect.properties
	@echo library=php4>net/php/servlet.properties
	@test ! -f reflect.class || mv reflect.class net/php # bug in KJC javac
	javac -classpath .:@SERVLET_CLASSPATH@:@JAVA_CLASSPATH@ net/php/servlet.java
	@test ! -f servlet.class || mv servlet.class net/php # bug in KJC javac
	zip -q0 phpsrvlt.jar net/php/*.class net/php/*.properties
	@rm net/php/servlet.* net/php/reflect.*
	@rmdir net/php
	@rmdir net
