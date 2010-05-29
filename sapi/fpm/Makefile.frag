fpm: $(SAPI_FPM_PATH)

$(builddir)/fpm: 
	@mkdir -p $(builddir)/fpm

$(SAPI_FPM_PATH): $(builddir)/fpm $(PHP_GLOBAL_OBJS) $(PHP_SAPI_OBJS) $(SAPI_EXTRA_DEPS)
	$(BUILD_FPM)

$(builddir)/fpm/fpm_conf.lo: $(builddir)/../../main/build-defs.h

install-build: install-fpm

install-fpm: install-sapi
	@echo "Installing PHP FPM binary:        $(INSTALL_ROOT)$(sbindir)/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(sbindir)
	@$(mkinstalldirs) $(INSTALL_ROOT)$(localstatedir)/log
	@$(mkinstalldirs) $(INSTALL_ROOT)$(localstatedir)/run
	@$(INSTALL) -m 0755 $(SAPI_FPM_PATH) $(INSTALL_ROOT)$(sbindir)/$(program_prefix)php-fpm$(program_suffix)$(EXEEXT)

	@echo "Installing PHP FPM config:        $(INSTALL_ROOT)$(sysconfdir)/" && \
	$(mkinstalldirs) $(INSTALL_ROOT)$(sysconfdir) || :

	@$(INSTALL_DATA) sapi/fpm/php-fpm.conf $(INSTALL_ROOT)$(sysconfdir)/php-fpm.conf.default || :

	@echo "Installing PHP FPM man page:      $(INSTALL_ROOT)$(mandir)/man1/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(mandir)/man1
	@$(INSTALL_DATA) sapi/fpm/php-fpm.1 $(INSTALL_ROOT)$(mandir)/man1/php-fpm$(program_suffix).1

