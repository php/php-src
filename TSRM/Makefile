CC = gcc
CFLAGS = -O2 -DPTHREADS -DHAVE_STDARG_H=1
OBJS = TSRM.o
AR = ar

libtsrm.a:	$(OBJS)
	$(AR) cru libtsrm.a $(OBJS)
