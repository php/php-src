--TEST--
mysql_fetch_row (OO-Style) 
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$mysql = mysqli_connect("localhost", $user, $passwd);

	$mysql->select_db("test");		
	$result = $mysql->query("SELECT CURRENT_USER()");
	$row = $result->fetch_row();
	$result->close();
	
	var_dump($row);

	$mysql->close();
?>
--EXPECT--
array(1) {
  [0]=>
  string(14) "root@localhost"
}
