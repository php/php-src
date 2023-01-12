/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Sascha Schumann <sascha@schumann.cx>                         |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "ext/standard/php_var.h"
#include "php_incomplete_class.h"
#include "zend_portability.h"
#include "zend_exceptions.h"

/* {{{ reference-handling for unserializer: var_* */
#define VAR_ENTRIES_MAX 1018     /* 1024 - offsetof(php_unserialize_data, entries) / sizeof(void*) */
#define VAR_DTOR_ENTRIES_MAX 255 /* 256 - offsetof(var_dtor_entries, data) / sizeof(zval) */
#define VAR_ENTRIES_DBG 0

/* VAR_FLAG used in var_dtor entries to signify an entry on which
 * __wakeup/__unserialize should be called */
#define VAR_WAKEUP_FLAG 1
#define VAR_UNSERIALIZE_FLAG 2

/* Each element is encoded using at least 2 characters. */
#define IS_FAKE_ELEM_COUNT(num_elems, serialized_len) \
	((num_elems) > (serialized_len) / 2)

typedef struct {
	zend_long used_slots;
	void *next;
	zval *data[VAR_ENTRIES_MAX];
} var_entries;

typedef struct {
	zend_long used_slots;
	void *next;
	zval data[VAR_DTOR_ENTRIES_MAX];
} var_dtor_entries;

struct php_unserialize_data {
	var_entries      *last;
	var_dtor_entries *first_dtor;
	var_dtor_entries *last_dtor;
	HashTable        *allowed_classes;
	HashTable        *ref_props;
	zend_long         cur_depth;
	zend_long         max_depth;
	var_entries       entries;
};

PHPAPI php_unserialize_data_t php_var_unserialize_init(void) {
	php_unserialize_data_t d;
	/* fprintf(stderr, "UNSERIALIZE_INIT    == lock: %u, level: %u\n", BG(serialize_lock), BG(unserialize).level); */
	if (BG(serialize_lock) || !BG(unserialize).level) {
		d = emalloc(sizeof(struct php_unserialize_data));
		d->last = &d->entries;
		d->first_dtor = d->last_dtor = NULL;
		d->allowed_classes = NULL;
		d->ref_props = NULL;
		d->cur_depth = 0;
		d->max_depth = BG(unserialize_max_depth);
		d->entries.used_slots = 0;
		d->entries.next = NULL;
		if (!BG(serialize_lock)) {
			BG(unserialize).data = d;
			BG(unserialize).level = 1;
		}
	} else {
		d = BG(unserialize).data;
		++BG(unserialize).level;
	}
	return d;
}

PHPAPI void php_var_unserialize_destroy(php_unserialize_data_t d) {
	/* fprintf(stderr, "UNSERIALIZE_DESTROY == lock: %u, level: %u\n", BG(serialize_lock), BG(unserialize).level); */
	if (BG(serialize_lock) || BG(unserialize).level == 1) {
		var_destroy(&d);
		efree(d);
	}
	if (!BG(serialize_lock) && !--BG(unserialize).level) {
		BG(unserialize).data = NULL;
	}
}

PHPAPI HashTable *php_var_unserialize_get_allowed_classes(php_unserialize_data_t d) {
	return d->allowed_classes;
}
PHPAPI void php_var_unserialize_set_allowed_classes(php_unserialize_data_t d, HashTable *classes) {
	d->allowed_classes = classes;
}

PHPAPI void php_var_unserialize_set_max_depth(php_unserialize_data_t d, zend_long max_depth) {
	d->max_depth = max_depth;
}
PHPAPI zend_long php_var_unserialize_get_max_depth(php_unserialize_data_t d) {
	return d->max_depth;
}

PHPAPI void php_var_unserialize_set_cur_depth(php_unserialize_data_t d, zend_long cur_depth) {
	d->cur_depth = cur_depth;
}
PHPAPI zend_long php_var_unserialize_get_cur_depth(php_unserialize_data_t d) {
	return d->cur_depth;
}

static inline void var_push(php_unserialize_data_t *var_hashx, zval *rval)
{
	var_entries *var_hash = (*var_hashx)->last;
#if VAR_ENTRIES_DBG
	fprintf(stderr, "var_push(" ZEND_LONG_FMT "): %d\n", var_hash?var_hash->used_slots:-1L, Z_TYPE_P(rval));
#endif

	if (var_hash->used_slots == VAR_ENTRIES_MAX) {
		var_hash = emalloc(sizeof(var_entries));
		var_hash->used_slots = 0;
		var_hash->next = 0;

		(*var_hashx)->last->next = var_hash;
		(*var_hashx)->last = var_hash;
	}

	var_hash->data[var_hash->used_slots++] = rval;
}

PHPAPI void var_push_dtor(php_unserialize_data_t *var_hashx, zval *rval)
{
	if (Z_REFCOUNTED_P(rval)) {
		zval *tmp_var = var_tmp_var(var_hashx);
		if (!tmp_var) {
			return;
		}
		ZVAL_COPY(tmp_var, rval);
	}
}

static zend_never_inline void var_push_dtor_value(php_unserialize_data_t *var_hashx, zval *rval)
{
	if (Z_REFCOUNTED_P(rval)) {
		zval *tmp_var = var_tmp_var(var_hashx);
		if (!tmp_var) {
			return;
		}
		ZVAL_COPY_VALUE(tmp_var, rval);
	}
}

