<?php

/** @generate-function-entries */

function snmpget(string $host, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmpgetnext(string $host, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmpwalk(string $host, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmprealwalk(string $host, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

/** @alias snmprealwalk */
function snmpwalkoid(string $host, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmpset(string $host, string $community, array|string $object_id, array|string $type, array|string $value, int $timeout = -1, int $retries = -1): array|bool {}

function snmp_get_quick_print(): bool {}

function snmp_set_quick_print(int $quick_print): bool {}

function snmp_set_enum_print(int $enum_print): bool {}

function snmp_set_oid_output_format(int $oid_format): bool {}

/** @alias snmp_set_oid_output_format */
function snmp_set_oid_numeric_print(int $oid_format): bool {}

function snmp2_get(string $host, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmp2_getnext(string $host, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmp2_walk(string $host, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmp2_real_walk(string $host, string $community, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmp2_set(string $host, string $community, array|string $object_id, array|string $type, array|string $value, int $timeout = -1, int $retries = -1): array|bool {}

function snmp3_get(string $host, string $sec_name, string $sec_level, string $auth_protocol, string $auth_passphrase, string $priv_protocol, string $priv_passphrase, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmp3_getnext(string $host, string $sec_name, string $sec_level, string $auth_protocol, string $auth_passphrase, string $priv_protocol, string $priv_passphrase, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmp3_walk(string $host, string $sec_name, string $sec_level, string $auth_protocol, string $auth_passphrase, string $priv_protocol, string $priv_passphrase, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmp3_real_walk(string $host, string $sec_name, string $sec_level, string $auth_protocol, string $auth_passphrase, string $priv_protocol, string $priv_passphrase, array|string $object_id, int $timeout = -1, int $retries = -1): array|bool {}

function snmp3_set(string $host, string $sec_name, string $sec_level, string $auth_protocol, string $auth_passphrase, string $priv_protocol, string $priv_passphrase, array|string $object_id, array|string $type, array|string $value, int $timeout = -1, int $retries = -1): array|bool {}

function snmp_set_valueretrieval(int $method): bool {}

function snmp_get_valueretrieval(): int {}

function snmp_read_mib(string $filename): bool {}

class SNMP
{
    public function __construct(int $version, string $host, string $community, int $timeout = -1, int $retries = -1) {}

    /** @return bool */
    public function close() {}

    /** @return bool */
    public function setSecurity(string $sec_level, string $auth_protocol = '', string $auth_passphrase = '', string $priv_protocol = '', string $priv_passphrase = '', string $contextName = '', string $contextEngineID = '') {}

    /** @return array|bool */
    public function get(array|string $object_id, bool $use_orignames = false) {}

    /** @return array|bool */
    public function getnext(array|string $object_id) {}

    /** @return array|bool */
    public function walk(array|string $object_id, bool $suffix_keys = false, int $max_repetitions = -1, int $non_repeaters = -1) {}

    /** @return array|bool */
    public function set(array|string $object_id, array|string $type, array|string $value) {}

    /** @return int */
    public function getErrno() {}

    /** @return string */
    public function getError() {}
}
