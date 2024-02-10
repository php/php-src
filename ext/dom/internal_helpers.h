/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef DOM_INTERNAL_HELPERS
#define DOM_INTERNAL_HELPERS

/* We're using the type flags of the zval to store an extra flag. */
#define DOM_Z_OWNED(z, v)   ZVAL_PTR(z, (void *) v)
#define DOM_Z_UNOWNED(z, v) ZVAL_INDIRECT(z, (void *) v)
#define DOM_Z_IS_OWNED(z)   (Z_TYPE_P(z) == IS_PTR)

#ifdef DOM_CE_H

#define DOM_DEF_GET_CE_FUNC(name) \
static zend_always_inline zend_class_entry *dom_get_##name##_ce(bool modern) \
{ \
   return modern ? dom_modern_##name##_class_entry : dom_##name##_class_entry; \
}

static zend_always_inline zend_class_entry *dom_get_html_document_ce(bool modern)
{
	return modern ? dom_html_document_class_entry : dom_document_class_entry;
}

static zend_always_inline zend_class_entry *dom_get_xml_document_ce(bool modern)
{
	return modern ? dom_xml_document_class_entry : dom_document_class_entry;
}

static zend_always_inline zend_class_entry *dom_get_dtd_namednodemap_ce(bool modern)
{
	return modern ? dom_modern_dtd_namednodemap_class_entry : dom_namednodemap_class_entry;
}

DOM_DEF_GET_CE_FUNC(node)
DOM_DEF_GET_CE_FUNC(documenttype)
DOM_DEF_GET_CE_FUNC(element)
DOM_DEF_GET_CE_FUNC(attr)
DOM_DEF_GET_CE_FUNC(entity)
DOM_DEF_GET_CE_FUNC(entityreference)
DOM_DEF_GET_CE_FUNC(processinginstruction)
DOM_DEF_GET_CE_FUNC(comment)
DOM_DEF_GET_CE_FUNC(text)
DOM_DEF_GET_CE_FUNC(cdatasection)
DOM_DEF_GET_CE_FUNC(notation)
DOM_DEF_GET_CE_FUNC(documentfragment)
DOM_DEF_GET_CE_FUNC(namednodemap)
DOM_DEF_GET_CE_FUNC(nodelist)

#endif

#endif
