--TEST--
Function snmp2_real_walk
--CREDITS--
Olivier Doucet Olivier Doucet Boris Lytochkin
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

echo "Checking error handling\n";
var_dump(snmp2_real_walk($hostname, $community, '.1.3.6.1.2.1.1', ''));
var_dump(snmp2_real_walk($hostname, $community, '.1.3.6.1.2.1.1', $timeout, ''));

echo "Checking working\n";
echo "Single OID\n";
$return = snmp2_real_walk($hostname, $community, '.1.3.6.1.2.1.1', $timeout, $retries);
var_dump(gettype($return));
var_dump(sizeof($return));
var_dump(key($return));
var_dump(array_shift($return));

echo "Single OID in array\n";
$return = snmp2_real_walk($hostname, $community, array('.1.3.6.1.2.1.1'), $timeout, $retries);
var_dump(gettype($return));
var_dump(sizeof($return));
var_dump(key($return));
var_dump(array_shift($return));

echo "More error handling\n";
echo "Multiple correct OID\n";
$return = snmp2_real_walk($hostname, $community, array('.1.3.6.1.2.1.1', '.1.3.6'), $timeout, $retries);
var_dump($return);

echo "Multiple OID with wrong OID\n";
$return = snmp2_real_walk($hostname, $community, array('.1.3.6.1.2.1.1', '.1.3.6...1'), $timeout, $retries);
var_dump($return);
$return = snmp2_real_walk($hostname, $community, array('.1.3.6...1', '.1.3.6.1.2.1.1'), $timeout, $retries);
var_dump($return);

echo "Single nonexisting OID\n";
$return = snmp2_real_walk($hostname, $community, array('.1.3.6.99999.0.99999.111'), $timeout, $retries);
var_dump($return);

?>
--EXPECTF--
Checking error handling

Warning: snmp2_real_walk() expects parameter 4 to be int, %s given in %s on line %d
bool(false)

Warning: snmp2_real_walk() expects parameter 5 to be int, %s given in %s on line %d
bool(false)
Checking working
Single OID
string(5) "array"
int(%d)
string(%d) "%s"
string(%d) "%s"
Single OID in array
string(5) "array"
int(%d)
string(%d) "%s"
string(%d) "%s"
More error handling
Multiple correct OID

Warning: snmp2_real_walk(): Multi OID walks are not supported! in %s on line %d
bool(false)
Multiple OID with wrong OID

Warning: snmp2_real_walk(): Multi OID walks are not supported! in %s on line %d
bool(false)

Warning: snmp2_real_walk(): Multi OID walks are not supported! in %s on line %d
bool(false)
Single nonexisting OID

Warning: snmp2_real_walk(): Error in packet at '%s': No more variables left in this MIB View (It is past the end of the MIB tree) in %s on line %d
bool(false)
