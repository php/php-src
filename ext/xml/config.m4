dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(xml,whether to enable XML support,
[  --disable-xml           Disable XML support using bundled expat lib], yes)

if test "$PHP_XML" = "yes"; then
	AC_DEFINE(HAVE_XML,  1, [ ])

	if test "$PHP_BUNDLE_EXPAT" = "no" && test "$PHP_BUNDLE_LIBXML" = "no"; then
		AC_MSG_ERROR(xml support is enabled, however both xml libraries have been disabled.)
	fi

	PHP_NEW_EXTENSION(xml, compat.c xml.c, $ext_shared)
fi