static zend_always_inline zval *tmp_var(php_unserialize_data_t *var_hashx, zend_long num)
{
    var_dtor_entries *var_hash;
	zend_long used_slots;

    if (!var_hashx || !*var_hashx || num < 1) {
        return NULL;
    }

    var_hash = (*var_hashx)->last_dtor;
    if (!var_hash || var_hash->used_slots + num > VAR_DTOR_ENTRIES_MAX) {
        var_hash = emalloc(sizeof(var_dtor_entries));
        var_hash->used_slots = 0;
        var_hash->next = 0;

        if (!(*var_hashx)->first_dtor) {
            (*var_hashx)->first_dtor = var_hash;
        } else {
            (*var_hashx)->last_dtor->next = var_hash;
        }

        (*var_hashx)->last_dtor = var_hash;
    }
	for (used_slots = var_hash->used_slots; var_hash->used_slots < used_slots + num; var_hash->used_slots++) {
		ZVAL_UNDEF(&var_hash->data[var_hash->used_slots]);
		Z_EXTRA(var_hash->data[var_hash->used_slots]) = 0;
	}
    return &var_hash->data[used_slots];
}

PHPAPI zval *var_tmp_var(php_unserialize_data_t *var_hashx)
{
    return tmp_var(var_hashx, 1);
}

PHPAPI void var_replace(php_unserialize_data_t *var_hashx, zval *ozval, zval *nzval)
{
	zend_long i;
	var_entries *var_hash = &(*var_hashx)->entries;
#if VAR_ENTRIES_DBG
	fprintf(stderr, "var_replace(" ZEND_LONG_FMT "): %d\n", var_hash?var_hash->used_slots:-1L, Z_TYPE_P(nzval));
#endif

	while (var_hash) {
		for (i = 0; i < var_hash->used_slots; i++) {
			if (var_hash->data[i] == ozval) {
				var_hash->data[i] = nzval;
				/* do not break here */
			}
		}
		var_hash = var_hash->next;
	}
}

static zval *var_access(php_unserialize_data_t *var_hashx, zend_long id)
{
	var_entries *var_hash = &(*var_hashx)->entries;
#if VAR_ENTRIES_DBG
	fprintf(stderr, "var_access(" ZEND_LONG_FMT "): " ZEND_LONG_FMT "\n", var_hash?var_hash->used_slots:-1L, id);
#endif

	while (id >= VAR_ENTRIES_MAX && var_hash && var_hash->used_slots == VAR_ENTRIES_MAX) {
		var_hash = var_hash->next;
		id -= VAR_ENTRIES_MAX;
	}

	if (!var_hash) return NULL;

	if (id < 0 || id >= var_hash->used_slots) return NULL;

	return var_hash->data[id];
}

PHPAPI void var_destroy(php_unserialize_data_t *var_hashx)
{
	void *next;
	zend_long i;
	var_entries *var_hash = (*var_hashx)->entries.next;
	var_dtor_entries *var_dtor_hash = (*var_hashx)->first_dtor;
	bool delayed_call_failed = 0;

#if VAR_ENTRIES_DBG
	fprintf(stderr, "var_destroy( " ZEND_LONG_FMT ")\n", var_hash?var_hash->used_slots:-1L);
#endif

	while (var_hash) {
		next = var_hash->next;
		efree_size(var_hash, sizeof(var_entries));
		var_hash = next;
	}

	while (var_dtor_hash) {
		for (i = 0; i < var_dtor_hash->used_slots; i++) {
			zval *zv = &var_dtor_hash->data[i];
#if VAR_ENTRIES_DBG
			fprintf(stderr, "var_destroy dtor(%p, %ld)\n", &var_dtor_hash->data[i], Z_REFCOUNT_P(&var_dtor_hash->data[i]));
#endif

			if (Z_EXTRA_P(zv) == VAR_WAKEUP_FLAG) {
				/* Perform delayed __wakeup calls */
				if (!delayed_call_failed) {
					zval retval;
					zend_fcall_info fci;
					zend_fcall_info_cache fci_cache;

					ZEND_ASSERT(Z_TYPE_P(zv) == IS_OBJECT);

					fci.size = sizeof(fci);
					fci.object = Z_OBJ_P(zv);
					fci.retval = &retval;
					fci.param_count = 0;
					fci.params = NULL;
					fci.named_params = NULL;
					ZVAL_UNDEF(&fci.function_name);

					fci_cache.function_handler = zend_hash_find_ptr(
						&fci.object->ce->function_table, ZSTR_KNOWN(ZEND_STR_WAKEUP));
					fci_cache.object = fci.object;
					fci_cache.called_scope = fci.object->ce;

					BG(serialize_lock)++;
					if (zend_call_function(&fci, &fci_cache) == FAILURE || Z_ISUNDEF(retval)) {
						delayed_call_failed = 1;
						GC_ADD_FLAGS(Z_OBJ_P(zv), IS_OBJ_DESTRUCTOR_CALLED);
					}
					BG(serialize_lock)--;

					zval_ptr_dtor(&retval);
				} else {
					GC_ADD_FLAGS(Z_OBJ_P(zv), IS_OBJ_DESTRUCTOR_CALLED);
				}
			} else if (Z_EXTRA_P(zv) == VAR_UNSERIALIZE_FLAG) {
				/* Perform delayed __unserialize calls */
				if (!delayed_call_failed) {
					zval param;
					ZVAL_COPY(&param, &var_dtor_hash->data[i + 1]);

					BG(serialize_lock)++;
					zend_call_known_instance_method_with_1_params(
						Z_OBJCE_P(zv)->__unserialize, Z_OBJ_P(zv), NULL, &param);
					if (EG(exception)) {
						delayed_call_failed = 1;
						GC_ADD_FLAGS(Z_OBJ_P(zv), IS_OBJ_DESTRUCTOR_CALLED);
					}
					BG(serialize_lock)--;
					zval_ptr_dtor(&param);
				} else {
					GC_ADD_FLAGS(Z_OBJ_P(zv), IS_OBJ_DESTRUCTOR_CALLED);
				}
			}

			i_zval_ptr_dtor(zv);
		}
		next = var_dtor_hash->next;
		efree_size(var_dtor_hash, sizeof(var_dtor_entries));
		var_dtor_hash = next;
	}

	if ((*var_hashx)->ref_props) {
		zend_hash_destroy((*var_hashx)->ref_props);
		FREE_HASHTABLE((*var_hashx)->ref_props);
	}
}

