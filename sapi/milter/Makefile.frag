$(SAPI_MILTER_PATH): $(PHP_GLOBAL_OBJS) $(PHP_MILTER_OBJS)
	$(BUILD_MILTER)

install-milter: $(SAPI_MILTER_PATH)
	@echo "Installing PHP Milter binary:        $(INSTALL_ROOT)$(bindir)/"
	@$(INSTALL_CLI)

