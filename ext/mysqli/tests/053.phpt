--TEST--
not freed resultset 
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/************************
	 * non freed resultset 
	 ************************/
	$link = mysqli_connect($host, $user, $passwd);

	$result = mysqli_query($link, "SELECT CURRENT_USER()");
	mysqli_close($link);
	printf("Ok\n");

?>
--EXPECT--
Ok
