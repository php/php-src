/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: John Coggeshall <john@php.net>                               |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_tidy.h"

#if HAVE_TIDY

#include "php_ini.h"
#include "ext/standard/info.h"

#include "tidy.h"
#include "buffio.h"

/* compatibility with older versions of libtidy */
#ifndef TIDY_CALL
#define TIDY_CALL
#endif

#define PHP_TIDY_MODULE_VERSION	"2.0"

/* {{{ ext/tidy macros
*/
#define FIX_BUFFER(bptr) do { if ((bptr)->size) { (bptr)->bp[(bptr)->size-1] = '\0'; } } while(0)

#define TIDY_SET_CONTEXT \
    zval *object = getThis();

#define TIDY_FETCH_OBJECT	\
	PHPTidyObj *obj;	\
	TIDY_SET_CONTEXT; \
	if (object) {	\
		if (zend_parse_parameters_none() == FAILURE) {	\
			return;	\
		}	\
	} else {	\
		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, NULL, "O", &object, tidy_ce_doc) == FAILURE) {	\
			RETURN_FALSE;	\
		}	\
	}	\
	obj = (PHPTidyObj *) zend_object_store_get_object(object TSRMLS_CC);	\

#define TIDY_FETCH_ONLY_OBJECT	\
	PHPTidyObj *obj;	\
	TIDY_SET_CONTEXT; \
	if (zend_parse_parameters_none() == FAILURE) {	\
		return;	\
	}	\
	obj = (PHPTidyObj *) zend_object_store_get_object(object TSRMLS_CC);	\

