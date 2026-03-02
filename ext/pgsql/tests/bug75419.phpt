--TEST--
Bug #75419 Default link leaked via pg_close()
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
include('inc/config.inc');

$db1 = pg_connect($conn_str, PGSQL_CONNECT_FORCE_NEW);
$db2 = pg_connect($conn_str, PGSQL_CONNECT_FORCE_NEW);
pg_close($db1);
var_dump(pg_ping());
?>
--EXPECTF--
Deprecated: pg_ping(): Automatic fetching of PostgreSQL connection is deprecated in %s on line %d
bool(true)
