--TEST--
Function snmp_get_quick_print / snmp_set_quick_print
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

echo "Checking working\n";
var_dump(snmp_get_quick_print());
snmp_set_quick_print(false);
var_dump(snmp_get_quick_print());
snmp_set_quick_print(true);
var_dump(snmp_get_quick_print());

?>
--EXPECTF--
Checking working
bool(%s)
bool(false)
bool(true)
