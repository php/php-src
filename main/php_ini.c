/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@php.net>                                  |
   +----------------------------------------------------------------------+
 */

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
#include "win32/winutil.h"
#endif

#if HAVE_SCANDIR && HAVE_ALPHASORT && HAVE_DIRENT_H
#include <dirent.h>
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

/* {{{ php_ini_displayer_cb */
static ZEND_COLD void php_ini_displayer_cb(zend_ini_entry *ini_entry, int type)
{
	if (ini_entry->displayer) {
		ini_entry->displayer(ini_entry, type);
	} else {
		char *display_string;
		size_t display_string_length;
		int esc_html=0;

		if (type == ZEND_INI_DISPLAY_ORIG && ini_entry->modified) {
			if (ini_entry->orig_value && ZSTR_VAL(ini_entry->orig_value)[0]) {
				display_string = ZSTR_VAL(ini_entry->orig_value);
				display_string_length = ZSTR_LEN(ini_entry->orig_value);
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
		} else if (ini_entry->value && ZSTR_VAL(ini_entry->value)[0]) {
			display_string = ZSTR_VAL(ini_entry->value);
			display_string_length = ZSTR_LEN(ini_entry->value);
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
			php_html_puts(display_string, display_string_length);
		} else {
			PHPWRITE(display_string, display_string_length);
		}
	}
}
/* }}} */

/* {{{ display_ini_entries */
PHPAPI ZEND_COLD void display_ini_entries(zend_module_entry *module)
{
	int module_number;
	zend_ini_entry *ini_entry;
	bool first = 1;

	if (module) {
		module_number = module->module_number;
	} else {
		module_number = 0;
	}

	ZEND_HASH_MAP_FOREACH_PTR(EG(ini_directives), ini_entry) {
		if (ini_entry->module_number != module_number) {
			continue;
		}
		if (first) {
			php_info_print_table_start();
			php_info_print_table_header(3, "Directive", "Local Value", "Master Value");
			first = 0;
		}
		if (!sapi_module.phpinfo_as_text) {
			PUTS("<tr>");
			PUTS("<td class=\"e\">");
			PHPWRITE(ZSTR_VAL(ini_entry->name), ZSTR_LEN(ini_entry->name));
			PUTS("</td><td class=\"v\">");
			php_ini_displayer_cb(ini_entry, ZEND_INI_DISPLAY_ACTIVE);
			PUTS("</td><td class=\"v\">");
			php_ini_displayer_cb(ini_entry, ZEND_INI_DISPLAY_ORIG);
			PUTS("</td></tr>\n");
		} else {
			PHPWRITE(ZSTR_VAL(ini_entry->name), ZSTR_LEN(ini_entry->name));
			PUTS(" => ");
			php_ini_displayer_cb(ini_entry, ZEND_INI_DISPLAY_ACTIVE);
			PUTS(" => ");
			php_ini_displayer_cb(ini_entry, ZEND_INI_DISPLAY_ORIG);
			PUTS("\n");
		}
	} ZEND_HASH_FOREACH_END();
	if (!first) {
		php_info_print_table_end();
	}
}
/* }}} */

/* php.ini support */
#define PHP_EXTENSION_TOKEN		"extension"
#define ZEND_EXTENSION_TOKEN	"zend_extension"

/* {{{ config_zval_dtor */
PHPAPI void config_zval_dtor(zval *zvalue)
{
	if (Z_TYPE_P(zvalue) == IS_ARRAY) {
		zend_hash_destroy(Z_ARRVAL_P(zvalue));
		free(Z_ARR_P(zvalue));
	} else if (Z_TYPE_P(zvalue) == IS_STRING) {
		zend_string_release_ex(Z_STR_P(zvalue), 1);
	}
}
/* Reset / free active_ini_sectin global */
#define RESET_ACTIVE_INI_HASH() do { \
	active_ini_hash = NULL;          \
	is_special_section = 0;          \
} while (0)
/* }}} */

