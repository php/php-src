--TEST--
PostgreSQL pg_fetch_*() functions
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include("result.inc");
?>
--EXPECT--
object(stdClass)(3) {
  ["num"]=>
  string(1) "1"
  ["str"]=>
  string(3) "ABC"
  ["bin"]=>
  NULL
}
array(6) {
  [0]=>
  string(1) "1"
  ["num"]=>
  string(1) "1"
  [1]=>
  string(3) "ABC"
  ["str"]=>
  string(3) "ABC"
  [2]=>
  NULL
  ["bin"]=>
  NULL
}
array(3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(3) "ABC"
  [2]=>
  NULL
}
array(3) {
  ["num"]=>
  string(1) "2"
  ["str"]=>
  string(3) "ABC"
  ["bin"]=>
  NULL
}
bool(true)
Ok
