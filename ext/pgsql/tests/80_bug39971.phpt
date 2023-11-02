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

$dbh = @pg_connect($conn_str);
if (!$dbh) {
    die ("Could not connect to the server");
}

pg_query($dbh, "CREATE TABLE php_test (id SERIAL, tm timestamp NOT NULL)");

$values = array('tm' => 'now()');
pg_insert($dbh, 'php_test', $values);

$ids = array('id' => 1);
pg_update($dbh, 'php_test', $values, $ids);

pg_query($dbh, "DROP TABLE php_test");
pg_close($dbh);
?>
===DONE===
--EXPECT--
===DONE===
