## Process this file with automake to produce Makefile.in -*- makefile -*-
AUTOMAKE_OPTIONS=foreign
noinst_LIBRARIES=libzend.a
libzend_a_SOURCES=\
	zend-parser.y zend-scanner.l \
	zend_alloc.c zend_compile.c zend_constants.c \
	zend_execute.c zend_execute_API.c zend_highlight.c zend_llist.c \
	zend_opcode.c zend_operators.c zend_ptr_stack.c zend_stack.c \
	zend_variables.c zend.c zend_API.c zend_extensions.c zend_hash.c \
	zend_list.c zend_indent.c

# automake isn't too clever about "non-standard" use of lex and yacc

zend-scanner.c: zend-scanner.l
	$(LEX) -Pzend -ozend-scanner.c -i zend-scanner.l

zend-scanner.cc: zend-scanner.l
	$(LEX) -+ -Sflex.skl -Pzend -ozend-scanner.cc -i zend-scanner.l

zend-parser.h: zend-parser.c
zend-parser.c: zend-parser.y
	$(YACC) -p zend -v -d zend-parser.y -o zend-parser.c

clean:
	-rm -f zend-parser.c zend-parser.h zend-scanner.c zend-parser.output

