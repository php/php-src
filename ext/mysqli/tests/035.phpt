--TEST--
function test: mysqli_get_server_info
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);

	$sinfo = substr(mysqli_get_server_info($link),0,1);

	var_dump(strlen($sinfo));

	mysqli_close($link);
?>
--EXPECT--
int(1)
