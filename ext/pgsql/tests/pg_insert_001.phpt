--TEST--
PostgreSQL pg_select() - basic test using schema
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$schema_name = 'schema_pg_insert_001';
$table_name = 'table_pg_insert_001';

$conn = pg_connect($conn_str);

pg_query($conn, "CREATE SCHEMA {$schema_name}");
pg_query($conn, "CREATE TABLE {$schema_name}.{$table_name} (id INT, id2 INT)");


pg_insert($conn, $table_name, array('id' => 1, 'id2' => 1));

pg_insert($conn, "{$schema_name}.{$table_name}", array('id' => 1, 'id2' => 2));

var_dump(pg_insert($conn, "{$schema_name}.{$table_name}", array('id' => 1, 'id2' => 2), PGSQL_DML_STRING));

var_dump(pg_select($conn, "{$schema_name}.{$table_name}", array('id' => 1)));

?>
--CLEAN--
<?php
require_once('inc/config.inc');
$schema_name = 'schema_pg_insert_001';
$table_name = 'table_pg_insert_001';

$conn = pg_connect($conn_str);
pg_query($conn, "DROP TABLE IF EXISTS {$schema_name}.{$table_name}");
pg_query($conn, "DROP SCHEMA IF EXISTS {$schema_name}");
?>
--EXPECTF--
Warning: pg_insert(): Table 'table_pg_insert_001' doesn't exists in %s on line %d
string(83) "INSERT INTO "schema_pg_insert_001"."table_pg_insert_001" ("id","id2") VALUES (1,2);"
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["id2"]=>
    string(1) "2"
  }
}
