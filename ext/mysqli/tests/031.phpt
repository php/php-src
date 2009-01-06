--TEST--
function test: mysqli_error
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
	$error = mysqli_error($link);
	var_dump($error);

	mysqli_select_db($link, $db);

	mysqli_query($link, "SELECT * FROM non_exisiting_table");
	$error = mysqli_error($link);

	var_dump($error);

	mysqli_close($link);
	print "done!";
?>
--EXPECTF--
unicode(0) ""
unicode(%d) "%s"
done!
