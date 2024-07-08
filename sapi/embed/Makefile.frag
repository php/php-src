install-embed:
	@echo "Installing PHP Embed SAPI:         $(INSTALL_ROOT)$(libdir)"
	@$(mkinstalldirs) $(INSTALL_ROOT)$(orig_libdir)/pkgconfig
	@$(INSTALL_DATA) sapi/embed/php-embed.pc $(INSTALL_ROOT)$(orig_libdir)/pkgconfig/$(program_prefix)php-embed$(program_suffix).pc
