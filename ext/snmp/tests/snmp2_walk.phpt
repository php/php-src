--TEST--
Function snmp2_walk
--CREDITS--
Olivier Doucet Olivier Doucet Boris Lytochkin
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

echo "Checking working\n";
echo "Single OID\n";
$return = snmp2_walk($hostname, $community, '.1.3.6.1.2.1.1', $timeout, $retries);

var_dump(gettype($return));
var_dump(sizeof($return));
var_dump(gettype($return[0]));
var_dump(gettype($return[1]));

echo "Single OID in array\n";
$return = snmp2_walk($hostname, $community, array('.1.3.6.1.2.1.1'), $timeout, $retries);
var_dump(gettype($return));
var_dump(gettype($return[0]));

echo "Default OID\n";
$return = snmpwalk($hostname, $community, '', $timeout, $retries);
var_dump(gettype($return));
var_dump(gettype($return[0]));

echo "More error handling\n";
echo "Single incorrect OID\n";
$return = snmpwalk($hostname, $community, '.1.3.6...1', $timeout, $retries);
var_dump($return);

echo "Multiple correct OID\n";
$return = snmp2_walk($hostname, $community, array('.1.3.6.1.2.1.1', '.1.3.6'), $timeout, $retries);
var_dump($return);

echo "Multiple OID with wrong OID\n";
$return = snmp2_walk($hostname, $community, array('.1.3.6.1.2.1.1', '.1.3.6...1'), $timeout, $retries);
var_dump($return);
$return = snmp2_walk($hostname, $community, array('.1.3.6...1', '.1.3.6.1.2.1.1'), $timeout, $retries);
var_dump($return);

echo "Single nonexisting OID\n";
$return = snmp2_walk($hostname, $community, array('.1.3.6.99999.0.99999.111'), $timeout, $retries);
var_dump($return);

?>
--EXPECTF--
Checking working
Single OID
string(5) "array"
int(%d)
string(6) "string"
string(6) "string"
Single OID in array
string(5) "array"
string(6) "string"
Default OID
string(5) "array"
string(6) "string"
More error handling
Single incorrect OID

Warning: snmpwalk(): Invalid object identifier: %s in %s on line %d
bool(false)
Multiple correct OID

Warning: snmp2_walk(): Multi OID walks are not supported! in %s on line %d
bool(false)
Multiple OID with wrong OID

Warning: snmp2_walk(): Multi OID walks are not supported! in %s on line %d
bool(false)

Warning: snmp2_walk(): Multi OID walks are not supported! in %s on line %d
bool(false)
Single nonexisting OID

Warning: snmp2_walk(): Error in packet at '%s': No more variables left in this MIB View (It is past the end of the MIB tree) in %s on line %d
bool(false)
