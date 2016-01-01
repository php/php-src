/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */
#ifndef DOM_CE_H
#define DOM_CE_H

extern zend_class_entry *dom_domexception_class_entry;
extern zend_class_entry *dom_domstringlist_class_entry;
extern zend_class_entry *dom_namelist_class_entry;
extern zend_class_entry *dom_domimplementationlist_class_entry;
extern zend_class_entry *dom_domimplementationsource_class_entry;
extern zend_class_entry *dom_domimplementation_class_entry;
extern zend_class_entry *dom_documentfragment_class_entry;
extern zend_class_entry *dom_document_class_entry;
extern zend_class_entry *dom_nodelist_class_entry;
extern zend_class_entry *dom_namednodemap_class_entry;
extern zend_class_entry *dom_characterdata_class_entry;
extern zend_class_entry *dom_attr_class_entry;
extern zend_class_entry *dom_element_class_entry;
extern zend_class_entry *dom_text_class_entry;
extern zend_class_entry *dom_comment_class_entry;
extern zend_class_entry *dom_typeinfo_class_entry;
extern zend_class_entry *dom_userdatahandler_class_entry;
extern zend_class_entry *dom_domerror_class_entry;
extern zend_class_entry *dom_domerrorhandler_class_entry;
extern zend_class_entry *dom_domlocator_class_entry;
extern zend_class_entry *dom_domconfiguration_class_entry;
extern zend_class_entry *dom_cdatasection_class_entry;
extern zend_class_entry *dom_documenttype_class_entry;
extern zend_class_entry *dom_notation_class_entry;
extern zend_class_entry *dom_entity_class_entry;
extern zend_class_entry *dom_entityreference_class_entry;
extern zend_class_entry *dom_processinginstruction_class_entry;
extern zend_class_entry *dom_string_extend_class_entry;
#if defined(LIBXML_XPATH_ENABLED)
extern zend_class_entry *dom_xpath_class_entry;
#endif
extern zend_class_entry *dom_namespace_node_class_entry;

#endif /* DOM_CE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
