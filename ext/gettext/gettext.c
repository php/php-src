/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
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

function_entry php_gettext_functions[] = {
    PHP_FE(textdomain,								NULL)
    PHP_FE(gettext,									NULL)
    PHP_FALIAS(_,				gettext,			NULL)
	PHP_FE(dgettext,								NULL)
	PHP_FE(dcgettext,								NULL)
	PHP_FE(bindtextdomain,							NULL)
    {NULL, NULL, NULL}
};

zend_module_entry php_gettext_module_entry = {
	"gettext", php_gettext_functions, NULL, NULL, NULL, NULL, PHP_MINFO(gettext), STANDARD_MODULE_PROPERTIES
};

PHP_MINFO_FUNCTION(gettext)
{
	php_printf("GNU gettext support active.");
}

PHP_FUNCTION(textdomain)
{
    pval **domain;
    char *domain_name, *retval;
	char *val;

    if (ARG_COUNT(ht) != 1 || getParametersEx(1, &domain) == FAILURE) {
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

PHP_FUNCTION(gettext)
{
    pval **msgid;
    char *msgstr;

    if (ARG_COUNT(ht) != 1 || getParametersEx(1, &msgid) == FAILURE) {
		WRONG_PARAM_COUNT;
    }
    convert_to_string_ex(msgid);

    msgstr = gettext((*msgid)->value.str.val);

    RETURN_STRING(msgstr, 1);
}

PHP_FUNCTION(dgettext)
{
	pval **domain_name, **msgid;
	char *msgstr;

	if (ARG_COUNT(ht) != 2
		|| getParametersEx(2, &domain_name, &msgid) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(domain_name);
	convert_to_string_ex(msgid);

	msgstr = dgettext((*domain_name)->value.str.val, (*msgid)->value.str.val);

	RETURN_STRING(msgstr, 1);
}

PHP_FUNCTION(dcgettext)
{
	pval **domain_name, **msgid, **category;
	char *msgstr;

	if (ARG_COUNT(ht) != 3
		|| getParametersEx(3, &domain_name, &msgid, &category) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(domain_name);
	convert_to_string_ex(msgid);
	convert_to_long_ex(category);

	msgstr = dcgettext((*domain_name)->value.str.val,
					   (*msgid)->value.str.val,
					   (*category)->value.lval);

	RETURN_STRING(msgstr, 1);
}

PHP_FUNCTION(bindtextdomain)
{
	pval **domain_name, **dir;
	char *retval, *dir_name;
	char *val;

	if (ARG_COUNT(ht) != 2
		|| getParametersEx(2, &domain_name, &dir) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(domain_name);
	convert_to_string_ex(dir);

	val = (*dir)->value.str.val;
	if (strcmp(val, "") && strcmp(val, "0")) {
		dir_name = val;
	} else {
		dir_name = NULL;
	}

	retval = bindtextdomain((*domain_name)->value.str.val, dir_name);

	RETURN_STRING(retval, 1);
}

#endif /* HAVE_LIBINTL */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
