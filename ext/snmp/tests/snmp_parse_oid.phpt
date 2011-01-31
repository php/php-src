--TEST--                                 
Function snmp_parse_oid
--CREDITS--
Boris Lytochkin
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
require_once(dirname(__FILE__).'/snmp_include.inc');

//EXPECTF format is quickprint OFF
snmp_set_quick_print(false);
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);

$oid1 = 'SNMPv2-MIB::sysContact.0';
$type1 = 's';
$oldvalue1 = snmpget($hostname, $communityWrite, $oid1, $timeout, $retries);
$newvalue1 = $oldvalue1 . '0';

echo "Checking SNMP_MAXOIDS_IN_PDU\n";
$i  = 0;
$oids = array();
while($i++ < 128){
	$oids[] = '.1.3.6.1.2.1.1.7.0';
}
$z = snmp2_get($hostname, $community, $oids, $timeout, $retries);
var_dump($z);

?>
--EXPECTF--
Checking SNMP_MAXOIDS_IN_PDU

Warning: snmp2_get(): Could not process more than %d OIDs in singe GET/GETNEXT/SET query in %s on line %d
bool(false)