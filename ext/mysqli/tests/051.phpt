--TEST--
free statement after close
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	/************************
	 * free statement after close
	 ************************/
	$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

	$stmt1 = mysqli_prepare($link, "SELECT CURRENT_USER()");
	mysqli_execute($stmt1);

	mysqli_close($link);
	@mysqli_stmt_close($stmt1);
	printf("Ok\n");
?>
--EXPECT--
Ok
