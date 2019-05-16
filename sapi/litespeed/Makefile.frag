litespeed: $(SAPI_LITESPEED_PATH)

$(SAPI_LITESPEED_PATH): $(PHP_GLOBAL_OBJS) $(PHP_BINARY_OBJS) $(PHP_LITESPEED_OBJS)
	$(BUILD_LITESPEED)

install-litespeed: $(SAPI_LITESPEED_PATH)
	@echo "Installing PHP LiteSpeed binary:  $(INSTALL_ROOT)$(bindir)/"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(bindir)
	@$(INSTALL) -m 0755 $(SAPI_LITESPEED_PATH) $(INSTALL_ROOT)$(bindir)/$(program_prefix)lsphp$(program_suffix)
