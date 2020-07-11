/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 7b3ff8b991fc7e424aaf1e86cfbebe662a30c48f */

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_simplexml_load_file, 0, 1, SimpleXMLElement, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class_name, IS_STRING, 1, "SimpleXMLElement::class")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ns, IS_STRING, 0, "\'\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, is_prefix, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_simplexml_load_string, 0, 1, SimpleXMLElement, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class_name, IS_STRING, 1, "SimpleXMLElement::class")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ns, IS_STRING, 0, "\'\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, is_prefix, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_simplexml_import_dom, 0, 1, SimpleXMLElement, 1)
	ZEND_ARG_OBJ_INFO(0, node, DOMNode, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, class_name, IS_STRING, 1, "SimpleXMLElement::class")
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
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, recursive, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SimpleXMLElement_getDocNamespaces, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, recursive, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, from_root, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SimpleXMLElement_children, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ns, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, is_prefix, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_class_SimpleXMLElement_attributes arginfo_class_SimpleXMLElement_children

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SimpleXMLElement___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, data_is_url, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ns, IS_STRING, 0, "\'\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, is_prefix, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SimpleXMLElement_addChild, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ns, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_SimpleXMLElement_addAttribute arginfo_class_SimpleXMLElement_addChild

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SimpleXMLElement_getName, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_SimpleXMLElement___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SimpleXMLElement_count arginfo_class_SimpleXMLElement_getName

#define arginfo_class_SimpleXMLElement_rewind arginfo_class_SimpleXMLElement_getName

#define arginfo_class_SimpleXMLElement_valid arginfo_class_SimpleXMLElement_getName

#define arginfo_class_SimpleXMLElement_current arginfo_class_SimpleXMLElement_getName

#define arginfo_class_SimpleXMLElement_key arginfo_class_SimpleXMLElement_getName

#define arginfo_class_SimpleXMLElement_next arginfo_class_SimpleXMLElement_getName

#define arginfo_class_SimpleXMLElement_hasChildren arginfo_class_SimpleXMLElement_getName

#define arginfo_class_SimpleXMLElement_getChildren arginfo_class_SimpleXMLElement_getName


ZEND_FUNCTION(simplexml_load_file);
ZEND_FUNCTION(simplexml_load_string);
ZEND_FUNCTION(simplexml_import_dom);
ZEND_METHOD(SimpleXMLElement, xpath);
ZEND_METHOD(SimpleXMLElement, registerXPathNamespace);
ZEND_METHOD(SimpleXMLElement, asXML);
ZEND_METHOD(SimpleXMLElement, getNamespaces);
ZEND_METHOD(SimpleXMLElement, getDocNamespaces);
ZEND_METHOD(SimpleXMLElement, children);
ZEND_METHOD(SimpleXMLElement, attributes);
ZEND_METHOD(SimpleXMLElement, __construct);
ZEND_METHOD(SimpleXMLElement, addChild);
ZEND_METHOD(SimpleXMLElement, addAttribute);
ZEND_METHOD(SimpleXMLElement, getName);
ZEND_METHOD(SimpleXMLElement, __toString);
ZEND_METHOD(SimpleXMLElement, count);
ZEND_METHOD(SimpleXMLElement, rewind);
ZEND_METHOD(SimpleXMLElement, valid);
ZEND_METHOD(SimpleXMLElement, current);
ZEND_METHOD(SimpleXMLElement, key);
ZEND_METHOD(SimpleXMLElement, next);
ZEND_METHOD(SimpleXMLElement, hasChildren);
ZEND_METHOD(SimpleXMLElement, getChildren);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(simplexml_load_file, arginfo_simplexml_load_file)
	ZEND_FE(simplexml_load_string, arginfo_simplexml_load_string)
	ZEND_FE(simplexml_import_dom, arginfo_simplexml_import_dom)
	ZEND_FE_END
};


static const zend_function_entry class_SimpleXMLElement_methods[] = {
	ZEND_ME(SimpleXMLElement, xpath, arginfo_class_SimpleXMLElement_xpath, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, registerXPathNamespace, arginfo_class_SimpleXMLElement_registerXPathNamespace, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, asXML, arginfo_class_SimpleXMLElement_asXML, ZEND_ACC_PUBLIC)
	ZEND_MALIAS(SimpleXMLElement, saveXML, asXML, arginfo_class_SimpleXMLElement_saveXML, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, getNamespaces, arginfo_class_SimpleXMLElement_getNamespaces, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, getDocNamespaces, arginfo_class_SimpleXMLElement_getDocNamespaces, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, children, arginfo_class_SimpleXMLElement_children, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, attributes, arginfo_class_SimpleXMLElement_attributes, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, __construct, arginfo_class_SimpleXMLElement___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, addChild, arginfo_class_SimpleXMLElement_addChild, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, addAttribute, arginfo_class_SimpleXMLElement_addAttribute, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, getName, arginfo_class_SimpleXMLElement_getName, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, __toString, arginfo_class_SimpleXMLElement___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, count, arginfo_class_SimpleXMLElement_count, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, rewind, arginfo_class_SimpleXMLElement_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, valid, arginfo_class_SimpleXMLElement_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, current, arginfo_class_SimpleXMLElement_current, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, key, arginfo_class_SimpleXMLElement_key, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, next, arginfo_class_SimpleXMLElement_next, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, hasChildren, arginfo_class_SimpleXMLElement_hasChildren, ZEND_ACC_PUBLIC)
	ZEND_ME(SimpleXMLElement, getChildren, arginfo_class_SimpleXMLElement_getChildren, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_SimpleXMLIterator_methods[] = {
	ZEND_FE_END
};
