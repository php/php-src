--TEST--
PostgreSQL pg_update() - basic test using schema
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$schema_name = 'schema_pg_update_001';
$table_name = 'table_pg_update_001';

$conn = pg_connect($conn_str);

pg_query($conn, "CREATE SCHEMA {$schema_name}");

pg_query($conn, "CREATE TABLE {$table_name} (id INT, id2 INT)");
pg_query($conn, "CREATE TABLE {$schema_name}.{$table_name} (id INT, id2 INT)");


pg_insert($conn, $table_name, array('id' => 1, 'id2' => 1));
pg_insert($conn, "{$schema_name}.{$table_name}", array('id' => 1, 'id2' => 2));

pg_update($conn, $table_name, array('id' => 10), array('id' => 1));
var_dump(pg_update($conn, $table_name, array('id' => 10), array('id' => 1), PGSQL_DML_STRING));

pg_update($conn, "{$schema_name}.{$table_name}", array('id' => 100), array('id2' => 2));
var_dump(pg_update($conn, "{$schema_name}.{$table_name}", array('id' => 100), array('id2' => 2), PGSQL_DML_STRING));

$rs = pg_query($conn, "SELECT * FROM {$table_name} UNION SELECT * FROM {$schema_name}.{$table_name} ORDER BY id");
while ($row = pg_fetch_assoc($rs)) {
    var_dump($row);
}

?>
--CLEAN--
<?php
require_once('inc/config.inc');
$schema_name = 'schema_pg_update_001';
$table_name = 'table_pg_update_001';

$conn = pg_connect($conn_str);

pg_query($conn, "DROP TABLE {$table_name}");
pg_query($conn, "DROP TABLE {$schema_name}.{$table_name}");
pg_query($conn, "DROP SCHEMA {$schema_name}");
?>
--EXPECT--
string(54) "UPDATE "table_pg_update_001" SET "id"=10 WHERE "id"=1;"
string(79) "UPDATE "schema_pg_update_001"."table_pg_update_001" SET "id"=100 WHERE "id2"=2;"
array(2) {
  ["id"]=>
  string(2) "10"
  ["id2"]=>
  string(1) "1"
}
array(2) {
  ["id"]=>
  string(3) "100"
  ["id2"]=>
  string(1) "2"
}