/* {{{ php_ini_parser_cb */
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
				if (!is_special_section && zend_string_equals_literal_ci(Z_STR_P(arg1), PHP_EXTENSION_TOKEN)) { /* load PHP extension */
					extension_name = estrndup(Z_STRVAL_P(arg2), Z_STRLEN_P(arg2));
					zend_llist_add_element(&extension_lists.functions, &extension_name);
				} else if (!is_special_section && zend_string_equals_literal_ci(Z_STR_P(arg1), ZEND_EXTENSION_TOKEN)) { /* load Zend extension */
					extension_name = estrndup(Z_STRVAL_P(arg2), Z_STRLEN_P(arg2));
					zend_llist_add_element(&extension_lists.engine, &extension_name);

				/* All other entries are added into either configuration_hash or active ini section array */
				} else {
					/* Store in active hash */
					entry = zend_hash_update(active_hash, Z_STR_P(arg1), arg2);
					Z_STR_P(entry) = zend_string_dup(Z_STR_P(entry), 1);
				}
			}
			break;

		case ZEND_INI_PARSER_POP_ENTRY: {
				zval option_arr;
				zval *find_arr;

				if (!arg2) {
					/* bare string - nothing to do */
					break;
				}

/* fprintf(stdout, "ZEND_INI_PARSER_POP_ENTRY: %s[%s] = %s\n",Z_STRVAL_P(arg1), Z_STRVAL_P(arg3), Z_STRVAL_P(arg2)); */

				/* If option not found in hash or is not an array -> create array, otherwise add to existing array */
				if ((find_arr = zend_hash_find(active_hash, Z_STR_P(arg1))) == NULL || Z_TYPE_P(find_arr) != IS_ARRAY) {
					ZVAL_NEW_PERSISTENT_ARR(&option_arr);
					zend_hash_init(Z_ARRVAL(option_arr), 8, NULL, config_zval_dtor, 1);
					find_arr = zend_hash_update(active_hash, Z_STR_P(arg1), &option_arr);
				}

				/* arg3 is possible option offset name */
				if (arg3 && Z_STRLEN_P(arg3) > 0) {
					entry = zend_symtable_update(Z_ARRVAL_P(find_arr), Z_STR_P(arg3), arg2);
				} else {
					entry = zend_hash_next_index_insert(Z_ARRVAL_P(find_arr), arg2);
				}
				Z_STR_P(entry) = zend_string_dup(Z_STR_P(entry), 1);
			}
			break;

		case ZEND_INI_PARSER_SECTION: { /* Create an array of entries of each section */

/* fprintf(stdout, "ZEND_INI_PARSER_SECTION: %s\n",Z_STRVAL_P(arg1)); */

				char *key = NULL;
				size_t key_len;

				/* PATH sections */
				if (!zend_binary_strncasecmp(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1), "PATH", sizeof("PATH") - 1, sizeof("PATH") - 1)) {
					key = Z_STRVAL_P(arg1);
					key = key + sizeof("PATH") - 1;
					key_len = Z_STRLEN_P(arg1) - sizeof("PATH") + 1;
					is_special_section = 1;
					has_per_dir_config = 1;

					/* make the path lowercase on Windows, for case insensitivity. Does nothing for other platforms */
					TRANSLATE_SLASHES_LOWER(key);

				/* HOST sections */
				} else if (!zend_binary_strncasecmp(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1), "HOST", sizeof("HOST") - 1, sizeof("HOST") - 1)) {
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
					if ((entry = zend_hash_str_find(target_hash, key, key_len)) == NULL) {
						zval section_arr;

						ZVAL_NEW_PERSISTENT_ARR(&section_arr);
						zend_hash_init(Z_ARRVAL(section_arr), 8, NULL, (dtor_func_t) config_zval_dtor, 1);
						entry = zend_hash_str_update(target_hash, key, key_len, &section_arr);
					}
					if (Z_TYPE_P(entry) == IS_ARRAY) {
						active_ini_hash = Z_ARRVAL_P(entry);
					}
				}
			}
			break;
	}
}
/* }}} */

