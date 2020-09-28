<?php

/** @generate-function-entries */

#ifdef HAVE_ORALDAP
/** @return resource|false */
function ldap_connect(?string $uri = null, int $port = 389, string $wallet = UNKNOWN, string $password = UNKNOWN, int $auth_mode = GSLC_SSL_NO_AUTH) {}
#else
/** @return resource|false */
function ldap_connect(?string $uri = null, int $port = 389) {}
#endif

/** @param resource $link */
function ldap_unbind($link): bool {}

/**
 * @param resource $link
 * @alias ldap_unbind
 */
function ldap_close($link): bool {}

/** @param resource $link */
function ldap_bind($link, ?string $dn = null, ?string $password = null): bool {}

/**
 * @param resource $link
 * @return resource|false
 */
function ldap_bind_ext($link, ?string $dn = null, ?string $password = null, array $controls = []) {}

#ifdef HAVE_LDAP_SASL
/** @param resource $link */
function ldap_sasl_bind($link, ?string $dn = null, ?string $password = null, ?string $mech = null, ?string $realm = null, ?string $authc_id = null, ?string $authz_id = null, ?string $props = null): bool {}
#endif

/**
 * @param resource|array $link
 * @return resource|false
 */
function ldap_read($link, array|string $base, array|string $filter, array $attributes = [], int $attrsonly = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, array $controls = []) {}

/**
 * @param resource|array $link
 * @return resource|false
 */
function ldap_list($link, array|string $base, array|string $filter, array $attributes = [], int $attrsonly = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, array $controls = []) {}

/**
 * @param resource|array $link
 * @return resource|false
 */
function ldap_search($link, array|string $base, array|string $filter, array $attributes = [], int $attrsonly = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, array $controls = []) {}

/** @param resource $link */
function ldap_free_result($link): bool {}


/**
 * @param resource $link
 * @param resource $result
 */
function ldap_count_entries($link, $result): int {}

/**
 * @param resource $link
 * @param resource $result
 * @return resource|false
 */
function ldap_first_entry($link, $result) {}

/**
 * @param resource $link
 * @param resource $result
 * @return resource|false
 */
function ldap_next_entry($link, $result) {}

/**
 * @param resource $link
 * @param resource $result
 */
function ldap_get_entries($link, $result): array|false {}

/**
 * @param resource $link
 * @param resource $entry
 */
function ldap_first_attribute($link, $entry): string|false {}

/**
 * @param resource $link
 * @param resource $entry
 */
function ldap_next_attribute($link, $entry): string|false {}

/**
 * @param resource $link
 * @param resource $entry
 */
function ldap_get_attributes($link, $entry): array {}

/**
 * @param resource $link
 * @param resource $entry
 */
function ldap_get_values_len($link, $entry, string $attribute): array|false {}

/**
 * @param resource $link
 * @param resource $entry
 * @alias ldap_get_values_len
 */
function ldap_get_values($link, $entry, string $attribute): array|false {}

/**
 * @param resource $link
 * @param resource $entry
 */
function ldap_get_dn($link, $entry): string|false {}

function ldap_explode_dn(string $dn, int $with_attrib): array|false {}

function ldap_dn2ufn(string $dn): string|false {}

/** @param resource $link */
function ldap_add($link, string $dn, array $entry, array $controls = []): bool {}

/**
 * @param resource $link
 * @return resource|false
 */
function ldap_add_ext($link, string $dn, array $entry, array $controls = []) {}

/** @param resource $link */
function ldap_delete($link, string $dn, array $controls = []): bool {}

/**
 * @param resource $link
 * @return resource|false
 */
function ldap_delete_ext($link, string $dn, array $controls = []) {}

/** @param resource $link */
function ldap_modify_batch($link, string $dn, array $modifications_info, array $controls = []): bool {}

/** @param resource $link */
function ldap_mod_add($link, string $dn, array $entry, array $controls = []): bool {}

/**
 * @param resource $link
 * @return resource|false
 */
function ldap_mod_add_ext($link, string $dn, array $entry, array $controls = []) {}

/** @param resource $link */
function ldap_mod_replace($link, string $dn, array $entry, array $controls = []): bool {}

/**
 * @param resource $link
 * @alias ldap_mod_replace
 */
function ldap_modify($link, string $dn, array $entry, array $controls = []): bool {}

