--TEST--
function test: mysqli_get_host_info
--FILE--
<?php
	$user = "root";
	$passwd = "";

	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	$hinfo = mysqli_get_host_info($link);

	var_dump($hinfo);

	mysqli_close($link);
?>
--EXPECT--
string(25) "Localhost via UNIX socket"
