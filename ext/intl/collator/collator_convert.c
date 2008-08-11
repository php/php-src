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

#define COLLATOR_CONVERT_RETURN_FAILED( retval ) \
{                                                \
    zval_add_ref( &retval );                     \
    return retval;                               \
}

/* {{{ collator_convert_object_to_string
 * Convert object to UTF16-encoded string.
 */
zval* collator_convert_object_to_string( zval* obj TSRMLS_DC )
{
	zval* zstr        = NULL;

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

		if( Z_OBJ_HT_P(obj)->cast_object( obj, zstr, IS_UNICODE, 0 TSRMLS_CC ) == FAILURE )
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

	if( Z_TYPE_P( str ) != IS_UNICODE )
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

	if( Z_TYPE_P(arg) != IS_UNICODE )
	{
		zend_make_printable_zval(arg, &arg_copy, &use_copy);

		if( use_copy )
		{
			/* Don't copy arg_copy data to str. */
			ALLOC_ZVAL( str );
			*str = arg_copy;

			/* Reset refcounter. */
			INIT_PZVAL( str );
		}
		else
		{
			zval_add_ref( &arg );
			str = arg;
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

	if( Z_TYPE_P( arg ) != IS_UNICODE )
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

		zval_add_ref( &arg );
		n_arg = arg;
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
