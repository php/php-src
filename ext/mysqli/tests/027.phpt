--TEST--
function test: mysqli_stat
--FILE--
<?php
	$user = "root";
	$passwd = "";

	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	$status = mysqli_stat($link);

	$x = explode('  ', $status);

	var_dump(count($x));

	mysqli_close($link);
?>
--EXPECT--
int(10)
