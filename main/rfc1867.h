#ifndef RFC1867_H
#define RFC1867_H

#include "SAPI.h"

#define MULTIPART_CONTENT_TYPE "multipart/form-data"

SAPI_POST_READER_FUNC(rfc1867_post_reader);
SAPI_POST_HANDLER_FUNC(rfc1867_post_handler);

#define FILE_UPLOAD_INPUT_BUFFER_SIZE 8192

void destroy_uploaded_files_hash(TSRMLS_D);

#endif /* RFC1867_H */
