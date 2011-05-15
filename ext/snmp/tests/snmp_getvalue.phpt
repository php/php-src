--TEST--                                 
Function snmp_getvalue
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
string(%d) "%s"
Check parsing of different OID types
int(%d)
