--TEST--
Bug #75419 Default link leaked via pg_close()
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include('config.inc');

$db1 = pg_connect($conn_str, PGSQL_CONNECT_FORCE_NEW);
$db2 = pg_connect($conn_str, PGSQL_CONNECT_FORCE_NEW);
pg_close($db1);
try {
    pg_ping();
} catch (Error $e) {
    echo $e->getMessage() . PHP_EOL;
}
?>
--EXPECTF--
pg_ping() expects exactly 1 argument, 0 given
