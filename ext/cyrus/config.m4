dnl
dnl $Id$
dnl

PHP_ARG_WITH(cyrus, for cyrus imap support,
[  --with-cyrus            Include cyrus imap support])

if test "$PHP_CYRUS" != "no"; then
	found_cyrus=no
	found_sasl=no
	found_openssl=no
	for i in /usr /usr/local $PHP_CYRUS; do
		if test -r $i/include/cyrus/imclient.h && test "$found_cyrus" = "no"; then
			PHP_ADD_INCLUDE($i/include)
			PHP_ADD_LIBRARY_WITH_PATH(cyrus, $i/lib, CYRUS_SHARED_LIBADD)
			found_cyrus=yes
		fi
		if test -r $i/include/sasl.h && test "$found_sasl" = "no"; then
			PHP_ADD_INCLUDE($i/include)
			PHP_ADD_LIBRARY_WITH_PATH(sasl, $i/lib, CYRUS_SHARED_LIBADD)
			found_sasl=yes
		elif test -r $i/include/sasl/sasl.h && test "$found_sasl" = "no"; then
			PHP_ADD_INCLUDE($i/include/sasl)
			PHP_ADD_LIBRARY_WITH_PATH(sasl, $i/lib, CYRUS_SHARED_LIBADD)
			found_sasl=yes
		fi
		if test -r $i/include/openssl/ssl.h && test "$found_openssl" = "no" && test "$PHP_OPENSSL" = "no"; then
			PHP_ADD_LIBRARY_WITH_PATH(ssl, $i/lib, CYRUS_SHARED_LIBADD)
			PHP_ADD_LIBRARY_WITH_PATH(crypto, $i/lib, CYRUS_SHARED_LIBADD)
			found_openssl=yes
		fi
	done

	if test "$found_cyrus" = "no"; then
		AC_MSG_RESULT(not found)
		AC_MSG_ERROR(Please Re-install the cyrus distribution)
	fi

	if test "$found_sasl" = "no"; then
		AC_MSG_RESULT(sasl not found)
		AC_MSG_ERROR(Please Re-install the cyrus distribution)
	fi

	PHP_NEW_EXTENSION(cyrus, cyrus.c, $ext_shared)
	PHP_SUBST(CYRUS_SHARED_LIBADD)
	AC_DEFINE(HAVE_CYRUS,1,[ ])
fi
