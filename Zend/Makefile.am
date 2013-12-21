## Process this file with automake to produce Makefile.in -*- makefile -*-

#CLEANFILES = zend_language_parser.c zend_language_parser.h zend_language_scanner.c zend_language_parser.output zend_ini_parser.c zend_ini_parser.h zend_ini_scanner.c zend_ini_parser.output

AUTOMAKE_OPTIONS=foreign
noinst_LTLIBRARIES=libZend.la

libZend_la_SOURCES=\
	zend_language_parser.y zend_language_scanner.l \
	zend_ini_parser.y zend_ini_scanner.l \
	zend_alloc.c zend_compile.c zend_constants.c zend_dynamic_array.c \
	zend_execute.c zend_execute_API.c zend_highlight.c zend_llist.c \
	zend_vm_opcodes.c zend_opcode.c zend_operators.c zend_ptr_stack.c zend_stack.c \
	zend_variables.c zend.c zend_API.c zend_extensions.c zend_hash.c \
	zend_list.c zend_indent.c zend_builtin_functions.c zend_sprintf.c \
	zend_ini.c zend_qsort.c zend_objects.c zend_object_handlers.c \
	zend_objects_API.c zend_ts_hash.c zend_stream.c \
	zend_default_classes.c \
	zend_iterators.c zend_interfaces.c zend_exceptions.c \
	zend_strtod.c zend_closures.c zend_float.c zend_string.c zend_signal.c \
	zend_generators.c

libZend_la_LDFLAGS =
libZend_la_LIBADD = @ZEND_EXTRA_LIBS@

# automake isn't too clever about "non-standard" use of lex and yacc

$(libZend_la_OBJECTS): zend_language_parser.h

zend_ini_scanner.lo: zend_ini_parser.h

# Language parser/scanner rules

zend_language_scanner.c: $(srcdir)/zend_language_scanner.l
	$(RE2C) $(RE2C_FLAGS) --no-generation-date --case-inverted -cbdFt $(srcdir)/zend_language_scanner_defs.h -o$@ $(srcdir)/zend_language_scanner.l

zend_language_parser.h: zend_language_parser.c
zend_language_parser.c: $(srcdir)/zend_language_parser.y
	$(YACC) -p zend -v -d $(srcdir)/zend_language_parser.y -o zend_language_parser.c

# INI parser/scanner rules

zend_ini_parser.c: $(srcdir)/zend_ini_parser.y
	$(YACC) -p ini_ -v -d $(srcdir)/zend_ini_parser.y -o zend_ini_parser.c

zend_ini_scanner.c: $(srcdir)/zend_ini_scanner.l
	$(RE2C) $(RE2C_FLAGS) --no-generation-date --case-inverted -cbdFt $(srcdir)/zend_ini_scanner_defs.h -o$@ $(srcdir)/zend_ini_scanner.l

zend_ini_parser.h: zend_ini_parser.c

depend:

zend_execute.lo: $(srcdir)/zend_execute.c
	$(LIBTOOL) --mode=compile $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) $(INLINE_CFLAGS) -c $(srcdir)/zend_execute.c

