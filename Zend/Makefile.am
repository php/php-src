## Process this file with automake to produce Makefile.in -*- makefile -*-

#CLEANFILES = zend-parser.c zend-parser.h zend-scanner.c zend-parser.output 

AUTOMAKE_OPTIONS=foreign
noinst_LIBRARIES=libzend.a
libzend_a_SOURCES=\
	zend-parser.y zend-scanner.l \
	zend_alloc.c zend_compile.c zend_constants.c \
	zend_execute.c zend_execute_API.c zend_highlight.c zend_llist.c \
	zend_opcode.c zend_operators.c zend_ptr_stack.c zend_stack.c \
	zend_variables.c zend.c zend_API.c zend_extensions.c zend_hash.c \
	zend_list.c zend_indent.c

# taken from automake 1.4
CXXCOMPILE = $(CXX) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)

# automake isn't too clever about "non-standard" use of lex and yacc

zend-scanner.o: @ZEND_SCANNER_SRC@
	@ZEND_SCANNER_COMPILE@

zend-scanner.c: zend-scanner.l
	$(LEX) -Pzend -ozend-scanner.c -i $(srcdir)/zend-scanner.l

zend-scanner.cc: zend-scanner.l
	$(LEX) -+ -S$(srcdir)/flex.skl -Pzend -ozend-scanner.cc -i $(srcdir)/zend-scanner.l

zend-parser.h: zend-parser.c
zend-parser.c: zend-parser.y
	$(YACC) -p zend -v -d $(srcdir)/zend-parser.y -o zend-parser.c

