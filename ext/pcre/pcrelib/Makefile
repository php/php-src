# Make file for PCRE (Perl-Compatible Regular Expression) library.

# Edit CC, CFLAGS, and RANLIB for your system.

# It is believed that RANLIB=ranlib is required for AIX, BSDI, FreeBSD, Linux,
# MIPS RISCOS, NetBSD, OpenBSD, Digital Unix, and Ultrix.

# Use CFLAGS = -DUSE_BCOPY on SunOS4 and any other system that lacks the
# memmove() function, but has bcopy().

# Use CFLAGS = -DSTRERROR_FROM_ERRLIST on SunOS4 and any other system that
# lacks the strerror() function, but can provide the equivalent by indexing
# into errlist.

AR = ar cq
CC = gcc -O2 -Wall
CFLAGS =
RANLIB = @true

##########################################################################

OBJ = maketables.o get.o study.o pcre.o

all:            libpcre.a libpcreposix.a pcretest pgrep

pgrep:          libpcre.a pgrep.o
		$(CC) $(CFLAGS) -o pgrep pgrep.o libpcre.a

pcretest:       libpcre.a libpcreposix.a pcretest.o
		$(PURIFY) $(CC) $(CFLAGS) -o pcretest pcretest.o libpcre.a libpcreposix.a

libpcre.a:      $(OBJ)
		-rm -f libpcre.a
		$(AR) libpcre.a $(OBJ)
		$(RANLIB) libpcre.a

libpcreposix.a: pcreposix.o
		-rm -f libpcreposix.a
		$(AR) libpcreposix.a pcreposix.o
		$(RANLIB) libpcreposix.a

pcre.o:         chartables.c pcre.c pcre.h internal.h Makefile
		$(CC) -c $(CFLAGS) pcre.c

pcreposix.o:    pcreposix.c pcreposix.h internal.h pcre.h Makefile
		$(CC) -c $(CFLAGS) pcreposix.c

maketables.o:   maketables.c pcre.h internal.h Makefile
		$(CC) -c $(CFLAGS) maketables.c

get.o:          get.c pcre.h internal.h Makefile
		$(CC) -c $(CFLAGS) get.c

study.o:        study.c pcre.h internal.h Makefile
		$(CC) -c $(CFLAGS) study.c

pcretest.o:     pcretest.c pcre.h Makefile
		$(CC) -c $(CFLAGS) pcretest.c

pgrep.o:        pgrep.c pcre.h Makefile
		$(CC) -c $(CFLAGS) pgrep.c

# An auxiliary program makes the default character table source

chartables.c:   dftables
		./dftables >chartables.c

dftables:       dftables.c maketables.c pcre.h internal.h Makefile
		$(CC) -o dftables $(CFLAGS) dftables.c

# We deliberately omit dftables and chartables.c from 'make clean'; once made
# chartables.c shouldn't change, and if people have edited the tables by hand,
# you don't want to throw them away.

clean:;         -rm -f *.o *.a pcretest pgrep

runtest:        all
		./RunTest

# End
