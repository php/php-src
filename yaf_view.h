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

#ifndef YAF_VIEW_H
#define YAF_VIEW_H

#define yaf_view_instance yaf_view_simple_instance
#define yaf_view_ce		 yaf_view_simple_ce

#define YAF_VIEW_PROPERTY_NAME_TPLVARS 	"_tpl_vars"
#define YAF_VIEW_PROPERTY_NAME_TPLDIR	"_tpl_dir"

extern zend_class_entry *yaf_view_interface_ce;
extern zend_class_entry *yaf_view_simple_ce;

yaf_view_t * yaf_view_instance(yaf_view_t * this_ptr, zval *tpl_dir, zval *options TSRMLS_DC);

YAF_STARTUP_FUNCTION(view);
#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
