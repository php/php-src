/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Harald Radi <h.radi@nme.at>                                  |
   |         Alan Brown <abrown@pobox.com>                                |
   |         Paul Shortis <pshortis@dataworx.com.au>                      |
   +----------------------------------------------------------------------+
 */

/*
 * 03.6.2001
 * Added SafeArray ==> Hash support
 */

/* 
 * Paul Shortis June 7, 2001 <pshortis@dataworx.com.au> - Added code to support SafeArray passing
 * to COM objects. Support includes passing arrays of variants as well
 * as typed arrays.
 */
 
#ifdef PHP_WIN32

#include "php.h"
#include "php_COM.h"
#include "php_VARIANT.h"

/* prototypes */

static void comval_to_variant(pval *pval_arg, VARIANT *var_arg TSRMLS_DC);

/* implementations */
PHPAPI void php_pval_to_variant(pval *pval_arg, VARIANT *var_arg, int codepage TSRMLS_DC)
{
	VARTYPE type = VT_EMPTY;	/* default variant type */

	switch (Z_TYPE_P(pval_arg)) {
		case IS_NULL:
			type = VT_NULL;
			break;

		case IS_BOOL:
			type = VT_BOOL;
			break;

		case IS_OBJECT:
			if (!strcmp(Z_OBJCE_P(pval_arg)->name, "VARIANT")) {
				type = VT_VARIANT|VT_BYREF;
			} else {
				type = VT_DISPATCH;
			}
			break;

		case IS_ARRAY:
			type = VT_ARRAY;
			break;

		case IS_RESOURCE:
		case IS_CONSTANT:
		case IS_CONSTANT_ARRAY:
			/* ?? */
			break;

		case IS_LONG:
			type = VT_I4;   /* assuming 32-bit platform */
			break;

		case IS_DOUBLE:
			type = VT_R8;   /* assuming 64-bit double precision */
			break;

		case IS_STRING:
			type = VT_BSTR;
			break;
	}

	php_pval_to_variant_ex2(pval_arg, var_arg, type, codepage TSRMLS_CC);
}


PHPAPI void php_pval_to_variant_ex(pval *pval_arg, VARIANT *var_arg, pval *pval_type, int codepage TSRMLS_DC)
{
	php_pval_to_variant_ex2(pval_arg, var_arg, (unsigned short) Z_LVAL_P(pval_type), codepage TSRMLS_CC);
}


