--TEST--
mysqli fetch long values
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect("localhost", $user, $passwd);

	mysqli_select_db($link, "test");

  	mysqli_query($link,"DROP TABLE IF EXISTS test_bind_fetch");
  	mysqli_query($link,"CREATE TABLE test_bind_fetch(c1 int unsigned,
                                                     c2 int unsigned,
                                                     c3 int,
                                                     c4 int,
                                                     c5 int,
                                                     c6 int unsigned,
                                                     c7 int)");

	mysqli_query($link, "INSERT INTO test_bind_fetch VALUES (-23,35999,NULL,-500,-9999999,-0,0)");

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
  int(0)
  [1]=>
  int(35999)
  [2]=>
  NULL
  [3]=>
  int(-500)
  [4]=>
  int(-9999999)
  [5]=>
  int(0)
  [6]=>
  int(0)
}
