#ifndef PHP_HTTP_H
#define PHP_HTTP_H

void send_http_soap_request(zval *this_ptr, xmlDoc *doc, char *function_name, char *soapaction TSRMLS_DC);
void get_http_soap_response(zval *this_ptr, char **buffer, int *buffer_len TSRMLS_DC);

char *get_http_header_value(char *headers, char *type);
int get_http_body(SOAP_STREAM socketd, char *headers,  char **response, int *out_size TSRMLS_DC);
int get_http_headers(SOAP_STREAM socketd,char **response, int *out_size TSRMLS_DC);

#ifndef PHP_STREAMS
#ifndef ZEND_WIN32
# ifndef closesocket
#  define closesocket close
# endif
#endif

#ifndef SOCK_CLOSE
# define SOCK_CLOSE(s) shutdown(s, 0); closesocket(s)
#endif

SOCKET get_socket(char* host,int portno,int timeout);
#endif

#endif
