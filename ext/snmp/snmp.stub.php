<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue NETSNMP_OID_OUTPUT_SUFFIX
 */
const SNMP_OID_OUTPUT_SUFFIX = UNKNOWN;
/**
 * @var int
 * @cvalue NETSNMP_OID_OUTPUT_MODULE
 */
const SNMP_OID_OUTPUT_MODULE = UNKNOWN;
/**
 * @var int
 * @cvalue NETSNMP_OID_OUTPUT_FULL
 */
const SNMP_OID_OUTPUT_FULL = UNKNOWN;
/**
 * @var int
 * @cvalue NETSNMP_OID_OUTPUT_NUMERIC
 */
const SNMP_OID_OUTPUT_NUMERIC = UNKNOWN;
/**
 * @var int
 * @cvalue NETSNMP_OID_OUTPUT_UCD
 */
const SNMP_OID_OUTPUT_UCD = UNKNOWN;
/**
 * @var int
 * @cvalue NETSNMP_OID_OUTPUT_NONE
 */
const SNMP_OID_OUTPUT_NONE = UNKNOWN;

/**
 * @var int
 * @cvalue SNMP_VALUE_LIBRARY
 */
const SNMP_VALUE_LIBRARY = UNKNOWN;
/**
 * @var int
 * @cvalue SNMP_VALUE_PLAIN
 */
const SNMP_VALUE_PLAIN = UNKNOWN;
/**
 * @var int
 * @cvalue SNMP_VALUE_OBJECT
 */
const SNMP_VALUE_OBJECT = UNKNOWN;

/**
 * @var int
 * @cvalue ASN_BIT_STR
 */
const SNMP_BIT_STR = UNKNOWN;
/**
 * @var int
 * @cvalue ASN_OCTET_STR
 */
const SNMP_OCTET_STR = UNKNOWN;
/**
 * @var int
 * @cvalue ASN_OPAQUE
 */
const SNMP_OPAQUE = UNKNOWN;
/**
 * @var int
 * @cvalue ASN_NULL
 */
const SNMP_NULL = UNKNOWN;
/**
 * @var int
 * @cvalue ASN_OBJECT_ID
 */
const SNMP_OBJECT_ID = UNKNOWN;
/**
 * @var int
 * @cvalue ASN_IPADDRESS
 */
const SNMP_IPADDRESS = UNKNOWN;
/**
 * @var int
 * @cvalue ASN_GAUGE
 */
const SNMP_COUNTER = UNKNOWN;
/**
 * @var int
 * @cvalue ASN_UNSIGNED
 */
const SNMP_UNSIGNED = UNKNOWN;
/**
 * @var int
 * @cvalue ASN_TIMETICKS
 */
const SNMP_TIMETICKS = UNKNOWN;
/**
 * @var int
 * @cvalue ASN_UINTEGER
 */
const SNMP_UINTEGER = UNKNOWN;
/**
 * @var int
 * @cvalue ASN_INTEGER
 */
const SNMP_INTEGER = UNKNOWN;
/**
 * @var int
 * @cvalue ASN_COUNTER64
 */
const SNMP_COUNTER64 = UNKNOWN;

function snmpget(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): mixed {}

function snmpgetnext(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): mixed {}

