--TEST--
mysqli fetch float values
--INI--
precision=12
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

 	mysqli_query($link,"CREATE TABLE test_bind_fetch(c1 float(3),
                                                     c2 float,
                                                     c3 float unsigned,
                                                     c4 float,
                                                     c5 float,
                                                     c6 float,
                                                     c7 float(10) unsigned)");


	mysqli_query($link, "INSERT INTO test_bind_fetch (c1,c2,c3,c4,c5,c6,c7) VALUES (3.1415926535,-0.000001, -5, 999999999999,
											sin(0.6), 1.00000000000001, 888888888888888)");

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
  float(3.14159274101)
  [1]=>
  float(-9.99999997475E-7)
  [2]=>
  float(0)
  [3]=>
  float(999999995904)
  [4]=>
  float(0.564642488956)
  [5]=>
  float(1)
  [6]=>
  float(888888914608000)
}
