# CGI binary or fhttpd module

INSTALL_IT=:

php: all-recursive $(OBJS) $(REGEX_LIB) $(FHTTPD_LIB)
	$(CC) $(CFLAGS) -o $(SAPI_TARGET) $(LDFLAGS) $(OBJS) -Lsapi -lphpsapi $(LIBS)
