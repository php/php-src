--TEST--
pg_copy_from() escapes the table name argument
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');

$db = pg_connect($conn_str);
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_from_target');
pg_query($db, 'CREATE TABLE pg_copy_from_target (v text)');
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_from_other');
pg_query($db, 'CREATE TABLE pg_copy_from_other (v text)');

$evil = "pg_copy_from_other FROM STDIN --";
var_dump(pg_copy_from($db, $evil, ["redirected\n"]));

$rows = pg_fetch_all(pg_query($db, 'SELECT v FROM pg_copy_from_other')) ?: [];
var_dump($rows);

?>
--CLEAN--
<?php
include('inc/config.inc');
$db = pg_connect($conn_str);
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_from_target');
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_from_other');
?>
--EXPECTF--
Warning: pg_copy_from(): Invalid table_name 'pg_copy_from_other FROM STDIN --': must be a plain identifier or schema.table in %s on line %d
bool(false)
array(0) {
}
