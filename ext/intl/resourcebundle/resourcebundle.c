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

#include <unicode/ures.h>

#include <zend.h>
#include <zend_API.h>

#include "intl_convert.h"
#include "intl_data.h"
#include "resourcebundle/resourcebundle_class.h"

/* {{{ ResourceBundle_extract_value */
void resourcebundle_extract_value( zval *return_value, ResourceBundle_object *source )
{
	UResType               restype;
	const UChar*           ufield;
	const uint8_t*         bfield;
	const int32_t*         vfield;
	int32_t                ilen;
	int                    i;
	zend_long              lfield;
	ResourceBundle_object* newrb;

	restype = ures_getType( source->child );
	switch (restype)
	{
		case URES_STRING:
			ufield = ures_getString( source->child, &ilen, &INTL_DATA_ERROR_CODE(source) );
			INTL_METHOD_CHECK_STATUS(source, "Failed to retrieve string value");
			INTL_METHOD_RETVAL_UTF8(source, (UChar *)ufield, ilen, 0);
			break;

		case URES_BINARY:
			bfield = ures_getBinary( source->child, &ilen, &INTL_DATA_ERROR_CODE(source) );
			INTL_METHOD_CHECK_STATUS(source, "Failed to retrieve binary value");
			ZVAL_STRINGL( return_value, (char *) bfield, ilen );
			break;

		case URES_INT:
			lfield = ures_getInt( source->child, &INTL_DATA_ERROR_CODE(source) );
			INTL_METHOD_CHECK_STATUS(source, "Failed to retrieve integer value");
			ZVAL_LONG( return_value, lfield );
			break;

		case URES_INT_VECTOR:
			vfield = ures_getIntVector( source->child, &ilen, &INTL_DATA_ERROR_CODE(source) );
			INTL_METHOD_CHECK_STATUS(source, "Failed to retrieve vector value");
			array_init( return_value );
			for (i=0; i<ilen; i++) {
				add_next_index_long( return_value, vfield[i] );
			}
			break;

		case URES_ARRAY:
		case URES_TABLE:
			object_init_ex( return_value, ResourceBundle_ce_ptr );
			newrb = Z_INTL_RESOURCEBUNDLE_P(return_value);
			newrb->me = source->child;
			source->child = NULL;
			intl_errors_reset(INTL_DATA_ERROR_P(source));
			break;

		default:
			intl_errors_set(INTL_DATA_ERROR_P(source), U_ILLEGAL_ARGUMENT_ERROR, "Unknown resource type", 0);
			RETURN_FALSE;
			break;
	}
}
/* }}} */
