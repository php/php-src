--TEST--
function test: mysqli_errno
--FILE--
<?php
	$user = "root";
	$passwd = "";

	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);
	$errno = mysqli_errno($link);
	var_dump($errno);

	mysqli_select_db($link, "test");

	mysqli_query($link, "select * from non_exisiting_table");
	$errno = mysqli_errno($link);	

	var_dump($errno);

	mysqli_close($link);
?>
--EXPECT--
int(0)
int(1146)
