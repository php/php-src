#ifndef PHP_HTTP_H
#define PHP_HTTP_H

int send_http_soap_request(zval *this_ptr, xmlDoc *doc, char *location, char *soapaction, int soap_version TSRMLS_DC);
int get_http_soap_response(zval *this_ptr, char **buffer, int *buffer_len TSRMLS_DC);

#endif
