--TEST--
mysqli_stmt_affected_rows (delete)
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	mysqli_select_db($link, "test");

	mysqli_query($link, "DROP TABLE IF EXISTS test_affected");
	mysqli_query($link, "CREATE TABLE test_affected (foo int)");

	mysqli_query($link, "INSERT INTO test_affected VALUES (1),(2),(3),(4),(5)");

	$stmt = mysqli_prepare($link, "DELETE FROM test_affected WHERE foo=?");
	mysqli_bind_param($stmt, &$c1, MYSQLI_BIND_INT);

	$c1 = 2;

	mysqli_execute($stmt);
	$x = mysqli_stmt_affected_rows($stmt);

	mysqli_stmt_close($stmt);	
	var_dump($x==1);

	mysqli_close($link);
?>
--EXPECT--
bool(true)
