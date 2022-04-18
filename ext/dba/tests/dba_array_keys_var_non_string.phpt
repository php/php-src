--TEST--
DBA check behaviour of key as an array with non string elements
--EXTENSIONS--
dba
--SKIPIF--
<?php
    require_once(__DIR__ .'/skipif.inc');
    die("info $HND handler used");
?>
--FILE--
<?php
require_once(__DIR__ .'/test.inc');
$db_file = dba_open($db_file, "n", $handler);

if ($db_file === false) {
    die('Error creating database');
}

$key = [5, 5.21];

var_dump($key);
var_dump(dba_insert($key, 'Test', $db_file));
var_dump($key);
var_dump(dba_fetch($key, $db_file));
var_dump($key);

dba_close($db_file);

?>
--CLEAN--
<?php
    require(__DIR__ .'/clean.inc');
?>
--EXPECT--
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
