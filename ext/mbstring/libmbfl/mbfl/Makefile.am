lib_LTLIBRARIES=libmbfl.la
libmbfl_la_SOURCES=mbfilter.c mbfl_string.c mbfl_language.c mbfl_encoding.c mbfl_convert.c mbfl_ident.c mbfl_memory_device.c mbfl_allocators.c mbfl_filter_output.c mbfilter_pass.c mbfilter_wchar.c mbfilter_8bit.c
libmbfl_filters_la=../filters/libmbfl_filters.la
libmbfl_nls_la=../nls/libmbfl_nls.la
libmbfl_la_LIBADD=$(libmbfl_filters_la) $(libmbfl_nls_la)
libmbfl_includedir=$(includedir)/mbfl
libmbfl_include_HEADERS=mbfilter.h mbfl_consts.h mbfl_encoding.h mbfl_language.h mbfl_string.h mbfl_convert.h mbfl_ident.h mbfl_memory_device.h mbfl_allocators.h mbfl_defs.h

$(libmbfl_filters_la):
	$(MAKE) -C `dirname $(libmbfl_filters_la)`

$(libmbfl_nls_la):
	$(MAKE) -C `dirname $(libmbfl_nls_la)`
