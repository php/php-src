# -*- makefile -*-

pear_install_targets = \
	install-pear \
	install-headers \
	install-build \
	install-programs

peardir=$(PEAR_INSTALLDIR)

#PEAR_SUBDIRS = \
#	 Archive \
#	 Console \
#	 PEAR \
#	 PEAR/Command \
#	 PEAR/Frontend \
#	 XML 

# These are moving to /pear (in cvs):
# 	Crypt \
# 	File \
# 	Date \
# 	DB \
# 	HTML \
# 	HTTP \
# 	Image \
# 	Mail \
# 	Net \
#	Schedule \

#PEAR_FILES = \
#	 Archive/Tar.php \
#	 Console/Getopt.php \
#	 PEAR.php \
#	 PEAR/Autoloader.php \
#	 PEAR/Command.php \
#	 PEAR/Command/Auth.php \
#	 PEAR/Command/Common.php \
#	 PEAR/Command/Config.php \
#	 PEAR/Command/Install.php \
#	 PEAR/Command/Package.php \
#	 PEAR/Command/Registry.php \
#	 PEAR/Command/Remote.php \
#	 PEAR/Frontend/CLI.php \
#	 PEAR/Frontend/Gtk.php \
#	 PEAR/Common.php \
#	 PEAR/Config.php \
#	 PEAR/Dependency.php \
#	 PEAR/Installer.php \
#	 PEAR/Packager.php \
#	 PEAR/Registry.php \
#	 PEAR/Remote.php \
#	 System.php \
#	 XML/Parser.php 

# These are moving to /pear (in cvs):
# 	Crypt/CBC.php \
# 	Crypt/HCEMD5.php \
# 	DB.php \
# 	DB/common.php \
# 	DB/fbsql.php \
# 	DB/ibase.php \
# 	DB/ifx.php \
# 	DB/msql.php \
# 	DB/mssql.php \
# 	DB/mysql.php \
# 	DB/oci8.php \
# 	DB/odbc.php \
# 	DB/pgsql.php \
# 	DB/storage.php \
# 	DB/sybase.php \
# 	Date/Calc.php \
# 	Date/Human.php \
# 	File/Find.php \
# 	File/Passwd.php \
# 	File/SearchReplace.php \
# 	HTML/Common.php \
# 	HTML/Form.php \
# 	HTML/IT.php \
# 	HTML/ITX.php \
# 	HTML/IT_Error.php \
# 	HTML/Page.php \
# 	HTML/Processor.php \
# 	HTML/Select.php \
# 	HTML/Table.php \
# 	HTTP.php \
# 	HTTP/Compress.php \
# 	Mail.php \
# 	Mail/RFC822.php \
# 	Mail/sendmail.php \
# 	Mail/smtp.php \
# 	Net/Curl.php \
# 	Net/Dig.php \
# 	Net/SMTP.php \
#	Net/Socket.php \
#	Schedule/At.php \

PEARCMD=$(top_builddir)/sapi/cli/php -d include_path=$(top_srcdir)/pear pear/scripts/pear.in

install-pear-installer: $(top_builddir)/sapi/cli/php
	@for descfile in $(srcdir)/package-*.xml; do \
	    tmp="$${descfile%.xml}"; \
	    pkgname="$${tmp#*-}"; \
	    pkgver=`grep '<version>' $$descfile|head -1|cut -d\> -f2|cut -d\< -f1`; \
	    if $(PEARCMD) shell-test $$pkgname; then \
	    	if ! $(PEARCMD) shell-test $$pkgname $$pkgver; then \
	    	    $(PEARCMD) -q upgrade $$descfile | sed -e "s/^/$$pkgname $$pkgver: /"; \
	    	fi; \
	    else \
		$(PEARCMD) -q install $$descfile | sed -e "s/^/$$pkgname $$pkgver: /"; \
	    fi; \
	done

install-pear-packages: # requires cli installed
	@/bin/ls -1 $(srcdir)/packages | while read package; do \
	    case $$package in \
		*.tgz) pkg=$${package%.tgz};; \
		*.tar) pkg=$${package%.tar};; \
		*) continue;; \
	    esac; \
	    pkgname="$${pkg%-*}"; pkgver="$${pkg#*-}"; \
	    if $(INSTALL_ROOT)$(bindir)/pear -d php_dir=$(INSTALL_ROOT)$(PEAR_INSTALLDIR) shell-test $$pkgname $$pkgver; then \
		echo "$$pkgname $$pkgver: already installed"; \
	    else \
		$(INSTALL_ROOT)$(bindir)/pear -q -d php_dir=$(INSTALL_ROOT)$(PEAR_INSTALLDIR) -d bin_dir=$(INSTALL_ROOT)$(bindir) -d doc_dir=$(INSTALL_ROOT)$(datadir)/doc/pear -d ext_dir=$(INSTALL_ROOT)$(EXTENSION_DIR) install $(srcdir)/packages/$$package 2>&1 | sed -e "s/^/$$pkgname $$pkgver: /"; \
	    fi; \
	done

install-pear:
	@if $(mkinstalldirs) $(INSTALL_ROOT)$(peardir); then \
		$(MAKE) install-pear-installer; \
		$(MAKE) install-pear-packages; \
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
	for file in $(INSTALL_ROOT)$(bindir)/pearcmd-*.php; do \
		rm -f $$file; \
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
	ext/xml/expat \
	main \
	ext/mbstring \
	ext/pgsql \
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
