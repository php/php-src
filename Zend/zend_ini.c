/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2003 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_qsort.h"
#include "zend_API.h"
#include "zend_ini.h"
#include "zend_alloc.h"
#include "zend_operators.h"

static HashTable *registered_zend_ini_directives; 

#define NO_VALUE_PLAINTEXT		"no value"
#define NO_VALUE_HTML			"<i>no value</i>"

/*
 * hash_apply functions
 */
static int zend_remove_ini_entries(zend_ini_entry *ini_entry, int *module_number TSRMLS_DC)
{
	if (ini_entry->module_number == *module_number) {
		return 1;
	} else {
		return 0;
	}
}


static int zend_restore_ini_entry_cb(zend_ini_entry *ini_entry, int stage TSRMLS_DC)
{
	if (ini_entry->modified) {
		if (ini_entry->on_modify) {
			ini_entry->on_modify(ini_entry, ini_entry->orig_value, ini_entry->orig_value_length, ini_entry->mh_arg1, ini_entry->mh_arg2, ini_entry->mh_arg3, stage TSRMLS_CC);
		}
		efree(ini_entry->value);
		ini_entry->value = ini_entry->orig_value;
		ini_entry->value_length = ini_entry->orig_value_length;
		ini_entry->modified = 0;
		ini_entry->orig_value = NULL;
		ini_entry->orig_value_length = 0;
	}
	return 0;
}

/*
 * Startup / shutdown
 */
