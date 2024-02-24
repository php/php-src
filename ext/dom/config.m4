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
    PHP_LEXBOR_CFLAGS="-I@ext_srcdir@/lexbor -DLEXBOR_STATIC"
    LEXBOR_DIR="lexbor/lexbor"
    LEXBOR_SOURCES="$LEXBOR_DIR/ports/posix/lexbor/core/memory.c \
      $LEXBOR_DIR/core/array_obj.c $LEXBOR_DIR/core/array.c $LEXBOR_DIR/core/avl.c $LEXBOR_DIR/core/bst.c $LEXBOR_DIR/core/diyfp.c $LEXBOR_DIR/core/conv.c $LEXBOR_DIR/core/dobject.c $LEXBOR_DIR/core/dtoa.c $LEXBOR_DIR/core/hash.c $LEXBOR_DIR/core/mem.c $LEXBOR_DIR/core/mraw.c $LEXBOR_DIR/core/print.c $LEXBOR_DIR/core/serialize.c $LEXBOR_DIR/core/shs.c $LEXBOR_DIR/core/str.c $LEXBOR_DIR/core/strtod.c \
      $LEXBOR_DIR/dom/interface.c $LEXBOR_DIR/dom/interfaces/attr.c $LEXBOR_DIR/dom/interfaces/cdata_section.c $LEXBOR_DIR/dom/interfaces/character_data.c $LEXBOR_DIR/dom/interfaces/comment.c $LEXBOR_DIR/dom/interfaces/document.c $LEXBOR_DIR/dom/interfaces/document_fragment.c $LEXBOR_DIR/dom/interfaces/document_type.c $LEXBOR_DIR/dom/interfaces/element.c $LEXBOR_DIR/dom/interfaces/node.c $LEXBOR_DIR/dom/interfaces/processing_instruction.c $LEXBOR_DIR/dom/interfaces/shadow_root.c $LEXBOR_DIR/dom/interfaces/text.c \
      $LEXBOR_DIR/html/tokenizer/error.c $LEXBOR_DIR/html/tokenizer/state_comment.c $LEXBOR_DIR/html/tokenizer/state_doctype.c $LEXBOR_DIR/html/tokenizer/state_rawtext.c $LEXBOR_DIR/html/tokenizer/state_rcdata.c $LEXBOR_DIR/html/tokenizer/state_script.c $LEXBOR_DIR/html/tokenizer/state.c \
      $LEXBOR_DIR/html/tree/active_formatting.c $LEXBOR_DIR/html/tree/error.c $LEXBOR_DIR/html/tree/insertion_mode/after_after_body.c $LEXBOR_DIR/html/tree/insertion_mode/after_after_frameset.c $LEXBOR_DIR/html/tree/insertion_mode/after_body.c $LEXBOR_DIR/html/tree/insertion_mode/after_frameset.c $LEXBOR_DIR/html/tree/insertion_mode/after_head.c $LEXBOR_DIR/html/tree/insertion_mode/before_head.c $LEXBOR_DIR/html/tree/insertion_mode/before_html.c $LEXBOR_DIR/html/tree/insertion_mode/foreign_content.c $LEXBOR_DIR/html/tree/insertion_mode/in_body.c $LEXBOR_DIR/html/tree/insertion_mode/in_caption.c $LEXBOR_DIR/html/tree/insertion_mode/in_cell.c $LEXBOR_DIR/html/tree/insertion_mode/in_column_group.c $LEXBOR_DIR/html/tree/insertion_mode/in_frameset.c $LEXBOR_DIR/html/tree/insertion_mode/in_head.c $LEXBOR_DIR/html/tree/insertion_mode/in_head_noscript.c $LEXBOR_DIR/html/tree/insertion_mode/initial.c $LEXBOR_DIR/html/tree/insertion_mode/in_row.c $LEXBOR_DIR/html/tree/insertion_mode/in_select.c $LEXBOR_DIR/html/tree/insertion_mode/in_select_in_table.c $LEXBOR_DIR/html/tree/insertion_mode/in_table_body.c $LEXBOR_DIR/html/tree/insertion_mode/in_table.c $LEXBOR_DIR/html/tree/insertion_mode/in_table_text.c $LEXBOR_DIR/html/tree/insertion_mode/in_template.c $LEXBOR_DIR/html/tree/insertion_mode/text.c $LEXBOR_DIR/html/tree/open_elements.c \
      $LEXBOR_DIR/encoding/big5.c $LEXBOR_DIR/encoding/decode.c $LEXBOR_DIR/encoding/encode.c $LEXBOR_DIR/encoding/encoding.c $LEXBOR_DIR/encoding/euc_kr.c $LEXBOR_DIR/encoding/gb18030.c $LEXBOR_DIR/encoding/iso_2022_jp_katakana.c $LEXBOR_DIR/encoding/jis0208.c $LEXBOR_DIR/encoding/jis0212.c $LEXBOR_DIR/encoding/range.c $LEXBOR_DIR/encoding/res.c $LEXBOR_DIR/encoding/single.c \
      $LEXBOR_DIR/html/encoding.c $LEXBOR_DIR/html/interface.c $LEXBOR_DIR/html/parser.c $LEXBOR_DIR/html/token.c $LEXBOR_DIR/html/token_attr.c $LEXBOR_DIR/html/tokenizer.c $LEXBOR_DIR/html/tree.c \
      $LEXBOR_DIR/html/interfaces/anchor_element.c $LEXBOR_DIR/html/interfaces/area_element.c $LEXBOR_DIR/html/interfaces/audio_element.c $LEXBOR_DIR/html/interfaces/base_element.c $LEXBOR_DIR/html/interfaces/body_element.c $LEXBOR_DIR/html/interfaces/br_element.c $LEXBOR_DIR/html/interfaces/button_element.c $LEXBOR_DIR/html/interfaces/canvas_element.c $LEXBOR_DIR/html/interfaces/data_element.c $LEXBOR_DIR/html/interfaces/data_list_element.c $LEXBOR_DIR/html/interfaces/details_element.c $LEXBOR_DIR/html/interfaces/dialog_element.c $LEXBOR_DIR/html/interfaces/directory_element.c $LEXBOR_DIR/html/interfaces/div_element.c $LEXBOR_DIR/html/interfaces/d_list_element.c $LEXBOR_DIR/html/interfaces/document.c $LEXBOR_DIR/html/interfaces/element.c $LEXBOR_DIR/html/interfaces/embed_element.c $LEXBOR_DIR/html/interfaces/field_set_element.c $LEXBOR_DIR/html/interfaces/font_element.c $LEXBOR_DIR/html/interfaces/form_element.c $LEXBOR_DIR/html/interfaces/frame_element.c $LEXBOR_DIR/html/interfaces/frame_set_element.c $LEXBOR_DIR/html/interfaces/head_element.c $LEXBOR_DIR/html/interfaces/heading_element.c $LEXBOR_DIR/html/interfaces/hr_element.c $LEXBOR_DIR/html/interfaces/html_element.c $LEXBOR_DIR/html/interfaces/iframe_element.c $LEXBOR_DIR/html/interfaces/image_element.c $LEXBOR_DIR/html/interfaces/input_element.c $LEXBOR_DIR/html/interfaces/label_element.c $LEXBOR_DIR/html/interfaces/legend_element.c $LEXBOR_DIR/html/interfaces/li_element.c $LEXBOR_DIR/html/interfaces/link_element.c $LEXBOR_DIR/html/interfaces/map_element.c $LEXBOR_DIR/html/interfaces/marquee_element.c $LEXBOR_DIR/html/interfaces/media_element.c $LEXBOR_DIR/html/interfaces/menu_element.c $LEXBOR_DIR/html/interfaces/meta_element.c $LEXBOR_DIR/html/interfaces/meter_element.c $LEXBOR_DIR/html/interfaces/mod_element.c $LEXBOR_DIR/html/interfaces/object_element.c $LEXBOR_DIR/html/interfaces/o_list_element.c $LEXBOR_DIR/html/interfaces/opt_group_element.c $LEXBOR_DIR/html/interfaces/option_element.c $LEXBOR_DIR/html/interfaces/output_element.c $LEXBOR_DIR/html/interfaces/paragraph_element.c $LEXBOR_DIR/html/interfaces/param_element.c $LEXBOR_DIR/html/interfaces/picture_element.c $LEXBOR_DIR/html/interfaces/pre_element.c $LEXBOR_DIR/html/interfaces/progress_element.c $LEXBOR_DIR/html/interfaces/quote_element.c $LEXBOR_DIR/html/interfaces/script_element.c $LEXBOR_DIR/html/interfaces/select_element.c $LEXBOR_DIR/html/interfaces/slot_element.c $LEXBOR_DIR/html/interfaces/source_element.c $LEXBOR_DIR/html/interfaces/span_element.c $LEXBOR_DIR/html/interfaces/style_element.c $LEXBOR_DIR/html/interfaces/table_caption_element.c $LEXBOR_DIR/html/interfaces/table_cell_element.c $LEXBOR_DIR/html/interfaces/table_col_element.c $LEXBOR_DIR/html/interfaces/table_element.c $LEXBOR_DIR/html/interfaces/table_row_element.c $LEXBOR_DIR/html/interfaces/table_section_element.c $LEXBOR_DIR/html/interfaces/template_element.c $LEXBOR_DIR/html/interfaces/text_area_element.c $LEXBOR_DIR/html/interfaces/time_element.c $LEXBOR_DIR/html/interfaces/title_element.c $LEXBOR_DIR/html/interfaces/track_element.c $LEXBOR_DIR/html/interfaces/u_list_element.c $LEXBOR_DIR/html/interfaces/unknown_element.c $LEXBOR_DIR/html/interfaces/video_element.c $LEXBOR_DIR/html/interfaces/window.c \
      $LEXBOR_DIR/selectors/selectors.c \
      $LEXBOR_DIR/ns/ns.c \
      $LEXBOR_DIR/tag/tag.c"
    PHP_NEW_EXTENSION(dom, [php_dom.c attr.c document.c \
                            xml_document.c html_document.c xml_serializer.c html5_serializer.c html5_parser.c namespace_compat.c \
                            domexception.c parentnode.c \
                            processinginstruction.c cdatasection.c \
                            documentfragment.c domimplementation.c \
                            element.c node.c characterdata.c \
                            documenttype.c entity.c \
                            nodelist.c text.c comment.c \
                            entityreference.c \
                            notation.c xpath.c dom_iterators.c \
                            namednodemap.c xpath_callbacks.c \
                            $LEXBOR_SOURCES],
                            $ext_shared,,$PHP_LEXBOR_CFLAGS)
    PHP_ADD_BUILD_DIR($ext_builddir/$LEXBOR_DIR/ports/posix/lexbor/core)
    PHP_ADD_BUILD_DIR($ext_builddir/$LEXBOR_DIR/core)
    PHP_ADD_BUILD_DIR($ext_builddir/$LEXBOR_DIR/dom/interfaces)
    PHP_ADD_BUILD_DIR($ext_builddir/$LEXBOR_DIR/html/tree/insertion_mode)
    PHP_ADD_BUILD_DIR($ext_builddir/$LEXBOR_DIR/html/tokenizer)
    PHP_ADD_BUILD_DIR($ext_builddir/$LEXBOR_DIR/html/interfaces)
    PHP_ADD_BUILD_DIR($ext_builddir/$LEXBOR_DIR/encoding)
    PHP_ADD_BUILD_DIR($ext_builddir/$LEXBOR_DIR/selectors)
    PHP_ADD_BUILD_DIR($ext_builddir/$LEXBOR_DIR/ns)
    PHP_ADD_BUILD_DIR($ext_builddir/$LEXBOR_DIR/tag)
    PHP_SUBST(DOM_SHARED_LIBADD)
    PHP_INSTALL_HEADERS([ext/dom], [xml_common.h xpath_callbacks.h namespace_compat.h])
    PHP_ADD_EXTENSION_DEP(dom, libxml)
  ])
fi
