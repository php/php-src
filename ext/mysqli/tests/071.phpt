--TEST--
mysqli kill
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd, "test");

	$result = $mysql->query("SELECT CONNECTION_ID() AS id");

	$row = $result->fetch_assoc();

    $result->free();
	
	var_dump($mysql->ping());

	var_dump($mysql->kill($row["id"]));

	var_dump($mysql->ping());

	$mysql->close();
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
