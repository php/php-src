EXTRA_DIST=mk_eaw_tbl.awk
lib_LTLIBRARIES=libmbfl.la
libmbfl_la_SOURCES=mbfilter.c \
	mbfl_string.c \
	mbfl_language.c \
	mbfl_encoding.c \
	mbfl_convert.c \
	mbfl_ident.c \
	mbfl_memory_device.c \
	mbfl_allocators.c \
	mbfl_filter_output.c \
	mbfilter_pass.c \
	mbfilter_wchar.c \
	mbfilter_8bit.c \
	eaw_table.h
libmbfl_filters_la=../filters/libmbfl_filters.la
libmbfl_nls_la=../nls/libmbfl_nls.la
libmbfl_la_LIBADD=$(libmbfl_filters_la) $(libmbfl_nls_la)
libmbfl_la_LDFLAGS=-version-info $(SHLIB_VERSION)
libmbfl_includedir=$(includedir)/mbfl
libmbfl_include_HEADERS=mbfilter.h \
	mbfl_consts.h \
	mbfl_encoding.h \
	mbfl_language.h \
	mbfl_string.h \
	mbfl_convert.h \
	mbfl_ident.h \
	mbfl_memory_device.h \
	mbfl_allocators.h \
	mbfl_defs.h \
	mbfl_filter_output.h \
	mbfilter_pass.h \
	mbfilter_wchar.h \
	mbfilter_8bit.h

mbfilter.c: eaw_table.h

eaw_table.h: mk_eaw_tbl.awk
	$(AWK) -v TABLE_NAME=mbfl_eaw_table -f mk_eaw_tbl.awk EastAsianWidth.txt > $@

EastAsianWidth.txt:
	$(FETCH_VIA_FTP) ftp://ftp.unicode.org/Public/UNIDATA/EastAsianWidth.txt

$(libmbfl_filters_la):
	$(MAKE) -C `dirname $(libmbfl_filters_la)`

$(libmbfl_nls_la):
	$(MAKE) -C `dirname $(libmbfl_nls_la)`
