--TEST--
Bug #77597: mysqli_fetch_field hangs scripts
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php

require_once("connect.inc");
$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket);

$mysqli->query('DROP TABLE IF EXISTS a');
$mysqli->query('CREATE TABLE a (b int)');
$mysqli->query('INSERT INTO a VALUES (1),(2),(3),(4),(5),(6),(7),(8),(9)');

$mysqli->real_query("SELECT * FROM a");

$result = $mysqli->store_result(MYSQLI_STORE_RESULT_COPY_DATA);

$field = $result->fetch_field();
var_dump($field->name);

?>
--EXPECT--
string(1) "b"
