#include "rfc1867.h"

SAPI_POST_READER_FUNC(rfc1867_post_reader)
{
	char *boundary;
	uint boundary_len;
	char input_buffer[FILE_UPLOAD_INPUT_BUFFER_SIZE];
	uint read_bytes;

	sapi_module.sapi_error(E_COMPILE_ERROR, "File upload is not yet supported");
	return;

	boundary = strstr(content_type_dup, "boundary");
	if (!boundary || !(boundary=strchr(boundary, '='))) {
		sapi_module.sapi_error(E_COMPILE_ERROR, "Missing boundary in multipart/form-data POST data");
		return;
	}
	boundary++;
	boundary_len = strlen(boundary);

	for (;;) {
		read_bytes = sapi_module.read_post(input_buffer, FILE_UPLOAD_INPUT_BUFFER_SIZE-100 SLS_CC);
		if (read_bytes<=0) {
			break;
		}
	}
}
