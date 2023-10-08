<?php

/** @generate-class-entries */

namespace {
    /**
     * @var int
     * @cvalue LDAP_DEREF_NEVER
     */
    const LDAP_DEREF_NEVER = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_DEREF_SEARCHING
     */
    const LDAP_DEREF_SEARCHING = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_DEREF_FINDING
     */
    const LDAP_DEREF_FINDING = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_DEREF_ALWAYS
     */
    const LDAP_DEREF_ALWAYS = UNKNOWN;

    /**
     * @var int
     * @cvalue LDAP_MODIFY_BATCH_ADD
     */
    const LDAP_MODIFY_BATCH_ADD = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_MODIFY_BATCH_REMOVE
     */
    const LDAP_MODIFY_BATCH_REMOVE = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_MODIFY_BATCH_REMOVE_ALL
     */
    const LDAP_MODIFY_BATCH_REMOVE_ALL = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_MODIFY_BATCH_REPLACE
     */
    const LDAP_MODIFY_BATCH_REPLACE = UNKNOWN;
    /**
     * @var string
     * @cvalue LDAP_MODIFY_BATCH_ATTRIB
     */
    const LDAP_MODIFY_BATCH_ATTRIB = UNKNOWN;
    /**
     * @var string
     * @cvalue LDAP_MODIFY_BATCH_MODTYPE
     */
    const LDAP_MODIFY_BATCH_MODTYPE = UNKNOWN;
    /**
     * @var string
     * @cvalue LDAP_MODIFY_BATCH_VALUES
     */
    const LDAP_MODIFY_BATCH_VALUES = UNKNOWN;

#if ((LDAP_API_VERSION > 2000) || defined(HAVE_ORALDAP))
    /**
     * @var int
     * @cvalue LDAP_OPT_DEREF
     */
    const LDAP_OPT_DEREF = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_SIZELIMIT
     */
    const LDAP_OPT_SIZELIMIT = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_TIMELIMIT
     */
    const LDAP_OPT_TIMELIMIT = UNKNOWN;
#ifdef LDAP_OPT_NETWORK_TIMEOUT
    /**
     * @var int
     * @cvalue LDAP_OPT_NETWORK_TIMEOUT
     */
    const LDAP_OPT_NETWORK_TIMEOUT = UNKNOWN;
#endif
#if (!defined(LDAP_OPT_NETWORK_TIMEOUT) && defined(LDAP_X_OPT_CONNECT_TIMEOUT))
    /**
     * @var int
     * @cvalue LDAP_X_OPT_CONNECT_TIMEOUT
     */
    const LDAP_OPT_NETWORK_TIMEOUT = UNKNOWN;
#endif
#ifdef LDAP_OPT_TIMEOUT
    /**
     * @var int
     * @cvalue LDAP_OPT_TIMEOUT
     */
    const LDAP_OPT_TIMEOUT = UNKNOWN;
#endif
    /**
     * @var int
     * @cvalue LDAP_OPT_PROTOCOL_VERSION
     */
    const LDAP_OPT_PROTOCOL_VERSION = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_ERROR_NUMBER
     */
    const LDAP_OPT_ERROR_NUMBER = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_REFERRALS
     */
    const LDAP_OPT_REFERRALS = UNKNOWN;
#ifdef LDAP_OPT_RESTART
    /**
     * @var int
     * @cvalue LDAP_OPT_RESTART
     */
    const LDAP_OPT_RESTART = UNKNOWN;
#endif
#ifdef LDAP_OPT_HOST_NAME
    /**
     * @var int
     * @cvalue LDAP_OPT_HOST_NAME
     */
    const LDAP_OPT_HOST_NAME = UNKNOWN;
#endif
    /**
     * @var int
     * @cvalue LDAP_OPT_ERROR_STRING
     */
    const LDAP_OPT_ERROR_STRING = UNKNOWN;
#ifdef LDAP_OPT_MATCHED_DN
    /**
     * @var int
     * @cvalue LDAP_OPT_MATCHED_DN
     */
    const LDAP_OPT_MATCHED_DN = UNKNOWN;
#endif
    /**
     * @var int
     * @cvalue LDAP_OPT_SERVER_CONTROLS
     */
    const LDAP_OPT_SERVER_CONTROLS = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_CLIENT_CONTROLS
     */
    const LDAP_OPT_CLIENT_CONTROLS = UNKNOWN;
#endif
#ifdef LDAP_OPT_DEBUG_LEVEL
    /**
     * @var int
     * @cvalue LDAP_OPT_DEBUG_LEVEL
     */
    const LDAP_OPT_DEBUG_LEVEL = UNKNOWN;
#endif

#ifdef LDAP_OPT_DIAGNOSTIC_MESSAGE
    /**
     * @var int
     * @cvalue LDAP_OPT_DIAGNOSTIC_MESSAGE
     */
    const LDAP_OPT_DIAGNOSTIC_MESSAGE = UNKNOWN;
#endif

#ifdef HAVE_LDAP_SASL
    /**
     * @var int
     * @cvalue LDAP_OPT_X_SASL_MECH
     */
    const LDAP_OPT_X_SASL_MECH = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_SASL_REALM
     */
    const LDAP_OPT_X_SASL_REALM = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_SASL_AUTHCID
     */
    const LDAP_OPT_X_SASL_AUTHCID = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_SASL_AUTHZID
     */
    const LDAP_OPT_X_SASL_AUTHZID = UNKNOWN;
#endif

#ifdef LDAP_OPT_X_SASL_NOCANON
    /**
     * @var int
     * @cvalue LDAP_OPT_X_SASL_NOCANON
     */
    const LDAP_OPT_X_SASL_NOCANON = UNKNOWN;
#endif
#ifdef LDAP_OPT_X_SASL_USERNAME
    /**
     * @var int
     * @cvalue LDAP_OPT_X_SASL_USERNAME
     */
    const LDAP_OPT_X_SASL_USERNAME = UNKNOWN;
#endif

#ifdef ORALDAP
    /**
     * @var int
     * @cvalue GSLC_SSL_NO_AUTH
     */
    const GSLC_SSL_NO_AUTH = UNKNOWN;
    /**
     * @var int
     * @cvalue GSLC_SSL_ONEWAY_AUTH
     */
    const GSLC_SSL_ONEWAY_AUTH = UNKNOWN;
    /**
     * @var int
     * @cvalue GSLC_SSL_TWOWAY_AUTH
     */
    const GSLC_SSL_TWOWAY_AUTH = UNKNOWN;
#endif

#if (LDAP_API_VERSION > 2000)
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_REQUIRE_CERT
     */
    const LDAP_OPT_X_TLS_REQUIRE_CERT = UNKNOWN;

    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_NEVER
     */
    const LDAP_OPT_X_TLS_NEVER = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_HARD
     */
    const LDAP_OPT_X_TLS_HARD = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_DEMAND
     */
    const LDAP_OPT_X_TLS_DEMAND = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_ALLOW
     */
    const LDAP_OPT_X_TLS_ALLOW = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_TRY
     */
    const LDAP_OPT_X_TLS_TRY = UNKNOWN;

    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_CACERTDIR
     */
    const LDAP_OPT_X_TLS_CACERTDIR = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_CACERTFILE
     */
    const LDAP_OPT_X_TLS_CACERTFILE = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_CERTFILE
     */
    const LDAP_OPT_X_TLS_CERTFILE = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_CIPHER_SUITE
     */
    const LDAP_OPT_X_TLS_CIPHER_SUITE = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_KEYFILE
     */
    const LDAP_OPT_X_TLS_KEYFILE = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_RANDOM_FILE
     */
    const LDAP_OPT_X_TLS_RANDOM_FILE = UNKNOWN;
#endif

#ifdef LDAP_OPT_X_TLS_CRLCHECK
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_CRLCHECK
     */
    const LDAP_OPT_X_TLS_CRLCHECK = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_CRL_NONE
     */
    const LDAP_OPT_X_TLS_CRL_NONE = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_CRL_PEER
     */
    const LDAP_OPT_X_TLS_CRL_PEER = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_CRL_ALL
     */
    const LDAP_OPT_X_TLS_CRL_ALL = UNKNOWN;
#endif

#ifdef LDAP_OPT_X_TLS_DHFILE
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_DHFILE
     */
    const LDAP_OPT_X_TLS_DHFILE = UNKNOWN;
#endif

#ifdef LDAP_OPT_X_TLS_CRLFILE
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_CRLFILE
     */
    const LDAP_OPT_X_TLS_CRLFILE = UNKNOWN;
#endif

#ifdef LDAP_OPT_X_TLS_PROTOCOL_MIN
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_PROTOCOL_MIN
     */
    const LDAP_OPT_X_TLS_PROTOCOL_MIN = UNKNOWN;

    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_PROTOCOL_SSL2
     */
    const LDAP_OPT_X_TLS_PROTOCOL_SSL2 = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_PROTOCOL_SSL3
     */
    const LDAP_OPT_X_TLS_PROTOCOL_SSL3 = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_PROTOCOL_TLS1_0
     */
    const LDAP_OPT_X_TLS_PROTOCOL_TLS1_0 = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_PROTOCOL_TLS1_1
     */
    const LDAP_OPT_X_TLS_PROTOCOL_TLS1_1 = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_PROTOCOL_TLS1_2
     */
    const LDAP_OPT_X_TLS_PROTOCOL_TLS1_2 = UNKNOWN;
#endif

#ifdef LDAP_OPT_X_TLS_PACKAGE
    /**
     * @var int
     * @cvalue LDAP_OPT_X_TLS_PACKAGE
     */
    const LDAP_OPT_X_TLS_PACKAGE = UNKNOWN;
#endif

#ifdef LDAP_OPT_X_KEEPALIVE_IDLE
    /**
     * @var int
     * @cvalue LDAP_OPT_X_KEEPALIVE_IDLE
     */
    const LDAP_OPT_X_KEEPALIVE_IDLE = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_KEEPALIVE_PROBES
     */
    const LDAP_OPT_X_KEEPALIVE_PROBES = UNKNOWN;
    /**
     * @var int
     * @cvalue LDAP_OPT_X_KEEPALIVE_INTERVAL
     */
    const LDAP_OPT_X_KEEPALIVE_INTERVAL = UNKNOWN;
#endif

