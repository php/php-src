/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Uwe Steinmann                                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include <stdlib.h>
#include <errno.h>

extern "C"
{
#include "php.h"
#include "php_globals.h"
#include "ext/standard/head.h"
#include "ext/standard/info.h"
#if 0
#include "ext/standard/php_standard.h"
#include "fopen-wrappers.h"
#endif
#include "SAPI.h"
}

#ifdef PHP_WIN32
#include <winsock.h>
#endif
#ifdef HAVE_MMAP 
#include <sys/mman.h>
#endif

#if HAVE_HWAPI

static int le_hwapip;
static int le_hwapi_objectp;
static int le_hwapi_attributep;
static int le_hwapi_errorp;
static int le_hwapi_contentp;
static int le_hwapi_reasonp;

static zend_class_entry *hw_api_class_entry_ptr;
static zend_class_entry *hw_api_object_class_entry_ptr;
static zend_class_entry *hw_api_attribute_class_entry_ptr;
static zend_class_entry *hw_api_error_class_entry_ptr;
static zend_class_entry *hw_api_content_class_entry_ptr;
static zend_class_entry *hw_api_reason_class_entry_ptr;

#include "php_ini.h"
#include "php_hwapi.h"

//#ifdef __cplusplus
//extern "C" {
#include <sdk/api/api.h>
#include <sdk/hgcsp/apihgcsp.h>
//}
//#endif

function_entry hwapi_functions[] = {
	PHP_FE(hwapi_dummy,								NULL)
	PHP_FE(hwapi_init,								NULL)
	PHP_FE(hwapi_hgcsp,								NULL)
	PHP_FE(hwapi_object,								NULL)
	PHP_FE(hwapi_children,								NULL)
	PHP_FE(hwapi_parents,								NULL)
	PHP_FE(hwapi_find,								NULL)
	PHP_FE(hwapi_identify,								NULL)
	PHP_FE(hwapi_remove,								NULL)
	PHP_FE(hwapi_content,								NULL)
	PHP_FE(hwapi_copy,								NULL)
	PHP_FE(hwapi_link,								NULL)
	PHP_FE(hwapi_move,								NULL)
	PHP_FE(hwapi_lock,								NULL)
	PHP_FE(hwapi_unlock,								NULL)
	PHP_FE(hwapi_replace,								NULL)
	PHP_FE(hwapi_object_new,								NULL)
	PHP_FE(hwapi_object_count,								NULL)
	PHP_FE(hwapi_object_title,								NULL)
	PHP_FE(hwapi_object_attreditable,								NULL)
	PHP_FE(hwapi_object_assign,								NULL)
	PHP_FE(hwapi_object_attribute,								NULL)
	PHP_FE(hwapi_object_insert,								NULL)
	PHP_FE(hwapi_object_remove,								NULL)
	PHP_FE(hwapi_object_value,								NULL)
	PHP_FE(hwapi_attribute_new,								NULL)
	PHP_FE(hwapi_attribute_key,								NULL)
	PHP_FE(hwapi_attribute_value,								NULL)
	PHP_FE(hwapi_attribute_values,								NULL)
	PHP_FE(hwapi_attribute_langdepvalue,								NULL)
	PHP_FE(hwapi_content_new,								NULL)
	{NULL, NULL, NULL}
};

static function_entry php_hw_api_functions[] = {
	{"hgcsp", PHP_FN(hwapi_hgcsp), NULL},
	{"object", PHP_FN(hwapi_object), NULL},
	{"children", PHP_FN(hwapi_children), NULL},
	{"mychildren", PHP_FN(hwapi_mychildren), NULL},
	{"parents", PHP_FN(hwapi_parents), NULL},
	{"find", PHP_FN(hwapi_find), NULL},
	{"identify", PHP_FN(hwapi_identify), NULL},
	{"remove", PHP_FN(hwapi_remove), NULL},
	{"content", PHP_FN(hwapi_content), NULL},
	{"copy", PHP_FN(hwapi_copy), NULL},
	{"link", PHP_FN(hwapi_link), NULL},
	{"move", PHP_FN(hwapi_move), NULL},
	{"lock", PHP_FN(hwapi_lock),	NULL},
	{"unlock", PHP_FN(hwapi_unlock), NULL},
	{"replace", PHP_FN(hwapi_replace), NULL},
	{"insert", PHP_FN(hwapi_insert), NULL},
	{"insertdocument", PHP_FN(hwapi_insertdocument), NULL},
	{"insertcollection", PHP_FN(hwapi_insertcollection), NULL},
	{"srcanchors", PHP_FN(hwapi_srcanchors), NULL},
	{"dstanchors", PHP_FN(hwapi_dstanchors), NULL},
	{"objectbyanchor", PHP_FN(hwapi_objectbyanchor), NULL},
	{"dstofsrcanchor", PHP_FN(hwapi_dstofsrcanchor), NULL},
	{"srcsofdst", PHP_FN(hwapi_srcsofdst), NULL},
	{"checkin", PHP_FN(hwapi_checkin), NULL},
	{"checkout", PHP_FN(hwapi_checkout), NULL},
	{"setcommittedversion", PHP_FN(hwapi_setcommittedversion), NULL},
	{"revert", PHP_FN(hwapi_revert), NULL},
	{"history", PHP_FN(hwapi_history), NULL},
	{"removeversion", PHP_FN(hwapi_removeversion), NULL},
	{"freeversion", PHP_FN(hwapi_freeversion), NULL},
	{"configurationhistory", PHP_FN(hwapi_configurationhistory), NULL},
	{"saveconfiguration", PHP_FN(hwapi_saveconfiguration), NULL},
	{"restoreconfiguration", PHP_FN(hwapi_restoreconfiguration), NULL},
	{"removeconfiguration", PHP_FN(hwapi_removeconfiguration), NULL},
	{"mergeconfiguration", PHP_FN(hwapi_mergeconfiguration), NULL},
	{"user", PHP_FN(hwapi_user), NULL},
	{"userlist", PHP_FN(hwapi_userlist), NULL},
	{"hwstat", PHP_FN(hwapi_hwstat), NULL},
	{"dcstat", PHP_FN(hwapi_dcstat), NULL},
	{"dbstat", PHP_FN(hwapi_dbstat), NULL},
	{"ftstat", PHP_FN(hwapi_ftstat), NULL},
	{"info", PHP_FN(hwapi_info), NULL},
	{NULL, NULL, NULL}
};

static function_entry php_hw_api_object_functions[] = {
	{"hw_api_object", PHP_FN(hwapi_object_new), NULL},
	{"count", PHP_FN(hwapi_object_count), NULL},
	{"title", PHP_FN(hwapi_object_title), NULL},
	{"attributeeditable", PHP_FN(hwapi_object_attreditable), NULL},
	{"assign", PHP_FN(hwapi_object_assign), NULL},
	{"attribute", PHP_FN(hwapi_object_attribute), NULL},
	{"insert", PHP_FN(hwapi_object_insert), NULL},
	{"remove", PHP_FN(hwapi_object_remove), NULL},
	{"value", PHP_FN(hwapi_object_value), NULL},
	{NULL, NULL, NULL}
};

static function_entry php_hw_api_attribute_functions[] = {
	{"hw_api_attribute", PHP_FN(hwapi_attribute_new), NULL},
	{"key", PHP_FN(hwapi_attribute_key), NULL},
	{"value", PHP_FN(hwapi_attribute_value), NULL},
	{"values", PHP_FN(hwapi_attribute_values), NULL},
	{"langdepvalue", PHP_FN(hwapi_attribute_langdepvalue), NULL},
	{NULL, NULL, NULL}
};

static function_entry php_hw_api_error_functions[] = {
	{"count", PHP_FN(hwapi_error_count), NULL},
	{"reason", PHP_FN(hwapi_error_reason), NULL},
	{NULL, NULL, NULL}
};

static function_entry php_hw_api_content_functions[] = {
	{"hw_api_content", PHP_FN(hwapi_content_new), NULL},
	{"read", PHP_FN(hwapi_content_read), NULL},
	{"mimetype", PHP_FN(hwapi_content_mimetype), NULL},
	{NULL, NULL, NULL}
};

static function_entry php_hw_api_reason_functions[] = {
	{"type", PHP_FN(hwapi_reason_type), NULL},
	{"description", PHP_FN(hwapi_reason_description), NULL},
	{NULL, NULL, NULL}
};

void hw_api_class_startup();
void hw_api_object_class_startup();
void hw_api_attribute_class_startup();
void hw_api_error_class_startup();
void hw_api_content_class_startup();
void hw_api_reason_class_startup();
static zval *php_hwapi_object_new(void *obj, int rsrc_type);

