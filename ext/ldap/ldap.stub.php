<?php

/** @generate-function-entries */

#ifdef HAVE_ORALDAP
/** @return resource|false */
function ldap_connect(?string $uri = null, int $port = 389, string $wallet = UNKNOWN, string $password = UNKNOWN, int $auth_mode = GSLC_SSL_NO_AUTH) {}
#else
/** @return resource|false */
function ldap_connect(?string $uri = null, int $port = 389) {}
#endif

/** @param resource $ldap */
function ldap_unbind($ldap): bool {}

/**
 * @param resource $ldap
 * @alias ldap_unbind
 */
function ldap_close($ldap): bool {}

/** @param resource $ldap */
function ldap_bind($ldap, ?string $dn = null, ?string $password = null): bool {}

/**
 * @param resource $ldap
 * @return resource|false
 */
function ldap_bind_ext($ldap, ?string $dn = null, ?string $password = null, ?array $controls = null) {}

#ifdef HAVE_LDAP_SASL
/** @param resource $ldap */
function ldap_sasl_bind($ldap, ?string $dn = null, ?string $password = null, ?string $mech = null, ?string $realm = null, ?string $authc_id = null, ?string $authz_id = null, ?string $props = null): bool {}
#endif

/**
 * @param resource|array $ldap
 * @return resource|array|false
 */
function ldap_read($ldap, array|string $base, array|string $filter, array $attributes = [], int $attributes_only = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, ?array $controls = null) {}

/**
 * @param resource|array $ldap
 * @return resource|array|false
 */
function ldap_list($ldap, array|string $base, array|string $filter, array $attributes = [], int $attributes_only = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, ?array $controls = null) {}

/**
 * @param resource|array $ldap
 * @return resource|array|false
 */
function ldap_search($ldap, array|string $base, array|string $filter, array $attributes = [], int $attributes_only = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, ?array $controls = null) {}

/** @param resource $ldap */
function ldap_free_result($ldap): bool {}


/**
 * @param resource $ldap
 * @param resource $result
 */
function ldap_count_entries($ldap, $result): int {}

/**
 * @param resource $ldap
 * @param resource $result
 * @return resource|false
 */
function ldap_first_entry($ldap, $result) {}

/**
 * @param resource $ldap
 * @param resource $entry
 * @return resource|false
 */
function ldap_next_entry($ldap, $entry) {}

/**
 * @param resource $ldap
 * @param resource $result
 */
function ldap_get_entries($ldap, $result): array|false {}

/**
 * @param resource $ldap
 * @param resource $entry
 */
function ldap_first_attribute($ldap, $entry): string|false {}

/**
 * @param resource $ldap
 * @param resource $entry
 */
function ldap_next_attribute($ldap, $entry): string|false {}

/**
 * @param resource $ldap
 * @param resource $entry
 */
function ldap_get_attributes($ldap, $entry): array {}

/**
 * @param resource $ldap
 * @param resource $entry
 */
function ldap_get_values_len($ldap, $entry, string $attribute): array|false {}

/**
 * @param resource $ldap
 * @param resource $entry
 * @alias ldap_get_values_len
 */
function ldap_get_values($ldap, $entry, string $attribute): array|false {}

/**
 * @param resource $ldap
 * @param resource $entry
 */
function ldap_get_dn($ldap, $entry): string|false {}

function ldap_explode_dn(string $dn, int $with_attrib): array|false {}

function ldap_dn2ufn(string $dn): string|false {}

/** @param resource $ldap */
function ldap_add($ldap, string $dn, array $entry, ?array $controls = null): bool {}

/**
 * @param resource $ldap
 * @return resource|false
 */
function ldap_add_ext($ldap, string $dn, array $entry, ?array $controls = null) {}

/** @param resource $ldap */
function ldap_delete($ldap, string $dn, ?array $controls = null): bool {}

/**
 * @param resource $ldap
 * @return resource|false
 */
function ldap_delete_ext($ldap, string $dn, ?array $controls = null) {}

/** @param resource $ldap */
function ldap_modify_batch($ldap, string $dn, array $modifications_info, ?array $controls = null): bool {}

/** @param resource $ldap */
function ldap_mod_add($ldap, string $dn, array $entry, ?array $controls = null): bool {}

/**
 * @param resource $ldap
 * @return resource|false
 */
function ldap_mod_add_ext($ldap, string $dn, array $entry, ?array $controls = null) {}

/** @param resource $ldap */
function ldap_mod_replace($ldap, string $dn, array $entry, ?array $controls = null): bool {}

