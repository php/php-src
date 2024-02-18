/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 864b3389d4f99b0d7302ae399544e6fb9fb80b7e */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_textdomain, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, domain, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gettext, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo__ arginfo_gettext

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dgettext, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, domain, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dcgettext, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, domain, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, category, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_bindtextdomain, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, domain, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, directory, IS_STRING, 1)
ZEND_END_ARG_INFO()

#if defined(HAVE_NGETTEXT)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ngettext, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, singular, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, plural, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_DNGETTEXT)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dngettext, 0, 4, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, domain, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, singular, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, plural, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_DCNGETTEXT)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_dcngettext, 0, 5, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, domain, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, singular, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, plural, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, count, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, category, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_BIND_TEXTDOMAIN_CODESET)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_bind_textdomain_codeset, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, domain, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, codeset, IS_STRING, 1)
ZEND_END_ARG_INFO()
#endif

ZEND_FUNCTION(textdomain);
ZEND_FUNCTION(gettext);
ZEND_FUNCTION(dgettext);
ZEND_FUNCTION(dcgettext);
ZEND_FUNCTION(bindtextdomain);
#if defined(HAVE_NGETTEXT)
ZEND_FUNCTION(ngettext);
#endif
#if defined(HAVE_DNGETTEXT)
ZEND_FUNCTION(dngettext);
#endif
#if defined(HAVE_DCNGETTEXT)
ZEND_FUNCTION(dcngettext);
#endif
#if defined(HAVE_BIND_TEXTDOMAIN_CODESET)
ZEND_FUNCTION(bind_textdomain_codeset);
#endif

static const zend_function_entry ext_functions[] = {
	ZEND_FE(textdomain, arginfo_textdomain)
	ZEND_FE(gettext, arginfo_gettext)
	ZEND_FALIAS(_, gettext, arginfo__)
	ZEND_FE(dgettext, arginfo_dgettext)
	ZEND_FE(dcgettext, arginfo_dcgettext)
	ZEND_FE(bindtextdomain, arginfo_bindtextdomain)
#if defined(HAVE_NGETTEXT)
	ZEND_FE(ngettext, arginfo_ngettext)
#endif
#if defined(HAVE_DNGETTEXT)
	ZEND_FE(dngettext, arginfo_dngettext)
#endif
#if defined(HAVE_DCNGETTEXT)
	ZEND_FE(dcngettext, arginfo_dcngettext)
#endif
#if defined(HAVE_BIND_TEXTDOMAIN_CODESET)
	ZEND_FE(bind_textdomain_codeset, arginfo_bind_textdomain_codeset)
#endif
	ZEND_FE_END
};
