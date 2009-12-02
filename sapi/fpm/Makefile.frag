fpm: $(SAPI_FPM_PATH)

$(SAPI_FPM_PATH): $(PHP_GLOBAL_OBJS) $(PHP_SAPI_OBJS) $(SAPI_EXTRA_DEPS)
	$(BUILD_FPM)

install: install-fpm

install-fpm: all
	@echo "Installing PHP FPM binary:        $(INSTALL_ROOT)$(php_fpm_bin_path)"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(php_fpm_bin_dir)
	@$(mkinstalldirs) $(INSTALL_ROOT)$(php_fpm_pid_dir)
	@$(mkinstalldirs) $(INSTALL_ROOT)$(php_fpm_log_dir)
	@$(INSTALL) -m 0755 $(SAPI_FPM_PATH) $(INSTALL_ROOT)$(php_fpm_bin_path)$(program_suffix)$(EXEEXT)

	@test "$(php_fpm_conf)" && \
	echo "Installing PHP FPM config:        $(INSTALL_ROOT)$(php_fpm_conf_path)" && \
	$(mkinstalldirs) $(INSTALL_ROOT)$(php_fpm_conf_dir) || :

	@test "$(php_fpm_conf)" && \
	test -f "$(INSTALL_ROOT)$(php_fpm_conf_path)" && \
	$(INSTALL_DATA) --backup=numbered $(INSTALL_ROOT)$(php_fpm_conf_path) $(INSTALL_ROOT)$(php_fpm_conf_path).old || :

	@test "$(php_fpm_conf)" && \
	$(INSTALL_DATA) sapi/fpm/conf/php-fpm.conf $(INSTALL_ROOT)$(php_fpm_conf_path).default && \
	ln -sf $(INSTALL_ROOT)$(php_fpm_conf_path).default $(INSTALL_ROOT)$(php_fpm_conf_path) || :

	@echo "Installing PHP FPM man page:      $(INSTALL_ROOT)$(mandir)/man1/$(php_fpm_bin)$(program_suffix).1"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(mandir)/man1
	@$(INSTALL_DATA) sapi/fpm/$(php_fpm_bin).1 $(INSTALL_ROOT)$(mandir)/man1/$(php_fpm_bin)$(program_suffix).1

	@test -d /etc/nginx/ && \
	echo "Installing NGINX sample config:   /etc/nginx/nginx-site-conf.sample" && \
	$(mkinstalldirs) $(INSTALL_ROOT)/etc/nginx && \
	$(INSTALL_DATA) -b sapi/fpm/nginx-site-conf.sample $(INSTALL_ROOT)/etc/nginx/nginx-site-conf.sample || :

	@test -d /usr/local/etc/nginx/ && \
	echo "Installing NGINX sample config:   /usr/local/etc/nginx/nginx-site-conf.sample" && \
	$(mkinstalldirs) $(INSTALL_ROOT)/usr/local/etc/nginx && \
	$(INSTALL_DATA) -b sapi/fpm/nginx-site-conf.sample $(INSTALL_ROOT)/usr/local/etc/nginx/nginx-site-conf.sample || :

	@test -d /usr/local/nginx/conf/ && \
	echo "Installing NGINX sample config:   /usr/local/nginx/conf/nginx-site-conf.sample" && \
	$(mkinstalldirs) $(INSTALL_ROOT)/usr/local/nginx/conf && \
	$(INSTALL_DATA) -b sapi/fpm/nginx-site-conf.sample $(INSTALL_ROOT)/usr/local/nginx/conf/nginx-site-conf.sample || :

	@echo "" || :

