--TEST--
mysqli fetch functions 
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	mysqli_select_db($link, "test");

	$stmt = mysqli_prepare($link, "SELECT current_user(), database()");
	mysqli_bind_result($stmt, &$c0, &$c1); 
	mysqli_execute($stmt);

	mysqli_fetch($stmt);

	$test = array($c0, $c1);

	var_dump($test);

	mysqli_close($link);
?>
--EXPECT--
array(2) {
  [0]=>
  string(14) "root@localhost"
  [1]=>
  string(4) "test"
}
