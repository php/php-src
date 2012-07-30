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
#include "collator.h"
#include "collator_class.h"
#include "collator_sort.h"
#include "collator_convert.h"
#include "intl_convert.h"

#if !defined(HAVE_PTRDIFF_T) && !defined(_PTRDIFF_T_DEFINED)
typedef long ptrdiff_t;
#endif

/**
 * Declare 'index' which will point to sort key in sort key
 * buffer.
 */
typedef struct _collator_sort_key_index {
	char* key;       /* pointer to sort key */
	zval** zstr;     /* pointer to original string(hash-item) */
} collator_sort_key_index_t;

ZEND_EXTERN_MODULE_GLOBALS( intl )

static const size_t DEF_SORT_KEYS_BUF_SIZE = 1048576;
static const size_t DEF_SORT_KEYS_BUF_INCREMENT = 1048576;

static const size_t DEF_SORT_KEYS_INDX_BUF_SIZE = 1048576;
static const size_t DEF_SORT_KEYS_INDX_BUF_INCREMENT = 1048576;

static const size_t DEF_UTF16_BUF_SIZE = 1024;

/* {{{ collator_regular_compare_function */
static int collator_regular_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	Collator_object* co = NULL;

	int rc      = SUCCESS;

	zval* str1  = collator_convert_object_to_string( op1 TSRMLS_CC );
	zval* str2  = collator_convert_object_to_string( op2 TSRMLS_CC );

	zval* num1  = NULL;
	zval* num2  = NULL;
	zval* norm1 = NULL;
	zval* norm2 = NULL;

	/* If both args are strings AND either of args is not numeric string
	 * then use ICU-compare. Otherwise PHP-compare. */
	if( Z_TYPE_P(str1) == IS_STRING && Z_TYPE_P(str2) == IS_STRING &&
		( str1 == ( num1 = collator_convert_string_to_number_if_possible( str1 ) ) ||
		  str2 == ( num2 = collator_convert_string_to_number_if_possible( str2 ) ) ) )
	{
		/* Fetch collator object. */
		co = (Collator_object *) zend_object_store_get_object( INTL_G(current_collator) TSRMLS_CC );

		if (!co || !co->ucoll) {
			intl_error_set_code( NULL, COLLATOR_ERROR_CODE( co ) TSRMLS_CC );
			intl_errors_set_custom_msg( COLLATOR_ERROR_P( co ),
				"Object not initialized", 0 TSRMLS_CC );
			php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Object not initialized");
		}

		/* Compare the strings using ICU. */
		result->value.lval = ucol_strcoll(
				co->ucoll,
				INTL_Z_STRVAL_P(str1), INTL_Z_STRLEN_P(str1),
				INTL_Z_STRVAL_P(str2), INTL_Z_STRLEN_P(str2) );
		result->type = IS_LONG;
	}
	else
	{
		/* num1 is set if str1 and str2 are strings. */
		if( num1 )
		{
			if( num1 == str1 )
			{
				/* str1 is string but not numeric string
				 * just convert it to utf8. 
				 */
				norm1 = collator_convert_zstr_utf16_to_utf8( str1 );

				/* num2 is not set but str2 is string => do normalization. */
				norm2 = collator_normalize_sort_argument( str2 );
			}
			else
			{
				/* str1 is numeric strings => passthru to PHP-compare. */
				zval_add_ref( &num1 );
				norm1 = num1;

				/* str2 is numeric strings => passthru to PHP-compare. */
				zval_add_ref( &num2 );
				norm2 = num2;
			}
		}
		else
		{
			/* num1 is not set if str1 or str2 is not a string => do normalization. */
			norm1 = collator_normalize_sort_argument( str1 );

			/* if num1 is not set then num2 is not set as well => do normalization. */
			norm2 = collator_normalize_sort_argument( str2 );
		}

		rc = compare_function( result, norm1, norm2 TSRMLS_CC );

		zval_ptr_dtor( &norm1 );
		zval_ptr_dtor( &norm2 );
	}

	if( num1 )
		zval_ptr_dtor( &num1 );

	if( num2 )
		zval_ptr_dtor( &num2 );

	zval_ptr_dtor( &str1 );
	zval_ptr_dtor( &str2 );

	return rc;
}
/* }}} */

