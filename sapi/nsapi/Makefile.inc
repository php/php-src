# NSAPI/AOLserver module

INSTALL_IT=:

php4_nsapi.so: libmodphp4-so.a
	g++ $(LDFLAGS) -shared sapi/nsapi/aolserver.o -o $@ libmodphp4-so.a $(LIBS)
