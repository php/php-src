--TEST--
PostgreSQL pg_delete() - basic test using schema
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$schema_name = 'schema_pg_delete_001';
$table_name = 'table_pg_delete_001';

$conn = pg_connect($conn_str);

pg_query($conn, "CREATE SCHEMA {$schema_name}");

pg_query($conn, "CREATE TABLE {$table_name} (id INT, id2 INT)");
pg_query($conn, "CREATE TABLE {$schema_name}.{$table_name} (id INT, id2 INT)");

pg_insert($conn, $table_name, array('id' => 1, 'id2' => 1));
pg_insert($conn, $table_name, array('id' => 1, 'id2' => 2));
pg_insert($conn, $table_name, array('id' => 1, 'id2' => 2));
pg_insert($conn, $table_name, array('id' => 3, 'id2' => 3));

pg_insert($conn, "{$schema_name}.{$table_name}", array('id' => 1, 'id2' => 1));
pg_insert($conn, "{$schema_name}.{$table_name}", array('id' => 1, 'id2' => 2));
pg_insert($conn, "{$schema_name}.{$table_name}", array('id' => 2, 'id2' => 3));
pg_insert($conn, "{$schema_name}.{$table_name}", array('id' => 2, 'id2' => 3));

pg_delete($conn, $table_name, array('id' => 1, 'id2' => 0));
pg_delete($conn, $table_name, array('id' => 1, 'id2' => 2));
var_dump(pg_delete($conn, $table_name, array('id' => 1, 'id2' => 2), PGSQL_DML_STRING));

pg_delete($conn, "{$schema_name}.{$table_name}", array('id' => 2, 'id2' => 1));
pg_delete($conn, "{$schema_name}.{$table_name}", array('id' => 2, 'id2' => 3));
var_dump(pg_delete($conn, "{$schema_name}.{$table_name}", array('id' => 2, 'id2' => 3), PGSQL_DML_STRING));

var_dump(pg_fetch_all(pg_query($conn, "SELECT * FROM {$table_name}")));
var_dump(pg_fetch_all(pg_query($conn, "SELECT * FROM {$schema_name}.{$table_name}")));

/* Inexistent */
pg_delete($conn, 'bar', array('id' => 1, 'id2' => 2));
var_dump(pg_delete($conn, 'bar', array('id' => 1, 'id2' => 2), PGSQL_DML_STRING));

?>
--CLEAN--
<?php
require_once('inc/config.inc');
$schema_name = 'schema_pg_delete_001';
$table_name = 'table_pg_delete_001';

$conn = pg_connect($conn_str);

pg_query($conn, "DROP TABLE {$table_name}");
pg_query($conn, "DROP TABLE {$schema_name}.{$table_name}");
pg_query($conn, "DROP SCHEMA {$schema_name}");
?>
--EXPECTF--
string(59) "DELETE FROM "table_pg_delete_001" WHERE "id"=1 AND "id2"=2;"
string(82) "DELETE FROM "schema_pg_delete_001"."table_pg_delete_001" WHERE "id"=2 AND "id2"=3;"
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["id2"]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "3"
    ["id2"]=>
    string(1) "3"
  }
}
array(2) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["id2"]=>
    string(1) "1"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["id2"]=>
    string(1) "2"
  }
}

Warning: pg_delete(): Table 'bar' doesn't exists in %s on line %d

Warning: pg_delete(): Table 'bar' doesn't exists in %s on line %d
bool(false)
