# Redirect cli source objects to sapi/embed/cli, so the version without main() doesn't conflict with cli sapis version with main()
EMBED_CLI_SRCDIR = $(top_srcdir)/sapi/cli
EMBED_CLI_CC = $(LIBTOOL) --tag=CC --mode=compile $(CC) $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS) -Isapi/cli/ -I$(EMBED_CLI_SRCDIR) -DPHP_EMBED_SAPI -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1

sapi/embed/cli/php_cli.lo: $(EMBED_CLI_SRCDIR)/php_cli.c
	$(EMBED_CLI_CC) -c $< -o $@

sapi/embed/cli/php_http_parser.lo: $(EMBED_CLI_SRCDIR)/php_http_parser.c
	$(EMBED_CLI_CC) -c $< -o $@

sapi/embed/cli/php_cli_server.lo: $(EMBED_CLI_SRCDIR)/php_cli_server.c
	$(EMBED_CLI_CC) -c $< -o $@

sapi/embed/cli/ps_title.lo: $(EMBED_CLI_SRCDIR)/ps_title.c
	$(EMBED_CLI_CC) -c $< -o $@

sapi/embed/cli/php_cli_process_title.lo: $(EMBED_CLI_SRCDIR)/php_cli_process_title.c
	$(EMBED_CLI_CC) -c $< -o $@
