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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_tidy.h"

#if HAVE_TIDY

#include "php_ini.h"
#include "ext/standard/info.h"
#include "Zend/zend_API.h"
#include "Zend/zend_hash.h"
#include "safe_mode.h"
#include "Zend/zend_default_classes.h"
#include "Zend/zend_object_handlers.h"
#include "Zend/zend_hash.h"

ZEND_DECLARE_MODULE_GLOBALS(tidy)

PHP_INI_BEGIN()
STD_PHP_INI_ENTRY("tidy.default_config",	"",	PHP_INI_SYSTEM,		OnUpdateString,		default_config,		zend_tidy_globals,	tidy_globals)
PHP_INI_ENTRY("tidy.clean_output",     "0",    PHP_INI_PERDIR,         NULL)
PHP_INI_END()

function_entry tidy_functions[] = {
	PHP_FE(tidy_setopt,             NULL)
	PHP_FE(tidy_getopt,             NULL)
	PHP_FE(tidy_parse_string,       NULL)
	PHP_FE(tidy_parse_file,         NULL)
	PHP_FE(tidy_get_output,         NULL)
	PHP_FE(tidy_get_error_buffer,   NULL)
	PHP_FE(tidy_clean_repair,       NULL)
	PHP_FE(tidy_repair_string,	NULL)
	PHP_FE(tidy_repair_file,	NULL)
	PHP_FE(tidy_diagnose,           NULL)
	PHP_FE(tidy_get_release,	NULL)
	PHP_FE(tidy_get_config,		NULL)
	PHP_FE(tidy_get_status,		NULL)
	PHP_FE(tidy_get_html_ver,	NULL)
	PHP_FE(tidy_is_xhtml,		NULL)
	PHP_FE(tidy_is_xml,		NULL)
	PHP_FE(tidy_error_count,	NULL)
	PHP_FE(tidy_warning_count,	NULL)
	PHP_FE(tidy_access_count,	NULL)
	PHP_FE(tidy_config_count,	NULL)
	PHP_FE(tidy_load_config,	NULL)
	PHP_FE(tidy_load_config_enc,	NULL)
	PHP_FE(tidy_set_encoding,	NULL)
	PHP_FE(tidy_save_config,	NULL)
	PHP_FE(tidy_get_root,		NULL)
	PHP_FE(tidy_get_head,		NULL)
	PHP_FE(tidy_get_html,		NULL)
	PHP_FE(tidy_get_body,		NULL)
	PHP_FE(tidy_reset_config,	NULL)
	PHP_FE(tidy_snapshot_config,	NULL)
	PHP_FE(tidy_restore_config,	NULL)
	PHP_FE(ob_tidyhandler,		NULL)
	{NULL, NULL, NULL}
};

function_entry tidy_funcs_doc[] = {
	TIDY_METHOD_MAP(setopt, tidy_setopt, NULL)
	TIDY_METHOD_MAP(getopt, tidy_getopt, NULL)
	TIDY_METHOD_MAP(parse_string, tidy_parse_string, NULL)
	TIDY_METHOD_MAP(parse_file, tidy_parse_file, NULL)
	TIDY_METHOD_MAP(clean_repair, tidy_clean_repair, NULL)
	TIDY_METHOD_MAP(repair_string, tidy_repair_string, NULL)
	TIDY_METHOD_MAP(repair_file, tidy_repair_file, NULL)
	TIDY_METHOD_MAP(diagnose, tidy_diagnose, NULL)
	TIDY_METHOD_MAP(get_release, tidy_get_release, NULL)
	TIDY_METHOD_MAP(get_config, tidy_get_config, NULL)
	TIDY_METHOD_MAP(get_status, tidy_get_status, NULL)
	TIDY_METHOD_MAP(get_html_ver, tidy_get_html_ver, NULL)
	TIDY_METHOD_MAP(is_xhtml, tidy_is_xhtml, NULL)
	TIDY_METHOD_MAP(is_xml, tidy_is_xml, NULL)
	TIDY_METHOD_MAP(load_config, tidy_load_config, NULL)
	TIDY_METHOD_MAP(load_config_enc, tidy_load_config_enc, NULL)
	TIDY_METHOD_MAP(set_encoding, tidy_set_encoding, NULL)
	TIDY_METHOD_MAP(save_config, tidy_save_config, NULL)
	TIDY_METHOD_MAP(root, tidy_get_root, NULL)
	TIDY_METHOD_MAP(head, tidy_get_head, NULL)
	TIDY_METHOD_MAP(html, tidy_get_html, NULL)
	TIDY_METHOD_MAP(body, tidy_get_body, NULL)
	TIDY_METHOD_MAP(reset_config, tidy_reset_config, NULL)
	TIDY_METHOD_MAP(snapshot_config, tidy_snapshot_config, NULL)
	TIDY_METHOD_MAP(restore_config, tidy_restore_config, NULL)
	{NULL, NULL, NULL}
};

function_entry tidy_funcs_node[] = {

	TIDY_NODE_ME(__construct, NULL)
	TIDY_NODE_ME(has_children, NULL)
	TIDY_NODE_ME(has_siblings, NULL)
	TIDY_NODE_ME(is_comment, NULL)
	TIDY_NODE_ME(is_html, NULL)
	TIDY_NODE_ME(is_text, NULL)
	TIDY_NODE_ME(is_jste, NULL)
	TIDY_NODE_ME(is_asp, NULL)
	TIDY_NODE_ME(is_php, NULL)
	{NULL, NULL, NULL}
};

function_entry tidy_funcs_attr[] = {
	TIDY_ATTR_ME(__construct, NULL)
	{NULL, NULL, NULL}
};

function_entry tidy_funcs_exception[] = {
	{NULL, NULL, NULL}
};

zend_class_entry *tidy_ce_doc, *tidy_ce_node, *tidy_ce_attr, *tidy_ce_exception;

static zend_object_handlers tidy_object_handlers_doc;
static zend_object_handlers tidy_object_handlers_node;
static zend_object_handlers tidy_object_handlers_attr;
static zend_object_handlers tidy_object_handlers_exception;

