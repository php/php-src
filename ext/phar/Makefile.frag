
$(srcdir)/phar_path_check.c: $(srcdir)/phar_path_check.re
	$(RE2C) -b -o $(srcdir)/phar_path_check.c $(srcdir)/phar_path_check.re)

pharcmd: $(builddir)/phar.php $(builddir)/phar.phar

$(builddir)/phar.php: $(srcdir)/build_precommand.php $(srcdir)/phar/*.inc $(srcdir)/phar/*.php
	$(PHP_EXECUTABLE) $(srcdir)/build_precommand.php > $(builddir)/phar.php

$(builddir)/phar.phar: $(builddir)/phar.php $(srcdir)/phar/*.inc $(srcdir)/phar/*.php
	$(PHP_EXECUTABLE) -d phar.readonly=0 $(srcdir)/phar.php pack -f $(builddir)/phar.phar -a pharcommand -c auto -x CVS -p 0 -s $(srcdir)/phar/phar.php -h sha1 $(srcdir)/phar/
	@chmod +x $(builddir)/phar.phar

