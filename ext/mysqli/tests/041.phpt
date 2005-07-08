--TEST--
function test: mysqli_warning_count()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);
	mysqli_select_db($link, "test");

	mysqli_query($link, "DROP TABLE IF EXISTS test_warnings");
	mysqli_query($link, "DROP TABLE IF EXISTS test_warnings");

	var_dump(mysqli_warning_count($link));

	mysqli_close($link);
?>
--EXPECT--
int(1)
