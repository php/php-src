--TEST--
PostgreSQL pg_metadata()
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
error_reporting(E_ALL);

include 'config.inc';

$db = pg_connect($conn_str);

$meta = pg_meta_data($db, $table_name);

var_dump($meta);
?>
--EXPECT--
array(3) {
  ["num"]=>
  array(6) {
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
    ["array dims"]=>
    int(0)
  }
  ["str"]=>
  array(6) {
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
    ["array dims"]=>
    int(0)
  }
  ["bin"]=>
  array(6) {
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
    ["array dims"]=>
    int(0)
  }
}
