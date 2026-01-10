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
#define DOM_Z_OWNED(z, v)   ZVAL_PTR(z, (void *) (v))
#define DOM_Z_UNOWNED(z, v) ZVAL_INDIRECT(z, (void *) (v))
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
DOM_DEF_GET_CE_FUNC(domimplementation)

#endif

static zend_always_inline size_t dom_minimum_modification_nr_since_parsing(php_libxml_ref_obj *doc_ptr)
{
	/* For old-style DOM, we need a "new DOMDocument" + a load, so the minimum modification nr is 2.
	 * For new-style DOM, we need only to call a named constructor, so the minimum modification nr is 1. */
	return doc_ptr->class_type == PHP_LIBXML_CLASS_MODERN ? 1 : 2;
}

static zend_always_inline void dom_mark_document_cache_as_modified_since_parsing(php_libxml_ref_obj *doc_ptr)
{
	if (doc_ptr) {
		doc_ptr->cache_tag.modification_nr = MAX(dom_minimum_modification_nr_since_parsing(doc_ptr) + 1,
												 doc_ptr->cache_tag.modification_nr);
	}
}

/* Marks the ID cache as potentially stale */
static zend_always_inline void dom_mark_ids_modified(php_libxml_ref_obj *doc_ptr)
{
	/* Although this is currently a wrapper function, it's best to abstract the actual implementation away. */
	dom_mark_document_cache_as_modified_since_parsing(doc_ptr);
}

static zend_always_inline bool dom_is_document_cache_modified_since_parsing(php_libxml_ref_obj *doc_ptr)
{
	return !doc_ptr || doc_ptr->cache_tag.modification_nr > dom_minimum_modification_nr_since_parsing(doc_ptr);
}

static zend_always_inline zend_long dom_mangle_pointer_for_key(const void *ptr)
{
	zend_ulong value = (zend_ulong) (uintptr_t) ptr;
	/* Rotate 3/4 bits for better hash distribution because the low 3/4 bits are normally 0. */
	const size_t rol_amount = (SIZEOF_ZEND_LONG == 8) ? 4 : 3;
	return (value >> rol_amount) | (value << (sizeof(value) * 8 - rol_amount));
}

#endif
