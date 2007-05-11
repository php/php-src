
$(srcdir)/phar_path_check.c: $(srcdir)/phar_path_check.re
	$(RE2C) -b -o $(srcdir)/phar_path_check.c $(srcdir)/phar_path_check.re)

$(builddir)/phar.phar: $(srcdir)/phar/*.inc $(srcdir)/phar/*.php
	php -d phar.readonly=0 $(srcdir)/phar.php pack -f $(builddir)/phar.phar -a pharcommand -c gz -x CVS -s $(srcdir)/phar/phar.php $(srcdir)/phar/

