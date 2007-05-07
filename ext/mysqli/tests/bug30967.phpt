--TEST--
Bug #30967 (problems with properties declared in the class extending the class extending MySQLi)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	class mysql1 extends mysqli {
	}

	class mysql2 extends mysql1 {
	}

	$mysql = new mysql2($host, $user, $passwd, "test");

	$mysql->query("THIS DOES NOT WORK");
	printf("%d\n", $mysql->errno);

	$mysql->close();	
?>
--EXPECTF--
1064
