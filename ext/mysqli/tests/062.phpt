--TEST--
resultset constructor 
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd);

	$mysql->real_query("SELECT 'foo' FROM DUAL");

	$myresult = new mysqli_result($mysql);

	$row = $myresult->fetch_row();
	$myresult->close();
	$mysql->close();

	var_dump($row);
?>
--EXPECT--
array(1) {
  [0]=>
  string(3) "foo"
}
