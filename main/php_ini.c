/*
   +----------------------------------------------------------------------+
   | PHP version 4.0													  |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group				   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,	  |
   | that is bundled with this package in the file LICENSE, and is		|
   | available at through the world-wide-web at						   |
   | http://www.php.net/license/2_02.txt.								 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to		  |
   | license@php.net so we can mail you a copy immediately.			   |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@zend.com>								 |
   +----------------------------------------------------------------------+
 */


#include "php.h"
#include "ext/standard/info.h"
#include "zend_ini.h"


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
	/*zend_hash_apply_with_argument(&known_directives, (int (*)(void *, void *)) zend_ini_displayer, (void *) (long) module_number); */
	php_info_print_table_end();
}

