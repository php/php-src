<?php

//TODO: missing arginfo functions defined in C using

#ifdef HAVE_ORALDAP
/** @return resource|false */
function ldap_connect(string $hostname = UNKNOWN, int $port = 389, string $wallet = UNKNOWN, string $wallet_passwd = UNKNOWN, int $authmode = GSLC_SSL_NO_AUTH) {}
#else
/** @return resource|false */
function ldap_connect(string $hostname = UNKNOWN, int $port = 389) {}
#endif

/**
 * @param resource $link_identifier
 */
function ldap_unbind($link_identifier): bool {}

/**
 * @param resource $link_identifier
 */
function ldap_close($link_identifier): bool {}

/**
 * @param resource $link_identifier
 */
function ldap_bind($link_identifier, string $bind_rdn = UNKNOWN, string $bind_password = UNKNOWN): bool {}

/**
 * @param resource $link_identifier
 * @return resource|false
 */
function ldap_bind_ext($link_identifier, string $bind_rdn = UNKNOWN, string $bind_password = UNKNOWN, array $servercontrols = []) {}

#ifdef HAVE_LDAP_SASL
/**
 * @param resource $link
 */
function ldap_sasl_bind($link, string $binddn = UNKNOWN, string $password = UNKNOWN, string $sasl_mech = UNKNOWN, string $sasl_realm = UNKNOWN, string $sasl_authc_id = UNKNOWN, string $sasl_authz_id = UNKNOWN, string $props = UNKNOWN): bool {}
#endif

/**
 * @param resource|array $link_identifier
 * @param string|array $base_dn
 * @param string|array $filter
 * @return resource|false
 */
function ldap_read($link_identifier, $base_dn, $filter, array $attributes = [], int $attrsonly = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, array $servercontrols = []) {}

/**
 * @param resource|array $link_identifier
 * @param string|array $base_dn
 * @param string|array $filter
 * @return resource|false
 */
function ldap_list($link_identifier, $base_dn, $filter, array $attributes = [], int $attrsonly = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, array $servercontrols = []) {}

/**
 * @param resource|array $link_identifier
 * @param string|array $base_dn
 * @param string|array $filter
 * @return resource|false
 */
function ldap_search($link_identifier, $base_dn, $filter, array $attributes = [], int $attrsonly = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, array $servercontrols = []) {}

/**
 * @param resource $link_identifier
 */
function ldap_free_result($link_identifier): bool {}


/**
 * @param resource $link_identifier
 * @param resource $result_identifier
 */
function ldap_count_entries($link_identifier, $result_identifier): int {}

/**
 * @param resource $link_identifier
 * @param resource $result_identifier
 * @return resource|false
 */
function ldap_first_entry($link_identifier, $result_identifier) {}

/**
 * @param resource $link_identifier
 * @param resource $result_identifier
 * @return resource|false
 */
function ldap_next_entry($link_identifier, $result_identifier) {}

/**
 * @param resource $link_identifier
 * @param resource $result_identifier
 */
function ldap_get_entries($link_identifier, $result_identifier): array|false {}

/**
 * @param resource $link_identifier
 * @param resource $result_entry_identifier
 */
function ldap_first_attribute($link_identifier, $result_entry_identifier, int $dummy_ber = UNKNOWN): string|false {}

/**
 * @param resource $link_identifier
 * @param resource $result_entry_identifier
 */
function ldap_next_attribute($link_identifier, $result_entry_identifier, int $dummy_ber = UNKNOWN): string|false {}

/**
 * @param resource $link_identifier
 * @param resource $result_entry_identifier
 */
function ldap_get_attributes($link_identifier, $result_entry_identifier): array {}

/**
 * @param resource $link_identifier
 * @param resource $result_entry_identifier
 */
function ldap_get_values($link_identifier, $result_entry_identifier, string $attribute): array|false {}

/**
 * @param resource $link_identifier
 * @param resource $result_entry_identifier
 */
function ldap_get_values_len($link_identifier, $result_entry_identifier, string $attribute): array|false {}

/**
 * @param resource $link_identifier
 * @param resource $result_entry_identifier
 */
function ldap_get_dn($link_identifier, $result_entry_identifier): string|false {}

function ldap_explode_dn(string $dn, int $with_attrib): array|false {}

function ldap_dn2ufn(string $dn): string|false {}

/** @param resource $link_identifier */
function ldap_add($link_identifier, string $dn, array $entry, array $servercontrols = []): bool {}

/**
 * @param resource $link_identifier
 * @return resource|false
 */
function ldap_add_ext($link_identifier, string $dn, array $entry, array $servercontrols = []) {}

/** @param resource $link_identifier */
function ldap_delete($link_identifier, string $dn, array $servercontrols = []): bool {}

/**
 * @param resource $link_identifier
 * @return resource|false
 */
function ldap_delete_ext($link_identifier, string $dn, array $servercontrols = []) {}

/**
 * @param resource $link_identifier
 */
function ldap_modify($link_identifier, string $dn, array $entry, array $servercontrols = []): bool {}

