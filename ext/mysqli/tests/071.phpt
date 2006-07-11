--TEST--
mysqli thread_id & kill
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd, "test");

	var_dump($mysql->ping());

	var_dump($mysql->kill($mysql->thread_id));

	var_dump($mysql->ping());

	$mysql->close();

	$mysql = new mysqli($host, $user, $passwd, "test");

	var_dump(mysqli_ping($mysql));

	var_dump(mysqli_kill($mysql, mysqli_thread_id($mysql)));

	var_dump(mysqli_ping($mysql));

	$mysql->close();
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
