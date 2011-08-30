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

#ifndef YAF_EXCEPTION_H
#define YAF_EXCEPTION_H

#define YAF_MAX_BUILDIN_EXCEPTION	10

#define YAF_ERR_BASE 				512
#define YAF_UERR_BASE				1024
#define YAF_ERR_MASK				127

#define YAF_ERR_STARTUP_FAILED 		512
#define YAF_ERR_ROUTE_FAILED 		513
#define YAF_ERR_DISPATCH_FAILED 	514
#define YAF_ERR_NOTFOUND_MODULE 	515 
#define YAF_ERR_NOTFOUND_CONTROLLER 516 
#define YAF_ERR_NOTFOUND_ACTION 	517
#define YAF_ERR_NOTFOUND_VIEW 		518
#define YAF_ERR_CALL_FAILED			519
#define YAF_ERR_AUTOLOAD_FAILED 	520
#define YAF_ERR_TYPE_ERROR			521

#define YAF_EXCEPTION_OFFSET(x) (x & YAF_ERR_MASK)

#define YAF_CORRESPOND_ERROR(x) (x>>9L)

#define YAF_EXCEPTION_HANDLE(dispatcher, request, response) \
	if (EG(exception)) { \
		if (YAF_G(catch_exception)) { \
			yaf_dispatcher_exception_handler(dispatcher, request, response TSRMLS_CC); \
		} \
		return NULL; \
	}

#define YAF_EXCEPTION_HANDLE_NORET(dispatcher, request, response) \
	if (EG(exception)) { \
		if (YAF_G(catch_exception)) { \
			yaf_dispatcher_exception_handler(dispatcher, request, response TSRMLS_CC); \
		} \
	}

#define YAF_EXCEPTION_ERASE_EXCEPTION() \
	do { \
		EG(current_execute_data)->opline = EG(opline_before_exception); \
	} while(0)


extern zend_class_entry *yaf_ce_RuntimeException;
extern zend_class_entry *yaf_exception_ce;
void yaf_trigger_error(int type TSRMLS_DC, char *format, ...);
void yaf_throw_exception(long code, char *message TSRMLS_DC);

YAF_STARTUP_FUNCTION(exception);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
