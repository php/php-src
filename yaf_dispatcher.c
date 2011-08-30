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
#include "Zend/zend_API.h"
#include "Zend/zend_interfaces.h"

#include "php_yaf.h"
#include "yaf_namespace.h"
#include "yaf_dispatcher.h"
#include "yaf_controller.h"
#include "yaf_action.h"
#include "yaf_application.h"
#include "yaf_view.h"
#include "yaf_response.h"
#include "yaf_loader.h"
#include "yaf_router.h"
#include "yaf_request.h"
#include "yaf_config.h"
#include "yaf_plugin.h"
#include "yaf_exception.h"

zend_class_entry * yaf_dispatcher_ce;

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_dispatch_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, request)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_seterrhdler_arginfo, 0, 0, 2)
    ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, error_types)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_flush_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_regplugin_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, plugin)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_setrequest_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, plugin)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_throwex_arginfo, 0, 0, 0)
    ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_catchex_arginfo, 0, 0, 0)
    ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_autorender_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_returnresp_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_initview_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, templates_dir)
	ZEND_ARG_ARRAY_INFO(0, options, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_setview_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, view)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_setappdir_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, directory)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_setctrl_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, controller)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_setmodule_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, module)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(yaf_dispatcher_setaction_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, action)
ZEND_END_ARG_INFO()

/* }}} */

/** {{{ yaf_dispatcher_t * yaf_dispatcher_instance(zval *this_ptr TSRMLS_DC) 
*/
yaf_dispatcher_t * yaf_dispatcher_instance(yaf_dispatcher_t *this_ptr TSRMLS_DC) {
	zval				*plugins;
	yaf_router_t	 	*router;
	yaf_dispatcher_t 	*instance;

	instance = zend_read_static_property(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_INSTANCE), 1 TSRMLS_CC);

	if (IS_OBJECT == Z_TYPE_P(instance) 
			&& instanceof_function(Z_OBJCE_P(instance), yaf_dispatcher_ce TSRMLS_CC)) {
		Z_ADDREF_P(instance);
		return instance;
	}

	if (this_ptr) {
		instance = this_ptr;
		return this_ptr;
	} else {
		instance = NULL;
		MAKE_STD_ZVAL(instance);
		object_init_ex(instance, yaf_dispatcher_ce);
	}

	/** unecessary yet 
	MAKE_STD_ZVAL(args);
	array_init(args);
	yaf_update_property(instance, YAF_DISPATCHER_PROPERTY_NAME_ARGS, 	  args);
	*/

	MAKE_STD_ZVAL(plugins);
	array_init(plugins);
	zend_update_property(yaf_dispatcher_ce, instance, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_PLUGINS), plugins TSRMLS_CC);
	zval_ptr_dtor(&plugins);

	router	 = yaf_router_instance(NULL TSRMLS_CC);

	zend_update_property(yaf_dispatcher_ce, instance, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_ROUTER), router TSRMLS_CC);
	zend_update_property_string(yaf_dispatcher_ce, instance, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_MODULE), 	YAF_G(default_module) TSRMLS_CC);
	zend_update_property_string(yaf_dispatcher_ce, instance, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_CONTROLLER), YAF_G(default_controller) TSRMLS_CC);
	zend_update_property_string(yaf_dispatcher_ce, instance, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_ACTION), 	YAF_G(default_action) TSRMLS_CC);
	zend_update_static_property(yaf_dispatcher_ce, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_INSTANCE), instance TSRMLS_CC);

	zval_ptr_dtor(&router);

	return instance;
}
/* }}} */

/** {{{ static void yaf_dispatcher_get_call_parmaters(zend_class_entry *request_ce, yaf_request_t *request, zend_function *fptr, zval ****params, uint *count TSRMLS_DC)
 */
static void yaf_dispatcher_get_call_parmaters(zend_class_entry *request_ce, yaf_request_t *request, zend_function *fptr, zval ****params, uint *count TSRMLS_DC) {
	zval 		  	*args, **arg;
	zend_arg_info 	*arg_info;
	uint 		 	current;
	HashTable 		*params_ht;

	args = zend_read_property(request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_PARAMS), 1 TSRMLS_CC);

	params_ht = Z_ARRVAL_P(args);

	arg_info  = fptr->common.arg_info;
	*params   = safe_emalloc(sizeof(zval **), fptr->common.num_args, 0);
	for (current = 0;current < fptr->common.num_args; current++, arg_info++) {
		if (zend_hash_find(params_ht, arg_info->name, arg_info->name_len + 1, (void **)&arg) == SUCCESS) {
			(*params)[current] = arg;
			(*count)++;
		} else {
			char *key;
			uint  keylen;
			long idx, llen;

			arg  = NULL;
			llen = arg_info->name_len + 1;
			/* since we need search ignoring case, can't use zend_hash_find */
			for(zend_hash_internal_pointer_reset(params_ht);
					zend_hash_has_more_elements(params_ht) == SUCCESS;
					zend_hash_move_forward(params_ht)) {

				if (zend_hash_get_current_key_ex(params_ht, &key, &keylen, &idx, 0, NULL) == HASH_KEY_IS_STRING) {
					if (keylen == llen && !strncasecmp(key, arg_info->name, keylen)) {
						if (zend_hash_get_current_data(params_ht, (void**)&arg) == SUCCESS) {
							/* return when we find first match, there is a trap 
							 * when multi different parameters in different case presenting in params_ht
							 * only the first take affect
							 */
							(*params)[current] = arg;
							(*count)++;
							break;
						}
					}
				}
			}

			if (NULL == arg) {
				break;
			}
		}	
	}
}
/* }}} */

/** {{{ static yaf_view_t * yaf_dispatcher_init_view(yaf_dispatcher_t *dispatcher, zval *tpl_dir, zval *options TSRMLS_DC)
*/
 yaf_view_t * yaf_dispatcher_init_view(yaf_dispatcher_t *dispatcher, zval *tpl_dir, zval *options TSRMLS_DC) {
	yaf_view_t *view = zend_read_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_VIEW), 1 TSRMLS_CC);
	if (view && IS_OBJECT == Z_TYPE_P(view)
			&& instanceof_function(Z_OBJCE_P(view), yaf_view_interface_ce TSRMLS_CC)) {
		return view;
	}

	view = yaf_view_instance(NULL, tpl_dir, options TSRMLS_CC);
	zend_update_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_VIEW), view TSRMLS_CC);

	return view;
}
/* }}} */

