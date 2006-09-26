--TEST--
mysqli warning_count, get_warnings
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd, "test");

	$mysql->query("DROP TABLE IF EXISTS not_exists");

	var_dump($mysql->warning_count);

	$w = $mysql->get_warnings();

	var_dump($w->errno);
	var_dump($w->message);
	var_dump($w->sqlstate);

	$mysql->close();
?>
--EXPECT--
int(1)
int(1051)
string(26) "Unknown table 'not_exists'"
string(5) "HY000"
