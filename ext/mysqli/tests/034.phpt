--TEST--
function test: mysqli_get_proto_info
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);

	$pinfo = mysqli_get_proto_info($link);

	var_dump($pinfo);

	mysqli_close($link);
?>
--EXPECT--
int(10)
