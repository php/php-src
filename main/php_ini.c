/*
   +----------------------------------------------------------------------+
   | PHP version 4.0													  |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group				      |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,	  |
   | that is bundled with this package in the file LICENSE, and is		  |
   | available at through the world-wide-web at						      |
   | http://www.php.net/license/2_02.txt.								  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to		  |
   | license@php.net so we can mail you a copy immediately.			      |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@zend.com>								  |
   +----------------------------------------------------------------------+
 */


#include "php.h"
#include "ext/standard/info.h"
#include "zend_ini.h"
#include "php_ini.h"
#include "ext/standard/dl.h"
#include "zend_extensions.h"

static HashTable configuration_hash;
PHPAPI extern char *php_ini_path;


static void php_ini_displayer_cb(zend_ini_entry *ini_entry, int type)
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


static int php_ini_displayer(zend_ini_entry *ini_entry, int module_number)
{
	if (ini_entry->module_number != module_number) {
		return 0;
	}

	PUTS("<TR VALIGN=\"baseline\" BGCOLOR=\"" PHP_CONTENTS_COLOR "\">");
	PUTS("<TD BGCOLOR=\"" PHP_ENTRY_NAME_COLOR "\"><B>");
	PHPWRITE(ini_entry->name, ini_entry->name_length-1);
	PUTS("</B><BR></TD><TD ALIGN=\"center\">");
	php_ini_displayer_cb(ini_entry, ZEND_INI_DISPLAY_ACTIVE);
	PUTS("</TD><TD ALIGN=\"center\">");
	php_ini_displayer_cb(ini_entry, ZEND_INI_DISPLAY_ORIG);
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
	zend_ini_apply_with_argument((apply_func_arg_t) php_ini_displayer, (void *) (long) module_number);
	php_info_print_table_end();
}



/* php.ini support */

#ifdef ZTS
# if (ZEND_DEBUG)
# define ZEND_EXTENSION_TOKEN	"zend_extension_debug_ts"
# else
# define ZEND_EXTENSION_TOKEN	"zend_extension_ts"
# endif
#else
# if (ZEND_DEBUG)
# define ZEND_EXTENSION_TOKEN	"zend_extension_debug"
# else
# define ZEND_EXTENSION_TOKEN	"zend_extension"
# endif
#endif


static void pvalue_config_destructor(zval *pvalue)
{   
    if (pvalue->type == IS_STRING && pvalue->value.str.val != empty_string) {
        free(pvalue->value.str.val);
    }
}


static void php_config_ini_parser_cb(zval *arg1, zval *arg2, int callback_type, void *arg)
{
	switch (callback_type) {
		case ZEND_INI_PARSER_ENTRY: {
				zval *entry;

				if (!arg2) {
					break;
				}
				if (!strcasecmp(Z_STRVAL_P(arg1), "extension")) { /* load function module */
					zval dummy;

					php_dl(arg2, MODULE_PERSISTENT, &dummy);
				} else if (!strcasecmp(Z_STRVAL_P(arg1), ZEND_EXTENSION_TOKEN)) { /* load Zend extension */
					zend_load_extension(Z_STRVAL_P(arg2));
				} else {
					zend_hash_update(&configuration_hash, Z_STRVAL_P(arg1), Z_STRLEN_P(arg1)+1, arg2, sizeof(zval), (void **) &entry);
					Z_STRVAL_P(entry) = zend_strndup(Z_STRVAL_P(entry), Z_STRLEN_P(entry));
					php_alter_ini_entry(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1)+1, Z_STRVAL_P(arg2), Z_STRLEN_P(arg2)+1, PHP_INI_SYSTEM, PHP_INI_STAGE_STARTUP);
				}
			}
			break;
		case ZEND_INI_PARSER_SECTION:
			break;
	}
}


