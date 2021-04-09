<?php

/** @generate-class-entries */

/** @strict-properties */
final class LDAP
{
}

/** @strict-properties */
final class LDAPResult
{
}

/** @strict-properties */
final class LDAPResultEntry
{
}

#ifdef HAVE_ORALDAP
function ldap_connect(?string $uri = null, int $port = 389, string $wallet = UNKNOWN, string $password = UNKNOWN, int $auth_mode = GSLC_SSL_NO_AUTH): LDAP|false {}
#else
function ldap_connect(?string $uri = null, int $port = 389): LDAP|false {}
#endif

function ldap_unbind(LDAP $ldap): bool {}

/** @alias ldap_unbind */
function ldap_close(LDAP $ldap): bool {}

function ldap_bind(LDAP $ldap, ?string $dn = null, ?string $password = null): bool {}

function ldap_bind_ext(LDAP $ldap, ?string $dn = null, ?string $password = null, ?array $controls = null): LDAPResult|false {}

#ifdef HAVE_LDAP_SASL
function ldap_sasl_bind(LDAP $ldap, ?string $dn = null, ?string $password = null, ?string $mech = null, ?string $realm = null, ?string $authc_id = null, ?string $authz_id = null, ?string $props = null): bool {}
#endif

/** @param LDAP|array $ldap */
function ldap_read($ldap, array|string $base, array|string $filter, array $attributes = [], int $attributes_only = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, ?array $controls = null): LDAPResult|array|false {}

/** @param LDAP|array $ldap */
function ldap_list($ldap, array|string $base, array|string $filter, array $attributes = [], int $attributes_only = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, ?array $controls = null): LDAPResult|array|false {}

/** @param LDAP|array $ldap */
function ldap_search($ldap, array|string $base, array|string $filter, array $attributes = [], int $attributes_only = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, ?array $controls = null): LDAPResult|array|false {}

function ldap_free_result(LDAPResult $result): bool {}

function ldap_count_entries(LDAP $ldap, LDAPResult $result): int {}

function ldap_first_entry(LDAP $ldap, LDAPResult $result): LDAPResultEntry|false {}

function ldap_next_entry(LDAP $ldap, LDAPResultEntry $entry): LDAPResultEntry|false {}

function ldap_get_entries(LDAP $ldap, LDAPResult $result): array|false {}

function ldap_first_attribute(LDAP $ldap, LDAPResultEntry $entry): string|false {}

function ldap_next_attribute(LDAP $ldap, LDAPResultEntry $entry): string|false {}

function ldap_get_attributes(LDAP $ldap, LDAPResultEntry $entry): array {}

function ldap_get_values_len(LDAP $ldap, LDAPResultEntry $entry, string $attribute): array|false {}

/** @alias ldap_get_values_len */
function ldap_get_values(LDAP $ldap, LDAPResultEntry $entry, string $attribute): array|false {}

function ldap_get_dn(LDAP $ldap, LDAPResultEntry $entry): string|false {}

function ldap_explode_dn(string $dn, int $with_attrib): array|false {}

function ldap_dn2ufn(string $dn): string|false {}

function ldap_add(LDAP $ldap, string $dn, array $entry, ?array $controls = null): bool {}

function ldap_add_ext(LDAP $ldap, string $dn, array $entry, ?array $controls = null): LDAPResult|false {}

function ldap_delete(LDAP $ldap, string $dn, ?array $controls = null): bool {}

function ldap_delete_ext(LDAP $ldap, string $dn, ?array $controls = null): LDAPResult|false {}

function ldap_modify_batch(LDAP $ldap, string $dn, array $modifications_info, ?array $controls = null): bool {}

function ldap_mod_add(LDAP $ldap, string $dn, array $entry, ?array $controls = null): bool {}

function ldap_mod_add_ext(LDAP $ldap, string $dn, array $entry, ?array $controls = null): LDAPResult|false {}

function ldap_mod_replace(LDAP $ldap, string $dn, array $entry, ?array $controls = null): bool {}

