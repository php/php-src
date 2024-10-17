PHP_ARG_ENABLE([mysqlnd],
  [whether to enable mysqlnd],
  [AS_HELP_STRING([--enable-mysqlnd],
    [Enable mysqlnd explicitly, will be done implicitly when required by other
    extensions])],
  [no],
  [yes])

dnl Empty variable means 'no' (for phpize builds).
AS_VAR_IF([PHP_OPENSSL],, [PHP_OPENSSL=no])

PHP_ARG_WITH([mysqlnd-ssl],
  [whether to enable extended SSL support in mysqlnd],
  [AS_HELP_STRING([--with-mysqlnd-ssl],
    [Explicitly enable extended SSL support in the mysqlnd extension when
    building without openssl extension or when using phpize. If the openssl
    extension is enabled at the configure step (--with-openssl), extended SSL is
    enabled implicitly regardless of this option.])],
  [$PHP_OPENSSL],
  [no])

PHP_ARG_ENABLE([mysqlnd-compression-support],
  [whether to enable compressed protocol support in mysqlnd],
  [AS_HELP_STRING([--disable-mysqlnd-compression-support],
    [Disable support for the MySQL compressed protocol in mysqlnd])],
  [yes],
  [no])

dnl If some extension uses mysqlnd it will get compiled in PHP core
if test "$PHP_MYSQLND" != "no" || test "$PHP_MYSQLND_ENABLED" = "yes"; then
  AS_VAR_IF([PHP_MYSQLND_COMPRESSION_SUPPORT], [no],,
    [PHP_SETUP_ZLIB([MYSQLND_SHARED_LIBADD],
      [AC_DEFINE([MYSQLND_COMPRESSION_ENABLED], [1],
        [Define to 1 if mysqlnd has compressed protocol support.])])])

  AC_DEFINE([MYSQLND_SSL_SUPPORTED], [1],
    [Define to 1 if mysqlnd core SSL is enabled.])

  PHP_NEW_EXTENSION([mysqlnd], m4_normalize([
    mysqlnd_alloc.c
    mysqlnd_auth.c
    mysqlnd_block_alloc.c
    mysqlnd_charset.c
    mysqlnd_commands.c
    mysqlnd_connection.c
    mysqlnd_debug.c
    mysqlnd_driver.c
    mysqlnd_ext_plugin.c
    mysqlnd_loaddata.c
    mysqlnd_plugin.c
    mysqlnd_protocol_frame_codec.c
    mysqlnd_ps_codec.c
    mysqlnd_ps.c
    mysqlnd_read_buffer.c
    mysqlnd_result_meta.c
    mysqlnd_result.c
    mysqlnd_reverse_api.c
    mysqlnd_statistics.c
    mysqlnd_vio.c
    mysqlnd_wireprotocol.c
    php_mysqlnd.c
  ]),
  [$ext_shared],,
  [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])

  AS_VAR_IF([PHP_MYSQLND_SSL], [no],,
    [PHP_SETUP_OPENSSL([MYSQLND_SHARED_LIBADD], [
      AC_DEFINE([MYSQLND_HAVE_SSL], [1],
        [Define to 1 if mysqlnd extended SSL is enabled through a system library.])
      PHP_ADD_EXTENSION_DEP(mysqlnd, hash)
    ])])

  PHP_INSTALL_HEADERS([ext/mysqlnd/])
  PHP_SUBST([MYSQLND_SHARED_LIBADD])
fi
