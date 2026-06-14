/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
  +----------------------------------------------------------------------+
  | Authors: Brad Lafountain <rodif_bl@yahoo.com>                        |
  |          Shane Caraveo <shane@caraveo.com>                           |
  |          Dmitry Stogov <dmitry@php.net>                              |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_HTTP_H
#define PHP_HTTP_H

int make_http_soap_request(
	zval *this_ptr, zend_string *buf, zend_string *location, char *soapaction,
	int soap_version, zend_string *uri_parser_class, zval *return_value
);

int proxy_authentication(zval* this_ptr, smart_str* soap_headers);
int basic_authentication(zval* this_ptr, smart_str* soap_headers);
void http_context_headers(php_stream_context* context,
                          bool has_authorization,
                          bool has_proxy_authorization,
                          bool has_cookies,
                          smart_str* soap_headers);
#endif
