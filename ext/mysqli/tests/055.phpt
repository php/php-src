--TEST--
free nothing 
--FILE--
<?php
	include "connect.inc";
	
	/************************
	 * don't free anything 
	 ************************/
	$link = mysqli_connect("localhost", $user, $passwd);

	$result2 = mysqli_query($link, "SELECT CURRENT_USER()");
	$stmt2 = mysqli_prepare($link, "SELECT CURRENT_USER()");
	printf("Ok\n");
?>
--EXPECT--
Ok
