## Process this file with automake to produce Makefile.in
noinst_LTLIBRARIES=libphpext_standard.la
libphpext_standard_la_SOURCES=\
	array.c base64.c basic_functions.c browscap.c crypt.c cyr_convert.c datetime.c \
	dir.c dl.c dns.c exec.c file.c filestat.c flock_compat.c \
	formatted_print.c fsock.c head.c html.c image.c info.c iptc.c lcg.c \
	link.c mail.c math.c md5.c metaphone.c microtime.c pack.c pageinfo.c \
	parsedate.y post.c quot_print.c rand.c reg.c soundex.c string.c \
	syslog.c type.c uniqid.c url.c url_scanner.c var.c output.c

$(srcdir)/url_scanner.c: $(srcdir)/url_scanner.re
	-re2c $< > $@.new && mv $@.new $@

#number.o: number.c
#	$(CC) $(CFLAGS) -w@WARNING_LEVEL@ -c $< -o $@
