#include "php_apache.h"

PHP_FUNCTION(apache_sub_req)
{
	zval **p1;
	request_req *rr;
	php_struct *ctx;
	SLS_FETCH();
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &p1) == FAILURE)
		WRONG_NUM_ARGS;

	convert_to_string_ex(p1);

	ctx = SG(server_context);
	rr = ap_sub_req_lookup_uri(Z_STRVAL_PP(p1), ctx->f->r);
	if (rr->status == HTTP_OK) {
		ap_run_sub_req(rr);
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

