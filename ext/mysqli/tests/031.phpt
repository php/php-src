--TEST--
function test: mysqli_error
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);
	$error = mysqli_error($link);
	var_dump($error);

	mysqli_select_db($link, "test");

	mysqli_query($link, "select * from non_exisiting_table");
	$error = mysqli_error($link);	

	var_dump($error);

	mysqli_close($link);
?>
--EXPECT--
string(0) ""
string(46) "Table 'test.non_exisiting_table' doesn't exist"