ZEND_API int zend_ini_startup(TSRMLS_D)
{
	registered_zend_ini_directives = (HashTable *) malloc(sizeof(HashTable));

	EG(ini_directives) = registered_zend_ini_directives;
	if (zend_hash_init_ex(registered_zend_ini_directives, 100, NULL, NULL, 1, 0)==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}


ZEND_API int zend_ini_shutdown(TSRMLS_D)
{
	zend_hash_destroy(EG(ini_directives));
	return SUCCESS;
}


ZEND_API int zend_ini_deactivate(TSRMLS_D)
{
	zend_hash_apply_with_argument(EG(ini_directives), (apply_func_arg_t) zend_restore_ini_entry_cb, (void *) ZEND_INI_STAGE_DEACTIVATE TSRMLS_CC);
	return SUCCESS;
}


#ifdef ZTS
ZEND_API int zend_copy_ini_directives(TSRMLS_D)
{
	zend_ini_entry ini_entry;

	EG(ini_directives) = (HashTable *) malloc(sizeof(HashTable));
	if (zend_hash_init_ex(EG(ini_directives), registered_zend_ini_directives->nNumOfElements, NULL, NULL, 1, 0)==FAILURE) {
		return FAILURE;
	}
	zend_hash_copy(EG(ini_directives), registered_zend_ini_directives, NULL, &ini_entry, sizeof(zend_ini_entry));
	zend_ini_refresh_caches(ZEND_INI_STAGE_STARTUP TSRMLS_CC);
	return SUCCESS;
}
#endif


static int ini_key_compare(const void *a, const void *b TSRMLS_DC)
{
	Bucket *f;
	Bucket *s;
 
	f = *((Bucket **) a);
	s = *((Bucket **) b);

	if (f->nKeyLength==0 && s->nKeyLength==0) { /* both numeric */
		return ZEND_NORMALIZE_BOOL(f->nKeyLength - s->nKeyLength);
	} else if (f->nKeyLength==0) { /* f is numeric, s is not */
		return -1;
	} else if (s->nKeyLength==0) { /* s is numeric, f is not */
		return 1;
	} else { /* both strings */
		return zend_binary_strcasecmp(f->arKey, f->nKeyLength, s->arKey, s->nKeyLength);
	}
}


ZEND_API void zend_ini_sort_entries(TSRMLS_D)
{
	zend_hash_sort(EG(ini_directives), zend_qsort, ini_key_compare, 0 TSRMLS_CC);
}

/*
 * Registration / unregistration
 */

ZEND_API int zend_register_ini_entries(zend_ini_entry *ini_entry, int module_number TSRMLS_DC)
{
	zend_ini_entry *p = ini_entry;
	zend_ini_entry *hashed_ini_entry;
	zval default_value;

	while (p->name) {
		p->module_number = module_number;
		if (zend_hash_add(registered_zend_ini_directives, p->name, p->name_length, p, sizeof(zend_ini_entry), (void **) &hashed_ini_entry)==FAILURE) {
			zend_unregister_ini_entries(module_number TSRMLS_CC);
			return FAILURE;
		}
		if ((zend_get_configuration_directive(p->name, p->name_length, &default_value))==SUCCESS) {
			if (!hashed_ini_entry->on_modify
				|| hashed_ini_entry->on_modify(hashed_ini_entry, default_value.value.str.val, default_value.value.str.len, hashed_ini_entry->mh_arg1, hashed_ini_entry->mh_arg2, hashed_ini_entry->mh_arg3, ZEND_INI_STAGE_STARTUP TSRMLS_CC)==SUCCESS) {
				hashed_ini_entry->value = default_value.value.str.val;
				hashed_ini_entry->value_length = default_value.value.str.len;
			}
		} else {
			if (hashed_ini_entry->on_modify) {
				hashed_ini_entry->on_modify(hashed_ini_entry, hashed_ini_entry->value, hashed_ini_entry->value_length, hashed_ini_entry->mh_arg1, hashed_ini_entry->mh_arg2, hashed_ini_entry->mh_arg3, ZEND_INI_STAGE_STARTUP TSRMLS_CC);
			}
		}
		p++;
	}
	return SUCCESS;
}


ZEND_API void zend_unregister_ini_entries(int module_number TSRMLS_DC)
{
	zend_hash_apply_with_argument(registered_zend_ini_directives, (apply_func_arg_t) zend_remove_ini_entries, (void *) &module_number TSRMLS_CC);
}


static int zend_ini_refresh_cache(zend_ini_entry *p, int stage TSRMLS_DC)
{
	if (p->on_modify) {
		p->on_modify(p, p->value, p->value_length, p->mh_arg1, p->mh_arg2, p->mh_arg3, stage TSRMLS_CC);
	}
	return 0;
}


ZEND_API void zend_ini_refresh_caches(int stage TSRMLS_DC)
{
	zend_hash_apply_with_argument(EG(ini_directives), (apply_func_arg_t) zend_ini_refresh_cache, (void *)(long) stage TSRMLS_CC);
}


ZEND_API int zend_alter_ini_entry(char *name, uint name_length, char *new_value, uint new_value_length, int modify_type, int stage)
{
	zend_ini_entry *ini_entry;
	char *duplicate;
	TSRMLS_FETCH();

	if (zend_hash_find(EG(ini_directives), name, name_length, (void **) &ini_entry)==FAILURE) {
		return FAILURE;
	}

	if (!(ini_entry->modifyable & modify_type)) {
		return FAILURE;
	}

	duplicate = estrndup(new_value, new_value_length);
	
	if (!ini_entry->on_modify
		|| ini_entry->on_modify(ini_entry, duplicate, new_value_length, ini_entry->mh_arg1, ini_entry->mh_arg2, ini_entry->mh_arg3, stage TSRMLS_CC)==SUCCESS) {
		if (!ini_entry->modified) {
			ini_entry->orig_value = ini_entry->value;
			ini_entry->orig_value_length = ini_entry->value_length;
		} else { /* we already changed the value, free the changed value */
			efree(ini_entry->value);
		}
		ini_entry->value = duplicate;
		ini_entry->value_length = new_value_length;
		ini_entry->modified = 1;
	} else {
		efree(duplicate);
	}

	return SUCCESS;
}


ZEND_API int zend_restore_ini_entry(char *name, uint name_length, int stage)
{
	zend_ini_entry *ini_entry;
	TSRMLS_FETCH();

	if (zend_hash_find(EG(ini_directives), name, name_length, (void **) &ini_entry)==FAILURE) {
		return FAILURE;
	}

	zend_restore_ini_entry_cb(ini_entry, stage TSRMLS_CC);
	return SUCCESS;
}


ZEND_API int zend_ini_register_displayer(char *name, uint name_length, void (*displayer)(zend_ini_entry *ini_entry, int type))
{
	zend_ini_entry *ini_entry;

	if (zend_hash_find(registered_zend_ini_directives, name, name_length, (void **) &ini_entry)==FAILURE) {
		return FAILURE;
	}

	ini_entry->displayer = displayer;
	return SUCCESS;
}



/*
 * Data retrieval
 */

ZEND_API long zend_ini_long(char *name, uint name_length, int orig)
{
	zend_ini_entry *ini_entry;
	TSRMLS_FETCH();

	if (zend_hash_find(EG(ini_directives), name, name_length, (void **) &ini_entry)==SUCCESS) {
		if (orig && ini_entry->modified) {
			return (ini_entry->orig_value ? strtol(ini_entry->orig_value, NULL, 0) : 0);
		} else if (ini_entry->value) {
			return strtol(ini_entry->value, NULL, 0);
		}
	}

	return 0;
}


ZEND_API double zend_ini_double(char *name, uint name_length, int orig)
{
	zend_ini_entry *ini_entry;
	TSRMLS_FETCH();

	if (zend_hash_find(EG(ini_directives), name, name_length, (void **) &ini_entry)==SUCCESS) {
		if (orig && ini_entry->modified) {
			return (double) (ini_entry->orig_value ? strtod(ini_entry->orig_value, NULL) : 0.0);
		} else if (ini_entry->value) {
			return (double) strtod(ini_entry->value, NULL);
		}
	}

	return 0.0;
}


ZEND_API char *zend_ini_string(char *name, uint name_length, int orig)
{
	zend_ini_entry *ini_entry;
	TSRMLS_FETCH();

	if (zend_hash_find(EG(ini_directives), name, name_length, (void **) &ini_entry)==SUCCESS) {
		if (orig && ini_entry->modified) {
			return ini_entry->orig_value;
		} else {
			return ini_entry->value;
		}
	}

	return "";
}


static void zend_ini_displayer_cb(zend_ini_entry *ini_entry, int type)
{
	if (ini_entry->displayer) {
		ini_entry->displayer(ini_entry, type);
	} else {
		char *display_string;
		uint display_string_length;

		if (type==ZEND_INI_DISPLAY_ORIG && ini_entry->modified) {
			if (ini_entry->orig_value) {
				display_string = ini_entry->orig_value;
				display_string_length = ini_entry->orig_value_length;
			} else {
				if (zend_uv.html_errors) {
					display_string = NO_VALUE_HTML;
					display_string_length = sizeof(NO_VALUE_HTML)-1;
				} else {
					display_string = NO_VALUE_PLAINTEXT;
					display_string_length = sizeof(NO_VALUE_PLAINTEXT)-1;
				}	
			}
		} else if (ini_entry->value && ini_entry->value[0]) {
			display_string = ini_entry->value;
			display_string_length = ini_entry->value_length;
		} else {
			if (zend_uv.html_errors) {
				display_string = NO_VALUE_HTML;
				display_string_length = sizeof(NO_VALUE_HTML)-1;
			} else {
				display_string = NO_VALUE_PLAINTEXT;
				display_string_length = sizeof(NO_VALUE_PLAINTEXT)-1;
			}	
		}
		ZEND_WRITE(display_string, display_string_length);
	}
}


ZEND_INI_DISP(zend_ini_boolean_displayer_cb)
{
	int value;

	if (type==ZEND_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = (ini_entry->orig_value ? atoi(ini_entry->orig_value) : 0);
	} else if (ini_entry->value) {
		value = atoi(ini_entry->value);
	} else {
		value = 0;
	}
	if (value) {
		ZEND_PUTS("On");
	} else {
		ZEND_PUTS("Off");
	}
}


ZEND_INI_DISP(zend_ini_color_displayer_cb)
{
	char *value;

	if (type==ZEND_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ini_entry->orig_value;
	} else if (ini_entry->value) {
		value = ini_entry->value;
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


ZEND_INI_DISP(display_link_numbers)
{
	char *value;

	if (type==ZEND_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ini_entry->orig_value;
	} else if (ini_entry->value) {
		value = ini_entry->value;
	} else {
		value = NULL;
	}

	if (value) {
		if (atoi(value)==-1) {
			ZEND_PUTS("Unlimited");
		} else {
			zend_printf("%s", value);
		}
	}
}


/* Standard message handlers */

ZEND_API ZEND_INI_MH(OnUpdateBool)
{
	zend_bool *p;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base;

	base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	p = (zend_bool *) (base+(size_t) mh_arg1);

	*p = (zend_bool) atoi(new_value);
	return SUCCESS;
}


ZEND_API ZEND_INI_MH(OnUpdateInt)
{
	long *p;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base;

	base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	p = (long *) (base+(size_t) mh_arg1);

	*p = zend_atoi(new_value, new_value_length);
	return SUCCESS;
}


ZEND_API ZEND_INI_MH(OnUpdateReal)
{
	double *p;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base;

	base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	p = (double *) (base+(size_t) mh_arg1);

	*p = strtod(new_value, NULL);
	return SUCCESS;
}


ZEND_API ZEND_INI_MH(OnUpdateString)
{
	char **p;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base;

	base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	p = (char **) (base+(size_t) mh_arg1);

	*p = new_value;
	return SUCCESS;
}


ZEND_API ZEND_INI_MH(OnUpdateStringUnempty)
{
	char **p;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base;

	base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	if (new_value && !new_value[0]) {
		return FAILURE;
	}

	p = (char **) (base+(size_t) mh_arg1);

	*p = new_value;
	return SUCCESS;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
