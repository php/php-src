--TEST--
function test: mysqli_stat
--FILE--
<?php
	$user = "root";
	$passwd = "";

	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	$status = mysqli_stat($link);


	var_dump(strlen($status) > 0);

	mysqli_close($link);
?>
--EXPECT--
bool(true)
