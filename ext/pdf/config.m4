dnl
dnl $Id$
dnl

PHP_ARG_WITH(pdflib, whether to include PDFlib support,
[  --with-pdflib     PDFlib support is part of PECL now.])

if test "$PHP_PDFLIB" != "no"; then
  AC_MSG_ERROR([The PDFlib extension is moved to the PECL repository. Please get it from http://pecl.php.net/package/pdflib and build it using PEAR.])
fi
