--TEST--
not freed resultset 
--FILE--
<?php
	include "connect.inc";
	
	/************************
	 * non freed resultset 
	 ************************/
	$link = mysqli_connect("localhost", $user, $passwd);

	$result = mysqli_query($link, "SELECT CURRENT_USER()");
	mysqli_close($link);
	printf("Ok\n");

?>
--EXPECT--
Ok
