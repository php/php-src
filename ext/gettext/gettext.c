/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2009 The PHP Group                                |
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
    {NULL, NULL, NULL}
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

#define RETVAL_FS_STRING(s, f) \
	RETVAL_STRING((s), (f)); \
	if (UG(unicode)) { \
		zval_string_to_unicode_ex(return_value, ZEND_U_CONVERTER(UG(filesystem_encoding_conv)) TSRMLS_CC); \
	}
#define RETURN_FS_STRING(s, f) \
	RETVAL_FS_STRING((s), (f)); \
	return;

/* {{{ proto string textdomain(string domain) U
   Set the textdomain to "domain". Returns the current domain */
PHP_NAMED_FUNCTION(zif_textdomain)
{
	char *domain_str;
	int domain_len;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&", &domain_str, &domain_len, ZEND_U_CONVERTER(UG(filesystem_encoding_conv)))) {
		return;
	}
	
	PHP_GETTEXT_DOMAIN_LENGTH_CHECK

	if (!domain_len || (domain_len == 1 && *domain_str == '0')) {
		domain_str = NULL;
	}
	RETURN_FS_STRING(textdomain(domain_str), ZSTR_DUPLICATE);
}
/* }}} */

/* {{{ proto binary gettext(string msgid) U
   Return the translation of msgid for the current domain, or msgid unaltered if a translation does not exist */
PHP_NAMED_FUNCTION(zif_gettext)
{
	char *msgid_str;
	int msgid_len;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&", &msgid_str, &msgid_len, UG(ascii_conv))) {
		return;
	}

	PHP_GETTEXT_LENGTH_CHECK("msgid", msgid_len)

	RETURN_STRING(gettext(msgid_str), ZSTR_DUPLICATE);
}
/* }}} */

/* {{{ proto binary dgettext(string domain_name, string msgid) U
   Return the translation of msgid for domain_name, or msgid unaltered if a translation does not exist */
PHP_NAMED_FUNCTION(zif_dgettext)
{
	char *domain_str, *msgid_str;
	int domain_len, msgid_len;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&s&", &domain_str, &domain_len, ZEND_U_CONVERTER(UG(filesystem_encoding_conv)), &msgid_str, &msgid_len, UG(ascii_conv))) {
		return;
	}

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK
	PHP_GETTEXT_LENGTH_CHECK("msgid", msgid_len)

	RETURN_STRING(dgettext(domain_str, msgid_str), ZSTR_DUPLICATE);
}
/* }}} */

/* {{{ proto binary dcgettext(string domain_name, string msgid, int category) U
   Return the translation of msgid for domain_name and category, or msgid unaltered if a translation does not exist */
PHP_NAMED_FUNCTION(zif_dcgettext)
{
	char *domain_str, *msgid_str;
	int domain_len, msgid_len;
	long category;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&s&l", &domain_str, &domain_len, ZEND_U_CONVERTER(UG(filesystem_encoding_conv)), &msgid_str, &msgid_len, UG(ascii_conv), &category)) {
		return;
	}

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK
	PHP_GETTEXT_LENGTH_CHECK("msgid", msgid_len)

	RETURN_STRING(dcgettext(domain_str, msgid_str, category), ZSTR_DUPLICATE);
}
/* }}} */

/* {{{ proto string bindtextdomain(string domain_name, string dir) U
   Bind to the text domain domain_name, looking for translations in dir. Returns the current domain */
PHP_NAMED_FUNCTION(zif_bindtextdomain)
{
	char *domain_str, *dir_str, dir_tmp[MAXPATHLEN] = {0};
	int domain_len, dir_len;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&s&", &domain_str, &domain_len, ZEND_U_CONVERTER(UG(filesystem_encoding_conv)), &dir_str, &dir_len, ZEND_U_CONVERTER(UG(filesystem_encoding_conv)))) {
		return;
	}

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK

	if (!domain_len || !*domain_str) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "the first parameter must not be empty");
		RETURN_FALSE;
	}
	if (!dir_len || (dir_len == 1 && *dir_str == '0')) {
		if (!VCWD_GETCWD(dir_tmp, sizeof(dir_tmp))) {
			RETURN_FALSE;
		}
	} else if (!VCWD_REALPATH(dir_str, dir_tmp)) {
		RETURN_FALSE;
	}
	RETURN_FS_STRING(bindtextdomain(domain_str, dir_tmp), ZSTR_DUPLICATE);
}
/* }}} */

