--TEST--
mysqli fetch char/text 
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include ("connect.inc");

	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);

	mysqli_select_db($link, "test");

  	mysqli_query($link,"DROP TABLE IF EXISTS test_bind_fetch");
  	mysqli_query($link,"CREATE TABLE test_bind_fetch(c1 char(10), c2 text)");

	mysqli_query($link, "INSERT INTO test_bind_fetch VALUES ('1234567890', 'this is a test0')");
	mysqli_query($link, "INSERT INTO test_bind_fetch VALUES ('1234567891', 'this is a test1')");
	mysqli_query($link, "INSERT INTO test_bind_fetch VALUES ('1234567892', 'this is a test2')");
	mysqli_query($link, "INSERT INTO test_bind_fetch VALUES ('1234567893', 'this is a test3')");

	$stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch ORDER BY c1");
	mysqli_bind_result($stmt, $c1, $c2);
	mysqli_execute($stmt);
	$i=4;
	while ($i--) {
		mysqli_fetch($stmt);
		$test = array($c1,$c2);
		var_dump($test);
	}


	mysqli_stmt_close($stmt);
	mysqli_close($link);
?>
--EXPECT--
array(2) {
  [0]=>
  string(10) "1234567890"
  [1]=>
  string(15) "this is a test0"
}
array(2) {
  [0]=>
  string(10) "1234567891"
  [1]=>
  string(15) "this is a test1"
}
array(2) {
  [0]=>
  string(10) "1234567892"
  [1]=>
  string(15) "this is a test2"
}
array(2) {
  [0]=>
  string(10) "1234567893"
  [1]=>
  string(15) "this is a test3"
}
