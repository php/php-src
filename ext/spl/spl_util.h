#ifndef SPL_UTIL
#define SPL_UTIL

#include "zend_types.h"

static zend_long spl_offset_convert_to_long(zval *offset) /* {{{ */
{
	try_again:
	switch (Z_TYPE_P(offset)) {
		case IS_STRING: {
			zend_ulong index;
			if (ZEND_HANDLE_NUMERIC(Z_STR_P(offset), index)) {
				return (zend_long) index;
			}
			break;
		}
		case IS_DOUBLE:
			return zend_dval_to_lval_safe(Z_DVAL_P(offset));
		case IS_LONG:
			return Z_LVAL_P(offset);
		case IS_FALSE:
			return 0;
		case IS_TRUE:
			return 1;
		case IS_REFERENCE:
			offset = Z_REFVAL_P(offset);
			goto try_again;
		case IS_RESOURCE:
			zend_use_resource_as_offset(offset);
			return Z_RES_HANDLE_P(offset);
	}

	zend_type_error("Illegal offset type");
	return 0;
}

#endif