/** {{{ static inline void yaf_dispatcher_fix_default(yaf_dispatcher_t *dispatcher, yaf_request_t *request TSRMLS_DC)
*/
static inline void yaf_dispatcher_fix_default(yaf_dispatcher_t *dispatcher, yaf_request_t *request TSRMLS_DC) {
	zval	*module, *controller, *action;

	module 		= zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), 1 TSRMLS_CC);
	action	 	= zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), 1 TSRMLS_CC);
	controller 	= zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), 1 TSRMLS_CC);

	if (!module || Z_TYPE_P(module) != IS_STRING || !Z_STRLEN_P(module)) {
		zval *default_module = zend_read_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_MODULE), 1 TSRMLS_CC);
		zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), default_module TSRMLS_CC);
	} else {
		ZVAL_STRINGL(module, zend_str_tolower_dup(Z_STRVAL_P(module), Z_STRLEN_P(module)), Z_STRLEN_P(module), 0);
		*(Z_STRVAL_P(module)) = toupper(*(Z_STRVAL_P(module)));
		zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), module TSRMLS_CC);
	}

	if (!controller || Z_TYPE_P(controller) != IS_STRING || !Z_STRLEN_P(controller)) {
		zval *default_controller = zend_read_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_CONTROLLER), 1 TSRMLS_CC);
		zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), default_controller TSRMLS_CC);
	} else {
		char *p;
		ZVAL_STRINGL(controller, zend_str_tolower_dup(Z_STRVAL_P(controller), Z_STRLEN_P(controller)), Z_STRLEN_P(controller), 0);
		p = Z_STRVAL_P(controller);

		/**
		 * upper contolerr name
		 * eg: Index_sub -> Index_Sub
		 */
		*p = toupper(*p);
		while (*p != '\0') {
			if (*p == '_'
#ifdef YAF_HAVE_NAMESPACE
					|| *p == '\\'	
#endif
			   ) {
				if (*(p+1) != '\0') {
					*(p+1) = toupper(*(p+1));
					p++;
				}
			}
			p++;
		}

		zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), controller TSRMLS_CC);
	}


	if (!action || Z_TYPE_P(action) != IS_STRING || !Z_STRLEN_P(action)) {
		zval *default_action = zend_read_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_ACTION), 1 TSRMLS_CC);
		zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), default_action TSRMLS_CC);
	} else {
		ZVAL_STRINGL(action, zend_str_tolower_dup(Z_STRVAL_P(action), Z_STRLEN_P(action)), Z_STRLEN_P(action), 0);
		zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), action TSRMLS_CC);
	}
}
/* }}} */

/** {{{int yaf_dispatcher_set_request(yaf_dispatcher_t *dispatcher, yaf_request_t *request TSRMLS_DC)
*/
int yaf_dispatcher_set_request(yaf_dispatcher_t *dispatcher, yaf_request_t *request TSRMLS_DC) {
	if (request) {
		zend_update_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_REQUEST), request TSRMLS_CC);
		return 1;
	}
	return 0;
}
/* }}} */

/** {{{ zend_class_entry * yaf_dispatcher_get_controller(char *app_dir, char *module, char *controller, int len, int def_module TSRMLS_DC)
 */
zend_class_entry * yaf_dispatcher_get_controller(char *app_dir, char *module, char *controller, int len, int def_module TSRMLS_DC) {
	char *directory 	= NULL;
	int	 directory_len 	= 0;

	if (def_module) {
		directory_len = spprintf(&directory, 0, "%s%c%s", app_dir, DEFAULT_SLASH, "controllers");
	} else {
		directory_len = spprintf(&directory, 0, "%s%c%s%c%s%c%s", app_dir, DEFAULT_SLASH,
				"modules", DEFAULT_SLASH, module, DEFAULT_SLASH, "controllers");
	}

	if (directory_len) {
		char *class			  = NULL;
		char *class_lowercase = NULL;
		int	 class_len		  = 0;
		zend_class_entry **ce = NULL;

		if (YAF_G(name_suffix)) {
			class_len = spprintf(&class, 0, "%s%s%s", controller, YAF_G(name_separator), "Controller");
		} else {
			class_len = spprintf(&class, 0, "%s%s%s", "Controller", YAF_G(name_separator), controller);
		}

		class_lowercase = zend_str_tolower_dup(class, class_len);

		if (zend_hash_find(EG(class_table), class_lowercase, class_len + 1, (void *)&ce) != SUCCESS) {

			if (!yaf_internal_autoload(controller, len, &directory TSRMLS_CC)) {
				yaf_trigger_error(YAF_ERR_NOTFOUND_CONTROLLER TSRMLS_CC, "Could not find controller script %s", directory);
				efree(class);
				efree(class_lowercase);
				efree(directory);
				return NULL;
			} else if (zend_hash_find(EG(class_table), class_lowercase, class_len + 1, (void **) &ce) != SUCCESS)  {
				yaf_trigger_error(YAF_ERR_AUTOLOAD_FAILED TSRMLS_CC, "Could not find class %s in controller script %s", class, directory);
				efree(class);
				efree(class_lowercase);
				efree(directory);
				return 0;
			} else if (!instanceof_function(*ce, yaf_controller_ce TSRMLS_CC)) {
				yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "Controller must be an instance of %s", yaf_controller_ce->name);
				efree(class);
				efree(class_lowercase);
				efree(directory);
				return 0;
			}
		}

		efree(class);
		efree(class_lowercase);
		efree(directory);

		return *ce;
	}

	return NULL;
}
/* }}} */

/** {{{ zend_class_entry * yaf_dispatcher_get_action(char *app_dir, yaf_controller_t *controller, char *module, int def_module, char *action, int len TSRMLS_DC)
 */
