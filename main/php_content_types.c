#include "php.h"
#include "SAPI.h"
#include "rfc1867.h"

static sapi_post_content_type_reader php_post_content_types[] = {
	{ MULTIPART_CONTENT_TYPE,		sizeof(MULTIPART_CONTENT_TYPE)-1,		rfc1867_post_reader	},
	{ NULL, 0, NULL }
}

int php_startup_SAPI_content_types()
{
	sapi_register_post_readers(php_post_content_types);
	return SUCCESS;
}
