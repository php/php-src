--TEST--
mysql_fetch_row (OO-Style) 
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$mysql = mysqli_connect($host, $user, $passwd);

	$mysql->select_db("test");		
	$result = $mysql->query("SELECT CURRENT_USER()");
	$row = $result->fetch_row();
	$result->close();

	$ok = ($row[0] == $user . "@" . $host);	
	var_dump($ok);

	$mysql->close();
?>
--EXPECT--
bool(true)
