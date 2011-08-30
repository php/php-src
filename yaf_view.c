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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "main/SAPI.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_alloc.h"

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_exception.h"
#include "yaf_loader.h"
#include "yaf_view.h"

#include "views/interface.c"
#include "views/simple.c"

#if 0
static yaf_view_struct yaf_buildin_views[] = {
	{"classical", &yaf_view_simple_ce, yaf_view_simple_init},
	{NULL, NULL, NULL}
};
#endif

#if 0
/** {{{ yaf_view_t * yaf_view_instance(yaf_view_t *this_ptr TSRMLS_DC)
*/
yaf_view_t * yaf_view_instance(yaf_view_t *this_ptr TSRMLS_DC) {
	yaf_view_t		*view	= NULL;
	yaf_view_struct 	*p 		= yaf_buildin_views;

	for(;;++p) {
		yaf_current_view = p;
		yaf_view_ce = *(p->ce);
		break;
	}

	yaf_view_ce = *(yaf_current_view->ce);

	MAKE_STD_ZVAL(view);
	object_init_ex(view, *(yaf_current_view->ce));

	if (yaf_current_view->init) {
		yaf_current_view->init(view TSRMLS_CC);
	}
	MAKE_STD_ZVAL(view);
	object_init_ex(view, yaf_view_simple_ce);
	yaf_view_simple_init(view TSRMLS_CC);

	return view;
}
/* }}} */
#endif

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(view) {
	YAF_STARTUP(view_interface);
	YAF_STARTUP(view_simple);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
