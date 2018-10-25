/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Pierre-Alain Joye <paj@pearfr.org>                           |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_ENCHANT_H
#define PHP_ENCHANT_H

extern zend_module_entry enchant_module_entry;
#define phpext_enchant_ptr &enchant_module_entry

#define PHP_ENCHANT_VERSION PHP_VERSION

#ifdef PHP_WIN32
#define PHP_ENCHANT_API __declspec(dllexport)
#else
#define PHP_ENCHANT_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(enchant);
PHP_MSHUTDOWN_FUNCTION(enchant);
PHP_MINFO_FUNCTION(enchant);

PHP_FUNCTION(enchant_broker_init);
PHP_FUNCTION(enchant_broker_free);
PHP_FUNCTION(enchant_broker_get_error);
PHP_FUNCTION(enchant_broker_set_dict_path);
PHP_FUNCTION(enchant_broker_get_dict_path);
PHP_FUNCTION(enchant_broker_list_dicts);
PHP_FUNCTION(enchant_broker_request_dict);
PHP_FUNCTION(enchant_broker_request_pwl_dict);
PHP_FUNCTION(enchant_broker_free_dict);
PHP_FUNCTION(enchant_broker_dict_exists);
PHP_FUNCTION(enchant_broker_set_ordering);
PHP_FUNCTION(enchant_broker_describe);

PHP_FUNCTION(enchant_dict_check);
PHP_FUNCTION(enchant_dict_suggest);
PHP_FUNCTION(enchant_dict_add_to_personal);
PHP_FUNCTION(enchant_dict_add_to_session);
PHP_FUNCTION(enchant_dict_is_in_session);
PHP_FUNCTION(enchant_dict_store_replacement);
PHP_FUNCTION(enchant_dict_get_error);
PHP_FUNCTION(enchant_dict_describe);
PHP_FUNCTION(enchant_dict_quick_check);

#ifdef ZTS
#define ENCHANT_G(v) TSRMG(enchant_globals_id, zend_enchant_globals *, v)
#else
#define ENCHANT_G(v) (enchant_globals.v)
#endif

#endif	/* PHP_ENCHANT_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
