
$(srcdir)/java.c : $(srcdir)/php_java.jar

$(srcdir)/php_java.jar : $(srcdir)/reflect.java
	@$(mkinstalldirs) $(srcdir)/net/php
	@cp $(srcdir)/reflect.java $(srcdir)/net/php
	@echo library=php_java > $(srcdir)/net/php/reflect.properties
	@$(JAVA_C) $(srcdir)/net/php/reflect.java
	@test ! -f reflect.class || mv reflect.class $(srcdir)/net/php # bug in KJC javac
	@$(JAVA_JAR) $(srcdir)/php_java.jar $(srcdir)/net/php/*.class $(srcdir)/net/php/*.properties
	@rm $(srcdir)/net/php/reflect.*
	@rmdir $(srcdir)/net/php
	@rmdir $(srcdir)/net
