--TEST--
function test: mysqli_get_server_info
--FILE--
<?php
	$user = "root";
	$passwd = "";

	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	$sinfo = substr(mysqli_get_server_info($link),0,1);

	var_dump($sinfo);

	mysqli_close($link);
?>
--EXPECT--
string(1) "4"
