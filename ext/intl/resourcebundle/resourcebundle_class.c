/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Hans-Peter Oeri (University of St.Gallen) <hp@oeri.ch>      |
   +----------------------------------------------------------------------+
 */

#include <stdlib.h>
#include <unicode/ures.h>
#include <unicode/uenum.h>

#include <zend.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>
#include <php.h>

#include "php_intl.h"
#include "intl_data.h"
#include "intl_common.h"

#include "resourcebundle/resourcebundle.h"
#include "resourcebundle/resourcebundle_iterator.h"
#include "resourcebundle/resourcebundle_class.h"
#include "resourcebundle/resourcebundle_arginfo.h"

zend_class_entry *ResourceBundle_ce_ptr = NULL;

static zend_object_handlers ResourceBundle_object_handlers;

/* {{{ ResourceBundle_object_free */
static void ResourceBundle_object_free( zend_object *object )
{
	ResourceBundle_object *rb = php_intl_resourcebundle_fetch_object(object);

	// only free local errors
	intl_error_reset( INTL_DATA_ERROR_P(rb) );

	if (rb->me) {
		ures_close( rb->me );
	}
	if (rb->child) {
		ures_close( rb->child );
	}

	zend_object_std_dtor( &rb->zend );
}
/* }}} */

/* {{{ ResourceBundle_object_create */
static zend_object *ResourceBundle_object_create( zend_class_entry *ce )
{
	ResourceBundle_object *rb;

	rb = zend_object_alloc(sizeof(ResourceBundle_object), ce);

	zend_object_std_init( &rb->zend, ce );
	object_properties_init( &rb->zend, ce);

	intl_error_init( INTL_DATA_ERROR_P(rb) );
	rb->me = NULL;
	rb->child = NULL;

	return &rb->zend;
}
/* }}} */