/* {{{ collator_numeric_compare_function
 * Convert input args to double and compare it.
 */
static int collator_numeric_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	int rc     = SUCCESS;
	zval* num1 = NULL;
	zval* num2 = NULL;

	if( Z_TYPE_P(op1) == IS_STRING )
	{
		num1 = collator_convert_string_to_double( op1 );
		op1 = num1;
	}

	if( Z_TYPE_P(op2) == IS_STRING )
	{
		num2 = collator_convert_string_to_double( op2 );
		op2 = num2;
	}

	rc = numeric_compare_function( result, op1, op2 TSRMLS_CC);

	if( num1 )
		zval_ptr_dtor( &num1 );
	if( num2 )
		zval_ptr_dtor( &num2 );

	return rc;
}
/* }}} */

/* {{{ collator_icu_compare_function
 * Direct use of ucol_strcoll.
*/
static int collator_icu_compare_function(zval *result, zval *op1, zval *op2 TSRMLS_DC)
{
	int rc              = SUCCESS;
	Collator_object* co = NULL;
	zval* str1          = NULL;
	zval* str2          = NULL;

	str1 = collator_make_printable_zval( op1 );
	str2 = collator_make_printable_zval( op2 );

	/* Fetch collator object. */
	co = (Collator_object *) zend_object_store_get_object( INTL_G(current_collator) TSRMLS_CC );

	/* Compare the strings using ICU. */
	result->value.lval = ucol_strcoll(
			co->ucoll,
			INTL_Z_STRVAL_P(str1), INTL_Z_STRLEN_P(str1),
			INTL_Z_STRVAL_P(str2), INTL_Z_STRLEN_P(str2) );
	result->type = IS_LONG;

	zval_ptr_dtor( &str1 );
	zval_ptr_dtor( &str2 );

	return rc;
}
/* }}} */

/* {{{ collator_compare_func
 * Taken from PHP5 source (array_data_compare).
 */
static int collator_compare_func( const void* a, const void* b TSRMLS_DC )
{
	Bucket *f;
	Bucket *s;
	zval result;
	zval *first;
	zval *second;

	f = *((Bucket **) a);
	s = *((Bucket **) b);

	first = *((zval **) f->pData);
	second = *((zval **) s->pData);

	if( INTL_G(compare_func)( &result, first, second TSRMLS_CC) == FAILURE )
		return 0;

	if( Z_TYPE(result) == IS_DOUBLE )
	{
		if( Z_DVAL(result) < 0 )
			return -1;
		else if( Z_DVAL(result) > 0 )
			return 1;
		else
			return 0;
	}

	convert_to_long(&result);

	if( Z_LVAL(result) < 0 )
		return -1;
	else if( Z_LVAL(result) > 0 )
		return 1;

	return 0;
}
/* }}} */

/* {{{ collator_cmp_sort_keys
 * Compare sort keys
 */
static int collator_cmp_sort_keys( const void *p1, const void *p2 TSRMLS_DC )
{
	char* key1 = ((collator_sort_key_index_t*)p1)->key;
	char* key2 = ((collator_sort_key_index_t*)p2)->key;

	return strcmp( key1, key2 );
}
/* }}} */

/* {{{ collator_get_compare_function
 * Choose compare function according to sort flags.
 */
static collator_compare_func_t collator_get_compare_function( const long sort_flags )
{
	collator_compare_func_t func;

	switch( sort_flags )
	{
		case COLLATOR_SORT_NUMERIC:
			func = collator_numeric_compare_function;
			break;

		case COLLATOR_SORT_STRING:
			func = collator_icu_compare_function;
			break;

		case COLLATOR_SORT_REGULAR:
		default:
			func = collator_regular_compare_function;
			break;
	}

	return func;
}
/* }}} */

/* {{{ collator_sort_internal
 * Common code shared by collator_sort() and collator_asort() API functions.
 */