zend_class_entry * yaf_dispatcher_get_action(char *app_dir, yaf_controller_t *controller, char *module, int def_module, char *action, int len TSRMLS_DC) {
	zval **ppaction, *actions_map;

	actions_map = zend_read_property(Z_OBJCE_P(controller), controller, ZEND_STRL(YAF_CONTROLLER_PROPERTY_NAME_ACTIONS), 1 TSRMLS_CC);
	if (IS_ARRAY == Z_TYPE_P(actions_map)) {
		if (zend_hash_find(Z_ARRVAL_P(actions_map), action, len + 1, (void **)&ppaction) == SUCCESS) {
			char *action_path;
			uint action_path_len;

			action_path_len = spprintf(&action_path, 0, "%s%c%s", app_dir, DEFAULT_SLASH, Z_STRVAL_PP(ppaction));
			if (yaf_loader_import(action_path, action_path_len, 0 TSRMLS_CC)) {
				zend_class_entry **ce;
				char *class, *class_lowercase;
				uint  class_len;
				char *action_upper = estrndup(action, len);

				*(action_upper) = toupper(*action_upper);

				if (YAF_G(name_suffix)) {
					class_len = spprintf(&class, 0, "%s%s%s", action_upper, YAF_G(name_separator), "Action");
				} else {
					class_len = spprintf(&class, 0, "%s%s%s", "Action", YAF_G(name_separator), action_upper);
				}

				class_lowercase = zend_str_tolower_dup(class, class_len);

				if (zend_hash_find(EG(class_table), class_lowercase, class_len + 1, (void **) &ce) == SUCCESS) {
					efree(action_path);
					efree(action_upper);
					efree(class_lowercase);

					if (instanceof_function(*ce, yaf_action_ce TSRMLS_CC)) {
						efree(class);
						return *ce;
					} else {
						yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "Action %s must extends from %s", class, yaf_action_ce->name);
						efree(class);
					}

				} else {
					yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION TSRMLS_CC, "Could not find action %s in %s", class, action_path);
				}

				efree(action_path);
				efree(action_upper);
				efree(class);
				efree(class_lowercase);

			} else {
				yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION TSRMLS_CC, "Could not find action script %s", action_path);
				efree(action_path);
			}
		} else {
			yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION TSRMLS_CC, "There is no method %s%s in %s::$%s", 
					action, "Action", Z_OBJCE_P(controller)->name, YAF_CONTROLLER_PROPERTY_NAME_ACTIONS);
		}
	} else
/* {{{ This only effects internally */
	   	if (YAF_G(st_compatible)) {
		char *directory, *class, *class_lowercase, *p;
		uint directory_len, class_len;
		zend_class_entry **ce;
		char *action_upper = estrndup(action, len);

		/**
		 * upper Action Name
		 * eg: Index_sub -> Index_Sub
		 */
		p = action_upper;
		*(p) = toupper(*p);
		while (*p != '\0') {
			if (*p == '_'
#ifdef YAF_HAVE_NAMESPACE
					|| *p == '\\'	
#endif
			   ) {
				if (*(p+1) != '\0') {
					*(p+1) = toupper(*(p+1));
					p++;
				}
			}
			p++;
		}

		if (def_module) {
			directory_len = spprintf(&directory, 0, "%s%c%s", app_dir, DEFAULT_SLASH, "actions");
		} else {
			directory_len = spprintf(&directory, 0, "%s%c%s%c%s%c%s", app_dir, DEFAULT_SLASH,
					"modules", DEFAULT_SLASH, module, DEFAULT_SLASH, "actions");
		}

		if (YAF_G(name_suffix)) {
			class_len = spprintf(&class, 0, "%s%s%s", action_upper, YAF_G(name_separator), "Action");
		} else {
			class_len = spprintf(&class, 0, "%s%s%s", "Action", YAF_G(name_separator), action_upper);
		}

		class_lowercase = zend_str_tolower_dup(class, class_len);

		if (zend_hash_find(EG(class_table), class_lowercase, class_len + 1, (void *)&ce) != SUCCESS) {
			if (!yaf_internal_autoload(action_upper, len, &directory TSRMLS_CC)) {
				yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION TSRMLS_CC, "Could not find action script %s", directory);

				efree(class);
				efree(action_upper);
				efree(class_lowercase);
				efree(directory);
				return NULL;
			} else if (zend_hash_find(EG(class_table), class_lowercase, class_len + 1, (void **) &ce) != SUCCESS)  {
				yaf_trigger_error(YAF_ERR_AUTOLOAD_FAILED TSRMLS_CC, "Could find class %s in action script %s", class, directory);

				efree(class);
				efree(action_upper);
				efree(class_lowercase);
				efree(directory);
				return NULL;
			} else if (!instanceof_function(*ce, yaf_action_ce TSRMLS_CC)) {
				yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "Action must be an instance of %s", yaf_action_ce->name);

				efree(class);
				efree(action_upper);
				efree(class_lowercase);
				efree(directory);
				return NULL;
			}
		}

		efree(class);
		efree(action_upper);
		efree(class_lowercase);
		efree(directory);

		return *ce;
	} else 
/* }}} */
	{
		yaf_trigger_error(YAF_ERR_NOTFOUND_ACTION TSRMLS_CC, "There is no method %s%s in %s", action, "Action", Z_OBJCE_P(controller)->name);
	}
	
	return NULL;
}
/* }}} */

