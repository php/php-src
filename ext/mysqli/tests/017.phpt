--TEST--
mysqli fetch functions 
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);

	mysqli_select_db($link, "test");

	$stmt = mysqli_prepare($link, "SELECT md5('bar'), database(), 'foo'");
	mysqli_bind_result($stmt, $c0, $c1, $c2); 
	mysqli_execute($stmt);

	mysqli_fetch($stmt);
	mysqli_stmt_close($stmt);

	$test = array($c0, $c1, $c2);

	var_dump($test);
	mysqli_close($link);
?>
--EXPECT--
array(3) {
  [0]=>
  string(32) "37b51d194a7513e45b56f6524f2d51f2"
  [1]=>
  string(4) "test"
  [2]=>
  string(3) "foo"
}
