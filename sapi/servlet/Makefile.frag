
$(builddir)/java.c : $(srcdir)/../../ext/java/java.c
	@cp $(srcdir)/../../ext/java/java.c $(builddir)

$(builddir)/phpsrvlt.jar : $(srcdir)/servlet.java $(srcdir)/../../ext/java/reflect.java
	$(mkinstalldirs) $(builddir)/net/php
	@echo library=php4 > $(builddir)/net/php/reflect.properties
	@echo library=php4 > $(builddir)/net/php/servlet.properties
	@cp $(builddir)/formatter.java $(builddir)/net/php
	@cp $(builddir)/servlet.java $(builddir)/net/php
	@cp $(srcdir)/../../ext/java/reflect.java $(builddir)/net/php
	cd $(builddir) && javac net/php/reflect.java
	@test ! -f $(builddir)/reflect.class || mv $(builddir)/reflect.class $(builddir)/net/php # bug in KJC javac
	javac -classpath .:$(SERVLET_CLASSPATH):$(CLASSPATH) $(builddir)/net/php/servlet.java
	@test ! -f $(builddir)/servlet.class || mv $(builddir)/servlet.class $(builddir)/net/php # bug in KJC javac
	javac -classpath .:$(SERVLET_CLASSPATH):$(CLASSPATH) $(builddir)/net/php/formatter.java
	@test ! -f $(builddir)/formatter.class || mv $(builddir)/formatter.class $(builddir)/net/php # bug in KJC javac
	cd $(builddir)/ && $(JAVA_JAR) phpsrvlt.jar net/php/*.class net/php/*.properties
	@rm -rf $(builddir)/net