PHPAPI void php_pval_to_variant_ex2(pval *pval_arg, VARIANT *var_arg, VARTYPE type, int codepage TSRMLS_DC)
{
	OLECHAR *unicode_str;

	VariantInit(var_arg);
	V_VT(var_arg) = type;

	if (V_VT(var_arg) & VT_ARRAY) {
		/* For now we'll just handle single dimension arrays, we'll use the data type of the first element for the
		   output data type */
		HashTable *ht = Z_ARRVAL(*pval_arg);
		int numberOfElements = zend_hash_num_elements(ht);
		SAFEARRAY *safeArray;
		SAFEARRAYBOUND bounds[1];
		VARIANT *v, var;
		zval **entry;        /* An entry in the input array */
		
		type &= ~VT_ARRAY;

		if (V_VT(var_arg) == (VT_ARRAY|VT_BYREF)) {		/* == is intended, because VT_*|VT_BYREF|VT_ARRAY means something diffrent */
			type &= ~VT_BYREF;
			ALLOC_VARIANT(V_VARIANTREF(var_arg));
			var_arg = V_VARIANTREF(var_arg);		/* put the array in that VARIANT */
		}
		if (!type) {
			/* if no type is given we take the variant type */
			type = VT_VARIANT;
		}

		bounds[0].lLbound = 0;
		bounds[0].cElements = numberOfElements;
		safeArray = SafeArrayCreate(type, 1, bounds);
		
		if (NULL == safeArray) {
			php_error( E_WARNING,"Unable to convert php array to VARIANT array - %s", numberOfElements ? "" : "(Empty input array)");
			ZVAL_FALSE(pval_arg);
		} else {
			V_ARRAY(var_arg) = safeArray;
			V_VT(var_arg) = VT_ARRAY|type;                /* Now have a valid safe array allocated */
			if (SUCCEEDED(SafeArrayLock(safeArray))) {
				ulong i;
				UINT size = SafeArrayGetElemsize(safeArray);

				zend_hash_internal_pointer_reset(ht);
				for (i = 0; i < (ulong)numberOfElements; ++i) {
					if ((zend_hash_get_current_data(ht, (void **)&entry) == SUCCESS) && (entry != NULL)) { /* Get a pointer to the php array element */
						/* Add another value to the safe array */
						if (SUCCEEDED(SafeArrayPtrOfIndex(safeArray, &i, &v))) {			/* Pointer to output element entry retrieved successfully */
							if (type == VT_VARIANT) {
								php_pval_to_variant(*entry, v, codepage TSRMLS_CC);				/* Do the required conversion */
							} else {
								php_pval_to_variant_ex2(*entry, &var, type, codepage TSRMLS_CC);	/* Do the required conversion */
								memcpy(v, &(var.byref), size);
							}
						} else {
							php_error( E_WARNING,"phpArrayToSafeArray() - Unable to retrieve pointer to output element number (%d)", i);
						}
					}
					zend_hash_move_forward(ht);
				}
				SafeArrayUnlock( safeArray);
			} else {
				php_error( E_WARNING,"phpArrayToSafeArray() - Unable to lock safeArray");
			}
		}
	} else {
		switch (V_VT(var_arg)) {

			case VT_NULL:
			case VT_VOID:
				ZVAL_NULL(pval_arg);
				break; 
			
			case VT_UI1:
				convert_to_long_ex(&pval_arg);
				V_UI1(var_arg) = (unsigned char) Z_LVAL_P(pval_arg);
				break;

			case VT_I2:
				convert_to_long_ex(&pval_arg);
				V_I2(var_arg) = (short) Z_LVAL_P(pval_arg);
				break;

			case VT_I4:
				convert_to_long_ex(&pval_arg);
				V_I4(var_arg) = Z_LVAL_P(pval_arg);
				break;

			case VT_R4:
				convert_to_double_ex(&pval_arg);
				V_R4(var_arg) = (float) Z_DVAL_P(pval_arg);
				break;

			case VT_R8:
				convert_to_double_ex(&pval_arg);
				V_R8(var_arg) = Z_DVAL_P(pval_arg);
				break;

			case VT_BOOL:
				convert_to_boolean_ex(&pval_arg);
				if (Z_LVAL_P(pval_arg)) {
					V_BOOL(var_arg) = VT_TRUE;
				} else {
					V_BOOL(var_arg) = VT_FALSE;
				}
				break;

			case VT_ERROR:
				convert_to_long_ex(&pval_arg);
				V_ERROR(var_arg) = Z_LVAL_P(pval_arg);
				break;

			case VT_CY:
				convert_to_double_ex(&pval_arg);
				VarCyFromR8(Z_DVAL_P(pval_arg), &V_CY(var_arg));
				break;

			case VT_DATE: {
					SYSTEMTIME wintime;
					struct tm *phptime;

					switch (Z_TYPE_P(pval_arg)) {
						case IS_DOUBLE:
							/* already a VariantTime value */
							V_DATE(var_arg) = Z_DVAL_P(pval_arg);
							break;

						/** @todo
						case IS_STRING:
						*/
							/* string representation of a time value */

						default:
							/* a PHP time value ? */
							convert_to_long_ex(&pval_arg);
							phptime = gmtime(&(Z_LVAL_P(pval_arg)));
							memset(&wintime, 0, sizeof(wintime));

							wintime.wYear = phptime->tm_year + 1900;
							wintime.wMonth = phptime->tm_mon + 1;
							wintime.wDay = phptime->tm_mday;
							wintime.wHour = phptime->tm_hour;
							wintime.wMinute = phptime->tm_min;
							wintime.wSecond = phptime->tm_sec;

							SystemTimeToVariantTime(&wintime, &V_DATE(var_arg));
							break;
					}
				}
				break;

			case VT_BSTR:
				convert_to_string_ex(&pval_arg);
				unicode_str = php_char_to_OLECHAR(Z_STRVAL_P(pval_arg), Z_STRLEN_P(pval_arg), codepage TSRMLS_CC);
				V_BSTR(var_arg) = SysAllocStringByteLen((char *) unicode_str, Z_STRLEN_P(pval_arg) * sizeof(OLECHAR));
				efree(unicode_str);
				break;

			case VT_DECIMAL:
				convert_to_string_ex(&pval_arg);
				unicode_str = php_char_to_OLECHAR(Z_STRVAL_P(pval_arg), Z_STRLEN_P(pval_arg), codepage TSRMLS_CC);
				VarDecFromStr(unicode_str, LOCALE_SYSTEM_DEFAULT, 0, &V_DECIMAL(var_arg));
				break;

			case VT_DECIMAL|VT_BYREF:
				convert_to_string_ex(&pval_arg);
				unicode_str = php_char_to_OLECHAR(Z_STRVAL_P(pval_arg), Z_STRLEN_P(pval_arg), codepage TSRMLS_CC);
				VarDecFromStr(unicode_str, LOCALE_SYSTEM_DEFAULT, 0, V_DECIMALREF(var_arg));
				break;

			case VT_UNKNOWN:
				comval_to_variant(pval_arg, var_arg TSRMLS_CC);
				if (V_VT(var_arg) != VT_DISPATCH) {
					VariantInit(var_arg);
				} else {
					V_VT(var_arg) = VT_UNKNOWN;
					V_UNKNOWN(var_arg) = (IUnknown *) V_DISPATCH(var_arg);
				}
				break;

			case VT_DISPATCH:
				if (!strcmp(Z_OBJCE_P(pval_arg)->name, "COM")) {
					comval_to_variant(pval_arg, var_arg TSRMLS_CC);
				} else {
					V_DISPATCH(var_arg) = php_COM_export_object(pval_arg TSRMLS_CC);
					if (V_DISPATCH(var_arg)) {
						V_VT(var_arg) = VT_DISPATCH;
					}
				}
				if (V_VT(var_arg) != VT_DISPATCH) {
					VariantInit(var_arg);
				}
				break;

			case VT_UI1|VT_BYREF:
				convert_to_long(pval_arg);
				V_UI1REF(var_arg) = (unsigned char FAR*) &Z_LVAL_P(pval_arg);
				break;

			case VT_I2|VT_BYREF:
				convert_to_long(pval_arg);
				V_I2REF(var_arg) = (short FAR*) &Z_LVAL_P(pval_arg);
				break;

			case VT_I4|VT_BYREF:
				convert_to_long(pval_arg);
				V_I4REF(var_arg) = (long FAR*) &Z_LVAL_P(pval_arg);
				break;

			case VT_R4|VT_BYREF:
				convert_to_double(pval_arg);
				V_R4REF(var_arg) = (float FAR*) &Z_LVAL_P(pval_arg);
				break;

			case VT_R8|VT_BYREF:
				convert_to_double(pval_arg);
				V_R8REF(var_arg) = (double FAR*) &Z_LVAL_P(pval_arg);
				break;

			case VT_BOOL|VT_BYREF:
				convert_to_boolean(pval_arg);
				/* emalloc or malloc ? */
				V_BOOLREF(var_arg) = (short FAR*) pemalloc(sizeof(short), 1);
				if (Z_LVAL_P(pval_arg)) {
					*V_BOOLREF(var_arg) = VT_TRUE;
				} else {
					*V_BOOLREF(var_arg) = VT_TRUE;
				}
				break;

			case VT_ERROR|VT_BYREF:
				convert_to_long(pval_arg);
				V_ERRORREF(var_arg) = (long FAR*) &Z_LVAL_P(pval_arg);
				break;

			case VT_CY|VT_BYREF:
				convert_to_double_ex(&pval_arg);
				VarCyFromR8(Z_DVAL_P(pval_arg), var_arg->pcyVal);
				break;

			case VT_DATE|VT_BYREF: {
					SYSTEMTIME wintime;
					struct tm *phptime;

					phptime = gmtime(&(Z_LVAL_P(pval_arg)));
					memset(&wintime, 0, sizeof(wintime));

					wintime.wYear   = phptime->tm_year + 1900;
					wintime.wMonth  = phptime->tm_mon + 1;
					wintime.wDay    = phptime->tm_mday;
					wintime.wHour   = phptime->tm_hour;
					wintime.wMinute = phptime->tm_min;
					wintime.wSecond = phptime->tm_sec;

					SystemTimeToVariantTime(&wintime, var_arg->pdate);
				}
				break;

			case VT_BSTR|VT_BYREF:
				convert_to_string(pval_arg);
				V_BSTRREF(var_arg) = (BSTR FAR*) emalloc(sizeof(BSTR FAR*));
				unicode_str = php_char_to_OLECHAR(Z_STRVAL_P(pval_arg), Z_STRLEN_P(pval_arg), codepage TSRMLS_CC);
				*V_BSTRREF(var_arg) = SysAllocString(unicode_str);
				efree(unicode_str);
				break;

			case VT_UNKNOWN|VT_BYREF:
				comval_to_variant(pval_arg, var_arg TSRMLS_CC);
				if (V_VT(var_arg) != VT_DISPATCH) {
					VariantInit(var_arg);
				} else {
					V_VT(var_arg) = VT_UNKNOWN|VT_BYREF;
					V_UNKNOWNREF(var_arg) = (IUnknown **) &V_DISPATCH(var_arg);
				}
				break;

			case VT_DISPATCH|VT_BYREF:
				comval_to_variant(pval_arg, var_arg TSRMLS_CC);
				if (V_VT(var_arg) != VT_DISPATCH) {
					VariantInit(var_arg);
				} else {
					V_VT(var_arg) = VT_DISPATCH|VT_BYREF;
					V_DISPATCHREF(var_arg) = &V_DISPATCH(var_arg);
				}
				break;

			case VT_VARIANT:
				php_error(E_WARNING,"VT_VARIANT is invalid. Use VT_VARIANT|VT_BYREF instead.");
				/* break missing intentionally */
			case VT_VARIANT|VT_BYREF: {
					int tp;
					pval **var_handle;

					/* fetch the VARIANT structure */
					zend_hash_index_find(Z_OBJPROP_P(pval_arg), 0, (void **) &var_handle);

					V_VT(var_arg) = VT_VARIANT|VT_BYREF;
					V_VARIANTREF(var_arg) = (VARIANT FAR*) zend_list_find(Z_LVAL_P(*var_handle), &tp);

					if (!V_VARIANTREF(var_arg) && (tp != IS_VARIANT)) {
						VariantInit(var_arg);
					}
				}
			/*
				should be, but isn't :)

				if (V_VT(var_arg) != (VT_VARIANT|VT_BYREF)) {
					VariantInit(var_arg);
				}
			*/
				break;

			case VT_I1:
				convert_to_long_ex(&pval_arg);
				V_I1(var_arg) = (char)Z_LVAL_P(pval_arg);
				break;

			case VT_UI2:
				convert_to_long_ex(&pval_arg);
				V_UI2(var_arg) = (unsigned short)Z_LVAL_P(pval_arg);
				break;

			case VT_UI4:
				convert_to_long_ex(&pval_arg);
				V_UI4(var_arg) = (unsigned long)Z_LVAL_P(pval_arg);
				break;

			case VT_INT:
				convert_to_long_ex(&pval_arg);
				V_INT(var_arg) = (int)Z_LVAL_P(pval_arg);
				break;

			case VT_UINT:
				convert_to_long_ex(&pval_arg);
				V_UINT(var_arg) = (unsigned int)Z_LVAL_P(pval_arg);
				break;

			case VT_I1|VT_BYREF:
				convert_to_long(pval_arg);
				V_I1REF(var_arg) = (char FAR*) &Z_LVAL_P(pval_arg);
				break;

			case VT_UI2|VT_BYREF:
				convert_to_long(pval_arg);
				V_UI2REF(var_arg) = (unsigned short FAR*) &Z_LVAL_P(pval_arg);
				break;

			case VT_UI4|VT_BYREF:
				convert_to_long(pval_arg);
				V_UI4REF(var_arg) = (unsigned long FAR*) &Z_LVAL_P(pval_arg);
				break;

			case VT_INT|VT_BYREF:
				convert_to_long(pval_arg);
				V_INTREF(var_arg) = (int FAR*) &Z_LVAL_P(pval_arg);
				break;

			case VT_UINT|VT_BYREF:
				convert_to_long(pval_arg);
				V_UINTREF(var_arg) = (unsigned int FAR*) &Z_LVAL_P(pval_arg);
				break;

			default:
				php_error(E_WARNING,"Unsupported variant type: %d (0x%X)", V_VT(var_arg), V_VT(var_arg));
		}
	}
}


