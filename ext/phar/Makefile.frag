
$(srcdir)/phar_path_check.c: $(srcdir)/phar_path_check.re
	@(cd $(top_srcdir); $(RE2C) -b -o ext/phar/phar_path_check.c ext/phar/phar_path_check.re)
