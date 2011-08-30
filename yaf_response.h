/*
  +----------------------------------------------------------------------+
  | Yet Another Framework                                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Xinchen Hui  <laruence@php.net>                              |
  +----------------------------------------------------------------------+
*/
   
/* $Id$ */

#ifndef PHP_YAF_RESPONSE_H
#define PHP_YAF_RESPONSE_H

#define YAF_RESPONSE_PROPERTY_NAME_HEADER			"_header"
#define YAF_RESPONSE_PROPERTY_NAME_BODY				"_body"
#define YAF_RESPONSE_PROPERTY_NAME_HEADEREXCEPTION	"_sendheader"
#define YAF_RESPONSE_PROPERTY_NAME_RESPONSECODE		"_response_code"

extern zend_class_entry *yaf_response_ce;
extern zend_class_entry *yaf_response_http_ce;
extern zend_class_entry *yaf_response_cli_ce;

yaf_response_t * yaf_response_instance(yaf_response_t *this_ptr, char *sapi_name TSRMLS_DC);
int yaf_response_alter_body(yaf_response_t *response, char *name, int name_len, char *body, long body_len, int prepend TSRMLS_DC);
int yaf_response_send(yaf_response_t *response TSRMLS_DC);
int yaf_response_set_redirect(yaf_response_t *response, char *url, int len TSRMLS_DC);
int yaf_response_clear_body(yaf_response_t *response TSRMLS_DC);

YAF_STARTUP_FUNCTION(response);

#endif
