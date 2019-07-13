/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
 */

#include "transliterator_class.h"
#include "php_intl.h"
#include "transliterator_methods.h"
#include "intl_error.h"
#include "intl_convert.h"
#include "intl_data.h"

#include <unicode/utrans.h>

zend_class_entry *Transliterator_ce_ptr = NULL;

zend_object_handlers Transliterator_handlers;

/* {{{ int transliterator_object_construct( zval *object, UTransliterator *utrans, UErrorCode *status )
 * Initialize internals of Transliterator_object.
 */
int transliterator_object_construct( zval *object,
									 UTransliterator *utrans,
									 UErrorCode *status )
{
	const UChar           *ustr_id;
	int32_t               ustr_id_len;
	zend_string           *u8str;
	zval                  tmp;
	Transliterator_object *to;

	TRANSLITERATOR_METHOD_FETCH_OBJECT_NO_CHECK;

	assert( to->utrans == NULL );
	/* this assignment must happen before any return with failure because the
	 * caller relies on it always being made (so it can just destroy the object
	 * to close the transliterator) */
	to->utrans = utrans;

	ustr_id = utrans_getUnicodeID( utrans, &ustr_id_len );
	u8str = intl_convert_utf16_to_utf8(ustr_id, (int ) ustr_id_len, status );
	if( !u8str )
	{
		return FAILURE;
	}

	ZVAL_NEW_STR(&tmp, u8str);
	zend_update_property(Transliterator_ce_ptr, object,
		"id", sizeof( "id" ) - 1, &tmp );
	GC_DELREF(u8str);
	return SUCCESS;
}
/* }}} */

/*
 * Auxiliary functions needed by objects of 'Transliterator' class
 */

/* {{{ void transliterator_object_init( Transliterator_object* to )
 * Initialize internals of Transliterator_object.
 */
static void transliterator_object_init( Transliterator_object* to )
{
	if( !to )
		return;

	intl_error_init( TRANSLITERATOR_ERROR_P( to ) );
}
/* }}} */

/* {{{ void transliterator_object_destroy( Transliterator_object* to )
 * Clean up mem allocted by internals of Transliterator_object
 */
static void transliterator_object_destroy( Transliterator_object* to )
{
	if( !to )
		return;

	if( to->utrans )
	{
		utrans_close( to->utrans );
		to->utrans = NULL;
	}

	intl_error_reset( TRANSLITERATOR_ERROR_P( to ) );
}
/* }}} */

/* {{{ Transliterator_objects_free */
static void Transliterator_objects_free( zend_object *object )
{
	Transliterator_object* to = php_intl_transliterator_fetch_object(object);

	zend_object_std_dtor( &to->zo );

	transliterator_object_destroy( to );
}
/* }}} */

/* {{{ Transliterator_object_create */
static zend_object *Transliterator_object_create( zend_class_entry *ce )
{
	Transliterator_object* intern;

	intern = zend_object_alloc(sizeof(Transliterator_object), ce);

	zend_object_std_init( &intern->zo, ce );
    object_properties_init( &intern->zo, ce );
	transliterator_object_init( intern );

	intern->zo.handlers = &Transliterator_handlers;

	return &intern->zo;
}
/* }}} */

/*
 * Object handlers for Transliterator class (and subclasses)
 */

