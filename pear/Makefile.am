## Process this file with automake to produce Makefile.in

peardir=$(prefix)/lib/php
pear_DBdir=$(prefix)/lib/php/DB

install-data-local:
	-$(mkinstalldirs) $(peardir) $(pear_DBdir) && \
	$(INSTALL_DATA) $(srcdir)/DB.php $(peardir) && \
	$(INSTALL_DATA) $(srcdir)/DB/common.php $(pear_DBdir) && \
	$(INSTALL_DATA) $(srcdir)/DB/odbc.php $(pear_DBdir) && \
	$(INSTALL_DATA) $(srcdir)/DB/mysql.php $(pear_DBdir)
