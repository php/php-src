<?php

/** @param array|string $object_id */
function snmpget(string $host, string $community, $object_id, int $timeout = UNKNOWN, int $retries = UNKNOWN): array|bool {}

/** @param array|string $object_id */
function snmpgetnext(string $host, string $community, $object_id, int $timeout = UNKNOWN, int $retries = UNKNOWN): array|bool {}

/** @param array|string $object_id */
function snmpwalk(string $host, string $community, $object_id, int $timeout = UNKNOWN, int $retries = UNKNOWN): array|bool {}

/** @param array|string $object_id */
function snmprealwalk(string $host, string $community, $object_id, int $timeout = UNKNOWN, int $retries = UNKNOWN): array|bool {}

/** @param array|string $object_id */
function snmpwalkoid(string $host, string $community, $object_id, int $timeout = UNKNOWN, int $retries = UNKNOWN): array|bool {}

/**
 * @param array|string $object_id
 * @param array|string $type
 * @param array|string $value
 */
function snmpset(string $host, string $community, $object_id, $type, $value, int $timeout = UNKNOWN, int $retries = UNKNOWN): array|bool {}

function snmp_get_quick_print(): bool {}

function snmp_set_quick_print(int $quick_print): bool {}

function snmp_set_enum_print(int $enum_print): bool {}

function snmp_set_oid_output_format(int $oid_format): bool {}

function snmp_set_oid_numeric_print(int $oid_format): bool {}

/** @param array|string $object_id */
function snmp2_get(string $host, string $community, $object_id, int $timeout = UNKNOWN, int $retries = UNKNOWN): array|bool {}

/** @param array|string $object_id */
function snmp2_getnext(string $host, string $community, $object_id, int $timeout = UNKOWN, int $retries = UNKNOWN): array|bool {}

/** @param array|string $object_id */
function snmp2_walk(string $host, string $community, $object_id, int $timeout = UNKNOWN, int $retries = UNKNOWN): array|bool {}

/** @param array|string $object_id */
function snmp2_real_walk(string $host, string $community, $object_id, int $timeout = UNKNOWN, int $retries = UNKNOWN): array|bool {}

/**
 * @param array|string $object_id
 * @param array|string $type
 * @param array|string $value
 */
function snmp2_set(string $host, string $community, $object_id, $type, $value, int $timeout = UNKNOWN, int $retries = UNKNOWN): array|bool {}

/** @param array|string $object_id */
function snmp3_get(string $host, string $sec_name, string $sec_level, string $auth_protocol, string $auth_passphrase, string $priv_protocol, string $priv_passphrase, $object_id, int $timeout = UNKNOWN, int $retries = UNKNOWN): array|bool {}

/** @param array|string $object_id */
function snmp3_getnext(string $host, string $sec_name, string $sec_level, string $auth_protocol, string $auth_passphrase, string $priv_protocol, string $priv_passphrase, $object_id, int $timeout = UNKNOWN, int $retries = UNKOWN): array|bool {}

/** @param array|string $object_id */
function snmp3_walk(string $host, string $sec_name, string $sec_level, string $auth_protocol, string $auth_passphrase, string $priv_protocol, string $priv_passphrase, $object_id, int $timeout = UNKNOWN, int $retries = UNKNOWN): array|bool {}

/** @param array|string $object_id */
function snmp3_real_walk(string $host, string $sec_name, string $sec_level, string $auth_protocol, string $auth_passphrase, string $priv_protocol, string $priv_passphrase, $object_id, int $timeout = UNKNOWN, int $retries = UNKNOWN): array|bool {}

/**
 * @param array|string $object_id
 * @param array|string $type
 * @param array|string $value
 */
function snmp3_set(string $host, string $sec_name, string $sec_level, string $auth_protocol, string $auth_passphrase, string $priv_protocol, string $priv_passphrase, $object_id, $type, $value, int $timeout = UNKNOWN, int $retries = UNKNOWN): array|bool {}

function snmp_set_valueretrieval(int $method): bool {}

function snmp_get_valueretrieval(): int {}

function snmp_read_mib(string $filename): bool {}

class SNMP
{
	function __construct(int $version, string $host, string $community, int $timeout = UNKNOWN, int $retries = UNKNOWN) {}

	/** @return bool */
	function close() {}

	/** @return bool */
	function setSecurity(string $sec_level, string $auth_protocol = '', string $auth_passphrase = '', string $priv_protocol = '', string $priv_passphrase = '', string $contextName = '', string $contextEngineID = '') {}

	/**
	 * @param array|string $object_id
	 * @return array|bool
	 */
	function get($object_id, bool $use_orignames = false) {}

	/**
	 * @param array|string $object_id
	 * @return array|bool
	 */
	function getnext($object_id) {}

	/**
	 * @param array|string $object_id
	 * @return array|bool
	 */
	function walk($object_id, bool $suffix_keys = false, int $max_repetitions = UNKNOWN, int $non_repeaters = UNKNOWN) {}

	/**
	 * @param array|string $object_id
	 * @param array|string $type
	 * @param array|string $value
	 * @return array|bool
	 */
	function set($object_id, $type, $value) {}

	/** @return int */
	function getErrno() {}

	/** @return string */
	function getError() {}
}
