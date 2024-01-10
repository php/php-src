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

#ifndef ZEND_INI_H
#define ZEND_INI_H

#define ZEND_INI_USER	(1<<0)
#define ZEND_INI_PERDIR	(1<<1)
#define ZEND_INI_SYSTEM	(1<<2)

#define ZEND_INI_ALL (ZEND_INI_USER|ZEND_INI_PERDIR|ZEND_INI_SYSTEM)

#define ZEND_INI_MH(name) int name(zend_ini_entry *entry, zend_string *new_value, void *mh_arg1, void *mh_arg2, void *mh_arg3, int stage)
#define ZEND_INI_DISP(name) ZEND_COLD void name(zend_ini_entry *ini_entry, int type)

typedef struct _zend_ini_entry_def {
	const char *name;
	ZEND_INI_MH((*on_modify));
	void *mh_arg1;
	void *mh_arg2;
	void *mh_arg3;
	const char *value;
	void (*displayer)(zend_ini_entry *ini_entry, int type);

	uint32_t value_length;
	uint16_t name_length;
	uint8_t modifiable;
} zend_ini_entry_def;

struct _zend_ini_entry {
	zend_string *name;
	ZEND_INI_MH((*on_modify));
	void *mh_arg1;
	void *mh_arg2;
	void *mh_arg3;
	zend_string *value;
	zend_string *orig_value;
	void (*displayer)(zend_ini_entry *ini_entry, int type);

	int module_number;

	uint8_t modifiable;
	uint8_t orig_modifiable;
	uint8_t modified;

};

BEGIN_EXTERN_C()
ZEND_API void zend_ini_startup(void);
ZEND_API void zend_ini_shutdown(void);
ZEND_API void zend_ini_global_shutdown(void);
ZEND_API void zend_ini_deactivate(void);
ZEND_API void zend_ini_dtor(HashTable *ini_directives);

ZEND_API void zend_copy_ini_directives(void);

ZEND_API void zend_ini_sort_entries(void);

ZEND_API zend_result zend_register_ini_entries(const zend_ini_entry_def *ini_entry, int module_number);
ZEND_API zend_result zend_register_ini_entries_ex(const zend_ini_entry_def *ini_entry, int module_number, int module_type);
ZEND_API void zend_unregister_ini_entries(int module_number);
ZEND_API void zend_unregister_ini_entries_ex(int module_number, int module_type);
ZEND_API void zend_ini_refresh_caches(int stage);
ZEND_API zend_result zend_alter_ini_entry(zend_string *name, zend_string *new_value, int modify_type, int stage);
ZEND_API zend_result zend_alter_ini_entry_ex(zend_string *name, zend_string *new_value, int modify_type, int stage, bool force_change);
ZEND_API zend_result zend_alter_ini_entry_chars(zend_string *name, const char *value, size_t value_length, int modify_type, int stage);
ZEND_API zend_result zend_alter_ini_entry_chars_ex(zend_string *name, const char *value, size_t value_length, int modify_type, int stage, int force_change);
ZEND_API zend_result zend_restore_ini_entry(zend_string *name, int stage);
ZEND_API void display_ini_entries(zend_module_entry *module);

ZEND_API zend_long zend_ini_long(const char *name, size_t name_length, int orig);
ZEND_API double zend_ini_double(const char *name, size_t name_length, int orig);
ZEND_API char *zend_ini_string(const char *name, size_t name_length, int orig);
ZEND_API char *zend_ini_string_ex(const char *name, size_t name_length, int orig, bool *exists);
ZEND_API zend_string *zend_ini_get_value(zend_string *name);
ZEND_API bool zend_ini_parse_bool(zend_string *str);

/**
 * Parses an ini quantity
 *
 * The value parameter must be a string in the form
 *
 *     sign? digits ws* multiplier?
 *
 * with
 *
 *     sign: [+-]
 *     digit: [0-9]
 *     digits: digit+
 *     ws: [ \t\n\r\v\f]
 *     multiplier: [KMG]
 *
 * Leading and trailing whitespaces are ignored.
 *
 * If the string is empty or consists only of only whitespaces, 0 is returned.
 *
 * Digits is parsed as decimal unless the first digit is '0', in which case
 * digits is parsed as octal.
 *
 * The multiplier is case-insensitive. K, M, and G multiply the quantity by
 * 2**10, 2**20, and 2**30, respectively.
 *
 * For backwards compatibility, ill-formatted values are handled as follows:
 * - No leading digits: value is treated as '0'
 * - Invalid multiplier: multiplier is ignored
 * - Invalid characters between digits and multiplier: invalid characters are
 *   ignored
 * - Integer overflow: The result of the overflow is returned
 *
 * In any of these cases an error string is stored in *errstr (caller must
 * release it), otherwise *errstr is set to NULL.
 */