/* {{{ ResourceBundle_ctor */
static int resourcebundle_ctor(INTERNAL_FUNCTION_PARAMETERS, zend_error_handling *error_handling, bool *error_handling_replaced)
{
	const char *bundlename;
	size_t		bundlename_len = 0;
	const char *locale;
	size_t		locale_len = 0;
	bool	fallback = 1;

	zval                  *object = return_value;
	ResourceBundle_object *rb = Z_INTL_RESOURCEBUNDLE_P( object );

	intl_error_reset( NULL );

	if( zend_parse_parameters( ZEND_NUM_ARGS(), "s!s!|b",
		&locale, &locale_len, &bundlename, &bundlename_len, &fallback ) == FAILURE )
	{
		return FAILURE;
	}

	if (error_handling != NULL) {
		zend_replace_error_handling(EH_THROW, IntlException_ce_ptr, error_handling);
		*error_handling_replaced = 1;
	}

	if (rb->me) {
		zend_throw_error(NULL, "ResourceBundle object is already constructed");
		return FAILURE;
	}

	INTL_CHECK_LOCALE_LEN_OR_FAILURE(locale_len);

	if (locale == NULL) {
		locale = intl_locale_get_default();
	}

	if (bundlename_len >= MAXPATHLEN) {
		zend_argument_value_error(2, "is too long");
		return FAILURE;
	}

	if (fallback) {
		rb->me = ures_open(bundlename, locale, &INTL_DATA_ERROR_CODE(rb));
	} else {
		rb->me = ures_openDirect(bundlename, locale, &INTL_DATA_ERROR_CODE(rb));
	}

	INTL_CTOR_CHECK_STATUS(rb, "resourcebundle_ctor: Cannot load libICU resource bundle");

	if (!fallback && (INTL_DATA_ERROR_CODE(rb) == U_USING_FALLBACK_WARNING ||
			INTL_DATA_ERROR_CODE(rb) == U_USING_DEFAULT_WARNING)) {
		char *pbuf;
		intl_errors_set_code(NULL, INTL_DATA_ERROR_CODE(rb));
		spprintf(&pbuf, 0, "resourcebundle_ctor: Cannot load libICU resource "
				"'%s' without fallback from %s to %s",
				bundlename ? bundlename : "(default data)", locale,
				ures_getLocaleByType(
					rb->me, ULOC_ACTUAL_LOCALE, &INTL_DATA_ERROR_CODE(rb)));
		intl_errors_set_custom_msg(INTL_DATA_ERROR_P(rb), pbuf, 1);
		efree(pbuf);
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

/* {{{ ResourceBundle object constructor */
PHP_METHOD( ResourceBundle, __construct )
{
	zend_error_handling error_handling;
	bool error_handling_replaced = 0;

	return_value = ZEND_THIS;
	if (resourcebundle_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU, &error_handling, &error_handling_replaced) == FAILURE) {
		if (!EG(exception)) {
			zend_throw_exception(IntlException_ce_ptr, "Constructor failed", 0);
		}
	}
	if (error_handling_replaced) {
		zend_restore_error_handling(&error_handling);
	}
}
/* }}} */

/* {{{ */
PHP_FUNCTION( resourcebundle_create )
{
	object_init_ex( return_value, ResourceBundle_ce_ptr );
	if (resourcebundle_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU, NULL, NULL) == FAILURE) {
		zval_ptr_dtor(return_value);
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ resourcebundle_array_fetch */
static zval *resource_bundle_array_fetch(
	zend_object *object, zend_string *offset_str, zend_long offset_int,
	zval *return_value, bool fallback, uint32_t offset_arg_num)
{
	int32_t index = 0;
	char *key = NULL;
	bool is_numeric = offset_str == NULL;
	char *pbuf;
	ResourceBundle_object *rb;

	rb = php_intl_resourcebundle_fetch_object(object);
	intl_error_reset(NULL);
	intl_error_reset(INTL_DATA_ERROR_P(rb));

	if (offset_str) {
		// TODO Check is not empty?
		key = ZSTR_VAL(offset_str);
		rb->child = ures_getByKey(rb->me, key, rb->child, &INTL_DATA_ERROR_CODE(rb) );
	} else {
		if (UNEXPECTED(offset_int < (zend_long)INT32_MIN || offset_int > (zend_long)INT32_MAX)) {
			if (offset_arg_num) {
				zend_argument_value_error(offset_arg_num, "index must be between %d and %d", INT32_MIN, INT32_MAX);
			} else {
				zend_value_error("Index must be between %d and %d", INT32_MIN, INT32_MAX);
			}
			return NULL;
		}
		index = (int32_t)offset_int;
		rb->child = ures_getByIndex(rb->me, index, rb->child, &INTL_DATA_ERROR_CODE(rb));
	}

	intl_error_set_code( NULL, INTL_DATA_ERROR_CODE(rb) );
	if (U_FAILURE(INTL_DATA_ERROR_CODE(rb))) {
		if (is_numeric) {
			spprintf( &pbuf, 0, "Cannot load resource element %d", index );
		} else {
			spprintf( &pbuf, 0, "Cannot load resource element '%s'", key );
		}
		intl_errors_set_custom_msg( INTL_DATA_ERROR_P(rb), pbuf, 1 );
		efree(pbuf);
		RETVAL_NULL();
		return return_value;
	}

	if (!fallback && (INTL_DATA_ERROR_CODE(rb) == U_USING_FALLBACK_WARNING || INTL_DATA_ERROR_CODE(rb) == U_USING_DEFAULT_WARNING)) {
		UErrorCode icuerror;
		const char * locale = ures_getLocaleByType( rb->me, ULOC_ACTUAL_LOCALE, &icuerror );
		if (is_numeric) {
			spprintf(&pbuf, 0, "Cannot load element %d without fallback from to %s", index, locale);
		} else {
			spprintf(&pbuf, 0, "Cannot load element '%s' without fallback from to %s", key, locale);
		}
		intl_errors_set_custom_msg( INTL_DATA_ERROR_P(rb), pbuf, 1);
		efree(pbuf);
		RETVAL_NULL();
		return return_value;
	}

	resourcebundle_extract_value( return_value, rb );
	return return_value;
}
/* }}} */

/* {{{ resourcebundle_array_get */
zval *resourcebundle_array_get(zend_object *object, zval *offset, int type, zval *rv)
{
	if (offset == NULL) {
		zend_throw_error(NULL, "Cannot apply [] to ResourceBundle object");
		return NULL;
	}

	if (Z_TYPE_P(offset) == IS_LONG) {
		return resource_bundle_array_fetch(object, /* offset_str */ NULL, Z_LVAL_P(offset), rv, /* fallback */ true, /* arg_num */ 0);
	} else if (Z_TYPE_P(offset) == IS_STRING) {
		return resource_bundle_array_fetch(object, Z_STR_P(offset), /* offset_int */ 0, rv, /* fallback */ true, /* arg_num */ 0);
	} else {
		zend_illegal_container_offset(object->ce->name, offset, type);
		return NULL;
	}
}
/* }}} */

/* {{{ Get resource identified by numerical index or key name. */
PHP_FUNCTION( resourcebundle_get )
{
	bool fallback = true;
	zend_object *resource_bundle = NULL;
	zend_string *offset_str = NULL;
	zend_long offset_long = 0;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_OBJ_OF_CLASS(resource_bundle, ResourceBundle_ce_ptr)
		Z_PARAM_STR_OR_LONG(offset_str, offset_long)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(fallback)
	ZEND_PARSE_PARAMETERS_END();

	zval *retval = resource_bundle_array_fetch(resource_bundle, offset_str, offset_long, return_value, fallback, /* arg_num */ 2);
	if (!retval) {
		RETURN_THROWS();
	}
}
/* }}} */

PHP_METHOD(ResourceBundle , get)
{
	bool fallback = true;
	zend_string *offset_str = NULL;
	zend_long offset_long = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR_OR_LONG(offset_str, offset_long)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(fallback)
	ZEND_PARSE_PARAMETERS_END();

	zval *retval = resource_bundle_array_fetch(Z_OBJ_P(ZEND_THIS), offset_str, offset_long, return_value, fallback, /* arg_num */ 1);
	if (!retval) {
		RETURN_THROWS();
	}
}
/* }}} */

/* {{{ resourcebundle_array_count */
static zend_result resourcebundle_array_count(zend_object *object, zend_long *count)
{
	ResourceBundle_object *rb = php_intl_resourcebundle_fetch_object(object);

	if (rb->me == NULL) {
		intl_errors_set(&rb->error, U_ILLEGAL_ARGUMENT_ERROR,
				"Found unconstructed ResourceBundle", 0);
		return 0;
	}

	*count = ures_getSize( rb->me );

	return SUCCESS;
}
/* }}} */

/* {{{ Get resources count */
PHP_FUNCTION( resourcebundle_count )
{
	int32_t                len;
	RESOURCEBUNDLE_METHOD_INIT_VARS;

	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O", &object, ResourceBundle_ce_ptr ) == FAILURE ) {
		RETURN_THROWS();
	}

	RESOURCEBUNDLE_METHOD_FETCH_OBJECT;

	len = ures_getSize( rb->me );
	RETURN_LONG( len );
}

/* {{{ Get available locales from ResourceBundle name */
PHP_FUNCTION( resourcebundle_locales )
{
	char * bundlename;
	size_t    bundlename_len = 0;
	const char * entry;
	int entry_len;
	UEnumeration *icuenum;
	UErrorCode   icuerror = U_ZERO_ERROR;

	intl_errors_reset( NULL );

	if( zend_parse_parameters(ZEND_NUM_ARGS(), "s", &bundlename, &bundlename_len ) == FAILURE )
	{
		RETURN_THROWS();
	}

	if (bundlename_len >= MAXPATHLEN) {
		zend_argument_value_error(1, "is too long");
		RETURN_THROWS();
	}

	if(bundlename_len == 0) {
		// fetch default locales list
		bundlename = NULL;
	}

	icuenum = ures_openAvailableLocales( bundlename, &icuerror );
	INTL_CHECK_STATUS(icuerror, "Cannot fetch locales list");

	uenum_reset( icuenum, &icuerror );
	INTL_CHECK_STATUS(icuerror, "Cannot iterate locales list");

	array_init( return_value );
	while ((entry = uenum_next( icuenum, &entry_len, &icuerror ))) {
		add_next_index_stringl( return_value, (char *) entry, entry_len);
	}
	uenum_close( icuenum );
}
/* }}} */

/* {{{ Get text description for ResourceBundle's last error code. */
PHP_FUNCTION( resourcebundle_get_error_code )
{
	RESOURCEBUNDLE_METHOD_INIT_VARS;

	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, ResourceBundle_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	rb = Z_INTL_RESOURCEBUNDLE_P( object );

	RETURN_LONG(INTL_DATA_ERROR_CODE(rb));
}
/* }}} */

/* {{{ Get text description for ResourceBundle's last error. */
PHP_FUNCTION( resourcebundle_get_error_message )
{
	zend_string* message = NULL;
	RESOURCEBUNDLE_METHOD_INIT_VARS;

	if( zend_parse_method_parameters( ZEND_NUM_ARGS(), getThis(), "O",
		&object, ResourceBundle_ce_ptr ) == FAILURE )
	{
		RETURN_THROWS();
	}

	rb = Z_INTL_RESOURCEBUNDLE_P( object );
	message = intl_error_get_message(INTL_DATA_ERROR_P(rb));
	RETURN_STR(message);
}
/* }}} */

PHP_METHOD(ResourceBundle, getIterator) {
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

/* {{{ resourcebundle_register_class
 * Initialize 'ResourceBundle' class
 */
void resourcebundle_register_class( void )
{
	ResourceBundle_ce_ptr = register_class_ResourceBundle(zend_ce_aggregate, zend_ce_countable);
	ResourceBundle_ce_ptr->create_object = ResourceBundle_object_create;
	ResourceBundle_ce_ptr->default_object_handlers = &ResourceBundle_object_handlers;
	ResourceBundle_ce_ptr->get_iterator = resourcebundle_get_iterator;

	ResourceBundle_object_handlers = std_object_handlers;
	ResourceBundle_object_handlers.offset = XtOffsetOf(ResourceBundle_object, zend);
	ResourceBundle_object_handlers.clone_obj	  = NULL; /* ICU ResourceBundle has no clone implementation */
	ResourceBundle_object_handlers.free_obj = ResourceBundle_object_free;
	ResourceBundle_object_handlers.read_dimension = resourcebundle_array_get;
	ResourceBundle_object_handlers.count_elements = resourcebundle_array_count;
}
/* }}} */