static void collator_sort_internal( int renumber, INTERNAL_FUNCTION_PARAMETERS )
{
	zval*          array            = NULL;
	HashTable*     hash             = NULL;
	zval*          saved_collator   = NULL;
	long           sort_flags       = COLLATOR_SORT_REGULAR;

	COLLATOR_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oa|l",
		&object, Collator_ce_ptr, &array, &sort_flags ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"collator_sort_internal: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	COLLATOR_METHOD_FETCH_OBJECT;

	/* Set 'compare function' according to sort flags. */
	INTL_G(compare_func) = collator_get_compare_function( sort_flags );

	hash = HASH_OF( array );

	/* Convert strings in the specified array from UTF-8 to UTF-16. */
	collator_convert_hash_from_utf8_to_utf16( hash, COLLATOR_ERROR_CODE_P( co ) );
	COLLATOR_CHECK_STATUS( co, "Error converting hash from UTF-8 to UTF-16" );

	/* Save specified collator in the request-global (?) variable. */
	saved_collator = INTL_G( current_collator );
	INTL_G( current_collator ) = object;

	/* Sort specified array. */
	zend_hash_sort( hash, zend_qsort, collator_compare_func, renumber TSRMLS_CC );

	/* Restore saved collator. */
	INTL_G( current_collator ) = saved_collator;

	/* Convert strings in the specified array back to UTF-8. */
	collator_convert_hash_from_utf16_to_utf8( hash, COLLATOR_ERROR_CODE_P( co ) );
	COLLATOR_CHECK_STATUS( co, "Error converting hash from UTF-16 to UTF-8" );

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool Collator::sort( Collator $coll, array(string) $arr [, int $sort_flags] )
 * Sort array using specified collator. }}} */
/* {{{ proto bool collator_sort(  Collator $coll, array(string) $arr [, int $sort_flags] )
 * Sort array using specified collator.
 */
PHP_FUNCTION( collator_sort )
{
	collator_sort_internal( TRUE, INTERNAL_FUNCTION_PARAM_PASSTHRU );
}
/* }}} */

/* {{{ proto bool Collator::sortWithSortKeys( Collator $coll, array(string) $arr )
 * Equivalent to standard PHP sort using Collator.
 * Uses ICU ucol_getSortKey for performance. }}} */
/* {{{ proto bool collator_sort_with_sort_keys( Collator $coll, array(string) $arr )
 * Equivalent to standard PHP sort using Collator.
 * Uses ICU ucol_getSortKey for performance.
 */
