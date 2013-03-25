/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "ext/standard/info.h"
#include "zend_ini.h"
#include "zend_ini_scanner.h"
#include "php_ini.h"
#include "ext/standard/dl.h"
#include "zend_extensions.h"
#include "zend_highlight.h"
#include "SAPI.h"
#include "php_main.h"
#include "php_scandir.h"
#ifdef PHP_WIN32
#include "win32/php_registry.h"
#endif

#if HAVE_SCANDIR && HAVE_ALPHASORT && HAVE_DIRENT_H
#include <dirent.h>
#endif

#ifndef S_ISREG
#define S_ISREG(mode)   (((mode) & S_IFMT) == S_IFREG)
#endif

#ifdef PHP_WIN32
#define TRANSLATE_SLASHES_LOWER(path) \
	{ \
		char *tmp = path; \
		while (*tmp) { \
			if (*tmp == '\\') *tmp = '/'; \
			else *tmp = tolower(*tmp); \
				tmp++; \
		} \
	}
#else
#define TRANSLATE_SLASHES_LOWER(path)
#endif


typedef struct _php_extension_lists {
	zend_llist engine;
	zend_llist functions;
} php_extension_lists;

/* True globals */
static int is_special_section = 0;
static HashTable *active_ini_hash;
static HashTable configuration_hash;
static int has_per_dir_config = 0;
static int has_per_host_config = 0;
PHPAPI char *php_ini_opened_path=NULL;
static php_extension_lists extension_lists;
PHPAPI char *php_ini_scanned_path=NULL;
PHPAPI char *php_ini_scanned_files=NULL;

/* {{{ php_ini_displayer_cb
 */
static void php_ini_displayer_cb(zend_ini_entry *ini_entry, int type TSRMLS_DC)
{
	if (ini_entry->displayer) {
		ini_entry->displayer(ini_entry, type);
	} else {
		char *display_string;
		uint display_string_length, esc_html=0;

		if (type == ZEND_INI_DISPLAY_ORIG && ini_entry->modified) {
			if (ini_entry->orig_value && ini_entry->orig_value[0]) {
				display_string = ini_entry->orig_value;
				display_string_length = ini_entry->orig_value_length;
				esc_html = !sapi_module.phpinfo_as_text;
			} else {
				if (!sapi_module.phpinfo_as_text) {
					display_string = "<i>no value</i>";
					display_string_length = sizeof("<i>no value</i>") - 1;
				} else {
					display_string = "no value";
					display_string_length = sizeof("no value") - 1;
				}
			}
		} else if (ini_entry->value && ini_entry->value[0]) {
			display_string = ini_entry->value;
			display_string_length = ini_entry->value_length;
			esc_html = !sapi_module.phpinfo_as_text;
		} else {
			if (!sapi_module.phpinfo_as_text) {
				display_string = "<i>no value</i>";
				display_string_length = sizeof("<i>no value</i>") - 1;
			} else {
				display_string = "no value";
				display_string_length = sizeof("no value") - 1;
			}
		}

		if (esc_html) {
			php_html_puts(display_string, display_string_length TSRMLS_CC);
		} else {
			PHPWRITE(display_string, display_string_length);
		}
	}
}
/* }}} */

/* {{{ php_ini_displayer
 */
static int php_ini_displayer(zend_ini_entry *ini_entry, int module_number TSRMLS_DC)
{
	if (ini_entry->module_number != module_number) {
		return 0;
	}
	if (!sapi_module.phpinfo_as_text) {
		PUTS("<tr>");
		PUTS("<td class=\"e\">");
		PHPWRITE(ini_entry->name, ini_entry->name_length - 1);
		PUTS("</td><td class=\"v\">");
		php_ini_displayer_cb(ini_entry, ZEND_INI_DISPLAY_ACTIVE TSRMLS_CC);
		PUTS("</td><td class=\"v\">");
		php_ini_displayer_cb(ini_entry, ZEND_INI_DISPLAY_ORIG TSRMLS_CC);
		PUTS("</td></tr>\n");
	} else {
		PHPWRITE(ini_entry->name, ini_entry->name_length - 1);
		PUTS(" => ");
		php_ini_displayer_cb(ini_entry, ZEND_INI_DISPLAY_ACTIVE TSRMLS_CC);
		PUTS(" => ");
		php_ini_displayer_cb(ini_entry, ZEND_INI_DISPLAY_ORIG TSRMLS_CC);
		PUTS("\n");
	}
	return 0;
}
/* }}} */

