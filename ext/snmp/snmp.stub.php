<?php

/** @generate-class-entries */

function snmpget(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): stdClass|array|string|bool {}

function snmpgetnext(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): stdClass|array|string|bool {}

function snmpwalk(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmprealwalk(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

/** @alias snmprealwalk */
function snmpwalkoid(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmpset(string $hostname, string $community, array|string $object_id, array|string $type, array|string $value, int $timeout = -1, int $retries = -1): array|bool {}

function snmp_get_quick_print(): bool {}

function snmp_set_quick_print(bool $enable): bool {}

function snmp_set_enum_print(bool $enable): bool {}

function snmp_set_oid_output_format(int $format): bool {}

/** @alias snmp_set_oid_output_format */
function snmp_set_oid_numeric_print(int $format): bool {}

function snmp2_get(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): stdClass|array|string|bool {}

function snmp2_getnext(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): stdClass|array|string|bool {}

function snmp2_walk(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmp2_real_walk(string $hostname, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmp2_set(string $hostname, string $community, array|string $object_id, array|string $type, array|string $value, int $timeout = -1, int $retries = -1): array|bool {}

function snmp3_get(
    string $hostname, string $security_name, string $security_level,
    string $auth_protocol, string $auth_passphrase,
    string $privacy_protocol, string $privacy_passphrase,
    array|string $object_id, int $timeout = -1, int $retries = -1): stdClass|array|string|bool {}

function snmp3_getnext(
    string $hostname, string $security_name, string $security_level,
    string $auth_protocol, string $auth_passphrase,
    string $privacy_protocol, string $privacy_passphrase,
    array|string $object_id, int $timeout = -1, int $retries = -1): stdClass|array|string|bool {}

function snmp3_walk(
    string $hostname, string $security_name, string $security_level,
    string $auth_protocol, string $auth_passphrase,
    string $privacy_protocol, string $privacy_passphrase,
    array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmp3_real_walk(
    string $hostname, string $security_name, string $security_level,
    string $auth_protocol, string $auth_passphrase,
    string $privacy_protocol, string $privacy_passphrase,
    array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmp3_set(
    string $hostname, string $security_name, string $security_level,
    string $auth_protocol, string $auth_passphrase,
    string $privacy_protocol, string $privacy_passphrase,
    array|string $object_id, array|string $type, array|string $value,
    int $timeout = -1, int $retries = -1): array|bool {}

function snmp_set_valueretrieval(int $method): bool {}

function snmp_get_valueretrieval(): int {}

function snmp_read_mib(string $filename): bool {}

class SNMP
{
    public function __construct(int $version, string $hostname, string $community, int $timeout = -1, int $retries = -1) {}

    /** @return bool */
    public function close() {}

    /** @return bool */
    public function setSecurity(
        string $securityLevel, string $authProtocol = "", string $authPassphrase = "",
        string $privacyProtocol = "", string $privacyPassphrase = "",
        string $contextName = "", string $contextEngineId = "") {}

    /** @return array|bool */
    public function get(array|string $objectId, bool $preserveKeys = false) {}

    /** @return array|bool */
    public function getnext(array|string $objectId) {}

    /** @return array|bool */
    public function walk(array|string $objectId, bool $suffixAsKey = false, int $maxRepetitions = -1, int $nonRepeaters = -1) {}

    /** @return array|bool */
    public function set(array|string $objectId, array|string $type, array|string $value) {}

    /** @return int */
    public function getErrno() {}

    /** @return string */
    public function getError() {}
}

class SNMPException extends RuntimeException
{
}
