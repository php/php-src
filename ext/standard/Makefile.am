## Process this file with automake to produce Makefile.in
INCLUDES=@INCLUDES@ -I@top_srcdir@ -I@top_srcdir@/libzend
noinst_LIBRARIES=libphpext_standard.a
libphpext_standard_a_SOURCES=\
	base64.c basic_functions.c browscap.c datetime.c dir.c \
	dns.c exec.c file.c filestat.c formatted_print.c fsock.c \
	html.c image.c info.c link.c mail.c math.c md5.c microtime.c \
	pack.c pageinfo.c rand.c reg.c soundex.c string.c \
	syslog.c type.c uniqid.c url.c iptc.c var.c quot_print.c \
	cyr_convert.c flock_compat.c crypt.c dl.c head.c post.c \
	parsedate.y lcg.c url_scanner.c metaphone.c

$(srcdir)/url_scanner.c: $(srcdir)/url_scanner.re
	re2c -b $< > $@

#number.o: number.c
#	$(CC) $(CFLAGS) -w@WARNING_LEVEL@ -c $< -o $@
