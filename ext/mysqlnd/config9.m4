PHP_ARG_ENABLE([mysqlnd],
  [whether to enable mysqlnd],
  [AS_HELP_STRING([--enable-mysqlnd],
    [Enable mysqlnd explicitly, will be done implicitly when required by other
    extensions])],
  [no],
  [yes])

PHP_ARG_ENABLE([mysqlnd_compression_support],
  [whether to disable compressed protocol support in mysqlnd],
  [AS_HELP_STRING([--disable-mysqlnd-compression-support],
    [Disable support for the MySQL compressed protocol in mysqlnd])],
  [yes],
  [no])

PHP_ARG_ENABLE([mysqlnd_sasl_support],
  [whether to enable sasl support in mysqlnd],
  [AS_HELP_STRING([--enable-mysqlnd-sasl-support],
    [Enable support for the sasl protocol in mysqlnd])],
  [no],
  [yes])
  
  
PHP_ARG_ENABLE([mysqlnd_ldap_krb_support],
  [whether to enable ldap kerberos support in mysqlnd],
  [AS_HELP_STRING([--enable-mysqlnd-ldap-krb-support],
    [Enable support for the ldap kerberos protocol in mysqlnd])],
  [no],
  [yes])

dnl If some extension uses mysqlnd it will get compiled in PHP core
if test "$PHP_MYSQLND" != "no" || test "$PHP_MYSQLND_ENABLED" = "yes"; then
  mysqlnd_ps_sources="mysqlnd_ps.c mysqlnd_ps_codec.c"
  mysqlnd_base_sources="mysqlnd_connection.c mysqlnd_alloc.c mysqlnd_charset.c mysqlnd_wireprotocol.c \
                   mysqlnd_loaddata.c mysqlnd_reverse_api.c mysqlnd_vio.c mysqlnd_protocol_frame_codec.c \
                   mysqlnd_statistics.c mysqlnd_driver.c mysqlnd_ext_plugin.c mysqlnd_auth.c \
				   mysqlnd_result.c mysqlnd_result_meta.c mysqlnd_debug.c mysqlnd_commands.c \
				   mysqlnd_block_alloc.c mysqlnd_read_buffer.c mysqlnd_plugin.c php_mysqlnd.c"


  if test "$PHP_MYSQLND_COMPRESSION_SUPPORT" != "no"; then
    PKG_CHECK_MODULES([ZLIB], [zlib])
    PHP_EVAL_LIBLINE($ZLIB_LIBS, MYSQLND_SHARED_LIBADD)
    PHP_EVAL_INCLINE($ZLIB_CFLAGS)
    AC_DEFINE([MYSQLND_COMPRESSION_ENABLED], 1, [Enable compressed protocol support])
  fi

  if test "$PHP_MYSQLND_SASL_SUPPORT" != "no"; then
    PKG_CHECK_MODULES([SASL], [libsasl2])
    PHP_EVAL_LIBLINE($SASL_LIBS, MYSQLND_SHARED_LIBADD)
    PHP_EVAL_INCLINE($SASL_CFLAGS)
    AC_DEFINE([MYSQLND_HAVE_SASL], 1, [SASL protocol support])
  fi
  
  if test "$PHP_MYSQLND_LDAP_KRB_SUPPORT" != "no"; then
    PKG_CHECK_MODULES([KRB5], [libkrb5])
    PHP_EVAL_LIBLINE($KRB5_LIBS, MYSQLND_SHARED_LIBADD)
    PHP_EVAL_INCLINE($KRB5_CFLAGS)
    AC_DEFINE([MYSQLND_HAVE_KRB5], 1, [LDAP Kerberos protocol support])
  fi

  AC_DEFINE([MYSQLND_SSL_SUPPORTED], 1, [Enable core mysqlnd SSL code])
  AC_DEFINE([MYSQLND_SASL_SUPPORTED], 1, [Enable sasl code])

  test -z "$PHP_OPENSSL" && PHP_OPENSSL=no

  if test "$PHP_OPENSSL" != "no" || test "$PHP_OPENSSL_DIR" != "no"; then
    PHP_SETUP_OPENSSL(MYSQLND_SHARED_LIBADD, [AC_DEFINE(MYSQLND_HAVE_SSL,1,[Enable mysqlnd code that uses OpenSSL directly])])
  fi

  mysqlnd_sources="$mysqlnd_base_sources $mysqlnd_ps_sources"
  PHP_NEW_EXTENSION(mysqlnd, $mysqlnd_sources, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_ADD_BUILD_DIR([ext/mysqlnd], 1)
  PHP_INSTALL_HEADERS([ext/mysqlnd/])
fi

if test "$PHP_MYSQLND" != "no" || test "$PHP_MYSQLND_ENABLED" = "yes" || test "$PHP_MYSQLI" != "no"; then
  PHP_ADD_BUILD_DIR([ext/mysqlnd], 1)
fi
