--TEST--
Function snmp_get_valueretrieval / snmp_set_valueretrieval
--CREDITS--
Olivier Doucet
--EXTENSIONS--
snmp
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

echo "Checking error handling\n";
try {
    var_dump(snmp_set_valueretrieval(67));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Checking working\n";
var_dump(snmp_get_valueretrieval());
snmp_set_valueretrieval(SNMP_VALUE_LIBRARY);
var_dump(snmp_get_valueretrieval() === SNMP_VALUE_LIBRARY);
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);
var_dump(snmp_get_valueretrieval() === SNMP_VALUE_PLAIN);
snmp_set_valueretrieval(SNMP_VALUE_OBJECT);
var_dump(snmp_get_valueretrieval() === SNMP_VALUE_OBJECT);
snmp_set_valueretrieval(SNMP_VALUE_PLAIN|SNMP_VALUE_OBJECT);
var_dump(snmp_get_valueretrieval() === (SNMP_VALUE_PLAIN|SNMP_VALUE_OBJECT));
snmp_set_valueretrieval(SNMP_VALUE_LIBRARY|SNMP_VALUE_OBJECT);
var_dump(snmp_get_valueretrieval() === (SNMP_VALUE_LIBRARY|SNMP_VALUE_OBJECT));

?>
--EXPECTF--
Checking error handling
snmp_set_valueretrieval(): Argument #1 ($method) must be a bitmask of SNMP_VALUE_LIBRARY, SNMP_VALUE_PLAIN, and SNMP_VALUE_OBJECT
Checking working
int(%d)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