/**
 * @param resource $ldap
 * @alias ldap_mod_replace
 */
function ldap_modify($ldap, string $dn, array $entry, ?array $controls = null): bool {}

/**
 * @param resource $ldap
 * @return resource|false
 */
function ldap_mod_replace_ext($ldap, string $dn, array $entry, ?array $controls = null) {}

/** @param resource $ldap */
function ldap_mod_del($ldap, string $dn, array $entry, ?array $controls = null): bool {}

/**
 * @param resource $ldap
 * @return resource|false
 */
function ldap_mod_del_ext($ldap, string $dn, array $entry, ?array $controls = null) {}

/** @param resource $ldap */
function ldap_errno($ldap): int {}

/** @param resource $ldap */
function ldap_error($ldap): string {}

function ldap_err2str(int $errno): string {}

/** @param resource $ldap */
function ldap_compare($ldap, string $dn, string $attribute, string $value, ?array $controls = null): bool|int {}

#if (LDAP_API_VERSION > 2000) || defined(HAVE_ORALDAP)
/** @param resource $ldap */
function ldap_rename($ldap, string $dn, string $new_rdn, string $new_parent, bool $delete_old_rdn, ?array $controls = null): bool {}

/**
 * @param resource $ldap
 * @return resource|false
 */
function ldap_rename_ext($ldap, string $dn, string $new_rdn, string $new_parent, bool $delete_old_rdn, ?array $controls = null) {}


/**
 * @param resource $ldap
 * @param array|string|int $value
 */
function ldap_get_option($ldap, int $option, &$value = null): bool {}

/**
 * @param resource|null $ldap
 * @param array|string|int|bool $value
 */
function ldap_set_option($ldap, int $option, $value): bool {}

/**
 * @param resource $ldap
 * @param resource $result
 */
function ldap_count_references($ldap, $result): int {}

/**
 * @param resource $ldap
 * @param resource $result
 * @return resource|false
 */
function ldap_first_reference($ldap, $result) {}

/**
 * @param resource $ldap
 * @param resource $entry
 * @return resource|false
 */
function ldap_next_reference($ldap, $entry) {}

#ifdef HAVE_LDAP_PARSE_REFERENCE
/**
 * @param resource $ldap
 * @param resource $entry
 * @param array $referrals
 */
function ldap_parse_reference($ldap, $entry, &$referrals): bool {}
#endif

#ifdef HAVE_LDAP_PARSE_RESULT
/**
 * @param resource $ldap
 * @param resource $result
 * @param int $error_code
 * @param string $matched_dn
 * @param string $error_message
 * @param array $referrals
 * @param array $controls
 */
function ldap_parse_result($ldap, $result, &$error_code, &$matched_dn = null, &$error_message = null, &$referrals = null, &$controls = null): bool {}
#endif
#endif

#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
/** @param resource $ldap */
function ldap_set_rebind_proc($ldap, ?callable $callback): bool {}
#endif

#ifdef HAVE_LDAP_START_TLS_S
/** @param resource $ldap */
function ldap_start_tls($ldap): bool {}
#endif

function ldap_escape(string $value, string $ignore = "", int $flags = 0): string {}

#ifdef STR_TRANSLATION
function ldap_t61_to_8859(string $value): string|false {}

function ldap_8859_to_t61(string $value): string|false {}
#endif


#ifdef HAVE_LDAP_EXTENDED_OPERATION_S
/**
 * @param resource $ldap
 * @param string $response_data
 * @param string $response_oid
 * @return resource|bool
 */
function ldap_exop($ldap, string $request_oid, ?string $request_data = null, ?array $controls = NULL, &$response_data = UNKNOWN, &$response_oid = null) {}
#endif

#ifdef HAVE_LDAP_PASSWD
/**
 * @param resource $ldap
 * @param array $controls
 */
function ldap_exop_passwd($ldap, string $user = "", string $old_password = "", string $new_password = "", &$controls = null): string|bool {}
#endif


#ifdef HAVE_LDAP_WHOAMI_S
/** @param resource $ldap */
function ldap_exop_whoami($ldap): string|false {}
#endif

#ifdef HAVE_LDAP_REFRESH_S
/** @param resource $ldap */
function ldap_exop_refresh($ldap, string $dn, int $ttl): int|false {}
#endif

#ifdef HAVE_LDAP_PARSE_EXTENDED_RESULT
/**
 * @param resource $ldap
 * @param resource $result
 * @param string $response_data
 * @param string $response_oid
 */
function ldap_parse_exop($ldap, $result, &$response_data = null, &$response_oid = null): bool {}
#endif
