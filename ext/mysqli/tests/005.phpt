--TEST--
mysqli fetch char/text long
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

	if (!mysqli_query($link, "DROP TABLE IF EXISTS test_bind_fetch"))
		printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, "CREATE TABLE test_bind_fetch(c1 char(10), c2 text) ENGINE=" . $engine))
		printf("[002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$a = str_repeat("A1", 32000);

	mysqli_query($link, "INSERT INTO test_bind_fetch VALUES ('1234567890', '$a')");

	$stmt = mysqli_prepare($link, "SELECT * FROM test_bind_fetch");
	mysqli_bind_result($stmt, $c1, $c2);
	mysqli_execute($stmt);
	mysqli_fetch($stmt);

	$test[] = $c1;
	$test[] = ($a == $c2) ? "32K String ok" : "32K String failed";

	var_dump($test);

	mysqli_stmt_close($stmt);
	mysqli_query($link, "DROP TABLE IF EXISTS test_bind_fetch");
	mysqli_close($link);
	print "done!";
?>
--EXPECTF--
array(2) {
  [0]=>
  unicode(10) "1234567890"
  [1]=>
  unicode(13) "32K String ok"
}
done!
