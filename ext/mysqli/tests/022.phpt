--TEST--
mysqli bind_param/bind_result char/text long 
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);

	mysqli_select_db($link, "test");

  	mysqli_query($link,"DROP TABLE IF EXISTS test_bind_fetch");
  	mysqli_query($link,"CREATE TABLE test_bind_fetch(c1 char(10), c2 text)");


	$stmt = mysqli_prepare($link, "INSERT INTO test_bind_fetch VALUES (?,?)");
	mysqli_bind_param($stmt, "ss", $a1, $a2);

	$a1 = "1234567890";
	$a2 = str_repeat("A1", 32000);

	mysqli_execute($stmt);
	mysqli_stmt_close($stmt);

	$stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch");
	mysqli_bind_result($stmt, $c1, $c2);
	mysqli_execute($stmt);
	mysqli_fetch($stmt);

	$test[] = $c1;
	$test[] = ($a2 == $c2) ? "32K String ok" : "32K String failed";

	var_dump($test);

	mysqli_stmt_close($stmt);
	mysqli_close($link);
?>
--EXPECT--
array(2) {
  [0]=>
  string(10) "1234567890"
  [1]=>
  string(13) "32K String ok"
}
