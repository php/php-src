/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Alex Plotnick <alex@wgate.com>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include <stdio.h>
#include "php.h"
#include "php_gettext.h"

#if HAVE_LIBINTL

#include <libintl.h>
#include "ext/standard/info.h"

function_entry php_gettext_functions[] = {
	PHP_FE(textdomain,			NULL)
	PHP_FE(gettext,				NULL)
	PHP_FALIAS(_,	gettext,	NULL)
	PHP_FE(dgettext,			NULL)
	PHP_FE(dcgettext,			NULL)
	PHP_FE(bindtextdomain,		NULL)
    {NULL, NULL, NULL}
};

zend_module_entry php_gettext_module_entry = {
	"gettext", php_gettext_functions, PHP_MINIT(gettext), NULL, NULL, NULL, PHP_MINFO(gettext), STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_GETTEXT
ZEND_GET_MODULE(php_gettext)
#endif

PHP_MINFO_FUNCTION(gettext)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "GNU GetText Support", "enabled");
	php_info_print_table_end();
}


PHP_MINIT_FUNCTION(gettext)
{
	REGISTER_LONG_CONSTANT("LC_CTYPE", LC_CTYPE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_NUMERIC", LC_NUMERIC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_TIME", LC_TIME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_COLLATE", LC_COLLATE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_MONETARY", LC_MONETARY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_MESSAGES", LC_MESSAGES, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LC_ALL", LC_ALL, CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}

/* {{{ proto string textdomain(string domain)
   Set the textdomain to "domain". Returns the current domain */
PHP_FUNCTION(textdomain)
{
	pval **domain;
	char *domain_name, *retval;
	char *val;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &domain) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(domain);

	val = (*domain)->value.str.val;
	if (strcmp(val, "") && strcmp(val, "0")) {
		domain_name = val;
	} else {
		domain_name = NULL;
	}

	retval = textdomain(domain_name);

	RETURN_STRING(retval, 1);
}
/* }}} */

/* {{{ proto string gettext(string msgid)
   Return the translation of msgid for the current domain, or msgid unaltered if a translation does not exist */
PHP_FUNCTION(gettext)
{
	pval **msgid;
	char *msgstr;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &msgid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(msgid);

	msgstr = gettext((*msgid)->value.str.val);

	RETURN_STRING(msgstr, 1);
}
/* }}} */

/* {{{ proto string dgettext(string domain_name, string msgid)
   Return the translation of msgid for domain_name, or msgid unaltered if a translation does not exist */
PHP_FUNCTION(dgettext)
{
	pval **domain_name, **msgid;
	char *msgstr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &domain_name, &msgid) == FAILURE)	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(domain_name);
	convert_to_string_ex(msgid);

	msgstr = dgettext((*domain_name)->value.str.val, (*msgid)->value.str.val);

	RETURN_STRING(msgstr, 1);
}
/* }}} */

/* {{{ proto string dcgettext(string domain_name, string msgid, long category)
   Return the translation of msgid for domain_name and category, or msgid unaltered if a translation does not exist */
PHP_FUNCTION(dcgettext)
{
	pval **domain_name, **msgid, **category;
	char *msgstr;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &domain_name, &msgid, &category) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(domain_name);
	convert_to_string_ex(msgid);
	convert_to_long_ex(category);

	msgstr = dcgettext(	(*domain_name)->value.str.val,
						(*msgid)->value.str.val,
						(*category)->value.lval);

	RETURN_STRING(msgstr, 1);
}
/* }}} */

/* {{{ proto string bindtextdomain(string domain_name, string dir)
   Bind to the text domain domain_name, looking for translations in dir. Returns the current domain */
PHP_FUNCTION(bindtextdomain)
{
	pval **domain_name, **dir;
	char *retval, dir_name[MAXPATHLEN];

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &domain_name, &dir) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(domain_name);
	convert_to_string_ex(dir);

	if (strcmp((*dir)->value.str.val, "") && strcmp((*dir)->value.str.val, "0")) {
		V_REALPATH((*dir)->value.str.val, dir_name);
	} else {
		V_GETCWD(dir_name, MAXPATHLEN);
	}

	retval = bindtextdomain((*domain_name)->value.str.val, dir_name);

	RETURN_STRING(retval, 1);
}
/* }}} */

#endif /* HAVE_LIBINTL */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
