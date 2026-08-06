--TEST--
pg_copy_to() (query) source form: parens-wrap, already-parenthesised, and statement-injection rejection
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');

$db = pg_connect($conn_str);
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_to_qsource');
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_to_qinjected');
pg_query($db, 'CREATE TABLE pg_copy_to_qsource (v text)');
pg_query($db, "INSERT INTO pg_copy_to_qsource VALUES ('a'), ('b')");

var_dump(pg_copy_to($db, '(SELECT v FROM pg_copy_to_qsource ORDER BY v)'));

var_dump(pg_copy_to($db, '((SELECT v FROM pg_copy_to_qsource ORDER BY v))'));

$evil = '(SELECT 1); DROP TABLE pg_copy_to_qsource; --';
var_dump(pg_copy_to($db, $evil));

$wrapper_close = '(SELECT 1)); DROP TABLE pg_copy_to_qsource; --';
var_dump(pg_copy_to($db, $wrapper_close));

$r = pg_query($db, "SELECT 1 FROM pg_tables WHERE tablename = 'pg_copy_to_qsource'");
var_dump(pg_num_rows($r));

$r = pg_query($db, "SELECT 1 FROM pg_tables WHERE tablename = 'pg_copy_to_qinjected'");
var_dump(pg_num_rows($r));

?>
--CLEAN--
<?php
include('inc/config.inc');
$db = pg_connect($conn_str);
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_to_qsource');
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_to_qinjected');
?>
--EXPECTF--
array(2) {
  [0]=>
  string(2) "a
"
  [1]=>
  string(2) "b
"
}
array(2) {
  [0]=>
  string(2) "a
"
  [1]=>
  string(2) "b
"
}

Warning: pg_copy_to(): Invalid query source '(SELECT 1); DROP TABLE pg_copy_to_qsource; --': must be a single balanced parenthesised expression in %s on line %d
bool(false)

Warning: pg_copy_to(): Invalid query source '(SELECT 1)); DROP TABLE pg_copy_to_qsource; --': must be a single balanced parenthesised expression in %s on line %d
bool(false)
int(1)
int(0)
