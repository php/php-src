/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_sort.h"
#include "zend_API.h"
#include "zend_ini.h"
#include "zend_alloc.h"
#include "zend_operators.h"
#include "zend_strtod.h"
#include "zend_modules.h"
#include "zend_smart_str.h"
#include <ctype.h>

static HashTable *registered_zend_ini_directives;

#define NO_VALUE_PLAINTEXT		"no value"
#define NO_VALUE_HTML			"<i>no value</i>"

static inline bool zend_is_whitespace(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

/*
 * hash_apply functions
 */
static int zend_remove_ini_entries(zval *el, void *arg) /* {{{ */
{
	zend_ini_entry *ini_entry = (zend_ini_entry *)Z_PTR_P(el);
	int module_number = *(int *)arg;

	return ini_entry->module_number == module_number;
}
/* }}} */

static zend_result zend_restore_ini_entry_cb(zend_ini_entry *ini_entry, int stage) /* {{{ */
{
	zend_result result = FAILURE;

	if (ini_entry->modified) {
		if (ini_entry->on_modify) {
			zend_try {
			/* even if on_modify bails out, we have to continue on with restoring,
				since there can be allocated variables that would be freed on MM shutdown
				and would lead to memory corruption later ini entry is modified again */
				result = ini_entry->on_modify(ini_entry, ini_entry->orig_value, ini_entry->mh_arg1, ini_entry->mh_arg2, ini_entry->mh_arg3, stage);
			} zend_end_try();
		}
		if (stage == ZEND_INI_STAGE_RUNTIME && result == FAILURE) {
			/* runtime failure is OK */
			return FAILURE;
		}
		if (ini_entry->value != ini_entry->orig_value) {
			zend_string_release(ini_entry->value);
		}
		ini_entry->value = ini_entry->orig_value;
		ini_entry->modifiable = ini_entry->orig_modifiable;
		ini_entry->modified = 0;
		ini_entry->orig_value = NULL;
		ini_entry->orig_modifiable = 0;
	}
	return SUCCESS;
}
/* }}} */

static void free_ini_entry(zval *zv) /* {{{ */
{
	zend_ini_entry *entry = (zend_ini_entry*)Z_PTR_P(zv);

	zend_string_release_ex(entry->name, 1);
	if (entry->value) {
		zend_string_release(entry->value);
	}
	if (entry->orig_value) {
		zend_string_release_ex(entry->orig_value, 1);
	}
	free(entry);
}
/* }}} */

/*
 * Startup / shutdown
 */
ZEND_API void zend_ini_startup(void) /* {{{ */
{
	registered_zend_ini_directives = (HashTable *) malloc(sizeof(HashTable));

	EG(ini_directives) = registered_zend_ini_directives;
	EG(modified_ini_directives) = NULL;
	EG(error_reporting_ini_entry) = NULL;
	zend_hash_init(registered_zend_ini_directives, 128, NULL, free_ini_entry, 1);
}
/* }}} */

ZEND_API void zend_ini_shutdown(void) /* {{{ */
{
	zend_ini_dtor(EG(ini_directives));
}
/* }}} */

ZEND_API void zend_ini_dtor(HashTable *ini_directives) /* {{{ */
{
	zend_hash_destroy(ini_directives);
	free(ini_directives);
}
/* }}} */

ZEND_API void zend_ini_global_shutdown(void) /* {{{ */
{
	zend_hash_destroy(registered_zend_ini_directives);
	free(registered_zend_ini_directives);
}
/* }}} */

ZEND_API void zend_ini_deactivate(void) /* {{{ */
{
	if (EG(modified_ini_directives)) {
		zend_ini_entry *ini_entry;

		ZEND_HASH_MAP_FOREACH_PTR(EG(modified_ini_directives), ini_entry) {
			zend_restore_ini_entry_cb(ini_entry, ZEND_INI_STAGE_DEACTIVATE);
		} ZEND_HASH_FOREACH_END();
		zend_hash_destroy(EG(modified_ini_directives));
		FREE_HASHTABLE(EG(modified_ini_directives));
		EG(modified_ini_directives) = NULL;
	}
}
/* }}} */

#ifdef ZTS
static void copy_ini_entry(zval *zv) /* {{{ */
{
	zend_ini_entry *old_entry = (zend_ini_entry*)Z_PTR_P(zv);
	zend_ini_entry *new_entry = pemalloc(sizeof(zend_ini_entry), 1);

	Z_PTR_P(zv) = new_entry;
	memcpy(new_entry, old_entry, sizeof(zend_ini_entry));
	if (old_entry->name) {
		new_entry->name = zend_string_dup(old_entry->name, 1);
	}
	if (old_entry->value) {
		new_entry->value = zend_string_dup(old_entry->value, 1);
	}
	if (old_entry->orig_value) {
		new_entry->orig_value = zend_string_dup(old_entry->orig_value, 1);
	}
}
/* }}} */

ZEND_API void zend_copy_ini_directives(void) /* {{{ */
{
	EG(modified_ini_directives) = NULL;
	EG(error_reporting_ini_entry) = NULL;
	EG(ini_directives) = (HashTable *) malloc(sizeof(HashTable));
	zend_hash_init(EG(ini_directives), registered_zend_ini_directives->nNumOfElements, NULL, free_ini_entry, 1);
	zend_hash_copy(EG(ini_directives), registered_zend_ini_directives, copy_ini_entry);
}
/* }}} */
#endif

static int ini_key_compare(Bucket *f, Bucket *s) /* {{{ */
{
	if (!f->key && !s->key) { /* both numeric */
		if (f->h > s->h) {
			return -1;
		} else if (f->h < s->h) {
			return 1;
		}
		return 0;
	} else if (!f->key) { /* f is numeric, s is not */
		return -1;
	} else if (!s->key) { /* s is numeric, f is not */
		return 1;
	} else { /* both strings */
		return zend_binary_strcasecmp(ZSTR_VAL(f->key), ZSTR_LEN(f->key), ZSTR_VAL(s->key), ZSTR_LEN(s->key));
	}
}
/* }}} */

ZEND_API void zend_ini_sort_entries(void) /* {{{ */
{
	zend_hash_sort(EG(ini_directives), ini_key_compare, 0);
}
/* }}} */

/*
 * Registration / unregistration
 */
ZEND_API zend_result zend_register_ini_entries_ex(const zend_ini_entry_def *ini_entry, int module_number, int module_type) /* {{{ */
{
	zend_ini_entry *p;
	zval *default_value;
	HashTable *directives = registered_zend_ini_directives;

#ifdef ZTS
	/* if we are called during the request, eg: from dl(),
	 * then we should not touch the global directives table,
	 * and should update the per-(request|thread) version instead.
	 * This solves two problems: one is that ini entries for dl()'d
	 * extensions will now work, and the second is that updating the
	 * global hash here from dl() is not mutex protected and can
	 * lead to death.
	 */
	if (directives != EG(ini_directives)) {
		ZEND_ASSERT(module_type == MODULE_TEMPORARY);
		directives = EG(ini_directives);
	} else {
		ZEND_ASSERT(module_type == MODULE_PERSISTENT);
	}
#endif

	while (ini_entry->name) {
		p = pemalloc(sizeof(zend_ini_entry), 1);
		p->name = zend_string_init_interned(ini_entry->name, ini_entry->name_length, 1);
		p->on_modify = ini_entry->on_modify;
		p->mh_arg1 = ini_entry->mh_arg1;
		p->mh_arg2 = ini_entry->mh_arg2;
		p->mh_arg3 = ini_entry->mh_arg3;
		p->value = NULL;
		p->orig_value = NULL;
		p->displayer = ini_entry->displayer;
		p->modifiable = ini_entry->modifiable;

		p->orig_modifiable = 0;
		p->modified = 0;
		p->module_number = module_number;

		if (zend_hash_add_ptr(directives, p->name, (void*)p) == NULL) {
			if (p->name) {
				zend_string_release_ex(p->name, 1);
			}
			zend_unregister_ini_entries_ex(module_number, module_type);
			return FAILURE;
		}
		if (((default_value = zend_get_configuration_directive(p->name)) != NULL) &&
		    (!p->on_modify || p->on_modify(p, Z_STR_P(default_value), p->mh_arg1, p->mh_arg2, p->mh_arg3, ZEND_INI_STAGE_STARTUP) == SUCCESS)) {

			p->value = zend_new_interned_string(zend_string_copy(Z_STR_P(default_value)));
		} else {
			p->value = ini_entry->value ?
				zend_string_init_interned(ini_entry->value, ini_entry->value_length, 1) : NULL;

			if (p->on_modify) {
				p->on_modify(p, p->value, p->mh_arg1, p->mh_arg2, p->mh_arg3, ZEND_INI_STAGE_STARTUP);
			}
		}
		ini_entry++;
	}
	return SUCCESS;
}
/* }}} */

ZEND_API zend_result zend_register_ini_entries(const zend_ini_entry_def *ini_entry, int module_number) /* {{{ */
{
	zend_module_entry *module;

	/* Module is likely to be the last one in the list */
	ZEND_HASH_REVERSE_FOREACH_PTR(&module_registry, module) {
		if (module->module_number == module_number) {
			return zend_register_ini_entries_ex(ini_entry, module_number, module->type);
		}
	} ZEND_HASH_FOREACH_END();

	return FAILURE;
}
/* }}} */

ZEND_API void zend_unregister_ini_entries_ex(int module_number, int module_type) /* {{{ */
{
	static HashTable *ini_directives;

	if (module_type == MODULE_TEMPORARY) {
		ini_directives = EG(ini_directives);
	} else {
		ini_directives = registered_zend_ini_directives;
	}

	zend_hash_apply_with_argument(ini_directives, zend_remove_ini_entries, (void *) &module_number);
}
/* }}} */

ZEND_API void zend_unregister_ini_entries(int module_number) /* {{{ */
{
	zend_module_entry *module;

	/* Module is likely to be the last one in the list */
	ZEND_HASH_REVERSE_FOREACH_PTR(&module_registry, module) {
		if (module->module_number == module_number) {
			zend_unregister_ini_entries_ex(module_number, module->type);
			return;
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

#ifdef ZTS
ZEND_API void zend_ini_refresh_caches(int stage) /* {{{ */
{
	zend_ini_entry *p;

	ZEND_HASH_MAP_FOREACH_PTR(EG(ini_directives), p) {
		if (p->on_modify) {
			p->on_modify(p, p->value, p->mh_arg1, p->mh_arg2, p->mh_arg3, stage);
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */
#endif

ZEND_API zend_result zend_alter_ini_entry(zend_string *name, zend_string *new_value, int modify_type, int stage) /* {{{ */
{

	return zend_alter_ini_entry_ex(name, new_value, modify_type, stage, 0);
}
/* }}} */

ZEND_API zend_result zend_alter_ini_entry_chars(zend_string *name, const char *value, size_t value_length, int modify_type, int stage) /* {{{ */
{
	zend_result ret;
	zend_string *new_value;

	new_value = zend_string_init(value, value_length, !(stage & ZEND_INI_STAGE_IN_REQUEST));
	ret = zend_alter_ini_entry_ex(name, new_value, modify_type, stage, 0);
	zend_string_release(new_value);
	return ret;
}
/* }}} */

ZEND_API zend_result zend_alter_ini_entry_chars_ex(zend_string *name, const char *value, size_t value_length, int modify_type, int stage, int force_change) /* {{{ */
{
	zend_result ret;
	zend_string *new_value;

	new_value = zend_string_init(value, value_length, !(stage & ZEND_INI_STAGE_IN_REQUEST));
	ret = zend_alter_ini_entry_ex(name, new_value, modify_type, stage, force_change);
	zend_string_release(new_value);
	return ret;
}
/* }}} */

ZEND_API zend_result zend_alter_ini_entry_ex(zend_string *name, zend_string *new_value, int modify_type, int stage, bool force_change) /* {{{ */
{
	zend_ini_entry *ini_entry;
	zend_string *duplicate;
	uint8_t modifiable;
	bool modified;

	if ((ini_entry = zend_hash_find_ptr(EG(ini_directives), name)) == NULL) {
		return FAILURE;
	}

	modifiable = ini_entry->modifiable;
	modified = ini_entry->modified;

	if (stage == ZEND_INI_STAGE_ACTIVATE && modify_type == ZEND_INI_SYSTEM) {
		ini_entry->modifiable = ZEND_INI_SYSTEM;
	}

	if (!force_change) {
		if (!(ini_entry->modifiable & modify_type)) {
			return FAILURE;
		}
	}

	if (!EG(modified_ini_directives)) {
		ALLOC_HASHTABLE(EG(modified_ini_directives));
		zend_hash_init(EG(modified_ini_directives), 8, NULL, NULL, 0);
	}
	if (!modified) {
		ini_entry->orig_value = ini_entry->value;
		ini_entry->orig_modifiable = modifiable;
		ini_entry->modified = 1;
		zend_hash_add_ptr(EG(modified_ini_directives), ini_entry->name, ini_entry);
	}

	duplicate = zend_string_copy(new_value);

	if (!ini_entry->on_modify
		|| ini_entry->on_modify(ini_entry, duplicate, ini_entry->mh_arg1, ini_entry->mh_arg2, ini_entry->mh_arg3, stage) == SUCCESS) {
		if (modified && ini_entry->orig_value != ini_entry->value) { /* we already changed the value, free the changed value */
			zend_string_release(ini_entry->value);
		}
		ini_entry->value = duplicate;
	} else {
		zend_string_release(duplicate);
		return FAILURE;
	}

	return SUCCESS;
}
/* }}} */

ZEND_API zend_result zend_restore_ini_entry(zend_string *name, int stage) /* {{{ */
{
	zend_ini_entry *ini_entry;

	if ((ini_entry = zend_hash_find_ptr(EG(ini_directives), name)) == NULL ||
		(stage == ZEND_INI_STAGE_RUNTIME && (ini_entry->modifiable & ZEND_INI_USER) == 0)) {
		return FAILURE;
	}

	if (EG(modified_ini_directives)) {
		if (zend_restore_ini_entry_cb(ini_entry, stage) == 0) {
			zend_hash_del(EG(modified_ini_directives), name);
		} else {
			return FAILURE;
		}
	}

	return SUCCESS;
}
/* }}} */

ZEND_API zend_result zend_ini_register_displayer(const char *name, uint32_t name_length, void (*displayer)(zend_ini_entry *ini_entry, int type)) /* {{{ */
{
	zend_ini_entry *ini_entry;

	ini_entry = zend_hash_str_find_ptr(registered_zend_ini_directives, name, name_length);
	if (ini_entry == NULL) {
		return FAILURE;
	}

	ini_entry->displayer = displayer;
	return SUCCESS;
}
/* }}} */

/*
 * Data retrieval
 */

ZEND_API zend_long zend_ini_long(const char *name, size_t name_length, int orig) /* {{{ */
{
	zend_ini_entry *ini_entry;

	ini_entry = zend_hash_str_find_ptr(EG(ini_directives), name, name_length);
	if (ini_entry) {
		if (orig && ini_entry->modified) {
			return (ini_entry->orig_value ? ZEND_STRTOL(ZSTR_VAL(ini_entry->orig_value), NULL, 0) : 0);
		} else {
			return (ini_entry->value      ? ZEND_STRTOL(ZSTR_VAL(ini_entry->value), NULL, 0)      : 0);
		}
	}

	return 0;
}
/* }}} */

ZEND_API double zend_ini_double(const char *name, size_t name_length, int orig) /* {{{ */
{
	zend_ini_entry *ini_entry;

	ini_entry = zend_hash_str_find_ptr(EG(ini_directives), name, name_length);
	if (ini_entry) {
		if (orig && ini_entry->modified) {
			return (double) (ini_entry->orig_value ? zend_strtod(ZSTR_VAL(ini_entry->orig_value), NULL) : 0.0);
		} else {
			return (double) (ini_entry->value      ? zend_strtod(ZSTR_VAL(ini_entry->value), NULL)      : 0.0);
		}
	}

	return 0.0;
}
/* }}} */

ZEND_API char *zend_ini_string_ex(const char *name, size_t name_length, int orig, bool *exists) /* {{{ */
{
	zend_ini_entry *ini_entry;

	ini_entry = zend_hash_str_find_ptr(EG(ini_directives), name, name_length);
	if (ini_entry) {
		if (exists) {
			*exists = 1;
		}

		if (orig && ini_entry->modified) {
			return ini_entry->orig_value ? ZSTR_VAL(ini_entry->orig_value) : NULL;
		} else {
			return ini_entry->value ? ZSTR_VAL(ini_entry->value) : NULL;
		}
	} else {
		if (exists) {
			*exists = 0;
		}
		return NULL;
	}
}
/* }}} */

ZEND_API char *zend_ini_string(const char *name, size_t name_length, int orig) /* {{{ */
{
	bool exists = 1;
	char *return_value;

	return_value = zend_ini_string_ex(name, name_length, orig, &exists);
	if (!exists) {
		return NULL;
	} else if (!return_value) {
		return_value = "";
	}
	return return_value;
}
/* }}} */


ZEND_API zend_string *zend_ini_str_ex(const char *name, size_t name_length, bool orig, bool *exists) /* {{{ */
{
	zend_ini_entry *ini_entry;

	ini_entry = zend_hash_str_find_ptr(EG(ini_directives), name, name_length);
	if (ini_entry) {
		if (exists) {
			*exists = 1;
		}

		if (orig && ini_entry->modified) {
			return ini_entry->orig_value ? ini_entry->orig_value : NULL;
		} else {
			return ini_entry->value ? ini_entry->value : NULL;
		}
	} else {
		if (exists) {
			*exists = 0;
		}
		return NULL;
	}
}
/* }}} */

ZEND_API zend_string *zend_ini_str(const char *name, size_t name_length, bool orig) /* {{{ */
{
	bool exists = 1;
	zend_string *return_value;

	return_value = zend_ini_str_ex(name, name_length, orig, &exists);
	if (!exists) {
		return NULL;
	} else if (!return_value) {
		return_value = ZSTR_EMPTY_ALLOC();
	}
	return return_value;
}
/* }}} */

ZEND_API zend_string *zend_ini_get_value(zend_string *name) /* {{{ */
{
	zend_ini_entry *ini_entry;

	ini_entry = zend_hash_find_ptr(EG(ini_directives), name);
	if (ini_entry) {
		return ini_entry->value ? ini_entry->value : ZSTR_EMPTY_ALLOC();
	} else {
		return NULL;
	}
}
/* }}} */

ZEND_API bool zend_ini_parse_bool(zend_string *str)
{
	if (zend_string_equals_literal_ci(str, "true")
			|| zend_string_equals_literal_ci(str, "yes")
			|| zend_string_equals_literal_ci(str, "on")
	) {
		return 1;
	} else {
		return atoi(ZSTR_VAL(str)) != 0;
	}
}

typedef enum {
	ZEND_INI_PARSE_QUANTITY_SIGNED,
	ZEND_INI_PARSE_QUANTITY_UNSIGNED,
} zend_ini_parse_quantity_signed_result_t;

static const char *zend_ini_consume_quantity_prefix(const char *const digits, const char *const str_end) {
	const char *digits_consumed = digits;
	/* Ignore leading whitespace. */
	while (digits_consumed < str_end && zend_is_whitespace(*digits_consumed)) {++digits_consumed;}
	if (digits_consumed[0] == '+' || digits_consumed[0] == '-') {
		++digits_consumed;
	}

	if (digits_consumed[0] == '0' && !isdigit(digits_consumed[1])) {
		/* Value is just 0 */
		if ((digits_consumed+1) == str_end) {
			return digits;
		}

		switch (digits_consumed[1]) {
			case 'x':
			case 'X':
			case 'o':
			case 'O':
			case 'b':
			case 'B':
				digits_consumed += 2;
				break;
		}
	}
	return digits_consumed;
}

static zend_ulong zend_ini_parse_quantity_internal(zend_string *value, zend_ini_parse_quantity_signed_result_t signed_result, zend_string **errstr) /* {{{ */
{
	char *digits_end = NULL;
	char *str = ZSTR_VAL(value);
	char *str_end = &str[ZSTR_LEN(value)];
	char *digits = str;
	bool overflow = false;
	zend_ulong factor;
	smart_str invalid = {0};
	smart_str interpreted = {0};
	smart_str chr = {0};

	/* Ignore leading whitespace. ZEND_STRTOL() also skips leading whitespaces,
	 * but we need the position of the first non-whitespace later. */
	while (digits < str_end && zend_is_whitespace(*digits)) {++digits;}

	/* Ignore trailing whitespace */
	while (digits < str_end && zend_is_whitespace(*(str_end-1))) {--str_end;}

	if (digits == str_end) {
		*errstr = NULL;
		return 0;
	}

	bool is_negative = false;
	if (digits[0] == '+') {
		++digits;
	} else if (digits[0] == '-') {
		is_negative = true;
		++digits;
	}

	/* if there is no digit after +/- */
	if (!isdigit(digits[0])) {
		/* Escape the string to avoid null bytes and to make non-printable chars
		 * visible */
		smart_str_append_escaped(&invalid, ZSTR_VAL(value), ZSTR_LEN(value));
		smart_str_0(&invalid);

		*errstr = zend_strpprintf(0, "Invalid quantity \"%s\": no valid leading digits, interpreting as \"0\" for backwards compatibility",
						ZSTR_VAL(invalid.s));

		smart_str_free(&invalid);
		return 0;
	}

	int base = 0;
	if (digits[0] == '0' && !isdigit(digits[1])) {
		/* Value is just 0 */
		if ((digits+1) == str_end) {
			*errstr = NULL;
			return 0;
		}

		switch (digits[1]) {
			/* Multiplier suffixes */
			case 'g':
			case 'G':
			case 'm':
			case 'M':
			case 'k':
			case 'K':
				goto evaluation;
			case 'x':
			case 'X':
				base = 16;
				break;
			case 'o':
			case 'O':
				base = 8;
				break;
			case 'b':
			case 'B':
				base = 2;
				break;
			default:
				*errstr = zend_strpprintf(0, "Invalid prefix \"0%c\", interpreting as \"0\" for backwards compatibility",
					digits[1]);
				return 0;
        }
        digits += 2;
		if (UNEXPECTED(digits == str_end)) {
			/* Escape the string to avoid null bytes and to make non-printable chars
			 * visible */
			smart_str_append_escaped(&invalid, ZSTR_VAL(value), ZSTR_LEN(value));
			smart_str_0(&invalid);

			*errstr = zend_strpprintf(0, "Invalid quantity \"%s\": no digits after base prefix, interpreting as \"0\" for backwards compatibility",
							ZSTR_VAL(invalid.s));

			smart_str_free(&invalid);
			return 0;
		}
		if (UNEXPECTED(digits != zend_ini_consume_quantity_prefix(digits, str_end))) {
			/* Escape the string to avoid null bytes and to make non-printable chars
			 * visible */
			smart_str_append_escaped(&invalid, ZSTR_VAL(value), ZSTR_LEN(value));
			smart_str_0(&invalid);

			*errstr = zend_strpprintf(0, "Invalid quantity \"%s\": no digits after base prefix, interpreting as \"0\" for backwards compatibility",
							ZSTR_VAL(invalid.s));

			smart_str_free(&invalid);
			return 0;
		}
	}
	evaluation:

	errno = 0;
	zend_ulong retval = ZEND_STRTOUL(digits, &digits_end, base);

	if (errno == ERANGE) {
		overflow = true;
	} else if (signed_result == ZEND_INI_PARSE_QUANTITY_UNSIGNED) {
		if (is_negative) {
			/* Ignore "-1" as it is commonly used as max value, for instance in memory_limit=-1. */
			if (retval == 1 && digits_end == str_end) {
				retval = -1;
			} else {
				overflow = true;
			}
		}
	} else if (signed_result == ZEND_INI_PARSE_QUANTITY_SIGNED) {
		/* Handle PHP_INT_MIN case */
		if (is_negative && retval == ((zend_ulong)ZEND_LONG_MAX +1)) {
			retval = 0u - retval;
		} else if ((zend_long) retval < 0) {
			overflow = true;
		} else if (is_negative) {
			retval = 0u - retval;
		}
	}

	if (UNEXPECTED(digits_end == digits)) {
		/* No leading digits */

		/* Escape the string to avoid null bytes and to make non-printable chars
		 * visible */
		smart_str_append_escaped(&invalid, ZSTR_VAL(value), ZSTR_LEN(value));
		smart_str_0(&invalid);

		*errstr = zend_strpprintf(0, "Invalid quantity \"%s\": no valid leading digits, interpreting as \"0\" for backwards compatibility",
						ZSTR_VAL(invalid.s));

		smart_str_free(&invalid);
		return 0;
	}

	/* Allow for whitespace between integer portion and any suffix character */
	while (digits_end < str_end && zend_is_whitespace(*digits_end)) ++digits_end;

	/* No exponent suffix. */
	if (digits_end == str_end) {
		goto end;
	}

	switch (*(str_end-1)) {
		case 'g':
		case 'G':
			factor = 1<<30;
			break;
		case 'm':
		case 'M':
			factor = 1<<20;
			break;
		case 'k':
		case 'K':
			factor = 1<<10;
			break;
		default:
			/* Unknown suffix */
			smart_str_append_escaped(&invalid, ZSTR_VAL(value), ZSTR_LEN(value));
			smart_str_0(&invalid);
			smart_str_append_escaped(&interpreted, str, digits_end - str);
			smart_str_0(&interpreted);
			smart_str_append_escaped(&chr, str_end-1, 1);
			smart_str_0(&chr);

			*errstr = zend_strpprintf(0, "Invalid quantity \"%s\": unknown multiplier \"%s\", interpreting as \"%s\" for backwards compatibility",
						ZSTR_VAL(invalid.s), ZSTR_VAL(chr.s), ZSTR_VAL(interpreted.s));

			smart_str_free(&invalid);
			smart_str_free(&interpreted);
			smart_str_free(&chr);

			return retval;
	}

	if (!overflow) {
		if (signed_result == ZEND_INI_PARSE_QUANTITY_SIGNED) {
			zend_long sretval = (zend_long)retval;
			if (sretval > 0) {
				overflow = (zend_long)retval > ZEND_LONG_MAX / (zend_long)factor;
			} else {
				overflow = (zend_long)retval < ZEND_LONG_MIN / (zend_long)factor;
			}
		} else {
			overflow = retval > ZEND_ULONG_MAX / factor;
		}
	}

	retval *= factor;

	if (UNEXPECTED(digits_end != str_end-1)) {
		/* More than one character in suffix */
		smart_str_append_escaped(&invalid, ZSTR_VAL(value), ZSTR_LEN(value));
		smart_str_0(&invalid);
		smart_str_append_escaped(&interpreted, str, digits_end - str);
		smart_str_0(&interpreted);
		smart_str_append_escaped(&chr, str_end-1, 1);
		smart_str_0(&chr);

		*errstr = zend_strpprintf(0, "Invalid quantity \"%s\", interpreting as \"%s%s\" for backwards compatibility",
						ZSTR_VAL(invalid.s), ZSTR_VAL(interpreted.s), ZSTR_VAL(chr.s));

		smart_str_free(&invalid);
		smart_str_free(&interpreted);
		smart_str_free(&chr);

		return retval;
	}

end:
	if (UNEXPECTED(overflow)) {
		smart_str_append_escaped(&invalid, ZSTR_VAL(value), ZSTR_LEN(value));
		smart_str_0(&invalid);

		/* Not specifying the resulting value here because the caller may make
		 * additional conversions. Not specifying the allowed range
		 * because the caller may do narrower range checks. */
		*errstr = zend_strpprintf(0, "Invalid quantity \"%s\": value is out of range, using overflow result for backwards compatibility",
						ZSTR_VAL(invalid.s));

		smart_str_free(&invalid);
		smart_str_free(&interpreted);
		smart_str_free(&chr);

		return retval;
	}

	*errstr = NULL;
	return retval;
}
/* }}} */

ZEND_API zend_long zend_ini_parse_quantity(zend_string *value, zend_string **errstr) /* {{{ */
{
	return (zend_long) zend_ini_parse_quantity_internal(value, ZEND_INI_PARSE_QUANTITY_SIGNED, errstr);
}
/* }}} */

ZEND_API zend_ulong zend_ini_parse_uquantity(zend_string *value, zend_string **errstr) /* {{{ */
{
	return zend_ini_parse_quantity_internal(value, ZEND_INI_PARSE_QUANTITY_UNSIGNED, errstr);
}
/* }}} */

ZEND_API zend_long zend_ini_parse_quantity_warn(zend_string *value, zend_string *setting) /* {{{ */
{
	zend_string *errstr;
	zend_long retval = zend_ini_parse_quantity(value, &errstr);

	if (errstr) {
		zend_error(E_WARNING, "Invalid \"%s\" setting. %s", ZSTR_VAL(setting), ZSTR_VAL(errstr));
		zend_string_release(errstr);
	}

	return retval;
}
/* }}} */

ZEND_API zend_ulong zend_ini_parse_uquantity_warn(zend_string *value, zend_string *setting) /* {{{ */
{
	zend_string *errstr;
	zend_ulong retval = zend_ini_parse_uquantity(value, &errstr);

	if (errstr) {
		zend_error(E_WARNING, "Invalid \"%s\" setting. %s", ZSTR_VAL(setting), ZSTR_VAL(errstr));
		zend_string_release(errstr);
	}

	return retval;
}
/* }}} */

ZEND_INI_DISP(zend_ini_boolean_displayer_cb) /* {{{ */
{
	int value;
	zend_string *tmp_value;

	if (type == ZEND_INI_DISPLAY_ORIG && ini_entry->modified) {
		tmp_value = (ini_entry->orig_value ? ini_entry->orig_value : NULL );
	} else if (ini_entry->value) {
		tmp_value = ini_entry->value;
	} else {
		tmp_value = NULL;
	}

	if (tmp_value) {
		value = zend_ini_parse_bool(tmp_value);
	} else {
		value = 0;
	}

	if (value) {
		ZEND_PUTS("On");
	} else {
		ZEND_PUTS("Off");
	}
}
/* }}} */

ZEND_INI_DISP(zend_ini_color_displayer_cb) /* {{{ */
{
	char *value;

	if (type == ZEND_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ZSTR_VAL(ini_entry->orig_value);
	} else if (ini_entry->value) {
		value = ZSTR_VAL(ini_entry->value);
	} else {
		value = NULL;
	}
	if (value) {
		if (zend_uv.html_errors) {
			zend_printf("<font style=\"color: %s\">%s</font>", value, value);
		} else {
			ZEND_PUTS(value);
		}
	} else {
		if (zend_uv.html_errors) {
			ZEND_PUTS(NO_VALUE_HTML);
		} else {
			ZEND_PUTS(NO_VALUE_PLAINTEXT);
		}
	}
}
/* }}} */

ZEND_INI_DISP(display_link_numbers) /* {{{ */
{
	char *value;

	if (type == ZEND_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ZSTR_VAL(ini_entry->orig_value);
	} else if (ini_entry->value) {
		value = ZSTR_VAL(ini_entry->value);
	} else {
		value = NULL;
	}

	if (value) {
		if (atoi(value) == -1) {
			ZEND_PUTS("Unlimited");
		} else {
			zend_printf("%s", value);
		}
	}
}
/* }}} */

/* Standard message handlers */
ZEND_API ZEND_INI_MH(OnUpdateBool) /* {{{ */
{
	bool *p = (bool *) ZEND_INI_GET_ADDR();
	*p = zend_ini_parse_bool(new_value);
	return SUCCESS;
}
/* }}} */

ZEND_API ZEND_INI_MH(OnUpdateLong) /* {{{ */
{
	zend_long *p = (zend_long *) ZEND_INI_GET_ADDR();
	*p = zend_ini_parse_quantity_warn(new_value, entry->name);
	return SUCCESS;
}
/* }}} */

ZEND_API ZEND_INI_MH(OnUpdateLongGEZero) /* {{{ */
{
	zend_long tmp = zend_ini_parse_quantity_warn(new_value, entry->name);
	if (tmp < 0) {
		return FAILURE;
	}

	zend_long *p = (zend_long *) ZEND_INI_GET_ADDR();
	*p = tmp;

	return SUCCESS;
}
/* }}} */

ZEND_API ZEND_INI_MH(OnUpdateReal) /* {{{ */
{
	double *p = (double *) ZEND_INI_GET_ADDR();
	*p = zend_strtod(ZSTR_VAL(new_value), NULL);
	return SUCCESS;
}
/* }}} */

ZEND_API ZEND_INI_MH(OnUpdateString) /* {{{ */
{
	char **p = (char **) ZEND_INI_GET_ADDR();
	*p = new_value ? ZSTR_VAL(new_value) : NULL;
	return SUCCESS;
}
/* }}} */

ZEND_API ZEND_INI_MH(OnUpdateStringUnempty) /* {{{ */
{
	if (new_value && !ZSTR_VAL(new_value)[0]) {
		return FAILURE;
	}

	char **p = (char **) ZEND_INI_GET_ADDR();
	*p = new_value ? ZSTR_VAL(new_value) : NULL;
	return SUCCESS;
}
/* }}} */

ZEND_API ZEND_INI_MH(OnUpdateStr) /* {{{ */
{
	zend_string **p = (zend_string **) ZEND_INI_GET_ADDR();
	*p = new_value;
	return SUCCESS;
}
/* }}} */

ZEND_API ZEND_INI_MH(OnUpdateStrNotEmpty) /* {{{ */
{
	if (new_value && ZSTR_LEN(new_value) == 0) {
		return FAILURE;
	}

	zend_string **p = (zend_string **) ZEND_INI_GET_ADDR();
	*p = new_value;
	return SUCCESS;
}
/* }}} */
