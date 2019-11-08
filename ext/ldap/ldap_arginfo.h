/* This is a generated file, edit the .stub.php file instead. */

#if defined(HAVE_ORALDAP)
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_connect, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, wallet, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, wallet_passwd, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, authmode, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

#if !(defined(HAVE_ORALDAP))
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_connect, 0, 0, 0)
	ZEND_ARG_TYPE_INFO(0, hostname, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_unbind, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()

#define arginfo_ldap_close arginfo_ldap_unbind

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_bind, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_TYPE_INFO(0, bind_rdn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, bind_password, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_bind_ext, 0, 0, 1)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_TYPE_INFO(0, bind_rdn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, bind_password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, servercontrols, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#if defined(HAVE_LDAP_SASL)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_sasl_bind, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_TYPE_INFO(0, binddn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, sasl_mech, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, sasl_realm, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, sasl_authc_id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, sasl_authz_id, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, props, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_read, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, base_dn)
	ZEND_ARG_INFO(0, filter)
	ZEND_ARG_TYPE_INFO(0, attributes, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, attrsonly, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, sizelimit, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, timelimit, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, deref, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, servercontrols, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_ldap_list arginfo_ldap_read

#define arginfo_ldap_search arginfo_ldap_read

#define arginfo_ldap_free_result arginfo_ldap_unbind

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_count_entries, 0, 2, IS_LONG, 0)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_first_entry, 0, 0, 2)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_identifier)
ZEND_END_ARG_INFO()

#define arginfo_ldap_next_entry arginfo_ldap_first_entry

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ldap_get_entries, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ldap_first_attribute, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_entry_identifier)
	ZEND_ARG_TYPE_INFO(0, dummy_ber, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_ldap_next_attribute arginfo_ldap_first_attribute

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_get_attributes, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_entry_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ldap_get_values, 0, 3, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_entry_identifier)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_ldap_get_values_len arginfo_ldap_get_values

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ldap_get_dn, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_INFO(0, result_entry_identifier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ldap_explode_dn, 0, 2, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, dn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, with_attrib, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ldap_dn2ufn, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, dn, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_add, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_TYPE_INFO(0, dn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, entry, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, servercontrols, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_add_ext, 0, 0, 3)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_TYPE_INFO(0, dn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, entry, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, servercontrols, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_delete, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_TYPE_INFO(0, dn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, servercontrols, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_delete_ext, 0, 0, 2)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_TYPE_INFO(0, dn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, servercontrols, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_ldap_modify arginfo_ldap_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_modify_batch, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_TYPE_INFO(0, dn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, modifications_info, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, servercontrols, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_ldap_mod_add arginfo_ldap_add

#define arginfo_ldap_mod_add_ext arginfo_ldap_add_ext

#define arginfo_ldap_mod_replace arginfo_ldap_add

#define arginfo_ldap_mod_replace_ext arginfo_ldap_add_ext

#define arginfo_ldap_mod_del arginfo_ldap_add

#define arginfo_ldap_mod_del_ext arginfo_ldap_add_ext

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_errno, 0, 1, IS_LONG, 0)
	ZEND_ARG_INFO(0, link)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_error, 0, 1, IS_STRING, 0)
	ZEND_ARG_INFO(0, link)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_err2str, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, errno, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ldap_compare, 0, 4, MAY_BE_BOOL|MAY_BE_LONG)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_TYPE_INFO(0, dn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, attribute, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, servercontrols, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#if defined(LDAP_CONTROL_PAGEDRESULTS)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_control_paged_result, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_TYPE_INFO(0, pagesize, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, iscritical, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, cookie, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(LDAP_CONTROL_PAGEDRESULTS)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_control_paged_result_response, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(1, cookie)
	ZEND_ARG_INFO(1, estimated)
ZEND_END_ARG_INFO()
#endif

#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_rename, 0, 5, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_TYPE_INFO(0, dn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, newrdn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, newparent, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, deleteoldrdn, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, servercontrols, IS_ARRAY, 0)
ZEND_END_ARG_INFO()
#endif

#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_rename_ext, 0, 0, 5)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_TYPE_INFO(0, dn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, newrdn, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, newparent, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, deleteoldrdn, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, servercontrols, IS_ARRAY, 0)
ZEND_END_ARG_INFO()
#endif

#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_get_option, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_INFO(1, retval)
ZEND_END_ARG_INFO()
#endif

#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_set_option, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link_identifier)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_INFO(0, newval)
ZEND_END_ARG_INFO()
#endif

#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_first_reference, 0, 0, 2)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, result)
ZEND_END_ARG_INFO()
#endif

#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_next_reference, 0, 0, 2)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, entry)
ZEND_END_ARG_INFO()
#endif

#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP && defined(HAVE_LDAP_PARSE_REFERENCE)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_parse_reference, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, entry)
	ZEND_ARG_INFO(1, referrals)
ZEND_END_ARG_INFO()
#endif

#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP && defined(HAVE_LDAP_PARSE_RESULT)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_parse_result, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(1, errcode)
	ZEND_ARG_INFO(1, matcheddn)
	ZEND_ARG_INFO(1, errmsg)
	ZEND_ARG_INFO(1, referrals)
	ZEND_ARG_INFO(1, serverctrls)
ZEND_END_ARG_INFO()
#endif

#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_set_rebind_proc, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_LDAP_START_TLS_S)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_start_tls, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link_identifier)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_escape, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ignore, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 0)
ZEND_END_ARG_INFO()

#if defined(STR_TRANSLATION)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ldap_t61_to_8859, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()
#endif

#if defined(STR_TRANSLATION)
#define arginfo_ldap_8859_to_t61 arginfo_ldap_t61_to_8859
#endif

#if defined(HAVE_LDAP_EXTENDED_OPERATION_S)
ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_exop, 0, 0, 2)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_TYPE_INFO(0, reqoid, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, reqdata, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, servercontrols, IS_ARRAY, 1)
	ZEND_ARG_INFO(1, retdata)
	ZEND_ARG_INFO(1, retoid)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_LDAP_PASSWD)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ldap_exop_passwd, 0, 1, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_TYPE_INFO(0, user, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, oldpw, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, newpw, IS_STRING, 0)
	ZEND_ARG_INFO(1, serverctrls)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_LDAP_WHOAMI_S)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ldap_exop_whoami, 0, 1, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_INFO(0, link)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_LDAP_REFRESH_S)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_ldap_exop_refresh, 0, 3, MAY_BE_LONG|MAY_BE_FALSE)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_TYPE_INFO(0, dn, IS_STRING, 0)
	ZEND_ARG_INFO(0, ttl)
ZEND_END_ARG_INFO()
#endif

#if defined(HAVE_LDAP_PARSE_EXTENDED_RESULT)
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ldap_parse_exop, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_INFO(0, link)
	ZEND_ARG_INFO(0, result)
	ZEND_ARG_INFO(1, retdata)
	ZEND_ARG_INFO(1, retoid)
ZEND_END_ARG_INFO()
#endif
