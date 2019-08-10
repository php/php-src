<?php

#ifdef HAVE_ORALDAP
/** @return resource|false */
function ldap_connect(string $host = null, int $port = 389, string $wallet = null, string $wallet_passwd = null, int $authmode = GSLC_SSL_NO_AUTH) {}
#else
/** @return resource|false */
function ldap_connect(string $host = null, int $port = 389) {}
#endif

/**
 * @param resource $link
 * TODO: this is an alias of ldap_unbind()
 */
function ldap_close($link): bool {}

/**
 * @param resource $link
 */
function ldap_unbind($link): bool {}

/**
 * @param resource $link
 */
function ldap_bind($link, string $dn, string $password = null): bool {}

/** @return resource|false */
function ldap_bind_ext($link, string $dn, string $password = null, array $serverctrls = []) {}

#ifdef HAVE_LDAP_SASL
function ldap_sasl_bind($link, string $bind_rdn, string $password = null, string $sasl_mech = null, string $sasl_realm = null, string $sasl_authc_id = null, string $sasl_authz_id = null, string $props = null): bool {}
#endif

/**
 * @param resource|array $link
 * @return resource
 */
function ldap_read($link, string $base_dn, string $filter, array $attrs = [], int $attrsonly = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, array $servercontrols = []) {}

/**
 * @param resource|array $link
 * @return resource
 */
function ldap_list($link, string $base_dn, string $filter, array $attrs = [], int $attrsonly = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, array $servercontrols = []) {}

/**
 * @param resource|array $link
 * @return resource
 */
function ldap_search($link, string $base_dn, string $filter, array $attrs = [], int $attrsonly = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, array $servercontrols = []) {}

/**
 * @param resource $link
 * @param resource $result
 * @return int|false
 */
function ldap_count_entries($link, $result) {}

/**
 * @param resource $link
 * @param resource $result
 * @return resource
 */
function ldap_first_entry($link, $result) {}

/**
 * @param resource $link
 * @param resource $result_entry
 * @return resource
 */
function ldap_next_entry($link, $result_entry) {}



