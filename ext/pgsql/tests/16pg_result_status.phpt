--TEST--
PostgreSQL pg_result_status()
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
include 'inc/config.inc';
$table_name = "table_16pg_result_status";

$db = pg_connect($conn_str);
pg_query($db, "create table {$table_name} (num int, str text, bin bytea)");

$sql = "SELECT * FROM ".$table_name." WHERE num = -2";
$result = pg_query($db, "BEGIN;END");

echo pg_result_status($result)."\n";
echo pg_result_status($result, PGSQL_STATUS_STRING)."\n";
?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_16pg_result_status";

$db = pg_connect($conn_str);
pg_query($db, "drop table {$table_name}");
?>
--EXPECT--
1
COMMIT
