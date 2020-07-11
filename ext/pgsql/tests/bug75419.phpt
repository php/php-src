--TEST--
Bug #75419 Default link leaked via pg_close()
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include('config.inc');

$db1 = pg_connect($conn_str, PGSQL_CONNECT_FORCE_NEW);
$db2 = pg_connect($conn_str, PGSQL_CONNECT_FORCE_NEW);
pg_close($db1);
var_dump(pg_ping());
--EXPECT--
bool(true)
