fpm: $(SAPI_FPM_PATH)

$(SAPI_FPM_PATH): $(PHP_GLOBAL_OBJS) $(PHP_BINARY_OBJS) $(PHP_FASTCGI_OBJS) $(PHP_FPM_OBJS)
	$(BUILD_FPM)

install-fpm: $(SAPI_FPM_PATH)
	@echo "Installing PHP FPM binary:        $(INSTALL_ROOT)$(sbindir)/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(sbindir)
	@$(mkinstalldirs) $(INSTALL_ROOT)$(localstatedir)/log
	@$(mkinstalldirs) $(INSTALL_ROOT)$(localstatedir)/run
	@$(INSTALL) -m 0755 $(SAPI_FPM_PATH) $(INSTALL_ROOT)$(sbindir)/$(program_prefix)php-fpm$(program_suffix)$(EXEEXT)

	@echo "Installing PHP FPM config:        $(INSTALL_ROOT)$(sysconfdir)/" && \
	$(mkinstalldirs) $(INSTALL_ROOT)$(sysconfdir)/php-fpm.d || :
	@$(INSTALL_DATA) sapi/fpm/php-fpm.conf $(INSTALL_ROOT)$(sysconfdir)/php-fpm.conf.default || :
	@$(INSTALL_DATA) sapi/fpm/www.conf $(INSTALL_ROOT)$(sysconfdir)/php-fpm.d/www.conf.default || :

	@echo "Installing PHP FPM man page:      $(INSTALL_ROOT)$(mandir)/man8/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(mandir)/man8
	@$(INSTALL_DATA) sapi/fpm/php-fpm.8 $(INSTALL_ROOT)$(mandir)/man8/php-fpm$(program_suffix).8

	@echo "Installing PHP FPM status page:   $(INSTALL_ROOT)$(datadir)/fpm/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(datadir)/fpm
	@$(INSTALL_DATA) sapi/fpm/status.html $(INSTALL_ROOT)$(datadir)/fpm/status.html
