PHP_ARG_ENABLE([dom],
  [whether to enable DOM support],
  [AS_HELP_STRING([--disable-dom],
    [Disable DOM support])],
  [yes])

if test "$PHP_DOM" != "no"; then
  PHP_SETUP_LIBXML([DOM_SHARED_LIBADD], [
    AC_DEFINE([HAVE_DOM], [1],
      [Define to 1 if the PHP extension 'dom' is available.])
    PHP_NEW_EXTENSION([dom], m4_normalize([
        attr.c
        cdatasection.c
        characterdata.c
        comment.c
        document.c
        documentfragment.c
        documenttype.c
        dom_iterators.c
        domexception.c
        domimplementation.c
        element.c
        entity.c
        entityreference.c
        html_collection.c
        html_document.c
        html5_parser.c
        html5_serializer.c
        infra.c
        inner_outer_html_mixin.c
        namednodemap.c
        namespace_compat.c
        node.c
        nodelist.c
        notation.c
        obj_map.c
        parentnode/css_selectors.c
        parentnode/tree.c
        php_dom.c
        private_data.c
        processinginstruction.c
        text.c
        token_list.c
        xml_document.c
        xml_serializer.c
        xpath_callbacks.c
        xpath.c
        lexbor/selectors-adapted/selectors.c
      ]),
      [$ext_shared],,
      [])
    PHP_ADD_BUILD_DIR([
      $ext_builddir/parentnode
      $ext_builddir/lexbor/selectors-adapted
    ])
    PHP_SUBST([DOM_SHARED_LIBADD])
    PHP_INSTALL_HEADERS([ext/dom], m4_normalize([
      dom_ce.h
      namespace_compat.h
      xml_common.h
      xpath_callbacks.h
    ]))
    PHP_ADD_EXTENSION_DEP(dom, libxml)
    PHP_ADD_EXTENSION_DEP(dom, lexbor)
  ])
fi
