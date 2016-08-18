$(srcdir)/json_scanner.c: $(srcdir)/json_scanner.re
	$(RE2C) -t $(srcdir)/php_json_scanner_defs.h --no-generation-date -bci -o $@ $(srcdir)/json_scanner.re

$(srcdir)/json_parser.tab.c: $(srcdir)/json_parser.y
	$(YACC) --defines -l $(srcdir)/json_parser.y -o $@
