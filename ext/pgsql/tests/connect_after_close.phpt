--TEST--
Reopen connection after it was closed
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
include('inc/config.inc');

/* Run me under valgrind */
$db1 = pg_connect($conn_str);
unset($db1);
var_dump(pg_close());

$db2 = pg_connect($conn_str);
unset($db2);
var_dump(pg_close());
?>
--EXPECTF--
Deprecated: pg_close(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d
bool(true)

Deprecated: pg_close(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d
bool(true)
