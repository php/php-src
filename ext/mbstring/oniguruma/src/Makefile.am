## Makefile.am for Oniguruma
libname = libonig.la

AM_CFLAGS = -Wall
AM_CPPFLAGS = -I$(top_srcdir)

include_HEADERS = oniguruma.h oniggnu.h

posix_headers = onigposix.h

if ENABLE_POSIX_API
posix_sources = regposix.c regposerr.c
include_HEADERS += $(posix_headers)
AM_CFLAGS += -DUSE_POSIX_API
else
posix_sources =
endif

if ENABLE_BINARY_COMPATIBLE_POSIX_API
AM_CFLAGS += -DUSE_BINARY_COMPATIBLE_POSIX_API
endif


lib_LTLIBRARIES = $(libname)

libonig_la_SOURCES = regint.h regparse.h regenc.h st.h \
	regparse.c regcomp.c regexec.c \
	regenc.c regerror.c regext.c regsyntax.c regtrav.c regversion.c st.c \
	reggnu.c \
  $(posix_sources) \
	unicode.c \
	unicode_unfold_key.c \
	unicode_fold1_key.c \
	unicode_fold2_key.c \
	unicode_fold3_key.c \
	ascii.c utf8.c \
	utf16_be.c utf16_le.c \
	utf32_be.c utf32_le.c \
	euc_jp.c euc_jp_prop.c \
	sjis.c sjis_prop.c \
	iso8859_1.c \
	iso8859_2.c  iso8859_3.c \
	iso8859_4.c  iso8859_5.c \
	iso8859_6.c  iso8859_7.c \
	iso8859_8.c  iso8859_9.c \
	iso8859_10.c iso8859_11.c \
	iso8859_13.c iso8859_14.c \
	iso8859_15.c iso8859_16.c \
	euc_tw.c euc_kr.c big5.c \
	gb18030.c koi8_r.c cp1251.c \
	onig_init.c

EXTRA_DIST = koi8.c mktable.c \
	unicode_fold_data.c unicode_property_data.c \
	unicode_property_data_posix.c \
	unicode_egcb_data.c unicode_wb_data.c


libonig_la_LDFLAGS = $(EXTRA_LIBONIG_LDFLAGS) -version-info $(LTVERSION)

if USE_LIBONIG_DEF_FILE

libonig_la_LDFLAGS +=  -Wl,--output-def,$(LIBONIG_DEF_FILE)

install-data-hook:
	echo "$(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=install $(INSTALL_DATA) $(LIBONIG_DEF_FILE) $(DESTDIR)$(libdir)"; \
	$(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=install $(INSTALL_DATA) $(LIBONIG_DEF_FILE) $(DESTDIR)$(libdir) || exit 1

uninstall-hook:
	echo " $(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=uninstall rm -f '$(DESTDIR)$(libdir)/$(LIBONIG_DEF_FILE)'"; \
	$(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=uninstall rm -f "$(DESTDIR)$(libdir)/$(LIBONIG_DEF_FILE)"

endif


dll:
	$(CXX) -shared -Wl,--output-def,libonig.def -o libonig.dll *.o \
	$(LIBS)
	strip libonig.dll


# character-types-table source generator
mktable: mktable.c regenc.h
	$(CC) -I$(top_srcdir) -o mktable mktable.c
