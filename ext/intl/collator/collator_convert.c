/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Vadim Savchuk <vsavchuk@productengine.com>                  |
   |          Dmitry Lakhtyuk <dlakhtyuk@productengine.com>               |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_intl.h"
#include "collator_class.h"
#include "collator_is_numeric.h"
#include "collator_convert.h"
#include "intl_convert.h"

#include <unicode/ustring.h>
#include <php.h>

#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION <= 1)
#define CAST_OBJECT_SHOULD_FREE ,0
#else
#define CAST_OBJECT_SHOULD_FREE
#endif

#define COLLATOR_CONVERT_RETURN_FAILED(retval) { \
			zval_add_ref( &retval );             \
			return retval;                       \
	}

/* {{{ collator_convert_hash_item_from_utf8_to_utf16 */
static void collator_convert_hash_item_from_utf8_to_utf16(
	HashTable* hash, int hashKeyType, char* hashKey, ulong hashIndex,
	UErrorCode* status )
{
	const char* old_val;
	int         old_val_len;
	UChar*      new_val      = NULL;
	int         new_val_len  = 0;
	zval**      hashData     = NULL;
	zval*       znew_val     = NULL;

	/* Get current hash item. */
	zend_hash_get_current_data( hash, (void**) &hashData );

	/* Process string values only. */
	if( Z_TYPE_P( *hashData ) != IS_STRING )
		return;

	old_val     = Z_STRVAL_P( *hashData );
	old_val_len = Z_STRLEN_P( *hashData );

	/* Convert it from UTF-8 to UTF-16LE and save the result to new_val[_len]. */
	intl_convert_utf8_to_utf16( &new_val, &new_val_len, old_val, old_val_len, status );
	if( U_FAILURE( *status ) )
		return;

	/* Update current hash item with the converted value. */
	MAKE_STD_ZVAL( znew_val );
	ZVAL_STRINGL( znew_val, (char*)new_val, UBYTES(new_val_len), FALSE );

	if( hashKeyType == HASH_KEY_IS_STRING )
	{
		zend_hash_update( hash, hashKey, strlen( hashKey ) + 1,
			(void*) &znew_val, sizeof(zval*), NULL );
	}
	else /* hashKeyType == HASH_KEY_IS_LONG */
	{
		zend_hash_index_update( hash, hashIndex,
			(void*) &znew_val, sizeof(zval*), NULL );
	}
}
/* }}} */

/* {{{ collator_convert_hash_item_from_utf16_to_utf8 */
static void collator_convert_hash_item_from_utf16_to_utf8(
	HashTable* hash, int hashKeyType, char* hashKey, ulong hashIndex,
	UErrorCode* status )
{
	const char* old_val;
	int         old_val_len;
	char*       new_val      = NULL;
	int         new_val_len  = 0;
	zval**      hashData     = NULL;
	zval*       znew_val     = NULL;

	/* Get current hash item. */
	zend_hash_get_current_data( hash, (void**) &hashData );

	/* Process string values only. */
	if( Z_TYPE_P( *hashData ) != IS_STRING )
		return;

	old_val     = Z_STRVAL_P( *hashData );
	old_val_len = Z_STRLEN_P( *hashData );

	/* Convert it from UTF-16LE to UTF-8 and save the result to new_val[_len]. */
	intl_convert_utf16_to_utf8( &new_val, &new_val_len,
		(UChar*)old_val, UCHARS(old_val_len), status );
	if( U_FAILURE( *status ) )
		return;

	/* Update current hash item with the converted value. */
	MAKE_STD_ZVAL( znew_val );
	ZVAL_STRINGL( znew_val, (char*)new_val, new_val_len, FALSE );

	if( hashKeyType == HASH_KEY_IS_STRING )
	{
		zend_hash_update( hash, hashKey, strlen( hashKey ) + 1,
			(void*) &znew_val, sizeof(zval*), NULL );
	}
	else /* hashKeyType == HASH_KEY_IS_LONG */
	{
		zend_hash_index_update( hash, hashIndex,
			(void*) &znew_val, sizeof(zval*), NULL );
	}
}
/* }}} */

/* {{{ collator_convert_hash_from_utf8_to_utf16
 *  Convert values of the given hash from UTF-8 encoding to UTF-16LE.
 */
void collator_convert_hash_from_utf8_to_utf16( HashTable* hash, UErrorCode* status )
{
	ulong    hashIndex    = 0;
	char*    hashKey      = NULL;
	int      hashKeyType  = 0;

	zend_hash_internal_pointer_reset( hash );
	while( ( hashKeyType = zend_hash_get_current_key( hash, &hashKey, &hashIndex, 0 ) )
			!= HASH_KEY_NON_EXISTANT )
	{
		/* Convert current hash item from UTF-8 to UTF-16LE. */
		collator_convert_hash_item_from_utf8_to_utf16(
			hash, hashKeyType, hashKey, hashIndex, status );
		if( U_FAILURE( *status ) )
			return;

		/* Proceed to the next item. */
		zend_hash_move_forward( hash );
	}
}
/* }}} */

