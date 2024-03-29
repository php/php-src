--TEST--
PostgreSQL pg_metadata()
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
error_reporting(E_ALL);

include 'inc/config.inc';
$table_name = "table_11pg_meta_data";

$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (num int, str text, bin bytea)");

$meta = pg_meta_data($db, $table_name);

var_dump($meta);
?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_11pg_meta_data";

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECT--
array(3) {
  ["num"]=>
  array(7) {
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
    ["is enum"]=>
    bool(false)
  }
  ["str"]=>
  array(7) {
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
    ["is enum"]=>
    bool(false)
  }
  ["bin"]=>
  array(7) {
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
    ["is enum"]=>
    bool(false)
  }
}
