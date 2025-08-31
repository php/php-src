--TEST--
Function snmp_set_oid_output_format
--CREDITS--
Olivier Doucet
--EXTENSIONS--
snmp
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
if (!function_exists('snmp_set_oid_output_format')) die('skip This function is only available if using NET_SNMP');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

echo "Checking error handling\n";
try {
    var_dump(snmp_set_oid_output_format(123));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Checking working\n";
var_dump(snmp_set_oid_output_format(SNMP_OID_OUTPUT_FULL));
var_dump(snmp_set_oid_output_format(SNMP_OID_OUTPUT_NUMERIC));
?>
--EXPECT--
Checking error handling
snmp_set_oid_output_format(): Argument #1 ($format) must be an SNMP_OID_OUTPUT_* constant
Checking working
bool(true)
bool(true)
