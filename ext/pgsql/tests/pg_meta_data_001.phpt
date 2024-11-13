--TEST--
PostgreSQL pg_meta_data() - basic test using schema
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$schema_name = 'schema_pg_meta_data_001';
$table_name = 'table_pg_meta_data_001';

$conn = pg_connect($conn_str);

pg_query($conn, "CREATE SCHEMA {$schema_name}");

pg_query($conn, "CREATE TABLE {$schema_name}.{$table_name} (id INT, id2 INT)");

pg_query($conn, "CREATE TABLE {$table_name} (id INT, id3 INT)");


var_dump(pg_meta_data($conn, $table_name));
var_dump(pg_meta_data($conn, "{$schema_name}.{$table_name}"));
var_dump(pg_meta_data($conn, "{$schema_name}.{$table_name}", TRUE));

?>
--CLEAN--
<?php
require_once('inc/config.inc');
$schema_name = 'schema_pg_meta_data_001';
$table_name = 'table_pg_meta_data_001';

$conn = pg_connect($conn_str);
pg_query($conn, "DROP TABLE IF EXISTS {$table_name}");
pg_query($conn, "DROP TABLE IF EXISTS {$schema_name}.{$table_name}");
pg_query($conn, "DROP SCHEMA IF EXISTS {$schema_name}");
?>
--EXPECT--
array(2) {
  ["id"]=>
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
  ["id3"]=>
  array(7) {
    ["num"]=>
    int(2)
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
}
array(2) {
  ["id"]=>
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
  ["id2"]=>
  array(7) {
    ["num"]=>
    int(2)
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
}
array(2) {
  ["id"]=>
  array(11) {
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
    ["is base"]=>
    bool(true)
    ["is composite"]=>
    bool(false)
    ["is pseudo"]=>
    bool(false)
    ["description"]=>
    string(0) ""
  }
  ["id2"]=>
  array(11) {
    ["num"]=>
    int(2)
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
    ["is base"]=>
    bool(true)
    ["is composite"]=>
    bool(false)
    ["is pseudo"]=>
    bool(false)
    ["description"]=>
    string(0) ""
  }
}
