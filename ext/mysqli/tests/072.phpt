--TEST--
mysqli warning_count, get_warnings
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php die('skip mysqli_warning class not functional yet?'); ?>
--FILE--
<?php
	include "connect.inc";

	$mysql = new mysqli($host, $user, $passwd, "test");

	$mysql->query("DROP TABLE IF EXISTS not_exists");

	var_dump($mysql->warning_count);

	$w = $mysql->get_warnings();

	var_dump($w->errno);
	var_dump($w->message);
#	var_dump($w->sqlstate);

	$mysql->close();
?>
--EXPECT--
1
1051
Unknown table 'not_exists'