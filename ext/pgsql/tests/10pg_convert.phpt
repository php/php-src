--TEST--
PostgreSQL pg_convert()
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include("pg_convert.inc");
?>
--EXPECT--
array(3) {
  ["num"]=>
  int(1234)
  ["str"]=>
  string(5) "'AAA'"
  ["bin"]=>
  string(5) "'BBB'"
}
