--TEST--
PostgreSQL drop db
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
// drop test table

include('inc/config.inc');
$table_name = "table_9999dropdb";
$table_name_92 = "table_9999dropdb_92";
$view_name = "view_9999dropdb";

$db = pg_connect($conn_str);
pg_query($db, "create table {$table_name} (num int, str text, bin bytea)");
pg_query($db, "create table {$table_name_92} (textary text[],  jsn json)");
pg_query($db, "create view {$view_name} as select * from {$table_name}");

pg_query($db, "DROP VIEW {$view_name}");
pg_query($db, "DROP TABLE ".$table_name);
@pg_query($db, "DROP TABLE ".$table_name_92);

echo "OK";

?>
--EXPECT--
OK
