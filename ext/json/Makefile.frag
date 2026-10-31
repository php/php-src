$(srcdir)/json_scanner.c $(srcdir)/php_json_scanner_defs.h: $(srcdir)/json_scanner.re $(srcdir)/json_parser.tab.h
	@$(RE2C) $(RE2C_FLAGS) -t $(srcdir)/php_json_scanner_defs.h -bci -o $(srcdir)/json_scanner.c $(srcdir)/json_scanner.re

$(srcdir)/json_parser.tab.c $(srcdir)/json_parser.tab.h: $(srcdir)/json_parser.y
	@$(YACC) $(YFLAGS) --defines -l $(srcdir)/json_parser.y -o $(srcdir)/json_parser.tab.c

$(srcdir)/php_json_escape_table.h: $(srcdir)/gen_json_escape_table.php
	@if test ! -z "$(PHP)"; then \
		$(PHP) $(srcdir)/gen_json_escape_table.php; \
	fi;
