dnl config.m4 for extension dom

PHP_ARG_ENABLE(dom, whether to enable DOM support,
[  --disable-dom           Disable DOM support], yes)

if test -z "$PHP_LIBXML_DIR"; then
  PHP_ARG_WITH(libxml-dir, libxml2 install dir,
  [  --with-libxml-dir[=DIR]   DOM: libxml2 install prefix], no, no)
fi

if test "$PHP_DOM" != "no"; then

  if test "$PHP_LIBXML" = "no"; then
    AC_MSG_ERROR([DOM extension requires LIBXML extension, add --enable-libxml])
  fi

  PHP_SETUP_LIBXML(DOM_SHARED_LIBADD, [
    AC_DEFINE(HAVE_DOM,1,[ ])
    PHP_NEW_EXTENSION(dom, [php_dom.c attr.c document.c domerrorhandler.c \
                            domstringlist.c domexception.c namelist.c \
                            processinginstruction.c cdatasection.c \
                            documentfragment.c domimplementation.c \
                            element.c node.c string_extend.c characterdata.c \
                            documenttype.c domimplementationlist.c entity.c \
                            nodelist.c text.c comment.c domconfiguration.c \
                            domimplementationsource.c entityreference.c \
                            notation.c xpath.c dom_iterators.c \
                            typeinfo.c domerror.c domlocator.c namednodemap.c userdatahandler.c],
                            $ext_shared)
    PHP_SUBST(DOM_SHARED_LIBADD)
    PHP_INSTALL_HEADERS([ext/dom/xml_common.h])
    PHP_ADD_EXTENSION_DEP(dom, libxml)
  ], [
    AC_MSG_ERROR([libxml2 not found. Please check your libxml2 installation.])
  ])
fi
