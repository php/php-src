/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_com_dotnet.h"
#include "php_com_dotnet_internal.h"

/* create an automation SafeArray from a PHP array.
 * Only creates a single-dimensional array of variants.
 * The keys of the PHP hash MUST be numeric.  If the array
 * is sparse, then the gaps will be filled with NULL variants */
static void safe_array_from_zval(VARIANT *v, zval *z, int codepage)
{
	SAFEARRAY *sa = NULL;
	SAFEARRAYBOUND bound;
	HashPosition pos;
	int keytype;
	zend_string *strindex;
	zend_ulong intindex = 0;
	VARIANT *va;
	zval *item;

	/* find the largest array index, and assert that all keys are integers */
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(z), &pos);
	for (;; zend_hash_move_forward_ex(Z_ARRVAL_P(z), &pos)) {

		keytype = zend_hash_get_current_key_ex(Z_ARRVAL_P(z), &strindex, &intindex, &pos);

		if (HASH_KEY_IS_STRING == keytype) {
			goto bogus;
		} else if (HASH_KEY_NON_EXISTENT == keytype) {
			break;
		} else if (intindex > UINT_MAX) {
			php_error_docref(NULL, E_WARNING, "COM: max number %u of elements in safe array exceeded", UINT_MAX);
			break;
		}
	}

	/* allocate the structure */
	bound.lLbound = 0;
	bound.cElements = zend_hash_num_elements(Z_ARRVAL_P(z));
	sa = SafeArrayCreate(VT_VARIANT, 1, &bound);

	/* get a lock on the array itself */
	SafeArrayAccessData(sa, &va);
	va = (VARIANT*)sa->pvData;

	/* now fill it in */
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(z), &pos);
	for (;; zend_hash_move_forward_ex(Z_ARRVAL_P(z), &pos)) {
		if (NULL == (item = zend_hash_get_current_data_ex(Z_ARRVAL_P(z), &pos))) {
			break;
		}
		zend_hash_get_current_key_ex(Z_ARRVAL_P(z), &strindex, &intindex, &pos);
		php_com_variant_from_zval(&va[intindex], item, codepage);
	}

	/* Unlock it and stuff it into our variant */
	SafeArrayUnaccessData(sa);
	V_VT(v) = VT_ARRAY|VT_VARIANT;
	V_ARRAY(v) = sa;

	return;

bogus:
	php_error_docref(NULL, E_WARNING, "COM: converting from PHP array to VARIANT array; only arrays with numeric keys are allowed");

	V_VT(v) = VT_NULL;

	if (sa) {
		SafeArrayUnlock(sa);
		SafeArrayDestroy(sa);
	}
}

PHP_COM_DOTNET_API void php_com_variant_from_zval(VARIANT *v, zval *z, int codepage)
{
	php_com_dotnet_object *obj;
	zend_uchar ztype = IS_NULL;

	if (z) {
		ZVAL_DEREF(z);
		ztype = Z_TYPE_P(z);
	}

	switch (ztype) {
		case IS_NULL:
			V_VT(v) = VT_NULL;
			break;

		case IS_FALSE:
			V_VT(v) = VT_BOOL;
			V_BOOL(v) = VARIANT_FALSE;
			break;

		case IS_TRUE:
			V_VT(v) = VT_BOOL;
			V_BOOL(v) = VARIANT_TRUE;
			break;

		case IS_OBJECT:
			if (php_com_is_valid_object(z)) {
				obj = CDNO_FETCH(z);
				if (V_VT(&obj->v) == VT_DISPATCH) {
					/* pass the underlying object */
					V_VT(v) = VT_DISPATCH;
					if (V_DISPATCH(&obj->v)) {
						IDispatch_AddRef(V_DISPATCH(&obj->v));
					}
					V_DISPATCH(v) = V_DISPATCH(&obj->v);
				} else {
					/* pass the variant by reference */
					V_VT(v) = VT_VARIANT | VT_BYREF;
					V_VARIANTREF(v) = &obj->v;
				}
			} else {
				/* export the PHP object using our COM wrapper */
				V_VT(v) = VT_DISPATCH;
				V_DISPATCH(v) = php_com_wrapper_export(z);
			}
			break;

		case IS_ARRAY:
			/* map as safe array */
			safe_array_from_zval(v, z, codepage);
			break;

		case IS_LONG:
#if SIZEOF_ZEND_LONG == 4
			V_VT(v) = VT_I4;
			V_I4(v) = Z_LVAL_P(z);
#else
			V_VT(v) = VT_I8;
			V_I8(v) = Z_LVAL_P(z);
#endif
			break;

		case IS_DOUBLE:
			V_VT(v) = VT_R8;
			V_R8(v) = Z_DVAL_P(z);
			break;

		case IS_STRING:
			V_VT(v) = VT_BSTR;
			V_BSTR(v) = php_com_string_to_bstr(Z_STR_P(z), codepage);
			break;

		case IS_RESOURCE:
		case IS_CONSTANT_AST:
		default:
			V_VT(v) = VT_NULL;
			break;
	}
}

