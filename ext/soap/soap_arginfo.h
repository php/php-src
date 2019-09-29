/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_use_soap_error_handler, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, handler, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_is_soap_fault, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapParam___construct, 0, 0, 2)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapHeader___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, namespace, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_TYPE_INFO(0, mustunderstand, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, actor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapFault___construct, 0, 0, 2)
	ZEND_ARG_INFO(0, faultcode)
	ZEND_ARG_TYPE_INFO(0, faultstring, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, faultactor, IS_STRING, 1)
	ZEND_ARG_INFO(0, detail)
	ZEND_ARG_TYPE_INFO(0, faultname, IS_STRING, 1)
	ZEND_ARG_INFO(0, headerfault)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapFault___toString, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapVar___construct, 0, 0, 2)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_TYPE_INFO(0, type_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, type_namespace, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, node_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, node_namespace, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapServer___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, wsdl)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapServer_fault, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, code, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, string, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, actor, IS_STRING, 0)
	ZEND_ARG_INFO(0, details)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapServer_addSoapHeader, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, object, SoapHeader, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapServer_setPersistence, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, mode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapServer_setClass, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 0)
	ZEND_ARG_VARIADIC_INFO(0, argv)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapServer_setObject, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SoapServer_getFunctions arginfo_class_SoapFault___toString

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapServer_addFunction, 0, 0, 1)
	ZEND_ARG_INFO(0, functions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapServer_handle, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, soap_request, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_SoapClient___construct arginfo_class_SoapServer___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapClient___call, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, function_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, arguments, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapClient___soapCall, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, function_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, arguments, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 1)
	ZEND_ARG_INFO(0, input_headers)
	ZEND_ARG_INFO(0, output_headers)
ZEND_END_ARG_INFO()

#define arginfo_class_SoapClient___getFunctions arginfo_class_SoapFault___toString

#define arginfo_class_SoapClient___getTypes arginfo_class_SoapFault___toString

#define arginfo_class_SoapClient___getLastRequest arginfo_class_SoapFault___toString

#define arginfo_class_SoapClient___getLastResponse arginfo_class_SoapFault___toString

#define arginfo_class_SoapClient___getLastRequestHeaders arginfo_class_SoapFault___toString

#define arginfo_class_SoapClient___getLastResponseHeaders arginfo_class_SoapFault___toString

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapClient___doRequest, 0, 0, 4)
	ZEND_ARG_TYPE_INFO(0, request, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, location, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, action, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, version, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, one_way, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapClient___setCookie, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_SoapClient___getCookies arginfo_class_SoapFault___toString

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapClient___setSoapHeaders, 0, 0, 0)
	ZEND_ARG_INFO(0, soapheaders)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_SoapClient___setLocation, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, new_location, IS_STRING, 0)
ZEND_END_ARG_INFO()