/**
 * @param resource $link
 * @return resource|false
 */
function ldap_mod_replace_ext($link, string $dn, array $entry, array $controls = []) {}

/** @param resource $link */
function ldap_mod_del($link, string $dn, array $entry, array $controls = []): bool {}

/**
 * @param resource $link
 * @return resource|false
 */
function ldap_mod_del_ext($link, string $dn, array $entry, array $controls = []) {}

/** @param resource $link */
function ldap_errno($link): int {}

/** @param resource $link */
function ldap_error($link): string {}

function ldap_err2str(int $errno): string {}

/** @param resource $link */
function ldap_compare($link, string $dn, string $attribute, string $value, array $controls = []): bool|int {}


#ifdef LDAP_CONTROL_PAGEDRESULTS
/**
 * @param resource $link
 * @deprecated since 7.4
 */
function ldap_control_paged_result($link, int $pagesize, bool $iscritical = false, string $cookie = ""): bool {}

/**
 * @param resource $link
 * @param resource $result
 * @param string $cookie
 * @param int $estimated
 * @deprecated since 7.4
 */
function ldap_control_paged_result_response($link, $result, &$cookie = null, &$estimated = null): bool {}
#endif

#if (LDAP_API_VERSION > 2000) || defined(HAVE_ORALDAP)
/** @param resource $link */
function ldap_rename($link, string $dn, string $new_rdn, string $new_parent, bool $delete_old_rdn, array $controls = []): bool {}

/**
 * @param resource $link
 * @return resource|false
 */
function ldap_rename_ext($link, string $dn, string $newrdn, string $newparent, bool $deleteoldrdn, array $controls = []) {}


/**
 * @param resource $link
 * @param array|string|int $retval
 */
function ldap_get_option($link, int $option, &$retval = null): bool {}

/**
 * @param resource|null $link
 * @param array|string|int|bool $newval
 */
function ldap_set_option($link, int $option, $value): bool {}

/**
 * @param resource $link
 * @param resource $result
 */
function ldap_count_references($link, $result): int {}

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
 * @param array $referrals
 */
function ldap_parse_reference($link, $entry, &$referrals): bool {}
#endif

#ifdef HAVE_LDAP_PARSE_RESULT
/**
 * @param resource $link
 * @param resource $result
 * @param int $errcode
 * @param string $matcheddn
 * @param string $errmsg
 * @param array $referrals
 * @param array $controls
 */
function ldap_parse_result($link, $result, &$errcode, &$matcheddn = null, &$errmsg = null, &$referrals = null, &$controls = null): bool {}
#endif
#endif

#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
/** @param resource $link */
function ldap_set_rebind_proc($link, ?callable $callback): bool {}
#endif

#ifdef HAVE_LDAP_START_TLS_S
/** @param resource $link */
function ldap_start_tls($link): bool {}
#endif

function ldap_escape(string $value, string $ignore = "", int $flags = 0): string {}

#ifdef STR_TRANSLATION
function ldap_t61_to_8859(string $value): string|false {}

function ldap_8859_to_t61(string $value): string|false {}
#endif


#ifdef HAVE_LDAP_EXTENDED_OPERATION_S
/**
 * @param resource $link
 * @param string $retdata
 * @param string $retoid
 * @return resource|bool
 */
function ldap_exop($link, string $reqoid, ?string $reqdata = null, ?array $controls = [], &$retdata = null, &$retoid = null) {}
#endif

#ifdef HAVE_LDAP_PASSWD
/**
 * @param resource $link
 * @param array $controls
 */
function ldap_exop_passwd($link, string $user = "", string $old_password = "", string $new_password = "", &$controls = null): string|bool {}
#endif


#ifdef HAVE_LDAP_WHOAMI_S
/** @param resource $link */
function ldap_exop_whoami($link): string|bool {}
#endif

#ifdef HAVE_LDAP_REFRESH_S
/** @param resource $link */
function ldap_exop_refresh($link, string $dn, int $ttl): int|false {}
#endif

#ifdef HAVE_LDAP_PARSE_EXTENDED_RESULT
/**
 * @param resource $link
 * @param resource $result
 * @param string $retdata
 * @param string $retoid
 */
function ldap_parse_exop($link, $result, &$retdata = null, &$retoid = null): bool {}
#endif
