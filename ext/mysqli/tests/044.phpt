--TEST--
mysqli_get_server_version
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	$i = mysqli_get_server_version($link);

	$test = $i / $i;

	var_dump($test);

	mysqli_close($link);
?>
--EXPECT--
int(1)
