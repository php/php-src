piedir=$(PIE_INSTALLDIR)

PIE_PHP_FLAGS = -dmemory_limit=-1

CURL = `which curl 2>/dev/null`
WGET = `which wget 2>/dev/null`
FETCH = `which fetch 2>/dev/null`
FETCH_PHP = $(top_srcdir)/pear/fetch.php
GH = `which gh 2>/dev/null`
PIE_PHAR_URL = https://github.com/php/pie/releases/latest/download/pie.phar
PIE_PHAR_TEMP_DL_LOCATION = $(top_srcdir)/pie/pie_temp.phar
PIE_PHAR_DESTINATION = $(INSTALL_ROOT)$(piedir)/pie

$(PIE_PHAR_DESTINATION):
	@echo "Installing PIE:                   $(PIE_PHAR_DESTINATION)"
# First, figure out a way to download the phar, with curl, wget, fetch, or a backup PHP script...
	@if test ! -z "$(CURL)" && test -x "$(CURL)"; then \
		"$(CURL)" --no-progress-meter --silent --location "${PIE_PHAR_URL}" --output $(PIE_PHAR_TEMP_DL_LOCATION); \
	elif test ! -z "$(WGET)" && test -x "$(WGET)"; then \
		"$(WGET)" "${PIE_PHAR_URL}" --quiet --no-directories --output-document=$(PIE_PHAR_TEMP_DL_LOCATION); \
	elif test ! -z "$(FETCH)" && test -x "$(FETCH)"; then \
		"$(FETCH)" -o $(PIE_PHAR_TEMP_DL_LOCATION) "${PIE_PHAR_URL}"; \
	else \
		$(top_builddir)/sapi/cli/php -n "${FETCH_PHP}" "${PIE_PHAR_URL}" $(PIE_PHAR_TEMP_DL_LOCATION) ; \
	fi
# Try to verify using `gh` CLI, or if not use `self-verify` (which isn't the best, since it could already have been tampered)
	@if test ! -z "$(GH)" && test -x "$(GH)"; then \
		"$(GH)" attestation verify --owner=php $(PIE_PHAR_TEMP_DL_LOCATION); \
	else \
		$(top_builddir)/sapi/cli/php $(PIE_PHP_FLAGS) $(PIE_PHAR_TEMP_DL_LOCATION) self-verify; \
	fi
# Once verified, move it to the real location
	@mv $(PIE_PHAR_TEMP_DL_LOCATION) $(PIE_PHAR_DESTINATION)
	@chmod +x $(PIE_PHAR_DESTINATION)

.PHONY: install-pie
install-pie: $(PIE_PHAR_DESTINATION)
	@$(top_builddir)/sapi/cli/php $(PIE_PHP_FLAGS) $(PIE_PHAR_DESTINATION) self-update > /dev/null 2>&1
	@$(top_builddir)/sapi/cli/php $(PIE_PHP_FLAGS) $(PIE_PHAR_DESTINATION) --version
