dnl $Id$
dnl config.m4 for extension soap

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

PHP_ARG_ENABLE(soap, whether to enable soap support,
Make sure that the comment is aligned:
[  --enable-soap           Enable soap support])

if test "$PHP_SOAP" != "no"; then
  PHP_ADD_INCLUDE(/usr/local/include/libxml2)
  PHP_ADD_LIBRARY_WITH_PATH(xml2,/usr/local/lib,SOAP_SHARED_LIBADD)
  PHP_EXTENSION(soap, $ext_shared)
fi