//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_get_entries, 0, 0, 2)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, result_identifier)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_first_attribute, 0, 0, 2)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, result_entry_identifier)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_next_attribute, 0, 0, 2)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, result_entry_identifier)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_get_attributes, 0, 0, 2)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, result_entry_identifier)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_get_values, 0, 0, 3)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, result_entry_identifier)
//	ZEND_ARG_INFO(0, attribute)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_get_values_len, 0, 0, 3)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, result_entry_identifier)
//	ZEND_ARG_INFO(0, attribute)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_get_dn, 0, 0, 2)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, result_entry_identifier)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_explode_dn, 0, 0, 2)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, with_attrib)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_dn2ufn, 0, 0, 1)
//	ZEND_ARG_INFO(0, dn)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_add, 0, 0, 3)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, entry)
//	ZEND_ARG_INFO(0, servercontrols)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_add_ext, 0, 0, 3)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, entry)
//	ZEND_ARG_INFO(0, servercontrols)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_delete, 0, 0, 2)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, servercontrols)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_delete_ext, 0, 0, 2)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, servercontrols)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_modify, 0, 0, 3)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, entry)
//	ZEND_ARG_INFO(0, servercontrols)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_modify_batch, 0, 0, 3)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_ARRAY_INFO(0, modifications_info, 0)
//	ZEND_ARG_INFO(0, servercontrols)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_mod_add, 0, 0, 3)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, entry)
//	ZEND_ARG_INFO(0, servercontrols)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_mod_add_ext, 0, 0, 3)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, entry)
//	ZEND_ARG_INFO(0, servercontrols)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_mod_replace, 0, 0, 3)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, entry)
//	ZEND_ARG_INFO(0, servercontrols)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_mod_replace_ext, 0, 0, 3)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, entry)
//	ZEND_ARG_INFO(0, servercontrols)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_mod_del, 0, 0, 3)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, entry)
//	ZEND_ARG_INFO(0, servercontrols)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_mod_del_ext, 0, 0, 3)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, entry)
//	ZEND_ARG_INFO(0, servercontrols)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_err2str, 0, 0, 1)
//	ZEND_ARG_INFO(0, errno)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_compare, 0, 0, 4)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, attribute)
//	ZEND_ARG_INFO(0, value)
//	ZEND_ARG_INFO(0, servercontrols)
//ZEND_END_ARG_INFO()
//
//#ifdef LDAP_CONTROL_PAGEDRESULTS
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_control_paged_result, 0, 0, 2)
//	ZEND_ARG_INFO(0, link)
//	ZEND_ARG_INFO(0, pagesize)
//	ZEND_ARG_INFO(0, iscritical)
//	ZEND_ARG_INFO(0, cookie)
//ZEND_END_ARG_INFO();
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_control_paged_result_response, 0, 0, 2)
//	ZEND_ARG_INFO(0, link)
//	ZEND_ARG_INFO(0, result)
//	ZEND_ARG_INFO(1, cookie)
//	ZEND_ARG_INFO(1, estimated)
//ZEND_END_ARG_INFO();
//#endif
//
//#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_rename, 0, 0, 5)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, newrdn)
//	ZEND_ARG_INFO(0, newparent)
//	ZEND_ARG_INFO(0, deleteoldrdn)
//	ZEND_ARG_INFO(0, servercontrols)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_rename_ext, 0, 0, 5)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, newrdn)
//	ZEND_ARG_INFO(0, newparent)
//	ZEND_ARG_INFO(0, deleteoldrdn)
//	ZEND_ARG_INFO(0, servercontrols)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_get_option, 0, 0, 3)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, option)
//	ZEND_ARG_INFO(1, retval)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_set_option, 0, 0, 3)
//	ZEND_ARG_INFO(0, link_identifier)
//	ZEND_ARG_INFO(0, option)
//	ZEND_ARG_INFO(0, newval)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_first_reference, 0, 0, 2)
//	ZEND_ARG_INFO(0, link)
//	ZEND_ARG_INFO(0, result)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_next_reference, 0, 0, 2)
//	ZEND_ARG_INFO(0, link)
//	ZEND_ARG_INFO(0, entry)
//ZEND_END_ARG_INFO()
//
//#ifdef HAVE_LDAP_PARSE_REFERENCE
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_parse_reference, 0, 0, 3)
//	ZEND_ARG_INFO(0, link)
//	ZEND_ARG_INFO(0, entry)
//	ZEND_ARG_INFO(1, referrals)
//ZEND_END_ARG_INFO()
//#endif
//
//
//#ifdef HAVE_LDAP_PARSE_RESULT
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_parse_result, 0, 0, 3)
//	ZEND_ARG_INFO(0, link)
//	ZEND_ARG_INFO(0, result)
//	ZEND_ARG_INFO(1, errcode)
//	ZEND_ARG_INFO(1, matcheddn)
//	ZEND_ARG_INFO(1, errmsg)
//	ZEND_ARG_INFO(1, referrals)
//	ZEND_ARG_INFO(1, serverctrls)
//ZEND_END_ARG_INFO()
//#endif
//#endif
//
//#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_set_rebind_proc, 0, 0, 2)
//	ZEND_ARG_INFO(0, link)
//	ZEND_ARG_INFO(0, callback)
//ZEND_END_ARG_INFO()
//#endif
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_escape, 0, 0, 1)
//	ZEND_ARG_INFO(0, value)
//	ZEND_ARG_INFO(0, ignore)
//	ZEND_ARG_INFO(0, flags)
//ZEND_END_ARG_INFO()
//
//#ifdef STR_TRANSLATION
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_t61_to_8859, 0, 0, 1)
//	ZEND_ARG_INFO(0, value)
//ZEND_END_ARG_INFO()
//
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_8859_to_t61, 0, 0, 1)
//	ZEND_ARG_INFO(0, value)
//ZEND_END_ARG_INFO()
//#endif
//
//#ifdef HAVE_LDAP_EXTENDED_OPERATION_S
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_exop, 0, 0, 2)
//	ZEND_ARG_INFO(0, link)
//	ZEND_ARG_INFO(0, reqoid)
//	ZEND_ARG_INFO(0, reqdata)
//	ZEND_ARG_INFO(0, servercontrols)
//	ZEND_ARG_INFO(1, retdata)
//	ZEND_ARG_INFO(1, retoid)
//ZEND_END_ARG_INFO()
//#endif
//
//#ifdef HAVE_LDAP_PASSWD
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_exop_passwd, 0, 0, 1)
//	ZEND_ARG_INFO(0, link)
//	ZEND_ARG_INFO(0, user)
//	ZEND_ARG_INFO(0, oldpw)
//	ZEND_ARG_INFO(0, newpw)
//	ZEND_ARG_INFO(1, serverctrls)
//ZEND_END_ARG_INFO()
//#endif
//
//#ifdef HAVE_LDAP_WHOAMI_S
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_exop_whoami, 0, 0, 1)
//	ZEND_ARG_INFO(0, link)
//ZEND_END_ARG_INFO()
//#endif
//
//#ifdef HAVE_LDAP_REFRESH_S
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_exop_refresh, 0, 0, 3)
//	ZEND_ARG_INFO(0, link)
//	ZEND_ARG_INFO(0, dn)
//	ZEND_ARG_INFO(0, ttl)
//ZEND_END_ARG_INFO()
//#endif
//
//#ifdef HAVE_LDAP_PARSE_EXTENDED_RESULT
//ZEND_BEGIN_ARG_INFO_EX(arginfo_ldap_parse_exop, 0, 0, 4)
//	ZEND_ARG_INFO(0, link)
//	ZEND_ARG_INFO(0, result)
//	ZEND_ARG_INFO(1, retdata)
//	ZEND_ARG_INFO(1, retoid)
//ZEND_END_ARG_INFO()
//#endif
///* }}} */
///