zend_module_entry tidy_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"tidy",
	tidy_functions,
	PHP_MINIT(tidy),
	PHP_MSHUTDOWN(tidy),
	PHP_RINIT(tidy),
	PHP_RSHUTDOWN(tidy),
	PHP_MINFO(tidy),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_TIDY_MODULE_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TIDY
ZEND_GET_MODULE(tidy)
#endif

void *php_tidy_malloc(size_t len)
{
	return emalloc(len);
}

void *php_tidy_realloc(void *buf, size_t len)
{
	return erealloc(buf, len);
}

void php_tidy_free(void *buf)
{
	efree(buf);
}

void php_tidy_panic(ctmbstr msg)
{
	zend_error(E_ERROR, "Could not allocate memory for tidy! (Reason: %s)", (char *)msg);
}

static int _php_tidy_set_tidy_opt(TidyDoc doc, char *optname, zval *value)
{
	TidyOption opt;
	
	opt = tidyGetOptionByName(doc, optname);

	if (!opt) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown Tidy Configuration Option '%s'", optname);
		return FAILURE;
	}
	
	if (tidyOptIsReadOnly(opt)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attempted to set read-only option '%s'", optname);
		return FAILURE;
	}

	switch(tidyOptGetType(opt)) {
		case TidyString:
			convert_to_string_ex(&value);
			if (tidyOptSetValue(doc, tidyOptGetId(opt), Z_STRVAL_P(value))) {
				return SUCCESS;
			}
			break;

		case TidyInteger:
			convert_to_long_ex(&value);
			if (tidyOptSetInt(doc, tidyOptGetId(opt), Z_LVAL_P(value))) {
				return SUCCESS;
			}
			break;

		case TidyBoolean:
			convert_to_long_ex(&value);
			if (tidyOptSetBool(doc,  tidyOptGetId(opt), Z_LVAL_P(value))) {
				return SUCCESS;
			}
			break;

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to determine type of Tidy configuration constant to set");
			break;
	}	
	
	return FAILURE;
}

static void php_tidy_quick_repair(INTERNAL_FUNCTION_PARAMETERS, zend_bool is_file)
{
	char *data=NULL, *cfg_file=NULL, *arg1;
	int cfg_file_len, arg1_len;
	zend_bool use_include_path = 0;
	zval *object = getThis();
	PHPTidyObj *obj;

	if (object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sb", &arg1, &arg1_len, &cfg_file, &cfg_file_len, &use_include_path) == FAILURE) {
			RETURN_FALSE;
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os|sb", &object, tidy_ce_doc,
								 &arg1, &arg1_len, &cfg_file, &cfg_file_len, &use_include_path) == FAILURE) {
			return;
		}

		tidy_instanciate(tidy_ce_doc, return_value TSRMLS_CC);
	}

	obj = (PHPTidyObj *) zend_object_store_get_object(return_value TSRMLS_CC);

	if (is_file) {
		if (!(data = php_tidy_file_to_mem(arg1, use_include_path TSRMLS_CC))) {
			RETURN_FALSE;
		}
	} else {
		data = arg1;
	}

	if (cfg_file && cfg_file[0]) {
		TIDY_SAFE_MODE_CHECK(cfg_file);
		if (tidyLoadConfig(obj->ptdoc->doc, cfg_file) < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not load configuration file '%s'", cfg_file);
			RETVAL_FALSE;
		}
	}

	if (data) {
		if (tidyParseString(obj->ptdoc->doc, data) < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", obj->ptdoc->errbuf->bp);
			RETVAL_FALSE;
		} else {
			obj->ptdoc->parsed = TRUE;
			if (tidyCleanAndRepair(obj->ptdoc->doc) >= 0) {
				TidyBuffer output = {0};

				tidySaveBuffer (obj->ptdoc->doc, &output);
				RETVAL_STRING(output.bp, 1);
				tidyBufFree(&output);
			} else {
				RETVAL_FALSE;
			}
		}
	}

	if (is_file) {
		efree(data);
	}
}

static char *php_tidy_file_to_mem(char *filename, zend_bool use_include_path TSRMLS_DC)
{
	php_stream *stream;
	int len;
	char *data = NULL;

	if (!(stream = php_stream_open_wrapper(filename, "rb", (use_include_path ? USE_PATH : 0) | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL))) {
		return NULL;
	}
	if ((len = php_stream_copy_to_mem(stream, &data, PHP_STREAM_COPY_ALL, 0)) == 0) {
		data = estrdup("");
	}
	php_stream_close(stream);

	return data;
}

static void tidy_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	PHPTidyObj *intern = (PHPTidyObj *)object;

	zend_hash_destroy(intern->std.properties);
	FREE_HASHTABLE(intern->std.properties);

	intern->ptdoc->ref_count--;

	if (intern->ptdoc->ref_count <= 0) {
		tidyBufFree(intern->ptdoc->errbuf);
		efree(intern->ptdoc->errbuf);
		tidyRelease(intern->ptdoc->doc);
		efree(intern->ptdoc);
	}

	efree(object);
}

