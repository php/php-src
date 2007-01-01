/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2007 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Alex Plotnick <alex@wgate.com>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_LIBINTL

#include <stdio.h>
#include <libintl.h>
#include "ext/standard/info.h"
#include "php_gettext.h"

/* {{{ php_gettext_functions[]
 */
function_entry php_gettext_functions[] = {
	PHP_NAMED_FE(textdomain,		zif_textdomain,		NULL)
	PHP_NAMED_FE(gettext,			zif_gettext,		NULL)
	/* Alias for gettext() */
	PHP_NAMED_FE(_,					zif_gettext,		NULL)
	PHP_NAMED_FE(dgettext,			zif_dgettext,		NULL)
	PHP_NAMED_FE(dcgettext,			zif_dcgettext,		NULL)
	PHP_NAMED_FE(bindtextdomain,	zif_bindtextdomain,	NULL)
#if HAVE_NGETTEXT
	PHP_NAMED_FE(ngettext,			zif_ngettext,		NULL)
#endif
#if HAVE_DNGETTEXT
	PHP_NAMED_FE(dngettext,			zif_dngettext,		NULL)
#endif
#if HAVE_DCNGETTEXT
	PHP_NAMED_FE(dcngettext,		zif_dcngettext,		NULL)
#endif
#if HAVE_BIND_TEXTDOMAIN_CODESET
	PHP_NAMED_FE(bind_textdomain_codeset,	zif_bind_textdomain_codeset,	NULL)
#endif
    {NULL, NULL, NULL}
};
/* }}} */

zend_module_entry php_gettext_module_entry = {
	STANDARD_MODULE_HEADER,
	"gettext",
	php_gettext_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(php_gettext),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_GETTEXT
ZEND_GET_MODULE(php_gettext)
#endif

PHP_MINFO_FUNCTION(php_gettext)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "GetText Support", "enabled");
	php_info_print_table_end();
}

/* {{{ proto string textdomain(string domain)
   Set the textdomain to "domain". Returns the current domain */
PHP_NAMED_FUNCTION(zif_textdomain)
{
	zval **domain;
	char *domain_name, *retval;
	char *val;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &domain) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(domain);

	val = Z_STRVAL_PP(domain);
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
PHP_NAMED_FUNCTION(zif_gettext)
{
	zval **msgid;
	char *msgstr;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &msgid) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(msgid);

	msgstr = gettext(Z_STRVAL_PP(msgid));

	RETURN_STRING(msgstr, 1);
}
/* }}} */

/* {{{ proto string dgettext(string domain_name, string msgid)
   Return the translation of msgid for domain_name, or msgid unaltered if a translation does not exist */
PHP_NAMED_FUNCTION(zif_dgettext)
{
	zval **domain_name, **msgid;
	char *msgstr;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &domain_name, &msgid) == FAILURE)	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(domain_name);
	convert_to_string_ex(msgid);

	msgstr = dgettext(Z_STRVAL_PP(domain_name), Z_STRVAL_PP(msgid));

	RETURN_STRING(msgstr, 1);
}
/* }}} */

/* {{{ proto string dcgettext(string domain_name, string msgid, long category)
   Return the translation of msgid for domain_name and category, or msgid unaltered if a translation does not exist */
PHP_NAMED_FUNCTION(zif_dcgettext)
{
	zval **domain_name, **msgid, **category;
	char *msgstr;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &domain_name, &msgid, &category) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(domain_name);
	convert_to_string_ex(msgid);
	convert_to_long_ex(category);

	msgstr = dcgettext(Z_STRVAL_PP(domain_name), Z_STRVAL_PP(msgid), Z_LVAL_PP(category));

	RETURN_STRING(msgstr, 1);
}
/* }}} */

/* {{{ proto string bindtextdomain(string domain_name, string dir)
   Bind to the text domain domain_name, looking for translations in dir. Returns the current domain */
