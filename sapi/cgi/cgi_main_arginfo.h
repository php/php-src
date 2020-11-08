/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: ccb32ad91944af15fe1fcb44b97c28a79c1aad86 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_apache_child_terminate, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_apache_request_headers, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_getallheaders arginfo_apache_request_headers

#define arginfo_apache_response_headers arginfo_apache_request_headers


ZEND_FUNCTION(apache_child_terminate);
ZEND_FUNCTION(apache_request_headers);
ZEND_FUNCTION(apache_response_headers);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(apache_child_terminate, arginfo_apache_child_terminate)
	ZEND_FE(apache_request_headers, arginfo_apache_request_headers)
	ZEND_FALIAS(getallheaders, apache_request_headers, arginfo_getallheaders)
	ZEND_FE(apache_response_headers, arginfo_apache_response_headers)
	ZEND_FE_END
};
