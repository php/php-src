--TEST--
Function snmpset
--CREDITS--
Olivier Doucet Olivier Doucet Boris Lytochkin
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

echo "Check error handing\n";
echo "No type & no value (timeout & retries instead)\n";
try {
    $z = snmpset($hostname, $communityWrite, 'SNMPv2-MIB::sysLocation.0', $timeout, $retries);
    var_dump($z);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "No value (timeout instead), retries instead of timeout\n";
$z = snmpset($hostname, $communityWrite, 'SNMPv2-MIB::sysLocation.0', 'q', $timeout, $retries);
var_dump($z);

echo "Bogus OID\n";
$z = snmpset($hostname, $communityWrite, '.1.3.6.777.888.999.444.0', 's', 'bbb', $timeout, $retries);
var_dump($z);

echo "Checking working\n";
$oid1 = 'SNMPv2-MIB::sysContact.0';
$oldvalue1 = snmpget($hostname, $communityWrite, $oid1, $timeout, $retries);
$newvalue1 = $oldvalue1 . '0';
$oid2 = 'SNMPv2-MIB::sysLocation.0';
$oldvalue2 = snmpget($hostname, $communityWrite, $oid1, $timeout, $retries);
$newvalue2 = $oldvalue2 . '0';

echo "Single OID\n";
$z = snmpset($hostname, $communityWrite, $oid1, 's', $newvalue1, $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $newvalue1));
$z = snmpset($hostname, $communityWrite, $oid1, 's', $oldvalue1, $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));

echo "Multiple OID\n";
$z = snmpset($hostname, $communityWrite, array($oid1, $oid2), array('s','s'), array($newvalue1, $newvalue2), $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $newvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $newvalue2));
$z = snmpset($hostname, $communityWrite, array($oid1, $oid2), array('s','s'), array($oldvalue1, $oldvalue2), $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $oldvalue2));

echo "Multiple OID, single type & value\n";
$z = snmpset($hostname, $communityWrite, array($oid1, $oid2), 's', $newvalue1, $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $newvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $newvalue1));
$z = snmpset($hostname, $communityWrite, array($oid1, $oid2), array('s','s'), array($oldvalue1, $oldvalue2), $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $oldvalue2));

echo "Multiple OID, single type, multiple value\n";
$z = snmpset($hostname, $communityWrite, array($oid1, $oid2), 's', array($newvalue1, $newvalue2), $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $newvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $newvalue2));
$z = snmpset($hostname, $communityWrite, array($oid1, $oid2), array('s','s'), array($oldvalue1, $oldvalue2), $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $oldvalue2));


echo "More error handing\n";
echo "Single OID, single type in array, single value\n";
try {
    $z = snmpset($hostname, $communityWrite, $oid1, array('s'), $newvalue1, $timeout, $retries);
    var_dump($z);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $oldvalue2));

echo "Single OID, single type, single value in array\n";
try {
    $z = snmpset($hostname, $communityWrite, $oid1, 's', array($newvalue1), $timeout, $retries);
    var_dump($z);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $oldvalue2));

echo "Multiple OID, 1st wrong type\n";
try {
    $z = snmpset($hostname, $communityWrite, array($oid1, $oid2), array('sw','s'), array($newvalue1, $newvalue2), $timeout, $retries);
    var_dump($z);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $oldvalue2));

echo "Multiple OID, 2nd wrong type\n";
try {
    $z = snmpset($hostname, $communityWrite, array($oid1, $oid2), array('s','sb'), array($newvalue1, $newvalue2), $timeout, $retries);
    var_dump($z);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $oldvalue2));

echo "Multiple OID, single type in array, multiple value\n";
$z = snmpset($hostname, $communityWrite, array($oid1, $oid2), array('s'), array($newvalue1, $newvalue2), $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $oldvalue2));

echo "Multiple OID & type, single value in array\n";
$z = snmpset($hostname, $communityWrite, array($oid1, $oid2), array('s', 's'), array($newvalue1), $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $oldvalue2));

echo "Multiple OID, 1st bogus, single type, multiple value\n";
$z = snmpset($hostname, $communityWrite, array($oid1 . '44.55.66.77', $oid2), 's', array($newvalue1, $newvalue2), $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $oldvalue2));

echo "Multiple OID, 2nd bogus, single type, multiple value\n";
$z = snmpset($hostname, $communityWrite, array($oid1, $oid2 . '44.55.66.77'), 's', array($newvalue1, $newvalue2), $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $oldvalue2));

echo "Multiple OID, single multiple type (1st bogus), multiple value\n";
$z = snmpset($hostname, $communityWrite, array($oid1, $oid2), array('q', 's'), array($newvalue1, $newvalue2), $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $oldvalue2));

echo "Multiple OID, single multiple type (2nd bogus), multiple value\n";
$z = snmpset($hostname, $communityWrite, array($oid1, $oid2), array('s', 'w'), array($newvalue1, $newvalue2), $timeout, $retries);
var_dump($z);
var_dump((snmpget($hostname, $communityWrite, $oid1, $timeout, $retries) === $oldvalue1));
var_dump((snmpget($hostname, $communityWrite, $oid2, $timeout, $retries) === $oldvalue2));

?>
--EXPECTF--
Check error handing
No type & no value (timeout & retries instead)
Type must be a single character
No value (timeout instead), retries instead of timeout

Warning: snmpset(): Could not add variable: OID='%s' type='q' value='%i': Bad variable type ("q") in %s on line %d
bool(false)
Bogus OID

Warning: snmpset(): Error in packet at '%s': (noSuchName) There is no such variable name in this MIB. in %s on line %d
bool(false)
Checking working
Single OID
bool(true)
bool(true)
bool(true)
bool(true)
Multiple OID
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Multiple OID, single type & value
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Multiple OID, single type, multiple value
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
More error handing
Single OID, single type in array, single value
Type must be of type string when object ID is a string
bool(true)
bool(true)
Single OID, single type, single value in array
Value must be of type string when object ID is a string
bool(true)
bool(true)
Multiple OID, 1st wrong type
Type must be a single character
bool(true)
bool(true)
Multiple OID, 2nd wrong type
Type must be a single character
bool(true)
bool(true)
Multiple OID, single type in array, multiple value

Warning: snmpset(): '%s': no type set in %s on line %d
bool(false)
bool(true)
bool(true)
Multiple OID & type, single value in array

Warning: snmpset(): '%s': no value set in %s on line %d
bool(false)
bool(true)
bool(true)
Multiple OID, 1st bogus, single type, multiple value

Warning: snmpset(): Error in packet at '%s': (noSuchName) There is no such variable name in this MIB. in %s on line %d
bool(false)
bool(true)
bool(true)
Multiple OID, 2nd bogus, single type, multiple value

Warning: snmpset(): Error in packet at '%s': (noSuchName) There is no such variable name in this MIB. in %s on line %d
bool(false)
bool(true)
bool(true)
Multiple OID, single multiple type (1st bogus), multiple value

Warning: snmpset(): Could not add variable: OID='%s' type='q' value='%s': Bad variable type ("q") in %s on line %d
bool(false)
bool(true)
bool(true)
Multiple OID, single multiple type (2nd bogus), multiple value

Warning: snmpset(): Could not add variable: OID='%s' type='w' value='%s': Bad variable type ("w") in %s on line %d
bool(false)
bool(true)
bool(true)
