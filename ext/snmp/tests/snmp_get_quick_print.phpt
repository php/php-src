--TEST--
Function snmp_get_quick_print / snmp_set_quick_print
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
require_once(dirname(__FILE__).'/snmp_include.inc');

echo "Checking error handling\n";
var_dump(snmp_get_quick_print('noarg'));
var_dump(snmp_set_quick_print('noarg'));
var_dump(snmp_set_quick_print());

echo "Checking working\n";
var_dump(snmp_get_quick_print());
snmp_set_quick_print(false);
var_dump(snmp_get_quick_print());
snmp_set_quick_print(true);
var_dump(snmp_get_quick_print());

?>
--EXPECTF--
Checking error handling

Warning: snmp_get_quick_print() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: snmp_set_quick_print() expects parameter 1 to be integer, %s given in %s on line %d
bool(false)

Warning: snmp_set_quick_print() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)
Checking working
bool(%s)
bool(false)
bool(true)
