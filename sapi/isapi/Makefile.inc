# ISAPI/Zeus module

INSTALL_IT=:

php4isapi.so: libmodphp4-so.a
	g++ $(LDFLAGS) -shared sapi/isapi/php4isapi.o -o $@ libmodphp4-so.a $(LIBS)