/* }}} */

static zend_string *unserialize_str(const unsigned char **p, size_t len, size_t maxlen)
{
	size_t i, j;
	zend_string *str = zend_string_safe_alloc(1, len, 0, 0);
	unsigned char *end = *(unsigned char **)p+maxlen;

	if (end < *p) {
		zend_string_efree(str);
		return NULL;
	}

	for (i = 0; i < len; i++) {
		if (*p >= end) {
			zend_string_efree(str);
			return NULL;
		}
		if (**p != '\\') {
			ZSTR_VAL(str)[i] = (char)**p;
		} else {
			unsigned char ch = 0;

			for (j = 0; j < 2; j++) {
				(*p)++;
				if (**p >= '0' && **p <= '9') {
					ch = (ch << 4) + (**p -'0');
				} else if (**p >= 'a' && **p <= 'f') {
					ch = (ch << 4) + (**p -'a'+10);
				} else if (**p >= 'A' && **p <= 'F') {
					ch = (ch << 4) + (**p -'A'+10);
				} else {
					zend_string_efree(str);
					return NULL;
				}
			}
			ZSTR_VAL(str)[i] = (char)ch;
		}
		(*p)++;
	}
	ZSTR_VAL(str)[i] = 0;
	ZSTR_LEN(str) = i;
	return str;
}

static inline int unserialize_allowed_class(
		zend_string *lcname, php_unserialize_data_t *var_hashx)
{
	HashTable *classes = (*var_hashx)->allowed_classes;

	if(classes == NULL) {
		return 1;
	}
	if(!zend_hash_num_elements(classes)) {
		return 0;
	}

	return zend_hash_exists(classes, lcname);
}

#define YYFILL(n) do { } while (0)
#define YYCTYPE unsigned char
#define YYCURSOR cursor
#define YYLIMIT limit
#define YYMARKER marker


/*!re2c
uiv = [0-9]+;
iv = [+-]? [0-9]+;
nv = [+-]? ([0-9]* "." [0-9]+|[0-9]+ "." [0-9]*);
nvexp = (iv | nv) [eE] iv;
any = [\000-\377];
object = [OC];
*/



static inline zend_long parse_iv2(const unsigned char *p, const unsigned char **q)
{
	zend_ulong result = 0;
	zend_ulong neg = 0;
	const unsigned char *start;

	if (*p == '-') {
		neg = 1;
		p++;
	} else if (UNEXPECTED(*p == '+')) {
		p++;
	}

	while (UNEXPECTED(*p == '0')) {
		p++;
	}

	start = p;

	while (*p >= '0' && *p <= '9') {
		result = result * 10 + ((zend_ulong)(*p) - '0');
		p++;
	}

	if (q) {
		*q = p;
	}

	/* number too long or overflow */
	if (UNEXPECTED(p - start > MAX_LENGTH_OF_LONG - 1)
	 || (SIZEOF_ZEND_LONG == 4
	 	&& UNEXPECTED(p - start == MAX_LENGTH_OF_LONG - 1)
	 	&& UNEXPECTED(*start > '2'))
	 || UNEXPECTED(result > ZEND_LONG_MAX + neg)) {
		php_error_docref(NULL, E_WARNING, "Numerical result out of range");
		return (!neg) ? ZEND_LONG_MAX : ZEND_LONG_MIN;
	}

	return (zend_long) ((!neg) ? result : -result);
}

static inline zend_long parse_iv(const unsigned char *p)
{
	return parse_iv2(p, NULL);
}

/* no need to check for length - re2c already did */
static inline size_t parse_uiv(const unsigned char *p)
{
	unsigned char cursor;
	size_t result = 0;

	while (1) {
		cursor = *p;
		if (cursor >= '0' && cursor <= '9') {
			result = result * 10 + (size_t)(cursor - (unsigned char)'0');
		} else {
			break;
		}
		p++;
	}
	return result;
}

#define UNSERIALIZE_PARAMETER zval *rval, const unsigned char **p, const unsigned char *max, php_unserialize_data_t *var_hash
#define UNSERIALIZE_PASSTHRU rval, p, max, var_hash

static int php_var_unserialize_internal(UNSERIALIZE_PARAMETER);

static zend_always_inline int process_nested_array_data(UNSERIALIZE_PARAMETER, HashTable *ht, zend_long elements)
{
	if (var_hash) {
		if ((*var_hash)->max_depth > 0 && (*var_hash)->cur_depth >= (*var_hash)->max_depth) {
			php_error_docref(NULL, E_WARNING,
				"Maximum depth of " ZEND_LONG_FMT " exceeded. "
				"The depth limit can be changed using the max_depth unserialize() option "
				"or the unserialize_max_depth ini setting",
				(*var_hash)->max_depth);
			return 0;
		}
		(*var_hash)->cur_depth++;
	}

	while (elements-- > 0) {
		zval key, *data;
		zend_ulong idx;

		ZVAL_UNDEF(&key);

		if (!php_var_unserialize_internal(&key, p, max, NULL)) {
			zval_ptr_dtor(&key);
			goto failure;
		}

		if (Z_TYPE(key) == IS_LONG) {
			idx = Z_LVAL(key);
numeric_key:
			data = zend_hash_index_lookup(ht, idx);
			if (UNEXPECTED(Z_TYPE_INFO_P(data) != IS_NULL)) {
				var_push_dtor_value(var_hash, data);
				ZVAL_NULL(data);
			}
		} else if (Z_TYPE(key) == IS_STRING) {
			if (UNEXPECTED(ZEND_HANDLE_NUMERIC(Z_STR(key), idx))) {
				zval_ptr_dtor_str(&key);
				goto numeric_key;
			}
			data = zend_hash_lookup(ht, Z_STR(key));
			if (UNEXPECTED(Z_TYPE_INFO_P(data) != IS_NULL)) {
				var_push_dtor_value(var_hash, data);
				ZVAL_NULL(data);
			}
			zval_ptr_dtor_str(&key);
		} else {
			zval_ptr_dtor(&key);
			goto failure;
		}

		if (!php_var_unserialize_internal(data, p, max, var_hash)) {
			goto failure;
		}

		if (elements && *(*p-1) != ';' && *(*p-1) != '}') {
			(*p)--;
			goto failure;
		}
	}

	if (var_hash) {
		(*var_hash)->cur_depth--;
	}
	return 1;

failure:
	if (var_hash) {
		(*var_hash)->cur_depth--;
	}
	return 0;
}