PHP_COM_DOTNET_API int php_com_zval_from_variant(zval *z, VARIANT *v, int codepage)
{
	OLECHAR *olestring = NULL;
	int ret = SUCCESS;

	switch (V_VT(v)) {
		case VT_EMPTY:
		case VT_NULL:
		case VT_VOID:
			ZVAL_NULL(z);
			break;
		case VT_UI1:
			ZVAL_LONG(z, (zend_long)V_UI1(v));
			break;
		case VT_I1:
			ZVAL_LONG(z, (zend_long)V_I1(v));
			break;
		case VT_UI2:
			ZVAL_LONG(z, (zend_long)V_UI2(v));
			break;
		case VT_I2:
			ZVAL_LONG(z, (zend_long)V_I2(v));
			break;
		case VT_UI4:  /* TODO: promote to double if large? */
			ZVAL_LONG(z, (long)V_UI4(v));
			break;
		case VT_I4:
			ZVAL_LONG(z, (long)V_I4(v));
			break;
#if SIZEOF_ZEND_LONG == 8
		case VT_UI8:
			ZVAL_LONG(z, (zend_long)V_UI8(v));
			break;
		case VT_I8:
			ZVAL_LONG(z, (zend_long)V_I8(v));
			break;
#endif
		case VT_INT:
			ZVAL_LONG(z, V_INT(v));
			break;
		case VT_UINT: /* TODO: promote to double if large? */
			ZVAL_LONG(z, (zend_long)V_UINT(v));
			break;
		case VT_R4:
			ZVAL_DOUBLE(z, (double)V_R4(v));
			break;
		case VT_R8:
			ZVAL_DOUBLE(z, V_R8(v));
			break;
		case VT_BOOL:
			ZVAL_BOOL(z, V_BOOL(v) ? 1 : 0);
			break;
		case VT_BSTR:
			olestring = V_BSTR(v);
			if (olestring) {
				zend_string *str = php_com_bstr_to_string(olestring, codepage);
				ZVAL_STR(z, str);
				olestring = NULL;
			}
			break;
		case VT_UNKNOWN:
			if (V_UNKNOWN(v) != NULL) {
				IDispatch *disp;

				if (SUCCEEDED(IUnknown_QueryInterface(V_UNKNOWN(v), &IID_IDispatch, &disp))) {
					php_com_wrap_dispatch(z, disp, codepage);
					IDispatch_Release(disp);
				} else {
					ret = FAILURE;
				}
			}
			break;

		case VT_DISPATCH:
			if (V_DISPATCH(v) != NULL) {
				php_com_wrap_dispatch(z, V_DISPATCH(v), codepage);
			}
			break;

		case VT_VARIANT:
			/* points to another variant */
			return php_com_zval_from_variant(z, V_VARIANTREF(v), codepage);

		default:
			php_com_wrap_variant(z, v, codepage);
	}

	if (olestring) {
		efree(olestring);
	}

	if (ret == FAILURE) {
		php_error_docref(NULL, E_WARNING, "variant->zval: conversion from 0x%x ret=%d", V_VT(v), ret);
	}

	return ret;
}