#define TIDY_APPLY_CONFIG_ZVAL(_doc, _val) \
    if(_val) { \
        if(Z_TYPE_PP(_val) == IS_ARRAY) { \
            _php_tidy_apply_config_array(_doc, HASH_OF(*_val) TSRMLS_CC); \
        } else { \
            convert_to_string_ex(_val); \
            TIDY_OPEN_BASE_DIR_CHECK(Z_STRVAL_PP(_val)); \
            switch (tidyLoadConfig(_doc, Z_STRVAL_PP(_val))) { \
              case -1: \
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not load configuration file '%s'", Z_STRVAL_PP(_val)); \
                break; \
              case 1: \
                php_error_docref(NULL TSRMLS_CC, E_NOTICE, "There were errors while parsing the configuration file '%s'", Z_STRVAL_PP(_val)); \
                break; \
            } \
        } \
    }

#define REGISTER_TIDY_CLASS(classname, name, parent, __flags) \
	{ \
		zend_class_entry ce; \
		INIT_CLASS_ENTRY(ce, # classname, tidy_funcs_ ## name); \
		ce.create_object = tidy_object_new_ ## name; \
		tidy_ce_ ## name = zend_register_internal_class_ex(&ce, parent, NULL TSRMLS_CC); \
		tidy_ce_ ## name->ce_flags |= __flags;  \
		memcpy(&tidy_object_handlers_ ## name, zend_get_std_object_handlers(), sizeof(zend_object_handlers)); \
		tidy_object_handlers_ ## name.clone_obj = NULL; \
	}

#define TIDY_TAG_CONST(tag) REGISTER_LONG_CONSTANT("TIDY_TAG_" #tag, TidyTag_##tag, CONST_CS | CONST_PERSISTENT)
#define TIDY_NODE_CONST(name, type) REGISTER_LONG_CONSTANT("TIDY_NODETYPE_" #name, TidyNode_##type, CONST_CS | CONST_PERSISTENT)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
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

#define ADD_PROPERTY_STRINGL(_table, _key, _string, _len) \
   { \
       zval *tmp; \
       MAKE_STD_ZVAL(tmp); \
       if (_string) { \
           ZVAL_STRINGL(tmp, (char *)_string, _len, 1); \
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

#define ADD_PROPERTY_BOOL(_table, _key, _bool) \
    { \
       zval *tmp; \
       MAKE_STD_ZVAL(tmp); \
       ZVAL_BOOL(tmp, _bool); \
       zend_hash_update(_table, #_key, sizeof(#_key), (void *)&tmp, sizeof(zval *), NULL); \
   }

#define TIDY_OPEN_BASE_DIR_CHECK(filename) \
if (php_check_open_basedir(filename TSRMLS_CC)) { \
	RETURN_FALSE; \
} \

#define TIDY_SET_DEFAULT_CONFIG(_doc) \
	if (TG(default_config) && TG(default_config)[0]) { \
		if (tidyLoadConfig(_doc, TG(default_config)) < 0) { \
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to load Tidy configuration file at '%s'.", TG(default_config)); \
		} \
	}
/* }}} */

/* {{{ ext/tidy structs 
*/
typedef struct _PHPTidyDoc PHPTidyDoc;
typedef struct _PHPTidyObj PHPTidyObj;

typedef enum {
	is_node,
	is_doc
} tidy_obj_type;

typedef enum {
	is_root_node,
	is_html_node,
	is_head_node,
	is_body_node
} tidy_base_nodetypes;

struct _PHPTidyDoc {
	TidyDoc			doc;
	TidyBuffer		*errbuf;
	unsigned int	ref_count;
	unsigned int    initialized:1;
};

struct _PHPTidyObj {
	zend_object		std;
	TidyNode		node;
	tidy_obj_type	type;
	PHPTidyDoc		*ptdoc;
};
/* }}} */

/* {{{ ext/tidy prototypes
*/
static char *php_tidy_file_to_mem(char *, zend_bool, int * TSRMLS_DC);
static void tidy_object_free_storage(void * TSRMLS_DC);
static zend_object_value tidy_object_new_node(zend_class_entry * TSRMLS_DC);
static zend_object_value tidy_object_new_doc(zend_class_entry * TSRMLS_DC);
static zval * tidy_instanciate(zend_class_entry *, zval * TSRMLS_DC);
static int tidy_doc_cast_handler(zval *, zval *, int TSRMLS_DC);
static int tidy_node_cast_handler(zval *, zval *, int TSRMLS_DC);
static void tidy_doc_update_properties(PHPTidyObj * TSRMLS_DC);
static void tidy_add_default_properties(PHPTidyObj *, tidy_obj_type TSRMLS_DC);
static void *php_tidy_get_opt_val(PHPTidyDoc *, TidyOption, TidyOptionType * TSRMLS_DC);
static void php_tidy_create_node(INTERNAL_FUNCTION_PARAMETERS, tidy_base_nodetypes);
static int _php_tidy_set_tidy_opt(TidyDoc, char *, zval * TSRMLS_DC);
static int _php_tidy_apply_config_array(TidyDoc doc, HashTable *ht_options TSRMLS_DC);
static void _php_tidy_register_nodetypes(INIT_FUNC_ARGS);
static void _php_tidy_register_tags(INIT_FUNC_ARGS);
static PHP_INI_MH(php_tidy_set_clean_output);
static void php_tidy_clean_output_start(const char *name, size_t name_len TSRMLS_DC);
static php_output_handler *php_tidy_output_handler_init(const char *handler_name, size_t handler_name_len, size_t chunk_size, int flags TSRMLS_DC);
static int php_tidy_output_handler(void **nothing, php_output_context *output_context);

static PHP_MINIT_FUNCTION(tidy);
static PHP_MSHUTDOWN_FUNCTION(tidy);
static PHP_RINIT_FUNCTION(tidy);
static PHP_MINFO_FUNCTION(tidy);

static PHP_FUNCTION(tidy_getopt);
static PHP_FUNCTION(tidy_parse_string);
static PHP_FUNCTION(tidy_parse_file);
static PHP_FUNCTION(tidy_clean_repair);
static PHP_FUNCTION(tidy_repair_string);
static PHP_FUNCTION(tidy_repair_file);
static PHP_FUNCTION(tidy_diagnose);
static PHP_FUNCTION(tidy_get_output);
static PHP_FUNCTION(tidy_get_error_buffer);
static PHP_FUNCTION(tidy_get_release);
static PHP_FUNCTION(tidy_get_config);
static PHP_FUNCTION(tidy_get_status);
static PHP_FUNCTION(tidy_get_html_ver);
#if HAVE_TIDYOPTGETDOC
static PHP_FUNCTION(tidy_get_opt_doc);
#endif
static PHP_FUNCTION(tidy_is_xhtml);
static PHP_FUNCTION(tidy_is_xml);
static PHP_FUNCTION(tidy_error_count);
static PHP_FUNCTION(tidy_warning_count);
static PHP_FUNCTION(tidy_access_count);
static PHP_FUNCTION(tidy_config_count);

static PHP_FUNCTION(tidy_get_root);
static PHP_FUNCTION(tidy_get_html);
static PHP_FUNCTION(tidy_get_head);
static PHP_FUNCTION(tidy_get_body);

static TIDY_DOC_METHOD(__construct);
static TIDY_DOC_METHOD(parseFile);
static TIDY_DOC_METHOD(parseString);

static TIDY_NODE_METHOD(hasChildren);
static TIDY_NODE_METHOD(hasSiblings);
static TIDY_NODE_METHOD(isComment);
static TIDY_NODE_METHOD(isHtml);
static TIDY_NODE_METHOD(isText);
static TIDY_NODE_METHOD(isJste);
static TIDY_NODE_METHOD(isAsp);
static TIDY_NODE_METHOD(isPhp);
static TIDY_NODE_METHOD(getParent);
static TIDY_NODE_METHOD(__construct);
/* }}} */

ZEND_DECLARE_MODULE_GLOBALS(tidy)

PHP_INI_BEGIN()
STD_PHP_INI_ENTRY("tidy.default_config",	"",		PHP_INI_SYSTEM,		OnUpdateString,				default_config,		zend_tidy_globals,	tidy_globals)
STD_PHP_INI_ENTRY("tidy.clean_output",		"0",	PHP_INI_USER,		php_tidy_set_clean_output,	clean_output,		zend_tidy_globals,	tidy_globals)
PHP_INI_END()

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_tidy_parse_string, 0, 0, 1)
	ZEND_ARG_INFO(0, input)
	ZEND_ARG_INFO(0, config_options)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_get_error_buffer, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_get_output, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tidy_parse_file, 0, 0, 1)
	ZEND_ARG_INFO(0, file)
	ZEND_ARG_INFO(0, config_options)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_INFO(0, use_include_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_clean_repair, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tidy_repair_string, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
	ZEND_ARG_INFO(0, config_file)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tidy_repair_file, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, config_file)
	ZEND_ARG_INFO(0, encoding)
	ZEND_ARG_INFO(0, use_include_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_diagnose, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_get_release, 0)
ZEND_END_ARG_INFO()

#if HAVE_TIDYOPTGETDOC
ZEND_BEGIN_ARG_INFO_EX(arginfo_tidy_get_opt_doc, 0, 0, 2)
	ZEND_ARG_INFO(0, resource)
	ZEND_ARG_INFO(0, optname)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO(arginfo_tidy_get_config, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_get_status, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_get_html_ver, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_is_xhtml, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_is_xml, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_error_count, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_warning_count, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_access_count, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_config_count, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tidy_getopt, 0, 0, 1)
	ZEND_ARG_INFO(0, option)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_get_root, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_get_html, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tidy_get_head, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_tidy_get_body, 0, 0, 1)
	ZEND_ARG_INFO(0, tidy)
ZEND_END_ARG_INFO()
/* }}} */

static const zend_function_entry tidy_functions[] = {
	PHP_FE(tidy_getopt,             arginfo_tidy_getopt)
	PHP_FE(tidy_parse_string,       arginfo_tidy_parse_string)
	PHP_FE(tidy_parse_file,         arginfo_tidy_parse_file)
	PHP_FE(tidy_get_output,         arginfo_tidy_get_output)
	PHP_FE(tidy_get_error_buffer,   arginfo_tidy_get_error_buffer) 
	PHP_FE(tidy_clean_repair,       arginfo_tidy_clean_repair)
	PHP_FE(tidy_repair_string,	arginfo_tidy_repair_string)
	PHP_FE(tidy_repair_file,	arginfo_tidy_repair_file)
	PHP_FE(tidy_diagnose,           arginfo_tidy_diagnose)
	PHP_FE(tidy_get_release,	arginfo_tidy_get_release)
	PHP_FE(tidy_get_config,		arginfo_tidy_get_config)
	PHP_FE(tidy_get_status,		arginfo_tidy_get_status)
	PHP_FE(tidy_get_html_ver,	arginfo_tidy_get_html_ver)
	PHP_FE(tidy_is_xhtml,		arginfo_tidy_is_xhtml)
	PHP_FE(tidy_is_xml,		arginfo_tidy_is_xml)
	PHP_FE(tidy_error_count,	arginfo_tidy_error_count)
	PHP_FE(tidy_warning_count,	arginfo_tidy_warning_count)
	PHP_FE(tidy_access_count,	arginfo_tidy_access_count)
	PHP_FE(tidy_config_count,	arginfo_tidy_config_count) 
#if HAVE_TIDYOPTGETDOC
	PHP_FE(tidy_get_opt_doc,	arginfo_tidy_get_opt_doc)
#endif
	PHP_FE(tidy_get_root,		arginfo_tidy_get_root)
	PHP_FE(tidy_get_head,		arginfo_tidy_get_head)
	PHP_FE(tidy_get_html,		arginfo_tidy_get_html)
	PHP_FE(tidy_get_body,		arginfo_tidy_get_body)
	PHP_FE_END
};

static const zend_function_entry tidy_funcs_doc[] = {
	TIDY_METHOD_MAP(getOpt, tidy_getopt, NULL)
	TIDY_METHOD_MAP(cleanRepair, tidy_clean_repair, NULL)
	TIDY_DOC_ME(parseFile, NULL)
	TIDY_DOC_ME(parseString, NULL)
	TIDY_METHOD_MAP(repairString, tidy_repair_string, NULL)
	TIDY_METHOD_MAP(repairFile, tidy_repair_file, NULL)
	TIDY_METHOD_MAP(diagnose, tidy_diagnose, NULL)
	TIDY_METHOD_MAP(getRelease, tidy_get_release, NULL)
	TIDY_METHOD_MAP(getConfig, tidy_get_config, NULL)
	TIDY_METHOD_MAP(getStatus, tidy_get_status, NULL)
	TIDY_METHOD_MAP(getHtmlVer, tidy_get_html_ver, NULL)
#if HAVE_TIDYOPTGETDOC
	TIDY_METHOD_MAP(getOptDoc, tidy_get_opt_doc, NULL)
#endif
	TIDY_METHOD_MAP(isXhtml, tidy_is_xhtml, NULL)
	TIDY_METHOD_MAP(isXml, tidy_is_xml, NULL)
	TIDY_METHOD_MAP(root, tidy_get_root, NULL)
	TIDY_METHOD_MAP(head, tidy_get_head, NULL)
	TIDY_METHOD_MAP(html, tidy_get_html, NULL)
	TIDY_METHOD_MAP(body, tidy_get_body, NULL)
	TIDY_DOC_ME(__construct, NULL)
	PHP_FE_END
};

static const zend_function_entry tidy_funcs_node[] = {
	TIDY_NODE_ME(hasChildren, NULL)
	TIDY_NODE_ME(hasSiblings, NULL)
	TIDY_NODE_ME(isComment, NULL)
	TIDY_NODE_ME(isHtml, NULL)
	TIDY_NODE_ME(isText, NULL)
	TIDY_NODE_ME(isJste, NULL)
	TIDY_NODE_ME(isAsp, NULL)
	TIDY_NODE_ME(isPhp, NULL)
	TIDY_NODE_ME(getParent, NULL)
	TIDY_NODE_PRIVATE_ME(__construct, NULL)
	PHP_FE_END
};

static zend_class_entry *tidy_ce_doc, *tidy_ce_node;

static zend_object_handlers tidy_object_handlers_doc;
static zend_object_handlers tidy_object_handlers_node;

zend_module_entry tidy_module_entry = {
	STANDARD_MODULE_HEADER,
	"tidy",
	tidy_functions,
	PHP_MINIT(tidy),
	PHP_MSHUTDOWN(tidy),
	PHP_RINIT(tidy),
	NULL,
	PHP_MINFO(tidy),
	PHP_TIDY_MODULE_VERSION,
	PHP_MODULE_GLOBALS(tidy),
	NULL,
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_TIDY
ZEND_GET_MODULE(tidy)
#endif

static void* TIDY_CALL php_tidy_malloc(size_t len)
{
	return emalloc(len);
}

static void* TIDY_CALL php_tidy_realloc(void *buf, size_t len)
{
	return erealloc(buf, len);
}

static void TIDY_CALL php_tidy_free(void *buf)
{
	efree(buf);
}

static void TIDY_CALL php_tidy_panic(ctmbstr msg)
{
	TSRMLS_FETCH();
	php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not allocate memory for tidy! (Reason: %s)", (char *)msg);
}

static int _php_tidy_set_tidy_opt(TidyDoc doc, char *optname, zval *value TSRMLS_DC)
{
	TidyOption opt = tidyGetOptionByName(doc, optname);
	zval conv = *value;

	if (!opt) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unknown Tidy Configuration Option '%s'", optname);
		return FAILURE;
	}
	
	if (tidyOptIsReadOnly(opt)) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Attempting to set read-only option '%s'", optname);
		return FAILURE;
	}

	switch(tidyOptGetType(opt)) {
		case TidyString:
			if (Z_TYPE(conv) != IS_STRING) {
				zval_copy_ctor(&conv);
				convert_to_string(&conv);
			}
			if (tidyOptSetValue(doc, tidyOptGetId(opt), Z_STRVAL(conv))) {
				if (Z_TYPE(conv) != Z_TYPE_P(value)) {
					zval_dtor(&conv);
				}
				return SUCCESS;
			}
			if (Z_TYPE(conv) != Z_TYPE_P(value)) {
				zval_dtor(&conv);
			}
			break;

		case TidyInteger:
			if (Z_TYPE(conv) != IS_LONG) {
				zval_copy_ctor(&conv);
				convert_to_long(&conv);
			}
			if (tidyOptSetInt(doc, tidyOptGetId(opt), Z_LVAL(conv))) {
				return SUCCESS;
			}
			break;

		case TidyBoolean:
			if (Z_TYPE(conv) != IS_LONG) {
				zval_copy_ctor(&conv);
				convert_to_long(&conv);
			}
			if (tidyOptSetBool(doc, tidyOptGetId(opt), Z_LVAL(conv))) {
				return SUCCESS;
			}
			break;

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to determine type of configuration option");
			break;
	}	

	return FAILURE;
}

static void php_tidy_quick_repair(INTERNAL_FUNCTION_PARAMETERS, zend_bool is_file)
{
	char *data=NULL, *arg1, *enc = NULL;
	int arg1_len, enc_len = 0, data_len = 0;
	zend_bool use_include_path = 0;
	TidyDoc doc;
	TidyBuffer *errbuf;
	zval **config = NULL;

	if (is_file) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|Zsb", &arg1, &arg1_len, &config, &enc, &enc_len, &use_include_path) == FAILURE) {
			RETURN_FALSE;
		}
		if (!(data = php_tidy_file_to_mem(arg1, use_include_path, &data_len TSRMLS_CC))) {
			RETURN_FALSE;
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|Zsb", &arg1, &arg1_len, &config, &enc, &enc_len, &use_include_path) == FAILURE) {
			RETURN_FALSE;
		}
		data = arg1;
		data_len = arg1_len;
	}

	doc = tidyCreate();
	errbuf = emalloc(sizeof(TidyBuffer));
	tidyBufInit(errbuf);
	
	if (tidySetErrorBuffer(doc, errbuf) != 0) {
		tidyBufFree(errbuf);
		efree(errbuf);
		tidyRelease(doc);
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not set Tidy error buffer");
	}
	
	tidyOptSetBool(doc, TidyForceOutput, yes);
	tidyOptSetBool(doc, TidyMark, no);
	
	TIDY_SET_DEFAULT_CONFIG(doc);
	
	if (config) {
		TIDY_APPLY_CONFIG_ZVAL(doc, config);
	}

	if(enc_len) {
		if (tidySetCharEncoding(doc, enc) < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not set encoding '%s'", enc);
			RETVAL_FALSE;
		}
	}
	
	if (data) {
		TidyBuffer buf;

		tidyBufInit(&buf);
		tidyBufAttach(&buf, (byte *) data, data_len);

		if (tidyParseBuffer(doc, &buf) < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errbuf->bp);
			RETVAL_FALSE;
		} else {
			if (tidyCleanAndRepair(doc) >= 0) {
				TidyBuffer output;
				tidyBufInit(&output);

				tidySaveBuffer (doc, &output);
				FIX_BUFFER(&output);
				RETVAL_STRINGL((char *) output.bp, output.size ? output.size-1 : 0, 1);
				tidyBufFree(&output);
			} else {
				RETVAL_FALSE;
			}
		}
	}

	if (is_file) {
		efree(data);
	}

	tidyBufFree(errbuf);
	efree(errbuf);
	tidyRelease(doc);
}

static char *php_tidy_file_to_mem(char *filename, zend_bool use_include_path, int *len TSRMLS_DC)
{
	php_stream *stream;
	char *data = NULL;

	if (!(stream = php_stream_open_wrapper(filename, "rb", (use_include_path ? USE_PATH : 0), NULL))) {
		return NULL;
	}
	if ((*len = (int) php_stream_copy_to_mem(stream, (void*) &data, PHP_STREAM_COPY_ALL, 0)) == 0) {
		data = estrdup("");
		*len = 0;
	}
	php_stream_close(stream);

	return data;
}

static void tidy_object_free_storage(void *object TSRMLS_DC)
{
	PHPTidyObj *intern = (PHPTidyObj *)object;

	zend_object_std_dtor(&intern->std TSRMLS_CC);

	if (intern->ptdoc) {
		intern->ptdoc->ref_count--;

		if (intern->ptdoc->ref_count <= 0) {
			tidyBufFree(intern->ptdoc->errbuf);
			efree(intern->ptdoc->errbuf);
			tidyRelease(intern->ptdoc->doc);
			efree(intern->ptdoc);
		}
	}

	efree(object);
}

static void tidy_object_new(zend_class_entry *class_type, zend_object_handlers *handlers,
							zend_object_value *retval, tidy_obj_type objtype TSRMLS_DC)
{
	PHPTidyObj *intern;

	intern = emalloc(sizeof(PHPTidyObj));
	memset(intern, 0, sizeof(PHPTidyObj));
	zend_object_std_init(&intern->std, class_type TSRMLS_CC);
	object_properties_init(&intern->std, class_type);

	switch(objtype) {
		case is_node:
			break;

		case is_doc:
			intern->ptdoc = emalloc(sizeof(PHPTidyDoc));
			intern->ptdoc->doc = tidyCreate();
			intern->ptdoc->ref_count = 1;
			intern->ptdoc->initialized = 0;
			intern->ptdoc->errbuf = emalloc(sizeof(TidyBuffer));
			tidyBufInit(intern->ptdoc->errbuf);

			if (tidySetErrorBuffer(intern->ptdoc->doc, intern->ptdoc->errbuf) != 0) {
				tidyBufFree(intern->ptdoc->errbuf);
				efree(intern->ptdoc->errbuf);
				tidyRelease(intern->ptdoc->doc);
				efree(intern->ptdoc);
				efree(intern);
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not set Tidy error buffer");
			}

			tidyOptSetBool(intern->ptdoc->doc, TidyForceOutput, yes);
			tidyOptSetBool(intern->ptdoc->doc, TidyMark, no);

			TIDY_SET_DEFAULT_CONFIG(intern->ptdoc->doc);

			tidy_add_default_properties(intern, is_doc TSRMLS_CC);
			break;
	}

	retval->handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) tidy_object_free_storage, NULL TSRMLS_CC);
	retval->handlers = handlers;
}

static zend_object_value tidy_object_new_node(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	tidy_object_new(class_type, &tidy_object_handlers_node, &retval, is_node TSRMLS_CC);
	return retval;
}

static zend_object_value tidy_object_new_doc(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	tidy_object_new(class_type, &tidy_object_handlers_doc, &retval, is_doc TSRMLS_CC);
	return retval;
}

static zval * tidy_instanciate(zend_class_entry *pce, zval *object TSRMLS_DC)
{
	if (!object) {
		ALLOC_ZVAL(object);
	}

	Z_TYPE_P(object) = IS_OBJECT;
	object_init_ex(object, pce);
	Z_SET_REFCOUNT_P(object, 1);
	Z_SET_ISREF_P(object);
	return object;
}

static int tidy_doc_cast_handler(zval *in, zval *out, int type TSRMLS_DC)
{
	TidyBuffer output;
	PHPTidyObj *obj;

	switch(type) {
		case IS_LONG:
			ZVAL_LONG(out, 0);
			break;

		case IS_DOUBLE:
			ZVAL_DOUBLE(out, 0);
			break;

		case IS_BOOL:
			ZVAL_BOOL(out, TRUE);
			break;

		case IS_STRING:
			obj = (PHPTidyObj *)zend_object_store_get_object(in TSRMLS_CC);
			tidyBufInit(&output);
			tidySaveBuffer (obj->ptdoc->doc, &output);
			ZVAL_STRINGL(out, (char *) output.bp, output.size ? output.size-1 : 0, 1);
			tidyBufFree(&output);
			break;

		default:
			return FAILURE;
	}

	return SUCCESS;
}

static int tidy_node_cast_handler(zval *in, zval *out, int type TSRMLS_DC)
{
	TidyBuffer buf;
	PHPTidyObj *obj;

	switch(type) {
		case IS_LONG:
			ZVAL_LONG(out, 0);
			break;

		case IS_DOUBLE:
			ZVAL_DOUBLE(out, 0);
			break;

		case IS_BOOL:
			ZVAL_BOOL(out, TRUE);
			break;

		case IS_STRING:
			obj = (PHPTidyObj *)zend_object_store_get_object(in TSRMLS_CC);
			tidyBufInit(&buf);
			if (obj->ptdoc) {
				tidyNodeGetText(obj->ptdoc->doc, obj->node, &buf);
				ZVAL_STRINGL(out, (char *) buf.bp, buf.size-1, 1);
			} else {
				ZVAL_EMPTY_STRING(out);
			}
			tidyBufFree(&buf);
			break;

		default:
			return FAILURE;
	}

	return SUCCESS;
}

static void tidy_doc_update_properties(PHPTidyObj *obj TSRMLS_DC)
{

	TidyBuffer output;
	zval *temp;

	tidyBufInit(&output);
	tidySaveBuffer (obj->ptdoc->doc, &output);
	
	if (output.size) {
		if (!obj->std.properties) {
			rebuild_object_properties(&obj->std);
		}
		MAKE_STD_ZVAL(temp);
		ZVAL_STRINGL(temp, (char*)output.bp, output.size-1, TRUE);
		zend_hash_update(obj->std.properties, "value", sizeof("value"), (void *)&temp, sizeof(zval *), NULL);
	}
	
	tidyBufFree(&output);

	if (obj->ptdoc->errbuf->size) {
		if (!obj->std.properties) {
			rebuild_object_properties(&obj->std);
		}
		MAKE_STD_ZVAL(temp);
		ZVAL_STRINGL(temp, (char*)obj->ptdoc->errbuf->bp, obj->ptdoc->errbuf->size-1, TRUE);
		zend_hash_update(obj->std.properties, "errorBuffer", sizeof("errorBuffer"), (void *)&temp, sizeof(zval *), NULL);
	}
}

static void tidy_add_default_properties(PHPTidyObj *obj, tidy_obj_type type TSRMLS_DC)
{

	TidyBuffer buf;
	TidyAttr	tempattr;
	TidyNode	tempnode;
	zval *attribute, *children, *temp;
	PHPTidyObj *newobj;

	switch(type) {

		case is_node:
			if (!obj->std.properties) {
				rebuild_object_properties(&obj->std);
			}
			tidyBufInit(&buf);
			tidyNodeGetText(obj->ptdoc->doc, obj->node, &buf);
			ADD_PROPERTY_STRINGL(obj->std.properties, value, buf.bp, buf.size ? buf.size-1 : 0);
			tidyBufFree(&buf);

			ADD_PROPERTY_STRING(obj->std.properties, name, tidyNodeGetName(obj->node));
			ADD_PROPERTY_LONG(obj->std.properties, type, tidyNodeGetType(obj->node));
			ADD_PROPERTY_LONG(obj->std.properties, line, tidyNodeLine(obj->node));
			ADD_PROPERTY_LONG(obj->std.properties, column, tidyNodeColumn(obj->node));
			ADD_PROPERTY_BOOL(obj->std.properties, proprietary, tidyNodeIsProp(obj->ptdoc->doc, obj->node));

			switch(tidyNodeGetType(obj->node)) {
				case TidyNode_Root:
				case TidyNode_DocType:
				case TidyNode_Text:
				case TidyNode_Comment:
					break;
	
				default:
					ADD_PROPERTY_LONG(obj->std.properties, id, tidyNodeGetId(obj->node));
			}

			tempattr = tidyAttrFirst(obj->node);
			MAKE_STD_ZVAL(attribute);

			if (tempattr) {
				char *name, *val;
				array_init(attribute);

				do {
					name = (char *)tidyAttrName(tempattr);
					val = (char *)tidyAttrValue(tempattr);
					if (name && val) {
						add_assoc_string(attribute, name, val, TRUE);
					}
				} while((tempattr = tidyAttrNext(tempattr)));
			} else {
				ZVAL_NULL(attribute);
			}
			zend_hash_update(obj->std.properties, "attribute", sizeof("attribute"), (void *)&attribute, sizeof(zval *), NULL);

			tempnode = tidyGetChild(obj->node);

			MAKE_STD_ZVAL(children);
			if (tempnode) {
				array_init(children);
				do {
					MAKE_STD_ZVAL(temp);
					tidy_instanciate(tidy_ce_node, temp TSRMLS_CC);
					newobj = (PHPTidyObj *) zend_object_store_get_object(temp TSRMLS_CC);
					newobj->node = tempnode;
					newobj->type = is_node;
					newobj->ptdoc = obj->ptdoc;
					newobj->ptdoc->ref_count++;

					tidy_add_default_properties(newobj, is_node TSRMLS_CC);
					add_next_index_zval(children, temp);

				} while((tempnode = tidyGetNext(tempnode)));

			} else {
				ZVAL_NULL(children);
			}

			zend_hash_update(obj->std.properties, "child", sizeof("child"), (void *)&children, sizeof(zval *), NULL);

			break;

		case is_doc:
			if (!obj->std.properties) {
				rebuild_object_properties(&obj->std);
			}
			ADD_PROPERTY_NULL(obj->std.properties, errorBuffer);
			ADD_PROPERTY_NULL(obj->std.properties, value);
			break;
	}
}

static void *php_tidy_get_opt_val(PHPTidyDoc *ptdoc, TidyOption opt, TidyOptionType *type TSRMLS_DC)
{
	*type = tidyOptGetType(opt);

	switch (*type) {
		case TidyString: {
			char *val = (char *) tidyOptGetValue(ptdoc->doc, tidyOptGetId(opt));
			if (val) {
				return (void *) estrdup(val);
			} else {
				return (void *) estrdup("");
			}
		}
			break;

		case TidyInteger:
			return (void *) tidyOptGetInt(ptdoc->doc, tidyOptGetId(opt));
			break;

		case TidyBoolean:
			return (void *) tidyOptGetBool(ptdoc->doc, tidyOptGetId(opt));
			break;
	}

	/* should not happen */
	return NULL;
}

static void php_tidy_create_node(INTERNAL_FUNCTION_PARAMETERS, tidy_base_nodetypes node_type)
{
	PHPTidyObj *newobj;
	TidyNode node;
	TIDY_FETCH_OBJECT;

	switch (node_type) {
		case is_root_node:
			node = tidyGetRoot(obj->ptdoc->doc);
			break;

		case is_html_node:
			node = tidyGetHtml(obj->ptdoc->doc);
			break;

		case is_head_node:
			node = tidyGetHead(obj->ptdoc->doc);
			break;

		case is_body_node:
			node = tidyGetBody(obj->ptdoc->doc);
			break;

		default:
			RETURN_NULL();
			break;
	}

	if (!node) {
		RETURN_NULL();
	}

	tidy_instanciate(tidy_ce_node, return_value TSRMLS_CC);
	newobj = (PHPTidyObj *) zend_object_store_get_object(return_value TSRMLS_CC);
	newobj->type  = is_node;
	newobj->ptdoc = obj->ptdoc;
	newobj->node  = node;
	newobj->ptdoc->ref_count++;

	tidy_add_default_properties(newobj, is_node TSRMLS_CC);
}

static int _php_tidy_apply_config_array(TidyDoc doc, HashTable *ht_options TSRMLS_DC)
{
	char *opt_name;
	zval **opt_val;
	ulong opt_indx;
	uint opt_name_len;
	zend_bool clear_str;

	for (zend_hash_internal_pointer_reset(ht_options);
		 zend_hash_get_current_data(ht_options, (void *) &opt_val) == SUCCESS;
		 zend_hash_move_forward(ht_options)) {

		switch (zend_hash_get_current_key_ex(ht_options, &opt_name, &opt_name_len, &opt_indx, FALSE, NULL)) {
			case HASH_KEY_IS_STRING:
			clear_str = 0;
			break;

			case HASH_KEY_IS_LONG:
			continue; /* ignore numeric keys */

			default:
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Could not retrieve key from option array");
			return FAILURE;
		}

		_php_tidy_set_tidy_opt(doc, opt_name, *opt_val TSRMLS_CC);
		if (clear_str) {
			efree(opt_name);
		}
	}
	
	return SUCCESS;
}

static int php_tidy_parse_string(PHPTidyObj *obj, char *string, int len, char *enc TSRMLS_DC)
{
	TidyBuffer buf;

	if(enc) {
		if (tidySetCharEncoding(obj->ptdoc->doc, enc) < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not set encoding '%s'", enc);
			return FAILURE;
		}
	}

	obj->ptdoc->initialized = 1;

	tidyBufInit(&buf);
	tidyBufAttach(&buf, (byte *) string, len);
	if (tidyParseBuffer(obj->ptdoc->doc, &buf) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", obj->ptdoc->errbuf->bp);
		return FAILURE;
	}
	tidy_doc_update_properties(obj TSRMLS_CC);

	return SUCCESS;
}

static PHP_MINIT_FUNCTION(tidy)
{
	tidySetMallocCall(php_tidy_malloc);
	tidySetReallocCall(php_tidy_realloc);
	tidySetFreeCall(php_tidy_free);
	tidySetPanicCall(php_tidy_panic);

	REGISTER_INI_ENTRIES();
	REGISTER_TIDY_CLASS(tidy, doc,	NULL, 0);
	REGISTER_TIDY_CLASS(tidyNode, node,	NULL, ZEND_ACC_FINAL_CLASS);

	tidy_object_handlers_doc.cast_object = tidy_doc_cast_handler;
	tidy_object_handlers_node.cast_object = tidy_node_cast_handler;

	_php_tidy_register_tags(INIT_FUNC_ARGS_PASSTHRU);
	_php_tidy_register_nodetypes(INIT_FUNC_ARGS_PASSTHRU);

	php_output_handler_alias_register(ZEND_STRL("ob_tidyhandler"), php_tidy_output_handler_init TSRMLS_CC);

	return SUCCESS;
}

static PHP_RINIT_FUNCTION(tidy)
{
	php_tidy_clean_output_start(ZEND_STRL("ob_tidyhandler") TSRMLS_CC);

	return SUCCESS;
}

static PHP_MSHUTDOWN_FUNCTION(tidy)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}