static void tidy_object_new(zend_class_entry *class_type, zend_object_handlers *handlers,
							zend_object_value *retval, tidy_obj_type objtype TSRMLS_DC)
{
	PHPTidyObj *intern;
	zval *tmp;

	intern = emalloc(sizeof(PHPTidyObj));
	memset(intern, 0, sizeof(PHPTidyObj));
	intern->std.ce = class_type;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	switch(objtype) {
		case is_node:
			break;

		case is_attr:
			break;

		case is_doc:
			tidySetMallocCall(php_tidy_malloc);
			tidySetReallocCall(php_tidy_realloc);
			tidySetFreeCall(php_tidy_free);
			tidySetPanicCall(php_tidy_panic);

			intern->ptdoc = emalloc(sizeof(PHPTidyDoc));
			intern->ptdoc->doc = tidyCreate();
			intern->ptdoc->parsed = FALSE;
			intern->ptdoc->repaired = FALSE;
			intern->ptdoc->ref_count = 1;
			intern->ptdoc->errbuf = emalloc(sizeof(TidyBuffer));
			tidyBufInit(intern->ptdoc->errbuf);

			if (tidySetErrorBuffer(intern->ptdoc->doc, intern->ptdoc->errbuf) != 0) {
				zend_error(E_ERROR, "Could not set Tidy error buffer");
			}

			tidyOptSetBool(intern->ptdoc->doc, TidyForceOutput, yes);
			tidyOptSetBool(intern->ptdoc->doc, TidyMark, no);

			TIDY_SET_DEFAULT_CONFIG(intern->ptdoc->doc);

			tidy_add_default_properties(intern, is_doc TSRMLS_CC);
			break;

		default:
			break;
	}

	retval->handle = zend_objects_store_put(intern, tidy_object_dtor, NULL TSRMLS_CC);
	retval->handlers = handlers;
}

static zend_object_value tidy_object_new_node(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	tidy_object_new(class_type, &tidy_object_handlers_node, &retval, is_node TSRMLS_CC);
	return retval;
}

static zend_object_value tidy_object_new_attr(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	tidy_object_new(class_type, &tidy_object_handlers_attr, &retval, is_attr TSRMLS_CC);
	return retval;
}

static zend_object_value tidy_object_new_doc(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	tidy_object_new(class_type, &tidy_object_handlers_doc, &retval, is_doc TSRMLS_CC);
	return retval;
}

static zend_object_value tidy_object_new_exception(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	tidy_object_new(class_type, &tidy_object_handlers_exception, &retval, is_exception TSRMLS_CC);
	return retval;
}

static zend_class_entry *tidy_get_ce_node(zval *object TSRMLS_DC)
{
	return tidy_ce_node;
}

static zend_class_entry *tidy_get_ce_attr(zval *object TSRMLS_DC)
{
	return tidy_ce_attr;
}

static zend_class_entry *tidy_get_ce_doc(zval *object TSRMLS_DC)
{
	return tidy_ce_doc;
}

static zval * tidy_instanciate(zend_class_entry *pce, zval *object TSRMLS_DC)
{
	if (!object) {
		ALLOC_ZVAL(object);
	}
	Z_TYPE_P(object) = IS_OBJECT;
	object_init_ex(object, pce);
	object->refcount = 1;
	object->is_ref = 1;
	return object;
}

static int tidy_doc_cast_handler(zval *in, zval *out, int type, int free TSRMLS_DC)
{
	TidyBuffer output = {0};
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
			tidySaveBuffer (obj->ptdoc->doc, &output);
			ZVAL_STRINGL(out, output.bp, output.size, TRUE);
			tidyBufFree(&output);
			break;

		default:
			return FAILURE;
	}

	return SUCCESS;
}

static int tidy_node_cast_handler(zval *in, zval *out, int type, int free TSRMLS_DC)
{
	TidyBuffer buf = {0};
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
			tidyNodeGetText(obj->ptdoc->doc, obj->node, &buf);
			ZVAL_STRINGL(out, buf.bp, buf.size, TRUE);
			tidyBufFree(&buf);
			break;

		default:
			return FAILURE;
	}

	return SUCCESS;
}

static void tidy_doc_update_properties(PHPTidyObj *obj TSRMLS_DC)
{
	if (obj->ptdoc->parsed) {
		TidyBuffer output = {0};
		zval *temp;

		tidySaveBuffer (obj->ptdoc->doc, &output);

		if (output.size) {
			MAKE_STD_ZVAL(temp);
			ZVAL_STRINGL(temp, output.bp, output.size, TRUE);
			zend_hash_update(obj->std.properties, "value", sizeof("value"), (void *)&temp, sizeof(zval *), NULL);
		}

		tidyBufFree(&output);

		if (obj->ptdoc->errbuf->size) {
			MAKE_STD_ZVAL(temp);
			ZVAL_STRINGL(temp, obj->ptdoc->errbuf->bp, obj->ptdoc->errbuf->size, TRUE);
			zend_hash_update(obj->std.properties, "error_buf", sizeof("error_buf"), (void *)&temp, sizeof(zval *), NULL);
		}
	}
}

