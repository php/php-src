--TEST--
resultset constructor 
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd);

	$stmt = new mysqli_stmt($mysql, "SELECT 'foo' FROM DUAL");
	$stmt->execute();
	$stmt->bind_result($foo);
	$stmt->fetch();
	$stmt->close();
	$mysql->close();

	var_dump($foo);
?>
--EXPECT--
string(3) "foo"
