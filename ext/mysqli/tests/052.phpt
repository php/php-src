--TEST--
call statement after close
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/************************
	 * statement call  after close 
	 ************************/
	$link = mysqli_connect($host, $user, $passwd);

	$stmt2 = mysqli_prepare($link, "SELECT CURRENT_USER()");

	mysqli_close($link);
	@mysqli_execute($stmt2);
	@mysqli_stmt_close($stmt2);
	printf("Ok\n");
?>
--EXPECT--
Ok
