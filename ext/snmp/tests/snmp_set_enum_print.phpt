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

echo "Checking working\n";
var_dump(snmp_set_enum_print(0));
var_dump(snmp_set_enum_print(1));
?>
--EXPECT--
Checking working
bool(true)
bool(true)
