#ifndef RFC1867_H
#define RFC1867_H

#include "SAPI.h"

#define MULTIPART_CONTENT_TYPE "multipart/form-data"

SAPI_API SAPI_POST_HANDLER_FUNC(rfc1867_post_handler);

void destroy_uploaded_files_hash(TSRMLS_D);
void php_rfc1867_register_constants(TSRMLS_D);

#endif /* RFC1867_H */
