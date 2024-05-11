--TEST--
Reusing connection with same connection string
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
include('inc/config.inc');

$db1 = pg_connect($conn_str);
$db2 = pg_connect($conn_str);
var_dump($db1, $db2);
?>
--EXPECT--
object(PgSql\Connection)#1 (0) {
}
object(PgSql\Connection)#1 (0) {
}
