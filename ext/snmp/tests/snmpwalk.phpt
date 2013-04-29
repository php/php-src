--TEST--                                 
Function snmpwalk
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
var_dump(snmpwalk($hostname, $community, '.1.3.6.1.2.1.1', ''));
var_dump(snmpwalk($hostname, $community, '.1.3.6.1.2.1.1', $timeout, ''));

echo "Checking working\n";
echo "Single OID\n";
$return = snmpwalk($hostname, $community, '.1.3.6.1.2.1.1', $timeout, $retries);

var_dump(gettype($return));
var_dump(sizeof($return));
var_dump(gettype($return[0]));
var_dump(gettype($return[1]));

echo "Single OID in array\n";
$return = snmpwalk($hostname, $community, array('.1.3.6.1.2.1.1'), $timeout, $retries);
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
$return = snmpwalk($hostname, $community, array('.1.3.6.1.2.1.1', '.1.3.6'), $timeout, $retries);
var_dump($return);

echo "Multiple OID with wrong OID\n";
$return = snmpwalk($hostname, $community, array('.1.3.6.1.2.1.1', '.1.3.6...1'), $timeout, $retries);
var_dump($return);
$return = snmpwalk($hostname, $community, array('.1.3.6...1', '.1.3.6.1.2.1.1'), $timeout, $retries);
var_dump($return);

echo "Single nonexisting OID\n";
$return = snmpwalk($hostname, $community, array('.1.3.6.99999.0.99999.111'), $timeout, $retries);
var_dump($return);

?>
--EXPECTF--
Checking error handling

Warning: snmpwalk() expects parameter 4 to be long, %s given in %s on line %d
bool(false)

Warning: snmpwalk() expects parameter 5 to be long, %s given in %s on line %d
bool(false)
Checking working
Single OID
%unicode|string%(5) "array"
int(%d)
%unicode|string%(6) "string"
%unicode|string%(6) "string"
Single OID in array
%unicode|string%(5) "array"
%unicode|string%(6) "string"
Default OID
%unicode|string%(5) "array"
%unicode|string%(6) "string"
More error handling
Single incorrect OID

Warning: snmpwalk(): Invalid object identifier: %s in %s on line %d
bool(false)
Multiple correct OID

Warning: snmpwalk(): Multi OID walks are not supported! in %s on line %d
bool(false)
Multiple OID with wrong OID

Warning: snmpwalk(): Multi OID walks are not supported! in %s on line %d
bool(false)

Warning: snmpwalk(): Multi OID walks are not supported! in %s on line %d
bool(false)
Single nonexisting OID

Warning: snmpwalk(): Error in packet at '%s': (noSuchName) There is no such variable name in this MIB. in %s on line %d
bool(false)
