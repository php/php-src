/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
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
  | Author: John Coggeshall <john@php.net>                               |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_TIDY_H
#define PHP_TIDY_H

#include "tidyenum.h"
#include "tidy.h"
#include "buffio.h"

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

#define PHP_TIDY_MODULE_VERSION	"2.0-dev"


#define REMOVE_NEWLINE(_z) _z->value.str.val[_z->value.str.len-1] = '\0'; _z->value.str.len--;

#define TIDYDOC_FROM_OBJECT(tdoc, object) \
	{ \
		PHPTidyObj *obj = (PHPTidyObj*) zend_object_store_get_object(object TSRMLS_CC); \
		tdoc = obj->ptdoc; \
	}

#define TIDY_FETCH_OBJECT	\
	zval *object = getThis();	\
	PHPTidyObj *obj;	\
	if (object) {	\
		if (ZEND_NUM_ARGS()) {	\
			WRONG_PARAM_COUNT;	\
		}	\
	} else {	\
		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, NULL, "O", &object, tidy_ce_doc) == FAILURE) {	\
			RETURN_FALSE;	\
		}	\
	}	\
	obj = (PHPTidyObj *) zend_object_store_get_object(object TSRMLS_CC);	\

#define TIDY_FETCH_ONLY_OBJECT	\
	zval *object = getThis();	\
	PHPTidyObj *obj;	\
	if (ZEND_NUM_ARGS()) {	\
		WRONG_PARAM_COUNT;	\
	}	\
	obj = (PHPTidyObj *) zend_object_store_get_object(object TSRMLS_CC);	\

#define TIDY_FETCH_PARSED_OBJECT	\
	TIDY_FETCH_OBJECT;	\
	TIDY_PARSED_CHECK(obj);	\

#define Z_OBJ_P(zval_p) zend_objects_get_address(zval_p TSRMLS_CC)

#define TIDY_METHOD_MAP(name, func_name, arg_types) \
	ZEND_NAMED_FE(name, ZEND_FN(func_name), arg_types)

#define TIDY_THROW(message) \
	PG(suppress_errors) = 0; \
	EG(exception) = zend_throw_exception(tidy_ce_exception, message, 0 TSRMLS_CC);

#define TIDY_NODE_METHOD(name)    PHP_FUNCTION(tnm_ ##name)
#define TIDY_NODE_ME(name, param)      TIDY_METHOD_MAP(name, tnm_ ##name, param)
#define TIDY_DOC_METHOD(name)     PHP_FUNCTION(tdm_ ##name)
#define TIDY_DOC_ME(name, param)  TIDY_METHOD_MAP(name, tdm_ ##name, param)
#define TIDY_ATTR_METHOD(name)    PHP_FUNCTION(tam_ ##name)
#define TIDY_ATTR_ME(name, param) TIDY_METHOD_MAP(name, tam_ ##name, param)

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