PHP_NAMED_FUNCTION(zif_bindtextdomain)
{
	zval **domain_name, **dir;
	char *retval, dir_name[MAXPATHLEN];

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &domain_name, &dir) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(domain_name);
	convert_to_string_ex(dir);

	if (Z_STRVAL_PP(domain_name)[0] == '\0') {
		php_error(E_WARNING, "The first parameter of bindtextdomain must not be empty");
		RETURN_FALSE;
	}
	
	if (Z_STRVAL_PP(dir)[0] != '\0' && strcmp(Z_STRVAL_PP(dir), "0")) {
		VCWD_REALPATH(Z_STRVAL_PP(dir), dir_name);
	} else {
		VCWD_GETCWD(dir_name, MAXPATHLEN);
	}

	retval = bindtextdomain(Z_STRVAL_PP(domain_name), dir_name);

	RETURN_STRING(retval, 1);
}
/* }}} */

#if HAVE_NGETTEXT
/* {{{ proto string ngettext(string MSGID1, string MSGID2, int N)
   Plural version of gettext() */
PHP_NAMED_FUNCTION(zif_ngettext)
{
	zval **msgid1, **msgid2, **count;
	char *msgstr;

	RETVAL_FALSE;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &msgid1, &msgid2, &count) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_string_ex(msgid1);
		convert_to_string_ex(msgid2);
		convert_to_long_ex(count);

		msgstr = ngettext(Z_STRVAL_PP(msgid1), Z_STRVAL_PP(msgid2), Z_LVAL_PP(count));
		if (msgstr) {
			RETVAL_STRING (msgstr, 1);
		}
	}
}
/* }}} */
#endif

#if HAVE_DNGETTEXT
/* {{{ proto string dngettext (string domain, string msgid1, string msgid2, int count)
   Plural version of dgettext() */
PHP_NAMED_FUNCTION(zif_dngettext)
{
	zval **domain, **msgid1, **msgid2, **count;

	RETVAL_FALSE;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &domain, &msgid1, &msgid2, &count) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		char *msgstr;
		
		convert_to_string_ex(domain);
		convert_to_string_ex(msgid1);
		convert_to_string_ex(msgid2);
		convert_to_long_ex(count);

		msgstr = dngettext(Z_STRVAL_PP(domain), Z_STRVAL_PP(msgid1), Z_STRVAL_PP(msgid2), Z_LVAL_PP(count));
		if (msgstr) {
			RETVAL_STRING(msgstr, 1);
		}
	}
}
/* }}} */
#endif

#if HAVE_DCNGETTEXT
/* {{{ proto string dcngettext (string domain, string msgid1, string msgid2, int n, int category)
   Plural version of dcgettext() */								
PHP_NAMED_FUNCTION(zif_dcngettext)
{
	zval **domain, **msgid1, **msgid2, **count, **category;

	RETVAL_FALSE;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(4, &domain, &msgid1, &msgid2, &count, &category) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		char* msgstr = NULL;

		convert_to_string_ex(domain);
		convert_to_string_ex(msgid1);
		convert_to_string_ex(msgid2);
		convert_to_long_ex(count);
		convert_to_long_ex(category);

		msgstr = dcngettext(Z_STRVAL_PP(domain), Z_STRVAL_PP(msgid1), Z_STRVAL_PP(msgid2), Z_LVAL_PP(count), Z_LVAL_PP(category));

		if (msgstr) {
			RETVAL_STRING(msgstr, 1);
		}
	}
}
/* }}} */
#endif

#if HAVE_BIND_TEXTDOMAIN_CODESET

/* {{{ proto string bind_textdomain_codeset (string domain, string codeset)
   Specify the character encoding in which the messages from the DOMAIN message catalog will be returned. */
PHP_NAMED_FUNCTION(zif_bind_textdomain_codeset)
{
	zval **domain, **codeset;
	char *retval;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &domain, &codeset) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_string_ex(domain);
		convert_to_string_ex(codeset);
		
		retval = bind_textdomain_codeset(Z_STRVAL_PP(domain), Z_STRVAL_PP(codeset));

		if (!retval) {
			RETURN_FALSE;
		}
		RETURN_STRING(retval, 1);
	}
}
/* }}} */
#endif


#endif /* HAVE_LIBINTL */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */

