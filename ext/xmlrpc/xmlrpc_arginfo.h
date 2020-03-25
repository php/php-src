/* This is a generated file, edit the .stub.php file instead. */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlrpc_encode, 0, 1, IS_STRING, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlrpc_decode, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, xml, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlrpc_decode_request, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, xml, IS_STRING, 0)
	ZEND_ARG_INFO(1, method)
	ZEND_ARG_TYPE_INFO(0, encoding, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlrpc_encode_request, 0, 2, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 1)
	ZEND_ARG_INFO(0, params)
	ZEND_ARG_TYPE_INFO(0, output_options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlrpc_set_type, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(1, value)
	ZEND_ARG_TYPE_INFO(0, type, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlrpc_is_fault, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, arg, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlrpc_server_create, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlrpc_server_destroy, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, server)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlrpc_server_register_method, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, server)
	ZEND_ARG_TYPE_INFO(0, method_name, IS_STRING, 0)
	ZEND_ARG_INFO(0, function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlrpc_server_call_method, 0, 0, 3)
	ZEND_ARG_INFO(0, server)
	ZEND_ARG_TYPE_INFO(0, xml, IS_STRING, 0)
	ZEND_ARG_INFO(0, user_data)
	ZEND_ARG_TYPE_INFO(0, output_options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_xmlrpc_parse_method_descriptions, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, xml, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlrpc_server_add_introspection_data, 0, 2, IS_LONG, 0)
	ZEND_ARG_INFO(0, server)
	ZEND_ARG_TYPE_INFO(0, desc, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_xmlrpc_server_register_introspection_callback, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, server)
	ZEND_ARG_INFO(0, function)
ZEND_END_ARG_INFO()