static void tidy_globals_ctor(zend_tidy_globals *g TSRMLS_DC)
{
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

			memset(&buf, 0, sizeof(buf));
			tidyNodeGetText(obj->ptdoc->doc, obj->node, &buf);
			buf.bp[buf.size-1] = '\0';
			ADD_PROPERTY_STRING(obj->std.properties, value, buf.bp);
			tidyBufFree(&buf);

			ADD_PROPERTY_STRING(obj->std.properties, name, tidyNodeGetName(obj->node));
			ADD_PROPERTY_LONG(obj->std.properties, type, tidyNodeGetType(obj->node));

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
					newobj->attr = NULL;
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

		case is_attr:
			ADD_PROPERTY_STRING(obj->std.properties, name, tidyAttrName(obj->attr));
			ADD_PROPERTY_STRING(obj->std.properties, value, tidyAttrValue(obj->attr));
			ADD_PROPERTY_LONG(obj->std.properties, id, tidyAttrGetId(obj->attr));
			break;

		case is_doc:
			ADD_PROPERTY_NULL(obj->std.properties, error_buf);
			ADD_PROPERTY_NULL(obj->std.properties, value);
			break;

		case is_exception:
		default:
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

static void php_tidy_create_node(INTERNAL_FUNCTION_PARAMETERS, tidy_base_nodetypes node)
{
	PHPTidyObj *newobj;
	TIDY_FETCH_PARSED_OBJECT;

	tidy_instanciate(tidy_ce_node, return_value TSRMLS_CC);
	newobj = (PHPTidyObj *) zend_object_store_get_object(return_value TSRMLS_CC);
	newobj->attr = NULL;
	newobj->type = is_node;
	newobj->ptdoc = obj->ptdoc;
	newobj->ptdoc->ref_count++;

	switch(node) {
		case is_root_node:
			newobj->node = tidyGetRoot(newobj->ptdoc->doc);
			break;

		case is_html_node:
			newobj->node = tidyGetHtml(newobj->ptdoc->doc);
			break;

		case is_head_node:
			newobj->node = tidyGetHead(newobj->ptdoc->doc);
			break;

		case is_body_node:
			newobj->node = tidyGetBody(newobj->ptdoc->doc);
			break;
	}

	tidy_add_default_properties(newobj, is_node TSRMLS_CC);
}

static int _php_tidy_apply_config_array(TidyDoc doc, HashTable *ht_options)
{
	char *opt_name;
	zval **opt_val;
	ulong opt_indx;
	
	for (zend_hash_internal_pointer_reset(ht_options);
		 zend_hash_get_current_data(ht_options, (void **)&opt_val) == SUCCESS;
		 zend_hash_move_forward(ht_options)) {
		
		if(zend_hash_get_current_key(ht_options, &opt_name, &opt_indx, FALSE) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not retrieve key from array");
		}
		
		_php_tidy_set_tidy_opt(doc, opt_name, *opt_val);
					
	}
	
	return SUCCESS;
}

static void php_tidy_parse_file(INTERNAL_FUNCTION_PARAMETERS)
{
	char *inputfile;
	int input_len;
	zend_bool use_include_path = 0;
	char *contents;
	zval *options = NULL;
	
	zend_bool is_object = FALSE;

	zval *object = getThis();
	PHPTidyObj *obj;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ab", &inputfile, &input_len, &options, &use_include_path) == FAILURE) {
		RETURN_FALSE;
	}

	if (object) {
		is_object = TRUE;
		obj = (PHPTidyObj *)zend_object_store_get_object(object TSRMLS_CC);

		if (obj->ptdoc->parsed) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot parse two documents with the same tidy instance");
			RETURN_FALSE;
		}
	} else {
		tidy_instanciate(tidy_ce_doc, return_value TSRMLS_CC);
		obj = (PHPTidyObj *) zend_object_store_get_object(return_value TSRMLS_CC);
	}

	if (!(contents = php_tidy_file_to_mem(inputfile, use_include_path TSRMLS_CC))) {
		RETURN_FALSE;
	}

	if(options) {
		_php_tidy_apply_config_array(obj->ptdoc->doc, HASH_OF(options));		
	}
	
	if (tidyParseString(obj->ptdoc->doc, contents) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", obj->ptdoc->errbuf->bp);
		RETVAL_FALSE;
	} else {
		obj->ptdoc->parsed = TRUE;
		tidy_doc_update_properties(obj TSRMLS_CC);

		if (is_object) {
			RETVAL_TRUE;
		}
	}

	efree(contents);
}