ZEND_API zend_long zend_ini_parse_quantity(zend_string *value, zend_string **errstr);

/**
 * Unsigned variant of zend_ini_parse_quantity
 */
ZEND_API zend_ulong zend_ini_parse_uquantity(zend_string *value, zend_string **errstr);

ZEND_API zend_long zend_ini_parse_quantity_warn(zend_string *value, zend_string *setting);

ZEND_API zend_ulong zend_ini_parse_uquantity_warn(zend_string *value, zend_string *setting);

ZEND_API zend_result zend_ini_register_displayer(const char *name, uint32_t name_length, void (*displayer)(zend_ini_entry *ini_entry, int type));

ZEND_API ZEND_INI_DISP(zend_ini_boolean_displayer_cb);
ZEND_API ZEND_INI_DISP(zend_ini_color_displayer_cb);
ZEND_API ZEND_INI_DISP(display_link_numbers);
END_EXTERN_C()

#define ZEND_INI_BEGIN()		static const zend_ini_entry_def ini_entries[] = {
#define ZEND_INI_END()		{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0} };

#define ZEND_INI_ENTRY3_EX(name, default_value, modifiable, on_modify, arg1, arg2, arg3, displayer) \
	{ name, on_modify, arg1, arg2, arg3, default_value, displayer, sizeof(default_value)-1, sizeof(name)-1, modifiable },

#define ZEND_INI_ENTRY3(name, default_value, modifiable, on_modify, arg1, arg2, arg3) \
	ZEND_INI_ENTRY3_EX(name, default_value, modifiable, on_modify, arg1, arg2, arg3, NULL)

#define ZEND_INI_ENTRY2_EX(name, default_value, modifiable, on_modify, arg1, arg2, displayer) \
	ZEND_INI_ENTRY3_EX(name, default_value, modifiable, on_modify, arg1, arg2, NULL, displayer)

#define ZEND_INI_ENTRY2(name, default_value, modifiable, on_modify, arg1, arg2) \
	ZEND_INI_ENTRY2_EX(name, default_value, modifiable, on_modify, arg1, arg2, NULL)

#define ZEND_INI_ENTRY1_EX(name, default_value, modifiable, on_modify, arg1, displayer) \
	ZEND_INI_ENTRY3_EX(name, default_value, modifiable, on_modify, arg1, NULL, NULL, displayer)

#define ZEND_INI_ENTRY1(name, default_value, modifiable, on_modify, arg1) \
	ZEND_INI_ENTRY1_EX(name, default_value, modifiable, on_modify, arg1, NULL)

#define ZEND_INI_ENTRY_EX(name, default_value, modifiable, on_modify, displayer) \
	ZEND_INI_ENTRY3_EX(name, default_value, modifiable, on_modify, NULL, NULL, NULL, displayer)

#define ZEND_INI_ENTRY(name, default_value, modifiable, on_modify) \
	ZEND_INI_ENTRY_EX(name, default_value, modifiable, on_modify, NULL)