static int is_property_visibility_changed(zend_class_entry *ce, zval *key)
{
	if (zend_hash_num_elements(&ce->properties_info) > 0) {
		zend_property_info *existing_propinfo;
		const char *unmangled_class = NULL;
		const char *unmangled_prop;
		size_t unmangled_prop_len;

		if (UNEXPECTED(zend_unmangle_property_name_ex(Z_STR_P(key), &unmangled_class, &unmangled_prop, &unmangled_prop_len) == FAILURE)) {
			zval_ptr_dtor_str(key);
			return -1;
		}

		if (unmangled_class == NULL) {
			existing_propinfo = zend_hash_find_ptr(&ce->properties_info, Z_STR_P(key));
			if (existing_propinfo != NULL) {
				zval_ptr_dtor_str(key);
				ZVAL_STR_COPY(key, existing_propinfo->name);
				return 1;
			}
		} else {
			if (!strcmp(unmangled_class, "*")
			 || !strcasecmp(unmangled_class, ZSTR_VAL(ce->name))) {
				existing_propinfo = zend_hash_str_find_ptr(
					&ce->properties_info, unmangled_prop, unmangled_prop_len);
				if (existing_propinfo != NULL) {
					zval_ptr_dtor_str(key);
					ZVAL_STR_COPY(key, existing_propinfo->name);
					return 1;
				}
			}
		}
	}
	return 0;
}


static zend_always_inline int process_nested_object_data(UNSERIALIZE_PARAMETER, HashTable *ht, zend_long elements, zend_object *obj)
{
	if (var_hash) {
		if ((*var_hash)->max_depth > 0 && (*var_hash)->cur_depth >= (*var_hash)->max_depth) {
			php_error_docref(NULL, E_WARNING,
				"Maximum depth of " ZEND_LONG_FMT " exceeded. "
				"The depth limit can be changed using the max_depth unserialize() option "
				"or the unserialize_max_depth ini setting",
				(*var_hash)->max_depth);
			return 0;
		}
		(*var_hash)->cur_depth++;
	}

	while (elements-- > 0) {
		zval key, *data;
		zend_property_info *info = NULL;

		ZVAL_UNDEF(&key);

		if (!php_var_unserialize_internal(&key, p, max, NULL)) {
			zval_ptr_dtor(&key);
			goto failure;
		}

		if (EXPECTED(Z_TYPE(key) == IS_STRING)) {
string_key:
			data = zend_hash_find(ht, Z_STR(key));
			if (data != NULL) {
				if (Z_TYPE_P(data) == IS_INDIRECT) {
declared_property:
					/* This is a property with a declaration */
					data = Z_INDIRECT_P(data);
					info = zend_get_typed_property_info_for_slot(obj, data);
					if (info) {
						if (Z_ISREF_P(data)) {
							/* If the value is overwritten, remove old type source from ref. */
							ZEND_REF_DEL_TYPE_SOURCE(Z_REF_P(data), info);
						}

						if ((*var_hash)->ref_props) {
							/* Remove old entry from ref_props table, if it exists. */
							zend_hash_index_del(
								(*var_hash)->ref_props, (uintptr_t) data);
						}
					}
					/* We may override default property value, but they are usually immutable */
					if (Z_REFCOUNTED_P(data)) {
						var_push_dtor_value(var_hash, data);
					}
					ZVAL_NULL(data);
				} else {
					/* Unusual override of dynamic property */
					int ret = is_property_visibility_changed(obj->ce, &key);

					if (ret > 0) {
						goto second_try;
					} else if (!ret) {
						var_push_dtor_value(var_hash, data);
						ZVAL_NULL(data);
					} else if (ret < 0) {
						goto failure;
					}
				}
			} else {
				int ret = is_property_visibility_changed(obj->ce, &key);

				if (EXPECTED(!ret)) {
					if (UNEXPECTED(obj->ce->ce_flags & ZEND_ACC_NO_DYNAMIC_PROPERTIES)) {
						zend_throw_error(NULL, "Cannot create dynamic property %s::$%s",
							ZSTR_VAL(obj->ce->name), zend_get_unmangled_property_name(Z_STR_P(&key)));
						zval_ptr_dtor_str(&key);
						goto failure;
					} else if (!(obj->ce->ce_flags & ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES)) {
						zend_error(E_DEPRECATED, "Creation of dynamic property %s::$%s is deprecated",
							ZSTR_VAL(obj->ce->name), zend_get_unmangled_property_name(Z_STR_P(&key)));
						if (EG(exception)) {
							zval_ptr_dtor_str(&key);
							goto failure;
						}
					}

					data = zend_hash_add_new(ht, Z_STR(key), &EG(uninitialized_zval));
				} else if (ret < 0) {
					goto failure;
				} else {
second_try:
					data = zend_hash_lookup(ht, Z_STR(key));
					if (Z_TYPE_P(data) == IS_INDIRECT) {
						goto declared_property;
					} else if (UNEXPECTED(Z_TYPE_INFO_P(data) != IS_NULL)) {
						var_push_dtor_value(var_hash, data);
						ZVAL_NULL(data);
					}
				}
			}
			zval_ptr_dtor_str(&key);
		} else if (Z_TYPE(key) == IS_LONG) {
			/* object properties should include no integers */
			convert_to_string(&key);
			goto string_key;
		} else {
			zval_ptr_dtor(&key);
			goto failure;
		}

		if (!php_var_unserialize_internal(data, p, max, var_hash)) {
			if (info && Z_ISREF_P(data)) {
				/* Add type source even if we failed to unserialize.
				 * The data is still stored in the property. */
				ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(data), info);
			}
			goto failure;
		}

		if (UNEXPECTED(info)) {
			if (!zend_verify_prop_assignable_by_ref(info, data, /* strict */ 1)) {
				zval_ptr_dtor(data);
				ZVAL_UNDEF(data);
				goto failure;
			}

			if (Z_ISREF_P(data)) {
				ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(data), info);
			} else {
				/* Remember to which property this slot belongs, so we can add a
				 * type source if it is turned into a reference lateron. */
				if (!(*var_hash)->ref_props) {
					(*var_hash)->ref_props = emalloc(sizeof(HashTable));
					zend_hash_init((*var_hash)->ref_props, 8, NULL, NULL, 0);
				}
				zend_hash_index_update_ptr(
					(*var_hash)->ref_props, (uintptr_t) data, info);
			}
		}

		if (elements && *(*p-1) != ';' && *(*p-1) != '}') {
			(*p)--;
			goto failure;
		}
	}

	if (var_hash) {
		(*var_hash)->cur_depth--;
	}
	return 1;

