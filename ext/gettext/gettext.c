/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2014 The PHP Group                                |
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
#include "ext/standard/info.h"
#include "php_gettext.h"

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_textdomain, 0)
	ZEND_ARG_INFO(0, domain)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_gettext, 0)
	ZEND_ARG_INFO(0, msgid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dgettext, 0)
	ZEND_ARG_INFO(0, domain_name)
	ZEND_ARG_INFO(0, msgid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_dcgettext, 0)
	ZEND_ARG_INFO(0, domain_name)
	ZEND_ARG_INFO(0, msgid)
	ZEND_ARG_INFO(0, category)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_bindtextdomain, 0)
	ZEND_ARG_INFO(0, domain_name)
	ZEND_ARG_INFO(0, dir)
ZEND_END_ARG_INFO()

#if HAVE_NGETTEXT
ZEND_BEGIN_ARG_INFO(arginfo_ngettext, 0)
	ZEND_ARG_INFO(0, msgid1)
	ZEND_ARG_INFO(0, msgid2)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()
#endif

#if HAVE_DNGETTEXT
ZEND_BEGIN_ARG_INFO(arginfo_dngettext, 0)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, msgid1)
	ZEND_ARG_INFO(0, msgid2)
	ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()
#endif

#if HAVE_DCNGETTEXT
ZEND_BEGIN_ARG_INFO(arginfo_dcngettext, 0)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, msgid1)
	ZEND_ARG_INFO(0, msgid2)
	ZEND_ARG_INFO(0, count)
	ZEND_ARG_INFO(0, category)
ZEND_END_ARG_INFO()
#endif

#if HAVE_BIND_TEXTDOMAIN_CODESET
ZEND_BEGIN_ARG_INFO(arginfo_bind_textdomain_codeset, 0)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, codeset)
ZEND_END_ARG_INFO()
#endif
/* }}} */

/* {{{ php_gettext_functions[]
 */
const zend_function_entry php_gettext_functions[] = {
	PHP_NAMED_FE(textdomain,		zif_textdomain,		arginfo_textdomain)
	PHP_NAMED_FE(gettext,			zif_gettext,		arginfo_gettext)
	/* Alias for gettext() */
	PHP_NAMED_FE(_,					zif_gettext,		arginfo_gettext)
	PHP_NAMED_FE(dgettext,			zif_dgettext,		arginfo_dgettext)
	PHP_NAMED_FE(dcgettext,			zif_dcgettext,		arginfo_dcgettext)
	PHP_NAMED_FE(bindtextdomain,	zif_bindtextdomain,	arginfo_bindtextdomain)
#if HAVE_NGETTEXT
	PHP_NAMED_FE(ngettext,			zif_ngettext,		arginfo_ngettext)
#endif
#if HAVE_DNGETTEXT
	PHP_NAMED_FE(dngettext,			zif_dngettext,		arginfo_dngettext)
#endif
#if HAVE_DCNGETTEXT
	PHP_NAMED_FE(dcngettext,		zif_dcngettext,		arginfo_dcngettext)
#endif
#if HAVE_BIND_TEXTDOMAIN_CODESET
	PHP_NAMED_FE(bind_textdomain_codeset,	zif_bind_textdomain_codeset,	arginfo_bind_textdomain_codeset)
#endif
    PHP_FE_END
};
/* }}} */

#include <libintl.h>

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

#define PHP_GETTEXT_MAX_DOMAIN_LENGTH 1024
#define PHP_GETTEXT_MAX_MSGID_LENGTH 4096

#define PHP_GETTEXT_DOMAIN_LENGTH_CHECK \
	if (domain_len > PHP_GETTEXT_MAX_DOMAIN_LENGTH) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "domain passed too long"); \
		RETURN_FALSE; \
	}

#define PHP_GETTEXT_LENGTH_CHECK(check_name, check_len) \
	if (check_len > PHP_GETTEXT_MAX_MSGID_LENGTH) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s passed too long", check_name); \
		RETURN_FALSE; \
	}

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
	char *domain, *domain_name, *retval;
	int domain_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &domain, &domain_len) == FAILURE) {
		return;
	}

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK

	if (strcmp(domain, "") && strcmp(domain, "0")) {
		domain_name = domain;
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
	char *msgid, *msgstr;
	int msgid_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &msgid, &msgid_len) == FAILURE) {
		return;
	}

	PHP_GETTEXT_LENGTH_CHECK("msgid", msgid_len)
	msgstr = gettext(msgid);

	RETURN_STRING(msgstr, 1);
}
/* }}} */

/* {{{ proto string dgettext(string domain_name, string msgid)
   Return the translation of msgid for domain_name, or msgid unaltered if a translation does not exist */
PHP_NAMED_FUNCTION(zif_dgettext)
{
	char *domain, *msgid, *msgstr;
	int domain_len, msgid_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &domain, &domain_len, &msgid, &msgid_len) == FAILURE)	{
		return;
	}

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK
	PHP_GETTEXT_LENGTH_CHECK("msgid", msgid_len)

	msgstr = dgettext(domain, msgid);

	RETURN_STRING(msgstr, 1);
}
/* }}} */

