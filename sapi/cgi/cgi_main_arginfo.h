/* This is a generated file, edit cgi_main.stub.php instead.
 * Stub hash: d12211af01f723fc4ee7b24fd6d43205521cbf66 */

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
	ZEND_RAW_FENTRY("getallheaders", zif_apache_request_headers, arginfo_getallheaders, 0, NULL, NULL)
	ZEND_FE(apache_response_headers, arginfo_apache_response_headers)
	ZEND_FE_END
};
