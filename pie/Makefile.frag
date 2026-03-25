piedir=$(PIE_INSTALLDIR)

PIE_PHP_FLAGS = -dmemory_limit=-1

CURL = `which curl 2>/dev/null`
WGET = `which wget 2>/dev/null`
FETCH = `which fetch 2>/dev/null`
GH = `which gh 2>/dev/null`
PIE_PHAR_URL = https://github.com/php/pie/releases/latest/download/pie.phar

PIE_PHAR_DESTINATION = $(INSTALL_ROOT)$(piedir)/pie

$(PIE_PHAR_DESTINATION):
	@echo "Installing PIE:                   $(PIE_PHAR_DESTINATION)"
	@if test ! -z "$(CURL)" && test -x "$(CURL)"; then \
		"$(CURL)" --no-progress-meter --silent --location "${PIE_PHAR_URL}" --output $(PIE_PHAR_DESTINATION); \
	elif test ! -z "$(WGET)" && test -x "$(WGET)"; then \
		"$(WGET)" "${PIE_PHAR_URL}" --quiet --no-directories --output-document=$(PIE_PHAR_DESTINATION); \
	elif test ! -z "$(FETCH)" && test -x "$(FETCH)"; then \
		"$(FETCH)" -o $(PIE_PHAR_DESTINATION) "${PIE_PHAR_URL}"; \
	else \
		$(top_builddir)/sapi/cli/php -n $(top_srcdir)/pear/fetch.php "${PIE_PHAR_URL}" $(PIE_PHAR_DESTINATION) ; \
	fi
	@if test ! -z "$(GH)" && test -x "$(GH)"; then \
		"$(GH)" attestation verify --owner=php $(PIE_PHAR_DESTINATION); \
	else \
		$(top_builddir)/sapi/cli/php $(PIE_PHP_FLAGS) $(PIE_PHAR_DESTINATION) self-verify; \
	fi
	@chmod +x $(PIE_PHAR_DESTINATION)

.PHONY: install-pie
install-pie: $(PIE_PHAR_DESTINATION)
	@$(top_builddir)/sapi/cli/php $(PIE_PHP_FLAGS) $(PIE_PHAR_DESTINATION) self-update > /dev/null 2>&1
	@$(top_builddir)/sapi/cli/php $(PIE_PHP_FLAGS) $(PIE_PHAR_DESTINATION) --version
