#include "php.h"
#include "SAPI.h"
#include "rfc1867.h"

#include "php_content_types.h"

static sapi_post_content_type_reader php_post_content_types[] = {
	{ MULTIPART_CONTENT_TYPE,		sizeof(MULTIPART_CONTENT_TYPE)-1,		rfc1867_post_reader	},
	{ NULL, 0, NULL }
};


SAPI_POST_READER_FUNC(php_default_post_reader)
{
	char *data;
	ELS_FETCH();

	sapi_read_standard_form_data(content_type_dup SLS_CC);
	data = estrndup(SG(request_info).post_data,SG(request_info).post_data_length);
	SET_VAR_STRINGL("HTTP_RAW_POST_DATA", data, SG(request_info).post_data_length);
}


int php_startup_SAPI_content_types(void)
{
	sapi_register_post_readers(php_post_content_types);
	sapi_register_default_post_reader(php_default_post_reader);
	return SUCCESS;
}