#ifdef ZTS
#define STD_ZEND_INI_ENTRY(name, default_value, modifiable, on_modify, property_name, struct_type, struct_ptr) \
	ZEND_INI_ENTRY2(name, default_value, modifiable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr##_id)
#define STD_ZEND_INI_ENTRY_EX(name, default_value, modifiable, on_modify, property_name, struct_type, struct_ptr, displayer) \
	ZEND_INI_ENTRY2_EX(name, default_value, modifiable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr##_id, displayer)
#define STD_ZEND_INI_BOOLEAN(name, default_value, modifiable, on_modify, property_name, struct_type, struct_ptr) \
	ZEND_INI_ENTRY3_EX(name, default_value, modifiable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr##_id, NULL, zend_ini_boolean_displayer_cb)
#else
#define STD_ZEND_INI_ENTRY(name, default_value, modifiable, on_modify, property_name, struct_type, struct_ptr) \
	ZEND_INI_ENTRY2(name, default_value, modifiable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr)
#define STD_ZEND_INI_ENTRY_EX(name, default_value, modifiable, on_modify, property_name, struct_type, struct_ptr, displayer) \
	ZEND_INI_ENTRY2_EX(name, default_value, modifiable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr, displayer)
#define STD_ZEND_INI_BOOLEAN(name, default_value, modifiable, on_modify, property_name, struct_type, struct_ptr) \
	ZEND_INI_ENTRY3_EX(name, default_value, modifiable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr, NULL, zend_ini_boolean_displayer_cb)
#endif

#define INI_INT(name) zend_ini_long((name), strlen(name), 0)
#define INI_FLT(name) zend_ini_double((name), strlen(name), 0)
#define INI_STR(name) zend_ini_string_ex((name), strlen(name), 0, NULL)
#define INI_BOOL(name) ((bool) INI_INT(name))

#define INI_ORIG_INT(name)	zend_ini_long((name), strlen(name), 1)
#define INI_ORIG_FLT(name)	zend_ini_double((name), strlen(name), 1)
#define INI_ORIG_STR(name)	zend_ini_string((name), strlen(name), 1)
#define INI_ORIG_BOOL(name) ((bool) INI_ORIG_INT(name))

#define REGISTER_INI_ENTRIES() zend_register_ini_entries_ex(ini_entries, module_number, type)
#define UNREGISTER_INI_ENTRIES() zend_unregister_ini_entries_ex(module_number, type)
#define DISPLAY_INI_ENTRIES() display_ini_entries(zend_module)

#define REGISTER_INI_DISPLAYER(name, displayer) zend_ini_register_displayer((name), strlen(name), displayer)
#define REGISTER_INI_BOOLEAN(name) REGISTER_INI_DISPLAYER(name, zend_ini_boolean_displayer_cb)

/* Standard message handlers */
BEGIN_EXTERN_C()
ZEND_API ZEND_INI_MH(OnUpdateBool);
ZEND_API ZEND_INI_MH(OnUpdateLong);
ZEND_API ZEND_INI_MH(OnUpdateLongGEZero);
ZEND_API ZEND_INI_MH(OnUpdateReal);
/* char* versions */
ZEND_API ZEND_INI_MH(OnUpdateString);
ZEND_API ZEND_INI_MH(OnUpdateStringUnempty);
/* zend_string* versions */
ZEND_API ZEND_INI_MH(OnUpdateStr);
ZEND_API ZEND_INI_MH(OnUpdateStrNotEmpty);
END_EXTERN_C()

#define ZEND_INI_DISPLAY_ORIG	1
#define ZEND_INI_DISPLAY_ACTIVE	2

#define ZEND_INI_STAGE_STARTUP		(1<<0)
#define ZEND_INI_STAGE_SHUTDOWN		(1<<1)
#define ZEND_INI_STAGE_ACTIVATE		(1<<2)
#define ZEND_INI_STAGE_DEACTIVATE	(1<<3)
#define ZEND_INI_STAGE_RUNTIME		(1<<4)
#define ZEND_INI_STAGE_HTACCESS		(1<<5)

#define ZEND_INI_STAGE_IN_REQUEST   (ZEND_INI_STAGE_ACTIVATE|ZEND_INI_STAGE_DEACTIVATE|ZEND_INI_STAGE_RUNTIME|ZEND_INI_STAGE_HTACCESS)

/* INI parsing engine */
typedef void (*zend_ini_parser_cb_t)(zval *arg1, zval *arg2, zval *arg3, int callback_type, void *arg);
BEGIN_EXTERN_C()
ZEND_API zend_result zend_parse_ini_file(zend_file_handle *fh, bool unbuffered_errors, int scanner_mode, zend_ini_parser_cb_t ini_parser_cb, void *arg);
ZEND_API zend_result zend_parse_ini_string(char *str, bool unbuffered_errors, int scanner_mode, zend_ini_parser_cb_t ini_parser_cb, void *arg);
END_EXTERN_C()

/* INI entries */
#define ZEND_INI_PARSER_ENTRY     1 /* Normal entry: foo = bar */
#define ZEND_INI_PARSER_SECTION	  2 /* Section: [foobar] */
#define ZEND_INI_PARSER_POP_ENTRY 3 /* Offset entry: foo[] = bar */

typedef struct _zend_ini_parser_param {
	zend_ini_parser_cb_t ini_parser_cb;
	void *arg;
} zend_ini_parser_param;

#ifndef ZTS
# define ZEND_INI_GET_BASE() ((char *) mh_arg2)
#else
# define ZEND_INI_GET_BASE() ((char *) ts_resource(*((int *) mh_arg2)))
#endif

#define ZEND_INI_GET_ADDR() (ZEND_INI_GET_BASE() + (size_t) mh_arg1)

#endif /* ZEND_INI_H */
