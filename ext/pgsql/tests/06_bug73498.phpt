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
pg_query($db, "create table {$table_name} (num int, str text, bin bytea)");
pg_query($db, "create view {$view_name} as select * from {$table_name}");
pg_query($db, "insert into {$table_name} default values");

$rows = pg_copy_to($db, "(select * from {$view_name})");

var_dump(gettype($rows));
var_dump(count($rows) > 0);

?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_06_bug73498";
$view_name = "view_06_bug73498";

$db = pg_connect($conn_str);
pg_query($db, "drop view {$view_name}");
pg_query($db, "drop table {$table_name}");
?>
--EXPECT--
string(5) "array"
bool(true)
