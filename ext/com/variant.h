#ifndef VARIANT_H
#define VARIANT_H

#if PHP_WIN32

#define ALLOC_VARIANT(v)	(v) = (VARIANT *) emalloc(sizeof(VARIANT));					\
							VariantInit(v);

#define FREE_VARIANT(v)		VariantClear(v);											\
							efree(v);


#define IS_VARIANT			php_VARIANT_get_le_variant()

#define ZVAL_VARIANT(z, v)		if (V_VT(v) == VT_DISPATCH) { \
									if (V_DISPATCH(v)) { \
										comval *obj; \
										ALLOC_COM(obj); \
										php_COM_set(obj, &V_DISPATCH(v), TRUE TSRMLS_CC); \
										ZVAL_COM((z), obj); \
										efree(v); \
									} else { \
										ZVAL_NULL(z); \
									} \
								} else { \
									php_variant_to_pval((v), (z), codepage TSRMLS_CC); \
									FREE_VARIANT(v); \
								}

#define RETVAL_VARIANT(v)	ZVAL_VARIANT(return_value, (v));
#define RETURN_VARIANT(v)	RETVAL_VARIANT(v)											\
							return;


#endif  /* PHP_WIN32 */

#endif  /* VARIANT_H */
