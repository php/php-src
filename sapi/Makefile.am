## Process this file with automake to produce Makefile.in

PHP_SAPI=@PHP_SAPI@
PHP_SAPI_LIB=$(PHP_SAPI)/libphpsapi_$(PHP_SAPI).a
SUBDIRS = $(PHP_SAPI)

noinst_LIBRARIES=libphpsapi.a

libphpsapi.a: $(PHP_SAPI_LIB)
	cp $(PHP_SAPI_LIB) $@
	@test "`uname -s`" = "Rhapsody" && $(RANLIB) $@ || exit 0

$(PHP_SAPI_LIB):
	(cd $(PHP_SAPI); $(MAKE) libphpsapi_$(PHP_SAPI).a)
