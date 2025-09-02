--TEST--
PostgreSQL copy functions
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$table_name = "table_06copy";

$db = pg_connect($conn_str);
pg_query($db, "CREATE TABLE {$table_name} (num int, str text, bin bytea)");

$rows = pg_copy_to($db, $table_name);

pg_query($db, "DELETE FROM $table_name");

pg_copy_from($db, $table_name, $rows);

echo "OK";

?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_06copy";

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECT--
OK