static PHP_MINFO_FUNCTION(tidy)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Tidy support", "enabled");
	php_info_print_table_row(2, "libTidy Release", (char *)tidyReleaseDate());
	php_info_print_table_row(2, "Extension Version", PHP_TIDY_MODULE_VERSION " ($Id$)");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

static PHP_INI_MH(php_tidy_set_clean_output)
{
	int status;
	zend_bool value;

	if (new_value_length==2 && strcasecmp("on", new_value)==0) {
		value = (zend_bool) 1;
	} else if (new_value_length==3 && strcasecmp("yes", new_value)==0) {
		value = (zend_bool) 1;
	} else if (new_value_length==4 && strcasecmp("true", new_value)==0) {
		value = (zend_bool) 1;
	} else {
		value = (zend_bool) atoi(new_value);
	}

	if (stage == PHP_INI_STAGE_RUNTIME) {
		status = php_output_get_status(TSRMLS_C);

		if (value && (status & PHP_OUTPUT_WRITTEN)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot enable tidy.clean_output - there has already been output");
			return FAILURE;
		}
		if (status & PHP_OUTPUT_SENT) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot change tidy.clean_output - headers already sent");
			return FAILURE;
		}
	}

	status = OnUpdateBool(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);

	if (stage == PHP_INI_STAGE_RUNTIME && value) {
		if (!php_output_handler_started(ZEND_STRL("ob_tidyhandler") TSRMLS_CC)) {
			php_tidy_clean_output_start(ZEND_STRL("ob_tidyhandler") TSRMLS_CC);
		}
	}

	return status;
}