failure:
	if (var_hash) {
		(*var_hash)->cur_depth--;
	}
	return 0;
}

static inline int finish_nested_data(UNSERIALIZE_PARAMETER)
{
	if (*p >= max || **p != '}') {
		return 0;
	}

	(*p)++;
	return 1;
}

static inline int object_custom(UNSERIALIZE_PARAMETER, zend_class_entry *ce)
{
	zend_long datalen;

	datalen = parse_iv2((*p) + 2, p);

	if (max - (*p) < 2) {
		return 0;
	}

	if ((*p)[0] != ':') {
		return 0;
	}

	if ((*p)[1] != '{') {
		(*p) += 1;
		return 0;
	}

	(*p) += 2;

	if (datalen < 0 || (max - (*p)) <= datalen) {
		zend_error(E_WARNING, "Insufficient data for unserializing - " ZEND_LONG_FMT " required, " ZEND_LONG_FMT " present", datalen, (zend_long)(max - (*p)));
		return 0;
	}

	/* Check that '}' is present before calling ce->unserialize() to mitigate issues
	 * with unserialize reading past the end of the passed buffer if the string is not
	 * appropriately terminated (usually NUL terminated, but '}' is also sufficient.) */
	if ((*p)[datalen] != '}') {
		(*p) += datalen;
		return 0;
	}

	if (ce->unserialize == NULL) {
		zend_error(E_WARNING, "Class %s has no unserializer", ZSTR_VAL(ce->name));
		object_init_ex(rval, ce);
	} else if (ce->unserialize(rval, ce, (const unsigned char*)*p, datalen, (zend_unserialize_data *)var_hash) != SUCCESS) {
		return 0;
	}

	(*p) += datalen + 1; /* +1 for '}' */
	return 1;
}

#ifdef PHP_WIN32
# pragma optimize("", off)
#endif
static inline int object_common(UNSERIALIZE_PARAMETER, zend_long elements, bool has_unserialize)
{
	HashTable *ht;
	bool has_wakeup;

	if (has_unserialize) {
		zval ary, *tmp;

		if (elements >= HT_MAX_SIZE) {
			return 0;
		}

		array_init_size(&ary, elements);
		/* Avoid reallocation due to packed -> mixed conversion. */
		zend_hash_real_init_mixed(Z_ARRVAL(ary));
		if (!process_nested_array_data(UNSERIALIZE_PASSTHRU, Z_ARRVAL(ary), elements)) {
			ZVAL_DEREF(rval);
			GC_ADD_FLAGS(Z_OBJ_P(rval), IS_OBJ_DESTRUCTOR_CALLED);
			zval_ptr_dtor(&ary);
			return 0;
		}

		/* Delay __unserialize() call until end of serialization. We use two slots here to
		 * store both the object and the unserialized data array. */
		ZVAL_DEREF(rval);
		tmp = tmp_var(var_hash, 2);
		ZVAL_COPY(tmp, rval);
		Z_EXTRA_P(tmp) = VAR_UNSERIALIZE_FLAG;
		tmp++;
		ZVAL_COPY_VALUE(tmp, &ary);

		return finish_nested_data(UNSERIALIZE_PASSTHRU);
	}

	has_wakeup = Z_OBJCE_P(rval) != PHP_IC_ENTRY
		&& zend_hash_exists(&Z_OBJCE_P(rval)->function_table, ZSTR_KNOWN(ZEND_STR_WAKEUP));

	ht = Z_OBJPROP_P(rval);
	if (elements >= (zend_long)(HT_MAX_SIZE - zend_hash_num_elements(ht))) {
		return 0;
	}

	zend_hash_extend(ht, zend_hash_num_elements(ht) + elements, HT_IS_PACKED(ht));
	if (!process_nested_object_data(UNSERIALIZE_PASSTHRU, ht, elements, Z_OBJ_P(rval))) {
		if (has_wakeup) {
			ZVAL_DEREF(rval);
			GC_ADD_FLAGS(Z_OBJ_P(rval), IS_OBJ_DESTRUCTOR_CALLED);
		}
		return 0;
	}

	ZVAL_DEREF(rval);
	if (has_wakeup) {
		/* Delay __wakeup call until end of serialization */
		zval *wakeup_var = var_tmp_var(var_hash);
		ZVAL_COPY(wakeup_var, rval);
		Z_EXTRA_P(wakeup_var) = VAR_WAKEUP_FLAG;
	}

	return finish_nested_data(UNSERIALIZE_PASSTHRU);
}
#ifdef PHP_WIN32
# pragma optimize("", on)
#endif

