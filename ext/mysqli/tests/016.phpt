--TEST--
mysqli fetch user variable 
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	mysqli_select_db($link, "test");

	mysqli_query($link, "SET @dummy='foobar'");

	$stmt = mysqli_prepare($link, "SELECT @dummy");
	mysqli_bind_result($stmt, &$dummy);
	mysqli_execute($stmt);
	mysqli_fetch($stmt);

	var_dump($dummy);

	mysqli_stmt_close($stmt);
	mysqli_close($link);
?>
--EXPECT--
string(6) "foobar"
