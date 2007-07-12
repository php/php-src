--TEST--
function test: mysqli_affected_rows
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket);

	mysqli_select_db($link, $db);

	mysqli_query($link, "drop table if exists general_test");
	mysqli_query($link, "create table general_test (a int)");
	mysqli_query($link, "insert into general_test values (1),(2),(3)");

	$afc = mysqli_affected_rows($link);

	var_dump($afc);

	mysqli_close($link);
	print "done!";
?>
--EXPECT--
int(3)
done!