/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 3419f4e77bd091e09e0cfc55d81f443d5a3396ff */

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
	ZEND_FALIAS(getallheaders, litespeed_request_headers, arginfo_getallheaders)
	ZEND_FALIAS(apache_request_headers, litespeed_request_headers, arginfo_apache_request_headers)
	ZEND_FE(litespeed_response_headers, arginfo_litespeed_response_headers)
	ZEND_FALIAS(apache_response_headers, litespeed_response_headers, arginfo_apache_response_headers)
	ZEND_FE(apache_get_modules, arginfo_apache_get_modules)
	ZEND_FE(litespeed_finish_request, arginfo_litespeed_finish_request)
	ZEND_FE_END
};
