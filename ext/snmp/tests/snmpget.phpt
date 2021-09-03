--TEST--
Function snmpget
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

echo "Checking working\n";
echo "Single OID, default timeout and retries\n";
var_dump(snmpget($hostname, $community, '.1.3.6.1.2.1.1.1.0'));
echo "Single OID, default retries\n";
var_dump(snmpget($hostname, $community, '.1.3.6.1.2.1.1.1.0', $timeout));
echo "Single OID\n";
var_dump(snmpget($hostname, $community, '.1.3.6.1.2.1.1.1.0', $timeout, $retries));
echo "Single OID in array\n";
var_dump(snmpget($hostname, $community, array('.1.3.6.1.2.1.1.1.0'), $timeout, $retries));
echo "Multiple OID\n";
var_dump(snmpget($hostname, $community, array('.1.3.6.1.2.1.1.1.0', '.1.3.6.1.2.1.1.3.0'), $timeout, $retries));

echo "More error handling\n";
echo "Single OID\n";
var_dump(snmpget($hostname, $community, '.1.3.6.1.2.1..1.1.0', $timeout, $retries));
echo "Single OID in array\n";
var_dump(snmpget($hostname, $community, array('.1.3.6.1.2.1...1.1.0'), $timeout, $retries));
echo "Multiple OID\n";
var_dump(snmpget($hostname, $community, array('.1.3.6.1.2.1...1.1.0', '.1.3.6.1.2.1.1.3.0'), $timeout, $retries));

echo "noSuchName checks\n";
echo "Single OID\n";
var_dump(snmpget($hostname, $community, '.1.3.6.1.2.1.1.1.110', $timeout, $retries));
echo "Single OID in array\n";
var_dump(snmpget($hostname, $community, array('.1.3.6.1.2.1.1.1.110'), $timeout, $retries));
echo "Multiple OID\n";
var_dump(snmpget($hostname, $community, array('.1.3.6.1.2.1.1.1.0', '.1.3.6.1.2.1.1.3.220'), $timeout, $retries));


?>
--EXPECTF--
Checking working
Single OID, default timeout and retries
string(%d) "%s"
Single OID, default retries
string(%d) "%s"
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

Warning: snmpget(): Invalid object identifier: .1.3.6.1.2.1..1.1.0 in %s on line %d
bool(false)
Single OID in array

Warning: snmpget(): Invalid object identifier: .1.3.6.1.2.1...1.1.0 in %s on line %d
bool(false)
Multiple OID

Warning: snmpget(): Invalid object identifier: .1.3.6.1.2.1...1.1.0 in %s on line %d
bool(false)
noSuchName checks
Single OID

Warning: snmpget(): Error in packet at '%s': (noSuchName) There is no such variable name in this MIB. in %s on line %d
bool(false)
Single OID in array

Warning: snmpget(): Error in packet at '%s': (noSuchName) There is no such variable name in this MIB. in %s on line %d
bool(false)
Multiple OID

Warning: snmpget(): Error in packet at '%s': (noSuchName) There is no such variable name in this MIB. in %s on line %d
array(1) {
  ["%s"]=>
  string(%d) "%s"
}
