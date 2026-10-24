--TEST--
pg_copy_to() / pg_copy_from() default null marker round-trip
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');

$db = pg_connect($conn_str);
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_default_null');
pg_query($db, 'CREATE TABLE pg_copy_default_null (id int, v text)');
pg_query($db, "INSERT INTO pg_copy_default_null VALUES (1, 'hello'), (2, NULL)");

$rows = pg_copy_to($db, 'pg_copy_default_null');
var_dump($rows);

pg_query($db, 'DELETE FROM pg_copy_default_null');
var_dump(pg_copy_from($db, 'pg_copy_default_null', $rows));
var_dump(pg_fetch_all(pg_query($db, 'SELECT v FROM pg_copy_default_null ORDER BY id')));

?>
--CLEAN--
<?php
include('inc/config.inc');
$db = pg_connect($conn_str);
pg_query($db, 'DROP TABLE IF EXISTS pg_copy_default_null');
?>
--EXPECT--
array(2) {
  [0]=>
  string(8) "1	hello
"
  [1]=>
  string(5) "2	\N
"
}
bool(true)
array(2) {
  [0]=>
  array(1) {
    ["v"]=>
    string(5) "hello"
  }
  [1]=>
  array(1) {
    ["v"]=>
    NULL
  }
}
