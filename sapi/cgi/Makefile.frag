$(SAPI_CGI_PATH): $(PHP_GLOBAL_OBJS) $(PHP_SAPI_OBJS)
	$(BUILD_CGI)

install-cgi: $(SAPI_CGI_PATH)
	@echo "Installing PHP CGI binary:        $(INSTALL_ROOT)$(bindir)/"
	@$(INSTALL_CGI)