/* {{{ php_load_php_extension_cb */
static void php_load_php_extension_cb(void *arg)
{
#ifdef HAVE_LIBDL
	php_load_extension(*((char **) arg), MODULE_PERSISTENT, 0);
#endif
}
/* }}} */

/* {{{ php_load_zend_extension_cb */
#ifdef HAVE_LIBDL
static void php_load_zend_extension_cb(void *arg)
{
	char *filename = *((char **) arg);
	const size_t length = strlen(filename);

#ifndef PHP_WIN32
	(void) length;
#endif

	if (IS_ABSOLUTE_PATH(filename, length)) {
		zend_load_extension(filename);
	} else {
		DL_HANDLE handle;
		char *libpath;
		char *extension_dir = INI_STR("extension_dir");
		int slash_suffix = 0;
		char *err1, *err2;

		if (extension_dir && extension_dir[0]) {
			slash_suffix = IS_SLASH(extension_dir[strlen(extension_dir)-1]);
		}

		/* Try as filename first */
		if (slash_suffix) {
			spprintf(&libpath, 0, "%s%s", extension_dir, filename); /* SAFE */
		} else {
			spprintf(&libpath, 0, "%s%c%s", extension_dir, DEFAULT_SLASH, filename); /* SAFE */
		}

		handle = (DL_HANDLE)php_load_shlib(libpath, &err1);
		if (!handle) {
			/* If file does not exist, consider as extension name and build file name */
			char *orig_libpath = libpath;

			if (slash_suffix) {
				spprintf(&libpath, 0, "%s" PHP_SHLIB_EXT_PREFIX "%s." PHP_SHLIB_SUFFIX, extension_dir, filename); /* SAFE */
			} else {
				spprintf(&libpath, 0, "%s%c" PHP_SHLIB_EXT_PREFIX "%s." PHP_SHLIB_SUFFIX, extension_dir, DEFAULT_SLASH, filename); /* SAFE */
			}

			handle = (DL_HANDLE)php_load_shlib(libpath, &err2);
			if (!handle) {
				php_error(E_CORE_WARNING, "Failed loading Zend extension '%s' (tried: %s (%s), %s (%s))",
					filename, orig_libpath, err1, libpath, err2);
				efree(orig_libpath);
				efree(err1);
				efree(libpath);
				efree(err2);
				return;
			}

			efree(orig_libpath);
			efree(err1);
		}

#ifdef PHP_WIN32
		if (!php_win32_image_compatible(handle, &err1)) {
				php_error(E_CORE_WARNING, err1);
				efree(err1);
				efree(libpath);
				DL_UNLOAD(handle);
				return;
		}
#endif

		zend_load_extension_handle(handle, libpath);
		efree(libpath);
	}
}
#else
static void php_load_zend_extension_cb(void *arg) { }
#endif
/* }}} */

