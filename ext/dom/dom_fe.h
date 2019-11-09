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

#ifndef DOM_FE_H
#define DOM_FE_H

extern const zend_function_entry php_dom_domexception_class_functions[];
extern const zend_function_entry php_dom_domimplementation_class_functions[];
extern const zend_function_entry php_dom_documentfragment_class_functions[];
extern const zend_function_entry php_dom_document_class_functions[];
extern const zend_function_entry php_dom_node_class_functions[];
extern const zend_function_entry php_dom_nodelist_class_functions[];
extern const zend_function_entry php_dom_namednodemap_class_functions[];
extern const zend_function_entry php_dom_characterdata_class_functions[];
extern const zend_function_entry php_dom_attr_class_functions[];
extern const zend_function_entry php_dom_element_class_functions[];
extern const zend_function_entry php_dom_text_class_functions[];
extern const zend_function_entry php_dom_comment_class_functions[];
extern const zend_function_entry php_dom_cdatasection_class_functions[];
extern const zend_function_entry php_dom_documenttype_class_functions[];
extern const zend_function_entry php_dom_notation_class_functions[];
extern const zend_function_entry php_dom_entity_class_functions[];
extern const zend_function_entry php_dom_entityreference_class_functions[];
extern const zend_function_entry php_dom_processinginstruction_class_functions[];
extern const zend_function_entry php_dom_xpath_class_functions[];

/* domexception errors */
typedef enum {
/* PHP_ERR is non-spec code for PHP errors: */
	PHP_ERR                        = 0,
	INDEX_SIZE_ERR                 = 1,
	DOMSTRING_SIZE_ERR             = 2,
	HIERARCHY_REQUEST_ERR          = 3,
	WRONG_DOCUMENT_ERR             = 4,
	INVALID_CHARACTER_ERR          = 5,
	NO_DATA_ALLOWED_ERR            = 6,
	NO_MODIFICATION_ALLOWED_ERR    = 7,
	NOT_FOUND_ERR                  = 8,
	NOT_SUPPORTED_ERR              = 9,
	INUSE_ATTRIBUTE_ERR            = 10,
/* Introduced in DOM Level 2: */
	INVALID_STATE_ERR              = 11,
/* Introduced in DOM Level 2: */
	SYNTAX_ERR                     = 12,
/* Introduced in DOM Level 2: */
	INVALID_MODIFICATION_ERR       = 13,
/* Introduced in DOM Level 2: */
	NAMESPACE_ERR                  = 14,
/* Introduced in DOM Level 2: */
	INVALID_ACCESS_ERR             = 15,
/* Introduced in DOM Level 3: */
	VALIDATION_ERR                 = 16
} dom_exception_code;

/* domimplementation methods */
PHP_METHOD(domimplementation, hasFeature);
PHP_METHOD(domimplementation, createDocumentType);
PHP_METHOD(domimplementation, createDocument);
PHP_METHOD(domimplementation, getFeature);

/* domdocumentfragment methods */
PHP_METHOD(domdocumentfragment, __construct);
PHP_METHOD(domdocumentfragment, appendXML);

/* domdocument methods */
PHP_METHOD(domdocument, createElement);
PHP_METHOD(domdocument, createDocumentFragment);
PHP_METHOD(domdocument, createTextNode);
PHP_METHOD(domdocument, createComment);
PHP_METHOD(domdocument, createCDATASection);
PHP_METHOD(domdocument, createProcessingInstruction);
PHP_METHOD(domdocument, createAttribute);
PHP_METHOD(domdocument, createEntityReference);
PHP_METHOD(domdocument, getElementsByTagName);
PHP_METHOD(domdocument, importNode);
PHP_METHOD(domdocument, createElementNS);
PHP_METHOD(domdocument, createAttributeNS);
PHP_METHOD(domdocument, getElementsByTagNameNS);
PHP_METHOD(domdocument, getElementById);
PHP_METHOD(domdocument, adoptNode);
PHP_METHOD(domdocument, normalizeDocument);
PHP_METHOD(domdocument, __construct);
	/* convienience methods */
PHP_METHOD(domdocument, load);
PHP_METHOD(domdocument, save);
PHP_METHOD(domdocument, loadXML);
PHP_METHOD(domdocument, saveXML);
PHP_METHOD(domdocument, validate);
PHP_METHOD(domdocument, xinclude);
PHP_METHOD(domdocument, registerNodeClass);

