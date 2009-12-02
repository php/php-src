fpm: $(SAPI_FPM_PATH)

$(SAPI_FPM_PATH): $(PHP_GLOBAL_OBJS) $(PHP_SAPI_OBJS) $(SAPI_EXTRA_DEPS)
	$(BUILD_FPM)

install-build: install-fpm

install-fpm: install-sapi
	@echo "Installing PHP FPM binary:        $(INSTALL_ROOT)$(php_fpm_bin_dir)/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(php_fpm_bin_dir)
	@$(mkinstalldirs) $(INSTALL_ROOT)$(php_fpm_pid_dir)
	@$(mkinstalldirs) $(INSTALL_ROOT)$(php_fpm_log_dir)
	@$(INSTALL) -m 0755 $(SAPI_FPM_PATH) $(INSTALL_ROOT)$(php_fpm_bin_path)$(program_suffix)$(EXEEXT)

	@test "$(php_fpm_conf)" && \
	echo "Installing PHP FPM config:        $(INSTALL_ROOT)$(php_fpm_conf_dir)/" && \
	$(mkinstalldirs) $(INSTALL_ROOT)$(php_fpm_conf_dir) || :

	@test "$(php_fpm_conf)" && \
	$(INSTALL_DATA) sapi/fpm/conf/php-fpm.conf $(INSTALL_ROOT)$(php_fpm_conf_path).default || :

	@echo "Installing PHP FPM man page:      $(INSTALL_ROOT)$(mandir)/man1/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(mandir)/man1
	@$(INSTALL_DATA) sapi/fpm/$(php_fpm_bin).1 $(INSTALL_ROOT)$(mandir)/man1/$(php_fpm_bin)$(program_suffix).1

