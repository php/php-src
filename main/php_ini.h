#ifndef _PHP_INI_H
#define _PHP_INI_H

#define PHP_INI_USER	(1<<0)
#define PHP_INI_PERDIR	(1<<1)
#define PHP_INI_SYSTEM	(1<<2)

#define PHP_INI_ALL (PHP_INI_USER|PHP_INI_PERDIR|PHP_INI_SYSTEM)

typedef struct _php_ini_entry php_ini_entry;

#define PHP_INI_MH(name) int name(php_ini_entry *entry, char *new_value, uint new_value_length, void *mh_arg1, void *mh_arg2, void *mh_arg3)
#define PHP_INI_DISP(name) void name(php_ini_entry *ini_entry, int type)

struct _php_ini_entry {
	int module_number;
	int modifyable;
	char *name;
	uint name_length;
	PHP_INI_MH((*on_modify));
	void *mh_arg1;
	void *mh_arg2;
	void *mh_arg3;

	char *value;
	uint value_length;

	char *orig_value;
	uint orig_value_length;
	int modified;

	void (*displayer)(php_ini_entry *ini_entry, int type);
};


int php_ini_mstartup();
int php_ini_mshutdown();
int php_ini_rshutdown();

PHPAPI int php_register_ini_entries(php_ini_entry *ini_entry, int module_number);
PHPAPI void php_unregister_ini_entries(int module_number);
PHPAPI int php_alter_ini_entry(char *name, uint name_length, char *new_value, uint new_value_length, int modify_type);
PHPAPI int php_restore_ini_entry(char *name, uint name_length);
PHPAPI void display_ini_entries(zend_module_entry *module);

PHPAPI long php_ini_long(char *name, uint name_length, int orig);
PHPAPI double php_ini_double(char *name, uint name_length, int orig);
PHPAPI char *php_ini_string(char *name, uint name_length, int orig);

PHPAPI int php_ini_register_displayer(char *name, uint name_length, void (*displayer)(php_ini_entry *ini_entry, int type));
PHPAPI PHP_INI_DISP(php_ini_boolean_displayer_cb);
PHPAPI PHP_INI_DISP(php_ini_color_displayer_cb);

#define PHP_INI_BEGIN()		static php_ini_entry ini_entries[] = {
#define PHP_INI_END()		{ 0, 0, NULL, 0, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0, 0, NULL } };

#define PHP_INI_ENTRY3_EX(name, default_value, modifyable, on_modify, arg1, arg2, arg3, displayer) \
	{ 0, modifyable, name, sizeof(name), on_modify, arg1, arg2, arg3, default_value, sizeof(default_value)-1, NULL, 0, 0, displayer },

#define PHP_INI_ENTRY3(name, default_value, modifyable, on_modify, arg1, arg2, arg3) \
	PHP_INI_ENTRY3_EX(name, default_value, modifyable, on_modify, arg1, arg2, arg3, NULL)

#define PHP_INI_ENTRY2_EX(name, default_value, modifyable, on_modify, arg1, arg2, displayer) \
	PHP_INI_ENTRY3_EX(name, default_value, modifyable, on_modify, arg1, arg2, NULL, displayer)

#define PHP_INI_ENTRY2(name, default_value, modifyable, on_modify, arg1, arg2) \
	PHP_INI_ENTRY2_EX(name, default_value, modifyable, on_modify, arg1, arg2, NULL)

#define PHP_INI_ENTRY1_EX(name, default_value, modifyable, on_modify, arg1, displayer) \
	PHP_INI_ENTRY3_EX(name, default_value, modifyable, on_modify, arg1, NULL, NULL, displayer)

#define PHP_INI_ENTRY1(name, default_value, modifyable, on_modify, arg1) \
	PHP_INI_ENTRY1_EX(name, default_value, modifyable, on_modify, arg1, NULL)
	
#define PHP_INI_ENTRY_EX(name, default_value, modifyable, on_modify, displayer) \
	PHP_INI_ENTRY3_EX(name, default_value, modifyable, on_modify, NULL, NULL, NULL, displayer)

#define PHP_INI_ENTRY(name, default_value, modifyable, on_modify) \
	PHP_INI_ENTRY_EX(name, default_value, modifyable, on_modify, NULL)

#ifdef ZTS
#define STD_PHP_INI_ENTRY(name, default_value, modifyable, on_modify, property_name, struct_type, struct_ptr) \
	PHP_INI_ENTRY2(name, default_value, modifyable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr##_id)
#define STD_PHP_INI_BOOLEAN(name, default_value, modifyable, on_modify, property_name, struct_type, struct_ptr) \
	PHP_INI_ENTRY3_EX(name, default_value, modifyable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr##_id, NULL, php_ini_boolean_displayer_cb)
#else
#define STD_PHP_INI_ENTRY(name, default_value, modifyable, on_modify, property_name, struct_type, struct_ptr) \
	PHP_INI_ENTRY2(name, default_value, modifyable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr)
#define STD_PHP_INI_BOOLEAN(name, default_value, modifyable, on_modify, property_name, struct_type, struct_ptr) \
	PHP_INI_ENTRY3_EX(name, default_value, modifyable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr, NULL, php_ini_boolean_displayer_cb)
#endif

#define INI_INT(name) php_ini_long((name), sizeof(name), 0)
#define INI_FLT(name) php_ini_double((name), sizeof(name), 0)
#define INI_STR(name) php_ini_string((name), sizeof(name), 0)


#define INI_ORIG_INT(name)	php_ini_long((name), sizeof(name), 1)
#define INI_ORIG_FLT(name)	php_ini_double((name), sizeof(name), 1)
#define INI_ORIG_STR(name)	php_ini_string((name), sizeof(name), 1)


#define REGISTER_INI_ENTRIES() php_register_ini_entries(ini_entries, module_number)
#define UNREGISTER_INI_ENTRIES() php_unregister_ini_entries(module_number)
#define DISPLAY_INI_ENTRIES() display_ini_entries(module)

#define REGISTER_INI_DISPLAYER(name, displayer) php_ini_register_displayer((name), sizeof(name), displayer)
#define REGISTER_INI_BOOLEAN(name) REGISTER_INI_DISPLAYER(name, php_ini_boolean_displayer_cb)

pval *cfg_get_entry(char *name, uint name_length);


/* Standard message handlers */
PHP_INI_MH(OnUpdateInt);
PHP_INI_MH(OnUpdateReal);
PHP_INI_MH(OnUpdateString);
PHP_INI_MH(OnUpdateStringUnempty);


#define PHP_INI_DISPLAY_ORIG	1
#define PHP_INI_DISPLAY_ACTIVE	2

#endif /* _PHP_INI_H */
