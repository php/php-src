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
	char                  *str_id;
	size_t                str_id_len;
	Transliterator_object *to;

	TRANSLITERATOR_METHOD_FETCH_OBJECT_NO_CHECK;

	assert( to->utrans == NULL );
	/* this assignment must happen before any return with failure because the
	 * caller relies on it always being made (so it can just destroy the object
	 * to close the transliterator) */
	to->utrans = utrans;

	ustr_id = utrans_getUnicodeID( utrans, &ustr_id_len );
	intl_convert_utf16_to_utf8( &str_id, &str_id_len, ustr_id, (int ) ustr_id_len, status );
	if( U_FAILURE( *status ) )
	{
		return FAILURE;
	}

	zend_update_property_stringl(Transliterator_ce_ptr, object,
		"id", sizeof( "id" ) - 1, str_id, str_id_len );
	efree( str_id );
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

/* {{{ Transliterator_objects_dtor */
static void Transliterator_objects_dtor(
	zend_object *object )
{
	zend_objects_destroy_object( object );
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
static zend_object *Transliterator_object_create(
	zend_class_entry *ce )
{
	Transliterator_object* intern;

	intern = ecalloc( 1, sizeof( Transliterator_object ) + sizeof(zval) * (ce->default_properties_count - 1));

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
		UTransliterator *utrans = NULL;
		zval			tempz; /* dummy zval to pass to transliterator_object_construct */

		/* guaranteed to return NULL if it fails */
		utrans = utrans_clone( to_orig->utrans, TRANSLITERATOR_ERROR_CODE_P( to_orig ) );

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
			php_error_docref( NULL, E_ERROR, "%s", err_msg->val );
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

#define TRANSLITERATOR_PROPERTY_HANDLER_PROLOG  \
	zval tmp_member;							\
	if( Z_TYPE_P( member ) != IS_STRING )		\
	{											\
		tmp_member = *member;					\
		zval_copy_ctor( &tmp_member );			\
		convert_to_string( &tmp_member );		\
		member = &tmp_member;					\
		cache_slot = NULL;						\
    }

#define TRANSLITERATOR_PROPERTY_HANDLER_EPILOG	\
	if( member == &tmp_member )					\
	{											\
		zval_dtor( &tmp_member );				\
	}

/* {{{ get_property_ptr_ptr handler */
static zval *Transliterator_get_property_ptr_ptr( zval *object, zval *member, int type, void **cache_slot )
{
	zval *retval;

	TRANSLITERATOR_PROPERTY_HANDLER_PROLOG;

	if(zend_binary_strcmp( "id", sizeof( "id" ) - 1,
		Z_STRVAL_P( member ), Z_STRLEN_P( member ) ) == 0 )
	{
		retval = NULL; /* fallback to read_property */
	}
	else
	{
		retval = std_object_handlers.get_property_ptr_ptr( object, member, type, cache_slot );
	}

	TRANSLITERATOR_PROPERTY_HANDLER_EPILOG;

	return retval;
}
/* }}} */

/* {{{ read_property handler */
static zval *Transliterator_read_property( zval *object, zval *member, int type, void **cache_slot, zval *rv )
{
	zval *retval;

	TRANSLITERATOR_PROPERTY_HANDLER_PROLOG;

	if( ( type != BP_VAR_R && type != BP_VAR_IS ) &&
		( zend_binary_strcmp( "id", sizeof( "id" ) - 1,
		Z_STRVAL_P( member ), Z_STRLEN_P( member ) ) == 0 ) )
	{
		php_error_docref0( NULL, E_WARNING, "The property \"id\" is read-only" );
		retval = &EG( uninitialized_zval );
	}
	else
	{
		retval = std_object_handlers.read_property( object, member, type, cache_slot, rv );
	}

	TRANSLITERATOR_PROPERTY_HANDLER_EPILOG;

	return retval;
}

/* }}} */

/* {{{ write_property handler */
static void Transliterator_write_property( zval *object, zval *member, zval *value,
	void **cache_slot )
{
	TRANSLITERATOR_PROPERTY_HANDLER_PROLOG;

	if( ( EG( scope ) != Transliterator_ce_ptr ) &&
		( zend_binary_strcmp( "id", sizeof( "id" ) - 1,
		Z_STRVAL_P( member ), Z_STRLEN_P( member ) ) == 0 ) )
	{
		php_error_docref0( NULL, E_WARNING, "The property \"id\" is read-only" );
	}
	else
	{
		std_object_handlers.write_property( object, member, value, cache_slot );
	}

	TRANSLITERATOR_PROPERTY_HANDLER_EPILOG;
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

ZEND_BEGIN_ARG_INFO_EX( ainfo_trans_create_inverse, 0, 0, 1 )
	ZEND_ARG_OBJ_INFO( 0, orig_trans, Transliterator, 0 )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_trans_me_transliterate, 0, 0, 1 )
	ZEND_ARG_INFO( 0, subject )
	ZEND_ARG_INFO( 0, start )
	ZEND_ARG_INFO( 0, end )
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX( ainfo_trans_error, 0, 0, 1 )
	ZEND_ARG_OBJ_INFO( 0, trans, Transliterator, 0 )
ZEND_END_ARG_INFO()

/* }}} */

/* {{{ Transliterator_class_functions
 * Every 'Transliterator' class method has an entry in this table
 */
zend_function_entry Transliterator_class_functions[] = {
	PHP_ME( Transliterator,			__construct,						ainfo_trans_void,				ZEND_ACC_PRIVATE | ZEND_ACC_CTOR | ZEND_ACC_FINAL )
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
	memcpy( &Transliterator_handlers, zend_get_std_object_handlers(),
		sizeof Transliterator_handlers );
	Transliterator_handlers.offset = XtOffsetOf(Transliterator_object, zo);
	Transliterator_handlers.dtor_obj = Transliterator_objects_dtor;
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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
