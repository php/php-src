--TEST--
mysqli fetch system variables
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);

	mysqli_select_db($link, "test");

	mysqli_query($link, "SET AUTOCOMMIT=0");

	$stmt = mysqli_prepare($link, "SELECT @@autocommit");
	mysqli_bind_result($stmt, $c0); 
	mysqli_execute($stmt);

	mysqli_fetch($stmt);

	var_dump($c0);

	mysqli_close($link);
?>
--EXPECT--
int(0)
