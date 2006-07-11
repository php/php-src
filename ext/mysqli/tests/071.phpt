--TEST--
mysqli thread_id & kill
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd, "test");

	var_dump($mysql->ping());

	var_dump($mysql->kill($mysql->thread_id));

	var_dump($mysql->ping());

	$mysql->close();
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