PHP_COM_DOTNET_API int php_com_copy_variant(VARIANT *dstvar, VARIANT *srcvar)
{
	int ret = SUCCESS;

	switch (V_VT(dstvar) & ~VT_BYREF) {
	case VT_EMPTY:
	case VT_NULL:
	case VT_VOID:
		/* should not be possible */
		break;

	case VT_UI1:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_UI1REF(dstvar) = V_UI1(srcvar);
		} else {
			 V_UI1(dstvar) = V_UI1(srcvar);
		}
		break;

	case VT_I1:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_I1REF(dstvar) = V_I1(srcvar);
		} else {
			V_I1(dstvar) = V_I1(srcvar);
		}
		break;

	case VT_UI2:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_UI2REF(dstvar) = V_UI2(srcvar);
		} else {
			V_UI2(dstvar) = V_UI2(srcvar);
		}
		break;

	case VT_I2:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_I2REF(dstvar) = V_I2(srcvar);
		} else {
			V_I2(dstvar) = V_I2(srcvar);
		}
		break;

	case VT_UI4:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_UI4REF(dstvar) = V_UI4(srcvar);
		} else {
			V_UI4(dstvar) = V_UI4(srcvar);
		}
		break;

	case VT_I4:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_I4REF(dstvar) = V_I4(srcvar);
		} else {
			V_I4(dstvar) = V_I4(srcvar);
		}
		break;
#if SIZEOF_ZEND_LONG == 8
	case VT_UI8:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_UI8REF(dstvar) = V_UI8(srcvar);
		} else {
			V_UI8(dstvar) = V_UI8(srcvar);
		}
		break;

	case VT_I8:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_I8REF(dstvar) = V_I8(srcvar);
		} else {
			V_I8(dstvar) = V_I8(srcvar);
		}
		break;
#endif
	case VT_INT:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_INTREF(dstvar) = V_INT(srcvar);
		} else {
			V_INT(dstvar) = V_INT(srcvar);
		}
		break;

	case VT_UINT:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_UINTREF(dstvar) = V_UINT(srcvar);
		} else {
			V_UINT(dstvar) = V_UINT(srcvar);
		}
		break;

	case VT_R4:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_R4REF(dstvar) = V_R4(srcvar);
		} else {
			V_R4(dstvar) = V_R4(srcvar);
		}
		break;

	case VT_R8:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_R8REF(dstvar) = V_R8(srcvar);
		} else {
			V_R8(dstvar) = V_R8(srcvar);
		}
		break;

	case VT_BOOL:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_BOOLREF(dstvar) = V_BOOL(srcvar);
		} else {
			V_BOOL(dstvar) = V_BOOL(srcvar);
		}
        break;

	case VT_BSTR:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_BSTRREF(dstvar) = V_BSTR(srcvar);
		} else {
			V_BSTR(dstvar) = V_BSTR(srcvar);
        }
		break;

	case VT_UNKNOWN:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_UNKNOWNREF(dstvar) = V_UNKNOWN(srcvar);
		} else {
			V_UNKNOWN(dstvar) = V_UNKNOWN(srcvar);
		}
		break;

	case VT_DISPATCH:
		if (V_VT(dstvar) & VT_BYREF) {
			*V_DISPATCHREF(dstvar) = V_DISPATCH(srcvar);
		} else {
			V_DISPATCH(dstvar) = V_DISPATCH(srcvar);
		}
		break;

	case VT_VARIANT:
		return php_com_copy_variant(V_VARIANTREF(dstvar), srcvar);

	default:
		php_error_docref(NULL, E_WARNING, "variant->__construct: failed to copy from 0x%x to 0x%x", V_VT(dstvar), V_VT(srcvar));
		ret = FAILURE;
	}
	return ret;
}