PHPAPI int php_variant_to_pval(VARIANT *var_arg, pval *pval_arg, int codepage TSRMLS_DC)
{
	/* Changed the function to return a value for recursive error testing */
	/* Existing calls will be unaffected by the change - so it */
	/* seemed like the smallest impact on unfamiliar code */
	int ret = SUCCESS; 

	INIT_PZVAL(pval_arg);

	/* Add SafeArray support */
	if (V_ISARRAY(var_arg)) {
		SAFEARRAY *array = V_ARRAY(var_arg);
		LONG indices[1];
		LONG lbound=0, ubound;
		VARTYPE vartype;
		register int ii;
		UINT Dims;
		VARIANT vv;
		pval *element;
		HRESULT hr;

		/* TODO: Add support for multi-dimensional SafeArrays */
		/* For now just validate that the SafeArray has one dimension */
		if (1 != (Dims = SafeArrayGetDim(array))) {
			php_error(E_WARNING,"Unsupported: multi-dimensional (%d) SafeArrays", Dims);
			ZVAL_NULL(pval_arg);
			return FAILURE;
		}
        SafeArrayLock(array);

		/* This call has failed for everything I have tried */
		/* But best leave it to be on the safe side */
		if (FAILED(SafeArrayGetVartype(array, &vartype)) || (vartype == VT_EMPTY)) {
			/* Fall back to what we do know */
			/* Mask off the array bit and assume */
			/* what is left is the type of the array */
			/* elements */
			vartype = V_VT(var_arg) & ~VT_ARRAY;
		}
		SafeArrayGetUBound(array, 1, &ubound);
		SafeArrayGetLBound(array, 1, &lbound);

		/* Since COM returned an array we set up the php */
		/* return value to be an array */
		array_init(pval_arg);

		/* Walk the safe array */
		for (ii=lbound;ii<=ubound;ii++) {
			indices[0] = ii;
			VariantInit(&vv); /* Docs say this just set the vt field, but you never know */
			/* Set up a variant to pass to a recursive call */
			/* So that we do not need to have two copies */
			/* of the code */
			if (VT_VARIANT == vartype) {
				hr = SafeArrayGetElement(array, indices, (VOID *) &(vv));
			} else {
				V_VT(&vv) = vartype;
				hr = SafeArrayGetElement(array, indices, (VOID *) &(vv.lVal));
			}
			if (FAILED(hr)) {
				/* Failure to retieve an element probably means the array is sparse */
				/* So leave the php array sparse too */
				continue;
			}
			/* Create an element to be added to the array */
			ALLOC_ZVAL(element);
			/* Call ourself again to handle the base type conversion */
			/* If SafeArrayGetElement proclaims to allocate */
			/* memory for a BSTR, so the recursive call frees */
			/* the string correctly */
			if (FAILURE == php_variant_to_pval(&vv, element, codepage TSRMLS_CC)) {
				/* Error occurred setting up array element */
				/* Error was displayed by the recursive call */
				FREE_ZVAL(element);
				/* TODO: Do we stop here, or go on and */
				/* try to make sense of the rest of the array */
				/* Going on leads to multiple errors displayed */
				/* for the same conversion. For large arrays that */
				/* could be very annoying */
				/* And if we don't go on - what to do about */
				/* the parts of the array that are OK? */
				/* break; */
			} else {
				/* Just insert the element into our return array */
				add_index_zval(pval_arg, ii, element);
			}
		}
		SafeArrayUnlock(array);
	} else switch (var_arg->vt & ~VT_BYREF) {
		case VT_EMPTY:
			ZVAL_NULL(pval_arg);
			break;

		case VT_UI1:
			if (V_ISBYREF(var_arg)) {
				ZVAL_LONG(pval_arg, (long)*V_UI1REF(var_arg));
			} else {
				ZVAL_LONG(pval_arg, (long)V_UI1(var_arg));
			}
			break;

		case VT_I2:
			if (V_ISBYREF(var_arg)) {
				ZVAL_LONG(pval_arg, (long )*V_I2REF(var_arg));
			} else {
				ZVAL_LONG(pval_arg, (long)V_I2(var_arg));
			}
			break;

		case VT_I4:
			if (V_ISBYREF(var_arg)) {
				ZVAL_LONG(pval_arg, *V_I4REF(var_arg));
			} else {
				ZVAL_LONG(pval_arg, V_I4(var_arg));
			}
			break;

		case VT_R4:
			if (V_ISBYREF(var_arg)) {
				ZVAL_DOUBLE(pval_arg, (double)*V_R4REF(var_arg));
			} else {
				ZVAL_DOUBLE(pval_arg, (double)V_R4(var_arg));
			}
			break;

		case VT_R8:
			if (V_ISBYREF(var_arg)) {
				ZVAL_DOUBLE(pval_arg, *V_R8REF(var_arg));
			} else {
				ZVAL_DOUBLE(pval_arg, V_R8(var_arg));
			}
			break;

		/* 96bit uint */
		case VT_DECIMAL: {
				OLECHAR *unicode_str;
				switch (VarBstrFromDec(&V_DECIMAL(var_arg), LOCALE_SYSTEM_DEFAULT, 0, &unicode_str)) {
					case S_OK:
						Z_STRVAL_P(pval_arg) = php_OLECHAR_to_char(unicode_str, &Z_STRLEN_P(pval_arg), codepage TSRMLS_CC);
						Z_TYPE_P(pval_arg) = IS_STRING;
						break;

					default:
						ZVAL_NULL(pval_arg);
						ret = FAILURE;
						php_error(E_WARNING, "Error converting DECIMAL value to PHP string");
						break;
				}
			}
			break;

		/* Currency */
		case VT_CY:
			if (V_ISBYREF(var_arg)) {
				VarR8FromCy(*V_CYREF(var_arg), &Z_DVAL_P(pval_arg));
			} else {
				VarR8FromCy(V_CY(var_arg), &Z_DVAL_P(pval_arg));
			}
			Z_TYPE_P(pval_arg) = IS_DOUBLE;
			break;

		case VT_BOOL:
			if (V_ISBYREF(var_arg)) {
				if (*V_BOOLREF(var_arg)) {
					ZVAL_BOOL(pval_arg, Z_TRUE);
				} else {
					ZVAL_BOOL(pval_arg, Z_FALSE);
				}
			} else {
				if (V_BOOL(var_arg)) {
					ZVAL_BOOL(pval_arg, Z_TRUE);
				} else {
					ZVAL_BOOL(pval_arg, Z_FALSE);
				}
			}
			break;

		case VT_NULL:
		case VT_VOID:
			ZVAL_NULL(pval_arg);
			break;

		case VT_VARIANT:
			php_variant_to_pval(V_VARIANTREF(var_arg), pval_arg, codepage TSRMLS_CC);
			break;

		case VT_BSTR:
			Z_TYPE_P(pval_arg) = IS_STRING;

			if (V_ISBYREF(var_arg)) {
				if (*V_BSTR(var_arg)) {
					Z_STRVAL_P(pval_arg) = php_OLECHAR_to_char(*V_BSTRREF(var_arg), &Z_STRLEN_P(pval_arg), codepage TSRMLS_CC);
				} else {
					ZVAL_NULL(pval_arg);
				}
				efree(V_BSTRREF(var_arg));
			} else {
				if (V_BSTR(var_arg)) {
					Z_STRVAL_P(pval_arg) = php_OLECHAR_to_char(V_BSTR(var_arg), &Z_STRLEN_P(pval_arg), codepage TSRMLS_CC);
				} else {
					ZVAL_NULL(pval_arg);
				}
			}

			break;

		case VT_DATE: {
				BOOL success;
				SYSTEMTIME wintime;
				struct tm phptime;

				if (V_ISBYREF(var_arg)) {
					success = VariantTimeToSystemTime(*V_DATEREF(var_arg), &wintime);
				} else {
					success = VariantTimeToSystemTime(V_DATE(var_arg), &wintime);
				}

				if (success) {
					memset(&phptime, 0, sizeof(phptime));

					phptime.tm_year  = wintime.wYear - 1900;
					phptime.tm_mon   = wintime.wMonth - 1;
					phptime.tm_mday  = wintime.wDay;
					phptime.tm_hour  = wintime.wHour;
					phptime.tm_min   = wintime.wMinute;
					phptime.tm_sec   = wintime.wSecond;
					phptime.tm_isdst = -1;

					tzset();
					ZVAL_LONG(pval_arg, mktime(&phptime));
				} else {
					ret = FAILURE;
				}
			}
			break;

		case VT_UNKNOWN:
			if (V_UNKNOWN(var_arg) == NULL) {
				V_DISPATCH(var_arg) = NULL;
			} else {
				HRESULT hr;

				hr = V_UNKNOWN(var_arg)->lpVtbl->QueryInterface(var_arg->punkVal, &IID_IDispatch, &V_DISPATCH(var_arg));

				if (FAILED(hr)) {
					char *error_message;

					error_message = php_COM_error_message(hr TSRMLS_CC);
					php_error(E_WARNING,"Unable to obtain IDispatch interface:  %s", error_message);
					LocalFree(error_message);

					V_DISPATCH(var_arg) = NULL;
				}
			}
			/* break missing intentionaly */
		case VT_DISPATCH: {
				comval *obj;

				if (V_DISPATCH(var_arg) == NULL) {
					ret = FAILURE;
					ZVAL_NULL(pval_arg);
				} else {
					ALLOC_COM(obj);
					php_COM_set(obj, &V_DISPATCH(var_arg), FALSE TSRMLS_CC);
					
					ZVAL_COM(pval_arg, obj);
				}
			}
			break;

		case VT_I1:
			if (V_ISBYREF(var_arg)) {
				ZVAL_LONG(pval_arg, (long)*V_I1REF(var_arg));
			} else {
				ZVAL_LONG(pval_arg, (long)V_I1(var_arg));
			}
			break;

		case VT_UI2:
			if (V_ISBYREF(var_arg)) {
				ZVAL_LONG(pval_arg, (long)*V_UI2REF(var_arg));
			} else {
				ZVAL_LONG(pval_arg, (long)V_UI2(var_arg));
			}
			break;

		case VT_UI4:
			if (V_ISBYREF(var_arg)) {
				ZVAL_LONG(pval_arg, (long)*V_UI4REF(var_arg));
			} else {
				ZVAL_LONG(pval_arg, (long)V_UI4(var_arg));
			}
			break;

		case VT_INT:
			if (V_ISBYREF(var_arg)) {
				ZVAL_LONG(pval_arg, (long)*V_INTREF(var_arg));
			} else {
				ZVAL_LONG(pval_arg, (long)V_INT(var_arg));
			}
			break;

		case VT_UINT:
			if (V_ISBYREF(var_arg)) {
				ZVAL_LONG(pval_arg, (long)*V_UINTREF(var_arg));
			} else {
				ZVAL_LONG(pval_arg, (long)V_UINT(var_arg));
			}
			break;

		default:
			php_error(E_WARNING,"Unsupported variant type: %d (0x%X)", V_VT(var_arg), V_VT(var_arg));
			ZVAL_NULL(pval_arg);
			ret = FAILURE;
			break;
	}
	return ret;
}


