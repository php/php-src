--TEST--
mysqli bind_param/bind_result with send_long_data
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
	mysqli_query($link, "SET sql_mode=''");

	mysqli_query($link,"DROP TABLE IF EXISTS test_bind_fetch");
	mysqli_query($link,"CREATE TABLE test_bind_fetch(c1 varchar(10), c2 text)");

	$stmt = mysqli_prepare ($link, "INSERT INTO test_bind_fetch VALUES (?,?)");
	mysqli_bind_param($stmt, "sb", $c1, $c2);

	$c1 = "Hello World";

	mysqli_send_long_data($stmt, 1, "This is the first sentence.");
	mysqli_send_long_data($stmt, 1, " And this is the second sentence.");
	mysqli_send_long_data($stmt, 1, " And finally this is the last sentence.");

	mysqli_execute($stmt);
	mysqli_stmt_close($stmt);

	$stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch");
	mysqli_bind_result($stmt, $d1, $d2);
	mysqli_execute($stmt);
	mysqli_fetch($stmt);

	$test = array($d1,$d2);

	var_dump($test);

	mysqli_stmt_close($stmt);
	mysqli_query($link, "DROP TABLE IF EXISTS test_bind_fetch");
	mysqli_close($link);
	print "done!";
--EXPECTF--
array(2) {
  [0]=>
  unicode(10) "Hello Worl"
  [1]=>
  %s(99) "This is the first sentence. And this is the second sentence. And finally this is the last sentence."
}
done!
