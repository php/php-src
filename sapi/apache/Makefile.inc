# -*- makefile -*-

INSTALL_IT=$(APACHE_INSTALL)

# Apache modules
libphp4.a libmodphp4.a: all-recursive $(OBJS) sapi/libphpsapi.a $(REGEX_LIB)
	$(AR) $@.tmp $(OBJS)
	$(srcdir)/scripts/armerge $@ $@.tmp \
	    ext/libphpext.a libzend/libzend.a sapi/libphpsapi.a $(TSRM_LIB)
	@rm -f $@.tmp
	$(RANLIB) $(SAPI_TARGET)

# Apache modules
libmodphp4-so.a: all-recursive $(OBJS) sapi/libphpsapi.a $(REGEX_LIB)
	$(AR) $@.tmp $(OBJS)
	$(srcdir)/scripts/armerge $@ $@.tmp \
	    ext/libphpext.a libzend/libzend.a sapi/libphpsapi.a $(TSRM_LIB)
	@rm -f $@.tmp
	$(RANLIB) libmodphp4-so.a

# Apache 1.3 shared module
libphp4.so: libmodphp4-so.a $(REGEX_LIB)
	(cd sapi/apache; $(MAKE) mod_php4.o)
	$(APXS) $(INCLUDE) -c -o libphp4.so $(VERSION_SCRIPT) $(RPATHS) sapi/apache/mod_php4.c libmodphp4-so.a $(APXS_LDFLAGS) $(APXS_EXP)
