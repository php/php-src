--TEST--
multiple binds
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";
	
	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);

	mysqli_select_db($link, "test");

  	mysqli_query($link,"DROP TABLE IF EXISTS mbind");
  	mysqli_query($link,"CREATE TABLE mbind (a int, b varchar(10))");

	$stmt = mysqli_prepare($link, "INSERT INTO mbind VALUES (?,?)");
	
	mysqli_bind_param($stmt, "is", $a, $b);

	$a = 1;
	$b = "foo";

	mysqli_execute($stmt);

	mysqli_bind_param($stmt, "is", $c, $d);

	$c = 2;
	$d = "bar";

	mysqli_execute($stmt);
	mysqli_stmt_close($stmt);

	$stmt = mysqli_prepare($link, "SELECT * FROM mbind");
	mysqli_execute($stmt);

	mysqli_bind_result($stmt, $e, $f);
	mysqli_fetch($stmt);

	mysqli_bind_result($stmt, $g, $h);
	mysqli_fetch($stmt);

	var_dump((array($e,$f,$g,$h)));

	mysqli_close($link);
?>
--EXPECT--
array(4) {
  [0]=>
  int(1)
  [1]=>
  string(3) "foo"
  [2]=>
  int(2)
  [3]=>
  string(3) "bar"
}
