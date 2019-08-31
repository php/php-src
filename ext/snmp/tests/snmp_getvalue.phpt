--TEST--
Function snmp_getvalue
--CREDITS--
Boris Lytochkin
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

//EXPECTF format is quickprint OFF
snmp_set_quick_print(false);

echo "Get with SNMP_VALUE_LIBRARY\n";
snmp_set_valueretrieval(SNMP_VALUE_LIBRARY);
var_dump(snmpget($hostname, $community, '.1.3.6.1.2.1.1.1.0', $timeout, $retries));

echo "Get with SNMP_VALUE_PLAIN\n";
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);
var_dump(snmpget($hostname, $community, '.1.3.6.1.2.1.1.1.0', $timeout, $retries));

echo "Get with SNMP_VALUE_OBJECT\n";
snmp_set_valueretrieval(SNMP_VALUE_OBJECT);
$z = snmpget($hostname, $community, '.1.3.6.1.2.1.1.1.0', $timeout, $retries);
echo gettype($z)."\n";
var_dump($z->type);
var_dump($z->value);

echo "Get with SNMP_VALUE_OBJECT | SNMP_VALUE_PLAIN\n";
snmp_set_valueretrieval(SNMP_VALUE_OBJECT | SNMP_VALUE_PLAIN);
$z = snmpget($hostname, $community, '.1.3.6.1.2.1.1.1.0', $timeout, $retries);
echo gettype($z)."\n";
var_dump($z->type);
var_dump($z->value);

echo "Get with SNMP_VALUE_OBJECT for BITS OID\n";
snmp_set_valueretrieval(SNMP_VALUE_OBJECT);
$z = snmpget($hostname, $community, '.1.3.6.1.2.1.88.1.4.2.1.3.6.95.115.110.109.112.100.95.108.105.110.107.68.111.119.110', $timeout, $retries);
echo gettype($z)."\n";
var_dump($z->type);
var_dump($z->value);

echo "Get with SNMP_VALUE_OBJECT | SNMP_VALUE_PLAIN for BITS OID\n";
snmp_set_valueretrieval(SNMP_VALUE_OBJECT | SNMP_VALUE_PLAIN);
$z = snmpget($hostname, $community, '.1.3.6.1.2.1.88.1.4.2.1.3.6.95.115.110.109.112.100.95.108.105.110.107.68.111.119.110', $timeout, $retries);
echo gettype($z)."\n";
var_dump($z->type);
var_dump(is_numeric($z->value));
var_dump(is_string($z->value));
var_dump(bin2hex($z->value));

echo "Check parsing of different OID types\n";
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);
var_dump(count(snmp2_walk($hostname, $community, '.', $timeout, $retries)));

?>
--EXPECTF--
Get with SNMP_VALUE_LIBRARY
string(%d) "STRING: %s"
Get with SNMP_VALUE_PLAIN
string(%d) "%s"
Get with SNMP_VALUE_OBJECT
object
int(4)
string(%d) "STRING: %s"
Get with SNMP_VALUE_OBJECT | SNMP_VALUE_PLAIN
object
int(4)
string(%d) "%s"
Get with SNMP_VALUE_OBJECT for BITS OID
object
int(4)
string(25) "BITS: %d %s"
Get with SNMP_VALUE_OBJECT | SNMP_VALUE_PLAIN for BITS OID
object
int(4)
bool(false)
bool(true)
string(2) "%d"
Check parsing of different OID types
int(%d)
