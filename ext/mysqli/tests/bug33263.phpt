--TEST--
Bug #33263 (mysqli_real_connect in __construct) 
--SKIPIF--
<?php 
require_once('skipif.inc'); 
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php

	include "connect.inc";

	class test extends mysqli
	{
		public function __construct($host, $user, $passwd, $db, $port, $socket) {
			parent::init();
			parent::real_connect($host, $user, $passwd, $db, $port, $socket);
		}
	}

	$mysql = new test($host, $user, $passwd, $db, $port, $socket);

	$stmt = $mysql->prepare("SELECT DATABASE()");
	$stmt->execute();
	$stmt->bind_result($db);
	$stmt->fetch();
	$stmt->close();

	var_dump($db);

	$mysql->close();
?>
--EXPECTF--
%s(4) "test"
