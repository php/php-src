## Process this file with automake to produce Makefile.in

pecldir=$(prefix)/lib/php
pecl_DBdir=$(prefix)/lib/php/DB

install-data-local:
	$(mkinstalldirs) $(pecldir) $(pecl_DBdir)
	$(INSTALL_DATA) $(srcdir)/DB.php $(pecldir)
	$(INSTALL_DATA) $(srcdir)/DB/odbc.php $(pecl_DBdir)