/* {{{ php_ini_available
 */
static int php_ini_available(zend_ini_entry *ini_entry, int *module_number_available TSRMLS_DC)
{
	if (ini_entry->module_number == *module_number_available) {
		*module_number_available = -1;
		return ZEND_HASH_APPLY_STOP;
	} else {
		return ZEND_HASH_APPLY_KEEP;
	}
}
/* }}} */

/* {{{ display_ini_entries
 */
PHPAPI void display_ini_entries(zend_module_entry *module)
{
	int module_number, module_number_available;
	TSRMLS_FETCH();

	if (module) {
		module_number = module->module_number;
	} else {
		module_number = 0;
	}
	module_number_available = module_number;
	zend_hash_apply_with_argument(EG(ini_directives), (apply_func_arg_t) php_ini_available, &module_number_available TSRMLS_CC);
	if (module_number_available == -1) {
		php_info_print_table_start();
		php_info_print_table_header(3, "Directive", "Local Value", "Master Value");
		zend_hash_apply_with_argument(EG(ini_directives), (apply_func_arg_t) php_ini_displayer, (void *) (zend_intptr_t) module_number TSRMLS_CC);
		php_info_print_table_end();
	}
}
/* }}} */

/* php.ini support */
#define PHP_EXTENSION_TOKEN		"extension"
#define ZEND_EXTENSION_TOKEN	"zend_extension"

/* {{{ config_zval_dtor
 */
PHPAPI void config_zval_dtor(zval *zvalue)
{
	if (Z_TYPE_P(zvalue) == IS_ARRAY) {
		zend_hash_destroy(Z_ARRVAL_P(zvalue));
		free(Z_ARRVAL_P(zvalue));
	} else if (Z_TYPE_P(zvalue) == IS_STRING) {
		free(Z_STRVAL_P(zvalue));
	}
}
/* Reset / free active_ini_sectin global */
#define RESET_ACTIVE_INI_HASH() do { \
	active_ini_hash = NULL;          \
	is_special_section = 0;          \
} while (0)
/* }}} */

/* {{{ php_ini_parser_cb
 */
