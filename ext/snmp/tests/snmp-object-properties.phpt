--TEST--
OO API: SNMP object properties
--CREDITS--
Boris Lytochkin
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

//EXPECTF format is quickprint OFF
snmp_set_enum_print(false);
snmp_set_quick_print(false);
snmp_set_valueretrieval(SNMP_VALUE_PLAIN);
snmp_set_oid_output_format(SNMP_OID_OUTPUT_FULL);

echo "Check working\n";

$session = new SNMP(SNMP::VERSION_1, $hostname, $community, $timeout, $retries);
var_dump($session);

$session->max_oids = 40;
$session->enum_print = TRUE;
$session->quick_print = TRUE;
$session->valueretrieval = SNMP_VALUE_LIBRARY;
$session->oid_output_format = SNMP_OID_OUTPUT_NUMERIC;
$session->oid_increasing_check = FALSE;

var_dump($session);

$session->max_oids = "40";
$session->enum_print = "1";
$session->quick_print = "1";
$session->valueretrieval = "1";
$session->oid_output_format = "3";
$session->oid_increasing_check = "45";

var_dump($session);

var_dump(property_exists($session, "enum_print"));
var_dump(isset($session->enum_print));
var_dump(empty($session->enum_print));

$param=123;
$session->$param = "param_value";
var_dump($session);
var_dump($session->$param);
var_dump(property_exists($session, $param));

echo "Error handling\n";
$param = 'there is no such parameter';
var_dump($session->$param);
var_dump(property_exists($session, $param));

try {
    $session->valueretrieval = 67;
    var_dump($session->valueretrieval);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    $session->oid_output_format = 78;
    var_dump($session->oid_output_format);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    $session->info = array("blah" => 2);
    var_dump($session->info);
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$session->max_oids = NULL;
var_dump($session->max_oids);
?>
--EXPECTF--
Check working
object(SNMP)#%d (%d) {
  ["info"]=>
  array(3) {
    ["hostname"]=>
    string(%d) "%s"
    ["timeout"]=>
    int(%i)
    ["retries"]=>
    int(%d)
  }
  ["max_oids"]=>
  NULL
  ["valueretrieval"]=>
  int(1)
  ["quick_print"]=>
  bool(false)
  ["enum_print"]=>
  bool(false)
  ["oid_output_format"]=>
  int(3)
  ["oid_increasing_check"]=>
  bool(true)
  ["exceptions_enabled"]=>
  int(0)
}
object(SNMP)#%d (%d) {
  ["info"]=>
  array(3) {
    ["hostname"]=>
    string(%d) "%s"
    ["timeout"]=>
    int(%i)
    ["retries"]=>
    int(%d)
  }
  ["max_oids"]=>
  int(40)
  ["valueretrieval"]=>
  int(0)
  ["quick_print"]=>
  bool(true)
  ["enum_print"]=>
  bool(true)
  ["oid_output_format"]=>
  int(4)
  ["oid_increasing_check"]=>
  bool(false)
  ["exceptions_enabled"]=>
  int(0)
}
object(SNMP)#%d (%d) {
  ["info"]=>
  array(3) {
    ["hostname"]=>
    string(%d) "%s"
    ["timeout"]=>
    int(%i)
    ["retries"]=>
    int(%d)
  }
  ["max_oids"]=>
  int(40)
  ["valueretrieval"]=>
  int(1)
  ["quick_print"]=>
  bool(true)
  ["enum_print"]=>
  bool(true)
  ["oid_output_format"]=>
  int(3)
  ["oid_increasing_check"]=>
  bool(true)
  ["exceptions_enabled"]=>
  int(0)
}
bool(true)
bool(true)
bool(false)
object(SNMP)#%d (%d) {
  ["info"]=>
  array(3) {
    ["hostname"]=>
    string(%d) "%s"
    ["timeout"]=>
    int(%i)
    ["retries"]=>
    int(%d)
  }
  ["max_oids"]=>
  int(40)
  ["valueretrieval"]=>
  int(1)
  ["quick_print"]=>
  bool(true)
  ["enum_print"]=>
  bool(true)
  ["oid_output_format"]=>
  int(3)
  ["oid_increasing_check"]=>
  bool(true)
  ["exceptions_enabled"]=>
  int(0)
  ["123"]=>
  string(11) "param_value"
}
string(11) "param_value"
bool(true)
Error handling

Warning: Undefined property: SNMP::$there is no such parameter in %s on line %d
NULL
bool(false)
SNMP retrieval method must be a bitmask of SNMP_VALUE_LIBRARY, SNMP_VALUE_PLAIN, and SNMP_VALUE_OBJECT
SNMP output print format must be an SNMP_OID_OUTPUT_* constant
SNMP::$info property is read-only
NULL