PHP_MINIT_FUNCTION(tidy)
{
	ZEND_INIT_MODULE_GLOBALS(tidy, tidy_globals_ctor, tidy_globals_dtor);

	REGISTER_INI_ENTRIES();
	REGISTER_TIDY_CLASS(doc,	NULL);
	REGISTER_TIDY_CLASS(node,	NULL);
	REGISTER_TIDY_CLASS(attr,	NULL);
	REGISTER_TIDY_CLASS(exception,	zend_exception_get_default());

	tidy_object_handlers_doc.get_class_entry = tidy_get_ce_doc;
	tidy_object_handlers_node.get_class_entry = tidy_get_ce_node;
	tidy_object_handlers_attr.get_class_entry = tidy_get_ce_attr;

	tidy_object_handlers_doc.cast_object = tidy_doc_cast_handler;
	tidy_object_handlers_node.cast_object = tidy_node_cast_handler;

	_php_tidy_register_tags(INIT_FUNC_ARGS_PASSTHRU);
	_php_tidy_register_attributes(INIT_FUNC_ARGS_PASSTHRU);
	_php_tidy_register_nodetypes(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}


PHP_MSHUTDOWN_FUNCTION(tidy)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(tidy)
{
	if (INI_BOOL("tidy.clean_output") == TRUE) {
		if (php_start_ob_buffer_named("ob_tidyhandler", 0, 1 TSRMLS_CC) == FAILURE) {
			zend_error(E_NOTICE, "Unable to use Tidy for output buffering.");
		}
	}

	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(tidy)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(tidy)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Tidy support", "enabled");
	php_info_print_table_row(2, "libTidy Library Version", (char *)tidyReleaseDate());
	php_info_print_table_row(2, "Extension Version", PHP_TIDY_MODULE_VERSION " ($Id$)");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

TIDY_ATTR_METHOD(__construct)
{
}

PHP_FUNCTION(ob_tidyhandler)
{
	char *input;
	int input_len;
	long mode;
	TidyBuffer errbuf = {0};
	TidyDoc doc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &input, &input_len, &mode) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	doc = tidyCreate();
	tidyBufInit(&errbuf);

	tidyOptSetBool(doc, TidyForceOutput, yes);
	tidyOptSetBool(doc, TidyMark, no);

	if (tidySetErrorBuffer(doc, &errbuf) != 0) {
		tidyRelease(doc);
		tidyBufFree(&errbuf);

		zend_error(E_ERROR, "Could not set Tidy error buffer");
	}

	TIDY_SET_DEFAULT_CONFIG(doc);

	if (input_len > 1) {
		if (tidyParseString(doc, input) < 0 || tidyCleanAndRepair(doc) < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", errbuf.bp);
		}
		{
			TidyBuffer output = {0};
			tidyBufInit(&output);

			tidySaveBuffer(doc, &output);
			RETVAL_STRING(output.bp, 1);

			tidyBufFree(&output);
		}
	} else {
		RETVAL_NULL();
	}

	tidyRelease(doc);
	tidyBufFree(&errbuf);
}

/* {{{ proto bool tidy_parse_string(string input)
   Parse a document stored in a string */
PHP_FUNCTION(tidy_parse_string)
{
	char *input;
	int input_len;
	zval *options = NULL;
	
	zend_bool is_object = FALSE;

	zval *object = getThis();
	PHPTidyObj *obj;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|a", &input, &input_len, &options) == FAILURE) {
		RETURN_FALSE;
	}

	if (object) {
		is_object = TRUE;
		obj = (PHPTidyObj *)zend_object_store_get_object(object TSRMLS_CC);

		if (obj->ptdoc->parsed) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot parse two documents with the same tidy instance");
			RETURN_FALSE;
		}
	} else {
		tidy_instanciate(tidy_ce_doc, return_value TSRMLS_CC);
		obj = (PHPTidyObj *) zend_object_store_get_object(return_value TSRMLS_CC);
	}

	if(options) {
		_php_tidy_apply_config_array(obj->ptdoc->doc, HASH_OF(options));
	}
	
	if (tidyParseString(obj->ptdoc->doc, input) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", obj->ptdoc->errbuf->bp);
		return;
	}

	obj->ptdoc->parsed = TRUE;

	tidy_doc_update_properties(obj TSRMLS_CC);

	if (is_object) {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ proto string tidy_get_error_buffer([boolean detailed])
   Return warnings and errors which occured parsing the specified document*/
PHP_FUNCTION(tidy_get_error_buffer)
{
	TIDY_FETCH_OBJECT;

	RETURN_STRING(obj->ptdoc->errbuf->bp, 1);
}
/* }}} */

/* {{{ proto string tidy_get_output()
   Return a string representing the parsed tidy markup */
PHP_FUNCTION(tidy_get_output)
{
	TidyBuffer output = {0};
	TIDY_FETCH_PARSED_OBJECT;

	tidySaveBuffer(obj->ptdoc->doc, &output);

	RETVAL_STRING(output.bp, 1);

	tidyBufFree(&output);
}
/* }}} */

/* {{{ proto boolean tidy_parse_file(string file [, array config_options [, bool use_include_path]])
   Parse markup in file or URI */
PHP_FUNCTION(tidy_parse_file)
{
	php_tidy_parse_file(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto boolean tidy_clean_repair()
   Execute configured cleanup and repair operations on parsed markup */
PHP_FUNCTION(tidy_clean_repair)
{
	TIDY_FETCH_OBJECT;

	if (tidyCleanAndRepair(obj->ptdoc->doc) >= 0) {
		obj->ptdoc->repaired = TRUE;
		tidy_doc_update_properties(obj TSRMLS_CC);
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean tidy_repair_string(string data [, string config_file])
   Repair a string using an optionally provided configuration file */
PHP_FUNCTION(tidy_repair_string)
{
	php_tidy_quick_repair(INTERNAL_FUNCTION_PARAM_PASSTHRU, FALSE);
}
/* }}} */

/* {{{ proto boolean tidy_repair_file(string filename [, string config_file [, bool use_include_path]])
   Repair a file using an optionally provided configuration file */
PHP_FUNCTION(tidy_repair_file)
{
	php_tidy_quick_repair(INTERNAL_FUNCTION_PARAM_PASSTHRU, TRUE);
}
/* }}} */

/* {{{ proto boolean tidy_diagnose()
   Run configured diagnostics on parsed and repaired markup. */
PHP_FUNCTION(tidy_diagnose)
{
	TIDY_FETCH_OBJECT;

	if (tidyRunDiagnostics(obj->ptdoc->doc) >= 0) {
		tidy_doc_update_properties(obj TSRMLS_CC);
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string tidy_get_release()
   Get release date (version) for Tidy library */
PHP_FUNCTION(tidy_get_release)
{
	if (ZEND_NUM_ARGS()) {
		WRONG_PARAM_COUNT;
	}

	RETURN_STRING((char *)tidyReleaseDate(), 1);
}
/* }}} */

/* {{{ proto array tidy_get_config()
   Get current Tidy configuarion */
PHP_FUNCTION(tidy_get_config)
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
PHP_FUNCTION(tidy_get_status)
{
	TIDY_FETCH_OBJECT;

	RETURN_LONG(tidyStatus(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto int tidy_get_html_ver()
   Get the Detected HTML version for the specified document. */
PHP_FUNCTION(tidy_get_html_ver)
{
	TIDY_FETCH_PARSED_OBJECT;

	RETURN_LONG(tidyDetectedHtmlVersion(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto boolean tidy_is_xhtml()
   Indicates if the document is a XHTML document. */
PHP_FUNCTION(tidy_is_xhtml)
{
	TIDY_FETCH_PARSED_OBJECT;

	RETURN_BOOL(tidyDetectedXhtml(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto boolean tidy_is_xhtml()
   Indicates if the document is a generic (non HTML/XHTML) XML document. */
PHP_FUNCTION(tidy_is_xml)
{
	TIDY_FETCH_PARSED_OBJECT;

	RETURN_BOOL(tidyDetectedGenericXml(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto int tidy_error_count()
   Returns the Number of Tidy errors encountered for specified document. */
PHP_FUNCTION(tidy_error_count)
{
	TIDY_FETCH_PARSED_OBJECT;

	RETURN_LONG(tidyErrorCount(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto int tidy_warning_count()
   Returns the Number of Tidy warnings encountered for specified document. */
PHP_FUNCTION(tidy_warning_count)
{
	TIDY_FETCH_PARSED_OBJECT;

	RETURN_LONG(tidyWarningCount(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto int tidy_access_count()
   Returns the Number of Tidy accessibility warnings encountered for specified document. */
PHP_FUNCTION(tidy_access_count)
{
	TIDY_FETCH_PARSED_OBJECT;

	RETURN_LONG(tidyAccessWarningCount(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto int tidy_config_count()
   Returns the Number of Tidy configuration errors encountered for specified document. */
PHP_FUNCTION(tidy_config_count)
{
	TIDY_FETCH_PARSED_OBJECT;

	RETURN_LONG(tidyConfigErrorCount(obj->ptdoc->doc));
}
/* }}} */

/* {{{ proto void tidy_load_config(string filename)
   Load an ASCII Tidy configuration file */
PHP_FUNCTION(tidy_load_config)
{
	char *filename;
	int filename_len;

	zval *object = getThis();
	PHPTidyObj *obj;

	if (object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
			RETURN_FALSE;
		}
	} else {
		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, NULL, "Os", &object, tidy_ce_doc, &filename, &filename_len) == FAILURE) {
			RETURN_FALSE;
		}
	}

	TIDY_SAFE_MODE_CHECK(filename);

	obj = (PHPTidyObj *) zend_object_store_get_object(object TSRMLS_CC);

	if (tidyLoadConfig(obj->ptdoc->doc, filename) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not load configuration file '%s'", filename);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void tidy_load_config(string filename, string encoding)
   Load an ASCII Tidy configuration file with the specified encoding */
PHP_FUNCTION(tidy_load_config_enc)
{
	char *filename, *encoding;
	int file_len, enc_len;

	zval *object = getThis();
	PHPTidyObj *obj;

	if (object) {
		if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &filename, &file_len, &encoding, &enc_len) == FAILURE) {
			RETURN_FALSE;
		}
	} else {
		if(zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, NULL, "Oss", &object, tidy_ce_doc,
										&filename, &file_len, &encoding, &enc_len) == FAILURE) {
			RETURN_FALSE;
		}
	}

	TIDY_SAFE_MODE_CHECK(filename);

	obj = (PHPTidyObj *) zend_object_store_get_object(object TSRMLS_CC);

	if (tidyLoadConfigEnc(obj->ptdoc->doc, filename, encoding) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not load configuration file '%s' using encoding '%s'", filename, encoding);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean tidy_set_encoding(string encoding)
   Set the input/output character encoding for parsing markup. Values include: ascii, latin1, raw, utf8, iso2022, mac, win1252, utf16le, utf16be, utf16, big5 and shiftjis. */
PHP_FUNCTION(tidy_set_encoding)
{
	zval *object = getThis();
	PHPTidyObj *obj;
	char *encoding;
	int enc_len;

	if (object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &encoding, &enc_len) == FAILURE) {
			RETURN_FALSE;
		}
	} else {
		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, NULL, "Os", &object, tidy_ce_doc, &encoding, &enc_len) == FAILURE) {
			RETURN_FALSE;
		}
	}

	obj = (PHPTidyObj *) zend_object_store_get_object(object TSRMLS_CC);

	if (tidySetCharEncoding(obj->ptdoc->doc, encoding) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not set encoding '%s'", encoding);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean tidy_snapshot_config()
   Take a snapshot of the configuration for this document so it can be modified
   and later restored. */
PHP_FUNCTION(tidy_snapshot_config)
{
	TIDY_FETCH_OBJECT;

	if (tidyOptSnapshot(obj->ptdoc->doc)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

/* {{{ proto boolean tidy_restore_config()
   Restore the configuration for the document to the last snapshot */
PHP_FUNCTION(tidy_restore_config)
{
	TIDY_FETCH_OBJECT;

	if (tidyOptResetToSnapshot(obj->ptdoc->doc)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

/* {{{ proto boolean tidy_restore_config(boolean ignore_default)
   Reset the configuration for the document to default config */
PHP_FUNCTION(tidy_reset_config)
{
	TIDY_FETCH_OBJECT;

	if (tidyOptResetAllToDefault(obj->ptdoc->doc)) {
		if (TG(default_config) && TG(default_config)[0]) {
			if (tidyLoadConfig(obj->ptdoc->doc, TG(default_config)) < 0) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to load Tidy configuration file at '%s'.", TG(default_config));
				RETURN_FALSE;
			}
		}
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

/* {{{ proto boolean tidy_save_config(string filename)
   Save current settings to named file. Only non-default values are written. */
PHP_FUNCTION(tidy_save_config)
{
	zval *object = getThis();
	PHPTidyObj *obj;
	char *filename;
	int file_len;

	if (object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &file_len) == FAILURE) {
			RETURN_FALSE;
		}
	} else {
		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, NULL, "Os", &object, tidy_ce_doc, &filename, &file_len) == FAILURE) {
			RETURN_FALSE;
		}
	}

	TIDY_SAFE_MODE_CHECK(filename);

	obj = (PHPTidyObj *) zend_object_store_get_object(object TSRMLS_CC);

	if (tidyOptSaveFile(obj->ptdoc->doc, filename) < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not write tidy configuration file '%s'", filename);
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto boolean tidy_setopt(string option, mixed newvalue)
   Updates the configuration settings for the specified tidy document. */
PHP_FUNCTION(tidy_setopt)
{
	zval *object = getThis();
	PHPTidyObj *obj;

	zval *value;
	char *optname;
	int optname_len;

	if (object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &optname, &optname_len, &value) == FAILURE) {
			RETURN_FALSE;
		}
	} else {
		if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, NULL, "Osz", &object, tidy_ce_doc, &optname, &optname_len, &value) == FAILURE) {
			RETURN_FALSE;
		}
	}

	obj = (PHPTidyObj *) zend_object_store_get_object(object TSRMLS_CC);

	if(_php_tidy_set_tidy_opt(obj->ptdoc->doc, optname, value) == SUCCESS) {
		RETURN_TRUE;
	}
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto mixed tidy_getopt(string option)
   Returns the value of the specified configuration option for the tidy document. */
PHP_FUNCTION(tidy_getopt)
{
	zval *object = getThis();
	PHPTidyObj *obj;
	char *optname;
	void *optval;
	int optname_len;
	TidyOption opt;
	TidyOptionType optt;

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
				RETURN_NULL();
			}
			break;

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to determine type of Tidy configuration constant to get");
			break;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto TidyNode tidy_get_root()
   Returns a TidyNode Object representing the root of the tidy parse tree */
PHP_FUNCTION(tidy_get_root)
{
	php_tidy_create_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, is_root_node);
}
/* }}} */

/* {{{ proto TidyNode tidy_get_html()
   Returns a TidyNode Object starting from the <HTML> tag of the tidy parse tree */
PHP_FUNCTION(tidy_get_html)
{
	php_tidy_create_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, is_html_node);
}
/* }}} */

/* {{{ proto TidyNode tidy_get_head()
   Returns a TidyNode Object starting from the <HEAD> tag of the tidy parse tree */
PHP_FUNCTION(tidy_get_head)
{
	php_tidy_create_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, is_head_node);
}
/* }}} */

/* {{{ proto TidyNode tidy_get_body(resource tidy)
   Returns a TidyNode Object starting from the <BODY> tag of the tidy parse tree */
PHP_FUNCTION(tidy_get_body)
{
	php_tidy_create_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, is_body_node);
}
/* }}} */

/* {{{ proto void tidy_node::tidy_node()
   Constructor. */
TIDY_NODE_METHOD(__construct)
{
}
/* }}} */

/* {{{ proto boolean tidy_node::has_children()
   Returns true if this node has children */
TIDY_NODE_METHOD(has_children)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyGetChild(obj->node)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidy_node::has_siblings()
   Returns true if this node has siblings */
TIDY_NODE_METHOD(has_siblings)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyGetNext(obj->node)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidy_node::is_comment()
   Returns true if this node represents a comment */
TIDY_NODE_METHOD(is_comment)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyNodeGetType(obj->node) == TidyNode_Comment) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidy_node::is_html()
   Returns true if this node is part of a HTML document */
TIDY_NODE_METHOD(is_html)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyNodeGetType(obj->node) & (TidyNode_Start | TidyNode_End | TidyNode_StartEnd)) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto boolean tidy_node::is_xhtml()
   Returns true if this node is part of a XHTML document */
TIDY_NODE_METHOD(is_xhtml)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyDetectedXhtml(obj->ptdoc->doc)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidy_node::is_xml()
   Returns true if this node is part of a XML document */
TIDY_NODE_METHOD(is_xml)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyDetectedGenericXml(obj->ptdoc->doc)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidy_node::is_text()
   Returns true if this node represents text (no markup) */
TIDY_NODE_METHOD(is_text)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyNodeGetType(obj->node) == TidyNode_Text) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidy_node::is_jste()
   Returns true if this node is JSTE */
TIDY_NODE_METHOD(is_jste)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyNodeGetType(obj->node) == TidyNode_Jste) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidy_node::is_asp()
   Returns true if this node is ASP */
TIDY_NODE_METHOD(is_asp)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyNodeGetType(obj->node) == TidyNode_Asp) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto boolean tidy_node::is_php()
   Returns true if this node is PHP */
TIDY_NODE_METHOD(is_php)
{
	TIDY_FETCH_ONLY_OBJECT;

	if (tidyNodeGetType(obj->node) == TidyNode_Php) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

void _php_tidy_register_nodetypes(INIT_FUNC_ARGS)
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

void _php_tidy_register_tags(INIT_FUNC_ARGS)
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

void _php_tidy_register_attributes(INIT_FUNC_ARGS)
{
	TIDY_ATTR_CONST(UNKNOWN);
	TIDY_ATTR_CONST(ABBR);
	TIDY_ATTR_CONST(ACCEPT);
	TIDY_ATTR_CONST(ACCEPT_CHARSET);
	TIDY_ATTR_CONST(ACCESSKEY);
	TIDY_ATTR_CONST(ACTION);
	TIDY_ATTR_CONST(ADD_DATE);
	TIDY_ATTR_CONST(ALIGN);
	TIDY_ATTR_CONST(ALINK);
	TIDY_ATTR_CONST(ALT);
	TIDY_ATTR_CONST(ARCHIVE);
	TIDY_ATTR_CONST(AXIS);
	TIDY_ATTR_CONST(BACKGROUND);
	TIDY_ATTR_CONST(BGCOLOR);
	TIDY_ATTR_CONST(BGPROPERTIES);
	TIDY_ATTR_CONST(BORDER);
	TIDY_ATTR_CONST(BORDERCOLOR);
	TIDY_ATTR_CONST(BOTTOMMARGIN);
	TIDY_ATTR_CONST(CELLPADDING);
	TIDY_ATTR_CONST(CELLSPACING);
	TIDY_ATTR_CONST(CHAR);
	TIDY_ATTR_CONST(CHAROFF);
	TIDY_ATTR_CONST(CHARSET);
	TIDY_ATTR_CONST(CHECKED);
	TIDY_ATTR_CONST(CITE);
	TIDY_ATTR_CONST(CLASS);
	TIDY_ATTR_CONST(CLASSID);
	TIDY_ATTR_CONST(CLEAR);
	TIDY_ATTR_CONST(CODE);
	TIDY_ATTR_CONST(CODEBASE);
	TIDY_ATTR_CONST(CODETYPE);
	TIDY_ATTR_CONST(COLOR);
	TIDY_ATTR_CONST(COLS);
	TIDY_ATTR_CONST(COLSPAN);
	TIDY_ATTR_CONST(COMPACT);
	TIDY_ATTR_CONST(CONTENT);
	TIDY_ATTR_CONST(COORDS);
	TIDY_ATTR_CONST(DATA);
	TIDY_ATTR_CONST(DATAFLD);
 /* TIDY_ATTR_CONST(DATAFORMATSAS); */
	TIDY_ATTR_CONST(DATAPAGESIZE);
	TIDY_ATTR_CONST(DATASRC);
	TIDY_ATTR_CONST(DATETIME);
	TIDY_ATTR_CONST(DECLARE);
	TIDY_ATTR_CONST(DEFER);
	TIDY_ATTR_CONST(DIR);
	TIDY_ATTR_CONST(DISABLED);
	TIDY_ATTR_CONST(ENCODING);
	TIDY_ATTR_CONST(ENCTYPE);
	TIDY_ATTR_CONST(FACE);
	TIDY_ATTR_CONST(FOR);
	TIDY_ATTR_CONST(FRAME);
	TIDY_ATTR_CONST(FRAMEBORDER);
	TIDY_ATTR_CONST(FRAMESPACING);
	TIDY_ATTR_CONST(GRIDX);
	TIDY_ATTR_CONST(GRIDY);
	TIDY_ATTR_CONST(HEADERS);
	TIDY_ATTR_CONST(HEIGHT);
	TIDY_ATTR_CONST(HREF);
	TIDY_ATTR_CONST(HREFLANG);
	TIDY_ATTR_CONST(HSPACE);
	TIDY_ATTR_CONST(HTTP_EQUIV);
	TIDY_ATTR_CONST(ID);
	TIDY_ATTR_CONST(ISMAP);
	TIDY_ATTR_CONST(LABEL);
	TIDY_ATTR_CONST(LANG);
	TIDY_ATTR_CONST(LANGUAGE);
	TIDY_ATTR_CONST(LAST_MODIFIED);
	TIDY_ATTR_CONST(LAST_VISIT);
	TIDY_ATTR_CONST(LEFTMARGIN);
	TIDY_ATTR_CONST(LINK);
	TIDY_ATTR_CONST(LONGDESC);
	TIDY_ATTR_CONST(LOWSRC);
	TIDY_ATTR_CONST(MARGINHEIGHT);
	TIDY_ATTR_CONST(MARGINWIDTH);
	TIDY_ATTR_CONST(MAXLENGTH);
	TIDY_ATTR_CONST(MEDIA);
	TIDY_ATTR_CONST(METHOD);
	TIDY_ATTR_CONST(MULTIPLE);
	TIDY_ATTR_CONST(NAME);
	TIDY_ATTR_CONST(NOHREF);
	TIDY_ATTR_CONST(NORESIZE);
	TIDY_ATTR_CONST(NOSHADE);
	TIDY_ATTR_CONST(NOWRAP);
	TIDY_ATTR_CONST(OBJECT);
	TIDY_ATTR_CONST(OnAFTERUPDATE);
	TIDY_ATTR_CONST(OnBEFOREUNLOAD);
	TIDY_ATTR_CONST(OnBEFOREUPDATE);
	TIDY_ATTR_CONST(OnBLUR);
	TIDY_ATTR_CONST(OnCHANGE);
	TIDY_ATTR_CONST(OnCLICK);
	TIDY_ATTR_CONST(OnDATAAVAILABLE);
	TIDY_ATTR_CONST(OnDATASETCHANGED);
	TIDY_ATTR_CONST(OnDATASETCOMPLETE);
	TIDY_ATTR_CONST(OnDBLCLICK);
	TIDY_ATTR_CONST(OnERRORUPDATE);
	TIDY_ATTR_CONST(OnFOCUS);
	TIDY_ATTR_CONST(OnKEYDOWN);
	TIDY_ATTR_CONST(OnKEYPRESS);
	TIDY_ATTR_CONST(OnKEYUP);
	TIDY_ATTR_CONST(OnLOAD);
	TIDY_ATTR_CONST(OnMOUSEDOWN);
	TIDY_ATTR_CONST(OnMOUSEMOVE);
	TIDY_ATTR_CONST(OnMOUSEOUT);
	TIDY_ATTR_CONST(OnMOUSEOVER);
	TIDY_ATTR_CONST(OnMOUSEUP);
	TIDY_ATTR_CONST(OnRESET);
	TIDY_ATTR_CONST(OnROWENTER);
	TIDY_ATTR_CONST(OnROWEXIT);
	TIDY_ATTR_CONST(OnSELECT);
	TIDY_ATTR_CONST(OnSUBMIT);
	TIDY_ATTR_CONST(OnUNLOAD);
	TIDY_ATTR_CONST(PROFILE);
	TIDY_ATTR_CONST(PROMPT);
	TIDY_ATTR_CONST(RBSPAN);
	TIDY_ATTR_CONST(READONLY);
	TIDY_ATTR_CONST(REL);
	TIDY_ATTR_CONST(REV);
	TIDY_ATTR_CONST(RIGHTMARGIN);
	TIDY_ATTR_CONST(ROWS);
	TIDY_ATTR_CONST(ROWSPAN);
	TIDY_ATTR_CONST(RULES);
	TIDY_ATTR_CONST(SCHEME);
	TIDY_ATTR_CONST(SCOPE);
	TIDY_ATTR_CONST(SCROLLING);
	TIDY_ATTR_CONST(SELECTED);
	TIDY_ATTR_CONST(SHAPE);
	TIDY_ATTR_CONST(SHOWGRID);
	TIDY_ATTR_CONST(SHOWGRIDX);
	TIDY_ATTR_CONST(SHOWGRIDY);
	TIDY_ATTR_CONST(SIZE);
	TIDY_ATTR_CONST(SPAN);
	TIDY_ATTR_CONST(SRC);
	TIDY_ATTR_CONST(STANDBY);
	TIDY_ATTR_CONST(START);
	TIDY_ATTR_CONST(STYLE);
	TIDY_ATTR_CONST(SUMMARY);
	TIDY_ATTR_CONST(TABINDEX);
	TIDY_ATTR_CONST(TARGET);
	TIDY_ATTR_CONST(TEXT);
	TIDY_ATTR_CONST(TITLE);
	TIDY_ATTR_CONST(TOPMARGIN);
	TIDY_ATTR_CONST(TYPE);
	TIDY_ATTR_CONST(USEMAP);
	TIDY_ATTR_CONST(VALIGN);
	TIDY_ATTR_CONST(VALUE);
	TIDY_ATTR_CONST(VALUETYPE);
	TIDY_ATTR_CONST(VERSION);
	TIDY_ATTR_CONST(VLINK);
	TIDY_ATTR_CONST(VSPACE);
	TIDY_ATTR_CONST(WIDTH);
	TIDY_ATTR_CONST(WRAP);
	TIDY_ATTR_CONST(XML_LANG);
	TIDY_ATTR_CONST(XML_SPACE);
	TIDY_ATTR_CONST(XMLNS);
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