/** @alias ldap_mod_replace */
function ldap_modify(LDAP $ldap, string $dn, array $entry, ?array $controls = null): bool {}

function ldap_mod_replace_ext(LDAP $ldap, string $dn, array $entry, ?array $controls = null): LDAPResult|false {}

function ldap_mod_del(LDAP $ldap, string $dn, array $entry, ?array $controls = null): bool {}

function ldap_mod_del_ext(LDAP $ldap, string $dn, array $entry, ?array $controls = null): LDAPResult|false {}

function ldap_errno(LDAP $ldap): int {}

function ldap_error(LDAP $ldap): string {}

function ldap_err2str(int $errno): string {}

function ldap_compare(LDAP $ldap, string $dn, string $attribute, string $value, ?array $controls = null): bool|int {}

#if (LDAP_API_VERSION > 2000) || defined(HAVE_ORALDAP)
function ldap_rename(LDAP $ldap, string $dn, string $new_rdn, string $new_parent, bool $delete_old_rdn, ?array $controls = null): bool {}

function ldap_rename_ext(LDAP $ldap, string $dn, string $new_rdn, string $new_parent, bool $delete_old_rdn, ?array $controls = null): LDAPResult|false {}

/**
 * @param array|string|int $value
 */
function ldap_get_option(LDAP $ldap, int $option, &$value = null): bool {}

/** @param array|string|int|bool $value */
function ldap_set_option(?LDAP $ldap, int $option, $value): bool {}

function ldap_count_references(LDAP $ldap, LDAPResult $result): int {}

function ldap_first_reference(LDAP $ldap, LDAPResult $result): LDAPResultEntry|false {}

function ldap_next_reference(LDAP $ldap, LDAPResultEntry $entry): LDAPResultEntry|false {}

#ifdef HAVE_LDAP_PARSE_REFERENCE
/** @param array $referrals */
function ldap_parse_reference(LDAP $ldap, LDAPResultEntry $entry, &$referrals): bool {}
#endif

#ifdef HAVE_LDAP_PARSE_RESULT
/**
 * @param int $error_code
 * @param string $matched_dn
 * @param string $error_message
 * @param array $referrals
 * @param array $controls
 */
function ldap_parse_result(LDAP $ldap, LDAPResult $result, &$error_code, &$matched_dn = null, &$error_message = null, &$referrals = null, &$controls = null): bool {}
#endif
#endif

#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)
function ldap_set_rebind_proc(LDAP $ldap, ?callable $callback): bool {}
#endif

#ifdef HAVE_LDAP_START_TLS_S
function ldap_start_tls(LDAP $ldap): bool {}
#endif

function ldap_escape(string $value, string $ignore = "", int $flags = 0): string {}

#ifdef STR_TRANSLATION
function ldap_t61_to_8859(string $value): string|false {}

function ldap_8859_to_t61(string $value): string|false {}
#endif


#ifdef HAVE_LDAP_EXTENDED_OPERATION_S
/**
 * @param string $response_data
 * @param string $response_oid
 */
function ldap_exop(LDAP $ldap, string $request_oid, ?string $request_data = null, ?array $controls = NULL, &$response_data = UNKNOWN, &$response_oid = null): LDAPResult|bool {}
#endif

#ifdef HAVE_LDAP_PASSWD
/**
 * @param array $controls
 */
function ldap_exop_passwd(LDAP $ldap, string $user = "", string $old_password = "", string $new_password = "", &$controls = null): string|bool {}
#endif


#ifdef HAVE_LDAP_WHOAMI_S
function ldap_exop_whoami(LDAP $ldap): string|false {}
#endif

#ifdef HAVE_LDAP_REFRESH_S
function ldap_exop_refresh(LDAP $ldap, string $dn, int $ttl): int|false {}
#endif

#ifdef HAVE_LDAP_PARSE_EXTENDED_RESULT
/**
 * @param string $response_data
 * @param string $response_oid
 */
function ldap_parse_exop(LDAP $ldap, LDAPResult $result, &$response_data = null, &$response_oid = null): bool {}
#endif
