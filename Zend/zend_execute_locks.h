#ifndef ZEND_EXECUTE_LOCKS_H
#define ZEND_EXECUTE_LOCKS_H

#define PZVAL_LOCK(z) zend_pzval_lock_func(z)

static inline void zend_pzval_lock_func(zval *z)
{
	z->refcount++;
}

#define PZVAL_UNLOCK(z) zend_pzval_unlock_func(z TSRMLS_CC)

static inline void zend_pzval_unlock_func(zval *z TSRMLS_DC)
{
	z->refcount--;
	if (!z->refcount) {
		z->refcount = 1;
		z->is_ref = 0;
		EG(garbage)[EG(garbage_ptr)++] = z;
	}
}

static inline void zend_clean_garbage(TSRMLS_D)
{
	while (EG(garbage_ptr)) {
		zval_ptr_dtor(&EG(garbage)[--EG(garbage_ptr)]);
	}
}

#define SELECTIVE_PZVAL_LOCK(pzv, pzn)		if (!((pzn)->u.EA.type & EXT_TYPE_UNUSED)) { PZVAL_LOCK(pzv); }

#endif /* ZEND_EXECUTE_LOCKS_H */