/* {{{ com_variant_create_instance - ctor for new VARIANT() */
PHP_METHOD(variant, __construct)
{
	/* VARTYPE == unsigned short */ zend_long vt = VT_EMPTY;
	zend_long codepage = CP_ACP;
	zval *object = getThis();
	php_com_dotnet_object *obj;
	zval *zvalue = NULL;
	HRESULT res;

	if (ZEND_NUM_ARGS() == 0) {
		/* just leave things as-is - an empty variant */
		return;
	}

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(),
		"z!|ll", &zvalue, &vt, &codepage)) {
			RETURN_THROWS();
	}

	php_com_initialize();
	obj = CDNO_FETCH(object);

	if (ZEND_NUM_ARGS() == 3) {
		obj->code_page = (int)codepage;
	}

	if (zvalue) {
		php_com_variant_from_zval(&obj->v, zvalue, obj->code_page);
	}

	/* Only perform conversion if variant not already of type passed */
	if ((ZEND_NUM_ARGS() >= 2) && (vt != V_VT(&obj->v))) {

		/* If already an array and VT_ARRAY is passed then:
			- if only VT_ARRAY passed then do not perform a conversion
			- if VT_ARRAY plus other type passed then perform conversion
			  but will probably fail (original behavior)
		*/
		if ((vt & VT_ARRAY) && (V_VT(&obj->v) & VT_ARRAY)) {
			zend_long orig_vt = vt;

			vt &= ~VT_ARRAY;
			if (vt) {
				vt = orig_vt;
			}
		}

		if (vt) {
			res = VariantChangeType(&obj->v, &obj->v, 0, (VARTYPE)vt);

			if (FAILED(res)) {
				char *werr, *msg;

				werr = php_win32_error_to_msg(res);
				spprintf(&msg, 0, "Variant type conversion failed: %s", werr);
				php_win32_error_msg_free(werr);

				php_com_throw_exception(res, msg);
				efree(msg);
			}
		}
	}

	if (V_VT(&obj->v) != VT_DISPATCH && obj->typeinfo) {
		ITypeInfo_Release(obj->typeinfo);
		obj->typeinfo = NULL;
	}
}
/* }}} */

/* {{{ Assigns a new value for a variant object */
PHP_FUNCTION(variant_set)
{
	zval *zobj, *zvalue = NULL;
	php_com_dotnet_object *obj;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(),
			"Oz!", &zobj, php_com_variant_class_entry, &zvalue)) {
		RETURN_THROWS();
	}

	obj = CDNO_FETCH(zobj);

	/* dtor the old value */
	if (obj->typeinfo) {
		ITypeInfo_Release(obj->typeinfo);
		obj->typeinfo = NULL;
	}
	if (obj->sink_dispatch) {
		php_com_object_enable_event_sink(obj, FALSE);
		IDispatch_Release(obj->sink_dispatch);
		obj->sink_dispatch = NULL;
	}

	VariantClear(&obj->v);

	php_com_variant_from_zval(&obj->v, zvalue, obj->code_page);
	/* remember we modified this variant */
	obj->modified = 1;
}
/* }}} */

enum variant_binary_opcode {
	VOP_ADD, VOP_CAT, VOP_SUB, VOP_MUL, VOP_AND, VOP_DIV,
	VOP_EQV, VOP_IDIV, VOP_IMP, VOP_MOD, VOP_OR, VOP_POW,
	VOP_XOR
};

enum variant_unary_opcode {
	VOP_ABS, VOP_FIX, VOP_INT, VOP_NEG, VOP_NOT
};