static void php_ini_parser_cb(zval *arg1, zval *arg2, zval *arg3, int callback_type, HashTable *target_hash)
{
	zval *entry;
	HashTable *active_hash;
	char *extension_name;

	if (active_ini_hash) {
		active_hash = active_ini_hash;
	} else {
		active_hash = target_hash;
	}

	switch (callback_type) {
		case ZEND_INI_PARSER_ENTRY: {
				if (!arg2) {
					/* bare string - nothing to do */
					break;
				}

				/* PHP and Zend extensions are not added into configuration hash! */
				if (!is_special_section && !strcasecmp(Z_STRVAL_P(arg1), PHP_EXTENSION_TOKEN)) { /* load PHP extension */
					extension_name = estrndup(Z_STRVAL_P(arg2), Z_STRLEN_P(arg2));
					zend_llist_add_element(&extension_lists.functions, &extension_name);
				} else if (!is_special_section && !strcasecmp(Z_STRVAL_P(arg1), ZEND_EXTENSION_TOKEN)) { /* load Zend extension */
					extension_name = estrndup(Z_STRVAL_P(arg2), Z_STRLEN_P(arg2));
					zend_llist_add_element(&extension_lists.engine, &extension_name);

				/* All other entries are added into either configuration_hash or active ini section array */
				} else {
					/* Store in active hash */
					zend_hash_update(active_hash, Z_STRVAL_P(arg1), Z_STRLEN_P(arg1) + 1, arg2, sizeof(zval), (void **) &entry);
					Z_STRVAL_P(entry) = zend_strndup(Z_STRVAL_P(entry), Z_STRLEN_P(entry));
				}
			}
			break;

		case ZEND_INI_PARSER_POP_ENTRY: {
				zval *option_arr;
				zval *find_arr;

				if (!arg2) {
					/* bare string - nothing to do */
					break;
				}

/* fprintf(stdout, "ZEND_INI_PARSER_POP_ENTRY: %s[%s] = %s\n",Z_STRVAL_P(arg1), Z_STRVAL_P(arg3), Z_STRVAL_P(arg2)); */

				/* If option not found in hash or is not an array -> create array, otherwise add to existing array */
				if (zend_hash_find(active_hash, Z_STRVAL_P(arg1), Z_STRLEN_P(arg1) + 1, (void **) &find_arr) == FAILURE || Z_TYPE_P(find_arr) != IS_ARRAY) {
					option_arr = (zval *) pemalloc(sizeof(zval), 1);
					INIT_PZVAL(option_arr);
					Z_TYPE_P(option_arr) = IS_ARRAY;
					Z_ARRVAL_P(option_arr) = (HashTable *) pemalloc(sizeof(HashTable), 1);
					zend_hash_init(Z_ARRVAL_P(option_arr), 0, NULL, (dtor_func_t) config_zval_dtor, 1);
					zend_hash_update(active_hash, Z_STRVAL_P(arg1), Z_STRLEN_P(arg1) + 1, option_arr, sizeof(zval), (void **) &find_arr);
					free(option_arr);
				}

				/* arg3 is possible option offset name */
				if (arg3 && Z_STRLEN_P(arg3) > 0) {
					zend_symtable_update(Z_ARRVAL_P(find_arr), Z_STRVAL_P(arg3), Z_STRLEN_P(arg3) + 1, arg2, sizeof(zval), (void **) &entry);
				} else {
					zend_hash_next_index_insert(Z_ARRVAL_P(find_arr), arg2, sizeof(zval), (void **) &entry);
				}
				Z_STRVAL_P(entry) = zend_strndup(Z_STRVAL_P(entry), Z_STRLEN_P(entry));
			}
			break;

		case ZEND_INI_PARSER_SECTION: { /* Create an array of entries of each section */

/* fprintf(stdout, "ZEND_INI_PARSER_SECTION: %s\n",Z_STRVAL_P(arg1)); */

				char *key = NULL;
				uint key_len;

				/* PATH sections */
				if (!strncasecmp(Z_STRVAL_P(arg1), "PATH", sizeof("PATH") - 1)) {
					key = Z_STRVAL_P(arg1);
					key = key + sizeof("PATH") - 1;
					key_len = Z_STRLEN_P(arg1) - sizeof("PATH") + 1;
					is_special_section = 1;
					has_per_dir_config = 1;

					/* make the path lowercase on Windows, for case insensitivity. Does nothing for other platforms */
					TRANSLATE_SLASHES_LOWER(key);

				/* HOST sections */
				} else if (!strncasecmp(Z_STRVAL_P(arg1), "HOST", sizeof("HOST") - 1)) {
					key = Z_STRVAL_P(arg1);
					key = key + sizeof("HOST") - 1;
					key_len = Z_STRLEN_P(arg1) - sizeof("HOST") + 1;
					is_special_section = 1;
					has_per_host_config = 1;
					zend_str_tolower(key, key_len); /* host names are case-insensitive. */

				} else {
					is_special_section = 0;
				}

				if (key && key_len > 0) {
					/* Strip any trailing slashes */
					while (key_len > 0 && (key[key_len - 1] == '/' || key[key_len - 1] == '\\')) {
						key_len--;
						key[key_len] = 0;
					}

					/* Strip any leading whitespace and '=' */
					while (*key && (
						*key == '=' ||
						*key == ' ' ||
						*key == '\t'
					)) {
						key++;
						key_len--;
					}

					/* Search for existing entry and if it does not exist create one */
					if (zend_hash_find(target_hash, key, key_len + 1, (void **) &entry) == FAILURE) {
						zval *section_arr;

						section_arr = (zval *) pemalloc(sizeof(zval), 1);
						INIT_PZVAL(section_arr);
						Z_TYPE_P(section_arr) = IS_ARRAY;
						Z_ARRVAL_P(section_arr) = (HashTable *) pemalloc(sizeof(HashTable), 1);
						zend_hash_init(Z_ARRVAL_P(section_arr), 0, NULL, (dtor_func_t) config_zval_dtor, 1);
						zend_hash_update(target_hash, key, key_len + 1, section_arr, sizeof(zval), (void **) &entry);
						free(section_arr);
					}
					active_ini_hash = Z_ARRVAL_P(entry);
				}
			}
			break;
	}
}
/* }}} */

