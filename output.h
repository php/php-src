#ifndef _OUTPUT_BUFFER
#define _OUTPUT_BUFFER

#include "php.h"

PHPAPI void zend_output_startup();

/* exported output functions */
int (*zend_body_write)(const char *str, uint str_length);		/* string output */
int (*zend_header_write)(const char *str, uint str_length);	/* unbuffer string output */
void zend_start_ob_buffering();
void zend_end_ob_buffering(int send_buffer);
int zend_ob_get_buffer(pval *p);

/* HEAD support */
void set_header_request(int value);

#endif /* _OUTPUT_BUFFER */