/** {{{ int yaf_dispatcher_handle(yaf_dispatcher_t *dispatcher, yaf_request_t *request,  yaf_response_t *response, yaf_view_t *view TSRMLS_DC)
*/
int yaf_dispatcher_handle(yaf_dispatcher_t *dispatcher, yaf_request_t *request,  yaf_response_t *response, yaf_view_t *view TSRMLS_DC) {
	zend_class_entry *request_ce;
	char *app_dir = YAF_G(directory);

	request_ce = Z_OBJCE_P(request);

	yaf_request_set_dispatched(request, 1 TSRMLS_CC);
	if (!app_dir) {
		yaf_trigger_error(YAF_ERR_STARTUP_FAILED TSRMLS_CC, "%s requires %s(which set the application.directory) to be initialized first", 
				yaf_dispatcher_ce->name, yaf_application_ce->name);
		return 0;
	} else {
		int	is_def_module = 0;
		int is_def_ctr = 0;
		zval *module, *controller, *dmodule, *dcontroller, *return_response, *instantly_flush;
		zend_class_entry *ce;
		yaf_controller_t *executor;
		zend_function    *fptr;

		module		= zend_read_property(request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), 1 TSRMLS_CC);
		controller	= zend_read_property(request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), 1 TSRMLS_CC);

		dmodule		= zend_read_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_MODULE), 1 TSRMLS_CC);
		dcontroller = zend_read_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_CONTROLLER), 1 TSRMLS_CC);

		if (Z_TYPE_P(module) != IS_STRING
				|| !Z_STRLEN_P(module)) {
			yaf_trigger_error(YAF_ERR_DISPATCH_FAILED TSRMLS_CC, "Unexcepted a empty module name");
			return 0;
		} else if (!yaf_application_is_module_name(Z_STRVAL_P(module), Z_STRLEN_P(module) TSRMLS_CC)) {
			yaf_trigger_error(YAF_ERR_NOTFOUND_MODULE TSRMLS_CC, "There is no module %s", Z_STRVAL_P(module));
			return 0;
		}

		if (Z_TYPE_P(controller) != IS_STRING
				|| !Z_STRLEN_P(controller)) {
			yaf_trigger_error(YAF_ERR_DISPATCH_FAILED TSRMLS_CC, "Unexcepted a empty controller name");
			return 0;
		}

		if(strncasecmp(Z_STRVAL_P(dmodule), Z_STRVAL_P(module), Z_STRLEN_P(module)) == 0) {
			is_def_module = 1;
		} 

		if (strncasecmp(Z_STRVAL_P(dcontroller), Z_STRVAL_P(controller), Z_STRLEN_P(controller)) == 0) {
			is_def_ctr = 1;
		} 

		ce = yaf_dispatcher_get_controller(app_dir, Z_STRVAL_P(module), Z_STRVAL_P(controller), Z_STRLEN_P(controller), is_def_module TSRMLS_CC);
		if (!ce) {
			return 0;
		} else {
			zval  *action, *view_dir, *render, *ret = NULL;
			char  *action_lower, *func_name;
			uint  func_name_len;
			zend_class_entry *view_ce;

			yaf_controller_t *icontroller;

			MAKE_STD_ZVAL(icontroller);
			object_init_ex(icontroller, ce);

			/* cause controller's constructor is a final method, so it must be a internal function 
			   do { 
			   zend_function *constructor = NULL;
			   constructor = Z_OBJ_HT_P(exec_ctr)->get_constructor(exec_ctr TSRMLS_CC);
			   if (constructor != NULL) {
			   if (zend_call_method_with_2_params(&exec_ctr, *ce
			   , &constructor, NULL, &ret, request, response) == NULL) {
			   yaf_trigger_error(YAF_ERR_CALL_FAILED, "function call for %s::__construct failed", (*ce)->name);
			   return 0;
			   }
			   }
			   } while(0);
			*/
			yaf_controller_construct(ce, icontroller, request, response, view, NULL TSRMLS_CC);

			MAKE_STD_ZVAL(view_dir);
			Z_TYPE_P(view_dir) = IS_STRING;

			if (is_def_module) {
				Z_STRLEN_P(view_dir) = spprintf(&(Z_STRVAL_P(view_dir)), 0, "%s/%s", app_dir ,"views");
			} else {
				Z_STRLEN_P(view_dir) = spprintf(&(Z_STRVAL_P(view_dir)), 0, "%s/%s/%s/%s", app_dir,
						"modules", Z_STRVAL_P(module), "views");
			}

			/** tell the view engine where to find templates */
			if ((view_ce = Z_OBJCE_P(view)) == yaf_view_simple_ce) {
				zend_update_property(view_ce, view,  ZEND_STRL(YAF_VIEW_PROPERTY_NAME_TPLDIR), view_dir TSRMLS_CC);
			} else {
				zend_call_method_with_1_params(&view, view_ce, NULL, "setscriptpath", NULL, view_dir);
			}

			zval_ptr_dtor(&view_dir);

			zend_update_property(ce, icontroller, ZEND_STRL(YAF_CONTROLLER_PROPERTY_NAME_NAME),	controller TSRMLS_CC);

			action		 = zend_read_property(request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), 1 TSRMLS_CC);
			action_lower = zend_str_tolower_dup(Z_STRVAL_P(action), Z_STRLEN_P(action));

			/* cause the action might call the forward to override the old action */
			Z_ADDREF_P(action);

			func_name_len    = spprintf(&func_name,  0, "%s%s", action_lower, "action");
			efree(action_lower);

			if (zend_hash_find(&((ce)->function_table), func_name, func_name_len + 1, (void **)&fptr) == SUCCESS) {
				uint count = 0;
				zval ***call_args = NULL;

				ret = NULL;

				executor = icontroller;
				if (fptr->common.num_args) {	
					zval *method_name;

					yaf_dispatcher_get_call_parmaters(request_ce, request, fptr, &call_args, &count TSRMLS_CC);
					MAKE_STD_ZVAL(method_name);
					ZVAL_STRINGL(method_name, func_name, func_name_len, 0);

					call_user_function_ex(&(ce)->function_table, &icontroller, method_name, &ret, count, call_args, 1, NULL TSRMLS_CC);

					efree(method_name);
					efree(call_args);
				} else {
					zend_call_method(&icontroller, ce, NULL, func_name, func_name_len, &ret, 0, NULL, NULL TSRMLS_CC);
				}

				efree(func_name);

				if (!ret) {
					Z_DELREF_P(action);
					zval_dtor(icontroller);
					efree(icontroller);
					return 0;
				}

				if ((Z_TYPE_P(ret) == IS_BOOL
							&& !Z_BVAL_P(ret))) {
					zval_ptr_dtor(&ret);
					Z_DELREF_P(action);
					zval_dtor(icontroller);
					efree(icontroller);
					return 0;
				}
			} else if ((ce = yaf_dispatcher_get_action(app_dir, icontroller, 
							Z_STRVAL_P(module), is_def_module, Z_STRVAL_P(action), Z_STRLEN_P(action) TSRMLS_CC)) 
					&& (zend_hash_find(&(ce)->function_table, YAF_ACTION_EXECUTOR_NAME, 
							sizeof(YAF_ACTION_EXECUTOR_NAME), (void **)&fptr) == SUCCESS)) {
				zval ***call_args;
				yaf_action_t *iaction;
				uint count = 0; 

				MAKE_STD_ZVAL(iaction);
				object_init_ex(iaction, ce);

				yaf_controller_construct(ce, iaction, request, response, view, NULL TSRMLS_CC);
				executor = iaction;

				zend_update_property(ce, iaction, ZEND_STRL(YAF_CONTROLLER_PROPERTY_NAME_NAME), controller TSRMLS_CC);
				zend_update_property(ce, iaction, ZEND_STRL(YAF_ACTION_PROPERTY_NAME_CTRL), icontroller TSRMLS_CC);

				if (fptr->common.num_args) {
					zval *method_name = NULL;

					yaf_dispatcher_get_call_parmaters(request_ce, request, fptr, &call_args, &count TSRMLS_CC);
					MAKE_STD_ZVAL(method_name);
					ZVAL_STRINGL(method_name, YAF_ACTION_EXECUTOR_NAME, sizeof(YAF_ACTION_EXECUTOR_NAME) - 1, 0);

					call_user_function_ex(&(ce)->function_table, &iaction, method_name, &ret, count, call_args, 1, NULL TSRMLS_CC);

					efree(method_name);
					efree(call_args);
				} else {
					zend_call_method_with_0_params(&iaction, ce, NULL, "execute", &ret);
				}

				if (!ret) {
					Z_DELREF_P(action);
					zval_dtor(iaction);
					efree(iaction);
					zval_dtor(icontroller);
					efree(icontroller);
					return 0;
				}

				if (( Z_TYPE_P(ret) == IS_BOOL
							&& !Z_BVAL_P(ret))) {
					zval_ptr_dtor(&ret);
					Z_DELREF_P(action);
					zval_dtor(iaction);
					efree(iaction);
					zval_dtor(icontroller);
					efree(icontroller);
					return 0;
				} 
			} else {
				Z_DELREF_P(action);
				zval_dtor(icontroller);
				efree(icontroller);
				return 0;
			}	

			render = zend_read_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_RENDER), 1 TSRMLS_CC);
			return_response = zend_read_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_RETURN), 1 TSRMLS_CC);
			instantly_flush	= zend_read_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_FLUSH), 1 TSRMLS_CC);

			if (executor && Z_BVAL_P(render)) {
				ret = NULL;
				if (!Z_BVAL_P(instantly_flush)) {
					zend_call_method_with_1_params(&executor, ce, NULL, "render", &ret, action);
					zval_dtor(executor);
					efree(executor);

					if (ret && Z_TYPE_P(ret) == IS_STRING && Z_STRLEN_P(ret)) {
						yaf_response_alter_body(response, NULL, 0, Z_STRVAL_P(ret), Z_STRLEN_P(ret), 0 TSRMLS_CC);
						zval_ptr_dtor(&ret);
					} else if (ret) {
						zval_ptr_dtor(&ret);
						Z_DELREF_P(action);
						return 0;
					}
				} else {
					zend_call_method_with_1_params(&executor, ce, NULL, "display", &ret, action);
					zval_dtor(executor);
					efree(executor);

					if (!ret) {
						Z_DELREF_P(action);
						return 0;
					}

					if ((Z_TYPE_P(ret) == IS_BOOL && !Z_BVAL_P(ret))) {
						zval_ptr_dtor(&ret);
						Z_DELREF_P(action);
						return 0;
					}
				}

			}
			Z_DELREF_P(action);

			return 1;
		}
	}	

	return 0;
}
/* }}} */

