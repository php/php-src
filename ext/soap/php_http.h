#ifndef PHP_HTTP_H
#define PHP_HTTP_H

void send_http_soap_request(zval *this_ptr, xmlDoc *doc, char *function_name, char *soapaction TSRMLS_DC);
void get_http_soap_response(zval *this_ptr, char **buffer, int *buffer_len TSRMLS_DC);

char *get_http_header_value(char *headers, char *type);
int get_http_body(php_stream *socketd, char *headers,  char **response, int *out_size TSRMLS_DC);
int get_http_headers(php_stream *socketd,char **response, int *out_size TSRMLS_DC);

#endif
