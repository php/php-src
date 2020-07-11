--TEST--
Function snmp_set_oid_output_format
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
if (!function_exists('snmp_set_oid_output_format')) die('skip This function is only available if using NET_SNMP');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

echo "Checking error handling\n";
var_dump(snmp_set_oid_output_format(123));

echo "Checking working\n";
var_dump(snmp_set_oid_output_format(SNMP_OID_OUTPUT_FULL));
var_dump(snmp_set_oid_output_format(SNMP_OID_OUTPUT_NUMERIC));
?>
--EXPECTF--
Checking error handling

Warning: snmp_set_oid_output_format(): Unknown SNMP output print format '123' in %s on line %d
bool(false)
Checking working
bool(true)
bool(true)