PHPAPI OLECHAR *php_char_to_OLECHAR(char *C_str, uint strlen, int codepage TSRMLS_DC)
{
	BOOL error = FALSE;
	OLECHAR *unicode_str;

	if (strlen == -1) {
		/* request needed buffersize */
		strlen = MultiByteToWideChar(codepage, (codepage == CP_UTF8 ? 0 : MB_PRECOMPOSED | MB_ERR_INVALID_CHARS), C_str, -1, NULL, 0);
	} else {
		/* \0 terminator */
		strlen++;
	}

	if (strlen >= 0) {
		unicode_str = (OLECHAR *) emalloc(sizeof(OLECHAR) * strlen);

		/* convert string */
		error = !MultiByteToWideChar(codepage, (codepage == CP_UTF8 ? 0 : MB_PRECOMPOSED | MB_ERR_INVALID_CHARS), C_str, strlen, unicode_str, strlen);
	} else {
		/* return a zero-length string */
		unicode_str = (OLECHAR *) emalloc(sizeof(OLECHAR));
		*unicode_str = 0;

		error = TRUE;
	}

	if (error) {
		switch (GetLastError()) {
			case ERROR_NO_UNICODE_TRANSLATION:
				php_error(E_WARNING, "No unicode translation available for the specified string");
				break;
			case ERROR_INSUFFICIENT_BUFFER:
				php_error(E_WARNING, "Internal Error: Insufficient Buffer");
				break;
			default:
				php_error(E_WARNING, "Unknown error in php_char_to_OLECHAR()");
		}
	}

	return unicode_str;
}