/*
 * NOTE: tidy does not support iterative/cumulative parsing, so chunk-sized output handler is not possible
 */

static void php_tidy_clean_output_start(const char *name, size_t name_len TSRMLS_DC)
{
	php_output_handler *h;

	if (TG(clean_output) && (h = php_tidy_output_handler_init(name, name_len, 0, PHP_OUTPUT_HANDLER_STDFLAGS TSRMLS_CC))) {
		php_output_handler_start(h TSRMLS_CC);
	}
}

static php_output_handler *php_tidy_output_handler_init(const char *handler_name, size_t handler_name_len, size_t chunk_size, int flags TSRMLS_DC)
{
	if (chunk_size) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot use a chunk size for ob_tidyhandler");
		return NULL;
	}
	if (!TG(clean_output)) {
		TG(clean_output) = 1;
	}
	return php_output_handler_create_internal(handler_name, handler_name_len, php_tidy_output_handler, chunk_size, flags TSRMLS_CC);
}

static int php_tidy_output_handler(void **nothing, php_output_context *output_context)
{
	int status = FAILURE;
	TidyDoc doc;
	TidyBuffer inbuf, outbuf, errbuf;
	PHP_OUTPUT_TSRMLS(output_context);

	if (TG(clean_output) && (output_context->op & PHP_OUTPUT_HANDLER_START) && (output_context->op & PHP_OUTPUT_HANDLER_FINAL)) {
		doc = tidyCreate();
		tidyBufInit(&errbuf);

		if (0 == tidySetErrorBuffer(doc, &errbuf)) {
			tidyOptSetBool(doc, TidyForceOutput, yes);
			tidyOptSetBool(doc, TidyMark, no);

			TIDY_SET_DEFAULT_CONFIG(doc);

			tidyBufInit(&inbuf);
			tidyBufAttach(&inbuf, (byte *) output_context->in.data, output_context->in.used);

			if (0 <= tidyParseBuffer(doc, &inbuf) && 0 <= tidyCleanAndRepair(doc)) {
				tidyBufInit(&outbuf);
				tidySaveBuffer(doc, &outbuf);
				FIX_BUFFER(&outbuf);
				output_context->out.data = (char *) outbuf.bp;
				output_context->out.used = outbuf.size ? outbuf.size-1 : 0;
				output_context->out.free = 1;
				status = SUCCESS;
			}
		}

		tidyRelease(doc);
		tidyBufFree(&errbuf);
	}

	return status;
}