static void variant_binary_operation(enum variant_binary_opcode op, INTERNAL_FUNCTION_PARAMETERS) /* {{{ */
{
	VARIANT vres;
	VARIANT left_val, right_val;
	VARIANT *vleft = NULL, *vright = NULL;
	zval *zleft = NULL, *zright = NULL;
	php_com_dotnet_object *obj;
	HRESULT result;
	int codepage = CP_ACP;

	VariantInit(&left_val);
	VariantInit(&right_val);
	VariantInit(&vres);

	if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
			ZEND_NUM_ARGS(), "OO", &zleft, php_com_variant_class_entry,
			&zright, php_com_variant_class_entry)) {
		obj = CDNO_FETCH(zleft);
		vleft = &obj->v;
		obj = CDNO_FETCH(zright);
		vright = &obj->v;
	} else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
			ZEND_NUM_ARGS(), "Oz!", &zleft, php_com_variant_class_entry,
			&zright)) {
		obj = CDNO_FETCH(zleft);
		vleft = &obj->v;
		vright = &right_val;
		php_com_variant_from_zval(vright, zright, codepage);
	} else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
			ZEND_NUM_ARGS(), "z!O", &zleft, &zright, php_com_variant_class_entry)) {
		obj = CDNO_FETCH(zright);
		vright = &obj->v;
		vleft = &left_val;
		php_com_variant_from_zval(vleft, zleft, codepage);
	} else if (SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS(),
			"z!z!", &zleft, &zright)) {

		vleft = &left_val;
		php_com_variant_from_zval(vleft, zleft, codepage);

		vright = &right_val;
		php_com_variant_from_zval(vright, zright, codepage);

	} else {
		RETURN_THROWS();
	}

	switch (op) {
		case VOP_ADD:
			result = VarAdd(vleft, vright, &vres);
			break;
		case VOP_CAT:
			result = VarCat(vleft, vright, &vres);
			break;
		case VOP_SUB:
			result = VarSub(vleft, vright, &vres);
			break;
		case VOP_MUL:
			result = VarMul(vleft, vright, &vres);
			break;
		case VOP_AND:
			result = VarAnd(vleft, vright, &vres);
			break;
		case VOP_DIV:
			result = VarDiv(vleft, vright, &vres);
			break;
		case VOP_EQV:
			result = VarEqv(vleft, vright, &vres);
			break;
		case VOP_IDIV:
			result = VarIdiv(vleft, vright, &vres);
			break;
		case VOP_IMP:
			result = VarImp(vleft, vright, &vres);
			break;
		case VOP_MOD:
			result = VarMod(vleft, vright, &vres);
			break;
		case VOP_OR:
			result = VarOr(vleft, vright, &vres);
			break;
		case VOP_POW:
			result = VarPow(vleft, vright, &vres);
			break;
		case VOP_XOR:
			result = VarXor(vleft, vright, &vres);
			break;
		/*Let say it fails as no valid op has been given */
		default:
			result = E_INVALIDARG;
	}

	if (SUCCEEDED(result)) {
		php_com_wrap_variant(return_value, &vres, codepage);
	} else {
		php_com_throw_exception(result, NULL);
	}

	VariantClear(&vres);
	VariantClear(&left_val);
	VariantClear(&right_val);
}
/* }}} */

