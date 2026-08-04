<?php
require_once (dirname(__FILE__).'/snmp_include.inc');
require_once dirname(__DIR__, 3) . '/tests/probe_cache.inc';

//test server is available
// this require snmpget to work ...
//snmpget  ( string $hostname  , string $community  ,
//string $object_id  [, int $timeout  [, int $retries  ]] )

try {
    ProbeCache::getFailure('snmp', [$hostname, $community, $timeout], static function () use ($hostname, $community, $timeout, $retries): void {
        if (@snmpget($hostname, $community, '.1.3.6.1.2.1.1.1.0', $timeout, $retries) === false) {
            throw new ProbeFailureException('NO SNMPD on this host or community invalid');
        }
    });
} catch (ProbeFailureException $e) {
    die("skip {$e->getMessage()}");
}