#define TIDY_TAG_CONST(tag) REGISTER_LONG_CONSTANT("TIDY_TAG_" #tag, TidyTag_##tag, CONST_CS | CONST_PERSISTENT)
#define TIDY_ATTR_CONST(attr) REGISTER_LONG_CONSTANT("TIDY_ATTR_" #attr, TidyAttr_##attr, CONST_CS | CONST_PERSISTENT)
#define TIDY_NODE_CONST(name, type) REGISTER_LONG_CONSTANT("TIDY_NODETYPE_" #name, TidyNode_##type, CONST_CS | CONST_PERSISTENT)

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define ADD_PROPERTY_STRING(_table, _key, _string) \
	{ \
		zval *tmp; \
		MAKE_STD_ZVAL(tmp); \
		if (_string) { \
			ZVAL_STRING(tmp, (char *)_string, 1); \
		} else { \
			ZVAL_EMPTY_STRING(tmp); \
		} \
		zend_hash_update(_table, #_key, sizeof(#_key), (void *)&tmp, sizeof(zval *), NULL); \
	}

#define ADD_PROPERTY_LONG(_table, _key, _long) \
	{ \
		zval *tmp; \
		MAKE_STD_ZVAL(tmp); \
		ZVAL_LONG(tmp, _long); \
		zend_hash_update(_table, #_key, sizeof(#_key), (void *)&tmp, sizeof(zval *), NULL); \
	}

#define ADD_PROPERTY_NULL(_table, _key) \
	{ \
		zval *tmp; \
		MAKE_STD_ZVAL(tmp); \
		ZVAL_NULL(tmp); \
		zend_hash_update(_table, #_key, sizeof(#_key), (void *)&tmp, sizeof(zval *), NULL); \
	}

#define TIDY_PARSED_CHECK(_obj) \
if (!_obj->ptdoc->parsed) { \
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "A document must be parsed before executing this function."); \
	RETURN_FALSE; \
}

#define TIDY_PARSED_REPAIR_CHECK(_obj) \
TIDY_PARSED_CHECK(_obj); \
if (!_obj->ptdoc->repaired) { \
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "You must clean and repair the document before executing this function."); \
	RETURN_FALSE; \
}

#define TIDY_SAFE_MODE_CHECK(filename) \
if ((PG(safe_mode) && (!php_checkuid(filename, NULL, CHECKUID_CHECK_FILE_AND_DIR))) || php_check_open_basedir(filename TSRMLS_CC)) { \
	RETURN_FALSE; \
} \

#define TIDY_SET_DEFAULT_CONFIG(_doc) \
	if (TG(default_config) && TG(default_config)[0]) { \
		if (tidyLoadConfig(_doc, TG(default_config)) < 0) { \
			zend_error(E_ERROR, "Unable to load Tidy configuration file at '%s'.", TG(default_config)); \
		} \
	}

typedef struct _PHPTidyDoc PHPTidyDoc;
typedef struct _PHPTidyObj PHPTidyObj;

typedef enum {
	is_node,
	is_attr,
	is_doc,
	is_exception
} tidy_obj_type;

typedef enum {
	is_root_node,
	is_html_node,
	is_head_node,
	is_body_node
} tidy_base_nodetypes;

struct _PHPTidyDoc {
	TidyDoc     doc;
	TidyBuffer  *errbuf;
	zend_bool   parsed;
	zend_bool   repaired;
	unsigned int ref_count;
};

struct _PHPTidyObj {
	zend_object         std;
	TidyNode            node;
	TidyAttr            attr;
	tidy_obj_type       type;
	PHPTidyDoc          *ptdoc;
};

static char *php_tidy_file_to_mem(char *, zend_bool TSRMLS_DC);
static void tidy_object_dtor(void *, zend_object_handle  TSRMLS_DC);
static zend_object_value tidy_object_new_node(zend_class_entry * TSRMLS_DC);
static zend_object_value tidy_object_new_attr(zend_class_entry * TSRMLS_DC);
static zend_object_value tidy_object_new_doc(zend_class_entry * TSRMLS_DC);
static zend_object_value tidy_object_new_exception(zend_class_entry * TSRMLS_DC);
static zend_class_entry *tidy_get_ce_node(zval * TSRMLS_DC);
static zend_class_entry *tidy_get_ce_attr(zval * TSRMLS_DC);
static zend_class_entry *tidy_get_ce_doc(zval * TSRMLS_DC);
static zval * tidy_instanciate(zend_class_entry *, zval * TSRMLS_DC);
static int tidy_doc_cast_handler(zval *, zval *, int, int TSRMLS_DC);
static int tidy_node_cast_handler(zval *, zval *, int, int TSRMLS_DC);
static void tidy_doc_update_properties(PHPTidyObj * TSRMLS_DC);
static void tidy_add_default_properties(PHPTidyObj *, tidy_obj_type TSRMLS_DC);
static void *php_tidy_get_opt_val(PHPTidyDoc *, TidyOption, TidyOptionType * TSRMLS_DC);
static void php_tidy_create_node(INTERNAL_FUNCTION_PARAMETERS, tidy_base_nodetypes);

void _php_tidy_register_nodetypes(INIT_FUNC_ARGS);
void _php_tidy_register_tags(INIT_FUNC_ARGS);
void _php_tidy_register_attributes(INIT_FUNC_ARGS);

PHP_MINIT_FUNCTION(tidy);
PHP_MSHUTDOWN_FUNCTION(tidy);
PHP_RINIT_FUNCTION(tidy);
PHP_RSHUTDOWN_FUNCTION(tidy);
PHP_MINFO_FUNCTION(tidy);

PHP_FUNCTION(tidy_reset_config);
PHP_FUNCTION(tidy_restore_config);
PHP_FUNCTION(tidy_snapshot_config);
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

PHP_FUNCTION(ob_tidyhandler);

PHP_FUNCTION(tidy_get_root);
PHP_FUNCTION(tidy_get_html);
PHP_FUNCTION(tidy_get_head);
PHP_FUNCTION(tidy_get_body);

TIDY_ATTR_METHOD(__construct);

TIDY_NODE_METHOD(__construct);
TIDY_NODE_METHOD(has_children);
TIDY_NODE_METHOD(has_siblings);
TIDY_NODE_METHOD(is_comment);
TIDY_NODE_METHOD(is_html);
TIDY_NODE_METHOD(is_xhtml);
TIDY_NODE_METHOD(is_xml);
TIDY_NODE_METHOD(is_text);
TIDY_NODE_METHOD(is_jste);
TIDY_NODE_METHOD(is_asp);
TIDY_NODE_METHOD(is_php);

ZEND_BEGIN_MODULE_GLOBALS(tidy)
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