PHP_FUNCTION( collator_sort_with_sort_keys )
{
	zval*       array                = NULL;
	HashTable*  hash                 = NULL;
	zval**      hashData             = NULL;                     /* currently processed item of input hash */

	char*       sortKeyBuf           = NULL;                     /* buffer to store sort keys */
	uint32_t    sortKeyBufSize       = DEF_SORT_KEYS_BUF_SIZE;   /* buffer size */
	ptrdiff_t   sortKeyBufOffset     = 0;                        /* pos in buffer to store sort key */
	int32_t     sortKeyLen           = 0;                        /* the length of currently processing key */
	uint32_t    bufLeft              = 0;
	uint32_t    bufIncrement         = 0;

	collator_sort_key_index_t* sortKeyIndxBuf = NULL;            /* buffer to store 'indexes' which will be passed to 'qsort' */
	uint32_t    sortKeyIndxBufSize   = DEF_SORT_KEYS_INDX_BUF_SIZE;
	uint32_t    sortKeyIndxSize      = sizeof( collator_sort_key_index_t );

	uint32_t    sortKeyCount         = 0;
	uint32_t    j                    = 0;

	UChar*      utf16_buf            = NULL;                     /* tmp buffer to hold current processing string in utf-16 */
	int         utf16_buf_size       = DEF_UTF16_BUF_SIZE;       /* the length of utf16_buf */
	int         utf16_len            = 0;                        /* length of converted string */

	HashTable* sortedHash            = NULL;

	COLLATOR_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oa",
		&object, Collator_ce_ptr, &array ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			"collator_sort_with_sort_keys: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	COLLATOR_METHOD_FETCH_OBJECT;

	if (!co || !co->ucoll) {
		intl_error_set_code( NULL, COLLATOR_ERROR_CODE( co ) TSRMLS_CC );
		intl_errors_set_custom_msg( COLLATOR_ERROR_P( co ),
			"Object not initialized", 0 TSRMLS_CC );
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Object not initialized");

		RETURN_FALSE;
	}

	/*
	 * Sort specified array.
	 */
	hash = HASH_OF( array );

	if( !hash || zend_hash_num_elements( hash ) == 0 )
		RETURN_TRUE;

	/* Create bufers */
	sortKeyBuf     = ecalloc( sortKeyBufSize,     sizeof( char    ) );
	sortKeyIndxBuf = ecalloc( sortKeyIndxBufSize, sizeof( uint8_t ) );
	utf16_buf      = eumalloc( utf16_buf_size );

	/* Iterate through input hash and create a sort key for each value. */
	zend_hash_internal_pointer_reset( hash );
	while( zend_hash_get_current_data( hash, (void**) &hashData ) == SUCCESS )
	{
		/* Convert current hash item from UTF-8 to UTF-16LE and save the result to utf16_buf. */

		utf16_len = utf16_buf_size;

		/* Process string values only. */
		if( Z_TYPE_PP( hashData ) == IS_STRING )
		{
			intl_convert_utf8_to_utf16( &utf16_buf, &utf16_len, Z_STRVAL_PP( hashData ), Z_STRLEN_PP( hashData ), COLLATOR_ERROR_CODE_P( co ) );

			if( U_FAILURE( COLLATOR_ERROR_CODE( co ) ) )
			{
				intl_error_set_code( NULL, COLLATOR_ERROR_CODE( co ) TSRMLS_CC );
				intl_errors_set_custom_msg( COLLATOR_ERROR_P( co ), "Sort with sort keys failed", 0 TSRMLS_CC );

				if( utf16_buf )
					efree( utf16_buf );

				efree( sortKeyIndxBuf );
				efree( sortKeyBuf );

				RETURN_FALSE;
			}
		}
		else
		{
			/* Set empty string */
			utf16_len = 0;
			utf16_buf[utf16_len] = 0;
		}

		if( (utf16_len + 1) > utf16_buf_size )
			utf16_buf_size = utf16_len + 1;

		/* Get sort key, reallocating the buffer if needed. */
		bufLeft = sortKeyBufSize - sortKeyBufOffset;

		sortKeyLen = ucol_getSortKey( co->ucoll,
									  utf16_buf,
									  utf16_len,
									  (uint8_t*)sortKeyBuf + sortKeyBufOffset,
									  bufLeft );

		/* check for sortKeyBuf overflow, increasing its size of the buffer if needed */
		if( sortKeyLen > bufLeft )
		{
			bufIncrement = ( sortKeyLen > DEF_SORT_KEYS_BUF_INCREMENT ) ? sortKeyLen : DEF_SORT_KEYS_BUF_INCREMENT;

			sortKeyBufSize += bufIncrement;
			bufLeft += bufIncrement;

			sortKeyBuf = erealloc( sortKeyBuf, sortKeyBufSize );

			sortKeyLen = ucol_getSortKey( co->ucoll, utf16_buf, utf16_len, (uint8_t*)sortKeyBuf + sortKeyBufOffset, bufLeft );
		}

		/*  check sortKeyIndxBuf overflow, increasing its size of the buffer if needed */
		if( ( sortKeyCount + 1 ) * sortKeyIndxSize > sortKeyIndxBufSize )
		{
			bufIncrement = ( sortKeyIndxSize > DEF_SORT_KEYS_INDX_BUF_INCREMENT ) ? sortKeyIndxSize : DEF_SORT_KEYS_INDX_BUF_INCREMENT;

			sortKeyIndxBufSize += bufIncrement;

			sortKeyIndxBuf = erealloc( sortKeyIndxBuf, sortKeyIndxBufSize );
		}

		sortKeyIndxBuf[sortKeyCount].key = (char*)sortKeyBufOffset;    /* remeber just offset, cause address */
		                                                               /* of 'sortKeyBuf' may be changed due to realloc. */
		sortKeyIndxBuf[sortKeyCount].zstr = hashData;

		sortKeyBufOffset += sortKeyLen;
		++sortKeyCount;

		zend_hash_move_forward( hash );
	}

	/* update ptrs to point to valid keys. */
	for( j = 0; j < sortKeyCount; j++ )
		sortKeyIndxBuf[j].key = sortKeyBuf + (ptrdiff_t)sortKeyIndxBuf[j].key;

	/* sort it */
	zend_qsort( sortKeyIndxBuf, sortKeyCount, sortKeyIndxSize, collator_cmp_sort_keys TSRMLS_CC );

	/* for resulting hash we'll assign new hash keys rather then reordering */
	ALLOC_HASHTABLE( sortedHash );
	zend_hash_init( sortedHash, 0, NULL, ZVAL_PTR_DTOR, 0 );

	for( j = 0; j < sortKeyCount; j++ )
	{
		zval_add_ref( sortKeyIndxBuf[j].zstr );
		zend_hash_next_index_insert( sortedHash, sortKeyIndxBuf[j].zstr, sizeof(zval **), NULL );
	}

	/* Save sorted hash into return variable. */
	zval_dtor( array );
	(array)->value.ht = sortedHash;
	(array)->type = IS_ARRAY;

	if( utf16_buf )
		efree( utf16_buf );

	efree( sortKeyIndxBuf );
	efree( sortKeyBuf );

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool Collator::asort( Collator $coll, array(string) $arr )
 * Sort array using specified collator, maintaining index association. }}} */