    /**
     * @var int
     * @cvalue PHP_LDAP_ESCAPE_FILTER
     */
    const LDAP_ESCAPE_FILTER = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_LDAP_ESCAPE_DN
     */
    const LDAP_ESCAPE_DN = UNKNOWN;

#ifdef HAVE_LDAP_EXTENDED_OPERATION_S
    /**
     * @var string
     * @cvalue LDAP_EXOP_START_TLS
     */
    const LDAP_EXOP_START_TLS = UNKNOWN;
    /**
     * @var string
     * @cvalue LDAP_EXOP_MODIFY_PASSWD
     */
    const LDAP_EXOP_MODIFY_PASSWD = UNKNOWN;
    /**
     * @var string
     * @cvalue LDAP_EXOP_REFRESH
     */
    const LDAP_EXOP_REFRESH = UNKNOWN;
    /**
     * @var string
     * @cvalue LDAP_EXOP_WHO_AM_I
     */
    const LDAP_EXOP_WHO_AM_I = UNKNOWN;
    /**
     * @var string
     * @cvalue LDAP_EXOP_TURN
     */
    const LDAP_EXOP_TURN = UNKNOWN;
#endif

#ifdef LDAP_CONTROL_MANAGEDSAIT
    /**
     * RFC 3296
     * @var string
     * @cvalue LDAP_CONTROL_MANAGEDSAIT
     */
    const LDAP_CONTROL_MANAGEDSAIT = UNKNOWN;
#endif
#ifdef LDAP_CONTROL_PROXY_AUTHZ
    /**
     * RFC 4370
     * @var string
     * @cvalue LDAP_CONTROL_PROXY_AUTHZ
     */
    const LDAP_CONTROL_PROXY_AUTHZ = UNKNOWN;
#endif
#ifdef LDAP_CONTROL_SUBENTRIES
    /**
     * RFC 3672
     * @var string
     * @cvalue LDAP_CONTROL_SUBENTRIES
     */
    const LDAP_CONTROL_SUBENTRIES = UNKNOWN;
#endif
#ifdef LDAP_CONTROL_VALUESRETURNFILTER
    /**
     * RFC 3876
     * @var string
     * @cvalue LDAP_CONTROL_VALUESRETURNFILTER
     */
    const LDAP_CONTROL_VALUESRETURNFILTER = UNKNOWN;
#endif
#ifdef LDAP_CONTROL_ASSERT
    /**
     * RFC 4528
     * @var string
     * @cvalue LDAP_CONTROL_ASSERT
     */
    const LDAP_CONTROL_ASSERT = UNKNOWN;
    /**
     * RFC 4527
     * @var string
     * @cvalue LDAP_CONTROL_PRE_READ
     */
    const LDAP_CONTROL_PRE_READ = UNKNOWN;
    /**
     * RFC 4527
     * @var string
     * @cvalue LDAP_CONTROL_POST_READ
     */
    const LDAP_CONTROL_POST_READ = UNKNOWN;
#endif
#ifdef LDAP_CONTROL_SORTREQUEST
    /**
     * RFC 2891
     * @var string
     * @cvalue LDAP_CONTROL_SORTREQUEST
     */
    const LDAP_CONTROL_SORTREQUEST = UNKNOWN;
    /**
     * RFC 2891
     * @var string
     * @cvalue LDAP_CONTROL_SORTRESPONSE
     */
    const LDAP_CONTROL_SORTRESPONSE = UNKNOWN;
#endif
#ifdef LDAP_CONTROL_PAGEDRESULTS
    /**
     * RFC 2696
     * @var string
     * @cvalue LDAP_CONTROL_PAGEDRESULTS
     */
    const LDAP_CONTROL_PAGEDRESULTS = UNKNOWN;
#endif
#ifdef LDAP_CONTROL_AUTHZID_REQUEST
    /**
     * RFC 3829
     * @var string
     * @cvalue LDAP_CONTROL_AUTHZID_REQUEST
     */
    const LDAP_CONTROL_AUTHZID_REQUEST = UNKNOWN;
    /**
     * RFC 3829
     * @var string
     * @cvalue LDAP_CONTROL_AUTHZID_RESPONSE
     */
    const LDAP_CONTROL_AUTHZID_RESPONSE = UNKNOWN;
#endif
#ifdef LDAP_CONTROL_SYNC
    /**
     * LDAP Content Synchronization Operation -- RFC 4533
     * @var string
     * @cvalue LDAP_CONTROL_SYNC
     */
    const LDAP_CONTROL_SYNC = UNKNOWN;
    /**
     * RLDAP Content Synchronization Operation -- RFC 4533
     * @var string
     * @cvalue LDAP_CONTROL_SYNC_STATE
     */
    const LDAP_CONTROL_SYNC_STATE = UNKNOWN;
    /**
     * RLDAP Content Synchronization Operation -- RFC 4533
     * @var string
     * @cvalue LDAP_CONTROL_SYNC_DONE
     */
    const LDAP_CONTROL_SYNC_DONE = UNKNOWN;
#endif

#ifdef LDAP_CONTROL_DONTUSECOPY
    /**
     * LDAP Don't Use Copy Control (RFC 6171)
     * @var string
     * @cvalue LDAP_CONTROL_DONTUSECOPY
     */
    const LDAP_CONTROL_DONTUSECOPY = UNKNOWN;
#endif
#ifdef LDAP_CONTROL_PASSWORDPOLICYREQUEST
    /**
     * Password policy Controls
     * @var string
     * @cvalue LDAP_CONTROL_PASSWORDPOLICYREQUEST
     */
    const LDAP_CONTROL_PASSWORDPOLICYREQUEST = UNKNOWN;
    /**
     * Password policy Controls
     * @var string
     * @cvalue LDAP_CONTROL_PASSWORDPOLICYRESPONSE
     */
    const LDAP_CONTROL_PASSWORDPOLICYRESPONSE = UNKNOWN;
#endif
#ifdef LDAP_CONTROL_X_INCREMENTAL_VALUES
    /**
     * MS Active Directory controls
     * @var string
     * @cvalue LDAP_CONTROL_X_INCREMENTAL_VALUES
     */
    const LDAP_CONTROL_X_INCREMENTAL_VALUES = UNKNOWN;
    /**
     * MS Active Directory controls
     * @var string
     * @cvalue LDAP_CONTROL_X_DOMAIN_SCOPE
     */
    const LDAP_CONTROL_X_DOMAIN_SCOPE = UNKNOWN;
    /**
     * MS Active Directory controls
     * @var string
     * @cvalue LDAP_CONTROL_X_PERMISSIVE_MODIFY
     */
    const LDAP_CONTROL_X_PERMISSIVE_MODIFY = UNKNOWN;
    /**
     * MS Active Directory controls
     * @var string
     * @cvalue LDAP_CONTROL_X_SEARCH_OPTIONS
     */
    const LDAP_CONTROL_X_SEARCH_OPTIONS = UNKNOWN;
    /**
     * MS Active Directory controls
     * @var string
     * @cvalue LDAP_CONTROL_X_TREE_DELETE
     */
    const LDAP_CONTROL_X_TREE_DELETE = UNKNOWN;
    /**
     * MS Active Directory controls
     * @var string
     * @cvalue LDAP_CONTROL_X_EXTENDED_DN
     */
    const LDAP_CONTROL_X_EXTENDED_DN = UNKNOWN;
#endif

#ifdef LDAP_CONTROL_VLVREQUEST
    /**
     * LDAP VLV
     * @var string
     * @cvalue LDAP_CONTROL_VLVREQUEST
     */
    const LDAP_CONTROL_VLVREQUEST = UNKNOWN;
    /**
     * LDAP VLV
     * @var string
     * @cvalue LDAP_CONTROL_VLVRESPONSE
     */
    const LDAP_CONTROL_VLVRESPONSE = UNKNOWN;
#endif

#ifdef HAVE_ORALDAP
    function ldap_connect(?string $uri = null, int $port = 389, string $wallet = UNKNOWN, #[\SensitiveParameter] string $password = UNKNOWN, int $auth_mode = GSLC_SSL_NO_AUTH): LDAP\Connection|false {}
#ifdef LDAP_API_FEATURE_X_OPENLDAP
    function ldap_connect_wallet(?string $uri = null, string $wallet, #[\SensitiveParameter] string $password, int $auth_mode = GSLC_SSL_NO_AUTH): LDAP\Connection|false {}
#endif
#else
    function ldap_connect(?string $uri = null, int $port = 389): LDAP\Connection|false {}
#endif

    function ldap_unbind(LDAP\Connection $ldap): bool {}

    /** @alias ldap_unbind */
    function ldap_close(LDAP\Connection $ldap): bool {}

    function ldap_bind(LDAP\Connection $ldap, ?string $dn = null, #[\SensitiveParameter] ?string $password = null): bool {}

    function ldap_bind_ext(LDAP\Connection $ldap, ?string $dn = null, #[\SensitiveParameter] ?string $password = null, ?array $controls = null): LDAP\Result|false {}

    #ifdef HAVE_LDAP_SASL
    function ldap_sasl_bind(LDAP\Connection $ldap, ?string $dn = null, #[\SensitiveParameter] ?string $password = null, ?string $mech = null, ?string $realm = null, ?string $authc_id = null, ?string $authz_id = null, ?string $props = null): bool {}
    #endif

    /** @param LDAP\Connection|array $ldap */
    function ldap_read($ldap, array|string $base, array|string $filter, array $attributes = [], int $attributes_only = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, ?array $controls = null): LDAP\Result|array|false {}

    /** @param LDAP\Connection|array $ldap */
    function ldap_list($ldap, array|string $base, array|string $filter, array $attributes = [], int $attributes_only = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, ?array $controls = null): LDAP\Result|array|false {}

    /** @param LDAP\Connection|array $ldap */
    function ldap_search($ldap, array|string $base, array|string $filter, array $attributes = [], int $attributes_only = 0, int $sizelimit = -1, int $timelimit = -1, int $deref = LDAP_DEREF_NEVER, ?array $controls = null): LDAP\Result|array|false {}

    function ldap_free_result(LDAP\Result $result): bool {}

    function ldap_count_entries(LDAP\Connection $ldap, LDAP\Result $result): int {}

    function ldap_first_entry(LDAP\Connection $ldap, LDAP\Result $result): LDAP\ResultEntry|false {}

    function ldap_next_entry(LDAP\Connection $ldap, LDAP\ResultEntry $entry): LDAP\ResultEntry|false {}

    /**
     * @return array<int|string, int|array>|false
     * @refcount 1
     */
    function ldap_get_entries(LDAP\Connection $ldap, LDAP\Result $result): array|false {}

    function ldap_first_attribute(LDAP\Connection $ldap, LDAP\ResultEntry $entry): string|false {}

    function ldap_next_attribute(LDAP\Connection $ldap, LDAP\ResultEntry $entry): string|false {}

    /**
     * @return array<int|string, int|string|array>
     * @refcount 1
     */
    function ldap_get_attributes(LDAP\Connection $ldap, LDAP\ResultEntry $entry): array {}

    /**
     * @return array<int|string, int|string>|false
     * @refcount 1
     */
    function ldap_get_values_len(LDAP\Connection $ldap, LDAP\ResultEntry $entry, string $attribute): array|false {}

    /**
     * @return array<int|string, int|string>|false
     * @refcount 1
     * @alias ldap_get_values_len
     */
    function ldap_get_values(LDAP\Connection $ldap, LDAP\ResultEntry $entry, string $attribute): array|false {}

    function ldap_get_dn(LDAP\Connection $ldap, LDAP\ResultEntry $entry): string|false {}

    /**
     * @return array<int|string, int|string>|false
     * @refcount 1
     */
    function ldap_explode_dn(string $dn, int $with_attrib): array|false {}

    function ldap_dn2ufn(string $dn): string|false {}

    function ldap_add(LDAP\Connection $ldap, string $dn, array $entry, ?array $controls = null): bool {}

    function ldap_add_ext(LDAP\Connection $ldap, string $dn, array $entry, ?array $controls = null): LDAP\Result|false {}

    function ldap_delete(LDAP\Connection $ldap, string $dn, ?array $controls = null): bool {}

    function ldap_delete_ext(LDAP\Connection $ldap, string $dn, ?array $controls = null): LDAP\Result|false {}

    function ldap_modify_batch(LDAP\Connection $ldap, string $dn, array $modifications_info, ?array $controls = null): bool {}

    function ldap_mod_add(LDAP\Connection $ldap, string $dn, array $entry, ?array $controls = null): bool {}

    function ldap_mod_add_ext(LDAP\Connection $ldap, string $dn, array $entry, ?array $controls = null): LDAP\Result|false {}

    function ldap_mod_replace(LDAP\Connection $ldap, string $dn, array $entry, ?array $controls = null): bool {}

    /** @alias ldap_mod_replace */
    function ldap_modify(LDAP\Connection $ldap, string $dn, array $entry, ?array $controls = null): bool {}

    function ldap_mod_replace_ext(LDAP\Connection $ldap, string $dn, array $entry, ?array $controls = null): LDAP\Result|false {}

    function ldap_mod_del(LDAP\Connection $ldap, string $dn, array $entry, ?array $controls = null): bool {}

    function ldap_mod_del_ext(LDAP\Connection $ldap, string $dn, array $entry, ?array $controls = null): LDAP\Result|false {}

    function ldap_errno(LDAP\Connection $ldap): int {}

    function ldap_error(LDAP\Connection $ldap): string {}

    function ldap_err2str(int $errno): string {}

    function ldap_compare(LDAP\Connection $ldap, string $dn, string $attribute, string $value, ?array $controls = null): bool|int {}

    #if (LDAP_API_VERSION > 2000) || defined(HAVE_ORALDAP)
    function ldap_rename(LDAP\Connection $ldap, string $dn, string $new_rdn, string $new_parent, bool $delete_old_rdn, ?array $controls = null): bool {}

    function ldap_rename_ext(LDAP\Connection $ldap, string $dn, string $new_rdn, string $new_parent, bool $delete_old_rdn, ?array $controls = null): LDAP\Result|false {}

    /** @param array|string|int $value */
    function ldap_get_option(LDAP\Connection $ldap, int $option, &$value = null): bool {}

    /** @param array|string|int|bool $value */
    function ldap_set_option(?LDAP\Connection $ldap, int $option, $value): bool {}

    function ldap_count_references(LDAP\Connection $ldap, LDAP\Result $result): int {}

    function ldap_first_reference(LDAP\Connection $ldap, LDAP\Result $result): LDAP\ResultEntry|false {}

    function ldap_next_reference(LDAP\Connection $ldap, LDAP\ResultEntry $entry): LDAP\ResultEntry|false {}

    #ifdef HAVE_LDAP_PARSE_REFERENCE
    /** @param array $referrals */
    function ldap_parse_reference(LDAP\Connection $ldap, LDAP\ResultEntry $entry, &$referrals): bool {}
    #endif

    #ifdef HAVE_LDAP_PARSE_RESULT
    /**
     * @param int $error_code
     * @param string $matched_dn
     * @param string $error_message
     * @param array $referrals
     * @param array $controls
     */
    function ldap_parse_result(LDAP\Connection $ldap, LDAP\Result $result, &$error_code, &$matched_dn = null, &$error_message = null, &$referrals = null, &$controls = null): bool {}
    #endif
    #endif

    #if (defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC))
    function ldap_set_rebind_proc(LDAP\Connection $ldap, ?callable $callback): bool {}
    #endif

    #ifdef HAVE_LDAP_START_TLS_S
    function ldap_start_tls(LDAP\Connection $ldap): bool {}
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
    function ldap_exop(LDAP\Connection $ldap, string $request_oid, ?string $request_data = null, ?array $controls = NULL, &$response_data = UNKNOWN, &$response_oid = null): LDAP\Result|bool {}
    /**
     * @param string $response_data
     * @param string $response_oid
     */
    function ldap_exop_sync(LDAP\Connection $ldap, string $request_oid, ?string $request_data = null, ?array $controls = NULL, &$response_data = null, &$response_oid = null): LDAP\Result|bool {}
    #endif

    #ifdef HAVE_LDAP_PASSWD
    /**
     * @param array $controls
     */
    function ldap_exop_passwd(LDAP\Connection $ldap, string $user = "", #[\SensitiveParameter] string $old_password = "", #[\SensitiveParameter] string $new_password = "", &$controls = null): string|bool {}
    #endif


    #ifdef HAVE_LDAP_WHOAMI_S
    function ldap_exop_whoami(LDAP\Connection $ldap): string|false {}
    #endif

    #ifdef HAVE_LDAP_REFRESH_S
    function ldap_exop_refresh(LDAP\Connection $ldap, string $dn, int $ttl): int|false {}
    #endif

    #ifdef HAVE_LDAP_PARSE_EXTENDED_RESULT
    /**
     * @param string $response_data
     * @param string $response_oid
     */
    function ldap_parse_exop(LDAP\Connection $ldap, LDAP\Result $result, &$response_data = null, &$response_oid = null): bool {}
    #endif
}

namespace LDAP {
    /**
     * @strict-properties
     * @not-serializable
     */
    final class Connection
    {
    }

    /**
     * @strict-properties
     * @not-serializable
     */
    final class Result
    {
    }

    /**
     * @strict-properties
     * @not-serializable
     */
    final class ResultEntry
    {
    }
}

