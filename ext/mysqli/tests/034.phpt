--TEST--
function test: mysqli_get_proto_info
--FILE--
<?php
	$user = "root";
	$passwd = "";

	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	$pinfo = mysqli_get_proto_info($link);

	var_dump($pinfo);

	mysqli_close($link);
?>
--EXPECT--
int(10)
