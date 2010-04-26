# -*- makefile -*-

peardir=$(PEAR_INSTALLDIR)

# Skip all php.ini files altogether
PEAR_INSTALL_FLAGS = -n -dshort_open_tag=0 -dopen_basedir= -derror_reporting=1803 -dmemory_limit=-1 -ddetect_unicode=0

WGET = `which wget 2>/dev/null`
FETCH = `which fetch 2>/dev/null`
PEAR_PREFIX = -dp a${program_prefix}
PEAR_SUFFIX = -ds a$(program_suffix)

install-pear-installer: $(SAPI_CLI_PATH)
	@$(top_builddir)/sapi/cli/php $(PEAR_INSTALL_FLAGS) pear/install-pear-nozlib.phar -d "$(peardir)" -b "$(bindir)" ${PEAR_PREFIX} ${PEAR_SUFFIX}

install-pear:
	@echo "Installing PEAR environment:      $(INSTALL_ROOT)$(peardir)/"
	@if test ! -f $(builddir)/install-pear-nozlib.phar; then \
		if test -f $(srcdir)/install-pear-nozlib.phar; then \
			cp $(srcdir)/install-pear-nozlib.phar $(builddir)/install-pear-nozlib.phar; \
		else \
			if test ! -z "$(WGET)" && test -x "$(WGET)"; then \
				"$(WGET)" http://pear.php.net/install-pear-nozlib.phar -nd -P $(builddir)/; \
			elif test ! -z "$(FETCH)" && test -x "$(FETCH)"; then \
				"$(FETCH)" -o $(builddir)/ http://pear.php.net/install-pear-nozlib.phar; \
			else \
				$(top_builddir)/sapi/cli/php -n $(srcdir)/fetch.php http://pear.php.net/install-pear-nozlib.phar $(builddir)/install-pear-nozlib.phar; \
			fi \
		fi \
	fi
	@if test -f $(builddir)/install-pear-nozlib.phar && $(mkinstalldirs) $(INSTALL_ROOT)$(peardir); then \
		$(MAKE) -s install-pear-installer; \
	else \
		cat $(srcdir)/install-pear.txt; \
	fi

