#include <stdlib.h>

#include "php.h"
#include "php_ini.h"
#include "zend_alloc.h"
#include "php_globals.h"

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
		efree(ini_entry->value);
		ini_entry->value = ini_entry->orig_value;
		ini_entry->value_length = ini_entry->orig_value_length;
		ini_entry->modified = 0;
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

int php_register_ini_entries(php_ini_entry *ini_entry, int module_number)
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
		if ((default_value=cfg_get_entry(p->name, p->name_length))) {
			if (!hashed_ini_entry->on_modify
				|| hashed_ini_entry->on_modify(hashed_ini_entry, default_value->value.str.val, default_value->value.str.len, hashed_ini_entry->mh_arg)==SUCCESS) {
				hashed_ini_entry->value = default_value->value.str.val;
				hashed_ini_entry->value_length = default_value->value.str.len;
			}
		}
		hashed_ini_entry->modified = 0;
		p++;
	}
	return SUCCESS;
}


void php_unregister_ini_entries(int module_number)
{
	_php3_hash_apply_with_argument(&known_directives, (int (*)(void *, void *)) php_remove_ini_entries, (void *) &module_number);
}


int php_alter_ini_entry(char *name, uint name_length, char *new_value, uint new_value_length, int modify_type)
{
	php_ini_entry *ini_entry;

	if (_php3_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==FAILURE) {
		return FAILURE;
	}

	if (!(ini_entry->modifyable & modify_type)) {
		return FAILURE;
	}

	if (!ini_entry->on_modify
		|| ini_entry->on_modify(ini_entry, new_value, new_value_length, ini_entry->mh_arg)==SUCCESS) {
		if (!ini_entry->orig_value) {
			ini_entry->orig_value = ini_entry->value;
			ini_entry->orig_value_length = ini_entry->value_length;
		} else { /* we already changed the value, free the changed value */
			efree(ini_entry->value);
		}
		ini_entry->value = estrndup(new_value, new_value_length);
		ini_entry->value_length = new_value_length;
		ini_entry->modified = 1;
	}

	return SUCCESS;
}


int php_restore_ini_entry(char *name, uint name_length)
{
	php_ini_entry *ini_entry;

	if (_php3_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==FAILURE) {
		return FAILURE;
	}

	if (ini_entry->orig_value) {
		ini_entry->value = ini_entry->orig_value;
		ini_entry->value_length = ini_entry->orig_value_length;
	}
	return SUCCESS;
}


/*
 * Data retrieval
 */

long php_ini_long(char *name, uint name_length, int orig)
{
	php_ini_entry *ini_entry;

	if (_php3_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==SUCCESS) {
		if (orig && ini_entry->orig_value) {
			return (long) atoi(ini_entry->orig_value);
		} else {
			return (long) atoi(ini_entry->value);
		}
	}

	return 0;
}


double php_ini_double(char *name, uint name_length, int orig)
{
	php_ini_entry *ini_entry;

	if (_php3_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==SUCCESS) {
		if (orig && ini_entry->orig_value) {
			return (double) strtod(ini_entry->orig_value, NULL);
		} else {
			return (double) strtod(ini_entry->value, NULL);
		}
	}

	return 0.0;
}


char *php_ini_string(char *name, uint name_length, int orig)
{
	php_ini_entry *ini_entry;

	if (_php3_hash_find(&known_directives, name, name_length, (void **) &ini_entry)==SUCCESS) {
		if (orig && ini_entry->orig_value) {
			return ini_entry->orig_value;
		} else {
			return ini_entry->value;
		}
	}

	return "";
}



/* Standard message handlers for core_globals */

PHP_INI_MH(OnUpdateInt)
{
	long *p;
#ifndef ZTS
	char *base = (char *) &core_globals;
#else
	char *base;
	PLS_FETCH();

	base = (char *) core_globals;
#endif

	p = (long *) (base+(size_t) mh_arg);

	*p = atoi(new_value);
	return SUCCESS;
}


PHP_INI_MH(OnUpdateReal)
{
	double *p;
#ifndef ZTS
	char *base = (char *) &core_globals;
#else
	char *base;
	PLS_FETCH();

	base = (char *) core_globals;
#endif

	p = (double *) (base+(size_t) mh_arg);

	*p = strtod(new_value, NULL);
	return SUCCESS;
}


PHP_INI_MH(OnUpdateString)
{
	char **p;
#ifndef ZTS
	char *base = (char *) &core_globals;
#else
	char *base;
	PLS_FETCH();

	base = (char *) core_globals;
#endif

	p = (char **) (base+(size_t) mh_arg);

	*p = new_value;
	return SUCCESS;
}
