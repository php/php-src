--TEST--
Function snmp_set_string_output_format
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
if (!function_exists('snmp_set_string_output_format')) die('skip This function is only available if using NET_SNMP');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

echo "Checking error handling\n";
try {
    var_dump(snmp_set_string_output_format(123));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Checking working\n";
var_dump(snmp_set_string_output_format(SNMP_STRING_OUTPUT_GUESS));
var_dump(snmp_set_string_output_format(SNMP_STRING_OUTPUT_ASCII));
var_dump(snmp_set_string_output_format(SNMP_STRING_OUTPUT_HEX));
?>
--EXPECT--
Checking error handling
snmp_set_string_output_format(): Argument #1 ($format) must be an SNMP_STRING_OUTPUT_* constant
Checking working
bool(true)
bool(true)
bool(true)
