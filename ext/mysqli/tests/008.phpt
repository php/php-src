--TEST--
mysqli fetch tinyint values
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);

	mysqli_select_db($link, "test");
	mysqli_query($link, "SET sql_mode=''");

  	mysqli_query($link,"DROP TABLE IF EXISTS test_bind_fetch");
  	mysqli_query($link,"CREATE TABLE test_bind_fetch(c1 tinyint,
                                                     c2 tinyint unsigned,
                                                     c3 tinyint not NULL,
                                                     c4 tinyint,
                                                     c5 tinyint,
                                                     c6 tinyint unsigned,
                                                     c7 tinyint)");

	mysqli_query($link, "INSERT INTO test_bind_fetch VALUES (-23,300,0,-100,-127,+30,0)");

	$stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch");
	mysqli_bind_result($stmt, $c1, $c2, $c3, $c4, $c5, $c6, $c7);
	mysqli_execute($stmt);
	mysqli_fetch($stmt);

	$test = array($c1,$c2,$c3,$c4,$c5,$c6,$c7);

	var_dump($test);

	mysqli_stmt_close($stmt);
	mysqli_close($link);
?>
--EXPECT--
array(7) {
  [0]=>
  int(-23)
  [1]=>
  int(255)
  [2]=>
  int(0)
  [3]=>
  int(-100)
  [4]=>
  int(-127)
  [5]=>
  int(30)
  [6]=>
  int(0)
}
