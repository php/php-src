--TEST--
free statement after close 
--FILE--
<?php
	include "connect.inc";
	
	/************************
	 * free statement after close 
	 ************************/
	$link = mysqli_connect("localhost", $user, $passwd);

	$stmt1 = mysqli_prepare($link, "SELECT CURRENT_USER()");
	mysqli_execute($stmt1);

	mysqli_close($link);
	@mysqli_stmt_close($stmt1);
	printf("Ok\n");
?>
--EXPECT--
Ok
