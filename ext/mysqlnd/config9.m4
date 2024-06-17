PHP_ARG_ENABLE([mysqlnd],
  [whether to enable mysqlnd],
  [AS_HELP_STRING([--enable-mysqlnd],
    [Enable mysqlnd explicitly, will be done implicitly when required by other
    extensions])],
  [no],
  [yes])

PHP_ARG_WITH([mysqlnd-ssl],
  [whether to explicitly enable SSL support in mysqlnd],
  [AS_HELP_STRING([--with-mysqlnd-ssl],
    [Explicitly enable SSL support in ext/mysqlnd when not building with
    ext/openssl. If ext/openssl is enabled at the configure step, SSL is enabled
    implicitly.])],
  [no],
  [no])

PHP_ARG_ENABLE([mysqlnd-compression-support],
  [whether to enable compressed protocol support in mysqlnd],
  [AS_HELP_STRING([--disable-mysqlnd-compression-support],
    [Disable support for the MySQL compressed protocol in mysqlnd])],
  [yes],
  [no])

dnl If some extension uses mysqlnd it will get compiled in PHP core
if test "$PHP_MYSQLND" != "no" || test "$PHP_MYSQLND_ENABLED" = "yes"; then
  mysqlnd_ps_sources="mysqlnd_ps.c mysqlnd_ps_codec.c"
  mysqlnd_base_sources="mysqlnd_connection.c mysqlnd_alloc.c mysqlnd_charset.c mysqlnd_wireprotocol.c \
                   mysqlnd_loaddata.c mysqlnd_reverse_api.c mysqlnd_vio.c mysqlnd_protocol_frame_codec.c \
                   mysqlnd_statistics.c mysqlnd_driver.c mysqlnd_ext_plugin.c mysqlnd_auth.c \
				   mysqlnd_result.c mysqlnd_result_meta.c mysqlnd_debug.c mysqlnd_commands.c \
				   mysqlnd_block_alloc.c mysqlnd_read_buffer.c mysqlnd_plugin.c php_mysqlnd.c"

  AS_VAR_IF([PHP_MYSQLND_COMPRESSION_SUPPORT], [no],,
    [PHP_SETUP_ZLIB([MYSQLND_SHARED_LIBADD],
      [AC_DEFINE([MYSQLND_COMPRESSION_ENABLED], [1],
        [Define to 1 if mysqlnd has compressed protocol support.])])])

  AC_DEFINE([MYSQLND_SSL_SUPPORTED], 1, [Enable core mysqlnd SSL code])

  dnl Empty variable means 'no' (for phpize builds).
  test -z "$PHP_OPENSSL" && PHP_OPENSSL=no

  if test "$PHP_OPENSSL" != "no" || test "$PHP_MYSQLND_SSL" != "no"; then
    PHP_SETUP_OPENSSL(MYSQLND_SHARED_LIBADD, [AC_DEFINE(MYSQLND_HAVE_SSL,1,[Enable mysqlnd code that uses OpenSSL directly])])
  fi

  mysqlnd_sources="$mysqlnd_base_sources $mysqlnd_ps_sources"
  PHP_NEW_EXTENSION(mysqlnd, $mysqlnd_sources, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_INSTALL_HEADERS([ext/mysqlnd/])
  PHP_SUBST([MYSQLND_SHARED_LIBADD])
fi
