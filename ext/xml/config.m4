# $Source$
# $Id$

AC_MSG_CHECKING(for XML support)
AC_ARG_WITH(xml,
[  --with-xml[=DIR]        Include XML support.  Will look for expat
                          in DIR if specified.  Set DIR to "shared" to
                          build as a dl, or "shared,DIR" to build as a dl
                          and still specify DIR.],[
  case $withval in
    shared)
      shared=yes
      withval=yes
      ;;
    shared,*)
      shared=yes
      withval=`echo $withval | sed -e 's/^shared,//'`
      ;;
    *)
      shared=no
      ;;
  esac
  if test "$withval" != "no"; then
    if test "$shared" = "yes"; then
      AC_MSG_RESULT([yes (shared)])
    else
      AC_MSG_RESULT([yes (static)])
    fi
    if test "$withval" = "yes"; then
      test -d /usr/include/xmltok && XML_INCLUDE="-I/usr/include/xmltok"
      test -d /usr/include/xml && XML_INCLUDE="-I/usr/include/xml"
      test -d /usr/local/include/xml && XML_INCLUDE="-I/usr/local/include/xml"
      AC_CHECK_LIB(expat, main, XML_LIBS="-lexpat", XML_LIBS="-lxmlparse -lxmltok")
    else
      XML_LIBS="-L$withval/lib -lexpat"
      if test -d $withval/include/xml; then
	XML_INCLUDE="$withval/include/xml"
      else
	XML_INCLUDE="$withval/include"
      fi
    fi
    AC_DEFINE(HAVE_LIBEXPAT, 1)
    AC_ADD_INCLUDE($XML_INCLUDE)
    PHP_EXTENSION(xml, $shared)
    if test "$shared" != "yes"; then
      EXTRA_LIBS="$EXTRA_LIBS $XML_LIBS"
    fi
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
]) 
AC_SUBST(XML_LIBS)
