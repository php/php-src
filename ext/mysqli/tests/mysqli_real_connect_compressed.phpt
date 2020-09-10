--TEST--
mysqli_real_connect() with compression
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--INI--
mysqli.allow_local_infile=1
--FILE--
<?php

include("connect.inc");

$mysqli = mysqli_init();
$result = my_mysqli_real_connect($mysqli, $host, $user, $passwd, "", $port, $socket, MYSQLI_CLIENT_COMPRESS);

$mysqli->query("DROP TABLE IF EXISTS test");
$create = "CREATE TABLE test (blob BLOB NOT NULL DEFAULT '')";

$data = str_repeat("x", 16 * 1024 * 1024);
$mysqli->query("INSERT INTO test(`blob`) VALUE ('$data')");

var_dump(mysqli_error_list($mysqli));
$mysqli->close();

?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECT--
