--TEST--
free resultset after close 
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/************************
	 * free resultset after close 
	 ************************/
	$link = mysqli_connect($host, $user, $passwd);

	$result1 = mysqli_query($link, "SELECT CURRENT_USER()");
	mysqli_close($link);
	mysqli_free_result($result1);
	printf("Ok\n");
?>
--EXPECT--
Ok
