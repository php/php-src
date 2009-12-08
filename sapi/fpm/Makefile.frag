fpm: $(SAPI_FPM_PATH)

$(SAPI_FPM_PATH): $(PHP_GLOBAL_OBJS) $(PHP_SAPI_OBJS) $(SAPI_EXTRA_DEPS)
	$(BUILD_FPM)

$(builddir)/fpm/fpm_conf.lo: $(builddir)/../../main/build-defs.h

install-build: install-fpm

install-fpm: install-sapi
	@echo "Installing PHP FPM binary:        $(INSTALL_ROOT)$(bindir)/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(bindir)
	@$(mkinstalldirs) $(INSTALL_ROOT)$(php_fpm_pid_dir)
	@$(mkinstalldirs) $(INSTALL_ROOT)$(php_fpm_log_dir)
	@$(INSTALL) -m 0755 $(SAPI_FPM_PATH) $(INSTALL_ROO)$(bindir)/$(program_prefix)php-fpm$(program_suffix)$(EXEEXT)

	@echo "Installing PHP FPM config:        $(INSTALL_ROOT)$(php_fpm_conf_dir)/" && \
	$(mkinstalldirs) $(INSTALL_ROOT)$(php_fpm_conf_dir) || :

	@$(INSTALL_DATA) sapi/fpm/conf/php-fpm.conf $(INSTALL_ROOT)$(php_fpm_conf_dir)/php-fpm.conf.default || :

	@echo "Installing PHP FPM man page:      $(INSTALL_ROOT)$(mandir)/man1/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(mandir)/man1
	@$(INSTALL_DATA) sapi/fpm/php-fpm.1 $(INSTALL_ROOT)$(mandir)/man1/php-fpm$(program_suffix).1

