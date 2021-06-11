--TEST--
Function snmp2_get
--CREDITS--
Olivier Doucet & Boris Lytochkin
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

echo "Checking error handling\n";
echo "Empty OID array\n";
try {
    var_dump(snmp2_get($hostname, $community, array(), $timeout, $retries));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Checking working\n";
echo "Single OID\n";
var_dump(snmp2_get($hostname, $community, '.1.3.6.1.2.1.1.1.0', $timeout, $retries));
echo "Single OID in array\n";
var_dump(snmp2_get($hostname, $community, array('.1.3.6.1.2.1.1.1.0'), $timeout, $retries));
echo "Multiple OID\n";
var_dump(snmp2_get($hostname, $community, array('.1.3.6.1.2.1.1.1.0', '.1.3.6.1.2.1.1.3.0'), $timeout, $retries));

echo "More error handling\n";
echo "Single OID\n";
var_dump(snmp2_get($hostname, $community, '.1.3.6.1.2..1.1.1.0', $timeout, $retries));
echo "Single OID in array\n";
var_dump(snmp2_get($hostname, $community, array('.1.3.6.1.2.1...1.1.0'), $timeout, $retries));
echo "Multiple OID\n";
var_dump(snmp2_get($hostname, $community, array('.1.3.6.1.2.1...1.1.0', '.1.3.6.1.2.1.1.3.0'), $timeout, $retries));

echo "noSuchName checks\n";
echo "Single OID\n";
var_dump(snmp2_get($hostname, $community, '.1.3.6.1.2.1.1.1.110', $timeout, $retries));
echo "Single OID in array\n";
var_dump(snmp2_get($hostname, $community, array('.1.3.6.1.2.1.1.1.110'), $timeout, $retries));
echo "Multiple OID\n";
var_dump(snmp2_get($hostname, $community, array('.1.3.6.1.2.1.1.1.0', '.1.3.6.1.2.1.1.3.220'), $timeout, $retries));


?>
--EXPECTF--
Checking error handling
Empty OID array
Array of object IDs cannot be empty
Checking working
Single OID
string(%d) "%s"
Single OID in array
array(1) {
  ["%s"]=>
  string(%d) "%s"
}
Multiple OID
array(2) {
  ["%s"]=>
  string(%d) "%s"
  ["%s"]=>
  string(%d) "%d"
}
More error handling
Single OID

Warning: snmp2_get(): Invalid object identifier: .1.3.6.1.2..1.1.1.0 in %s on line %d
bool(false)
Single OID in array

Warning: snmp2_get(): Invalid object identifier: .1.3.6.1.2.1...1.1.0 in %s on line %d
bool(false)
Multiple OID

Warning: snmp2_get(): Invalid object identifier: .1.3.6.1.2.1...1.1.0 in %s on line %d
bool(false)
noSuchName checks
Single OID

Warning: snmp2_get(): Error in packet at '%s': No Such Instance currently exists at this OID in %s on line %d
bool(false)
Single OID in array

Warning: snmp2_get(): Error in packet at '%s': No Such Instance currently exists at this OID in %s on line %d
bool(false)
Multiple OID

Warning: snmp2_get(): Error in packet at '%s': No Such Instance currently exists at this OID in %s on line %d
array(1) {
  ["%s"]=>
  string(%d) "%s"
}
