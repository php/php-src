--TEST--
snmpget() modifies object_id array source
--EXTENSIONS--
snmp
zend_test
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
if (getenv('SKIP_ASAN')) die('skip Causes timeout on asan');
?>
--FILE--
<?php
require_once(__DIR__.'/snmp_include.inc');

$bad_object_ids = array (
077 => 077, -066 => -066, -0345 => -0345, 0 => 0
);
var_dump($bad_object_ids);
var_dump(snmpget($hostname, "", $bad_object_ids) === false);
// The array should remain unmodified
var_dump($bad_object_ids);
try {
	snmpget($hostname, "", [0 => new stdClass()]);
} catch (Throwable $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	snmp2_set($hostname, $communityWrite, $bad_object_ids, [new stdClass()], [null]);
} catch (Throwable $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	snmp2_set($hostname, $communityWrite, $bad_object_ids, ["toolongtype"], [null]);
} catch (Throwable $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	snmp2_set($hostname, $communityWrite, $bad_object_ids, [zend_return_non_interned_string("onetoomuch")], [null]);
} catch (Throwable $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
array(4) {
  [63]=>
  int(63)
  [-54]=>
  int(-54)
  [-229]=>
  int(-229)
  [0]=>
  int(0)
}

Warning: snmpget(): Invalid object identifier: -54 in %s on line %d
bool(true)
array(4) {
  [63]=>
  int(63)
  [-54]=>
  int(-54)
  [-229]=>
  int(-229)
  [0]=>
  int(0)
}
Object of class stdClass could not be converted to string
Object of class stdClass could not be converted to string
snmp2_set(): Argument #4 ($type) must be a single character
snmp2_set(): Argument #4 ($type) must be a single character
