#ifndef PHP_CONTENT_TYPES_H
#define PHP_CONTENT_TYPES_H

#define DEFAULT_POST_CONTENT_TYPE "application/x-www-form-urlencoded"

SAPI_API SAPI_POST_READER_FUNC(php_default_post_reader);
SAPI_API SAPI_POST_HANDLER_FUNC(php_std_post_handler);
int php_startup_sapi_content_types(void);

#endif /* PHP_CONTENT_TYPES_H */
