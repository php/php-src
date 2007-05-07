--TEST--
Bug #33090 (mysql_prepare doesn't return an error)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include ("connect.inc");

	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);
	mysqli_select_db($link, "test");

	if (!($link->prepare("this makes no sense"))) {
		printf("%d\n", $link->errno);
		printf("%s\n", $link->sqlstate);
	}	
	$link->close();
?>
--EXPECT--
1064
42000