int php_init_config(void)
{
	PLS_FETCH();

	if (zend_hash_init(&configuration_hash, 0, NULL, (dtor_func_t) pvalue_config_destructor, 1)==FAILURE) {
		return FAILURE;
	}

#if USE_CONFIG_FILE
	{
		char *env_location,*default_location,*php_ini_search_path;
		int safe_mode_state = PG(safe_mode);
		char *open_basedir = PG(open_basedir);
		char *opened_path;
		int free_default_location=0;
		zend_file_handle fh;
		
		env_location = getenv("PHPRC");
		if (!env_location) {
			env_location="";
		}
#ifdef PHP_WIN32
		{
			if (php_ini_path) {
				default_location = php_ini_path;
			} else {
				default_location = (char *) malloc(512);
			
				if (!GetWindowsDirectory(default_location,255)) {
					default_location[0]=0;
				}
				free_default_location=1;
			}
		}
#else
		if (!php_ini_path) {
			default_location = CONFIGURATION_FILE_PATH;
		} else {
			default_location = php_ini_path;
		}
#endif

/* build a path */
		php_ini_search_path = (char *) malloc(sizeof(".")+strlen(env_location)+strlen(default_location)+2+1);

		if (!php_ini_path) {
#ifdef PHP_WIN32
			sprintf(php_ini_search_path,".;%s;%s",env_location,default_location);
#else
			sprintf(php_ini_search_path,".:%s:%s",env_location,default_location);
#endif
		} else {
			/* if path was set via -c flag, only look there */
			strcpy(php_ini_search_path,default_location);
		}
		PG(safe_mode) = 0;
		PG(open_basedir) = NULL;

		
		fh.handle.fp = php_fopen_with_path("php.ini", "r", php_ini_search_path, &opened_path);
		free(php_ini_search_path);
		if (free_default_location) {
			free(default_location);
		}
		PG(safe_mode) = safe_mode_state;
		PG(open_basedir) = open_basedir;

		if (!fh.handle.fp) {
			return SUCCESS;  /* having no configuration file is ok */
		}
		fh.type = ZEND_HANDLE_FP;
		fh.filename = opened_path;

		zend_parse_ini_file(&fh, 1, php_config_ini_parser_cb, NULL);

		if (opened_path) {
			zval tmp;
			
			tmp.value.str.val = strdup(opened_path);
			tmp.value.str.len = strlen(opened_path);
			tmp.type = IS_STRING;
			zend_hash_update(&configuration_hash,"cfg_file_path",sizeof("cfg_file_path"),(void *) &tmp,sizeof(zval),NULL);
#if DEBUG_CFG_PARSER
			php_printf("INI file opened at '%s'\n",opened_path);
#endif
			efree(opened_path);
		}
	}
	
#endif
	
	return SUCCESS;
}


int php_shutdown_config(void)
{
	zend_hash_destroy(&configuration_hash);
	return SUCCESS;
}


zval *cfg_get_entry(char *name, uint name_length)
{
	zval *tmp;

	if (zend_hash_find(&configuration_hash, name, name_length, (void **) &tmp)==SUCCESS) {
		return tmp;
	} else {
		return NULL;
	}
}


PHPAPI int cfg_get_long(char *varname,long *result)
{
	zval *tmp,var;
	
	if (zend_hash_find(&configuration_hash,varname,strlen(varname)+1,(void **) &tmp)==FAILURE) {
		*result=(long)NULL;
		return FAILURE;
	}
	var = *tmp;
	zval_copy_ctor(&var);
	convert_to_long(&var);
	*result = var.value.lval;
	return SUCCESS;
}


PHPAPI int cfg_get_double(char *varname,double *result)
{
	zval *tmp,var;
	
	if (zend_hash_find(&configuration_hash,varname,strlen(varname)+1,(void **) &tmp)==FAILURE) {
		*result=(double)0;
		return FAILURE;
	}
	var = *tmp;
	zval_copy_ctor(&var);
	convert_to_double(&var);
	*result = var.value.dval;
	return SUCCESS;
}


PHPAPI int cfg_get_string(char *varname, char **result)
{
	zval *tmp;

	if (zend_hash_find(&configuration_hash,varname,strlen(varname)+1,(void **) &tmp)==FAILURE) {
		*result=NULL;
		return FAILURE;
	}
	*result = tmp->value.str.val;
	return SUCCESS;
}