/* {{{ proto bool tidy_parse_string(string input [, mixed config_options [, string encoding]])
   Parse a document stored in a string */
static PHP_FUNCTION(tidy_parse_string)
{
	char *input, *enc = NULL;
	int input_len, enc_len = 0;
	zval **options = NULL;
	PHPTidyObj *obj;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|Zs", &input, &input_len, &options, &enc, &enc_len) == FAILURE) {
		RETURN_FALSE;
	}

	tidy_instanciate(tidy_ce_doc, return_value TSRMLS_CC);
	obj = (PHPTidyObj *) zend_object_store_get_object(return_value TSRMLS_CC);
		
	TIDY_APPLY_CONFIG_ZVAL(obj->ptdoc->doc, options);
	
	if(php_tidy_parse_string(obj, input, input_len, enc TSRMLS_CC) == FAILURE) {
		zval_dtor(return_value);
		INIT_ZVAL(*return_value);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string tidy_get_error_buffer()
   Return warnings and errors which occurred parsing the specified document*/
static PHP_FUNCTION(tidy_get_error_buffer)
{
	TIDY_FETCH_OBJECT;

	if (obj->ptdoc->errbuf && obj->ptdoc->errbuf->bp) {
		RETURN_STRINGL((char*)obj->ptdoc->errbuf->bp, obj->ptdoc->errbuf->size-1, 1);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string tidy_get_output()
   Return a string representing the parsed tidy markup */
static PHP_FUNCTION(tidy_get_output)
{
	TidyBuffer output;
	TIDY_FETCH_OBJECT;

	tidyBufInit(&output);
	tidySaveBuffer(obj->ptdoc->doc, &output);
	FIX_BUFFER(&output);
	RETVAL_STRINGL((char *) output.bp, output.size ? output.size-1 : 0, 1);
	tidyBufFree(&output);
}
/* }}} */

/* {{{ proto boolean tidy_parse_file(string file [, mixed config_options [, string encoding [, bool use_include_path]]])
   Parse markup in file or URI */
static PHP_FUNCTION(tidy_parse_file)
{
	char *inputfile, *enc = NULL;
	int input_len, contents_len, enc_len = 0;
	zend_bool use_include_path = 0;
	char *contents;
	zval **options = NULL;
	
	PHPTidyObj *obj;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|Zsb", &inputfile, &input_len,
							  &options, &enc, &enc_len, &use_include_path) == FAILURE) {
		RETURN_FALSE;
	}

	tidy_instanciate(tidy_ce_doc, return_value TSRMLS_CC);
	obj = (PHPTidyObj *) zend_object_store_get_object(return_value TSRMLS_CC);

	if (!(contents = php_tidy_file_to_mem(inputfile, use_include_path, &contents_len TSRMLS_CC))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot Load '%s' into memory%s", inputfile, (use_include_path) ? " (Using include path)" : "");
		RETURN_FALSE;
	}

	TIDY_APPLY_CONFIG_ZVAL(obj->ptdoc->doc, options);

	if(php_tidy_parse_string(obj, contents, contents_len, enc TSRMLS_CC) == FAILURE) {
		zval_dtor(return_value);
		INIT_ZVAL(*return_value);
		RETVAL_FALSE;
	}

	efree(contents);
}
/* }}} */

/* {{{ proto boolean tidy_clean_repair()
   Execute configured cleanup and repair operations on parsed markup */
static PHP_FUNCTION(tidy_clean_repair)
{
	TIDY_FETCH_OBJECT;

	if (tidyCleanAndRepair(obj->ptdoc->doc) >= 0) {
		tidy_doc_update_properties(obj TSRMLS_CC);
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean tidy_repair_string(string data [, mixed config_file [, string encoding]])
   Repair a string using an optionally provided configuration file */
static PHP_FUNCTION(tidy_repair_string)
{
	php_tidy_quick_repair(INTERNAL_FUNCTION_PARAM_PASSTHRU, FALSE);
}
/* }}} */

/* {{{ proto boolean tidy_repair_file(string filename [, mixed config_file [, string encoding [, bool use_include_path]]])
   Repair a file using an optionally provided configuration file */
static PHP_FUNCTION(tidy_repair_file)
{
	php_tidy_quick_repair(INTERNAL_FUNCTION_PARAM_PASSTHRU, TRUE);
}
/* }}} */

/* {{{ proto boolean tidy_diagnose()
   Run configured diagnostics on parsed and repaired markup. */
static PHP_FUNCTION(tidy_diagnose)
{
	TIDY_FETCH_OBJECT;

	if (obj->ptdoc->initialized && tidyRunDiagnostics(obj->ptdoc->doc) >= 0) {
		tidy_doc_update_properties(obj TSRMLS_CC);
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string tidy_get_release()
   Get release date (version) for Tidy library */
static PHP_FUNCTION(tidy_get_release)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_STRING((char *)tidyReleaseDate(), 1);
}
/* }}} */


#if HAVE_TIDYOPTGETDOC
/* {{{ proto string tidy_get_opt_doc(tidy resource, string optname)
   Returns the documentation for the given option name */
static PHP_FUNCTION(tidy_get_opt_doc)
{
	PHPTidyObj *obj;
	char *optval, *optname;
	int optname_len;
	TidyOption opt;

	TIDY_SET_CONTEXT;

	if (object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &optname, &optname_len) == FAILURE) {
			RETURN_FALSE;
		}
	} else {
		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, NULL, "Os", &object, tidy_ce_doc, &optname, &optname_len) == FAILURE) {
			RETURN_FALSE;
		}
	}

	obj = (PHPTidyObj *) zend_object_store_get_object(object TSRMLS_CC);

	opt = tidyGetOptionByName(obj->ptdoc->doc, optname);

	if (!opt) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown Tidy Configuration Option '%s'", optname);
		RETURN_FALSE;
	}

	if ( (optval = (char *) tidyOptGetDoc(obj->ptdoc->doc, opt)) ) {
		RETURN_STRING(optval, 1);
	}

	RETURN_FALSE;
}
/* }}} */
#endif


