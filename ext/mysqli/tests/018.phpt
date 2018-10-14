--TEST--
mysqli fetch system variables
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	/*** test mysqli_connect 127.0.0.1 ***/
	$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket);

	if (!mysqli_query($link, "SET AUTOCOMMIT=0"))
		printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$stmt = mysqli_prepare($link, "SELECT @@autocommit"))
		printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	mysqli_stmt_bind_result($stmt, $c0);
	mysqli_stmt_execute($stmt);

	mysqli_stmt_fetch($stmt);

	var_dump($c0);

	mysqli_close($link);
	print "done!";
?>
--EXPECT--
int(0)
done!
