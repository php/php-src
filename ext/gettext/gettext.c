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
   | Author: Alex Plotnick <alex@wgate.com>                               |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"

#ifdef HAVE_LIBINTL

#include <stdio.h>
#include <locale.h>
#include "ext/standard/info.h"
#include "php_gettext.h"
#include "gettext_arginfo.h"

#include <libintl.h>

zend_module_entry php_gettext_module_entry = {
	STANDARD_MODULE_HEADER,
	"gettext",
	ext_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(php_gettext),
	PHP_GETTEXT_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_GETTEXT
ZEND_GET_MODULE(php_gettext)
#endif

#define PHP_GETTEXT_MAX_DOMAIN_LENGTH 1024
#define PHP_GETTEXT_MAX_MSGID_LENGTH 4096

#define PHP_GETTEXT_DOMAIN_LENGTH_CHECK(_arg_num, domain_len) \
	if (UNEXPECTED(domain_len > PHP_GETTEXT_MAX_DOMAIN_LENGTH)) { \
		zend_argument_value_error(_arg_num, "is too long"); \
		RETURN_THROWS(); \
	} else if (domain_len == 0) { \
		zend_argument_must_not_be_empty_error(_arg_num); \
		RETURN_THROWS(); \
	}

#define PHP_GETTEXT_LENGTH_CHECK(_arg_num, check_len) \
	if (UNEXPECTED(check_len > PHP_GETTEXT_MAX_MSGID_LENGTH)) { \
		zend_argument_value_error(_arg_num, "is too long"); \
		RETURN_THROWS(); \
	}

#define PHP_DCGETTEXT_CATEGORY_CHECK(_arg_num, category) \
	if (category == LC_ALL) { \
		zend_argument_value_error(_arg_num, "cannot be LC_ALL"); \
		RETURN_THROWS(); \
	}

PHP_MINFO_FUNCTION(php_gettext)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "GetText Support", "enabled");
	php_info_print_table_end();
}

/* {{{ Set the textdomain to "domain". Returns the current domain */
PHP_FUNCTION(textdomain)
{
	char *domain_name = NULL, *retval = NULL;
	zend_string *domain = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(domain)
	ZEND_PARSE_PARAMETERS_END();

	if (domain != NULL) {
		PHP_GETTEXT_DOMAIN_LENGTH_CHECK(1, ZSTR_LEN(domain))
		if (zend_string_equals_literal(domain, "0")) {
			zend_argument_value_error(1, "cannot be zero");
			RETURN_THROWS();
		}
		domain_name = ZSTR_VAL(domain);
	}

	retval = textdomain(domain_name);

	RETURN_STRING(retval);
}
/* }}} */

/* {{{ Return the translation of msgid for the current domain, or msgid unaltered if a translation does not exist */
PHP_FUNCTION(gettext)
{
	char *msgstr = NULL;
	zend_string *msgid;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(msgid)
	ZEND_PARSE_PARAMETERS_END();

	PHP_GETTEXT_LENGTH_CHECK(1, ZSTR_LEN(msgid))
	msgstr = gettext(ZSTR_VAL(msgid));

	if (msgstr != ZSTR_VAL(msgid)) {
		RETURN_STRING(msgstr);
	} else {
		RETURN_STR_COPY(msgid);
	}
}
/* }}} */

/* {{{ Return the translation of msgid for domain_name, or msgid unaltered if a translation does not exist */
PHP_FUNCTION(dgettext)
{
	char *msgstr = NULL;
	zend_string *domain, *msgid;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR(domain)
		Z_PARAM_STR(msgid)
	ZEND_PARSE_PARAMETERS_END();

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK(1, ZSTR_LEN(domain))
	PHP_GETTEXT_LENGTH_CHECK(2, ZSTR_LEN(msgid))

	msgstr = dgettext(ZSTR_VAL(domain), ZSTR_VAL(msgid));

	if (msgstr != ZSTR_VAL(msgid)) {
		RETURN_STRING(msgstr);
	} else {
		RETURN_STR_COPY(msgid);
	}
}
/* }}} */

/* {{{ Return the translation of msgid for domain_name and category, or msgid unaltered if a translation does not exist */
PHP_FUNCTION(dcgettext)
{
	char *msgstr = NULL;
	zend_string *domain, *msgid;
	zend_long category;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STR(domain)
		Z_PARAM_STR(msgid)
		Z_PARAM_LONG(category)
	ZEND_PARSE_PARAMETERS_END();

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK(1, ZSTR_LEN(domain))
	PHP_GETTEXT_LENGTH_CHECK(2, ZSTR_LEN(msgid))
	PHP_DCGETTEXT_CATEGORY_CHECK(3, category)

	msgstr = dcgettext(ZSTR_VAL(domain), ZSTR_VAL(msgid), category);

	if (msgstr != ZSTR_VAL(msgid)) {
		RETURN_STRING(msgstr);
	} else {
		RETURN_STR_COPY(msgid);
	}
}
/* }}} */

