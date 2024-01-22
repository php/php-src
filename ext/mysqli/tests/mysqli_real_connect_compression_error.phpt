--TEST--
Bug #80107 mysqli_query() fails for ~16 MB long query when compression is enabled
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'connect.inc';
$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
if (!$link) {
    die(sprintf("skip Can't connect to MySQL Server - [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));
}
$result = $link->query("SHOW VARIABLES LIKE 'max_allowed_packet'");
if ($result->fetch_assoc()['Value'] < 0xffffff) {
    die('skip max_allowed_packet is less than 0xffffff');
}
?>
--FILE--
<?php

require_once 'connect.inc';

$data_size = 16777174;

// Insert with compression disabled:

$mysqli = mysqli_init();
$result = my_mysqli_real_connect($mysqli, $host, $user, $passwd, $db, $port, $socket);
$mysqli->query("DROP TABLE IF EXISTS test");
$mysqli->query("CREATE TABLE test (`blob` LONGBLOB NOT NULL) ENGINE=MyISAM");

$data = str_repeat("x", $data_size);
$mysqli->query("INSERT INTO $db.test(`blob`) VALUE ('$data')");

var_dump(mysqli_error_list($mysqli));
$mysqli->close();

// Insert with compression enabled:

$mysqli = mysqli_init();
$result = my_mysqli_real_connect($mysqli, $host, $user, $passwd, $db, $port, $socket, MYSQLI_CLIENT_COMPRESS);

$data = str_repeat("x", $data_size);
$mysqli->query("INSERT INTO $db.test(`blob`) VALUE ('$data')");

var_dump(mysqli_error_list($mysqli));
$mysqli->close();

?>
--CLEAN--
<?php
require_once 'clean_table.inc';
?>
--EXPECT--
array(0) {
}
array(0) {
}
