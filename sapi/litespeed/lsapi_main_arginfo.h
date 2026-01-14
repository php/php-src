/* This is a generated file, edit lsapi_main.stub.php instead.
 * Stub hash: 582b137a2788ae1b15853c4b78da551419390da9 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_litespeed_request_headers, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_getallheaders arginfo_litespeed_request_headers

#define arginfo_apache_request_headers arginfo_litespeed_request_headers

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_litespeed_response_headers, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_apache_response_headers arginfo_litespeed_response_headers

#define arginfo_apache_get_modules arginfo_litespeed_request_headers

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_litespeed_finish_request, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(litespeed_request_headers);
ZEND_FUNCTION(litespeed_response_headers);
ZEND_FUNCTION(apache_get_modules);
ZEND_FUNCTION(litespeed_finish_request);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(litespeed_request_headers, arginfo_litespeed_request_headers)
	ZEND_RAW_FENTRY("getallheaders", zif_litespeed_request_headers, arginfo_getallheaders, 0, NULL, NULL)
	ZEND_RAW_FENTRY("apache_request_headers", zif_litespeed_request_headers, arginfo_apache_request_headers, 0, NULL, NULL)
	ZEND_FE(litespeed_response_headers, arginfo_litespeed_response_headers)
	ZEND_RAW_FENTRY("apache_response_headers", zif_litespeed_response_headers, arginfo_apache_response_headers, 0, NULL, NULL)
	ZEND_FE(apache_get_modules, arginfo_apache_get_modules)
	ZEND_FE(litespeed_finish_request, arginfo_litespeed_finish_request)
	ZEND_FE_END
};
