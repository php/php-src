/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_simplexml_load_file, 0, 1, SimpleXMLElement, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, ns, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, is_prefix, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_simplexml_load_string, 0, 1, SimpleXMLElement, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, ns, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, is_prefix, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_simplexml_import_dom, 0, 1, SimpleXMLElement, 1)
	ZEND_ARG_OBJ_INFO(0, node, DOMNode, 0)
	ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SimpleXMLElement_xpath, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SimpleXMLElement_registerXPathNamespace, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ns, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SimpleXMLElement_asXML, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SimpleXMLElement_saveXML arginfo_class_SimpleXMLElement_asXML

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SimpleXMLElement_getNamespaces, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, recursive, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SimpleXMLElement_getDocNamespaces, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, recursive, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, from_root, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SimpleXMLElement_children, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, ns, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, is_prefix, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SimpleXMLElement_attributes arginfo_class_SimpleXMLElement_children

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SimpleXMLElement___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data_is_url, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, ns, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, is_prefix, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SimpleXMLElement_addChild, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, ns, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_SimpleXMLElement_addAttribute arginfo_class_SimpleXMLElement_addChild

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SimpleXMLElement_getName, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SimpleXMLElement___toString arginfo_class_SimpleXMLElement_getName

#define arginfo_class_SimpleXMLElement_count arginfo_class_SimpleXMLElement_getName
