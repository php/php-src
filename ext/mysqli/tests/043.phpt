--TEST--
mysqli_bind_param (UPDATE)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);

	mysqli_select_db($link, "test");

  	mysqli_query($link,"DROP TABLE IF EXISTS test_update");
  	mysqli_query($link,"CREATE TABLE test_update(a varchar(10),
                                                     b int)");

	mysqli_query($link, "INSERT INTO test_update VALUES ('foo', 2)");

	$stmt = mysqli_prepare($link, "UPDATE test_update SET a=?,b=? WHERE b=?");
	mysqli_bind_param($stmt, "sii", $c1, $c2, $c3);

	$c1 = "Rasmus";
	$c2 = 1;
	$c3 = 2;

	mysqli_execute($stmt);
	mysqli_stmt_close($stmt);

	$result = mysqli_query($link, "SELECT concat(a, ' is No. ', b) FROM test_update");
	$test = mysqli_fetch_row($result);
	mysqli_free_result($result);

	var_dump($test);

	mysqli_close($link);
?>
--EXPECT--
array(1) {
  [0]=>
  string(15) "Rasmus is No. 1"
}
