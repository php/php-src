<?php
extension_loaded('snmp') or die('skip snmp extension not available in this build');

require_once (dirname(__FILE__).'/snmp_include.inc');

//test server is available
// this require snmpget to work ...
//snmpget  ( string $hostname  , string $community  ,
//string $object_id  [, int $timeout  [, int $retries  ]] )

if (@snmpget($hostname, $community, '.1.3.6.1.2.1.1.1.0', $timeout) === false)
    die('skip NO SNMPD on this host or community invalid');
