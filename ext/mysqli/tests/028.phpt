--TEST--
function test: mysqli_character_set_name
--FILE--
<?php
	$user = "root";
	$passwd = "";

	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	$cset = mysqli_character_set_name($link);

	var_dump($cset);

	mysqli_close($link);
?>
--EXPECT--
string(6) "latin1"
