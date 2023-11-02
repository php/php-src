--TEST--
Bug #42783 (pg_insert() does not support an empty value array)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
require_once('inc/skipif.inc');
?>
--FILE--
<?php

require_once('inc/config.inc');

$dbh = @pg_connect($conn_str);
if (!$dbh) {
    die ("Could not connect to the server");
}

pg_query($dbh, "CREATE TABLE php_test (id SERIAL PRIMARY KEY, time TIMESTAMP NOT NULL DEFAULT now())");

pg_insert($dbh, 'php_test', array());

var_dump(pg_fetch_assoc(pg_query($dbh, "SELECT * FROM php_test")));

pg_query($dbh, "DROP TABLE php_test");
pg_close($dbh);
?>
--EXPECTF--
array(2) {
  ["id"]=>
  string(%d) "%d"
  ["time"]=>
  string(%d) "%s"
}
