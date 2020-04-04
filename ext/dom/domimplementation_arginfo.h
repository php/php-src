/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMImplementation_getFeature, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, feature, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, version, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_DOMImplementation_hasFeature arginfo_class_DOMImplementation_getFeature

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMImplementation_createDocumentType, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, publicId, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, systemId, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DOMImplementation_createDocument, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, namespaceURI, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, qualifiedName, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, doctype, DOMDocumentType, 0)
ZEND_END_ARG_INFO()
