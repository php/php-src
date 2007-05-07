--TEST--
Bug #36745 (LOAD DATA LOCAL INFILE doesn't return correct error message)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include ("connect.inc");

	/*** test mysqli_connect 127.0.0.1 ***/
	$mysql = mysqli_connect($host, $user, $passwd, "test");

	$mysql->query("DROP TABLE IF EXISTS litest");
	$mysql->query("CREATE TABLE litest (a VARCHAR(20))");

	$mysql->query("LOAD DATA LOCAL INFILE 'filenotfound' INTO TABLE litest");
	var_dump($mysql->error);

	$mysql->close();
	printf("Done");
?>
--EXPECTF--
string(%d) "%s"
Done