/** {{{ void yaf_dispatcher_exception_handler(yaf_dispatcher_t *dispatcher, yaf_request_t *request, yaf_response_t *response TSRMLS_DC)
*/
void yaf_dispatcher_exception_handler(yaf_dispatcher_t *dispatcher, yaf_request_t *request, yaf_response_t *response TSRMLS_DC) {
	zval *module, *controller, *action, *exception;
	yaf_view_t  *view;

	if (YAF_G(in_exception) || !EG(exception)) {
		return;
	}

	YAF_G(in_exception) = 1;

	MAKE_STD_ZVAL(controller);
	MAKE_STD_ZVAL(action);

	module = zend_read_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), 1 TSRMLS_CC);

	if (!module || Z_TYPE_P(module) != IS_STRING || !Z_STRLEN_P(module)) {
		module = zend_read_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_MODULE), 1 TSRMLS_CC);
		zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_MODULE), module TSRMLS_CC);
	}

	ZVAL_STRING(controller, YAF_ERROR_CONTROLLER, 1);
	ZVAL_STRING(action, 	YAF_ERROR_ACTION, 1);

	exception = EG(exception);
	EG(exception) = NULL;

	zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_CONTROLLER), controller TSRMLS_CC);
	zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_ACTION), action TSRMLS_CC);
	zend_update_property(yaf_request_ce, request, ZEND_STRL(YAF_REQUEST_PROPERTY_NAME_EXCEPTION), exception TSRMLS_CC);

	Z_DELREF_P(controller);
	Z_DELREF_P(action);

	/** use $request->getException() instand of */
	yaf_request_set_params_single(request, YAF_STRL("exception"), exception TSRMLS_CC);
	yaf_request_set_dispatched(request, 0 TSRMLS_CC);

	view = yaf_dispatcher_init_view(dispatcher, NULL, NULL TSRMLS_CC);

	if (!yaf_dispatcher_handle(dispatcher, request, response, view TSRMLS_CC)) {
		return;
	}

	(void)yaf_response_send(response TSRMLS_CC);

	YAF_EXCEPTION_ERASE_EXCEPTION();
}
/* }}} */

/** {{{ int yaf_dispatcher_route(yaf_dispatcher_t *dispatcher, yaf_request_t *request TSRMLS_DC)
 */