/* {{{ "Adds" two variant values together and returns the result */
PHP_FUNCTION(variant_add)
{
	variant_binary_operation(VOP_ADD, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ concatenates two variant values together and returns the result */
PHP_FUNCTION(variant_cat)
{
	variant_binary_operation(VOP_CAT, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ subtracts the value of the right variant from the left variant value and returns the result */
PHP_FUNCTION(variant_sub)
{
	variant_binary_operation(VOP_SUB, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ multiplies the values of the two variants and returns the result */
PHP_FUNCTION(variant_mul)
{
	variant_binary_operation(VOP_MUL, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ performs a bitwise AND operation between two variants and returns the result */
PHP_FUNCTION(variant_and)
{
	variant_binary_operation(VOP_AND, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Returns the result from dividing two variants */
PHP_FUNCTION(variant_div)
{
	variant_binary_operation(VOP_DIV, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Performs a bitwise equivalence on two variants */
PHP_FUNCTION(variant_eqv)
{
	variant_binary_operation(VOP_EQV, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Converts variants to integers and then returns the result from dividing them */
PHP_FUNCTION(variant_idiv)
{
	variant_binary_operation(VOP_IDIV, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Performs a bitwise implication on two variants */
PHP_FUNCTION(variant_imp)
{
	variant_binary_operation(VOP_IMP, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Divides two variants and returns only the remainder */
PHP_FUNCTION(variant_mod)
{
	variant_binary_operation(VOP_MOD, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Performs a logical disjunction on two variants */
PHP_FUNCTION(variant_or)
{
	variant_binary_operation(VOP_OR, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Returns the result of performing the power function with two variants */
PHP_FUNCTION(variant_pow)
{
	variant_binary_operation(VOP_POW, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Performs a logical exclusion on two variants */
PHP_FUNCTION(variant_xor)
{
	variant_binary_operation(VOP_XOR, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

static void variant_unary_operation(enum variant_unary_opcode op, INTERNAL_FUNCTION_PARAMETERS) /* {{{ */
{
	VARIANT vres;
	VARIANT left_val;
	VARIANT *vleft = NULL;
	zval *zleft = NULL;
	php_com_dotnet_object *obj;
	HRESULT result;
	int codepage = CP_ACP;

	VariantInit(&left_val);
	VariantInit(&vres);

	if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
			ZEND_NUM_ARGS(), "O", &zleft, php_com_variant_class_entry)) {
		obj = CDNO_FETCH(zleft);
		vleft = &obj->v;
	} else if (SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS(),
			"z!", &zleft)) {
		vleft = &left_val;
		php_com_variant_from_zval(vleft, zleft, codepage);
	} else {
		RETURN_THROWS();
	}

	switch (op) {
		case VOP_ABS:
			result = VarAbs(vleft, &vres);
			break;
		case VOP_FIX:
			result = VarFix(vleft, &vres);
			break;
		case VOP_INT:
			result = VarInt(vleft, &vres);
			break;
		case VOP_NEG:
			result = VarNeg(vleft, &vres);
			break;
		case VOP_NOT:
			result = VarNot(vleft, &vres);
			break;
		default:
			result = E_INVALIDARG;
	}

	if (SUCCEEDED(result)) {
		php_com_wrap_variant(return_value, &vres, codepage);
	} else {
		php_com_throw_exception(result, NULL);
	}

	VariantClear(&vres);
	VariantClear(&left_val);
}
/* }}} */

/* {{{ Returns the absolute value of a variant */
PHP_FUNCTION(variant_abs)
{
	variant_unary_operation(VOP_ABS, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Returns the integer part ? of a variant */
PHP_FUNCTION(variant_fix)
{
	variant_unary_operation(VOP_FIX, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Returns the integer portion of a variant */
PHP_FUNCTION(variant_int)
{
	variant_unary_operation(VOP_INT, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Performs logical negation on a variant */
PHP_FUNCTION(variant_neg)
{
	variant_unary_operation(VOP_NEG, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Performs bitwise not negation on a variant */
PHP_FUNCTION(variant_not)
{
	variant_unary_operation(VOP_NOT, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ Rounds a variant to the specified number of decimal places */
PHP_FUNCTION(variant_round)
{
	VARIANT vres;
	VARIANT left_val;
	VARIANT *vleft = NULL;
	zval *zleft = NULL;
	php_com_dotnet_object *obj;
	int codepage = CP_ACP;
	zend_long decimals = 0;

	VariantInit(&left_val);
	VariantInit(&vres);

	if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
			ZEND_NUM_ARGS(), "Ol", &zleft, php_com_variant_class_entry, &decimals)) {
		obj = CDNO_FETCH(zleft);
		vleft = &obj->v;
	} else if (SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS(),
			"z!l", &zleft, &decimals)) {
		vleft = &left_val;
		php_com_variant_from_zval(vleft, zleft, codepage);
	} else {
		RETURN_THROWS();
	}

	if (SUCCEEDED(VarRound(vleft, (int)decimals, &vres))) {
		php_com_wrap_variant(return_value, &vres, codepage);
	}

	VariantClear(&vres);
	VariantClear(&left_val);
}
/* }}} */

/* {{{ Compares two variants */
PHP_FUNCTION(variant_cmp)
{
	VARIANT left_val, right_val;
	VARIANT *vleft = NULL, *vright = NULL;
	zval *zleft = NULL, *zright = NULL;
	php_com_dotnet_object *obj;
	int codepage = CP_ACP;
	zend_long lcid = LOCALE_SYSTEM_DEFAULT;
	zend_long flags = 0;
	/* it is safe to ignore the warning for this line; see the comments in com_handlers.c */
	STDAPI VarCmp(LPVARIANT pvarLeft, LPVARIANT pvarRight, LCID lcid, DWORD flags);

	VariantInit(&left_val);
	VariantInit(&right_val);

	if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
			ZEND_NUM_ARGS(), "OO|ll", &zleft, php_com_variant_class_entry,
			&zright, php_com_variant_class_entry, &lcid, &flags)) {
		obj = CDNO_FETCH(zleft);
		vleft = &obj->v;
		obj = CDNO_FETCH(zright);
		vright = &obj->v;
	} else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
			ZEND_NUM_ARGS(), "Oz!|ll", &zleft, php_com_variant_class_entry,
			&zright, &lcid, &flags)) {
		obj = CDNO_FETCH(zleft);
		vleft = &obj->v;
		vright = &right_val;
		php_com_variant_from_zval(vright, zright, codepage);
	} else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
			ZEND_NUM_ARGS(), "z!O|ll", &zleft, &zright, php_com_variant_class_entry,
			&lcid, &flags)) {
		obj = CDNO_FETCH(zright);
		vright = &obj->v;
		vleft = &left_val;
		php_com_variant_from_zval(vleft, zleft, codepage);
	} else if (SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS(),
			"z!z!|ll", &zleft, &zright, &lcid, &flags)) {

		vleft = &left_val;
		php_com_variant_from_zval(vleft, zleft, codepage);

		vright = &right_val;
		php_com_variant_from_zval(vright, zright, codepage);

	} else {
		RETURN_THROWS();
	}

	ZVAL_LONG(return_value, VarCmp(vleft, vright, (LCID)lcid, (ULONG)flags));

	VariantClear(&left_val);
	VariantClear(&right_val);
}
/* }}} */

/* {{{ Converts a variant date/time value to unix timestamp */
PHP_FUNCTION(variant_date_to_timestamp)
{
	VARIANT vres;
	zval *zleft = NULL;
	php_com_dotnet_object *obj;

	VariantInit(&vres);

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(),
		"O", &zleft, php_com_variant_class_entry)) {
		RETURN_THROWS();
	}
	obj = CDNO_FETCH(zleft);

	if (SUCCEEDED(VariantChangeType(&vres, &obj->v, 0, VT_DATE))) {
		SYSTEMTIME systime;
		struct tm tmv;

		VariantTimeToSystemTime(V_DATE(&vres), &systime);

		memset(&tmv, 0, sizeof(tmv));
		tmv.tm_year = systime.wYear - 1900;
		tmv.tm_mon = systime.wMonth - 1;
		tmv.tm_mday = systime.wDay;
		tmv.tm_hour = systime.wHour;
		tmv.tm_min = systime.wMinute;
		tmv.tm_sec = systime.wSecond;
		tmv.tm_isdst = -1;

		tzset();
		RETVAL_LONG(mktime(&tmv));
	}

	VariantClear(&vres);
}
/* }}} */

/* {{{ Returns a variant date representation of a unix timestamp */
PHP_FUNCTION(variant_date_from_timestamp)
{
	zend_long timestamp;
	time_t ttstamp;
	SYSTEMTIME systime;
	struct tm *tmv;
	VARIANT res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(), "l",
			&timestamp)) {
		RETURN_THROWS();
	}

	if (timestamp < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	VariantInit(&res);
	tzset();
	ttstamp = timestamp;
	tmv = localtime(&ttstamp);

	/* Invalid after 23:59:59, December 31, 3000, UTC */
	if (!tmv) {
		zend_argument_value_error(1, "must not go past 23:59:59, December 31, 3000, UTC");
		RETURN_THROWS();
	}

	memset(&systime, 0, sizeof(systime));

	systime.wDay = tmv->tm_mday;
	systime.wHour = tmv->tm_hour;
	systime.wMinute = tmv->tm_min;
	systime.wMonth = tmv->tm_mon + 1;
	systime.wSecond = tmv->tm_sec;
	systime.wYear = tmv->tm_year + 1900;

	V_VT(&res) = VT_DATE;
	SystemTimeToVariantTime(&systime, &V_DATE(&res));

	php_com_wrap_variant(return_value, &res, CP_ACP);

	VariantClear(&res);
}
/* }}} */

/* {{{ Returns the VT_XXX type code for a variant */
PHP_FUNCTION(variant_get_type)
{
	zval *zobj;
	php_com_dotnet_object *obj;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(),
		"O", &zobj, php_com_variant_class_entry)) {
		RETURN_THROWS();
	}
	obj = CDNO_FETCH(zobj);

	RETURN_LONG(V_VT(&obj->v));
}
/* }}} */

/* {{{ Convert a variant into another type.  Variant is modified "in-place" */
PHP_FUNCTION(variant_set_type)
{
	zval *zobj;
	php_com_dotnet_object *obj;
	/* VARTYPE == unsigned short */ zend_long vt;
	HRESULT res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(),
		"Ol", &zobj, php_com_variant_class_entry, &vt)) {
		RETURN_THROWS();
	}
	obj = CDNO_FETCH(zobj);

	res = VariantChangeType(&obj->v, &obj->v, 0, (VARTYPE)vt);

	if (SUCCEEDED(res)) {
		if (vt != VT_DISPATCH && obj->typeinfo) {
			ITypeInfo_Release(obj->typeinfo);
			obj->typeinfo = NULL;
		}
	} else {
		char *werr, *msg;

		werr = php_win32_error_to_msg(res);
		spprintf(&msg, 0, "Variant type conversion failed: %s", werr);
		php_win32_error_msg_free(werr);

		php_com_throw_exception(res, msg);
		efree(msg);
	}
}
/* }}} */

/* {{{ Convert a variant into a new variant object of another type */
PHP_FUNCTION(variant_cast)
{
	zval *zobj;
	php_com_dotnet_object *obj;
	/* VARTYPE == unsigned short */ zend_long vt;
	VARIANT vres;
	HRESULT res;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS(),
		"Ol", &zobj, php_com_variant_class_entry, &vt)) {
		RETURN_THROWS();
	}
	obj = CDNO_FETCH(zobj);

	VariantInit(&vres);
	res = VariantChangeType(&vres, &obj->v, 0, (VARTYPE)vt);

	if (SUCCEEDED(res)) {
		php_com_wrap_variant(return_value, &vres, obj->code_page);
	} else {
		char *werr, *msg;

		werr = php_win32_error_to_msg(res);
		spprintf(&msg, 0, "Variant type conversion failed: %s", werr);
		php_win32_error_msg_free(werr);

		php_com_throw_exception(res, msg);
		efree(msg);
	}

	VariantClear(&vres);
}
/* }}} */