/* {{{ proto array tidy_get_config()
   Get current Tidy configuration */
static PHP_FUNCTION(tidy_get_config)
{
	TidyIterator itOpt;
	char *opt_name;
	void *opt_value;
	TidyOptionType optt;

	TIDY_FETCH_OBJECT;

	itOpt = tidyGetOptionList(obj->ptdoc->doc);

	array_init(return_value);

	while (itOpt) {
		TidyOption opt = tidyGetNextOption(obj->ptdoc->doc, &itOpt);

		opt_name = (char *)tidyOptGetName(opt);
		opt_value = php_tidy_get_opt_val(obj->ptdoc, opt, &optt TSRMLS_CC);
		switch (optt) {
			case TidyString:
				add_assoc_string(return_value, opt_name, (char*)opt_value, 0);
				break;

			case TidyInteger:
				add_assoc_long(return_value, opt_name, (long)opt_value);
				break;

			case TidyBoolean:
				add_assoc_bool(return_value, opt_name, (long)opt_value);
				break;
		}
	}

	return;
}
/* }}} */

/* {{{ proto int tidy_get_status()
   Get status of specfied document. */
static PHP_FUNCTION(tidy_get_status)
{
	TIDY_FETCH_OBJECT;

	RETURN_LONG(tidyStatus(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto int tidy_get_html_ver()
   Get the Detected HTML version for the specified document. */
static PHP_FUNCTION(tidy_get_html_ver)
{
	TIDY_FETCH_OBJECT;

	RETURN_LONG(tidyDetectedHtmlVersion(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto boolean tidy_is_xhtml()
   Indicates if the document is a XHTML document. */
static PHP_FUNCTION(tidy_is_xhtml)
{
	TIDY_FETCH_OBJECT;

	RETURN_BOOL(tidyDetectedXhtml(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto boolean tidy_is_xml()
   Indicates if the document is a generic (non HTML/XHTML) XML document. */
static PHP_FUNCTION(tidy_is_xml)
{
	TIDY_FETCH_OBJECT;

	RETURN_BOOL(tidyDetectedGenericXml(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto int tidy_error_count()
   Returns the Number of Tidy errors encountered for specified document. */
static PHP_FUNCTION(tidy_error_count)
{
	TIDY_FETCH_OBJECT;

	RETURN_LONG(tidyErrorCount(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto int tidy_warning_count()
   Returns the Number of Tidy warnings encountered for specified document. */
static PHP_FUNCTION(tidy_warning_count)
{
	TIDY_FETCH_OBJECT;

	RETURN_LONG(tidyWarningCount(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto int tidy_access_count()
   Returns the Number of Tidy accessibility warnings encountered for specified document. */
static PHP_FUNCTION(tidy_access_count)
{
	TIDY_FETCH_OBJECT;

	RETURN_LONG(tidyAccessWarningCount(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto int tidy_config_count()
   Returns the Number of Tidy configuration errors encountered for specified document. */
static PHP_FUNCTION(tidy_config_count)
{
	TIDY_FETCH_OBJECT;

	RETURN_LONG(tidyConfigErrorCount(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto mixed tidy_getopt(string option)
   Returns the value of the specified configuration option for the tidy document. */
static PHP_FUNCTION(tidy_getopt)
{  
	PHPTidyObj *obj;
	char *optname;
	void *optval;
	int optname_len;
	TidyOption opt;
	TidyOptionType optt;
	
	TIDY_SET_CONTEXT;

	if (object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &optname, &optname_len) == FAILURE) {
			RETURN_FALSE;
		}
	} else {
		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, NULL, "Os", &object, tidy_ce_doc, &optname, &optname_len) == FAILURE) {
			RETURN_FALSE;
		}
	}

	obj = (PHPTidyObj *) zend_object_store_get_object(object TSRMLS_CC);

	opt = tidyGetOptionByName(obj->ptdoc->doc, optname);

	if (!opt) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown Tidy Configuration Option '%s'", optname);
		RETURN_FALSE;
	}

	optval = php_tidy_get_opt_val(obj->ptdoc, opt, &optt TSRMLS_CC);
	switch (optt) {
		case TidyString:
			RETURN_STRING((char *)optval, 0);
			break;

		case TidyInteger:
			RETURN_LONG((long)optval);
			break;

		case TidyBoolean:
			if (optval) {
				RETURN_TRUE;
			} else {
				RETURN_FALSE;
			}
			break;

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to determine type of configuration option");
			break;
	}

	RETURN_FALSE;
}
/* }}} */

static TIDY_DOC_METHOD(__construct)
{
	char *inputfile = NULL, *enc = NULL;
	int input_len = 0, enc_len = 0, contents_len = 0;
	zend_bool use_include_path = 0;
	char *contents;
	zval **options = NULL;
	
	PHPTidyObj *obj;
	TIDY_SET_CONTEXT;	
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|pZsb", &inputfile, &input_len,
							  &options, &enc, &enc_len, &use_include_path) == FAILURE) {
		RETURN_FALSE;
	}
	
	obj = (PHPTidyObj *)zend_object_store_get_object(object TSRMLS_CC);
	
	if (inputfile) {
		if (!(contents = php_tidy_file_to_mem(inputfile, use_include_path, &contents_len TSRMLS_CC))) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot Load '%s' into memory%s", inputfile, (use_include_path) ? " (Using include path)" : "");
			return;
		}

		TIDY_APPLY_CONFIG_ZVAL(obj->ptdoc->doc, options);

		php_tidy_parse_string(obj, contents, contents_len, enc TSRMLS_CC);

		efree(contents);
	}
}

static TIDY_DOC_METHOD(parseFile)
{
	char *inputfile, *enc = NULL;
	int input_len, enc_len = 0, contents_len = 0;
	zend_bool use_include_path = 0;
	char *contents;
	zval **options = NULL;
	PHPTidyObj *obj;

	TIDY_SET_CONTEXT;

	obj = (PHPTidyObj *)zend_object_store_get_object(object TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|Zsb", &inputfile, &input_len,
							  &options, &enc, &enc_len, &use_include_path) == FAILURE) {
		RETURN_FALSE;
	}
	
	if (!(contents = php_tidy_file_to_mem(inputfile, use_include_path, &contents_len TSRMLS_CC))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot Load '%s' into memory%s", inputfile, (use_include_path) ? " (Using include path)" : "");
		RETURN_FALSE;
	}

	TIDY_APPLY_CONFIG_ZVAL(obj->ptdoc->doc, options);

	if(php_tidy_parse_string(obj, contents, contents_len, enc TSRMLS_CC) == FAILURE) {
		RETVAL_FALSE;
	} else {
		RETVAL_TRUE;
	}

	efree(contents);
}

static TIDY_DOC_METHOD(parseString)
{
	char *input, *enc = NULL;
	int input_len, enc_len = 0;
	zval **options = NULL;
	PHPTidyObj *obj;

	TIDY_SET_CONTEXT;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|Zs", &input, &input_len, &options, &enc, &enc_len) == FAILURE) {
		RETURN_FALSE;
	}

	obj = (PHPTidyObj *)zend_object_store_get_object(object TSRMLS_CC);

	TIDY_APPLY_CONFIG_ZVAL(obj->ptdoc->doc, options);

	if(php_tidy_parse_string(obj, input, input_len, enc TSRMLS_CC) == SUCCESS) {
		RETURN_TRUE;
	}
	
	RETURN_FALSE;
}


/* {{{ proto TidyNode tidy_get_root()
   Returns a TidyNode Object representing the root of the tidy parse tree */
static PHP_FUNCTION(tidy_get_root)
{
	php_tidy_create_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, is_root_node);
}
/* }}} */

/* {{{ proto TidyNode tidy_get_html()
   Returns a TidyNode Object starting from the <HTML> tag of the tidy parse tree */
static PHP_FUNCTION(tidy_get_html)
{
	php_tidy_create_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, is_html_node);
}
/* }}} */

/* {{{ proto TidyNode tidy_get_head()
   Returns a TidyNode Object starting from the <HEAD> tag of the tidy parse tree */
static PHP_FUNCTION(tidy_get_head)
{
	php_tidy_create_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, is_head_node);
}
/* }}} */

/* {{{ proto TidyNode tidy_get_body(resource tidy)
   Returns a TidyNode Object starting from the <BODY> tag of the tidy parse tree */
static PHP_FUNCTION(tidy_get_body)
{
	php_tidy_create_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, is_body_node);
}
/* }}} */

/* {{{ proto boolean tidyNode::hasChildren()
   Returns true if this node has children */
static TIDY_NODE_METHOD(hasChildren)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyGetChild(obj->node)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidyNode::hasSiblings()
   Returns true if this node has siblings */
static TIDY_NODE_METHOD(hasSiblings)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (obj->node && tidyGetNext(obj->node)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidyNode::isComment()
   Returns true if this node represents a comment */
static TIDY_NODE_METHOD(isComment)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyNodeGetType(obj->node) == TidyNode_Comment) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidyNode::isHtml()
   Returns true if this node is part of a HTML document */
static TIDY_NODE_METHOD(isHtml)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyNodeGetType(obj->node) & (TidyNode_Start | TidyNode_End | TidyNode_StartEnd)) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean tidyNode::isText()
   Returns true if this node represents text (no markup) */
static TIDY_NODE_METHOD(isText)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyNodeGetType(obj->node) == TidyNode_Text) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidyNode::isJste()
   Returns true if this node is JSTE */
static TIDY_NODE_METHOD(isJste)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyNodeGetType(obj->node) == TidyNode_Jste) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidyNode::isAsp()
   Returns true if this node is ASP */
static TIDY_NODE_METHOD(isAsp)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyNodeGetType(obj->node) == TidyNode_Asp) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidyNode::isPhp()
   Returns true if this node is PHP */
static TIDY_NODE_METHOD(isPhp)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyNodeGetType(obj->node) == TidyNode_Php) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto tidyNode tidyNode::getParent()
   Returns the parent node if available or NULL */
static TIDY_NODE_METHOD(getParent)
{
	TidyNode	parent_node;
	PHPTidyObj *newobj;
	TIDY_FETCH_ONLY_OBJECT;

	parent_node = tidyGetParent(obj->node);
	if(parent_node) {
		tidy_instanciate(tidy_ce_node, return_value TSRMLS_CC);
		newobj = (PHPTidyObj *) zend_object_store_get_object(return_value TSRMLS_CC);
		newobj->node = parent_node;
		newobj->type = is_node;
		newobj->ptdoc = obj->ptdoc;
		newobj->ptdoc->ref_count++;
		tidy_add_default_properties(newobj, is_node TSRMLS_CC);
	} else {
		ZVAL_NULL(return_value);
	}
}
/* }}} */


/* {{{ proto void tidyNode::__construct()
         __constructor for tidyNode. */
static TIDY_NODE_METHOD(__construct)
{
	php_error_docref(NULL TSRMLS_CC, E_ERROR, "You should not create a tidyNode manually");
}   
/* }}} */

static void _php_tidy_register_nodetypes(INIT_FUNC_ARGS)
{
	TIDY_NODE_CONST(ROOT, Root);
	TIDY_NODE_CONST(DOCTYPE, DocType);
	TIDY_NODE_CONST(COMMENT, Comment);
	TIDY_NODE_CONST(PROCINS, ProcIns);
	TIDY_NODE_CONST(TEXT, Text);
	TIDY_NODE_CONST(START, Start);
	TIDY_NODE_CONST(END, End);
	TIDY_NODE_CONST(STARTEND, StartEnd);
	TIDY_NODE_CONST(CDATA, CDATA);
	TIDY_NODE_CONST(SECTION, Section);
	TIDY_NODE_CONST(ASP, Asp);
	TIDY_NODE_CONST(JSTE, Jste);
	TIDY_NODE_CONST(PHP, Php);
	TIDY_NODE_CONST(XMLDECL, XmlDecl);
}

static void _php_tidy_register_tags(INIT_FUNC_ARGS)
{
	TIDY_TAG_CONST(UNKNOWN);
	TIDY_TAG_CONST(A);
	TIDY_TAG_CONST(ABBR);
	TIDY_TAG_CONST(ACRONYM);
	TIDY_TAG_CONST(ADDRESS);
	TIDY_TAG_CONST(ALIGN);
	TIDY_TAG_CONST(APPLET);
	TIDY_TAG_CONST(AREA);
	TIDY_TAG_CONST(B);
	TIDY_TAG_CONST(BASE);
	TIDY_TAG_CONST(BASEFONT);
	TIDY_TAG_CONST(BDO);
	TIDY_TAG_CONST(BGSOUND);
	TIDY_TAG_CONST(BIG);
	TIDY_TAG_CONST(BLINK);
	TIDY_TAG_CONST(BLOCKQUOTE);
	TIDY_TAG_CONST(BODY);
	TIDY_TAG_CONST(BR);
	TIDY_TAG_CONST(BUTTON);
	TIDY_TAG_CONST(CAPTION);
	TIDY_TAG_CONST(CENTER);
	TIDY_TAG_CONST(CITE);
	TIDY_TAG_CONST(CODE);
	TIDY_TAG_CONST(COL);
	TIDY_TAG_CONST(COLGROUP);
	TIDY_TAG_CONST(COMMENT);
	TIDY_TAG_CONST(DD);
	TIDY_TAG_CONST(DEL);
	TIDY_TAG_CONST(DFN);
	TIDY_TAG_CONST(DIR);
	TIDY_TAG_CONST(DIV);
	TIDY_TAG_CONST(DL);
	TIDY_TAG_CONST(DT);
	TIDY_TAG_CONST(EM);
	TIDY_TAG_CONST(EMBED);
	TIDY_TAG_CONST(FIELDSET);
	TIDY_TAG_CONST(FONT);
	TIDY_TAG_CONST(FORM);
	TIDY_TAG_CONST(FRAME);
	TIDY_TAG_CONST(FRAMESET);
	TIDY_TAG_CONST(H1);
	TIDY_TAG_CONST(H2);
	TIDY_TAG_CONST(H3);
	TIDY_TAG_CONST(H4);
	TIDY_TAG_CONST(H5);
	TIDY_TAG_CONST(H6);
	TIDY_TAG_CONST(HEAD);
	TIDY_TAG_CONST(HR);
	TIDY_TAG_CONST(HTML);
	TIDY_TAG_CONST(I);
	TIDY_TAG_CONST(IFRAME);
	TIDY_TAG_CONST(ILAYER);
	TIDY_TAG_CONST(IMG);
	TIDY_TAG_CONST(INPUT);
	TIDY_TAG_CONST(INS);
	TIDY_TAG_CONST(ISINDEX);
	TIDY_TAG_CONST(KBD);
	TIDY_TAG_CONST(KEYGEN);
	TIDY_TAG_CONST(LABEL);
	TIDY_TAG_CONST(LAYER);
	TIDY_TAG_CONST(LEGEND);
	TIDY_TAG_CONST(LI);
	TIDY_TAG_CONST(LINK);
	TIDY_TAG_CONST(LISTING);
	TIDY_TAG_CONST(MAP);
	TIDY_TAG_CONST(MARQUEE);
	TIDY_TAG_CONST(MENU);
	TIDY_TAG_CONST(META);
	TIDY_TAG_CONST(MULTICOL);
	TIDY_TAG_CONST(NOBR);
	TIDY_TAG_CONST(NOEMBED);
	TIDY_TAG_CONST(NOFRAMES);
	TIDY_TAG_CONST(NOLAYER);
	TIDY_TAG_CONST(NOSAVE);
	TIDY_TAG_CONST(NOSCRIPT);
	TIDY_TAG_CONST(OBJECT);
	TIDY_TAG_CONST(OL);
	TIDY_TAG_CONST(OPTGROUP);
	TIDY_TAG_CONST(OPTION);
	TIDY_TAG_CONST(P);
	TIDY_TAG_CONST(PARAM);
	TIDY_TAG_CONST(PLAINTEXT);
	TIDY_TAG_CONST(PRE);
	TIDY_TAG_CONST(Q);
	TIDY_TAG_CONST(RB);
	TIDY_TAG_CONST(RBC);
	TIDY_TAG_CONST(RP);
	TIDY_TAG_CONST(RT);
	TIDY_TAG_CONST(RTC);
	TIDY_TAG_CONST(RUBY);
	TIDY_TAG_CONST(S);
	TIDY_TAG_CONST(SAMP);
	TIDY_TAG_CONST(SCRIPT);
	TIDY_TAG_CONST(SELECT);
	TIDY_TAG_CONST(SERVER);
	TIDY_TAG_CONST(SERVLET);
	TIDY_TAG_CONST(SMALL);
	TIDY_TAG_CONST(SPACER);
	TIDY_TAG_CONST(SPAN);
	TIDY_TAG_CONST(STRIKE);
	TIDY_TAG_CONST(STRONG);
	TIDY_TAG_CONST(STYLE);
	TIDY_TAG_CONST(SUB);
	TIDY_TAG_CONST(SUP);
	TIDY_TAG_CONST(TABLE);
	TIDY_TAG_CONST(TBODY);
	TIDY_TAG_CONST(TD);
	TIDY_TAG_CONST(TEXTAREA);
	TIDY_TAG_CONST(TFOOT);
	TIDY_TAG_CONST(TH);
	TIDY_TAG_CONST(THEAD);
	TIDY_TAG_CONST(TITLE);
	TIDY_TAG_CONST(TR);
	TIDY_TAG_CONST(TT);
	TIDY_TAG_CONST(U);
	TIDY_TAG_CONST(UL);
	TIDY_TAG_CONST(VAR);
	TIDY_TAG_CONST(WBR);
	TIDY_TAG_CONST(XMP);
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