int yaf_dispatcher_route(yaf_dispatcher_t *dispatcher, yaf_request_t *request TSRMLS_DC) {
	zend_class_entry *router_ce;
	yaf_router_t *router = zend_read_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_ROUTER), 1 TSRMLS_CC);
	if (IS_OBJECT == Z_TYPE_P(router)) {
		if ((router_ce = Z_OBJCE_P(router)) == yaf_router_ce) {
			/* use buildin router */
			if (!yaf_router_route(router, request TSRMLS_CC)) {
				return 0;
			}
		} else {
			/* user custom router */
			zval *ret = zend_call_method_with_1_params(&router, router_ce, NULL, "route", &ret, request);
			if (Z_TYPE_P(ret) == IS_BOOL && Z_BVAL_P(ret) == 0) {
				yaf_trigger_error(YAF_ERR_ROUTE_FAILED TSRMLS_CC, "Routing request faild");
				return 0;
			}
		}
		return 1;
	}
	return 0;
}
/* }}} */

/** {{{ yaf_response_t * yaf_dispatcher_dispatch(yaf_dispatcher_t *dispatcher TSRMLS_DC)
*/
yaf_response_t * yaf_dispatcher_dispatch(yaf_dispatcher_t *dispatcher TSRMLS_DC) {
	zval *return_response, *plugins, *view;
	uint nesting = YAF_G(forward_limit);

	yaf_response_t  	*response;
	yaf_request_t		*request;

	response = yaf_response_instance(NULL, sapi_module.name TSRMLS_CC);
	request	 = zend_read_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_REQUEST), 1 TSRMLS_CC);
	plugins	 = zend_read_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_PLUGINS), 1 TSRMLS_CC);

	if (!request || IS_OBJECT != Z_TYPE_P(request)) {
		yaf_trigger_error(YAF_ERR_TYPE_ERROR TSRMLS_CC, "Expect a %s instance", yaf_request_ce->name);
		return NULL;
	}

	/* route request */
	if (!yaf_request_is_routed(request TSRMLS_CC)) {
		YAF_PLUGIN_HANDLE(plugins, YAF_PLUGIN_HOOK_ROUTESTARTUP, request, response);
		YAF_EXCEPTION_HANDLE(dispatcher, request, response);
		if (!yaf_dispatcher_route(dispatcher, request TSRMLS_CC)) {
			yaf_trigger_error(YAF_ERR_ROUTE_FAILED TSRMLS_CC, "Routing request failed");
			YAF_EXCEPTION_HANDLE_NORET(dispatcher, request, response);
			return NULL;
		}
		YAF_PLUGIN_HANDLE(plugins, YAF_PLUGIN_HOOK_ROUTESHUTDOWN, request, response);
		YAF_EXCEPTION_HANDLE(dispatcher, request, response);
		(void)yaf_request_set_routed(request, 1 TSRMLS_CC);
	}

	yaf_dispatcher_fix_default(dispatcher, request TSRMLS_CC);

	YAF_PLUGIN_HANDLE(plugins, YAF_PLUGIN_HOOK_LOOPSTARTUP, request, response);
	YAF_EXCEPTION_HANDLE(dispatcher, request, response);

	view = yaf_dispatcher_init_view(dispatcher, NULL, NULL TSRMLS_CC);

	do {
		YAF_PLUGIN_HANDLE(plugins, YAF_PLUGIN_HOOK_PREDISPATCH, request, response);
		if (!yaf_dispatcher_handle(dispatcher, request, response, view TSRMLS_CC)) {
			YAF_EXCEPTION_HANDLE(dispatcher, request, response);
			return NULL;
		}
		yaf_dispatcher_fix_default(dispatcher, request TSRMLS_CC);
		YAF_PLUGIN_HANDLE(plugins, YAF_PLUGIN_HOOK_POSTDISPATCH, request, response);
		YAF_EXCEPTION_HANDLE(dispatcher, request, response);
	} while (--nesting > 0 && !yaf_request_is_dispatched(request TSRMLS_CC));

	YAF_PLUGIN_HANDLE(plugins, YAF_PLUGIN_HOOK_LOOPSHUTDOWN, request, response);
	YAF_EXCEPTION_HANDLE(dispatcher, request, response);

	if (0 == nesting && !yaf_request_is_dispatched(request TSRMLS_CC)) {
		yaf_trigger_error(YAF_ERR_DISPATCH_FAILED TSRMLS_CC, "The max dispatch nesting %ld was reached", YAF_G(forward_limit));
		YAF_EXCEPTION_HANDLE_NORET(dispatcher, request, response);
		return NULL;
	}

	return_response = zend_read_property(yaf_dispatcher_ce, dispatcher, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_RETURN), 1 TSRMLS_CC);

	if (!Z_BVAL_P(return_response)) {
		(void)yaf_response_send(response TSRMLS_CC);
		yaf_response_clear_body(response TSRMLS_CC);
	}

	return response;
}
/* }}} */

/** {{{ proto private Yaf_Dispatcher::__construct(void) 
*/
PHP_METHOD(yaf_dispatcher, __construct) {
}
/* }}} */

/** {{{ proto private Yaf_Dispatcher::__sleep(void)
*/
PHP_METHOD(yaf_dispatcher, __sleep) {
}
/* }}} */

/** {{{ proto private Yaf_Dispatcher::__wakeup(void)
*/
PHP_METHOD(yaf_dispatcher, __wakeup) {
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setErrorHandler(string $callbacak[, int $error_types = E_ALL | E_STRICT ] )
*/
PHP_METHOD(yaf_dispatcher, setErrorHandler) {
	zval *callback, *error_type = NULL;
	zval *params[2]	 = {0};
	zval function	 = {{0}, 0};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &callback, &error_type) == FAILURE) {
		return;
	}

	params[0] = callback;
	if (error_type) {
		params[1] = error_type;
	}

	ZVAL_STRING(&function, "set_error_handler", 0);
	if (call_user_function(EG(function_table), NULL, &function, return_value, ZEND_NUM_ARGS(), params TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Call to set_error_handler failed");
	}
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::disableView() 
*/
PHP_METHOD(yaf_dispatcher, disableView) {
	yaf_dispatcher_t *self = getThis();
	zend_update_property_bool(yaf_dispatcher_ce, self, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_RENDER), 0 TSRMLS_CC);
	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::enableView() 
*/
PHP_METHOD(yaf_dispatcher, enableView) {
	yaf_dispatcher_t *self = getThis();
	zend_update_property_bool(yaf_dispatcher_ce, self, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_RENDER), 1 TSRMLS_CC);
	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::returnResponse() 
*/
PHP_METHOD(yaf_dispatcher, returnResponse) {
	long auto_response;
	yaf_dispatcher_t *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &auto_response) == FAILURE) {
		return;
	}

	zend_update_property_bool(yaf_dispatcher_ce, self, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_RETURN), (auto_response)? 1:0 TSRMLS_CC);

	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::flushInstantly(bool $flag) 
