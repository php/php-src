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
$table_name = 'table_80_bug42783';

$dbh = @pg_connect($conn_str);
if (!$dbh) {
    die ("Could not connect to the server");
}

pg_query($dbh, "CREATE TABLE {$table_name} (id SERIAL PRIMARY KEY, time TIMESTAMP NOT NULL DEFAULT now())");

pg_insert($dbh, $table_name, array());

var_dump(pg_fetch_assoc(pg_query($dbh, "SELECT * FROM {$table_name}")));

pg_close($dbh);
?>
--CLEAN--
<?php
require_once('inc/config.inc');
$table_name = 'table_80_bug42783';

$dbh = pg_connect($conn_str);
pg_query($dbh, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECTF--
array(2) {
  ["id"]=>
  string(%d) "%d"
  ["time"]=>
  string(%d) "%s"
}
