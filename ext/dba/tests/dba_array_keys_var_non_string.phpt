--TEST--
DBA check behaviour of key as an array with non string elements
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip_any();
?>
--FILE--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$name = 'array_keys_non_string.db';

$db = get_any_db($name);

$key = [5, 5.21];

var_dump($key);
var_dump(dba_insert($key, 'Test', $db));
var_dump($key);
var_dump(dba_fetch($key, $db));
var_dump($key);

dba_close($db);

?>
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'array_keys_non_string.db';
cleanup_standard_db($db_name);
?>
--EXPECTF--
Using handler: "%s"
array(2) {
  [0]=>
  int(5)
  [1]=>
  float(5.21)
}
bool(true)
array(2) {
  [0]=>
  int(5)
  [1]=>
  float(5.21)
}
string(4) "Test"
array(2) {
  [0]=>
  int(5)
  [1]=>
  float(5.21)
}