/* {{{ Bind to the text domain domain_name, looking for translations in dir. Returns the current domain */
PHP_FUNCTION(bindtextdomain)
{
	zend_string *domain, *dir = NULL;
	char *retval, dir_name[MAXPATHLEN];

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(domain)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(dir)
	ZEND_PARSE_PARAMETERS_END();

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK(1, ZSTR_LEN(domain))

	if (dir == NULL) {
		RETURN_STRING(bindtextdomain(ZSTR_VAL(domain), NULL));
	}

	if (ZSTR_LEN(dir) != 0 && !zend_string_equals_literal(dir, "0")) {
		if (!VCWD_REALPATH(ZSTR_VAL(dir), dir_name)) {
			RETURN_FALSE;
		}
	} else if (!VCWD_GETCWD(dir_name, MAXPATHLEN)) {
		RETURN_FALSE;
	}

	retval = bindtextdomain(ZSTR_VAL(domain), dir_name);

	RETURN_STRING(retval);
}
/* }}} */

#ifdef HAVE_NGETTEXT
/* {{{ Plural version of gettext() */
PHP_FUNCTION(ngettext)
{
	char *msgstr = NULL;
	zend_string *msgid1, *msgid2;
	zend_long count;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STR(msgid1)
		Z_PARAM_STR(msgid2)
		Z_PARAM_LONG(count)
	ZEND_PARSE_PARAMETERS_END();

	PHP_GETTEXT_LENGTH_CHECK(1, ZSTR_LEN(msgid1))
	PHP_GETTEXT_LENGTH_CHECK(2, ZSTR_LEN(msgid2))

	msgstr = ngettext(ZSTR_VAL(msgid1), ZSTR_VAL(msgid2), count);

	ZEND_ASSERT(msgstr);
	RETURN_STRING(msgstr);
}
/* }}} */
#endif

#ifdef HAVE_DNGETTEXT
/* {{{ Plural version of dgettext() */
PHP_FUNCTION(dngettext)
{
	char *msgstr = NULL;
	zend_string *domain, *msgid1, *msgid2;
	zend_long count;

	ZEND_PARSE_PARAMETERS_START(4, 4)
		Z_PARAM_STR(domain)
		Z_PARAM_STR(msgid1)
		Z_PARAM_STR(msgid2)
		Z_PARAM_LONG(count)
	ZEND_PARSE_PARAMETERS_END();

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK(1, ZSTR_LEN(domain))
	PHP_GETTEXT_LENGTH_CHECK(2, ZSTR_LEN(msgid1))
	PHP_GETTEXT_LENGTH_CHECK(3, ZSTR_LEN(msgid2))

	msgstr = dngettext(ZSTR_VAL(domain), ZSTR_VAL(msgid1), ZSTR_VAL(msgid2), count);

	ZEND_ASSERT(msgstr);
	RETURN_STRING(msgstr);
}
/* }}} */
#endif

#ifdef HAVE_DCNGETTEXT
/* {{{ Plural version of dcgettext() */
PHP_FUNCTION(dcngettext)
{
	char *msgstr = NULL;
	zend_string *domain, *msgid1, *msgid2;
	zend_long count, category;

	RETVAL_FALSE;

	ZEND_PARSE_PARAMETERS_START(5, 5)
		Z_PARAM_STR(domain)
		Z_PARAM_STR(msgid1)
		Z_PARAM_STR(msgid2)
		Z_PARAM_LONG(count)
		Z_PARAM_LONG(category)
	ZEND_PARSE_PARAMETERS_END();

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK(1, ZSTR_LEN(domain))
	PHP_GETTEXT_LENGTH_CHECK(2, ZSTR_LEN(msgid1))
	PHP_GETTEXT_LENGTH_CHECK(3, ZSTR_LEN(msgid2))
	PHP_DCGETTEXT_CATEGORY_CHECK(5, category)

	msgstr = dcngettext(ZSTR_VAL(domain), ZSTR_VAL(msgid1), ZSTR_VAL(msgid2), count, category);

	ZEND_ASSERT(msgstr);
	RETURN_STRING(msgstr);
}
/* }}} */
#endif

#ifdef HAVE_BIND_TEXTDOMAIN_CODESET

/* {{{ Specify the character encoding in which the messages from the DOMAIN message catalog will be returned. */
PHP_FUNCTION(bind_textdomain_codeset)
{
	char *retval = NULL;
	zend_string *domain, *codeset = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(domain)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(codeset)
	ZEND_PARSE_PARAMETERS_END();

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK(1, ZSTR_LEN(domain))

	retval = bind_textdomain_codeset(ZSTR_VAL(domain), codeset ? ZSTR_VAL(codeset) : NULL);

	if (!retval) {
		RETURN_FALSE;
	}
	RETURN_STRING(retval);
}
/* }}} */
#endif


#endif /* HAVE_LIBINTL */
