--TEST--
Function snmpset (without MIBs loading)
--CREDITS--
Boris Lytochkin
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--ENV--
MIBS=
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

//EXPECTF format is quickprint OFF
snmp_set_quick_print(false);
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);

echo "Check error handing\n";
echo "Nonexisting OID\n";
$z = snmpset($hostname, $communityWrite, '.1.3.6.777.888.999.444.0', 's', 'bbb', $timeout, $retries);
var_dump($z);

echo "Bogus OID\n";
$z = snmpset($hostname, $communityWrite, '.1.3...6.777.888.999.444.0', 's', 'bbb', $timeout, $retries);
var_dump($z);


echo "Checking working\n";
$oid1 = '.1.3.6.1.2.1.1.4.0';
$oldvalue1 = snmpget($hostname, $communityWrite, $oid1, $timeout, $retries);
$newvalue1 = $oldvalue1 . '0';

echo "Single OID\n";
$z = snmpset($hostname, $communityWrite, $oid1, 's', $newvalue1, $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $newvalue1));
$z = snmpset($hostname, $communityWrite, $oid1, 's', $oldvalue1, $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));

?>
--EXPECTF--
Check error handing
Nonexisting OID

Warning: snmpset(): Error in packet at '%s': (noSuchName) There is no such variable name in this MIB. in %s on line %d
bool(false)
Bogus OID

Warning: snmpset(): Invalid object identifier: %s in %s on line %d
bool(false)
Checking working
Single OID
bool(true)
bool(true)
bool(true)
bool(true)
