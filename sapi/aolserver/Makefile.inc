# NSAPI/AOLserver module

INSTALL_IT=:

php4_nsapi.so: all-recursive
	g++ $(LDFLAGS) -shared sapi/nsapi/aolserver.o -o $@ libmodphp4-so.a $(LIBS)