/* {{{ collator_convert_hash_from_utf16_to_utf8
 * Convert values of the given hash from UTF-16LE encoding to UTF-8.
 */
void collator_convert_hash_from_utf16_to_utf8( HashTable* hash, UErrorCode* status )
{
	ulong    hashIndex    = 0;
	char*    hashKey      = NULL;
	int      hashKeyType  = 0;

	zend_hash_internal_pointer_reset( hash );
	while( ( hashKeyType = zend_hash_get_current_key( hash, &hashKey, &hashIndex, 0 ) )
			!= HASH_KEY_NON_EXISTANT )
	{
		/* Convert current hash item from UTF-16LE to UTF-8. */
		collator_convert_hash_item_from_utf16_to_utf8(
			hash, hashKeyType, hashKey, hashIndex, status );
		if( U_FAILURE( *status ) ) {
			return;
		}

		/* Proceed to the next item. */
		zend_hash_move_forward( hash );
	}
}
/* }}} */

/* {{{ collator_convert_zstr_utf16_to_utf8
 *
 * Convert string from utf16 to utf8.
 *
 * @param  zval* utf16_zval String to convert.
 *
 * @return zval* Converted string.
 */
zval* collator_convert_zstr_utf16_to_utf8( zval* utf16_zval )
{
	zval* utf8_zval   = NULL;
	char* str         = NULL;
	int   str_len     = 0;
	UErrorCode status = U_ZERO_ERROR;

	/* Convert to utf8 then. */
	intl_convert_utf16_to_utf8( &str, &str_len,
		(UChar*) Z_STRVAL_P(utf16_zval), UCHARS( Z_STRLEN_P(utf16_zval) ), &status );
	if( U_FAILURE( status ) )
		php_error( E_WARNING, "Error converting utf16 to utf8 in collator_convert_zval_utf16_to_utf8()" );

	ALLOC_INIT_ZVAL( utf8_zval );
	ZVAL_STRINGL( utf8_zval, str, str_len, FALSE );

	return utf8_zval;
}
/* }}} */

/* {{{ collator_convert_zstr_utf8_to_utf16
 *
 * Convert string from utf8 to utf16.
 *
 * @param  zval* utf8_zval String to convert.
 *
 * @return zval* Converted string.
 */
zval* collator_convert_zstr_utf8_to_utf16( zval* utf8_zval )
{
	zval* zstr        = NULL;
	UChar* ustr       = NULL;
	int    ustr_len   = 0;
	UErrorCode status = U_ZERO_ERROR;

	/* Convert the string to UTF-16. */
	intl_convert_utf8_to_utf16(
			&ustr, &ustr_len,
			Z_STRVAL_P( utf8_zval ), Z_STRLEN_P( utf8_zval ),
			&status );
	if( U_FAILURE( status ) )
		php_error( E_WARNING, "Error casting object to string in collator_convert_zstr_utf8_to_utf16()" );

	/* Set string. */
	ALLOC_INIT_ZVAL( zstr );
	ZVAL_STRINGL( zstr, (char*)ustr, UBYTES(ustr_len), FALSE );

	return zstr;
}
/* }}} */

/* {{{ collator_convert_object_to_string
 * Convert object to UTF16-encoded string.
 */
zval* collator_convert_object_to_string( zval* obj TSRMLS_DC )
{
	zval* zstr        = NULL;
	UErrorCode status = U_ZERO_ERROR;
	UChar* ustr       = NULL;
	int    ustr_len   = 0;

	/* Bail out if it's not an object. */
	if( Z_TYPE_P( obj ) != IS_OBJECT )
	{
		COLLATOR_CONVERT_RETURN_FAILED( obj );
	}

	/* Try object's handlers. */
	if( Z_OBJ_HT_P(obj)->get )
	{
		zstr = Z_OBJ_HT_P(obj)->get( obj TSRMLS_CC );

		switch( Z_TYPE_P( zstr ) )
		{
			case IS_OBJECT:
				{
					/* Bail out. */
					zval_ptr_dtor( &zstr );
					COLLATOR_CONVERT_RETURN_FAILED( obj );
				} break;

			case IS_STRING:
				break;

			default:
				{
					convert_to_string( zstr );
				} break;
		}
	}
	else if( Z_OBJ_HT_P(obj)->cast_object )
	{
		ALLOC_INIT_ZVAL( zstr );

		if( Z_OBJ_HT_P(obj)->cast_object( obj, zstr, IS_STRING CAST_OBJECT_SHOULD_FREE TSRMLS_CC ) == FAILURE )
		{
			/* cast_object failed => bail out. */
			zval_ptr_dtor( &zstr );
			COLLATOR_CONVERT_RETURN_FAILED( obj );
		}
	}

	/* Object wasn't successfuly converted => bail out. */
	if( zstr == NULL )
	{
		COLLATOR_CONVERT_RETURN_FAILED( obj );
	}

	/* Convert the string to UTF-16. */
	intl_convert_utf8_to_utf16(
			&ustr, &ustr_len,
			Z_STRVAL_P( zstr ), Z_STRLEN_P( zstr ),
			&status );
	if( U_FAILURE( status ) )
		php_error( E_WARNING, "Error casting object to string in collator_convert_object_to_string()" );

	/* Cleanup zstr to hold utf16 string. */
	zval_dtor( zstr );

	/* Set string. */
	ZVAL_STRINGL( zstr, (char*)ustr, UBYTES(ustr_len), FALSE );

	/* Don't free ustr cause it's set in zstr without copy.
	 * efree( ustr );
	 */

	return zstr;
}
/* }}} */

