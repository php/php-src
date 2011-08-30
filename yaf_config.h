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

#ifndef YAF_CONFIG_H
#define YAF_CONFIG_H

#define YAF_EXTRACT_FUNC_NAME			"extract"
#define	YAF_CONFIG_PROPERT_NAME			"_config"
#define YAF_CONFIG_PROPERT_NAME_READONLY "_readonly"

struct _yaf_config_cache {
	long ctime;
	HashTable *data;
};

typedef struct _yaf_config_cache yaf_config_cache;

extern zend_class_entry *yaf_config_ce;

yaf_config_t * yaf_config_instance(yaf_config_t *this_ptr, zval *arg1, zval *arg2 TSRMLS_DC);
void yaf_config_unserialize(yaf_config_t *self, HashTable *data TSRMLS_DC);

#ifndef pestrndup
/* before php-5.2.4, pestrndup is not defined */
#define pestrndup(s, length, persistent) ((persistent)?zend_strndup((s),(length)):estrndup((s),(length)))
#endif

YAF_STARTUP_FUNCTION(config);
#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