/* {{{ proto bool collator_asort( Collator $coll, array(string) $arr )
 * Sort array using specified collator, maintaining index association.
 */
PHP_FUNCTION( collator_asort )
{
	collator_sort_internal( FALSE, INTERNAL_FUNCTION_PARAM_PASSTHRU );
}
/* }}} */

/* {{{ proto bool Collator::getSortKey( Collator $coll, string $str )
 * Get a sort key for a string from a Collator. }}} */
/* {{{ proto bool collator_get_sort_key( Collator $coll, string $str )
 * Get a sort key for a string from a Collator. }}} */
PHP_FUNCTION( collator_get_sort_key )
{
	char*            str      = NULL;
	int              str_len  = 0;
	UChar*           ustr     = NULL;
	int              ustr_len = 0;
	uint8_t*         key     = NULL;
	int              key_len = 0;

	COLLATOR_METHOD_INIT_VARS

	/* Parse parameters. */
	if( zend_parse_method_parameters( ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os",
		&object, Collator_ce_ptr, &str, &str_len ) == FAILURE )
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
			 "collator_get_sort_key: unable to parse input params", 0 TSRMLS_CC );

		RETURN_FALSE;
	}

	/* Fetch the object. */
	COLLATOR_METHOD_FETCH_OBJECT;

	if (!co || !co->ucoll) {
		intl_error_set_code( NULL, COLLATOR_ERROR_CODE( co ) TSRMLS_CC );
		intl_errors_set_custom_msg( COLLATOR_ERROR_P( co ),
			"Object not initialized", 0 TSRMLS_CC );
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "Object not initialized");

		RETURN_FALSE;
	}

	/*
	 * Compare given strings (converting them to UTF-16 first).
	 */

	/* First convert the strings to UTF-16. */
	intl_convert_utf8_to_utf16(
		&ustr, &ustr_len, str, str_len, COLLATOR_ERROR_CODE_P( co ) );
	if( U_FAILURE( COLLATOR_ERROR_CODE( co ) ) )
	{
		/* Set global error code. */
		intl_error_set_code( NULL, COLLATOR_ERROR_CODE( co ) TSRMLS_CC );

		/* Set error messages. */
		intl_errors_set_custom_msg( COLLATOR_ERROR_P( co ),
			"Error converting first argument to UTF-16", 0 TSRMLS_CC );
		efree( ustr );
		RETURN_FALSE;
	}

	/* ucol_getSortKey is exception in that the key length includes the 
	 * NUL terminator*/
	key_len = ucol_getSortKey(co->ucoll, ustr, ustr_len, key, 0);
	if(!key_len) {
		efree( ustr );
		RETURN_FALSE;
	}
	key = emalloc(key_len);
	key_len = ucol_getSortKey(co->ucoll, ustr, ustr_len, key, key_len);
	efree( ustr );
	if(!key_len) {
		RETURN_FALSE;
	}
	RETURN_STRINGL((char *)key, key_len - 1, 0);
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
