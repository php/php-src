/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
 */


#include <stdlib.h>

#include "php.h"
#include "php_ini.h"
#include "zend_alloc.h"
#include "php_globals.h"
#include "ext/standard/info.h"

static HashTable known_directives;


/*
 * hash_apply functions
 */
static int php_remove_ini_entries(php_ini_entry *ini_entry, int *module_number)
{
	if (ini_entry->module_number == *module_number) {
		return 1;
	} else {
		return 0;
	}
}


static int php_restore_ini_entry_cb(php_ini_entry *ini_entry, int stage)
{
	if (ini_entry->modified) {
		if (ini_entry->on_modify) {
			ini_entry->on_modify(ini_entry, ini_entry->orig_value, ini_entry->orig_value_length, ini_entry->mh_arg1, ini_entry->mh_arg2, ini_entry->mh_arg3, stage);
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
int php_ini_mstartup()
{
	if (zend_hash_init_ex(&known_directives, 100, NULL, NULL, 1, 0)==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}


int php_ini_mshutdown()
{
	zend_hash_destroy(&known_directives);
	return SUCCESS;
}


int php_ini_rshutdown()
{
	zend_hash_apply_with_argument(&known_directives, (int (*)(void *, void *)) php_restore_ini_entry_cb, (void *) PHP_INI_STAGE_DEACTIVATE);
	return SUCCESS;
}


static int ini_key_compare(const void *a, const void *b)
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


void php_ini_sort_entries(void)
{
	zend_hash_sort(&known_directives, qsort, ini_key_compare, 0);
}

/*
 * Registration / unregistration
 */

PHPAPI int php_register_ini_entries(php_ini_entry *ini_entry, int module_number)
{
	php_ini_entry *p = ini_entry;
	php_ini_entry *hashed_ini_entry;
	pval *default_value;

	while (p->name) {
		p->module_number = module_number;
		if (zend_hash_add(&known_directives, p->name, p->name_length, p, sizeof(php_ini_entry), (void **) &hashed_ini_entry)==FAILURE) {
			php_unregister_ini_entries(module_number);
			return FAILURE;
		}
		if (hashed_ini_entry->on_modify) {
			hashed_ini_entry->on_modify(hashed_ini_entry, hashed_ini_entry->value, hashed_ini_entry->value_length, hashed_ini_entry->mh_arg1, hashed_ini_entry->mh_arg2, hashed_ini_entry->mh_arg3, PHP_INI_STAGE_STARTUP);
		}
		if ((default_value=cfg_get_entry(p->name, p->name_length))) {
			if (!hashed_ini_entry->on_modify
				|| hashed_ini_entry->on_modify(hashed_ini_entry, default_value->value.str.val, default_value->value.str.len, hashed_ini_entry->mh_arg1, hashed_ini_entry->mh_arg2, hashed_ini_entry->mh_arg3, PHP_INI_STAGE_STARTUP)==SUCCESS) {
				hashed_ini_entry->value = default_value->value.str.val;
				hashed_ini_entry->value_length = default_value->value.str.len;
			}
		} else {
			if (hashed_ini_entry->on_modify) {
				hashed_ini_entry->on_modify(hashed_ini_entry, hashed_ini_entry->value, hashed_ini_entry->value_length, hashed_ini_entry->mh_arg1, hashed_ini_entry->mh_arg2, hashed_ini_entry->mh_arg3, PHP_INI_STAGE_STARTUP);
			}
		}
		hashed_ini_entry->modified = 0;
		p++;
	}
	return SUCCESS;
}


PHPAPI void php_unregister_ini_entries(int module_number)
{
	zend_hash_apply_with_argument(&known_directives, (int (*)(void *, void *)) php_remove_ini_entries, (void *) &module_number);
}


static int php_ini_refresh_cache(php_ini_entry *p, int stage)
{
	if (p->on_modify) {
		p->on_modify(p, p->value, p->value_length, p->mh_arg1, p->mh_arg2, p->mh_arg3, stage);
	}
	return 0;
}


PHPAPI void php_ini_refresh_caches(int stage)
{
	zend_hash_apply_with_argument(&known_directives, (int (*)(void *, void *)) php_ini_refresh_cache, (void *) stage);
}


PHPAPI int php_alter_ini_entry(char *name, uint name_length, char *new_value, uint new_value_length, int modify_type, int stage)
{
	php_ini_entry *ini_entry;
	char *duplicate;

	if (zend_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==FAILURE) {
		return FAILURE;
	}

	if (!(ini_entry->modifyable & modify_type)) {
		return FAILURE;
	}

	duplicate = estrndup(new_value, new_value_length);
	
	if (!ini_entry->on_modify
		|| ini_entry->on_modify(ini_entry, duplicate, new_value_length, ini_entry->mh_arg1, ini_entry->mh_arg2, ini_entry->mh_arg3, stage)==SUCCESS) {
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


PHPAPI int php_restore_ini_entry(char *name, uint name_length, int stage)
{
	php_ini_entry *ini_entry;

	if (zend_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==FAILURE) {
		return FAILURE;
	}

	php_restore_ini_entry_cb(ini_entry, stage);
	return SUCCESS;
}


PHPAPI int php_ini_register_displayer(char *name, uint name_length, void (*displayer)(php_ini_entry *ini_entry, int type))
{
	php_ini_entry *ini_entry;

	if (zend_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==FAILURE) {
		return FAILURE;
	}

	ini_entry->displayer = displayer;
	return SUCCESS;
}



/*
 * Data retrieval
 */

PHPAPI long php_ini_long(char *name, uint name_length, int orig)
{
	php_ini_entry *ini_entry;

	if (zend_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==SUCCESS) {
		if (orig && ini_entry->modified) {
			return (ini_entry->orig_value ? strtol(ini_entry->orig_value, NULL, 0) : 0);
		} else if (ini_entry->value) {
			return strtol(ini_entry->value, NULL, 0);
		}
	}

	return 0;
}


PHPAPI double php_ini_double(char *name, uint name_length, int orig)
{
	php_ini_entry *ini_entry;

	if (zend_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==SUCCESS) {
		if (orig && ini_entry->modified) {
			return (double) (ini_entry->orig_value ? strtod(ini_entry->orig_value, NULL) : 0.0);
		} else if (ini_entry->value) {
			return (double) strtod(ini_entry->value, NULL);
		}
	}

	return 0.0;
}


PHPAPI char *php_ini_string(char *name, uint name_length, int orig)
{
	php_ini_entry *ini_entry;

	if (zend_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==SUCCESS) {
		if (orig && ini_entry->modified) {
			return ini_entry->orig_value;
		} else {
			return ini_entry->value;
		}
	}

	return "";
}


php_ini_entry *get_ini_entry(char *name, uint name_length)
{
	php_ini_entry *ini_entry;

	if (zend_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==SUCCESS) {
		return ini_entry;
	} else {
		return NULL;
	}
}


static void php_ini_displayer_cb(php_ini_entry *ini_entry, int type)
{
	if (ini_entry->displayer) {
		ini_entry->displayer(ini_entry, type);
	} else {
		char *display_string;
		uint display_string_length;

		if (type==PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
			if (ini_entry->orig_value) {
				display_string = ini_entry->orig_value;
				display_string_length = ini_entry->orig_value_length;
			} else {
				display_string = "<i>no value</i>";
				display_string_length = sizeof("<i>no value</i>")-1;
			}
		} else if (ini_entry->value && ini_entry->value[0]) {
			display_string = ini_entry->value;
			display_string_length = ini_entry->value_length;
		} else {
			display_string = "<i>no value</i>";
			display_string_length = sizeof("<i>no value</i>")-1;
		}
		PHPWRITE(display_string, display_string_length);
	}
}


PHP_INI_DISP(php_ini_boolean_displayer_cb)
{
	int value;

	if (type==PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = (ini_entry->orig_value ? atoi(ini_entry->orig_value) : 0);
	} else if (ini_entry->value) {
		value = atoi(ini_entry->value);
	} else {
		value = 0;
	}
	if (value) {
		PUTS("On");
	} else {
		PUTS("Off");
	}
}


PHP_INI_DISP(php_ini_color_displayer_cb)
{
	char *value;

	if (type==PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ini_entry->orig_value;
	} else if (ini_entry->value) {
		value = ini_entry->value;
	} else {
		value = NULL;
	}
	if (value) {
		php_printf("<font color=\"%s\">%s</font>", value, value);
	} else {
		PUTS("<i>no value</i>;");
	}
}


PHP_INI_DISP(display_link_numbers)
{
	char *value;

	if (type==PHP_INI_DISPLAY_ORIG && ini_entry->modified) {
		value = ini_entry->orig_value;
	} else if (ini_entry->value) {
		value = ini_entry->value;
	} else {
		value = NULL;
	}

	if (value) {
		if (atoi(value)==-1) {
			PUTS("Unlimited");
		} else {
			php_printf("%s", value);
		}
	}
}


static int php_ini_displayer(php_ini_entry *ini_entry, int module_number)
{
	if (ini_entry->module_number != module_number) {
		return 0;
	}

	PUTS("<TR VALIGN=\"baseline\" BGCOLOR=\"" PHP_CONTENTS_COLOR "\">");
	PUTS("<TD BGCOLOR=\"" PHP_ENTRY_NAME_COLOR "\"><B>");
	PHPWRITE(ini_entry->name, ini_entry->name_length-1);
	PUTS("</B><BR></TD><TD ALIGN=\"center\">");
	php_ini_displayer_cb(ini_entry, PHP_INI_DISPLAY_ACTIVE);
	PUTS("</TD><TD ALIGN=\"center\">");
	php_ini_displayer_cb(ini_entry, PHP_INI_DISPLAY_ORIG);
	PUTS("</TD></TR>\n");
	return 0;
}


PHPAPI void display_ini_entries(zend_module_entry *module)
{
	int module_number;

	if (module) {
		module_number = module->module_number;
	} else {
		module_number = 0;
	}
	php_info_print_table_start();
	php_info_print_table_header(3, "Directive", "Local Value", "Master Value");
	zend_hash_apply_with_argument(&known_directives, (int (*)(void *, void *)) php_ini_displayer, (void *) (long) module_number);
	php_info_print_table_end();
}


/* Standard message handlers */

PHPAPI PHP_INI_MH(OnUpdateBool)
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


PHPAPI PHP_INI_MH(OnUpdateInt)
{
	long *p;
#ifndef ZTS
	char *base = (char *) mh_arg2;
#else
	char *base;

	base = (char *) ts_resource(*((int *) mh_arg2));
#endif

	p = (long *) (base+(size_t) mh_arg1);

	*p = atoi(new_value);
	return SUCCESS;
}


PHPAPI PHP_INI_MH(OnUpdateReal)
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


PHPAPI PHP_INI_MH(OnUpdateString)
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


PHPAPI PHP_INI_MH(OnUpdateStringUnempty)
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
