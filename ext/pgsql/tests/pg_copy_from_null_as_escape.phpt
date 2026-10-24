--TEST--
pg_copy_from() escapes the null_as argument
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');

$db = pg_connect($conn_str);
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_null_as_target');
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_null_as_injected');
pg_query($db, 'CREATE TABLE pg_copy_null_as_target (v text)');

$evil = "X'; CREATE TABLE pg_copy_null_as_injected (v text); --";
var_dump(pg_copy_from($db, 'pg_copy_null_as_target', ["row\n"], "\t", $evil));

$r = pg_query($db, "SELECT 1 FROM pg_tables WHERE tablename = 'pg_copy_null_as_injected'");
var_dump(pg_num_rows($r));

$r = pg_query($db, 'SELECT v FROM pg_copy_null_as_target ORDER BY v');
var_dump(pg_fetch_all($r));

?>
--CLEAN--
<?php
include('inc/config.inc');
$db = pg_connect($conn_str);
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_null_as_target');
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_null_as_injected');
?>
--EXPECT--
bool(true)
int(0)
array(1) {
  [0]=>
  array(1) {
    ["v"]=>
    string(3) "row"
  }
}
