
pear_install_targets = \
	install-data-local \
	install-headers \
	install-build \
	install-programs

peardir=$(PEAR_INSTALLDIR)

PEAR_SUBDIRS = \
	Archive \
	Console \
	Crypt \
	Date \
	DB \
	File \
	HTML \
	HTTP \
	Image \
	Mail \
	Net \
	PEAR \
	PEAR/Command \
	PEAR/Frontend \
	Schedule \
	XML 

PEAR_FILES = \
	Archive/Tar.php \
	Console/Getopt.php \
	Crypt/CBC.php \
	Crypt/HCEMD5.php \
	Date/Calc.php \
	Date/Human.php \
	DB.php \
	DB/common.php \
	DB/fbsql.php \
	DB/ibase.php \
	DB/ifx.php \
	DB/msql.php \
	DB/mssql.php \
	DB/mysql.php \
	DB/oci8.php \
	DB/odbc.php \
	DB/pgsql.php \
	DB/storage.php \
	DB/sybase.php \
	File/Find.php \
	File/Passwd.php \
	File/SearchReplace.php \
	HTML/Common.php \
	HTML/Form.php \
	HTML/IT.php \
	HTML/ITX.php \
	HTML/IT_Error.php \
	HTML/Page.php \
	HTML/Processor.php \
	HTML/Select.php \
	HTML/Table.php \
	HTTP.php \
	HTTP/Compress.php \
	Mail.php \
	Mail/RFC822.php \
	Mail/sendmail.php \
	Mail/smtp.php \
	Net/Curl.php \
	Net/Dig.php \
	Net/SMTP.php \
	Net/Socket.php \
	PEAR.php \
	PEAR/Autoloader.php \
	PEAR/Command.php \
	PEAR/Command/Auth.php \
	PEAR/Command/Common.php \
	PEAR/Command/Config.php \
	PEAR/Command/Install.php \
	PEAR/Command/Package.php \
	PEAR/Command/Registry.php \
	PEAR/CommandResponse.php \
	PEAR/Frontend/CLI.php \
	PEAR/Common.php \
	PEAR/Config.php \
	PEAR/Dependency.php \
	PEAR/Installer.php \
	PEAR/Packager.php \
	PEAR/Registry.php \
	PEAR/Remote.php \
	PEAR/Uploader.php \
	Schedule/At.php \
	System.php \
	XML/Parser.php 

install-pear:
	@if $(mkinstalldirs) $(INSTALL_ROOT)$(peardir); then \
		for i in $(PEAR_SUBDIRS); do \
			$(mkinstalldirs) $(INSTALL_ROOT)$(peardir)/$$i; \
		done; \
		for i in $(PEAR_FILES); do \
			echo "Installing $$i"; \
			dir=`echo $$i|sed 's%[^/][^/]*$$%%'`; \
			$(INSTALL_DATA) $(srcdir)/$$i $(INSTALL_ROOT)$(peardir)/$$dir; \
		done; \
	else \
		cat $(srcdir)/install-pear.txt; \
		exit 5; \
	fi

phpincludedir = $(includedir)/php
phpbuilddir = $(prefix)/lib/php/build

BUILD_FILES = \
	pear/pear.m4 \
	build/mkdep.awk \
	build/shtool \
	Makefile.global \
	scan_makefile_in.awk \
	acinclude.m4

bin_SCRIPTS = phpize php-config pear pearize phptar

install-build:
	@echo "Installing build environment"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(phpbuilddir) $(INSTALL_ROOT)$(bindir) && \
	(cd $(top_srcdir) && cp $(BUILD_FILES) $(INSTALL_ROOT)$(phpbuilddir))

install-programs:
	@for prog in $(bin_SCRIPTS); do \
		echo "Installing program: $$prog"; \
		$(INSTALL) -m 755 $(builddir)/scripts/$$prog $(INSTALL_ROOT)$(bindir)/$$prog; \
	done; \
	for prog in phpextdist; do \
		echo "Installing program: $$prog"; \
		$(INSTALL) -m 755 $(srcdir)/scripts/$$prog $(INSTALL_ROOT)$(bindir)/$$prog; \
	done

HEADER_DIRS = \
	/ \
	Zend \
	TSRM \
	ext/standard \
	ext/session \
	ext/xml \
	ext/xml/expat/xmlparse \
	ext/xml/expat/xmltok \
	main \
	regex

install-headers:
	-@for i in $(HEADER_DIRS); do \
		paths="$$paths $(INSTALL_ROOT)$(phpincludedir)/$$i"; \
	done; \
	$(mkinstalldirs) $$paths && \
	echo "Installing header files" && \
	for i in $(HEADER_DIRS); do \
		(cd $(top_srcdir)/$$i && cp -p *.h $(INSTALL_ROOT)$(phpincludedir)/$$i; \
		cd $(top_builddir)/$$i && cp -p *.h $(INSTALL_ROOT)$(phpincludedir)/$$i) 2>/dev/null || true; \
	done

$(builddir)/scripts/pear: $(srcdir)/scripts/pear.in $(top_builddir)/config.status
	(CONFIG_FILES=$@ CONFIG_HEADERS= $(top_builddir)/config.status)

$(builddir)/scripts/phpize: $(srcdir)/scripts/phpize.in $(top_builddir)/config.status
	(CONFIG_FILES=$@ CONFIG_HEADERS= $(top_builddir)/config.status)

$(builddir)/scripts/phptar: $(srcdir)/scripts/phptar.in $(top_builddir)/config.status
	(CONFIG_FILES=$@ CONFIG_HEADERS= $(top_builddir)/config.status)

$(builddir)/scripts/pearize: $(srcdir)/scripts/pearize.in $(top_builddir)/config.status
	(CONFIG_FILES=$@ CONFIG_HEADERS= $(top_builddir)/config.status)

$(builddir)/scripts/php-config: $(srcdir)/scripts/php-config.in $(top_builddir)/config.status
	(CONFIG_FILES=$@ CONFIG_HEADERS= $(top_builddir)/config.status)
