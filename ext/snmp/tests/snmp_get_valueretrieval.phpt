--TEST--                                 
Function snmp_get_valueretrieval / snmp_set_valueretrieval
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
var_dump(snmp_get_valueretrieval('noarg'));
var_dump(snmp_set_valueretrieval('noarg'));
var_dump(snmp_set_valueretrieval(67));

echo "Checking working\n";
var_dump(snmp_get_valueretrieval());
snmp_set_valueretrieval(SNMP_VALUE_LIBRARY);
var_dump(snmp_get_valueretrieval() === SNMP_VALUE_LIBRARY);
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);
var_dump(snmp_get_valueretrieval() === SNMP_VALUE_PLAIN);
snmp_set_valueretrieval(SNMP_VALUE_OBJECT);
var_dump(snmp_get_valueretrieval() === SNMP_VALUE_OBJECT);

?>
--EXPECTF--
Checking error handling
int(%d)

Warning: snmp_set_valueretrieval() expects parameter 1 to be long, %s given in %s on line %d
bool(false)

Warning: snmp_set_valueretrieval(): Unknown SNMP value retrieval method '67' in %s on line %d
bool(false)
Checking working
int(%d)
bool(true)
bool(true)
bool(true)