PHPAPI int php_var_unserialize(UNSERIALIZE_PARAMETER)
{
	var_entries *orig_var_entries = (*var_hash)->last;
	zend_long orig_used_slots = orig_var_entries ? orig_var_entries->used_slots : 0;
	int result;

	result = php_var_unserialize_internal(UNSERIALIZE_PASSTHRU);

	if (!result) {
		/* If the unserialization failed, mark all elements that have been added to var_hash
		 * as NULL. This will forbid their use by other unserialize() calls in the same
		 * unserialization context. */
		var_entries *e = orig_var_entries;
		zend_long s = orig_used_slots;
		while (e) {
			for (; s < e->used_slots; s++) {
				e->data[s] = NULL;
			}

			e = e->next;
			s = 0;
		}
	}

	return result;
}

static int php_var_unserialize_internal(UNSERIALIZE_PARAMETER)
{
	const unsigned char *cursor, *limit, *marker, *start;
	zval *rval_ref;

	limit = max;
	cursor = *p;

	if (YYCURSOR >= YYLIMIT) {
		return 0;
	}

	if (var_hash && (*p)[0] != 'R') {
		var_push(var_hash, rval);
	}

	start = cursor;

/*!re2c

"R:" uiv ";"		{
	zend_long id;

 	*p = YYCURSOR;
	if (!var_hash) return 0;

	id = parse_uiv(start + 2) - 1;
	if (id == -1 || (rval_ref = var_access(var_hash, id)) == NULL) {
		return 0;
	}

	if (rval_ref == rval || (Z_ISREF_P(rval_ref) && Z_REFVAL_P(rval_ref) == rval)) {
		return 0;
	}

	if (!Z_ISREF_P(rval_ref)) {
		zend_property_info *info = NULL;
		if ((*var_hash)->ref_props) {
			info = zend_hash_index_find_ptr((*var_hash)->ref_props, (uintptr_t) rval_ref);
		}
		ZVAL_NEW_REF(rval_ref, rval_ref);
		if (info) {
			ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(rval_ref), info);
		}
	}

	ZVAL_COPY(rval, rval_ref);

	return 1;
}

"r:" uiv ";"		{
	zend_long id;

 	*p = YYCURSOR;
	if (!var_hash) return 0;

	id = parse_uiv(start + 2) - 1;
	if (id == -1 || (rval_ref = var_access(var_hash, id)) == NULL) {
		return 0;
	}

	if (rval_ref == rval) {
		return 0;
	}

	ZVAL_DEREF(rval_ref);
	if (Z_TYPE_P(rval_ref) != IS_OBJECT) {
		return 0;
	}

	ZVAL_COPY(rval, rval_ref);

	return 1;
}

"N;"	{
	*p = YYCURSOR;
	ZVAL_NULL(rval);
	return 1;
}

"b:0;"	{
	*p = YYCURSOR;
	ZVAL_FALSE(rval);
	return 1;
}

"b:1;"	{
	*p = YYCURSOR;
	ZVAL_TRUE(rval);
	return 1;
}

"i:" iv ";"	{
#if SIZEOF_ZEND_LONG == 4
	int digits = YYCURSOR - start - 3;

	if (start[2] == '-' || start[2] == '+') {
		digits--;
	}

	/* Use double for large zend_long values that were serialized on a 64-bit system */
	if (digits >= MAX_LENGTH_OF_LONG - 1) {
		if (digits == MAX_LENGTH_OF_LONG - 1) {
			int cmp = strncmp((char*)YYCURSOR - MAX_LENGTH_OF_LONG, long_min_digits, MAX_LENGTH_OF_LONG - 1);

			if (!(cmp < 0 || (cmp == 0 && start[2] == '-'))) {
				goto use_double;
			}
		} else {
			goto use_double;
		}
	}
#endif
	*p = YYCURSOR;
	ZVAL_LONG(rval, parse_iv(start + 2));
	return 1;
}

"d:" ("NAN" | "-"? "INF") ";"	{
	*p = YYCURSOR;

	if (!strncmp((char*)start + 2, "NAN", 3)) {
		ZVAL_DOUBLE(rval, ZEND_NAN);
	} else if (!strncmp((char*)start + 2, "INF", 3)) {
		ZVAL_DOUBLE(rval, ZEND_INFINITY);
	} else if (!strncmp((char*)start + 2, "-INF", 4)) {
		ZVAL_DOUBLE(rval, -ZEND_INFINITY);
	} else {
		ZVAL_NULL(rval);
	}

	return 1;
}

"d:" (iv | nv | nvexp) ";"	{
#if SIZEOF_ZEND_LONG == 4
use_double:
#endif
	*p = YYCURSOR;
	ZVAL_DOUBLE(rval, zend_strtod((const char *)start + 2, NULL));
	return 1;
}

"s:" uiv ":" ["] 	{
	size_t len, maxlen;
	char *str;

	len = parse_uiv(start + 2);
	maxlen = max - YYCURSOR;
	if (maxlen < len) {
		*p = start + 2;
		return 0;
	}

	str = (char*)YYCURSOR;

	YYCURSOR += len;

	if (*(YYCURSOR) != '"') {
		*p = YYCURSOR;
		return 0;
	}

	if (*(YYCURSOR + 1) != ';') {
		*p = YYCURSOR + 1;
		return 0;
	}

	YYCURSOR += 2;
	*p = YYCURSOR;

	if (!var_hash) {
		/* Array or object key unserialization */
		ZVAL_STR(rval, zend_string_init_existing_interned(str, len, 0));
	} else {
		ZVAL_STRINGL_FAST(rval, str, len);
	}
	return 1;
}

