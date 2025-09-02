--TEST--
Bug 73498 Incorrect DELIMITER syntax for pg_copy_to()
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$table_name = "table_06_bug73498";
$view_name = "view_06_bug73498";

$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (num int, str text, bin bytea)");
pg_query($db, "CREATE VIEW {$view_name} as SELECT * FROM {$table_name}");
pg_query($db, "INSERT INTO {$table_name} DEFAULT VALUES");

$rows = pg_copy_to($db, "(SELECT * FROM {$view_name})");

var_dump(gettype($rows));
var_dump(count($rows) > 0);

?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_06_bug73498";
$view_name = "view_06_bug73498";

$db = pg_connect($conn_str);
pg_query($db, "DROP VIEW IF EXISTS {$view_name}");
pg_query($db, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECT--
string(5) "array"
bool(true)
