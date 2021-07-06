--TEST--
Generic timeout (wrong community)
--CREDITS--
Boris Lytochkin
--EXTENSIONS--
snmp
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

//EXPECTF format is quickprint OFF
snmp_set_quick_print(false);
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);

var_dump(snmpget($hostname, 'timeout_community_432', '.1.3.6.1.2.1.1.1.0', $timeout, $retries));
var_dump(snmpget($hostname, 'timeout_community_432', array('.1.3.6.1.2.1.1.1.0'), $timeout, $retries));

?>
--EXPECTF--
Warning: snmpget(): No response from %s in %s on line %d
bool(false)

Warning: snmpget(): No response from %s in %s on line %d
bool(false)
