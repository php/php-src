/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 4.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
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


static int php_restore_ini_entry_cb(php_ini_entry *ini_entry)
{
	if (ini_entry->modified) {
		if (ini_entry->on_modify) {
			ini_entry->on_modify(ini_entry, ini_entry->orig_value, ini_entry->orig_value_length, ini_entry->mh_arg1, ini_entry->mh_arg2, ini_entry->mh_arg3);
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
	if (_php3_hash_init(&known_directives, 100, NULL, NULL, 1)==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}


int php_ini_mshutdown()
{
	_php3_hash_destroy(&known_directives);
	return SUCCESS;
}


int php_ini_rshutdown()
{
	_php3_hash_apply(&known_directives, (int (*)(void *)) php_restore_ini_entry_cb);
	return SUCCESS;
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
		if (_php3_hash_add(&known_directives, p->name, p->name_length, p, sizeof(php_ini_entry), (void **) &hashed_ini_entry)==FAILURE) {
			php_unregister_ini_entries(module_number);
			return FAILURE;
		}
		if (hashed_ini_entry->on_modify) {
			hashed_ini_entry->on_modify(hashed_ini_entry, hashed_ini_entry->value, hashed_ini_entry->value_length, hashed_ini_entry->mh_arg1, hashed_ini_entry->mh_arg2, hashed_ini_entry->mh_arg3);
		}
		if ((default_value=cfg_get_entry(p->name, p->name_length))) {
			if (!hashed_ini_entry->on_modify
				|| hashed_ini_entry->on_modify(hashed_ini_entry, default_value->value.str.val, default_value->value.str.len, hashed_ini_entry->mh_arg1, hashed_ini_entry->mh_arg2, hashed_ini_entry->mh_arg3)==SUCCESS) {
				hashed_ini_entry->value = default_value->value.str.val;
				hashed_ini_entry->value_length = default_value->value.str.len;
			}
		} else {
			if (hashed_ini_entry->on_modify) {
				hashed_ini_entry->on_modify(hashed_ini_entry, hashed_ini_entry->value, hashed_ini_entry->value_length, hashed_ini_entry->mh_arg1, hashed_ini_entry->mh_arg2, hashed_ini_entry->mh_arg3);
			}
		}
		hashed_ini_entry->modified = 0;
		p++;
	}
	return SUCCESS;
}


PHPAPI void php_unregister_ini_entries(int module_number)
{
	_php3_hash_apply_with_argument(&known_directives, (int (*)(void *, void *)) php_remove_ini_entries, (void *) &module_number);
}


PHPAPI int php_alter_ini_entry(char *name, uint name_length, char *new_value, uint new_value_length, int modify_type)
{
	php_ini_entry *ini_entry;
	char *duplicate;

	if (_php3_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==FAILURE) {
		return FAILURE;
	}

	if (!(ini_entry->modifyable & modify_type)) {
		return FAILURE;
	}

	duplicate = estrndup(new_value, new_value_length);
	
	if (!ini_entry->on_modify
		|| ini_entry->on_modify(ini_entry, duplicate, new_value_length, ini_entry->mh_arg1, ini_entry->mh_arg2, ini_entry->mh_arg3)==SUCCESS) {
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


PHPAPI int php_restore_ini_entry(char *name, uint name_length)
{
	php_ini_entry *ini_entry;

	if (_php3_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==FAILURE) {
		return FAILURE;
	}

	php_restore_ini_entry_cb(ini_entry);
	return SUCCESS;
}


PHPAPI int php_ini_register_displayer(char *name, uint name_length, void (*displayer)(php_ini_entry *ini_entry, int type))
{
	php_ini_entry *ini_entry;

	if (_php3_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==FAILURE) {
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

	if (_php3_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==SUCCESS) {
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

	if (_php3_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==SUCCESS) {
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

	if (_php3_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==SUCCESS) {
		if (orig && ini_entry->modified) {
			return ini_entry->orig_value;
		} else {
			return ini_entry->value;
		}
	}

	return "";
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
		php3_printf("<font color=\"%s\">%s</font>", value, value);
	} else {
		PUTS("<i>no value</i>;");
	}
}


static int php_ini_displayer(php_ini_entry *ini_entry, int module_number)
{
	if (ini_entry->module_number != module_number) {
		return 0;
	}

	PUTS("<tr><td align=\"center\" bgcolor=\"" PHP_ENTRY_NAME_COLOR "\">");
	PHPWRITE(ini_entry->name, ini_entry->name_length-1);
	PUTS("<td align=\"center\" bgcolor=\"" PHP_CONTENTS_COLOR "\">");
	php_ini_displayer_cb(ini_entry, PHP_INI_DISPLAY_ACTIVE);
	PUTS("</td><td align=\"center\" bgcolor=\"" PHP_CONTENTS_COLOR "\">");
	php_ini_displayer_cb(ini_entry, PHP_INI_DISPLAY_ORIG);
	PUTS("</td></tr>\n");
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
	PUTS("<table border=5 width=\"600\">\n");
	php_info_print_table_header(3, "Directive", "Local Value", "Master Value");
	zend_hash_apply_with_argument(&known_directives, (int (*)(void *, void *)) php_ini_displayer, (void *) (long) module_number);
	PUTS("</table>\n");
}


/* Standard message handlers */

PHP_INI_MH(OnUpdateInt)
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


PHP_INI_MH(OnUpdateReal)
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


PHP_INI_MH(OnUpdateString)
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


PHP_INI_MH(OnUpdateStringUnempty)
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
