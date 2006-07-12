--TEST--
mysqli_autocommit() tests
--SKIPIF--
<?php 
	require_once('skipif.inc'); 
?>
--FILE--
<?php

include "connect.inc";

$mysqli = new mysqli($host, $user, $passwd, "test");

var_dump($mysqli->autocommit(false));
$result = $mysqli->query("SELECT @@autocommit");
var_dump($result->fetch_row());

var_dump($mysqli->autocommit(true));
$result = $mysqli->query("SELECT @@autocommit");
var_dump($result->fetch_row());

?>
--EXPECT--
bool(true)
array(1) {
  [0]=>
  string(1) "0"
}
bool(true)
array(1) {
  [0]=>
  string(1) "1"
}
