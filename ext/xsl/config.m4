PHP_ARG_WITH([xsl],
  [whether to build with XSL support],
  [AS_HELP_STRING([--with-xsl],
    [Build with XSL support])])

if test "$PHP_XSL" != "no"; then
  PKG_CHECK_MODULES([XSL], [libxslt >= 1.1.0])

  PHP_EVAL_INCLINE([$XSL_CFLAGS])
  PHP_EVAL_LIBLINE([$XSL_LIBS], [XSL_SHARED_LIBADD])

  PKG_CHECK_MODULES([EXSLT], [libexslt],
  [
    PHP_EVAL_INCLINE([$EXSLT_CFLAGS])
    PHP_EVAL_LIBLINE([$EXSLT_LIBS], [XSL_SHARED_LIBADD])
    AC_DEFINE([HAVE_XSL_EXSLT], [1],
      [Define to 1 if the system has the EXSLT extension library for XSLT.])
  ], [:])

  AC_DEFINE([HAVE_XSL], [1],
    [Define to 1 if the PHP extension 'xsl' is available.])
  PHP_NEW_EXTENSION([xsl], [php_xsl.c xsltprocessor.c], [$ext_shared])
  PHP_SUBST([XSL_SHARED_LIBADD])
  PHP_ADD_EXTENSION_DEP(xsl, libxml)
  PHP_ADD_EXTENSION_DEP(xsl, dom)
fi
