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

    if test -z "$XML_INCLUDE" ; then
    if test "$withval" = "yes"; then
      test -d /usr/include/xml && XML_INCLUDE="/usr/include/xml"
      test -d /usr/local/include/xml && XML_INCLUDE="/usr/local/include/xml"
      test -d /usr/include/xmltok && XML_INCLUDE="/usr/include/xmltok"
      AC_CHECK_LIB(expat, main, XML_LIBS="-lexpat", 
	AC_CHECK_LIB(xmltok, main,
	  AC_CHECK_LIB(xmlparse, main, XML_LIBS="-lxmlparse -lxmltok", 
	    AC_MSG_ERROR(No expat library found for the xml module),"-lxmltok"),
	  AC_MSG_ERROR(No expart library found for the xml module))
      )
    else
      XML_LIBS="-L$withval/lib -lexpat"
      if test -d $withval/include/xml; then
	XML_INCLUDE="$withval/include/xml"
      else
	XML_INCLUDE="$withval/include"
      fi
    fi
    fi
    AC_DEFINE(HAVE_LIBEXPAT, 1)
    PHP_EXTENSION(xml, $shared)
    if test "$shared" != "yes"; then
      EXTRA_LIBS="$EXTRA_LIBS $XML_LIBS"
      AC_ADD_INCLUDE($XML_INCLUDE)
      XML_INCLUDE=""
      XML_STATIC="libphpext_xml.a"
    else
      XML_INCLUDE="-I$XML_INCLUDE"
      XML_SHARED="xml.la"
    fi
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
]) 
AC_SUBST(XML_LIBS)
AC_SUBST(XML_INCLUDE)
AC_SUBST(XML_STATIC)
AC_SUBST(XML_SHARED)
