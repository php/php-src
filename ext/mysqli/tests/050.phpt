--TEST--
non freed statement test 
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/************************
	 * non freed stamement
	 ************************/
	$link = mysqli_connect($host, $user, $passwd);

	$stmt = mysqli_prepare($link, "SELECT CURRENT_USER()");
	mysqli_execute($stmt);

	mysqli_close($link);
	printf("Ok\n");
?>
--EXPECT--
Ok
