--TEST--
sqlmode + bind
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);

	mysqli_select_db($link, "test");

	mysqli_query($link, "SET SQL_MODE='PIPES_AS_CONCAT'");

  	mysqli_query($link,"DROP TABLE IF EXISTS mbind");
  	mysqli_query($link,"CREATE TABLE mbind (b varchar(25))");

	$stmt = mysqli_prepare($link, "INSERT INTO mbind VALUES (?||?)");
	
	mysqli_bind_param($stmt, "ss", $a, $b);

	$a = "foo";
	$b = "bar";

	mysqli_execute($stmt);

	mysqli_stmt_close($stmt);

	$stmt = mysqli_prepare($link, "SELECT * FROM mbind");
	mysqli_execute($stmt);

	mysqli_bind_result($stmt, $e);
	mysqli_fetch($stmt);

	var_dump($e);

	mysqli_close($link);
?>
--EXPECT--
string(6) "foobar"
