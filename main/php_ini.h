#ifndef _PHP_INI_H
#define _PHP_INI_H


#define PHP_INI_USER	(1<<0)
#define PHP_INI_PERDIR	(1<<1)
#define PHP_INI_SYSTEM	(1<<2)

#define PHP_INI_ALL (PHP_INI_USER|PHP_INI_PERDIR|PHP_INI_SYSTEM)

typedef struct _php_ini_entry php_ini_entry;

struct _php_ini_entry {
	int module_number;
	int modifyable;
	char *name;
	uint name_length;
	int (*on_modify)(php_ini_entry *entry, char *new_value, uint new_value_length);

	char *value;
	uint value_length;

	char *orig_value;
	uint orig_value_length;
	int modified;
};


int php_ini_mstartup();
int php_ini_mshutdown();
int php_ini_rshutdown();

int php_register_ini_entries(php_ini_entry *ini_entry, int module_number);
void php_unregister_ini_entries(int module_number);
int php_alter_ini_entry(char *name, uint name_length, char *new_value, uint new_value_length, int modify_type);

long php_ini_long(char *name, uint name_length);
double php_ini_double(char *name, uint name_length);
char *php_ini_string(char *name, uint name_length);

#define PHP_INI_BEGIN()								static php_ini_entry ini_entries[] = {

#define PHP_INI_ENTRY(name, default_value, modifyable, on_modify) \
	{ 0, modifyable, name, sizeof(name), on_modify, default_value, sizeof(default_value)-1, NULL, 0, 0 },

#define PHP_INI_END() \
	{ 0, 0, NULL, 0, NULL, NULL, 0, NULL, 0, 0 } };

#define INI_INT(name) php_ini_long((name), sizeof(name))
#define INI_FLT(name) php_ini_double((name), sizeof(name))
#define INI_STR(name) php_ini_string((name), sizeof(name))


#define REGISTER_INI_ENTRIES() php_register_ini_entries(ini_entries, module_number)
#define UNREGISTER_INI_ENTRIES() php_unregister_ini_entries(module_number)

pval *cfg_get_entry(char *name, uint name_length);

#endif /* _PHP_INI_H */
