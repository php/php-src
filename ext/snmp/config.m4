PHP_ARG_WITH([snmp],
  [for SNMP support],
  [AS_HELP_STRING([--with-snmp],
    [Include SNMP support])])

if test "$PHP_SNMP" != "no"; then
  dnl 5.4 includes shutdown_snmp_logging.
  dnl We used to check for SHA-256, but since PHP needs OpenSSL 1.1.1,
  dnl and those algorithms' API are always available since then...
  PKG_CHECK_MODULES([SNMP], [netsnmp >= 5.4])

  PHP_EVAL_INCLINE([$SNMP_CFLAGS])
  PHP_EVAL_LIBLINE([$SNMP_LIBS], [SNMP_SHARED_LIBADD])

  AC_DEFINE(HAVE_SNMP,1,[ ])

  PHP_NEW_EXTENSION(snmp, snmp.c, $ext_shared)
  PHP_SUBST([SNMP_SHARED_LIBADD])
fi
