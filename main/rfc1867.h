#ifndef _RFC1867_H
#define _RFC1867_H

#include "SAPI.h"

#define MULTIPART_CONTENT_TYPE "multipart/form-data"

SAPI_POST_READER_FUNC(rfc1867_post_reader);

#define FILE_UPLOAD_INPUT_BUFFER_SIZE 8192

#endif /* _RFC1867_H */