function snmpwalk(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|false {}

function snmprealwalk(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|false {}

/** @alias snmprealwalk */
function snmpwalkoid(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|false {}

function snmpset(string $hostname, string $community, array|string $object_id, array|string $type, array|string $value, int $timeout = -1, int $retries = -1): bool {}

function snmp_get_quick_print(): bool {}

function snmp_set_quick_print(bool $enable): bool {}

function snmp_set_enum_print(bool $enable): bool {}

function snmp_set_oid_output_format(int $format): bool {}

/** @alias snmp_set_oid_output_format */
function snmp_set_oid_numeric_print(int $format): bool {}

function snmp2_get(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): mixed {}

function snmp2_getnext(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): mixed {}

function snmp2_walk(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|false {}

function snmp2_real_walk(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|false {}

function snmp2_set(string $hostname, string $community, array|string $object_id, array|string $type, array|string $value, int $timeout = -1, int $retries = -1): bool {}

function snmp3_get(
    string $hostname, string $security_name, string $security_level,
    string $auth_protocol, string $auth_passphrase,
    string $privacy_protocol, string $privacy_passphrase,
    array|string $object_id, int $timeout = -1, int $retries = -1): mixed {}

function snmp3_getnext(
    string $hostname, string $security_name, string $security_level,
    string $auth_protocol, string $auth_passphrase,
    string $privacy_protocol, string $privacy_passphrase,
    array|string $object_id, int $timeout = -1, int $retries = -1): mixed {}

function snmp3_walk(
    string $hostname, string $security_name, string $security_level,
    string $auth_protocol, string $auth_passphrase,
    string $privacy_protocol, string $privacy_passphrase,
    array|string $object_id, int $timeout = -1, int $retries = -1): array|false {}

function snmp3_real_walk(
    string $hostname, string $security_name, string $security_level,
    string $auth_protocol, string $auth_passphrase,
    string $privacy_protocol, string $privacy_passphrase,
    array|string $object_id, int $timeout = -1, int $retries = -1): array|false {}

function snmp3_set(
    string $hostname, string $security_name, string $security_level,
    string $auth_protocol, string $auth_passphrase,
    string $privacy_protocol, string $privacy_passphrase,
    array|string $object_id, array|string $type, array|string $value,
    int $timeout = -1, int $retries = -1): bool {}

function snmp_set_valueretrieval(int $method): bool {}

function snmp_get_valueretrieval(): int {}

function snmp_read_mib(string $filename): bool {}

class SNMP
{
    /**
     * @var int
     * @cvalue SNMP_VERSION_1
     * @link snmp.class.constants.version-1
     */
    public const VERSION_1 = UNKNOWN;
    /**
     * @var int
     * @cvalue SNMP_VERSION_2c
     * @link snmp.class.constants.version-2c
     */
    public const VERSION_2c = UNKNOWN;
    /**
     * @var int
     * @cvalue SNMP_VERSION_2c
     * @link snmp.class.constants.version-2c
     */
    public const VERSION_2C = UNKNOWN;
    /**
     * @var int
     * @cvalue SNMP_VERSION_3
     * @link snmp.class.constants.version-3
     */
    public const VERSION_3 = UNKNOWN;

    /**
     * @var int
     * @cvalue PHP_SNMP_ERRNO_NOERROR
     * @link snmp.class.constants.errno-noerror
     */
    public const ERRNO_NOERROR = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_SNMP_ERRNO_ANY
     * @link snmp.class.constants.errno-any
     */
    public const ERRNO_ANY = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_SNMP_ERRNO_GENERIC
     * @link snmp.class.constants.errno-generic
     */
    public const ERRNO_GENERIC = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_SNMP_ERRNO_TIMEOUT
     * @link snmp.class.constants.errno-timeout
     */
    public const ERRNO_TIMEOUT = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_SNMP_ERRNO_ERROR_IN_REPLY
     * @link snmp.class.constants.errno-error-in-reply
     */
    public const ERRNO_ERROR_IN_REPLY = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_SNMP_ERRNO_OID_NOT_INCREASING
     * @link snmp.class.constants.errno-oid-not-increasing
     */
    public const ERRNO_OID_NOT_INCREASING = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_SNMP_ERRNO_OID_PARSING_ERROR
     * @link snmp.class.constants.errno-oid-parsing-error
     */
    public const ERRNO_OID_PARSING_ERROR = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_SNMP_ERRNO_MULTIPLE_SET_QUERIES
     * @link snmp.class.constants.errno-multiple-set-queries
     */
    public const ERRNO_MULTIPLE_SET_QUERIES = UNKNOWN;

    /** @readonly */
    public array $info;
    public ?int $max_oids;
    public int $valueretrieval;
    public bool $quick_print;
    public bool $enum_print;
    public int $oid_output_format;
    public bool $oid_increasing_check;
    public int $exceptions_enabled;

    public function __construct(int $version, string $hostname, string $community, int $timeout = -1, int $retries = -1) {}

    /** @tentative-return-type */
    public function close(): bool {}

    /** @tentative-return-type */
    public function setSecurity(
        string $securityLevel, string $authProtocol = "", string $authPassphrase = "",
        string $privacyProtocol = "", string $privacyPassphrase = "",
        string $contextName = "", string $contextEngineId = ""): bool {}

    /** @tentative-return-type */
    public function get(array|string $objectId, bool $preserveKeys = false): mixed {}

    /** @tentative-return-type */
    public function getnext(array|string $objectId): mixed {}

    /** @tentative-return-type */
    public function walk(array|string $objectId, bool $suffixAsKey = false, int $maxRepetitions = -1, int $nonRepeaters = -1): array|false {}

    /** @tentative-return-type */
    public function set(array|string $objectId, array|string $type, array|string $value): bool {}

    /** @tentative-return-type */
    public function getErrno(): int {}

    /** @tentative-return-type */
    public function getError(): string {}
}

class SNMPException extends RuntimeException
{
}