/* {{{ clone handler for Transliterator */
static zend_object *Transliterator_clone_obj( zval *object )
{
	Transliterator_object *to_orig,
	                      *to_new;
	zend_object 		  *ret_val;
	intl_error_reset( NULL );

	to_orig = Z_INTL_TRANSLITERATOR_P( object );
	intl_error_reset( INTL_DATA_ERROR_P( to_orig ) );
	ret_val = Transliterator_ce_ptr->create_object( Z_OBJCE_P( object ) );
	to_new  = php_intl_transliterator_fetch_object( ret_val );

	zend_objects_clone_members( &to_new->zo, &to_orig->zo );

	if( to_orig->utrans != NULL )
	{
		zval			tempz; /* dummy zval to pass to transliterator_object_construct */

		/* guaranteed to return NULL if it fails */
		UTransliterator *utrans = utrans_clone( to_orig->utrans, TRANSLITERATOR_ERROR_CODE_P( to_orig ) );

		if( U_FAILURE( TRANSLITERATOR_ERROR_CODE( to_orig ) ) )
			goto err;

		ZVAL_OBJ(&tempz, ret_val);
		transliterator_object_construct( &tempz, utrans,
			TRANSLITERATOR_ERROR_CODE_P( to_orig ) );

		if( U_FAILURE( TRANSLITERATOR_ERROR_CODE( to_orig ) ) )
		{
			zend_string *err_msg;
err:

			if( utrans != NULL )
				transliterator_object_destroy( to_new );

			/* set the error anyway, in case in the future we decide not to
			 * throw an error. It also helps build the error message */
			intl_error_set_code( NULL, INTL_DATA_ERROR_CODE( to_orig ) );
			intl_errors_set_custom_msg( TRANSLITERATOR_ERROR_P( to_orig ),
				"Could not clone transliterator", 0 );

			err_msg = intl_error_get_message( TRANSLITERATOR_ERROR_P( to_orig ) );
			zend_throw_error( NULL, "%s", ZSTR_VAL(err_msg) );
			zend_string_free( err_msg ); /* if it's changed into a warning */
			/* do not destroy tempz; we need to return something */
		}
	}
	else
	{
		/* We shouldn't have unconstructed objects in the first place */
		php_error_docref( NULL, E_WARNING,
			"Cloning unconstructed transliterator." );
	}

	return ret_val;
}
/* }}} */

#define TRANSLITERATOR_PROPERTY_HANDLER_PROLOG(return_fail) \
	zval tmp_member;							\
	if( Z_TYPE_P( member ) != IS_STRING )		\
	{											\
		zend_string *_str =						\
			zval_try_get_string_func(member);	\
		if (UNEXPECTED(!_str)) { return_fail; }	\
		ZVAL_STR(&tmp_member, _str);			\
		member = &tmp_member;					\
		cache_slot = NULL;						\
    }

#define TRANSLITERATOR_PROPERTY_HANDLER_EPILOG	\
	if( member == &tmp_member )					\
	{											\
		zval_ptr_dtor_str( &tmp_member );		\
	}

/* {{{ get_property_ptr_ptr handler */
static zval *Transliterator_get_property_ptr_ptr( zval *object, zval *member, int type, void **cache_slot )
{
	zval *retval;

	TRANSLITERATOR_PROPERTY_HANDLER_PROLOG(return NULL);

	if(zend_binary_strcmp( "id", sizeof( "id" ) - 1,
		Z_STRVAL_P( member ), Z_STRLEN_P( member ) ) == 0 )
	{
		retval = NULL; /* fallback to read_property */
	}
	else
	{
		retval = zend_std_get_property_ptr_ptr( object, member, type, cache_slot );
	}

	TRANSLITERATOR_PROPERTY_HANDLER_EPILOG;

	return retval;
}
/* }}} */

/* {{{ read_property handler */
static zval *Transliterator_read_property( zval *object, zval *member, int type, void **cache_slot, zval *rv )
{
	zval *retval;

	TRANSLITERATOR_PROPERTY_HANDLER_PROLOG(return &EG(uninitialized_zval));

	if( ( type != BP_VAR_R && type != BP_VAR_IS ) &&
		( zend_binary_strcmp( "id", sizeof( "id" ) - 1,
		Z_STRVAL_P( member ), Z_STRLEN_P( member ) ) == 0 ) )
	{
		php_error_docref(NULL, E_WARNING, "The property \"id\" is read-only" );
		retval = &EG( uninitialized_zval );
	}
	else
	{
		retval = zend_std_read_property( object, member, type, cache_slot, rv );
	}

	TRANSLITERATOR_PROPERTY_HANDLER_EPILOG;

	return retval;
}

/* }}} */

/* {{{ write_property handler */
static zval *Transliterator_write_property( zval *object, zval *member, zval *value, void **cache_slot )
{
	zend_class_entry *scope;
	TRANSLITERATOR_PROPERTY_HANDLER_PROLOG(return value);

	if (EG(fake_scope)) {
		scope = EG(fake_scope);
	} else {
		scope = zend_get_executed_scope();
	}
	if( ( scope != Transliterator_ce_ptr ) &&
		( zend_binary_strcmp( "id", sizeof( "id" ) - 1,
		Z_STRVAL_P( member ), Z_STRLEN_P( member ) ) == 0 ) )
	{
		php_error_docref(NULL, E_WARNING, "The property \"id\" is read-only" );
	}
	else
	{
		value = zend_std_write_property( object, member, value, cache_slot );
	}

	TRANSLITERATOR_PROPERTY_HANDLER_EPILOG;

	return value;
}
/* }}} */

