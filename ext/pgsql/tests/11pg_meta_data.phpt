--TEST--
PostgreSQL pg_metadata()
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include("pg_meta_data.inc");
?>
--EXPECT--
array(3) {
  ["num"]=>
  array(5) {
    ["num"]=>
    int(1)
    ["type"]=>
    string(4) "int4"
    ["len"]=>
    int(4)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
  }
  ["str"]=>
  array(5) {
    ["num"]=>
    int(2)
    ["type"]=>
    string(4) "text"
    ["len"]=>
    int(-1)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
  }
  ["bin"]=>
  array(5) {
    ["num"]=>
    int(3)
    ["type"]=>
    string(5) "bytea"
    ["len"]=>
    int(-1)
    ["not null"]=>
    bool(false)
    ["has default"]=>
    bool(false)
  }
}
