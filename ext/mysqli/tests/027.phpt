--TEST--
function test: mysqli_stat
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);

	$status = mysqli_stat($link);


	var_dump(strlen($status) > 0);

	mysqli_close($link);
?>
--EXPECT--
bool(true)