*/
PHP_METHOD(yaf_dispatcher, flushInstantly) {
	long instantly_flush;
	yaf_dispatcher_t *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &instantly_flush) == FAILURE) {
		return;
	}

	zend_update_property_bool(yaf_dispatcher_ce, self, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_FLUSH), (instantly_flush)? 1:0 TSRMLS_CC);

	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::registerPlugin(Yaf_Plugin_Abstract $plugin)
*/
PHP_METHOD(yaf_dispatcher, registerPlugin) {
	zval *plugin, *plugins;
	yaf_dispatcher_t *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &plugin) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(plugin) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(plugin), yaf_plugin_ce TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expect a %s instance", yaf_plugin_ce->name);
		RETURN_FALSE;
	}

	plugins = zend_read_property(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_PLUGINS), 1 TSRMLS_CC);

	Z_ADDREF_P(plugin);
	add_next_index_zval(plugins, plugin);

	RETVAL_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setRequest(Yaf_Request_Abstract $request) 
*/
PHP_METHOD(yaf_dispatcher, setRequest) {
	yaf_request_t	 *request;
	yaf_dispatcher_t *self;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &request) == FAILURE) {
		return;
	}

	if (IS_OBJECT != Z_TYPE_P(request) 
		   || !instanceof_function(Z_OBJCE_P(request), yaf_request_ce TSRMLS_CC))	{
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Expects a %s instance", yaf_request_ce->name);
		RETURN_FALSE;
	} 
	
	self = getThis();
	if (yaf_dispatcher_set_request(self, request TSRMLS_CC)) {
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::getInstance(void) 
*/
PHP_METHOD(yaf_dispatcher, getInstance) {
	yaf_dispatcher_t *dispatcher = yaf_dispatcher_instance(NULL TSRMLS_CC);
	RETURN_ZVAL(dispatcher, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::getRouter(void) 
*/
PHP_METHOD(yaf_dispatcher, getRouter) {
	yaf_router_t *router = zend_read_property(yaf_dispatcher_ce, getThis(), ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_ROUTER), 1 TSRMLS_CC);
	RETURN_ZVAL(router, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::getRequest(void) 
*/
PHP_METHOD(yaf_dispatcher, getRequest) {
	yaf_request_t *request = zend_read_property(yaf_dispatcher_ce, getThis(), ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_REQUEST), 1 TSRMLS_CC);
	RETURN_ZVAL(request, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::getApplication(void) 
*/
PHP_METHOD(yaf_dispatcher, getApplication) {
	PHP_MN(yaf_application_app)(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::dispatch(yaf_request_t $request)
*/
PHP_METHOD(yaf_dispatcher, dispatch) {
	yaf_request_t 	*request;
	yaf_response_t 	*response;
	yaf_dispatcher_t *self;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &request) == FAILURE) {
		return;
	}

	self = getThis();
	zend_update_property(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_REQUEST), request TSRMLS_CC);
	if ((response = yaf_dispatcher_dispatch(self TSRMLS_CC))) {
		RETURN_ZVAL(response, 1, 1);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::throwException(bool $flag=0) 
*/
PHP_METHOD(yaf_dispatcher, throwException) {
	int flag;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &flag) == FAILURE) {
		return;
	}	

	YAF_G(throw_exception) = flag? 1: 0;
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::catchException(bool $flag=0) 
*/
PHP_METHOD(yaf_dispatcher, catchException) {
	int flag;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &flag) == FAILURE) {
		return;
	}	

	YAF_G(catch_exception) = flag? 1: 0;
	RETURN_ZVAL(getThis(), 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::autoRender(int $flag)                             
 */                                                                                        
PHP_METHOD(yaf_dispatcher, autoRender) {                                                   
    long flag;
    yaf_dispatcher_t *self = getThis();                                                    
                                                                                          
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &flag) == FAILURE) {        
		return;
    }                                                                                     
                                                                                          
    zend_update_property_bool(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_RENDER), flag? 1 : 0 TSRMLS_CC);
                                                                                          
    RETURN_ZVAL(self, 1, 0); 
}                                                                                         
/* }}} */     

/** {{{ proto public Yaf_Dispatcher::initView(string $tpl_dir, array $options = NULL)
*/
PHP_METHOD(yaf_dispatcher, initView) {
	yaf_view_t 	*view 	 = NULL;
	zval 		*tpl_dir = NULL;
	zval 		*options = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &tpl_dir, &options) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	view = yaf_dispatcher_init_view(getThis(), tpl_dir, options TSRMLS_CC);
	if (view) {
		RETURN_ZVAL(view, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setView(Yaf_View_Interface $view)
*/
PHP_METHOD(yaf_dispatcher, setView) {
	yaf_view_t		*view;
	yaf_dispatcher_t *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &view) == FAILURE) {
		return;
	}

	if (view && IS_OBJECT == Z_TYPE_P(view)
			&& instanceof_function(Z_OBJCE_P(view), yaf_view_interface_ce TSRMLS_CC)) {
		zend_update_property(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_VIEW), view TSRMLS_CC);
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setAppDirectory(string $directory) 
*/
PHP_METHOD(yaf_dispatcher, setAppDirectory) {
	int	 len;
	char *directory;
	yaf_dispatcher_t *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &directory, &len) == FAILURE) {
		return;
	}

	if (!len || !IS_ABSOLUTE_PATH(directory, len)) {
		RETURN_FALSE;
	}

	efree(YAF_G(directory));
	
	YAF_G(directory) = estrndup(directory, len);

	RETURN_ZVAL(self, 1, 0);
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setDefaultModule(string $name)
*/
PHP_METHOD(yaf_dispatcher, setDefaultModule) {
	zval *module;
	zval *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &module) == FAILURE) {
		return;
	}

	if (IS_STRING == Z_TYPE_P(module) && Z_STRLEN_P(module) 
			&& yaf_application_is_module_name(Z_STRVAL_P(module), Z_STRLEN_P(module) TSRMLS_CC)) {
		zval *module_std;
		MAKE_STD_ZVAL(module_std);
		ZVAL_STRING(module_std, zend_str_tolower_dup(Z_STRVAL_P(module), Z_STRLEN_P(module)), 0);
		*Z_STRVAL_P(module_std) = toupper(*Z_STRVAL_P(module_std));
		zend_update_property(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_MODULE), module_std TSRMLS_CC);
		
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setDefaultController(string $name)
*/
PHP_METHOD(yaf_dispatcher, setDefaultController) {
	zval *controller;
	zval *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &controller) == FAILURE) {
		return;
	}

	if (IS_STRING == Z_TYPE_P(controller) && Z_STRLEN_P(controller)) {
		zval *controller_std;
		MAKE_STD_ZVAL(controller_std);
		ZVAL_STRING(controller_std, zend_str_tolower_dup(Z_STRVAL_P(controller), Z_STRLEN_P(controller)), 0);
		*Z_STRVAL_P(controller_std) = toupper(*Z_STRVAL_P(controller_std));
		zend_update_property(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_CONTROLLER), controller_std TSRMLS_CC);
		
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::setDefaultAction(string $name)
*/
PHP_METHOD(yaf_dispatcher, setDefaultAction) {
	zval *action;
	zval *self = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &action) == FAILURE) {
		return;
	}

	if (action && IS_STRING == Z_TYPE_P(action) && Z_STRLEN_P(action)) {
		zval *action_lower;
		MAKE_STD_ZVAL(action_lower);
		ZVAL_STRING(action_lower, zend_str_tolower_dup(Z_STRVAL_P(action), Z_STRLEN_P(action)), 0);
		zend_update_property(yaf_dispatcher_ce, self, ZEND_STRL(YAF_DISPATCHER_PROPERTY_NAME_ACTION), action_lower TSRMLS_CC);
		
		RETURN_ZVAL(self, 1, 0);
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Yaf_Dispatcher::__desctruct(void)
*/
PHP_METHOD(yaf_dispatcher, __destruct) {
}
/* }}} */

/** {{{ proto private Yaf_Dispatcher::__clone(void)
*/
PHP_METHOD(yaf_dispatcher, __clone) {
}
/* }}} */

/** {{{ yaf_dispatcher_methods 
*/
zend_function_entry yaf_dispatcher_methods[] = {
	PHP_ME(yaf_dispatcher, __construct, 		NULL, ZEND_ACC_PRIVATE|ZEND_ACC_CTOR) 
	PHP_ME(yaf_dispatcher, __clone,				NULL, ZEND_ACC_PRIVATE|ZEND_ACC_CLONE)
	PHP_ME(yaf_dispatcher, __sleep,				NULL, ZEND_ACC_PRIVATE)
	PHP_ME(yaf_dispatcher, __wakeup,			NULL, ZEND_ACC_PRIVATE)
	PHP_ME(yaf_dispatcher, enableView,			yaf_dispatcher_void_arginfo,  		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, disableView,			yaf_dispatcher_void_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, initView,			yaf_dispatcher_initview_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setView,				yaf_dispatcher_setview_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setRequest,			yaf_dispatcher_setrequest_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getApplication,		yaf_dispatcher_void_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getRouter,			yaf_dispatcher_void_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getRequest,			yaf_dispatcher_void_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setErrorHandler,		yaf_dispatcher_seterrhdler_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setAppDirectory,		yaf_dispatcher_setappdir_arginfo,	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setDefaultModule,	yaf_dispatcher_setmodule_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setDefaultController,yaf_dispatcher_setctrl_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, setDefaultAction,	yaf_dispatcher_setaction_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, returnResponse,		yaf_dispatcher_returnresp_arginfo,  ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, autoRender,			yaf_dispatcher_autorender_arginfo,	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, flushInstantly,		yaf_dispatcher_flush_arginfo, 		ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, getInstance,			yaf_dispatcher_void_arginfo, 		ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(yaf_dispatcher, dispatch,			yaf_dispatcher_dispatch_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, throwException,		yaf_dispatcher_throwex_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, catchException,		yaf_dispatcher_catchex_arginfo, 	ZEND_ACC_PUBLIC)
	PHP_ME(yaf_dispatcher, registerPlugin,		yaf_dispatcher_regplugin_arginfo, 	ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ YAF_STARTUP_FUNCTION
*/
YAF_STARTUP_FUNCTION(dispatcher) {
	zend_class_entry ce;

	YAF_INIT_CLASS_ENTRY(ce, "Yaf_Dispatcher", "Yaf\\Dispatcher", yaf_dispatcher_methods);

	yaf_dispatcher_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);
	yaf_dispatcher_ce->ce_flags |= ZEND_ACC_FINAL_CLASS;

	zend_declare_property_null(yaf_dispatcher_ce, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_ROUTER), 	ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_dispatcher_ce, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_VIEW), 	ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_dispatcher_ce, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_REQUEST), 	ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_dispatcher_ce, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_PLUGINS), 	ZEND_ACC_PROTECTED TSRMLS_CC);

	zend_declare_property_null(yaf_dispatcher_ce, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_INSTANCE), ZEND_ACC_PROTECTED|ZEND_ACC_STATIC TSRMLS_CC);

	zend_declare_property_bool(yaf_dispatcher_ce, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_RENDER),	1,  ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_bool(yaf_dispatcher_ce, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_RETURN),   0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_bool(yaf_dispatcher_ce, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_FLUSH), 	0, ZEND_ACC_PROTECTED TSRMLS_CC);

	zend_declare_property_null(yaf_dispatcher_ce, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_MODULE), 		ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_dispatcher_ce, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_CONTROLLER), 	ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(yaf_dispatcher_ce, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_ACTION), 	 	ZEND_ACC_PROTECTED TSRMLS_CC);

	/*zend_declare_property_null(yaf_dispatcher_ce, YAF_STRL(YAF_DISPATCHER_PROPERTY_NAME_ARGS), 	 	ZEND_ACC_PROTECTED TSRMLS_CC);*/

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