"S:" uiv ":" ["] 	{
	size_t len, maxlen;
	zend_string *str;

	len = parse_uiv(start + 2);
	maxlen = max - YYCURSOR;
	if (maxlen < len) {
		*p = start + 2;
		return 0;
	}

	if ((str = unserialize_str(&YYCURSOR, len, maxlen)) == NULL) {
		return 0;
	}

	if (*(YYCURSOR) != '"') {
		zend_string_efree(str);
		*p = YYCURSOR;
		return 0;
	}

	if (*(YYCURSOR + 1) != ';') {
		efree(str);
		*p = YYCURSOR + 1;
		return 0;
	}

	YYCURSOR += 2;
	*p = YYCURSOR;

	ZVAL_STR(rval, str);
	return 1;
}

"a:" uiv ":" "{" {
	zend_long elements = parse_iv(start + 2);
	/* use iv() not uiv() in order to check data range */
	*p = YYCURSOR;
    if (!var_hash) return 0;

	if (elements < 0 || elements >= HT_MAX_SIZE || IS_FAKE_ELEM_COUNT(elements, max - YYCURSOR)) {
		return 0;
	}

	if (elements) {
		array_init_size(rval, elements);
		/* we can't convert from packed to hash during unserialization, because
		   reference to some zvals might be kept in var_hash (to support references) */
		zend_hash_real_init_mixed(Z_ARRVAL_P(rval));
	} else {
		ZVAL_EMPTY_ARRAY(rval);
		return finish_nested_data(UNSERIALIZE_PASSTHRU);
	}

	/* The array may contain references to itself, in which case we'll be modifying an
	 * rc>1 array. This is okay, since the array is, ostensibly, only visible to
	 * unserialize (in practice unserialization handlers also see it). Ideally we should
	 * prohibit "r:" references to non-objects, as we only generate them for objects. */
	HT_ALLOW_COW_VIOLATION(Z_ARRVAL_P(rval));

	if (!process_nested_array_data(UNSERIALIZE_PASSTHRU, Z_ARRVAL_P(rval), elements)) {
		return 0;
	}

	return finish_nested_data(UNSERIALIZE_PASSTHRU);
}

object ":" uiv ":" ["]	{
	size_t len, maxlen;
	zend_long elements;
	char *str;
	zend_string *class_name;
	zend_class_entry *ce;
	bool incomplete_class = 0;
	bool custom_object = 0;
	bool has_unserialize = 0;

	zval user_func;
	zval retval;
	zval args[1];

    if (!var_hash) return 0;
	if (*start == 'C') {
		custom_object = 1;
	}

	len = parse_uiv(start + 2);
	maxlen = max - YYCURSOR;
	if (maxlen < len || len == 0) {
		*p = start + 2;
		return 0;
	}

	str = (char*)YYCURSOR;

	YYCURSOR += len;

	if (*(YYCURSOR) != '"') {
		*p = YYCURSOR;
		return 0;
	}
	if (*(YYCURSOR+1) != ':') {
		*p = YYCURSOR+1;
		return 0;
	}

	if (len == 0) {
		/* empty class names are not allowed */
		return 0;
	}

	if (str[0] == '\000') {
		/* runtime definition keys are not allowed */
		return 0;
	}

	if (str[0] == '\\') {
		/* class name can't start from namespace separator */
		return 0;
	}

	class_name = zend_string_init_interned(str, len, 0);

	do {
		zend_string *lc_name;

		if (!(*var_hash)->allowed_classes && ZSTR_HAS_CE_CACHE(class_name)) {
			ce = ZSTR_GET_CE_CACHE(class_name);
			if (ce) {
				break;
			}
		}

		lc_name = zend_string_tolower(class_name);
		if(!unserialize_allowed_class(lc_name, var_hash)) {
			zend_string_release_ex(lc_name, 0);
			if (!zend_is_valid_class_name(class_name)) {
				zend_string_release_ex(class_name, 0);
				return 0;
			}
			incomplete_class = 1;
			ce = PHP_IC_ENTRY;
			break;
		}

		if ((*var_hash)->allowed_classes && ZSTR_HAS_CE_CACHE(class_name)) {
			ce = ZSTR_GET_CE_CACHE(class_name);
			if (ce) {
				zend_string_release_ex(lc_name, 0);
				break;
			}
		}

		ce = zend_hash_find_ptr(EG(class_table), lc_name);
		if (ce
		 && (ce->ce_flags & ZEND_ACC_LINKED)
		 && !(ce->ce_flags & ZEND_ACC_ANON_CLASS)) {
			zend_string_release_ex(lc_name, 0);
			break;
		}

		if (!ZSTR_HAS_CE_CACHE(class_name) && !zend_is_valid_class_name(class_name)) {
			zend_string_release_ex(lc_name, 0);
			zend_string_release_ex(class_name, 0);
			return 0;
		}

		/* Try to find class directly */
		BG(serialize_lock)++;
		ce = zend_lookup_class_ex(class_name, lc_name, 0);
		BG(serialize_lock)--;
		zend_string_release_ex(lc_name, 0);
		if (EG(exception)) {
			zend_string_release_ex(class_name, 0);
			return 0;
		}

		if (ce) {
			break;
		}

		/* Check for unserialize callback */
		if ((PG(unserialize_callback_func) == NULL) || (PG(unserialize_callback_func)[0] == '\0')) {
			incomplete_class = 1;
			ce = PHP_IC_ENTRY;
			break;
		}

		/* Call unserialize callback */
		ZVAL_STRING(&user_func, PG(unserialize_callback_func));

		ZVAL_STR(&args[0], class_name);
		BG(serialize_lock)++;
		call_user_function(NULL, NULL, &user_func, &retval, 1, args);
		BG(serialize_lock)--;
		zval_ptr_dtor(&retval);

		if (EG(exception)) {
			zend_string_release_ex(class_name, 0);
			zval_ptr_dtor(&user_func);
			return 0;
		}

		/* The callback function may have defined the class */
		BG(serialize_lock)++;
		if ((ce = zend_lookup_class(class_name)) == NULL) {
			php_error_docref(NULL, E_WARNING, "Function %s() hasn't defined the class it was called for", Z_STRVAL(user_func));
			incomplete_class = 1;
			ce = PHP_IC_ENTRY;
		}
		BG(serialize_lock)--;

		zval_ptr_dtor(&user_func);
	} while (0);

	*p = YYCURSOR;

	if (ce->ce_flags & ZEND_ACC_NOT_SERIALIZABLE) {
		if (ce->ce_flags & ZEND_ACC_SUBCLASS_SERIALIZABLE) {
			zend_throw_exception_ex(NULL, 0, "Unserialization of '%s' is not allowed, unless you extend the class and provide a unserialisation method",
				ZSTR_VAL(ce->name));
		} else {
			zend_throw_exception_ex(NULL, 0, "Unserialization of '%s' is not allowed",
				ZSTR_VAL(ce->name));
		}
		zend_string_release_ex(class_name, 0);
		return 0;
	}

	if (custom_object) {
		int ret;

		ret = object_custom(UNSERIALIZE_PASSTHRU, ce);

		if (ret && incomplete_class) {
			php_store_class_name(rval, class_name);
		}
		zend_string_release_ex(class_name, 0);
		return ret;
	}

	if (*p >= max - 2) {
		zend_error(E_WARNING, "Bad unserialize data");
		zend_string_release_ex(class_name, 0);
		return 0;
	}

	elements = parse_iv2(*p + 2, p);
	if (elements < 0 || IS_FAKE_ELEM_COUNT(elements, max - YYCURSOR)) {
		zend_string_release_ex(class_name, 0);
		return 0;
	}

	YYCURSOR = *p;

	if (*(YYCURSOR) != ':') {
		return 0;
	}
	if (*(YYCURSOR+1) != '{') {
		*p = YYCURSOR+1;
		return 0;
	}

	*p += 2;

	has_unserialize = !incomplete_class && ce->__unserialize;

	/* If this class implements Serializable, it should not land here but in object_custom().
	 * The passed string obviously doesn't descend from the regular serializer. However, if
	 * there is both Serializable::unserialize() and __unserialize(), then both may be used,
	 * depending on the serialization format. */
	if (ce->serialize != NULL && !has_unserialize) {
		zend_error(E_WARNING, "Erroneous data format for unserializing '%s'", ZSTR_VAL(ce->name));
		zend_string_release_ex(class_name, 0);
		return 0;
	}

	if (object_init_ex(rval, ce) == FAILURE) {
		zend_string_release_ex(class_name, 0);
		return 0;
	}

	if (incomplete_class) {
		php_store_class_name(rval, class_name);
	}
	zend_string_release_ex(class_name, 0);

	return object_common(UNSERIALIZE_PASSTHRU, elements, has_unserialize);
}

