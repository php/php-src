## Process this file with automake to produce Makefile.in -*- makefile -*-

#CLEANFILES = zend-parser.c zend-parser.h zend-scanner.c zend-parser.output 

ZEND_SCANNER = @ZEND_SCANNER@

AUTOMAKE_OPTIONS=foreign
EXTRA_LTLIBRARIES=libzend_cc.la libzend_c.la
noinst_LTLIBRARIES=$(ZEND_SCANNER) libzend.la

libzend_cc_la_SOURCES=zend-scanner-cc.cc
libzend_c_la_SOURCES=zend-scanner.c

libzend_la_SOURCES=\
	zend-parser.y \
	zend_alloc.c zend_compile.c zend_constants.c \
	zend_execute.c zend_execute_API.c zend_highlight.c zend_llist.c \
	zend_opcode.c zend_operators.c zend_ptr_stack.c zend_stack.c \
	zend_variables.c zend.c zend_API.c zend_extensions.c zend_hash.c \
	zend_list.c zend_indent.c zend_builtin_functions.c

libzend_la_LIBADD = $(ZEND_SCANNER)
libzend_la_LDFLAGS = @EXTRA_LIBS@
	
# automake isn't too clever about "non-standard" use of lex and yacc

zend-scanner.l: zend-parser.h

zend-scanner.c: zend-scanner.l
	$(LEX) -Pzend -o$@ -i $(srcdir)/zend-scanner.l

zend-scanner-cc.cc: zend-scanner.l
	$(LEX) -+ -B -i -S$(srcdir)/flex.skl -Pzend -o$@ $(srcdir)/zend-scanner.l

zend-parser.h: zend-parser.c
zend-parser.c: zend-parser.y
	$(YACC) -p zend -v -d $(srcdir)/zend-parser.y -o zend-parser.c

