dnl $Id$
dnl config.m4 for extension soap

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

PHP_ARG_ENABLE(soap, whether to enable soap support,
Make sure that the comment is aligned:
[  --enable-soap[=DIR]      Enable soap support.  DIR is libxml2
                           library directory.])

if test "$PHP_SOAP" != "no"; then
  if test "$PHP_SOAP" = "yes"; then
    for i in /usr/local /usr; do
      if test -d "$i/include/libxml2/libxml"; then
        XML2_INCDIR=$i/include/libxml2
        XML2_LIBDIR=$i/lib
      fi      
    done
  else
    if test -d "$PHP_SOAP/include/libxml2/libxml"; then
      XML2_INCDIR=$PHP_SOAP/include/libxml2
      XML2_LIBDIR=$PHP_SOAP/lib
    fi      
  fi

  if test -z "$XML2_INCDIR"; then
    AC_MSG_ERROR(Cannot find libxml2 header. Please specify correct libxml2 installation path)
  fi    

  AC_DEFINE(HAVE_PHP_SOAP,1,[Whether you have soap module])

  PHP_ADD_INCLUDE($XML2_INCDIR)
  PHP_ADD_LIBRARY_WITH_PATH(xml2,$XML2_LIBDIR,SOAP_SHARED_LIBADD)
  PHP_NEW_EXTENSION(soap, soap.c php_encoding.c php_http.c php_packet_soap.c php_schema.c php_sdl.c php_xml.c, $ext_shared)
fi
