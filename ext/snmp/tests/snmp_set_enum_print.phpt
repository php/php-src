--TEST--
Function snmp_set_enum_print
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
if (!function_exists('snmp_set_enum_print')) die('skip This function is only available if using NET_SNMP');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

echo "Checking error handling\n";
var_dump(snmp_set_enum_print());

echo "Checking working\n";
var_dump(snmp_set_enum_print(0));
var_dump(snmp_set_enum_print(1));
?>
--EXPECTF--
Checking error handling

Warning: snmp_set_enum_print() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)
Checking working
bool(true)
bool(true)
