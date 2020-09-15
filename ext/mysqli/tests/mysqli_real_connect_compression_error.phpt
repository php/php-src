--TEST--
Bug #80107 mysqli_query() fails for ~16 MB long query when compression is enabled
--XFAIL--
The second INSERT query fails with MySQL server has gone away
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

// Insert with compression disabled:

$mysqli = mysqli_init();
$result = my_mysqli_real_connect($mysqli, $host, $user, $passwd, $db, $port, $socket);
$mysqli->query("DROP TABLE IF EXISTS test");
$mysqli->query("CREATE TABLE test (`blob` LONGBLOB NOT NULL)");

$data = str_repeat("x", 16777174);
$mysqli->query("INSERT INTO $db.test(`blob`) VALUE ('$data')");

var_dump(mysqli_error_list($mysqli));
$mysqli->close();

// Insert with compression enabled:

$mysqli = mysqli_init();
$result = my_mysqli_real_connect($mysqli, $host, $user, $passwd, $db, $port, $socket, MYSQLI_CLIENT_COMPRESS);

$data = str_repeat("x", 16777174);
$mysqli->query("INSERT INTO $db.test(`blob`) VALUE ('$data')");

var_dump(mysqli_error_list($mysqli));
$mysqli->close();

?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECT--
array(0) {
}
array(0) {
}