/* {{{ php_load_php_extension_cb
 */
static void php_load_php_extension_cb(void *arg TSRMLS_DC)
{
#ifdef HAVE_LIBDL
	php_load_extension(*((char **) arg), MODULE_PERSISTENT, 0 TSRMLS_CC);
#endif
}
/* }}} */

/* {{{ php_load_zend_extension_cb
 */
static void php_load_zend_extension_cb(void *arg TSRMLS_DC)
{
	char *filename = *((char **) arg);
	int length = strlen(filename);

	if (IS_ABSOLUTE_PATH(filename, length)) {
		zend_load_extension(filename);
	} else {
	    char *libpath;
		char *extension_dir = INI_STR("extension_dir");
		int extension_dir_len = strlen(extension_dir);

		if (IS_SLASH(extension_dir[extension_dir_len-1])) {
			spprintf(&libpath, 0, "%s%s", extension_dir, filename);
		} else {
			spprintf(&libpath, 0, "%s%c%s", extension_dir, DEFAULT_SLASH, filename);
		}
		zend_load_extension(libpath);
		efree(libpath);
	}
}
/* }}} */

/* {{{ php_init_config
 */
int php_init_config(TSRMLS_D)
{
	char *php_ini_file_name = NULL;
	char *php_ini_search_path = NULL;
	int php_ini_scanned_path_len;
	char *open_basedir;
	int free_ini_search_path = 0;
	zend_file_handle fh;

	if (zend_hash_init(&configuration_hash, 0, NULL, (dtor_func_t) config_zval_dtor, 1) == FAILURE) {
		return FAILURE;
	}

	if (sapi_module.ini_defaults) {
		sapi_module.ini_defaults(&configuration_hash);
	}

	zend_llist_init(&extension_lists.engine, sizeof(char *), (llist_dtor_func_t) free_estring, 1);
	zend_llist_init(&extension_lists.functions, sizeof(char *), (llist_dtor_func_t) free_estring, 1);

	open_basedir = PG(open_basedir);

	if (sapi_module.php_ini_path_override) {
		php_ini_file_name = sapi_module.php_ini_path_override;
		php_ini_search_path = sapi_module.php_ini_path_override;
		free_ini_search_path = 0;
	} else if (!sapi_module.php_ini_ignore) {
		int search_path_size;
		char *default_location;
		char *env_location;
		static const char paths_separator[] = { ZEND_PATHS_SEPARATOR, 0 };
#ifdef PHP_WIN32
		char *reg_location;
		char phprc_path[MAXPATHLEN];
#endif

		env_location = getenv("PHPRC");

#ifdef PHP_WIN32
		if (!env_location) {
			char dummybuf;
			int size;

			SetLastError(0);

			/*If the given bugger is not large enough to hold the data, the return value is 
			the buffer size,  in characters, required to hold the string and its terminating 
			null character. We use this return value to alloc the final buffer. */
			size = GetEnvironmentVariableA("PHPRC", &dummybuf, 0);
			if (GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
				/* The environment variable doesn't exist. */
				env_location = "";
			} else {
				if (size == 0) {
					env_location = "";
				} else {
					size = GetEnvironmentVariableA("PHPRC", phprc_path, size);
					if (size == 0) {
						env_location = "";
					} else {
						env_location = phprc_path;
					}
				}
			}
		}
#else
		if (!env_location) {
			env_location = "";
		}
#endif
		/*
		 * Prepare search path
		 */

		search_path_size = MAXPATHLEN * 4 + strlen(env_location) + 3 + 1;
		php_ini_search_path = (char *) emalloc(search_path_size);
		free_ini_search_path = 1;
		php_ini_search_path[0] = 0;

		/* Add environment location */
		if (env_location[0]) {
			if (*php_ini_search_path) {
				strlcat(php_ini_search_path, paths_separator, search_path_size);
			}
			strlcat(php_ini_search_path, env_location, search_path_size);
			php_ini_file_name = env_location;
		}

#ifdef PHP_WIN32
		/* Add registry location */
		reg_location = GetIniPathFromRegistry();
		if (reg_location != NULL) {
			if (*php_ini_search_path) {
				strlcat(php_ini_search_path, paths_separator, search_path_size);
			}
			strlcat(php_ini_search_path, reg_location, search_path_size);
			efree(reg_location);
		}
#endif

		/* Add cwd (not with CLI) */
		if (!sapi_module.php_ini_ignore_cwd) {
			if (*php_ini_search_path) {
				strlcat(php_ini_search_path, paths_separator, search_path_size);
			}
			strlcat(php_ini_search_path, ".", search_path_size);
		}

		if (PG(php_binary)) {
			char *separator_location, *binary_location;

			binary_location = estrdup(PG(php_binary));
			separator_location = strrchr(binary_location, DEFAULT_SLASH);

			if (separator_location && separator_location != binary_location) {
				*(separator_location) = 0;
			}
			if (*php_ini_search_path) {
				strlcat(php_ini_search_path, paths_separator, search_path_size);
			}
			strlcat(php_ini_search_path, binary_location, search_path_size);
			efree(binary_location);
		}

		/* Add default location */
#ifdef PHP_WIN32
		default_location = (char *) emalloc(MAXPATHLEN + 1);

		if (0 < GetWindowsDirectory(default_location, MAXPATHLEN)) {
			if (*php_ini_search_path) {
				strlcat(php_ini_search_path, paths_separator, search_path_size);
			}
			strlcat(php_ini_search_path, default_location, search_path_size);
		}

		/* For people running under terminal services, GetWindowsDirectory will
		 * return their personal Windows directory, so lets add the system
		 * windows directory too */
		if (0 < GetSystemWindowsDirectory(default_location, MAXPATHLEN)) {
			if (*php_ini_search_path) {
				strlcat(php_ini_search_path, paths_separator, search_path_size);
			}
			strlcat(php_ini_search_path, default_location, search_path_size);
		}
		efree(default_location);

#else
		default_location = PHP_CONFIG_FILE_PATH;
		if (*php_ini_search_path) {
			strlcat(php_ini_search_path, paths_separator, search_path_size);
		}
		strlcat(php_ini_search_path, default_location, search_path_size);
#endif
	}

	PG(open_basedir) = NULL;

	/*
	 * Find and open actual ini file
	 */

	memset(&fh, 0, sizeof(fh));

	/* If SAPI does not want to ignore all ini files OR an overriding file/path is given.
	 * This allows disabling scanning for ini files in the PHP_CONFIG_FILE_SCAN_DIR but still
	 * load an optional ini file. */
	if (!sapi_module.php_ini_ignore || sapi_module.php_ini_path_override) {

		/* Check if php_ini_file_name is a file and can be opened */
		if (php_ini_file_name && php_ini_file_name[0]) {
			struct stat statbuf;

			if (!VCWD_STAT(php_ini_file_name, &statbuf)) {
				if (!((statbuf.st_mode & S_IFMT) == S_IFDIR)) {
					fh.handle.fp = VCWD_FOPEN(php_ini_file_name, "r");
					if (fh.handle.fp) {
						fh.filename = php_ini_opened_path = expand_filepath(php_ini_file_name, NULL TSRMLS_CC);
					}
				}
			}
		}

		/* Otherwise search for php-%sapi-module-name%.ini file in search path */
		if (!fh.handle.fp) {
			const char *fmt = "php-%s.ini";
			char *ini_fname;
			spprintf(&ini_fname, 0, fmt, sapi_module.name);
			fh.handle.fp = php_fopen_with_path(ini_fname, "r", php_ini_search_path, &php_ini_opened_path TSRMLS_CC);
			efree(ini_fname);
			if (fh.handle.fp) {
				fh.filename = php_ini_opened_path;
			}
		}

		/* If still no ini file found, search for php.ini file in search path */
		if (!fh.handle.fp) {
			fh.handle.fp = php_fopen_with_path("php.ini", "r", php_ini_search_path, &php_ini_opened_path TSRMLS_CC);
			if (fh.handle.fp) {
				fh.filename = php_ini_opened_path;
			}
		}
	}

	if (free_ini_search_path) {
		efree(php_ini_search_path);
	}

	PG(open_basedir) = open_basedir;

	if (fh.handle.fp) {
		fh.type = ZEND_HANDLE_FP;
		RESET_ACTIVE_INI_HASH();

		zend_parse_ini_file(&fh, 1, ZEND_INI_SCANNER_NORMAL, (zend_ini_parser_cb_t) php_ini_parser_cb, &configuration_hash TSRMLS_CC);

		{
			zval tmp;

			Z_STRLEN(tmp) = strlen(fh.filename);
			Z_STRVAL(tmp) = zend_strndup(fh.filename, Z_STRLEN(tmp));
			Z_TYPE(tmp) = IS_STRING;
			Z_SET_REFCOUNT(tmp, 0);

			zend_hash_update(&configuration_hash, "cfg_file_path", sizeof("cfg_file_path"), (void *) &tmp, sizeof(zval), NULL);
			if (php_ini_opened_path) {
				efree(php_ini_opened_path);
			}
			php_ini_opened_path = zend_strndup(Z_STRVAL(tmp), Z_STRLEN(tmp));
		}
	}

	/* Check for PHP_INI_SCAN_DIR environment variable to override/set config file scan directory */
	php_ini_scanned_path = getenv("PHP_INI_SCAN_DIR");
	if (!php_ini_scanned_path) {
		/* Or fall back using possible --with-config-file-scan-dir setting (defaults to empty string!) */
		php_ini_scanned_path = PHP_CONFIG_FILE_SCAN_DIR;
	}
	php_ini_scanned_path_len = strlen(php_ini_scanned_path);

	/* Scan and parse any .ini files found in scan path if path not empty. */
	if (!sapi_module.php_ini_ignore && php_ini_scanned_path_len) {
		struct dirent **namelist;
		int ndir, i;
		struct stat sb;
		char ini_file[MAXPATHLEN];
		char *p;
		zend_file_handle fh2;
		zend_llist scanned_ini_list;
		zend_llist_element *element;
		int l, total_l = 0;

		if ((ndir = php_scandir(php_ini_scanned_path, &namelist, 0, php_alphasort)) > 0) {
			zend_llist_init(&scanned_ini_list, sizeof(char *), (llist_dtor_func_t) free_estring, 1);
			memset(&fh2, 0, sizeof(fh2));

			for (i = 0; i < ndir; i++) {

				/* check for any file with .ini extension */
				if (!(p = strrchr(namelist[i]->d_name, '.')) || (p && strcmp(p, ".ini"))) {
					free(namelist[i]);
					continue;
				}
				/* Reset active ini section */
				RESET_ACTIVE_INI_HASH();

				if (IS_SLASH(php_ini_scanned_path[php_ini_scanned_path_len - 1])) {
					snprintf(ini_file, MAXPATHLEN, "%s%s", php_ini_scanned_path, namelist[i]->d_name);
				} else {
					snprintf(ini_file, MAXPATHLEN, "%s%c%s", php_ini_scanned_path, DEFAULT_SLASH, namelist[i]->d_name);
				}
				if (VCWD_STAT(ini_file, &sb) == 0) {
					if (S_ISREG(sb.st_mode)) {
						if ((fh2.handle.fp = VCWD_FOPEN(ini_file, "r"))) {
							fh2.filename = ini_file;
							fh2.type = ZEND_HANDLE_FP;

							if (zend_parse_ini_file(&fh2, 1, ZEND_INI_SCANNER_NORMAL, (zend_ini_parser_cb_t) php_ini_parser_cb, &configuration_hash TSRMLS_CC) == SUCCESS) {
								/* Here, add it to the list of ini files read */
								l = strlen(ini_file);
								total_l += l + 2;
								p = estrndup(ini_file, l);
								zend_llist_add_element(&scanned_ini_list, &p);
							}
						}
					}
				}
				free(namelist[i]);
			}
			free(namelist);

			if (total_l) {
				int php_ini_scanned_files_len = (php_ini_scanned_files) ? strlen(php_ini_scanned_files) + 1 : 0;
				php_ini_scanned_files = (char *) realloc(php_ini_scanned_files, php_ini_scanned_files_len + total_l + 1);
				if (!php_ini_scanned_files_len) {
					*php_ini_scanned_files = '\0';
				}
				total_l += php_ini_scanned_files_len;
				for (element = scanned_ini_list.head; element; element = element->next) {
					if (php_ini_scanned_files_len) {
						strlcat(php_ini_scanned_files, ",\n", total_l);
					}
					strlcat(php_ini_scanned_files, *(char **)element->data, total_l);
					strlcat(php_ini_scanned_files, element->next ? ",\n" : "\n", total_l);
				}
			}
			zend_llist_destroy(&scanned_ini_list);
		}
	} else {
		/* Make sure an empty php_ini_scanned_path ends up as NULL */
		php_ini_scanned_path = NULL;
	}

	if (sapi_module.ini_entries) {
		/* Reset active ini section */
		RESET_ACTIVE_INI_HASH();
		zend_parse_ini_string(sapi_module.ini_entries, 1, ZEND_INI_SCANNER_NORMAL, (zend_ini_parser_cb_t) php_ini_parser_cb, &configuration_hash TSRMLS_CC);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ php_shutdown_config
 */
int php_shutdown_config(void)
{
	zend_hash_destroy(&configuration_hash);
	if (php_ini_opened_path) {
		free(php_ini_opened_path);
		php_ini_opened_path = NULL;
	}
	if (php_ini_scanned_files) {
		free(php_ini_scanned_files);
		php_ini_scanned_files = NULL;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ php_ini_register_extensions
 */
void php_ini_register_extensions(TSRMLS_D)
{
	zend_llist_apply(&extension_lists.engine, php_load_zend_extension_cb TSRMLS_CC);
	zend_llist_apply(&extension_lists.functions, php_load_php_extension_cb TSRMLS_CC);

	zend_llist_destroy(&extension_lists.engine);
	zend_llist_destroy(&extension_lists.functions);
}
/* }}} */

/* {{{ php_parse_user_ini_file
 */
PHPAPI int php_parse_user_ini_file(const char *dirname, char *ini_filename, HashTable *target_hash TSRMLS_DC)
{
	struct stat sb;
	char ini_file[MAXPATHLEN];
	zend_file_handle fh;

	snprintf(ini_file, MAXPATHLEN, "%s%c%s", dirname, DEFAULT_SLASH, ini_filename);

	if (VCWD_STAT(ini_file, &sb) == 0) {
		if (S_ISREG(sb.st_mode)) {
			memset(&fh, 0, sizeof(fh));
			if ((fh.handle.fp = VCWD_FOPEN(ini_file, "r"))) {
				fh.filename = ini_file;
				fh.type = ZEND_HANDLE_FP;

				/* Reset active ini section */
				RESET_ACTIVE_INI_HASH();

				if (zend_parse_ini_file(&fh, 1, ZEND_INI_SCANNER_NORMAL, (zend_ini_parser_cb_t) php_ini_parser_cb, target_hash TSRMLS_CC) == SUCCESS) {
					/* FIXME: Add parsed file to the list of user files read? */
					return SUCCESS;
				}
				return FAILURE;
			}
		}
	}
	return FAILURE;
}
/* }}} */

/* {{{ php_ini_activate_config
 */
PHPAPI void php_ini_activate_config(HashTable *source_hash, int modify_type, int stage TSRMLS_DC)
{
	char *str;
	zval *data;
	uint str_len;
	ulong num_index;

	/* Walk through config hash and alter matching ini entries using the values found in the hash */
	for (zend_hash_internal_pointer_reset(source_hash);
		zend_hash_get_current_key_ex(source_hash, &str, &str_len, &num_index, 0, NULL) == HASH_KEY_IS_STRING;
		zend_hash_move_forward(source_hash)
	) {
		zend_hash_get_current_data(source_hash, (void **) &data);
		zend_alter_ini_entry_ex(str, str_len, Z_STRVAL_P(data), Z_STRLEN_P(data), modify_type, stage, 0 TSRMLS_CC);
	}
}
/* }}} */

/* {{{ php_ini_has_per_dir_config
 */
PHPAPI int php_ini_has_per_dir_config(void)
{
	return has_per_dir_config;
}
/* }}} */

/* {{{ php_ini_activate_per_dir_config
 */
PHPAPI void php_ini_activate_per_dir_config(char *path, uint path_len TSRMLS_DC)
{
	zval *tmp2;
	char *ptr;

#if PHP_WIN32
	char path_bak[MAXPATHLEN];
#endif

	if (path_len > MAXPATHLEN) {
		return;
	}

#if PHP_WIN32
	memcpy(path_bak, path, path_len);
	path_bak[path_len] = 0;
	TRANSLATE_SLASHES_LOWER(path_bak);
	path = path_bak;
#endif

	/* Walk through each directory in path and apply any found per-dir-system-configuration from configuration_hash */
	if (has_per_dir_config && path && path_len) {
		ptr = path + 1;
		while ((ptr = strchr(ptr, '/')) != NULL) {
			*ptr = 0;
			/* Search for source array matching the path from configuration_hash */
			if (zend_hash_find(&configuration_hash, path, strlen(path) + 1, (void **) &tmp2) == SUCCESS) {
				php_ini_activate_config(Z_ARRVAL_P(tmp2), PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE TSRMLS_CC);
			}
			*ptr = '/';
			ptr++;
		}
	}
}
/* }}} */

/* {{{ php_ini_has_per_host_config
 */
PHPAPI int php_ini_has_per_host_config(void)
{
	return has_per_host_config;
}
/* }}} */

/* {{{ php_ini_activate_per_host_config
 */
PHPAPI void php_ini_activate_per_host_config(const char *host, uint host_len TSRMLS_DC)
{
	zval *tmp;

	if (has_per_host_config && host && host_len) {
		/* Search for source array matching the host from configuration_hash */
		if (zend_hash_find(&configuration_hash, host, host_len, (void **) &tmp) == SUCCESS) {
			php_ini_activate_config(Z_ARRVAL_P(tmp), PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE TSRMLS_CC);
		}
	}
}
/* }}} */

/* {{{ cfg_get_entry
 */
PHPAPI zval *cfg_get_entry(const char *name, uint name_length)
{
	zval *tmp;

	if (zend_hash_find(&configuration_hash, name, name_length, (void **) &tmp) == SUCCESS) {
		return tmp;
	} else {
		return NULL;
	}
}
/* }}} */

/* {{{ cfg_get_long
 */
PHPAPI int cfg_get_long(const char *varname, long *result)
{
	zval *tmp, var;

	if (zend_hash_find(&configuration_hash, varname, strlen(varname) + 1, (void **) &tmp) == FAILURE) {
		*result = 0;
		return FAILURE;
	}
	var = *tmp;
	zval_copy_ctor(&var);
	convert_to_long(&var);
	*result = Z_LVAL(var);
	return SUCCESS;
}
/* }}} */

/* {{{ cfg_get_double
 */
PHPAPI int cfg_get_double(const char *varname, double *result)
{
	zval *tmp, var;

	if (zend_hash_find(&configuration_hash, varname, strlen(varname) + 1, (void **) &tmp) == FAILURE) {
		*result = (double) 0;
		return FAILURE;
	}
	var = *tmp;
	zval_copy_ctor(&var);
	convert_to_double(&var);
	*result = Z_DVAL(var);
	return SUCCESS;
}
/* }}} */

/* {{{ cfg_get_string
 */
PHPAPI int cfg_get_string(const char *varname, char **result)
{
	zval *tmp;

	if (zend_hash_find(&configuration_hash, varname, strlen(varname)+1, (void **) &tmp) == FAILURE) {
		*result = NULL;
		return FAILURE;
	}
	*result = Z_STRVAL_P(tmp);
	return SUCCESS;
}
/* }}} */

PHPAPI HashTable* php_ini_get_configuration_hash(void) /* {{{ */
{
	return &configuration_hash;
} /* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