/* {{{ collator_convert_string_to_number
 *
 * Convert string to number.
 *
 * @param  zval* str String to convert.
 *
 * @return zval* Number. If str is not numeric string return number zero.
 */
zval* collator_convert_string_to_number( zval* str )
{
	zval* num = collator_convert_string_to_number_if_possible( str );
	if( num == str )
	{
		/* String wasn't converted => return zero. */
		zval_ptr_dtor( &num );

		ALLOC_INIT_ZVAL( num );
		ZVAL_LONG( num, 0 );
	}

	return num;
}
/* }}} */

/* {{{ collator_convert_string_to_double
 *
 * Convert string to double.
 *
 * @param  zval* str String to convert.
 *
 * @return zval* Number. If str is not numeric string return number zero.
 */
zval* collator_convert_string_to_double( zval* str )
{
	zval* num = collator_convert_string_to_number( str );
	if( Z_TYPE_P(num) == IS_LONG )
	{
		ZVAL_DOUBLE( num, Z_LVAL_P( num ) );
	}

	return num;
}
/* }}} */

/* {{{ collator_convert_string_to_number_if_possible
 *
 * Convert string to numer.
 *
 * @param  zval* str String to convert.
 *
 * @return zval* Number if str is numeric string. Otherwise
 *               original str param.
 */
zval* collator_convert_string_to_number_if_possible( zval* str )
{
	zval* num      = NULL;
	int is_numeric = 0;
	long lval      = 0;
	double dval    = 0;

	if( Z_TYPE_P( str ) != IS_STRING )
	{
		COLLATOR_CONVERT_RETURN_FAILED( str );
	}

	if( ( is_numeric = collator_is_numeric( (UChar*) Z_STRVAL_P(str), UCHARS( Z_STRLEN_P(str) ), &lval, &dval, 1 ) ) )
	{
		ALLOC_INIT_ZVAL( num );

		if( is_numeric == IS_LONG )
			Z_LVAL_P(num) = lval;
		if( is_numeric == IS_DOUBLE )
			Z_DVAL_P(num) = dval;

		Z_TYPE_P(num) = is_numeric;
	}
	else
	{
		COLLATOR_CONVERT_RETURN_FAILED( str );
	}

	return num;
}
/* }}} */

/* {{{ collator_make_printable_zval
 *
 * Returns string from input zval.
 *
 * @param  zval* arg zval to get string from
 *
 * @return zval* UTF16 string.
 */
zval* collator_make_printable_zval( zval* arg )
{
	zval arg_copy;
	int use_copy = 0;
	zval* str    = NULL;

	if( Z_TYPE_P(arg) != IS_STRING )
	{
		zend_make_printable_zval(arg, &arg_copy, &use_copy);

		if( use_copy )
		{
			str = collator_convert_zstr_utf8_to_utf16( &arg_copy );
			zval_dtor( &arg_copy );
		}
		else
		{
			str = collator_convert_zstr_utf8_to_utf16( arg );
		}
	}
	else
	{
		COLLATOR_CONVERT_RETURN_FAILED( arg );
	}

	return str;
}
/* }}} */

/* {{{ collator_normalize_sort_argument
 *
 * Normalize argument to use in sort's compare function.
 *
 * @param  zval* arg Sort's argument to normalize.
 *
 * @return zval* Normalized copy of arg or unmodified arg
 *               if normalization is not needed.
 */
zval* collator_normalize_sort_argument( zval* arg )
{
	zval* n_arg = NULL;

	if( Z_TYPE_P( arg ) != IS_STRING )
	{
		/* If its not a string then nothing to do.
		 * Return original arg.
		 */
		COLLATOR_CONVERT_RETURN_FAILED( arg );
	}

	/* Try convert to number. */
	n_arg = collator_convert_string_to_number_if_possible( arg );

	if( n_arg == arg )
	{
		/* Conversion to number failed. */
		zval_ptr_dtor( &n_arg );

		/* Convert string to utf8. */
		n_arg = collator_convert_zstr_utf16_to_utf8( arg );
	}

	return n_arg;
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
