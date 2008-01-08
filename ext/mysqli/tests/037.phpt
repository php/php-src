--TEST--
function test: mysqli_field_count()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php

	include "connect.inc";

	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket);

	mysqli_select_db($link, $db);

	mysqli_query($link, "DROP TABLE IF EXISTS test_result");

	mysqli_query($link, "CREATE TABLE test_result (a int, b varchar(10)) ENGINE = " . $engine);

	mysqli_query($link, "INSERT INTO test_result VALUES (1, 'foo')");
	$ir[] = mysqli_field_count($link);

	mysqli_real_query($link, "SELECT * FROM test_result");
	$ir[] = mysqli_field_count($link);


	var_dump($ir);

	mysqli_query($link, "DROP TABLE IF EXISTS test_result");
	mysqli_close($link);
	print "done!";
?>
--EXPECT--
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(2)
}
done!