/**
 * @param resource $link_identifier
 */
function ldap_modify_batch($link_identifier, string $dn, array $modifications_info, array $servercontrols = []): bool {}

/**
 * @param resource $link_identifier
 */
function ldap_mod_add($link_identifier, string $dn, array $entry, array $servercontrols = []): bool {}

/**
 * @param resource $link_identifier
 * @return resource|false
 */
function ldap_mod_add_ext($link_identifier, string $dn, array $entry, array $servercontrols = []) {}


/**
 * @param resource $link_identifier
 */
function ldap_mod_replace($link_identifier, string $dn, array $entry, array $servercontrols = []): bool {}

/**
 * @param resource $link_identifier
 * @return resource|false
 */
function ldap_mod_replace_ext($link_identifier, string $dn, array $entry, array $servercontrols = []) {}

/**
 * @param resource $link_identifier
 */
function ldap_mod_del($link_identifier, string $dn, array $entry, array $servercontrols = []): bool {}

/**
 * @param resource $link_identifier
 * @return resource|false
 */
function ldap_mod_del_ext($link_identifier, string $dn, array $entry, array $servercontrols = []) {}

/**
 * @param resource $link
 */
function ldap_errno($link): int {}

/**
 * @param resource $link
 */
function ldap_error($link): string {}

function ldap_err2str(int $errno): string {}

/** @param resource $link_identifier */
function ldap_compare($link_identifier, string $dn, string $attribute, string $value, array $servercontrols = []): bool|int {}


#ifdef LDAP_CONTROL_PAGEDRESULTS
/**
 * @param resource $link
 */
function ldap_control_paged_result($link, int $pagesize, bool $iscritical = false, string $cookie = ''): bool {}

/**
 * @param resource $link
 * @param resource $result
 */
function ldap_control_paged_result_response($link, $result, &$cookie = null, &$estimated = null): bool {}
#endif

#if (LDAP_API_VERSION > 2000) || HAVE_NSLDAP || HAVE_ORALDAP
/**
 * @param resource $link_identifier
 */
function ldap_rename($link_identifier, string $dn, string $newrdn, string $newparent, bool $deleteoldrdn, array $servercontrols = []): bool {}

/**
 * @param resource $link_identifier
 * @return resource|false
 */
function ldap_rename_ext($link_identifier, string $dn, string $newrdn, string $newparent, bool $deleteoldrdn, array $servercontrols = []) {}


/**
 * @param resource $link_identifier
 */
function ldap_get_option($link_identifier, int $option, &$retval = null): bool {}

/**
 * @param ?resource $link_identifier
 */
function ldap_set_option($link_identifier, int $option, $newval): bool {}

/**
 * @param resource $link
 * @param resource $result
 * @return resource|false
 */
function ldap_first_reference($link, $result) {}

/**
 * @param resource $link
 * @param resource $entry
 * @return resource|false
 */
function ldap_next_reference($link, $entry) {}

#ifdef HAVE_LDAP_PARSE_REFERENCE
/**
 * @param resource $link
 * @param resource $entry
 */
function ldap_parse_reference($link, $entry, &$referrals): bool {}
#endif

#ifdef HAVE_LDAP_PARSE_RESULT
/**
 * @param resource $link
 * @param resource $result
 */
function ldap_parse_result($link, $result, &$errcode, &$matcheddn = null, &$errmsg = null, &$referrals = null, &$serverctrls = null): bool {}
#endif
#endif

#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
/**
 * @param resource $link
 * @param callable $callback
 */
function ldap_set_rebind_proc($link, $callback): bool {}
#endif

#ifdef HAVE_LDAP_START_TLS_S
function ldap_start_tls($link_identifier): bool {}
#endif

function ldap_escape(string $value, string $ignore = '', int $flags = 0): string {}

#ifdef STR_TRANSLATION
function ldap_t61_to_8859(string $value): string|false {}

function ldap_8859_to_t61(string $value): string|false {}
#endif


#ifdef HAVE_LDAP_EXTENDED_OPERATION_S
/**
 * @param resource $link
 * @return resource|bool
 */
function ldap_exop($link, string $reqoid, ?string $reqdata = null, ?array $servercontrols = [], &$retdata = null, &$retoid = null) {}
#endif

#ifdef HAVE_LDAP_PASSWD
/** @param resource $link */
function ldap_exop_passwd($link, string $user = '', string $oldpw = '', string $newpw = '', &$serverctrls = null): string|bool {}
#endif


#ifdef HAVE_LDAP_WHOAMI_S
/** @param resource $link */
function ldap_exop_whoami($link): string|bool {}
#endif


#ifdef HAVE_LDAP_REFRESH_S
/** @param resource $link */
function ldap_exop_refresh($link, string $dn, $ttl): int|false {}
#endif


#ifdef HAVE_LDAP_PARSE_EXTENDED_RESULT
/**
 * @param resource $link
 * @param resource $result
 */
function ldap_parse_exop($link, $result, &$retdata = null, &$retoid = null): bool {}
#endif