#if defined(LIBXML_HTML_ENABLED)
PHP_METHOD(domdocument, loadHTML);
PHP_METHOD(domdocument, loadHTMLFile);
PHP_METHOD(domdocument, saveHTML);
PHP_METHOD(domdocument, saveHTMLFile);
#endif  /* defined(LIBXML_HTML_ENABLED) */

#if defined(LIBXML_SCHEMAS_ENABLED)
PHP_METHOD(domdocument, schemaValidate);
PHP_METHOD(domdocument, schemaValidateSource);
PHP_METHOD(domdocument, relaxNGValidate);
PHP_METHOD(domdocument, relaxNGValidateSource);
#endif

/* domnode methods */
PHP_METHOD(domnode, insertBefore);
PHP_METHOD(domnode, replaceChild);
PHP_METHOD(domnode, removeChild);
PHP_METHOD(domnode, appendChild);
PHP_METHOD(domnode, hasChildNodes);
PHP_METHOD(domnode, cloneNode);
PHP_METHOD(domnode, normalize);
PHP_METHOD(domnode, isSupported);
PHP_METHOD(domnode, hasAttributes);
PHP_METHOD(domnode, isSameNode);
PHP_METHOD(domnode, lookupPrefix);
PHP_METHOD(domnode, isDefaultNamespace);
PHP_METHOD(domnode, lookupNamespaceURI);
PHP_METHOD(domnode, C14N);
PHP_METHOD(domnode, C14NFile);
PHP_METHOD(domnode, getNodePath);
PHP_METHOD(domnode, getLineNo);

/* domnodelist methods */
PHP_METHOD(domnodelist, item);
PHP_METHOD(domnodelist, count);

/* domnamednodemap methods */
PHP_METHOD(domnamednodemap, getNamedItem);
PHP_METHOD(domnamednodemap, item);
PHP_METHOD(domnamednodemap, getNamedItemNS);
PHP_METHOD(domnamednodemap, count);

/* domcharacterdata methods */
PHP_METHOD(domcharacterdata, substringData);
PHP_METHOD(domcharacterdata, appendData);
PHP_METHOD(domcharacterdata, insertData);
PHP_METHOD(domcharacterdata, deleteData);
PHP_METHOD(domcharacterdata, replaceData);

/* domattr methods */
PHP_METHOD(domattr, isId);
PHP_METHOD(domattr, __construct);

/* domelement methods */
PHP_METHOD(domelement, getAttribute);
PHP_METHOD(domelement, setAttribute);
PHP_METHOD(domelement, removeAttribute);
PHP_METHOD(domelement, getAttributeNode);
PHP_METHOD(domelement, setAttributeNode);
PHP_METHOD(domelement, removeAttributeNode);
PHP_METHOD(domelement, getElementsByTagName);
PHP_METHOD(domelement, getAttributeNS);
PHP_METHOD(domelement, setAttributeNS);
PHP_METHOD(domelement, removeAttributeNS);
PHP_METHOD(domelement, getAttributeNodeNS);
PHP_METHOD(domelement, setAttributeNodeNS);
PHP_METHOD(domelement, getElementsByTagNameNS);
PHP_METHOD(domelement, hasAttribute);
PHP_METHOD(domelement, hasAttributeNS);
PHP_METHOD(domelement, setIdAttribute);
PHP_METHOD(domelement, setIdAttributeNS);
PHP_METHOD(domelement, setIdAttributeNode);
PHP_METHOD(domelement, __construct);

/* domtext methods */
PHP_METHOD(domtext, splitText);
PHP_METHOD(domtext, isWhitespaceInElementContent);
PHP_METHOD(domtext, replaceWholeText);
PHP_METHOD(domtext, __construct);

/* domcomment methods */
PHP_METHOD(domcomment, __construct);

/* domcdatasection methods */
PHP_METHOD(domcdatasection, __construct);

/* domdocumenttype methods */

/* domnotation methods */

/* domentity methods */

/* domentityreference methods */
PHP_METHOD(domentityreference, __construct);

/* domprocessinginstruction methods */
PHP_METHOD(domprocessinginstruction, __construct);

#if defined(LIBXML_XPATH_ENABLED)
/* xpath methods */
PHP_METHOD(domxpath, __construct);
PHP_METHOD(domxpath, registerNamespace);
PHP_METHOD(domxpath, query);
PHP_METHOD(domxpath, evaluate);
PHP_METHOD(domxpath, registerPhpFunctions);
#endif

#endif /* DOM_FE_H */
