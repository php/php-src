--TEST--
function test: mysqli_character_set_name
--FILE--
<?php
	$user = "root";
	$passwd = "";

	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	$cset = substr(mysqli_character_set_name($link),0,6);

	var_dump($cset);

	mysqli_close($link);
?>
--EXPECT--
string(6) "latin1"
