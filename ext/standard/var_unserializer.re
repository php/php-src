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
  | Author: Sascha Schumann <sascha@schumann.cx>                         |
  +----------------------------------------------------------------------+
*/

#include "php.h"
#include "ext/standard/php_var.h"
#include "php_incomplete_class.h"
#include "zend_portability.h"

/* {{{ reference-handling for unserializer: var_* */
#define VAR_ENTRIES_MAX 1018     /* 1024 - offsetof(php_unserialize_data, entries) / sizeof(void*) */
#define VAR_DTOR_ENTRIES_MAX 255 /* 256 - offsetof(var_dtor_entries, data) / sizeof(zval) */
#define VAR_ENTRIES_DBG 0

/* VAR_FLAG used in var_dtor entries to signify an entry on which
 * __wakeup/__unserialize should be called */
#define VAR_WAKEUP_FLAG 1
#define VAR_UNSERIALIZE_FLAG 2

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
	zend_bool delayed_call_failed = 0;
	zval wakeup_name;
	ZVAL_UNDEF(&wakeup_name);

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
					if (Z_ISUNDEF(wakeup_name)) {
						ZVAL_STRINGL(&wakeup_name, "__wakeup", sizeof("__wakeup") - 1);
					}

					BG(serialize_lock)++;
					if (call_user_function(NULL, zv, &wakeup_name, &retval, 0, 0) == FAILURE || Z_ISUNDEF(retval)) {
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

	zval_ptr_dtor_nogc(&wakeup_name);

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
		zend_string *class_name, php_unserialize_data_t *var_hashx)
{
	HashTable *classes = (*var_hashx)->allowed_classes;
	zend_string *lcname;
	int res;
	ALLOCA_FLAG(use_heap)

	if(classes == NULL) {
		return 1;
	}
	if(!zend_hash_num_elements(classes)) {
		return 0;
	}

	ZSTR_ALLOCA_ALLOC(lcname, ZSTR_LEN(class_name), use_heap);
	zend_str_tolower_copy(ZSTR_VAL(lcname), ZSTR_VAL(class_name), ZSTR_LEN(class_name));
	res = zend_hash_exists(classes, lcname);
	ZSTR_ALLOCA_FREE(lcname, use_heap);
	return res;
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

static int php_var_unserialize_internal(UNSERIALIZE_PARAMETER, int as_key);

static zend_always_inline int process_nested_data(UNSERIALIZE_PARAMETER, HashTable *ht, zend_long elements, zend_object *obj)
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
		zval key, *data, d, *old_data;
		zend_ulong idx;
		zend_property_info *info = NULL;

		ZVAL_UNDEF(&key);

		if (!php_var_unserialize_internal(&key, p, max, NULL, 1)) {
			zval_ptr_dtor(&key);
			goto failure;
		}

		data = NULL;
		ZVAL_UNDEF(&d);

		if (!obj) {
			if (Z_TYPE(key) == IS_LONG) {
				idx = Z_LVAL(key);
numeric_key:
				if (UNEXPECTED((old_data = zend_hash_index_find(ht, idx)) != NULL)) {
					//??? update hash
					var_push_dtor(var_hash, old_data);
					data = zend_hash_index_update(ht, idx, &d);
				} else {
					data = zend_hash_index_add_new(ht, idx, &d);
				}
			} else if (Z_TYPE(key) == IS_STRING) {
				if (UNEXPECTED(ZEND_HANDLE_NUMERIC(Z_STR(key), idx))) {
					goto numeric_key;
				}
				if (UNEXPECTED((old_data = zend_hash_find(ht, Z_STR(key))) != NULL)) {
					//??? update hash
					var_push_dtor(var_hash, old_data);
					data = zend_hash_update(ht, Z_STR(key), &d);
				} else {
					data = zend_hash_add_new(ht, Z_STR(key), &d);
				}
			} else {
				zval_ptr_dtor(&key);
				goto failure;
			}
		} else {
			if (EXPECTED(Z_TYPE(key) == IS_STRING)) {
string_key:
				if (obj && zend_hash_num_elements(&obj->ce->properties_info) > 0) {
					zend_property_info *existing_propinfo;
					zend_string *new_key;
					const char *unmangled_class = NULL;
					const char *unmangled_prop;
					size_t unmangled_prop_len;
					zend_string *unmangled;

					if (UNEXPECTED(zend_unmangle_property_name_ex(Z_STR(key), &unmangled_class, &unmangled_prop, &unmangled_prop_len) == FAILURE)) {
						zval_ptr_dtor(&key);
						goto failure;
					}

					unmangled = zend_string_init(unmangled_prop, unmangled_prop_len, 0);

					existing_propinfo = zend_hash_find_ptr(&obj->ce->properties_info, unmangled);
                    if ((unmangled_class == NULL || !strcmp(unmangled_class, "*") || !strcasecmp(unmangled_class, ZSTR_VAL(obj->ce->name)))
                            && (existing_propinfo != NULL)
							&& (existing_propinfo->flags & ZEND_ACC_PPP_MASK)) {
						if (existing_propinfo->flags & ZEND_ACC_PROTECTED) {
							new_key = zend_mangle_property_name(
								"*", 1, ZSTR_VAL(unmangled), ZSTR_LEN(unmangled), 0);
							zend_string_release_ex(unmangled, 0);
						} else if (existing_propinfo->flags & ZEND_ACC_PRIVATE) {
							if (unmangled_class != NULL && strcmp(unmangled_class, "*") != 0) {
								new_key = zend_mangle_property_name(
									unmangled_class, strlen(unmangled_class),
										ZSTR_VAL(unmangled), ZSTR_LEN(unmangled),
										0);
							} else {
								new_key = zend_mangle_property_name(
									ZSTR_VAL(existing_propinfo->ce->name), ZSTR_LEN(existing_propinfo->ce->name),
									ZSTR_VAL(unmangled), ZSTR_LEN(unmangled),
									0);
							}
							zend_string_release_ex(unmangled, 0);
						} else {
							ZEND_ASSERT(existing_propinfo->flags & ZEND_ACC_PUBLIC);
							new_key = unmangled;
						}
						zval_ptr_dtor_str(&key);
						ZVAL_STR(&key, new_key);
					} else {
						zend_string_release_ex(unmangled, 0);
					}
				}

				if ((old_data = zend_hash_find(ht, Z_STR(key))) != NULL) {
					if (Z_TYPE_P(old_data) == IS_INDIRECT) {
						/* This is a property with a declaration */
						old_data = Z_INDIRECT_P(old_data);
						info = zend_get_typed_property_info_for_slot(obj, old_data);
						if (info) {
							if (Z_ISREF_P(old_data)) {
								/* If the value is overwritten, remove old type source from ref. */
								ZEND_REF_DEL_TYPE_SOURCE(Z_REF_P(old_data), info);
							}

							if ((*var_hash)->ref_props) {
								/* Remove old entry from ref_props table, if it exists. */
								zend_hash_index_del(
									(*var_hash)->ref_props, (zend_uintptr_t) old_data);
							}
						}
						var_push_dtor(var_hash, old_data);
						Z_TRY_DELREF_P(old_data);
						ZVAL_COPY_VALUE(old_data, &d);
						data = old_data;
					} else {
						var_push_dtor(var_hash, old_data);
						data = zend_hash_update_ind(ht, Z_STR(key), &d);
					}
				} else {
					data = zend_hash_add_new(ht, Z_STR(key), &d);
				}
			} else if (Z_TYPE(key) == IS_LONG) {
				/* object properties should include no integers */
				convert_to_string(&key);
				goto string_key;
			} else {
				zval_ptr_dtor(&key);
				goto failure;
			}
		}

		if (!php_var_unserialize_internal(data, p, max, var_hash, 0)) {
			if (info && Z_ISREF_P(data)) {
				/* Add type source even if we failed to unserialize.
				 * The data is still stored in the property. */
				ZEND_REF_ADD_TYPE_SOURCE(Z_REF_P(data), info);
			}
			zval_ptr_dtor(&key);
			goto failure;
		}

		if (UNEXPECTED(info)) {
			if (!zend_verify_prop_assignable_by_ref(info, data, /* strict */ 1)) {
				zval_ptr_dtor(data);
				ZVAL_UNDEF(data);
				zval_ptr_dtor_nogc(&key);
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
					(*var_hash)->ref_props, (zend_uintptr_t) data, info);
			}
		}

		if (BG(unserialize).level > 1) {
			var_push_dtor(var_hash, data);
		}
		zval_ptr_dtor_str(&key);

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

	(*p) += 2;

	if (datalen < 0 || (max - (*p)) <= datalen) {
		zend_error(E_WARNING, "Insufficient data for unserializing - " ZEND_LONG_FMT " required, " ZEND_LONG_FMT " present", datalen, (zend_long)(max - (*p)));
		return 0;
	}

	/* Check that '}' is present before calling ce->unserialize() to mitigate issues
	 * with unserialize reading past the end of the passed buffer if the string is not
	 * appropriately terminated (usually NUL terminated, but '}' is also sufficient.) */
	if ((*p)[datalen] != '}') {
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
static inline int object_common(UNSERIALIZE_PARAMETER, zend_long elements, zend_bool has_unserialize)
{
	HashTable *ht;
	zend_bool has_wakeup;

	if (has_unserialize) {
		zval ary, *tmp;

		if (elements >= HT_MAX_SIZE) {
			return 0;
		}

		array_init_size(&ary, elements);
		/* Avoid reallocation due to packed -> mixed conversion. */
		zend_hash_real_init_mixed(Z_ARRVAL(ary));
		if (!process_nested_data(UNSERIALIZE_PASSTHRU, Z_ARRVAL(ary), elements, NULL)) {
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
		&& zend_hash_str_exists(&Z_OBJCE_P(rval)->function_table, "__wakeup", sizeof("__wakeup")-1);

	ht = Z_OBJPROP_P(rval);
	if (elements >= (zend_long)(HT_MAX_SIZE - zend_hash_num_elements(ht))) {
		return 0;
	}

	zend_hash_extend(ht, zend_hash_num_elements(ht) + elements, HT_FLAGS(ht) & HASH_FLAG_PACKED);
	if (!process_nested_data(UNSERIALIZE_PASSTHRU, ht, elements, Z_OBJ_P(rval))) {
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

	result = php_var_unserialize_internal(UNSERIALIZE_PASSTHRU, 0);

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

static int php_var_unserialize_internal(UNSERIALIZE_PARAMETER, int as_key)
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

	if (Z_ISUNDEF_P(rval_ref) || (Z_ISREF_P(rval_ref) && Z_ISUNDEF_P(Z_REFVAL_P(rval_ref)))) {
		return 0;
	}

	if (!Z_ISREF_P(rval_ref)) {
		zend_property_info *info = NULL;
		if ((*var_hash)->ref_props) {
			info = zend_hash_index_find_ptr((*var_hash)->ref_props, (zend_uintptr_t) rval_ref);
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

	if (as_key) {
		ZVAL_STR(rval, zend_string_init_interned(str, len, 0));
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

	if (elements < 0 || elements >= HT_MAX_SIZE || elements > max - YYCURSOR) {
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

	if (!process_nested_data(UNSERIALIZE_PASSTHRU, Z_ARRVAL_P(rval), elements, NULL)) {
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
	zend_bool incomplete_class = 0;
	zend_bool custom_object = 0;
	zend_bool has_unserialize = 0;

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

	class_name = zend_string_init(str, len, 0);
	if (!zend_is_valid_class_name(class_name)) {
		zend_string_release_ex(class_name, 0);
		return 0;
	}

	do {
		if(!unserialize_allowed_class(class_name, var_hash)) {
			incomplete_class = 1;
			ce = PHP_IC_ENTRY;
			break;
		}

		/* Try to find class directly */
		BG(serialize_lock)++;
		ce = zend_lookup_class(class_name);
		if (ce) {
			BG(serialize_lock)--;
			if (EG(exception)) {
				zend_string_release_ex(class_name, 0);
				return 0;
			}
			break;
		}
		BG(serialize_lock)--;

		if (EG(exception)) {
			zend_string_release_ex(class_name, 0);
			return 0;
		}

		/* Check for unserialize callback */
		if ((PG(unserialize_callback_func) == NULL) || (PG(unserialize_callback_func)[0] == '\0')) {
			incomplete_class = 1;
			ce = PHP_IC_ENTRY;
			break;
		}

		/* Call unserialize callback */
		ZVAL_STRING(&user_func, PG(unserialize_callback_func));

		ZVAL_STR_COPY(&args[0], class_name);
		BG(serialize_lock)++;
		if (call_user_function(NULL, NULL, &user_func, &retval, 1, args) != SUCCESS) {
			BG(serialize_lock)--;
			if (EG(exception)) {
				zend_string_release_ex(class_name, 0);
				zval_ptr_dtor(&user_func);
				zval_ptr_dtor(&args[0]);
				return 0;
			}
			php_error_docref(NULL, E_WARNING, "defined (%s) but not found", Z_STRVAL(user_func));
			incomplete_class = 1;
			ce = PHP_IC_ENTRY;
			zval_ptr_dtor(&user_func);
			zval_ptr_dtor(&args[0]);
			break;
		}
		BG(serialize_lock)--;
		zval_ptr_dtor(&retval);
		if (EG(exception)) {
			zend_string_release_ex(class_name, 0);
			zval_ptr_dtor(&user_func);
			zval_ptr_dtor(&args[0]);
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
		zval_ptr_dtor(&args[0]);
		break;
	} while (1);

	*p = YYCURSOR;

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
	if (elements < 0 || elements > max - YYCURSOR) {
		zend_string_release_ex(class_name, 0);
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

"}" {
	/* this is the case where we have less data than planned */
	php_error_docref(NULL, E_NOTICE, "Unexpected end of serialized data");
	return 0; /* not sure if it should be 0 or 1 here? */
}

any	{ return 0; }

*/

	return 0;
}
