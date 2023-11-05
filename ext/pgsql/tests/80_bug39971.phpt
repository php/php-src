--TEST--
Bug #39971 (8.0+) (pg_insert/pg_update do not allow now() to be used for timestamp fields)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
require_once('inc/skipif.inc');
?>
--FILE--
<?php

require_once('inc/config.inc');
$table_name = 'table_80_bug39971';

$dbh = pg_connect($conn_str);

pg_query($dbh, "CREATE TABLE {$table_name} (id SERIAL, tm timestamp NOT NULL)");

$values = array('tm' => 'now()');
pg_insert($dbh, $table_name, $values);

$ids = array('id' => 1);
pg_update($dbh, $table_name, $values, $ids);

pg_close($dbh);
?>
===DONE===
--CLEAN--
<?php
require_once('inc/config.inc');
$table_name = 'table_80_bug39971';

$dbh = pg_connect($conn_str);
pg_query($dbh, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECT--
===DONE===
