--TEST--
pg_copy_to() escapes the table name argument
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');

$db = pg_connect($conn_str);
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_to_source');
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_to_injected');
pg_query($db, 'CREATE TABLE pg_copy_to_source (v text)');

$evil = "pg_copy_to_source; CREATE TABLE pg_copy_to_injected (v text); --";
var_dump(pg_copy_to($db, $evil));

$r = pg_query($db, "SELECT 1 FROM pg_tables WHERE tablename = 'pg_copy_to_injected'");
var_dump(pg_num_rows($r));

?>
--CLEAN--
<?php
include('inc/config.inc');
$db = pg_connect($conn_str);
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_to_source');
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_to_injected');
?>
--EXPECTF--
Warning: pg_copy_to(): Invalid table_name 'pg_copy_to_source; CREATE TABLE pg_copy_to_injected (v text); --': must be a plain identifier or schema.table in %s on line %d
bool(false)
int(0)