#if HAVE_NGETTEXT
/* {{{ proto binary ngettext(string msgid1, string msgid2, int count) U
   Plural version of gettext() */
PHP_NAMED_FUNCTION(zif_ngettext)
{
	char *msgid_str1, *msgid_str2, *msgstr;
	int msgid1_len, msgid2_len;
	long count;

	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&s&l", &msgid_str1, &msgid1_len, UG(ascii_conv), &msgid_str2, &msgid2_len, UG(ascii_conv), &count)) {
		RETURN_FALSE;
	}

	PHP_GETTEXT_LENGTH_CHECK("msgid1", msgid1_len)
	PHP_GETTEXT_LENGTH_CHECK("msgid2", msgid2_len)

	if ((msgstr = ngettext(msgid_str1, msgid_str2, count))) {
		RETURN_STRING(msgstr, ZSTR_DUPLICATE);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */
#endif

#if HAVE_DNGETTEXT
/* {{{ proto binary dngettext (string domain, string msgid1, string msgid2, int count) U
   Plural version of dgettext() */
PHP_NAMED_FUNCTION(zif_dngettext)
{
	char *domain_str, *msgid_str1, *msgid_str2, *msgstr;
	int domain_len, msgid1_len, msgid2_len;
	long count;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&s&s&l", &domain_str, &domain_len, ZEND_U_CONVERTER(UG(filesystem_encoding_conv)), &msgid_str1, &msgid1_len, UG(ascii_conv), &msgid_str2, &msgid2_len, UG(ascii_conv), &count)) {
		RETURN_FALSE;
	}

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK
	PHP_GETTEXT_LENGTH_CHECK("msgid1", msgid1_len)
	PHP_GETTEXT_LENGTH_CHECK("msgid2", msgid2_len)

	if ((msgstr = dngettext(domain_str, msgid_str1, msgid_str2, count))) {
		RETURN_STRING(msgstr, ZSTR_DUPLICATE);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */
#endif

#if HAVE_DCNGETTEXT
/* {{{ proto binary dcngettext (string domain, string msgid1, string msgid2, int count, int category) U
   Plural version of dcgettext() */
PHP_NAMED_FUNCTION(zif_dcngettext)
{
	char *domain_str, *msgid_str1, *msgid_str2, *msgstr;
	int domain_len, msgid1_len, msgid2_len;
	long count, category;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&s&s&ll", &domain_str, &domain_len, ZEND_U_CONVERTER(UG(filesystem_encoding_conv)), &msgid_str1, &msgid1_len, UG(ascii_conv), &msgid_str2, &msgid2_len, UG(ascii_conv), &count, &category)) {
		RETURN_FALSE;
	}

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK
	PHP_GETTEXT_LENGTH_CHECK("msgid1", msgid1_len)
	PHP_GETTEXT_LENGTH_CHECK("msgid2", msgid2_len)

	if ((msgstr = dcngettext(domain_str, msgid_str1, msgid_str2, count, category))) {
		RETURN_STRING(msgstr, ZSTR_DUPLICATE);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */
#endif

#if HAVE_BIND_TEXTDOMAIN_CODESET
/* {{{ proto string bind_textdomain_codeset (string domain, string codeset) U
   Specify the character encoding in which the messages from the DOMAIN message catalog will be returned. */
PHP_NAMED_FUNCTION(zif_bind_textdomain_codeset)
{
	char *domain_str, *codeset_str, *codeset_ret;
	int domain_len, codeset_len;
	
	if (SUCCESS != zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s&s&", &domain_str, &domain_len, ZEND_U_CONVERTER(UG(filesystem_encoding_conv)), &codeset_str, &codeset_len, UG(ascii_conv))) {
		return;
	}

	PHP_GETTEXT_DOMAIN_LENGTH_CHECK

	if (!codeset_len) {
		codeset_str = NULL;
	}
	if ((codeset_ret = bind_textdomain_codeset(domain_str, codeset_str))) {
		RETURN_ASCII_STRING(codeset_ret, ZSTR_DUPLICATE);
	} else {
		RETURN_FALSE;
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