zend_module_entry hwapi_module_entry = {
	STANDARD_MODULE_HEADER,
	"hwapi",
	hwapi_functions,
	PHP_MINIT(hwapi),
	PHP_MSHUTDOWN(hwapi),
	PHP_RINIT(hwapi),
	NULL,
	PHP_MINFO(hwapi),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef ZTS
int hwapi_globals_id;
#else
PHP_HWAPI_API zend_hwapi_globals hwapi_globals;
#endif

#ifdef COMPILE_DL_HWAPI
ZEND_GET_MODULE(hwapi)
#endif

static void print_reason(const HW_API_Reason& reason) {
	HW_API_String str_type;
	switch(reason.type()) {
		case HW_API_Reason::HW_API_ERROR:
			str_type = "Error";
			break;
		case HW_API_Reason::HW_API_WARNING:
			str_type = "Warning";
			break;
		case HW_API_Reason::HW_API_MESSAGE:
			str_type = "Message";
			break;
	}
	fprintf(stderr, "%s: %s\n", str_type.string(), reason.description("en").string());
}

static void print_error(const HW_API_Error& error) {
	for (int i=0; i < error.count(); i++) {
		HW_API_Reason reason;
		error.reason(i, reason);
		print_reason(reason);
	}
}

static void print_object(const HW_API_Object& object) {
	fprintf(stderr, "%s\n", object.title("en").string());
	for (int i=0; i < object.count(); i++) {
		HW_API_Attribute attrib;
		object.attribute(i, attrib);
		fprintf(stderr, "%s=%s\n", attrib.key().string(), attrib.value().string());
	}
}

static void php_hwapi_init_globals(zend_hwapi_globals *hwapi_globals) {
}

static void php_free_hwapi(zend_rsrc_list_entry *rsrc) {
	HW_API *obj;

	obj = (HW_API *) (rsrc->ptr);
	delete obj;
}

static void php_free_hwapi_object(zend_rsrc_list_entry *rsrc) {
	HW_API_Object *obj;

	obj = (HW_API_Object *) (rsrc->ptr);
	delete obj;
}

static void php_free_hwapi_attribute(zend_rsrc_list_entry *rsrc) {
	HW_API_Attribute *obj;

	obj = (HW_API_Attribute *) (rsrc->ptr);
	if(obj)
		delete obj;
}

static void php_free_hwapi_error(zend_rsrc_list_entry *rsrc) {
	HW_API_Error *obj;

	obj = (HW_API_Error *) (rsrc->ptr);
	if(obj)
		delete obj;
}

static void php_free_hwapi_content(zend_rsrc_list_entry *rsrc) {
	HW_API_Content *obj;

	obj = (HW_API_Content *) (rsrc->ptr);
	if(obj)
		delete obj;
}

static void php_free_hwapi_reason(zend_rsrc_list_entry *rsrc) {
	HW_API_Reason *obj;

	obj = (HW_API_Reason *) (rsrc->ptr);
	if(obj)
		delete obj;
}

static void print_hwapi_stringarray(const HW_API_StringArray& strings) {
	for(int i=0; i<strings.count(); i++) {
		HW_API_String str;
		strings.string(i, str);
		fprintf(stderr, "%s\n", str.string());
	}
}

static int stringArray2indexArray(pval **return_value, HW_API_StringArray *values) {
	array_init(*return_value);
	for (int i=0; i<values->count(); i++) {
		HW_API_String str;
		values->string(i, str);
		str.string();
		add_next_index_string(*return_value, (char *) str.string(), 1);
	}
	return 1;
}

static int objectArray2indexArray(pval **return_value, HW_API_ObjectArray *objarr) {
	array_init(*return_value);

	for(int i=0; i<objarr->count(); i++) {
		zval *child;
		HW_API_Object obj, *objp;
		objarr->object(i, obj);
		objp = new HW_API_Object(obj);
		child = php_hwapi_object_new(objp, le_hwapi_objectp);
		add_next_index_zval(*return_value, child);
	}
	return 1;
}

static void *php_hwapi_get_object(zval *wrapper, int rsrc_type1) {
	void *obj;
	zval **handle;
	int type;

	if (Z_TYPE_P(wrapper) != IS_OBJECT) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Wrapper is not an object");
	}
	if (zend_hash_find(Z_OBJPROP_P(wrapper), "this", sizeof("this"), (void **)&handle) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Underlying object missing");
	}

	obj = zend_list_find(Z_LVAL_PP(handle), &type);
	if (!obj || (type != rsrc_type1)) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Underlying object missing or of invalid type");
	}
	return obj;
}

static zval *php_hwapi_object_new(void *obj, int rsrc_type) {
	zval *wrapper, *handle;
	int ret;

	MAKE_STD_ZVAL(wrapper);
	if (!obj) {
		ZVAL_NULL(wrapper);
		return wrapper;
	}

	/* construct an object with some methods */
	if(rsrc_type == le_hwapi_attributep)
		object_init_ex(wrapper, hw_api_attribute_class_entry_ptr);
	else if(rsrc_type == le_hwapi_objectp)
		object_init_ex(wrapper, hw_api_object_class_entry_ptr);
	else if(rsrc_type == le_hwapip)
		object_init_ex(wrapper, hw_api_class_entry_ptr);
	else if(rsrc_type == le_hwapi_errorp)
		object_init_ex(wrapper, hw_api_error_class_entry_ptr);
	else if(rsrc_type == le_hwapi_contentp)
		object_init_ex(wrapper, hw_api_content_class_entry_ptr);
	else if(rsrc_type == le_hwapi_reasonp)
		object_init_ex(wrapper, hw_api_reason_class_entry_ptr);

	MAKE_STD_ZVAL(handle);
	ZEND_REGISTER_RESOURCE(handle, obj, rsrc_type);
	zend_hash_update(Z_OBJPROP_P(wrapper), "this", sizeof("this"), &handle, sizeof(zval *), NULL);
	return(wrapper);
}

