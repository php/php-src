/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Author: Alex Plotnick <alex@wgate.com>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include <stdio.h>
#include "php.h"
#include "internal_functions.h"
#include "php3_gettext.h"

#if HAVE_LIBINTL

#include <libintl.h>

function_entry php3_gettext_functions[] = {
    {"textdomain",		php3_textdomain,		NULL},
    {"gettext",			php3_gettext,			NULL},
    {"_",				php3_gettext,			NULL},
	{"dgettext",		php3_dgettext,			NULL},
	{"dcgettext",		php3_dcgettext,			NULL},
	{"bindtextdomain",	php3_bindtextdomain,	NULL},
    {NULL, NULL, NULL}
};

php3_module_entry php3_gettext_module_entry = {
	"gettext", php3_gettext_functions, NULL, NULL, NULL, NULL, php3_info_gettext, STANDARD_MODULE_PROPERTIES
};

void php3_info_gettext(void)
{
	php3_printf("GNU gettext support active.");
}

void php3_textdomain(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *domain;
    char *domain_name, *retval;

    if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &domain) == FAILURE) {
		WRONG_PARAM_COUNT;
    }
    convert_to_string(domain);

	if (strcmp(domain->value.str.val, "")
		&& strcmp(domain->value.str.val, "0"))
	{
		domain_name = domain->value.str.val;
	} else {
		domain_name = NULL;
	}

	retval = textdomain(domain_name);

    RETURN_STRING(retval, 1);
}

void php3_gettext(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *msgid;
    char *msgstr;

    if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &msgid) == FAILURE) {
		WRONG_PARAM_COUNT;
    }
    convert_to_string(msgid);

    msgstr = gettext(msgid->value.str.val);

    RETURN_STRING(msgstr, 1);
}

void php3_dgettext(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *domain_name, *msgid;
	char *msgstr;

	if (ARG_COUNT(ht) != 2
		|| getParameters(ht, 2, &domain_name, &msgid) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string(domain_name);
	convert_to_string(msgid);

	msgstr = dgettext(domain_name->value.str.val, msgid->value.str.val);

	RETURN_STRING(msgstr, 1);
}

void php3_dcgettext(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *domain_name, *msgid, *category;
	char *msgstr;

	if (ARG_COUNT(ht) != 3
		|| getParameters(ht, 3, &domain_name, &msgid, &category) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string(domain_name);
	convert_to_string(msgid);
	convert_to_long(category);

	msgstr = dcgettext(domain_name->value.str.val,
					   msgid->value.str.val,
					   category->value.lval);

	RETURN_STRING(msgstr, 1);
}

void php3_bindtextdomain(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *domain_name, *dir;
	char *retval, *dir_name;

	if (ARG_COUNT(ht) != 2
		|| getParameters(ht, 2, &domain_name, &dir) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string(domain_name);
	convert_to_string(dir);

	if (strcmp(dir->value.str.val, "")
		&& strcmp(dir->value.str.val, "0"))
	{
		dir_name = dir->value.str.val;
	} else {
		dir_name = NULL;
	}

	retval = bindtextdomain(domain_name->value.str.val, dir_name);

	RETURN_STRING(retval, 1);
}

#endif /* HAVE_LIBINTL */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