"E:" uiv ":" ["] {
	if (!var_hash) return 0;

	size_t len = parse_uiv(start + 2);
	size_t maxlen = max - YYCURSOR;
	if (maxlen < len || len == 0) {
		*p = start + 2;
		return 0;
	}

	char *str = (char *) YYCURSOR;
	YYCURSOR += len;

	if (*(YYCURSOR) != '"') {
		*p = YYCURSOR;
		return 0;
	}
	if (*(YYCURSOR+1) != ';') {
		*p = YYCURSOR+1;
		return 0;
	}

	char *colon_ptr = memchr(str, ':', len);
	if (colon_ptr == NULL) {
		php_error_docref(NULL, E_WARNING, "Invalid enum name '%.*s' (missing colon)", (int) len, str);
		return 0;
	}
	size_t colon_pos = colon_ptr - str;

	zend_string *enum_name = zend_string_init(str, colon_pos, 0);
	zend_string *case_name = zend_string_init(&str[colon_pos + 1], len - colon_pos - 1, 0);

	if (!zend_is_valid_class_name(enum_name)) {
		goto fail;
	}

	zend_class_entry *ce = zend_lookup_class(enum_name);
	if (!ce) {
		php_error_docref(NULL, E_WARNING, "Class '%s' not found", ZSTR_VAL(enum_name));
		goto fail;
	}
	if (!(ce->ce_flags & ZEND_ACC_ENUM)) {
		php_error_docref(NULL, E_WARNING, "Class '%s' is not an enum", ZSTR_VAL(enum_name));
		goto fail;
	}

	YYCURSOR += 2;
	*p = YYCURSOR;

	zend_class_constant *c = zend_hash_find_ptr(CE_CONSTANTS_TABLE(ce), case_name);
	if (!c) {
		php_error_docref(NULL, E_WARNING, "Undefined constant %s::%s", ZSTR_VAL(enum_name), ZSTR_VAL(case_name));
		goto fail;
	}

	if (!(ZEND_CLASS_CONST_FLAGS(c) & ZEND_CLASS_CONST_IS_CASE)) {
		php_error_docref(NULL, E_WARNING, "%s::%s is not an enum case", ZSTR_VAL(enum_name), ZSTR_VAL(case_name));
		goto fail;
	}

	zend_string_release_ex(enum_name, 0);
	zend_string_release_ex(case_name, 0);

	zval *value = &c->value;
	if (Z_TYPE_P(value) == IS_CONSTANT_AST) {
		if (zval_update_constant_ex(value, c->ce) == FAILURE) {
			return 0;
		}
	}
	ZEND_ASSERT(Z_TYPE_P(value) == IS_OBJECT);
	ZVAL_COPY(rval, value);

	return 1;

fail:
	zend_string_release_ex(enum_name, 0);
	zend_string_release_ex(case_name, 0);
	return 0;
}

"}" {
	/* this is the case where we have less data than planned */
	php_error_docref(NULL, E_WARNING, "Unexpected end of serialized data");
	return 0; /* not sure if it should be 0 or 1 here? */
}

any	{ return 0; }

*/

	return 0;
}
