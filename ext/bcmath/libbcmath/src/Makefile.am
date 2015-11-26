# Makefile for bcmath library

lib_LIBRARIES = libbcmath.a

include_HEADERS = bcmath.h

libbcmath_a_SOURCES= add.c compare.c debug.c div.c divmod.c doaddsub.c \
	init.c 	int2num.c nearzero.c neg.c num2long.c num2str.c output.c \
	raise.c raisemod.c recmul.c rmzero.c sqrt.c str2num.c sub.c zero.c \
	outofmem.c rt.c

INCLUDES = -I$(srcdir) -I..

CFLAGS = @CFLAGS@
CPPFLAGS = $(INCLUDES) -Wall

MAINTAINERCLEANFILES= Makefile.in $(libbcmath_a_SOURCES) private.h bcmath.h

all: $(bin_LIBRARIES)

clean:
	rm -f $(OBJS) $(LIB) *~
