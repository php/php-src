PHP_ARG_ENABLE([dom],
  [whether to enable DOM support],
  [AS_HELP_STRING([--disable-dom],
    [Disable DOM support])],
  [yes])

if test "$PHP_DOM" != "no"; then

  if test "$PHP_LIBXML" = "no"; then
    AC_MSG_ERROR([DOM extension requires LIBXML extension, add --with-libxml])
  fi

  PHP_SETUP_LIBXML(DOM_SHARED_LIBADD, [
    AC_DEFINE(HAVE_DOM,1,[ ])
    PHP_NEW_EXTENSION(dom, [php_dom.c attr.c document.c \
                            domexception.c parentnode.c \
                            processinginstruction.c cdatasection.c \
                            documentfragment.c domimplementation.c \
                            element.c node.c characterdata.c \
                            documenttype.c entity.c \
                            nodelist.c text.c comment.c \
                            entityreference.c \
                            notation.c xpath.c dom_iterators.c \
                            namednodemap.c],
                            $ext_shared)
    PHP_SUBST(DOM_SHARED_LIBADD)
    PHP_INSTALL_HEADERS([ext/dom/xml_common.h])
    PHP_ADD_EXTENSION_DEP(dom, libxml)
  ])
fi
