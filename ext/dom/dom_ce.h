/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#ifndef DOM_CE_H
#define DOM_CE_H

extern PHP_DOM_EXPORT zend_class_entry *dom_node_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_domexception_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_domimplementation_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_documentfragment_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_document_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_nodelist_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_namednodemap_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_characterdata_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_attr_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_element_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_text_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_comment_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_cdatasection_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_documenttype_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_notation_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_entity_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_entityreference_class_entry;
extern PHP_DOM_EXPORT zend_class_entry *dom_processinginstruction_class_entry;
#ifdef LIBXML_XPATH_ENABLED
extern PHP_DOM_EXPORT zend_class_entry *dom_xpath_class_entry;
#endif
extern PHP_DOM_EXPORT zend_class_entry *dom_namespace_node_class_entry;

#endif /* DOM_CE_H */