static HW_API_StringArray *make_HW_API_StringArray(HashTable *lht) {
	int count, j;
	HW_API_StringArray *sarr;

	sarr = new HW_API_StringArray();

	count = zend_hash_num_elements(lht);
	zend_hash_internal_pointer_reset(lht);
	for(j=0; j<count; j++) {
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		switch((*keydata)->type) {
			case IS_STRING:
				sarr->insert((HW_API_String) (*keydata)->value.str.val);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return sarr;
}

static HW_API_object_In *make_HW_API_object_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_object_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_object_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "version"))
					in->setVersion((*keydata)->value.str.val);
				break;
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				}
				break;
				}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_children_In *make_HW_API_children_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_children_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_children_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "objectQuery"))
					in->setObjectQuery((*keydata)->value.str.val);
				break;
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				}
				break;
				}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_parents_In *make_HW_API_parents_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_parents_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_parents_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "objectQuery"))
					in->setObjectQuery((*keydata)->value.str.val);
				break;
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				}
				break;
				}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_find_In *make_HW_API_find_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_find_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_find_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "keyQuery"))
					in->setKeyQuery((*keydata)->value.str.val);
				else if(!strcmp(key, "objectQuery"))
					in->setObjectQuery((*keydata)->value.str.val);
				else if(!strcmp(key, "fullTextQuery"))
					in->setFullTextQuery((*keydata)->value.str.val);
				break;
			case IS_LONG:
				if(!strcmp(key, "startIndex"))
					in->setStartIndex((*keydata)->value.lval);
				else if(!strcmp(key, "numberOfObjectsToGet"))
					in->setNumberOfObjectsToGet((*keydata)->value.lval);
				else if(!strcmp(key, "exactMatchLimit"))
					in->setExactMatchLimit((*keydata)->value.lval);
				break;
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				} else if(!strcmp(key, "languages")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setLanguages(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				} else if(!strcmp(key, "scope")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setScope(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				}

				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_identify_In *make_HW_API_identify_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_identify_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_identify_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "username"))
					in->setUsername((*keydata)->value.str.val);
				else if(!strcmp(key, "password"))
					in->setPassword((*keydata)->value.str.val);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_remove_In *make_HW_API_remove_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_remove_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_remove_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "parentIdentifier"))
					in->setParentIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "objectQuery"))
					in->setObjectQuery((*keydata)->value.str.val);
				break;
			case IS_LONG:
				if(!strcmp(key, "mode"))
					in->setMode((*keydata)->value.lval);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_content_In *make_HW_API_content_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_content_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_content_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				break;
			case IS_LONG:
				if(!strcmp(key, "mode"))
					in->setMode((*keydata)->value.lval);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_copy_In *make_HW_API_copy_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_copy_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_copy_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "destinationParentIdentifier"))
					in->setDestinationParentIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				break;
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_link_In *make_HW_API_link_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_link_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_link_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "destinationParentIdentifier"))
					in->setDestinationParentIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_move_In *make_HW_API_move_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_move_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_move_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "destinationParentIdentifier"))
					in->setDestinationParentIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "sourceParentIdentifier"))
					in->setSourceParentIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_lock_In *make_HW_API_lock_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_lock_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_lock_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectQuery"))
					in->setObjectQuery((*keydata)->value.str.val);
				else if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				break;
			case IS_LONG:
				if(!strcmp(key, "mode"))
					in->setMode((*keydata)->value.lval);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_unlock_In *make_HW_API_unlock_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_unlock_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_unlock_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectQuery"))
					in->setObjectQuery((*keydata)->value.str.val);
				else if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				break;
			case IS_LONG:
				if(!strcmp(key, "mode"))
					in->setMode((*keydata)->value.lval);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_replace_In *make_HW_API_replace_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_replace_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_replace_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				break;
			case IS_OBJECT: {
				zend_class_entry *ce;
				ce = zend_get_class_entry(*keydata);
				if(!strcmp(key, "object")) {
//					if(!((*keydata)->value.obj.ce->name, "hw_api_object")) {
					if(!(ce->name, "hw_api_object")) {
						HW_API_Object *obj;
						obj = (HW_API_Object *) php_hwapi_get_object(*keydata, le_hwapi_objectp);
						in->setObject(*obj);
					}
				} else if(!strcmp(key, "parameters")) {
					if(!(ce->name, "hw_api_object")) {
						HW_API_Object *obj;
						obj = (HW_API_Object *) php_hwapi_get_object(*keydata, le_hwapi_objectp);
						in->setParameters(*obj);
					}
				} else if(!strcmp(key, "content")) {
					if(!(ce->name, "hw_api_content")) {
						HW_API_Content *obj;
						obj = (HW_API_Content *) php_hwapi_get_object(*keydata, le_hwapi_contentp);
						in->setContent(*obj);
					}
				}
				/* FIXME: HW_API_Object and HW_API_Content needs to be handelt */
				break;
				}
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				}
				break;
				}
			case IS_LONG:
				if(!strcmp(key, "mode"))
					in->setMode((*keydata)->value.lval);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_insert_In *make_HW_API_insert_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_insert_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_insert_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_OBJECT: {
				zend_class_entry *ce;
				ce = zend_get_class_entry(*keydata);
				if(!strcmp(key, "object")) {
					if(!strcmp(ce->name, "hw_api_object")) {
						HW_API_Object *obj;
						obj = (HW_API_Object *) php_hwapi_get_object(*keydata, le_hwapi_objectp);
						in->setObject(*obj);
					}
				} else if(!strcmp(key, "parameters")) {
					if(!strcmp(ce->name, "hw_api_object")) {
						HW_API_Object *obj;
						obj = (HW_API_Object *) php_hwapi_get_object(*keydata, le_hwapi_objectp);
						in->setParameters(*obj);
					}
				} else if(!strcmp(key, "content")) {
					if(!strcmp(ce->name, "hw_api_content")) {
						HW_API_Content *obj;
						obj = (HW_API_Content *) php_hwapi_get_object(*keydata, le_hwapi_contentp);
						in->setContent(*obj);
					}
				}
				/* FIXME: HW_API_Object and HW_API_Content needs to be handelt */
				break;
				}
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				}
				break;
				}
			case IS_LONG:
				if(!strcmp(key, "mode"))
					in->setMode((*keydata)->value.lval);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_insertDocument_In *make_HW_API_insertDocument_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_insertDocument_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_insertDocument_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_OBJECT: {
				zend_class_entry *ce;
				ce = zend_get_class_entry(*keydata);
				if(!strcmp(key, "object")) {
					if(!strcmp(ce->name, "hw_api_object")) {
						HW_API_Object *obj;
						obj = (HW_API_Object *) php_hwapi_get_object(*keydata, le_hwapi_objectp);
						in->setObject(*obj);
					}
				} else if(!strcmp(key, "parameters")) {
					if(!strcmp(ce->name, "hw_api_object")) {
						HW_API_Object *obj;
						obj = (HW_API_Object *) php_hwapi_get_object(*keydata, le_hwapi_objectp);
						in->setParameters(*obj);
					}
				} else if(!strcmp(key, "content")) {
					if(!strcmp(ce->name, "hw_api_content")) {
						HW_API_Content *obj;
						obj = (HW_API_Content *) php_hwapi_get_object(*keydata, le_hwapi_contentp);
						in->setContent(*obj);
					}
				}
				/* FIXME: HW_API_Object and HW_API_Content needs to be handelt */
				break;
				}
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				}
				break;
				}
			case IS_LONG:
				if(!strcmp(key, "mode"))
					in->setMode((*keydata)->value.lval);
				break;
			case IS_STRING:
				if(!strcmp(key, "parentIdentifier"))
					in->setParentIdentifier((*keydata)->value.str.val);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_insertCollection_In *make_HW_API_insertCollection_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_insertCollection_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_insertCollection_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "parentIdentifier"))
					in->setParentIdentifier((*keydata)->value.str.val);
				break;
			case IS_OBJECT: {
				zend_class_entry *ce;
				ce = zend_get_class_entry(*keydata);
				if(!strcmp(key, "object")) {
					if(!strcmp(ce->name, "hw_api_object")) {
						HW_API_Object *obj;
						obj = (HW_API_Object *) php_hwapi_get_object(*keydata, le_hwapi_objectp);
						in->setObject(*obj);
					}
				} else if(!strcmp(key, "parameters")) {
					if(!strcmp(ce->name, "hw_api_object")) {
						HW_API_Object *obj;
						obj = (HW_API_Object *) php_hwapi_get_object(*keydata, le_hwapi_objectp);
						in->setParameters(*obj);
					}
				}
				/* FIXME: HW_API_Object and HW_API_Content needs to be handelt */
				break;
				}
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_insertAnchor_In *make_HW_API_insertAnchor_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_insertAnchor_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_insertAnchor_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "documentIdentifier"))
					in->setDocumentIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "destinationIdentifier"))
					in->setDestinationIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "hint"))
					in->setHint((*keydata)->value.str.val);
				break;
			case IS_OBJECT: {
				zend_class_entry *ce;
				ce = zend_get_class_entry(*keydata);
				if(!strcmp(key, "object")) {
					if(!strcmp(ce->name, "hw_api_object")) {
						HW_API_Object *obj;
						obj = (HW_API_Object *) php_hwapi_get_object(*keydata, le_hwapi_objectp);
						in->setObject(*obj);
					}
				} else if(!strcmp(key, "parameters")) {
					if(!strcmp(ce->name, "hw_api_object")) {
						HW_API_Object *obj;
						obj = (HW_API_Object *) php_hwapi_get_object(*keydata, le_hwapi_objectp);
						in->setParameters(*obj);
					}
				}
				/* FIXME: HW_API_Object and HW_API_Content needs to be handelt */
				break;
				}
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_srcAnchors_In *make_HW_API_srcAnchors_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_srcAnchors_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_srcAnchors_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "objectQuery"))
					in->setObjectQuery((*keydata)->value.str.val);
				break;
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_dstAnchors_In *make_HW_API_dstAnchors_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_dstAnchors_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_dstAnchors_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "objectQuery"))
					in->setObjectQuery((*keydata)->value.str.val);
				break;
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_dstOfSrcAnchor_In *make_HW_API_dstOfSrcAnchor_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_dstOfSrcAnchor_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_dstOfSrcAnchor_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				break;
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_objectByAnchor_In *make_HW_API_objectByAnchor_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_objectByAnchor_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_objectByAnchor_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				break;
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_srcsOfDst_In *make_HW_API_srcsOfDst_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_srcsOfDst_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_srcsOfDst_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "objectQuery"))
					in->setObjectQuery((*keydata)->value.str.val);
				break;
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly by freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_checkIn_In *make_HW_API_checkIn_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_checkIn_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_checkIn_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "objectQuery"))
					in->setObjectQuery((*keydata)->value.str.val);
				else if(!strcmp(key, "version"))
					in->setVersion((*keydata)->value.str.val);
				else if(!strcmp(key, "comment"))
					in->setComment((*keydata)->value.str.val);
				break;
			case IS_LONG:
				if(!strcmp(key, "mode"))
					in->setMode((*keydata)->value.lval);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_checkOut_In *make_HW_API_checkOut_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_checkOut_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_checkOut_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "objectQuery"))
					in->setObjectQuery((*keydata)->value.str.val);
				else if(!strcmp(key, "version"))
					in->setVersion((*keydata)->value.str.val);
				break;
			case IS_LONG:
				if(!strcmp(key, "mode"))
					in->setMode((*keydata)->value.lval);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_setCommittedVersion_In *make_HW_API_setCommittedVersion_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_setCommittedVersion_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_setCommittedVersion_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "version"))
					in->setVersion((*keydata)->value.str.val);
				break;
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly be freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_revert_In *make_HW_API_revert_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_revert_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_revert_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "objectQuery"))
					in->setObjectQuery((*keydata)->value.str.val);
				else if(!strcmp(key, "version"))
					in->setVersion((*keydata)->value.str.val);
				break;
			case IS_LONG:
				if(!strcmp(key, "mode"))
					in->setMode((*keydata)->value.lval);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_history_In *make_HW_API_history(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_history_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_history_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
/*				else if(!strcmp(key, "objectQuery"))
					in->objectQuery((*keydata)->value.str.val);*/
				break;
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly be freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_removeVersion_In *make_HW_API_removeVersion_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_removeVersion_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_removeVersion_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "version"))
					in->setVersion((*keydata)->value.str.val);
				break;
			case IS_LONG:
				if(!strcmp(key, "mode"))
					in->setMode((*keydata)->value.lval);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_freeVersion_In *make_HW_API_freeVersion_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_freeVersion_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_freeVersion_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "version"))
					in->setVersion((*keydata)->value.str.val);
				break;
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly be freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_configurationHistory_In *make_HW_API_configurationHistory_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_configurationHistory_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_configurationHistory_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_saveConfiguration_In *make_HW_API_saveConfiguration_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_saveConfiguration_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_saveConfiguration_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "version"))
					in->setVersion((*keydata)->value.str.val);
				else if(!strcmp(key, "comment"))
					in->setComment((*keydata)->value.str.val);
				break;
			case IS_LONG:
				if(!strcmp(key, "mode"))
					in->setMode((*keydata)->value.lval);
				break;
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly be freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_restoreConfiguration_In *make_HW_API_restoreConfiguration_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_restoreConfiguration_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_restoreConfiguration_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "version"))
					in->setVersion((*keydata)->value.str.val);
				break;
			case IS_LONG:
				if(!strcmp(key, "mode"))
					in->setMode((*keydata)->value.lval);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_removeConfiguration_In *make_HW_API_removeConfiguration_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_removeConfiguration_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_removeConfiguration_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "version"))
					in->setVersion((*keydata)->value.str.val);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_mergeConfiguration_In *make_HW_API_mergeConfiguration_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_mergeConfiguration_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_mergeConfiguration_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_STRING:
				if(!strcmp(key, "objectIdentifier"))
					in->setObjectIdentifier((*keydata)->value.str.val);
				else if(!strcmp(key, "version"))
					in->setVersion((*keydata)->value.str.val);
				break;
			case IS_LONG:
				if(!strcmp(key, "mode"))
					in->setMode((*keydata)->value.lval);
				break;
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_user_In *make_HW_API_user_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_user_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_user_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly be freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_userlist_In *make_HW_API_userlist_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_userlist_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_userlist_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly be freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_hwStat_In *make_HW_API_hwStat_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_hwStat_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_hwStat_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly be freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_dcStat_In *make_HW_API_dcStat_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_dcStat_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_dcStat_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly be freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_dbStat_In *make_HW_API_dbStat_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_dbStat_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_dbStat_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly be freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_ftStat_In *make_HW_API_ftStat_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_ftStat_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_ftStat_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly be freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

static HW_API_info_In *make_HW_API_info_In(zval *arg1) {
	int count, i;
	HashTable *lht;
	HW_API_info_In *in;

	lht = arg1->value.ht;

	if(0 == (count = zend_hash_num_elements(lht))) {
		return NULL;
	}

	in = new HW_API_info_In();

	zend_hash_internal_pointer_reset(lht);
	for(i=0; i<count; i++) {
		ulong ind;
		char *key;
		zval **keydata;
		zend_hash_get_current_data(lht, (void **) &keydata);
		zend_hash_get_current_key(lht, &key, &ind, 0);
		switch((*keydata)->type) {
			case IS_ARRAY: {
				HW_API_StringArray *sarr;

				if(!strcmp(key, "attributeSelector")) {
					sarr = make_HW_API_StringArray((*keydata)->value.ht);
					in->setAttributeSelector(*sarr);
					/* FIXME: sarr can be propperly be freed now */
				}
				break;
			}
		}
		zend_hash_move_forward(lht);
	}
	return(in);
}

PHP_INI_BEGIN()
//	STD_PHP_INI_ENTRY("hwapi.allow_persistent", "0", PHP_INI_SYSTEM, OnUpdateInt, allow_persistent, zend_hwapi_globals, hwapi_globals)
PHP_INI_END()

