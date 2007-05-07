--TEST--
Bug #33263 (mysqli_real_connect in __construct) 
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php

	include "connect.inc";

	class test extends mysqli
	{
		public function __construct($host, $user, $passwd, $db) {
			parent::init();
			parent::real_connect($host, $user, $passwd, $db);
		}
	}

	$mysql = new test($host, $user, $passwd, "test");

	$stmt = $mysql->prepare("SELECT DATABASE()");
	$stmt->execute();
	$stmt->bind_result($db);
	$stmt->fetch();
	$stmt->close();

	var_dump($db);

	$mysql->close();	
?>
--EXPECT--
string(4) "test"
