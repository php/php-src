--TEST--
function test: mysqli_warning_count()
--FILE--
<?php

	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	mysqli_select_db($link, "test");

	mysqli_query($link, "DROP TABLE IF EXISTS test_warnings");

	mysqli_query($link, "CREATE TABLE test_warnings (a int not null");

	mysqli_query($link, "INSERT INTO test_warnings VALUES (1),(2),(NULL)");
	$num = mysqli_warning_count($link);
	var_dump($num);

	mysqli_close($link);
?>
--EXPECT--
int(1)
