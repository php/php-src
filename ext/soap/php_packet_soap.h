#ifndef PHP_PACKET_SOAP_H
#define PHP_PACKET_SOAP_H

int parse_packet_soap(zval *this_ptr, char *buffer, int buffer_size, sdlFunctionPtr fn, char *fn_name, zval ***ret, int *num_params TSRMLS_DC);

#endif