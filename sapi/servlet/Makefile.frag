
sapi/servlet/java.c : sapi/servlet/../../ext/java/java.c sapi/servlet/phpsrvlt.jar
	@cp sapi/servlet/../../ext/java/java.c sapi/servlet

sapi/servlet/phpsrvlt.jar : sapi/servlet/servlet.java sapi/servlet/../../ext/java/reflect.java
	$(mkinstalldirs) sapi/servlet/net/php
	@echo library=php4 > sapi/servlet/net/php/reflect.properties
	@echo library=php4 > sapi/servlet/net/php/servlet.properties
	@cp sapi/servlet/formatter.java sapi/servlet/net/php
	@cp sapi/servlet/servlet.java sapi/servlet/net/php
	@cp sapi/servlet/../../ext/java/reflect.java sapi/servlet/net/php
	cd sapi/servlet && javac net/php/reflect.java
	@test ! -f sapi/servlet/reflect.class || mv sapi/servlet/reflect.class sapi/servlet/net/php # bug in KJC javac
	cd sapi/servlet && javac -classpath .:$(SERVLET_CLASSPATH):$(CLASSPATH):. net/php/servlet.java
	@test ! -f sapi/servlet/servlet.class || mv sapi/servlet/servlet.class sapi/servlet/net/php # bug in KJC javac
	cd sapi/servlet && javac -classpath .:$(SERVLET_CLASSPATH):$(CLASSPATH):. net/php/formatter.java
	@test ! -f sapi/servlet/formatter.class || mv sapi/servlet/formatter.class sapi/servlet/net/php # bug in KJC javac
	cd sapi/servlet/ && $(JAVA_JAR) phpsrvlt.jar net/php/*.class net/php/*.properties
	@rm -rf sapi/servlet/net
