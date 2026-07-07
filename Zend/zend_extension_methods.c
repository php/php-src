#include "zend.h"
#include "zend_API.h"
#include "zend_extension_methods.h"

/* PROTOTYPE NOTE: stored in a true-global for NTS simplicity.
 * TODO(land): move into zend_executor_globals for ZTS; opcache SHM persistence. */
static HashTable *ext_registry = NULL; /* lc target name -> HashTable* of methods */

void zend_extension_methods_startup(void)
{
	ext_registry = pemalloc(sizeof(HashTable), 1);
	zend_hash_init(ext_registry, 8, NULL, NULL, 1);
}

void zend_extension_methods_shutdown(void)
{
	if (ext_registry) {
		HashTable *methods;
		ZEND_HASH_FOREACH_PTR(ext_registry, methods) {
			zend_hash_destroy(methods);
			pefree(methods, 1);
		} ZEND_HASH_FOREACH_END();
		zend_hash_destroy(ext_registry);
		pefree(ext_registry, 1);
		ext_registry = NULL;
	}
}

ZEND_API void zend_extension_methods_register(zend_string *target_lc, zend_class_entry *ext_ce)
{
	HashTable *methods = zend_hash_find_ptr(ext_registry, target_lc);
	zend_string *name;
	zval *zv;

	if (!methods) {
		methods = pemalloc(sizeof(HashTable), 1);
		zend_hash_init(methods, 8, NULL, NULL, 1);
		zend_hash_add_ptr(ext_registry, target_lc, methods);
	}

	ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(&ext_ce->function_table, name, zv) {
		zend_function *fn = Z_PTR_P(zv);
		/* Real methods must always win; conflicts between extensions: first wins.
		 * TODO(land): E_WARNING or fatal on duplicate registration. */
		if (!zend_hash_add_ptr(methods, name, fn)) {
			continue;
		}
		/* Keep out of the polymorphic inline cache for the prototype so we
		 * never serve a stale fbc to a file... (correctness over speed; the
		 * cached path + JIT support is future work). */
		fn->common.fn_flags |= ZEND_ACC_NEVER_CACHE;
	} ZEND_HASH_FOREACH_END();
}

ZEND_API zend_function *zend_extension_methods_get(const zend_class_entry *ce, zend_string *lc_method_name)
{
	if (!ext_registry || zend_hash_num_elements(ext_registry) == 0) {
		return NULL;
	}
	/* Most-derived match first: walk the inheritance chain, then interfaces. */
	for (const zend_class_entry *c = ce; c; c = c->parent) {
		HashTable *methods = zend_hash_find_ptr_lc(ext_registry, c->name);
		if (methods) {
			zend_function *fn = zend_hash_find_ptr(methods, lc_method_name);
			if (fn) {
				return fn;
			}
		}
	}
	for (uint32_t i = 0; i < ce->num_interfaces; i++) {
		HashTable *methods = zend_hash_find_ptr_lc(ext_registry, ce->interfaces[i]->name);
		if (methods) {
			zend_function *fn = zend_hash_find_ptr(methods, lc_method_name);
			if (fn) {
				return fn;
			}
		}
	}
	return NULL;
}
