/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: John Coggeshall  <john@php.net>                              |
  |         Ilia Alshanetsky <ilia@php.net>				 |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_TIDY_H
#define PHP_TIDY_H

extern zend_module_entry tidy_module_entry;
#define phpext_tidy_ptr &tidy_module_entry

#ifdef PHP_WIN32
#define PHP_TIDY_API __declspec(dllexport)
#else
#define PHP_TIDY_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include "tidyenum.h"
#include "tidy.h"
#include "buffio.h"

#ifdef ZTS
#define TG(v) TSRMG(tidy_globals_id, zend_tidy_globals *, v)
#else
#define TG(v) (tidy_globals.v)
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define TIDY_RV_FALSE(__t) __t->type = IS_BOOL; __t->value.lval = FALSE
#define TIDY_RV_TRUE(__t) __t->type = IS_BOOL; __t->value.lval = TRUE

#define REMOVE_NEWLINE(_z) _z->value.str.val[_z->value.str.len-1] = '\0'; _z->value.str.len--;

#define TIDY_TAG_CONST(tag) REGISTER_LONG_CONSTANT("TIDY_TAG_" #tag, TidyTag_##tag, CONST_CS | CONST_PERSISTENT)
#define TIDY_ATTR_CONST(attr) REGISTER_LONG_CONSTANT("TIDY_ATTR_" #attr, TidyAttr_##attr, CONST_CS | CONST_PERSISTENT)
#define TIDY_NODE_CONST(name, type) REGISTER_LONG_CONSTANT("TIDY_NODETYPE_" #name, TidyNode_##type, CONST_CS | CONST_PERSISTENT)

#define PHP_IS_TIDYUNDEF    0
#define PHP_IS_TIDYNODE     1
#define PHP_IS_TIDYATTR     2


struct _PHPTidyDoc {
    
    TidyDoc     doc;
    TidyBuffer  *errbuf;
    zend_bool   parsed;
};

typedef struct _PHPTidyDoc PHPTidyDoc;
typedef struct _PHPTidyObj PHPTidyObj;

struct _PHPTidyObj {
    zend_object         obj;
    TidyNode            node;
    TidyAttr            attr;
    unsigned int        type;
    PHPTidyObj          *parent;
    unsigned int        refcount;
};


PHP_MINIT_FUNCTION(tidy);
PHP_MSHUTDOWN_FUNCTION(tidy);
PHP_RINIT_FUNCTION(tidy);
PHP_RSHUTDOWN_FUNCTION(tidy);
PHP_MINFO_FUNCTION(tidy);

PHP_FUNCTION(tidy_setopt);
PHP_FUNCTION(tidy_getopt);
PHP_FUNCTION(tidy_parse_string);
PHP_FUNCTION(tidy_parse_file);
PHP_FUNCTION(tidy_clean_repair);
PHP_FUNCTION(tidy_repair_string);
PHP_FUNCTION(tidy_repair_file);
PHP_FUNCTION(tidy_diagnose);
PHP_FUNCTION(tidy_get_output);
PHP_FUNCTION(tidy_get_error_buffer);
PHP_FUNCTION(tidy_get_release);
PHP_FUNCTION(tidy_reset_config);
PHP_FUNCTION(tidy_get_config);
PHP_FUNCTION(tidy_get_status);
PHP_FUNCTION(tidy_get_html_ver);
PHP_FUNCTION(tidy_is_xhtml);
PHP_FUNCTION(tidy_is_xml);
PHP_FUNCTION(tidy_error_count);
PHP_FUNCTION(tidy_warning_count);
PHP_FUNCTION(tidy_access_count);
PHP_FUNCTION(tidy_config_count);
PHP_FUNCTION(tidy_load_config);
PHP_FUNCTION(tidy_load_config_enc);
PHP_FUNCTION(tidy_set_encoding);
PHP_FUNCTION(tidy_save_config);

#ifdef ZEND_ENGINE_2

PHP_FUNCTION(tidy_get_root);
PHP_FUNCTION(tidy_get_html);
PHP_FUNCTION(tidy_get_head);
PHP_FUNCTION(tidy_get_body);

void php_tidy_obj_clone(void *, void ** TSRMLS_DC);
void php_tidy_obj_dtor(void *, zend_object_handle TSRMLS_DC);

zend_object_value php_tidy_create_obj(zend_class_entry * TSRMLS_DC);
zend_object_value php_tidy_register_object(PHPTidyObj *intern TSRMLS_DC);
zval *_php_tidy_create_obj_zval(unsigned int objtype,
                                        PHPTidyObj *parent,
                                        void *data
                                        TSRMLS_DC);

void _php_tidy_create_obj(zval *return_value,
                                unsigned int objtype,
                                PHPTidyObj *parent,
                                void *data
                                TSRMLS_DC);
/* object handlers */
zval * tidy_property_read(zval *object, zval *member, zend_bool silent TSRMLS_DC);
void tidy_property_write(zval *obj, zval *member, zval *value TSRMLS_DC);
zval ** tidy_property_get_ptr(zval *obj, zval *member TSRMLS_DC);
int tidy_property_exists(zval *object, zval *member, int check_empty TSRMLS_DC);
void tidy_property_delete(zval *obj, zval *member TSRMLS_DC);
HashTable * tidy_get_properties(zval *object TSRMLS_DC);
union _zend_function * tidy_get_method(zval *obj, char *method, int method_len TSRMLS_DC);
int tidy_call_method(char *method, INTERNAL_FUNCTION_PARAMETERS);
union _zend_function * tidy_get_constructor(zval *obj TSRMLS_DC);
zend_class_entry * tidy_get_class_entry(zval *obj TSRMLS_DC);
int tidy_get_class_name(zval *obj, char **class_name, zend_uint *name_len, int parent TSRMLS_DC);
int tidy_objects_compare(zval *obj_one, zval *obj_two TSRMLS_DC);
void tidy_object_cast(zval *readobj, zval *writeobj, int type, int should_free TSRMLS_DC);
void tidy_write_dim(zval *object, zval *offset, zval *value TSRMLS_DC);
void tidy_del_dim(zval *object, zval *offset TSRMLS_DC);
zval *tidy_read_dim(zval *object, zval *offset TSRMLS_DC);

zend_bool _php_tidy_attr_call_method(char *method, INTERNAL_FUNCTION_PARAMETERS);
zend_bool _php_tidy_node_call_method(char *method, INTERNAL_FUNCTION_PARAMETERS);
void _php_tidy_init_prop_hashtables();

#endif

/* resource dtor */
void dtor_TidyDoc(zend_rsrc_list_entry * TSRMLS_DC);

/* constant register helpers */
void _php_tidy_register_nodetypes(INIT_FUNC_ARGS);
void _php_tidy_register_tags(INIT_FUNC_ARGS);
void _php_tidy_register_attributes(INIT_FUNC_ARGS);

ZEND_BEGIN_MODULE_GLOBALS(tidy)
	PHPTidyDoc *tdoc;
	zend_bool used;
	char *default_config;
ZEND_END_MODULE_GLOBALS(tidy)

#ifdef ZTS
#define TG(v) TSRMG(tidy_globals_id, zend_tidy_globals *, v)
#else
#define TG(v) (tidy_globals.v)
#endif

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
