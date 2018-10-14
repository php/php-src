--TEST--
Function snmp2_set (without MIBs loading)
--CREDITS--
Boris Lytockin
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--ENV--
return <<<END
MIBS=
END;
--FILE--
<?php
require_once(dirname(__FILE__).'/snmp_include.inc');

//EXPECTF format is quickprint OFF
snmp_set_quick_print(false);
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);

echo "Check error handing\n";
echo "Nonexisting OID\n";
$z = snmp2_set($hostname, $communityWrite, '.1.3.6.777.888.999.444.0', 's', 'bbb', $timeout, $retries);
var_dump($z);

echo "Bogus OID\n";
$z = snmp2_set($hostname, $communityWrite, '.1.3.6...777.888.999.444.0', 's', 'bbb', $timeout, $retries);
var_dump($z);

echo "Checking working\n";
$oid1 = '.1.3.6.1.2.1.1.4.0';
$oldvalue1 = snmpget($hostname, $communityWrite, $oid1, $timeout, $retries);
$newvalue1 = $oldvalue1 . '0';

echo "Single OID\n";
$z = snmp2_set($hostname, $communityWrite, $oid1, 's', $newvalue1, $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $newvalue1));
$z = snmp2_set($hostname, $communityWrite, $oid1, 's', $oldvalue1, $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));

?>
--EXPECTF--
Check error handing
Nonexisting OID

Warning: snmp2_set(): Error in packet at '%s': notWritable (That object does not support modification) in %s on line %d
bool(false)
Bogus OID

Warning: snmp2_set(): Invalid object identifier: %s in %s on line %d
bool(false)
Checking working
Single OID
bool(true)
bool(true)
bool(true)
bool(true)
