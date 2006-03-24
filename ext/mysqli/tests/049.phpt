--TEST--
mysql_fetch_row (OO-Style) 
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$mysql = mysqli_connect($host, $user, $passwd);

	$mysql->select_db("test");		
	$result = $mysql->query("SELECT DATABASE()");
	$row = $result->fetch_row();
	$result->close();

	var_dump($row);

	$mysql->close();
?>
--EXPECT--
array(1) {
  [0]=>
  string(4) "test"
}
