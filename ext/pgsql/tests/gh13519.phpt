--TEST--
GH-13519 - PGSQL_CONNECT_FORCE_NEW with persistent connections.
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include 'config.inc';

$db1 = pg_pconnect($conn_str);
$pid1 = pg_get_pid($db1);
for ($i = 0; $i < 3; $i ++) {
	$db2 = pg_pconnect($conn_str);
	var_dump($pid1 === pg_get_pid($db2));
}
for ($i = 0; $i < 3; $i ++) {
	$db2 = pg_pconnect($conn_str, PGSQL_CONNECT_FORCE_NEW);
	var_dump($pid1 === pg_get_pid($db2));
	pg_close($db2);
}
pg_close($db1);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
