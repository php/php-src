--TEST--
mysqli ping
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd, "test");
	var_dump($mysql->ping());
	$mysql->close();
?>
--EXPECT--
bool(true)