PHP_MINIT_FUNCTION(hwapi) {
	ZEND_INIT_MODULE_GLOBALS(hwapi, php_hwapi_init_globals, NULL);

	le_hwapip = zend_register_list_destructors_ex(php_free_hwapi, NULL, "hw_api", module_number);
	le_hwapi_attributep = zend_register_list_destructors_ex(php_free_hwapi_attribute, NULL, "hw_api_attribute", module_number);
	le_hwapi_objectp = zend_register_list_destructors_ex(php_free_hwapi_object, NULL, "hw_api_object", module_number);
	le_hwapi_errorp = zend_register_list_destructors_ex(php_free_hwapi_error, NULL, "hw_api_error", module_number);
	le_hwapi_contentp = zend_register_list_destructors_ex(php_free_hwapi_content, NULL, "hw_api_content", module_number);
	le_hwapi_reasonp = zend_register_list_destructors_ex(php_free_hwapi_reason, NULL, "hw_api_reason", module_number);
	hw_api_class_startup();
	hw_api_object_class_startup();
	hw_api_attribute_class_startup();
	hw_api_error_class_startup();
	hw_api_content_class_startup();
	hw_api_reason_class_startup();

	REGISTER_LONG_CONSTANT("HW_API_REMOVE_NORMAL", HW_API_remove_In::NORMAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_REMOVE_PHYSICAL", HW_API_remove_In::PHYSICAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_REMOVE_REMOVELINKS", HW_API_remove_In::REMOVELINKS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_REMOVE_NONRECURSIVE", HW_API_remove_In::NONRECURSIVE, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("HW_API_REPLACE_NORMAL", HW_API_replace_In::NORMAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_REPLACE_FORCE_VERSION_CONTROL", HW_API_replace_In::FORCE_VERSION_CONTROL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_REPLACE_AUTOMATIC_CHECKOUT", HW_API_replace_In::AUTOMATIC_CHECKOUT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_REPLACE_AUTOMATIC_CHECKIN", HW_API_replace_In::AUTOMATIC_CHECKIN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_REPLACE_PLAIN", HW_API_replace_In::PLAIN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_REPLACE_REVERT_IF_NOT_CHANGED", HW_API_replace_In::REVERT_IF_NOT_CHANGED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_REPLACE_KEEP_TIME_MODIFIED", HW_API_replace_In::KEEP_TIME_MODIFIED, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("HW_API_INSERT_NORMAL", HW_API_insert_In::NORMAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_INSERT_FORCE_VERSION_CONTROL", HW_API_insert_In::FORCE_VERSION_CONTROL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_INSERT_AUTOMATIC_CHECKOUT", HW_API_insert_In::AUTOMATIC_CHECKOUT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_INSERT_PLAIN", HW_API_insert_In::PLAIN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_INSERT_KEEP_TIME_MODIFIED", HW_API_insert_In::KEEP_TIME_MODIFIED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_INSERT_DELAY_INDEXING", HW_API_insert_In::DELAY_INDEXING, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("HW_API_LOCK_NORMAL", HW_API_lock_In::NORMAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_LOCK_RECURSIVE", HW_API_lock_In::RECURSIVE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_LOCK_SESSION", HW_API_lock_In::SESSION, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("HW_API_CONTENT_ALLLINKS", HW_API_content_In::ALLLINKS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_CONTENT_REACHABLELINKS", HW_API_content_In::REACHABLELINKS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_CONTENT_PLAIN", HW_API_content_In::PLAIN, CONST_CS | CONST_PERSISTENT);

	REGISTER_LONG_CONSTANT("HW_API_REASON_ERROR", HW_API_Reason::HW_API_ERROR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_REASON_WARNING", HW_API_Reason::HW_API_WARNING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("HW_API_REASON_MESSAGE", HW_API_Reason::HW_API_MESSAGE, CONST_CS | CONST_PERSISTENT);

	/* Make sure there are at least default values, though the MessageFilePath
	 * is bogus. If it isn't set at all the web server dies.
	 */
	HW_API_init_In initargs;
	initargs.setArgv0("PHP HWAPI");
	initargs.setMessageFilePath("");
	HW_API::init(initargs);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(hwapi) {
	return SUCCESS;
}

PHP_RINIT_FUNCTION(hwapi) {
	return SUCCESS;
}

PHP_MINFO_FUNCTION(hwapi) {
	php_info_print_table_start();
	php_info_print_table_row(2, "Hyperwave API Support", "enabled");
	php_info_print_table_end();
}

/* {{{ proto string hwapi_dummy(int link, int id, int msgid)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_dummy) {
	pval **arg1, **arg2, **arg3;
	int link, id, type, msgid;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void hwapi_init(string hostname, int port)
   Hyperwave initialisation */
PHP_FUNCTION(hwapi_init) {
	zval **argv[2], *id;
	HW_API_init_In initargs;
	HW_API_init_Out out;
	int ret;
	HW_API_HGCSP *db;
	zval *rv;
	int argc = ZEND_NUM_ARGS();

	if (((argc < 1) || (argc > 2)) || zend_get_parameters_array_ex(argc, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	id = getThis();

	convert_to_string_ex(argv[0]);
	initargs.setArgv0("PHP HWAPI");
	initargs.setMessageFilePath(Z_STRVAL_PP(argv[0]));
	if(argc == 2) {
		convert_to_string_ex(argv[1]);
		initargs.setWhatString(Z_STRVAL_PP(argv[1])) ;
	}
	if(id) {
		db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);
		if(!db) {
			RETURN_FALSE;
		}
		out = db->init(initargs);
	} else {
		out = HW_API::init(initargs);
	}

	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new((HW_API_Error *) err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hwapi_hgcsp(string hostname, int port)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_hgcsp) {
	zval **argv[2];
	HW_API_HGCSP *db;
	zval *rv;
	int argc = ZEND_NUM_ARGS();

	if (((argc < 1) || (argc > 2)) || zend_get_parameters_array_ex(argc, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(argv[0]);
	if(argc == 2) {
		convert_to_long_ex(argv[1]);
		db = new HW_API_HGCSP(Z_STRVAL_PP(argv[0]), Z_LVAL_PP(argv[1]));
	} else {
		db = new HW_API_HGCSP(Z_STRVAL_PP(argv[0]));
	}

	rv = php_hwapi_object_new((HW_API_HGCSP *) db, le_hwapip);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);

//	ret = zend_list_insert(db, le_hwapip);

	/* construct an object with some methods */
//	object_init_ex(return_value, hw_api_class_entry_ptr);
//	add_property_resource(return_value, "this", ret);
}
/* }}} */

/* {{{ proto object hwapi_object(array object_in)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_object) {
	pval **arg1, *rv, *id;
	HW_API_Object *newobj;
	HW_API_object_In *in;
	HW_API_object_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_object_In(*arg1);

	out = db->object(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new((HW_API_Error *) err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	}

	//Frage: Diese Zeile erzeugt erst mit dem Konstruktor von HW_API_Object
	//eine Kopie und danach durch das assign. Wie kann man das verhindern.
	newobj = new HW_API_Object();
	*newobj = out.object();
//	newobj = new HW_API_Object(out.object());

	rv = php_hwapi_object_new(newobj, le_hwapi_objectp);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto string hwapi_mychildren(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_mychildren) {
	zval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_children_In *in;
	HW_API_children_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = new HW_API_children_In();
	in->setObjectIdentifier(Z_STRVAL_PP(arg1));
	out = db->children(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new((HW_API_Error *) err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		objarr = (HW_API_ObjectArray) out.objects();
	}

	array_init(return_value);

	for(i=0; i<objarr.count(); i++) {
		zval *child;
		HW_API_Object obj, *objp;
		objarr.object(i, obj);
		objp = new HW_API_Object(obj);
		child = php_hwapi_object_new(objp, le_hwapi_objectp);
		add_next_index_zval(return_value, child);
	}
	return;
	for(i=0; i<objarr.count(); i++) {
		HW_API_Object obj;
		HW_API_Attribute attr;
		objarr.object(i, obj);
		obj.attribute("GOid", attr);
		add_next_index_string(return_value, (char *) attr.value().string(), 1);
	}
}
/* }}} */

/* {{{ proto string hwapi_children(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_children) {
	zval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_children_In *in;
	HW_API_children_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_children_In(*arg1);

	out = db->children(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new((HW_API_Error *) err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		objarr = (HW_API_ObjectArray) out.objects();
	}

	objectArray2indexArray(&return_value, &objarr);
}
/* }}} */

/* {{{ proto string hwapi_parents(string hostname, int port)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_parents) {
	zval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_parents_In *in;
	HW_API_parents_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_parents_In(*arg1);

	out = db->parents(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new((HW_API_Error *) err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		objarr = (HW_API_ObjectArray) out.objects();
	}

	objectArray2indexArray(&return_value, &objarr);
}
/* }}} */

/* {{{ proto string hwapi_find(array parameters)
   Finds objects */
PHP_FUNCTION(hwapi_find) {
	zval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_find_In *in;
	HW_API_find_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_find_In(*arg1);

	out = db->find(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new((HW_API_Error *) err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} 
	objarr = (HW_API_ObjectArray) out.objects();

	/* FIXME: No a good idea to return just the objects. There is actually
	 * more to return. Changing this to an object of type hw_api_find_out would
	 * mean to change hw_api_parents() and hw_api_children() as well. */
	array_init(return_value);

	/* FIXME: More than just the list of objects is returned by find() */
	if(0 == objectArray2indexArray(&rv, &objarr)) {
		RETURN_FALSE;
	}
	zend_hash_add(return_value->value.ht, "objects", 8, &rv, sizeof(zval *), NULL);
	add_assoc_long(return_value, "endIndex", out.endIndex());
	add_assoc_bool(return_value, "haveMore", out.haveMore() ? true : false);
	add_assoc_long(return_value, "numberOfThingsFound", out.numberOfThingsFound());
}
/* }}} */

/* {{{ proto string hwapi_identify(string hostname, int port)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_identify) {
	pval **arg1, *id, *rv;
	HW_API_identify_In *in;
	HW_API_identify_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_identify_In(*arg1);

	out = db->identify(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new((HW_API_Error *) err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	}

	printf("hwapi_identify\n");
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hwapi_remove(array parameters)
   Remove an object */
PHP_FUNCTION(hwapi_remove) {
	pval **arg1, *id, *rv;
	HW_API_remove_In *in;
	HW_API_remove_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_remove_In(*arg1);

	out = db->remove(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	}

	printf("hwapi_remove\n");
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hwapi_content(string hostname, int port)
   Retrieve content of object */
PHP_FUNCTION(hwapi_content) {
	pval **arg1, *id, *rv;
	HW_API_content_In *in;
	HW_API_content_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_content_In(*arg1);

	out = db->content(*in);
	delete in;
	if (!out.error().error()) {
		HW_API_Content *content = new HW_API_Content(out.content());
		rv = php_hwapi_object_new(content, le_hwapi_contentp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto string hwapi_copy(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_copy) {
	pval **arg1, *id, *rv;
	HW_API_copy_In *in;
	HW_API_copy_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_copy_In(*arg1);

	out = db->copy(*in);
	delete in;
	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new((HW_API_Object*) object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new((HW_API_Error *) err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);

	printf("hwapi_copy\n");
}
/* }}} */

/* {{{ proto string hwapi_link(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_link) {
	pval **arg1, *id, *rv;
	HW_API_link_In *in;
	HW_API_link_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_link_In(*arg1);

	out = db->link(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	}

	printf("hwapi_link\n");
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hwapi_move(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_move) {
	pval **arg1, *id, *rv;
	HW_API_move_In *in;
	HW_API_move_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_move_In(*arg1);

	out = db->move(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	}

	printf("hwapi_move\n");
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hwapi_lock(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_lock) {
	pval **arg1, *id, *rv;
	HW_API_lock_In *in;
	HW_API_lock_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_lock_In(*arg1);

	out = db->lock(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	}
	
	printf("hwapi_lock\n");
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hwapi_unlock(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_unlock) {
	pval **arg1, *id, *rv;
	HW_API_unlock_In *in;
	HW_API_unlock_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_unlock_In(*arg1);

	out = db->unlock(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	}
	
	printf("hwapi_unlock\n");
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hwapi_replace(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_replace) {
	pval **arg1, *id, *rv;
	HW_API_replace_In *in;
	HW_API_replace_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_replace_In(*arg1);

	out = db->replace(*in);
	delete in;
	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto string hwapi_insert(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_insert) {
	pval **arg1, *id, *rv;
	HW_API_insert_In *in;
	HW_API_insert_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_insert_In(*arg1);

	out = db->insert(*in);
	delete in;
	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto string hwapi_insertdocument(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_insertdocument) {
	pval **arg1, *id, *rv;
	HW_API_insertDocument_In *in;
	HW_API_insertDocument_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_insertDocument_In(*arg1);

	out = db->insertDocument(*in);
	delete in;
	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
	
	printf("hwapi_insertdocument\n");
}
/* }}} */

/* {{{ proto string hwapi_insertcollection(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_insertcollection) {
	pval **arg1, *id, *rv;
	HW_API_insertCollection_In *in;
	HW_API_insertCollection_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_insertCollection_In(*arg1);

	out = db->insertCollection(*in);
	delete in;
	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new((HW_API_Error *) &err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
	
	printf("hwapi_insertcollection\n");
}
/* }}} */

/* {{{ proto string hwapi_insertanchor(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_insertanchor) {
	pval **arg1, *id, *rv;
	HW_API_insertAnchor_In *in;
	HW_API_insertAnchor_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_insertAnchor_In(*arg1);

	out = db->insertAnchor(*in);
	delete in;
	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
	
	printf("hwapi_insertanchor\n");
}
/* }}} */

/* {{{ proto string hwapi_srcanchors(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_srcanchors) {
	pval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_srcAnchors_In *in;
	HW_API_srcAnchors_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_srcAnchors_In(*arg1);

	out = db->srcAnchors(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		objarr = (HW_API_ObjectArray) out.objects();
	}

	objectArray2indexArray(&return_value, &objarr);
}
/* }}} */

/* {{{ proto string hwapi_dstanchors(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_dstanchors) {
	pval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_dstAnchors_In *in;
	HW_API_dstAnchors_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_dstAnchors_In(*arg1);

	out = db->dstAnchors(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		objarr = (HW_API_ObjectArray) out.objects();
	}

	objectArray2indexArray(&return_value, &objarr);
}
/* }}} */

/* {{{ proto string hwapi_objectbyanchor(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_objectbyanchor) {
	pval **arg1, *id, *rv;
	HW_API_objectByAnchor_In *in;
	HW_API_objectByAnchor_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_objectByAnchor_In(*arg1);
	/* FIXME: return value of any make_HW_API_xxx function should be checked
	 * for NULL
	 */

	out = db->objectByAnchor(*in);
	delete in;
	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);

	printf("hwapi_objectbyanchor\n");
}
/* }}} */

/* {{{ proto string hwapi_dstofsrcanchor(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_dstofsrcanchor) {
	pval **arg1, *id, *rv;
	HW_API_dstOfSrcAnchor_In *in;
	HW_API_dstOfSrcAnchor_Out out;
	HW_API_HGCSP *db;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_dstOfSrcAnchor_In(*arg1);

	out = db->dstOfSrcAnchor(*in);
	delete in;
	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);

	printf("hwapi_dstofsrcanchor\n");
}
/* }}} */

/* {{{ proto string hwapi_srcsofdst(array parameters)
   Hyperwave dummy function */
PHP_FUNCTION(hwapi_srcsofdst) {
	pval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_srcsOfDst_In *in;
	HW_API_srcsOfDst_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_srcsOfDst_In(*arg1);

	out = db->srcsOfDst(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		objarr = (HW_API_ObjectArray) out.objects();
	}

	objectArray2indexArray(&return_value, &objarr);
}
/* }}} */

/* {{{ proto string hwapi_checkin(array parameters)
   Checking in a document */
PHP_FUNCTION(hwapi_checkin) {
	pval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_checkIn_In *in;
	HW_API_checkIn_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_checkIn_In(*arg1);

	out = db->checkIn(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		RETURN_TRUE;
	}

	printf("hwapi_checkin\n");
}
/* }}} */

/* {{{ proto string hwapi_checkout(array parameters)
   Checking out a document */
PHP_FUNCTION(hwapi_checkout) {
	pval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_checkOut_In *in;
	HW_API_checkOut_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_checkOut_In(*arg1);

	out = db->checkOut(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		RETURN_TRUE;
	}

	printf("hwapi_checkout\n");
}
/* }}} */

/* {{{ proto string hwapi_setcommittedversion(array parameters)
   setcommittedversion */
PHP_FUNCTION(hwapi_setcommittedversion) {
	pval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_setCommittedVersion_In *in;
	HW_API_setCommittedVersion_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_setCommittedVersion_In(*arg1);

	out = db->setCommittedVersion(*in);
	delete in;
	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);

	printf("hwapi_setcommittedversion\n");
}
/* }}} */

/* {{{ proto string hwapi_revert(array parameters)
   Reverting to a former document */
PHP_FUNCTION(hwapi_revert) {
	pval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_revert_In *in;
	HW_API_revert_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_revert_In(*arg1);

	out = db->revert(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		RETURN_TRUE;
	}

	printf("hwapi_revert\n");
}
/* }}} */

/* {{{ proto string hwapi_history(array parameters)
   history */
PHP_FUNCTION(hwapi_history) {
	pval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_history_In *in;
	HW_API_history_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_history(*arg1);

	out = db->history(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new((HW_API_Error *) err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		objarr = (HW_API_ObjectArray) out.objects();
	}

	objectArray2indexArray(&return_value, &objarr);
}
/* }}} */

/* {{{ proto string hwapi_removeversion(array parameters)
   Reverting to a former document */
PHP_FUNCTION(hwapi_removeversion) {
	pval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_removeVersion_In *in;
	HW_API_removeVersion_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_removeVersion_In(*arg1);

	out = db->removeVersion(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		RETURN_TRUE;
	}

	printf("hwapi_removeversion\n");
}
/* }}} */

/* {{{ proto object hwapi_freeversion(array parameters)
   freeversion */
PHP_FUNCTION(hwapi_freeversion) {
	pval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_freeVersion_In *in;
	HW_API_freeVersion_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_freeVersion_In(*arg1);

	out = db->freeVersion(*in);
	delete in;
	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);

	printf("hwapi_freeversion\n");
}
/* }}} */

/* {{{ proto array hwapi_configurationhistory(array parameters)
   Returns configuration history of object */
PHP_FUNCTION(hwapi_configurationhistory) {
	zval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_configurationHistory_In *in;
	HW_API_configurationHistory_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_configurationHistory_In(*arg1);

	out = db->configurationHistory(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new((HW_API_Error *) err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		objarr = (HW_API_ObjectArray) out.objects();
	}

	objectArray2indexArray(&return_value, &objarr);
}
/* }}} */

/* {{{ proto object hwapi_saveconfiguration(array parameters)
   Save configuration for an object */
PHP_FUNCTION(hwapi_saveconfiguration) {
	pval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_saveConfiguration_In *in;
	HW_API_saveConfiguration_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_saveConfiguration_In(*arg1);

	out = db->saveConfiguration(*in);
	delete in;
	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);

	printf("hwapi_saveconfiguration\n");
}
/* }}} */

/* {{{ proto object hwapi_restoreconfiguration(array parameters)
   Restore configuration for an object */
PHP_FUNCTION(hwapi_restoreconfiguration) {
	pval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_restoreConfiguration_In *in;
	HW_API_restoreConfiguration_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_restoreConfiguration_In(*arg1);

	out = db->restoreConfiguration(*in);
	delete in;
	if (!out.error().error()) {
		RETURN_STRING((char *) (out.progressIdentifier().string()), 1);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);

	printf("hwapi_restoreconfiguration\n");
}
/* }}} */

/* {{{ proto object hwapi_mergeconfiguration(array parameters)
   Merge configuration for an object */
PHP_FUNCTION(hwapi_mergeconfiguration) {
	pval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_mergeConfiguration_In *in;
	HW_API_mergeConfiguration_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_mergeConfiguration_In(*arg1);

	out = db->mergeConfiguration(*in);
	delete in;
	if (!out.error().error()) {
		RETURN_STRING((char *) (out.progressIdentifier().string()), 1);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);

	printf("hwapi_mergeconfiguration\n");
}
/* }}} */

/* {{{ proto object hwapi_removeconfiguration(array parameters)
   Removes configuration */
PHP_FUNCTION(hwapi_removeconfiguration) {
	pval **arg1, *id, *rv;
	HW_API_ObjectArray objarr;
	HW_API_removeConfiguration_In *in;
	HW_API_removeConfiguration_Out out;
	HW_API_HGCSP *db;
	int i;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_array_ex(arg1);

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	in = make_HW_API_removeConfiguration_In(*arg1);

	out = db->removeConfiguration(*in);
	delete in;
	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		RETURN_TRUE;
	}

	printf("hwapi_removeconfiguration\n");
}
/* }}} */

/* {{{ proto string hwapi_user(array parameters)
   Returns information about user */
PHP_FUNCTION(hwapi_user) {
	pval **arg1, *id, *rv;
	HW_API_user_In *in;
	HW_API_user_Out out;
	HW_API_HGCSP *db;
	int argc;

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	argc = ZEND_NUM_ARGS();
	switch(argc) {
		case 0:
			out = db->user(HW_API_user_In());
			break;
		case 1:
			if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_array_ex(arg1);
			in = make_HW_API_user_In(*arg1);
			if(NULL == in)
				out = db->user(HW_API_user_In());
			else
				out = db->user(*in);
			delete in;
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);

}
/* }}} */

/* {{{ proto string hwapi_userlist(array parameters)
   Returns list of login in users */
PHP_FUNCTION(hwapi_userlist) {
	zval **arg1, *id, *rv;
	HW_API_userlist_In *in;
	HW_API_userlist_Out out;
	HW_API_ObjectArray objarr;
	HW_API_HGCSP *db;
	int i;
	int argc;

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	argc = ZEND_NUM_ARGS();
	switch(argc) {
		case 0:
			out = db->userlist(HW_API_userlist_In());
			break;
		case 1:
			if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_array_ex(arg1);
			in = make_HW_API_userlist_In(*arg1);
			if(NULL == in)
				out = db->userlist(HW_API_userlist_In());
			else
				out = db->userlist(*in);
			delete in;
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if (out.error().error()) {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new((HW_API_Error *) err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		objarr = (HW_API_ObjectArray) out.objects();
	}

	objectArray2indexArray(&return_value, &objarr);
}
/* }}} */

/* {{{ proto string hwapi_hwstat(array parameters)
   Returns information about hgserver */
PHP_FUNCTION(hwapi_hwstat) {
	pval **arg1, *id, *rv;
	HW_API_hwStat_In *in;
	HW_API_hwStat_Out out;
	HW_API_HGCSP *db;
	int argc;

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	argc = ZEND_NUM_ARGS();
	switch(argc) {
		case 0:
			out = db->hwStat(HW_API_hwStat_In());
			break;
		case 1:
			if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_array_ex(arg1);
			in = make_HW_API_hwStat_In(*arg1);
			if(NULL == in)
				out = db->hwStat(HW_API_hwStat_In());
			else
				out = db->hwStat(*in);
			delete in;
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto string hwapi_dcstat(array parameters)
   Returns information about hgserver */
PHP_FUNCTION(hwapi_dcstat) {
	pval **arg1, *id, *rv;
	HW_API_dcStat_In *in;
	HW_API_dcStat_Out out;
	HW_API_HGCSP *db;
	int argc;

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	argc = ZEND_NUM_ARGS();
	switch(argc) {
		case 0:
			out = db->dcStat(HW_API_dcStat_In());
			break;
		case 1:
			if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_array_ex(arg1);
			in = make_HW_API_dcStat_In(*arg1);
			if(NULL == in)
				out = db->dcStat(HW_API_dcStat_In());
			else
				out = db->dcStat(*in);
			delete in;
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto string hwapi_dbstat(array parameters)
   Returns information about hgserver */
PHP_FUNCTION(hwapi_dbstat) {
	pval **arg1, *id, *rv;
	HW_API_dbStat_In *in;
	HW_API_dbStat_Out out;
	HW_API_HGCSP *db;
	int argc;

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	argc = ZEND_NUM_ARGS();
	switch(argc) {
		case 0:
			out = db->dbStat(HW_API_dbStat_In());
			break;
		case 1:
			if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_array_ex(arg1);
			in = make_HW_API_dbStat_In(*arg1);
			if(NULL == in)
				out = db->dbStat(HW_API_dbStat_In());
			else
				out = db->dbStat(*in);
			delete in;
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto string hwapi_ftstat(array parameters)
   Returns information about ftserver */
PHP_FUNCTION(hwapi_ftstat) {
	pval **arg1, *id, *rv;
	HW_API_ftStat_In *in;
	HW_API_ftStat_Out out;
	HW_API_HGCSP *db;
	int argc;

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	argc = ZEND_NUM_ARGS();
	switch(argc) {
		case 0:
			out = db->ftStat(HW_API_ftStat_In());
			break;
		case 1:
			if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_array_ex(arg1);
			in = make_HW_API_ftStat_In(*arg1);
			if(NULL == in)
				out = db->ftStat(HW_API_ftStat_In());
			else
				out = db->ftStat(*in);
			delete in;
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if (!out.error().error()) {
		HW_API_Object *object = new HW_API_Object(out.object());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
	}
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto string hwapi_info(array parameters)
   Returns information about server */
PHP_FUNCTION(hwapi_info) {
	pval **arg1, *id, *rv, *rv1, *rv2, *rv3;
	HW_API_info_In *in;
	HW_API_info_Out out;
	HW_API_HGCSP *db;
	int argc;

	id = getThis();
	db = (HW_API_HGCSP *) php_hwapi_get_object(id, le_hwapip);

	if(!db) {
		RETURN_FALSE;
	}

	argc = ZEND_NUM_ARGS();
	switch(argc) {
		case 0:
			out = db->info(HW_API_info_In());
			break;
		case 1:
			if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_array_ex(arg1);
			in = make_HW_API_info_In(*arg1);
			if(NULL == in)
				out = db->info(HW_API_info_In());
			else
				out = db->info(*in);
			delete in;
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if (!out.error().error()) {
		HW_API_StringArray languages, customidx, systemidx;
		array_init(return_value);
		HW_API_Object *object = new HW_API_Object(out.typeInfo());
		rv = php_hwapi_object_new(object, le_hwapi_objectp);
		zend_hash_add(return_value->value.ht, "typeInfo", 9, &rv, sizeof(zval *), NULL);
		languages = out.languages();
		MAKE_STD_ZVAL(rv1);
		if(0 == stringArray2indexArray(&rv1, &languages))
			RETURN_FALSE;
		zend_hash_add(return_value->value.ht, "languages", 10, &rv1, sizeof(zval *), NULL);
		customidx = out.customIdx();
		MAKE_STD_ZVAL(rv2);
		if(0 == stringArray2indexArray(&rv2, &customidx))
			RETURN_FALSE;
		zend_hash_add(return_value->value.ht, "customIdx", 10, &rv2, sizeof(zval *), NULL);
		systemidx = out.systemIdx();
		MAKE_STD_ZVAL(rv3);
		if(0 == stringArray2indexArray(&rv3, &systemidx))
			RETURN_FALSE;
		zend_hash_add(return_value->value.ht, "systemIdx", 10, &rv3, sizeof(zval *), NULL);
	} else {
		HW_API_Error *err = new HW_API_Error(out.error());
		rv = php_hwapi_object_new(err, le_hwapi_errorp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
	}
}
/* }}} */

/* {{{ proto string hwapi_object_new()
   Creates new HW_API_Object */
PHP_FUNCTION(hwapi_object_new) {
	pval **arg1, **arg2, *rv;
	HW_API_Object *obj;
	const HW_API_Object *srcobj;
	int ret;

	switch(ZEND_NUM_ARGS()) {
		case 0:
			obj = new HW_API_Object();
			break;
		case 1:
			if (zend_get_parameters_ex(1, &arg1) == FAILURE)
				WRONG_PARAM_COUNT;
				srcobj = (const HW_API_Object *) php_hwapi_get_object(*arg1, le_hwapi_objectp);
				obj = new HW_API_Object(*srcobj);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	rv = php_hwapi_object_new(obj, le_hwapi_objectp);
//zend_print_pval_r(rv, 0);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto string hwapi_object_count()
   Counts number of attributes of an HW_API_Object */
PHP_FUNCTION(hwapi_object_count) {
	zval *id, **tmp;
	HW_API_Object *objp;
	int value;

	id = getThis();
	objp = (HW_API_Object *) php_hwapi_get_object(id, le_hwapi_objectp);

	if(!objp) {
		RETURN_FALSE;
	}
	value = objp->count();

	RETURN_LONG(value);
}
/* }}} */

/* {{{ proto string hwapi_object_title(string language)
   Returns title of HW_API_Object for given language */
PHP_FUNCTION(hwapi_object_title) {
	zval **arg1, *id, **tmp;
	HW_API_Object *objp;
	char *value;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	id = getThis();
	objp = (HW_API_Object *) php_hwapi_get_object(id, le_hwapi_objectp);

	convert_to_string_ex(arg1);

	// Warning: It is import to duplicate the string before RETURN.
	// If that is not done the HW_API_String destructor will be called
	// before RETURN_STRING can duplicate the string.
	value = (char *) estrdup(objp->title(Z_STRVAL_PP(arg1)).string());

	RETURN_STRING(value, 0);
}
/* }}} */

/* {{{ proto string hwapi_object_attreditable(int attr, string username, bool is_system)
   Hyperwave object_attreditable function */
PHP_FUNCTION(hwapi_object_attreditable) {
	zval *id, **arg1, **arg2, **arg3;
	HW_API_Object *objp;
	HW_API_Attribute *attrp;
	bool value;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	id = getThis();
	objp = (HW_API_Object *) php_hwapi_get_object(id, le_hwapi_objectp);
	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	switch(Z_TYPE_PP(arg1)) {
		case IS_STRING:
			convert_to_string_ex(arg1);
			value = objp->attributeEditable((char *) Z_STRVAL_PP(arg1), (char *) Z_STRVAL_PP(arg2), (bool) Z_LVAL_PP(arg3));
			break;
		case IS_OBJECT:
			attrp = (HW_API_Attribute *) php_hwapi_get_object(*arg1, le_hwapi_attributep);
			value = objp->attributeEditable(*attrp, (char *) Z_STRVAL_PP(arg2), (bool) Z_LVAL_PP(arg3));
			break;
	}
	if(value) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto object hwapi_object_assign(int object)
   Hyperwave object_assign function */
PHP_FUNCTION(hwapi_object_assign) {
	zval *id, **arg1, **arg2;

	printf("hwapi_object_assign\n");
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hwapi_object_attribute(int index, object &attribute)
   Hyperwave object_attribute function */
PHP_FUNCTION(hwapi_object_attribute) {
	zval **arg1, *id, *rv;
	HW_API_Attribute *attrp, attr;
	HW_API_Object *objp;
	int error;

	if ((ZEND_NUM_ARGS() != 1) || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	id = getThis();
	objp = (HW_API_Object *) php_hwapi_get_object(id, le_hwapi_objectp);
	if(!objp) {
		RETURN_FALSE;
	}

	switch(Z_TYPE_PP(arg1)) {
		case IS_LONG:
			error = objp->attribute(Z_LVAL_PP(arg1), attr);
			break;
		case IS_STRING:
			error = objp->attribute(HW_API_String(Z_STRVAL_PP(arg1)), attr);
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "HW_API_Object::attribute() needs string or long as parameter");
			RETURN_FALSE;
	}

	if(error) {
		attrp = new HW_API_Attribute(attr);
		rv = php_hwapi_object_new(attrp, le_hwapi_attributep);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string hwapi_object_insert(object attr)
   Inserts new HW_API_Attribute into HW_API_Object */
PHP_FUNCTION(hwapi_object_insert) {
	zval **arg1, *id;
	HW_API_Object *objp;
	HW_API_Attribute *attrp;
	char *value;

	if (ZEND_NUM_ARGS() != 1 || (zend_get_parameters_ex(1, &arg1) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}
	id = getThis();
	objp = (HW_API_Object *) php_hwapi_get_object(id, le_hwapi_objectp);
	if(!objp) {
		RETURN_FALSE;
	}
	attrp = (HW_API_Attribute *) php_hwapi_get_object(*arg1, le_hwapi_attributep);

	objp->insert(*attrp);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string hwapi_object_remove(string name)
   Removes HW_API_Attribute with given name from HW_API_Object */
PHP_FUNCTION(hwapi_object_remove) {
	zval **arg1, *id, **tmp;
	HW_API_Object *objp;
	int error;

	if ((ZEND_NUM_ARGS() != 1) || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	id = getThis();
	objp = (HW_API_Object *) php_hwapi_get_object(id, le_hwapi_objectp);
	if(!objp) {
		RETURN_FALSE;
	}

	error = objp->remove(Z_STRVAL_PP(arg1));

	if(!error) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string hwapi_object_value(string name)
   Returns attribute value of given attribute */
PHP_FUNCTION(hwapi_object_value) {
	zval **arg1, *id;
	HW_API_Object *objp;
	HW_API_Attribute *attrp;
	HW_API_String value;
	int error;

	if ((ZEND_NUM_ARGS() != 1) || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	id = getThis();
	objp = (HW_API_Object *) php_hwapi_get_object(id, le_hwapi_objectp);
	if(!objp) {
		RETURN_FALSE;
	}

	error = objp->value((HW_API_String) Z_STRVAL_PP(arg1), value);

	printf("hwapi_object_value\n");
	if(error) {
		char *str = (char *) estrdup(value.string());
		RETURN_STRING(str, 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string hwapi_attribute_new([string name][, string value])
   Creates new HW_API_Attribute */
PHP_FUNCTION(hwapi_attribute_new) {
	zval *rv, **arg1, **arg2;
	HW_API_Attribute *attrp;
	int ret;

	/* FIXME: I'm not sure if the constructor of HW_API_Attribute takes normal C-Strings
	 * or if it has to be HW_API_String. Currently C-Strings are passed.
	 */
	switch(ZEND_NUM_ARGS()) {
		case 0:
			break;
			attrp = new HW_API_Attribute();
		case 1:
			if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(arg1);
			attrp = new HW_API_Attribute(Z_STRVAL_PP(arg1));
			break;
		case 2: //* FIXME: Second Parameter can be string or array of strings
			if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(arg1);
			switch((*arg2)->type) {
				case IS_ARRAY: {
					HashTable *lht = (*arg2)->value.ht;
					int i, count;
					HW_API_StringArray values;
					if(NULL == lht)
						RETURN_FALSE;

					if(0 == (count = zend_hash_num_elements(lht))) {
						attrp = new HW_API_Attribute(Z_STRVAL_PP(arg1));
						break;
					}
					zend_hash_internal_pointer_reset(lht);
					for(i=0; i<count; i++) {
						zval *keydata, **keydataptr;
						zend_hash_get_current_data(lht, (void **) &keydataptr);
						/* FIXME: just hope this doesn't do any harm. Maybe convert_to_string
						 * changes the type of the array element */
						convert_to_string_ex(keydataptr);
						keydata = *keydataptr;
						values.insert(keydata->value.str.val);
						zend_hash_move_forward(lht);
					}
					attrp = new HW_API_Attribute(Z_STRVAL_PP(arg1), values);
					break;
				}
				default:
					convert_to_string_ex(arg2);
					attrp = new HW_API_Attribute(Z_STRVAL_PP(arg1), Z_STRVAL_PP(arg2));
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	rv = php_hwapi_object_new(attrp, le_hwapi_attributep);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);

	printf("hwapi_attribute_new\n");
}
/* }}} */

/* {{{ proto string hwapi_attribute_key()
   Returns key of an hwapi_attribute */
PHP_FUNCTION(hwapi_attribute_key) {
	zval *id;
	HW_API_Attribute *attrp;
	char *value;

	id = getThis();
	attrp = (HW_API_Attribute *) php_hwapi_get_object(id, le_hwapi_attributep);
	
	// Warning: It is import to duplicate the string before RETURN.
	// If that is not done the HW_API_String destructor will be called
	// before RETURN_STRING can duplicate the string.
	value = (char *) estrdup((attrp->key()).string());

	RETURN_STRING(value, 0);
}
/* }}} */

/* {{{ proto string hwapi_attribute_value()
   Returns value of hw_api_attribute */
PHP_FUNCTION(hwapi_attribute_value) {
	zval *id;
	HW_API_Attribute *attrp;
	char *value;

	id = getThis();
	attrp = (HW_API_Attribute *) php_hwapi_get_object(id, le_hwapi_attributep);

	// Warning: It is import to duplicate the string before RETURN.
	// If that is not done the HW_API_String destructor will be called
	// before RETURN_STRING can duplicate the string.
	value = (char *) estrdup(attrp->value().string());

	RETURN_STRING(value, 0);
}
/* }}} */

/* {{{ proto string hwapi_attribute_values()
   Returns all values of an attribute as an array */
PHP_FUNCTION(hwapi_attribute_values) {
	zval *id, **tmp;
	HW_API_Attribute *attrp;
	HW_API_StringArray values;

	id = getThis();
	attrp = (HW_API_Attribute *) php_hwapi_get_object(id, le_hwapi_attributep);

	values = attrp->values();
	if(0 == stringArray2indexArray(&return_value, &values))
		RETURN_FALSE;
}
/* }}} */

/* {{{ proto string hwapi_attribute_langdepvalue(string language)
   Returns value of attribute with givenn language */
PHP_FUNCTION(hwapi_attribute_langdepvalue) {
	zval **arg1, *id, **tmp;
	HW_API_Attribute *attrp;
	char *value;

	if((ZEND_NUM_ARGS() != 1) || (zend_get_parameters_ex(1, &arg1) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);

	id = getThis();
	attrp = (HW_API_Attribute *) php_hwapi_get_object(id, le_hwapi_attributep);

	value = (char *) attrp->langDepValue((*arg1)->value.str.val).string();
	printf("hwapi_attribute_langdepvalue\n");
	RETURN_STRING(value, 1);
}
/* }}} */

/* {{{ proto string hwapi_content_new([string name][, string value])
   Creates new HW_API_Content */
PHP_FUNCTION(hwapi_content_new) {
	zval *rv, **arg1, **arg2, **arg3;
	HW_API_Content *contp;
	int ret;

	switch(ZEND_NUM_ARGS()) {
		case 0:
			contp = new HW_API_Content();
			break;
		case 1:
			if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			switch((*arg1)->type) {
				case IS_OBJECT:
					break;
				default:
					convert_to_string_ex(arg1);
					contp = new HW_API_Content(Z_STRVAL_PP(arg1));
			}
			break;
		case 2: //First Parameter is long, second the file name
			if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(arg1);
			convert_to_string_ex(arg2);
			contp = new HW_API_Content(HW_API_Content::File, Z_STRVAL_PP(arg2));
			break;
		case 3: //First Parameter is long or string, second and third is string
			if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			switch((*arg1)->type) {
				case IS_LONG:
					convert_to_string_ex(arg2);
					convert_to_string_ex(arg3);
					contp = new HW_API_Content(HW_API_Content::File, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3));
					break;
				default:
					convert_to_string_ex(arg1);
					convert_to_string_ex(arg2);
					convert_to_string_ex(arg3);
					contp = new HW_API_Content(Z_STRVAL_PP(arg1), Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3));
			} 
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	rv = php_hwapi_object_new(contp, le_hwapi_contentp);
	SEPARATE_ZVAL(&rv);
	*return_value = *rv;
	FREE_ZVAL(rv);
}
/* }}} */

/* {{{ proto string hwapi_content_read(string buffer, int length)
   Reads length bytes from content */
PHP_FUNCTION(hwapi_content_read) {
	zval **arg1, **arg2, *id, **tmp;
	HW_API_Content *contentp;
	char *value;
	int len;

	if((ZEND_NUM_ARGS() != 2) || (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(arg2);

	id = getThis();
	contentp = (HW_API_Content *) php_hwapi_get_object(id, le_hwapi_contentp);

	zval_dtor(*arg1);
	Z_TYPE_PP(arg1) = IS_STRING;
	/* FIXME: Need to finish the new zval */
	value = (char *) emalloc(Z_LVAL_PP(arg2)+1);

	Z_STRVAL_PP(arg1) = value;
	len = contentp->read(value, Z_LVAL_PP(arg2));
	value[len] = '\0';
	Z_STRLEN_PP(arg1) = len;

	RETURN_LONG(len);
}
/* }}} */

/* {{{ proto string hwapi_content_mimetype()
   Returns MimeType of document */
PHP_FUNCTION(hwapi_content_mimetype) {
	zval *id;
	HW_API_Content *contentp;
	HW_API_String hwstr;

	id = getThis();
	contentp = (HW_API_Content *) php_hwapi_get_object(id, le_hwapi_contentp);

	hwstr = contentp->mimetype();

	RETURN_STRING((char *) hwstr.string(), 1);
}
/* }}} */

/* {{{ proto string hwapi_error_count()
   Counts number of reasons of an HW_API_Error */
PHP_FUNCTION(hwapi_error_count) {
	zval *id, **tmp;
	HW_API_Error *objp;
	int value;

	id = getThis();
	objp = (HW_API_Error *) php_hwapi_get_object(id, le_hwapi_errorp);

	if(!objp) {
		RETURN_FALSE;
	}
	value = objp->count();

	RETURN_LONG(value);
}
/* }}} */

/* {{{ proto string hwapi_error_reason(int index)
   Returns a reason of an HW_API_Error */
PHP_FUNCTION(hwapi_error_reason) {
	zval **arg1, *id, **tmp;
	HW_API_Error *objp;
	HW_API_Reason reason;
	bool error;

	if((ZEND_NUM_ARGS() != 1) || (zend_get_parameters_ex(1, &arg1) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(arg1);

	id = getThis();
	objp = (HW_API_Error *) php_hwapi_get_object(id, le_hwapi_errorp);

	if(!objp) {
		RETURN_FALSE;
	}
	if(!objp->error())
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "This is not an error");

	error = objp->reason(Z_LVAL_PP(arg1), reason);
	if(error) {
		zval *rv;
		HW_API_Reason *reasonp = new HW_API_Reason(reason);
		rv = php_hwapi_object_new(reasonp, le_hwapi_reasonp);
		SEPARATE_ZVAL(&rv);
		*return_value = *rv;
		FREE_ZVAL(rv);
		return;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto string hwapi_reason_type()
   Returns the type of HW_API_Reason */
PHP_FUNCTION(hwapi_reason_type) {
	zval **arg1, *id, **tmp;
	HW_API_Reason *objp;

	id = getThis();
	objp = (HW_API_Reason *) php_hwapi_get_object(id, le_hwapi_reasonp);

	if(!objp) {
		RETURN_FALSE;
	}
	RETURN_LONG((long) (objp->type()));
}
/* }}} */

/* {{{ proto string hwapi_reason_description(string language)
   Returns description of HW_API_Reason */
PHP_FUNCTION(hwapi_reason_description) {
	zval **arg1, *id, **tmp;
	HW_API_Reason *objp;
	HW_API_String desc;

	if((ZEND_NUM_ARGS() != 1) || (zend_get_parameters_ex(1, &arg1) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);

	id = getThis();
	objp = (HW_API_Reason *) php_hwapi_get_object(id, le_hwapi_reasonp);

	if(!objp) {
		RETURN_FALSE;
	}
	desc = objp->description((HW_API_String) (Z_STRVAL_PP(arg1)));
	RETURN_STRING((char *) desc.string(), 1);
}
/* }}} */

/* hw_api_class_get_property(zend_property_reference *property_reference) {{{
 *
 */
pval hw_api_class_get_property(zend_property_reference *property_reference) {
	pval result;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;


	printf("Reading a property from a HW_API object:\n");

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (Z_TYPE_P(overloaded_property)) {
			case OE_IS_ARRAY:
				printf("Array offset:  ");
				break;
			case OE_IS_OBJECT:
				printf("Object property:  ");
				break;
		}
		switch (Z_TYPE(overloaded_property->element)) {
			case IS_LONG:
				printf("%ld (numeric)\n", Z_LVAL(overloaded_property->element));
				break;
			case IS_STRING:
				printf("'%s'\n", Z_STRVAL(overloaded_property->element));
				break;
		}
		pval_destructor(&overloaded_property->element);
	}

	Z_STRVAL(result) = estrndup("testing", 7);
	Z_STRLEN(result) = 7;
	Z_TYPE(result) = IS_STRING;
	return result;
}
/* }}} */

/* hw_api_class_set_property(zend_property_reference *property_reference, pval *value) {{{
 */
int hw_api_class_set_property(zend_property_reference *property_reference, pval *value) {
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;

	printf("Writing to a property from a HW_API object:\n");
	printf("Writing '");
	zend_print_variable(value);
	printf("'\n");

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (Z_TYPE_P(overloaded_property)) {
			case OE_IS_ARRAY:
				printf("Array offset:  ");
				break;
			case OE_IS_OBJECT:
				printf("Object property:  ");
				break;
		}
		switch (Z_TYPE(overloaded_property->element)) {
			case IS_LONG:
				printf("%ld (numeric)\n", Z_LVAL(overloaded_property->element));
				break;
			case IS_STRING:
				printf("'%s'\n", Z_STRVAL(overloaded_property->element));
				break;
		}
		pval_destructor(&overloaded_property->element);
	}

	return 0;
}
/* }}} */

/* hw_api_class_startup() {{{
 */
void hw_api_class_startup() {
	zend_class_entry ce;

	INIT_OVERLOADED_CLASS_ENTRY(ce, "HW_API", php_hw_api_functions,
								NULL,
								NULL,
								NULL);

	hw_api_class_entry_ptr = zend_register_internal_class_ex(&ce, NULL, NULL);
}
/* }}} */

/* hw_api_object_class_get_property(zend_property_reference *property_reference) {{{
 */
pval hw_api_object_class_get_property(zend_property_reference *property_reference) {
	pval result;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;


	printf("Reading a property from a HW_API_Object object:\n");

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (Z_TYPE_P(overloaded_property)) {
			case OE_IS_ARRAY:
				printf("Array offset:  ");
				break;
			case OE_IS_OBJECT:
				printf("Object property:  ");
				break;
		}
		switch (Z_TYPE(overloaded_property->element)) {
			case IS_LONG:
				printf("%ld (numeric)\n", Z_LVAL(overloaded_property->element));
				break;
			case IS_STRING:
				printf("'%s'\n", Z_STRVAL(overloaded_property->element));
				break;
		}
		pval_destructor(&overloaded_property->element);
	}

	Z_STRVAL(result) = estrndup("testing", 7);
	Z_STRLEN(result) = 7;
	Z_TYPE(result) = IS_STRING;
	return result;
}
/* }}} */

/* hw_api_object_class_set_property(zend_property_reference *property_reference, pval *value) {{{
 */
int hw_api_object_class_set_property(zend_property_reference *property_reference, pval *value) {
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;

	printf("Writing to a property from a HW_API_Object object:\n");
	printf("Writing '");
	zend_print_variable(value);
	printf("'\n");

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (Z_TYPE_P(overloaded_property)) {
			case OE_IS_ARRAY:
				printf("Array offset:  ");
				break;
			case OE_IS_OBJECT:
				printf("Object property:  ");
				break;
		}
		switch (Z_TYPE(overloaded_property->element)) {
			case IS_LONG:
				printf("%ld (numeric)\n", Z_LVAL(overloaded_property->element));
				break;
			case IS_STRING:
				printf("'%s'\n", Z_STRVAL(overloaded_property->element));
				break;
		}
		pval_destructor(&overloaded_property->element);
	}

	return 0;
}
/* }}} */

/* {{{ hw_api_object_class_startup()
 */
void hw_api_object_class_startup() {
	zend_class_entry ce;

	INIT_OVERLOADED_CLASS_ENTRY(ce, "hw_api_object", php_hw_api_object_functions,
								NULL,
								NULL,
								NULL);

	hw_api_object_class_entry_ptr = zend_register_internal_class_ex(&ce, NULL, NULL);
}
/* }}} */

/* hw_api_attribute_class_get_property(zend_property_reference *property_reference) {{{
 */
pval hw_api_attribute_class_get_property(zend_property_reference *property_reference) {
	pval result;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;

	printf("Reading a property from a HW_API_Attribute object:\n");

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (Z_TYPE_P(overloaded_property)) {
			case OE_IS_ARRAY:
				printf("Array offset:  ");
				break;
			case OE_IS_OBJECT:
				printf("Object property:  ");
				break;
		}
		switch (Z_TYPE(overloaded_property->element)) {
			case IS_LONG:
				printf("%ld (numeric)\n", Z_LVAL(overloaded_property->element));
				break;
			case IS_STRING:
				printf("'%s'\n", Z_STRVAL(overloaded_property->element));
				break;
		}
		pval_destructor(&overloaded_property->element);
	}

	Z_STRVAL(result) = estrndup("testing", 7);
	Z_STRLEN(result) = 7;
	Z_TYPE(result) = IS_STRING;
	return result;
}
/* }}} */

/* hw_api_attribute_class_set_property(zend_property_reference *property_reference, pval *value) {{{
 */
int hw_api_attribute_class_set_property(zend_property_reference *property_reference, pval *value) {
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;

	printf("Writing to a property from a HW_API_Attribute object:\n");
	printf("Writing '");
	zend_print_variable(value);
	printf("'\n");

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		switch (Z_TYPE_P(overloaded_property)) {
			case OE_IS_ARRAY:
				printf("Array offset:  ");
				break;
			case OE_IS_OBJECT:
				printf("Object property:  ");
				break;
		}
		switch (Z_TYPE(overloaded_property->element)) {
			case IS_LONG:
				printf("%ld (numeric)\n", Z_LVAL(overloaded_property->element));
				break;
			case IS_STRING:
				printf("'%s'\n", Z_STRVAL(overloaded_property->element));
				break;
		}
		pval_destructor(&overloaded_property->element);
	}

	return 0;
}
/* }}} */

/* hw_api_attribute_class_startup() {{{
 */
void hw_api_attribute_class_startup() {
	zend_class_entry hw_api_attribute_class_entry;

	INIT_OVERLOADED_CLASS_ENTRY(hw_api_attribute_class_entry, "HW_API_Attribute", php_hw_api_attribute_functions,
								NULL,
								NULL,
								NULL);

	hw_api_attribute_class_entry_ptr = zend_register_internal_class(&hw_api_attribute_class_entry);
}
/* }}} */

/* hw_api_error_class_startup() {{{
 */
void hw_api_error_class_startup() {
	zend_class_entry hw_api_error_class_entry;

	INIT_OVERLOADED_CLASS_ENTRY(hw_api_error_class_entry, "HW_API_Error", php_hw_api_error_functions,
								NULL,
								NULL,
								NULL);

	hw_api_error_class_entry_ptr = zend_register_internal_class(&hw_api_error_class_entry);
}
/* }}} */

/* hw_api_content_class_startup() {{{
 */
void hw_api_content_class_startup() {
	zend_class_entry hw_api_content_class_entry;

	INIT_OVERLOADED_CLASS_ENTRY(hw_api_content_class_entry, "HW_API_Content", php_hw_api_content_functions,
								NULL,
								NULL,
								NULL);

	hw_api_content_class_entry_ptr = zend_register_internal_class(&hw_api_content_class_entry);
}
/* }}} */

/* hw_api_reason_class_startup() {{{
 */
void hw_api_reason_class_startup() {
	zend_class_entry hw_api_reason_class_entry;

	INIT_OVERLOADED_CLASS_ENTRY(hw_api_reason_class_entry, "HW_API_Reason", php_hw_api_reason_functions,
								NULL,
								NULL,
								NULL);

	hw_api_reason_class_entry_ptr = zend_register_internal_class(&hw_api_reason_class_entry);
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