/* {{{ proto string dcgettext(string domain_name, string msgid, long category)
   Return the translation of msgid for domain_name and category, or msgid unaltered if a translation does not exist */
PHP_NAMED_FUNCTION(zif_dcgettext)
{
	char *domain, *msgid, *msgstr;
	int domain_len, msgid_len;
	long category;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssl", &domain, &domain_len, &msgid, &msgid_len, &category) == FAILURE) {
		return;
	}

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK
	PHP_GETTEXT_LENGTH_CHECK("msgid", msgid_len)

	msgstr = dcgettext(domain, msgid, category);

	RETURN_STRING(msgstr, 1);
}
/* }}} */

/* {{{ proto string bindtextdomain(string domain_name, string dir)
   Bind to the text domain domain_name, looking for translations in dir. Returns the current domain */
PHP_NAMED_FUNCTION(zif_bindtextdomain)
{
	char *domain, *dir;
	int domain_len, dir_len;
	char *retval, dir_name[MAXPATHLEN];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &domain, &domain_len, &dir, &dir_len) == FAILURE) {
		return;
	}

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK

	if (domain[0] == '\0') {
		php_error(E_WARNING, "The first parameter of bindtextdomain must not be empty");
		RETURN_FALSE;
	}

	if (dir[0] != '\0' && strcmp(dir, "0")) {
		if (!VCWD_REALPATH(dir, dir_name)) {
			RETURN_FALSE;
		}
	} else if (!VCWD_GETCWD(dir_name, MAXPATHLEN)) {
		RETURN_FALSE;
	}

	retval = bindtextdomain(domain, dir_name);

	RETURN_STRING(retval, 1);
}
/* }}} */

#if HAVE_NGETTEXT
/* {{{ proto string ngettext(string MSGID1, string MSGID2, int N)
   Plural version of gettext() */
PHP_NAMED_FUNCTION(zif_ngettext)
{
	char *msgid1, *msgid2, *msgstr;
	int msgid1_len, msgid2_len;
	long count;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssl", &msgid1, &msgid1_len, &msgid2, &msgid2_len, &count) == FAILURE) {
		return;
	}

	PHP_GETTEXT_LENGTH_CHECK("msgid1", msgid1_len)
	PHP_GETTEXT_LENGTH_CHECK("msgid2", msgid2_len)

	msgstr = ngettext(msgid1, msgid2, count);
	if (msgstr) {
		RETVAL_STRING(msgstr, 1);
	}
}
/* }}} */
#endif

#if HAVE_DNGETTEXT
/* {{{ proto string dngettext (string domain, string msgid1, string msgid2, int count)
   Plural version of dgettext() */
PHP_NAMED_FUNCTION(zif_dngettext)
{
	char *domain, *msgid1, *msgid2, *msgstr = NULL;
	int domain_len, msgid1_len, msgid2_len;
	long count;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssl", &domain, &domain_len,
		&msgid1, &msgid1_len, &msgid2, &msgid2_len, &count) == FAILURE) {
		return;
	}

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK
	PHP_GETTEXT_LENGTH_CHECK("msgid1", msgid1_len)
	PHP_GETTEXT_LENGTH_CHECK("msgid2", msgid2_len)

	msgstr = dngettext(domain, msgid1, msgid2, count);
	if (msgstr) {
		RETVAL_STRING(msgstr, 1);
	}
}
/* }}} */
#endif

#if HAVE_DCNGETTEXT
/* {{{ proto string dcngettext (string domain, string msgid1, string msgid2, int n, int category)
   Plural version of dcgettext() */
PHP_NAMED_FUNCTION(zif_dcngettext)
{
	char *domain, *msgid1, *msgid2, *msgstr = NULL;
	int domain_len, msgid1_len, msgid2_len;
	long count, category;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sssll", &domain, &domain_len,
		&msgid1, &msgid1_len, &msgid2, &msgid2_len, &count, &category) == FAILURE) {
		return;
	}

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK
	PHP_GETTEXT_LENGTH_CHECK("msgid1", msgid1_len)
	PHP_GETTEXT_LENGTH_CHECK("msgid2", msgid2_len)

	msgstr = dcngettext(domain, msgid1, msgid2, count, category);

	if (msgstr) {
		RETVAL_STRING(msgstr, 1);
	}
}
/* }}} */
#endif

#if HAVE_BIND_TEXTDOMAIN_CODESET

/* {{{ proto string bind_textdomain_codeset (string domain, string codeset)
   Specify the character encoding in which the messages from the DOMAIN message catalog will be returned. */
PHP_NAMED_FUNCTION(zif_bind_textdomain_codeset)
{
	char *domain, *codeset, *retval = NULL;
	int domain_len, codeset_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &domain, &domain_len, &codeset, &codeset_len) == FAILURE) {
		return;
	}

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK

	retval = bind_textdomain_codeset(domain, codeset);

	if (!retval) {
		RETURN_FALSE;
	}
	RETURN_STRING(retval, 1);
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

