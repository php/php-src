#ifndef _ZEND_INI_H
#define _ZEND_INI_H


#define ZEND_INI_USER	(1<<0)
#define ZEND_INI_PERDIR	(1<<1)
#define ZEND_INI_SYSTEM	(1<<2)

#define ZEND_INI_ALL (ZEND_INI_USER|ZEND_INI_PERDIR|ZEND_INI_SYSTEM)

typedef struct {
	int module_number;
	int modifyable;
	char *name;
	uint name_length;

	char *value;
	uint value_length;

	char *orig_value;
	uint orig_value_length;
	int modified;
} zend_ini_entry;


int zend_ini_mstartup();
int zend_ini_mshutdown();
int zend_ini_rshutdown();

int zend_register_ini_entries(zend_ini_entry *ini_entry, int module_number);
void zend_unregister_ini_entries(int module_number);
int zend_alter_ini_entry(char *name, uint name_length, char *new_value, uint new_value_length, int modify_type);

long zend_ini_long(char *name, uint name_length);
double zend_ini_double(char *name, uint name_length);
char *zend_ini_string(char *name, uint name_length);

#define ZEND_INI_BEGIN()								static zend_ini_entry ini_entries[] = {
#define ZEND_INI_ENTRY(name, default_value, modifyable)	{ 0, modifyable, name, sizeof(name), default_value, sizeof(default_value)-1, NULL, 0, 0 },
#define ZEND_INI_END()									{ 0, 0, NULL, 0, NULL, 0, NULL, 0, 0 } };

#define INI_INT(name) zend_ini_long((name), sizeof(name))
#define INI_FLT(name) zend_ini_double((name), sizeof(name))
#define INI_STR(name) zend_ini_string((name), sizeof(name))

pval *cfg_get_entry(char *name, uint name_length);

#endif /* _ZEND_INI_H */
