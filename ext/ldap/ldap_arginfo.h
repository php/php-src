/* This is a generated file, edit the .stub.php file instead. */

#if defined(HAVE_ORALDAP)
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_connect, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, wallet, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, wallet_passwd, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, authmode, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if !(defined(HAVE_ORALDAP))
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_connect, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_close, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link)
ZEND_END_ARG_INFO()

#define arginfo_ldap_unbind arginfo_ldap_close

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_bind, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_TYPE_INFO(0, dn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_bind_ext, 0, 0, 2)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_TYPE_INFO(0, dn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, serverctrls, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_LDAP_SASL)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_sasl_bind, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_TYPE_INFO(0, bind_rdn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, sasl_mech, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, sasl_realm, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, sasl_authc_id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, sasl_authz_id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, props, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_read, 0, 0, 3)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_TYPE_INFO(0, base_dn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, filter, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, attrs, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, attrsonly, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, sizelimit, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, timelimit, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, deref, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, servercontrols, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_ldap_list arginfo_ldap_read

#define arginfo_ldap_search arginfo_ldap_read
