--TEST--
PostgreSQL pg_select()
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include("pg_select.inc");
?>
--EXPECT--
array(1) {
  [0]=>
  array(3) {
    ["num"]=>
    string(4) "1234"
    ["str"]=>
    string(3) "AAA"
    ["bin"]=>
    string(3) "BBB"
  }
}
Ok
