# $Source$
# $Id$

AC_MSG_CHECKING(for XML support)
AC_ARG_WITH(xml,
[  --with-xml              Include XML support],[
  if test "$withval" != "no"; then
    if test "$withval" = "yes"; then
      test -d /usr/include/xmltok && XML_INCLUDE="-I/usr/include/xmltok"
      test -d /usr/include/xml && XML_INCLUDE="-I/usr/include/xml"
      test -d /usr/local/include/xml && XML_INCLUDE="-I/usr/local/include/xml"
      test -d /usr/include/xml && XML_INCLUDE="-I/usr/include/xml"
      XML_INCLUDE="-I/usr/include/xml"
      AC_CHECK_LIB(expat, main, XML_LIBS="-lexpat", XML_LIBS="-lxmlparse -lxmltok")
    else
      XML_LIBS="-L$withval/lib -lexpat"
      if test -d $withval/include/xml; then
	XML_INCLUDE="-I$withval/include/xml"
      else
	XML_INCLUDE="-I$withval/include"
      fi
    fi
    AC_DEFINE(HAVE_LIBEXPAT, 1)
    AC_MSG_RESULT(yes)
    PHP_EXTENSION(xml)
    EXTRA_LIBS="$EXTRA_LIBS $XML_LIBS"
    INCLUDES="$INCLUDES $XML_INCLUDE"
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
]) 
