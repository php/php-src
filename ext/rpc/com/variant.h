/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Harald Radi <h.radi@nme.at>                                  |
   +----------------------------------------------------------------------+
 */


#ifndef VARIANT_H
#define VARIANT_H

#if PHP_WIN32

#define ALLOC_VARIANT(v)	(v) = (variantval *) emalloc(sizeof(variantval));	\
							(v)->var = (VARIANT *) emalloc(sizeof(VARIANT));	\
							(v)->codepage = CP_ACP;								\
							VariantInit((v)->var);

#define FREE_VARIANT(v)		VariantClear((v)->var);								\
							efree((v)->var);									\
							efree((v));

#define ZVAL_VARIANT(z, v, cp)													\
	if (V_VT(v) == VT_DISPATCH) {												\
		comval *obj;															\
		ALLOC_COM(obj);															\
		php_COM_set(obj, &V_DISPATCH(v), TRUE);									\
		rpc_object_from_data_ex(z, com, obj, NULL);								\
	} else {																	\
		php_variant_to_zval((v), (z), cp);										\
		VariantClear(v);														\
	}

#define RETVAL_VARIANT(v, cp)	ZVAL_VARIANT(return_value, v, cp)
#define RETURN_VARIANT(v, cp)	RETVAL_VARIANT(v, cp)							\
								return;

typedef struct variantval_ {
	VARIANT* var;
	long codepage;
} variantval;

void php_variant_init(int module_number TSRMLS_DC);
void php_variant_shutdown(TSRMLS_D);

ZEND_FUNCTION(variant_load);

#endif  /* PHP_WIN32 */

#endif  /* VARIANT_H */