/*
 * 'Transliterator' class registration structures & functions
 */

/* {{{ Transliterator methods arguments info */

ZEND_BEGIN_ARG_INFO_EX( ainfo_trans_void, 0, 0, 0 )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_trans_create, 0, 0, 1 )
	ZEND_ARG_INFO( 0, id )
	ZEND_ARG_INFO( 0, direction )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_trans_create_from_rules, 0, 0, 1 )
	ZEND_ARG_INFO( 0, rules )
	ZEND_ARG_INFO( 0, direction )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_trans_me_transliterate, 0, 0, 1 )
	ZEND_ARG_INFO( 0, subject )
	ZEND_ARG_INFO( 0, start )
	ZEND_ARG_INFO( 0, end )
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ Transliterator_class_functions
 * Every 'Transliterator' class method has an entry in this table
 */
static const zend_function_entry Transliterator_class_functions[] = {
	PHP_ME( Transliterator,			__construct,						ainfo_trans_void,				ZEND_ACC_PRIVATE | ZEND_ACC_FINAL )
	PHP_ME_MAPPING( create,			transliterator_create,				ainfo_trans_create,				ZEND_ACC_STATIC |ZEND_ACC_PUBLIC )
	PHP_ME_MAPPING( createFromRules,transliterator_create_from_rules,	ainfo_trans_create_from_rules,	ZEND_ACC_STATIC | ZEND_ACC_PUBLIC )
	PHP_ME_MAPPING( createInverse,	transliterator_create_inverse,		ainfo_trans_void,				ZEND_ACC_PUBLIC )
	PHP_ME_MAPPING( listIDs,		transliterator_list_ids,			ainfo_trans_void,				ZEND_ACC_STATIC | ZEND_ACC_PUBLIC )
	PHP_ME_MAPPING( transliterate,	transliterator_transliterate,		ainfo_trans_me_transliterate,	ZEND_ACC_PUBLIC )
	PHP_ME_MAPPING( getErrorCode,	transliterator_get_error_code,		ainfo_trans_void,				ZEND_ACC_PUBLIC )
	PHP_ME_MAPPING( getErrorMessage,transliterator_get_error_message,	ainfo_trans_void,				ZEND_ACC_PUBLIC )
	PHP_FE_END
};
/* }}} */

/* {{{ transliterator_register_Transliterator_class
 * Initialize 'Transliterator' class
 */
void transliterator_register_Transliterator_class( void )
{
	zend_class_entry ce;

	/* Create and register 'Transliterator' class. */
	INIT_CLASS_ENTRY( ce, "Transliterator", Transliterator_class_functions );
	ce.create_object = Transliterator_object_create;
	Transliterator_ce_ptr = zend_register_internal_class( &ce );
	memcpy( &Transliterator_handlers, &std_object_handlers,
		sizeof Transliterator_handlers );
	Transliterator_handlers.offset = XtOffsetOf(Transliterator_object, zo);
	Transliterator_handlers.free_obj = Transliterator_objects_free;
	Transliterator_handlers.clone_obj = Transliterator_clone_obj;
	Transliterator_handlers.get_property_ptr_ptr = Transliterator_get_property_ptr_ptr;
	Transliterator_handlers.read_property = Transliterator_read_property;
	Transliterator_handlers.write_property = Transliterator_write_property;

	/* Declare 'Transliterator' class properties */
	if( !Transliterator_ce_ptr )
	{
		zend_error( E_ERROR,
			"Transliterator: attempt to create properties "
			"on a non-registered class." );
		return;
	}
	zend_declare_property_null( Transliterator_ce_ptr,
		"id", sizeof( "id" ) - 1, ZEND_ACC_PUBLIC );

	/* constants are declared in transliterator_register_constants, called from MINIT */

}
/* }}} */
