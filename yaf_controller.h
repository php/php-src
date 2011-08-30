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

#ifndef YAF_CONTROLLER_H
#define YAF_CONTROLLER_H

#define YAF_CONTROLLER_PROPERTY_NAME_MODULE		"_module"
#define YAF_CONTROLLER_PROPERTY_NAME_NAME		"_name"
#define YAF_CONTROLLER_PROPERTY_NAME_SCRIPT		"_script_path"
#define YAF_CONTROLLER_PROPERTY_NAME_RESPONSE	"_response"
#define YAF_CONTROLLER_PROPERTY_NAME_REQUEST	"_request"
#define YAF_CONTROLLER_PROPERTY_NAME_ARGS		"_invoke_args"
#define YAF_CONTROLLER_PROPERTY_NAME_ACTIONS	"actions"	
#define YAF_CONTROLLER_PROPERTY_NAME_VIEW		"_view"

extern zend_class_entry *yaf_controller_ce;
int yaf_controller_construct(zend_class_entry *ce, yaf_controller_t *self, 
		yaf_request_t *request, yaf_response_t *response, yaf_view_t *view, zval *args TSRMLS_DC);
YAF_STARTUP_FUNCTION(controller);
#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

