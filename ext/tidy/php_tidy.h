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

#define PHP_ME_MAPPING(name, func_name, arg_types) \
	ZEND_NAMED_FE(name, ZEND_FN(func_name), arg_types)

#define PHP_NODE_METHOD(name)    PHP_FUNCTION(tnm_ ##name)
#define PHP_ATTR_METHOD(name)    PHP_FUNCTION(tam_ ##name)
#define PHP_NODE_ME(name, param)      PHP_ME_MAPPING(name, tnm_ ##name, param)
#define PHP_ATTR_ME(name, param)      PHP_ME_MAPPING(name, tam_ ##name, param)



#define TIDY_REGISTER_OBJECT(_type, _object, _ptr) \
	{ \
		tidy_object *obj; \
		obj = (tidy_object*)zend_object_store_get_object(_object TSRMLS_CC); \
		obj->type = is_ ## _type; \
		obj->u._type = _ptr; \
	}

#define REGISTER_TIDY_CLASS(name, parent) \
	{ \
		zend_class_entry ce; \
		INIT_CLASS_ENTRY(ce, "tidy_" # name, tidy_funcs_ ## name); \
		ce.create_object = tidy_object_new_ ## name; \
		tidy_ce_ ## name = zend_register_internal_class_ex(&ce, parent, NULL TSRMLS_CC); \
		tidy_ce_ ## name->ce_flags |= ZEND_ACC_FINAL_CLASS; \
        memcpy(&tidy_object_handlers_ ## name, zend_get_std_object_handlers(), sizeof(zend_object_handlers)); \
		tidy_object_handlers_ ## name.clone_obj = NULL; \
    }

#define GET_THIS_CONTAINER() \
    PHPTidyObj *obj; \
    { \
        zval *object = getThis(); \
        obj = (PHPTidyObj *)zend_object_store_get_object(object TSRMLS_CC); \
    }

#define INSTANCIATE_NODE(_zval, _container, _node) \
	tidy_instanciate(tidy_ce_node, _zval TSRMLS_CC); \
	_container = (PHPTidyObj *) zend_object_store_get_object(_zval TSRMLS_CC); \
	_container->node = _node; \
	_container->attr = NULL; \
	_container->type = is_node; \
	tidy_add_default_properities(_container, is_node TSRMLS_CC);

#define INSTANCIATE_ATTR(_zval, _container, _attr) \
	tidy_instanciate(tidy_ce_attr, _zval TSRMLS_CC); \
	_container = (PHPTidyObj *) zend_object_store_get_object(_zval TSRMLS_CC); \
	_container->node = NULL; \
	_container->attr = _attr; \
	_container->type = is_attr; \
	tidy_add_default_properities(_container, is_attr TSRMLS_CC);

#define PHP_NODE_METHOD_IS_TYPE(_type, _const) \
PHP_NODE_METHOD(is_ ##_type) \
{ \
	GET_THIS_CONTAINER(); \
	if(tidyNodeGetType(obj->node) == _const) {\
		RETURN_TRUE; \
	} else { \
		RETURN_FALSE; \
	} \
}

typedef enum {
    is_node,
    is_attr
} tidy_obj_type;

struct _PHPTidyDoc {
    
    TidyDoc     doc;
    TidyBuffer  *errbuf;
    zend_bool   parsed;
};

typedef struct _PHPTidyDoc PHPTidyDoc;
typedef struct _PHPTidyObj PHPTidyObj;

struct _PHPTidyObj {
    zend_object         std;
    TidyNode            node;
    TidyAttr            attr;
    tidy_obj_type       type;
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

PHP_FUNCTION(tidy_get_root);
PHP_FUNCTION(tidy_get_html);
PHP_FUNCTION(tidy_get_head);
PHP_FUNCTION(tidy_get_body);

PHP_NODE_METHOD(__construct);
PHP_NODE_METHOD(attributes);
PHP_NODE_METHOD(children);

PHP_NODE_METHOD(has_children);
PHP_NODE_METHOD(has_siblings);
PHP_NODE_METHOD(is_comment);
PHP_NODE_METHOD(is_html);
PHP_NODE_METHOD(is_xhtml);
PHP_NODE_METHOD(is_xml);
PHP_NODE_METHOD(is_text);
PHP_NODE_METHOD(is_jste);
PHP_NODE_METHOD(is_asp);
PHP_NODE_METHOD(is_php);

PHP_NODE_METHOD(next);
PHP_NODE_METHOD(prev);
PHP_NODE_METHOD(get_attr);
PHP_NODE_METHOD(get_nodes);

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
