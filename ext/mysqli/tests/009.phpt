--TEST--
mysqli fetch bigint values
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	mysqli_select_db($link, "test");

  	mysqli_query($link,"DROP TABLE IF EXISTS test_bind_fetch");
  	mysqli_query($link,"CREATE TABLE test_bind_fetch(c1 bigint default 5,
                                                     c2 bigint,
                                                     c3 bigint not NULL,
                                                     c4 bigint unsigned,
                                                     c5 bigint unsigned,
                                                     c6 bigint unsigned,
                                                     c7 bigint unsigned)");

	mysqli_query($link, "INSERT INTO test_bind_fetch (c2,c3,c4,c5,c6,c7) VALUES (-23,4.0,33333333333333,0,-333333333333,99.9)");

	$stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch");
	mysqli_bind_result($stmt, &$c1, &$c2, &$c3, &$c4, &$c5, &$c6, &$c7);
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
  int(5)
  [1]=>
  int(-23)
  [2]=>
  int(4)
  [3]=>
  string(14) "33333333333333"
  [4]=>
  int(0)
  [5]=>
  string(13) "-333333333333"
  [6]=>
  int(100)
}