PHPAPI char *php_OLECHAR_to_char(OLECHAR *unicode_str, uint *out_length, int codepage TSRMLS_DC)
{
	char *C_str;
	uint length = 0;

	/* request needed buffersize */
	uint reqSize = WideCharToMultiByte(codepage, codepage == CP_UTF8 ? 0 : WC_COMPOSITECHECK, unicode_str, -1, NULL, 0, NULL, NULL);

	if (reqSize) {
		C_str = (char *) emalloc(sizeof(char) * reqSize);

		/* convert string */
		length = WideCharToMultiByte(codepage, codepage == CP_UTF8 ? 0 : WC_COMPOSITECHECK, unicode_str, -1, C_str, reqSize, NULL, NULL) - 1;
	} else {
		C_str = (char *) emalloc(sizeof(char));
		*C_str = 0;

		php_error(E_WARNING,"Error in php_OLECHAR_to_char()");
	}

	if (out_length) {
		*out_length = length;
	}

	return C_str;
}


static void comval_to_variant(pval *pval_arg, VARIANT *var_arg TSRMLS_DC)
{
	pval **comval_handle;
	comval *obj;
	int type;

	/* fetch the comval structure */
	zend_hash_index_find(Z_OBJPROP_P(pval_arg), 0, (void **) &comval_handle);
	obj = (comval *)zend_list_find(Z_LVAL_P(*comval_handle), &type);
	if (!obj || (type != IS_COM) || !C_ISREFD(obj)) {
		VariantInit(var_arg);
	} else {
		V_VT(var_arg) = VT_DISPATCH;
		V_DISPATCH(var_arg) = C_DISPATCH(obj);
	}
}

#endif /* PHP_WIN32 */