/* {{{ php_init_config */
int php_init_config(void)
{
	char *php_ini_file_name = NULL;
	char *php_ini_search_path = NULL;
	int php_ini_scanned_path_len;
	char *open_basedir;
	int free_ini_search_path = 0;
	zend_string *opened_path = NULL;

	zend_hash_init(&configuration_hash, 8, NULL, config_zval_dtor, 1);

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

			/*If the given buffer is not large enough to hold the data, the return value is
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

		search_path_size = MAXPATHLEN * 4 + (int)strlen(env_location) + 3 + 1;
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

	FILE *fp = NULL;
	char *filename = NULL;
	bool free_filename = false;

	/* If SAPI does not want to ignore all ini files OR an overriding file/path is given.
	 * This allows disabling scanning for ini files in the PHP_CONFIG_FILE_SCAN_DIR but still
	 * load an optional ini file. */
	if (!sapi_module.php_ini_ignore || sapi_module.php_ini_path_override) {

		/* Check if php_ini_file_name is a file and can be opened */
		if (php_ini_file_name && php_ini_file_name[0]) {
			zend_stat_t statbuf = {0};

			if (!VCWD_STAT(php_ini_file_name, &statbuf)) {
				if (!((statbuf.st_mode & S_IFMT) == S_IFDIR)) {
					fp = VCWD_FOPEN(php_ini_file_name, "r");
					if (fp) {
						filename = expand_filepath(php_ini_file_name, NULL);
						free_filename = true;
					}
				}
			}
		}

		/* Otherwise search for php-%sapi-module-name%.ini file in search path */
		if (!fp) {
			const char *fmt = "php-%s.ini";
			char *ini_fname;
			spprintf(&ini_fname, 0, fmt, sapi_module.name);
			fp = php_fopen_with_path(ini_fname, "r", php_ini_search_path, &opened_path);
			efree(ini_fname);
			if (fp) {
				filename = ZSTR_VAL(opened_path);
			}
		}

		/* If still no ini file found, search for php.ini file in search path */
		if (!fp) {
			fp = php_fopen_with_path("php.ini", "r", php_ini_search_path, &opened_path);
			if (fp) {
				filename = ZSTR_VAL(opened_path);
			}
		}
	}

	if (free_ini_search_path) {
		efree(php_ini_search_path);
	}

	PG(open_basedir) = open_basedir;

	if (fp) {
		zend_file_handle fh;
		zend_stream_init_fp(&fh, fp, filename);
		RESET_ACTIVE_INI_HASH();

		zend_parse_ini_file(&fh, 1, ZEND_INI_SCANNER_NORMAL, (zend_ini_parser_cb_t) php_ini_parser_cb, &configuration_hash);

		{
			zval tmp;

			ZVAL_NEW_STR(&tmp, zend_string_init(filename, strlen(filename), 1));
			zend_hash_str_update(&configuration_hash, "cfg_file_path", sizeof("cfg_file_path")-1, &tmp);
			if (opened_path) {
				zend_string_release_ex(opened_path, 0);
			}
			php_ini_opened_path = zend_strndup(Z_STRVAL(tmp), Z_STRLEN(tmp));
		}
		zend_destroy_file_handle(&fh);

		if (free_filename) {
			efree(filename);
		}
	}

	/* Check for PHP_INI_SCAN_DIR environment variable to override/set config file scan directory */
	php_ini_scanned_path = getenv("PHP_INI_SCAN_DIR");
	if (!php_ini_scanned_path) {
		/* Or fall back using possible --with-config-file-scan-dir setting (defaults to empty string!) */
		php_ini_scanned_path = PHP_CONFIG_FILE_SCAN_DIR;
	}
	php_ini_scanned_path_len = (int)strlen(php_ini_scanned_path);

	/* Scan and parse any .ini files found in scan path if path not empty. */
	if (!sapi_module.php_ini_ignore && php_ini_scanned_path_len) {
		struct dirent **namelist;
		int ndir, i;
		zend_stat_t sb = {0};
		char ini_file[MAXPATHLEN];
		char *p;
		zend_llist scanned_ini_list;
		zend_llist_element *element;
		int l, total_l = 0;
		char *bufpath, *debpath, *endpath;
		int lenpath;

		zend_llist_init(&scanned_ini_list, sizeof(char *), (llist_dtor_func_t) free_estring, 1);

		bufpath = estrdup(php_ini_scanned_path);
		for (debpath = bufpath ; debpath ; debpath=endpath) {
			endpath = strchr(debpath, DEFAULT_DIR_SEPARATOR);
			if (endpath) {
				*(endpath++) = 0;
			}
			if (!debpath[0]) {
				/* empty string means default builtin value
				   to allow "/foo/php.d:" or ":/foo/php.d" */
				debpath = PHP_CONFIG_FILE_SCAN_DIR;
			}
			lenpath = (int)strlen(debpath);

			if (lenpath > 0 && (ndir = php_scandir(debpath, &namelist, 0, php_alphasort)) > 0) {

				for (i = 0; i < ndir; i++) {

					/* check for any file with .ini extension */
					if (!(p = strrchr(namelist[i]->d_name, '.')) || (p && strcmp(p, ".ini"))) {
						free(namelist[i]);
						continue;
					}
					/* Reset active ini section */
					RESET_ACTIVE_INI_HASH();

					if (IS_SLASH(debpath[lenpath - 1])) {
						snprintf(ini_file, MAXPATHLEN, "%s%s", debpath, namelist[i]->d_name);
					} else {
						snprintf(ini_file, MAXPATHLEN, "%s%c%s", debpath, DEFAULT_SLASH, namelist[i]->d_name);
					}
					if (VCWD_STAT(ini_file, &sb) == 0) {
						if (S_ISREG(sb.st_mode)) {
							zend_file_handle fh;
							FILE *file = VCWD_FOPEN(ini_file, "r");
							if (file) {
								zend_stream_init_fp(&fh, file, ini_file);
								if (zend_parse_ini_file(&fh, 1, ZEND_INI_SCANNER_NORMAL, (zend_ini_parser_cb_t) php_ini_parser_cb, &configuration_hash) == SUCCESS) {
									/* Here, add it to the list of ini files read */
									l = (int)strlen(ini_file);
									total_l += l + 2;
									p = estrndup(ini_file, l);
									zend_llist_add_element(&scanned_ini_list, &p);
								}
								zend_destroy_file_handle(&fh);
							}
						}
					}
					free(namelist[i]);
				}
				free(namelist);
			}
		}
		efree(bufpath);

		if (total_l) {
			int php_ini_scanned_files_len = (php_ini_scanned_files) ? (int)strlen(php_ini_scanned_files) + 1 : 0;
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
	} else {
		/* Make sure an empty php_ini_scanned_path ends up as NULL */
		php_ini_scanned_path = NULL;
	}

	if (sapi_module.ini_entries) {
		/* Reset active ini section */
		RESET_ACTIVE_INI_HASH();
		zend_parse_ini_string(sapi_module.ini_entries, 1, ZEND_INI_SCANNER_NORMAL, (zend_ini_parser_cb_t) php_ini_parser_cb, &configuration_hash);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ php_shutdown_config */
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

/* {{{ php_ini_register_extensions */
void php_ini_register_extensions(void)
{
	zend_llist_apply(&extension_lists.engine, php_load_zend_extension_cb);
	zend_llist_apply(&extension_lists.functions, php_load_php_extension_cb);

	zend_llist_destroy(&extension_lists.engine);
	zend_llist_destroy(&extension_lists.functions);
}
/* }}} */

/* {{{ php_parse_user_ini_file */
PHPAPI int php_parse_user_ini_file(const char *dirname, const char *ini_filename, HashTable *target_hash)
{
	zend_stat_t sb = {0};
	char ini_file[MAXPATHLEN];

	snprintf(ini_file, MAXPATHLEN, "%s%c%s", dirname, DEFAULT_SLASH, ini_filename);

	if (VCWD_STAT(ini_file, &sb) == 0) {
		if (S_ISREG(sb.st_mode)) {
			zend_file_handle fh;
			int ret = FAILURE;

			zend_stream_init_fp(&fh, VCWD_FOPEN(ini_file, "r"), ini_file);
			if (fh.handle.fp) {
				/* Reset active ini section */
				RESET_ACTIVE_INI_HASH();

#if ZEND_RC_DEBUG
				/* User inis are parsed during SAPI activate (part of the request),
				 * but persistently allocated to allow caching. This is fine as long as
				 * strings are duplicated in php_ini_activate_config(). */
				bool orig_rc_debug = zend_rc_debug;
				zend_rc_debug = false;
#endif
				ret = zend_parse_ini_file(&fh, 1, ZEND_INI_SCANNER_NORMAL, (zend_ini_parser_cb_t) php_ini_parser_cb, target_hash);
#if ZEND_RC_DEBUG
				zend_rc_debug = orig_rc_debug;
#endif
				if (ret == SUCCESS) {
					/* FIXME: Add parsed file to the list of user files read? */
				}
			}
			zend_destroy_file_handle(&fh);
			return ret;
		}
	}
	return FAILURE;
}
/* }}} */

/* {{{ php_ini_activate_config */
PHPAPI void php_ini_activate_config(HashTable *source_hash, int modify_type, int stage)
{
	zend_string *str;
	zval *data;

	/* Walk through config hash and alter matching ini entries using the values found in the hash */
	ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(source_hash, str, data) {
		zend_string *data_str = zend_string_dup(Z_STR_P(data), 0);
		zend_alter_ini_entry_ex(str, data_str, modify_type, stage, 0);
		zend_string_release(data_str);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ php_ini_has_per_dir_config */
PHPAPI int php_ini_has_per_dir_config(void)
{
	return has_per_dir_config;
}
/* }}} */

/* {{{ php_ini_activate_per_dir_config */
PHPAPI void php_ini_activate_per_dir_config(char *path, size_t path_len)
{
	zval *tmp2;
	char *ptr;

#ifdef PHP_WIN32
	char path_bak[MAXPATHLEN];
#endif

#ifdef PHP_WIN32
	/* MAX_PATH is \0-terminated, path_len == MAXPATHLEN would overrun path_bak */
	if (path_len >= MAXPATHLEN) {
#else
	if (path_len > MAXPATHLEN) {
#endif
		return;
	}

#ifdef PHP_WIN32
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
			if ((tmp2 = zend_hash_str_find(&configuration_hash, path, strlen(path))) != NULL) {
				php_ini_activate_config(Z_ARRVAL_P(tmp2), PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
			}
			*ptr = '/';
			ptr++;
		}
	}
}
/* }}} */

/* {{{ php_ini_has_per_host_config */
PHPAPI int php_ini_has_per_host_config(void)
{
	return has_per_host_config;
}
/* }}} */

/* {{{ php_ini_activate_per_host_config */
PHPAPI void php_ini_activate_per_host_config(const char *host, size_t host_len)
{
	zval *tmp;

	if (has_per_host_config && host && host_len) {
		/* Search for source array matching the host from configuration_hash */
		if ((tmp = zend_hash_str_find(&configuration_hash, host, host_len)) != NULL) {
			php_ini_activate_config(Z_ARRVAL_P(tmp), PHP_INI_SYSTEM, PHP_INI_STAGE_ACTIVATE);
		}
	}
}
/* }}} */

/* {{{ cfg_get_entry */
PHPAPI zval *cfg_get_entry_ex(zend_string *name)
{
	return zend_hash_find(&configuration_hash, name);
}
/* }}} */

/* {{{ cfg_get_entry */
PHPAPI zval *cfg_get_entry(const char *name, size_t name_length)
{
	return zend_hash_str_find(&configuration_hash, name, name_length);
}
/* }}} */

/* {{{ cfg_get_long */
PHPAPI int cfg_get_long(const char *varname, zend_long *result)
{
	zval *tmp;

	if ((tmp = zend_hash_str_find(&configuration_hash, varname, strlen(varname))) == NULL) {
		*result = 0;
		return FAILURE;
	}
	*result = zval_get_long(tmp);
	return SUCCESS;
}
/* }}} */

/* {{{ cfg_get_double */
PHPAPI int cfg_get_double(const char *varname, double *result)
{
	zval *tmp;

	if ((tmp = zend_hash_str_find(&configuration_hash, varname, strlen(varname))) == NULL) {
		*result = (double) 0;
		return FAILURE;
	}
	*result = zval_get_double(tmp);
	return SUCCESS;
}
/* }}} */

/* {{{ cfg_get_string */
PHPAPI int cfg_get_string(const char *varname, char **result)
{
	zval *tmp;

	if ((tmp = zend_hash_str_find(&configuration_hash, varname, strlen(varname))) == NULL) {
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
